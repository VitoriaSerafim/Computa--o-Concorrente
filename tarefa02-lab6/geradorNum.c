#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

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

void gerarNumeros(const char* filename, long long int N, int minValue, int maxValue) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }
    long long int primos = 0;
    srand(time(NULL));
    fwrite(&N, sizeof(long long int), 1, file);
    for (int i = 0; i < N; i++) {
        int number = minValue + rand() % (maxValue - minValue + 1);
        fwrite(&number, sizeof(int), 1, file);
        if(ehPrimo(number)) primos++;
    }
     fwrite(-1, sizeof(int), 1, file);
    fwrite(&primos, sizeof(int), 1, file);
    printf("Foram encontrados %lld primos\n", primos);
    fclose(file);
    printf("Arquivo gerado com sucesso: %s\n", filename);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <nome_arquivo> <quantidade_numeros>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* filename = argv[1];
    long long int N = atoi(argv[2]);
    int minValue = 1;
    int maxValue = 1000;

    gerarNumeros(filename, N, minValue, maxValue);

    return EXIT_SUCCESS;
}