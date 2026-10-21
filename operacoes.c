#include "operacoes.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>

FILE* p_acessos = NULL;
FILE* p_prod = NULL;

Idx2Niveis idx_acessos = {NULL, 0, NULL, 0};
Idx2Niveis idx_prod = {NULL, 0, NULL, 0};

long long n_acessos = 0;
long long n_prod = 0;

/* =============== CARREGAR_INDICE ===============
 * Carrega arquivo .idx na RAM */
int carregar_idx(char* arq_base, char* arq_topo, Idx2Niveis* idx) {
    FILE* fbase, * ftopo;

    long long tam_base,  tam_topo;

    fbase = carregar_arquivo(arq_base, "rb");
    ftopo = carregar_arquivo(arq_topo, "rb");


    fseek(fbase, 0, SEEK_END);
    tam_base = ftell(fbase);
    fseek(fbase, 0, SEEK_SET);

    fseek(ftopo, 0, SEEK_END);
    tam_topo = ftell(ftopo);
    fseek(ftopo, 0, SEEK_SET);

    idx->n_base = (int)(tam_base / sizeof(EntradaIdx));
    idx->n_topo = (int)(tam_topo / sizeof(EntradaIdx));

    idx->base = (EntradaIdx*)malloc(idx->n_base * sizeof(EntradaIdx));
    idx->topo = (EntradaIdx*)malloc(idx->n_topo * sizeof(EntradaIdx));

    if (idx->base == NULL || idx->topo == NULL) {
        printf("Erro: sem memoria para os indices\n");

        if (idx->base != NULL) {
            free(idx->base);
            idx->base = NULL;
        }

        if (idx->topo != NULL) {
            free(idx->topo);
            idx->topo = NULL;
        }

        idx->n_base = 0;
        idx->n_topo = 0;

        fclose(fbase);
        fclose(ftopo);
        return 0;
    }

    fread(idx->base, sizeof(EntradaIdx), idx->n_base, fbase);
    fread(idx->topo, sizeof(EntradaIdx), idx->n_topo, ftopo);

    fclose(fbase);
    fclose(ftopo);
    return 1;
}

/* =============== LIBERAR_INDICE ===============
 * Libera da memoria arquivo .idx na RAM */
void liberar_indice(Idx2Niveis* idx) {
    if (idx->base != NULL) {
        free(idx->base);
        idx->base = NULL;
    }

    if (idx->topo != NULL) {
        free(idx->topo);
        idx->topo = NULL;
    }

    idx->n_base = 0;
    idx->n_topo = 0;
}

/* =============== BUSCA_BINARIA_IDX  ===============
 * Encontra o intervalo onde é <= chave */
int busca_bin_idx(EntradaIdx* lista, int ini, int fim, int chave) {
    int res = -1;

    while (ini <= fim) {
        int meio = (ini + fim) / 2;

        if (lista[meio].chave <= chave) {
            res = meio;
            ini = meio + 1;
        } else {
            fim = meio - 1;
        }
    }

    return res;
}

/* =============== ABRIR_ARQ_DADOS ===============
 * Abre os arquivos e carrega os 4 arquivos de indice na RAM */
int abrir_arquivos() {
    long long tam;

    p_acessos = carregar_arquivo(BIN_ACESSOS, "rb+");
    p_prod = carregar_arquivo(BIN_PRODUTOS, "rb+");

    fseek(p_acessos, 0, SEEK_END);
    tam = ftell(p_acessos);
    n_acessos = tam / (sizeof(Acesso) + 1);

    fseek(p_prod, 0, SEEK_END);
    tam = ftell(p_prod);
    n_prod = tam / (sizeof(Produto) + 1);

    if (!carregar_idx(IDX_BASE, IDX_TOPO, &idx_acessos)) {
        printf("Erro ao carregar os indices de acesso\n");

        fclose(p_acessos);
        fclose(p_prod);

        p_acessos = NULL;
        p_prod = NULL;

        return 0;
    }

    if (!carregar_idx(IDX_BASE_PROD, IDX_TOPO_PROD, &idx_prod)) {
        printf("Erro ao carregar os indices de produtos\n");
        liberar_indice(&idx_acessos);

        fclose(p_acessos);
        fclose(p_prod);

        p_acessos = NULL;
        p_prod = NULL;

        return 0;
    }

    return 1;
}

/* =============== FECHA_ARQ_DADOS ===============
 * Fecha os arquivos e tira os 4 arquivos de indice na RAM */
void fechar_arquivos() {
    if (p_acessos != NULL) {
        fclose(p_acessos);
        p_acessos = NULL;
    }

    if (p_prod != NULL) {
        fclose(p_prod);
        p_prod = NULL;
    }

    liberar_indice(&idx_acessos);
    liberar_indice(&idx_prod);
}

void reconstruir_idx_prod() {
    FILE * base, *topo;
    long long i, pos;

    base = carregar_arquivo(IDX_BASE_PROD,"wb");
    topo = carregar_arquivo(IDX_TOPO_PROD, "wb");

    fseek(p_prod, 0, SEEK_SET);

    for(i=0; i < n_prod; i++) {
        Produto p;

        pos = ftell(p_prod);

        if(fread(&p, sizeof(Produto), 1, p_prod) != 1) {
            break;
        }

        fgetc(p_prod);

        if(i% PROD_BASE == 0) {
            EntradaIdx e;
            e.chave = p.prod_id;
            e.pos = pos;
            fwrite(&e, sizeof(EntradaIdx),1, base);
        }

        if(i%PROD_TOPO == 0) {
            EntradaIdx e;
            e.chave = p.prod_id;
            e.pos = pos;
            fwrite(&e, sizeof(EntradaIdx),1, topo);
        }
    }

    fclose(base);
    fclose(topo);

    liberar_indice(&idx_prod);
    carregar_idx(IDX_BASE_PROD, IDX_TOPO_PROD, &idx_prod);
}

void prod_mostrar(long long ini, long long qtd) {
    long long i;
    long long tam_reg= sizeof(Produto)+1;

    if(ini < 0) {
        ini = 0;
    }

    if(ini+qtd > n_prod) {
        qtd = n_prod - ini;
    }

    fseek(p_prod, ini * tam_reg, SEEK_SET);

    for(i=0; i < qtd; i++) {
        Produto p;

        if(fread(&p, sizeof(Produto), 1, p_prod) != 1) {
            break;
        }

        fgetc(p_prod);

        printf("[%lld] id=%d | R$ %.2f | cat=%lld | %.30s | %.50s\n", ini+i, p.prod_id, p.preco, p.cat_id, p.marca, p.cat_cod);
    }
}

int prod_busca_bin(int prod_id, Produto *out) {
    long long esq =0;
    long long dir = n_prod -1;
    long long tam_reg = sizeof(Produto) +1;

    while(esq <= dir) {
        long long meio = (esq + dir) /2;

        fseek(p_prod, meio * tam_reg, SEEK_SET);
        fread(out, sizeof(Produto), 1, p_prod);

        if(out->prod_id == prod_id) {
            return 1;
        }

        if(out->prod_id < prod_id) {
            esq = meio+1;
        } else {
            dir = meio -1;
        }
    }

    return 0;
}

int prod_consulta_idx(int prod_id, Produto *out) {
    int itopo, ibase, ini_base, fim_base, i;
    long long pos;

    itopo = busca_bin_idx(idx_prod.topo, 0, idx_prod.n_topo - 1, prod_id);
    
    if(itopo < 0) {
        return 0;
    }

    ini_base = itopo * 10;
    fim_base = ini_base + 9;

    if(fim_base >= idx_prod.n_base) {
        fim_base = idx_prod.n_base - 1;
    }

    ibase = busca_bin_idx(idx_prod.base, ini_base, fim_base, prod_id);

    if(ibase < 0) {
        return 0;
    }

    pos = idx_prod.base[ibase].pos;
    fseek(p_prod, pos, SEEK_SET);

    for(i=0; i < PROD_BASE; i++) {
        if(fread(out, sizeof(Produto), 1, p_prod) != 1) {
            return 0;
        }

        fgetc(p_prod);

        if(out->prod_id == prod_id) {
            return 1;
        } 

        if(out->prod_id > prod_id) {
            return 0;
        }
    }

    return 0;
}

int prod_inserir(Produto *novo) {
    Produto tmp;
    FILE *out;

    long long i;
    long long tam_reg = sizeof(Produto) +1;
    int inserido = 0;

    if(prod_busca_bin(novo->prod_id, &tmp)) {
        printf("Produto %d ja existe\n", novo->prod_id);
        return 0;
    }

    out = carregar_arquivo(BIN_TEMP_PROD, "wb");
    fseek(p_prod, 0, SEEK_SET);

    for(i=0; i < n_prod; i++) {
        Produto p;

        if(fread(&p, sizeof(Produto),1, p_prod) != 1) {
            break;
        }

        fgetc(p_prod);

        if(!inserido && p.prod_id > novo->prod_id) {
            fwrite(novo, sizeof(Produto),1, out);
            fputc('\n', out);
            inserido = 1;
        }

        fwrite(&p, sizeof(Produto),1, out);
        fputc('\n', out);
    }

    
    if(!inserido) {
        fwrite(novo, sizeof(Produto),1, out);
        fputc('\n', out);
    }

    fclose(out);
    fclose(p_prod);

    remove(BIN_PRODUTOS);
    rename(BIN_TEMP_PROD, BIN_PRODUTOS);
    p_prod = carregar_arquivo(BIN_PRODUTOS, "rb+");

    fseek(p_prod, 0, SEEK_END);
    n_prod = ftell(p_prod) / tam_reg;

    reconstruir_idx_prod();
    return 1;
}

int prod_remover(int prod_id) {
    Produto tmp;
    FILE *out;
    
    long long i;
    long long tam_reg = sizeof(Produto) +1;
    
    int removidos = 0;

    if(!prod_busca_bin(prod_id, &tmp)) {
        printf("Produto %d nao encontrado\n", prod_id);
        return 0;
    }

    out = carregar_arquivo(BIN_TEMP_PROD, "wb");
    fseek(p_prod, 0, SEEK_SET);

    for(i=0; i < n_prod; i++) {
        Produto p;

        if(fread(&p, sizeof(Produto), 1, p_prod) != 1) {
            break;
        }

        fgetc(p_prod);

        if(p.prod_id != prod_id) {
            fwrite(&p, sizeof(Produto),1, out);
            fputc('\n', out);
        } else {
            removidos++;
        }
    }

    fclose(out);
    fclose(p_prod);

    remove(BIN_PRODUTOS);
    rename(BIN_TEMP_PROD, BIN_PRODUTOS);
    p_prod = carregar_arquivo(BIN_PRODUTOS, "rb+");

    fseek(p_prod, 0 ,SEEK_END);
    n_prod = ftell(p_prod)/tam_reg;

    reconstruir_idx_prod();

    return (removidos > 0);
}

