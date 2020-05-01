#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>

typedef struct file {
    int pid;
    char tipo[2];
    char nome[20];
    unsigned long genoma;
} file_entry;

int main(void) {
    int shmid;
    int n;
    file_entry *entries;

    if (fork() == 0) {
        if ((shmid = shmget(20441, sizeof(file_entry) + 256, IPC_CREAT | 0666)) == -1) {
            printf("shmget");
            exit(2);
        }

        entries = (file_entry *)shmat(shmid, 0, 0);
        if (entries == NULL) {
            printf("problem2");
            exit(2);
        }

        printf("\nChild Reading ....\n\n");
        printf("%d\n", entries->pid);
        printf("%s\n", entries->tipo);
        printf("%s\n", entries->nome);
        printf("%lu\n", entries->genoma);
        putchar('\n');
        printf("\nDone\n\n");
    } else {
        if ((shmid = shmget(20441, sizeof(file_entry) + 256, IPC_CREAT | 0666)) == -1) {
            printf("problem3");
            exit(2);
        }

        entries = (file_entry *)shmat(shmid, 0, 0);
        if (entries == NULL) {
            printf("problem4");
            exit(2);
        }
        printf("done attachment");

        int pidD = 4001;
        char tipoD[2] = "A";
        char nomeD[20] = "sfds";
        unsigned long genomaD = 10000;

        entries->pid = pidD;
        sprintf(entries->tipo, "%s", tipoD);
        sprintf(entries->nome, "%s", nomeD);
        entries->genoma = genomaD;

        wait(NULL);
        shmdt(&shmid);
    }
    exit(0);
}