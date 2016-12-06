#ifndef CUDA_BE_GPUTIMER_CUH
#define CUDA_BE_GPUTIMER_CUH

/* Common dependencies */
#include <cuda.h>

namespace Gpu {

  struct Timer {
  public:
    static float timeMs; // in ms
    static cudaEvent_t startEvent, stopEvent;
    
    static void initialize();
    
    static void reset();
    
    static float getTimeMs();
    
    static void start(cudaEvent_t& event=startEvent, cudaStream_t stream=0);
    
    static float stop(cudaEvent_t& s_event=startEvent, cudaEvent_t& e_event=stopEvent, cudaStream_t stream=0);

  };
};

#endif
