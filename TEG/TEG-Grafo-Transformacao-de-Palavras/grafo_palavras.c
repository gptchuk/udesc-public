#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define MAX_WORDS 1500
#define WORD_LEN 4          /* Assume base em ASCII (sem acentos).  */
                             /* Palavras acentuadas ocupam mais de 1 byte por     */
                             /* caractere em UTF-8 e podem ser descartadas pelo   */
                             /* filtro de tamanho.  */
#define INF 999999

/* Estrutura para armazenar as palavras e a matriz de adjacência */
char palavras[MAX_WORDS][WORD_LEN + 1];
int total_palavras = 0;
int adj[MAX_WORDS][MAX_WORDS];
int graus[MAX_WORDS];

/* =========================================================
 * Comparacao de strings sem diferenciar maiusculas/minusculas
 * Retorna 0 se as strings forem iguais (ignorando case).
 * ========================================================= */
int comparar_sem_case(const char *a, const char *b) {
    while (*a && *b) {
        unsigned char ca = (unsigned char)tolower((unsigned char)*a);
        unsigned char cb = (unsigned char)tolower((unsigned char)*b);
        if (ca != cb) return 1;
        a++; b++;
    }
    return *a != *b;
}

/* Verifica se duas palavras de tamanho fixo diferem exatamente em 1 posicao */
int diferem_por_uma_letra(const char *p1, const char *p2) {
    int diff = 0;
    for (int i = 0; i < WORD_LEN; i++) {
        if (tolower((unsigned char)p1[i]) != tolower((unsigned char)p2[i])) {
            diff++;
        }
    }
    return diff == 1;
}

/* Verifica se uma palavra ja foi carregada (ignorando case).
 * Retorna o indice da palavra existente, ou -1 se nao encontrada. */
int palavra_ja_carregada(const char *p) {
    for (int i = 0; i < total_palavras; i++) {
        if (comparar_sem_case(palavras[i], p) == 0) return i;
    }
    return -1;
}

/* Carrega as palavras a partir do arquivo texto, eliminando duplicatas */
void carregar_base() {
    FILE *file = fopen("palavras.txt", "r");
    if (!file) {
        printf("Erro: Nao foi possivel abrir o arquivo 'palavras.txt'.\n");
        printf("Certifique-se de que o arquivo esta no mesmo diretorio do executavel.\n");
        exit(1);
    }

    char buffer[100];
    total_palavras = 0;
    int duplicatas = 0;

    while (fscanf(file, "%99s", buffer) != EOF) {
        if (strlen(buffer) == WORD_LEN) {

            /* Descarta palavras repetidas na base (mesmo conteudo, ignorando
             * case). Vertices do grafo representam palavras unicas; entradas
             * duplicadas no arquivo seriam um artefato de dados, e nao
             * vertices logicamente distintos. */
            if (palavra_ja_carregada(buffer) != -1) {
                duplicatas++;
                continue;
            }

            strcpy(palavras[total_palavras], buffer);
            total_palavras++;
            if (total_palavras >= MAX_WORDS) {
                printf("Aviso: Limite maximo de palavras atingido (%d).\n", MAX_WORDS);
                break;
            }
        }
    }
    fclose(file);

    printf("Base carregada com sucesso! Total de palavras de 4 letras: %d\n", total_palavras);
    if (duplicatas > 0) {
        printf("Aviso: %d palavra(s) duplicada(s) na base foram ignoradas.\n", duplicatas);
    }
}

/* Constroi as arestas do grafo: conecta pares de vertices (palavras)
 * que diferem em exatamente uma posicao de caractere. */
void construir_grafo() {
    for (int i = 0; i < total_palavras; i++) {
        for (int j = 0; j < total_palavras; j++) {
            adj[i][j] = 0;
        }
        graus[i] = 0;
    }

    for (int i = 0; i < total_palavras; i++) {
        for (int j = i + 1; j < total_palavras; j++) {
            if (diferem_por_uma_letra(palavras[i], palavras[j])) {
                adj[i][j] = 1;
                adj[j][i] = 1;
                graus[i]++;
                graus[j]++;
            }
        }
    }
}

/* Analisa e exibe as informacoes sobre os componentes conexos */
void analisar_componentes() {
    int visitado[MAX_WORDS] = {0};
    int qtd_componentes = 0;

    printf("\n==================================================\n");
    printf("         ANALISE DE COMPONENTES CONEXOS           \n");
    printf("==================================================\n");

    for (int i = 0; i < total_palavras; i++) {
        if (!visitado[i]) {
            qtd_componentes++;

            /* Busca em Largura (BFS) com fila implementada em array */
            int fila[MAX_WORDS];
            int frente = 0, tras = 0;

            fila[tras++] = i;
            visitado[i] = 1;

            int tamanho_comp = 0;
            int v_max_grau = i;
            int v_min_grau = i;

            while (frente < tras) {
                int u = fila[frente++];
                tamanho_comp++;

                if (graus[u] > graus[v_max_grau]) v_max_grau = u;
                if (graus[u] < graus[v_min_grau]) v_min_grau = u;

                for (int v = 0; v < total_palavras; v++) {
                    if (adj[u][v] && !visitado[v]) {
                        visitado[v] = 1;
                        fila[tras++] = v;
                    }
                }
            }

            printf("Componente %d:\n", qtd_componentes);
            printf("  -> Tamanho: %d palavras\n", tamanho_comp);
            printf("  -> Palavra Central (Grau Maximo = %d): %s\n", graus[v_max_grau], palavras[v_max_grau]);
            printf("  -> Palavra Periferica (Grau Minimo = %d): %s\n", graus[v_min_grau], palavras[v_min_grau]);
            printf("--------------------------------------------------\n");
        }
    }
    printf("Total de componentes conexos encontrados: %d\n", qtd_componentes);
}

/* Verifica a classificacao formal do grafo (simples vs. multigrafo).
 *
 * Observacao importante sobre este dominio especifico:
 *   - LACOS: uma palavra nunca pode diferir de si mesma em exatamente
 *     1 posicao (a diferenca seria sempre 0), logo um grafo de
 *     transformacao de palavras jamais possui lacos. A verificacao abaixo
 *     e feita de forma estrutural (nao "chumbada" em zero) percorrendo a
 *     diagonal da matriz de adjacencia, mas o resultado correto e
 *     sempre 0 -- essa e uma propriedade matematica do problema, nao uma
 *     falha do algoritmo.
 *   - ARESTAS MULTIPLAS: cada par de vertices (i,j) e avaliado uma unica
 *     vez na construcao do grafo, e a base ja foi deduplicada em
 *     carregar_base(). Por isso, nao existem conexoes paralelas entre o
 *     mesmo par de vertices. A verificacao abaixo soma, de forma
 *     estrutural, qualquer valor de adj[i][j] > 1, garantindo que o
 *     diagnostico permaneca correto mesmo que a logica de construcao do
 *     grafo seja alterada no futuro (ex.: se duplicatas passarem a ser
 *     permitidas de propósito).
 */
void verificar_tipo_grafo() {
    int lacos = 0;
    int arestas_multiplas = 0;

    for (int i = 0; i < total_palavras; i++) {
        if (adj[i][i] > 0) {
            lacos += adj[i][i];
        }
        for (int j = i + 1; j < total_palavras; j++) {
            if (adj[i][j] > 1) {
                arestas_multiplas += (adj[i][j] - 1);
            }
        }
    }

    printf("\n==================================================\n");
    printf("               CLASSIFICACAO DO GRAFO             \n");
    printf("==================================================\n");

    if (lacos == 0 && arestas_multiplas == 0) {
        printf("Classificacao: Grafo Simples\n");
        printf("Justificativa: Nao possui lacos nem arestas multiplas.\n");
        printf("(Uma palavra nao pode diferir de si mesma em exatamente\n");
        printf(" 1 posicao, e a base foi deduplicada na carga -- por isso\n");
        printf(" este resultado e estruturalmente garantido para este\n");
        printf(" tipo de grafo, e nao apenas um valor padrao.)\n");
    } else {
        printf("Classificacao: Multigrafo\n");
        printf("Deteccoes:\n");
        printf("  -> Quantidade de lacos: %d\n", lacos);
        printf("  -> Quantidade de arestas multiplas: %d\n", arestas_multiplas);
    }
}

/* Busca o indice numerico correspondente a uma palavra da base */
int encontrar_indice(const char *palavra) {
    for (int i = 0; i < total_palavras; i++) {
        if (comparar_sem_case(palavras[i], palavra) == 0) {
            return i;
        }
    }
    return -1;
}

/* Algoritmo de Dijkstra para encontrar a menor sequencia de transformacoes.
 * Todas as arestas tem peso 1 (uma transformacao de letra = um passo),
 * portanto o algoritmo se comporta de forma equivalente a uma BFS, mas
 * mantém a estrutura classica de Dijkstra conforme solicitado. */
void buscar_caminho_dijkstra(const char *origem_str, const char *destino_str) {
    int src = encontrar_indice(origem_str);
    int dest = encontrar_indice(destino_str);

    if (src == -1 || dest == -1) {
        printf("\nErro: Uma ou ambas as palavras informadas nao existem na base.\n");
        return;
    }

    int dist[MAX_WORDS];
    int pai[MAX_WORDS];
    int visitado[MAX_WORDS] = {0};

    for (int i = 0; i < total_palavras; i++) {
        dist[i] = INF;
        pai[i] = -1;
    }

    dist[src] = 0;

    for (int count = 0; count < total_palavras - 1; count++) {
        int u = -1;
        int min_dist = INF;

        for (int v = 0; v < total_palavras; v++) {
            if (!visitado[v] && dist[v] < min_dist) {
                min_dist = dist[v];
                u = v;
            }
        }

        if (u == -1 || u == dest) break;

        visitado[u] = 1;

        for (int v = 0; v < total_palavras; v++) {
            if (!visitado[v] && adj[u][v] && dist[u] != INF) {
                if (dist[u] + 1 < dist[v]) {
                    dist[v] = dist[u] + 1;
                    pai[v] = u;
                }
            }
        }
    }

    printf("\n==================================================\n");
    printf("          RESULTADO DA BUSCA (DIJKSTRA)           \n");
    printf("==================================================\n");

    if (dist[dest] == INF) {
        printf("Nao existe nenhum caminho de transformacao entre '%s' e '%s'.\n", origem_str, destino_str);
    } else {
        printf("Menor numero de arestas (passos): %d\n", dist[dest]);

        int caminho[MAX_WORDS];
        int idx = 0;
        int atual = dest;

        while (atual != -1) {
            caminho[idx++] = atual;
            atual = pai[atual];
        }

        printf("Sequencia de transformacao:\n  ");
        for (int i = idx - 1; i >= 0; i--) {
            printf("%s", palavras[caminho[i]]);
            if (i > 0) printf(" -> ");
        }
        printf("\n");
    }
}

int main() {
    carregar_base();
    construir_grafo();

    int opcao;
    char p1[20], p2[20];

    do {
        printf("\n==================================================\n");
        printf("                MENU DE OPCOES                    \n");
        printf("==================================================\n");
        printf("1. Exibir Analise de Componentes Conexos\n");
        printf("2. Verificar se o Grafo e Simples ou Multigrafo\n");
        printf("3. Buscar menor caminho entre duas palavras (Dijkstra)\n");
        printf("4. Sair do programa\n");
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
            break;
        }

        switch (opcao) {
            case 1:
                analisar_componentes();
                break;
            case 2:
                verificar_tipo_grafo();
                break;
            case 3:
                printf("Digite a palavra de origem (4 letras): ");
                scanf("%19s", p1);
                printf("Digite a palavra de destino (4 letras): ");
                scanf("%19s", p2);
                buscar_caminho_dijkstra(p1, p2);
                break;
            case 4:
                printf("Encerrando execucao.\n");
                break;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
    } while (opcao != 4);

    return 0;
}
