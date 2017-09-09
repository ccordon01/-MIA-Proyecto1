#ifndef SPLIT_H
#define SPLIT_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

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
    free(inicios);
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

#endif // SPLIT_H
