/*
 * globals.cuh
 *
 *  Created on: Jan 13, 2016
 *      Author: ffiorett
 */

#ifndef GLOBALS_CUH_
#define GLOBALS_CUH_

#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include <cuda_runtime.h>

/**
 * This macro checks return value of the CUDA runtime call and exits
 * the application if the call failed.
 *
 * See cuda.h for error code descriptions.
 */
#define cuCheck(ans) { gpuAssert((ans), __FILE__, __LINE__); }

inline void gpuAssert(cudaError_t code, char *file, int line,
		      bool abort = true) {
  if (code != cudaSuccess) {
    fprintf(stderr, "GPUassert: %s %s %d\n", cudaGetErrorString(code), file,
	    line);
    if (abort)
      exit(code);
  }
}

#endif /* GLOBALS_CUH_ */
