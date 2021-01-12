#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
//obtiene la evolucion temporal de una funcion de onda
// metodo de Euler explicito,implicito  o Crank-Nicolson, segun el valor del parametro lambda
double a;
int isleep = 500;    // tiempo (ms) de parada  entre dibujo y dibujo, para retardar el programa
float lambda = 0.50; // lambda = 0 => Euler explicito, 1 => implicito, 0.5 = Crank-Nicolson
double V0 = 300.;    // Altura del escalon en eV (o cte de energ�a potencial)

double V(double x) // Funcion que da la energia potencial en eV
{
  double x0 = 280., x1 = 320., x2 = 320.1;
  if (x < x1)
    return 0; //Escalon de potencial
  //if(x<x1 && x > x0) return 0;  //Pozo cuadrado
  //if(x > x1||x < x0) return 0;  //Barrera
  else if (x > x2)
    return V0;
  else
    return V0 * (x - x1) / (x2 - x1); // Escalon no discontinuo: V pasa suavemente de 0 a V0 entre x1 y x2

  //return 1.e-2*V0*(x-x0)/a*(x-x0)/a;  //Arm�nico simple centrado en x0
}
main()
{
  FILE *pfilFdO0, *pfilFdO, *pfilPot, *pfiltxt;
  double D, alpha, FR[20001], FI[20001], FRo[20001], FIo[20001], P[20001], Pold;
  double v[20001];   //Potencial reducido
  double AA, BB, CC; // calculos intermedios
  double t = 0, tmax, dt, x, dx, x0, err = 1.;
  double k0, Norma, xmed, x2med, Deltax, Pmax, Vmax, kmed;
  double Energ = 350.;                               // Energia en eV
  double hbar = 1.054589, m = 9.10953, e = 1.602189; //ctes fundamentales, para SI multiplicar por 1.e-34, 1.e-31,1.e-19
  int j, n, Nx1 = 9000, Nx2 = 12000, Nt = 1000000, jmax;
  dx = 0.03;                                         //Paso de x en A
  dt = 0.005;                                        // Paso de tiempo * 1.e-17 s
  a = 100 * dx;                                      // anchura espacial del paquete. ejemplo: 100 pasos
  x0 = 280.;                                         //Centro inicial del paquete
  tmax = Nt * dt;                                    //tiempo para finalizar
  D = hbar * dt / (2 * m * dx * dx);                 //cte D algo menor que la unidad
  alpha = 1.e-2 * 2 * m * dx / hbar * dx / hbar * e; // Cte para multiplicar por el potencial dado en eV
  k0 = 0.1 * sqrt(2 * m * Energ * e) / hbar;         //No de ondas medio (A-1)
  printf("DATOS DEL PROGRAMA");
  printf("\n Unidades: dt = %lf x 1.e-17s, dx = %lf Angs\n masa m = %lfx1.e-31 kg", dt, dx, m);
  printf("\n D=hbar*dt/(2*m*dx*dx) = %lf\nalpha = dx^2*2*m*e/hbar^2= %lf eV-1, lambda(CN) = %lf", D, alpha, lambda);
  printf("\n Altura escal�n: V0 %lf eV, energ�a: <E> = %lf eV\n k0 = %lf Angs-1", V0, Energ, k0);
  printf("\n Paquete gaussiano, Dx = a/srt(2) = %lf Angs", a / sqrt(2.));

  // Funcion inicial
  FR[Nx1] = 0.;
  FI[Nx1] = 0.;
  FR[Nx2] = 0;
  FI[Nx2] = 0;
  //Paquete gaussiano de anchura espacial a
  Norma = 0;
  for (j = Nx1 + 1; j < Nx2; j++)
  {
    x = j * dx;
    FR[j] = exp(-(x - x0) / a * (x - x0) / a) * cos(k0 * (x - x0));
    FI[j] = exp(-(x - x0) / a * (x - x0) / a) * sin(k0 * (x - x0));
    P[j] = FR[j] * FR[j] + FI[j] * FI[j];
    Norma = Norma + P[j];
  }
  Norma = sqrt(Norma * dx);
  //Normaliza la funci�n dada y guarda el cuadrado en fichero
  pfilFdO0 = fopen("FdO_0.dat", "w");
  fprintf(pfilFdO0, "\n#___x |phi|^2      Re(phi)     Im(phi)");
  for (j = Nx1 + 1; j < Nx2; j++)
  {
    FR[j] = FR[j] / Norma;
    FI[j] = FI[j] / Norma;
    P[j] = P[j] / Norma / Norma;
    fprintf(pfilFdO0, "%lf %.8lf %.8lf %.8lf \n", j * dx, P[j], FR[j], FI[j]);
  }
  fclose(pfilFdO0);
  printf("\n\n Comenzar: Pulsar <RET>");
  getchar();
  // Calcula el potencial en los puntos y lo guarda en fichero (en eV)
  Vmax = 0.;
  for (j = Nx1; j <= Nx2; j++)
  {
    x = j * dx;
    v[j] = V(x);
    v[j] = alpha * v[j]; //Potencial reducido
    if (v[j] > Vmax)
      Vmax = v[j];
  }
  pfilPot = fopen("Potencial.dat", "w");
  //for(j=Nx1;j<=Nx2;j++) fprintf(pfilPot," %lf %.8lf\n",j*dx,0.5*v[j])/Vmax;
  fprintf(pfilPot, "#____x(A)   Epot(eV) )", j * dx, v[j]);
  for (j = Nx1; j <= Nx2; j++)
    fprintf(pfilPot, " %lf %.8lf\n", j * dx, V(j * dx));
  fclose(pfilPot);
  pfiltxt = fopen("Euler.txt", "w");
  fprintf(pfiltxt, "#  t(pasos), xmax , |F|^2max, Norma, xmed, Deltax <k>");
  //Comienza Euler
  for (n = 0; n <= Nt; n++)
  {
    //Renueva f. de onda
    for (j = Nx1; j <= Nx2; j++)
    {
      FRo[j] = FR[j];
      FIo[j] = FI[j];
    }
    if (n % 100 == 0) // Guarda FdO para dibujar, cada 100 pasos de tiempo
    {
      pfilFdO = fopen("FdO.dat", "w");
      fprintf(pfilFdO, "Funcion de onda para n= %i, t = %lf x10^-17 s", n, n * dt);
      fprintf(pfilFdO, "\n#___x |phi|^2      Re(phi)     Im(phi)");
      for (j = Nx1; j <= Nx2; j++)
      {
        fprintf(pfilFdO, "\n%lf %.8lf %.8lf %.8lf", j * dx, P[j], FR[j], FI[j]);
      }

      fclose(pfilFdO);
      if (n == 0) //Inicio del calculo: escribe titulos
      {
        printf("\n Ejecutar Plot_FdO.plt para dibujar la FdO inicial y el potencial. Pulsar <RET>");
        printf("\n\n Pulsar <RET> para iniciar la simulacion");
        printf("\n Pulsar cualquier tecla para detener el programa y para continuar la ejecucion");
        getchar();
        printf("\n\n t(pasos), xmax , |F|^2max, Norma, xmed, Deltax <k>)");
      }
      Sleep(isleep);
      if (kbhit())
      {
        printf("\n");
        system("pause");
      }
    }
    //Euler
    do
    {
      Norma = 0.;
      err = 0;
      FR[Nx1] = 0.;
      FI[Nx1] = 0.;
      FR[Nx2] = 0.;
      FI[Nx2] = 0.;
      for (j = Nx1 + 1; j <= Nx2 - 1; j++)
      {
        AA = D * lambda * (2 + v[j]);
        BB = FRo[j] - D * lambda * (FI[j + 1] + FI[j - 1]) + D * (1. - lambda) * ((2 + v[j]) * FIo[j] - FIo[j + 1] - FIo[j - 1]);
        CC = FIo[j] + D * lambda * (FR[j + 1] + FR[j - 1]) - D * (1. - lambda) * ((2 + v[j]) * FRo[j] - FRo[j + 1] - FRo[j - 1]);
        FR[j] = (BB + AA * CC) / (1 + AA * AA);
        FI[j] = (CC - AA * BB) / (1 + AA * AA);
        Pold = P[j];
        P[j] = FR[j] * FR[j] + FI[j] * FI[j];
        if (fabs(P[j] - Pold) > err)
          err = fabs(P[j] - Pold);
        Norma = Norma + P[j];
      }
    } while (err > 1.e-6);
    Norma = sqrt(Norma * dx);
    xmed = 0.;
    x2med = 0.;
    jmax = Nx1;
    Pmax = 0.;
    // Normaliza la FdO, (necesario para E explicito e implicito)
    // Y calcula |FdO|^2max, <x>, <x^2> y <k>
    for (j = Nx1; j <= Nx2; j++)
    {
      x = j * dx;
      FR[j] = FR[j] / Norma;
      FI[j] = FI[j] / Norma;
      P[j] = FR[j] * FR[j] + FI[j] * FI[j];
      xmed = xmed + P[j] * x;
      x2med = x2med + P[j] * x * x;
      if (P[j] > Pmax)
      {
        jmax = j;
        Pmax = P[j];
      }
    }
    xmed = xmed * dx;
    x2med = x2med * dx;
    Deltax = sqrt(x2med - xmed * xmed);
    kmed = 0.;
    for (j = Nx1 + 1; j < Nx2; j++)
      kmed = kmed + (FI[j + 1] - FI[j - 1]) * FR[j]; // k [A-1] = p/hbar medio= 2*Int(FI'*FR*dx)
    if (n % 100 == 0)
    {
      printf("\n%i %lf %lf %lf %lf %lf %lf", n, jmax * dx, Pmax, Norma, xmed, Deltax, kmed);
      fprintf(pfiltxt, "\n%i %lf %lf %lf %lf %lf %lf", n, jmax * dx, Pmax, Norma, xmed, Deltax, kmed);
      fflush(pfiltxt); //Para dibujar datos actualizados
    }
  }
  close(pfiltxt);
}
