/*-------------------------------------------------------------------
 * PROJECT      epEBench - Enhanced Parametrizable Energy Benchmark
 * FILE NAME    cpu_status.cpp
 * INSTITUTION  Fernuni Hagen
 * REVISION     1.0
 * PREFIX
 * DESCRIPTION  reads out various CPU stats, as CPU load ...
 * CREATED      06-March-16, Robert Mueller
 * MODIFIED     n/a
 *-------------------------------------------------------------------
 * Full Description: n.a.
 *
 *-------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
//#include <sstream>
#include <fstream>
#include <iomanip>
//#include <string.h>
//#include <time.h>
//#include <linux/unistd.h>
//#include <linux/kernel.h>
//#include <linux/types.h>
//#include <sys/syscall.h>
//#include <pthread.h>
//#include <errno.h>

#include "globals.h"
#include "cpu_status.h"

#define NCPU 4
#define DELAY 8
#define DISPLCNT0 3.0

struct cpustats {
    unsigned long long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;
    float cpu_usage_max, cpu_usage_min, cpu_usage_mean;
    bool first = false;
};

struct store_args {
    char modelName[30];
    int thrCnt;
    int numCpu;
    };

static cpustats stat0;
volatile double cpu_usage_avg, cpu_mean;
volatile float tot_usage;


void init(){
    FILE* file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &stat0.lastTotalUser, &stat0.lastTotalUserLow,
    &stat0.lastTotalSys, &stat0.lastTotalIdle);
    fclose(file);
}

void wait ( uint ms )
{
	usleep(ms*1000);
}

double getCurrentValue(){
    double percent;
    FILE* file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;

    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %llu %llu %llu %llu", &totalUser, &totalUserLow, &totalSys, &totalIdle);
    fclose(file);

    if (totalUser < stat0.lastTotalUser || totalUserLow < stat0.lastTotalUserLow ||
        totalSys < stat0.lastTotalSys || totalIdle < stat0.lastTotalIdle){
        //Overflow detection. Just skip this value
        percent = -1.0;
    }
    else{
        total = (totalUser - stat0.lastTotalUser) + (totalUserLow - stat0.lastTotalUserLow) + (totalSys - stat0.lastTotalSys);
        percent = total;
        total += (totalIdle - stat0.lastTotalIdle);
        percent /= total;
//        percent *= 100;
    }

    stat0.lastTotalUser = totalUser;
    stat0.lastTotalUserLow = totalUserLow;
    stat0.lastTotalSys = totalSys;
    stat0.lastTotalIdle = totalIdle;

    return percent;
}

using namespace std;

void *cpu_usage(void *dat_ptr){

    double sum = 0.0, sum_tmp = 0.0;
    int anz=1, cnt = DISPLCNT0;
    float time = 0;
    store_args* wrparam = (store_args*)dat_ptr;
    ofstream logfile;

    logfile.open(LOG_FILE, ios_base::app);
    logfile << "# Testrun settings:\n"
            << "#   Model   : " << wrparam->modelName << "\n"
            << "#   Cpu#    : " << wrparam->numCpu << "\n"
            << "#   Thread# : " << wrparam->thrCnt << "\n"
            << "time\tusage\tavg_usage\n" << setprecision(4);

    init();
    wait(200);

    while (!done0) {
        do {
            cpu_usage_avg = getCurrentValue();
            wait(200);
        } while (cpu_usage_avg ==-1.0);
        if (anz == DELAY) stat0.cpu_usage_max = stat0.cpu_usage_min = stat0.cpu_usage_mean = cpu_usage_avg;
        else if (cpu_usage_avg > stat0.cpu_usage_max && anz > DELAY) stat0.cpu_usage_max = cpu_usage_avg;
        else if (cpu_usage_avg < stat0.cpu_usage_min) stat0.cpu_usage_min = cpu_usage_avg;
        sum += cpu_usage_avg;
        sum_tmp += cpu_usage_avg;
        cpu_mean = (sum / anz++);

        if (!cnt--) {
            cnt = DISPLCNT0;
            printf("\n\033[1Acpu usage [%5.2f% %/ %5.2f%]  cpu usage max [%5.2f%]  cpu usage min [%5.2f%]", 100*sum_tmp/(cnt+1), cpu_mean*100, stat0.cpu_usage_max*100, stat0.cpu_usage_min*100);
            logfile << time << "\t" << cpu_usage_avg << "\t" << cpu_mean << "\n";
            time += 0.2 * DISPLCNT0;
            sum_tmp = 0.0;

        }
    }
    logfile << "\n";

    return NULL;
}
