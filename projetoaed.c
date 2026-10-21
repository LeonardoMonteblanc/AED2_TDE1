#include "fn_aux.h"
#include "operacoes.h"
#include "preparar_dados.h"

#include <stdio.h>

int main() {
    printf("Trabalho AED II - Leonardo Monteblanco");
    /*
    1. tratar_dados() 
        1.1. gerar_bin() 
            -> acessos.bin + base.idx + topo.idx
            -> temp_prod.bin
        1.2. gerar_produtos()
            -> produtos.bin + base_prod.idx + topo_prod.idx
            - Exclui particoes e arquivo temp
    2. abrir_arquivos()
        2.1 Abre os arquivos .bin e carrega .idx na RAM
    */

    printf("Tratando base de dados\n");
    tratar_dados();

    printf("Verificando os arquivos gerados\n");
    if(!abrir_arquivos()) {
        printf("Falha ao abrir os arquivos\n");
        return 1;
    }

    printf("Produtos: %lld registros\n", n_prod);
    printf("Acessos: %lld registros\n", n_acessos);

    printf("Indices\n");
    printf("Base Produtos: %d\n", idx_prod.n_base);
    printf("Base Acessos: %d\n", idx_acessos.n_base);
    printf("Topo Produtos: %d\n", idx_prod.n_topo);
    printf("Topo Acessos: %d\n", idx_acessos.n_topo);

    fechar_arquivos();
    
    printf("Concluido!\n");
    return 0;
}