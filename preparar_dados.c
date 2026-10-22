#include "preparar_dados.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fn_aux.h"


/* =============== CONVERTER_EVENTO ===============
 * Padroniza os eventos para int
 * 0 => "view"
 * 1 => "cart"
 * 2 => "purchase"
 * -1 => outros/desconhecido*/
unsigned char converter_evento(char* texto) {
    if (strcmp(texto, "view") == 0) {
        return 0;
    } else if (strcmp(texto, "cart") == 0) {
        return 1;
    } else if (strcmp(texto, "purchase") == 0) {
        return 2;
    }
    return -1;
}

/* =============== REMOVER_QUEBRA_LINHA ===============
 * Remove /r e /r das linhas */
void remover_quebra(char* linha) {
    int i = 0;
    while (linha[i] != '\0') {
        if (linha[i] == '\r' || linha[i] == '\n') {
            linha[i] = '\0';
            return;
        }
        i++;
    }
}

/* =============== SEPARAR_POR_VIRGULA ===============
 * Separa a string por virgulas */
int separar_campos(char* linha, char* campos[], int max) {
    int quant = 1;
    int i = 0;

    campos[0] = linha;

    while (linha[i] != '\0' && quant < max) {
        if (linha[i] == ',') {
            linha[i] = '\0';
            campos[quant] = &linha[i + 1];
            quant++;
        }
        i++;
    }

    return quant;
}

/* =============== MONTAR_REGISTRO_ACESSO ===============
 * Preenche a struct usando os dados do csv
 * Ordem dos campos no CSV:
 * [0] event_time     [4] category_code
 * [1] event_type     [5] brand
 * [2] product_id     [6] price
 * [3] category_id    [7] user_id
 *                    [8] user_session
 */
void montar_acesso(char* campos[], int n, Acesso* a) {
    a->acesso_id = n;
    a->usuario_id = atoi(campos[7]);
    a->prod_id = atoi(campos[2]);
    a->evento_tipo = converter_evento(campos[1]);
    a->preco = atof(campos[6]);
    formatar_registro(a->evento_time, campos[0], 24);
}

/* =============== MONTAR_REGISTRO_PRODUTOS ===============
 * Preenche a struct usando os dados do csv */
void montar_produtos(char* campos[], Produto* p) {
    p->prod_id = atoi(campos[2]);
    p->cat_id = atoll(campos[3]);
    p->preco = atof(campos[6]);
    formatar_registro(p->marca, campos[5], 30);
    formatar_registro(p->cat_cod, campos[4], 50);
}

/* =============== GERAR_BIN ===============
 * Acessos: 1 acesso por linha
 * Acessos_base: 1 EntradaIdx a cada 100 acessos
 * Acessos_topo: 1 EntradaIdx a cada 1000 acessos
 * Temp_prod: 1 prod por linha */
void gerar_bin() {
    FILE* acessos, *base, * topo, * temp_prod;
    int cont, i, ignoradas;

    char* arq_csv[2] = {CSV_OUT, CSV_NOV};

    acessos = carregar_arquivo(BIN_ACESSOS, "wb");
    base = carregar_arquivo(IDX_BASE, "wb");
    topo = carregar_arquivo(IDX_TOPO, "wb");
    temp_prod = carregar_arquivo(BIN_TEMP_PROD, "wb");

    cont = 0;
    ignoradas = 0;
    for (i = 0; i < 2; i++) {
        FILE* csv;
        char linha[TAM_LINHA];

        csv = carregar_arquivo(arq_csv[i], "r");

        if (fgets(linha, TAM_LINHA, csv) == NULL) {
            printf("%s: vazio ou somente cabecalho\n", arq_csv[i]);
            fclose(csv);
            continue;
        }

        printf("Processando %s...\n", arq_csv[i]);

        while (fgets(linha, TAM_LINHA, csv) != NULL) {
            char* campos[9];
            int quant;
            Acesso a;
            Produto prod;
            long long pos;

            remover_quebra(linha);
            quant = separar_campos(linha, campos, 9);
			
			if(quant < 9) {
				ignoradas++;
				continue;
			}

            montar_acesso(campos, cont + 1, &a);
            pos = ftell(acessos);

            fwrite(&a, sizeof(Acesso), 1, acessos);
            fputc('\n', acessos);

            if (cont % ACESSO_BASE == 0) {
                EntradaIdx e;
                e.chave = a.acesso_id;
                e.pos = pos;
                fwrite(&e, sizeof(EntradaIdx), 1, base);
            }

            if (cont % ACESSO_TOPO == 0) {
                EntradaIdx e;
                e.chave = a.acesso_id;
                e.pos = pos;
                fwrite(&e, sizeof(EntradaIdx), 1, topo);
            }

            montar_produtos(campos, &prod);
            fwrite(&prod, sizeof(Produto), 1, temp_prod);
            cont++;
        }

        fclose(csv);
    }
    fclose(acessos);
    fclose(base);
    fclose(topo);
    fclose(temp_prod);
}

/* =============== COMPARAR_PROD ===============
Compara os ID de produto
*/
int comparar_prod(const void* a, const void* b) {
    const Produto* pa = (const Produto*)a;
    const Produto* pb = (const Produto*)b;

    if (pa->prod_id < pb->prod_id) {
        return -1;
    }

    if (pa->prod_id > pb->prod_id) {
        return 1;
    }

    return 0;
}

/* =============== GERAR_PART_ORDENADA ===============
Gera uma partição ordenada de produtos apenas com produtos únicos
*/
int gerar_part_ordenada() {
    FILE* temp, * saida;
    Produto* buffer;
    
    size_t bytes_buffer, lidos;
    
    int num_part;
    char nome[64];

    temp = carregar_arquivo(BIN_TEMP_PROD, "rb");

    bytes_buffer = (size_t) REGISTRO_BLOCO * sizeof(Produto);
    buffer = (Produto*) malloc(bytes_buffer);

    if (buffer == NULL) {
        printf("Erro: memoria insufieciente para buffer\n");
        fclose(temp);
        return 0;
    }

    num_part = 0;

    while ((lidos = fread(buffer, sizeof(Produto), REGISTRO_BLOCO, temp)) > 0) {
        qsort(buffer, lidos, sizeof(Produto), comparar_prod);
        snprintf(nome, sizeof(nome), BIN_PART_PROD, num_part);

        saida = carregar_arquivo(nome, "wb");

        fwrite(buffer, sizeof(Produto), lidos, saida);
        fclose(saida);

        num_part++;
    }

    free(buffer);
    fclose(temp);
    return num_part;
}

/* =============== INTERCALA_PART_PROD ===============
Intercala as partições de produto removendo os repetidos e ordenando
*/
void intercalar_part_prod(int total_part) {
    FILE** parts;
    Produto* atuais;

    FILE* saida, * base, * topo;

    int ult_prod_id, gravados, i;
    long long pos;

    if (total_part <= 0) {
        printf("Nao ha particoes para intercalar\n");
        return;
    }

    parts = (FILE**)malloc(total_part * sizeof(FILE*));
    atuais = (Produto*)malloc(total_part * sizeof(Produto));

    if (parts == NULL || atuais == NULL) {
        printf("Erro: sem memoria para intercalar\n");
        if (parts != NULL){
            free(parts);
        }

        if (atuais != NULL){
            free(atuais);
        }

        return;
    }

    for (i = 0; i < total_part; i++) {
        char nome[64];

        snprintf(nome, sizeof(nome), BIN_PART_PROD, i);
        parts[i] = carregar_arquivo(nome, "rb");
        
        if (fread(&atuais[i], sizeof(Produto), 1, parts[i]) != 1) {
            fclose(parts[i]);
            parts[i] = NULL;
        }
    }

    saida = carregar_arquivo(BIN_PRODUTOS, "wb");
    base = carregar_arquivo(IDX_BASE_PROD, "wb");
    topo = carregar_arquivo(IDX_TOPO_PROD, "wb");

    ult_prod_id = -1;
    gravados = 0;

    while (1) {
        int menor;
        menor = -1;

        for (i = 0; i < total_part; i++) {
            if (parts[i] == NULL) {
                continue;
            }

            if (menor == -1 || atuais[i].prod_id < atuais[menor].prod_id) {
                menor = i;
            }
        }

        if (menor == -1) {
            break;
        }

        if (atuais[menor].prod_id != ult_prod_id) {
            ult_prod_id = atuais[menor].prod_id;

            pos = ftell(saida);
            fwrite(&atuais[menor], sizeof(Produto), 1, saida);
            fputc('\n', saida);

            if (gravados % PROD_BASE == 0) {
                EntradaIdx e;
                e.chave = atuais[menor].prod_id;
                e.pos = pos;
                fwrite(&e, sizeof(EntradaIdx), 1, base);
            }

            if (gravados % PROD_TOPO == 0) {
                EntradaIdx e;
                e.chave = atuais[menor].prod_id;
                e.pos = pos;
                fwrite(&e, sizeof(EntradaIdx), 1, topo);
            }
            gravados++;
        }
        if (fread(&atuais[menor], sizeof(Produto), 1, parts[menor]) != 1) {
            fclose(parts[menor]);
            parts[menor] = NULL;
        }
    }

    fclose(saida);
    fclose(base);
    fclose(topo);
    free(parts);
    free(atuais);

    printf("Produtos unicos %d\n", gravados);
}

/* =============== GERAR_PROD ===============
Particiona, ordena, intercala e remove os temporarios */
void gerar_produtos() {
    int n_parts, i;
    char nome[64];

    n_parts = gerar_part_ordenada();

    intercalar_part_prod(n_parts);
    remove(BIN_TEMP_PROD);

    for (i = 0; i < n_parts; i++) {
        snprintf(nome, sizeof(nome), BIN_PART_PROD, i);
        remove(nome);
    }

}

void tratar_dados() {
    printf("Convertendo CSV para BIN\n");
    gerar_bin();

    printf("Ordenando produtos\n");
    gerar_produtos();
}