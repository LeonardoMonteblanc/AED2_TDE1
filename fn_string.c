#include "fn_string.h"
#include "tipos.h"

#include <string.h>
#include <stdlib.h>

//=============== FORMATAR_REGISTRO ===============
void str_formatar_registro(char* formatada, char* original, int tam) {
    int i = 0;

    while (i < tam - 1 && original[i] != '\0') {
        formatada[i] = original[i];
        i++;
    }

    while (i < tam - 1) {
        formatada[i] = ' ';
        i++;
    }
    
    formatada[tam - 1] = '\0';
}


//=============== COMPARAR_STRINGS =============== 
int str_comparar_strings(char* formatada, char* original) {
    size_t tam = strlen(original);
    size_t pos;

    if (strncmp(formatada, original, tam) != 0) {
        return 0;
    }

    pos = tam + strspn(formatada + tam, " ");

    if (formatada[pos] == '\0') {
        return 1;
    }

    return 0;
}


//=============== CONVERTER_EVENTO =============== 
unsigned char str_converter_evento(char* texto) {
    if (strcmp(texto, "view") == 0) {
        return 0;
    } else if (strcmp(texto, "cart") == 0) {
        return 1;
    } else if (strcmp(texto, "purchase") == 0) {
        return 2;
    }
    return -1;
}


//=============== REMOVER_QUEBRA =============== 
void str_remover_quebra(char* linha) {
    int i = 0;
    while (linha[i] != '\0') {
        if (linha[i] == '\r' || linha[i] == '\n') {
            linha[i] = '\0';
            return;
        }
        i++;
    }
}


//=============== SEPARAR_LINHA =============== 
int str_separar_linha(char* linha, char* campos[], int max) {
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

//=============== percorrer_linhas_csv =============== 
int percorrer_linhas_csv(FILE *arquivo_csv, char *linha, char *campos[9]) {
    while(fgets(linha, TAM_LINHA, arquivo_csv)) {
        str_remover_quebra(linha);

        int quant = str_separar_linha(linha, campos, 9);
        if(quant >= 9) {
            return 1; // sucesso
        }

        fprintf(stderr, "AVISO: linha csv invalida: %s\n", linha);
    }
    return 0;
}