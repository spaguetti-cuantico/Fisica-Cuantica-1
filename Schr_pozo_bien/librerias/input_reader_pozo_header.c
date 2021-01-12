#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef INPUT_READER_POZO_INPUT_H_INCLUDED
#define INPUT_READER_POZO_INPUT_H_INCLUDED

/**
Esta libreria ha sido creada por un sript de python creado por Unai Leria Fortea
En caso de algun problema envie un mensaje a unaileria@gmail.com
O envie su duda por el servidor de discord https://discord.gg/N48TNVF
**/

typedef struct C
{
  int n;
  int ilist;
  double a; //Borde izquierdo
  double b; //Borde derecho
  int nivel_elegido; //Funcion de onda para listar, 0 = fundamental
  double electron_charge; //Carga del electron (x 10^-19)
  double h; //h barra (x10^-34)
  double uma; //una en kg (x10^-27)
  double muma; //Masa en kg
  double V0; //Potencial del pozo en eV
} c; 

int lee_configuracion(c *Config)
{
    FILE *F;    
    unsigned int Fsize;
    char *Fcopy;

    F = fopen("input_reader_pozo_config.txt", "r");
        
    fseek(F, 0, SEEK_END);
    Fsize = ftell(F);
    rewind(F);
        
    Fcopy = (char *)malloc(sizeof(char) * Fsize);
    fread(Fcopy, 1, Fsize, F);
    fclose(F);

        
    char *p;
    char s[25];
    

    strcpy(s, "n=");
    p = strstr(Fcopy, s);
    Config->n = atoi(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de n invalido \n'");
        free(Fcopy);
        return 0;
    }


    strcpy(s, "ilist=");
    p = strstr(Fcopy, s);
    Config->ilist = atoi(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de ilist invalido \n'");
        free(Fcopy);
        return 0;
    }


    strcpy(s, "a=");
    p = strstr(Fcopy, s);
    Config->a = atof(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de a invalido\n");
        free(Fcopy);
        return 0;
    }


    strcpy(s, "b=");
    p = strstr(Fcopy, s);
    Config->b = atof(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de b invalido\n");
        free(Fcopy);
        return 0;
    }


    strcpy(s, "nivel_elegido=");
    p = strstr(Fcopy, s);
    Config->nivel_elegido = atoi(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de nivel_elegido invalido \n'");
        free(Fcopy);
        return 0;
    }


    strcpy(s, "electron_charge=");
    p = strstr(Fcopy, s);
    Config->electron_charge = atof(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de electron_charge invalido\n");
        free(Fcopy);
        return 0;
    }


    strcpy(s, "h=");
    p = strstr(Fcopy, s);
    Config->h = atof(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de h invalido\n");
        free(Fcopy);
        return 0;
    }


    strcpy(s, "uma=");
    p = strstr(Fcopy, s);
    Config->uma = atof(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de uma invalido\n");
        free(Fcopy);
        return 0;
    }


    strcpy(s, "muma=");
    p = strstr(Fcopy, s);
    Config->muma = atof(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de muma invalido\n");
        free(Fcopy);
        return 0;
    }


    strcpy(s, "V0=");
    p = strstr(Fcopy, s);
    Config->V0 = atof(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de V0 invalido\n");
        free(Fcopy);
        return 0;
    }


    free(Fcopy);
    return 1;
}
#endif //INPUT_READER_POZO_INPUT_H_INCLUDED