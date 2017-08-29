#include <stdio.h>
#include "disk.h"
#define MAX_DISCO 20
#define MAX_NOMBRE 50
#define MAX_RUTA 50

// Variables globales


int main()
{
    /*  El programa creara un disco en el cual se escribiran los datos del estudiante
 *  seguido de un bitmap que el usuario va a definir mediante un menu y en base
 *  a este se hara un primer ajuste solamente indicado la cantidad de bloques
 *
 *  La estructura es la siguiente:
 *  ------------------------------------------------
 *  | DATOS | BITMAP (0000)                        |
 *  ------------------------------------------------
 *
 *  IMPORTANTE: Las estructuras estan definidas en disk.h
 */

    int opcion = 0;
    char nombre_disco[MAX_DISCO];
    char nombre_alumno[MAX_NOMBRE];
    char rutaDisco[MAX_RUTA];
    char carne[MAX_NOMBRE];
    int bloques, inicio, tamano_disco = 0;

    while (opcion != 5){
        printf(" ============================================================================= \n");
        printf("                                Menu principal \n");
        printf(" ============================================================================= \n");
        printf(" 1. Crear disco \n");
        printf(" 2. Ingresar bloques (primer ajuste) \n");
        printf(" 3. Eliminar bloques \n");
        printf(" 4. Mostrar disco \n");
        printf(" 5. Salir \n");
        printf(" -----------------------------------------------------------------------------\n");
        printf("Ingrese una opcion: ");
        scanf("%d", &opcion);

        switch (opcion){
        case 1:
            // Seccion para crear el disco
            system("clear");
            printf(" ============================================================================= \n");
            printf("                                 Crear disco \n");
            printf(" ============================================================================= \n");
            printf(" Ingrese el nombre del disco: ");
            scanf("%s", &nombre_disco);
            printf(" Ingrese el tamano del disco (en MB): ");
            scanf("%d", &tamano_disco);
            printf(" Ingrese al ruta del disco: ");
            scanf("%s", &rutaDisco);
            printf(" Ingrese el nombre del alumno: ");
            // %limite[regex]
            scanf("%s", nombre_alumno);
            printf(" Ingrese el carne: ");
            scanf("%s", &carne);
            printf(" Ingrese la cantidad de bloques: ");
            scanf("%d", &bloques);
            crear_disco(nombre_disco, tamano_disco, nombre_alumno, carne, bloques, rutaDisco);
            break;

        case 2:
            system("clear");
            printf(" ============================================================================= \n");
            printf("                                Ingresar bloques \n");
            printf(" ============================================================================= \n");
            printf(" Ingrese el nombre del disco: ");
            scanf("%s", &nombre_disco);
            printf(" Ingrese al ruta del disco: ");
            scanf("%s", &rutaDisco);
            printf(" Ingrese la cantidad de bloques: ");
            scanf("%d", &bloques);
            ingresar_bloques(nombre_disco, bloques,rutaDisco);
            break;

        case 3:
            system("clear");
            printf(" ============================================================================= \n");
            printf("                                Eliminar bloques \n");
            printf(" ============================================================================= \n");
            printf(" Ingrese el nombre del disco: ");
            scanf("%s", &nombre_disco);
            printf(" Ingrese el inicio: ");
            scanf("%d", &inicio);
            printf(" Ingrese la cantidad de bloques: ");
            scanf("%d", &bloques);
            printf(" Ingrese al ruta del disco: ");
            scanf("%s", &rutaDisco);
            borrar_bloques(nombre_disco, inicio, bloques, rutaDisco);
            break;

        case 4:
            // Mostrar el disco
            system("clear");
            printf(" ============================================================================= \n");
            printf("                                 Mostrar disco \n");
            printf(" ============================================================================= \n");
            printf(" Ingrese el nombre del disco: ");
            scanf("%s", &nombre_disco);
            printf(" Ingrese al ruta del disco: ");
            scanf("%s", &rutaDisco);
            mostrar_disco(nombre_disco,rutaDisco);
            break;
        }


    }
    return 0;
}


