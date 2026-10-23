#include "tipos.h"
#include "preparar_dados.h"
#include "arquivos.h"
#include "consultas.h"

// =============== VARIAVEIS GLOBAIS ===============
FILE* p_acessos = NULL;
FILE* p_prod = NULL;

Idx2Niveis idx_acessos = {NULL, 0, NULL, 0};
Idx2Niveis idx_prod = {NULL, 0, NULL, 0};

long long n_acessos = 0;
long long n_prod = 0;


int main() {
    printf("Trabalho AED II - Leonardo Monteblanco\n");

    // ETAPA 1: TRANSFORMAR CSV EM ACESSOS.BIN
    long long qtd_acessos = gravar_bin_acessos();
    printf("Acessos gravados %lld\n", qtd_acessos);

    // ETAPA 2: TRANSFORMAR CSV EM PRODUTOS_TEMP.BIN
    gravar_bin_produtos();

    // ETAPA 2.2: PARTICIONAR ORDENADO PRODUTOS_TEMP.BIN 
    int quantidade_particoes = particionar_produtos();

    // ETAPA 2.3: INTERCALAR AS PARTICOES REMOVENDO DUPLICADOS + ESCRITA DE PRODUTOS.BIN
    printf("Produtos gerou %d particoes", quantidade_particoes);
    intercalar_particoes(quantidade_particoes);
    

    return 0;
}