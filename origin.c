#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define M 2 // numero de recursos
#define N 2 // numero de processos

// Variáveis globais
int i = 0;
int j = 0;
pthread_mutex_t mutex;

// Matrizes e vetores do algoritmo
int avail[M];
int allocmatrix[N][M];
int MaxMatrix[N][M];
int NeedMatrix[N][M];

// Thread
void *procs(void *procsID)
{
  int pID = *(int *)procsID;
  int c = 0;
  while (c < 3)
  {
    //generate random requests
    sleep(1);
    int request[M];
    pthread_mutex_lock(&mutex);

    //NeedMatrix vector ko initialize karna
    for (i = 0; i < M; i++)
    {
      if (NeedMatrix[pID][i] != 0)
      {
        request[i] = rand() % NeedMatrix[pID][i];
      }
      else
      {
        request[i] = 0;
      }
    }

    printReqOrRelVector(request);

    getRes(pID, request);

    pthread_mutex_unlock(&mutex);

    //random release vector gernerate karne ke liye
    sleep(1);
    int releaseVector[M];
    pthread_mutex_lock(&mutex);

    //release vector ko initialize karna
    for (i = 0; i < M; i++)
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

int getRes(int pID, int request[])
{
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

  for (i = 0; i < M; ++i)
  {
    NeedMatrix[pID][i] -= request[i];
    allocmatrix[pID][i] += request[i];
    avail[i] -= request[i];
  }
  printf("Checking is the state is safe...\n");

  if (safemodecase() == 0)
  {
    printf("\nx========================x\n|Safe Mode. Resources Allocated|\nx=========================x\n");
    printf("\nAuthor:@Abhishek Sharma\n");

    exit(1);
    return 0;
  }
  else
  {
    printf("\nx=====================x\n|State is not safe.          |\nx=====================x\n");
    printf("\nAuthor:@Abhishek Sharma\n");
    exit(1);

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
  for (i = 0; i < M; i++)
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
  for (i = 0; i < M; ++i)
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

  for (i = 0; i < M; ++i)
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

  for (i = 0; i < M; ++i)
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
  for (i = 0; i < N; ++i)
  {
    printf("{ ");
    for (j = 0; j < M; ++j)
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
  for (i = 0; i < N; ++i)
  {
    printf("{ ");
    for (j = 0; j < M; ++j)
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
  for (i = 0; i < M; ++i)
  {
    printf("%d, ", avail[i]);
  }
  printf("\n");
  printf("\n");
  return;
}

void printReqOrRelVector(int vec[])
{
  printf("ReqOrRelVector: \n");
  for (i = 0; i < M; ++i)
  {
    printf("%d, ", vec[i]);
  }
  printf("\n");
  return;
}

// Função de estudo de estado
int safemodecase()
{
  int finish[N] = {0};
  int work[M];

  for (i = 0; i < M; i++)
  {
    work[i] = avail[i];
  }

  int k;
  for (i = 0; i < N; i++)
  {
    if (finish[i] == 0)
    {
      for (j = 0; j < M; j++)
      {
        if (NeedMatrix[i][j] <= work[j])
        {

          if (j == M - 1)
          {
            finish[i] = 1;
            for (k = 0; k < M; ++k)
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

  for (i = 0; i < N; i++)
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

  for (i = 0; i < M; i++)
  {

    scanf("%d", &avail[i]);
  }
  printf("Insira a matriz de recursos já alocados\n");
  for (i = 0; i < N; i++)
  {

    for (j = 0; j < M; j++)
    {

      scanf("%d", &allocmatrix[i][j]);
    }
  }
  printf("Insira a matriz máxima\n");
  for (i = 0; i < N; i++)
  {

    for (j = 0; j < M; j++)
    {

      scanf("%d", &MaxMatrix[i][j]);
    }
  }

  // Definindo valores da NeedMatrix
  for (i = 0; i < N; ++i)
  {
    for (j = 0; j < M; ++j)
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
  pthread_t *tid = malloc(sizeof(pthread_t) * N);

  int *pid = malloc(sizeof(int) * N); // process ID

  //initialize pid and create pthreads
  for (i = 0; i < N; i++)
  {
    *(pid + i) = i;
    pthread_create((tid + i), &attrDefault, procs, (pid + i));
  }

  //join threads
  for (i = 0; i < N; i++)
  {
    pthread_join(*(tid + i), NULL);
  }
  return 0;
}
