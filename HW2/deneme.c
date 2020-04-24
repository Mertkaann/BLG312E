#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define NUM_THREADS 2
struct args {
    int tid;
    int pid;
    int min_intval;
    int max_intval;
};
void printThread(struct args* arg) {
    printf("Thread %d.%d: searching in %d-%d\n", arg->pid, arg->tid, arg->min_intval, arg->max_intval);
}
void printProcess(struct args* arg) {
    printf("Slave %d: Started. Interval %d-%d\n", arg->pid, arg->min_intval, arg->max_intval);
}
void* func(void* arg) {
    struct args* targ = (struct args*)arg;
    //print(targ);
    for (int n = targ->min_intval; n <= targ->max_intval; n++) {
        int flag = 1;
        for (int i = 2; i <= sqrt(n); i++) {
            if (n % i == 0) {
                flag = 0;
                break;
            }
        }

        if (flag == 1) {
            //printf("%d is a prime number\n", n);
        }
    }
}

void create_nthreads(int nt, long pid, struct args* arg) {
    pthread_t thread[NUM_THREADS];
    struct args targs[NUM_THREADS];
    pthread_attr_t attr;
    int rc;
    long t;
    void* status;

    int interval_len = (arg->max_intval - arg->min_intval) / nt;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    struct args targ;
    targ.min_intval = arg->min_intval;
    int reminder = 0;
    for (t = 0; t < NUM_THREADS; ++t) {
        int minval, maxval;
        targ.pid = arg->pid;
        targ.tid = t + 1;

        targ.min_intval += t * interval_len;
        targ.max_intval = targ.min_intval + interval_len + reminder;
        targs[t] = targ;
    }
    for (t = 0; t < NUM_THREADS; ++t) {
        printThread(&targs[t]);

        rc = pthread_create(&thread[t], &attr, func, (void*)&targs[t]);
        if (rc) {
            printf("ERROR: return code from pthread_create() for thread %ld is %d\n", t, rc);
            exit(t);
        }
    }

    pthread_attr_destroy(&attr);

    for (t = 0; t < NUM_THREADS; ++t) {
        rc = pthread_join(thread[t], &status);
        if (rc) {
            printf("ERROR: return code from pthread_join() for thread %ld is %d\n", t, rc);
            exit(t);
        }
    }

    pthread_exit(NULL);
}
int main(int argc, char* argv[]) {
    struct args *arg, arge;
    arg = &arge;
    int np, nt;
    void* status;
    sscanf(argv[1], "%d", &arg->min_intval);
    sscanf(argv[2], "%d", &arg->max_intval);
    sscanf(argv[3], "%d", &np);
    sscanf(argv[4], "%d", &nt);
    printf("%d %d %d %d\n", arg->min_intval, arg->max_intval, np, nt);
    int interval_len = (arg->max_intval - arg->min_intval) / np;
    int reminder = 0;
    printf("Master: Started.\n");
    for (int i = 0; i < np; i++)  // loop will run n times (n=5)
    {
        if (fork() == 0) {
            arg->pid = i + 1;
            arg->min_intval += i * interval_len + reminder;
            arg->max_intval = arg->min_intval + interval_len;
            printProcess(arg);
            create_nthreads(2, getpid(), arg);
            break;
        }
        reminder += 1;
    }

    int pstatus;
    int corpse = wait(&pstatus);
    printf("Master: Done.\n");
    return 0;
}
