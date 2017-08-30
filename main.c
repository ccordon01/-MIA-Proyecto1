#include <stdio.h>
#include <locale.h>
#include "disk.h"
#include "estructurasdisk.h"
#define MAX_DISCO 20
//#define MAX_NOMBRE 50
#define MAX_RUTA 50

// Variables globales

char nombre_disco[MAX_DISCO];
char rutaDisco[MAX_RUTA];
int main()
{
    isEmpty(nombre_disco,"El nombre del disco esta vacio");
    /*SB super1;
    printf("%d \n", sizeof(super1));
    J jour;
    printf("%d \n", sizeof(jour));
    I inodos;
    printf("%d \n", sizeof(inodos));
    BA ba;
    printf("%d \n", sizeof(ba));
    BC bc;
    printf("%d \n", sizeof(BC));
    BI bi;
    printf("%d \n", sizeof(bi));
    BM bm;
    printf("%d \n", sizeof(bm));*/
    setlocale(LC_ALL, "spanish");
    //char nombre_alumno[MAX_NOMBRE];
    //char carne[MAX_NOMBRE];
    MenuPrincipal();
    return 0;
}

void MenuPrincipal(){
    int opcion = 0;
    int carne, inicio, tamano_disco = 0;
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
            /*printf(" Ingrese el nombre del alumno: ");
            // %limite[regex]
            scanf("%s", nombre_alumno);*/
            printf(" Ingrese el registro academico: ");
            scanf("%s", &carne);
            /*printf(" Ingrese la cantidad de bloques: ");
            scanf("%d", &bloques);*/
            crear_disco(nombre_disco, tamano_disco, carne, rutaDisco);
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
            /*printf(" Ingrese la cantidad de bloques: ");
            scanf("%d", &bloques);*/
            //ingresar_bloques(nombre_disco, bloques,rutaDisco);
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
            //scanf("%d", &bloques);
            printf(" Ingrese al ruta del disco: ");
            scanf("%s", &rutaDisco);
            //borrar_bloques(nombre_disco, inicio, bloques, rutaDisco);
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
}

int isEmpty(char * cadena,char * aviso){
    if(cadena[0]=='\0'){
        printf(aviso);
        return 0;
    }
    return 1;
}
