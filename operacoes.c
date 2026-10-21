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
 * Libera da memoria arquivo .idx na RAM
 */
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