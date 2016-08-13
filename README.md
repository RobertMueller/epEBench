This benchmark was designed during my master thesis to evaluate the energy consumption by using diverse workload on multi-core systems.\n
It is capable of:\n
    *Allocating a number of worker threads to execute certain workload models and operations
    *Providing realistic workloads
    *Workload models can be added and their properties be modified
    *Number of operations and operation types are selectable by the user
    *CPU workload can be set in percentage per thread or in total 
    *Executing load pattern with different workloads, thread counts and periods

Compile epEBench with "make -f makefile_arm" for armv7 systems or "make -f makefile_arm" for x86_64 ISA

Initiate a simple testrun with ./epebench -m vidplay -a 4 -n 4 -u 30 -t 30

usage: epebench [-h] [-v] [-tTIME] [-iINSTR] [-mMODEL] [-nTHREADS] [-aCPU] [-uUSAGE] [-d] [-p]\n 
Options:\n
	-t T1 {T2...Tn}	Time in seconds\n
		Opt: Enter a list used as test pattern [-p]\n
	-m MD1 {MD2...Mdn}	Model(s) to execute (see ebmodels.ini)\n
	-n N {N1...Nn}	Number of threads to allocate\n
		Opt: Enter a list used as test pattern [-p]\n
	-u U {U1...Un}	CPU-load setting (0 – 100)[%]\n
		Opt1: Enter a list used as test pattern[-p]\n
		Opt2: List of CPU-loads per thread (normal)\n
	-a CPU {CPU1...CPUn}	CPU affinity; amount or list of used cores\n
	-i N	Maximum No. of instructions in Mio\n
	-d	CPU-usage process disable. Secondary load controller will not be working\n
	-p	Test pattern mode\n
	-r	Enable thread pinning mode\n
	-h	show this help message and exit\n
	-v 	show epEBench version\n
\n
\n
Note-1: List of elements in test pattern mode loops cyclically until last element of longest list is reached.\n
Means, amount of testruns is defined by longest parameter list.\n
Note-2: If no option is passed, benchmark starts with default values.\n
Note-3: Thread pinning mode [-r] forces threads bound to assigned CPU but leads to a lower load level accuracy at high thread counts.\n
