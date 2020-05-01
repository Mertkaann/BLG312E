#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

struct list {
    int number;
    struct list *next;
};
typedef struct list list;

struct interval {
    int id;
    int min;
    int max;
};
typedef struct interval interval;

void readFromShrMem(int id) {
    int shmid = shmget(id, 2048, 0666 | IPC_CREAT);

    char *str = shmat(shmid, 0, 0);

    printf("%s", str);

    shmdt(str);
    shmctl(shmid, IPC_RMID, NULL);
}

void writeToShrMem(int id, char *data) {
    int shmid = shmget(id, 2048, 0666 | IPC_CREAT);

    char *str = shmat(shmid, 0, 0);
    memcpy(str, data, strlen(data) + 1);

    shmdt(str);
}

char *listToStr(list *head) {
    char *str = malloc(1000);
    list *current_node = head;
    while (current_node != NULL) {
        char s[10];
        sprintf(s, "%d ", current_node->number);
        strcat(str, s);
        current_node = current_node->next;
    }

    return str;
}

void *findPrime(void *value) {
    list *arr = malloc(sizeof(list));
    arr->number = -1;
    arr->next = NULL;

    list *iter = arr;

    int i;
    for (i = ((interval *) value)->min; i <= ((interval *) value)->max; i++) {
        if (i <= 1) {
            continue;
        }
        int boolean = 1;

        int j;
        for (j = 2; j <= (i / 2 + 1); j++) {
            if (i == 2) {
                break;
            }
            if (i % j != 0) {
                continue;
            } else {
                boolean = 0;
                break;
            }
        }
        if (boolean == 1) {
            if (arr->number == -1) {
                arr->number = i;
            } else {
                iter->next = malloc(sizeof(list));
                iter->next->number = i;
                iter->next->next = NULL;
                iter = iter->next;
            }
        }
    }

    if (arr->number != -1) {
        writeToShrMem(((interval *) value)->id, listToStr(arr));
    }

    return NULL;
}

int concat(int a, int b) {
    char s1[10];
    char s2[10];

    sprintf(s1, "%d", a);
    sprintf(s2, "%d", b);

    strcat(s1, s2);

    return atoi(s1);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("USAGE: ./program INVTERVAL_MIN INTERVAL_MAX NUMBER_OF_PROCESSES NUMBER_OF_THREADS\n");
        exit(1);
    }

    printf("Master: Started.\n");
    pid_t wpid;
    int status = 0;

    int np = atoi(argv[3]);
    int nt = atoi(argv[4]);

    int unit1 = (atoi(argv[2]) - atoi(argv[1])) / np;
    int processMin;
    int processMax = atoi(argv[1]) - 1;

    int i;
    for (i = 0; i < np; i++) {
        processMin = processMax + 1;
        processMax = processMin + unit1;

        if (abs(processMax - atoi(argv[2])) < unit1 - 1)
            processMax = atoi(argv[2]);

        int id = fork();
        if (id == 0) {
            printf("Slave%d: Started. Interval %d-%d\n", i + 1, processMin, processMax);

            int threadMin;
            int threadMax = processMin - 1;

            int j;
            for (j = 0; j < nt; j++) {
                threadMin = threadMax + 1;
                int unit2 = (processMax - threadMin) / (nt - j);
                threadMax = threadMin + unit2;

                if (abs(threadMax - processMax) < unit2 - 1)
                    threadMax = processMax;

                printf("Thread%d.%d: searching in %d-%d\n", i + 1, j + 1, threadMin, threadMax);

                interval *interval1 = malloc(sizeof(interval));
                interval1->min = threadMin;
                interval1->max = threadMax;
                interval1->id = concat(i + 1, j + 1);

                pthread_t thread;
                pthread_create(&thread, NULL, findPrime, (void *) interval1);
                pthread_join(thread, NULL);
            }

            printf("Slave%d: Done\n", i + 1);
            exit(0);
        }
    }

    while ((wpid = wait(&status)) > 0);

    printf("Master: Done. Prime numbers are:\n");

    for (i = 1; i <= np; i++) {
    	int j;
        for (j = 1; j <= nt; j++)
            readFromShrMem(concat(i, j));
    }



    printf("\n");

    return 0;
}
