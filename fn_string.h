#ifndef FN_STRING_H
#define FN_STRING_H

#include <stdio.h>


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
 * @brief Remove caracteres de quebra de linha do fim da string
 *
 * @param[in,out] linha Ponteiro para string que será modificada
 */
void str_remover_quebra(char* linha);

/**
 * @brief Recebe a linha lida direto do CSV
 *
 * @details Percorre essa linha e para cada virgula que encontra, separa a string lida em campos
 *
 * @param linha  Linha de entrada a ser dividida.
 * @param campos Vetor de saída com os campos separados.
 * @param max    Número máximo de campos.
 *
 * @return Quantidade de campos lidos e armazenados
 */
int str_separar_linha(char* linha, char* campos[], int max);

/**
 * @brief Recebe o ponteiro do arquivo_csv e da linha
 * 
 * @details Faz um loop por cada linha removendo quebras, separando a linha e validando
 * 
 * @param[in] arquivo_csv arquivo csv que está aberto para leitura
 * @param[out] linha linha que é lida 
 * 
 * @param[out] campos depois que a linha foi "tratada", prenche o vetor e retorna (vetor que será escrito no arquivo bin)
 * 
 * @return 1 se sucesso | 0 se falha
 */
int percorrer_linhas_csv(FILE *arquivo_csv, char *linha, char *campos[9]);
#endif
