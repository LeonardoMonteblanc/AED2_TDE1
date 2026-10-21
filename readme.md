# Algoritmos e Estruturas de Dados 2 - Arquivos de dados com índices parciais

Trabalho utilizando [dataset eCommerce behavior](https://www.kaggle.com/datasets/mkechinov/ecommerce-behavior-data-from-multi-category-store)

---
## Estrutura do Projeto

### Arquivos
```
Projeto/
├─ projetoaed.c
├─ fn_aux.c
├─ operacoes.c
├─ preparar_dados.c
```
---

### fn_aux.c
Funções auxiliares

- `formatar_registro()`: Adiciona espaços em branco para manter os registros com tamanho fixo
- `comparar_strings()`: Compara a string com espaços com a original
- `carregar_arquivo()`: Tenta abrir um arquivo, se não consegue, encerra o programa.

---
### preparar_dados.c 
Funções de tratamento e manipulação dos dados

**Tratamento de dados:**
- `converter_evento()` Converte o evento para um tipo numerico
- `remover_quebra()` Remore \r e \n do fim da linha
- `separar_campos()` Quebra linha na virgula
- `montar_acesso()` Preenche a struct de acesso com os dados
- `montar_produtos()`Preenche a struct de produtos com os dados

---
**Manipulação de dados:**
- `tratar_dados()` : Processa *.csv e gera os *.idx e *.bin
- `gerar_produtos()` Processa o *.bin temporario de produtos
  - `gerar_part_ordenada()` Cria partições ordenadas
  -  `intercalar_part_prod()` Intercala as partições criadas removendo duplicatas
- No fim apaga os arquivos temporarios e partições

---
### operacoes.c 

`carregar_idx`
`liberar_indice`
`abrir_arquivos`
`fechar_arquivos`


`busca_bin_idx`
`reconstruir_idx_prod`

**Operações**
- `prod_mostrar`
- `prod_busca_bin`
- `prod_consulta_idx`

### projetoaed.c
Main onde é chamado o fluxo principal

---

### Estrutura de pastas
```
├─ DADOS/
│  ├─ nov.csv
│  ├─ out.csv
├─ IDX/
│  ├─ base_prod.idx
│  ├─ topo_prod.idx
│  ├─ base.idx
│  ├─ topo.idx
├─ BIN/
│  ├─ acessos.bin
│  ├─ produtos.bin
├─ .gitignore
├─ README.md
```

---

## Consultar a serem realizadas

1. Quais produtos por marca e seu valor?
2. Quantos clientes únicos compraram e qual faturamento de cada um?
3. Quais categorias foram mais compradas? Qual faturamento da categoria?

