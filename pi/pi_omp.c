#include <stdio.h>
#include <math.h>
#include "omp.h"
int main(int argc, char *argv[]) {
  /* Derived from:
     https://en.wikipedia.org/wiki/Pi#Geometry_and_trigonometry
     https://en.wikipedia.org/wiki/Unit_circle
     https://en.wikipedia.org/wiki/Trapezoidal_rule */

  long int N = 2000000000, i_min = 0, i_max = N;
  double x_min = 0.0, x_max = 1.0; /* x_max = r, and this is a unit circle */
  double area = 0.0;
  double x_left = 0.0, x_right = 0.0, y_left = 0.0, y_right = 0.0, time = 0.0;

#pragma omp parallel private(x_left, x_right, y_left, y_right) reduction(+:area)
  {
    double start, end;
    start = omp_get_wtime();
    #pragma omp for
    for (long int i = i_min; i<i_max; i=i+1) {
      x_left = x_min+i*(x_max-x_min)*(1.0/N);
      x_right = x_left+(x_max-x_min)*(1.0/N);
      y_left = sqrt(1.0-x_left*x_left);
      y_right = sqrt(1.0-x_right*x_right);
      area = area + 0.5*(y_left+y_right)*(x_right-x_left);
    }
    end = omp_get_wtime();
    time = end-start;
    printf("thread %d of %d, time=%f, area=%f\n", omp_get_thread_num(), omp_get_num_threads(), time, area);
  }
  printf("Pi ~= 4*sum(areas) = %f\n", 4*area);
}
