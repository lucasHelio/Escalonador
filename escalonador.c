#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


#define QUANTUM 2           //tempo de ciclo
#define MAXPROCESSES 5      //Numero maximo de processos
#define MAXTIME 5          //tempo maximo que o processo pode ter

//enum status{Esperando=0,Concluido=2,Executando=1};
const char *STATUS_STRING[] = { "Executando", "Esperando", "Concluido"};


int ProcessExe = MAXPROCESSES;           //Processos a serem executados



//Estrutura do Processo
typedef struct reg
{
    int tempoChegada;   //momento que o processo chega na fila
    char status[10];    //status
    int pid;            //process identification
    int tp;             //tempo de processo
    //struct reg *proximo; //proximo processo na fila
}Processo;





typedef struct No {
    Processo *processo;
    struct No *proximo;
} No;

typedef struct {
    No *inicio, *fim;
    int tam;
} Lista;

// inserção no início da lista
void inserirInicio(Lista *lista, Processo *processo) {
    No *novo = (No*)malloc(sizeof(No)); // cria um novo nó
    novo->processo = processo;// (*novo).valor = valor
    strcpy(novo->processo->status, STATUS_STRING[1]/*EXECUTANDO*/);

    if(lista->inicio == NULL) { // a lista está vazia
        novo->proximo = NULL;
        lista->inicio = novo;
        lista->fim = novo;
     } else { // a lista não está vazia
         //novo->proximo = lista->inicio; //consertar
         //lista->fim = novo;
         //if(novo == lista->fim)
         if(strcmp(lista->inicio->processo->status, STATUS_STRING[3]/*CONCLUIDO*/) != 0){
            lista->fim = lista->inicio;
            lista->fim->proximo = NULL;
            return;
         }
         if(novo->processo == NULL){//não faz nada
            return;//continue;//return?
         }
         //ter atenção aqui
         lista->inicio = novo;
         strcpy(lista->inicio->processo->status, STATUS_STRING[1]);
     }
    lista->tam++;
}

// inserir no final da lista
void inserirFim(Lista *lista, Processo *processo) {
    No *novo = (No*)malloc(sizeof(No)); // cria um novo nó
    novo->processo = processo;
    novo->proximo = NULL;
    

    if(lista->inicio == NULL) { // lista vazia
        strcpy(novo->processo->status, STATUS_STRING[1]/*EXECUTANDO*/);
        lista->inicio = novo;
        lista->fim = novo;
    } else { // lista não vazia
        strcpy(novo->processo->status, STATUS_STRING[2]/*ESPERA*/);
        lista->fim->proximo = novo;
        lista->fim = novo;
    }
    lista->tam++;
}


void processoConcluido(Processo *p, Processo **ListaConcluidos){ //talves não esteja alterando P
    strcpy(p->status, "concluido");
    ListaConcluidos[MAXPROCESSES - ProcessExe]->pid = p->pid;
    ListaConcluidos[MAXPROCESSES - ProcessExe]->tempoChegada = p->tempoChegada;
    strcpy(ListaConcluidos[MAXPROCESSES - ProcessExe]->status, p->status);
    ListaConcluidos[MAXPROCESSES - ProcessExe]->tp = p->tp;
    ProcessExe--;
}

//imprime os dados do processo na tela
void printProcess(Processo *P){
    printf("Processo: %d    Status: %s      Tempo Restante: %d      Tempo Chegada: %d\n", P->pid, P->status, P->tp, P->tempoChegada);
}

void createProcesses (Processo **P) {
    for (int i=0; i<MAXPROCESSES; i++) {
        Processo *process = (Processo *) malloc(sizeof(Processo));      //alocação do processo
        int rTime = rand() % MAXTIME + 1;                                  //valor aleatorio de 1 até MAXTIME 
        int tempoC = rand() % MAXTIME;                      // DE 0 ATÉ 4 
        process->tempoChegada = tempoC;                     //TEMPO EM QUE O PROCESSO CHEGA A FILA
        process->tp = rTime;                                            //tempo de execução que o processo precisa
        process->pid = i;
        //if (i==0){strcpy(process->status,"Execucao");}
        //strcpy(process->status,"Espera");

        P[i] = process;
        
    }
}

//Escalonador Round-Robin (TODO: com feedback)
int Escalonador(Processo **P, Processo **ListaConcluidos){
    int t = 0;
    // TODO: alocar lista encadeada
    Lista *listaP = (Lista *) malloc(sizeof(Lista));      //alocação da lista
    listaP->inicio = NULL;
    while(ProcessExe){
        // 1 -checar se alguém chegou na fila (algum processo foi iniciado)
        for (int i=0; i<MAXPROCESSES; i++) {
            if (P[i]->tempoChegada == t) {
                if(listaP->inicio == NULL){
                    inserirInicio(listaP,P[i]);
                }
                else inserirFim(listaP,P[i]);
            }
            
        }


        // 2- ve se o processo atual já terminou ou atingiu o quantum
        if((t % QUANTUM == 0 && t != 0)|| (listaP->inicio->processo->tp <=0)&& listaP->inicio!=NULL){//passou o tempo de ciclo
            if(listaP->inicio->processo->tp <=0){ //processo concluido
                processoConcluido(listaP->inicio->processo, ListaConcluidos);
            }
            else{
                inserirFim(listaP,listaP->inicio->processo);
            }
            inserirInicio(listaP, listaP->inicio->proximo->processo); // acho que vai dar merda
            
            //print dos processos
            //for (int i=0; i<ProcessExe; i++) {
            //    printProcess(ListaConcluidos[i]);
            //}
        }
        // 3 - processo atual vai pra fila, se conclui ou continua em execucao
        // 4 - se processo atual foi pra fila pega o outro
        //strcpy(listaP->inicio->processo->status,"executando");
        listaP->inicio->processo->tp--; //diminui 1 do tempo do processo        


        t++;
    }
    
    return 0;
}





/**
 * Função para checar erros no malloc
 * 
 * @param size tamanho da realocação a ser feita
 **/
void *errorcheck_malloc(int size) {
    void *ptr = malloc(size);
    if(ptr == NULL) {
        printf("--ERRO: malloc()\n");
        exit(-1);
    }
    return ptr;
}



int main(){
    // inicialização
    srand(time(NULL));

    //fila de execução
    int fila[MAXPROCESSES];

    //Processo ListaConcluidos[MAXPROCESSES];
    Processo **ListaConcluidos = malloc(sizeof(Processo*) * MAXPROCESSES);

    Processo **processes = malloc(sizeof(Processo*) * MAXPROCESSES);

    createProcesses(processes);
    for (int i=0; i<MAXPROCESSES; i++) {
        printProcess(processes[i]);
    }

    printf("Vamos comecar o escalonador\n");
    Escalonador(processes, ListaConcluidos);

    return 0;
}