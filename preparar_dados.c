#include "preparar_dados.h"

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fn_string.h"
#include "tipos.h"
#include "arquivos.h"
#include "comparadores.h"

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
    a->id = n;
    a->usuario_id = atoi(campos[7]);
    a->produto_id = atoi(campos[2]);
    a->evento_tipo = str_converter_evento(campos[1]);
    a->preco = atof(campos[6]);
    str_formatar_registro(a->evento_time, campos[0], 24);
}

/* =============== MONTAR_REGISTRO_PRODUTOS ===============
 * Preenche a struct usando os dados do csv */
void montar_produtos(char* campos[], Produto* p) {
    p->id = atoi(campos[2]);
    p->categoria_id = atoll(campos[3]);
    p->preco = atof(campos[6]);
    str_formatar_registro(p->marca, campos[5], 30);
    str_formatar_registro(p->categoria_cod, campos[4], 50);
}

/* =============== GERAR_BIN ===============
 * Acessos: 1 acesso por linha
 * Acessos_base: 1 EntradaIdx a cada 100 acessos
 * Acessos_topo: 1 EntradaIdx a cada 1000 acessos
 * Temp_prod: 1 prod por linha */

 // CONVERTE ARQUIVO CSV DE ARQ_ACESSOS PARA 
// void converter_csv_to_bin(FILE *arq_acessos, Acesso *acesso_registro, long long contador, FILE *indice_base, FILE *indice_topo) {
//     long long posicao = ftell(arq_acessos);

//     fwrite(acesso_registro, sizeof(Acesso), 1, arq_acessos);
//     fputc('\n', arq_acessos);

//     gravar_arquivo_indice(contador, acesso_registro->id, posicao, indice_base, indice_topo);

//     (*contador)++;
// }

//NOVA FUNÇÃO




long long gravar_bin_acessos() {
    FILE *arq_acessos = abrir_arquivo(BIN_ACESSOS, "wb");
    FILE *indice_base = abrir_arquivo(IDX_BASE, "wb");
    FILE *indice_topo = abrir_arquivo(IDX_TOPO, "wb");

    long long contador = 0;
    char linha[TAM_LINHA];
    char *campos[9];

    char *arquivos_csv[2] = {CSV_OUT, CSV_NOV};
    
    for(int i = 0; i < 2; i++) {
        FILE *csv = abrir_arquivo(arquivos_csv[i], "r");

        fgets(linha, TAM_LINHA, csv); // pula cabeçalho
        printf("Processano acessos %s\n", arquivos_csv[i]);

        while(formatar_linhas_csv(csv, linha, campos)) {
            Acesso acesso_registro;

            montar_acesso(campos, contador+1, &acesso_registro);

            long long posicao = ftell(arq_acessos);

            fwrite(&acesso_registro, sizeof(Acesso), 1, arq_acessos);
            fputc('\n', arq_acessos);

            gravar_arquivo_indice(&contador, acesso_registro.id, posicao, indice_base, indice_topo);
        }
        fclose(csv);
    }

    fclose(arq_acessos);
    fclose(indice_base);
    fclose(indice_topo);

    return contador;
}

void gerar_bin() {
    FILE* acessos, *base, * topo, *temp_prod;
    int cont, i, ignoradas;

    char* arq_csv[2] = {CSV_OUT, CSV_NOV};

    acessos = abrir_arquivo(BIN_ACESSOS, "wb");
    base = abrir_arquivo(IDX_BASE, "wb");
    topo = abrir_arquivo(IDX_TOPO, "wb");
    temp_prod = abrir_arquivo(BIN_TEMP_PROD, "wb");

    cont = 0;
    ignoradas = 0;
    for (i = 0; i < 2; i++) {
        FILE* csv;
        char linha[TAM_LINHA];

        csv = abrir_arquivo(arq_csv[i], "r");

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

            str_remover_quebra(linha);
            quant = str_separar_linha(linha, campos, 9);
			
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
                e.chave = a.id;
                e.pos = pos;
                fwrite(&e, sizeof(EntradaIdx), 1, base);
            }

            if (cont % ACESSO_TOPO == 0) {
                EntradaIdx e;
                e.chave = a.id;
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



/* =============== GERAR_PART_ORDENADA ===============
Gera uma partição ordenada de produtos apenas com produtos únicos
*/
int gerar_part_ordenada() {
    FILE* temp, * saida;
    Produto* buffer;
    
    size_t bytes_buffer, lidos;
    
    int num_part;
    char nome[64];

    temp = abrir_arquivo(BIN_TEMP_PROD, "rb");

    bytes_buffer = (size_t) REGISTRO_BLOCO * sizeof(Produto);
    buffer = (Produto*) malloc(bytes_buffer);

    if (buffer == NULL) {
        printf("Erro: memoria insufieciente para buffer\n");
        fclose(temp);
        return 0;
    }

    num_part = 0;

    while ((lidos = fread(buffer, sizeof(Produto), REGISTRO_BLOCO, temp)) > 0) {
        qsort(buffer, lidos, sizeof(Produto), comparativo_produto);
        snprintf(nome, sizeof(nome), BIN_PART_PROD, num_part);

        saida = abrir_arquivo(nome, "wb");

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
        parts[i] = abrir_arquivo(nome, "rb");
        
        if (fread(&atuais[i], sizeof(Produto), 1, parts[i]) != 1) {
            fclose(parts[i]);
            parts[i] = NULL;
        }
    }

    saida = abrir_arquivo(BIN_PRODUTOS, "wb");
    base = abrir_arquivo(IDX_BASE_PROD, "wb");
    topo = abrir_arquivo(IDX_TOPO_PROD, "wb");

    ult_prod_id = -1;
    gravados = 0;

    while (1) {
        int menor;
        menor = -1;

        for (i = 0; i < total_part; i++) {
            if (parts[i] == NULL) {
                continue;
            }

            if (menor == -1 || atuais[i].id < atuais[menor].id) {
                menor = i;
            }
        }

        if (menor == -1) {
            break;
        }

        if (atuais[menor].id != ult_prod_id) {
            ult_prod_id = atuais[menor].id;

            pos = ftell(saida);
            fwrite(&atuais[menor], sizeof(Produto), 1, saida);
            fputc('\n', saida);

            if (gravados % PROD_BASE == 0) {
                EntradaIdx e;
                e.chave = atuais[menor].id;
                e.pos = pos;
                fwrite(&e, sizeof(EntradaIdx), 1, base);
            }

            if (gravados % PROD_TOPO == 0) {
                EntradaIdx e;
                e.chave = atuais[menor].id;
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