#ifndef ARVORE_B_H
#define ARVORE_B_H

// Estrutura para contabilizar o esforço computacional
typedef struct {
    long long comparacoes;
    long long rotacoes; // Representará splits, merges e transferências
} BStats;

// Estrutura do nó da Árvore B
typedef struct BNode {
    int *chaves;         // Array dinâmico de chaves
    struct BNode **filhos; // Array dinâmico de ponteiros para os filhos
    int num_chaves;      // Quantidade atual de chaves
    int folha;           // Booleano: 1 se for folha, 0 caso contrário
} BNode;

// Estrutura da Árvore B (guarda a raiz e a ordem definida)
typedef struct BTree {
    BNode *raiz;
    int ordem;           // Ordem m
    int max_chaves;      // 2 * m
    int min_chaves;      // m
} BTree;

// Inicializa uma Árvore B com a ordem desejada (1, 5 ou 10)
BTree* criar_arvore_b(int ordem);

// Insere uma chave na Árvore B e atualiza as estatísticas
void inserir_b(BTree* arvore, int chave, BStats* stats);

// Remove uma chave da Árvore B e atualiza as estatísticas
void remover_b(BTree* arvore, int chave, BStats* stats);

// Libera a memória alocada
void liberar_arvore_b(BTree* arvore);

#endif // ARVORE_B_H