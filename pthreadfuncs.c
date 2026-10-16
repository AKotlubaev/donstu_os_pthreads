#include "pthreadfuncs.h"
#include <time.h>
#include <inttypes.h>
#include <string.h>
//
// common resources - is a file for logging
int g_fd = -1;
pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

int counter = 0;

pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;
int g_buffer_full = 0;
int g_buffer_value = 0;

// git ID of current thread
pid_t getThreadID(void) {
    return (pid_t) syscall(SYS_gettid);
}

// write a string from thread with mutex
int write_line(const char *msg) {
    pthread_mutex_lock(&g_lock);
    char buffer[256];
    int len = snprintf(buffer, sizeof(buffer), "%s\n", msg);
    ssize_t bytes_written = write(g_fd, buffer, len);
    pthread_mutex_unlock(&g_lock);

    return (bytes_written < 0) ? -1 : 0;
}

// function for th
void *func_thread(void *arg) {
    struct ThreadArgs *t = (struct ThreadArgs *)arg;

printf("thread [%s]: pthread_self() = %lu, getThreadID() = %d\n", 
           t->tag, (unsigned long)pthread_self(), getThreadID());

     char buf[256];
    int actual_iterations = 0;
   
    for (int i = 0; i < COUNT_ITERATIONS; i++) {
        snprintf(buf, sizeof(buf),
            "[tag = %s] pid = %d ppid = %d tid = %d self = %lu iter = %d msg = '%s'",
            t->tag, getpid(), getppid(), getThreadID(),
            (unsigned long)pthread_self(), i, t->message);
        write_line(buf);

        struct timespec ts = {0, 100000000};
        nanosleep(&ts, NULL);
      actual_iterations++;
   }
        for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&g_lock);
        counter++;
        pthread_mutex_unlock(&g_lock);
    }
 
    free(t);

    pthread_exit(NULL);
}

void *producer_thread(void *arg) {
    (void)arg;
    char buf[256];  // ← теперь она используется ниже
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&g_lock);
        
        while (g_buffer_full) {
            pthread_cond_wait(&g_cond, &g_lock);
        }
        
        g_buffer_value = i;
        g_buffer_full = 1;
        
        pthread_cond_signal(&g_cond);
        pthread_mutex_unlock(&g_lock);

        // Вот здесь мы используем buf!
        snprintf(buf, sizeof(buf), "producer: put %d", i);
        write_line(buf); 
        
        struct timespec ts = {0, 10000000}; // 10 мс
        nanosleep(&ts, NULL);
    }
    pthread_exit(NULL);
}

void *consumer_thread(void *arg) {
    (void)arg;
    char buf[256];
    for (int i = 0; i < 10; i++) {
        int value;

        pthread_mutex_lock(&g_lock);
        while (!g_buffer_full) {
            pthread_cond_wait(&g_cond, &g_lock);
        }
        value = g_buffer_value;
        g_buffer_full = 0;
        pthread_cond_signal(&g_cond);
        pthread_mutex_unlock(&g_lock);

        /* логируем ВНЕ мьютекса — write_line сам захватит g_lock */
        snprintf(buf, sizeof(buf), "consumer: got %d", value);
        write_line(buf);
    }
    pthread_exit(NULL);
}

void about()  {
    printf("Pthread example\n");
}
