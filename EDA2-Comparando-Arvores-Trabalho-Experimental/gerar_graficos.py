import pandas as pd
import matplotlib.pyplot as plt

def gerar_graficos():
    # 1. Tenta carregar o arquivo CSV
    try:
        df = pd.read_csv('resultados_experimento.csv')
    except FileNotFoundError:
        print("Erro: O arquivo 'resultados_experimento.csv' não foi encontrado.")
        print("Certifique-se de rodar este script na mesma pasta do experimento em C.")
        return

    # O eixo X será sempre o Tamanho do conjunto de dados (N)
    x = df['Tamanho']

    print("Gerando gráfico de Adição...")
    # --- GRÁFICO 1: OPERAÇÃO DE ADIÇÃO ---
    plt.figure(figsize=(10, 6)) # Define o tamanho da janela do gráfico
    
    # Plota as 5 linhas da adição com marcadores diferentes para facilitar a visualização
    plt.plot(x, df['AVL_Ins'], label='Árvore AVL', marker='o', markersize=4)
    plt.plot(x, df['RB_Ins'], label='Árvore Rubro-Negra', marker='s', markersize=4)
    plt.plot(x, df['B1_Ins'], label='Árvore B (Ordem 1)', marker='^', markersize=4)
    plt.plot(x, df['B5_Ins'], label='Árvore B (Ordem 5)', marker='D', markersize=4)
    plt.plot(x, df['B10_Ins'], label='Árvore B (Ordem 10)', marker='v', markersize=4)

    plt.title('Esforço Computacional - Operação de Adição', fontsize=14)
    plt.xlabel('Tamanho do Conjunto de Dados (N)', fontsize=12)
    plt.ylabel('Esforço (Comparações + Modificações)', fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    plt.tight_layout()
    
    # Salva como PNG e limpa a figura
    plt.savefig('grafico_adicao.png', dpi=300)
    plt.clf()

    print("Gerando gráfico de Remoção...")
    # --- GRÁFICO 2: OPERAÇÃO DE REMOÇÃO ---
    plt.figure(figsize=(10, 6))
    
    # Plota as 5 linhas da remoção
    plt.plot(x, df['AVL_Rem'], label='Árvore AVL', marker='o', markersize=4)
    plt.plot(x, df['RB_Rem'], label='Árvore Rubro-Negra', marker='s', markersize=4)
    plt.plot(x, df['B1_Rem'], label='Árvore B (Ordem 1)', marker='^', markersize=4)
    plt.plot(x, df['B5_Rem'], label='Árvore B (Ordem 5)', marker='D', markersize=4)
    plt.plot(x, df['B10_Rem'], label='Árvore B (Ordem 10)', marker='v', markersize=4)

    plt.title('Esforço Computacional - Operação de Remoção', fontsize=14)
    plt.xlabel('Tamanho do Conjunto de Dados (N)', fontsize=12)
    plt.ylabel('Esforço (Comparações + Modificações)', fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend()
    plt.tight_layout()
    
    # Salva como PNG
    plt.savefig('grafico_remocao.png', dpi=300)
    
    print("Sucesso! Gráficos salvos como 'grafico_adicao.png' e 'grafico_remocao.png'.")

if __name__ == "__main__":
    gerar_graficos()