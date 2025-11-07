#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct Shared { 
    int multiple;  // to test if its multiple of
    int counter;    // Shared counter updated/read by both processes
};

// Helper function - Convert milliseconds to microseconds
static void nap(int ms){ usleep((useconds_t)ms*1000); }

int main(void){
    key_t key = 0x4444; // any constant key for the SHM segment
    // Allocate a shared memory segment large enough for struct Shared
    int shmid = shmget(key, sizeof(struct Shared), 0666);
    if(shmid < 0){ 
        perror("shmget child"); 
        exit(1);
    }
    // Attach the shared memory segment to this process' address space.
    struct Shared *sh = (struct Shared*)shmat(shmid, NULL, 0);
    if(sh == (void*)-1){ 
        perror("shmat child");
        exit(1);
    }

    // Wait until counter>100 to "start"
    while(sh->counter <= 100) { nap(50); }

    while(1){
        int c = sh->counter;  // Read current shared counter
        int m = sh->multiple;  // Read current multiple/divisor

        // Print depending on whether c is a multiple of m.
        if(c % m == 0){
            printf("[P2] counter=%d – %d is a multiple of %d\n", c, c, m);
        } else {
            printf("[P2] counter=%d (m=%d)\n", c, m);
        }
        fflush(stdout); // Force output to appear immediately
        if(c > 500) break; // both should stop >500
        nap(110); // Sleep for 100 milliseconds before next cycle
    }

    // After exiting loop, parent prints final message.
    printf("[P2] observed counter=%d (>500). Exiting.\n", sh->counter);
    shmdt(sh); // Detach shared memory from parent process.
    return 0;
}