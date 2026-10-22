#ifndef OPERACOES_H
#define OPERACOES_H

#include "fn_aux.h"


int abrir_arquivos();
void fechar_arquivos();

void prod_mostrar(long long ini, long long qtd);
int prod_busca_bin(int prod_id, Produto *out);
int prod_consulta_idx(int prod_id, Produto *out);
int prod_inserir(Produto *novo);
int prod_remover(int prod_id);

void acessos_mostrar(long long ini, long long qtd);
int acessos_busca_bin(int ac_id, Acesso *out);
int acessos_consulta_idx(int ac_id, Acesso *out);

#endif
