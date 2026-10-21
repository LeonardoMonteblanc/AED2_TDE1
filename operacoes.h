#ifndef OPERACOES_H
#define OPERACOES_H

#include "fn_aux.h"

extern FILE* p_acessos;
extern FILE* p_prod;

extern Idx2Niveis idx_acessos;
extern Idx2Niveis idx_prod;

extern long long n_acessos;
extern long long n_prod;

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
