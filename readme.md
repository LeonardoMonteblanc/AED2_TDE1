# Algoritmos e Estruturas de Dados 2 - Arquivos de dados com índices parciais

Trabalho utilizando [dataset eCommerce behavior](https://www.kaggle.com/datasets/mkechinov/ecommerce-behavior-data-from-multi-category-store)

---
## Consultar a serem realizadas

1. Quais produtos por marca e seu valor?
2. Quantos clientes únicos compraram e qual faturamento de cada um?
3. Quais categorias foram mais compradas? Qual faturamento da categoria?

---
## Estrutura do Projeto

### Arquivos - Código
```
Projeto/
├─ main.c
├─ preparar_dados.c
├─ consultas.c
├─ comparadores.c
├─ fn_string.c
├─ arquivos.c
├─ tipos.h
└─ README.md
```

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