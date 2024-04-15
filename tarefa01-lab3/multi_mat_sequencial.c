/* Disciplina: Programacao Concorrente */
/* Aluna: Vitória de Souza Serafim */
/* Laboratório: 3 */
/* Codigo: "Tarefa 01" do terceiro laboratório - código para multiplicação de matrizes (sequencial) */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// estrutura Matriz
typedef struct
{
    int linhas;
    int colunas;
    float *dados;
} Matriz;

// declaração das funções
Matriz *criar_matriz(int linhas, int colunas);
void liberar_matriz(Matriz *matriz);
Matriz *ler_matriz(char *nome_arquivo);
Matriz *multiplicar_matrizes(Matriz *matrizA, Matriz *matrizB);

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

// função para multiplicar duas matrizes
Matriz *multiplicar_matrizes(Matriz *matrizA, Matriz *matrizB)
{
    // verifica se as dimensões das matrizes são compatíveis para multiplicação
    if (matrizA->colunas != matrizB->linhas)
    {
        fprintf(stderr, "Erro: Dimensões das matrizes incompatíveis para multiplicação\n");
        exit(1);
    }

    // aloca memória para a matriz resultante
    Matriz *resultado = criar_matriz(matrizA->linhas, matrizB->colunas);

    // realiza a multiplicação das matrizes
    for (int i = 0; i < matrizA->linhas; i++)
    {
        for (int j = 0; j < matrizB->colunas; j++)
        {
            float soma = 0.0;
            for (int k = 0; k < matrizA->colunas; k++)
            {
                soma += matrizA->dados[i * matrizA->colunas + k] * matrizB->dados[k * matrizB->colunas + j];
            }
            resultado->dados[i * resultado->colunas + j] = soma;
        }
    }

    return resultado;
}

int main(int argc, char *argv[])
{
    // validação dos parâmetros de entrada do programa, caso seja diferente do esperando informa a maneira correta.
    if (argc != 4)
    {
        print("ERRO AO INICIAR O PROGRAMA\n");
        printf("EH ESPERADO: %s <matriz_1.bin> <matriz_2.bin> <matriz_resultado.bin>\n", argv[0]);
        return 1;
    }

    // realiza medição do tempo (em segundos)
    clock_t start, process_Start, process_End, end;
    double cpu_time_used, time_start, time_process, time_end;

    start = clock(); // Marca o início do tempo de execução

    FILE *descritorArquivo; // descritor do arquivo de saida
    size_t ret;             // retorno da funcao de escrita no arquivo de saida
    // realiza leitura das matrizes A e B dos arquivos binários (fornecidos como argumentos)
    Matriz *matrizA = ler_matriz(argv[1]);
    Matriz *matrizB = ler_matriz(argv[2]);


    process_Start = clock();
    Matriz *resultado = multiplicar_matrizes(matrizA, matrizB);
    process_End = clock();
    
    // escreve a matriz no arquivo
    // abre o arquivo para escrita binaria
    descritorArquivo = fopen(argv[3], "wb");
    if (!descritorArquivo)
    {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 3;
    }
    // escreve numero de linhas e de colunas
    ret = fwrite(&resultado->linhas, sizeof(int), 1, descritorArquivo);
    ret = fwrite(&resultado->colunas, sizeof(int), 1, descritorArquivo);
    // escreve os elementos da matriz
    ret = fwrite(resultado->dados, sizeof(float), (resultado->colunas * resultado-> linhas), descritorArquivo);
    if (ret < (resultado->colunas * resultado-> linhas))
    {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return 4;
    }

    // liberar memória alocada para as matrizes
    liberar_matriz(matrizA);
    liberar_matriz(matrizB);
    liberar_matriz(resultado);

    // finaliza a contagem do tempo (em segundos)
    end = clock(); // Marca o final do tempo de execução

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC; // Calcula o tempo de CPU usado
    time_end = ((double)(end - process_End)) / CLOCKS_PER_SEC; // 
    time_start = ((double)(process_Start - start)) / CLOCKS_PER_SEC; // 
    time_process = ((double)(process_End - process_Start)) / CLOCKS_PER_SEC; // 

    //print das informações dos tempos de execução de cada etapa do programa 
     printf("%f;%f;%f;%f\n", cpu_time_used, time_start, time_end, time_process);
    return 0;
}