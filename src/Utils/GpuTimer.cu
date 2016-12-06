#include "Utils/GpuTimer.cuh"
#include "Assert.cuh"


float Gpu::Timer::timeMs;
cudaEvent_t Gpu::Timer::startEvent;
cudaEvent_t Gpu::Timer::stopEvent;


void Gpu::Timer::initialize() {
  cuCheck(cudaEventCreate(&startEvent));
  cuCheck(cudaEventCreate(&stopEvent));
  timeMs = 0;
}

void Gpu::Timer::reset() {
  timeMs = 0;
}
    
float Gpu::Timer::getTimeMs() {
  return timeMs;
}
    
void Gpu::Timer::start(cudaEvent_t& event, cudaStream_t stream) {
  cuCheck(cudaEventRecord(event, stream));
}

float Gpu::Timer::stop(cudaEvent_t& s_event, cudaEvent_t& e_event, cudaStream_t stream) {
  float ms;
  cuCheck(cudaEventRecord(e_event, stream));
  cuCheck(cudaEventSynchronize(e_event));
  cuCheck(cudaEventElapsedTime(&ms, s_event, e_event));
  timeMs += ms;
  return ms;
}
