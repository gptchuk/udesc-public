#ifndef RUBRO_NEGRA_H
#define RUBRO_NEGRA_H

// Definição das cores dos nós
typedef enum { VERMELHO, PRETO } Cor;

// Estrutura para contabilizar o esforço computacional
typedef struct {
    long long comparacoes;
    long long rotacoes;
} RBStats;

// Estrutura do nó da Árvore Rubro-Negra
typedef struct RBNode {
    int chave;
    Cor cor;
    struct RBNode *esquerdo;
    struct RBNode *direito;
    struct RBNode *pai;
} RBNode;

// Retorna o ponteiro para o nullNode
RBNode* obter_nullNode(); 

// Insere uma chave na árvore Rubro-Negra e atualiza as estatísticas
RBNode* inserir_rb(RBNode* raiz, int chave, RBStats* stats);

// Remove uma chave da árvore Rubro-Negra e atualiza as estatísticas
RBNode* remover_rb(RBNode* raiz, int chave, RBStats* stats);

// Libera toda a memória alocada para a árvore de forma segura
void liberar_arvore_rb(RBNode* raiz);

#endif // RUBRO_NEGRA_H