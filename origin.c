#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// #define M 2 // numero de recursos
// #define N 2 // numero de processos

#define NumDeRecursos 2 // numero de recursos
#define NumDeProcess 2 // numero de processos

// Variáveis globais
int i = 0;
int j = 0;
pthread_mutex_t mutex;

// Matrizes e vetores do algoritmo
int avail[NumDeRecursos];
int allocmatrix[NumDeProcess][NumDeRecursos];
int MaxMatrix[NumDeProcess][NumDeRecursos];
int NeedMatrix[NumDeProcess][NumDeRecursos];

// Thread
void *procs(void *procsID)
{
  int pID = *(int *)procsID;
  int c = 0;
  while (c < 2)
  {
    //generate random requests
    sleep(1);
    int request[NumDeRecursos];
    pthread_mutex_lock(&mutex);

    //initialize requestVector using rand()
    for (i = 0; i < NumDeRecursos; i++)
    {
      if (NeedMatrix[pID][i] != 0)
      {
        request[i] = NeedMatrix[pID][i];
        // request[i] = rand() % NeedMatrix[pID][i];
      }
      else
      {
        request[i] = 0;
      }
    }

    printf("\nCustomer %d Requesting Resources:\n", pID);
    printReqOrRelVector(request);

    getRes(pID, request);

    pthread_mutex_unlock(&mutex);

    //release random number of resources
    sleep(1);
    int releaseVector[NumDeRecursos];
    pthread_mutex_lock(&mutex);

    //initialize releaseVector using rand()
    for (i = 0; i < NumDeRecursos; i++)
    {
      if (allocmatrix[pID][i] != 0)
      {
        releaseVector[i] = rand() % allocmatrix[pID][i];
      }
      else
      {
        releaseVector[i] = 0;
      }
    }

    printReqOrRelVector(releaseVector);
    relRes(pID, releaseVector);
    pthread_mutex_unlock(&mutex);

    c++;
  }
}

// allocate resources for a process
int getRes(int pID, int request[])
{
  // whether request number of resources is greater than needed
  if (casegreaterthanneed(pID, request) == -1)
  {
    printf("Number of requested Resources is more than Needed.\n");
    return -1;
  }
  printf("Resources are being allocated...\n");

  if (enoughtoalloccase(request) == -1)
  {
    printf("Not enough Resources.\n");
    return -1;
  }

  //pretend allocated
  for (i = 0; i < NumDeRecursos; ++i)
  {
    NeedMatrix[pID][i] -= request[i];
    allocmatrix[pID][i] += request[i];
    avail[i] -= request[i];
  }
  printf("Checking is the state is safe...\n");

  if (safemodecase() == 0)
  {
    printf("\nx========================x\n|Safe Mode. Resources Allocated|\nx=========================x\n");

    // exit(1);
    return 0;
  }
  else
  {
    // Retirando alterações
    for (i = 0; i < NumDeRecursos; ++i)
    {
      NeedMatrix[pID][i] += request[i];
      allocmatrix[pID][i] -= request[i];
      avail[i] += request[i];
    }
    printf("\nx=====================x\n|State is not safe.          |\nx=====================x\n");
    // exit(1);

    return -1;
  }
}

int relRes(int pID, int releaseVector[])
{

  if (caseengoughtorel(pID, releaseVector) == -1)
  {
    printf("Not enought Resources.\n");
    return -1;
  }
  for (i = 0; i < NumDeRecursos; i++)
  {
    allocmatrix[pID][i] -= releaseVector[i];
    NeedMatrix[pID][i] += releaseVector[i];
    avail[i] += releaseVector[i];
  }
  printf("Released.\nMetrix Available:\n");
  showavail();
  printf("Metrix Allocated:\n");
  Showalloc();
  printf("Metrix Need:\n");
  ShowNeed();
  return 0;
}

// Funções de verificação
int caseengoughtorel(int pID, int releaseVector[])
{
  for (i = 0; i < NumDeRecursos; ++i)
  {
    if (releaseVector[i] <= allocmatrix[pID][i])
    {
      continue;
    }
    else
    {
      return -1;
    }
  }
  return 0;
}

int casegreaterthanneed(int pID, int request[])
{

  for (i = 0; i < NumDeRecursos; ++i)
  {
    if (request[i] <= NeedMatrix[pID][i])
    {
      continue;
    }
    else
    {
      return -1;
    }
  }
  return 0;
}

int enoughtoalloccase(int request[])
{

  for (i = 0; i < NumDeRecursos; ++i)
  {
    if (request[i] <= avail[i])
    {
      continue;
    }
    else
    {
      return -1;
    }
  }
  return 0;
}

// Funções de output
void ShowNeed()
{
  for (i = 0; i < NumDeProcess; ++i)
  {
    printf("{ ");
    for (j = 0; j < NumDeRecursos; ++j)
    {
      printf("%d, ", NeedMatrix[i][j]);
    }
    printf("}\n");
  }
  printf("\n");
  return;
}

void Showalloc()
{
  for (i = 0; i < NumDeProcess; ++i)
  {
    printf("{ ");
    for (j = 0; j < NumDeRecursos; ++j)
    {
      printf("%d, ", allocmatrix[i][j]);
    }
    printf("}\n");
  }
  printf("\n");
  return;
}

void showavail()
{
  for (i = 0; i < NumDeRecursos; ++i)
  {
    printf("%d, ", avail[i]);
  }
  printf("\n");
  printf("\n");
  return;
}

void printReqOrRelVector(int vec[])
{
  printf("ReqOrRelVector: [");
  for (i = 0; i < NumDeRecursos; ++i)
  {
    printf("%d, ", vec[i]);
  }
  printf("]\n");
  return;
}

// Função de estudo de estado
// Checa se uma sequência segura existe caso ocorra uma alocaçao de recursos.
int safemodecase()
{
  int finish[NumDeProcess] = {0};
  int work[NumDeRecursos]; // Vetor temporário de recursos

  for (i = 0; i < NumDeRecursos; i++)
  {
    work[i] = avail[i];
  }

  int k;
  for (i = 0; i < NumDeProcess; i++)
  {
    if (finish[i] == 0)
    {
      for (j = 0; j < NumDeRecursos; j++)
      {
        if (NeedMatrix[i][j] <= work[j])
        {

          if (j == NumDeRecursos - 1)
          {
            finish[i] = 1;
            for (k = 0; k < NumDeRecursos; ++k)
            {
              work[k] += allocmatrix[i][k];
            }

            i = -1;
            break;
          }
          else
          {
            continue;
          }
        }
        else
        {
          break;
        }
      }
    }
    else
    {
      continue;
    }
  }

  for (i = 0; i < NumDeProcess; i++)
  {
    if (finish[i] == 0)
    {

      return -1;
    }
    else
    {
      continue;
    }
  }

  return 0;
}

int main()
{
  printf("Insira o vetor de recursos disponíveis\n");

  for (i = 0; i < NumDeRecursos; i++)
  {

    scanf("%d", &avail[i]);
  }
  printf("Insira a matriz de recursos já alocados\n");
  for (i = 0; i < NumDeProcess; i++)
  {

    for (j = 0; j < NumDeRecursos; j++)
    {

      scanf("%d", &allocmatrix[i][j]);
    }
  }
  printf("Insira a matriz máxima\n");
  for (i = 0; i < NumDeProcess; i++)
  {

    for (j = 0; j < NumDeRecursos; j++)
    {

      scanf("%d", &MaxMatrix[i][j]);
    }
  }

  // Definindo valores da NeedMatrix
  for (i = 0; i < NumDeProcess; ++i)
  {
    for (j = 0; j < NumDeRecursos; ++j)
    {
      NeedMatrix[i][j] = MaxMatrix[i][j] - allocmatrix[i][j];
    }
  }

  printf("Vetor de recursos disponíveis:\n");
  showavail();

  printf("Matriz de recursos alocados:\n");
  Showalloc();

  printf("Matriz máxima:\n");
  ShowNeed();

  pthread_mutex_init(&mutex, NULL);
  pthread_attr_t attrDefault;
  pthread_attr_init(&attrDefault);
  pthread_t *tid = malloc(sizeof(pthread_t) * NumDeProcess);

  int *pid = malloc(sizeof(int) * NumDeProcess); // process ID

  //initialize pid and create pthreads
  for (i = 0; i < NumDeProcess; i++)
  {
    *(pid + i) = i;
    pthread_create((tid + i), &attrDefault, procs, (pid + i));
  }

  //join threads
  for (i = 0; i < NumDeProcess; i++)
  {
    pthread_join(*(tid + i), NULL);
  }
  return 0;
}
