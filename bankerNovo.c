#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

int nResources, nProcesses;
int *resources;  //Ponteiro pros recursos
int **allocated; //ponteiro do ponteiro pros recursos
int **maxRequired;
int **need;
int *safeSeq;
int nProcessRan = 0;

pthread_mutex_t lockResources;
pthread_cond_t condition;

// get safe sequence is there is one else return false
bool getSafeSeq();
// process function
void *processCode(void *);

int main(int argc, char **argv)
{
  srand(time(NULL));

  printf("\nDigite o número de processos "); //
  scanf("%d", &nProcesses);

  printf("\nDigite o número de recursos ");
  scanf("%d", &nResources);
  //aloca memória pros recursos terem um endereço
  //Estrutura do malloc
  //Ponteiro = (tipo do parse)malloc()
  resources = (int *)malloc(nResources * sizeof(*resources)); //resourcees = PONTEIRO

  printf("\nInsira o vetor de recursos disponíveis (R1 R2 ...)? "); //nResources = INT
  for (int i = 0; i < nResources; i++)
  {
    scanf("%d", &resources[i]);
  }
  allocated = (int **)malloc(nProcesses * sizeof(*allocated));

  for (int i = 0; i < nProcesses; i++)
  {
    allocated[i] = (int *)malloc(nResources * sizeof(**allocated));
  }
  maxRequired = (int **)malloc(nProcesses * sizeof(*maxRequired));

  for (int i = 0; i < nProcesses; i++)
  {
    maxRequired[i] = (int *)malloc(nResources * sizeof(**maxRequired));
  }
  // allocated
  printf("\n");
  for (int i = 0; i < nProcesses; i++)
  {
    printf("\nNúmero de recursos alocados ao processo %d (R1 R2 ...) ", i + 1);
    for (int j = 0; j < nResources; j++)
    {
      scanf("%d", &allocated[i][j]);
    }
  }
  printf("\n");

  // maximum required resources
  for (int i = 0; i < nProcesses; i++)
  {
    printf("\nMáximo de recursos solicitados pelo processo %d (R1 R2 ...) ", i + 1);
    for (int j = 0; j < nResources; j++)
    {
      scanf("%d", &maxRequired[i][j]);
    }
  }
  printf("\n");

  // Criando NeedMatriz
  need = (int **)malloc(nProcesses * sizeof(*need));
  for (int i = 0; i < nProcesses; i++)
  {
    need[i] = (int *)malloc(nResources * sizeof(**need));
  }

  for (int i = 0; i < nProcesses; i++)
  {
    for (int j = 0; j < nResources; j++)
    {
      need[i][j] = maxRequired[i][j] - allocated[i][j];
    }
  }

  // Gerando sequência segura
  safeSeq = (int *)malloc(nProcesses * sizeof(*safeSeq));
  for (int i = 0; i < nProcesses; i++)
  {
    safeSeq[i] = -1;
  }

  if (!getSafeSeq())
  {
    printf("\nEstado inseguro!\n\n");
    exit(-1);
  }

  printf("\n\nSequencia segura encontrada : ");
  for (int i = 0; i < nProcesses; i++)
  {
    printf("%-3d", safeSeq[i] + 1);
  }

  printf("\nExecutando processos...\n\n");
  sleep(1);

  // setup das threads
  pthread_t processes[nProcesses];
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  int processNumber[nProcesses];
  for (int i = 0; i < nProcesses; i++)
  {
    processNumber[i] = i;
  }

  for (int i = 0; i < nProcesses; i++)
  {
    pthread_create(&processes[i], &attr, processCode, (void *)(&processNumber[i]));
  }

  for (int i = 0; i < nProcesses; i++)
  {
    pthread_join(processes[i], NULL);
  }

  printf("\nTodos os processos foram finalizados\n");

  // liberação dos recursos utilizados
  free(resources);
  for (int i = 0; i < nProcesses; i++)
  {
    free(allocated[i]);
    free(maxRequired[i]);
    free(need[i]);
  }
  free(allocated);
  free(maxRequired);
  free(need);
  free(safeSeq);
}

bool getSafeSeq()
{
  // get safe sequence
  int tempRes[nResources];
  for (int i = 0; i < nResources; i++)
  {
    tempRes[i] = resources[i];
  }

  bool finished[nProcesses];
  for (int i = 0; i < nProcesses; i++)
  {
    finished[i] = false;
  }

  int nfinished = 0;
  while (nfinished < nProcesses)
  {
    bool safe = false;

    for (int i = 0; i < nProcesses; i++)
    {
      if (!finished[i])
      {
        bool possible = true;

        for (int j = 0; j < nResources; j++)
        {
          if (need[i][j] > tempRes[j])
          {
            possible = false;
            break;
          }
        }
        if (possible)
        {
          for (int j = 0; j < nResources; j++)
          {
            tempRes[j] += allocated[i][j];
          }
          safeSeq[nfinished] = i;
          finished[i] = true;
          ++nfinished;
          safe = true;
        }
      }
    }

    if (!safe)
    {
      for (int k = 0; k < nProcesses; k++)
      {
        safeSeq[k] = -1;
      }
      return false; // no safe sequence found
    }
  }
  return true; // safe sequence found
}

// process code
void *processCode(void *arg)
{
  int p = *((int *)arg);

  // lock resources
  pthread_mutex_lock(&lockResources);

  // condition check
  while (p != safeSeq[nProcessRan])
  {
    pthread_cond_wait(&condition, &lockResources);
  }

  // process
  printf("\n--> Processos %d", p + 1);
  printf("\n\tRecursos alocados : ");
  for (int i = 0; i < nResources; i++)
  {
    printf("%3d", allocated[p][i]);
  }

  printf("\n\tRecursos necessários    : ");
  for (int i = 0; i < nResources; i++)
  {
    printf("%3d", need[p][i]);
  }

  printf("\n\tVetor de recursos disponíveis : ");
  for (int i = 0; i < nResources; i++)
  {
    printf("%3d", resources[i]);
  }

  printf("\n");
  sleep(1);

  printf("\tRecursos alocados!");
  printf("\n");
  sleep(1);

  printf("\tProcess Code Running...");
  printf("\n");

  sleep(rand() % 3 + 2); // process code
  printf("\tProcess Code Completed...");
  printf("\n");
  sleep(1);
  printf("\tDesalocando recursos...");
  printf("\n");
  sleep(1);
  printf("\tRecursos liberados!");
  for (int i = 0; i < nResources; i++)
  {
    resources[i] += allocated[p][i];
  }

  printf("\n\tDisponível : ");
  for (int i = 0; i < nResources; i++)
  {
    printf("%3d", resources[i]);
  }
  printf("\n\n");

  sleep(1);

  // condition broadcast
  nProcessRan++;
  pthread_cond_broadcast(&condition);
  pthread_mutex_unlock(&lockResources);
  pthread_exit(NULL);
}