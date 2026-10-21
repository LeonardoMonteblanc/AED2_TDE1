#ifndef FN_AUX_H
#define FN_AUX_H

#include <io.h>
#include <stdio.h>

#define ftell _ftelli64
#define fseek _fseeki64


#define DIR_DADOS "DADOS"
#define DIR_BIN "BIN"
#define DIR_IDX "IDX"

#define CSV_OUT DIR_DADOS "/out.csv"
#define CSV_NOV DIR_DADOS "/nov.csv"

#define BIN_ACESSOS DIR_BIN "/acessos.bin"
#define BIN_PRODUTOS DIR_BIN "/produtos.bin"
#define BIN_TEMP_PROD DIR_BIN "/temp_prod.bin"
#define BIN_PART_PROD DIR_BIN "/part_prod_%04d.bin"

#define IDX_BASE DIR_IDX "/base.idx"
#define IDX_TOPO DIR_IDX "/topo.idx"
#define IDX_BASE_PROD DIR_IDX "/base_prod.idx"
#define IDX_TOPO_PROD DIR_IDX "/topo_prod.idx"

#define ACESSO_BASE 100
#define ACESSO_TOPO 1000
#define PROD_BASE 100
#define PROD_TOPO 1000

/* =========================== STRUCT DE PROD ===========================*/
struct prod {
    int prod_id;       // 4 bytes
    double preco;      // 8 bytes
    long long cat_id;  // 8 bytes
    char marca[30];    // 30 bytes
    char cat_cod[50];  // 50 bytes
};  // 100 bytes + 4 bytes de padding ==> 104 bytes
typedef struct prod Produto;

/* =========================== STRUCT DE ACESSO ===========================*/
struct acesso {
    int acesso_id;              // 4 bytes
    int usuario_id;             // 4 bytes
    int prod_id;                // 4 bytes
    unsigned char evento_tipo;  // 1 byte
    double preco;               // 8 bytes
    char evento_time[24];       // 24 bytes
};  // 45 bytes + 3 bytes de padding ==> 48 bytes
typedef struct acesso Acesso;

/* =========================== INDICES ===========================*/
struct entrada_indice {
    int chave;      // 4 bytes
    long long pos;  // 8 bytes
};  // 12 bytes + 4 bytes de padding ==> 16 bytes
typedef struct entrada_indice EntradaIdx;

struct idx2 {
    EntradaIdx* base;
    int n_base;
    EntradaIdx* topo;
    int n_topo;
};
typedef struct idx2 Idx2Niveis;

void formatar_registro(char* formatada, char* original, int tam);
int comparar_strings(char* formatada, char* original);
FILE *carregar_arquivo(char *caminho, char *modo);
#endif
