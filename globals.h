/*-------------------------------------------------------------------
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

#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))

extern volatile int done0;
extern volatile int threads;
extern volatile double cpu_usage_avg, cpu_mean;
extern volatile float k_cpu;
extern volatile float tot_usage;
extern volatile double facUsage;
extern volatile double instcnt, max_instcnt;
extern volatile int imemBuf[];
extern volatile double dmemBuf[];
extern volatile float fmemBuf[];
extern bool enaCpuCntr;

struct globargs {
    struct args* globalthr_args;
};

extern struct globargs* thread_args;

#define CNT_DIV 0.000020
#define MEM_SIZE 1000000
#define LOG_FILE "epebench_loadlog.txt"

#endif // GLOBAL_H_INCLUDED
