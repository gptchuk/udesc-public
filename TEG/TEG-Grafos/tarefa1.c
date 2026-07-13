#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARQUIVO "teste2.csv"
#define MAX_VERTICES 1000

typedef struct No {
    int vertice;
    struct No *prox;
} No;

typedef struct Grafo{
    int numVertices;
    int existeVertice[MAX_VERTICES];
    No *listaAdj[MAX_VERTICES];
} Grafo;

No* criarNo(int vertice) {
    No *novoNo = malloc(sizeof(No));
    if (novoNo == NULL) {
        printf("Erro ao alocar memoria para o no.\n");
        exit(1);
    }
    novoNo->vertice = vertice;
    novoNo->prox = NULL;
    return novoNo;
}

void inicializarGrafo(Grafo *grafo) {
    grafo->numVertices = 0;
    for (int i = 0; i < MAX_VERTICES; i++) {
        grafo->listaAdj[i] = NULL;
        grafo->existeVertice[i] = 0;
    }
}

void adicionarAresta(Grafo *grafo, int origem, int destino) {
    No *novoNo = criarNo(destino);
    novoNo->prox = grafo->listaAdj[origem];
    grafo->listaAdj[origem] = novoNo;

    grafo->existeVertice[origem] = 1;
    grafo->existeVertice[destino] = 1;

    // atualiza quantidaded de vértices conhecida
    if (origem >= grafo->numVertices)
        grafo->numVertices = origem + 1;

    if (destino >= grafo->numVertices)
        grafo->numVertices = destino + 1;
}

void lerCSV(Grafo *grafo, const char *nomeArquivo) {
    char linha[100];
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    while (fgets(linha, sizeof(linha), arquivo)) {
        int origem, destino;
        if (sscanf(linha, "%d,%d", &origem, &destino) == 2) {
            adicionarAresta(grafo, origem, destino);
            if (origem != destino) { // caso seja um loop, não adicionamos a aresta no sentido contrário
                adicionarAresta(grafo, destino, origem); // o grafo é não direcionado, então adicionamos a aresta em ambos os sentidos
            }
        }
    }

    fclose(arquivo);
}

// imprimir grafo (lista de adjacências)
void imprimirGrafo(Grafo *grafo) {
    for (int i = 0; i < grafo->numVertices; i++) {
        if(grafo->listaAdj[i] == NULL) continue; // desconsideramos vértices sem arestas
        printf("%d: ", i);
        No *atual = grafo->listaAdj[i];
        while (atual != NULL) {
            printf("%d ", atual->vertice);
            atual = atual->prox;
        }
        printf("\n");
    }
}

void liberarGrafo(Grafo *grafo) {
    for (int i = 0; i < grafo->numVertices; i++) {
        No *atual = grafo->listaAdj[i];
        while (atual != NULL) {
            No *temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
}

int grauVertice(Grafo *grafo, int vertice) {
    int grau = 0;
    No *atual = grafo->listaAdj[vertice];
    while (atual != NULL) {
        if (atual->vertice == vertice)
            grau += 2; // laço conta duas vezes no grau
        else
            grau++;

        atual = atual->prox;
    }
    return grau;
}

void informarGraus(Grafo *grafo) {
    int primeiro = 1;
    int maior, menor;
    for (int i = 0; i < grafo->numVertices; i++) {
        if (grafo->listaAdj[i] == NULL) continue; // desconsideramos vértices sem arestas
        int grau = grauVertice(grafo, i);
        if (primeiro) {
            maior = grau;
            menor = grau;
            primeiro = 0;
        }
        if (grau > maior)
            maior = grau;
        if (grau < menor)
            menor = grau;
    }
    printf("Maior grau: %d\n", maior);
    printf("Menor grau: %d\n", menor);
}


void verificarMultigrafo (Grafo *grafo) {
    int lacos = 0, paresMultiplas = 0, totalArestasEntreParesMultiplas = 0;
    int matriz[MAX_VERTICES][MAX_VERTICES];

    for (int i = 0; i < grafo->numVertices; i++)
        for (int j = 0; j < grafo->numVertices; j++)
            matriz[i][j] = 0; // inicializa a matriz de adjacências

    for (int i = 0; i < grafo->numVertices; i++) {
        No *atual = grafo->listaAdj[i];

        while (atual != NULL) {
            int destino = atual->vertice;
            if (destino == i) lacos++; // conta laços

            matriz[i][destino]++; // incrementa a contagem de arestas entre i e destino
            atual = atual->prox;
        }
    }

    // verificamos arestas múltiplas
    for (int i = 0; i < grafo->numVertices; i++) {
        for (int j = i + 1; j < grafo->numVertices; j++) {

            // como o grafo não é direcionado, verificamos apenas a metade superior da matriz
            if (matriz[i][j] > 1) {
                paresMultiplas++; // conta pares com arestas múltiplas
                totalArestasEntreParesMultiplas += matriz[i][j]; // conta o número de arestas múltiplas
            }
        }
    }

    if (lacos == 0 && paresMultiplas == 0) {
        printf("O grafo eh simples.\n");
    } else {
        printf("O grafo eh um multigrafo.\n");
        printf("Numero de lacos: %d\n", lacos);
        printf("Numero de pares de vertices com arestas multiplas: %d\n", paresMultiplas);
        printf("Numero de total de arestas multiplas: %d\n", totalArestasEntreParesMultiplas);
    }
}

int DFS(Grafo *grafo, int vertice, int *visitado) { // também vou usar a DFS para retornar o tamanho dos componentes conexos
    visitado[vertice] = 1;

    int tamanho = 1; // conta o próprio vértice

    No *atual = grafo->listaAdj[vertice];
    while (atual != NULL) {
        int adjacente = atual->vertice;
        if (!visitado[adjacente]) {
            tamanho += DFS(grafo, adjacente, visitado);
        }
        atual = atual->prox;
    }
    return tamanho;
}

void contarComponentesConexos(Grafo *grafo) {
    int visitado[MAX_VERTICES] = {0};
    int componentes = 0;
    
    printf("\n|\tIndice\t|\tTamanho\t|\n");

    for (int i = 0; i < grafo->numVertices; i++) {
        if (!grafo->existeVertice[i]) continue;
        if (!visitado[i]) {
            int tamanho = DFS(grafo, i, visitado);
            componentes++;
            printf("|\t%d\t|\t%d\t|\n", componentes, tamanho);
        }
    }

    printf("Quantidade total de componentes conexos: %d\n\n", componentes);
}

int main() {
    Grafo grafo;
    inicializarGrafo(&grafo);
    lerCSV(&grafo, ARQUIVO);

    int caso = 0;
    do {
        printf(" --- Menu ---\n");
        printf("[1] imprimir o grafo\n");
        printf("[2] informar maior e menor grau\n");
        printf("[3] e verificar multigrafo\n");
        printf("[4] contar componentes conexos\n");
        printf("[0] para sair\n");
        printf(">> ");
        scanf("%d", &caso);
        switch (caso)
        {
        case 1:
            imprimirGrafo(&grafo);
            break;
        case 2:
            informarGraus(&grafo);
            break;
        case 3:
            verificarMultigrafo(&grafo);
            break;
        case 4:
            contarComponentesConexos(&grafo);
            break;
        case 0:
            printf("Saindo...\n");
            break;
        default:
            printf("Opcao invalida.\n");
            break;
        }
    } while (caso != 0);

    
    liberarGrafo(&grafo);
    return 0;
}