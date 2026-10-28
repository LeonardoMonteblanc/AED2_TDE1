#include "consultas.h"
#include "arquivos.h"
#include "comparadores.h"
#include "tipos.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// =============== PAGINACAO DA CONSULTA 1 ===============
#define LINHAS_PAGINA 25

/* pausa apenas com entrada interativa (tty real; o pty do Git Bash tambem
   e reconhecido pelo _isatty). pipes comuns (scripts) nao pausam. */
static int entrada_interativa(void) {
    return _isatty(_fileno(stdin));
}

static void pausar_paginacao(int linha) {
    int c;

    if(linha <= 0 || linha % LINHAS_PAGINA != 0 || !entrada_interativa()) {
        return;
    }

    printf("\n-- ENTER para continuar --\n");

    while((c = getchar()) != '\n' && c != EOF) {
    }
}

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

        printf("[%lld] id=%d | R$ %.2f | cat=%lld | %.30s | %.50s\n", ini+i, p.id, p.preco, p.categoria_id, p.marca, p.categoria_cod);
    }
}


// =============== EVENTO_TIPO_TEXTO ===============
const char* evento_tipo_texto(unsigned char tipo) {
    switch (tipo) {
        case 0:  return "view";
        case 1:  return "cart";
        case 2:  return "purchase";
        default: return "desconhecido";
    }
}


// =============== MOSTRAR_ACESSOS ===============
void mostrar_acessos(long long ini, long long qtd) {
    long long i;
    long long tam_reg = sizeof(Acesso) + 1;

    if(ini < 0) {
        ini = 0;
    }

    if(ini + qtd > n_acessos) {
        qtd = n_acessos - ini;
    }

    if(qtd <= 0) {
        printf("Nenhum acesso para mostrar\n");
        return;
    }

    fseek(p_acessos, ini * tam_reg, SEEK_SET);

    for(i = 0; i < qtd; i++) {
        Acesso a;

        if(fread(&a, sizeof(Acesso), 1, p_acessos) != 1) {
            break;
        }

        fgetc(p_acessos);

        printf("[%lld] id=%d | user=%d | prod=%d | %s | R$ %.2f | %.24s\n",
               ini+i, a.id, a.usuario_id, a.produto_id, evento_tipo_texto(a.evento_tipo), a.preco, a.evento_time);
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

        if(fread(out, sizeof(Produto), 1, p_prod) != 1) {
            return 0;
        }

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


// =============== BUSCA_BINARIA_ACESSOS ===============
int busca_bin_acessos(int acesso_id, Acesso *out) {
    long long esq = 0;
    long long dir = n_acessos - 1;
    long long tam_reg = sizeof(Acesso) + 1;

    while(esq <= dir) {
        long long meio = (esq + dir) / 2;

        fseek(p_acessos, meio * tam_reg, SEEK_SET);

        if(fread(out, sizeof(Acesso), 1, p_acessos) != 1) {
            return 0;
        }

        if(out->id == acesso_id) {
            return 1;
        }

        if(out->id < acesso_id) {
            esq = meio + 1;
        } else {
            dir = meio - 1;
        }
    }

    return 0;
}


// =============== BUSCAR_PRODUTO_COM_IDX ===============
int buscar_produto_com_idx(int prod_id, Produto *out) {
    int razao = INTERVALO_IDX_TOPO / INTERVALO_IDX_BASE;
    int itopo, ibase, ini_base, fim_base, i;
    long long pos;

    itopo = busca_bin_idx(idx_prod.topo, 0, idx_prod.qtd_topo - 1, prod_id);
    
    if(itopo < 0) {
        return 0;
    }

    ini_base = itopo * razao;
    fim_base = ini_base + razao - 1;

    if(fim_base >= idx_prod.qtd_base) {
        fim_base = idx_prod.qtd_base - 1;
    }

    ibase = busca_bin_idx(idx_prod.base, ini_base, fim_base, prod_id);

    if(ibase < 0) {
        return 0;
    }

    pos = idx_prod.base[ibase].pos;
    fseek(p_prod, pos, SEEK_SET);

    for(i=0; i < INTERVALO_IDX_BASE; i++) {
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


// =============== BUSCAR_ACESSO_COM_IDX ===============
int buscar_acesso_com_idx(int acesso_id, Acesso *out) {
    int razao = INTERVALO_IDX_TOPO / INTERVALO_IDX_BASE;
    int itopo, ibase, ini_base, fim_base, i;
    long long pos;

    itopo = busca_bin_idx(idx_acessos.topo, 0, idx_acessos.qtd_topo - 1, acesso_id);

    if(itopo < 0) {
        return 0;
    }

    ini_base = itopo * razao;
    fim_base = ini_base + razao - 1;

    if(fim_base >= idx_acessos.qtd_base) {
        fim_base = idx_acessos.qtd_base - 1;
    }

    ibase = busca_bin_idx(idx_acessos.base, ini_base, fim_base, acesso_id);

    if(ibase < 0) {
        return 0;
    }

    pos = idx_acessos.base[ibase].pos;
    fseek(p_acessos, pos, SEEK_SET);

    for(i = 0; i < INTERVALO_IDX_BASE; i++) {
        if(fread(out, sizeof(Acesso), 1, p_acessos) != 1) {
            return 0;
        }

        fgetc(p_acessos);

        if(out->id == acesso_id) {
            return 1;
        }

        if(out->id > acesso_id) {
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

    if(busca_bin_produtos(novo->id, &tmp)) {
        printf("Produto %d ja existe\n", novo->id);
        return 0;
    }

    out = abrir_arquivo(BIN_TEMP_PROD, "wb");
    fseek(p_prod, 0, SEEK_SET);

    for(i=0; i < n_prod; i++) {
        Produto p;

        if(fread(&p, sizeof(Produto),1, p_prod) != 1) {
            break;
        }

        fgetc(p_prod);

        if(!inserido && p.id > novo->id) {
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
    p_prod = abrir_arquivo(BIN_PRODUTOS, "rb+");

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

    if(!busca_bin_produtos(prod_id, &tmp)) {
        printf("Produto %d nao encontrado\n", prod_id);
        return 0;
    }

    out = abrir_arquivo(BIN_TEMP_PROD, "wb");
    fseek(p_prod, 0, SEEK_SET);

    for(i=0; i < n_prod; i++) {
        Produto p;

        if(fread(&p, sizeof(Produto), 1, p_prod) != 1) {
            break;
        }

        fgetc(p_prod);

        if(p.id != prod_id) {
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
    p_prod = abrir_arquivo(BIN_PRODUTOS, "rb+");

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

        if(str_comparar_strings(p.marca, marca)) {
            printf("  id=%d | R$ %.2f | %s\n", p.id, p.preco, p.categoria_cod);
            c++;
            pausar_paginacao(c);
        }
    }

    printf("Total: %d produtos\n", c);
}

// CONSULTA 2: CLIENTES UNICOS E FAT DE CADA
/*
DIVIDIR EM 
LER_COMPRAS
AGRUPAR_POR_USUARIO
EXIBIR

*/
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
            CompraRegistro *maior;

            cap *= 2;
            maior = (CompraRegistro *) realloc(compras, cap * sizeof(CompraRegistro));

            if(maior == NULL) {
                printf("Nao foi possivel alocar memoria para consulta 2\n");
                free(compras);
                return;
            }

            compras = maior;
        }

        compras[n_compras].usuario_id = a.usuario_id;
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
        int uid = compras[i].usuario_id;
        int qtd = 0;
        double tt = 0;

        while(i < n_compras && compras[i].usuario_id == uid) {
            qtd++;
            tt += compras[i].preco;
            i++;
        }

        agreg[n_agreg].usuario_id = uid;
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
        printf("user_id=%d | qtd=%d | R$ %.2f\n", agreg[i].usuario_id, agreg[i].qtd, agreg[i].total);
        
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

    AgregadoCategoria *cats;

    printf("\nFATURAMENTO POR CATEGORIA\n");
    cats = (AgregadoCategoria*) malloc(cap * sizeof(AgregadoCategoria));

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

        if(!buscar_produto_com_idx(a.produto_id, &prod)) {
            continue;
        }

        encontrado = -1;
        for(j = 0; j < n_cats; j++) {
            if(cats[j].categoria_id == prod.categoria_id) {
                encontrado = j;
                break;
            }
        }

        if(encontrado >= 0) {
            cats[encontrado].qtd++;
            cats[encontrado].total += a.preco;

        } else {
            if(n_cats >= cap) {
                AgregadoCategoria *maior;

                cap *= 2;
                maior = (AgregadoCategoria *) realloc(cats, cap * sizeof(AgregadoCategoria));

                if(maior == NULL) {
                    printf("Nao foi possivel realocar memoria para consulta 3\n");
                    free(cats);
                    return;
                }

                cats = maior;
            }
        cats[n_cats].categoria_id = prod.categoria_id;
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
    qsort(cats, n_cats, sizeof(AgregadoCategoria), comparativo_cat_agreg);


    printf("Top 10 por faturamento:\n");
    for(j = 0; j < n_cats && j < 10; j++) {
        printf("cat_id=%lld | qtd=%d | R$ %.2f\n", cats[j].categoria_id, cats[j].qtd, cats[j].total);
    }

    free(cats);
}
