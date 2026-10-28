<div style="font-family: 'SF Mono', 'Consolas', monospace; max-width: 1150px;">

  <h1 style="color: #212529; border-bottom: 2px solid #bf2c3b; padding-bottom: 10px; font-size: 24px; margin-bottom: 10px;">
    Algoritmos e Estruturas de Dados 2 <br>
    <span style="font-size: 18px; font-weight: normal; color: #6f42c1;">Arquivos de dados com índices parciais</span>
  </h1>

  <p style="color: #212529; font-size: 14px; margin-bottom: 30px;">
    Trabalho utilizando <a href="https://www.kaggle.com/datasets/mkechinov/ecommerce-behavior-data-from-multi-category-store" style="color: #bf2c3b; text-decoration: none; font-weight: bold; border-bottom: 1px dashed #bf2c3b;">dataset eCommerce behavior</a>
  </p>

  <h2 style="color: #212529; border-bottom: 1px solid #dee2e6; padding-bottom: 8px; font-size: 18px; margin-top: 30px; margin-bottom: 15px;">Compilar</h2>
  <p style="color: #212529; font-size: 14px;">
    <code style="background-color: transparent; color: #bf2c3b; border: 1px solid #dee2e6; padding: 2px 6px; border-radius: 4px; font-weight: bold;">make run</code> ===> compila e executa
  </p>

  <h2 style="color: #212529; border-bottom: 1px solid #dee2e6; padding-bottom: 8px; font-size: 18px; margin-top: 30px; margin-bottom: 15px;">Menu interativo</h2>
  <div style="color: #212529; border: 1px solid #dee2e6; border-radius: 6px; padding: 16px; line-height: 1.6; background-color: transparent; font-size: 13px;">
    <pre style="margin: 0; padding: 0; border: none; font-family: inherit; white-space: pre; background-color: transparent;">
<span style="color: #bf2c3b; font-weight: bold;">[1]</span>  Preparar dados
<span style="color: #bf2c3b; font-weight: bold;">[2]</span>  Produtos por marca
<span style="color: #bf2c3b; font-weight: bold;">[3]</span>  Compradores e faturamento
<span style="color: #bf2c3b; font-weight: bold;">[4]</span>  Faturamento por categoria
<span style="color: #bf2c3b; font-weight: bold;">[5]</span>  Buscar produto por ID
<span style="color: #bf2c3b; font-weight: bold;">[6]</span>  Buscar acesso por ID
<span style="color: #bf2c3b; font-weight: bold;">[7]</span>  Listar produtos 
<span style="color: #bf2c3b; font-weight: bold;">[8]</span>  Listar acessos 
<span style="color: #bf2c3b; font-weight: bold;">[9]</span>  Inserir produto
<span style="color: #bf2c3b; font-weight: bold;">[10]</span> Remover produto
<span style="color: #bf2c3b; font-weight: bold;">[0]</span>  Sair
</pre>
  </div>

  <h2 style="color: #212529; border-bottom: 1px solid #dee2e6; padding-bottom: 8px; font-size: 18px; margin-top: 30px; margin-bottom: 15px;">Consultas realizadas</h2>
  <ol style="color: #212529; font-size: 14px; line-height: 1.6; padding-left: 20px; margin-bottom: 30px;">
    <li style="margin-bottom: 5px;">Quais produtos por marca e seu valor?</li>
    <li style="margin-bottom: 5px;">Quantos clientes únicos compraram e qual faturamento de cada um?</li>
    <li style="margin-bottom: 5px;">Quais categorias foram mais compradas? Qual faturamento da categoria?</li>
  </ol>

  <h2 style="color: #212529; border-bottom: 1px solid #dee2e6; padding-bottom: 8px; font-size: 18px; margin-top: 30px; margin-bottom: 15px;">Estrutura do Projeto</h2>
  <h3 style="color: #6f42c1; font-size: 16px; margin-bottom: 15px;">Arquivos - Código</h3>

  <table style="width: 100%; border-collapse: collapse; border: none;">
    <tr>
      <td style="width: 32%; vertical-align: top; padding-right: 20px; border: none;">
        <div style="font-size: 13px; color: #212529; border: 1px solid #dee2e6; border-radius: 6px; padding: 16px; line-height: 1.7; background-color: transparent;">
          <pre style="margin: 0; padding: 0; border: none; font-family: inherit; white-space: pre; background-color: transparent;">
<span style="color: #bf2c3b; font-weight: bold;">📦 PROJETO - AED II</span>
├── <span style="color: #bf2c3b; font-weight: bold;">📁 DADOS/</span>
│   ├── <span style="color: #198754;">📊 nov.csv</span>
│   └── <span style="color: #198754;">📊 out.csv</span>
├── <span style="color: #bf2c3b; font-weight: bold;">📁 IDX/</span>
│   ├── <span style="color: #6f42c1;">📑 base_prod.idx</span>
│   ├── <span style="color: #6f42c1;">📑 topo_prod.idx</span>
│   ├── <span style="color: #6f42c1;">📑 base.idx</span>
│   └── <span style="color: #6f42c1;">📑 topo.idx</span>
└── <span style="color: #bf2c3b; font-weight: bold;">📁 BIN/</span>
    ├── <span style="color: #dc3545;">💾 acessos.bin</span>
    └── <span style="color: #dc3545;">💾 produtos.bin</span> </pre>
        </div>
      </td>
      <td style="width: 68%; vertical-align: top; border: none;">
        <table style="width: 100%; border-collapse: separate; border-spacing: 8px; margin: -8px;">
          <tr>
            <td style="width: 33.33%; padding: 0; vertical-align: top;">
              <div style="border: 1px solid #dee2e6; border-left: 6px solid #bf2c3b; border-radius: 8px; padding: 12px 14px; color: #212529; background-color: transparent; height: 100%; min-height: 85px; box-sizing: border-box;">
                <div style="color: #bf2c3b; font-weight: 700; margin-bottom: 6px; font-size: 13px;">main.c</div>
                <div style="color: #212529; font-size: 12px; line-height: 1.4;">Fluxo principal</div>
              </div>
            </td>
            <td style="width: 33.33%; padding: 0; vertical-align: top;">
              <div style="border: 1px solid #dee2e6; border-left: 6px solid #bf2c3b; border-radius: 8px; padding: 12px 14px; color: #212529; background-color: transparent; height: 100%; min-height: 85px; box-sizing: border-box;">
                <div style="color: #bf2c3b; font-weight: 700; margin-bottom: 6px; font-size: 13px;">arquivos.c</div>
                <div style="color: #212529; font-size: 12px; line-height: 1.4;">Carregar índices, carregar/liberar dados</div>
              </div>
            </td>
            <td style="width: 33.33%; padding: 0; vertical-align: top;">
              <div style="border: 1px solid #dee2e6; border-left: 6px solid #bf2c3b; border-radius: 8px; padding: 12px 14px; color: #212529; background-color: transparent; height: 100%; min-height: 85px; box-sizing: border-box;">
                <div style="color: #bf2c3b; font-weight: 700; margin-bottom: 6px; font-size: 13px;"> preparar_dados.c</div>
                <div style="color: #212529; font-size: 12px; line-height: 1.4;">CSV → BIN: acessos, partições, intercalação</div>
              </div>
            </td>
          </tr>
          <tr>
            <td style="width: 33.33%; padding: 0; vertical-align: top;">
              <div style="border: 1px solid #dee2e6; border-left: 6px solid #bf2c3b; border-radius: 8px; padding: 12px 14px; color: #212529; background-color: transparent; height: 100%; min-height: 85px; box-sizing: border-box;">
                <div style="color: #bf2c3b; font-weight: 700; margin-bottom: 6px; font-size: 13px;">consultas.c</div>
                <div style="color: #212529; font-size: 12px; line-height: 1.4;">Buscar, listagens e consultas</div>
              </div>
            </td>
            <td style="width: 33.33%; padding: 0; vertical-align: top;">
              <div style="border: 1px solid #dee2e6; border-left: 6px solid #bf2c3b; border-radius: 8px; padding: 12px 14px; color: #212529; background-color: transparent; height: 100%; min-height: 85px; box-sizing: border-box;">
                <div style="color: #bf2c3b; font-weight: 700; margin-bottom: 6px; font-size: 13px;">comparadores.c</div>
                <div style="color: #212529; font-size: 12px; line-height: 1.4;">Comparadores para qsort</div>
              </div>
            </td>
            <td style="width: 33.33%; padding: 0; vertical-align: top;">
              <div style="border: 1px solid #dee2e6; border-left: 6px solid #6f42c1; border-radius: 8px; padding: 12px 14px; color: #212529; background-color: transparent; height: 100%; min-height: 85px; box-sizing: border-box;">
                <div style="color: #6f42c1; font-weight: 700; margin-bottom: 6px; font-size: 13px;">tipos.h</div>
                <div style="color: #212529; font-size: 12px; line-height: 1.4;">Structs e constantes</div>
              </div>
            </td>
          </tr>
          <tr>
            <td style="width: 33.33%; padding: 0; vertical-align: top;">
              <div style="border: 1px solid #dee2e6; border-left: 6px solid #bf2c3b; border-radius: 8px; padding: 12px 14px; color: #212529; background-color: transparent; height: 100%; min-height: 85px; box-sizing: border-box;">
                <div style="color: #bf2c3b; font-weight: 700; margin-bottom: 6px; font-size: 13px;">fn_string.c</div>
                <div style="color: #212529; font-size: 12px; line-height: 1.4;">Funções auxiliares para strings</div>
              </div>
            </td>
            <td style="width: 33.33%; padding: 0; vertical-align: top;">
              <div style="border: 1px solid #dee2e6; border-left: 6px solid #6f42c1; border-radius: 8px; padding: 12px 14px; color: #212529; background-color: transparent; height: 100%; min-height: 85px; box-sizing: border-box;">
                <div style="color: #6f42c1; font-weight: 700; margin-bottom: 6px; font-size: 13px;">Makefile</div>
                <div style="color: #212529; font-size: 12px; line-height: 1.4;">Build do projeto</div>
              </div>
            </td>
          </tr>
        </table>
      </td>
    </tr>
  </table>
</div>