#include <string.h>
#include <thread>
#include <sched.h>
#include <iostream>

int main(int argc, char *argv[])
{
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(1, &cpuset);
	pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);// pin to core 1
	

    unsigned int cpu, node, res;
    res = getcpu(&cpu, &node);
    printf("running at cpu: %d & node: %d\n", cpu, node);
	return 0;
}