#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED

#include <stdarg.h>

double V(c *conf, double x)
{
    if (x >= conf->a && x <= conf->b)
        return 0; // Pozo cuadrado  de profundidad V0 en eV.
    else
        return conf->V0;
}

void super_print(FILE *output_file, const char *string, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, string);
    vsprintf(buffer, string, args);

    printf(buffer);
    fprintf(output_file, buffer);
    va_end(args);
}

void find_max_min_potential(c *conf, double *Vmax, double *Vmin)
{
    int i = 0;
    *Vmax = V(conf, i);
    *Vmin = V(conf, i);
    double new_V;
    double Dx = 2 * (conf->b - conf->a) / (conf->n - 1); //Delta de x

    for (int i = 0; i <= conf->n; i++)
    {
        // Esto solo funciona con esta configuración. La manera correcta de hacerlo no es desde
        // cero sino desde los extremos del intervalo / pozo
        new_V = V(conf, Dx * i);

        if (*Vmax < new_V)
            *Vmax = new_V;
        if (*Vmin > new_V)
            *Vmin = new_V;
    }
}

int matriz_diagonal(c *conf,
                    double *diagonal_principal,
                    double *diagonal_secundaria,
                    double **tridiagonal)
{
    diagonal_principal = (double *)calloc(conf->n, sizeof(double));
    diagonal_secundaria = (double *)calloc(conf->n, sizeof(double));
    double *identidad = (double *)calloc(conf->n * conf->n, sizeof(double));

    FILE *diagonal_matriz_out_file;
    diagonal_matriz_out_file = fopen("Schr_pozo_matriz.txt", "w");

    double pi = 2 * asin(1.);

    double Dx = 2 * a_angstroms_p / (conf->n - 1); //Delta de x
    double x1 = conf->a - (conf->n - 1) / 4 * Dx;
    double x2 = conf->b + (conf->n - 1) / 4 * Dx;
    double xi;

    double EeV, EdivE0, EinfeV, E1infeV;
    E1infeV = pi * pi / 2. * conf->h * conf->h / conf->muma / a_angstroms_p / a_angstroms_p / conf->electron_charge / conf->uma * 1.e-2; //Nivel fundamental

    // 2 m qe/h^2 en 1/(eV*A^2)
    double gamma = 2 * conf->muma * conf->uma * conf->electron_charge / conf->h / conf->h * 1.e2;

    super_print(diagonal_matriz_out_file, "\nDatos del calculo: POZO CUADRADO");
    super_print(diagonal_matriz_out_file, "\nm =%10le kg = %5.3lf  uma  h = %le (hbarra)", mass_p, conf->muma, conf->h * 1.e-34);
    super_print(diagonal_matriz_out_file, "\nParedes del pozo cuadrado: %lf %lf angstrom", conf->a, conf->b);
    super_print(diagonal_matriz_out_file, "\naltura de las paredes %lf eV", conf->V0);
    super_print(diagonal_matriz_out_file, "\nDx: %lf A, no. de puntos calculados de Fi: %i", Dx, conf->n);
    super_print(diagonal_matriz_out_file, "\nLista la función de onda número: %i", conf->nivel_elegido);
    super_print(diagonal_matriz_out_file, "\nExtremos del calculo: %lf, %lf A ", x1, x2);
    super_print(diagonal_matriz_out_file, "\nla F. de O. se hace cero (V => infinito) en los extremos");
    super_print(diagonal_matriz_out_file,
                "Lista algunos valores de la diagonal:2+ 2*m*Vi*Dx^2/h^2\
        \nNum  2+alpha     x[i](A)     vi      eV\
        \n=================================================");

    /* ejemplo diagonalizacion
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
            z[i + n * j] = 0.;
        d[i] = 2.;
        e[i] = -1.;
        z[i + n * i] = 1.;
    }
    */

    for (int i = 0; i < conf->n; i++)
    {
        xi = x1 + i * Dx;
        // los valores de la diagonal 2+ vi =2+ V(xi)*2m*Dx^2/h^2
        diagonal_principal[i] = 2. + V(conf, xi) * Dx * Dx * gamma;

        diagonal_secundaria[i] = -1.;

        /* 
        z[] matriz identidad n x n. Listada por filas
        En este programa solo lista los autovalores y un autovector
        */
        identidad[i + conf->n * i] = 1.;

        if (!(i % ((conf->n - 1) / 10))) //Lista solo uno de cada 100 valores de las vi
        {
            EeV = V(conf, xi);
            super_print(diagonal_matriz_out_file, "\n %i %10lf %10le %le %lf", i, diagonal_principal[i], xi, V(conf, xi) * Dx * Dx * gamma, EeV);
        }
    }

    int error; // Este valor es el que devuelve la funcion
    printf("\n\nDiagonalizando...");
    error = tql2(conf->n, diagonal_principal, diagonal_secundaria, identidad);
    printf("\nDiagonalizacion finalizada");

    super_print(diagonal_matriz_out_file, "\nLista %i primeros AUTOVALORES Y ENERIAS", conf->ilist);
    super_print(diagonal_matriz_out_file, "\nn  lambda_n    En(eV)    E pozo inf(eV)      E/E0");

    for (int i = 0; i < conf->ilist; i++)
    {
        EeV = diagonal_principal[i] / Dx / Dx / gamma;
        EdivE0 = diagonal_principal[i] / diagonal_principal[0];

        //Energías para el pozo infinito en eV (calculo analitico para comparar)
        EinfeV = E1infeV * (i + 1) * (i + 1); //Otros niveles
        super_print(diagonal_matriz_out_file, "\n%i %10lf %10lf %10lf %10lf", i, diagonal_principal[i], EeV, EinfeV, EdivE0);
    }

    fclose(diagonal_matriz_out_file);
    *tridiagonal = identidad;
    return error;
}

void plot(c *conf, double Vmax, double Vmin, double Phi2_max)
{
    FILE *plot_load;
    double Dx = 2 * (conf->b - conf->a) / (conf->n - 1); //Delta de x
    double x1 = conf->a - (conf->n - 1) / 4 * Dx;
    double x2 = conf->b + (conf->n - 1) / 4 * Dx;
    plot_load = fopen("Plot_Schr_pozo.plt", "w");
    fprintf(plot_load, "set ytics nomirror");
    fprintf(plot_load, "\nset y2tics");
    fprintf(plot_load, "\nset xrange [%lf:%lf]", x1, x2);
    fprintf(plot_load, "\nset yrange [%lf:%lf]", -sqrt(Phi2_max) * 1.1, sqrt(Phi2_max) * 1.1);
    fprintf(plot_load, "\nset y2range [%lf:%lf]", Vmin - (Vmax - Vmin) * 0.1, Vmax + (Vmax - Vmin) * 0.1);
    fprintf(plot_load, "\nset xlabel 'x(A)' ");
    fprintf(plot_load, "\nset ylabel 'F(x) A-1' ");
    fprintf(plot_load, "\nset y2label 'V(x) eV' ");
    fprintf(plot_load, "\nplot 'Schr_pozo_FdO.txt' using 1:2 axes x1y1 with lines lt 7 title 'F(x) A-1',%c", 92);
    fprintf(plot_load, "\n     'Schr_pozo_FdO.txt' using 1:3 axes x1y1 with lines lt 3 title 'F(x) pozo infinito',%c", 92);
    fprintf(plot_load, "\n     'Schr_pozo_FdO.txt' using 1:4 axes x1y2 with lines lt 1 title 'V(x) eV)'");
    fclose(plot_load);
    system("Plot_Schr_pozo.plt");
}

double foo(c *conf, double Vmax, double Vmin, double *matriz_tridiagonal)
{
#define nivel_elegido (conf->nivel_elegido)
#define n (conf->n)

    double xmed, x2med, P2med;

    double Potencial;
    double k, k0a, ro;
    double u1, u2, u, v; //Para calcular energia analiticamente
    double zteo;

    double Dx = 2 * a_angstroms_p / (n - 1); // Delta de x
    double x;
    double x1 = conf->a - (n - 1) / 4 * Dx; // x del borde izquierdo
    double x2 = conf->b + (n - 1) / 4 * Dx; // x del borde derecho
    double xi;

    double Norma = 0;
    double Phi2_max = 0.; // Valor maximo del cuadrado

    int index;
    for (int i = 0; i < n; i++)
    {
        index = i + nivel_elegido * n;
        Norma += matriz_tridiagonal[index] * matriz_tridiagonal[index];
        if (Phi2_max < matriz_tridiagonal[index] * matriz_tridiagonal[index])
            Phi2_max = matriz_tridiagonal[index] * matriz_tridiagonal[index];
    }
    Norma = sqrt(Dx * (Norma - (matriz_tridiagonal[0 + nivel_elegido * n] * matriz_tridiagonal[0 + nivel_elegido * n] + matriz_tridiagonal[n - 1 + nivel_elegido * n] * matriz_tridiagonal[n - 1 + nivel_elegido * n]) / 2.));
    if (matriz_tridiagonal[(int)floor((conf->a - x1) / Dx) + nivel_elegido * n] < 0)
        Norma = -Norma; //Elige el signo para que F(a) >0
    Phi2_max = Phi2_max / Norma / Norma;

    // Normalizar matriz trigonal
    for (int i = 0; i < n; i++)
    {
        index = i + nivel_elegido * n;
        matriz_tridiagonal[index] = matriz_tridiagonal[index] / Norma;
    }

    // Calcula <x>, Delta x, <P^2> = (Delta P)^2
    xmed = 0.;
    x2med = 0.;
    P2med = 0.;
    for (int i = 1; i <= n - 2; i++)
    {
        index = i + nivel_elegido * n;
        x = x1 + i * Dx;
        xmed = xmed + x * matriz_tridiagonal[index] * matriz_tridiagonal[index] * Dx;
        x2med = x2med + x * x * matriz_tridiagonal[index] * matriz_tridiagonal[index] * Dx;
        P2med = P2med + (matriz_tridiagonal[i + 1 + nivel_elegido * n] - matriz_tridiagonal[i - 1 + nivel_elegido * n]) * (matriz_tridiagonal[i + 1 + nivel_elegido * n] - matriz_tridiagonal[i - 1 + nivel_elegido * n]) / 4. / Dx;
    }

    printf("\n <x> = %lf", xmed);
    printf("\n <x2> = %lf, Delta x = %lf ", x2med, sqrt(x2med - xmed * xmed));
    printf("\n <P2> = %lf, Delta p = %lf, Delta x * Delta p = %lf*hbarra", P2med, sqrt(P2med), sqrt(x2med - xmed * xmed) * sqrt(P2med));
    printf("\n pulsar una tecla");
    getchar();

    printf("\n\n\n Teclear <RET> para listar funcion de onda No %i", nivel_elegido);
    getchar();
    printf("\n\n no punto Fi(x_I)       Fi_analitica");
    //fprintf(pfileout, "\n\n no punto Fi(x_I)       Fi_analitica");

    FILE *funcion_onda_file = fopen("Schr_pozo_FdO.txt", "w");
    fprintf(funcion_onda_file, "#_no punto Fi(x_I)  Fi_Vinf, analitica  Potencial(escalado)");
    //for(i=0;i<n;i+=(n-1)/100) // para listar uno cada 100 puntos

    double pi = 2 * asin(1);
    for (int i = 0; i < n; i++)
    {
        index = i + nivel_elegido * n;
        // Lista funcion de onda analitica para el pozo de profundidad infinita

        // Pozo cuadrado infinito. funcion de onda analitica para nivel nf
        x = (i - n / 4.) * Dx + conf->a;
        if (x >= conf->a && x <= conf->b)
            zteo = sqrt(2. / a_angstroms_p) * sin((nivel_elegido + 1) * pi * (x - conf->a) / a_angstroms_p);
        else
            zteo = 0.;
        //Pozo cuadrado de profundidad V0 f.d.o. fundamental. Calculo analitico
        /*En construccion :
			// k se obtiene de la ec. cos(ka/2)=k/k0, CT 47-a
			k0a = sqrt(2 * m * V0 * qe / h / h) * a_angstroms_p;
		//biseccion para acotar
		u1 = 0; //u= k*a/2
		u2 = pi / 2.;
		while (u2 - u1 > 1.e-3)
		{
			u = (u1 + u2) / 2.;
			v = 2 * u / k0a;
			if (cos(u) - v >= 0)
				u1 = u;
			else
				u2 = u;
		}
		//Newton para precisar
		for (j = 1; j < 10; j++)
			u = u + (cos(u) - v) / (sin(u) - 2 / k0a);
		//f.d.o. analitica
		roa = sqrt(k0a * k0a - 4 * u * u); // ro*aa
 xa=(i*Dx-(a+b)/2/(b-a);  // x/aa respecto del centro del pozo
 zteo=
 //etc...
 */
        //Normaliza la F. de O. Elige el signo para que F(a) >0

        printf("\n %5i\t%lf\t%lf", i, matriz_tridiagonal[index], zteo);
        //fprintf(pfileout, "\n %5i\t%lf\t%lf", i, matriz_tridiagonal[index], zteo);
        Potencial = (V(conf, x) - Vmin) / (Vmax - Vmin) * sqrt(Phi2_max); //Escala el potencial para dibujarlo con la F. d. O.
        fprintf(funcion_onda_file, "\n %lf\t%lf\t%lf\t%lf", x, matriz_tridiagonal[index], zteo, V(conf, x));
    }
#undef index
#undef n
#undef nivel_elegido

    return Phi2_max;
}
#endif //FUNCIONES_H_INCLUDED