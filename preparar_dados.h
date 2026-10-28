#ifndef PREPARAR_DADOS_H
#define PREPARAR_DADOS_H

#include "tipos.h"
#include <stdio.h>


void montar_acesso(char* campos[], int n, Acesso* a);
void montar_produtos(char* campos[], Produto* p);

/**
 * @brief Abre as particoes, verifica o primeiro registro de cada
 * 
 * @param[in] qtd_particoes Quantidade de particoes a serem lidas
 * @param[out] particoes Lista dos arquivos de particoes
 * @param[out] produtos_particao Lista dos produtos lidos das particoes
 * 
 * @return 1 Sucesso
 */

int ler_particoes(int qtd_particoes, FILE **particoes, Produto *produtos_particao);


/**
 * @brief Recebe as particoes de produto, verifica o primeiro registro da cada e retorn ao menor deles
 * 
 * @param[in] qtd_particoes Quantidade de particoes existentes
 * @param[out] particoes Lista de arquivo das particoes
 * @param[out] produtos_particao Lista dos produtos com primeiro registro de cada particao
 * 
 * @return 1 se sucesso
 */
int menor_particao(Produto* produtos_particao, int qtd_particoes);


/**
 * @brief Le os dados e grava um binario de acessos.bin + os indices
 * 
 * @details Le cada linha de cada arquivo, trata as linhas atraves de outras funcoes, após isso, chama a função que cria os indices
 * 
 * @return quantidade total de registros gravados
 */
long long gravar_bin_acessos();

/**
 * @brief Le os dados, grava os registros em um arquivo binario somente com os dados de produtos
 * 
 * @details Nao gera indices, nem ordena ou remove duplicatas
 */
void gravar_bin_produtos();


/**
 * @brief Le o arquivo binario de produtos e gera partições ordenadas
 * 
 * @return Quantidade de particoes criadas se sucesso ou 0 em falha
 */
int particionar_produtos();


/**
 * @brief Intecala as particoes existentes removendo os ids duplicados e gerando um arquivo binario final + os indices
 * 
 * @details Etapas executadas: 1. Alocar as particoes | 2. Ler as particoes | 3. Abre produtos.bin e indices para escrever | 4. Encontra o menor id para gravar e segue até esgotar | 5. encerra
 * 
 * @param[in] qtd_particoes Quantidades de particoes por intercalar
 */
void intercalar_particoes(int qtd_particoes);


/**
 * @brief Executa todas as etapas de preparacao dos dados (CSV -> BIN + indices)
 * 
 * @details Ordem das etapas: 1. Gera acessos.bin + indices | 2. Gera temp_prod.bin
 * 3. Particiona temp_prod.bin | 4. Intercala as particoes -> produtos.bin + indices
 */
void executar_pipeline(void);

#endif