#include <stdio.h>
#include <math.h>
#include "omp.h"
#include "mpi.h"
int main(int argc, char *argv[]) {
  /* Derived from:
     https://en.wikipedia.org/wiki/Pi#Geometry_and_trigonometry
     https://en.wikipedia.org/wiki/Unit_circle
     https://en.wikipedia.org/wiki/Trapezoidal_rule */

  /* Set up MPI parameters */
  MPI_Init(&argc, &argv);
  int n_processes = 0, my_rank = 0, namelen = 0;
  char node_name[MPI_MAX_PROCESSOR_NAME];
  MPI_Comm_size(MPI_COMM_WORLD, &n_processes);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Get_processor_name(node_name, &namelen); /* What node am I on? */

  long int N = 2000000000, i_min = 0, i_max = 0;
  double x_min = 0.0, x_max = 1.0; /* x_max = r, and this is a unit circle */
  double area = 0.0;
  double x_left = 0.0, x_right = 0.0, y_left = 0.0, y_right = 0.0;

  /* Since we've already seen "omp parallel for" divide a loop correctly
     among its threads, let's figure out the overall loop limits for each
     MPI process and let "omp parallel for" divide that up. */
  i_min = my_rank*(N/n_processes);
  if (my_rank<(n_processes-1)) {
    i_max = (my_rank+1)*(N/n_processes);
  }
  else {
    i_max = N;
  }
  printf("Rank %d on %s, %d <= i < %d\n", my_rank, node_name, i_min, i_max);

#pragma omp parallel private(x_left, x_right, y_left, y_right) reduction(+:area)
  {
    double start, end, time = 0.0;
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
    printf("rank %d, thread %d of %d, time=%f, area=%f\n", my_rank, omp_get_thread_num(), omp_get_num_threads(), time, area);
  }
  double areas[n_processes];
  MPI_Gather(&area, 1, MPI_DOUBLE, areas, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  /* Explanation: all ranks send 1 element of their "area" variable as
     double-precision to the double-precision array "areas" on rank 0.
     All non-zero ranks will ignore "areas", but the name has to exist
     for them, too. */

  /* Now rank 0 can sum up all the ranks' sub-areas and print it. */
  if (my_rank == 0) {
    area = 0.0;
    for (int i=0; i<n_processes; i=i+1) {
      area = area + areas[i];
    }
    printf("Pi ~= 4*sum(areas) = %f\n", 4*area);
  }
  MPI_Finalize();
}