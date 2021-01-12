#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef ROTOR_1D_INPUT_H_INCLUDED
#define ROTOR_1D_INPUT_H_INCLUDED

/**
Esta libreria ha sido creada por un sript de python creado por Unai Leria Fortea
En caso de algun problema envie un mensaje a unaileria@gmail.com
O envie su duda por el servidor de discord https://discord.gg/N48TNVF
**/

typedef struct Configuration
{
  double hb; //h barra, pero no se usa, las energias se dan en unidades de hb^2/(2*Inercia)
  int l; //l = m maximo ( -l <= m <= l, i = m+l, 0<=i<n, n = 2l+1)
  int nfun_i; //numero de funcion inicial
  int nfun_f; //numero de funcion final
  double dphiL; //incremento de phi para listar
  double V0; //Minimo potencial
  double epsilon; //bias del potencial(minimos no equivalentes).
  char nv; //para potencial V0cos(nv*phi)
} configuration; 

int lee_configuracion(configuration *Config)
{
FILE *F;    
    unsigned int Fsize;
    char *Fcopy;

    F = fopen("rotor_1D_config.txt", "rt");
        
    fseek(F, 0, SEEK_END);
    Fsize = ftell(F);
    rewind(F);
        
    Fcopy = (char *)malloc(sizeof(char) * Fsize);
    fread(Fcopy, 1, Fsize, F);
    fclose(F);

        
    char *p;
    char s[25];
    

    strcpy(s, "hb=");
    p = strstr(Fcopy, s);
    Config->hb = atof(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de hb invalido\n");
        free(Fcopy);
        return 0;
    }


    strcpy(s, "l=");
    p = strstr(Fcopy, s);
    Config->l = atoi(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de l invalido \n'");
        free(Fcopy);
        return 0;
    }


    strcpy(s, "nfun_i=");
    p = strstr(Fcopy, s);
    Config->nfun_i = atoi(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de nfun_i invalido \n'");
        free(Fcopy);
        return 0;
    }


    strcpy(s, "nfun_f=");
    p = strstr(Fcopy, s);
    Config->nfun_f = atoi(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de nfun_f invalido \n'");
        free(Fcopy);
        return 0;
    }


    strcpy(s, "dphiL=");
    p = strstr(Fcopy, s);
    Config->dphiL = atof(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de dphiL invalido\n");
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


    strcpy(s, "epsilon=");
    p = strstr(Fcopy, s);
    Config->epsilon = atof(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de epsilon invalido\n");
        free(Fcopy);
        return 0;
    }


    strcpy(s, "nv=");
    p = strstr(Fcopy, s);
    Config->nv = atoi(p + strlen(s));
    if (0)
    {
        printf("ERROR:valor de nv invalido\n");
        free(Fcopy);
        return 0;
    }


    free(Fcopy);
    return 1;
}
#endif //ROTOR_1D_INPUT_H_INCLUDED