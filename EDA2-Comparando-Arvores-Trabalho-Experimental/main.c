#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "avl.h"
#include "rubro_negra.h"
#include "arvore_b.h"

#define AMOSTRAS 10
#define TAMANHO_MAXIMO 10000
#define INCREMENTO 100 // Pula de 100 em 100 para não gerar um CSV gigante e ilegível

// Embaralha um vetor usando o algoritmo de Fisher-Yates (garante chaves aleatórias únicas)
void embaralhar(int *vetor, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = vetor[i];
        vetor[i] = vetor[j];
        vetor[j] = temp;
    }
}

int main() {
    srand((unsigned)time(NULL));

    // Abre o arquivo CSV para salvar os resultados
    FILE *arquivo_csv = fopen("resultados_experimento.csv", "w");
    if (arquivo_csv == NULL) {
        printf("Erro ao criar o arquivo CSV.\n");
        return 1;
    }

    // Cabeçalho do CSV
    fprintf(arquivo_csv, "Tamanho,AVL_Ins,RB_Ins,B1_Ins,B5_Ins,B10_Ins,AVL_Rem,RB_Rem,B1_Rem,B5_Rem,B10_Rem\n");

    printf("Iniciando experimento...\n");

    // Varia o tamanho do conjunto de dados de 100 até 10000
    for (int n = INCREMENTO; n <= TAMANHO_MAXIMO; n += INCREMENTO) {
        
        // Acumuladores de esforço (Comparações + Rotações/Splits) para calcular a média
        long long soma_avl_ins = 0, soma_rb_ins = 0, soma_b1_ins = 0, soma_b5_ins = 0, soma_b10_ins = 0;
        long long soma_avl_rem = 0, soma_rb_rem = 0, soma_b1_rem = 0, soma_b5_rem = 0, soma_b10_rem = 0;

        int *chaves_insercao = (int*)malloc(n * sizeof(int));
        int *chaves_remocao = (int*)malloc(n * sizeof(int));

        for (int amostra = 0; amostra < AMOSTRAS; amostra++) {
            
            // Prepara os vetores com números de 1 a N e os embaralha
            for (int i = 0; i < n; i++) {
                chaves_insercao[i] = i + 1;
                chaves_remocao[i] = i + 1;
            }
            embaralhar(chaves_insercao, n);
            embaralhar(chaves_remocao, n); // Ordem de remoção diferente da inserção

            // Inicialização das estruturas de dados e estatísticas
            AVLNode *raiz_avl = NULL;
            RBNode *raiz_rb = obter_nil_rb();
            BTree *arvore_b1 = criar_arvore_b(1);
            BTree *arvore_b5 = criar_arvore_b(5);
            BTree *arvore_b10 = criar_arvore_b(10);

            AVLStats avl_ins_st = {0, 0}, avl_rem_st = {0, 0};
            RBStats rb_ins_st = {0, 0}, rb_rem_st = {0, 0};
            BStats b1_ins_st = {0, 0}, b1_rem_st = {0, 0};
            BStats b5_ins_st = {0, 0}, b5_rem_st = {0, 0};
            BStats b10_ins_st = {0, 0}, b10_rem_st = {0, 0};

            // --- FASE DE INSERÇÃO ---
            for (int i = 0; i < n; i++) {
                raiz_avl = inserir_avl(raiz_avl, chaves_insercao[i], &avl_ins_st);
                raiz_rb = inserir_rb(raiz_rb, chaves_insercao[i], &rb_ins_st);
                inserir_b(arvore_b1, chaves_insercao[i], &b1_ins_st);
                inserir_b(arvore_b5, chaves_insercao[i], &b5_ins_st);
                inserir_b(arvore_b10, chaves_insercao[i], &b10_ins_st);
            }

            // Acumula o esforço total (Comparações + Modificações Estruturais)
            soma_avl_ins += (avl_ins_st.comparacoes + avl_ins_st.rotacoes);
            soma_rb_ins += (rb_ins_st.comparacoes + rb_ins_st.rotacoes);
            soma_b1_ins += (b1_ins_st.comparacoes + b1_ins_st.rotacoes);
            soma_b5_ins += (b5_ins_st.comparacoes + b5_ins_st.rotacoes);
            soma_b10_ins += (b10_ins_st.comparacoes + b10_ins_st.rotacoes);

            // --- FASE DE REMOÇÃO ---
            for (int i = 0; i < n; i++) {
                raiz_avl = remover_avl(raiz_avl, chaves_remocao[i], &avl_rem_st);
                raiz_rb = remover_rb(raiz_rb, chaves_remocao[i], &rb_rem_st);
                remover_b(arvore_b1, chaves_remocao[i], &b1_rem_st);
                remover_b(arvore_b5, chaves_remocao[i], &b5_rem_st);
                remover_b(arvore_b10, chaves_remocao[i], &b10_rem_st);
            }

            soma_avl_rem += (avl_rem_st.comparacoes + avl_rem_st.rotacoes);
            soma_rb_rem += (rb_rem_st.comparacoes + rb_rem_st.rotacoes);
            soma_b1_rem += (b1_rem_st.comparacoes + b1_rem_st.rotacoes);
            soma_b5_rem += (b5_rem_st.comparacoes + b5_rem_st.rotacoes);
            soma_b10_rem += (b10_rem_st.comparacoes + b10_rem_st.rotacoes);

            // Limpeza de memória de segurança (a remoção total já esvazia, mas é boa prática)
            liberar_arvore_avl(raiz_avl);
            liberar_arvore_rb(raiz_rb);
            liberar_arvore_b(arvore_b1);
            liberar_arvore_b(arvore_b5);
            liberar_arvore_b(arvore_b10);
        }

        free(chaves_insercao);
        free(chaves_remocao);

        // Salva a média das 10 amostras no CSV
        fprintf(arquivo_csv, "%d,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld,%lld\n",
                n,
                soma_avl_ins / AMOSTRAS, soma_rb_ins / AMOSTRAS, soma_b1_ins / AMOSTRAS, soma_b5_ins / AMOSTRAS, soma_b10_ins / AMOSTRAS,
                soma_avl_rem / AMOSTRAS, soma_rb_rem / AMOSTRAS, soma_b1_rem / AMOSTRAS, soma_b5_rem / AMOSTRAS, soma_b10_rem / AMOSTRAS);

        if (n % 1000 == 0) {
            printf("Processado N = %d...\n", n);
        }
    }

    fclose(arquivo_csv);
    printf("Experimento concluido! Resultados salvos em 'resultados_experimento.csv'.\n");
    return 0;
}