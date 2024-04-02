/* Disciplina: Programacao Concorrente */
/* Aluna: Vitória de Souza Serafim */
/* Laboratório: 2 */
/* Codigo: "Atividade 02" do segundo laboratório  */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 1000 //valor maximo dos elementos do vetor

//escopo global (para ser compartilhado com as threads)
float *vetor; //vetor de entrada

// Operação que cada thread irá executar
// No nosso caso passar por cada posição do vetor e acumular os valores em uma var

int main(int argc, char *argv[]) {
    long int n; //qtde de elementos do vetor que serao gerados
    float elem; //valor gerado para incluir no vetor
    int fator=1; //fator multiplicador para gerar números negativos
    float soma=0; //soma dos elementos da sequencia (no vetor) que sera gerada
    
    //valida e recebe os valores de entrada
    if(argc < 2) {
       printf("Use: %s <numero de elementos (maior que 0)> \n", argv[0]);
       return 1;
    }
    n = atol(argv[1]);
    if(n<1) {
       printf("ERRO: o numero de elementos deve ser maior que 0 \n");
       return 2;
    }	    

    //semente para gerar numeros randomicos
    srand((unsigned)time(NULL));

    //imprime a qtde de elementos do vetor (inteiro)
    printf("%ld\n", n);
    
    //gera o primeiro elemento e inicializa maior e menor
    elem = (rand() % MAX)/3.0 * fator;
    printf("%f ", elem);
    soma = elem;
    //gera e imprime os elementos restantes separado por espaco em branco
    for(int i=0; i<n-1; i++) {
        fator*=-1;
        elem = (rand() % MAX)/3.0 * fator;
        printf("%f ", elem);
        soma += elem;
    }
    
    //imprime maior e menor no final
    printf("\n%f\n", soma);

    return 0;
}