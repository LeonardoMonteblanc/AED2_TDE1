#ifndef CONSULTAS_H
#define CONSULTAS_H

#include "fn_string.h"
#include "tipos.h"

/**
 * @brief Busca binaria do intervalo onde <= chave
 * 
 * @param[in] lista Vetor de indices
 * @param[in] ini pos inicial do intervalo
 * @param[in] fim pos final do intervalo
 * @param[in] chave chave que vai ser buscada no indice
 * 
 * @return indice da ultima entrada
 */
int busca_bin_idx(EntradaIdx* lista, int ini, int fim, int chave);


/**
 * @brief Imprime os produtos entre as posicoes informadas
 * 
 * @param[in] ini inicial do arquivo
 * @param[in] qtd quantidade de produtos que quer ser visto
 * 
 * @note calcula as posições que deve ser lido do arquivo a partir de ini + tam do registro + qtd
 */
void mostrar_produtos(long long ini, long long qtd);


/**
 * @brief Faz uma busca binaria de prod_id no arquivo de produtos.bin
 * 
 * @details só funciona se produtos.bin estiver ordenado pelo id
 * 
 * @param[in] prod_id id que está sendo buscado
 * @param[out] out ponteiro que recebe o produto encontrado
 * 
 * @return 1 se encontrou | 0 se não encontrou
 */
int busca_bin_produtos(int prod_id, Produto *out);


/**
 * @brief Traduz o codigo do tipo de evento para texto legivel
 *
 * @param[in] tipo codigo do evento (0 = view, 1 = cart, 2 = purchase)
 *
 * @return string com o nome do evento
 */
const char* evento_tipo_texto(unsigned char tipo);


/**
 * @brief Imprime os acessos entre as posicoes informadas
 *
 * @param[in] ini posicao inicial do arquivo (0 = comeco)
 * @param[in] qtd quantidade de acessos que quer ser visto
 *
 * @note calcula as posicoes que devem ser lidas do arquivo a partir de ini + tam do registro + qtd
 */
void mostrar_acessos(long long ini, long long qtd);


/**
 * @brief Faz uma busca binaria de acesso_id direto no arquivo de acessos.bin
 *
 * @details só funciona se acessos.bin estiver ordenado pelo id
 *
 * @param[in] acesso_id id sequencial que está sendo buscado
 * @param[out] out ponteiro que recebe o acesso encontrado
 *
 * @return 1 se encontrou | 0 se não encontrou
 */
int busca_bin_acessos(int acesso_id, Acesso *out);


/**
 * @brief Busca um acesso usando os indices de 2 niveis de acessos
 *
 * @details localiza no indice topo o intervalo e depois busca dentro do indice base
 * a posicao onde esta o acesso_id e entao percorre o arquivo de dados
 *
 * @param[in] acesso_id id do acesso buscado
 * @param[out] out ponteiro que recebe o acesso encontrado
 *
 * @return 1 se encontrou | 0 se não encontrou
 */
int buscar_acesso_com_idx(int acesso_id, Acesso *out);


/**
 * @brief Busca um produto usando os dois indices
 *
 * @details localiza no indice topo o intervalo e depois busca dentro do indice base
 * localiza o intervalo onde esta o prod_id e entao abre o arquivo de dados
 * percorre o arquivo buscando o id
 *
 * @param[in] prod_id id do produto buscado
 * @param[out] out Ponteiro de produto que recebe o valor encontrado
 *
 * @return 1 se encontrou | 0 se não encontrou
 */
int buscar_produto_com_idx(int prod_id, Produto *out);


/**
 * @brief isnere um produto no arquivo binário mantendo a ordenação por id
 * 
 * @details se produto existe, nao faz nada
 * se produto não existe, cria um arquivo temp, copia para lá e intercala
 * com a inserção na posicao correta e reconstroi o indice
 * 
 * @param[in] novo produto que vai ser inserido
 * 
 * @note cria novo BIN_PRODUTOS e chama reconstruir_idx
 * @return 1 se a inserção ocorreu | 0 se nao foi ou produto existe
 */
int inserir_produto(Produto *novo);


/**
 * @brief Remove o produto do arquivo usando o id
 * @details se produto nao existe, nao faz nada
 * se produto existe, cria um arquivo temp, copia para lá e intercala
 * com a remocao na posicao correta e reconstroi o indice 
 * 
 * @param[in] prod_id id do produto que vai ser removido
 * @note cria novo BIN_PRODUTOS e chama reconstruir_idx
 *
 * @return retorna 1 se a remocao ocorreu | 0 se não foi
 */
int remover_produto(int prod_id);


void consulta_prod_marca(char *marca);
void consulta_compradores();
void consulta_fat_categoria();

#endif
