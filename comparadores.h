#ifndef COMPARADORES_H
#define COMPARADORES_H

#include "tipos.h"

/**
 * @brief Compara dois produtos a partir do produto_id
 * 
 * @details Funcao usada no qsort
 * 
 * @param[in] a Ponteiro 1 para Produto
 * @param[in] b Ponteiro 2 para Produto
 * 
 * @return Retorna 0 se iguais | 1 se a > b e -1 se a < b
 */


int comparativo_produto(const void *a, const void *b);


/**
 * @brief Compara duas compras usando o usuario_id a partir de dois ponteiros para CompraRegistro
 * 
 * @details Funcao usando no qsort
 * 
 * @param[in] a Ponteiro 1 para CompraRegistro
 * @param[in] b Ponteiro 2 para CompraRegistro
 * 
 * @return Retorna 0 se iguais | 1 se a > b e -1 se a < b
 */
int comparativo_compra(const void *a, const void *b);


/**
 * @brief Compara dois agregados de usuario por total
 * 
 * @details Funcao usada no qsort para ordenar o maior total primeiro
 * 
 * @param[in] a Ponteiro 1 para AgregadoUsuario
 * @param[in] b Ponteiro 2 para AgregadoUsuario
 * 
 * @return Retorna 0 se iguais | 1 se a > b e -1 se a < b
 */
int comparativo_usuario_agreg(const void *a, const void *b);


// =============== COMPARATIVO_CATEGORIA_AGREGADO ===============
/**
 * @brief Compara dois agregados da categoria pelo total
 * 
 * @details Funcao usada no qsort para ordenar o maior faturamento
 * 
 * @param[in] a Ponteiro 1 para AgregadoCategoria
 * @param[in] b Ponteiro 2 para AgregadoCategoria
 * 
 * @return Retorna 0 se iguais | 1 se a > b e -1 se a < b
 */
int comparativo_cat_agreg(const void *a, const void *b);

#endif
