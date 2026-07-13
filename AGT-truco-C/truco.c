#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct
{
    char nome[20]; // Nome da carta (Ex: "4 de moles", "11 de espadas", etc.)
    int valor;     // Valor de forca da carta (1 a 10)
    int naipe;     // Naipe da carta (1 a 4: moles, espadas, copas, paus)
} Carta;

void removerCarta(Carta arr[], int *tamanho, int posicao)
{
    // Verifica se a posicao eh valida
    if (posicao < 0 || posicao >= *tamanho)
    {
        printf("Posicao invalida!\n");
        return;
    }

    // Move os elementos apos o indice para a esquerda
    for (int i = posicao; i < *tamanho - 1; i++)
    {
        arr[i] = arr[i + 1];
    }

    // Diminui o tamanho do array (ja que o elemento foi "removido")
    (*tamanho)--;
}

int main()
{
    int partPC = 0;
    int partPlayer = 0;

    srand((unsigned)time(NULL)); // semente para gerar numero aleatorio

    // Este eh constante (usado como referencia)
    Carta baralhoInicial[] = {
        // Cartas de 4
        {"4 de moles", 1, 1},
        {"4 de espadas", 1, 2},
        {"4 de copas", 1, 3},
        {"4 de paus", 1, 4},

        // Cartas de 5
        {"5 de moles", 2, 1},
        {"5 de espadas", 2, 2},
        {"5 de copas", 2, 3},
        {"5 de paus", 2, 4},

        // Cartas de 6
        {"6 de moles", 3, 1},
        {"6 de espadas", 3, 2},
        {"6 de copas", 3, 3},
        {"6 de paus", 3, 4},

        // Cartas de 7
        {"7 de moles", 4, 1},
        {"7 de espadas", 4, 2},
        {"7 de copas", 4, 3},
        {"7 de paus", 4, 4},

        // Cartas de 10
        {"10 de moles", 5, 1},
        {"10 de espadas", 5, 2},
        {"10 de copas", 5, 3},
        {"10 de paus", 5, 4},

        // Cartas de 11
        {"11 de moles", 6, 1},
        {"11 de espadas", 6, 2},
        {"11 de copas", 6, 3},
        {"11 de paus", 6, 4},

        // Cartas de 12
        {"12 de moles", 7, 1},
        {"12 de espadas", 7, 2},
        {"12 de copas", 7, 3},
        {"12 de paus", 7, 4},

        // Cartas de 1 (C!s)
        {"1 de moles", 8, 1},
        {"1 de espadas", 8, 2},
        {"1 de copas", 8, 3},
        {"1 de paus", 8, 4},

        // Cartas de 2
        {"2 de moles", 9, 1},
        {"2 de espadas", 9, 2},
        {"2 de copas", 9, 3},
        {"2 de paus", 9, 4},

        // Cartas de 3
        {"3 de moles", 10, 1},
        {"3 de espadas", 10, 2},
        {"3 de copas", 10, 3},
        {"3 de paus", 10, 4}};

    // Simulacao das partidas (cada iteracao do while eh uma partida)
    while (partPlayer < 12 && partPC < 12)
    {
        int pontosRound = 1; // quanto vale a partida
        int ptPC = 0;
        int ptPlayer = 0; // pontos da partida
        int tamanhoMaoPC = 3;
        int tamanhoMaoPlayer = 3;
        int tamanhoBaralho = 40;

        Carta baralho[40];
        Carta maoPC[3];
        Carta maoPlayer[3];
        memcpy(baralho, baralhoInicial, sizeof(baralhoInicial)); // Copia a alocacao de memoria de "baralhoInicial"

        int primeiraMao = 0; // 0 = empache na 1a; 1 = jogador ganha 1a; -1 = pc ganha 1a

        // Exemplo de exibicao
        // for (int i = 0; i < tamanhoBaralho; i++)
        // {
        //     printf("Nome: %s | Valor: %d | Naipe: %d\n", baralho[i].nome, baralho[i].valor, baralho[i].naipe);
        // }

        // Distribui cartas
        for (int i = 0; i < 3; i++)
        {
            int numRand = rand() % tamanhoBaralho;
            // printf("PC %d\n", numRand);
            maoPC[i] = baralho[numRand];
            removerCarta(baralho, &tamanhoBaralho, numRand);
        }
        for (int i = 0; i < 3; i++)
        {
            int numRand = rand() % tamanhoBaralho;
            // printf("PY %d\n", numRand);
            maoPlayer[i] = baralho[numRand];
            removerCarta(baralho, &tamanhoBaralho, numRand);
        }

        // Exibicao de pontos
        printf("\n---------------------------------------\nPontuacao: %dx%d\n", partPlayer, partPC);

        // Selecao da manilha
        int iManilha = rand() % tamanhoBaralho; // Nesse caso nem precisa remover carta (nao havera mais distribuicao nessa partida)
        Carta manilha = baralho[iManilha];
        printf("Carta na mesa: %s\n", manilha.nome);

        // Simulacao das maos ("rounds" da partida)
        for (int i = 0; i < 3; i++)
        {
            // Definindo probabilidade de truco da maquina
            int chanceTruco;
            switch (i)
            {
            case 0: // 10% de chance
                chanceTruco = rand() % 10;
                break;

            case 1: // 20% de chance
                chanceTruco = rand() % 5;
                break;

            case 2: // 33% de chance
                chanceTruco = rand() % 3;
                break;

            default:
                break;
            }

            // PC pedindo truco
            if (chanceTruco == 0 && pontosRound != 3)
            {
                char resp;
                printf("\nTRUUUUCOO!!!\nVoce cai ou nao? (S/N)\n\n");

                // Exibicao cartas do jogador
                printf("Suas cartas:\n");
                for (int j = 0; j < tamanhoMaoPlayer; j++)
                {
                    printf("%s\n", maoPlayer[j].nome);
                }

                scanf(" %c", &resp);

                while (resp != 'S' && resp != 'N')
                {
                    printf("Resposta invalida\n");
                    scanf(" %c", &resp);
                }
                if (resp == 'S')
                {
                    pontosRound = 3;
                }
                else
                {
                    partPC++;
                    break;
                }
            }

            // Maquina seleciona a carta
            int iCartaPC = rand() % tamanhoMaoPC;
            Carta cartaPC = maoPC[iCartaPC];
            removerCarta(maoPC, &tamanhoMaoPC, iCartaPC);
            printf("\nA máquina jogou %s\n", cartaPC.nome);

            if (pontosRound != 3)
            {
                char resp;
                printf("\nQuer pedir truco? (S/N)\n");

                // Exibicao cartas do jogador (se for primeira mao)
                if (i == 0)
                {
                    printf("Suas cartas:\n");
                    for (int j = 0; j < tamanhoMaoPlayer; j++)
                    {
                        printf("%s\n", maoPlayer[j].nome);
                    }
                }

                scanf(" %c", &resp);

                while (resp != 'S' && resp != 'N')
                {
                    printf("Resposta invalida\n");
                    scanf(" %c", &resp);
                }
                if (resp == 'S')
                {
                    if (rand() % 2 == 0)
                    {
                        printf("A maquina cai!!\n");
                        pontosRound = 3;
                    }
                    else
                    {
                        printf("A maquina foge!!\n");
                        partPlayer++;
                        break;
                    }
                }
            }

            // Jogador seleciona a carta
            printf("\nQual carta ira jogar?\n\n");
            int iCartaPlayer;
            for (int j = 0; j < tamanhoMaoPlayer; j++)
            {
                printf("%d) %s\n", (j + 1), maoPlayer[j].nome);
            }
            do
            {
                printf("> ");
                scanf("%d", &iCartaPlayer);
                iCartaPlayer--;
                if (iCartaPlayer < 0 || iCartaPlayer >= tamanhoMaoPlayer)
                {
                    printf("Carta invalida; selecione uma do baralho\n");
                }
            } while (iCartaPlayer < 0 || iCartaPlayer >= tamanhoMaoPlayer);
            Carta cartaPlayer = maoPlayer[iCartaPlayer];
            removerCarta(maoPlayer, &tamanhoMaoPlayer, iCartaPlayer);

            // Decisao de pontuacao
            if (cartaPlayer.valor == (manilha.valor + 1))
            { // Jogador tem a manilha

                if (cartaPC.valor == (manilha.valor + 1))
                { // PC tem a manilha

                    if (cartaPlayer.naipe > cartaPC.naipe)
                    {
                        ptPlayer++;
                        printf("Ganhou a mao! Voce tem uma manilha mais forte.\n");
                    }
                    else
                    {
                        ptPC++;
                        printf("Perdeu a mao! O PC tem uma manilha mais forte.\n");
                    }
                }
                else
                { // PC nao tem manilha
                    ptPlayer++;
                    printf("Ganhou a mao! Voce tem uma manilha.\n");
                }
            }
            else if (cartaPC.valor == (manilha.valor + 1))
            { // soh PC tem manilha
                ptPC++;
                printf("Perdeu a mao! O PC tem uma manilha.\n");
            }
            else
            { // Ambos cartas diferentes
                if (cartaPlayer.valor > cartaPC.valor)
                { // Carta do player mais forte
                    ptPlayer++;
                    printf("Ganhou a mao! Voce tem uma carta mais forte.\n");
                }
                else if (cartaPC.valor > cartaPlayer.valor)
                { // Carta do PC mais forte
                    ptPC++;
                    printf("Perdeu a mao! O PC tem uma carta mais forte.\n");
                }
                else
                {
                    printf("Empache!!\n");
                }
            }

            // Definir quem fez a primeira mao
            if (i == 0 && ptPlayer > ptPC)
            {
                primeiraMao++; // = 1 -> Jogador fez a primeira
            }
            else
            {
                primeiraMao--; // = -1 -> PC fez a primeira
            }

            // Definicao de placar
            if (ptPC == 2 || (primeiraMao == -1 && ((i == 2 && ptPlayer == 1) || (i == 1 && ptPlayer == 0))) || (ptPC > ptPlayer && i == 1))
            {
                partPC += pontosRound;
                break;
            }
            if (ptPlayer == 2 || (primeiraMao == 1 && ((i == 2 && ptPC == 1) || (i == 1 && ptPC == 0))) || (ptPlayer > ptPC && i == 1))
            {
                partPlayer += pontosRound;
                break;
            }
            if (i == 2)
            { // Ultima mao, ganha o maior naipe
                if (cartaPlayer.naipe > cartaPC.naipe)
                {
                    printf("Ganhou a mao!! Na ultima mao voce tinha um naipe mais forte\n");
                    partPlayer += pontosRound;
                    break;
                }
                else
                {
                    printf("Perdeu a mao!! Na ultima mao o PC tinha um naipe mais forte\n");
                    partPC += pontosRound;
                    break;
                }
            }
        }
    }

    printf("Pontuacao final : %dx%d\n", partPlayer, partPC);
    if (partPC > partPlayer)
    {
        printf("O PC ganhou o jogo!! :c");
    }
    else
    {
        printf("Voce ganhou o jogo!!! :D");
    }

    return 0;
}