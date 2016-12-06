#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <chrono>

template<typename TimeT = std::chrono::milliseconds>
struct measure
{
  template<typename F, typename ...Args>
    static typename TimeT::rep execution(F&& func, Args&&... args)
  {
    auto start = std::chrono::system_clock::now();
    std::forward<decltype(func)>(func)(std::forward<Args>(args)...);
    auto duration = std::chrono::duration_cast< TimeT>
    (std::chrono::system_clock::now() - start);
    return duration.count();
  }
};


template<typename TimeT = std::chrono::milliseconds>
struct Timer
{
Timer() : started(false), elapsed(0), lastElapsed(0) {}

  void start () {
    if (!started) {
      started = true;
      startTime = std::chrono::system_clock::now();
      lastElapsed = 0;
    }
  }

  typename TimeT::rep stopWatch() {
    if (!started) return 0;

    stopTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast< TimeT>
    (std::chrono::system_clock::now() - startTime);
    lastElapsed = duration.count();
    elapsed += duration.count();
    startTime = stopTime;
    return elapsed;
  }

  void pause() {
    stopTime = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast< TimeT>
    (std::chrono::system_clock::now() - startTime);
    lastElapsed = duration.count();
    elapsed += duration.count();
    started = false;
  }

  typename TimeT::rep getElapsed() {
    return elapsed;
  }

  typename TimeT::rep getLastElapsed() {
    return lastElapsed;
  }

  bool isStarted() {
    return started;
  }

  bool started;
  std::chrono::time_point<std::chrono::system_clock> startTime;
  std::chrono::time_point<std::chrono::system_clock> stopTime;
  typename TimeT::rep elapsed;
  typename TimeT::rep lastElapsed;

};

#endif
