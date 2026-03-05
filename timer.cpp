#include <iostream>
#include <thread>
#include <time.h>
#include <chrono>
#include <err.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
using namespace std;

#define SIG SIGRTMIN

static volatile sig_atomic_t counter = 0;

void timer_handler(int sig)
{
  //This function can be empty and everything works, but when I try and not assign this handler, it breaks?
    //counter++;   //actual code needs to be async safe, so no printing for instance
}

int timerFunc() {
  struct itimerspec its;
  timer_t timer;
  struct sigevent sigev;
  struct sigaction sa;
  
  //This sets up the timing
  int start_delay = 1000, interval = 1000;  //these are in nanoseconds
  its.it_value.tv_sec = start_delay / 1000000;
  its.it_value.tv_nsec = (start_delay % 1000000) * 1000;
  its.it_interval.tv_sec = interval / 1000000;
  its.it_interval.tv_nsec = (interval % 1000000) * 1000;

  //This sets the handler. I.E. What function is called when a certain signal is received
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = timer_handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGALRM, &sa, NULL);

  //This sets what signal is sent when the timer goes off
  memset(&sigev, 0, sizeof(struct sigevent));
  sigev.sigev_notify = SIGEV_SIGNAL;
  sigev.sigev_signo = SIGALRM;

  //We create and arm the timer, just returning a value based on whether arming was succesful
  timer_create(CLOCK_MONOTONIC, &sigev, &timer);
  return timer_settime(timer, 0, &its, NULL);
}

void make_timer()
{
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGALRM);
  pthread_sigmask(SIG_UNBLOCK, &set, NULL);

  timerFunc();


  //This is the main loop and it's setup
  int number_of_samples = 1000;
  double samples[number_of_samples];
  struct timespec ts_new, ts_old;
  clock_gettime(CLOCK_MONOTONIC, &ts_new);
  double newTime = (double)ts_new.tv_sec + (double)ts_new.tv_nsec / 1e9;
  double oldTime;
  while (counter < number_of_samples){
    oldTime = newTime;
    sleep(-1); // Sleep is automatically woken up by a signal, so we just wait for the signal from the timer
    clock_gettime(CLOCK_MONOTONIC, &ts_new);
    newTime = (double)ts_new.tv_sec + (double)ts_new.tv_nsec / 1e9;
    double dif = newTime - oldTime;
    samples[counter] = dif;
    counter++;
    }

  //Do some post processing on the data we got
  double sum = 0, min_val = samples[0], max_val = samples[0];
  for (int i = 0; i < number_of_samples; ++i) {
      sum += samples[i];
      if(samples[i] < min_val){
        min_val = samples[i];
      }
      if(samples[i] > max_val){
        max_val = samples[i];
      }
  }
  double mean = sum / number_of_samples;
  printf("mean is %lf\n", mean);

  double sd = 0;
  for (int i = 0; i < number_of_samples; ++i) {
        sd += pow(samples[i] - mean, 2);
  }
  printf("standard deviation is %lf\n", sqrt(sd / number_of_samples));
  printf("min val is %lf. max val is %lf\n", min_val, max_val);
}

void *thread_func(void *arg){
  make_timer();
  return NULL;
}

void find_jitter()
{
  // This blocks the alarm signal in main thread
  sigset_t set;
  sigemptyset(&set);
  sigaddset(&set, SIGALRM);
  pthread_sigmask(SIG_BLOCK, &set, NULL);

  //We set up the thread that will run the jitter test
  pthread_t pthread;
  pthread_create(&pthread, NULL, thread_func, NULL);
  pthread_join(pthread,NULL);
}

int main(){
  find_jitter();
  return 0;
}

