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
  struct Process *lastHighPriority = NULL; // ultimo elemento da lista (encadeada circular) de espera
  struct Process *lastLowPriority = NULL; // ultimo elemento da lista (encadeada circular) de espera
  //struct Process *blockedList[MAXPROCESSES] = { NULL };
  int t_quantum = 0;            // tempo do quantum do processo em execução

  while(ProcessExe){
    printf("%d\n", t);

    decrementBlockedProcesses();
    
    // checagem de alta prioridade // não estamos trocando de processo a cada 2 ciclos
    if(lastHighPriority != NULL && lastHighPriority->next->status == RUNNING) {
      // diminui o tempo restante de execução do processo atual
      (lastHighPriority->next->pRemainingTime)--;
      printf("Processo %d diminuiu para %d.\n", lastHighPriority->next->pId, lastHighPriority->next->pRemainingTime);

      // checa se algum IO chegou (foi pedido algum IO)
      if(lastHighPriority->next->pIo.ioArrivalTime == (lastHighPriority->next->pExecTime - lastHighPriority->next->pRemainingTime)) {
        blockProcess(&lastHighPriority);
        
        traverse(lastHighPriority);
        // reinicia o quantum para o novo processo que entrará em execução
        t_quantum = 0;
      }
    }
    // checagem de baixa prioridade
    else if(lastLowPriority != NULL)  {     // diminui o tempo restante de execução do processo atual
      changeStatus(lastLowPriority ,RUNNING);
      (lastLowPriority->next->pRemainingTime)--;
      printf("Processo %d diminuiu para %d.\n", lastLowPriority->next->pId, lastLowPriority->next->pRemainingTime);

      // checa se algum IO chegou (foi pedido algum IO)
      if(lastLowPriority->next->pIo.ioArrivalTime == (lastLowPriority->next->pExecTime - lastLowPriority->next->pRemainingTime)) {
        blockProcess(&lastLowPriority);
        
        traverse(lastLowPriority);
        // reinicia o quantum para o novo processo que entrará em execução
        t_quantum = 0;
      }
    }

    // checa se alguém chegou na fila (algum processo foi iniciado)
    for (int i=0; i<MAXPROCESSES; i++) {
      if (P[i]->pArrivalTime == t) {
        // insere o processo que acabou de chegar no final da fila
        if(lastHighPriority == NULL){
          lastHighPriority = insertAtEnd(lastHighPriority,P[i]);
          changeStatus(lastHighPriority, RUNNING);
        }
        else {
          lastHighPriority = insertAtEnd(lastHighPriority,P[i]);
          changeStatus(lastHighPriority, READY);
        }

        // imprime a lista de espera atualizada
        traverse(lastHighPriority);
        printf("Processo %d entrou na lista de alta prioridade.\n", P[i]->pId);
        // imprime info do processo que acabou de chegar
        printProcess(P[i]);
      } 
    }
    
    checkBlockedProcesses(&lastLowPriority, &lastHighPriority); // n sei se é dps da checagem de quantum

    if(lastHighPriority != NULL){
        // checa se o processo em execução terminou
      if((lastHighPriority->next->pRemainingTime == 0)) { // 
        printf("Processo %d acabou.\n", lastHighPriority->next->pId);
        // deleta da lista de espera o processo que acabou de terminar
        deleteHead(&lastHighPriority); // INFO QUE VAI FALTAR NO FINAL
        
        traverse(lastHighPriority);
        // decrementa o total de processos a serem concluidos
        ProcessExe--;
        // reinicia o quantum para o novo processo que entrará em execução
        t_quantum = 0;
      }
      

      // checa se o processo em execução atingiu o tempo máximo de quantum
      if(t_quantum == QUANTUM) {
        printf("Processo %d voltou pra espera.\n", lastHighPriority->next->pId);
        // move o processo para o final da fila de espera
        changeHead(&lastHighPriority);
        traverse(lastHighPriority);
        // reinicia o quantum para o novo processo que entrará em execução
        t_quantum = 0;
      }

      // incrementa o tempo do quantum do processo em execução
      t_quantum++;
    }
    if(lastLowPriority != NULL) {
      // checa se o processo em execução terminou
      if((lastLowPriority->next->pRemainingTime == 0)) {
        printf("Processo %d acabou.\n", lastLowPriority->next->pId);
        // deleta da lista de espera o processo que acabou de terminar
        deleteHead(&lastLowPriority);
        
        traverse(lastLowPriority);
        // decrementa o total de processos a serem concluidos
        ProcessExe--;
        // reinicia o quantum para o novo processo que entrará em execução
        t_quantum = 0;
      }
      

      // checa se o processo em execução atingiu o tempo máximo de quantum
      if(t_quantum == QUANTUM) {
        printf("Processo %d voltou pra espera.\n", lastLowPriority->next->pId);
        // move o processo para o final da fila de espera
        changeHead(&lastLowPriority);
        traverse(lastLowPriority);
        // reinicia o quantum para o novo processo que entrará em execução
        t_quantum = 0;
      }

      // incrementa o tempo do quantum do processo em execução
      t_quantum++;
    }
    
    
    if(t>=50){
      break;
    }
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
    //int fila[MAXPROCESSES];

    Process **processes = malloc(sizeof(Process*) * MAXPROCESSES);

    createProcesses(processes);
    
    Escalonador(processes);

    return 0;
}