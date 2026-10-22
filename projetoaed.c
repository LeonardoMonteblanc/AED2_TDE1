#include "fn_aux.h"
#include "operacoes.h"
#include "preparar_dados.h"

#include <stdlib.h>
#include <stdio.h>



int main() {
    printf("Trabalho AED II - Leonardo Monteblanco\n");
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
    // ====== GERA OS ARQUIVOS NECESSARIOS PARA CONSULTA ======
    // printf("Tratando base de dados\n");
    // tratar_dados();

    // printf("Verificando os arquivos gerados\n");
    // if(!abrir_arquivos()) {
    //     printf("Falha ao abrir os arquivos\n");
    //     return 1;
    // }

    // printf("Produtos: %lld registros\n", n_prod);
    // printf("Acessos: %lld registros\n", n_acessos);

    // printf("Indices\n");
    // printf("Base Produtos: %d\n", idx_prod.n_base);
    // printf("Base Acessos: %d\n", idx_acessos.n_base);
    // printf("Topo Produtos: %d\n", idx_prod.n_topo);
    // printf("Topo Acessos: %d\n", idx_acessos.n_topo);

    // fechar_arquivos();
    
    // ================= CONSULTAS =================
    if(!abrir_arquivos()) {
        printf("Falha ao abrir os arquivos\n");
        return 1;
    }

    prod_mostrar(0,30); // mostra os primeiros 30 produtos

    Produto p;
    if(prod_busca_bin(1004237, &p)) {
        printf("Pesquisa binaria retornou: %d\n", p.prod_id);

    }

    if(prod_consulta_idx(1004237, &p)) {
        printf("Indice: %d\n", p.prod_id);
    }

    Produto novo;
    novo.prod_id = 12345678;
    novo.preco   = 99.99;
    novo.cat_id  = 12345;
    formatar_registro(novo.marca,   "TESTE",     30);
    formatar_registro(novo.cat_cod, "teste.cat", 50);

    prod_inserir(&novo);
    
    prod_mostrar(0,50);

    prod_remover(novo.prod_id);

     // ============ AS 3 CONSULTAS ============
    consulta_prod_marca("apple");
    consulta_compradores();
    consulta_fat_categoria();

    fechar_arquivos();
    printf("Concluido!\n");
    return 0;
}