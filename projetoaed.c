#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ftell _ftelli64
#define fseek _fseeki64


#define TAM_LINHA 1024
#define REGISTRO_BLOCO 10000000

/*
INDICE BASE => 1 ENTRADA A CADA 100 REGISTROS
INDICE TOPO => 1 ENTRADA A CADA 10 REGISTROS DE INDICE BASE
*/
#define ACESSO_BASE 100
#define ACESSO_TOPO 1000
#define PROD_BASE 100
#define PROD_TOPO 1000

struct prod {
  int prod_id;      // 4 bytes
  double preco;     // 8 bytes
  long long cat_id; // 8 bytes
  char marca[30];   // 30 bytes
  char cat_cod[50]; // 50 bytes
}; // 100 bytes + 4 bytes de padding ==> 104 bytes
typedef struct prod Produto;

struct acesso {
  int acesso_id;              // 4 bytes
  int usuario_id;             // 4 bytes
  int prod_id;                // 4 bytes
  unsigned char evento_tipo;  // 1 byte
  double preco;               // 8 bytes
  char evento_time[24];       // 24 bytes
}; // 45 bytes + 3 bytes de padding ==> 48 bytes
typedef struct acesso Acesso;

struct entrada_indice {
  int chave;  //4 bytes
  long long pos;   //8 bytes
}; // 12 bytes + 4 bytes de padding ==> 16 bytes
typedef struct entrada_indice EntradaIdx;

/*=========================== FUNÇÕES AUXILIARES ==============================*/

/* =============== FORMATAR_REGISTRO ===============
* Formata os registros para ter tam fixo adicionando os espaços */
void formatar_registro(char *formatada, char *original, int tam) {
  int i = 0;

  while(i < tam -1 && original[i] !='\0') {
    formatada[i] = original[i];
    i++;
  }

  while(i < tam-1) {
    formatada[i] = ' ';
    i++;
  }

  formatada[tam-1] = '\0';
}

/* =============== COMPARAR_STRINGS ===============
* Compara as strings originais com as strings formatadas com espaços */
int comparar_strings(char *formatada, char *original) {
  size_t tam = strlen(original);
  size_t pos;

  if(strncmp(formatada, original, tam) != 0) {
    return 0;
  }

  pos = tam + strspn(formatada + tam, " ");
  if(formatada[pos] == '\0') {
    return 1;
  }

  return 0;

}
/* =============== CONVERTER_EVENTO ===============
* Padroniza os eventos para int
* 0 => "view"
* 1 => "cart"
* 2 => "purchase" 
* -1 => outros/desconhecido*/
unsigned char converter_evento(char *texto) {
  if(strcmp(texto, "view") == 0) {
    return 0;
  } else if(strcmp(texto, "cart") == 0) {
    return 1;
  } else if(strcmp(texto, "purchase") == 0) {
    return 2;
  }
  return -1;

}

/* =============== REMOVER_QUEBRA_LINHA ===============
* Compara as strings originais com as strings formatadas com espaços */
void remover_quebra(char *linha) {
  int i = 0;

  while(linha[i] != '\0') {
    if(linha[i] == '\r' || linha[i] == '\n') {
      linha[i] = '\0';
      return;
    }
    i++;
  }
}

/* =============== SEPARAR_POR_VIRGULA ===============
* Separa a string por virgulas */
int separar_campos(char *linha, char *campos[], int max) {
  int quant = 1;
  int i = 0;

  campos[0] = linha;

  while(linha[i] != '\0' && quant < max) {
    if(linha[i] == ',') {
      linha[i] = '\0';
      campos[quant] = &linha[i+1];
      quant++;
    }
    i++;
  }

  return quant;
}

/* =============== MONTAR_REGISTRO_ACESSO ===============
* Preenche a struct usando os dados do csv 
 * Ordem dos campos no CSV:
 * [0] event_time     [4] category_code
 * [1] event_type     [5] brand
 * [2] product_id     [6] price
 * [3] category_id    [7] user_id   
 *                    [8] user_session
*/
void montar_acesso(char *campos[], int n, Acesso *a) {
  a->acesso_id = n;
  a->usuario_id = atoi(campos[7]);
  a->prod_id = atoi(campos[2]);
  a->evento_tipo = converter_evento(campos[1]);
  a->preco = atof(campos[6]);
  formatar_registro(a->evento_time, campos[0], 24);
}
/* =============== MONTAR_REGISTRO_PRODUTOS ===============
* Preenche a struct usando os dados do csv */
void montar_produtos(char *campos[], Produto *p) {
  p->prod_id = atoi(campos[2]);
  p->cat_id = atoll(campos[3]);
  p->preco = atof(campos[6]);
  formatar_registro(p->marca, campos[5],30);
  formatar_registro(p->cat_cod, campos[4], 50);
}

/* =============== PROCESSAR_CSV ===============
* Acessos: 1 acesso por linha
* Acessos_base: 1 EntradaIdx a cada 100 acessos
* Acessos_topo: 1 EntradaIdx a cada 1000 acessos
* Temp_prod: 1 prod por linha
*/
void processar_csv(char *arq, FILE *acessos, FILE *base, FILE *topo, FILE *temp_prod, int *cont) {
  time_t inicio;
  time_t fim;
  double tempott;

  FILE *csv;
  char linha[TAM_LINHA];

  int ignoradas= 0;

  csv = fopen(arq, "r");
  if(csv == NULL) {
    printf("Nao foi possivel abrir o arquivo!");
    return;
  }

  if(fgets(linha, TAM_LINHA, csv) == NULL) {
    printf("Arquivo somente com cabeçalho ou vazio");
    fclose(csv);
    return;
  }
  inicio = time(NULL);

  while(fgets(linha, TAM_LINHA, csv) != NULL) {
    char *campos[9];
    int quant;
    Acesso a;
    Produto prod;
    long long pos;

    remover_quebra(linha);
    quant = separar_campos(linha, campos, 9);

    if(quant <9) {
      ignoradas++;
      continue;
    }

    montar_acesso(campos, *cont + 1, &a);
    pos = ftell(acessos);
    
    fwrite(&a, sizeof(Acesso),1,acessos);
    fputc('\n', acessos);

    if(*cont % ACESSO_BASE == 0) {
      EntradaIdx e;
      e.chave = a.acesso_id;
      e.pos = pos;
      fwrite(&e, sizeof(EntradaIdx), 1, base);
    }

    if(*cont % ACESSO_TOPO == 0) {
      EntradaIdx e;
      e.chave = a.acesso_id;
      e.pos = pos;
      fwrite(&e, sizeof(EntradaIdx), 1, topo);
    }

    montar_produtos(campos, &prod);
    fwrite(&prod, sizeof(Produto), 1, temp_prod);
    (*cont)++;
  }

  fclose(csv);
  fim = time(NULL);

  tempott = difftime(fim, inicio);
  printf("tempo: %.0f s (%.1f min)\n", tempott, tempott / 60.0);

  if(ignoradas >0) {
    printf("%s: %d linhas ignoradas\n", arq, ignoradas);
  }
}
/* =============== GERAR_BIN ===============
* Processa OUT.CSV e NOV.CSV + acessos.bin usando processar_csv
*/
void gerar_bin() {
  FILE *acessos;
  FILE *base;
  FILE *topo;
  FILE *temp_prod;
  int cont;

  time_t inicio;
  time_t fim;
  double tempott;


  acessos = fopen("acessos.bin", "wb");
  if(acessos == NULL) {
    printf("Nao foi possivel criar/abrir arquivo acessos.bin\n");
    return;
  }

  base = fopen("base.idx", "wb");
  if(base == NULL) {
    printf("Nao foi possivel criar/abrir arquivo base.idx\n");
    fclose(acessos);
    return;
    
  }

  topo = fopen("topo.idx", "wb");
  if(topo == NULL) {
    printf("Nao foi possivel criar/abrir arquivo topo.idx\n");
    fclose(acessos);
    fclose(base);
    return;
  }

  temp_prod = fopen("temp_prod.bin", "wb");
  if(temp_prod == NULL) {
    printf("Nao foi possivel criar/abrir arquivo temp_prod.bin\n");
    fclose(acessos);
    fclose(base);
    fclose(topo);
    return;
  }

  cont = 0;
  inicio = time(NULL);

  printf("Processando OUT.CSV...\n");
  processar_csv("DADOS\\out.csv", acessos, base, topo, temp_prod, &cont);
  
  printf("Processando NOV.CSV...\n");
  processar_csv("DADOS\\nov.csv", acessos,base, topo, temp_prod, &cont);

  fclose(acessos);
  fclose(base);
  fclose(topo);
  fclose(temp_prod);
  fim = time(NULL);
  tempott = difftime(fim, inicio);
  
  printf("tempo: %.0f s (%.1f min)\n", tempott, tempott / 60.0);
  printf("Registrados gravados: %d\n", cont);

}


int main(void) {

  printf("Geracao de binariso + indices + temp_prod.bin\n");
  gerar_bin();
  return 0;
}