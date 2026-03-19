	#include <evl/thread.h>
	#include <evl/timer.h>
	#include <evl/clock.h>
	#include <evl/proxy.h>
	#include <evl/evl.h>
    #include <string.h>
	
#include <thread>
#include <sched.h>


void *thread_func(void *arg)
{
    int tfd = evl_attach_self("periodic-timer:%d");
    bool is_inband = evl_is_inband();
    evl_printf("is in band? %d\n", is_inband);
    evl_detach_self();
    return NULL;
}

void thread_test()
{
	pthread_t pthread;
	cpu_set_t cpuset;
	pthread_attr_t empty;
	pthread_attr_init(&empty);
	CPU_ZERO(&cpuset);
	CPU_SET(1, &cpuset);
	pthread_create(&pthread, &empty, thread_func, NULL);
	// pthread_setaffinity_np(pthread, sizeof(cpu_set_t), &cpuset);// pin to core 1
	pthread_join(pthread, NULL);
}

void main_test(){
	//initialize vars
	struct sched_param param;
	cpu_set_t cpuset;
	int ret, tfd;

	//set schedule
	param.sched_priority = 8;
	ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &param);

	//set core
	CPU_ZERO(&cpuset);
	CPU_SET(1, &cpuset);
	pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);// pin to core 1
	
	//attach thread
	tfd = evl_attach_self("app-main-thread:%d");
    bool is_inband = evl_is_inband();
    evl_printf("is in band? %d\n", is_inband);

	//running on what core?
	int core = getcpu();
	evl_printf("we are on core: %d\n", core);
}

int main(int argc, char *argv[])
{
	main_test();
	thread_test();
	return 0;
}