#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct partition{
    char part_status;// indica si la particion esta activa o no
    char part_type;//indica el tipo de particion
    char part_fit;//tipo de ajuste de la particion
    int part_start;//byte en el que inicia la particion
    int part_size;//tamaño total de la particion en bytes
    char part_name[16];//Nombre de la particion
}Partition;

typedef struct MBR{
    int mbr_tamano;//tamano total del disco en bytes
    time_t mbr_fecha_creacion;//fecha y hora de creacion del disco
    int mbr_disk_signature;//Numero random que identifica cada disco
    Partition mbr_partition_1;//Estructura con informacion de la particion 1
    Partition mbr_partition_2;
    Partition mbr_partition_3;
    Partition mbr_partition_4;
}MBR;

typedef struct EBR{
    char part_status;//indica si la particion esta activa o no
    char part_fit;//tipo de ajuste la particion
    int part_start;//indica el byte donde inicia la particion
    int part_size;//tamano total de la particion en bytes
    int part_next;//byte en el que esta el próximo EBR, -1 si no hay siguiente
    char part_name[16];//nombre de la particion
}EBR;

void CreacionDisco(int size, char path[256], char unidad[10], char nombre[256] ){

/*    char destino[500];

    strcpy(destino,path);
    strcat(destino,nombre);

    printf("%s",destino);

    FILE *archivo_binario;
    archivo_binario = fopen(destino,"wb+");
    if(archivo_binario == NULL){

        fseek(archivo, sizeof_record + r.contador*sizeof_estudiante, SEEK_SET);
        fwrite(&e, sizeof_estudiante, 1, archivo);
        printf("ERRO AL CARGAR EL ARCHIVO\n");

    }else{
        printf("DISCO CREADO!\n");
    }
*/
}

int main()
{
    printf("Proyecto Archivos\n");
    CreacionDisco(12,"/home/jerduar/","M","prueba.dsk");
    return 0;
}
