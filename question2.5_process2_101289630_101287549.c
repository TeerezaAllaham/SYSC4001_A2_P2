#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

struct Shared { 
    int multiple;  // to test if its multiple of
    int counter;    // Shared counter updated/read by both processes
};

// Helper function - Convert milliseconds to microseconds
static void nap(int ms){ usleep((useconds_t)ms*1000); }

// wait / down operation
static int P(int semid){
    struct sembuf op = {0, -1, 0};  // Operate on semaphore 0, subtract 1
    return semop(semid, &op, 1); // Perform the operation
}

// signal / up operation
static int V(int semid){
    struct sembuf op = {0, +1, 0}; // Operate on semaphore 0, add 1
    return semop(semid, &op, 1); // Perform the operation
}

int main(void){
    key_t shm_key = 0x5555;  // must match child
    key_t sem_key = 0x5556;  // any constant key for the SHM segment

    // Allocate a shared memory segment large enough for struct Shared
    int shmid = shmget(shm_key, sizeof(struct Shared), 0666);
    if(shmid < 0){ 
        perror("shmget child"); 
        exit(1);
    }

    // Create/get semaphore set of size 1
    int semid = semget(sem_key, 1, 0666);
    if(semid < 0){ 
        perror("semget child");
        exit(1);
    }

    // Attach shared memory
    struct Shared *sh = (struct Shared*)shmat(shmid, NULL, 0);
    if(sh == (void*)-1){ 
        perror("shmat child"); 
        exit(1);
    }

    // Wait until counter>100
    while(1){
        if(P(semid) < 0){ 
            perror("P wait"); 
            exit(1);
        }

        int c = sh->counter; // Read current shared counter
        int m = sh->multiple;  // Read current multiple/divisor
        int ready = (c > 100);  // Check if counter is above threshold
        if(V(semid) < 0){    // Unlock semaphore
            perror("V wait"); 
            exit(1);
        }

        if(ready) break;  // If ready, proceed to main loop
        nap(100);  // Sleep for 100 milliseconds before next cycle
    }

    // Wait until counter>500
    while(1){
        if(P(semid) < 0){   // Lock semaphore to access shared memory
            perror("P loop"); 
            exit(1);
        }

        int c = sh->counter; // Read current shared counter
        int m = sh->multiple;  // Read current multiple/divisor

        // Print depending on whether c is a multiple of m.
        if(c % m == 0){
            printf("[Process2] counter=%d - %d is a multiple of %d\n", c, c, m);
        } else {
            printf("[Process2] counter=%d (m=%d)\n", c, m);
        }

        int done = (c > 500);  // Both should stop >500
        if(V(semid) < 0){ 
            perror("V loop"); 
            exit(1);
        }

        if(done) break;   // If ready, proceed to main loop
        nap(100);  // Sleep for 100 milliseconds before next cycle
    }

    printf("[Process2] done; detaching.\n");
    shmdt(sh); // Detach shared memory from child process
    return 0;

}
