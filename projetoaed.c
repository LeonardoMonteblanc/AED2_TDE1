#include "fn_aux.h"
#include "operacoes.h"
#include "preparar_dados.h"

#include <stdlib.h>
#include <stdio.h>

// CONSULTA 1: PRODUTOS POR MARCA
void consulta_prod_marca(char*marca) {
    long long i;
    int c = 0;

    printf("\n PRODUTOS DA MARCA %s\n", marca);
    fseek(p_prod, 0,SEEK_SET);

    for(i=0; i< n_prod; i++) {
        Produto p;

        if(fread(&p, sizeof(Produto),1,p_prod) != 1) {
            break;
        }

        fgetc(p_prod);

        if(comparar_strings(p.marca, marca)) {
            printf("  id=%d | R$ %.2f | %s\n", p.prod_id, p.preco, p.cat_cod);
            c++;
        }
    }

    printf("Total: %d produtos\n", c);
}

// CONSULTA 2: CLIENTES UNICOS E FAT DE CADA
void consulta_compradores() {
    long long i;
    long long cap = 1000000;
    long long n_compras = 0;
    int n_agreg = 0;
    double fat_total = 0;
    long long top;

    CompraRegistro  *compras = NULL;
    AgregadoUsuario *agreg = NULL;

    printf("Compradores\n");

    compras = (CompraRegistro*)malloc(cap*sizeof(CompraRegistro));
    
    if(compras == NULL) {
        printf("Nao foi possivel alocar memoria para consulta 2\n");
        return;
    }
    
    fseek(p_acessos, 0, SEEK_SET);

    for(i=0; i < n_acessos; i++) {
        Acesso a;

        if(fread(&a, sizeof(Acesso),1,p_acessos) != 1) {
            break;
        }
        fgetc(p_acessos);
        if(a.evento_tipo != 2) {
            continue;
        }

        if(n_compras >= cap) {
            cap *= 2;
            compras = (CompraRegistro *) realloc(compras, cap *sizeof(CompraRegistro));

            if(compras == NULL) {
                printf("Nao foi possivel alocar memoria para consulta 2\n");
                return;
            }
        }

        compras[n_compras].user_id = a.usuario_id;
        compras[n_compras].preco = a.preco;
        n_compras++;
    }

    printf("Compras: %lld\n", n_compras);

    if(n_compras == 0) {
        free(compras);
        return;
    }
    

    qsort(compras, n_compras, sizeof(CompraRegistro), comparativo_compra);

    agreg = (AgregadoUsuario *) malloc(n_compras * sizeof(AgregadoUsuario));

    if(agreg == NULL) {
        printf("Nao foi possivel alocar memoria para consulta 2\n");
        free(compras);
        return;
    }

    i = 0;

    while(i < n_compras) {
        int uid = compras[i].user_id;
        int qtd = 0;
        double tt = 0;

        while(i < n_compras && compras[i].user_id == uid) {
            qtd++;
            tt += compras[i].preco;
            i++;
        }

        agreg[n_agreg].user_id = uid;
        agreg[n_agreg].qtd = qtd;
        agreg[n_agreg].total = tt;
        n_agreg++;

        fat_total += tt;
    }

    printf("Clientes unicos: %d\n", n_agreg);
    printf("Faturamento total: R$ %.2f\n", fat_total);

    qsort(agreg, n_agreg, sizeof(AgregadoUsuario), comparativo_usuario_agreg);

    if(n_agreg < 10) {
        top = n_agreg;
    } else {
        top = 10;
    }

    printf("Top 10 por faturamento:\n");
    for(i=0 ; i < top; i++) {
        printf("user_id=%d | qtd=%d | R$ %.2f\n", agreg[i].user_id, agreg[i].qtd, agreg[i].total);
        
    }

    free(compras);
    free(agreg);
}

// CONSULTA 3: FATURAMENTO POR CATEGORIA
void consulta_fat_categoria() {
    long long i;

    int n_cats = 0;
    int cap = 1000;
    int j, encontrado;

    double fat_total = 0;

    AgregadoCat *cats;

    printf("\nFATURAMENTO POR CATEGORI\n");
    cats = (AgregadoCat *) malloc(cap * sizeof(AgregadoCat));

    if(cats == NULL) {
        printf("Nao foi possivel alocar memoria para consulta 3\n");
        return;
    }

    fseek(p_acessos, 0, SEEK_SET);

    for(i=0; i < n_acessos; i++) {
        Acesso a;
        Produto prod;

        if(fread(&a, sizeof(Acesso), 1, p_acessos) != 1) {
            break;
        }

        fgetc(p_acessos);

        if(a.evento_tipo != 2) {
            continue;
        }

        if(!prod_consulta_idx(a.prod_id, &prod)) {
            continue;
        }

        encontrado = -1;
        for(j = 0; j < n_cats; j++) {
            if(cats[j].cat_id == prod.cat_id) {
                encontrado = j;
                break;
            }
        }

        if(encontrado >= 0) {
            cats[encontrado].qtd++;
            cats[encontrado].total += a.preco;

        } else {
            if(n_cats >= cap) {
                cap *= 2;
                cats = (AgregadoCat *) realloc(cats, cap * sizeof(AgregadoCat));

                if(cats == NULL) {
                    printf("Nao foi possivel realocar memoria para consulta 3\n");
                    return;
                }
            }
        cats[n_cats].cat_id = prod.cat_id;
        cats[n_cats].qtd = 1;
        cats[n_cats].total = a.preco;
        n_cats++;
        }
    }
    for(j=0; j <n_cats; j++) {
        fat_total += cats[j].total;
    }
    printf("Categorias distintas: %d\n", n_cats);
    printf("Faturamento total: R$ %.2f\n", fat_total);
    qsort(cats, n_cats, sizeof(AgregadoCat), comparativo_cat_agreg);


    printf("Top 10 por faturamento:\n");
    for(j = 0; j < n_cats && j < 10; j++) {
        printf("cat_id=%lld | qtd=%d | R$ %.2f\n", cats[j].cat_id, cats[j].qtd, cats[j].total);
    }

    free(cats);
}


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