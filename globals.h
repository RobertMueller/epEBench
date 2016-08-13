#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

extern volatile int done0;
extern volatile int threads;
extern volatile double cpu_usage_avg, cpu_mean;
extern volatile float k_cpu;
extern volatile float tot_usage;
extern volatile double instcnt, max_instcnt;
extern volatile int imemBuf[];
extern volatile double dmemBuf[];
extern volatile float fmemBuf[];
//extern volatile int usedCpu, numCpu, thrcnt;

struct globargs {
    struct args* globalthr_args;
};

extern struct globargs* thread_args;

#define CNT_DIV 0.000020
#define MEM_SIZE 1000000
#define LOG_FILE "epebench_loadlog.txt"

#endif // GLOBAL_H_INCLUDED
