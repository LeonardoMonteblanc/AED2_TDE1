#ifndef FN_STRING_H
#define FN_STRING_H
/** 
 * @brief Formata o registro atual extraido do CSV para um tamanho fixo adicionando espaços ao fim
 *
 * @param[out] formatada Ponteiro que recebe a string original depois de formatada
 * @param[in]  original  Ponteiro que entrega a string original para ser formatada
 * @param[in]  tam       Inteiro do tamanho do registro
 */
void str_formatar_registro(char* formatada, char* original, int tam);


/** 
 * @brief Compara uma string formatada e compara com ela no formato original
 *
 * @param[in] formatada Ponteiro que entrega a string formatada para comparar
 * @param[in] original  Ponteiro que entrega a string original para comparar
 *
 * @return Retorna 1 ou 0 determinando se é ou não igual as strings
 */
int str_comparar_strings(char* formatada, char* original);


/**
 * @brief Converte o texto do evento para um codigo
 * @details Padronixa para: 0 => "view" | 1 => "cart" | 2 => "purchase" e -1 => outros/desconhecido
 * 
 * @param[in] texto String com nome do evento
 * 
 * @return Código do evento padronizado
 */
unsigned char str_converter_evento(char* texto);


/**
 * @brief Remove quebra de linha ('\n' e\ou '\r') no fim da string
 * 
 * @param[in,out] linha Ponteiro para string que será modificada
 */
void str_remover_quebra(char *linha);


/**
 * @brief Recebe a linha lida direto do CSV 
 * 
 * @details Percorre essa linha e para cada virgula que encontra, separa a string lida em campos
 * 
 * @param[in,out] linha Ponteiro da string a ser dividida
 * @param[out] campo Vetor que recebe os ponteiros para o campo estradio
 * @param[in] max Numero maximo de campos a ser extraidos da linha
 * 
 * @return Quantidade de campos lidos e armazenados
 */
int str_separar_linha(char* linha, char* campos[], int max)



//=============== ABRIR_ARQUIVOS =============== 
/** 
 * @brief Recebe um ponteiro do caminho do arquivo e o modo de acesso
 *
 * @details Função tenta abrir o arquivo no modo indicado
 * @warning se não for possivel, encerra o programa e chama exit(1)
 *
 * @param[in] caminho Ponteiro com caminho do arquivo
 * @param[in] modo    Modo de acesso ao arquivo
 *
 * @return Ponteiro para FILE ou NULL
 */

FILE * abrir_arquivo(char *caminho, char *modo);
int comparativo_compra(const void *a, const void *b);
int comparativo_usuario_agreg(const void *a, const void *b);
int comparativo_cat_agreg(const void *a, const void *b);

#endif
