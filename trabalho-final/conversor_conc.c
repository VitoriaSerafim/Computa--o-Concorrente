/* Programa que converte imagens coloridas para escala de cinza de modo CONCORRENTE. 
Entrada: Imagem colorida e numero de threads
Saida: Imagem em tons de cinza e tempo de processamento */

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>
#include<sys/time.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int num_threads; // numero de threads a serem criadas

typedef struct {
    int thread_id; 
    int larg; 
    int alt;
    int canais;
    unsigned char *img;
    unsigned char *img_cinza;
    unsigned char *img_filtrada;
} DadosThread;

// função para converter uma imagem em tons de cinza
void *converter_cinza(void *arg) {
    int index;
    unsigned char r, g, b, cinza;
    DadosThread *dados = (DadosThread *)arg;
    int inicio = dados->alt / num_threads * dados->thread_id;
    int fim = (dados->thread_id == num_threads - 1) ? dados->alt : inicio + dados->alt / num_threads;

    for(int i = inicio; i < fim; i++) {
        for(int j = 0; j < dados->larg; j++) {
            index = (i * dados->larg + j) * dados->canais;
            r = dados->img[index];
            g = dados->img[index + 1];
            b = dados->img[index + 2];
            cinza = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
            dados->img_cinza[i * dados->larg + j] = cinza;
        }
    }
    return NULL;
}

// função para aplicar o filtro de média
void *aplicar_filtro(void *arg) {
    int soma;
    int cont; 
    int ni, nj;
    DadosThread *dados = (DadosThread *)arg;
    int inicio = dados->alt / num_threads * dados->thread_id;
    int fim = (dados->thread_id == num_threads - 1) ? dados->alt : inicio + dados->alt / num_threads;

    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};

    for(int i = inicio; i < fim; i++) {
        for(int j = 0; j < dados->larg; j++) {
            soma = dados->img_cinza[i * dados->larg + j];
            cont = 1;

            for(int k = 0; k < 4; k++) {
                ni = i + dx[k];
                nj = j + dy[k];

                if(ni >= 0 && ni < dados->alt && nj >= 0 && nj < dados->larg) {
                    soma += dados->img_cinza[ni * dados->larg + nj];
                    cont++;
                }
            }

            dados->img_filtrada[i * dados->larg + j] = soma / cont;
        }
    }
    return NULL;
}

int main(int argc, char **argv) {
    unsigned char *img;
    unsigned char *img_cinza;
    unsigned char *img_filtrada;
    int larg, alt; // dimensoes da imagem
    int tam; // tamanho da imagem
    int canais; // canais de cor da imagem
    struct timeval inicio, fim; // variaveis para marcar o tempo de execucao
    double tempo; // tempo total de processamento
    pthread_t *tid;  // identificadores da thread no sistema
    DadosThread *thread_dados;

    // INICIALIZACAO 
    {
    if(argc < 3) {
        printf("Digite: %s <nome do arquivo de imagem> <numero de threads>\n", argv[0]);
        return 1;
    }
    num_threads = atoi(argv[2]);

    // carrega imagem
    img = (unsigned char *) stbi_load(argv[1], &larg, &alt, &canais, 0);
    if(!img) {fprintf(stderr, "\nERRO -- erro ao carregar a imagem!\n"); return 2;}

    // printf("Imagem carregada: %dpx de largura, %dpx de altura e %d canais.\n", larg, alt, canais);
    tam = larg * alt;

    // aloca memória para a imagem em tons de cinza 
    img_cinza = (unsigned char *) malloc(tam * sizeof(unsigned char));
    if(img_cinza == NULL) {fprintf(stderr, "\nERRO -- malloc\n"); return 3;}

    // aloca memória para a imagem filtrada
    img_filtrada = (unsigned char *) malloc(tam * sizeof(unsigned char));
    if(img_filtrada == NULL) {fprintf(stderr, "\nERRO -- malloc\n"); return 3;}

    // alocacao das estruturas
    tid = (pthread_t *) malloc(sizeof(pthread_t) * num_threads);
    if(tid == NULL) {fprintf(stderr, "\nERRO -- malloc\n"); return 4;}
    thread_dados = (DadosThread *) malloc(sizeof(DadosThread) * num_threads);
    if(thread_dados == NULL) {fprintf(stderr, "\nERRO -- malloc\n"); return 4;}
    }

    // PROCESSAMENTO
    {
    gettimeofday(&inicio, NULL);

    // criacao das threads
    for (int i = 0; i < num_threads; i++) {
        thread_dados[i].thread_id = i;
        thread_dados[i].larg = larg;
        thread_dados[i].alt = alt;
        thread_dados[i].canais = canais;
        thread_dados[i].img = img;
        thread_dados[i].img_cinza = img_cinza;
        thread_dados[i].img_filtrada = img_filtrada;
        pthread_create(&tid[i], NULL, converter_cinza, (void *)&thread_dados[i]);
    }
    
    // espera pelo termino das threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(tid[i], NULL);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&tid[i], NULL, aplicar_filtro, (void *)&thread_dados[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(tid[i], NULL);
    }

    gettimeofday(&fim, NULL);
    tempo = (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1000000.0;
    }

    // FINALIZACAO
    { 
    // salva a imagem filtrada
    stbi_write_png("Imagem_Cinza_Concorrente.png", larg, alt, 1, img_filtrada, larg);

    // liberacao da memoria
    stbi_image_free(img);
    free(img_cinza);
    free(img_filtrada);
    free(tid);
    free(thread_dados);

    // printf("\nNumero de threads = %d\n", num_threads);
    printf(" %lf\n\n", tempo);
    }

    return 0;
}
