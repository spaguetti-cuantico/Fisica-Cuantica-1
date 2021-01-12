#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED

#include <stdarg.h>

//Funcion potencial
double V(configuration *Config, double phi)
{
    /* Potencial 
    *    V(phi) = V0*cos(nv*phi)
    *
    *   se calcula 2*I*V/h^2
    *   V0 = 2*I*Vmax/hbar^2
    */
    return Config->V0 * cos(Config->nv * phi * pi / 180.) + Config->V0 * Config->epsilon * cos(phi * pi / 180.);
}

//printf + fprintf
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

//Intral de V
double IntegralV(configuration *Config, int i, int j)
{
    //dphi = incremento en phi para la integral
    double I, phi, dphi = 0.2;
    I = 0.;

    for (phi = dphi; phi <= 360.; phi += dphi)
        I += V(Config, phi) * cos((i - j) * phi * pi / 180.);

    return I * dphi / 360.;
}

// Escribe en pantalla los datos del programa
void muestra_configuracion(configuration *Config)
{
    printf("\n programa Rotor_1D");
    printf("\n Energias = hbar^2/2I * autovalores");
    printf("\n Potencial: V0*cos( %i*phi), V0 = %lf", Config->nv, Config->V0);
    printf("\n base ortonormal de funciones usada: 1/sqrt(2*pi)*exp(i*m*phi)");
    printf("\n l = Maximo |m|: %i   -l <= m <= l Numero de funciones de base: %i ", Config->l, 2 * Config->l + 1);
    printf("\n Funciones de onda para dibujar:  F%i(phi)  - F%i(phi)", Config->nfun_i, Config->nfun_f);
    //printf("\n pulsar <RET> para continuar");
    //getchar();
}

void asigna_memoria(configuration *Config, double **a, double **a2, double **w, double **x, double **c, double **r)
{
    int n = 2 * Config->l + 1;

    *a = (double *)calloc(n * n, sizeof(double));
    *a2 = (double *)calloc(n * n, sizeof(double));
    *x = (double *)calloc(n * n, sizeof(double));
    *w = (double *)calloc(n, sizeof(double));
    *c = (double *)calloc(n, sizeof(double));
    *r = (double *)calloc(n * n, sizeof(double));
}

void libera_memoria(double *a, double *a2, double *w, double *x, double *c, double *r)
{
    free(a);
    free(a2);
    free(x);
    free(w);
    free(c);
    free(r);
}

void prepara_matrices(configuration *Config, double *a, double *a2)
{
    // Calcula elementos de matriz del hamiltoniano
    // psi(phi) Sum_j=0^n c[j]*exp(i*(j-l)*phi)/sqrt(2*pi)
    double H[100][100];
    int n = 2 * Config->l + 1;

    for (int i = 0, j; i < n; i++)
    {
        for (j = i; j < n; j++)
        {
            H[i][j] = IntegralV(Config, i, j);
            if (i == j)
                H[i][j] = H[i][j] + (i - Config->l) * (i - Config->l);
        }
    }

    for (int i = 1, j; i < n; i++)
        for (j = 0; j <= i - 1; j++)
            H[i][j] = H[j][i];

    for (int i = 0, j; i < n; i++)
        for (j = 0; j < n; j++)
            a[i + j * n] = H[i][j];

    /*  
    *Guarda una copia de la matriz, por filas. 
    *la original estara diagonalizada al salir de la funcioon rs
    */
    for (int j = 0, i; j < n; j++)
    {
        for (i = 0; i < n; i++)
        {
            a2[i + j * n] = a[i + j * n];
        }
    }
}

//Devuelve un entero codigo de error
int diagonaliza_matriz(configuration *Config, double *a, double *a2, double *w, int flag_autvector, double *x, double *r)
{

    int n = 2 * Config->l + 1;

    //DIAGONALIZA POR EL ALGORITMO QR:
    int ierr = rs(n, a, w, flag_autvector, x);

    /* 
    *A partir de aqui:
    *   n = 2l+1: numero de funciones de la base = numero de autovectores calculados
    *   
    *   w[]  array 1D de n = 2l+1 elementos con los autovalores
    *
    *   x[] = autovectores = coeficientes de las autofunciones en la base tomada,
    *       todos seguidos en un array 1D de n*n = (2l+1)*(2l+1) elementos
    *
    *   flag_autvector = valor dado a la entrada: 1 = calcular autovalores y autovectores, flag_autvector = 0: solo autovalores
    */

    if (ierr != 0) //Escribe mensaje de error si hay alguno
    {
        printf("\n");
        printf("TEST06 - Warning!\n");
        printf("  The error return flag IERR = %d\n", ierr);
        return ierr;
    }

    //Funcion que escribe el vector w[n]
    r8vec_print(n, w, "  Autovalores Lambda\n(Energias en unidades de beta, con base en la minima E potencial):");

    //Si se quieren los autovectores
    if (flag_autvector != 0)
    {
        //Multiplica las matrices a2[n x n] * x[n x n]:
        r = r8mat_mm_new(n, n, n, a2, x);

        for (int j = 0, i; j < n; j++)
        {
            for (i = 0; i < n; i++)
            {
                r[i + j * n] = r[i + j * n] - w[j] * x[i + j * n];
            }
        }
    }
    return ierr;
}

void genera_plots_Elias(configuration *Config, double *w, double *x, double *c)
{
    FILE *pflevels, *pfun;
    double phi;
    double Sci, psi_re, psi_im; // partes real e imaginaria de la funcion de onda
    double xmin, xmax, ymin, ymax, real;

    int n = 2 * Config->l + 1;

    //Guarda los Niveles obtenidos en fichero de texto y lo dibuja junto con el potencial
    xmin = -180;
    xmax = 180;
    ymin = -fabs(Config->V0) * 1.1;
    ymax = fabs(Config->V0) * 4;
    pflevels = fopen("R_1D_levels.plt", "w");
    fprintf(pflevels, "unset key");
    fprintf(pflevels, "\nset xrange [%4.0lf:%4.0lf]", xmin, xmax);
    fprintf(pflevels, "\nset yrange [%lf:%lf]", ymin, ymax);
    fprintf(pflevels, "\n# Autovalores (Niveles de energia * 2I/hb^2)");
    fprintf(pflevels, "\nplot %lf lt -1 lw 2", w[0]);
    for (int i = 1; i < n; i++)
        fprintf(pflevels, "\nreplot %lf lt -1 lw 2", w[i]);
    fprintf(pflevels, "\n# Potential");
    fprintf(pflevels, "\nreplot '-' with filledcurves y1=%lf lt 1 ", Config->V0 * 1.1);
    for (phi = -180; phi <= 360; phi = phi + 1)
        fprintf(pflevels, "\n %lf %lf", phi, V(Config, phi));
    fprintf(pflevels, "\nend");
    fclose(pflevels);

    //Lista, guarda y dibuja los valores de las funciones de onda solicitadas

    printf("\n\n Funciones de onda nºs: %i - %i", Config->nfun_i, Config->nfun_f);
    printf("\n\n phi(�)   Re_psi     Im_psi");
    xmin = -180.;
    xmax = 180.;
    ymin = -1.2;
    ymax = 1.2;
    pfun = fopen("R_1D_func.plt", "w");
    fprintf(pfun, "set multiplot");
    fprintf(pfun, "\nset xrange [%4.0lf:%4.0lf]", xmin, xmax);
    fprintf(pfun, "\nset yrange [%4.1lf:%4.1lf]", ymin, ymax);
    fprintf(pfun, "\nset nokey");
    fprintf(pfun, "\nset title \"F. de onda %i - %i\"", Config->nfun_i, Config->nfun_f);
    //fprintf(pfun,"plot 0 with lines");
    fprintf(pfun, "\n# Potencial");
    fprintf(pfun, "\nset label \"Potencial\" at %lf,%lf", xmin + (xmax - xmin) * 0.1, ymax - (ymax - ymin) * 0.1);
    fprintf(pfun, "\nplot 0 with lines lt -1 "); // Dibuja la linea de cero
    fprintf(pfun, "\nplot '-' with lines lt 2 ");
    for (phi = -180; phi <= 180; phi = phi + 1)
        fprintf(pfun, "\n %lf %lf", phi, V(Config, phi) / fabs(Config->V0));
    fprintf(pfun, "\nend");

    // Funciones de onda
    for (int j = Config->nfun_i, i; j <= Config->nfun_f; j++)
    {
        Sci = 0.; //Acumulador para normalizar
        for (i = 0; i < n; i++)
            Sci = Sci + x[i + j * n] * x[i + j * n];
        printf("\n\n  Coeficientes de la funcion de onda: %i", j);
        for (i = 0; i < n; i++)
        {
            c[i] = x[i + j * n];
            printf("\n %i %lf", i, c[i]);
        }

        fprintf(pfun, "\nplot '-' using 1:2 with lines lt %i", j + 3);
        real = 0; //Para determinar si la  F.de.O es real o imaginaria
        for (phi = -180.; phi <= 180.; phi = phi + Config->dphiL)
        {
            psi_re = 0.;
            psi_im = 0.;
            for (i = 0; i < n; i++)
            {
                psi_re = psi_re + c[i] * cos((i - Config->l) * phi * pi / 180.) / sqrt(2 * pi);
                psi_im = psi_im + c[i] * sin((i - Config->l) * phi * pi / 180.) / sqrt(2 * pi);
                real = real + fabs(psi_re) - fabs(psi_im); //Integra la diferencia para ver si es mayor la parte real
            }
            printf("\n %lf %lf %lf", phi, psi_re, psi_im);
            if (real >= 0)
                fprintf(pfun, "\n %lf %lf", phi, psi_re);
            else
                fprintf(pfun, "\n %lf %lf", phi, psi_im);
        }
        fprintf(pfun, "\nend");
    }

    fclose(pfun);
}
#endif //FUNCIONES_H_INCLUDED