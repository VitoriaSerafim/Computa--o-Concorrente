// Encontra o menor e o maior valor em um vetor de float
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TESTE

// escopo global (par ser compartilhado com as threads)
float *vetor; // vetor de entrada

// define o tipo de dado de entrada das threads
typedef struct
{
    float soma;
    int id;            // id da thread
    long int tamBloco; // tamanho do bloco (cada thread processa um bloco)
} tArgs;

// fluxo das threads
void *tarefa(void *arg)
{
    tArgs *args = (tArgs *)arg;
    long int ini = args->id * args->tamBloco; // elemento inicial do bloco da thread
    long int fim = ini + args->tamBloco;      // elemento final(nao processado) do bloco da thread

    // soma os elementos do bloco da thread
    for (long int i = ini; i < fim; i++)
    {
        args->soma += vetor[i];
    }

    // retorna o resultado da soma local
    pthread_exit((void *)args);
}

// fluxo principal
int main(int argc, char *argv[])
{
    float somaGeral = 0; // valores encontrados pela solucao concorrente
    int dim;             // dimensao do vetor de entrada
    long int tamBloco;   // tamanho do bloco de cada thread
    int nthreads;        // numero de threads que serao criadas

    // tArgs *args; //vetor de argumentos para as threads
    pthread_t *tid; // vetor de identificadores das threads no sistema
    tArgs *retorno; // valor de retorno das threads

#ifdef TESTE
    float somaCorreto = 0; // valores de referencia para os testes
#endif

    // recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
    if (argc < 2)
    {
        fprintf(stderr, "Digite: %s <numero threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]);
    printf("nthreads=%d\n", nthreads);

    // carrega o vetor de entrada
    scanf("%d", &dim); // primeiro pergunta a dimensao do vetor
    printf("dim=%d\n", dim);

    // aloca o vetor de entrada
    vetor = malloc(sizeof(float) * dim);
    if (vetor == NULL)
    {
        fprintf(stderr, "ERRO--malloc 01\n");
        return 2;
    }
    // preenche o vetor de entrada
    for (long int i = 0; i < dim; i++)
        scanf("%f", &vetor[i]);
#ifdef TESTE
    // le os valores esperados de maior e menor
    scanf("%f", &somaCorreto);
    for (long int i = 0; i < dim; i++)
        printf("%f ", vetor[i]);
#endif
    // inicializa as variaveis de saida com o valor na primeira posicao do vetor

    // cria as threads
    tid = (pthread_t *)malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL)
    {
        fprintf(stderr, "ERRO--malloc 02\n");
        return 3;
    }
    tamBloco = dim / nthreads; // tamanho do bloco de cada thread
    if (!tamBloco)
        printf("\nA quantidade de threads eh maior que a quantidade de elementos, a execucao sera sequencial!\n");
    for (int i = 0; i < nthreads; i++)
    {
        // aloca e preenche argumentos para thread
        tArgs *args = (tArgs *)malloc(sizeof(tArgs));
        if (args == NULL)
        {
            printf("--ERRO: malloc() 03\n");
            pthread_exit(NULL);
        }
        args->id = i;
        args->tamBloco = tamBloco;
        args->soma = 0;
        if (pthread_create(tid + i, NULL, tarefa, (void *)args))
        {
            fprintf(stderr, "ERRO--pthread_create\n");
            return 5;
        }
    }

    // processa a parte final do vetor
    if (dim % nthreads)
    {
        puts("\nMain com tarefa");
        for (long int i = dim - (dim % nthreads); i < dim; i++)
        {
            somaGeral += vetor[i];
        }
    }

    // aguarda o termino das threads
    for (int i = 0; i < nthreads; i++)
    {
        if (pthread_join(*(tid + i), (void **)&retorno))
        {
            fprintf(stderr, "ERRO--pthread_create\n");
            return 6;
        }
        somaGeral += retorno->soma;
    }

    // exibir os resultados
    printf("\n\nSoma: %f\n", somaGeral);

#ifdef TESTE
    printf("\n\nSoma correta: %f\n", somaCorreto);
#endif

    // libera as areas de memoria alocadas
    free(vetor);
    free(tid);

    return 0;
}