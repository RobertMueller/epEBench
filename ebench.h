#ifndef EBENCH_H_INCLUDED
#define EBENCH_H_INCLUDED

#define gettid() syscall(__NR_gettid)
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

void print_header(void);

#endif // EBENCH_H_INCLUDED



