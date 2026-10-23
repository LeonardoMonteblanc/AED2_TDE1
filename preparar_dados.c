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

// =============== MONTAR_REGISTRO_ACESSO ===============
//  * Preenche a struct usando os dados do csv
//  * Ordem dos campos no CSV:
//  * [0] event_time     [4] category_code
//  * [1] event_type     [5] brand
//  * [2] product_id     [6] price
//  * [3] category_id    [7] user_id
//  *                    [8] user_session
void montar_acesso(char* campos[], int n, Acesso* a) {
    a->id = n;
    a->usuario_id = atoi(campos[7]);
    a->produto_id = atoi(campos[2]);
    a->evento_tipo = str_converter_evento(campos[1]);
    a->preco = atof(campos[6]);
    str_formatar_registro(a->evento_time, campos[0], 24);
}

// =============== MONTAR_REGISTRO_PRODUTOS ===============
void montar_produtos(char* campos[], Produto* p) {
    p->id = atoi(campos[2]);
    p->categoria_id = atoll(campos[3]);
    p->preco = atof(campos[6]);
    str_formatar_registro(p->marca, campos[5], 30);
    str_formatar_registro(p->categoria_cod, campos[4], 50);
}


// =============== LER_PARTICOES ===============
int ler_particoes(int qtd_particoes, FILE **particoes, Produto *produtos_particao) {
    for(int i = 0; i < qtd_particoes; i++) {
        char nome[64];
        snprintf(nome, sizeof(nome), BIN_PART_PROD, i);

        particoes[i] = abrir_arquivo(nome, "rb");

        if(fread(&produtos_particao[i], sizeof(Produto), 1, particoes[i]) != 1) {
            fclose(particoes[i]);
            particoes[i] = NULL;
        }
    }

    return 1;
}


// =============== MENOR_PARTICAO ===============
int menor_particao(FILE **particoes, Produto *produtos_particao, int qtd_particoes) {
    int menor = -1;

    for(int i =0; i< qtd_particoes; i++) {
        if(!particoes[i]) {
            continue;
        }
        if(menor == -1 || produtos_particao[i].id < produtos_particao[menor].id) {
            menor = i;
        }
    }

    return menor;
}


// =============== GRAVAR_BINARIO_ACESSOS ===============
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

            contador++;

            gravar_arquivo_indice(&contador, acesso_registro.id, posicao, indice_base, indice_topo);
        }
        fclose(csv);
    }

    fclose(arq_acessos);
    fclose(indice_base);
    fclose(indice_topo);

    return contador;
}


// =============== GRAVAR_BINARIO_PRODUTOS ===============
void gravar_bin_produtos() {
    FILE *arq_temporario = abrir_arquivo(BIN_TEMP_PROD, "wb");

    char *arquivos_csv[2] = {CSV_OUT, CSV_NOV};

    char linha[TAM_LINHA];
    char *campos[9];

    for(int i = 0; i < 2; i++) {
        FILE *csv = abrir_arquivo(arquivos_csv[i], "r");
        fgets(linha, TAM_LINHA, csv); // pula cabeçalho

        printf("Processando produtos %s..\n", arquivos_csv[i]);

        while(formatar_linhas_csv(csv, linha, campos)) {
            Produto produto_registro;

            montar_produtos(campos, &produto_registro);
            fwrite(&produto_registro, sizeof(Produto), 1, arq_temporario);
        }
        fclose(csv);
    }

    fclose(arq_temporario);
    
}


// =============== PARTICIONAR_PRODUTOS ===============
int particionar_produtos() {
    FILE *arq_temporario, *saida;
    Produto *buffer;
    
    size_t bytes_buffer, lidos;
    
    int num_part;
    char nome[64];

    arq_temporario = abrir_arquivo(BIN_TEMP_PROD, "rb");

    bytes_buffer = (size_t) REGISTRO_BLOCO * sizeof(Produto);
    buffer = (Produto*) malloc(bytes_buffer);

    if (buffer == NULL) {
        printf("Erro: memoria insufieciente para buffer\n");
        fclose(arq_temporario);
        return 0;
    }

    num_part = 0;

    while ((lidos = fread(buffer, sizeof(Produto), REGISTRO_BLOCO, arq_temporario)) > 0) {
        qsort(buffer, lidos, sizeof(Produto), comparativo_produto);
        snprintf(nome, sizeof(nome), BIN_PART_PROD, num_part);

        saida = abrir_arquivo(nome, "wb");

        fwrite(buffer, sizeof(Produto), lidos, saida);
        fclose(saida);

        num_part++;
    }

    free(buffer);
    fclose(arq_temporario);
    return num_part;
}


// =============== INTERCALAR_PARTICOES ===============
void intercalar_particoes(int qtd_particoes) {
    if(qtd_particoes <= 0) {
        printf("Nao ha particoes para intercalar....\n");
        return;
    }

    // ALOCAÇÃO DE MEMORIA PARA LEITURA DAS PARTIÇÕES
    FILE **particoes = malloc(qtd_particoes * sizeof(FILE *));
    Produto *produtos_particao = malloc(qtd_particoes * sizeof(Produto));

    if(!particoes || !produtos_particao) {
        printf("Memoria insuficiente para alocar as particoes...\n");
        free(particoes);
        free(produtos_particao);
        return;
    }

    // ACESSO AS PARTIÇÕES
    ler_particoes(qtd_particoes, particoes, produtos_particao);

    FILE *arq_produtos = abrir_arquivo(BIN_PRODUTOS, "wb");
    FILE *indice_base = abrir_arquivo(IDX_BASE_PROD, "wb");
    FILE *indice_topo = abrir_arquivo(IDX_TOPO_PROD, "wb");

    int ultimo_id = -1;
    long long contador = 0;

    while(1) {
        int menor = menor_particao(particoes, produtos_particao, qtd_particoes);

        if(menor == -1)  {
            break;
        }

        //verificar se o produto é novo
        if(produtos_particao[menor].id != ultimo_id) {
            ultimo_id = produtos_particao[menor].id;

            long long posicao = ftell(arq_produtos);
            fwrite(&produtos_particao[menor], sizeof(Produto),1, arq_produtos);
            fputc('\n', arq_produtos);
            
            contador++;
            gravar_arquivo_indice(&contador, ultimo_id, posicao, indice_base, indice_topo);
        }

        if(fread(&produtos_particao[menor], sizeof(Produto), 1, particoes[menor]) != 1) {
            fclose(particoes[menor]);
            particoes[menor] = NULL;
        }
    }

    fclose(arq_produtos);
    for(int i =0; i<qtd_particoes; i++) {
        if(particoes[i]) {
            fclose(particoes[i]);
        }
    }

    free(particoes);
    free(produtos_particao);

    printf("Produtos unicos: %lld\n", contador);

}

