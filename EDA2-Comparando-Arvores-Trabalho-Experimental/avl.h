#ifndef AVL_H
#define AVL_H

// Estrutura para contabilizar o esforço computacional
typedef struct {
    long long comparacoes;
    long long rotacoes;
} AVLStats;

// Estrutura do nó da Árvore AVL
typedef struct AVLNode {
    int chave;
    int altura;
    struct AVLNode *esquerdo;
    struct AVLNode *direito;
} AVLNode;

// Cria um novo nó com a chave fornecida
AVLNode* criar_no_avl(int chave);

// Insere uma chave na árvore AVL e atualiza as estatísticas
AVLNode* inserir_avl(AVLNode* no, int chave, AVLStats* stats);

// Remove uma chave da árvore AVL e atualiza as estatísticas
AVLNode* remover_avl(AVLNode* no, int chave, AVLStats* stats);

// Libera toda a memória alocada para a árvore
void liberar_arvore_avl(AVLNode* raiz);

// Funções auxiliares para obter altura e fator de balanceamento
int obter_altura(AVLNode* no);
int obter_fator_balanceamento(AVLNode* no);

#endif // AVL_H