//=============================================================================
// Authors : Deborah Schrag & Joran Faber
// Group : asdfr-27 || ppd-3
// License : LGPL open source license
//
// Brief : This is the source file for finding the jitter of an EVL based timer
//
//=============================================================================

#include <evl/thread.h>
#include <evl/timer.h>
#include <evl/clock.h>
#include <evl/proxy.h>
#include <math.h>
#include <string.h>
#include <evl/evl.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fstream>
#include "actualEVLTimer.h"
using namespace std;


void write_to_csv(int nos, double samples)
{
    //append our data to the file
    ofstream myfile;
    myfile.open("jitter_data.csv", ios::app);
    myfile << nos << "," << samples << "\n";
    myfile.close();
}

void timespec_add_ns(struct timespec *__restrict r, const struct timespec *__restrict t, long ns)
{
    //we calculate and set the new time, based on supplied nanoseconds
    long s, rem;

    s = ns / 1000000000;
    rem = ns - s * 1000000000;
    r->tv_sec = t->tv_sec + s;
    r->tv_nsec = t->tv_nsec + rem;
    if (r->tv_nsec >= 1000000000) {
            r->tv_sec++;
            r->tv_nsec -= 1000000000;
    }
}

int set_attributes()
{
    //We set the preferred core & priority of the thread
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(1, &cpuset);  // pin to core 1
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    struct sched_param param;
    param.sched_priority = 8;
    return pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);
}

int attach_thread()
{
    //attach the evl thread
    int tfd = evl_attach_self("periodic-timer:%d");
    if (tfd < 0) { //If the call failed, then we're in band anyway, so use in band functions
        fprintf(stderr, "Failed to attach: %s (%d)\n", strerror(-tfd), -tfd);
    } 
    return tfd;
}

int setup_timer()
{
    //we create and arm an evl timer
    int tmfd, ret;
    struct itimerspec value, ovalue;
    struct timespec now;

    tmfd = evl_new_timer(EVL_CLOCK_MONOTONIC);
    ret = evl_read_clock(EVL_CLOCK_MONOTONIC, &now);
    timespec_add_ns(&value.it_value, &now, 1000000ULL);
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_nsec = 1000000;
    ret = evl_set_timer(tmfd, &value, &ovalue);
    return tmfd;
}

int fib(int n)
{
    //calculate the nth fibbonacci number
    //This is purposely done in a recursive manner, to create more calculations
	if (n == 0)
	{
		return 0;
	}
	if (n == 1)
	{
		return 1;
	}
	return (fib(n-1) + fib(n-2)); 
}

void mainloop(struct timespec ts_new, double *samples, int number_of_samples, int tmfd)
{
    //here, we run our main timer loop
    __u64 ticks;
    int ret;
    double newTime, oldTime;

    evl_read_clock(EVL_CLOCK_MONOTONIC, &ts_new);
    newTime = (double)ts_new.tv_sec + (double)ts_new.tv_nsec / 1e9;
    for (int counter = 0; counter < number_of_samples; counter++) {
        fib(21); //on the relbot, this seems to be the highest fib number it can recursively calculate within 0.001s
        ret = oob_read(tmfd, &ticks, sizeof(ticks));
        oldTime = newTime;
        evl_read_clock(EVL_CLOCK_MONOTONIC, &ts_new);
        newTime = (double)ts_new.tv_sec + (double)ts_new.tv_nsec / 1e9;
        double dif = newTime - oldTime;
        samples[counter] = dif;
    }
}

void post_processing(double *samples, int number_of_samples)
{
    //we perform some post processing on the collected data
    double sum = 0, min_val = samples[0], max_val = samples[0];
    for (int i = 0; i < number_of_samples; ++i) {
        sum += samples[i];
        if(samples[i] < min_val){
            min_val = samples[i];
        }
        if(samples[i] > max_val){
            max_val = samples[i];
        }
        write_to_csv(i, samples[i]);
    }
    double mean = sum / number_of_samples;
    double sd = 0;
    for (int i = 0; i < number_of_samples; ++i) {
            sd += pow(samples[i] - mean, 2);
    }
    
    printf("mean is %lf\n", mean);
    printf("standard deviation is %lf\n", sqrt(sd / number_of_samples));
    printf("min val is %lf & max val is %lf\n", min_val, max_val);
}

void *evl_jitter(void *arg)
{
    //The main function running on the thread
    int tmfd, number_of_samples = 1000;
    struct timespec ts_new;
    double samples[number_of_samples];

    set_attributes();
    attach_thread();
    tmfd = setup_timer();
    mainloop(ts_new, samples, number_of_samples, tmfd);
    evl_detach_self();  
    post_processing(samples, number_of_samples);
    return NULL;
}

void find_evl_jitter()
{
    //We want to find our evl jitter, report some basic results in the terminal and append the details to a csv file
    pthread_t pthread;
    pthread_create(&pthread, NULL, evl_jitter, NULL);
    pthread_join(pthread,NULL);
}

int main(int argc, char *argv[])
{
    find_evl_jitter();
	return 0;
}