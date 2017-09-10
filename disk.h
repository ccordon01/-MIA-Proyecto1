#ifndef DISK_H
#define DISK_H
#include "split.h"
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
    //superBloque.free_journal = 0;
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
    /*BC bloqueRoot;
    memset(&bloqueRoot.padre,0,sizeof(bloqueRoot.padre));
    memset(&bloqueRoot.nombre,0,sizeof(bloqueRoot.nombre));
    strcat(bloqueRoot.padre,"0");
    strcat(bloqueRoot.nombre,"/");
    for (int var = 0; var < 6; ++var) {
       bloqueRoot.hijo[var]=-1;
    }
    rootI.bloque_dir[0]=0;*/
    BM rootBM;
    rootBM.estado='1';
    //Actualizar Bitmap
    fseek(escritor, info.bitmap_inodos, SEEK_SET);
    fwrite(&rootBM,sizeof(BM),1,escritor);
    /*fseek(escritor, info.bitmap_bloques, SEEK_SET);
    fwrite(&rootBM,sizeof(BM),1,escritor);*/
    //Agregar inodo y bloque
    fseek(escritor,info.inodos,SEEK_SET);
    fwrite(&rootI,sizeof(I),1,escritor);

    /*fseek(escritor,info.bloques,SEEK_SET);
    fwrite(&bloqueRoot,sizeof(BC),1,escritor);*/

    //Journal
    J infoRoot;
    memset(&infoRoot.fecha,0,sizeof(infoRoot.fecha));
    strftime(infoRoot.fecha,128,"%d/%m/%y %H:%M:%S",tlocal1);
    memset(&infoRoot.descripsion,0,sizeof(infoRoot.descripsion));
    strcat(infoRoot.descripsion,"Creacion de la carpeta root (\"/\")");
    fseek(escritor,info.journal,SEEK_SET);
    fwrite(&infoRoot,sizeof(J),1,escritor);
    //Actualizar SuperBloque
    //info.free_bloque=1;
    info.free_inodo = 1;
    //info.free_journal=1;
    fseek(escritor, 0, SEEK_SET);
    fwrite(&info,sizeof(SB),1,escritor);
    fclose(escritor);
    //Crear archivo user.txt
    char **vacio;
    crear_archivo(nombre_disco,ruta_disco,"/",vacio,"1,G,root\n1,U,root,root,201504427\n","users.txt",1);

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
    //free(escritor);
    //free(tamano_disco);
}

/*
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
*/

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

void crear_archivo(char* nombre_disco,char* ruta_disco,char* path,char **pathE,char* contenido,char* nombre_archivo,int tipoA){
    int crear=0;
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    SB info;
    fseek(escritor, 0, SEEK_SET);
    fread(&info,sizeof(SB),1,escritor);
    I root;
    fseek(escritor,info.inodos,SEEK_SET);
    fread(&root,sizeof(I),1,escritor);
    //Inodo del archivo a crear
    I archivoNuevo;
    archivoNuevo.id=info.free_inodo;
    time_t tiempo1 = time(0);
    struct tm *tlocal1 = localtime(&tiempo1);
    memset(&archivoNuevo.fecha,0,sizeof(archivoNuevo.fecha));
    strftime(archivoNuevo.fecha,128,"%d/%m/%y %H:%M:%S",tlocal1);
    archivoNuevo.bloques_asign=0;
    archivoNuevo.tam=strlen(contenido);
    archivoNuevo.tipo_dato=0;
    for (int var = 0; var < 4; ++var) {
       archivoNuevo.bloque_dir[var]=-1;
    }
    archivoNuevo.bloque_ind1=-1;
    archivoNuevo.bloque_ind2=-1;
    //Metodo para crear inodo nuevo
    fclose(escritor);
    info=crear_inodo(archivoNuevo,nombre_disco,ruta_disco,info);
    /*//Actualizar superbloque
    escritor = fopen(pathaux, "rb+");
    fseek(escritor, 0, SEEK_SET);
    fread(&info,sizeof(SB),1,escritor);*/
    //Bloque de carpeta que apunta al archivo nuevo
    BC bcnuevo;
    memset(&bcnuevo.padre,0,sizeof(bcnuevo.padre));
    memset(&bcnuevo.nombre,0,sizeof(bcnuevo.nombre));
    strcat(bcnuevo.padre,"/");
    strcat(bcnuevo.nombre,nombre_archivo);
    for (int var = 0; var < 6; ++var) {
       bcnuevo.hijo[var]=-1;
    }
    bcnuevo.hijo[0]=archivoNuevo.id;
    if (strcmp(path, "/") == 0) {
        //Declarar en la raiz
        int ww = -1;
        for (int var = 0; var < 4; ++var) {
            if (root.bloque_dir[var]==-1) {
                ww = var;
            }
        }
        if (ww==-1) {
            //Bloques indirectos
            //Nivel 1
            BI bloqueNivel1;
            if (root.bloque_ind1==-1) {
                root.bloque_ind1=info.free_bloque;
                bloqueNivel1.nivel=1;
                for (int var = 0; var < 6; ++var) {
                    bloqueNivel1.apuntador[var]=-1;
                }
                info=crear_bloqueI(bloqueNivel1,nombre_disco,ruta_disco,info);
                bloqueNivel1.apuntador[0]=info.free_bloque;
                info=crear_bloqueC(bcnuevo,nombre_disco,ruta_disco,info);
                crear=1;
                ww=0;
            }
            else{
                fseek(escritor,info.bloques+(sizeof(BI)*root.bloque_ind1),SEEK_SET);
                fread(&bloqueNivel1,sizeof(BI),1,escritor);
                for (int var = 0; var < 6; ++var) {
                    if (bloqueNivel1.apuntador[var]==-1) {
                        ww = var;
                        break;
                    }
                }
                bloqueNivel1.apuntador[ww]=info.free_bloque;
                fclose(escritor);
                info=crear_bloqueC(bcnuevo,nombre_disco,ruta_disco,info);
                crear=1;
            }
            //Crear contenido
            int tamArchivo = archivoNuevo.tam;
            int lectorPos=0;
                //Bloques Directos
                for (int var = 0; var < 4; ++var) {
                        if (tamArchivo<=24) {
                            char subbuff[tamArchivo + 1];
                            memcpy( subbuff, &contenido[lectorPos], tamArchivo);
                            subbuff[tamArchivo] = '\0';
                            BA bloqueContenido;
                            memset(&bloqueContenido.padre,0,sizeof(bloqueContenido.padre));
                            memset(&bloqueContenido.nombre,0,sizeof(bloqueContenido.nombre));
                            memset(&bloqueContenido.informacion,0,sizeof(bloqueContenido.informacion));
                            strcat(bloqueContenido.padre,"/");
                            strcat(bloqueContenido.nombre,nombre_archivo);
                            strcat(bloqueContenido.informacion,subbuff);
                            archivoNuevo.bloque_dir[var]=info.free_bloque;
                            info=crear_bloqueA(bloqueContenido,nombre_disco,ruta_disco,info);
                            tamArchivo=0;
                            break;
                        } else {
                            char subbuff[25];
                            memcpy( subbuff, &contenido[lectorPos], 24);
                            subbuff[24] = '\0';
                            BA bloqueContenido;
                            memset(&bloqueContenido.padre,0,sizeof(bloqueContenido.padre));
                            memset(&bloqueContenido.nombre,0,sizeof(bloqueContenido.nombre));
                            memset(&bloqueContenido.informacion,0,sizeof(bloqueContenido.informacion));
                            strcat(bloqueContenido.padre,"/");
                            strcat(bloqueContenido.nombre,nombre_archivo);
                            strcat(bloqueContenido.informacion,subbuff);
                            archivoNuevo.bloque_dir[var]=info.free_bloque;
                            info=crear_bloqueA(bloqueContenido,nombre_disco,ruta_disco,info);
                            lectorPos=lectorPos+24;
                        }
                    tamArchivo=tamArchivo-24;
                }
                if (tamArchivo>0) {
                        //Bloques indirectos
                        //Nivel 1
                        BI bloqueNivel1;
                        if (root.bloque_ind1==-1) {
                            root.bloque_ind1=info.free_bloque;
                            bloqueNivel1.nivel=1;
                            for (int var = 0; var < 6; ++var) {
                                bloqueNivel1.apuntador[var]=-1;
                            }
                            info=crear_bloqueI(bloqueNivel1,nombre_disco,ruta_disco,info);
                            //bloqueNivel1.apuntador[0]=info.free_bloque;
                            for (int var = 0; var < 6; ++var) {
                                    if (tamArchivo<=24) {
                                        char subbuff[tamArchivo + 1];
                                        memcpy( subbuff, &contenido[lectorPos], tamArchivo);
                                        subbuff[tamArchivo] = '\0';
                                        BA bloqueContenido;
                                        memset(&bloqueContenido.padre,0,sizeof(bloqueContenido.padre));
                                        memset(&bloqueContenido.nombre,0,sizeof(bloqueContenido.nombre));
                                        memset(&bloqueContenido.informacion,0,sizeof(bloqueContenido.informacion));
                                        strcat(bloqueContenido.padre,"/");
                                        strcat(bloqueContenido.nombre,nombre_archivo);
                                        strcat(bloqueContenido.informacion,subbuff);
                                        bloqueNivel1.apuntador[var]=info.free_bloque;
                                        info=crear_bloqueA(bloqueContenido,nombre_disco,ruta_disco,info);
                                        break;
                                    } else {
                                        char subbuff[25];
                                        memcpy( subbuff, &contenido[lectorPos], 24);
                                        subbuff[24] = '\0';
                                        BA bloqueContenido;
                                        memset(&bloqueContenido.padre,0,sizeof(bloqueContenido.padre));
                                        memset(&bloqueContenido.nombre,0,sizeof(bloqueContenido.nombre));
                                        memset(&bloqueContenido.informacion,0,sizeof(bloqueContenido.informacion));
                                        strcat(bloqueContenido.padre,"/");
                                        strcat(bloqueContenido.nombre,nombre_archivo);
                                        strcat(bloqueContenido.informacion,subbuff);
                                        bloqueNivel1.apuntador[var]=info.free_bloque;
                                        info=crear_bloqueA(bloqueContenido,nombre_disco,ruta_disco,info);
                                        lectorPos=lectorPos+24;
                                    }
                                tamArchivo=tamArchivo-24;
                            }
                        }
                        //Actualizar BloqueNivel 1
                        escritor = fopen(pathaux, "rb+");
                        fseek(escritor,info.bloques+(sizeof(BI)*root.bloque_ind1),SEEK_SET);
                        fwrite(&bloqueNivel1,sizeof(BI),1,escritor);
                        fclose(escritor);
                }
            //Actualizar BloqueNivel 1
            escritor = fopen(pathaux, "rb+");
            fseek(escritor,info.bloques+(sizeof(BI)*root.bloque_ind1),SEEK_SET);
            fwrite(&bloqueNivel1,sizeof(BI),1,escritor);
            fclose(escritor);
        } else {
            //Asignar en bloque directo
            root.bloque_dir[ww]=info.free_bloque;
            //Crear bloque de carpeta
            //fclose(escritor);
            info=crear_bloqueC(bcnuevo,nombre_disco,ruta_disco,info);
            crear=1;
            /*//Actualizar superbloque
            fseek(escritor, 0, SEEK_SET);
            fread(&info,sizeof(SB),1,escritor);*/
            //escritor = fopen(pathaux, "rb+");
            //Llenar conenido en bloques directos
            int tamArchivo = archivoNuevo.tam;
            int lectorPos=0;
                //Bloques Directos
                for (int var = 0; var < 4; ++var) {
                        if (tamArchivo<=24) {
                            char subbuff[tamArchivo + 1];
                            memcpy( subbuff, &contenido[lectorPos], tamArchivo);
                            subbuff[tamArchivo] = '\0';
                            BA bloqueContenido;
                            memset(&bloqueContenido.padre,0,sizeof(bloqueContenido.padre));
                            memset(&bloqueContenido.nombre,0,sizeof(bloqueContenido.nombre));
                            memset(&bloqueContenido.informacion,0,sizeof(bloqueContenido.informacion));
                            strcat(bloqueContenido.padre,"/");
                            strcat(bloqueContenido.nombre,nombre_archivo);
                            strcat(bloqueContenido.informacion,subbuff);
                            archivoNuevo.bloque_dir[var]=info.free_bloque;
                            info=crear_bloqueA(bloqueContenido,nombre_disco,ruta_disco,info);
                            tamArchivo=0;
                            break;
                        } else {
                            char subbuff[25];
                            memcpy( subbuff, &contenido[lectorPos], 24);
                            subbuff[24] = '\0';
                            BA bloqueContenido;
                            memset(&bloqueContenido.padre,0,sizeof(bloqueContenido.padre));
                            memset(&bloqueContenido.nombre,0,sizeof(bloqueContenido.nombre));
                            memset(&bloqueContenido.informacion,0,sizeof(bloqueContenido.informacion));
                            strcat(bloqueContenido.padre,"/");
                            strcat(bloqueContenido.nombre,nombre_archivo);
                            strcat(bloqueContenido.informacion,subbuff);
                            archivoNuevo.bloque_dir[var]=info.free_bloque;
                            info=crear_bloqueA(bloqueContenido,nombre_disco,ruta_disco,info);
                            lectorPos=lectorPos+24;
                        }
                    tamArchivo=tamArchivo-24;
                }
                if (tamArchivo>0) {
                        //Bloques indirectos
                        //Nivel 1
                        BI bloqueNivel1;
                        if (root.bloque_ind1==-1) {
                            root.bloque_ind1=info.free_bloque;
                            bloqueNivel1.nivel=1;
                            for (int var = 0; var < 6; ++var) {
                                bloqueNivel1.apuntador[var]=-1;
                            }
                            info=crear_bloqueI(bloqueNivel1,nombre_disco,ruta_disco,info);
                            //bloqueNivel1.apuntador[0]=info.free_bloque;
                            for (int var = 0; var < 6; ++var) {
                                    if (tamArchivo<=24) {
                                        char subbuff[tamArchivo + 1];
                                        memcpy( subbuff, &contenido[lectorPos], tamArchivo);
                                        subbuff[tamArchivo] = '\0';
                                        BA bloqueContenido;
                                        memset(&bloqueContenido.padre,0,sizeof(bloqueContenido.padre));
                                        memset(&bloqueContenido.nombre,0,sizeof(bloqueContenido.nombre));
                                        memset(&bloqueContenido.informacion,0,sizeof(bloqueContenido.informacion));
                                        strcat(bloqueContenido.padre,"/");
                                        strcat(bloqueContenido.nombre,nombre_archivo);
                                        strcat(bloqueContenido.informacion,subbuff);
                                        bloqueNivel1.apuntador[var]=info.free_bloque;
                                        info=crear_bloqueA(bloqueContenido,nombre_disco,ruta_disco,info);
                                        break;
                                    } else {
                                        char subbuff[25];
                                        memcpy( subbuff, &contenido[lectorPos], 24);
                                        subbuff[24] = '\0';
                                        BA bloqueContenido;
                                        memset(&bloqueContenido.padre,0,sizeof(bloqueContenido.padre));
                                        memset(&bloqueContenido.nombre,0,sizeof(bloqueContenido.nombre));
                                        memset(&bloqueContenido.informacion,0,sizeof(bloqueContenido.informacion));
                                        strcat(bloqueContenido.padre,"/");
                                        strcat(bloqueContenido.nombre,nombre_archivo);
                                        strcat(bloqueContenido.informacion,subbuff);
                                        bloqueNivel1.apuntador[var]=info.free_bloque;
                                        info=crear_bloqueA(bloqueContenido,nombre_disco,ruta_disco,info);
                                        lectorPos=lectorPos+24;
                                    }
                                tamArchivo=tamArchivo-24;
                            }
                        }
                        //Actualizar BloqueNivel 1
                        escritor = fopen(pathaux, "rb+");
                        fseek(escritor,info.bloques+(sizeof(BI)*root.bloque_ind1),SEEK_SET);
                        fwrite(&bloqueNivel1,sizeof(BI),1,escritor);
                        fclose(escritor);
                }
        }
    } else {
        //En otros lados vergas
        escritor = fopen(pathaux, "rb+");
        int nInodo=0;
        int i;
        int ic=0;
        for(i=0;pathE[i]!=NULL;i++)
        {
            if (!(ic==1 && nInodo==0)) {
                ic++;
                I tempIn;
                fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
                fread(&tempIn,sizeof(I),1,escritor);
                //Revisar bloques directos
                for (int var = 0; var < 4; ++var) {
                    if (tempIn.bloque_dir[var]!=-1) {
                        BC bloqueCarpeta;
                        fseek(escritor,info.bloques+(tempIn.bloque_dir[var]*sizeof(BC)),SEEK_SET);
                        fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                        if (strcmp(pathE[i],bloqueCarpeta.nombre)==0) {
                            nInodo=bloqueCarpeta.hijo[0];
                            break;
                        }
                    }
                }
                //printf("%s\n", pathE[i]);
            }
            free(pathE[i]);
        }
        free(pathE);
        if (nInodo!=0) {

            I padre;
            fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
            fread(&padre,sizeof(I),1,escritor);
            int ww = -1;
            for (int var = 0; var < 4; ++var) {
                if (padre.bloque_dir[var]==-1) {
                    ww = var;
                    break;
                }
            }
            //Asignar en bloque directo
            padre.bloque_dir[ww]=info.free_bloque;
            //Crear bloque de carpeta
            fclose(escritor);
            info=crear_bloqueC(bcnuevo,nombre_disco,ruta_disco,info);
            crear=1;
            /*//Actualizar superbloque
            fseek(escritor, 0, SEEK_SET);
            fread(&info,sizeof(SB),1,escritor);*/
            //escritor = fopen(pathaux, "rb+");
            //Llenar conenido en bloques directos
            int tamArchivo = archivoNuevo.tam;
            int lectorPos=0;
                //Bloques Directos
                for (int var = 0; var < 4; ++var) {
                        if (tamArchivo<=24) {
                            char subbuff[tamArchivo + 1];
                            memcpy( subbuff, &contenido[lectorPos], tamArchivo);
                            subbuff[tamArchivo] = '\0';
                            BA bloqueContenido;
                            memset(&bloqueContenido.padre,0,sizeof(bloqueContenido.padre));
                            memset(&bloqueContenido.nombre,0,sizeof(bloqueContenido.nombre));
                            memset(&bloqueContenido.informacion,0,sizeof(bloqueContenido.informacion));
                            strcat(bloqueContenido.padre,"/");
                            strcat(bloqueContenido.nombre,nombre_archivo);
                            strcat(bloqueContenido.informacion,subbuff);
                            archivoNuevo.bloque_dir[var]=info.free_bloque;
                            info=crear_bloqueA(bloqueContenido,nombre_disco,ruta_disco,info);
                            break;
                        } else {
                            char subbuff[25];
                            memcpy( subbuff, &contenido[lectorPos], 24);
                            subbuff[24] = '\0';
                            BA bloqueContenido;
                            memset(&bloqueContenido.padre,0,sizeof(bloqueContenido.padre));
                            memset(&bloqueContenido.nombre,0,sizeof(bloqueContenido.nombre));
                            memset(&bloqueContenido.informacion,0,sizeof(bloqueContenido.informacion));
                            strcat(bloqueContenido.padre,"/");
                            strcat(bloqueContenido.nombre,nombre_archivo);
                            strcat(bloqueContenido.informacion,subbuff);
                            archivoNuevo.bloque_dir[var]=info.free_bloque;
                            info=crear_bloqueA(bloqueContenido,nombre_disco,ruta_disco,info);
                            lectorPos=lectorPos+24;
                        }
                    tamArchivo=tamArchivo-24;
                }
                if (tamArchivo>0) {
                        //Bloques indirectos
                        //Nivel 1
                        BI bloqueNivel1;
                        if (archivoNuevo.bloque_ind1==-1) {
                            archivoNuevo.bloque_ind1=info.free_bloque;
                            bloqueNivel1.nivel=1;
                            for (int var = 0; var < 6; ++var) {
                                bloqueNivel1.apuntador[var]=-1;
                            }
                            info=crear_bloqueI(bloqueNivel1,nombre_disco,ruta_disco,info);
                            //bloqueNivel1.apuntador[0]=info.free_bloque;
                            for (int var = 0; var < 6; ++var) {
                                    if (tamArchivo<=24) {
                                        char subbuff[tamArchivo + 1];
                                        memcpy( subbuff, &contenido[lectorPos], tamArchivo);
                                        subbuff[tamArchivo] = '\0';
                                        BA bloqueContenido;
                                        memset(&bloqueContenido.padre,0,sizeof(bloqueContenido.padre));
                                        memset(&bloqueContenido.nombre,0,sizeof(bloqueContenido.nombre));
                                        memset(&bloqueContenido.informacion,0,sizeof(bloqueContenido.informacion));
                                        strcat(bloqueContenido.padre,"/");
                                        strcat(bloqueContenido.nombre,nombre_archivo);
                                        strcat(bloqueContenido.informacion,subbuff);
                                        bloqueNivel1.apuntador[var]=info.free_bloque;
                                        info=crear_bloqueA(bloqueContenido,nombre_disco,ruta_disco,info);
                                        break;
                                    } else {
                                        char subbuff[25];
                                        memcpy( subbuff, &contenido[lectorPos], 24);
                                        subbuff[24] = '\0';
                                        BA bloqueContenido;
                                        memset(&bloqueContenido.padre,0,sizeof(bloqueContenido.padre));
                                        memset(&bloqueContenido.nombre,0,sizeof(bloqueContenido.nombre));
                                        memset(&bloqueContenido.informacion,0,sizeof(bloqueContenido.informacion));
                                        strcat(bloqueContenido.padre,"/");
                                        strcat(bloqueContenido.nombre,nombre_archivo);
                                        strcat(bloqueContenido.informacion,subbuff);
                                        bloqueNivel1.apuntador[var]=info.free_bloque;
                                        info=crear_bloqueA(bloqueContenido,nombre_disco,ruta_disco,info);
                                        lectorPos=lectorPos+24;
                                    }
                                tamArchivo=tamArchivo-24;
                            }
                        }
                        //Actualizar BloqueNivel 1
                        escritor = fopen(pathaux, "rb+");
                        fseek(escritor,info.bloques+(sizeof(BI)*archivoNuevo.bloque_ind1),SEEK_SET);
                        fwrite(&bloqueNivel1,sizeof(BI),1,escritor);
                        fclose(escritor);
                }
            crear_inodo(padre,nombre_disco,ruta_disco,info);
        }
    }
    if (crear==1) {
        crear_inodo(archivoNuevo,nombre_disco,ruta_disco,info);
    }
    escritor = fopen(pathaux, "rb+");
    fseek(escritor,info.inodos,SEEK_SET);
    fwrite(&root,sizeof(I),1,escritor);
    fseek(escritor, 0, SEEK_SET);
    fwrite(&info,sizeof(SB),1,escritor);
    free(pathaux);
    fclose(escritor);
    if (crear==1 && tipoA==1) {
        char* descrip=(char*)malloc(140);
        memset(&descrip[0], 0, sizeof(descrip));
        strcat(descrip, "Se creo el archivo \"");
        strcat(descrip, nombre_archivo);
        strcat(descrip, "\" en ");
        strcat(descrip, path);
        bitacoraReporte(descrip,nombre_disco,ruta_disco);
        free(descrip);
        printf("   Archivo creado correctamente!\n");
    } else {
        if(tipoA==1){
        printf("   No se pudo crear el archivo!\n");
        }
    }
}

void crear_carpeta(char* nombre_disco,char* ruta_disco,char* path,char **pathE,char* nombre_archivo){
    int crear=0;
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    SB info;
    fseek(escritor, 0, SEEK_SET);
    fread(&info,sizeof(SB),1,escritor);
    I root;
    fseek(escritor,info.inodos,SEEK_SET);
    fread(&root,sizeof(I),1,escritor);
    //Inodo del archivo a crear
    I archivoNuevo;
    archivoNuevo.id=info.free_inodo;
    time_t tiempo1 = time(0);
    struct tm *tlocal1 = localtime(&tiempo1);
    memset(&archivoNuevo.fecha,0,sizeof(archivoNuevo.fecha));
    strftime(archivoNuevo.fecha,128,"%d/%m/%y %H:%M:%S",tlocal1);
    archivoNuevo.bloques_asign=0;
    archivoNuevo.tam=0;
    archivoNuevo.tipo_dato=1;
    for (int var = 0; var < 4; ++var) {
       archivoNuevo.bloque_dir[var]=-1;
    }
    archivoNuevo.bloque_ind1=-1;
    archivoNuevo.bloque_ind2=-1;
    //Metodo para crear inodo nuevo
    fclose(escritor);
    info=crear_inodo(archivoNuevo,nombre_disco,ruta_disco,info);
    //Bloque de carpeta
    BC bcnuevo;
    memset(&bcnuevo.padre,0,sizeof(bcnuevo.padre));
    memset(&bcnuevo.nombre,0,sizeof(bcnuevo.nombre));
    strcat(bcnuevo.padre,"/");
    strcat(bcnuevo.nombre,nombre_archivo);
    for (int var = 0; var < 6; ++var) {
       bcnuevo.hijo[var]=-1;
    }
    bcnuevo.hijo[0]=archivoNuevo.id;
    if (strcmp(path, "/") == 0) {
        //Declarar en la raiz
        int ww = -1;
        for (int var = 0; var < 4; ++var) {
            if (root.bloque_dir[var]==-1) {
                ww = var;
                break;
            }
        }
        if (ww==-1) {
            //Bloques indirectos
            //Nivel 1
            BI bloqueNivel1;
            if (root.bloque_ind1==-1) {
                root.bloque_ind1=info.free_bloque;
                bloqueNivel1.nivel=1;
                for (int var = 0; var < 6; ++var) {
                    bloqueNivel1.apuntador[var]=-1;
                }
                info=crear_bloqueI(bloqueNivel1,nombre_disco,ruta_disco,info);
                bloqueNivel1.apuntador[0]=info.free_bloque;
                info=crear_bloqueC(bcnuevo,nombre_disco,ruta_disco,info);
                crear=1;
                ww=0;
            }
            else{
                fseek(escritor,info.bloques+(sizeof(BI)*root.bloque_ind1),SEEK_SET);
                fread(&bloqueNivel1,sizeof(BI),1,escritor);
                for (int var = 0; var < 6; ++var) {
                    if (bloqueNivel1.apuntador[var]==-1) {
                        ww = var;
                        break;
                    }
                }
                bloqueNivel1.apuntador[ww]=info.free_bloque;
                fclose(escritor);
                info=crear_bloqueC(bcnuevo,nombre_disco,ruta_disco,info);
                crear=1;
            }
            //Actualizar BloqueNivel 1
            escritor = fopen(pathaux, "rb+");
            fseek(escritor,info.bloques+(sizeof(BI)*root.bloque_ind1),SEEK_SET);
            fwrite(&bloqueNivel1,sizeof(BI),1,escritor);
            fclose(escritor);
        }
        else {
            //Asignar en bloque directo
            root.bloque_dir[ww]=info.free_bloque;
            //Crear bloque de carpeta
            //fclose(escritor);
            info=crear_bloqueC(bcnuevo,nombre_disco,ruta_disco,info);
            crear=1;
            /*//Actualizar superbloque
            fseek(escritor, 0, SEEK_SET);
            fread(&info,sizeof(SB),1,escritor);*/
        }
    } else {
        //En otros lados vergas
        escritor = fopen(pathaux, "rb+");
        int nInodo=0;
        int i;
        int ic=0;
        for(i=0;pathE[i]!=NULL;i++)
        {
            if (!(ic==1 && nInodo==0)) {
                ic++;
                I tempIn;
                fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
                fread(&tempIn,sizeof(I),1,escritor);
                //Revisar bloques directos
                for (int var = 0; var < 4; ++var) {
                    if (tempIn.bloque_dir[var]!=-1) {
                        BC bloqueCarpeta;
                        fseek(escritor,info.bloques+(tempIn.bloque_dir[var]*sizeof(BC)),SEEK_SET);
                        fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                        if (strcmp(pathE[i],bloqueCarpeta.nombre)==0) {
                            nInodo=bloqueCarpeta.hijo[0];
                            break;
                        }
                    }
                }
                //printf("%s\n", pathE[i]);
            }
            free(pathE[i]);
        }
        free(pathE);
        if (nInodo!=0) {
            I padre;
            fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
            fread(&padre,sizeof(I),1,escritor);
            int ww = -1;
            for (int var = 0; var < 4; ++var) {
                if (padre.bloque_dir[var]==-1) {
                    ww = var;
                    break;
                }
            }
            if (padre.tipo_dato==1) {
                if (ww==-1) {
                    //Bloques indirectos
                    //Nivel 1
                    BI bloqueNivel1;
                    if (padre.bloque_ind1==-1) {
                        padre.bloque_ind1=info.free_bloque;
                        bloqueNivel1.nivel=1;
                        for (int var = 0; var < 6; ++var) {
                            bloqueNivel1.apuntador[var]=-1;
                        }
                        info=crear_bloqueI(bloqueNivel1,nombre_disco,ruta_disco,info);
                        bloqueNivel1.apuntador[0]=info.free_bloque;
                        info=crear_bloqueC(bcnuevo,nombre_disco,ruta_disco,info);
                        crear=1;
                        ww=0;
                    }
                    else{
                        fseek(escritor,info.bloques+(sizeof(BI)*padre.bloque_ind1),SEEK_SET);
                        fread(&bloqueNivel1,sizeof(BI),1,escritor);
                        for (int var = 0; var < 6; ++var) {
                            if (bloqueNivel1.apuntador[var]==-1) {
                                ww = var;
                                break;
                            }
                        }
                        bloqueNivel1.apuntador[ww]=info.free_bloque;
                        fclose(escritor);
                        info=crear_bloqueC(bcnuevo,nombre_disco,ruta_disco,info);
                        crear=1;
                    }
                    //Actualizar BloqueNivel 1
                    escritor = fopen(pathaux, "rb+");
                    fseek(escritor,info.bloques+(sizeof(BI)*padre.bloque_ind1),SEEK_SET);
                    fwrite(&bloqueNivel1,sizeof(BI),1,escritor);
                    fclose(escritor);
                }
                else{
                padre.bloque_dir[ww]=info.free_bloque;
                fclose(escritor);
                info=crear_bloqueC(bcnuevo,nombre_disco,ruta_disco,info);
                crear=1;
                }
                crear_inodo(padre,nombre_disco,ruta_disco,info);
            }

        }
       }
    if (crear==1) {
        crear_inodo(archivoNuevo,nombre_disco,ruta_disco,info);
    }
    escritor = fopen(pathaux, "rb+");
    fseek(escritor,info.inodos,SEEK_SET);
    fwrite(&root,sizeof(I),1,escritor);
    fseek(escritor, 0, SEEK_SET);
    fwrite(&info,sizeof(SB),1,escritor);
    free(pathaux);
    fclose(escritor);
    if (crear==1) {
        char* descrip=(char*)malloc(140);
        memset(&descrip[0], 0, sizeof(descrip));
        strcat(descrip, "Se creo la carpeta \"");
        strcat(descrip, nombre_archivo);
        strcat(descrip, "\" en ");
        strcat(descrip, path);
        bitacoraReporte(descrip,nombre_disco,ruta_disco);
        free(descrip);
        printf(" Carpeta creada correctamente!\n");
    } else {
        printf(" No se pudo crear carpeta!\n");
    }
}

void visor_archivo(char* nombre_disco,char* ruta_disco,char* path,char **pathE,char* nombre_archivo){
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    SB info;
    fseek(escritor, 0, SEEK_SET);
    fread(&info,sizeof(SB),1,escritor);
    I root;
    fseek(escritor,info.inodos,SEEK_SET);
    fread(&root,sizeof(I),1,escritor);
    if (strcmp(path, "/") == 0) {
        //Declarar en la raiz
        //Revisar bloques directos
        int nInodo=-1;
        for (int var = 0; var < 4; ++var) {
            if (root.bloque_dir[var]!=-1) {
                BC bloqueCarpeta;
                fseek(escritor,info.bloques+(root.bloque_dir[var]*sizeof(BC)),SEEK_SET);
                fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                if (strcmp(nombre_archivo,bloqueCarpeta.nombre)==0) {
                    nInodo=bloqueCarpeta.hijo[0];
                    break;
                }
            }
        }
        if (nInodo==-1) {
            printf(" El archivo no existe! \n");
        } else {
            I padre;
            fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
            fread(&padre,sizeof(I),1,escritor);
            for (int var = 0; var < 4; ++var) {
                if (padre.bloque_dir[var]!=-1) {
                    BA temp;
                    fseek(escritor,info.bloques+(padre.bloque_dir[var]*sizeof(BA)),SEEK_SET);
                    fread(&temp,sizeof(BA),1,escritor);
                    printf("%s",temp.informacion);
                }
            }
            if (padre.bloque_ind1!=-1) {
                escritor = fopen(pathaux, "rb+");
                BI bloqueNivel1;
                fseek(escritor,info.bloques+(sizeof(BI)*padre.bloque_ind1),SEEK_SET);
                fread(&bloqueNivel1,sizeof(BI),1,escritor);
                fclose(escritor);
                for (int var = 0; var < 6; ++var) {
                    if (bloqueNivel1.apuntador[var]!=-1) {
                        BA temp;
                        fseek(escritor,info.bloques+(bloqueNivel1.apuntador[var]*sizeof(BA)),SEEK_SET);
                        fread(&temp,sizeof(BA),1,escritor);
                        printf("%s",temp.informacion);
                    }
                }
            }
        }
    } else {
        //En otros lados vergas
        escritor = fopen(pathaux, "rb+");
        int nInodo=0;
        int i;
        for(i=0;pathE[i]!=NULL;i++)
        {
            I tempIn;
            fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
            fread(&tempIn,sizeof(I),1,escritor);
            //Revisar bloques directos
            for (int var = 0; var < 4; ++var) {
                if (tempIn.bloque_dir[var]!=-1) {
                    BC bloqueCarpeta;
                    fseek(escritor,info.bloques+(tempIn.bloque_dir[var]*sizeof(BC)),SEEK_SET);
                    fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                    if (strcmp(pathE[i],bloqueCarpeta.nombre)==0) {
                        nInodo=bloqueCarpeta.hijo[0];
                    }
                }
            }
            //printf("%s\n", pathE[i]);
            //free(pathE[i]);
        }
        //free(pathE);

        I padreC;
        fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
        fread(&padreC,sizeof(I),1,escritor);
        nInodo=0;
        for (int var = 0; var < 4; ++var) {
            if (padreC.bloque_dir[var]!=-1) {
                BC bloqueCarpeta;
                fseek(escritor,info.bloques+(padreC.bloque_dir[var]*sizeof(BC)),SEEK_SET);
                fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                if (strcmp(nombre_archivo,bloqueCarpeta.nombre)==0) {
                    nInodo=bloqueCarpeta.hijo[0];
                    break;
                }
            }
        }
        if (nInodo==0) {
            printf(" El archivo no existe! \n");
        } else {
            I padre;
            fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
            fread(&padre,sizeof(I),1,escritor);
            for (int var = 0; var < 4; ++var) {
                if (padre.bloque_dir[var]!=-1) {
                    BA temp;
                    fseek(escritor,info.bloques+(padre.bloque_dir[var]*sizeof(BA)),SEEK_SET);
                    fread(&temp,sizeof(BA),1,escritor);
                    printf("%s",temp.informacion);
                }
            }
            if (padre.bloque_ind1!=-1) {
                escritor = fopen(pathaux, "rb+");
                BI bloqueNivel1;
                fseek(escritor,info.bloques+(sizeof(BI)*padre.bloque_ind1),SEEK_SET);
                fread(&bloqueNivel1,sizeof(BI),1,escritor);
                fclose(escritor);
                for (int var = 0; var < 6; ++var) {
                    if (bloqueNivel1.apuntador[var]!=-1) {
                        BA temp;
                        fseek(escritor,info.bloques+(bloqueNivel1.apuntador[var]*sizeof(BA)),SEEK_SET);
                        fread(&temp,sizeof(BA),1,escritor);
                        printf("%s",temp.informacion);
                    }
                }
            }
        }
        fclose(escritor);
    }
    escritor = fopen(pathaux, "rb+");
    fseek(escritor,info.inodos,SEEK_SET);
    fwrite(&root,sizeof(I),1,escritor);
    fseek(escritor, 0, SEEK_SET);
    fwrite(&info,sizeof(SB),1,escritor);
    free(pathaux);
    fclose(escritor);
}

void visor_carpeta(char* nombre_disco,char* ruta_disco,char* path,char **pathE){
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    SB info;
    fseek(escritor, 0, SEEK_SET);
    fread(&info,sizeof(SB),1,escritor);
    I root;
    fseek(escritor,info.inodos,SEEK_SET);
    fread(&root,sizeof(I),1,escritor);
    if (strcmp(path, "/") == 0) {
        //Declarar en la raiz
        //Revisar bloques directos
        for (int var = 0; var < 4; ++var) {
            if (root.bloque_dir[var]!=-1) {
                BC bloqueCarpeta;
                fseek(escritor,info.bloques+(root.bloque_dir[var]*sizeof(BC)),SEEK_SET);
                fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                printf(" - Nombre: %s",bloqueCarpeta.nombre);
                printf(" No. Inodo: %d",bloqueCarpeta.hijo[0]);
                I padre;
                fseek(escritor,info.inodos+(bloqueCarpeta.hijo[0]*sizeof(I)),SEEK_SET);
                fread(&padre,sizeof(I),1,escritor);
                printf(" Tipo: %d\n",padre.tipo_dato);
            }
        }
        if (root.bloque_ind1!=-1) {
            BI bloqueNivel1;
            fseek(escritor,info.bloques+(sizeof(BI)*root.bloque_ind1),SEEK_SET);
            fread(&bloqueNivel1,sizeof(BI),1,escritor);
            for (int var = 0; var < 6; ++var) {
                if (bloqueNivel1.apuntador[var]!=-1) {
                    BC bloqueCarpeta;
                    fseek(escritor,info.bloques+(bloqueNivel1.apuntador[var]*sizeof(BC)),SEEK_SET);
                    fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                    printf(" - Nombre: %s",bloqueCarpeta.nombre);
                    printf(" No. Inodo: %d",bloqueCarpeta.hijo[0]);
                    I padre;
                    fseek(escritor,info.inodos+(bloqueCarpeta.hijo[0]*sizeof(I)),SEEK_SET);
                    fread(&padre,sizeof(I),1,escritor);
                    printf(" Tipo: %d\n",padre.tipo_dato);
                }
            }
        }
    } else {
        //En otros lados vergas
        escritor = fopen(pathaux, "rb+");
        int nInodo=0;
        int i;
        for(i=0;pathE[i]!=NULL;i++)
        {
            I tempIn;
            fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
            fread(&tempIn,sizeof(I),1,escritor);
            //Revisar bloques directos
            for (int var = 0; var < 4; ++var) {
                if (tempIn.bloque_dir[var]!=-1) {
                    BC bloqueCarpeta;
                    fseek(escritor,info.bloques+(tempIn.bloque_dir[var]*sizeof(BC)),SEEK_SET);
                    fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                    if (strcmp(pathE[i],bloqueCarpeta.nombre)==0) {
                        nInodo=bloqueCarpeta.hijo[0];
                    }
                }
            }
            //printf("%s\n", pathE[i]);
            //free(pathE[i]);
        }
        //free(pathE);

        I padreC;
        fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
        fread(&padreC,sizeof(I),1,escritor);
        if (padreC.tipo_dato==1) {
            for (int var = 0; var < 4; ++var) {
                if (padreC.bloque_dir[var]!=-1) {
                    BC bloqueCarpeta;
                    fseek(escritor,info.bloques+(padreC.bloque_dir[var]*sizeof(BC)),SEEK_SET);
                    fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                    printf(" - Nombre: %s",bloqueCarpeta.nombre);
                    printf(" No. Inodo: %d",bloqueCarpeta.hijo[0]);
                    I padre;
                    fseek(escritor,info.inodos+(bloqueCarpeta.hijo[0]*sizeof(I)),SEEK_SET);
                    fread(&padre,sizeof(I),1,escritor);
                    printf(" Tipo: %d\n",padre.tipo_dato);
                }
            }
            if (padreC.bloque_ind1!=-1) {
                BI bloqueNivel1;
                fseek(escritor,info.bloques+(sizeof(BI)*padreC.bloque_ind1),SEEK_SET);
                fread(&bloqueNivel1,sizeof(BI),1,escritor);
                for (int var = 0; var < 6; ++var) {
                    if (bloqueNivel1.apuntador[var]!=-1) {
                        BC bloqueCarpeta;
                        fseek(escritor,info.bloques+(bloqueNivel1.apuntador[var]*sizeof(BC)),SEEK_SET);
                        fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                        printf(" - Nombre: %s",bloqueCarpeta.nombre);
                        printf(" No. Inodo: %d",bloqueCarpeta.hijo[0]);
                        I padre;
                        fseek(escritor,info.inodos+(bloqueCarpeta.hijo[0]*sizeof(I)),SEEK_SET);
                        fread(&padre,sizeof(I),1,escritor);
                        printf(" Tipo: %d\n",padre.tipo_dato);
                    }
                }
            }

        }
        fclose(escritor);
    }
    escritor = fopen(pathaux, "rb+");
    fseek(escritor,info.inodos,SEEK_SET);
    fwrite(&root,sizeof(I),1,escritor);
    fseek(escritor, 0, SEEK_SET);
    fwrite(&info,sizeof(SB),1,escritor);
    free(pathaux);
    fclose(escritor);
}

void eliminar(char* nombre_disco,char* ruta_disco,char* path,char **pathE,int tipoA){
    int inoAr[48];
    int contino=-1;
    int bloAr[48];
    int contblo=-1;
    char* descrip=(char*)malloc(140);
    memset(&descrip[0], 0, sizeof(descrip));
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    SB info;
    fseek(escritor, 0, SEEK_SET);
    fread(&info,sizeof(SB),1,escritor);
    I root;
    fseek(escritor,info.inodos,SEEK_SET);
    fread(&root,sizeof(I),1,escritor);
    I aux;
    int auxn;
    int nInodo=0;
    int nBloque=0;
    if (strcmp(path, "/") == 0) {
        //Declarar en la raiz
        printf(" No se puede eliminar el directorio \n");
    } else {
        //En otros lados vergas
        //escritor = fopen(pathaux, "rb+");
        int i;
        for(i=0;pathE[i]!=NULL;i++)
        {
            I tempIn;
            fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
            fread(&tempIn,sizeof(I),1,escritor);
            //Revisar bloques directos
            for (int var = 0; var < 4; ++var) {
                if (tempIn.bloque_dir[var]!=-1) {
                    BC bloqueCarpeta;
                    fseek(escritor,info.bloques+(tempIn.bloque_dir[var]*sizeof(BC)),SEEK_SET);
                    fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                    if (strcmp(pathE[i],bloqueCarpeta.nombre)==0) {
                        auxn = nInodo;
                        nInodo=bloqueCarpeta.hijo[0];
                        nBloque=tempIn.bloque_dir[var];
                        aux = tempIn;
                        aux.bloque_dir[var]=-1;
                    }
                }
            }
            //printf("%s\n", pathE[i]);
            //free(pathE[i]);
        }
        //free(pathE);
        if (nInodo!=0) {
            I padreC;
            fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
            fread(&padreC,sizeof(I),1,escritor);
            fclose(escritor);
            if (padreC.tipo_dato==1) {
                for (int var = 0; var < 4; ++var) {
                    if (padreC.bloque_dir[var]!=-1) {
                        escritor = fopen(pathaux, "rb+");
                        BC bloqueCarpeta;
                        fseek(escritor,info.bloques+(padreC.bloque_dir[var]*sizeof(BC)),SEEK_SET);
                        fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                        char pathA[100];
                        memset(&pathA,0,sizeof(pathA));
                        strcat(pathA,path);
                        strcat(pathA,"/");
                        strcat(pathA,bloqueCarpeta.nombre);
                        char subbuff[strlen(path)];
                        memcpy( subbuff, &pathA[1], strlen(pathA)-1);
                        subbuff[strlen(pathA)-1] = '\0';
                        char **retorno=split(subbuff, '/');
                        fclose(escritor);
                        contino++;
                        inoAr[contino]=bloqueCarpeta.hijo[0];
                        contblo++;
                        bloAr[contblo]=padreC.bloque_dir[var];
                        eliminar(nombre_disco,ruta_disco,pathA,retorno,tipoA);
                        /*escritor = fopen(pathaux, "rb+");
                        BM elim;
                        elim.estado='0';
                        fseek(escritor, info.bitmap_inodos+(sizeof(BM)*bloqueCarpeta.hijo[0]), SEEK_SET);
                        fwrite(&elim,sizeof(BM),1,escritor);
                        fseek(escritor, info.bitmap_bloques+(sizeof(BM)*padreC.bloque_dir[var]), SEEK_SET);
                        fwrite(&elim,sizeof(BM),1,escritor);
                        fclose(escritor);
                        info=eliminar_inodo(bloqueCarpeta.hijo[0],nombre_disco,ruta_disco,info);
                        info=eliminar_bloque(padreC.bloque_dir[var],nombre_disco,ruta_disco,info);*/
                        free(retorno);
                    }
                }
                if (padreC.bloque_ind1!=-1) {
                    escritor = fopen(pathaux, "rb+");
                    BI bloqueNivel1;
                    fseek(escritor,info.bloques+(sizeof(BI)*padreC.bloque_ind1),SEEK_SET);
                    fread(&bloqueNivel1,sizeof(BI),1,escritor);
                    fclose(escritor);
                    for (int var = 0; var < 6; ++var) {
                        if (bloqueNivel1.apuntador[var]!=-1) {
                            escritor = fopen(pathaux, "rb+");
                            BC bloqueCarpeta;
                            fseek(escritor,info.bloques+(bloqueNivel1.apuntador[var]*sizeof(BC)),SEEK_SET);
                            fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                            char pathA[100];
                            memset(&pathA,0,sizeof(pathA));
                            strcat(pathA,path);
                            strcat(pathA,"/");
                            strcat(pathA,bloqueCarpeta.nombre);
                            char subbuff[strlen(path)];
                            memcpy( subbuff, &pathA[1], strlen(pathA)-1);
                            subbuff[strlen(pathA)-1] = '\0';
                            char **retorno=split(subbuff, '/');
                            fclose(escritor);
                            contino++;
                            inoAr[contino]=bloqueCarpeta.hijo[0];
                            contblo++;
                            bloAr[contblo]=bloqueNivel1.apuntador[var];
                            eliminar(nombre_disco,ruta_disco,pathA,retorno,tipoA);
                            /*escritor = fopen(pathaux, "rb+");
                            BM elim;
                            elim.estado='0';
                            fseek(escritor, info.bitmap_inodos+(sizeof(BM)*bloqueCarpeta.hijo[0]), SEEK_SET);
                            fwrite(&elim,sizeof(BM),1,escritor);
                            fseek(escritor, info.bitmap_bloques+(sizeof(BM)*bloqueNivel1.apuntador[var]), SEEK_SET);
                            fwrite(&elim,sizeof(BM),1,escritor);
                            fclose(escritor);
                            info=eliminar_inodo(bloqueCarpeta.hijo[0],nombre_disco,ruta_disco,info);
                            info=eliminar_bloque(bloqueNivel1.apuntador[var],nombre_disco,ruta_disco,info);*/
                            free(retorno);
                        }
                    }
                }
                if (tipoA==1) {
                    printf("   Se elimino la carpeta %s\n",path);
                    strcat(descrip, "Se elimino la carpeta \"");
                    strcat(descrip, path);
                    strcat(descrip,"\"");
                }
            }
            else{
                for (int var = 0; var < 4; ++var) {
                    if (padreC.bloque_dir[var]!=-1) {
                        info=eliminar_bloque(padreC.bloque_dir[var],nombre_disco,ruta_disco,info);
                    }
                }
                if (padreC.bloque_ind1!=-1) {
                    escritor = fopen(pathaux, "rb+");
                    BI bloqueNivel1;
                    fseek(escritor,info.bloques+(sizeof(BI)*padreC.bloque_ind1),SEEK_SET);
                    fread(&bloqueNivel1,sizeof(BI),1,escritor);
                    fclose(escritor);
                    for (int var = 0; var < 6; ++var) {
                        if (bloqueNivel1.apuntador[var]!=-1) {
                            info=eliminar_bloque(bloqueNivel1.apuntador[var],nombre_disco,ruta_disco,info);
                        }
                    }
                }
                if (tipoA==1) {
                    printf("   Se elimino el archivo %s\n",path);
                    strcat(descrip, "Se elimino el archivo \"");
                    strcat(descrip, path);
                    strcat(descrip,"\"");
                }
            }
            /*escritor = fopen(pathaux, "rb+");
            BM elim;
            elim.estado='0';
            //Actualizar Bitmap
            fseek(escritor, info.bitmap_inodos+(sizeof(BM)*nInodo), SEEK_SET);
            fwrite(&elim,sizeof(BM),1,escritor);
            fseek(escritor, info.bitmap_bloques+(sizeof(BM)*nBloque), SEEK_SET);
            fwrite(&elim,sizeof(BM),1,escritor);
            fclose(escritor);
            info=eliminar_inodo(nInodo,nombre_disco,ruta_disco,info);
            info=eliminar_bloque(nBloque,nombre_disco,ruta_disco,info);*/
        }
        else{
        close(escritor);
        }
    }
    /*for (int var = 0; var < 48; ++var) {
        if (inoAr[var]!=0) {
            printf(" Inodo: %d\n",inoAr[var]);
            info=eliminar_inodo(inoAr[var],nombre_disco,ruta_disco,info);
        }
        else{
            break;
        }
    }*/
    /*for (int var = 0; var < 48; ++var) {
        if (bloAr[var]!=0) {
            printf(" Bloque: %d\n",bloAr[var]);
            info=eliminar_bloque(bloAr[var],nombre_disco,ruta_disco,info);
        }
        else{
            break;
        }
    }*/
    if (nInodo!=0) {
        info=eliminar_inodo(nInodo,nombre_disco,ruta_disco,info);
        info=eliminar_bloque(nBloque,nombre_disco,ruta_disco,info);
        if (tipoA==1) {
            bitacoraReporte(descrip,nombre_disco,ruta_disco);
            free(descrip);
            printf("   Se elimino el contenido de la ruta! \n");
        }
    }
    else{
        printf("   No se pudo eliminar el contenido de la ruta! \n");
    }
    escritor = fopen(pathaux, "rb+");
    fseek(escritor,info.inodos,SEEK_SET);
    fwrite(&root,sizeof(I),1,escritor);
    fseek(escritor,info.inodos+(auxn*sizeof(I)),SEEK_SET);
    fwrite(&aux,sizeof(I),1,escritor);
    fseek(escritor, 0, SEEK_SET);
    fwrite(&info,sizeof(SB),1,escritor);
    free(pathaux);
    fclose(escritor);
}

void mover(char* nombre_disco,char* ruta_disco,char* path,char **pathE,char* pathM,char **pathEM){
    int inoAr[48];
    int contino=-1;
    int bloAr[48];
    int contblo=-1;
    char* descrip=(char*)malloc(140);
    memset(&descrip[0], 0, sizeof(descrip));
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    SB info;
    fseek(escritor, 0, SEEK_SET);
    fread(&info,sizeof(SB),1,escritor);
    I root;
    fseek(escritor,info.inodos,SEEK_SET);
    fread(&root,sizeof(I),1,escritor);
    I aux;
    int auxn;
    int nInodo=0;
    int nBloque=0;
    int nInodoDestino=0;
    //int nBloqueDestino=0;
    if (strcmp(path, "/") == 0) {
        //Declarar en la raiz
        printf(" No se puede mover el directorio \n");
    } else {
        //En otros lados vergas
        //escritor = fopen(pathaux, "rb+");
        int i;
        for(i=0;pathE[i]!=NULL;i++)
        {
            I tempIn;
            fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
            fread(&tempIn,sizeof(I),1,escritor);
            //Revisar bloques directos
            for (int var = 0; var < 4; ++var) {
                if (tempIn.bloque_dir[var]!=-1) {
                    BC bloqueCarpeta;
                    fseek(escritor,info.bloques+(tempIn.bloque_dir[var]*sizeof(BC)),SEEK_SET);
                    fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                    if (strcmp(pathE[i],bloqueCarpeta.nombre)==0) {
                        auxn = nInodo;
                        nInodo=bloqueCarpeta.hijo[0];
                        nBloque=tempIn.bloque_dir[var];
                        aux = tempIn;
                        aux.bloque_dir[var]=-1;
                    }
                }
            }
            //printf("%s\n", pathE[i]);
            //free(pathE[i]);
        }
        //free(pathE);
        if (nInodo!=0) {
            if (strcmp(pathM, "/") == 0) {
                //Declarar en la raiz
                int ww = -1;
                for (int var = 0; var < 4; ++var) {
                    if (root.bloque_dir[var]==-1) {
                        ww = var;
                        break;
                    }
                }
                if (ww==-1) {
                    //Bloques indirectos
                    //Nivel 1
                    BI bloqueNivel1;
                    if (root.bloque_ind1==-1) {
                        root.bloque_ind1=info.free_bloque;
                        bloqueNivel1.nivel=1;
                        for (int var = 0; var < 6; ++var) {
                            bloqueNivel1.apuntador[var]=-1;
                        }
                        info=crear_bloqueI(bloqueNivel1,nombre_disco,ruta_disco,info);
                        bloqueNivel1.apuntador[0]=nBloque;
                    }
                    else{
                        fseek(escritor,info.bloques+(sizeof(BI)*root.bloque_ind1),SEEK_SET);
                        fread(&bloqueNivel1,sizeof(BI),1,escritor);
                        for (int var = 0; var < 6; ++var) {
                            if (bloqueNivel1.apuntador[var]==-1) {
                                ww = var;
                                break;
                            }
                        }
                        bloqueNivel1.apuntador[ww]=nBloque;
                    }
                    //Actualizar BloqueNivel 1
                    escritor = fopen(pathaux, "rb+");
                    fseek(escritor,info.bloques+(sizeof(BI)*root.bloque_ind1),SEEK_SET);
                    fwrite(&bloqueNivel1,sizeof(BI),1,escritor);
                    fclose(escritor);
                }
                else {
                    //Asignar en bloque directo
                    root.bloque_dir[ww]=nBloque;
                }
            } else {
                //En otros lados vergas
                escritor = fopen(pathaux, "rb+");
                int i;
                int ic=0;
                for(i=0;pathEM[i]!=NULL;i++)
                {
                    if (!(ic==1 && nInodoDestino==0)) {
                        ic++;
                        I tempIn;
                        fseek(escritor,info.inodos+(nInodoDestino*sizeof(I)),SEEK_SET);
                        fread(&tempIn,sizeof(I),1,escritor);
                        //Revisar bloques directos
                        for (int var = 0; var < 4; ++var) {
                            if (tempIn.bloque_dir[var]!=-1) {
                                BC bloqueCarpeta;
                                fseek(escritor,info.bloques+(tempIn.bloque_dir[var]*sizeof(BC)),SEEK_SET);
                                fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                                if (strcmp(pathEM[i],bloqueCarpeta.nombre)==0) {
                                    nInodoDestino=bloqueCarpeta.hijo[0];
                                    break;
                                }
                            }
                        }
                        //printf("%s\n", pathE[i]);
                    }
                    free(pathEM[i]);
                }
                free(pathEM);
                if (nInodoDestino!=0) {
                    I padre;
                    fseek(escritor,info.inodos+(nInodoDestino*sizeof(I)),SEEK_SET);
                    fread(&padre,sizeof(I),1,escritor);
                    int ww = -1;
                    for (int var = 0; var < 4; ++var) {
                        if (padre.bloque_dir[var]==-1) {
                            ww = var;
                            break;
                        }
                    }
                    if (padre.tipo_dato==1) {
                        if (ww==-1) {
                            //Bloques indirectos
                            //Nivel 1
                            BI bloqueNivel1;
                            if (padre.bloque_ind1==-1) {
                                padre.bloque_ind1=info.free_bloque;
                                bloqueNivel1.nivel=1;
                                for (int var = 0; var < 6; ++var) {
                                    bloqueNivel1.apuntador[var]=-1;
                                }
                                info=crear_bloqueI(bloqueNivel1,nombre_disco,ruta_disco,info);
                                bloqueNivel1.apuntador[0]=nBloque;
                            }
                            else{
                                fseek(escritor,info.bloques+(sizeof(BI)*padre.bloque_ind1),SEEK_SET);
                                fread(&bloqueNivel1,sizeof(BI),1,escritor);
                                for (int var = 0; var < 6; ++var) {
                                    if (bloqueNivel1.apuntador[var]==-1) {
                                        ww = var;
                                        break;
                                    }
                                }
                                bloqueNivel1.apuntador[ww]=nBloque;
                                fclose(escritor);
                            }
                            //Actualizar BloqueNivel 1
                            escritor = fopen(pathaux, "rb+");
                            fseek(escritor,info.bloques+(sizeof(BI)*padre.bloque_ind1),SEEK_SET);
                            fwrite(&bloqueNivel1,sizeof(BI),1,escritor);
                            fclose(escritor);
                        }
                        else{
                        padre.bloque_dir[ww]=nBloque;
                        fclose(escritor);
                        }
                        crear_inodo(padre,nombre_disco,ruta_disco,info);
                    }

                }
                else{
                    nInodoDestino=-1;
                }
            }

        }
        else{
        close(escritor);
        }
    }
    escritor = fopen(pathaux, "rb+");
    fseek(escritor,info.inodos,SEEK_SET);
    fwrite(&root,sizeof(I),1,escritor);
    fseek(escritor,info.inodos+(auxn*sizeof(I)),SEEK_SET);
    fwrite(&aux,sizeof(I),1,escritor);
    fseek(escritor, 0, SEEK_SET);
    fwrite(&info,sizeof(SB),1,escritor);
    free(pathaux);
    fclose(escritor);
    if (nInodo!=0 && nInodoDestino!=-1) {
        strcat(descrip, "Se movio \"");
        strcat(descrip, path);
        strcat(descrip, "\" en \"");
        strcat(descrip, pathM);
        strcat(descrip, "\"");
        bitacoraReporte(descrip,nombre_disco,ruta_disco);
        printf(" %s! \n",descrip);
        free(descrip);
    } else {
        printf(" No se pudo realizar la accion! \n");
    }
}

void bitacoraReporte(char* descrip,char* nombre_disco,char* ruta_disco){
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
    for (int var = 0; var < (info.numero_inodos*3); ++var) {
        fread(&journalReporte, sizeof(J), 1, escritor);
        if (strcmp(journalReporte.descripsion, "-") == 0) {
            J infoRoot;
            time_t tiempo1 = time(0);
            struct tm *tlocal1 = localtime(&tiempo1);
            memset(&infoRoot.fecha,0,sizeof(infoRoot.fecha));
            strftime(infoRoot.fecha,128,"%d/%m/%y %H:%M:%S",tlocal1);
            memset(&infoRoot.descripsion,0,sizeof(infoRoot.descripsion));
            strcat(infoRoot.descripsion,descrip);
            fseek(escritor,info.journal+sizeof(J)*var,SEEK_SET);
            fwrite(&infoRoot,sizeof(J),1,escritor);
            break;
        }
    }
    fclose(escritor);
}

void modificar_archivo(char* nombre_disco,char* ruta_disco,char* path,char **pathE,char* nombre_archivo){
    char* pathauxE=(char*)malloc(150);
    memset(&pathauxE[0], 0, sizeof(pathauxE));
    int nInodo=0;
    char* comando=(char*)malloc(150);
    char* pathaux=(char*)malloc(150);
    char* texto=(char*)malloc(1104);
    memset(&texto[0], 0, sizeof(texto));
    memset(&pathaux[0], 0, sizeof(pathaux));
    memset(&comando[0], 0, sizeof(comando));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    SB info;
    fseek(escritor, 0, SEEK_SET);
    fread(&info,sizeof(SB),1,escritor);
    I root;
    fseek(escritor,info.inodos,SEEK_SET);
    fread(&root,sizeof(I),1,escritor);
    if (strcmp(path, "/") == 0) {
        //Declarar en la raiz
        //Revisar bloques directos
        nInodo=-1;
        for (int var = 0; var < 4; ++var) {
            if (root.bloque_dir[var]!=-1) {
                BC bloqueCarpeta;
                fseek(escritor,info.bloques+(root.bloque_dir[var]*sizeof(BC)),SEEK_SET);
                fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                if (strcmp(nombre_archivo,bloqueCarpeta.nombre)==0) {
                    nInodo=bloqueCarpeta.hijo[0];
                    break;
                }
            }
        }
        if (nInodo==-1) {
            printf(" El archivo no existe! \n");
        } else {
            I padre;
            fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
            fread(&padre,sizeof(I),1,escritor);
            for (int var = 0; var < 4; ++var) {
                if (padre.bloque_dir[var]!=-1) {
                    BA temp;
                    fseek(escritor,info.bloques+(padre.bloque_dir[var]*sizeof(BA)),SEEK_SET);
                    fread(&temp,sizeof(BA),1,escritor);
                    //printf("%s",temp.informacion);
                    strcat(texto, temp.informacion);
                }
            }
        }
    } else {
        //En otros lados vergas
        escritor = fopen(pathaux, "rb+");
        int i;
        for(i=0;pathE[i]!=NULL;i++)
        {
            I tempIn;
            fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
            fread(&tempIn,sizeof(I),1,escritor);
            //Revisar bloques directos
            for (int var = 0; var < 4; ++var) {
                if (tempIn.bloque_dir[var]!=-1) {
                    BC bloqueCarpeta;
                    fseek(escritor,info.bloques+(tempIn.bloque_dir[var]*sizeof(BC)),SEEK_SET);
                    fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                    if (strcmp(pathE[i],bloqueCarpeta.nombre)==0) {
                        nInodo=bloqueCarpeta.hijo[0];
                    }
                }
            }
            //printf("%s\n", pathE[i]);
            //free(pathE[i]);
        }
        //free(pathE);

        I padreC;
        fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
        fread(&padreC,sizeof(I),1,escritor);
        nInodo=0;
        for (int var = 0; var < 4; ++var) {
            if (padreC.bloque_dir[var]!=-1) {
                BC bloqueCarpeta;
                fseek(escritor,info.bloques+(padreC.bloque_dir[var]*sizeof(BC)),SEEK_SET);
                fread(&bloqueCarpeta,sizeof(BC),1,escritor);
                if (strcmp(nombre_archivo,bloqueCarpeta.nombre)==0) {
                    nInodo=bloqueCarpeta.hijo[0];
                    break;
                }
            }
        }
        if (nInodo==0) {
            printf(" El archivo no existe! \n");
        } else {
            I padre;
            fseek(escritor,info.inodos+(nInodo*sizeof(I)),SEEK_SET);
            fread(&padre,sizeof(I),1,escritor);
            if (padre.tipo_dato==0) {
                for (int var = 0; var < 4; ++var) {
                    if (padre.bloque_dir[var]!=-1) {
                        BA temp;
                        fseek(escritor,info.bloques+(padre.bloque_dir[var]*sizeof(BA)),SEEK_SET);
                        fread(&temp,sizeof(BA),1,escritor);
                        //printf("%s",temp.informacion);
                        strcat(texto, temp.informacion);
                    }
                }

                if (padre.bloque_ind1!=-1) {
                    escritor = fopen(pathaux, "rb+");
                    BI bloqueNivel1;
                    fseek(escritor,info.bloques+(sizeof(BI)*padre.bloque_ind1),SEEK_SET);
                    fread(&bloqueNivel1,sizeof(BI),1,escritor);
                    fclose(escritor);
                    for (int var = 0; var < 6; ++var) {
                        if (bloqueNivel1.apuntador[var]!=-1) {
                            BA temp;
                            fseek(escritor,info.bloques+(bloqueNivel1.apuntador[var]*sizeof(BA)),SEEK_SET);
                            fread(&temp,sizeof(BA),1,escritor);
                            //printf("%s",temp.informacion);
                            strcat(texto, temp.informacion);
                        }
                    }
                }
            }
        }
        fclose(escritor);
    }
    //Mostrar informacion del archivo con nano
    FILE *fp = NULL;
    fp = fopen(nombre_archivo,"a");
    fprintf(fp, texto);
    fclose(fp);
    fflush(fp);
    //Mostrar archivo con nano
    strcat(comando,"nano ");
    strcat(comando,nombre_archivo);
    system(comando);
    free(comando);
    //Leer buffer
    char *source = NULL;
    fp = fopen(nombre_archivo, "r");
    if (fp != NULL) {
        /* Go to the end of the file. */
        if (fseek(fp, 0L, SEEK_END) == 0) {
            /* Get the size of the file. */
            long bufsize = ftell(fp);
            if (bufsize == -1) { /* Error */ }

            /* Allocate our buffer to that size. */
            source = malloc(sizeof(char) * (bufsize + 1));

            /* Go back to the start of the file. */
            if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }

            /* Read the entire file into memory. */
            size_t newLen = fread(source, sizeof(char), bufsize, fp);
            if ( ferror( fp ) != 0 ) {
                fputs("Error reading file", stderr);
            } else {
                source[newLen++] = '\0'; /* Just to be safe. */
            }
        }
        fclose(fp);
    }
    //printf("%s \n",source);

    //Eliminar archivo
    remove(nombre_archivo);

    if(nInodo!=0){
    //Eliminar archivo del disco
    if (strcmp(path, "/") == 0) {
        strcat(pathauxE, path);
        strcat(pathauxE, nombre_archivo);
    }
    else{
        strcat(pathauxE, path);
        strcat(pathauxE, "/");
        strcat(pathauxE, nombre_archivo);
    }
    //Split
    char subbuff[strlen(pathauxE)];
    memcpy( subbuff, &pathauxE[1], strlen(pathauxE)-1);
    subbuff[strlen(pathauxE)-1] = '\0';
            //int i;
    char **retorno=split(subbuff, '/');
    eliminar(nombre_disco,ruta_disco,pathauxE,retorno,0);
    free(retorno);
    //Recrear archivo
    crear_archivo(nombre_disco,ruta_disco,path,pathE,source,nombre_archivo,0);
    }

    //Actualizar la informacion
    free(source);
    escritor = fopen(pathaux, "rb+");
    fseek(escritor,info.inodos,SEEK_SET);
    fwrite(&root,sizeof(I),1,escritor);
    fseek(escritor, 0, SEEK_SET);
    fwrite(&info,sizeof(SB),1,escritor);
    free(pathaux);
    fclose(escritor);
    if (nInodo!=0) {
        char* descrip=(char*)malloc(140);
        memset(&descrip[0], 0, sizeof(descrip));
        strcat(descrip, "Se creo actualizo el archivo \"");
        strcat(descrip, nombre_archivo);
        strcat(descrip, "\" en ");
        strcat(descrip, path);
        bitacoraReporte(descrip,nombre_disco,ruta_disco);
        free(descrip);
        printf(" Archivo actualizado correctamente!\n");
    }
}

struct superbloque crear_inodo(I nuevo,char* nombre_disco,char* ruta_disco,SB info){
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    /*fseek(escritor, 0, SEEK_SET);
    SB info;
    fread(&info,sizeof(SB),1,escritor);*/
    //Escribir inodo nuevo
    fseek(escritor,info.inodos+(sizeof(I)*nuevo.id),SEEK_SET);
    fwrite(&nuevo,sizeof(I),1,escritor);
    BM rootBM;
    rootBM.estado='1';
    //Actualizar Bitmap
    fseek(escritor, info.bitmap_inodos+(sizeof(BM)*nuevo.id), SEEK_SET);
    fwrite(&rootBM,sizeof(BM),1,escritor);
    fclose(escritor);
    //Actualizar el siguiente inodo libre
    info.free_inodo=next_inodo(nombre_disco,ruta_disco);
    info.inodos_free--;
    /*//Actualizar superbloque
    escritor = fopen(pathaux, "rb+");
    fseek(escritor, 0, SEEK_SET);
    fread(&info,sizeof(SB),1,escritor);*/
    free(pathaux);
    //fclose(escritor);
    return info;
}

struct superbloque crear_bloqueC(BC nuevo,char* nombre_disco,char* ruta_disco,SB info){
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    /*fseek(escritor, 0, SEEK_SET);
    SB info;
    fread(&info,sizeof(SB),1,escritor);*/
    //Escribir bloque nuevo
    fseek(escritor,info.bloques+(sizeof(BC)*info.free_bloque),SEEK_SET);
    fwrite(&nuevo,sizeof(BC),1,escritor);

    BM rootBM;
    rootBM.estado='1';
    //Actualizar Bitmap
    fseek(escritor, info.bitmap_bloques+(sizeof(BM)*info.free_bloque), SEEK_SET);
    fwrite(&rootBM,sizeof(BM),1,escritor);
    fclose(escritor);
    //Actualizar el siguiente inodo libre
    info.free_bloque=next_bloque(nombre_disco,ruta_disco);
    info.bloques_free--;
    //Actualizar superbloque
    /*escritor = fopen(pathaux, "rb+");
    fseek(escritor, 0, SEEK_SET);
    fread(&info,sizeof(SB),1,escritor);*/
    free(pathaux);
    //fclose(escritor);
    return info;
}

struct superbloque crear_bloqueA(BA nuevo,char* nombre_disco,char* ruta_disco,SB info){
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    /*fseek(escritor, 0, SEEK_SET);
    SB info;
    fread(&info,sizeof(SB),1,escritor);*/
    //Escribir bloque nuevo
    fseek(escritor,info.bloques+(sizeof(BA)*info.free_bloque),SEEK_SET);
    fwrite(&nuevo,sizeof(BA),1,escritor);

    BM rootBM;
    rootBM.estado='1';
    //Actualizar Bitmap
    fseek(escritor, info.bitmap_bloques+(sizeof(BM)*info.free_bloque), SEEK_SET);
    fwrite(&rootBM,sizeof(BM),1,escritor);
    fclose(escritor);
    //Actualizar el siguiente inodo libre
    info.free_bloque=next_bloque(nombre_disco,ruta_disco);
    info.bloques_free--;
    //Actualizar superbloque
    /*escritor = fopen(pathaux, "rb+");
    fseek(escritor, 0, SEEK_SET);
    fread(&info,sizeof(SB),1,escritor);*/
    free(pathaux);
    //fclose(escritor);
    return info;
}

struct superbloque crear_bloqueI(BI nuevo,char* nombre_disco,char* ruta_disco,SB info){
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    /*fseek(escritor, 0, SEEK_SET);
    SB info;
    fread(&info,sizeof(SB),1,escritor);*/
    //Escribir bloque nuevo
    fseek(escritor,info.bloques+(sizeof(BI)*info.free_bloque),SEEK_SET);
    fwrite(&nuevo,sizeof(BI),1,escritor);

    BM rootBM;
    rootBM.estado='1';
    //Actualizar Bitmap
    fseek(escritor, info.bitmap_bloques+(sizeof(BM)*info.free_bloque), SEEK_SET);
    fwrite(&rootBM,sizeof(BM),1,escritor);
    fclose(escritor);
    //Actualizar el siguiente inodo libre
    info.free_bloque=next_bloque(nombre_disco,ruta_disco);
    info.bloques_free--;
    //Actualizar superbloque
    /*escritor = fopen(pathaux, "rb+");
    fseek(escritor, 0, SEEK_SET);
    fread(&info,sizeof(SB),1,escritor);*/
    free(pathaux);
    //fclose(escritor);
    return info;
}

struct superbloque eliminar_inodo(int pos,char* nombre_disco,char* ruta_disco,SB info){
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    /*fseek(escritor, 0, SEEK_SET);
    SB info;
    fread(&info,sizeof(SB),1,escritor);*/
    BM rootBM;
    rootBM.estado='0';
    //Actualizar Bitmap
    fseek(escritor, info.bitmap_inodos+(sizeof(BM)*pos), SEEK_SET);
    fwrite(&rootBM,sizeof(BM),1,escritor);
    /*BM estadoE;
    fseek(escritor, info.bitmap_inodos+(sizeof(BM)*pos), SEEK_SET);
    fread(&estadoE,sizeof(BM),1,escritor);
    printf("Inodo: %d Bitmap recien borrado: %c \n",pos,estadoE.estado);*/
    fclose(escritor);
    //Actualizar el siguiente inodo libre
    info.free_inodo=next_inodo(nombre_disco,ruta_disco);
    info.inodos_free++;
    /*//Actualizar superbloque
    escritor = fopen(pathaux, "rb+");
    fseek(escritor, 0, SEEK_SET);
    fread(&info,sizeof(SB),1,escritor);*/
    free(pathaux);
    //fclose(escritor);
    return info;
}

struct superbloque eliminar_bloque(int pos,char* nombre_disco,char* ruta_disco,SB info){
    char* pathaux=(char*)malloc(150);
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");
    FILE* escritor = fopen(pathaux, "rb+");
    /*fseek(escritor, 0, SEEK_SET);
    SB info;
    fread(&info,sizeof(SB),1,escritor);*/
    BM rootBM;
    rootBM.estado='0';
    //Actualizar Bitmap
    fseek(escritor, info.bitmap_bloques+(sizeof(BM)*pos), SEEK_SET);
    fwrite(&rootBM,sizeof(BM),1,escritor);
    /*BM estadoE;
    fseek(escritor, info.bitmap_bloques+(sizeof(BM)*pos), SEEK_SET);
    fread(&estadoE,sizeof(BM),1,escritor);
    printf("Bloque: %d  Bitmap recien borrado: %c\n",pos,estadoE.estado);*/
    fclose(escritor);
    //Actualizar el siguiente inodo libre
    info.free_bloque=next_bloque(nombre_disco,ruta_disco);
    info.bloques_free++;
    //Actualizar superbloque
    /*escritor = fopen(pathaux, "rb+");
    fseek(escritor, 0, SEEK_SET);
    fread(&info,sizeof(SB),1,escritor);*/
    free(pathaux);
    //fclose(escritor);
    return info;
}

int next_inodo(char* nombre_disco,char* ruta_disco){
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
    int i = 0;
    for (i = 0; i < (info.numero_inodos+ 10 - (info.numero_inodos%10)); i++){
        if(i < info.numero_inodos){
        fread(&bm, sizeof(BM), 1, escritor);
        if (bm.estado=='0') {
            return i;
        }
        }
    }
    return 0;
    fclose(escritor);
}

int next_bloque(char* nombre_disco,char* ruta_disco){
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
    int i = 0;
    for (i = 0; i < (info.numero_bloques+ 10 - (info.numero_bloques%10)); i++){
        if(i < info.numero_bloques){
        fread(&bm, sizeof(BM), 1, escritor);
        if (bm.estado=='0') {
            return i;
        }
        }
    }
    return 0;
    fclose(escritor);
}

#endif // DISK_H
