#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Data
{
    int dia, mes, ano;
};

struct Horario
{
    int hora, min;
};

struct Evento
{
    struct Data data;
    struct Horario inicial;
    struct Horario final;

    char descricao[50];
    char local[50];
};

void novo_evento (struct Evento *v, int *n);

int valida_data (int dia, int mes, int ano);
int valida_horario(int hora, int min);

void input_data_validada (int *dia, int *mes, int *ano);
void input_horario_validado (int *hora, int *min);

void mostrar_todos_eventos (struct Evento *v, int n);
void mostrar_eventos_por_data(struct Evento *v, int dia, int mes, int ano, int n);
void mostrar_eventos_por_desc(struct Evento *v, char desc[50], int n);
void remover_evento (struct Evento *v, int removedatadia, int removedatames, int removedataano, int removehora, int removeminuto, int *n);

int main () {
    int n = 0, i, op;
    // n é o tamanho do nosso vetor de Structs
    // i é contador
    // op é usado para a navegação

    struct Evento *v = NULL;

    char nome_arquivo[256];
    printf("Insira o nome do arquivo\n");
    scanf(" %s", nome_arquivo);
    //nome do arquivo -.-
    
    int dia, mes, ano, hora, min;
    char desc[50];

    FILE *entrada = fopen( nome_arquivo , "rt" );
    if (entrada == NULL)
    {
		printf("Arquivo nao encontrado!\n");
    }
    else {
        fscanf( entrada, "%d", &n );
        v = malloc( sizeof( struct Evento ) * n );
		for( i = 0 ; i < n ; i++ ){
			fscanf( entrada, " %[^\n]", v[i].descricao );
			fscanf( entrada, " %[^\n]", v[i].local );
			fscanf( entrada, "%d %d %d",    &v[i].data.dia,
										    &v[i].data.mes,
										    &v[i].data.ano );
			fscanf( entrada, "%d %d",   &v[i].inicial.hora,
									    &v[i].inicial.min );
			fscanf( entrada, "%d %d",   &v[i].final.hora,
									    &v[i].final.min );
		}
		fclose( entrada );
        printf("\n\t<<< Arquivo carregado! >>>\n\n");
    }
    
    struct Evento *temp;

    // Loop do Menu de Navegacao
    while ( op != 6 )
    {
        printf("\n- - - - - - - - - MENU - - - - - - - - -\n   [1] Cadastrar Novo Evento\n   [2] Mostrar todos os Eventos da Agenda \n   [3] Buscar Eventos por Data\n   [4] Buscar Eventos pela descricao\n   [5] Remover Evento\n   [6] Sair do Programa\n > Navegue inserindo os indices: [1] [2] [3] [4] [5] [6]\n");
        scanf(" %d", &op);
        switch ( op )
        {
        case 1:
            temp = realloc(v, sizeof(struct Evento) * (n + 1));
            if (temp != NULL) {
                v = temp;
                novo_evento(v, &n); // n só será incrementado se realloc der certo
            } else {
                printf("Falha ao realocar memória.\n");
            }
            break;
        case 2:
            mostrar_todos_eventos( v, n );    
            break;
        case 3:
            input_data_validada( &dia, &mes, &ano );
            mostrar_eventos_por_data( v, dia, mes, ano, n );
            break;
        case 4:
            printf("Qual a descricao do evento que esta buscando?\n");
            scanf(" %s", desc);

            mostrar_eventos_por_desc( v, desc, n );
            break;
        case 5:
            printf("Qual a data do evento que deseja remover?\n");
            input_data_validada( &dia, &mes, &ano );

            printf("Qual o horario do evento que deseja remover?\n");
            input_horario_validado( &hora, &min );

            remover_evento( v, dia, mes, ano, hora, min, &n );
            break;
        case 6:
            break;
        default:
            printf("Insira uma opcao valida\n");
            break;
        }
    }

    
    FILE *saida = fopen( "agenda.txt", "wt" );
	fprintf( saida, "%d\n", n );
    for ( int i = 0; i < n; i++ )
    {
        fprintf( saida, "%s\n",         v[i].descricao );
        fprintf( saida, "%s\n",         v[i].local );
        fprintf( saida, "%d %d %d\n",   v[i].data.dia,
                                        v[i].data.mes,
                                        v[i].data.ano );
        fprintf( saida, "%d %d\n",      v[i].inicial.hora,
                                        v[i].inicial.min );
        fprintf( saida, "%d %d\n",      v[i].final.hora,
                                        v[i].final.min );

        // print no terminal
        // printf( "%s\n", v[i].descricao);
        // printf( "%s\n", v[i].local);
        // printf( "%d %d %d\n", v[i].data.dia,
        //                             v[i].data.mes,
        //                             v[i].data.ano );
        // printf( "%d %d\n",    v[i].inicial.hora,
        //                             v[i].inicial.min);
        // printf( "%d %d\n",    v[i].final.hora,
        //                             v[i].final.min);

    }
	fclose( saida );
    free( v );
}
int comparar_eventos(const struct Evento *a, const struct Evento *b) {
    struct Evento evA = *a;
    struct Evento evB = *b;

    if (evA.data.ano != evB.data.ano) return evA.data.ano - evB.data.ano;
    if (evA.data.mes != evB.data.mes) return evA.data.mes - evB.data.mes;
    if (evA.data.dia != evB.data.dia) return evA.data.dia - evB.data.dia;

    int minutos_a = evA.inicial.hora * 60 + evA.inicial.min;
    int minutos_b = evB.inicial.hora * 60 + evB.inicial.min;
    return minutos_a - minutos_b;
}

void novo_evento(struct Evento *v, int *n) {
    int dia, mes, ano, hora, min, hora_f, min_f, i;
    char desc[50];
    char local[50];
    int ha_conflito_evento = 0;

    do {
        ha_conflito_evento = 0;
        input_data_validada(&dia, &mes, &ano);
        input_horario_validado(&hora, &min);

        do {
            printf("Qual o horario de termino do evento?\n");
            input_horario_validado(&hora_f, &min_f);
            if (hora_f < hora || (hora_f == hora && min_f <= min)) {
                printf("O horario final deve ser depois do horario inicial.\n");
            }
        } while (hora_f < hora || (hora_f == hora && min_f <= min));

        for (i = 0; i < *n; i++) {
            if (v[i].data.ano == ano && v[i].data.mes == mes && v[i].data.dia == dia) {
                int ini1 = hora * 60 + min;
                int fim1 = hora_f * 60 + min_f;
                int ini2 = v[i].inicial.hora * 60 + v[i].inicial.min;
                int fim2 = v[i].final.hora * 60 + v[i].final.min;

                if (!(fim1 <= ini2 || ini1 >= fim2)) {
                    ha_conflito_evento = 1;
                    break;
                }
            }
        }

        if (ha_conflito_evento == 1) {
            printf("Detectado um conflito de Horarios com o seguinte evento:\n");
            printf("| Descricao: %s\n", v[i].descricao);
            printf("| Local: %s\n", v[i].local);
            printf("| Data:  %d/%d/%d\n", v[i].data.dia, v[i].data.mes, v[i].data.ano);
            printf("| Inicio: %d:%02d\n", v[i].inicial.hora, v[i].inicial.min);
            printf("| Fim: %d:%02d\n", v[i].final.hora, v[i].final.min);
        }
    } while (ha_conflito_evento == 1);

    printf("Insira a descricao do evento:\n");
    scanf(" %[^\n]", desc);
    printf("Insira o local do evento:\n");
    scanf(" %[^\n]", local);

    // Inserir na posicao correta para manter ordenado
    int pos = *n;
    for (i = 0; i < *n; i++) {
        struct Evento novo;
        novo.data.dia = dia;
        novo.data.mes = mes;
        novo.data.ano = ano;
        novo.inicial.hora = hora;
        novo.inicial.min = min;
        if (comparar_eventos(&novo, &v[i]) < 0) {
            pos = i;
            break;
        }
    }

    for (i = *n; i > pos; i--) {
        v[i] = v[i - 1];
    }

    v[pos].data.dia = dia;
    v[pos].data.mes = mes;
    v[pos].data.ano = ano;
    v[pos].inicial.hora = hora;
    v[pos].inicial.min = min;
    v[pos].final.hora = hora_f;
    v[pos].final.min = min_f;
    strcpy(v[pos].descricao, desc);
    strcpy(v[pos].local, local);

    (*n)++;
}




void input_data_validada (int *dia, int *mes, int *ano) {
    printf("Insira a data desejada (dia, mes e ano) formato: \"dd mm aaaa\"\n");
    do {
        scanf(" %d %d %d", &(*dia), &(*mes), &(*ano));
        if (!valida_data(*dia, *mes, *ano)) printf("Insira valores validos para a data\n");
    } while (!valida_data(*dia, *mes, *ano));
}

void input_horario_validado (int *hora, int *min) {
    printf("Insira o horario desejado (hora e minutos), no seguinte formato: \"hh mm\"\n");
    do {
        scanf(" %d %d", &(*hora), &(*min));
        if (!valida_horario(*hora, *min)) printf("Insira valores validos para o horario\n");
    } while (!valida_horario(*hora, *min));
}

int valida_data (int dia, int mes, int ano) {
    if (mes > 0 && mes <= 12) {
        if (mes == 1 || mes == 3 || mes == 5|| mes == 7 || mes == 8 || mes == 10 || mes == 12) { //meses com 31 dias
            if (dia > 0 && dia <= 31)
            {
                return 1;
            }
        } else if (mes == 4 || mes == 6 || mes == 9 || mes == 11) { // meses com 30 dias
            if (dia > 0 && dia <= 30)
            {
                return 1;
            }
        } else if (mes == 2) { // fevereiro e seus 28 ou 29 dias
            int ehBissexto = (ano % 4 == 0 && (ano % 100 != 0 || ano % 400 == 0)); // boolean para verificar se ano eh bissexto
            if (ehBissexto) {
                if (dia > 0 && dia <= 29) return 1;
            } else {
                if (dia > 0 && dia <= 28) return 1;
            }
        }
    }
    return 0;
}

int valida_horario (int hora, int min) {
    if (hora >= 0 && hora < 24 && min >= 0 && min < 60) return 1;
    return 0;
}

void mostrar_todos_eventos(struct Evento *v,int n){

    for(int i = 0 ; i < n; i++ ){
            printf("\n");  
            printf( "| Evento %i:\n", i+1);
			printf( "| Descricao: %s \n",        v[i].descricao);
			printf( "| Local:     %s \n",        v[i].local);
			printf( "| Data:      %d/%d/%d\n",   v[i].data.dia,
									           v[i].data.mes,
									           v[i].data.ano );
			printf( "| Inicio:    %d:",      v[i].inicial.hora);
            if(v[i].inicial.min == 0) printf("00\n");
            else printf("%d\n",v[i].inicial.min);
			printf( "| Fim:       %d:",      v[i].final.hora);
            if(v[i].final.min == 0) printf("00\n");
            else printf("%d\n", v[i].final.min);
		}
}

void mostrar_eventos_por_data(struct Evento *v, int dia, int mes, int ano, int n){
    int evento_dia = 0;

    printf("\n");
    printf("Eventos do dia %i/%i/%i:\n\n", dia,mes,ano);

    for(int i = 0 ; i < n ; i++ ){
        if(v[i].data.dia == dia && v[i].data.mes == mes && v[i].data.ano == ano){
			printf( "| Descricao: %s \n",        v[i].descricao);
			printf( "| Local:     %s \n",        v[i].local);
			printf( "| Inicio:    %d:",      v[i].inicial.hora);
            if(v[i].inicial.min == 0) printf("00\n");
            else printf("%d\n",v[i].inicial.min);
			printf( "| Fim:       %d:",      v[i].final.hora);
            if(v[i].final.min == 0) printf("00\n");
            else printf("%d\n", v[i].final.min);
            evento_dia++;                    
        }
    }

    if(evento_dia == 0){
        printf("Nao ha eventos marcados nessa data\n");
    }

}    

void mostrar_eventos_por_desc(struct Evento *v,char desc[50],int n){
    int evento_desc = 0;


    printf("\n");
    printf("Eventos com a descricao \"%s\":\n\n", desc);

    for(int i = 0 ; i < n ; i++ ){
        if(strcmp(v[i].descricao,desc) == 0){
            printf("Evento %i:\n", evento_desc+1);  
			printf( "| Local:     %s \n",        v[i].local);
			printf( "| Data:      %d/%d/%d\n",   v[i].data.dia,
									           v[i].data.mes,
									           v[i].data.ano );
			printf( "| Inicio:    %d:",      v[i].inicial.hora);
            if(v[i].inicial.min == 0) printf("00\n");
            else printf("%d\n",v[i].inicial.min);
			printf( "| Fim:       %d:",      v[i].final.hora);
            if(v[i].final.min == 0) printf("00\n");
            else printf("%d\n", v[i].final.min);

            evento_desc++;                      
		}
    }
    if(evento_desc == 0){
        printf("Nao ha eventos marcados com esta descricao\n");
    }

}

void remover_evento (struct Evento *v,int removedatadia,int removedatames,int removedataano,int removehora,int removeminuto, int *n){
int removeuevento = 0;

    for(int i = 0 ; i < (*n) ; i++ ){
        if(v[i].data.dia == removedatadia && v[i].data.mes == removedatames && v[i].data.ano == removedataano && removehora == v[i].inicial.hora && removeminuto == v[i].inicial.min){
          if(i == (*n)-1){
            (*n)--;
            removeuevento++;
            }else if(i >= 0){
            strcpy(v[i].descricao,v[i+1].descricao);
		    strcpy(v[i].local,v[i+1].local);
		    v[i].data.dia  =    v[i+1].data.dia;
			v[i].data.mes =     v[i+1].data.mes;
			v[i].data.ano =     v[i+1].data.ano;
		    v[i].inicial.hora = v[i+1].inicial.hora;
			v[i].inicial.min =  v[i+1].inicial.min;
			v[i].final.hora =   v[i+1].final.hora;
			v[i].final.min =    v[i+1].final.min;
            (*n)--; 
            removeuevento++;
          }          
        }
    }

    if(removeuevento == 0){
    printf("Nao ha eventos com essa data e horario\n");
    }else{
    printf("Seu evento foi removido com sucesso\n");    
    }

}