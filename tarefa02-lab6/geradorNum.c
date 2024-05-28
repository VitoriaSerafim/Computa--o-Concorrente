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

void gerarNumeros(const char* filename, long long int N, int maxValue) {
    long long int primos = 0; // variavel para guardar a qtd de primos encontrados
    size_t ret; // retorno da funcao de escrita no arquivo de saida

    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }
    ret = fwrite(&N, sizeof(long int), 1, file);
    if(!ret) {fprintf(stderr, "\nERRO -- Nao foi possivel concluir operacao de escrita!\n");}

     // imprime tamanho da sequencia N
    printf("\nTAMANHO DA SEQUENCIA (N): %ld\n", N);
   
    srand(time(NULL));

    for (long long int i = 0; i < N; i++) {
        int number = (rand() % maxValue);
        if(ehPrimo(number)) primos++;
         ret = fwrite(&number, sizeof(long int), 1, file);
        if(!ret) {fprintf(stderr, "\nERRO -- Nao foi possivel concluir operacao de escrita!\n");}
   
    }
    ret = fwrite(&primos, sizeof(long int), 1, file);
    if(!ret) {fprintf(stderr, "\nERRO -- Nao foi possivel concluir operacao de escrita!\n");}
   
    printf("FORAM ENCONTRADOS %lld PRIMOS.\n", primos);
    fclose(file);
    printf("ARQUIVO GERADO COM SUCESSO: %s\n", filename);
}

int main(int argc, char *argv[]) {
    // aqui abrimos o arquivo .bin
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <nome_arquivo> <quantidade_numeros>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* filename = argv[1];
    long long int N = atoi(argv[2]);
    int maxValue = 1000;

    gerarNumeros(filename, N, maxValue);

    return EXIT_SUCCESS;
}