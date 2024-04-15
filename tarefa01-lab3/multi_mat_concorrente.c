/* Disciplina: Programacao Concorrente */
/* Aluna: Vitória de Souza Serafim */
/* Laboratório: 3 */
/* Codigo: "Tarefa 01" do terceiro laboratório - código para multiplicação de matrizes (concorrente) */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// estrutura Matriz
typedef struct
{
    int linhas;
    int colunas;
    float *dados;
} Matriz;

// estrutura para passar argumentos para a thread
typedef struct
{
    Matriz *matrizA;
    Matriz *matrizB;
    Matriz *resultado;
    int thread_id;
    int numero_threads;
} ThreadArgs;

// declaração das funções
Matriz *criar_matriz(int linhas, int colunas);
void liberar_matriz(Matriz *matriz);
Matriz *ler_matriz(char *nome_arquivo);
void *calcular_parte_matriz(void *args);
Matriz *multiplicar_matrizes(Matriz *matrizA, Matriz *matrizB, int numero_threads);

// função para alocar uma matriz dinamicamente
Matriz *criar_matriz(int linhas, int colunas)
{
    Matriz *matriz = malloc(sizeof(Matriz));
    if (matriz == NULL)
    {
        fprintf(stderr, "Erro de alocação de memória para matriz\n");
        exit(1);
    }
    matriz->linhas = linhas;
    matriz->colunas = colunas;
    matriz->dados = malloc(sizeof(float) * linhas * colunas);
    if (matriz->dados == NULL)
    {
        fprintf(stderr, "Erro de alocação de memória para dados da matriz\n");
        free(matriz);
        exit(1);
    }
    return matriz;
}

// função para liberar a memória alocada para uma matriz
void liberar_matriz(Matriz *matriz)
{
    free(matriz->dados);
    free(matriz);
}

// função para ler uma matriz de um arquivo binário
Matriz *ler_matriz(char *nome_arquivo)
{
    FILE *arquivo;
    int linhas, colunas;
    size_t ret;

    // abre e realiza leitura do arquivo binário
    arquivo = fopen(nome_arquivo, "rb");
    if (arquivo == NULL)
    {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }

    // leitura do número de linhas e colunas
    ret = fread(&linhas, sizeof(int), 1, arquivo);
    if (ret != 1)
    {
        fprintf(stderr, "Erro ao ler o número de linhas\n");
        fclose(arquivo);
        exit(1);
    }

    ret = fread(&colunas, sizeof(int), 1, arquivo);
    if (ret != 1)
    {
        fprintf(stderr, "Erro ao ler o número de colunas\n");
        fclose(arquivo);
        exit(1);
    }

    // aloca memória para a matriz
    Matriz *matriz = criar_matriz(linhas, colunas);

    // lê os elementos da matriz
    ret = fread(matriz->dados, sizeof(float), linhas * colunas, arquivo);
    if (ret != linhas * colunas)
    {
        fprintf(stderr, "Erro ao ler os elementos da matriz\n");
        fclose(arquivo);
        liberar_matriz(matriz);
        exit(1);
    }

    fclose(arquivo);
    return matriz;
}

// função para calcular uma parte da matriz resultante
void *calcular_parte_matriz(void *args)
{
    ThreadArgs *thread_args = (ThreadArgs *)args;

    // calcula a quantidade de linhas para cada thread
    int linhas_por_thread = thread_args->resultado->linhas / thread_args->numero_threads;
    int inicio = thread_args->thread_id * linhas_por_thread;
    int fim = (thread_args->thread_id == thread_args->numero_threads - 1) ? thread_args->resultado->linhas : inicio + linhas_por_thread;

    // realiza a multiplicação das matrizes para a parte atribuída à thread
    for (int i = inicio; i < fim; i++)
    {
        for (int j = 0; j < thread_args->matrizB->colunas; j++)
        {
            float soma = 0.0;
            for (int k = 0; k < thread_args->matrizA->colunas; k++)
            {
                soma += thread_args->matrizA->dados[i * thread_args->matrizA->colunas + k] * thread_args->matrizB->dados[k * thread_args->matrizB->colunas + j];
            }
            thread_args->resultado->dados[i * thread_args->resultado->colunas + j] = soma;
        }
    }

    pthread_exit(NULL);
}

// função para multiplicar duas matrizes
Matriz *multiplicar_matrizes(Matriz *matrizA, Matriz *matrizB, int numero_threads)
{
    // verifica se as dimensões das matrizes são compatíveis para multiplicação
    if (matrizA->colunas != matrizB->linhas)
    {
        fprintf(stderr, "Erro: Dimensões das matrizes incompatíveis para multiplicação\n");
        exit(1);
    }

    // aloca memória para a matriz resultante
    Matriz *resultado = criar_matriz(matrizA->linhas, matrizB->colunas);

    // inicializa threads e argumentos
    pthread_t threads[numero_threads];
    ThreadArgs thread_args[numero_threads];

    // cria as threads
    for (int i = 0; i < numero_threads; i++)
    {
        thread_args[i].matrizA = matrizA;
        thread_args[i].matrizB = matrizB;
        thread_args[i].resultado = resultado;
        thread_args[i].thread_id = i;
        thread_args[i].numero_threads = numero_threads;

        if (pthread_create(&threads[i], NULL, calcular_parte_matriz, (void *)&thread_args[i]) != 0)
        {
            fprintf(stderr, "Erro ao criar thread\n");
            exit(1);
        }
    }

    // espera pelas threads
    for (int i = 0; i < numero_threads; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            fprintf(stderr, "Erro ao aguardar thread\n");
            exit(1);
        }
    }

    return resultado;
}

int main(int argc, char *argv[])
{
    // validação dos parâmetros de entrada do programa, caso seja diferente do esperando informa a maneira correta.
    if (argc != 5)
    {
        print("ERRO AO INICIAR O PROGRAMA\n");
        printf("EH ESPERADO: %s <matriz_1.bin> <matriz_2.bin> <numero_de_threads> <matriz_resultado.bin>\n", argv[0]);
        return 1;
    }

    int numero_threads = atoi(argv[3]);

    // realiza medição do tempo (em segundos)
    clock_t start, process_Start, process_End, end;
    double cpu_time_used, time_start, time_process, time_end;

    start = clock(); // Marca o início do tempo de execução

    // realiza leitura das matrizes A e B dos arquivos binários (fornecidos como argumentos)
    Matriz *matrizA = ler_matriz(argv[1]);
    Matriz *matrizB = ler_matriz(argv[2]);

    process_Start = clock();
    Matriz *resultado = multiplicar_matrizes(matrizA, matrizB, numero_threads);
    process_End = clock();
    // escreve a matriz resultante no arquivo
    FILE *descritorArquivo = fopen(argv[4], "wb");
    if (!descritorArquivo)
    {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 3;
    }
    // escreve numero de linhas e de colunas
    size_t ret = fwrite(&resultado->linhas, sizeof(int), 1, descritorArquivo);
    ret = fwrite(&resultado->colunas, sizeof(int), 1, descritorArquivo);
    // escreve os elementos da matriz
    ret = fwrite(resultado->dados, sizeof(float), resultado->colunas * resultado->linhas, descritorArquivo);
    if (ret < resultado->colunas * resultado->linhas)
    {
        fprintf(stderr, "Erro de escrita no arquivo\n");
        return 4;
    }

    // liberar memória alocada para as matrizes
    liberar_matriz(matrizA);
    liberar_matriz(matrizB);
    liberar_matriz(resultado);

    // finaliza a contagem do tempo (em segundos)
    end = clock(); // Marca o final do tempo de execução

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC; // Calcula o tempo de CPU usado
    time_end = ((double)(end - process_End)) / CLOCKS_PER_SEC;  
    time_start = ((double)(process_Start - start)) / CLOCKS_PER_SEC;  
    time_process = ((double)(process_End - process_Start)) / CLOCKS_PER_SEC;  

    //print das informações dos tempos de execução de cada etapa do programa 
     printf("%f;%f;%f;%f\n", cpu_time_used, time_start, time_end, time_process);

    return 0;
}