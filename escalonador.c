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
  int t_quantum = 0;            // tempo do quantum do processo em execução




  printf("%d\n", t);
  while(ProcessExe){
    
    //checa se o processo de alta prioridade executado já terminou seu tempo restante
    if(lastHighPriority != NULL){
      if((lastHighPriority->next->pRemainingTime == 0)) { // 
        printf("Processo %d acabou.\n", lastHighPriority->next->pId);
        deleteHead(&lastHighPriority); // deleta da lista de espera o processo que acabou de terminar
        
        // printa a lista de alta prioridade
        printf("HIGH - ");
        traverse(lastHighPriority);
        
        ProcessExe--; // decrementa o total de processos a serem concluidos
        
        t_quantum = 0;  // reinicia o quantum para o novo processo que entrará em execução
      }
    }
    //checa se o processo de baixa prioridade executado já terminou seu tempo restante
    if(lastLowPriority != NULL) {
      // checa se o processo em execução terminou
      if((lastLowPriority->next->pRemainingTime == 0)) {
        printf("Processo %d acabou.\n", lastLowPriority->next->pId);
        deleteHead(&lastLowPriority); // deleta da lista de espera o processo que acabou de terminar

        // printa a lista de baixa prioridade
        printf("LOW - ");
        traverse(lastLowPriority);
        
        ProcessExe--; // decrementa o total de processos a serem concluidos
        t_quantum = 0;  // reinicia o quantum para o novo processo que entrará em execução
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
        printf("HIGH - ");
        traverse(lastHighPriority);
        printf("Processo %d entrou na lista de alta prioridade.\n", P[i]->pId);
        // imprime info do processo que acabou de chegar
        printProcess(P[i]);
      } 
    }

    // checar se há processos bloqueados e se eles precisam ser desbloqueados
    checkBlockedProcesses(&lastLowPriority, &lastHighPriority); // n sei se é dps da checagem de quantum
    
    // decrementa processos bloqueados
    decrementBlockedProcesses();

    // checa se o processo High Priority em execução terminou
    if(lastHighPriority != NULL){
      // checa se o processo High Priority em execução atingiu o tempo máximo de quantum
      if(t_quantum == QUANTUM) {
        printf("Processo %d voltou pra espera.\n", lastHighPriority->next->pId);
        // move o processo para o final da fila de espera
        changePriority(&lastHighPriority, &lastLowPriority);
        printf("HIGH - ");
        traverse(lastHighPriority);
        printf("LOW - ");
        traverse(lastLowPriority);
        t_quantum = 0;        // reinicia o quantum para o novo processo que entrará em execução
        
      }
      else  // incrementa o tempo do quantum do processo em execução
      t_quantum++;
    }

    // checa se o processo Low Priority em execução terminou
    else if(lastLowPriority != NULL) {
      // checa se o processo Low Priority em execução atingiu o tempo máximo de quantum
      if(t_quantum == QUANTUM) {
        printf("Processo %d voltou pra espera.\n", lastLowPriority->next->pId);
        // move o processo para o final da fila de espera
        changeHead(&lastLowPriority);
        printf("LOW - ");
        traverse(lastLowPriority);
        // reinicia o quantum para o novo processo que entrará em execução
        t_quantum = 0;
      }
      else
      // incrementa o tempo do quantum do processo em execução

      t_quantum++;
    }
    

    // incrementa o contador de tempo e printa ele na tela
    printf("%d\n", ++t);


    // checagem de alta prioridade 
    if(lastHighPriority != NULL && lastHighPriority->next->status == RUNNING) {
      // diminui o tempo restante de execução do processo atual
      (lastHighPriority->next->pRemainingTime)--;
      printf("Processo %d diminuiu para %d.\n", lastHighPriority->next->pId, lastHighPriority->next->pRemainingTime);

      // checa se algum IO chegou (foi pedido algum IO)
      if(lastHighPriority->next->pIo.ioArrivalTime == (lastHighPriority->next->pExecTime - lastHighPriority->next->pRemainingTime)) {
        blockProcess(&lastHighPriority);
        printf("HIGH - ");
        traverse(lastHighPriority);
        // reinicia o quantum para o novo processo que entrará em execução
        t_quantum = 0;
      }
    }

    // checagem de baixa prioridade
    else if(lastLowPriority != NULL)  {     // diminui o tempo restante de execução do processo atual
      changeStatus(lastLowPriority->next ,RUNNING);
      (lastLowPriority->next->pRemainingTime)--;
      printf("Processo %d diminuiu para %d.\n", lastLowPriority->next->pId, lastLowPriority->next->pRemainingTime);

      // checa se algum IO chegou (foi pedido algum IO)
      if(lastLowPriority->next->pIo.ioArrivalTime == (lastLowPriority->next->pExecTime - lastLowPriority->next->pRemainingTime)) {
        blockProcess(&lastLowPriority);
        printf("LOW - ");
        traverse(lastLowPriority);
        // reinicia o quantum para o novo processo que entrará em execução
        t_quantum = 0;
      }
    }
    
      
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
