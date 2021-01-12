#include <stdlib.h>
#include <stdio.h>
#include <time.h>

//Variables globales
double pi; //Valor de pi

#include "librerias/eispack.h"
#include "librerias/eispack.c"
#include "librerias/rotor_1D_header.c"
#include "librerias/funciones.h"

//Resuelve el rotor 1D tomando la base exp(i*m*phi)

int main()
{
	/*Asigar valor a variable global*/
	pi = 2 * asin(1.0);

	/*Variables necesarias para el programa*/
	configuration Config;

	int flag_autvector = 1; //flag_autvector = 1; Si  queremos listar autovectores

	//Punteros que apuntan a:
	double *a;	//Matriz de que habra que diagonalizar
	double *a2; //Copia de matriz a

	double *r; //No se muy bien que es. Si alguien lo descrubre que me lo diga. Gracias
	double *w; //Autovalores w[n]
	double *x; //Autovectores x[n*n]
	double *c; //Coeficientes de la funcion de onda en la base exp(i*m*phi)/sqrt(2*pi)

	lee_configuracion(&Config);

	muestra_configuracion(&Config);

	asigna_memoria(&Config, &a, &a2, &w, &x, &c, &r);

	prepara_matrices(&Config, a, a2);

	//AQUI ES DONDE DIAGONALIZA POR EL ALGORITMO QR
	int ierr = diagonaliza_matriz(&Config, a, a2, w, flag_autvector, x, r);
	/* 
    *	A partir de aqui:
    *  		n = 2l+1: numero de funciones de la base = numero de autovectores calculados
    *  		
    *  		w[]  array 1D de n = 2l+1 elementos con los autovalores
    *
    *  		x[] = autovectores = coeficientes de las autofunciones en la base tomada,
    *  		    todos seguidos en un array 1D de n*n = (2l+1)*(2l+1) elementos
    *
    *  		flag_autvector = valor dado a la entrada: 1 = calcular autovalores y autovectores, flag_autvector = 0: solo autovalores
    */

	genera_plots_Elias(&Config, w, x, c);

	libera_memoria(a, a2, w, x, c, r);

	return 0;
}