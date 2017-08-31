#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <math.h>
#include "disk.h"
#include "estructurasdisk.h"
#define MAX_DISCO 20
//#define MAX_NOMBRE 50
#define MAX_RUTA 50

// Variables globales
char nombre_disco[MAX_DISCO];
char rutaDisco[MAX_RUTA];
//char* nombre_disco;
//char* rutaDisco;
int main()
{
    //nombre_disco = (char*)malloc(MAX_DISCO);
    //rutaDisco = (char*)malloc((MAX_RUTA));
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
    while (opcion != 8){
        printf(" ============================================================================= \n");
        printf("                                Menu principal \n");
        printf(" ============================================================================= \n");
        printf(" 1. Crear disco \n");
        printf(" 2. Montar Disco \n");
        printf(" 3. Estado bloques e inodos \n");
        printf(" 4. Desmontar disco \n");
        printf(" 5. Bitacora \n");
        printf(" 8. Salir \n");
        printf(" -----------------------------------------------------------------------------\n");
        printf("Ingrese una opcion: ");
        scanf("%d", &opcion);

        switch (opcion){
        case 1:
            // Seccion para crear el disco
            system("clear");
            if (!isEmpty(rutaDisco,"")) {
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
                scanf("%d", &carne);
                /*printf(" Ingrese la cantidad de bloques: ");
                scanf("%d", &bloques);*/
                crear_disco(nombre_disco, tamano_disco, carne, rutaDisco);
            }else{
                printf(" Existe un disco montado!");
            }
            break;

        case 2:
            system("clear");
            if (!isEmpty(rutaDisco,"")) {
                printf(" ============================================================================= \n");
                printf("                                Montar Disco \n");
                printf(" ============================================================================= \n");
                printf(" Ingrese el nombre del disco: ");
                scanf("%s", &nombre_disco);
                printf(" Ingrese al ruta del disco: ");
                scanf("%s", &rutaDisco);
                if (montar_disco(nombre_disco,rutaDisco)) {
                    printf(" El disco se monto correctamente! \n");
                }else{
                    memset(&rutaDisco,0,sizeof(rutaDisco));
                    memset(&nombre_disco,0,sizeof(nombre_disco));
                    perror(" Error al montar el disco verificar que la ruta este correcta \n");
                }
            }else{
                printf(" Ya existe un disco montado!");
            }
            /*printf(" Ingrese la cantidad de bloques: ");
            scanf("%d", &bloques);*/
            //ingresar_bloques(nombre_disco, bloques,rutaDisco);
            break;

        case 3:
            system("clear");
            if (!isEmpty(rutaDisco,"")) {
                perror(" No existe ningun disco montando!");
            }else{
                printf(" ============================================================================= \n");
                printf("                              Estado De Los BitMap \n");
                printf(" ============================================================================= \n\n");
                estado_bloques(nombre_disco,rutaDisco);
                estado_inodos(nombre_disco,rutaDisco);
            }
            break;

        case 4:
            // Mostrar el disco
            system("clear");
            char op[MAX_RUTA];
            printf(" ============================================================================= \n");
            printf("                               Desmontar disco \n");
            printf(" ============================================================================= \n");
            printf(" Desea desmontar el disco: %s",nombre_disco);
            printf(" Ingrese \"S\" para confirmar.");
            scanf("%s", &op);
            if (strcmp(op, "S") == 0) {
                memset(&rutaDisco,0,sizeof(rutaDisco));
                memset(&nombre_disco,0,sizeof(nombre_disco));
                printf(" El disco fue desmontado correctamente!");
            } else {
                printf(" El disco continuara funcionando en el sistema!");
            }
            break;
        case 5:
            // Mostrar el disco
            system("clear");
            if (!isEmpty(rutaDisco,"")) {
                perror(" No existe ningun disco montando!");
            }else{
                printf(" ============================================================================= \n");
                printf("                                Bitacora \n");
                printf(" ============================================================================= \n\n");
                bitacora(nombre_disco,rutaDisco);
            }
            break;
        }
    }
}

int isEmpty(char * cadena,char * aviso){
    if(cadena[0]=='\0'){
        //strcat(aviso,"\n");
        printf(aviso);
        return 0;
    }
    return 1;
}
