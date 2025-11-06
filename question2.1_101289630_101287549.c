#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

static void nap(int ms) { usleep((useconds_t)ms*1000); }

int main(void){
    pid_t pid = fork();
    if(pid < 0) { perror("fork"); exit(1); }

    if(pid == 0){
        // Child = Process 2
        unsigned long c = 0;
        while(1){
        printf("[P2 pid=%d] counter=%lu\n", getpid(), c++);
        fflush(stdout);
        nap(120);
        }
    } else {
    // Parent = Process 1
        unsigned long c = 0;
        while(1){
        printf("[P1 pid=%d] counter=%lu\n", getpid(), c++);
        fflush(stdout);
        nap(100);
        }
    }
    return 0;
} // Stop with `ps` + `kill`