#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>


struct Shared { 
    int multiple;  // to test if its multiple of
    int counter;    // Shared counter updated/read by both processes
};

// Helper function - Convert milliseconds to microseconds
static void nap(int ms){ usleep((useconds_t)ms*1000); }


int main(void){
    key_t key = 0x4444; // any constant key for the SHM segment
    // Allocate a shared memory segment large enough for struct Shared
    int shmid = shmget(key, sizeof(struct Shared), IPC_CREAT | 0666);
    if(shmid < 0){ 
        perror("shmget"); 
        exit(1);
    }
    // Attach the shared memory segment to this process' address space.
    struct Shared *sh = (struct Shared*)shmat(shmid, NULL, 0);
    if(sh == (void*)-1){ 
        perror("shmat"); 
        exit(1);
    }

    // Initialize SHM
    sh->multiple = 3; // Initial divisor for to test multiple of
    sh->counter  = 0;   // Start counter at 0 


    // fork + exec child 
    pid_t pid = fork();
    if(pid < 0){ 
        perror("fork");
        exit(1);
    }
    if(pid == 0){
        // Replace child with external program: Process 2
        char *argv[] = {"./question2.4_process2_101289630_101287549", NULL};  // Argument list for execv()
        execv(argv[0], argv); // Execute the external program
        perror("execv question2.4_process2_101289630_101287549");
        exit(1);
    }

    // Parent loop
    while(1){
        int c = sh->counter;  // Read current shared counter
        int m = sh->multiple;  // Read current multiple/divisor

        // Print depending on whether c is a multiple of m.
        if(c % m == 0){
            printf("[Process1] counter=%d – %d is a multiple of %d\n", c, c, m);
        } else {
            printf("[Process1] counter=%d\n", c);
        }
        fflush(stdout); // Force output to appear immediately

        // increment and store back
        sh->counter = c + 1;

        if(sh->counter > 500) break; // both should stop >500
        nap(100); // Sleep for 100 milliseconds before next cycle
    }

    // After exiting loop, parent prints final message.
    printf("[P1] counter now %d (>500). Finishing.\n", sh->counter);
    shmdt(sh); // Detach shared memory from parent process.

    return 0;
}