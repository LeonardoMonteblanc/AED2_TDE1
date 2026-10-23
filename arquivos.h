#ifndef ARQUIVOS_H
#define ARQUIVOS_H

#include <stdio.h>
#include "tipos.h"


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


/**
 * @brief Grava em disco os arquivos de indice
 * 
 * @param[in] contador Ponteiro com a quantidade de registros processados
 * @param[in] chave_registro Chave a ser gravada no indice
 * @param[in] posicao Posicao do registro
 * @param[out] indice_base Arquivo indice_base
 * @param[out] indice_topo Arquivo indice_topo
 */
void gravar_arquivo_indice(long long contador, int chave_registro, long long posicao, FILE *indice_base, FILE *indice_topo);


/**
 * @brief Carrega indice base e topo para a RAM
 * 
 * @details Acessa os arquivos arq_base e arq_topo e aloca na struct idx
 * 
 * @param[in] arq_base Arquivo do indice base
 * @param[in] arq_topo Arquivo do indice topo
 * @param[out] idx Estrutura que recebe os vetores
 * 
 * @return 1 se deu certo | 0 em caso de falha
 */
int carregar_idx(char* arq_base, char* arq_topo, Idx2Niveis* idx);


/**
 * @brief Desaloca o indice alocado em memoria pelo carregar_idx
 */
void liberar_indice(Idx2Niveis* idx);


/**
 * @brief Abre os arquivos binarios de dados e carrea os indices em memoria, usa os ponteiros globais
 * 
 * @return 1 em caso de sucesso | 0 em caso de falha
 */
int carregar_dados();

/**
 * @brief Desaloca os indices e fecha os arquivos aberto por carregar_dados()
 */
void liberar_dados();


/**
 * @brief Refaz o index de produtos quando há alguma modificação
 */
void reconstruir_idx_prod();





#endif


