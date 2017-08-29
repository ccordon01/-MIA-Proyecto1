#ifndef DISK_H
#define DISK_H
#include <stdio.h>

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


void crear_disco(char * nombre_disco, int tamano_disco, char * nombre_estudiante, char * carne, int bloques,char * ruta_disco){
    char comando[500];

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
    char megas[15];
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

    // Una vez creado el archivo lo abrimos para poder ingresar las estructuras
    DATOS alumno;
    BM bm;

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

    char pathaux[150];
    memset(&pathaux[0], 0, sizeof(pathaux));
    strcat(pathaux, ruta_disco);
    strcat(pathaux, "/");
    strcat(pathaux, nombre_disco);
    strcat(pathaux, ".dsk");

    FILE* escritor = fopen(pathaux, "rb+");
    /*  fseek pone el puntero en la posicion que le indicamos en el segundo parametro
     *  primer parametro se manda la variable de tipo FILE*, segundo la posicion dentro del archivo
     *  y la tercera es el modo, ademas de usar SEEK_SET que solo lo setea, una funcion importante es
     *  SEEK_CUR que de la posicion actual se desplaza, para ir hacia atras se multiplica por -1 el segundo
     *  parametro. Por ejemplo: fseek(escritor, -1*sizeof(DATOS), SEEK_CUR);
     *  Acontinuacion solo nos situamos al inicio del archivo para escribir los datos
      */
    fseek(escritor, 0, SEEK_SET);
    // Llenamos de informacion la estructura de alumno
    strcpy(alumno.nombre, nombre_estudiante);
    alumno.carne = carne;
    alumno.bloques = bloques;
    /* Una vez lleno lo escribimos en el archivo con la funcion fwrite
     * Parametros de fwrite:
     * 1. Puntero a la direccion de memoria donde se encuentran los datos a escribir
     * 2. Tamano de CADA registro
     * 3. CANTIDAD de registros a escribir
     * 4. FILE abierto previamente
     */
    fwrite(&alumno, sizeof(DATOS), 1, escritor);

    // Escribimos el bitmap con la cantidad de bloques que el usuario ingreso
    bm.estado = '0';
    int i = 0;
    for (i = 0; i < bloques; i++){
        fwrite(&bm, sizeof(BM), 1, escritor);
    }

    // Cerramos el archivo con fclose
    fclose(escritor);
}

void mostrar_disco(char* nombre_disco){
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

void ingresar_bloques(char* nombre_disco, int cantidad){
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

void borrar_bloques(char* nombre_disco, int inicio, int cantidad){
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
#endif // DISK_H
