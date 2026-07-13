#include <stdio.h>
#include <stdlib.h>
#include "avl.h"

static int max(int a, int b) {
    return (a > b) ? a : b;
}

int obter_altura(AVLNode* no) {
    if (no == NULL) return 0;
    return no->altura;
}

int obter_fator_balanceamento(AVLNode* no) {
    if (no == NULL) return 0;
    return obter_altura(no->esquerdo) - obter_altura(no->direito);
}

AVLNode* criar_no_avl(int chave) {
    AVLNode* no = (AVLNode*)malloc(sizeof(AVLNode));
    if (no != NULL) {
        no->chave = chave;
        no->esquerdo = NULL;
        no->direito = NULL;
        no->altura = 1; // Novo nó é inicialmente adicionado como folha
    }
    return no;
}

// Rotação à direita
static AVLNode* rotacao_direita(AVLNode* y, AVLStats* stats) {
    stats->rotacoes++;
    AVLNode* x = y->esquerdo;
    AVLNode* T2 = x->direito;

    x->direito = y;
    y->esquerdo = T2;

    // Atualiza as alturas
    y->altura = max(obter_altura(y->esquerdo), obter_altura(y->direito)) + 1;
    x->altura = max(obter_altura(x->esquerdo), obter_altura(x->direito)) + 1;

    return x; // Nova raiz
}

// Rotação à esquerda
static AVLNode* rotacao_esquerda(AVLNode* x, AVLStats* stats) {
    stats->rotacoes++;
    AVLNode* y = x->direito;
    AVLNode* T2 = y->esquerdo;

    y->esquerdo = x;
    x->direito = T2;

    // Atualiza as alturas
    x->altura = max(obter_altura(x->esquerdo), obter_altura(x->direito)) + 1;
    y->altura = max(obter_altura(y->esquerdo), obter_altura(y->direito)) + 1;

    return y; // Nova raiz
}

AVLNode* inserir_avl(AVLNode* no, int chave, AVLStats* stats) {
    // 1. Executa a inserção padrão de uma BST
    if (no == NULL) {
        return criar_no_avl(chave);
    }

    stats->comparacoes++;
    if (chave < no->chave) {
        no->esquerdo = inserir_avl(no->esquerdo, chave, stats);
    } else if (chave > no->chave) {
        no->direito = inserir_avl(no->direito, chave, stats);
    } else {
        return no; // Chaves duplicadas não são permitidas na árvore AVL do teste
    }

    // 2. Atualiza a altura do nó pai
    no->altura = 1 + max(obter_altura(no->esquerdo), obter_altura(no->direito));

    // 3. Obtém o fator de balanceamento para verificar se o nó desbalanceou
    int fb = obter_fator_balanceamento(no);

    // Se o nó desbalancear, existem 4 casos:

    // Caso Esquerda-Esquerda (Símples Direita)
    if (fb > 1 && chave < no->esquerdo->chave) {
        stats->comparacoes++;
        return rotacao_direita(no, stats);
    }

    // Caso Direita-Direita (Simples Esquerda)
    if (fb < -1 && chave > no->direito->chave) {
        stats->comparacoes++;
        return  rotacao_esquerda(no, stats);
    }

    // Caso Esquerda-Direita (Dupla Direita)
    if (fb > 1 && chave > no->esquerdo->chave) {
        stats->comparacoes++;
        no->esquerdo = rotacao_esquerda(no->esquerdo, stats);
        return  rotacao_direita(no, stats);
    }

    // Caso Direita-Esquerda (Dupla Esquerda)
    if (fb < -1 && chave < no->direito->chave) {
        stats->comparacoes++;
        no->direito =  rotacao_direita(no->direito, stats);
        return  rotacao_esquerda(no, stats);
    }

    return no;
}

// Encontra o nó com o menor valor (usado na remoção)
static AVLNode* no_com_valor_minimo(AVLNode* no) {
    AVLNode* atual = no;
    while (atual->esquerdo != NULL) {
        atual = atual->esquerdo;
    }
    return atual;
}

AVLNode* remover_avl(AVLNode* raiz, int chave, AVLStats* stats) {
    // 1. Executa a remoção padrão de BST
    if (raiz == NULL) return raiz;

    stats->comparacoes++;
    if (chave < raiz->chave) {
        raiz->esquerdo = remover_avl(raiz->esquerdo, chave, stats);
    } else if (chave > raiz->chave) {
        raiz->direito = remover_avl(raiz->direito, chave, stats);
    } else {
        // Nó encontrado! Tratamento dos casos de remoção:
        if ((raiz->esquerdo == NULL) || (raiz->direito == NULL)) {
            AVLNode* temp = raiz->esquerdo ? raiz->esquerdo : raiz->direito;

            // Caso sem filhos
            if (temp == NULL) {
                temp = raiz;
                raiz = NULL;
            } else { // Caso com um filho
                *raiz = *temp; // Copia o conteúdo do filho não nulo
            }
            free(temp);
        } else {
            // Caso com dois filhos: obtém o sucessor em ordem (menor da subárvore direita)
            AVLNode* temp = no_com_valor_minimo(raiz->direito);

            raiz->chave = temp->chave; // Copia a chave do sucessor
            // Remove o sucessor em ordem
            raiz->direito = remover_avl(raiz->direito, temp->chave, stats);
        }
    }

    // Se a árvore tinha apenas um nó, retorna
    if (raiz == NULL) return raiz;

    // 2. Atualiza a altura do nó atual
    raiz->altura = 1 + max(obter_altura(raiz->esquerdo), obter_altura(raiz->direito));

    // 3. Obtém o fator de balanceamento
    int fb = obter_fator_balanceamento(raiz);

    // Se o nó desbalancear, trata os 4 casos:

    // Caso Esquerda-Esquerda
    if (fb > 1 && obter_fator_balanceamento(raiz->esquerdo) >= 0) {
        return  rotacao_direita(raiz, stats);
    }

    // Caso Esquerda-Direita
    if (fb > 1 && obter_fator_balanceamento(raiz->esquerdo) < 0) {
        raiz->esquerdo =  rotacao_esquerda(raiz->esquerdo, stats);
        return  rotacao_direita(raiz, stats);
    }

    // Caso Direita-Direita
    if (fb < -1 && obter_fator_balanceamento(raiz->direito) <= 0) {
        return  rotacao_esquerda(raiz, stats);
    }

    // Caso Direita-Esquerda
    if (fb < -1 && obter_fator_balanceamento(raiz->direito) > 0) {
        raiz->direito =  rotacao_direita(raiz->direito, stats);
        return  rotacao_esquerda(raiz, stats);
    }

    return raiz;
}

void liberar_arvore_avl(AVLNode* raiz) {
    if (raiz != NULL) {
        liberar_arvore_avl(raiz->esquerdo);
        liberar_arvore_avl(raiz->direito);
        free(raiz);
    }
}