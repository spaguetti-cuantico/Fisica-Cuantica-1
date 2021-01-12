#include <stdlib.h>
#include <stdio.h>
#include <time.h>

//# include "librerias/eispack.h"
#include "librerias/eispack.c"

#define mass (conf.muma * conf.uma * 1.e-27)
#define mass_p (conf->muma * conf->uma * 1.e-27)

#define a_angstroms (conf.b - conf.a)
#define a_angstroms_p (conf->b - conf->a)

#include "librerias/input_reader_pozo_header.c"
#include "librerias/funciones.c"

//Resuelve la ec de Schrodinger unidimensional para un pozo cuadrado
// de  potencial  1D (a, b) en metros: paredes

int main()
{ /* Metodo de diferencias finitas.
    Sea la funcion de onda F(x)
    y la Ec. de Schrodinger 1d: [-h^2/m diagonal_principal^2/dx^2-V(x)]F(x)=EF(x)
    Se aproxima  diagonal_principal^2F/dx^2 = (Fi+1+Fi-1-2*Fi)/Dx^2
    Definiendo alpha(x) = (2*m*Dx^2/h^2)*V(x) y lambda = (2*m*Dx^2/h^2)*E
    Se cumple la ecuacion en diferencias finitas:
    (2+alpha(x_i))*Fi -Fi+1-Fi-1 =lambda*Fi
    Considerando las Fi como componentes de un vector, 
    es una ecuaci�n de autovalores y autovectores.
    
    Se resuelve num�ricamente usando el paquete libre eispack (de GNU) que
    obtiene los autovalores y autovectores (valores de la autofuncion para cada
    autovalor)
    Se supone que el potencial debe ser muy alto excepto para x1 < x < x2.
    unico rango de valores con F ditinta de cero. 
  */
	c conf;
	lee_configuracion(&conf);

	double pi = 2 * asin(1);

	double Phi2_max;
	double Vmax, Vmin;

	// Esto no sé por qué está aquí. Pero lo dejo
	if (conf.n < conf.ilist)
	{
		conf.ilist = conf.n;
	}

	/* 
	Límites de x para calcular la FdO
 	la mitad de los puntos dentro del pozo y la mitad fuera 
	*/

	// Espaciado en metros entre puntos para calcular la funcion de onda
	double Dx = 2 * a_angstroms / (conf.n - 1);

	// Limites de x para calcular: se ha tomado el doble de la anchura del pozo dada
	double x1 = conf.a - (conf.n - 1) / 4 * Dx;
	double x2 = conf.b + (conf.n - 1) / 4 * Dx;

	// Puntos del borde del pozo
	int ia = floor((conf.a - x1) / Dx);
	int ib = ceil((conf.b - x1) / Dx);

	// Hallamos máximo y mínimo del potencial
	find_max_min_potential(&conf, &Vmax, &Vmin);

	/* 	Variables para la funcion de diagonalizacion de una matriz simetrica tridiagonal
 		diagonal_principal = 
		 	entrada: diagonal mayor de la matriz 2+alpha[i]
       		salida: vector de autovalores

		diagonal_secundaria = 
			diagonales superior e inferior
		en este problema diagonal_secundaria[i]=-1, diagonal_secundaria[0] no se usa

		matriz_tridiagonal= 
			matriz Para convertir matriz sim�trica en tridiagonal. 
		Para tridiagonal (es el caso) matriz_tridiagonal no se usa y se da la matriz identidad listada por filas.
	*/
	double *diagonal_principal, *diagonal_secundaria, **matriz_tridiagonal;
	matriz_tridiagonal = (double **)malloc(sizeof(double **));
	int err = matriz_diagonal(&conf, diagonal_principal, diagonal_secundaria, matriz_tridiagonal);

	//Funcion de onda no. nf (elegida arriba)
	// Hay n calculadas, que se pueden listar y/o guardar poniendo un for aqui
	//Calcula la norma de la F. de. O no nf: integral trapecio
	Phi2_max = foo(&conf, Vmax, Vmin, *matriz_tridiagonal);

	printf("\n Energias en Schr_pozo.txt\n Funcion de onda no %d en  Schr_pozo_FdO.txt", conf.nivel_elegido);

	// Ejecutamos gnuplot
	plot(&conf, Vmax, Vmin, Phi2_max);

	free(diagonal_principal);
	free(diagonal_secundaria);
	free(*matriz_tridiagonal);
	free(matriz_diagonal);
	printf("\n Teclear <RET> para terminar");
	getchar();
}