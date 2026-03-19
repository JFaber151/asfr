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
    using namespace std;


        void write_to_csv(int nos, double samples){
        ofstream myfile;
        myfile.open("jitter_data.csv", ios::app);
        myfile << nos << "," << samples << "\n";
        myfile.close();
    }

	void timespec_add_ns(struct timespec *__restrict r,
	     		     const struct timespec *__restrict t,
			     long ns)
	{
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

	int main(int argc, char *argv[])
	{
		struct itimerspec value, ovalue;
		int tfd, tmfd, ret, n = 0, number_of_samples = 1000;
		struct timespec now, ts_new, ts_old;
		__u64 ticks;
        double samples[number_of_samples];

        // Set CPU affinity - I hope this is correct
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(1, &cpuset);  // pin to core 1

        pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

        
	    struct sched_param param;
        param.sched_priority = 8;
        ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

		/* Attach to the core. */
		tfd = evl_attach_self("periodic-timer:%d");

        //check what value is returned
        if (tfd < 0) { //If the call failed, then we're not out of band anyway
            fprintf(stderr, "Failed to attach: %s (%d)\n", strerror(-tfd), -tfd);
        } 

        //check if in band

        bool is_inband = evl_is_inband();
        evl_printf("is in band, before creating timer? %d\n", is_inband);
		/* Create a timer on the built-in monotonic clock. */
		tmfd = evl_new_timer(EVL_CLOCK_MONOTONIC);
        is_inband = evl_is_inband();
        evl_printf("is in band, after changing timer? %d\n", is_inband);
		/* Set up a 1 Hz periodic timer. */
		ret = evl_read_clock(EVL_CLOCK_MONOTONIC, &now);


        

        /* EVL always uses absolute timeouts, add 1s to the current date */
		timespec_add_ns(&value.it_value, &now, 1000000ULL);
		value.it_interval.tv_sec = 0;
		value.it_interval.tv_nsec = 1000000;
		ret = evl_set_timer(tmfd, &value, &ovalue);

        evl_read_clock(EVL_CLOCK_MONOTONIC, &ts_new);

        double newTime = (double)ts_new.tv_sec + (double)ts_new.tv_nsec / 1e9;
        double oldTime;


        is_inband = evl_is_inband();
        evl_printf("is in band, before loop start? %d\n", is_inband);
		for (int counter = 0; counter < number_of_samples; counter++) {
		    /* Wait for the next tick to be notified. */
		    ret = oob_read(tmfd, &ticks, sizeof(ticks));
		    /*if (ticks > 1) {
		       	    evl_printf(stderr, "timer overrun! %lld ticks late\n",
			      	      ticks - 1);
			      break;
		    }*/
		    //evl_printf("TICKED, loops=%d\n", n++);
            
            is_inband = evl_is_inband();
            if(is_inband){
                evl_printf("is in band, in loop %d\n", is_inband);
            }
            oldTime = newTime;
            evl_read_clock(EVL_CLOCK_MONOTONIC, &ts_new); //This needs to be replaced for an evl function, so I assume the structs change too
            newTime = (double)ts_new.tv_sec + (double)ts_new.tv_nsec / 1e9;
            //evl_printf("new time is: %lf\n", newTime);
            double dif = newTime - oldTime;
            samples[counter] = dif;
		}

        //check if in band
        is_inband = evl_is_inband();
        evl_printf("is in band? %d\n", is_inband);

		/* Disable the timer (not required if closing). */
		value.it_interval.tv_sec = 0;
		value.it_interval.tv_nsec = 0;
		ret = evl_set_timer(tmfd, &value, NULL);

        evl_detach_self();  

        //Post Processing
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
        printf("mean is %lf\n", mean);

        double sd = 0;
        for (int i = 0; i < number_of_samples; ++i) {
                sd += pow(samples[i] - mean, 2);
        }
        printf("standard deviation is %lf\n", sqrt(sd / number_of_samples));
        printf("min val is %lf. max val is %lf\n", min_val, max_val);
		return 0;
}