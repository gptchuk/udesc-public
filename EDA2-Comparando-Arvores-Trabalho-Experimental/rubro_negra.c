#include <stdio.h>
#include <stdlib.h>
#include "rubro_negra.h"

// Definição estática do nó sentinela global e imutável
static RBNode nullNode = {0, PRETO, &nullNode, &nullNode, &nullNode};
#define NULL_NODE (&nullNode)

RBNode* obter_nullNode() {
    return NULL_NODE;
}

// Inicializa um novo nó vermelho com ponteiros apontando para NULL_NODE
static RBNode* criar_no_rb(int chave) {
    RBNode* no = (RBNode*)malloc(sizeof(RBNode));
    if (no != NULL) {
        no->chave = chave;
        no->cor = VERMELHO;
        no->esquerdo = NULL_NODE;
        no->direito = NULL_NODE;
        no->pai = NULL_NODE;
    }
    return no;
}

// Rotação para a esquerda adaptada para a estrutura rubro-negra
static RBNode* rotacao_esquerda(RBNode* raiz, RBNode* x, RBStats* stats) {
    stats->rotacoes++;
    RBNode* y = x->direito;
    x->direito = y->esquerdo;
    
    if (y->esquerdo != NULL_NODE) {
        y->esquerdo->pai = x;
    }
    
    y->pai = x->pai;
    
    if (x->pai == NULL_NODE) {
        raiz = y;
    } else if (x == x->pai->esquerdo) {
        x->pai->esquerdo = y;
    } else {
        x->pai->direito = y;
    }
    
    y->esquerdo = x;
    x->pai = y;
    
    return raiz;
}

// Rotação para a direita adaptada para a estrutura rubro-negra
static RBNode* rotacao_direita(RBNode* raiz, RBNode* x, RBStats* stats) {
    stats->rotacoes++;
    RBNode* y = x->esquerdo;
    x->esquerdo = y->direito;
    
    if (y->direito != NULL_NODE) {
        y->direito->pai = x;
    }
    
    y->pai = x->pai;
    
    if (x->pai == NULL_NODE) {
        raiz = y;
    } else if (x == x->pai->direito) {
        x->pai->direito = y;
    } else {
        x->pai->esquerdo = y;
    }
    
    y->direito = x;
    x->pai = y;
    
    return raiz;
}

// Corrige as propriedades da árvore após uma inserção
static RBNode* ajustar_insercao(RBNode* raiz, RBNode* z, RBStats* stats) {
    while (z->pai->cor == VERMELHO) {
        if (z->pai == z->pai->pai->esquerdo) {
            RBNode* y = z->pai->pai->direito; // Tio de z
            
            // Caso 1: O tio é vermelho -> Recoloração
            if (y->cor == VERMELHO) {
                z->pai->cor = PRETO;
                y->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                z = z->pai->pai;
            } else {
                // Caso 2: O tio é preto e z é filho à direita -> Rotação Esquerda
                if (z == z->pai->direito) {
                    z = z->pai;
                    raiz = rotacao_esquerda(raiz, z, stats);
                }
                // Caso 3: O tio é preto e z é filho à esquerda -> Rotação Direita
                z->pai->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                raiz = rotacao_direita(raiz, z->pai->pai, stats);
            }
        } else { // Casos simétricos quando o pai é filho à direita
            RBNode* y = z->pai->pai->esquerdo;
            
            if (y->cor == VERMELHO) {
                z->pai->cor = PRETO;
                y->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                z = z->pai->pai;
            } else {
                if (z == z->pai->esquerdo) {
                    z = z->pai;
                    raiz = rotacao_direita(raiz, z, stats);
                }
                z->pai->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                raiz = rotacao_esquerda(raiz, z->pai->pai, stats);
            }
        }
    }
    raiz->cor = PRETO; // A raiz deve ser sempre preta
    return raiz;
}

RBNode* inserir_rb(RBNode* raiz, int chave, RBStats* stats) {
    RBNode* z = criar_no_rb(chave);
    if (z == NULL) return raiz;
    
    RBNode* y = NULL_NODE;
    RBNode* x = (raiz == NULL) ? NULL_NODE : raiz;
    
    // Inserção padrão de BST
    while (x != NULL_NODE) {
        y = x;
        stats->comparacoes++;
        if (z->chave < x->chave) {
            x = x->esquerdo;
        } else if (z->chave > x->chave) {
            x = x->direito;
        } else {
            free(z); // Não permite duplicados no cenário de teste
            return (raiz == NULL) ? NULL_NODE : raiz;
        }
    }
    
    z->pai = y;
    if (y == NULL_NODE) {
        raiz = z;
    } else {
        if (z->chave < y->chave) {
            y->esquerdo = z;
        } else {
            y->direito = z;
        }
    }
    
    if (z->pai == NULL_NODE) {
        z->cor = PRETO;
        return z;
    }
    
    if (z->pai->pai == NULL_NODE) {
        return raiz;
    }
    
    return ajustar_insercao(raiz, z, stats);
}

// Auxiliar para substituir subárvores durante a remoção
static void transplante_rb(RBNode** raiz, RBNode* u, RBNode* v) {
    if (u->pai == NULL_NODE) {
        *raiz = v;
    } else if (u == u->pai->esquerdo) {
        u->pai->esquerdo = v;
    } else {
        u->pai->direito = v;
    }
    v->pai = u->pai;
}

static RBNode* no_minimo(RBNode* no) {
    while (no->esquerdo != NULL_NODE) {
        no = no->esquerdo;
    }
    return no;
}

// Corrige as propriedades da árvore após uma remoção
static RBNode* ajustar_remocao(RBNode* raiz, RBNode* x, RBStats* stats) {
    while (x != raiz && x->cor == PRETO) {
        if (x == x->pai->esquerdo) {
            RBNode* w = x->pai->direito; // Irmão de x
            
            // Caso 1: O irmão w é vermelho
            if (w->cor == VERMELHO) {
                w->cor = PRETO;
                x->pai->cor = VERMELHO;
                raiz = rotacao_esquerda(raiz, x->pai, stats);
                w = x->pai->direito;
            }
            
            // Caso 2: Ambos os filhos de w são pretos
            if (w->esquerdo->cor == PRETO && w->direito->cor == PRETO) {
                w->cor = VERMELHO;
                x = x->pai;
            } else {
                // Caso 3: Filho direito de w é preto e o esquerdo é vermelho
                if (w->direito->cor == PRETO) {
                    w->esquerdo->cor = PRETO;
                    w->cor = VERMELHO;
                    raiz = rotacao_direita(raiz, w, stats);
                    w = x->pai->direito;
                }
                // Caso 4: Filho direito de w é vermelho
                w->cor = x->pai->cor;
                x->pai->cor = PRETO;
                w->direito->cor = PRETO;
                raiz = rotacao_esquerda(raiz, x->pai, stats);
                x = raiz;
            }
        } else { // Casos simétricos
            RBNode* w = x->pai->esquerdo;
            
            if (w->cor == VERMELHO) {
                w->cor = PRETO;
                x->pai->cor = VERMELHO;
                raiz = rotacao_direita(raiz, x->pai, stats);
                w = x->pai->esquerdo;
            }
            
            if (w->direito->cor == PRETO && w->esquerdo->cor == PRETO) {
                w->cor = VERMELHO;
                x = x->pai;
            } else {
                if (w->esquerdo->cor == PRETO) {
                    w->direito->cor = PRETO;
                    w->cor = VERMELHO;
                    raiz = rotacao_esquerda(raiz, w, stats);
                    w = x->pai->esquerdo;
                }
                w->cor = x->pai->cor;
                x->pai->cor = PRETO;
                w->esquerdo->cor = PRETO;
                raiz = rotacao_direita(raiz, x->pai, stats);
                x = raiz;
            }
        }
    }
    x->cor = PRETO;
    return raiz;
}

RBNode* remover_rb(RBNode* raiz, int chave, RBStats* stats) {
    RBNode* z = (raiz == NULL) ? NULL_NODE : raiz;
    
    // Procura o nó a remover
    while (z != NULL_NODE) {
        stats->comparacoes++;
        if (chave == z->chave) {
            break;
        } else if (chave < z->chave) {
            z = z->esquerdo;
        } else {
            z = z->direito;
        }
    }
    
    if (z == NULL_NODE) return (raiz == NULL) ? NULL_NODE : raiz; // Chave inexistente
    
    RBNode* y = z;
    RBNode* x;
    Cor cor_original_y = y->cor;
    
    if (z->esquerdo == NULL_NODE) {
        x = z->direito;
        transplante_rb(&raiz, z, z->direito);
    } else if (z->direito == NULL_NODE) {
        x = z->esquerdo;
        transplante_rb(&raiz, z, z->esquerdo);
    } else {
        y = no_minimo(z->direito);
        cor_original_y = y->cor;
        x = y->direito;
        
        if (y->pai == z) {
            x->pai = y;
        } else {
            transplante_rb(&raiz, y, y->direito);
            y->direito = z->direito;
            y->direito->pai = y;
        }
        
        transplante_rb(&raiz, z, y);
        y->esquerdo = z->esquerdo;
        y->esquerdo->pai = y;
        y->cor = z->cor;
    }
    
    free(z);
    
    // Se a cor removida/movida foi preta, violamos propriedades de caminho negro
    if (cor_original_y == PRETO) {
        raiz = ajustar_remocao(raiz, x, stats);
    }
    
    return raiz;
}

static void liberar_aux(RBNode* no) {
    if (no != NULL_NODE) {
        liberar_aux(no->esquerdo);
        liberar_aux(no->direito);
        free(no);
    }
}

void liberar_arvore_rb(RBNode* raiz) {
    if (raiz != NULL && raiz != NULL_NODE) {
        liberar_aux(raiz);
    }
}