#ifndef PTHREADFUNCS_H
#define PTHREADFUNCS_H

#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>


// count of threads
#define COUNT_THREADS 4
// count of iterations
#define COUNT_ITERATIONS 10000

// struct send to thread
struct ThreadArgs {
    // my ID of thread
    int  id;
    // something string (with name of thread, f.e.)
    char tag[10];
    char message[64];
};

extern int counter;

extern pthread_cond_t g_cond;
extern int g_buffer_full;   // 1 — в буфере есть данные, 0 — пусто
extern int g_buffer_value;  // само «сообщение» из буфера (например, номер)

// common resources - is a file for logging
extern int g_fd;

// mutex
extern pthread_mutex_t g_lock;

// git ID of current thread
pid_t getThreadID(void);

// write a string from thread with mutex
int write_line(const char *msg);

// pointer for thread's function
void *func_thread(void *arg);

void about(void);

void *producer_thread(void *arg);
void *consumer_thread(void *arg);

#endif
