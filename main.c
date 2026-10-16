
#include "pthreadfuncs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syscall.h>

void *dummy_thread(void *arg) {
    (void)arg;
    pthread_exit(NULL);
}

int main(void) {
    // headline
    about();
    // array with threads
    pthread_t threads[COUNT_THREADS];
    struct ThreadArgs *args[COUNT_THREADS];

for (int i = 0; i < COUNT_THREADS; i++) {
    args[i] = malloc(sizeof(struct ThreadArgs));
    if (args[i] == NULL) {
        perror("malloc");
        return EXIT_FAILURE;
    }
    snprintf(args[i]->tag, sizeof(args[i]->tag), "T%d", i + 1);
    args[i]->id = i + 1;
    snprintf(args[i]->message, sizeof(args[i]->message),
             "Hello from thread %d", i + 1);
}

    // sys call - open
    // file, modes, rights
    g_fd = open("output.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (g_fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    printf("main: pid = %d, opened file: \'output.log\' (fd = %d)\n", getThreadID(), g_fd);

        // --- Для задания 32: идентификаторы главного потока ---
    printf("main thread: pthread_self() = %lu\n", (unsigned long)pthread_self());
    printf("main thread: getThreadID() = %d\n", getThreadID());
    printf("main thread: getpid()      = %d\n", getpid());
    // -----------------------------------------------------


    // create structs for threads
  /*  for (int i = 0; i < COUNT_THREADS; i++) {
        args[i].id = i + 1;
        snprintf(args[i].tag, sizeof(args[i].tag), "T%d", i + 1);
	snprintf(args[i].message, sizeof(args[i].message),
             "Hello from thread %d", i + 1);
    }*/    
    // create threads
    for (int i = 0; i < COUNT_THREADS; i++) {
    int rc = pthread_create(&threads[i], NULL, func_thread, args[i]);   /* вариант A */
    if (rc != 0) {
        fprintf(stderr, "pthread_create: %s\n", strerror(rc));
        return EXIT_FAILURE;
    }

    rc = pthread_detach(threads[i]);
    if (rc != 0) {
        fprintf(stderr, "pthread_detach: %s\n", strerror(rc));
    }
}

    // wait stoping all thread
    // wait for detached threads to finish (just sleep)
    struct timespec ts = {15, 0}; // 2 секунды
    nanosleep(&ts, NULL);

    printf("counter = %d (expected %d)\n", counter, COUNT_THREADS * 100000);

    // sys call for close file
        // write final log message
    char log_buf[128];
    snprintf(log_buf, sizeof(log_buf), "main: pid = %d, opened file: 'output.log' (fd = %d)", getThreadID(), g_fd);
    write_line(log_buf);

    // sys call for close file
    if (close(g_fd) < 0) {
        perror("close");
        return EXIT_FAILURE;
    }

    // remove mutex
    pthread_mutex_destroy(&g_lock);

   
    return EXIT_SUCCESS;
}
