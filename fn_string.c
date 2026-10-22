#include "fn_string.h"

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


FILE *abrir_arquivo(char *caminho, char *modo) {
	FILE *f = fopen(caminho, modo);

	if(f == NULL){
		printf("Nao foi possivel abrir/criar: %s\n", caminho);
        exit(1);
    }

	return f;
}



/* =============== FUNCOES AUXILIARES PARA COMPARAR ===============
*/
int comparativo_compra(const void *a, const void *b) {
    CompraRegistro *ca = (CompraRegistro *) a;
    CompraRegistro *cb = (CompraRegistro *) b;

    if(ca->user_id < cb->user_id) {
        return -1;
    }
    if(ca->user_id > cb->user_id) {
        return 1;
    }

    return 0;
}

int comparativo_usuario_agreg(const void *a, const void *b) {
    AgregadoUsuario *ca = (AgregadoUsuario *) a;
    AgregadoUsuario *cb = (AgregadoUsuario *) b;

    if(ca->total > cb->total) {
        return -1;
    }
    if(ca->total < cb->total) {
        return 1;
    }

    return 0;
}

int comparativo_cat_agreg(const void *a, const void *b) {
    AgregadoCat *ca = (AgregadoCat *) a;
    AgregadoCat *cb = (AgregadoCat *) b;

    if(ca->total > cb->total) {
        return -1;
    }
    if(ca->total < cb->total) {
        return 1;
    }

    return 0;
}
