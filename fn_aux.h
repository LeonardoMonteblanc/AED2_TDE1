#ifndef FN_AUX_H
#define FN_AUX_H


void formatar_registro(char* formatada, char* original, int tam);
int comparar_strings(char* formatada, char* original);
FILE *carregar_arquivo(char *caminho, char *modo);
int comparativo_compra(const void *a, const void *b);
int comparativo_usuario_agreg(const void *a, const void *b);
int comparativo_cat_agreg(const void *a, const void *b);

#endif
