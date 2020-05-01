#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
void ClientProcess();
struct Shm {
    long *ShmPTR;
    int size;
} * shm;
int *ShmPTR;
void main(int argc, char *argv[]) {
    int ShmID;

    pid_t pid;
    int status;

    if (argc != 5) {
        printf("Use: %s #1 #2 #3 #4\n", argv[0]);
        exit(1);
    }

    ShmID = shmget(IPC_PRIVATE, 4 * sizeof(struct Shm), IPC_CREAT | 0666);
    if (ShmID < 0) {
        printf("*** shmget error (server) ***\n");
        exit(1);
    }
    //printf("Server has received a shared memory of four integers...\n");

    ShmPTR = (int *)shmat(ShmID, NULL, 0);
    if ((int)ShmPTR == -1) {
        printf("*** shmat error (server) ***\n");
        exit(1);
    }
    //printf("Server has attached the shared memory...\n");

    ShmPTR[0] = atoi(argv[1]);
    ShmPTR[1] = atoi(argv[2]);
    ShmPTR[2] = atoi(argv[3]);
    ShmPTR[3] = atoi(argv[4]);
    printf("Server has filled %d %d %d %d in shared memory...\n",
           ShmPTR[0], ShmPTR[1], ShmPTR[2], ShmPTR[3]);
    int np = 2;
    //printf("Server is about to fork a child process...\n");
    for (int i = 0; i < np; i++)  // loop will run n times (n=np)
    {
        pid = fork();
        if (pid < 0) {
            printf("*** fork error (server) ***\n");
            exit(1);
        } else if (pid == 0) {
            ClientProcess();
            exit(0);
        }
        printf("Server %d has PRINTS %d %d %d %d in shared memory...\n",
               i, ShmPTR[0], ShmPTR[1], ShmPTR[2], ShmPTR[3]);
    }

    wait(&status);
    //printf("Server has detected the completion of its child...\n");
    printf("Server has PRINTS %d %d %d %d in shared memory...\n",
           ShmPTR[0], ShmPTR[1], ShmPTR[2], ShmPTR[3]);
    shmdt((void *)ShmPTR);
    //printf("Server has detached its shared memory...\n");
    shmctl(ShmID, IPC_RMID, NULL);
    //printf("Server has removed its shared memory...\n");
    printf("Server exits...\n");
    exit(0);
}

void ClientProcess() {
    //printf("   Client process started\n");
    printf("   Client found %d %d %d %d in shared memory\n",
           ShmPTR[0], ShmPTR[1], ShmPTR[2], ShmPTR[3]);
    if (ShmPTR[0] != 999)
        ShmPTR[0] = 999;
    else
        ShmPTR[0] = 669;
    //printf("   Client is about to exit\n");
}