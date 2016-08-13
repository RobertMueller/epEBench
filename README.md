This benchmark was designed for my master thesis to evaluate the energy consumption by using diverse workload on multi-core systems.

Compile epEBench with "make -f makefile_arm" for armv7 systems or "make -f makefile_arm" for x86_64 ISA

Initiate a simple testrun with ./epebench -m vidplay -a 4 -n 4 -u 30 -t 30

usage: epebench [-h] [-v] [-tTIME] [-iINSTR] [-mMODEL] [-nTHREADS] 
[-aCPU] [-uUSAGE] [-d] [-p] 
Options:
	-t T1 {T2...Tn}	Time in seconds
		Opt: Enter a list used as test pattern [-p]
	-m MD1 {MD2...Mdn}	Model(s) to execute (see ebmodels.ini)
	-n N {N1...Nn}	Number of threads to allocate
		Opt: Enter a list used as test pattern [-p]
	-u U {U1...Un}	CPU-load setting (0 – 100)[%]
		Opt1: Enter a list used as test pattern[-p]
		Opt2: List of CPU-loads per thread (normal)
	-a CPU {CPU1...CPUn}	CPU affinity; amount or list of used cores
	-i N	Maximum No. of instructions in Mio
	-d	CPU-usage process disable. Secondary load		controller will not be working
	-p	Test pattern mode
	-r	Enable thread pinning mode
	-h	show this help message and exit
	-v 	show epEBench version

Note-1: List of elements in test pattern mode loops cyclically until last element of longest list is reached. Means, amount of testruns is defined by longest parameter list.
Note-2: If no option is passed, benchmark starts with default values.
Note-3: Thread pinning mode [-r] forces threads bound to assigned CPU but leads to a lower load level accuracy at high thread counts.
