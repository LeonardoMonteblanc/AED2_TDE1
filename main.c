#include "tipos.h"
#include "preparar_dados.h"
#include "arquivos.h"
#include "consultas.h"
#include "fn_string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* p_acessos = NULL;
FILE* p_prod = NULL;

Idx2Niveis idx_acessos = {NULL, 0, NULL, 0};
Idx2Niveis idx_prod = {NULL, 0, NULL, 0};

long long n_acessos = 0;
long long n_prod = 0;

static void imprimir_produto(const Produto* p) {
    printf("  id=%d | R$ %.2f | cat=%lld | %s | %s\n", p->id, p->preco, p->categoria_id, p->marca, p->categoria_cod);
}

static void imprimir_acesso(const Acesso* a) {
    printf("  id=%d | user=%d | prod=%d | %s | R$ %.2f | %s\n", a->id, a->usuario_id, a->produto_id, evento_tipo_texto(a->evento_tipo), a->preco, a->evento_time);
}

static int ler_linha(char* buf, int tam) {
    if (!fgets(buf, tam, stdin)) {
        return 0;
    }
    buf[strcspn(buf, "\n")] = 0;
    return 1;
}

static long long ler_ll(const char* msg) {
    char buf[64];
    printf("%s", msg);
    ler_linha(buf, sizeof(buf));
    return atoll(buf);
}

static int ler_int(const char* msg) {
    return (int)ler_ll(msg);
}

static double ler_double(const char* msg) {
    char buf[64];
    printf("%s", msg);
    ler_linha(buf, sizeof(buf));
    return atof(buf);
}

static int confirmar(const char* msg) {
    char buf[16];
    printf("%s (s/n): ", msg);
    ler_linha(buf, sizeof(buf));
    return buf[0] == 's' || buf[0] == 'S';
}

static void garantir_carregado(void) {
    if (!dados_prontos()) {
        printf("Preparando dados...\n");
        executar_pipeline();
    }
    if (!carregar_dados()) {
        printf("Erro ao carregar dados.\n");
        exit(1);
    }
}

int main(void) {
    garantir_carregado();

    char buf[128];
    int op;

    while (1) {
        printf("\n=== MENU ===\n");
        printf("1) Preparar dados (CSV -> BIN + indices)\n");
        printf("2) Produtos por marca\n");
        printf("3) Compradores e faturamento\n");
        printf("4) Faturamento por categoria\n");
        printf("5) Buscar produto por ID\n");
        printf("6) Buscar acesso por ID\n");
        printf("7) Listar produtos\n");
        printf("8) Listar acessos\n");
        printf("9) Inserir produto\n");
        printf("10) Remover produto\n");
        printf("0) Sair\n> ");

        if (!ler_linha(buf, sizeof(buf))) break;
        op = atoi(buf);

        if (op == 0) break;

        switch (op) {
            case 1:
                if (dados_prontos() && !confirmar("Reprocessar dados?")) break;
                liberar_dados();
                executar_pipeline();
                garantir_carregado();
                printf("Dados preparados.\n");
                break;

            case 2: {
                printf("Marca: ");
                if (!ler_linha(buf, sizeof(buf)) || !buf[0]) { printf("Invalido.\n"); break; }
                consulta_prod_marca(buf);
                break;
            }

            case 3:
                consulta_compradores();
                break;

            case 4:
                consulta_fat_categoria();
                break;

            case 5: {
                long long id = ler_ll("ID produto: ");
                Produto p;
                if (buscar_produto_com_idx((int)id, &p)) {
                    printf("Encontrado:\n");
                    imprimir_produto(&p);
                } else {
                    printf("Produto %lld nao encontrado.\n", id);
                }
                break;
            }

            case 6: {
                long long id = ler_ll("ID acesso: ");
                Acesso a;
                if (buscar_acesso_com_idx((int)id, &a)) {
                    printf("Encontrado:\n");
                    imprimir_acesso(&a);
                } else {
                    printf("Acesso %lld nao encontrado.\n", id);
                }
                break;
            }

            case 7: {
                long long ini = ler_ll("Posicao inicial: ");
                long long qtd = ler_ll("Quantidade: ");
                if (qtd > 0) mostrar_produtos(ini, qtd);
                break;
            }

            case 8: {
                long long ini = ler_ll("Posicao inicial: ");
                long long qtd = ler_ll("Quantidade: ");
                if (qtd > 0) mostrar_acessos(ini, qtd);
                break;
            }

            case 9: {
                Produto novo = {0};
                novo.id = ler_int("ID: ");
                if (novo.id <= 0) { printf("ID invalido.\n"); break; }
                novo.preco = ler_double("Preco: ");
                if (novo.preco < 0) { printf("Preco invalido.\n"); break; }
                novo.categoria_id = ler_ll("ID categoria: ");
                printf("Marca: "); ler_linha(buf, sizeof(buf)); str_formatar_registro(novo.marca, buf, sizeof(novo.marca));
                printf("Cod categoria: "); ler_linha(buf, sizeof(buf)); str_formatar_registro(novo.categoria_cod, buf, sizeof(novo.categoria_cod));
                printf("\nNovo produto:\n");
                imprimir_produto(&novo);
                if (confirmar("Confirmar")) {
                    if (inserir_produto(&novo)) printf("Inserido. Total: %lld\n", n_prod);
                }
                break;
            }

            case 10: {
                int id = ler_int("ID produto: ");
                if (confirmar("Confirmar remocao")) {
                    if (remover_produto(id)) printf("Removido. Total: %lld\n", n_prod);
                }
                break;
            }

            default:
                printf("Opcao invalida.\n");
        }
    }

    liberar_dados();
    return 0;
}