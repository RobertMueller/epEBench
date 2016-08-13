#ifndef EBMODELS_H_INCLUDED
#define EBMODELS_H_INCLUDED

#define gettid() syscall(__NR_gettid)
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

void* ebmodel(void*);
void init_dmem();
void init_fmem();

#endif // EBMODELS_H_INCLUDED
