/* Disciplina: Programacao Concorrente */
/* Aluna: Vitória de Souza Serafim */
/* Laboratório: 1 */
/* Codigo: "Tarefa 05" do primeiro laboratório  */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define N_SIZE 15  // Tamanho do vetor (e quantidade de numeros que faremos a operação)
#define MTHREADS 5 // total de threads a serem criadas

// Estrutura  que utilizaremos para armazenar os argumentos da thread
//  e o vetor onde realizaremos nossa operação (elevar os nºs ao quadrado)
typedef struct
{
    int *vetor;                // vetor onde buscaremos os valores para aplicar a função
    int idThread, inicio, fim; // identificação e indices da(s) thread(s)
} t_Args;

// Função para inicializar o vetor com valores aleatórios
void initVetor(int *vetor, int tamanho)
{
    printf("Vetor original (aleatorio):\n");
    printf(" [ ");
    for (int i = 0; i < tamanho; i++)
    {
        vetor[i] = rand() % 20; // Números aleatórios de 0 a 19
        i == (tamanho - 1) ? printf(" %d ", vetor[i]) : printf(" %d ,", vetor[i]);
    }
    printf(" ] \n");
}

// Função/Operação que cada thread irá executar
// No nosso caso vamos elevar cada valor a[i] do vetor ao quadrado
void *aoQuadrado(void *args)
{
    t_Args *t_args = (t_Args *)args;

    // vamos inicializar e atribuir os valores dos indices para a operação
    int *vetor = t_args->vetor;
    int inicio = t_args->inicio;
    int fim = t_args->fim;

    for (int i = inicio; i < fim; i++)
    {
        vetor[i] = vetor[i] * vetor[i];
    }

    pthread_exit(NULL);
}

// Validação para verificar se os valores finais do vetor estão corretos
void verificarResultado(int *aux, int *vetor, int tamanho)
{
    // para validar nosso resultado, irei calcular o erro entre a resposta obtida e o vetor original
    //  defini uma variavel para calcular o erro, utilizarei como erro máximo o valor de 0.1 (um erro grande, até..)
    float erro;

    for (int i = 0; i < tamanho; i++)
    {
        erro = fabs(sqrt(vetor[i]) - aux[i]);

        if (erro > 0.1)
        {
            printf("ERRO - vetor não foi calculado corretamente!");
            i = tamanho + 1; // só para escapar o loop e sair da função
        }
    }
    printf("\n Vetor-Resposta Calculado Corretamente! Resultado: \n");
    printf(" [ ");
    for (int i = 0; i < tamanho; i++)
    {
        i == (tamanho - 1) ? printf(" %d ", vetor[i]) : printf(" %d ,", vetor[i]);
    }
    printf(" ] \n");
    printf("\n");
}

// Função para copiar o vetor definido e
//  validar se a operação de elevar ao quadrado foi feita corretamente
void copiaVetor(int *copia, int *original, int tamanho)
{
    for (int i = 0; i < tamanho; i++)
    {
        *(copia + i) = *(original + i);
    }
}

int main()
{
    int vetor[N_SIZE], aux[N_SIZE]; // nossos vetores
    initVetor(vetor, N_SIZE);       // inicialização do vetor principal
    copiaVetor(aux, vetor, N_SIZE); // copia para a validação do resultado

    // Criando threads
    pthread_t tid_sistema[MTHREADS]; // identificadores das threads no sistema
    t_Args args[MTHREADS];
    int tarefasPorThread = N_SIZE / MTHREADS;

    for (int i = 0; i < MTHREADS; i++)
    {
        args[i].vetor = vetor;
        args[i].inicio = i * tarefasPorThread;
        args[i].fim = (i == MTHREADS - 1) ? N_SIZE : (i + 1) * tarefasPorThread;
        pthread_create(&tid_sistema[i], NULL, aoQuadrado, (void *)&args[i]);
    }

    // espera todas as threads terminarem
    for (int i = 0; i < MTHREADS; i++)
    {
        if (pthread_join(tid_sistema[i], NULL))
        {
            printf("--ERRO: pthread_join() \n");
            exit(-1);
        }
    }

    verificarResultado(aux, vetor, N_SIZE);
    // Thread principal terminou
    pthread_exit(NULL);

    return 0;
}
