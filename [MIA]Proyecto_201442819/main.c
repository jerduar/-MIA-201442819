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

    Partition p;
        p.part_status = 'f';
        p.part_size = 0;

    MBR nuevo_mvr;
    nuevo_mvr.mbr_tamano = tamano;
    nuevo_mvr.mbr_disk_signature = rand() % 11;
    nuevo_mvr.mbr_fecha_creacion = time(0);

    nuevo_mvr.mbr_partition_1.part_status = p.part_status;
    nuevo_mvr.mbr_partition_2.part_status = p.part_status;
    nuevo_mvr.mbr_partition_3.part_status = p.part_status;
    nuevo_mvr.mbr_partition_4.part_status = p.part_status;

    nuevo_mvr.mbr_partition_1.part_size = p.part_size;
    nuevo_mvr.mbr_partition_2.part_size = p.part_size;
    nuevo_mvr.mbr_partition_3.part_size = p.part_size;
    nuevo_mvr.mbr_partition_4.part_size = p.part_size;


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

        if(unit == "K"){
            mult = 1024;
        }else if(unit == "M"){
            mult = mult * 1024;
        }else if(unit == "B"){
            mult = 1;
        }

        int tamano = mult * size; //* size;o

        Partition p;
        Partition aux;


        strcpy(p.part_name,name);
        p.part_size = tamano;
        p.part_fit = fit;
        p.part_type = type;
        p.part_status = 'v';
        p.part_start = 0;

        aux.part_status = mbr.mbr_partition_1.part_status;
        int tamano_disponible = mbr.mbr_tamano;
        //printf("tttsss %d\n", mbr.mbr_partition_2.part_size);
        if(mbr.mbr_partition_1.part_status != "f"){tamano_disponible = tamano_disponible - mbr.mbr_partition_1.part_size;}
        if(mbr.mbr_partition_2.part_status != "f"){tamano_disponible = tamano_disponible - mbr.mbr_partition_2.part_size;}
        if(mbr.mbr_partition_3.part_status != "f"){tamano_disponible = tamano_disponible - mbr.mbr_partition_3.part_size;}
        if(mbr.mbr_partition_4.part_status != "f"){tamano_disponible = tamano_disponible - mbr.mbr_partition_4.part_size;}

        mbr.mbr_partition_1.part_size = p.part_size;

        printf("disponible %d\n",tamano_disponible);
        printf("tamano : %d\n",tamano);
        if(tamano <= tamano_disponible && type == 'p'){
        printf("entre\n");
            if(mbr.mbr_partition_1.part_status == 'f'){
                mbr.mbr_partition_1.part_fit = p.part_fit;
                mbr.mbr_partition_1.part_size = p.part_size;
                mbr.mbr_partition_1.part_type = p.part_type;
                mbr.mbr_partition_1.part_status = 'v';
                strcpy(mbr.mbr_partition_1.part_name , p.part_name);
                printf("p1\n");

            }else if(mbr.mbr_partition_2.part_status == 'f'){
                mbr.mbr_partition_2.part_fit = p.part_fit;
                mbr.mbr_partition_2.part_size = p.part_size;
                mbr.mbr_partition_2.part_type = p.part_type;
                mbr.mbr_partition_2.part_status = 'v';
                strcpy(mbr.mbr_partition_2.part_name , p.part_name);
                printf("p2\n");

            }else if(mbr.mbr_partition_3.part_status == 'f'){
                mbr.mbr_partition_3.part_fit = p.part_fit;
                mbr.mbr_partition_3.part_size = p.part_size;
                mbr.mbr_partition_3.part_type = p.part_type;
                mbr.mbr_partition_3.part_status = p.part_status;
                strcpy(mbr.mbr_partition_3.part_name , p.part_name);
                printf("p3\n");

            }else if(mbr.mbr_partition_4.part_status == 'f'){
                mbr.mbr_partition_4.part_fit = p.part_fit;
                mbr.mbr_partition_4.part_size = p.part_size;
                mbr.mbr_partition_4.part_type = p.part_type;
                mbr.mbr_partition_4.part_status = p.part_status;
                strcpy(mbr.mbr_partition_4.part_name , p.part_name);
                printf("p4\n");
            }else{
                printf("ya no se pueden mas particiones primarias\n");
            }
        }else{
            printf("Ya no hay espacio suficiente\n");
        }

        archivo_binario = fopen(path,"rb+");
        fseek(archivo_binario,0,SEEK_SET);
        fwrite(&mbr , 1 , sizeof_MBR , archivo_binario );
        fclose(archivo_binario);

        printf("La particion ha sido creada\n");

    }else{
        printf("No se encuentra el disco buscado\n");
    }

}

void REP(char name[256],char path[256], char id[10], char ruta[256]){


    if(strcasecmp(name,"mbr") == 0){
        char instruccion[500] = "mkdir -p ";
        strcat(instruccion,path);
        printf("%s\n",instruccion);
        system(instruccion);


    MBR r;
    FILE *file = fopen("/home/jerduar/prueba.dsk","rb+");
    fseek(file,0,SEEK_SET);
    fread(&r,sizeof_MBR,1,file);
    fclose(file);


    printf("%s\n",path);
    char aux[256];
    memset(aux,'\0',256);
    strcpy(aux,path);
    printf("%s\n",aux);
    strcat(aux,"grafo.dot");
    printf("%s\n",aux);

    FILE *rep = fopen(aux,"wr+");

    int name = r.mbr_disk_signature;
    int size = r.mbr_tamano;
    time_t fecha = r.mbr_fecha_creacion;


    if(rep != NULL){
    char grafo[1024] = "graph g {\n";
    //strcat(grafo,"struct4 [shape=record,label=<<TABLE BORDER=\"2\" CELLBORDER=\"1\" CELLSPACING=\"0\"><TR><TD>Nombre</TD><TD PORT=\"f1\">VALOR</TD></TR>");
    //strcat(grafo,"<TR><TD>TAMANO</TD><TD>");strcat(grafo,size);strcat(grafo,"</TD></TR></TABLE>>];}");

    fprintf(rep,"graph g { struct4 [shape=record,label=<<TABLE BORDER=\"2\" CELLBORDER=\"1\" CELLSPACING=\"0\"><TR><TD>Nombre</TD><TD PORT=\"f1\">VALOR</TD></TR>");
    fprintf(rep,"<TR><TD>Disk_signature</TD><TD>%d</TD></TR><TR><TD>Tamaño</TD><TD>%d</TD></TR>",name,size);
    if(r.mbr_partition_1.part_size != 0){

        fprintf(rep,"<TR><TD>Part_status_1</TD><TD>%d</TD></TR><TR><TD>Part_type_1</TD><TD>%c</TD></TR>",r.mbr_partition_1.part_status,r.mbr_partition_1.part_type);
        fprintf(rep,"<TR><TD>Part_fit_1</TD><TD>%c</TD></TR><TR><TD>Part_start_1</TD><TD>%d</TD></TR>",r.mbr_partition_1.part_fit,r.mbr_partition_1.part_start);
        fprintf(rep,"<TR><TD>Part_size_1</TD><TD>%d</TD></TR><TR><TD>Part_name_1</TD><TD>%s</TD></TR>",r.mbr_partition_1.part_size,r.mbr_partition_1.part_name);


    }

    if(r.mbr_partition_2.part_size != 0){

        fprintf(rep,"<TR><TD>Part_status_1</TD><TD>%d</TD></TR><TR><TD>Part_type_2</TD><TD>%c</TD></TR>",r.mbr_partition_2.part_status,r.mbr_partition_2.part_type);
        fprintf(rep,"<TR><TD>Part_fit_1</TD><TD>%c</TD></TR><TR><TD>Part_start_2</TD><TD>%d</TD></TR>",r.mbr_partition_2.part_fit,r.mbr_partition_2.part_start);
        fprintf(rep,"<TR><TD>Part_size_1</TD><TD>%d</TD></TR><TR><TD>Part_name_2</TD><TD>%s</TD></TR>",r.mbr_partition_2.part_size,r.mbr_partition_2.part_name);


    }

    if(r.mbr_partition_3.part_size != 0){

        fprintf(rep,"<TR><TD>Part_status_3</TD><TD>%d</TD></TR><TR><TD>Part_type_3</TD><TD>%c</TD></TR>",r.mbr_partition_3.part_status,r.mbr_partition_3.part_type);
        fprintf(rep,"<TR><TD>Part_fit_3</TD><TD>%c</TD></TR><TR><TD>Part_start_3</TD><TD>%d</TD></TR>",r.mbr_partition_3.part_fit,r.mbr_partition_3.part_start);
        fprintf(rep,"<TR><TD>Part_size_3</TD><TD>%d</TD></TR><TR><TD>Part_name_3</TD><TD>%s</TD></TR>",r.mbr_partition_3.part_size,r.mbr_partition_3.part_name);


    }

    if(r.mbr_partition_4.part_size != 0){

        fprintf(rep,"<TR><TD>Part_status_4</TD><TD>%d</TD></TR><TR><TD>Part_type_4</TD><TD>%c</TD></TR>",r.mbr_partition_4.part_status,r.mbr_partition_4.part_type);
        fprintf(rep,"<TR><TD>Part_fit_4</TD><TD>%c</TD></TR><TR><TD>Part_start_4</TD><TD>%d</TD></TR>",r.mbr_partition_4.part_fit,r.mbr_partition_4.part_start);
        fprintf(rep,"<TR><TD>Part_size_4</TD><TD>%d</TD></TR><TR><TD>Part_name_4</TD><TD>%s</TD></TR>",r.mbr_partition_4.part_size,r.mbr_partition_4.part_name);


    }


    fprintf(rep,"</TABLE>>];}");
    fclose(rep);
    //fwrite(grafo,1,sizeof(grafo),rep);

    }

    }

}

//METODO PARA LEER LA CONSOLA
void Lector(){

    char *token = NULL;
    char scanner[256];
    char delimitador[] = " ";
    fflush(stdin);
    fgets(scanner,256,stdin);

    token = strtok(scanner,delimitador);

    while(token != NULL){

        //CASO EN EL QUE SE QUIERA CREAR UN NUEVO DISCO
        char nombre_param[256];
        char valor_param[256];
        int tamano_token_a;
        int tamano_token_b;

        char unit[50] = "M";
            char name[100];
            int size;
            char path[100];

            memset(unit, '\0', sizeof(unit));
            memset(name, '\0', sizeof(name));
            memset(valor_param, '\0', sizeof(valor_param));
            memset(path, '\0', sizeof(path));

            int bandera_unit = 0;
            int bandera_size = 0;
            int bandera_path = 0;
            int bandera_name = 0;
            int bandera_error = 0;

if(strcasecmp(token,"\n")!= 0){

        if(strcasecmp("mkdisk",token) == 0)
        {

            token = strtok(NULL,delimitador);

            while(token != NULL){
            memset(valor_param,'\0',sizeof(valor_param));
                tamano_token_a =  strlen(token);
                valor_param[0] = strrchr(token,':');
                if(valor_param != NULL){
                    tamano_token_b = strlen(valor_param)-1;
                    valor_param[0] = strrchr(valor_param,':');
                    //printf("VALORES: %s",valor_param);

                    for(int i = 0; i < tamano_token_a - tamano_token_b ; i++)
                    {
                        nombre_param[i] = token[i];
                    }


                    int tamano_aux = strlen(nombre_param);

                    for(int i = 0; i < tamano_token_a ; i++){
                        valor_param[i] = token[i + tamano_aux];
                    }

                    //printf("%s\n",valor_param);
                    char aux[256] = "";
                    memset(aux,'\0',sizeof(aux));
                    if(strcasecmp("-path::",nombre_param) == 0 || strcasecmp("-name::",nombre_param) == 0){


                    if(valor_param[0] == '"'){
                    //printf("EMPIEZA CON COMILLA\n");
                    //printf("char : %c\n",valor_param[strlen(valor_param)-2]);


                        if(valor_param[strlen(valor_param)-1] != '"' && valor_param[strlen(valor_param)-2] != '"'){

                            strcat(valor_param," ");
                            //printf("El problema esta aca3\n");
                            strcat(valor_param,token = strtok(NULL,"\""));
                            strcat(valor_param,"\"");
                            //printf("Valor: %s\n",valor_param);
                        }else{
                            //printf("TERMINA CON COMILLA\n");
                        }
                    }


                        for(int i = 0; valor_param[i+1] != '"' && i < strlen(valor_param); i++){
                            aux[i] = valor_param[i + 1];
                        }
                        //printf("Auxiliar: %s\n",aux);

                    }else{
                        strcpy(aux,valor_param);
                    }

                    printf("--Nombre: %s Valor: %s\n",nombre_param,aux);

                    if(strcasecmp(nombre_param,"-size::")==0){bandera_size = 1;size = atoi(aux);}else if
                    (strcasecmp(nombre_param,"-name::")==0){bandera_name = 1;strcpy(name,aux);} else if
                    (strcasecmp(nombre_param,"+unit::")==0){bandera_unit = 1;strcpy(unit,aux);} else if
                    (strcasecmp(nombre_param,"-path::")==0){bandera_path = 1;strcpy(path,aux);} else
                    {bandera_error = 1;}
                    //printf("El problema esta aca\n");

                }
                else
                {
                    printf("PARAMETRO ERRONEO\n");
                }

                if(token == NULL){token = NULL;printf("llego al finalm");}else{token = strtok(NULL,delimitador);}
                //printf("PROBLEMA!!!!\n");

            }
            if(bandera_size == 1 && bandera_name == 1 && bandera_path == 1 && bandera_error == 0){CreacionDisco(size,path,unit,name);}


        }


        else if(strcasecmp("fdisk",token) == 0)
        {

        }

        else if(strcasecmp("umount",token) == 0)
        {

        }

        else if(strcasecmp("mount",token) == 0)
        {

        }

        else if(strcasecmp("rmdisk",token) == 0)
        {

        }



        else
        {
            printf("HAY UN ERROR EN EL COMANDO\n");
        }

        }else{

            printf("ERROR!!!!!!!\n");
        }

        if(token == NULL){token = NULL;printf("llego al 2 final");}else{token = strtok(NULL,delimitador);}
    }

}

void Lector2(){

    char *token = NULL;
    char scanner[256];
    char buffer[256];
    char delimitador[] = " ";
    char param_name[256];
    char param_valor[256];

    memset(buffer,'\0',sizeof(buffer));
    memset(scanner,'\0',sizeof(scanner));

    fflush(stdin);
    fgets(scanner,256,stdin);

    strcpy(buffer,scanner);

    token = strtok(scanner,delimitador);

    if(strcasecmp("mkdisk",token) == 0){
        printf("estoy en mkdisk\n");
        int contador = strlen("mkdisk");
        printf("valor : %c\n",buffer[contador]);

        while(contador < strlen(buffer)){
            memset(param_name,'\0',sizeof(param_name));
            memset(param_valor,'\0',sizeof(param_valor));

            if(buffer[contador] == ' '){
                //printf("lei un espacio\n");
                contador++;
            }else if(buffer[contador] == '+' || buffer[contador] == '-'){
                //printf("llegue a un parametro\n");
                int jj = 0;
                while( contador < strlen(buffer)){
                    param_name[jj] = buffer[contador];
                    //printf("%c\n",buffer[contador]);
                    if(buffer[contador] == ':'){break;}
                    contador++;
                    jj++;
                }

                contador++;
                jj++;

                if(contador < strlen(buffer)){
                    if(buffer[contador] == ':'){param_name[jj] = buffer[contador];/*printf("token reconocido");*/}
                }else{break;}

                contador++;

                if(strcasecmp("-path::",param_name) == 0 || strcasecmp("-name::",param_name) == 0){
                    while(contador < strlen(buffer)){
                        if(buffer[contador] == ' '){
                            contador++;
                        }else if(buffer[contador] == '"'){

                            jj = 0;
                            contador++;
                            while(contador < strlen(buffer)){
                                if(buffer[contador] != '"'){
                                    param_valor[jj] = buffer[contador];
                                    contador++;
                                    jj++;
                                }else{
                                    break;
                                }
                            }

                        }else{
                            printf("SE ESPERABA CADENA\n");
                            break;
                        }
                    }

                }

                printf("Nombre: %s\n",param_name);
                printf("Valor: %s\n",param_valor);


            }else{
                printf("ES UN ERROR LEXICO\n");
                break;
            }

        }

    }else if(strcasecmp(token,"fdisk") == 0){


    }else{

        printf("COMANDO NO RECONOCIDO\n");
    }

}

int main()
{
//    printf("Proyecto Archivos\n");
    //CreacionDisco(1,"/home/jerduar/","M","prueba.dsk");
    /*int booleano = 1;
    printf("------------------------------- ARCHIVOS ------------------------------------\n");
    while(booleano == 1){

        Lector2();
    }*/

    //fdisk(2,"K","/home/jerduar/prueba.dsk",'p',"pru","fsds","nombre",12);

    MBR r;
    FILE *file = fopen("/home/jerduar/prueba.dsk","rb+");
    //printf("tamano mbr: %d\n",sizeof_MBR);
    fseek(file,0,SEEK_SET);
    fread(&r,sizeof_MBR,1,file);

    Partition p = r.mbr_partition_1;
    fclose(file);
    printf("%c\n",p.part_type);
    REP("mbr","/home/jerduar/archi/","3e","fasdfsdf");

    return 0;

}
