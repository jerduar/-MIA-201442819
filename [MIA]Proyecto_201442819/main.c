#include <stdio.h>
#include <stdlib.h>

void CreacionDisco(int size, char path[256], char unidad[10], char nombre[256] ){

    char destino[30];

    strcpy(destino,path);
    strcat(destino,nombre);

    printf("%s",destino);

    FILE *archivo_binario;
    archivo_binario = fopen(destino,"wb+");
    if(archivo_binario == NULL){
        printf("ERRO AL CARGAR EL ARCHIVO\n");
    }else{
        printf("DISCO CREADO!\n");
    }

}

int main()
{
    printf("Proyecto Archivos\n");
    CreacionDisco(12,"/home/jerduar/","M","prueba.dsk");
    return 0;
}
