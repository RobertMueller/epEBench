/*-------------------------------------------------------------------
 * PROJECT      epEBench - Enhanced Parametrizable Energy Benchmark
 * FILE NAME    ebmodels.cpp
 * INSTITUTION  Fernuni Hagen
 * REVISION     1.0
 * PREFIX
 * DESCRIPTION  benchmark load generator
 * CREATED      06-March-16, Robert Mueller
 * MODIFIED     n/a
 *-------------------------------------------------------------------
 * Full Description: n.a.
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
 *      * Neither the name of the Åbo Akademi University nor the
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

//#include <string.h>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <algorithm>
#include <string.h>
#include <linux/unistd.h>
//#include <pthread.h>
#include <errno.h>
//#include <iostream>
#include <sys/time.h>
//#include <time.h>
#include <iomanip>

#include "globals.h"
#include "ebmodels.h"
#include "ebloads.h"

#define FACTOR 300000
#define KP 0.000001 // 0.000001
#define KI 0.00002 // 0.00001
#define TLOOP 50000
#define H_TLOOP 51000
#define L_TLOOP 49000
#define STIME_MAX 5000000	// timeout
#define KP1 0.135		// 0.43@ n8 u30; 0.135 @n24 u30
#define KI1 0.045		// 0.140 n8 u30; 0.045 @n24 u30			 empirisch 0.03
#define KI1_CORR		//


#define _PERIOD_CTR
#define _PIN_CORE
//#define _CPULOAD_CNTR

//globals
double volatile max_instcnt, instcnt;
float volatile k_cpu = 0;

struct args {
    int counter;
    float usage;
    char modelName[30];
    int thrNo;
    int cpu;
    bool realloc;
};

#ifdef _DEBUG
#ifdef _RT_TASK

       static void
       display_sched_attr(int policy, struct sched_param *param)
       {
           printf("    policy=%s, priority=%d\n",
                   (policy == SCHED_FIFO)  ? "SCHED_FIFO" :
                   (policy == SCHED_RR)    ? "SCHED_RR" :
                   (policy == SCHED_OTHER) ? "SCHED_OTHER" :
                   "???",
                   param->sched_priority);
       }

       static void
       display_thread_sched_attr(char *msg)
       {
           int policy, s;
           struct sched_param param;

           s = pthread_getschedparam(pthread_self(), &policy, &param);
           if (s != 0)
               handle_error_en(s, "pthread_getschedparam");

           printf("%s\n", msg);
           display_sched_attr(policy, &param);
       }

#endif // _RT_TASK
#endif // _DEBUG

using namespace std;

void* ebmodel(void* ptr) {

    args* param = (args*)ptr;
    float target_usage = param->usage;
    int thrNo = param->thrNo;
    bool realloc = param->realloc;
    int s;
    float corr = 0, esum = 0, e = 0;
    float esum1 = 0, e1 = 1;

    int m4x4smul_SIMD = 0,
        v1x4smul_SIMD = 0,
        dmul64_SIMD = 0,
        smul32_SIMD = 0,
        ssub32_SIMD = 0,
        dsub64_SIMD = 0,
        dmem64_SIMD = 0,
        smem32_SIMD = 0,
        vmov_SIMD = 0,
        vconvert_SIMD = 0,
        m4x4smul = 0,
        dmul = 0,
        dadd = 0,
        imul = 0,
        iadd = 0,
        icompare = 0,
        logic = 0,
        branch = 0,
        imem = 0,
        dmem = 0,
        imov = 0,
        shift = 0,
        bitbyte = 0,
        nop = 0;

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(param->cpu, &cpuset);

    ifstream inifile;
    string line;

#ifdef _DEBUG
    ofstream fileloopcontrol, filecpucontrol;
    stringstream outstream, outstrcpu;
    fileloopcontrol.open("looptime_pi_control.txt");
    filecpucontrol.open("cpu_pi_control.txt");
    outstream << setprecision(5);
    outstrcpu << setprecision(5);
    outstrcpu << "period\tk\ttarget_usage\tusage\n";

    printf("ebmodel <%s> thread created [%ld]\n", param->modelName, gettid());
#ifdef _RT_TASK
    display_thread_sched_attr("Scheduler attributes of new thread");
#endif // RT_TASK
#endif // _DEBUG

    // load model characteristic parameter from *.ini file
    inifile.open("ebmodels.ini");
    if (!inifile.is_open()) {
	errExit("File not found: ebmodels.ini\nBenchmark aborted!\n");
	done0 = 1;
	}
    else
    {
        string str, val;
        bool found = false;

        while (!found && getline(inifile, line))
        {
            if ( !inifile.good() ) break;
            line.erase(remove(line.begin(),line.end(),' '),line.end());
            stringstream iss(line);

            getline(iss, str, '=');
            if (str == "model") {
                    getline(iss, str, '\n');
                    if (str == (string)param->modelName) found = true;
            }
        }
        if (found) {
            while (getline(inifile, line))
            {
                if ( !inifile.good() ) break;
                line.erase(remove(line.begin(),line.end(),' '),line.end());
                stringstream iss(line);

                getline(iss, str, '=');
                getline(iss, val, '\0');

                if (str == "m4x4smul_SIMD") m4x4smul_SIMD        = (int)(atof(val.c_str())*FACTOR);
                else if (str == "v1x4smul_SIMD") v1x4smul_SIMD   = (int)(atof(val.c_str())*FACTOR);
                else if (str == "dmul64_SIMD") dmul64_SIMD       = (int)(atof(val.c_str())*FACTOR);
                else if (str == "smul32_SIMD") smul32_SIMD       = (int)(atof(val.c_str())*FACTOR);
                else if (str == "ssub32_SIMD") ssub32_SIMD       = (int)(atof(val.c_str())*FACTOR);
                else if (str == "dsub64_SIMD") dsub64_SIMD       = (int)(atof(val.c_str())*FACTOR);
                else if (str == "dmem64_SIMD") dmem64_SIMD       = (int)(atof(val.c_str())*FACTOR);
                else if (str == "smem32_SIMD") smem32_SIMD       = (int)(atof(val.c_str())*FACTOR);
                else if (str == "vmov_SIMD") vmov_SIMD           = (int)(atof(val.c_str())*FACTOR);
                else if (str == "vconvert_SIMD") vconvert_SIMD   = (int)(atof(val.c_str())*FACTOR);
                else if (str == "m4x4smul") m4x4smul             = (int)(atof(val.c_str())*FACTOR);
                else if (str == "dmul") dmul                     = (int)(atof(val.c_str())*FACTOR);
                else if (str == "dadd") dadd                     = (int)(atof(val.c_str())*FACTOR);
                else if (str == "iadd") iadd                     = (int)(atof(val.c_str())*FACTOR);
                else if (str == "imul") imul                     = (int)(atof(val.c_str())*FACTOR);
                else if (str == "icompare") icompare             = (int)(atof(val.c_str())*FACTOR);
                else if (str == "logic") logic                   = (int)(atof(val.c_str())*FACTOR);
                else if (str == "branch") branch                 = (int)(atof(val.c_str())*FACTOR);
                else if (str == "imem") imem                     = (int)(atof(val.c_str())*FACTOR);
                else if (str == "dmem") dmem                     = (int)(atof(val.c_str())*FACTOR);
                else if (str == "imov") imov                     = (int)(atof(val.c_str())*FACTOR);
                else if (str == "shift") shift                   = (int)(atof(val.c_str())*FACTOR);
                else if (str == "bitbyte") bitbyte               = (int)(atof(val.c_str())*FACTOR);
                else if (str == "nop") nop                       = (int)(atof(val.c_str())*FACTOR);

                else if (line == "end_model") break;
                else if (line == "model") break;

                else if (thrNo==0) fprintf(stderr,"\nComponent <%s> not found.\nModel characteristic is not as expected!", str.c_str());

            }

            struct timeval t0, t1;
            double stime = TLOOP, rtime = 0;

            // run load model
            while (!done0) {
                gettimeofday(&t0, NULL);    // start time

#ifdef _PERIOD_CTR  // loop runtime control
//                if ((stime+rtime) > H_TLOOP || (stime+rtime) < L_TLOOP) {
                    e = TLOOP-(stime+rtime);
                    esum += e;
                    corr = (KP*e + KI*esum) < -0.95 ? -0.95 : (KP*e + KI*esum); // calculate correction value to keep load runtime + sleep around 50ms
#ifdef _DEBUG
                    outstream << "corr:\t" << corr << "\ttime:\t" << stime+rtime << "\n";
#endif // DEBUG
//                }
#endif // _PI
                // call load algorithms
                if (m4x4smul_SIMD)   run_m4x4smul_SIMD(m4x4smul_SIMD*(1+corr));
                if (v1x4smul_SIMD)  run_v1x4smul_SIMD(v1x4smul_SIMD*(1+corr));
                if (dmul64_SIMD)    run_dmul64_SIMD(dmul64_SIMD*(1+corr));
                if (smul32_SIMD)    run_smul32_SIMD(smul32_SIMD*(1+corr));
                if (ssub32_SIMD)    run_ssub32_SIMD(ssub32_SIMD*(1+corr));
                if (dsub64_SIMD)    run_dsub64_SIMD(dsub64_SIMD*(1+corr));
                if (dmem64_SIMD)    run_dmem64_SIMD(dmem64_SIMD*(1+corr));
                if (smem32_SIMD)    run_smem32_SIMD(smem32_SIMD*(1+corr));
                if (vmov_SIMD)      run_vmov_SIMD(vmov_SIMD*(1+corr));
                if (vconvert_SIMD)  run_vconvert_SIMD(vconvert_SIMD*(1+corr));
                if (m4x4smul)       run_m4x4smul(m4x4smul*(1+corr));
                if (dmul)           run_dmul(dmul*(1+corr));
                if (dadd)           run_dadd(dadd*(1+corr));
                if (iadd)           run_iadd(iadd*(1+corr));
                if (imul)           run_imul(imul*(1+corr));
                if (icompare)       run_icompare(icompare*(1+corr));
                if (logic)          run_logic(logic*(1+corr));
                if (branch)         run_branch(branch*(1+corr));
                if (imem)           run_imem(imem*(1+corr));
                if (dmem)           run_dmem(dmem*(1+corr));
                if (imov)           run_imov(imov*(1+corr));
                if (shift)          run_shift(shift*(1+corr));
                if (bitbyte)	    run_bitbyte(bitbyte*(1+corr));
                if (nop)            run_nop(nop*(1+corr));

                if ((instcnt > max_instcnt) && max_instcnt) done0 = 1; // stop benchmarking if max instruction counts reached

#ifdef _CPULOAD_CNTR // CPU load control
                if (param->thrNo == 0 && cpu_usage_avg) {
                    e1 = tot_usage - cpu_usage_avg;
                    esum1 += e1;
                    k_cpu = (KP1*e1 + KI1*esum1);
                    if (target_usage*facUsage + k_cpu < 0.01) k_cpu = 0.01 - target_usage*facUsage; // limit to min 1% CPU load
#ifdef _DEBUG
                    outstrcpu << stime+rtime << "\t" << k_cpu << "\t" << tot_usage << "\t" << cpu_usage_avg << "\n";
#endif // DEBUG
                }
#endif // _CPULOAD_CNTR

#ifdef _PIN_CORE  // reallocate thread to CPU, if changed by OS
                if (realloc) {
                	s = sched_getcpu();                  // get CPU affinity of this thread
                	if (s != param->cpu) {
#ifdef _DEBUG
                    	printf("\nCPU thread affinity changed by OS. Reallocation from CPU %d to CPU %d done.\n", s, param->cpu);
#endif
                   	 CPU_ZERO(&cpuset);
                   	 CPU_SET(param->cpu, &cpuset);
                    	s = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);               // set CPU affinity of main thread
                	}
		}
#endif // _PIN_CORE

                gettimeofday(&t1, NULL);        // finish time
                rtime = ((t1.tv_sec - t0.tv_sec)*1000000 + t1.tv_usec-t0.tv_usec);
                stime = rtime*(1/(target_usage*facUsage + k_cpu) - 1);
                if (stime > STIME_MAX) stime = STIME_MAX;
                else if (stime < 0) stime = 0;
                usleep((int)(stime));
            }
            //sleep(1);
        }
        else {

		fprintf(stderr,"\nebmodel <%s> not found. Thread dies [%ld]\nBenchmark aborted.\n", param->modelName, gettid());
		exit(EXIT_FAILURE);
		done0 = 1;
        }
    }

#ifdef _DEBUG

    if (param->thrNo == 0) {
            filecpucontrol << outstrcpu.str();
	    fileloopcontrol << outstream.str();
    }
    printf("\nebmodel <%s> thread dies [%ld]", param->modelName, gettid());
#endif // _DEBUG

    return NULL;
}
