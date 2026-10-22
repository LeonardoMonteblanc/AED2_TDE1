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
