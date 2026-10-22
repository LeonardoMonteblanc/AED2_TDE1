#ifndef TIPOS_H
#define TIPOS_H

// =============== INCLUDES ===============
#include <io.h>
#include <stdio.h>

/* =============== FTELL E FSEEK NO WIN ===============
ftell e fseek no windows retornam um long (4 bytes)
o que permite lidar com arquivos de até no maximo ==> 2^(32-1) bits ==> 2gb
para usar com arquivos maiores, precisa das funções ftelli64 e fseeki64*/
#define ftell _ftelli64
#define fseek _fseeki64

// =============== PASTAS ===============
#define DIR_DADOS "DADOS"
#define DIR_BIN "BIN"
#define DIR_IDX "IDX"

// =============== ARQUIVOS CSV (DADOS) ===============
#define CSV_OUT DIR_DADOS "/out.csv"
#define CSV_NOV DIR_DADOS "/nov.csv"
#define TAM_LINHA 1024

// =============== ARQUIVOS BIN ===============
#define BIN_ACESSOS DIR_BIN "/acessos.bin"
#define BIN_PRODUTOS DIR_BIN "/produtos.bin"
#define BIN_TEMP_PROD DIR_BIN "/temp_prod.bin"
#define BIN_PART_PROD DIR_BIN "/part_prod_%04d.bin"
#define REGISTRO_BLOCO 10000000


// =============== ARQUIVOS IDX ===============
#define IDX_BASE DIR_IDX "/base.idx"
#define IDX_TOPO DIR_IDX "/topo.idx"
#define IDX_BASE_PROD DIR_IDX "/base_prod.idx"
#define IDX_TOPO_PROD DIR_IDX "/topo_prod.idx"


// =============== CONSTANTES ===============
#define ACESSO_BASE 100
#define ACESSO_TOPO 1000
#define PROD_BASE 100
#define PROD_TOPO 1000


// =============== VARIAVEIS GLOBAIS ===============
extern FILE* p_acessos;
extern FILE* p_prod;

extern Idx2Niveis idx_acessos;
extern Idx2Niveis idx_prod;

extern long long n_acessos;
extern long long n_prod;


// =============== STRUCT DE PRODUTO ===============
struct prod {
    int id;                     // 4 bytes
    double preco;               // 8 bytes
    long long categoria_id;     // 8 bytes
    char marca[30];             // 30 bytes
    char categoria_cod[50];     // 50 bytes
};  // 100 bytes + 4 bytes de padding ==> 104 bytes
typedef struct prod Produto;


// =============== STRUCT DE ACESSO ===============
struct acesso {
    int id;              // 4 bytes
    int usuario_id;             // 4 bytes
    int produto_id;                // 4 bytes
    unsigned char evento_tipo;  // 1 byte
    double preco;               // 8 bytes
    char evento_time[24];       // 24 bytes
};  // 45 bytes + 3 bytes de padding ==> 48 bytes
typedef struct acesso Acesso;


// =============== INDICES ===============
struct entrada_indice {
    int chave;      // 4 bytes
    long long pos;  // 8 bytes
};  // 12 bytes + 4 bytes de padding ==> 16 bytes
typedef struct entrada_indice EntradaIdx;


// =============== INDICES 2 NIVEIS ===============
struct idx2 {
    EntradaIdx* base;
    int qtd_base;
    EntradaIdx* topo;
    int qtd_topo;
};
typedef struct idx2 Idx2Niveis;


// =============== STRUCT CONSULTA ===============
// STRUCT QUE CRIA UM AGREGADO ENTRE USUARIO ID E PREÇO
struct compra {
    int usuario_id;
    double preco;
}; typedef struct compra CompraRegistro;


// STRUCT QUE CRIA UM AGREGADO ENTRE USUARIO COM AS COMPRAS
struct user {
    int usuario_id;
    int qtd;
    double total;
}; typedef struct user AgregadoUsuario;


// STRUCT QUE CRIA UM AGREGADO ENTRE CATEGORIA E COMPRAS
struct cat {
    long long categoria_id;
    int qtd;
    double total;
}; typedef struct cat AgregadoCategoria;

#endif
