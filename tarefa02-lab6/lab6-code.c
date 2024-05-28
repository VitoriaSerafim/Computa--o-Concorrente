/* Disciplina: Programacao Concorrente */
/* Aluna: Vitória de Souza Serafim */
/* Laboratório: 6 */
/* Codigo: "Tarefa 02" do sexto laboratório - código para contar primos (concorrente)  */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <stdbool.h>
#include <semaphore.h>
#include <time.h>

#define BUFFER_SIZE 10  // Defina um valor padrão para o tamanho do buffer

typedef struct {
    int *buffer;
    int in;
    int out;
    int size;
    sem_t empty;
    sem_t full;
    pthread_mutex_t mutex;
} Buffer;

typedef struct {
    Buffer *buffer;
    int id;
    int primes_found;
} ThreadArgs;

Buffer buffer;
int total_primes = 0;
pthread_mutex_t total_primes_mutex = PTHREAD_MUTEX_INITIALIZER;

// Função para verificar se um número é primo (foi dada na descrição do laboratório)
int ehPrimo(long long int n) {
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

// Função do produtor
void* produtor(void* arg) {
    FILE *file = fopen((const char*)arg, "rb");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    int num;
    while (fread(&num, sizeof(int), 1, file)) {
        sem_wait(&buffer.empty);
        pthread_mutex_lock(&buffer.mutex);

        buffer.buffer[buffer.in] = num;
        buffer.in = (buffer.in + 1) % buffer.size;

        pthread_mutex_unlock(&buffer.mutex);
        sem_post(&buffer.full);
    }

    fclose(file);
    pthread_exit(NULL);
}

// Função do consumidor
void* consumidor(void* arg) {
    ThreadArgs *args = (ThreadArgs*)arg;

    while (1) {
        sem_wait(&buffer.full);
        pthread_mutex_lock(&buffer.mutex);

        int num = buffer.buffer[buffer.out];
        buffer.out = (buffer.out + 1) % buffer.size;

        pthread_mutex_unlock(&buffer.mutex);
        sem_post(&buffer.empty);

        if (ehPrimo(num)) {
            pthread_mutex_lock(&total_primes_mutex);
            total_primes++;
            args->primes_found++; // nÃo precisa entrar no lock pq eh local
            pthread_mutex_unlock(&total_primes_mutex);
           
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <quantidade_threads_consumidoras> <tamanho_buffer> <nome_arquivo>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_consumers = atoi(argv[1]);
    int buffer_size = atoi(argv[2]);
    int linhas, N; //dimensoes da matriz
    const char* filename = argv[3];
    FILE * descritorArquivo;
    size_t ret;


    buffer.size = buffer_size;
    buffer.buffer = malloc(buffer.size * sizeof(int));
    buffer.in = 0;
    buffer.out = 0;

   //abre o arquivo para leitura binaria
   descritorArquivo = fopen(filename, "rb");
   if(!descritorArquivo) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 2;
   }

   //le as dimensoes da matriz
   ret = fread(&N, sizeof(int), 1, descritorArquivo);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }
  printf("tamanho vetor %d  \n",N);

    sem_init(&buffer.empty, 0, buffer.size);
    sem_init(&buffer.full, 0, 0);
    pthread_mutex_init(&buffer.mutex, NULL);

    pthread_t producer_thread;
    pthread_t consumer_threads[num_consumers];
    ThreadArgs consumer_args[num_consumers];

    pthread_create(&producer_thread, NULL, produtor, (void*)filename);

    for (int i = 0; i < num_consumers; i++) {
        consumer_args[i].buffer = &buffer;
        consumer_args[i].id = i;
        consumer_args[i].primes_found = 0;
        pthread_create(&consumer_threads[i], NULL, consumidor, (void*)&consumer_args[i]);
    }

    pthread_join(producer_thread, NULL);

    for (int i = 0; i < num_consumers; i++) {
        pthread_cancel(consumer_threads[i]);
        pthread_join(consumer_threads[i], NULL);
    }

    int max_primes = 0;
    int winner_id = -1;
    for (int i = 0; i < num_consumers; i++) {
        if (consumer_args[i].primes_found > max_primes) {
            max_primes = consumer_args[i].primes_found;
            winner_id = i;
        }
        printf("Thread consumidora %d com %d primos\n", i, consumer_args[i].primes_found);
    }

    printf("\n");
    printf("Total de primos encontrados: %d\n", total_primes);
    printf("Thread consumidora vencedora %d com %d primos\n", winner_id, max_primes);


    free(buffer.buffer);
    sem_destroy(&buffer.empty);
    sem_destroy(&buffer.full);
    pthread_mutex_destroy(&buffer.mutex);
    pthread_mutex_destroy(&total_primes_mutex);

    return EXIT_SUCCESS;
}
