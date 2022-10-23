#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXPROCESSES 5      //Numero maximo de processos
#define MAXTIME 5          //tempo maximo que o processo pode ter
#define DISCTIME 2
#define TAPETIME 3
#define PRINTERTIME 4


typedef enum {
  WAITING, READY, RUNNING, BLOCKED, FINISHED
} STATUS;

//Tipagem de IO
typedef enum {
    NONE, DISC, TAPE, PRINTER
} IO_TYPE;

//
typedef struct InOut {
    IO_TYPE ioType;
    int ioArrivalTime;
    int ioExecTime;
} InOut;

//Estrutura do Processo
typedef struct Process
{
    int pArrivalTime;          //momento que o processo chega na fila
    STATUS status;           //status
    int pId;                   //process identification
    int pRemainingTime;
    int pExecTime;             //tempo de processo
    InOut pIo;
    struct Process *next;     //proximo processo na fila
}Process;

struct Process *blockedList[MAXPROCESSES];
//struct Process *blockedList[MAXPROCESSES] = { NULL };

void changeStatus(Process *process, STATUS status){
    process->status = status;
}


struct Process* insertAtEnd(Process * last, Process* newProcess) {
  
  // se a lista esta vazia o processo irá apontar pra ele mesmo
  if (last == NULL) {
    last = newProcess;
    newProcess -> next = last;
    //changeStatus(newProcess, RUNNING);
    return last;
  }

  //changeStatus(newProcess, WAITING);

  // novo processo aponta para a cabeca da lista já que ele agora será o ultimo
  newProcess->next = last->next;

  // o antigo ultimo processo aponta para o novo processo
  last->next = newProcess;

  // faz o novo processo ser o ultimno
  last = newProcess;

  return last;

  
}

void changeHead(struct Process** last) {
  changeStatus((*last)->next, READY);
  // faz a cabeca ser o ultimo processo
  *last = (*last)->next;
  changeStatus((*last)->next, RUNNING);
}

void removeHead(struct Process** last){
  if (*last == NULL) return;

  changeStatus((*last)->next, BLOCKED);

  // checa se a lista tem apenas
  if ((*last)->next == *last) {
    //free(*last);
    *last = NULL;
    return;
  }

  struct Process *d;

  d = (*last)->next;
  (*last)->next = d->next;

}


void deleteHead(struct Process** last) {
  // checa se a lista esta vazia, ou seja não tem nada a ser deletado
  if (*last == NULL) return;

  changeStatus((*last)->next, FINISHED);

  // checa se a lista tem apenas
  if ((*last)->next == *last) {
    free(*last);
    *last = NULL;
    return;
  }

  struct Process *d;

  d = (*last)->next;
  (*last)->next = d->next;
  changeStatus((*last)->next, RUNNING);
  free(d);

}


//imprime os dados do processo na tela
void printProcess(Process *P){
    printf("Processo: %d    Tempo Restante: %d      Tempo Chegada: %d     Tipo IO: %d     Chegada IO: %d    Proximo: %d\n", P->pId, P->pExecTime, P->pArrivalTime, P->pIo.ioType, P->pIo.ioArrivalTime, P->next->pId);
}

void createProcesses (Process **pList) {
    for (int i=0; i<MAXPROCESSES; i++) {
        Process *process = (Process *) malloc(sizeof(Process));      //alocação do processo
        int randomArrivalTime = rand() % MAXTIME;                      // DE 0 ATÉ 4 
        int randomExecTime = rand() % MAXTIME + 1;                                 //valor aleatorio de 1 até MAXTIME 
        int randomIoType;                                  // valor que indicará a tipagem do IO
        
        process->pId = i;       
        process->status = WAITING;
        process->pArrivalTime = randomArrivalTime;                                 //TEMPO EM QUE O PROCESSO CHEGA A FILA
        process->pExecTime = randomExecTime;                                       //tempo de execução que o processo precisa
        process->pRemainingTime = process->pExecTime;                                       //tempo de execução que resta do processo
        
        if (process->pExecTime == 1) {
          randomIoType = 0; // se o processo tem tempo de execução 1 n tem tempo de fzr um IO
        }
        else {
          randomIoType = rand() % 4;    //valor aleatorio de 0 até 3 
        }

        switch (randomIoType) {
          //caso de nenhum io (None)
          case 0:
            process->pIo.ioType = NONE;
            break;
          //caso de disco (Disc)
          case 1:
            process->pIo.ioType = DISC;
            process->pIo.ioExecTime = DISCTIME;
            break;
          //caso de fita (Tape)
          case 2:
            process->pIo.ioType = TAPE;
            process->pIo.ioExecTime = TAPETIME;
            break;
          //caso de impressora (Printer)
          case 3:
            process->pIo.ioType = PRINTER;
            process->pIo.ioExecTime = PRINTERTIME;
            break;
        }

        
        // se o processo tiver um IO, escolhe um tempo de chegada aleatório respeitando o limite de tempo do processo
        if (process->pIo.ioType != NONE){
          process->pIo.ioArrivalTime = (rand() % (process->pExecTime - 1)) + 1 ;
        }
    
        pList[i] = process;
        
    }
}

void traverseBlockedList() {
  printf("BLOQUEADOS - ");
  for (int i = 0; i<MAXPROCESSES; i++) {
    if ((blockedList)[i] == NULL){
      printf("PID: NULL ");
    }
    else {
      printf("PID: %d ", (blockedList)[i]->pId);
    }
    
  }
  printf("\n");
}

void unblockHead(Process **last ,int pid){
  //changeStatus(blockedList[pid], WAITING);
  
  (*last)  = insertAtEnd( (*last), blockedList[pid]); //coloca o processo no final da lista
  blockedList[pid] = NULL; //tirar de bloqueados
  traverseBlockedList();

  return;
}

void blockProcess(struct Process** last){
  //changeStatus((*last)->next, BLOCKED);
  printf("Processo %d entrou na lista de bloqueados por causa de um IO.\n", (*last)->next->pId);
  blockedList[(*last)->next->pId] = (*last)->next;
  
  
  removeHead(last);
  if((*last) != NULL){
    changeStatus((*last)->next, RUNNING);
  }
  traverseBlockedList();
}

void decrementBlockedProcesses () {
  for (int i = 0; i<MAXPROCESSES; i++) {
    if( blockedList[i] != NULL) {
      int temp = blockedList[i]->pIo.ioExecTime -1;
      blockedList[i]->pIo.ioExecTime = temp;
    }
  }
}


void checkBlockedProcesses (struct Process** lastLowPriority, struct Process** lastHighPriority) {
  for (int i = 0; i<MAXPROCESSES; i++) {
    if(blockedList[i] != NULL && blockedList[i]->pIo.ioExecTime == 0) {
      switch (blockedList[i]->pIo.ioType){
        case DISC:
          if (lastHighPriority == NULL && lastLowPriority == NULL) {
            unblockHead(lastLowPriority, i);
            changeStatus(*lastLowPriority, RUNNING);
          }
          else {
            unblockHead(lastLowPriority, i);
            changeStatus(*lastLowPriority, READY);
          }
          
          break;
        case TAPE:
        case PRINTER:
          if(lastHighPriority == NULL){
            unblockHead(lastHighPriority, i);
            changeStatus(*lastHighPriority, RUNNING);
          }
          else {
            unblockHead(lastHighPriority, i);
            changeStatus(*lastHighPriority, READY);
          }
          break;
        default:
        break;
      }
   
    }
  }
}


// imprime a lista de espera atualizada
void traverse(struct Process* last) {
  struct Process* p;

  // checa se a lista está vazia, se estiver nem precisa imprimir
  if (last == NULL) {
  printf("A lista esta vazia.\n");
  return;
  }

  // armazena o comeco da lista, por onde a impressão vai comecar 
  p = last->next;

  do {
    printf("PID: %d ", p->pId); // imprime o pid do processo
    p = p->next; // vai para o próximo elemento da lista
  } while (p != last->next); // até chegar na cabeca de novo

  printf("\n");
}