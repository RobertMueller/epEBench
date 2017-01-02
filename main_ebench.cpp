/*-------------------------------------------------------------------
 * PROJECT      epEBench - Enhanced Parametrizable Energy Benchmark
 * FILE NAME    main_ebench.cpp
 * INSTITUTION  Fernuni Hagen
 * REVISION     1.0.5
 * PREFIX
 * DESCRIPTION  Freely configurable Energy Benchmark for multicore ARM based embedded systems.
 *
 * CREATED      13-Aug-16, Robert Mueller
 * MODIFIED     17-Dec-16, Robert Mueller
 *
 * HISTORY      1.0.0 : created
 *              1.0.1
 *              1.0.2
 *              1.0.3
 *              1.0.4 : minor fixes
 *              1.1.0 : runtime entry now with floating point
 *			loop mode added: runs load setpoints in a loop
 *-------------------------------------------------------------------
 * Full Description: EpEBench is capable of creating a realistic workload on a multi-core system,
 *              and can be parametrized in order to stress only a certain aspect of the workload
 *              on the CPU
 *
 *-------------------------------------------------------------------
 * LICENSE      BSD-3
 *
 * Copyright (c) 2016, Robert Müller. All rights reserved.
 * Copyright (c) 2016, Fernuniversität in Hagen. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *      * Neither the name of the Fernuniverität Hagen nor the
 *        names of its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * -------------------------------------------------------------------*/

#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sstream>
#include <fstream>
#include <time.h>
//#include <linux/unistd.h>
//#include <linux/kernel.h>
//#include <linux/types.h>
//#include <sys/syscall.h>
#include <pthread.h>
#include <errno.h>
//#include <iostream>
//#include <ctype.h>
#include <sys/time.h>
//#include <time.h>

#include "ebmodels.h"
#include "ebloads.h"
#include "ebench.h"
#include "cpu_status.h"
#include "globals.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 1
#define VERSION_REV 0
#define VERSION_DATE __DATE__
#define VERSION_TIME __TIME__
#define STATUS "      "
#define MAX_THREADS 100
#define MAX_RUNS 50

#define RT_TASK

#define MAX(a,b) (((a)>(b))?(a):(b))


//static const unsigned long BUILD_NUMBER = __BUILD_NUMBER;

using namespace std;

int volatile done0;
double volatile facUsage;

typedef struct args {
    int counter;
    float usage;
    char modelName[30];
    int thrNo;
    int cpu;
    bool realloc;
    };

typedef struct store_args {
    char modelName[30];
    int thrCnt;
    int numCpu;
    };

void print_header(void) {

     printf("***************************************************\n");
     printf("* epEBench - Energy Benchmark                     *\n");
     printf("* Version %d.%d.%d%s                             *\n", VERSION_MAJOR, VERSION_MINOR, VERSION_REV, STATUS);
     printf("* build %s                               *\n", VERSION_DATE);
     printf("* Robert Mueller                                  *\n");
     printf("* 2016                                            *\n");
     printf("***************************************************\n");

}

void print_help(void) {

	//write(1,"\E[H\E[2J",7);
	printf("epEBench Version %d.%d.%d (%s %s)\n\n", VERSION_MAJOR, VERSION_MINOR, VERSION_REV, VERSION_DATE, VERSION_TIME);
	printf("Usage: epebench [-h] [-v] [-tTIME] [-iINSTR] [-mMODEL] [-nTHREADS] [-aCPU] [-uUSAGE] [-d] [-p]\n\n");
	printf("EpEBench is an energy benchmark capable of creating a realistic workload on a \n"
            "multi-core system, and can be parametrized in order to stress only a certain \n"
            "aspect of the workload on the CPU\n\n");
	printf("Options:\n");
	printf("\t-t T1 {T2...Tn}\t\tTime in seconds\n\t\t\t\tOpt: Enter a list used as test pattern (-p or -l)\n");
	printf("\t-m MD1 {MD2...MDn}\tModel(s) to execute (e.g. vidplay, gzip [Note-4])\n");
	printf("\t-n N {N1...Nn}\t\tNumber of threads to allocate\n\t\t\t\tOpt: Enter a list used as test pattern (use -p)\n");
	printf("\t-u U {U1...Un}\t\tCPU-load setting (0 - 100)[%].\n\t\t\t\tOpt1: Enter a list used as test pattern (-p or -l)\n");
	printf("\t\t\t\tOpt2: List of CPU-loads per thread (normal mode)\n");
	printf("\t-a CPU {CPU1...CPUn}\tCPU affinity; amount or list of used cores\n");
	printf("\t-i N\t\t\tMaximum No. of instructions [Mio]\n");
	printf("\t-l N\t\t\tLoop mode [Note-4]. No. of load profile loops\n");
	printf("\t-d \t\t\tCPU-usage process disable.\n\t\t\t\tSecondary load controller will not be working\n");
	printf("\t-p \t\t\tTest pattern mode [Note-1]\n");
	printf("\t-r \t\t\tEnable thread pinning mode\n");
	printf("\t-h \t\t\tShow this help message\n");
	printf("\t-v \t\t\tShow version\n\n");
	printf("Note-1: List of elements in test pattern/loop mode loops cyclically until last element of longest list is reached.\nMeans, amount of testruns is defined by longest parameter list.\n");
	printf("Note-2: If no option is passed, benchmark starts with default values.\n");
	printf("Note-3: Thread pinning mode [-r] forces threads bound to assigned CPU but leads to a lower load level accuracy at high thread counts.\n");
	printf("Note-4: Refer to ebmodels.ini file for more information and find all available workloads.\n");
	printf("\nGood luck!\n\n");
	exit(EXIT_SUCCESS);

}

int main (int argc, char **argv)
{
    float usage[MAX_THREADS] = {};
    int cpu[MAX_THREADS] = {1};
 	double runtime[MAX_RUNS] = {20};
 	int threads[MAX_THREADS] = {2};
    int c, m_cnt = 0, a_cnt=0, u_cnt=0, thrcnt=0, t_cnt=0, th_cnt = 0;
    int testrun = 0, max_runs = 0;
    int loops = 1, loop_cnt = 0, step = 0;
    bool loop_mode = false;
    bool show_usage = true, pattern = false, realloc = false;
    int numCpu = sysconf( _SC_NPROCESSORS_ONLN );
	int usedCpu = 0;
    int s, i, j;
    struct timeval t0, t1;
    char var_str[6] = "    ";

 	void* (*pt2loadfoo[MAX_THREADS])(void*) = {ebmodel, NULL};

    pthread_t thread[MAX_THREADS];
	pthread_t thread_cpu, thread_main;
	cpu_set_t cpuset;

	facUsage = 1;   // init factor for loop run

	remove(LOG_FILE);
	//write(1,"\E[H\E[2J",7);		// clear screen

#ifdef _RT_TASK
	sched_param schedParam;
	int policy, inheritsched;
	pthread_attr_t attr;
    pthread_attr_t *attrp;
#endif // _RT_TASK

	args* param[MAX_THREADS];
	for (int i=0; i < MAX_THREADS; i++) {
            param[i] = (struct args*)malloc(sizeof *param[i]);
            param[i]->thrNo = i;
            strcpy(param[i]->modelName,"");
	}

	store_args* wrparam;
	wrparam = (struct store_args*)malloc(sizeof *wrparam);

    // default values
	strcpy(param[0]->modelName,"vidplay");
	usage[0] = 0.3;
	cpu[0] = numCpu;
	threads[0] = numCpu;

	print_header();

    // ********** Parse program arguments *************************************
    opterr = 0;

    if (argc > 1) {
        while ((c = getopt (argc, argv, "m:a:u:t:n:i:l:pdhvr")) != EOF) {
            switch (c)
            {
                case 'm':       // model to run
                    optind--;
                    while (optind < argc && *argv[optind] != '-') strcpy(param[m_cnt++]->modelName,argv[optind++]);
                    break;
                case 'a':       // cpu affinity
                    optind--;
                    while (optind < argc && *argv[optind] != '-') {
                        if (atoi(argv[optind]) <= numCpu && atoi(argv[optind]) >= 0) cpu[a_cnt++] = atoi(argv[optind++]);
                        else {
                            cpu[a_cnt++] = numCpu;
                            printf("! CPU affinity to %d core(s) not allowed! Set to max. core(s): %d\n", atoi(argv[optind++]), numCpu);
                        }
                    }
                    break;
                case 'u':       // target cpu usage
                    optind--;
                    while (optind < argc && *argv[optind] != '-') {
                        if (abs(atoi(argv[optind])) <= 100) usage[u_cnt++] = abs(atoi(argv[optind++]))*0.01;
                        else {
                            usage[u_cnt++] = 1;
                            optind++;
                            printf("! CPU usage setting not feasible! CPU-load is set to 100%%.\n");
                        }
                    }
                    break;
                case 't':       // runtime
                    optind--;
                    while (optind < argc && *argv[optind] != '-') {
                        if (t_cnt > MAX_RUNS) break;

                        runtime[t_cnt++] = atof(argv[optind++]);

                    }
                    break;
                case 'i':       // max instruction counts in million
                    max_instcnt = (double)abs(atoi(optarg));
                    break;
                case 'd':       // disable CPU usage output
                    show_usage = false;
                    printf("=> CPU-usage process disabled. Secondary load controller is not working!\n");
                    break;
				case 'p':       // run test pattern
                    pattern = true;
                    break;
                case 'v':       // show version
                    printf("epEBench Version %d.%d.%d%s (%s %s)\n\n", VERSION_MAJOR, VERSION_MINOR, VERSION_REV, STATUS, VERSION_DATE, VERSION_TIME);
                    break;
               case 'r':       // enable thread reallocation mode
                    realloc = true;
		    printf("=> Thread pinning mode enabled.\n");
                    break;
                case 'n':       // number of threads
                    optind--;
                    while (optind < argc && *argv[optind] != '-') {
                        threads[th_cnt] = abs(atoi(argv[optind++]));
                        if (threads[th_cnt] > 99) {
                            threads[th_cnt] = 99;
                            printf("! Max allowed threads %d.\n", thrcnt);
                        }
                        th_cnt++;
                    }
                    break;
                case 'l':       // loops
                    loops = abs(atoi(optarg));
                    loops = (loops == 0) ? 1 : loops;
                    loop_mode = true;
                    break;
                case 'h':       // help - intentionally without break
                case '?':
                    if (optopt == 'i') fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                    else if (optopt == 't') fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                    else if (optopt == 'n') fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                    else if (optopt == 'u') fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                    else if (optopt == 'a') fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                    else if (optopt == 'm') fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                    else if (isprint (optopt)) fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                    else fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
					print_help();
                    return 1;
                default:		// should never come here
                    abort ();
            }
        }
    }
    else {
        // if no parameters passed, start a thread on all N cores with 30% load
        strcpy(param[0]->modelName,"vidplay");
		cpu[0] = numCpu;
		threads[0] = numCpu;
        usage[0] = 0.3 * threads[0]/numCpu;
        printf("> No parameters passed. Benchmark starts with default values.\n");
    }

    if (pattern || loop_mode) max_runs = MAX(u_cnt, MAX(t_cnt, MAX(th_cnt, a_cnt)));

    if (pattern && loop_mode) {
        printf("! Activation of both loop- and pattern-mode not permitted. Loop-mode will be active.\n");
        pattern = false;
        }

    if (pattern) { 
	printf("=> Test pattern run started.\n");
	}
    else if (loop_mode) {
		printf("=> Loop mode activated.\n");
		strcpy(var_str,"(avg)");
		}

        //************ start benchmark ***********************************************

	do {

        // default values
        if (!th_cnt) th_cnt = 1;
        if (!a_cnt) a_cnt = 1;
        if (!u_cnt) u_cnt = 1;
        if (!t_cnt) t_cnt = 1;
        if (!thrcnt) thrcnt = a_cnt;

        if (pattern) printf(">>> Testrun: %d/%d <<<\n",testrun+1,max_runs);
        thrcnt = threads[testrun%th_cnt];

        // assign model to all threads, if # of entered models < # of threads
        int t = thrcnt;
        if (m_cnt < thrcnt) {
            if (!m_cnt) m_cnt = 1;
            while (t--) strcpy(param[t]->modelName, param[t%m_cnt]->modelName);
        }

        // assign CPUs
        if (a_cnt == 1 || pattern == true) {
            int a = cpu[testrun%a_cnt];
            if (a > thrcnt) a = thrcnt;
            usedCpu = a;
            while (a--) param[a]->cpu = a;
        }
        else {
        int a = a_cnt;
        // get # of assigned CPUs, if affinity CPU list has been passed
            int mask=0, rol;
            while (a--) {
                for (int i=0; i < numCpu; i++) {
                    rol = 1;
                    rol <<= cpu[a];
                    mask |= rol;
                }
                param[a]->cpu = cpu[a];
            }
            for (int i=0; i < numCpu; i++) {
                usedCpu += mask & 1;
                mask >>= 1;
            }
        }
	
        // spread threads across all target CPUs, if # of entered CPUs < # of threads
        t = thrcnt;
        if (usedCpu < thrcnt) {
            while (t--) param[t]->cpu = param[t%usedCpu]->cpu > numCpu ? 0 : param[t%usedCpu]->cpu;
        }

        // generate thread numbers and assign realloc flag
        t = thrcnt;
        while (t--) {
                param[t]->thrNo = t;
                param[t]->realloc = realloc;
        }


        // evenly distribute target total CPU usage across all threads, if # of entered CPU usage == 1
		// OR a test pattern is entered (pattern = true)
        t = thrcnt;
        if (u_cnt == 1 || pattern || loop_mode) {
            // calculate total CPU load
            tot_usage = usedCpu/(float)numCpu;
            if (usage[testrun%u_cnt] <= tot_usage) tot_usage = usage[testrun%u_cnt];
            else printf("> !Too less allocated CPU cores to reach %2.0f%% CPU load!\n", usage[testrun%u_cnt]*100);
            while (t--) {
                float bal = 0;
                if (thrcnt%usedCpu && t/(usedCpu) == 0 && t > thrcnt%usedCpu-1)
                    bal = (thrcnt%usedCpu)/(float)usedCpu;
                else if (thrcnt%usedCpu && t/(usedCpu) == 1 && t%usedCpu <= (thrcnt%usedCpu)-1)
                    bal = -(1-(thrcnt%usedCpu)/(float)usedCpu);
                param[t]->usage = tot_usage * numCpu/(float)thrcnt * (1 + bal);
            }
        }
        else {
        // spread target CPU usage per thread across all thread, if # of entered CPU usage values < # of threads
            while (t--) param[t]->usage = usage[t%u_cnt];
            // calculate total CPU load
            for (int j = 0; j < numCpu; j++) {
                t = thrcnt;
                float k = 0;
                do {
                    if (param[t]->cpu == j) k += param[t]->usage;
                } while (t--);
                if (k > 1) k = 1;
                tot_usage += k / numCpu;
            }
        }

        wrparam->numCpu = numCpu;
        strcpy(wrparam->modelName, param[0]->modelName);
        wrparam->thrCnt = thrcnt;

        // ----------------start threads and set affinity -------------------------
        CPU_ZERO(&cpuset);

        thread_main = pthread_self();                                                       // get main thread ID
#ifdef _RT_TASK
        schedParam.sched_priority = 20;
        policy = SCHED_FIFO;

       /* Initialize thread attributes object according to options */
        attrp = NULL;

        s = pthread_attr_init(&attr);
        if (s != 0) handle_error_en(s, "pthread_attr_init");
        attrp = &attr;

        s = pthread_setschedparam(thread_main, policy, &schedParam);
        if (s != 0) handle_error_en(s, "pthread_setschedparam");
        s = pthread_getschedparam(thread_main, &policy, &schedParam);
        if (s != 0) handle_error_en(s, "pthread_getschedparam");

        s = pthread_attr_setinheritsched(&attr, PTHREAD_INHERIT_SCHED);
#endif

        if (s != 0) handle_error_en(s, "pthread_attr_setinheritsched");
        CPU_SET(param[0]->cpu, &cpuset);                                                           // set CPU
        s = pthread_setaffinity_np(thread_main, sizeof(cpu_set_t), &cpuset);               // set CPU affinity of main thread
        if (s != 0) handle_error_en(s, "pthread_setaffinity_np");
        s = pthread_getaffinity_np(thread_main, sizeof(cpu_set_t), &cpuset);                // get CPU affinity of main thread
        if (s != 0) handle_error_en(s, "pthread_getaffinity_np");

        if (show_usage) {
            pthread_create(&thread_cpu, NULL, cpu_usage, wrparam);                                 // create CPU usage thread
            CPU_SET(param[0]->cpu, &cpuset);                                                           // set CPU
            s = pthread_setaffinity_np(thread_cpu, sizeof(cpu_set_t), &cpuset);                // set CPU affinity of thread
            if (s != 0) handle_error_en(s, "pthread_setaffinity_np");
            s = pthread_getaffinity_np(thread_cpu, sizeof(cpu_set_t), &cpuset);                 // get CPU affinity of thread
            if (s != 0) handle_error_en(s, "pthread_getaffinity_np");
        }


        for (i=0; i < thrcnt; i++)
        {
            pthread_create(&thread[i], NULL, pt2loadfoo[0], param[i]);                     // create thread i

            CPU_SET(param[i]->cpu, &cpuset);                                               // set CPU
            s = pthread_setaffinity_np(thread[i], sizeof(cpu_set_t), &cpuset);             // set CPU affinity of thread i
            if (s != 0) handle_error_en(s, "pthread_setaffinity_np");
            s = pthread_getaffinity_np(thread[i], sizeof(cpu_set_t), &cpuset);             // get CPU affinity of thread i
            if (s != 0) handle_error_en(s, "pthread_getaffinity_np");
        }
		
		// recalculate average total usage if load profiles to run in loop mode
		double sum_tim = 0.0;
		if (loop_mode) {
			tot_usage = 0;
			for (i=0; i < max_runs; i++) {
				tot_usage += (runtime[i%t_cnt]*usage[i%u_cnt]);
				sum_tim += runtime[i%t_cnt];
			}
			tot_usage /= sum_tim;
			sum_tim *= loops;				
		}
		else sum_tim = runtime[testrun%t_cnt];				

        printf("\nInitial settings:\n");
        printf("    CPU load:   %2.0f%% %s\n", tot_usage * 100, var_str);
        printf("    cores#  :   %d\n", numCpu);
        printf("    threads#:   %d\n", thrcnt);
        if (m_cnt == 1) printf("    model   :   ");
        else printf("    models  :   ");
        int m = m_cnt;
        while (m--) printf("%s ",param[m]->modelName);
        printf("\n");
        if (!max_instcnt) printf("    runtime :   %2.1fs\n", sum_tim);
        else printf("    Inst count: %.0lf Mio\n", max_instcnt);

        float k=0;
        printf("\nReturned pthread affinity and core load:\n");
        printf("    used cores: %d\n", usedCpu);
            for (j = 0; j < CPU_SETSIZE; j++) {
                i = thrcnt;
                k = 0;
                if (CPU_ISSET(j, &cpuset)) {
					if (loop_mode) k = tot_usage * numCpu/usedCpu;                    
					else
					{					
						do {
                    	    if (param[i]->cpu == j) k += param[i]->usage;
                    	} while (i--);
					}
                    if (k > 1) k = 1;
                    printf("    CPU %d:      %2.1f%% %s\n", j, k*100, var_str);
                }
            }
		printf("\n");

        gettimeofday(&t0, NULL);        // start time measurement

        if (!max_instcnt) {
            if (loop_mode) {            // enter loop mode
                printf("Loop mode settings:\n");
                printf("    Setpoints:  ");
                for (step = 0; step < max_runs; step++) printf("[%2.0f%%/%2.2fs] ", usage[step%u_cnt]*100, runtime[step%t_cnt]);
              	printf("\n");
				
                for (loop_cnt; loop_cnt < loops; loop_cnt++) {
					printf("    loops#   :  %d/%d\n", loop_cnt+1, loops);

                    for (step = 0; step < max_runs; step++) {
                        tot_usage = usage[step%u_cnt];
                        facUsage = usage[step%u_cnt] / usage[0];
						printf("    steps#   :  %d/%d\n\n", step+1, max_runs);
                        usleep(runtime[step%t_cnt]*1E6);
						printf("\n\033[3A");
                    }
					printf("\n\033[2A");
                }
				printf("\033[3B");
            }
            else {
                usleep(runtime[testrun%t_cnt]*1E6);
                testrun++;
            }
            done0 = 1;
        }
        else testrun++;

        for (int i=0; i < thrcnt; i++) pthread_join(thread[i], NULL);
        if (show_usage) pthread_join(thread_cpu, NULL);

        gettimeofday(&t1, NULL);        // stop time measurement
        double ttim = ((t1.tv_sec - t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec)*0.001;
        printf("\nRun:\t%.2f ms\n",ttim);
        printf("Counts:\t%lf Mio\n", instcnt);
		printf("\n");

		instcnt = 0;
		done0 = 0;

	} while ((testrun < max_runs) && pattern);	// start a new testrun, if pattern run is active

    printf("Benchmarking finished.\n");
	return 0;
}
