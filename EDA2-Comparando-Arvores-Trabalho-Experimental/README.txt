Trabalho de EDA II - Estruturas de Dados Balanceadas

Conteúdo:
- Codigos-fonte (*.c, *.h)
- Script de geração de gráficos (Python)
- Resultados brutos (CSV)

Instruções:
1. Para compilar os experimentos:
   gcc main.c avl.c rubro_negra.c arvore_b.c -o experimento -O3

2. Para rodar o experimento:
   ./experimento

3. Para gerar os gráficos (requer pandas e matplotlib):
   em ambiente linux:
      sudo apt update && sudo apt install -y python3-full python3-pip
      python3 -m venv .venv
      source .venv/bin/activate
      pip install -r requirements.txt
      python3 gerar_graficos.py

   em ambiente windows:
      Instale python manualmente do site oficial (python.org). Depois continuea no cmd ou PowerShell:
      python -m venv .venv
      .venv\Scripts\activate
      pip install -r requirements.txt
      python gerar_graficos.py
      