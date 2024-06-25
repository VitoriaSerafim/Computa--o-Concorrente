/* Programa que converte imagens coloridas para escala de cinza de modo SEQUENCIAL. 
Entrada: Imagem colorida 
Saida: Imagem em tons de cinza e tempo de processamento */

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<sys/time.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



// função para converter uma imagem em tons de cinza
void converter_cinza(unsigned char *img, unsigned char *img_cinza, int larg, int alt, int canais) {
    int index;
    unsigned char r, g, b, cinza; 

    for(int i = 0; i < alt; i++) {
        for(int j = 0; j < larg; j++) {
            index = (i * larg + j) * canais;
            r = img[index];
            g = img[index + 1];
            b = img[index + 2];
            cinza = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b); // media ponderada dos canais (Metodo da luminosidade)
            img_cinza[i * larg + j] = cinza;
        }
    }
}

// função para aplicar o filtro de média
void aplicar_filtro(unsigned char *img_cinza, unsigned char *img_filtrada, int larg, int alt) {
    int soma;
    int cont; 
    int ni, nj;
    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};

    for(int i = 0; i < alt; i++) {
        for(int j = 0; j < larg; j++) {
            soma = img_cinza[i * larg + j];
            cont = 1;

            for (int k = 0; k < 4; k++) {
                ni = i + dx[k];
                nj = j + dy[k];

                if (ni >= 0 && ni < alt && nj >= 0 && nj < larg) {
                    soma += img_cinza[ni * larg + nj];
                    cont++;
                }
            }
            img_filtrada[i * larg + j] = soma / cont;
        }
    }
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
   

    // INICIALIZACAO 
    {
    if(argc < 2) {
        printf("Digite: %s <nome do arquivo de imagem>\n", argv[0]);
        return 1;
    }

    // carrega imagem
    img = (unsigned char *) stbi_load(argv[1], &larg, &alt, &canais, 0);
    if(!img) {fprintf(stderr, "\nERRO -- erro ao carregar a imagem!\n"); return 2;}

    printf("Imagem carregada: %dpx de largura, %dpx de altura e %d canais.\n", larg, alt, canais);
    tam = larg * alt;

    // Aloca memória para a imagem em tons de cinza 
    img_cinza = (unsigned char *) malloc(tam * sizeof(unsigned char));
    if(img_cinza == NULL) {fprintf(stderr, "\nERRO -- malloc\n"); return 3;}

    // Aloca memória para a imagem filtrada
    img_filtrada = (unsigned char *) malloc(tam * sizeof(unsigned char));
    if(img_filtrada == NULL) {fprintf(stderr, "\nERRO -- malloc\n"); return 3;}
    }
    
    // PROCESSAMENTO
    {
    gettimeofday(&inicio, NULL);

    // converte para escala de cinza
    converter_cinza(img, img_cinza, larg, alt, canais);

    // aplica o filtro de média
    aplicar_filtro(img_cinza, img_filtrada, larg, alt);

    gettimeofday(&fim, NULL);
    tempo = (fim.tv_sec - inicio.tv_sec) + (fim.tv_usec - inicio.tv_usec) / 1000000.0;
    }

    // FINALIZACAO
    {
    // salva a imagem filtrada
    stbi_write_png("Imagem_Cinza_Sequencial.png", larg, alt, 1, img_filtrada, larg);

    // liberacao da memoria
    stbi_image_free(img);
    free(img_cinza);
    free(img_filtrada);

    printf("%lf\n\n", tempo);
    }

    return 0;
}
