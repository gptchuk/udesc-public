#include <stdio.h>
#include <stdlib.h>
#include "arvore_b.h"

// Função auxiliar para alocar um nó
static BNode* alocar_no_b(BTree* arvore, int folha) {
    BNode* no = (BNode*)malloc(sizeof(BNode));
    no->folha = folha;
    no->num_chaves = 0;
    // Aloca max_chaves + 1 para suportar o estouro temporário do Bottom-Up
    no->chaves = (int*)malloc((arvore->max_chaves + 1) * sizeof(int));
    no->filhos = (BNode**)malloc((arvore->max_chaves + 2) * sizeof(BNode*));
    
    for (int i = 0; i < arvore->max_chaves + 2; i++) {
        no->filhos[i] = NULL;
    }
    return no;
}

BTree* criar_arvore_b(int ordem) {
    BTree* arvore = (BTree*)malloc(sizeof(BTree));
    arvore->ordem = ordem;
    arvore->max_chaves = 2 * ordem;
    arvore->min_chaves = ordem;
    arvore->raiz = alocar_no_b(arvore, 1);
    return arvore;
}

// Inserção Bottom-Up recursiva
static BNode* inserir_recursivo(BTree* arvore, BNode* no, int chave, int* chave_promovida, BStats* stats) {
    int i = no->num_chaves - 1;
    
    if (no->folha) {
        while (i >= 0 && chave < no->chaves[i]) {
            stats->comparacoes++;
            no->chaves[i + 1] = no->chaves[i];
            i--;
        }
        if (i >= 0) stats->comparacoes++; // Comparação de saída do while
        
        no->chaves[i + 1] = chave;
        no->num_chaves++;
    } else {
        while (i >= 0 && chave < no->chaves[i]) {
            stats->comparacoes++;
            i--;
        }
        if (i >= 0) stats->comparacoes++;
        
        i++;
        int chave_prom;
        BNode* filho_promovido = inserir_recursivo(arvore, no->filhos[i], chave, &chave_prom, stats);
        
        if (filho_promovido) {
            // Acomoda a chave e o ponteiro que subiram do filho
            int j = no->num_chaves - 1;
            while (j >= i) {
                no->chaves[j + 1] = no->chaves[j];
                no->filhos[j + 2] = no->filhos[j + 1];
                j--;
            }
            no->chaves[i] = chave_prom;
            no->filhos[i + 1] = filho_promovido;
            no->num_chaves++;
        }
    }

    // Verifica se o nó atual estourou a capacidade
    if (no->num_chaves > arvore->max_chaves) {
        stats->rotacoes++; // Conta como modificação estrutural
        BNode* novo_no = alocar_no_b(arvore, no->folha);
        int meio = arvore->min_chaves; // Mediana a ser promovida
        
        *chave_promovida = no->chaves[meio];
        
        // Copia a metade direita para o novo nó
        int k = 0;
        for (int j = meio + 1; j <= arvore->max_chaves; j++) {
            novo_no->chaves[k] = no->chaves[j];
            k++;
        }
        if (!no->folha) {
            k = 0;
            for (int j = meio + 1; j <= arvore->max_chaves + 1; j++) {
                novo_no->filhos[k] = no->filhos[j];
                k++;
            }
        }
        
        novo_no->num_chaves = arvore->max_chaves - meio;
        no->num_chaves = meio;
        
        return novo_no; // Retorna o irmão criado para ser acoplado no pai
    }
    
    return NULL;
}

void inserir_b(BTree* arvore, int chave, BStats* stats) {
    int chave_promovida;
    BNode* novo_filho = inserir_recursivo(arvore, arvore->raiz, chave, &chave_promovida, stats);
    
    // Se a raiz estourou, cria uma nova raiz crescendo a árvore para cima
    if (novo_filho) {
        stats->rotacoes++;
        BNode* nova_raiz = alocar_no_b(arvore, 0);
        nova_raiz->chaves[0] = chave_promovida;
        nova_raiz->filhos[0] = arvore->raiz;
        nova_raiz->filhos[1] = novo_filho;
        nova_raiz->num_chaves = 1;
        arvore->raiz = nova_raiz;
    }
}

// --- Funções de Remoção ---

static void merge(BTree* arvore, BNode* x, int idx, BStats* stats) {
    stats->rotacoes++;
    BNode* filho_esq = x->filhos[idx];
    BNode* filho_dir = x->filhos[idx + 1];
    
    filho_esq->chaves[arvore->min_chaves] = x->chaves[idx];
    
    for (int i = 0; i < filho_dir->num_chaves; ++i) {
        filho_esq->chaves[i + arvore->min_chaves + 1] = filho_dir->chaves[i];
    }
    if (!filho_esq->folha) {
        for (int i = 0; i <= filho_dir->num_chaves; ++i) {
            filho_esq->filhos[i + arvore->min_chaves + 1] = filho_dir->filhos[i];
        }
    }
    
    filho_esq->num_chaves += filho_dir->num_chaves + 1;
    
    for (int i = idx + 1; i < x->num_chaves; ++i) {
        x->chaves[i - 1] = x->chaves[i];
    }
    for (int i = idx + 2; i <= x->num_chaves; ++i) {
        x->filhos[i - 1] = x->filhos[i];
    }
    x->num_chaves--;
    
    free(filho_dir->chaves);
    free(filho_dir->filhos);
    free(filho_dir);
}

static void preencher(BTree* arvore, BNode* x, int idx, BStats* stats) {
    if (idx != 0 && x->filhos[idx - 1]->num_chaves > arvore->min_chaves) {
        stats->rotacoes++;
        BNode* filho = x->filhos[idx];
        BNode* irmao = x->filhos[idx - 1];
        
        for (int i = filho->num_chaves - 1; i >= 0; --i)
            filho->chaves[i + 1] = filho->chaves[i];
        if (!filho->folha) {
            for (int i = filho->num_chaves; i >= 0; --i)
                filho->filhos[i + 1] = filho->filhos[i];
            filho->filhos[0] = irmao->filhos[irmao->num_chaves];
        }
        filho->chaves[0] = x->chaves[idx - 1];
        x->chaves[idx - 1] = irmao->chaves[irmao->num_chaves - 1];
        filho->num_chaves++;
        irmao->num_chaves--;
    } 
    else if (idx != x->num_chaves && x->filhos[idx + 1]->num_chaves > arvore->min_chaves) {
        stats->rotacoes++;
        BNode* filho = x->filhos[idx];
        BNode* irmao = x->filhos[idx + 1];
        
        filho->chaves[filho->num_chaves] = x->chaves[idx];
        if (!filho->folha) {
            filho->filhos[filho->num_chaves + 1] = irmao->filhos[0];
        }
        x->chaves[idx] = irmao->chaves[0];
        for (int i = 1; i < irmao->num_chaves; ++i)
            irmao->chaves[i - 1] = irmao->chaves[i];
        if (!irmao->folha) {
            for (int i = 1; i <= irmao->num_chaves; ++i)
                irmao->filhos[i - 1] = irmao->filhos[i];
        }
        filho->num_chaves++;
        irmao->num_chaves--;
    } 
    else {
        if (idx != x->num_chaves)
            merge(arvore, x, idx, stats);
        else
            merge(arvore, x, idx - 1, stats);
    }
}

static void remover_interno(BTree* arvore, BNode* x, int chave, BStats* stats) {
    int idx = 0;
    while (idx < x->num_chaves && x->chaves[idx] < chave) {
        stats->comparacoes++;
        idx++;
    }
    if (idx < x->num_chaves) stats->comparacoes++; 

    if (idx < x->num_chaves && x->chaves[idx] == chave) {
        if (x->folha) {
            for (int i = idx + 1; i < x->num_chaves; ++i) {
                x->chaves[i - 1] = x->chaves[i];
            }
            x->num_chaves--;
        } else {
            BNode* y = x->filhos[idx];
            if (y->num_chaves > arvore->min_chaves) {
                while (!y->folha) y = y->filhos[y->num_chaves];
                int pred = y->chaves[y->num_chaves - 1];
                x->chaves[idx] = pred;
                remover_interno(arvore, x->filhos[idx], pred, stats);
            } else {
                BNode* z = x->filhos[idx + 1];
                if (z->num_chaves > arvore->min_chaves) {
                    while (!z->folha) z = z->filhos[0];
                    int suc = z->chaves[0];
                    x->chaves[idx] = suc;
                    remover_interno(arvore, x->filhos[idx + 1], suc, stats);
                } else {
                    merge(arvore, x, idx, stats);
                    remover_interno(arvore, x->filhos[idx], chave, stats);
                }
            }
        }
    } else {
        if (x->folha) return; 
        
        int flag = (idx == x->num_chaves);
        if (x->filhos[idx]->num_chaves <= arvore->min_chaves) {
            preencher(arvore, x, idx, stats);
        }
        
        if (flag && idx > x->num_chaves) {
            remover_interno(arvore, x->filhos[idx - 1], chave, stats);
        } else {
            remover_interno(arvore, x->filhos[idx], chave, stats);
        }
    }
}

void remover_b(BTree* arvore, int chave, BStats* stats) {
    if (!arvore->raiz) return;
    
    remover_interno(arvore, arvore->raiz, chave, stats);
    
    if (arvore->raiz->num_chaves == 0) {
        BNode* tmp = arvore->raiz;
        if (arvore->raiz->folha) {
            arvore->raiz = NULL;
        } else {
            arvore->raiz = arvore->raiz->filhos[0];
        }
        free(tmp->chaves);
        free(tmp->filhos);
        free(tmp);
    }
}

static void liberar_no(BNode* no) {
    if (no) {
        if (!no->folha) {
            for (int i = 0; i <= no->num_chaves; i++) {
                liberar_no(no->filhos[i]);
            }
        }
        free(no->chaves);
        free(no->filhos);
        free(no);
    }
}

void liberar_arvore_b(BTree* arvore) {
    if (arvore) {
        liberar_no(arvore->raiz);
        free(arvore);
    }
}