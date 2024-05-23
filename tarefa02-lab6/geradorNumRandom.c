#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void gerarNumeros(const char* filename, int N, int minValue, int maxValue) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        int number = minValue + rand() % (maxValue - minValue + 1);
        fwrite(&number, sizeof(int), 1, file);
    }

    fclose(file);
    printf("Arquivo gerado com sucesso: %s\n", filename);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <nome_arquivo> <quantidade_numeros> <valor_min> <valor_max>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* filename = argv[1];
    int N = atoi(argv[2]);
    int minValue = atoi(argv[3]);
    int maxValue = atoi(argv[4]);

    gerarNumeros(filename, N, minValue, maxValue);

    return EXIT_SUCCESS;
}