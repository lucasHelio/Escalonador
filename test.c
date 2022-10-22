#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "processHelper.h"

#define QUANTUM 2           //tempo de ciclo
#define MAXPROCESSES 5      //Numero maximo de processos
#define MAXTIME 5          //tempo maximo que o processo pode ter


int ProcessExe = MAXPROCESSES;           //Processos a serem executados


//Escalonador Round-Robin (TODO: com feedback)
int Escalonador(Process **P){
  int t = 0;                    // tempo geral do escalonador
  struct Process *last = NULL; // ultimo elemento da lista (encadeada circular) de espera
  //struct Process *blockedList[MAXPROCESSES] = { NULL };
  int t_quantum = 0;            // tempo do quantum do processo em execução

  while(ProcessExe){
    printf("%d\n", t);

    if(last != NULL) {
      // diminui o tempo restante de execução do processo atual
      (last->next->pRemainingTime)--;
      printf("Processo %d diminuiu para %d.\n", last->next->pId, last->next->pRemainingTime);

      decrementBlockedProcesses();
      printf("passou do decrement\n");
      // checa se algum IO chegou (foi pedido algum IO)
      if(last->next->pIo.ioArrivalTime == (last->next->pExecTime - last->next->pRemainingTime)) {
        blockProcess(&last);
        //printf("Processo %d voltou pra espera por causa de um IO.\n", last->next->pId);
        traverse(last);
        // reinicia o quantum para o novo processo que entrará em execução
        t_quantum = 0;
      }

      
    }

    // checa se alguém chegou na fila (algum processo foi iniciado)
    for (int i=0; i<MAXPROCESSES; i++) {
      if (P[i]->pArrivalTime == t) {
        // insere o processo que acabou de chegar no final da fila
        last = insertAtEnd(last,P[i]);

        // imprime a lista de espera atualizada
        traverse(last);
        printf("Processo %d entrou na lista de espera.\n", P[i]->pId);
        // imprime info do processo que acabou de chegar
        printProcess(P[i]);
      } 
    }
    
    checkBlockedProcesses(&last); // n sei se é dps da checagem de quantum

    if(last != NULL) {
      // checa se o processo em execução terminou
      if((last->next->pRemainingTime == 0)) {
        printf("Processo %d acabou.\n", last->next->pId);
        // deleta da lista de espera o processo que acabou de terminar
        deleteHead(&last);
        
        traverse(last);
        // decrementa o total de processos a serem concluidos
        ProcessExe--;
        // reinicia o quantum para o novo processo que entrará em execução
        t_quantum = 0;
      }

      

      // checa se o processo em execução atingiu o tempo máximo de quantum
      if(t_quantum == QUANTUM) {
        printf("Processo %d voltou pra espera.\n", last->next->pId);
        // move o processo para o final da fila de espera
        changeHead(&last);
        traverse(last);
        // reinicia o quantum para o novo processo que entrará em execução
        t_quantum = 0;
      }

      // incrementa o tempo do quantum do processo em execução
      t_quantum++;
    }
    //if(t>=30){
    //  break;
    //}
    // incrementa o tempo geral do escalonador
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

    Process **processes = malloc(sizeof(Process*) * MAXPROCESSES);

    createProcesses(processes);
    
    Escalonador(processes);

    return 0;
}