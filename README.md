epEBench - Energy Benchmark
=========================

This benchmark system was designed because of the need to generate diverse, customizable workload for evaluating the energy consumption on multi-core systems. 

Features
-------
It is capable of:

>- Allocating a number of worker threads to execute certain workload models and operations
>- Providing realistic workloads (**vidplay** -> multimedia app / **gzip** -> gzip app)
>- Workload models can be added and their properties be modified (edit **ebmodels.ini** file)
>- Number of operations and operation types are selectable by the user
>- CPU workload can be set in percentage per thread or in total 
>- Executing load pattern with different workloads, thread counts and periods

Two implemented workloads (vidplay and gzip) replicate behavior of real applications in resprect to instruction mix characteristic.

**vidplay**: Emulates multi-threaded video-player workload (QoS type)

**gzip**: Emulates single-threaded data de/compression workload (performance type)

In order to run certain operations as workload, refer to **ebmodels.ini** file.

Compilation
-------
Compile epEBench with ``make -f makefile_arm`` for armv7 systems or ``make -f makefile_intel`` for x86_64 ISA
Benchmarking
-------
Initiate a simple testrun with ``./epebench -m vidplay -a 3 -n 4 -u 30 -t 20``

This allocates four threads *[-n]* executing the **vidplay** workload *[-m]* distributed on three cores *[-a]* with a total CPU load of 30% *[-u]* for 20 seconds *[-t]*.
The following example shows, how to starts two different models running on four threads with different usage level each (vidplay: cpu0+1 @ 30% load per thread / gzip: cpu2+3 @ 90% load per thread).

Run two workload models with ``./epebench -m vidplay gzip -n 4 -a 4 -u 30 90 -t 20``

Manual
-------
usage: ``epebench [-h] [-v] [-tTIME] [-iINSTR] [-mMODEL] [-nTHREADS] [-aCPU] [-uUSAGE] [-d] [-p] [-r]``

Options:

	-t T1 {T2...Tn}	        Time in seconds
	                        Opt: Enter a list used as test pattern [-p]
	-m MD1 {MD2...Mdn}	    Model(s) to execute (e.g. vidplay, gzip [Note-4])
	-n N {N1...Nn}	        Number of threads to allocate
		                    Opt: Enter a list used as test pattern [-p]
	-u U {U1...Un}	        CPU-load setting (0 – 100)[%]
		                    Opt1: Enter a list used as test pattern[-p]
		                    Opt2: List of CPU-loads per thread (normal)
	-a CPU {CPU1...CPUn}	CPU affinity; amount or list of used cores
	-i N	                Maximum No. of instructions in Mio
	-d	                    CPU-usage process disable. Secondary load controller will not be working
	-p	                    Test pattern mode
	-r	                    Enable thread pinning mode
	-h	                    show this help message and exit
	-v 	                    show epEBench version
	
	
Note-1: List of elements in test pattern mode loops cyclically until last element of longest list is reached. Means, amount of testruns is defined by longest parameter list.
Note-2: If no option is passed, benchmark starts with default values.
Note-3: Thread pinning mode [-r] forces threads bound to assigned CPU but leads to a lower load level accuracy at high thread counts.
Note-4: Refer to ebmodels.ini file for more information and find all available workloads.

