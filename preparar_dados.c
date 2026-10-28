#include "tipos.h"
#include "preparar_dados.h"
#include "arquivos.h"
#include "consultas.h"
#include "fn_string.h"
#include "comparadores.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void montar_acesso(char* campos[], int n, Acesso* a) {
    if (n < 7) return;
    a->id = atoi(campos[0]);
    a->usuario_id = atoi(campos[1]);
    a->produto_id = atoi(campos[2]);
    a->evento_tipo = (unsigned char)atoi(campos[3]);
    a->preco = atof(campos[4]);
    str_formatar_registro(a->evento_time, campos[5], sizeof(a->evento_time));
}

void montar_produtos(char* campos[], Produto* p) {
    p->id = atoi(campos[0]);
    p->preco = atof(campos[1]);
    p->categoria_id = atoll(campos[2]);
    str_formatar_registro(p->marca, campos[3], sizeof(p->marca));
    str_formatar_registro(p->categoria_cod, campos[4], sizeof(p->categoria_cod));
}

int ler_particoes(int qtd_particoes, FILE** particoes, Produto* produtos_particao) {
    for (int i = 0; i < qtd_particoes; i++) {
        if (fread(&produtos_particao[i], sizeof(Produto), 1, particoes[i]) != 1) {
            return 0;
        }
        fgetc(particoes[i]);
    }
    return 1;
}

int menor_particao(Produto* produtos_particao, int qtd_particoes) {
    int menor_idx = -1;
    for (int i = 0; i < qtd_particoes; i++) {
        if (menor_idx == -1 || produtos_particao[i].id < produtos_particao[menor_idx].id) {
            menor_idx = i;
        }
    }
    return menor_idx;
}

long long gravar_bin_acessos() {
    FILE* f_out = abrir_arquivo(BIN_ACESSOS, "wb");
    FILE* idx_base = abrir_arquivo(IDX_BASE, "wb");
    FILE* idx_topo = abrir_arquivo(IDX_TOPO, "wb");

    char linha[TAM_LINHA];
    long long contador = 0;
    long long pos;

    FILE* f_nov = abrir_arquivo(CSV_NOV, "r");
    fgets(linha, TAM_LINHA, f_nov);

    while (fgets(linha, TAM_LINHA, f_nov)) {
        char* campos[7];
        int n = 0;
        char* token = strtok(linha, ",");
        while (token && n < 7) {
            campos[n++] = token;
            token = strtok(NULL, ",");
        }
        if (n < 7) continue;

        Acesso a;
        montar_acesso(campos, n, &a);

        pos = ftell(f_out);
        fwrite(&a, sizeof(Acesso), 1, f_out);
        fputc('\n', f_out);

        gravar_arquivo_indice(contador, a.id, pos, idx_base, idx_topo);
        contador++;
    }
    fclose(f_nov);

    FILE* f_out2 = abrir_arquivo(CSV_OUT, "r");
    fgets(linha, TAM_LINHA, f_out2);

    while (fgets(linha, TAM_LINHA, f_out2)) {
        char* campos[7];
        int n = 0;
        char* token = strtok(linha, ",");
        while (token && n < 7) {
            campos[n++] = token;
            token = strtok(NULL, ",");
        }
        if (n < 7) continue;

        Acesso a;
        montar_acesso(campos, n, &a);

        pos = ftell(f_out);
        fwrite(&a, sizeof(Acesso), 1, f_out);
        fputc('\n', f_out);

        gravar_arquivo_indice(contador, a.id, pos, idx_base, idx_topo);
        contador++;
    }
    fclose(f_out2);

    fclose(f_out);
    fclose(idx_base);
    fclose(idx_topo);

    return contador;
}

void gravar_bin_produtos() {
    FILE* f_prod = abrir_arquivo(BIN_TEMP_PROD, "wb");
    FILE* f_nov = abrir_arquivo(CSV_NOV, "r");
    char linha[TAM_LINHA];
    fgets(linha, TAM_LINHA, f_nov);

    while (fgets(linha, TAM_LINHA, f_nov)) {
        char* campos[5];
        int n = 0;
        char* token = strtok(linha, ",");
        while (token && n < 5) {
            campos[n++] = token;
            token = strtok(NULL, ",");
        }
        if (n < 5) continue;

        Produto p;
        montar_produtos(campos, &p);
        fwrite(&p, sizeof(Produto), 1, f_prod);
        fputc('\n', f_prod);
    }
    fclose(f_nov);

    FILE* f_out = abrir_arquivo(CSV_OUT, "r");
    fgets(linha, TAM_LINHA, f_out);

    while (fgets(linha, TAM_LINHA, f_out)) {
        char* campos[5];
        int n = 0;
        char* token = strtok(linha, ",");
        while (token && n < 5) {
            campos[n++] = token;
            token = strtok(NULL, ",");
        }
        if (n < 5) continue;

        Produto p;
        montar_produtos(campos, &p);
        fwrite(&p, sizeof(Produto), 1, f_prod);
        fputc('\n', f_prod);
    }
    fclose(f_out);
    fclose(f_prod);
}

int particionar_produtos() {
    long long tam_reg = sizeof(Produto) + 1;
    FILE* f = abrir_arquivo(BIN_TEMP_PROD, "rb");
    fseek(f, 0, SEEK_END);
    long long total_regs = ftell(f) / tam_reg;
    fseek(f, 0, SEEK_SET);

    int qtd_particoes = (total_regs + REGISTRO_BLOCO - 1) / REGISTRO_BLOCO;
    if (qtd_particoes == 0) qtd_particoes = 1;

    for (int i = 0; i < qtd_particoes; i++) {
        char nome[128];
        sprintf(nome, BIN_PART_PROD, i);
        FILE* part = abrir_arquivo(nome, "wb");

        long long regs_nesta = (i == qtd_particoes - 1) ? (total_regs - i * REGISTRO_BLOCO) : REGISTRO_BLOCO;
        if (regs_nesta <= 0) { fclose(part); continue; }

        Produto* buffer = malloc(regs_nesta * sizeof(Produto));
        for (long long j = 0; j < regs_nesta; j++) {
            fread(&buffer[j], sizeof(Produto), 1, f);
            fgetc(f);
        }

        qsort(buffer, regs_nesta, sizeof(Produto), comparativo_produto);

        for (long long j = 0; j < regs_nesta; j++) {
            fwrite(&buffer[j], sizeof(Produto), 1, part);
            fputc('\n', part);
        }

        free(buffer);
        fclose(part);
    }
    fclose(f);
    return qtd_particoes;
}

void intercalar_particoes(int qtd_particoes) {
    FILE** particoes = malloc(qtd_particoes * sizeof(FILE*));
    Produto* produtos_particao = malloc(qtd_particoes * sizeof(Produto));

    for (int i = 0; i < qtd_particoes; i++) {
        char nome[128];
        sprintf(nome, BIN_PART_PROD, i);
        particoes[i] = abrir_arquivo(nome, "rb");
    }

    ler_particoes(qtd_particoes, particoes, produtos_particao);

    FILE* f_prod = abrir_arquivo(BIN_PRODUTOS, "wb");
    FILE* idx_base = abrir_arquivo(IDX_BASE_PROD, "wb");
    FILE* idx_topo = abrir_arquivo(IDX_TOPO_PROD, "wb");

    long long contador = 0;
    long long pos;

    while (1) {
        int idx = menor_particao(produtos_particao, qtd_particoes);
        if (idx == -1) break;

        Produto p = produtos_particao[idx];
        int id_atual = p.id;

        pos = ftell(f_prod);
        fwrite(&p, sizeof(Produto), 1, f_prod);
        fputc('\n', f_prod);
        gravar_arquivo_indice(contador, p.id, pos, idx_base, idx_topo);
        contador++;

        while (1) {
            if (fread(&produtos_particao[idx], sizeof(Produto), 1, particoes[idx]) != 1) {
                produtos_particao[idx].id = -1;
                break;
            }
            fgetc(particoes[idx]);
            if (produtos_particao[idx].id != id_atual) break;

            pos = ftell(f_prod);
            fwrite(&produtos_particao[idx], sizeof(Produto), 1, f_prod);
            fputc('\n', f_prod);
            gravar_arquivo_indice(contador, produtos_particao[idx].id, pos, idx_base, idx_topo);
            contador++;
        }
    }

    for (int i = 0; i < qtd_particoes; i++) fclose(particoes[i]);
    free(particoes);
    free(produtos_particao);

    fclose(f_prod);
    fclose(idx_base);
    fclose(idx_topo);

    for (int i = 0; i < qtd_particoes; i++) {
        char nome[128];
        sprintf(nome, BIN_PART_PROD, i);
        remove(nome);
    }
    remove(BIN_TEMP_PROD);
}

void executar_pipeline(void) {
    printf("Gerando acessos.bin + indices...\n");
    n_acessos = gravar_bin_acessos();
    printf("Acessos: %lld\n", n_acessos);

    printf("Gerando produtos temp...\n");
    gravar_bin_produtos();

    printf("Particionando produtos...\n");
    int qtd = particionar_produtos();
    printf("Particoes: %d\n", qtd);

    printf("Intercalando particoes...\n");
    intercalar_particoes(qtd);

    printf("Pipeline concluido.\n");
}