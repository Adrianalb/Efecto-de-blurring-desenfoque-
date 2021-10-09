#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "omp.h"

#define NUM_THREADS 2

int main()
{
    omp_set_num_threads(NUM_THREADS);
    const double startTime = omp_get_wtime();
    
    FILE *image, *outputImage, *lecturas;
    image = fopen("sample2.bmp","rb");          //Imagen original a transformar
    outputImage = fopen("imagen5_blur.bmp","wb");    //Imagen transformada
    long ancho, alto;
    unsigned char r, g, b;               //Pixel
    unsigned char* ptr;
    unsigned char xx[54];
    int cuenta = 0;
    

    for(int i=0; i<54; i++) {
      xx[i] = fgetc(image);
      fputc(xx[i], outputImage);   //Copia cabecera a nueva imagen
    }

    ancho = (long)xx[20]*65536+(long)xx[19]*256+(long)xx[18];
    alto = (long)xx[24]*65536+(long)xx[23]*256+(long)xx[22];
    
    printf("Largo img: %li\n",alto);
    printf("Ancho img: %li\n",ancho);

    ptr = (unsigned char*)malloc(alto*ancho*3* sizeof(unsigned char));

    unsigned char img[alto][ancho];
    unsigned char imgBlur[alto][ancho];

    while(!feof(image)){
      b = fgetc(image);
      g = fgetc(image);
      r = fgetc(image);

      unsigned char pixel = 0.21*r+0.72*g+0.07*b;

      ptr[cuenta] = pixel; //b
      ptr[cuenta+1] = pixel; //g
      ptr[cuenta+2] = pixel; //r
      cuenta++;
    }

    int m = 0;
    #pragma omp for schedule(dynamic)
    for (int i = 0; i < alto; i++){
      for (int j = 0; j < ancho; j++){
        img[i][j] = ptr[m];
        m+=1;
      }
    }

    int med = 4;
    #pragma omp for schedule(dynamic)
    for (int i=med; i<alto-med; i++){
      for (int j=med; j<ancho-med; j++){
         int sum = 0;
         for (int x=-med; x<=med; x++){
            for (int y=-med; y<=med; y++)
               sum += img[i+x][j+y];
               imgBlur[i][j] = (int) sum/(81);
         }
       }
    }

    m = 0;
    #pragma omp for schedule(dynamic)
    for (int i = 0; i < alto; i++){
      for (int j = 0; j < ancho; j++){
        ptr[m] = imgBlur[i][j];
        ptr[m+1] = imgBlur[i][j];
        ptr[m+2] = imgBlur[i][j];
        m+=1;
      }
    }

    
    //Grises
    #pragma omp parallel
    {
      #pragma omp for schedule(dynamic)
      for (int i = 0; i < alto*ancho*3; ++i) {
        fputc(ptr[i], outputImage);
        fputc(ptr[i+1], outputImage);
        fputc(ptr[i+2], outputImage);
      }
    }
    
    free(ptr);
    fclose(image);
    fclose(outputImage);
    const double endTime = omp_get_wtime();
    printf("tomo (%lf) segundos.\n",(endTime - startTime));
    //printf("NUM_THREADS: %d", NUM_THREADS);
    return 0;
}