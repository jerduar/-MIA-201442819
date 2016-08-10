#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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

#define sizeof_MBR sizeof(MBR)
#define sizeof_EBR sizeof(EBR)
#define sizeof_PARTITION sizeof(Partition)

void CreacionDisco(int size, char path[256], char unidad[10], char nombre[256] ){

    char destino[500];

    strcpy(destino,path);
    strcat(destino,nombre);

    FILE *archivo_binario;

    int mult = 1024;

    if(strcasecmp(unidad,"M")==0){
        mult = mult * 1024;
    }else if(strcasecmp(unidad,"B")==0){
        mult = 1;
    }

    long tamano = mult*size;

    MBR nuevo_mvr;
    nuevo_mvr.mbr_tamano = tamano;
    nuevo_mvr.mbr_disk_signature = rand() % 11;
    printf("El tamaño es: %d\n", tamano);
    nuevo_mvr.mbr_fecha_creacion = time(0);


    archivo_binario = fopen(destino,"wb+");
    if(archivo_binario == NULL){
        printf("ERRO AL CARGAR EL ARCHIVO\n");

    }else{
        long num = tamano / sizeof("\0");
        for(int i = 0; i <= num ; i++){
            fseek(archivo_binario,i*sizeof("\0") ,SEEK_SET);
            fwrite("\0" , 1 , sizeof("\0") , archivo_binario );
        }

        fseek(archivo_binario,0 ,SEEK_SET);
        fwrite(&nuevo_mvr,1,sizeof(MBR),archivo_binario);


        fclose(archivo_binario);
        printf("DISCO CREADO!\n");
    }

}

void rmkdisk(char path[256]){
    if(remove(path) == 0){
        printf("Archivo con direccion %s eliminado",path);
    }else{
        printf("Archivo %s no encontrado",path);
    }
}

void fdisk(int size, char unit, char path[256], char type, char fit[2], char delet[4], char name[256], int add){
    //LECTURA DEL ARCHIVO DE DISCO
    FILE *archivo_binario;
    archivo_binario = fopen(path,"rb+");
    if(archivo_binario != NULL){
        //LECTURA DEL MBR
        MBR mbr;
        fseek(archivo_binario,0,SEEK_SET);
        fread(&mbr,sizeof_MBR,1,archivo_binario);
        fclose(archivo_binario);

        int mult = 1024;
        /*
        printf("sin problema\n");
        if(strcasecmp(unit,"K")== 0){
            mult = 1024;
        }else if(strcasecmp(unit,"M")==0){
            mult = mult * 1024;
        }else if(strcasecmp(unit,"B")==0){
            mult = 1;
        }*/

        long tamano = mult; //* size;


        strcpy(mbr.mbr_partition_1.part_name,name);
        mbr.mbr_partition_1.part_size=tamano;
        mbr.mbr_partition_1.part_fit = fit;
        mbr.mbr_partition_1.part_type = type;
        mbr.mbr_partition_1.part_status = 'a';
        mbr.mbr_partition_1.part_start = 0;

        //mbr.mbr_partition_1 = p;

        archivo_binario = fopen(path,"rb+");
        fseek(archivo_binario,0,SEEK_SET);
        fwrite(&mbr , 1 , sizeof_MBR , archivo_binario );
        fclose(archivo_binario);

        printf("La particion ha sido creada\n");

    }else{
        printf("No se encuentra el disco buscado\n");
    }

}

void Lector(){

    int activo = 1;
    char *scanner;
    char *result;

    while(activo==1){
        scanf("%s",&scanner);
        result = strtok(scanner," ");

        while(result != NULL){
        //printf("%s\n",result);
            if(strcasecmp(result,"MKDISK") == 0){
                //printf("%s\n", result);
            }
            result = strtok(NULL," ");
        }

        printf("%s\n",scanner);

    }

}

int main()
{
    printf("Proyecto Archivos\n");
    CreacionDisco(1,"/home/jerduar/","M","prueba.dsk");

    fdisk(2,"K","/home/jerduar/prueba.dsk","M","pru","fsds","nombre",12);

    MBR r;
    FILE *file = fopen("/home/jerduar/prueba.dsk","wb+");
    fseek(file,0,SEEK_SET);
    fread(&r,sizeof_MBR,1,file);
    fclose(file);
    printf("kjlj\n");
    printf("hhhh%s\n",r.mbr_partition_1.part_name);

    return 0;

}
