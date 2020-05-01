#include <math.h>
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
//#define mem(a, b) (2000 + ((b - a) / 1000 + 1) * 82)
#define mem(a, b) (b)
//2000 + ((b - a) / 1000 + 1) * 82
long* ShmPTR;
struct args {
    int tid;
    int pid;
    int min_intval;
    int max_intval;
    int* primes;
    long prime_n;
};
void printThread(struct args* arg) {
    printf("Thread %d.%d: searching in %d-%d\n", arg->pid, arg->tid, arg->min_intval, arg->max_intval);
}
void printProcess(struct args* arg) {
    printf("Slave %d: Started. Interval %d-%d\n", arg->pid, arg->min_intval, arg->max_intval);
}
void* func(void* arg) {
    printf("HELLO\n");

    struct args* targ = (struct args*)arg;
    targ->primes = (int*)malloc(mem(targ->min_intval, targ->max_intval) * sizeof(int));
    targ->prime_n = 0;
    /*
    //print(targ);
    int cur = 0;
    */
    /*
    for (int n = targ->min_intval; n <= targ->max_intval; n++) {
        int flag = 1;
        for (int i = 2; i <= sqrt(n); i++) {
            if (n % i == 0) {
                flag = 0;
                break;
            }
        }

        if (flag == 1) {
            //printf("Alrighty\n");
            targ->primes[targ->prime_n++] = n;
            ShmPTR[targ->min_intval + cur++] = n;
        }
    }
    
    printf("%d Found all Primes:\n", targ->tid);
    printf("prime_n: %ld\n", targ->prime_n);
    for (int i = 0; i < targ->prime_n; i++) {
        printf("%d ", targ->primes[i]);
    }
    printf("\n%d Finished!!!!!!!!!!!!!!!!\n\n", targ->tid);
*/
    pthread_exit((void*)targ);
}

void ClientProcess(int nt, struct args* arg) {
    pthread_t* thread = (pthread_t*)malloc(nt * sizeof(pthread_t));
    struct args* targs = (struct args*)malloc(nt * sizeof(struct args));
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
    for (t = 0; t < nt; ++t) {
        int minval, maxval;
        targ.pid = arg->pid;
        targ.tid = t + 1;

        targ.min_intval += t * interval_len;
        targ.max_intval = targ.min_intval + interval_len + reminder;
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
        printf("End thread\n");

        struct args* temp = status;

        printf("RETURN VALUE: %ld\n", (long)temp->prime_n);
        /*
        for (int i = 0; i < temp->prime_n; i++) {
            printf("%d ", temp->primes[i]);
        }
        printf("\n");
        */
        if (rc) {
            printf("ERROR: return code from pthread_join() for thread %ld is %d\n", t, rc);
            exit(t);
        }
    }

    pthread_exit(NULL);
}
void CreateProcesses(int nt, int np, struct args* arg) {
    pid_t pid;
    int reminder = 0;
    int interval_len = (arg->max_intval - arg->min_intval) / np;

    for (int i = 0; i < np; i++)  // loop will run n times (n=np)
    {
        /*
        pid = fork();
        printf("PID IS %d", pid);
        if (pid < 0) {
            printf("*** fork error (server) ***\n");
            exit(1);
        } else*/
        if (fork() == 0) {
            //printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");

            arg->pid = i + 1;
            arg->min_intval += i * interval_len + reminder;
            arg->max_intval = arg->min_intval + interval_len;
            printProcess(arg);
            //printf("IMPORTANTEEEE%d\n", nt);
            ClientProcess(nt, arg);
            exit(0);
        }
        reminder += 1;
    }
    int pstatus;
    int corpse;
    while ((corpse = wait(&pstatus)) > 0)
        ;
}
int main(int argc, char* argv[]) {
    //printf("AT THE TOP OF THE WORLD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    struct args *arg, arge;
    arg = &arge;
    int np, nt;

    int ShmID;

    void* status;
    sscanf(argv[1], "%d", &arg->min_intval);
    sscanf(argv[2], "%d", &arg->max_intval);
    sscanf(argv[3], "%d", &np);
    sscanf(argv[4], "%d", &nt);
    //printf("%d %d %d %d\n", arg->min_intval, arg->max_intval, np, nt);

    int memory = mem(arg->min_intval, arg->max_intval);

    ShmID = shmget(IPC_PRIVATE, memory * sizeof(long), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** shmget error (server) ***\n");
        exit(1);
    }
    //printf("Server has received a shared memory of%d, %d memory...\n", arg->max_intval - arg->min_intval, memory);
    ShmPTR = (long*)shmat(ShmID, NULL, 0);
    if ((long)ShmPTR == -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
    //printf("Server has attached the shared memory...\n");

    printf("Master: Started.\n");
    CreateProcesses(nt, np, arg);

    //printf("\n\n\n\n");

    printf("Master: Done. Prime numbers are: ");
    for (int i = 0; i < memory; i++) {
        if (ShmPTR[i] != 0)
            printf("%ld, ", ShmPTR[i]);
    }
    printf("\n");

    return 0;
}
