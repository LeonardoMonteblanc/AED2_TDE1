#include "arquivos.h"
#include "tipos.h"
#include "fn_string.h"
#include <stdlib.h>
#include <string.h>   

//=============== ABRIR_ARQUIVOS =============== 
FILE *abrir_arquivo(char *caminho, char *modo) {
	FILE *f = fopen(caminho, modo);

	if(f == NULL){
		printf("Nao foi possivel abrir/criar: %s\n", caminho);
        exit(1);
    }
	return f;
}


//=============== GRAVAR_ARQUIVO_INDICE =============== 
void gravar_arquivo_indice(long long contador, int chave_registro, long long posicao, FILE *indice_base, FILE *indice_topo) {
    if(contador % INTERVALO_IDX_BASE == 0) {
        EntradaIdx e;
        memset(&e, 0, sizeof(e));
        e.chave = chave_registro;
        e.pos = posicao;
        fwrite(&e, sizeof(EntradaIdx), 1, indice_base);
    }

    if(contador % INTERVALO_IDX_TOPO == 0) {
        EntradaIdx e;
        memset(&e, 0, sizeof(e));
        e.chave = chave_registro;
        e.pos = posicao;
        fwrite(&e, sizeof(EntradaIdx), 1, indice_topo);
    }
}


//=============== CARREGAR_IDX =============== 
int carregar_idx(char* arq_base, char* arq_topo, Idx2Niveis* idx) {
    FILE* indice_base, * indice_topo;

    long long tam_base,  tam_topo;

    indice_base = abrir_arquivo(arq_base, "rb");
    indice_topo = abrir_arquivo(arq_topo, "rb");


    fseek(indice_base, 0, SEEK_END);
    tam_base = ftell(indice_base);
    fseek(indice_base, 0, SEEK_SET);

    fseek(indice_topo, 0, SEEK_END);
    tam_topo = ftell(indice_topo);
    fseek(indice_topo, 0, SEEK_SET);

    idx->qtd_base = (int)(tam_base / sizeof(EntradaIdx));
    idx->qtd_topo = (int)(tam_topo / sizeof(EntradaIdx));

    idx->base = (EntradaIdx*)malloc(idx->qtd_base * sizeof(EntradaIdx));
    idx->topo = (EntradaIdx*)malloc(idx->qtd_topo * sizeof(EntradaIdx));

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

        idx->qtd_base = 0;
        idx->qtd_topo = 0;

        fclose(indice_base);
        fclose(indice_topo);
        return 0;
    }

    fread(idx->base, sizeof(EntradaIdx), idx->qtd_base, indice_base);
    fread(idx->topo, sizeof(EntradaIdx), idx->qtd_topo, indice_topo);

    fclose(indice_base);
    fclose(indice_topo);
    return 1;
}


//=============== LIBERAR_IDX =============== 
void liberar_indice(Idx2Niveis* idx) {
    if (idx->base != NULL) {
        free(idx->base);
        idx->base = NULL;
    }

    if (idx->topo != NULL) {
        free(idx->topo);
        idx->topo = NULL;
    }

    idx->qtd_base = 0;
    idx->qtd_topo = 0;
}


//=============== CARREGAR_DADOS =============== 
int carregar_dados() {
    long long tam;

    p_acessos = abrir_arquivo(BIN_ACESSOS, "rb+");
    p_prod = abrir_arquivo(BIN_PRODUTOS, "rb+");

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


//=============== LIBERAR_DADOS =============== 
void liberar_dados() {
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


//=============== RECONSTRUIR_IDX_PROD =============== 
void reconstruir_idx_prod() {
    FILE * base, *topo;
    long long i, pos;

    base = abrir_arquivo(IDX_BASE_PROD,"wb");
    topo = abrir_arquivo(IDX_TOPO_PROD, "wb");

    fseek(p_prod, 0, SEEK_SET);

    for(i=0; i < n_prod; i++) {
        Produto p;

        pos = ftell(p_prod);

        if(fread(&p, sizeof(Produto), 1, p_prod) != 1) {
            break;
        }

        fgetc(p_prod);

        if(i% INTERVALO_IDX_BASE == 0) {
            EntradaIdx e;
            memset(&e, 0, sizeof(e));
            e.chave = p.id;
            e.pos = pos;
            fwrite(&e, sizeof(EntradaIdx),1, base);
        }

        if(i%INTERVALO_IDX_TOPO == 0) {
            EntradaIdx e;
            memset(&e, 0, sizeof(e));
            e.chave = p.id;
            e.pos = pos;
            fwrite(&e, sizeof(EntradaIdx),1, topo);
        }
    }

    fclose(base);
    fclose(topo);

    liberar_indice(&idx_prod);
    carregar_idx(IDX_BASE_PROD, IDX_TOPO_PROD, &idx_prod);
}


