#ifndef CPU_STATUS_H_INCLUDED
#define CPU_STATUS_H_INCLUDED

void init();
void wait ( uint);
double getCurrentValue();
void *cpu_usage(void *data);

#endif // CPU_STATUS_H_INCLUDED


