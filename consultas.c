#include "consultas.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>

// =============== BUSCA_BIN_IDX ===============
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


// =============== MOSTRAR_PRODUTOS ===============
void mostrar_produtos(long long ini, long long qtd) {
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


// =============== BUSCA_BINARIA_PRODUTOS ===============
int busca_bin_produtos(int prod_id, Produto *out) {
    long long esq =0;
    long long dir = n_prod -1;
    long long tam_reg = sizeof(Produto) +1;

    while(esq <= dir) {
        long long meio = (esq + dir) /2;

        fseek(p_prod, meio * tam_reg, SEEK_SET);
        fread(out, sizeof(Produto), 1, p_prod);

        if(out->id == prod_id) {
            return 1;
        }

        if(out->id < prod_id) {
            esq = meio+1;
        } else {
            dir = meio -1;
        }
    }

    return 0;
}


// =============== BUSCAR_PRODUTO_COM_IDX ===============
int buscar_produto_com_idx(int prod_id, Produto *out) {
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

        if(out->id == prod_id) {
            return 1;
        } 

        if(out->id > prod_id) {
            return 0;
        }
    }
    return 0;
}


// =============== INSERIR_PRODUTO ===============
int inserir_produto(Produto *novo) {
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


// =============== REMOVER_PRODUTO ===============
int remover_produto(int prod_id) {
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