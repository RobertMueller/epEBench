/*-------------------------------------------------------------------
 * PROJECT      epEBench - Enhanced Parametrizable Energy Benchmark
 * FILE NAME    globals.cpp
 * INSTITUTION  Fernuni Hagen
 * REVISION     1.0
 * PREFIX
 * DESCRIPTION  global definitions
 * CREATED      06-March-16, Robert Mueller
 * MODIFIED     n/a
 *-------------------------------------------------------------------
 * Full Description: n.a.
 *
 *-------------------------------------------------------------------
 */

#include "globals.h"

volatile int done0;
volatile int threads;
volatile double cpu_usage_avg, cpu_mean;
volatile float tot_usage, k_cpu;
volatile double instcnt, max_instcnt;
//volatile int usedCpu, numCpu, thrcnt;
volatile int imemBuf[MEM_SIZE];
volatile double dmemBuf[MEM_SIZE];
volatile float fmemBuf[MEM_SIZE];





