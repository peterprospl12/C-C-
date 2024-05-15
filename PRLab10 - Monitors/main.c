#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
struct msg_buffer {
    long msg_type;
    char msg_text[1];
};

key_t key;
int msgid;
int* shared_resource;

void binary_semaphore_initialize() {
    key = ftok("progfile", 65);
    msgid = msgget(key, 0666 | IPC_CREAT);
    struct msg_buffer message;
    message.msg_type = 1;
    msgsnd(msgid, &message, sizeof(message), 0);
}

void binary_semaphore_wait() {
    struct msg_buffer message;
    msgrcv(msgid, &message, sizeof(message), 1, 0);
}

void binary_semaphore_signal() {
    struct msg_buffer message;
    message.msg_type = 1;
    msgsnd(msgid, &message, sizeof(message), 0);
}


int main() {
    binary_semaphore_initialize();
    int shmid = shmget(key, sizeof(int), 0666|IPC_CREAT);
    shared_resource = (int*) shmat(shmid, (void*)0, 0);
    *shared_resource = 0;

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        while(*shared_resource < 100){
            binary_semaphore_wait();
            *shared_resource +=1;
            printf("child: %d\n", *shared_resource);
            binary_semaphore_signal();

        }

    } else if (pid > 0) {
        while(*shared_resource < 100){
            binary_semaphore_wait();

            *shared_resource +=1;
            printf("parent: %d\n", *shared_resource);

            binary_semaphore_signal();

        }
        wait(NULL);
    } else {
        printf("Fork failed!\n");
        return 1;
    }
    shmdt(shared_resource);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}