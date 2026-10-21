#include "fn_aux.h"

#include <string.h>
#include <stdlib.h>

/* =============== FORMATAR_REGISTRO ===============
 * Formata os registros para ter tam fixo adicionando os espaços */
void formatar_registro(char* formatada, char* original, int tam) {
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

/* =============== COMPARAR_STRINGS ===============
 * Compara as strings originais com as strings formatadas com espaços */
int comparar_strings(char* formatada, char* original) {
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

/* =============== CARREGAR_ARQUIVO ===============
 * Tentar carregar um arquivo, se não for possivel, retorna erro */
FILE *carregar_arquivo(char *caminho, char *modo) {
	FILE *f = fopen(caminho, modo);

	if(f == NULL){
		printf("Nao foi possivel abrir/criar: %s\n", caminho);
        exit(7);
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
