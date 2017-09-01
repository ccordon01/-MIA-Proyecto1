#ifndef DISK_H
#define DISK_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "estructurasdisk.h"
/*  Aqui se van a definir las estructuras necesarias para escribir en el archivo binario
 *  para la clase se usaran unicamente dos estructuras con fines demostrativos
 *  la primera almacenara los datos del estudiante y la cantidad de bloques:
 *      - nombre de tipo char[50]
 *      - carne de tipo int
 *      - bloques de tipo int
 *
 *  La siguiente estructura es para llevar el control de un bitmap, unicamente guardara el
 *  estado (1/0) para implementar un algoritmo de primer ajuste
 */

typedef struct datos{
    char nombre[50];
    int carne;
    int bloques;
}DATOS;

typedef struct bitmap{
    char estado;
}BM;


void crear_disco(char* nombre_disco, int tamano_disco,int carne,char* ruta_disco){
    char* comando = (char*)malloc(500);
        /*  Para crear un disco con contenido se usara el comando dd de la terminal de linux
         *  $ dd
         *  seguido de la especificacion de llenar de ceros el archivo
         *  $ dd if=/dev/zero
         *  luego se define la ruta del archivo
         *  $ dd if=/dev/zero of=/home/cyno/Desktop/
         *  especificamos la agrupacion, es decir de cuanto en cuanto se agruparan los bytes
         *  $ dd if=/dev/zero of=/home/cyno/Desktop/ bs=1MB
         *  y por ultimo la cantidad que queremos de la agrupacion es decir si agrupamos por MB
         *  la cantidad de megabytes que ocupara nuestro archivo
         *  $ dd if=/dev/zero of=/home/cyno/Desktop bs=1MB count=10
         */

        // "limpia" la variable
        char* megas=(char*)malloc(15);
        memset(&comando[0], 0, sizeof(comando));
        memset(&megas[0], 0, sizeof(megas));
        strcat(comando, "dd if=/dev/zero of=");
        strcat(comando, ruta_disco);
        strcat(comando, "/");
        strcat(comando, nombre_disco);
        strcat(comando, ".dsk ");
        strcat(comando, "bs=1MB count=");
        // Se debe pasar el entero a char con sprintf
        sprintf(megas, "%d", tamano_disco);
        strcat(comando, megas);
        system(comando); // ejecuta el comando en la terminal del SO
        free(comando);
        free(megas);
    // Una vez creado el archivo lo abrimos para poder ingresar las estructuras
    /*DATOS alumno;
    BM bm;*/
    /*  Para abrir el archivo usamos la funcion fopen que recibe como primer parametro la ruta y segundo
     *  el modo, se pueden usar los siguientes:
     *  "r" : abrir un archivo para lectura, el fichero debe existir.
     *  "w" : abrir un archivo para escritura, se crea si no existe o se sobreescribe si existe.
     *  "a" : abrir un archivo para escritura al final del contenido, si no existe se crea.
     *  "r+" : abrir un archivo para lectura y escritura, el fichero debe existir.
     *  "w+" : crear un archivo para lectura y escritura, se crea si no existe o se sobreescribe si existe.
     *  "r+b ó rb+" : Abre un archivo en modo binario para actualización (lectura y escritura).
     *  "rb" : Abre un archivo en modo binario para lectura.
     */
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    free(pathaux);
    double n = (double)((double)(((tamano_disco-5)*1024))-sizeof(SB))/((54*sizeof(BA)) + 3*sizeof(J) + sizeof(I));
    int numero_estructuras = floor(n);
    printf("\nReporte de Creacion de Disco \n");
    printf(" Cantidad inodos: %d \n",numero_estructuras);
    printf(" Canditad bloques archivo o carpeta: %d \n",(numero_estructuras*46));
    printf(" Canditad bloques indirectos: %d \n",(numero_estructuras*8));
    SB superBloque;
    superBloque.numero_inodos=numero_estructuras;
    superBloque.numero_bloques=(numero_estructuras*52);
    superBloque.tam_bloques = 76;
    superBloque.numero_magico = carne;
    superBloque.bloques_free = (numero_estructuras*52);
    superBloque.inodos_free = numero_estructuras;
    time_t tiempo = time(0);
    memset(&superBloque.mount_time,0,sizeof(superBloque.mount_time));
    struct tm *tlocal = localtime(&tiempo);
    strftime(superBloque.mount_time,128,"%d/%m/%y %H:%M:%S",tlocal);
    //Siguiente libre
    superBloque.free_bloque = 0;
    superBloque.free_inodo = 0;
    superBloque.free_journal = 0;
    //Apuntador Journal
    superBloque.journal = sizeof(SB);
    //Apuntadores Bitmap
    superBloque.bitmap_inodos = superBloque.journal + (3*numero_estructuras*sizeof(J));
    superBloque.bitmap_bloques = superBloque.bitmap_inodos + numero_estructuras;
    //Apuntadores
    superBloque.inodos = superBloque.bitmap_bloques + (numero_estructuras*54);
    superBloque.bloques = superBloque.inodos + (numero_estructuras*(sizeof(I)));
    //SUPERBLOQUE
    fseek(escritor, 0, SEEK_SET);
    fwrite(&superBloque,sizeof(SB),1,escritor);
    //free(superBloque);
    fseek(escritor, 0, SEEK_SET);
    SB info;
    fread(&info,sizeof(SB),1,escritor);
    printf(" Fecha de cracion: %s  \n",info.mount_time);
    printf(" Creado por: %d \n",info.numero_magico);
    /*printf(" journal: %d \n",info.journal);
    printf(" bitmap_inodos: %d \n",info.bitmap_inodos);
    printf(" bitmap_bloques: %d \n",info.bitmap_bloques);
    printf(" inodos: %d \n",info.inodos);
    printf(" bloques: %d \n\n",info.bloques);*/
    //JOURNALING
    fseek(escritor, info.journal, SEEK_SET);
    int contador=info.journal;
    J journal;
    memset(&journal.descripsion,0,sizeof(journal.descripsion));
    strcat(journal.descripsion,"-");
    for (int var = 0; var < (3*numero_estructuras); ++var) {
        fwrite(&journal,sizeof(J),1,escritor);
        contador+=sizeof(J);
    }
    //printf(" journal: %d \n",contador);
    //BITMAP DE INODOS
    BM bitm;
    bitm.estado='0';
    for (int var = 0; var < numero_estructuras; ++var) {
        fwrite(&bitm,sizeof(BM),1,escritor);
        contador+=sizeof(BM);
    }
    //printf(" bitmap_inodos: %d \n",contador);
    //BITMAP DE BLOQUES
    for (int var = 0; var < (numero_estructuras*54); ++var) {
        fwrite(&bitm,sizeof(BM),1,escritor);
        contador+=sizeof(BM);
    }
    //printf(" bitmap_bloques: %d \n",contador);
    //INODOS
    I in;
    for (int var = 0; var < numero_estructuras; ++var) {
        fwrite(&in,sizeof(I),1,escritor);
    }
    //BLOQUES
    BA bloqa;
    for (int var = 0; var < (numero_estructuras*54); ++var) {
        fwrite(&bloqa,sizeof(BA),1,escritor);
    }


    //Creacion de la carpeta root ("/")
    //Inodo Root
    I rootI;
    rootI.id=info.free_inodo;
    time_t tiempo1 = time(0);
    struct tm *tlocal1 = localtime(&tiempo1);
    memset(&rootI.fecha,0,sizeof(rootI.fecha));
    strftime(rootI.fecha,128,"%d/%m/%y %H:%M:%S",tlocal1);
    rootI.bloques_asign=0;
    rootI.tam=0;
    rootI.tipo_dato=0;
    for (int var = 0; var < 4; ++var) {
       rootI.bloque_dir[var]=-1;
    }
    rootI.bloque_ind1=-1;
    rootI.bloque_ind2=-1;

    //Bloque Carpeta Root
    BC bloqueRoot;
    memset(&bloqueRoot.padre,0,sizeof(bloqueRoot.padre));
    memset(&bloqueRoot.nombre,0,sizeof(bloqueRoot.nombre));
    strcat(bloqueRoot.padre,"0");
    strcat(bloqueRoot.nombre,"/");
    for (int var = 0; var < 6; ++var) {
       bloqueRoot.hijo[var]=-1;
    }
    rootI.bloque_dir[0]=0;
    BM rootBM;
    rootBM.estado='1';
    //Actualizar Bitmap
    fseek(escritor, info.bitmap_inodos, SEEK_SET);
    fwrite(&rootBM,sizeof(BM),1,escritor);
    fseek(escritor, info.bitmap_bloques, SEEK_SET);
    fwrite(&rootBM,sizeof(BM),1,escritor);
    //Agregar inodo y bloque
    fseek(escritor,info.inodos,SEEK_SET);
    fwrite(&rootI,sizeof(I),1,escritor);

    fseek(escritor,info.bloques,SEEK_SET);
    fwrite(&bloqueRoot,sizeof(BC),1,escritor);

    //Journal
    J infoRoot;
    memset(&infoRoot.fecha,0,sizeof(infoRoot.fecha));
    strftime(infoRoot.fecha,128,"%d/%m/%y %H:%M:%S",tlocal1);
    memset(&infoRoot.descripsion,0,sizeof(infoRoot.descripsion));
    strcat(infoRoot.descripsion,"Creacion de la carpeta root (\"/\")");
    fseek(escritor,info.journal,SEEK_SET);
    fwrite(&infoRoot,sizeof(J),1,escritor);
    //Actualizar SuperBloque
    info.free_bloque=1;
    info.free_inodo = 1;
    info.free_journal=1;
    fseek(escritor, 0, SEEK_SET);
    fwrite(&info,sizeof(SB),1,escritor);
    //printf("%d",numero_estructuras);
    /*  fseek pone el puntero en la posicion que le indicamos en el segundo parametro
     *  primer parametro se manda la variable de tipo FILE*, segundo la posicion dentro del archivo
     *  y la tercera es el modo, ademas de usar SEEK_SET que solo lo setea, una funcion importante es
     *  SEEK_CUR que de la posicion actual se desplaza, para ir hacia atras se multiplica por -1 el segundo
     *  parametro. Por ejemplo: fseek(escritor, -1*sizeof(DATOS), SEEK_CUR);
     *  Acontinuacion solo nos situamos al inicio del archivo para escribir los datos
      */
    //fseek(escritor, 0, SEEK_SET);
    // Llenamos de informacion la estructura de alumno
    /*strcpy(alumno.nombre, nombre_estudiante);
    alumno.carne = (int)carne;
    alumno.bloques = bloques;*/
    /* Una vez lleno lo escribimos en el archivo con la funcion fwrite
     * Parametros de fwrite:
     * 1. Puntero a la direccion de memoria donde se encuentran los datos a escribir
     * 2. Tamano de CADA registro
     * 3. CANTIDAD de registros a escribir
     * 4. FILE abierto previamente
     */
    //fwrite(&alumno, sizeof(DATOS), 1, escritor);
    // Escribimos el bitmap con la cantidad de bloques que el usuario ingreso
    /*bm.estado = '0';
    int i = 0;
    for (i = 0; i < bloques; i++){
        fwrite(&bm, sizeof(BM), 1, escritor);
    }*/
    // Cerramos el archivo con fclose
    fclose(escritor);
    //free(escritor);
    //free(tamano_disco);
}

void mostrar_disco(char* nombre_disco, char* ruta_disco){
    char pathaux[150];
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");

    FILE* escritor = fopen(pathaux, "rb+");
    fseek(escritor, 0, SEEK_SET);
    DATOS alumno;
    BM bm;
    // fread sera la herramienta para obtener los datos y los parametros son los mismos que el fwrite
    fread(&alumno, sizeof(DATOS), 1, escritor);
    printf("------------------------------\n");
    printf(" Nombre: %s\n", alumno.nombre);
    printf(" Carne: %d\n", alumno.carne);
    printf(" Bloques que tiene el disco: %d\n", alumno.bloques);
    printf("------------------------------\n");

    // Leemos con un ciclo el bitmap
    int i = 0;
    for (i = 0; i < alumno.bloques; i++){
        if(i%10 == 0){
            printf("\n");
        }
        fread(&bm, sizeof(BM), 1, escritor);
        printf("| %c |", bm.estado);
    }
    printf("\n");
    fclose(escritor);
}

void ingresar_bloques(char* nombre_disco, int cantidad , char* ruta_disco){
    // Ingresar bloques usando el primer ajuste
    int bloques_disco = 0;
    char pathaux[150];
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");

    FILE* escritor = fopen(pathaux, "rb+");
    fseek(escritor, 0, SEEK_SET);

    DATOS aux_datos;
    fread(&aux_datos, sizeof(DATOS), 1, escritor);
    bloques_disco = aux_datos.bloques;
    int referencia, contador = 0;
    BM bm;

    // PRIMER AJUSTE
    int i = 0;
    for (i = 0; i < bloques_disco; i++){
        referencia++;
        fread(&bm, sizeof(BM), 1, escritor);
        if(bm.estado == '0'){
            contador++;
        }else{
            contador = 0;
        }
        if(contador == cantidad){
            referencia = referencia - cantidad; // cantidad o contador
            break;
        }
    }
    bm.estado = '1';
    fseek(escritor, referencia + sizeof(DATOS), SEEK_SET);
    //fwrite(&bm, sizeof(BM), cantidad, escritor);
    int j = 0;
    for (j = 0; j < cantidad; j++){
        fwrite(&bm, sizeof(BM), 1, escritor);
    }
    fclose(escritor);
}

void borrar_bloques(char* nombre_disco, int inicio, int cantidad,char* ruta_disco){
    char pathaux[150];
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");

    FILE* escritor = fopen(pathaux, "rb+");
    fseek(escritor, inicio - 1 + sizeof(DATOS) , SEEK_SET);
    BM bm;
    bm.estado = '0';

    int i = 0;
    for (i = 0; i < cantidad; i++){
        fwrite(&bm, sizeof(BM), 1, escritor);
    }
    fclose(escritor);
}

int montar_disco(char* nombre_disco,char* ruta_disco){
    FILE *f1;
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    f1 = fopen (pathaux, "rb");
    if (f1 == NULL)
    {
       return 0;
    }
    FILE* escritor = fopen(pathaux, "rb+");
    free(pathaux);
    fseek(escritor, 0, SEEK_SET);
    SB info;
    fread(&info,sizeof(SB),1,escritor);
    printf(" Ultima fecha que se monto el disco: %s  \n",info.mount_time);
    time_t tiempo1 = time(0);
    struct tm *tlocal1 = localtime(&tiempo1);
    strftime(info.mount_time,128,"%d/%m/%y %H:%M:%S",tlocal1);
    printf(" Fecha actual: %s  \n",info.mount_time);
    fseek(escritor, 0, SEEK_SET);
    fwrite(&info,sizeof(SB),1,escritor);
    fclose(escritor);
    return 1;
}

void estado_bloques(char* nombre_disco,char* ruta_disco){
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    free(pathaux);
    fseek(escritor, 0, SEEK_SET);
    SB info;
    BM bm;
    fread(&info,sizeof(SB),1,escritor);
    fseek(escritor, info.bitmap_bloques, SEEK_SET);
    // Leemos con un ciclo el bitmap
    printf("                                Bitmap De Bloques\n");
    int i = 0;
    printf("            ");
    for (i = 0; i < (info.numero_bloques+ 10 - (info.numero_bloques%10)); i++){
        if(i%10 == 0){
            printf("\n");
            printf("              ");
        }
        if(i < info.numero_bloques){
        fread(&bm, sizeof(BM), 1, escritor);
        printf("| %c |", bm.estado);
        }
        else {
        printf("| - |");
        }
    }
    printf("\n \n\ \n");
    fclose(escritor);
}

void estado_inodos(char* nombre_disco,char* ruta_disco){
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    free(pathaux);
    fseek(escritor, 0, SEEK_SET);
    SB info;
    BM bm;
    fread(&info,sizeof(SB),1,escritor);
    fseek(escritor, info.bitmap_inodos, SEEK_SET);
    // Leemos con un ciclo el bitmap
    printf("                                Bitmap De Inodos\n");
    int i = 0;
    printf("              ");
    for (i = 0; i < (info.numero_inodos+ 10 - (info.numero_inodos%10)); i++){
        if(i%10 == 0){
            printf("\n");
            printf("              ");
        }
        if(i < info.numero_inodos){
        fread(&bm, sizeof(BM), 1, escritor);
        printf("| %c |", bm.estado);
        }
        else {
        printf("| - |");
        }
    }
    printf("\n");
    fclose(escritor);
}

void bitacora(char* nombre_disco,char* ruta_disco){
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    free(pathaux);
    fseek(escritor, 0, SEEK_SET);
    SB info;
    fread(&info,sizeof(SB),1,escritor);
    J journalReporte;
    fseek(escritor, info.journal, SEEK_SET);
    printf("   Fecha             | Descripcion \n");
    for (int var = 0; var < (info.numero_inodos*3); ++var) {
        fread(&journalReporte, sizeof(J), 1, escritor);
        if (strcmp(journalReporte.descripsion, "-") == 0) {
            break;
        } else {
            printf("   %s |",journalReporte.fecha);
            printf(" %s \n",journalReporte.descripsion);
        }
    }
    fclose(escritor);
}

void crear_archivo(char* nombre_disco,char* ruta_disco,char* path,char* contenido,char* nombre_archivo){

}

int len_palabra(char *cadena, int inicio, char separador)
{
    int len=0, i=inicio;
    while(cadena[i]!=separador&&cadena[i]!='\0')
    {
        len++;
        i++;
    }
    return len;
}

char **split(char *cadena, char separador)
{
    int num_palabras=1, len_cadena=strlen(cadena), i, j=1, *inicios;
    for(i=0;i<len_cadena;i++)
    {
        if(cadena[i]==separador)
        {
            num_palabras++;
        }
    }
    inicios=malloc(sizeof(int)*num_palabras);
    inicios[0]=0;
    for(i=1;i<len_cadena;i++)
    {
        if(cadena[i]==separador)
        {
            inicios[j]=i+1;
            j++;
        }
    }
    char **retorno=malloc(sizeof(char *)*num_palabras+1);
    retorno[num_palabras]=NULL;
    for(i=0;i<num_palabras;i++)
    {
        j=len_palabra(cadena, inicios[i], separador);
        retorno[i]=(char *)malloc(sizeof(char)*j);
        strncpy(retorno[i], &cadena[inicios[i]], j);
        retorno[i][j]='\0';
    }
    free(inicios);
    return retorno;
}
#endif // DISK_H
