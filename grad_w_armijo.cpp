/*----------------------------------------------------------------
* File:     grad_w_armijo.cpp
*----------------------------------------------------------------
*
* Author:   Marek Rychlik (rychlik@arizona.edu)
* Date:     Wed Jan 22 16:39:46 2025
* Copying:  (C) Marek Rychlik, 2020. All rights reserved.
*
*----------------------------------------------------------------*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "energy.h"

double epsilon = 1;
double sigma = 1;
double b = 1;
double k_b = 100;

double dot(double *x, double *y, int n)
{
  double dot_val = 0;
  for(int i=0; i < n; ++i){
    dot_val += x[i] * y[i];
  }
  return dot_val;
}

double norm(double *x, int n)
{
  return sqrt(dot(x,x,n));
}


void gradient_method_test(int n_beads, int dimension, int maxit, double tol)
{
  double *grad;
  double *x = initialize_protein(n_beads, dimension, &grad);
  double alpha = 1;		// Initial step
  double rho = .9;		// Attenuation of alpha
  int n = n_beads * dimension;
  double energy;
  double *p = (double *)malloc(n * sizeof(double));
  double *x_new = (double *)malloc(n * sizeof(double));  
  double *grad_new = (double *)malloc(n * sizeof(double));  
  double c = 1e-4;

  // Initialize energy
  energy = total_energy(x,
			grad,
			n_beads,
			dimension,
			epsilon,
			sigma,
			b,
			k_b);
  int it = 0;
  printf("%9s\t%17s\t%12s\t%6s\n", "Iter","Energy","NormGrad","Alpha");
  for(; it < maxit; ++it) {
    double norm_grad = norm(grad, n);
    if (norm_grad < tol) {
      printf("Tolerance reached in iteration %d\n", it);
      break;
    }

    if(it % 1000==0) {
      printf("%9d\t%17.14g\t%12.6g\t%6g\n", it, energy, norm_grad, alpha);
      // Reset alpha
      alpha = 1;
    }

    do {
      // Seaarch Direction
      for(int i = 0; i < n; ++i) {
	p[i] = -grad[i];
      }    

      // New point
      for(int i = 0; i < n; ++i) {
	x_new[i] = x[i] + alpha * p[i];
      }

      double energy_new = total_energy(x_new,
				       grad_new,
				       n_beads,
				       dimension,
				       epsilon,
				       sigma,
				       b,
				       k_b);

      // Armijo condition
      if(energy_new <= energy + c * alpha * dot(grad, p, n)) {
	// Update energy and gradient
	energy = energy_new;
	for(int i = 0; i < n; ++i) {
	  x[i] = x_new[i];
	  grad[i] = grad_new[i];
	}    
	break;
      }
      alpha = rho * alpha;
    } while(1);
    
  }
  if(it == maxit)
    printf("Failed to converge in %d iterations.\n", maxit);
  free(p);
  free(x_new);
  free(grad_new);
  free(x);
  free(grad);
}



int main(int argc, char **argv)
{
  int n_beads = 10;
  int dimension = 3;
  int maxit = 100;
  double tol = 1e-8;

  if(argc>=2){
    n_beads = atoi(argv[1]);
  }
  if(argc >= 3) {
    dimension = atoi(argv[2]);
  }
  if(argc >= 4) {
    maxit = atoi(argv[3]);
  }
  if(argc >= 5) {
    tol = atof(argv[4]);
  }
  printf("Num. beads: %d, Dim.: %d, Max. iterations: %d, Tolerance: %g\n",
	 n_beads, dimension, maxit, tol);

  //time_test(n_beads, dimension);
  //gradient_test(n_beads, dimension);
  gradient_method_test(n_beads, dimension, maxit, tol);

  exit(EXIT_SUCCESS);
}
