/**
 * 
 * To Compile use:
 * gcc -o program Homework2.c -pthread 
 * 
 * To Run use:
 * ./program interval_min interval_max np nt
 * 
 * For some reason ./a.out does not appear so im using this compile command
 * */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define mem(a, b) (b)
long* ShmPTR;
int np, nt;
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

    for (int n = targ->min_intval; n <= targ->max_intval; n++) {
        if (n == 1 || n == 0)
            continue;

        int flag = 1;
        for (int i = 2; i * i < n; i++) {
            if (n % i == 0) {
                flag = 0;
                break;
            }
        }

        if (flag == 1) {
            ShmPTR[n] = n;
        }
    }

    pthread_exit((void*)targ);
}

void ClientProcess(int nt, struct args* arg) {
    pthread_t* thread = (pthread_t*)malloc(nt * sizeof(pthread_t));
    struct args* targs = (struct args*)malloc(nt * sizeof(struct args));
    pthread_attr_t attr;
    int rc;
    long t;
    void* status;

    int interval_len = arg->max_intval - arg->min_intval;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    struct args targ;
    for (t = 0; t < nt; ++t) {
        int minval, maxval;
        targ.pid = arg->pid;
        targ.tid = t + 1;
        targ.max_intval = arg->min_intval + (t + 1) * interval_len / nt;
        targ.min_intval = arg->min_intval + t * interval_len / nt;
        if (t != 0)
            targ.min_intval++;

        targs[t] = targ;
    }
    for (t = 0; t < nt; ++t) {
        printThread(&targs[t]);

        rc = pthread_create(&thread[t], &attr, func, (void*)&targs[t]);
        if (rc) {
            printf("ERROR: return code from pthread_create() for thread %ld is %d\n", t, rc);
            exit(t);
        }
    }

    pthread_attr_destroy(&attr);

    for (t = 0; t < nt; ++t) {
        rc = pthread_join(thread[t], &status);

        if (rc) {
            printf("ERROR: return code from pthread_join() for thread %ld is %d\n", t, rc);
            exit(t);
        }
    }

    pthread_exit(NULL);
}
void CreateProcesses(int nt, int np, struct args* arg) {
    int interval_len = arg->max_intval - arg->min_intval;

    for (int i = 0; i < np; i++) {
        if (fork() == 0) {
            arg->max_intval = arg->min_intval + (i + 1) * interval_len / np;
            arg->min_intval = arg->min_intval + i * interval_len / np;
            if (i != 0)
                arg->min_intval++;
            arg->pid = i + 1;

            printProcess(arg);
            ClientProcess(nt, arg);
            exit(0);
        }
    }
    int pstatus;
    int corpse;
    while ((corpse = wait(&pstatus)) > 0)
        ;
}
int main(int argc, char* argv[]) {
    struct args *arg, arge;
    arg = &arge;
    void* status;

    sscanf(argv[1], "%d", &arg->min_intval);
    sscanf(argv[2], "%d", &arg->max_intval);
    sscanf(argv[3], "%d", &np);
    sscanf(argv[4], "%d", &nt);
    int memory = mem(arg->min_intval, arg->max_intval);
    int ShmID;
    ShmID = shmget(IPC_PRIVATE, memory * sizeof(long), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** shmget error (server) ***\n");
        exit(1);
    }
    ShmPTR = (long*)shmat(ShmID, NULL, 0);
    if ((long)ShmPTR == -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }

    CreateProcesses(nt, np, arg);

    printf("Master: Done. Prime numbers are: ");
    for (int i = 0; i < memory; i++) {
        if (ShmPTR[i] != 0) {
            printf("%ld, ", ShmPTR[i]);
        }
    }
    printf("\n");
    return 0;
}

/**
 * 
 * To Compile use:
 * gcc -o program Homework2.c -pthread 
 * 
 * To Run use:
 * ./program interval_min interval_max np nt
 * 
 * For some reason ./a.out does not appear so im using this compile command
 * */
