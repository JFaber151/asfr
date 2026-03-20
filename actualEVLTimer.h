#ifndef actualEVLTimer
#define actualEVLTimer
//private:
    void write_to_csv(int nos, double samples);
    void timespec_add_ns(struct timespec *__restrict r, const struct timespec *__restrict t, long ns);
    int set_attributes();
    int attach_thread();
    int setup_timer();
    int fib(int n);
    void mainloop(struct timespec ts_new, double *samples, int number_of_samples, int tmfd);
    void post_processing(double *samples, int number_of_samples);
    void *evl_jitter(void *arg);
//public:
    void find_evl_jitter();
#endif