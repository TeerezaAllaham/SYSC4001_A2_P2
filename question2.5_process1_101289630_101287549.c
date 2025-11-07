#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <signal.h>

struct Shared { 
    int multiple;  // to test if its multiple of
    int counter;    // Shared counter updated/read by both processes
};

// Helper function - Convert milliseconds to microseconds
static void nap(int ms){ usleep((useconds_t)ms*1000); }

// wait / down operation
static int P(int semid){
    struct sembuf op = {0, -1, 0}; 
    return semop(semid, &op, 1);
}
// signal / up operation
static int V(int semid){
    struct sembuf op = {0, +1, 0};
    return semop(semid, &op, 1);
}


int main(void){
    key_t shm_key = 0x5555,  // must match child
    key_t sem_key = 0x5556;  // any constant key for the SHM segment

    // Allocate a shared memory segment large enough for struct Shared
    int shmid = shmget(shm_key, sizeof(struct Shared), IPC_CREAT | 0666);
    if(shmid < 0){ 
        perror("shmget"); 
        exit(1);
    }
    
    // Create/get semaphore set of size 1
    int semid = semget(sem_key, 1, IPC_CREAT | 0666);
    if(semid < 0){ 
        perror("semget");
        exit(1);
    }

    // Initialize semaphore to 1 
    if(semctl(semid, 0, SETVAL, 1) < 0){ 
        perror("semctl SETVAL"); 
        exit(1);
    }

    // Attach shared memory
    struct Shared *sh = (struct Shared*)shmat(shmid, NULL, 0);
    if(sh == (void*)-1){ 
        perror("shmat"); 
        exit(1);
    }

    // Initialize shared state under lock
    if(P(semid) < 0){ 
        perror("P initial"); 
        exit(1);
    }
    sh->multiple = 3;   // check multiples of 3
    sh->counter  = 0;   // start counter at 0
    if(V(semid) < 0){ 
        perror("V initial"); 
        exit(1);
    }

    // fork child 
    pid_t pid = fork();
    if(pid < 0){ 
        perror("fork"); 
        exit(1);
    }
    if(pid == 0){
        // Replace child with external program: Process 2
        char *argv[] = {"./question2.5_process2_101289630_101287549", NULL};  // Argument list for execv()
        execv(argv[0], argv); // Execute the external program
        perror("execv question2.5_process2_101289630_101287549");
        exit(1);
    }

    // Parent loop
    while(1){
        // Lock shared memory
        if(P(semid) < 0){ 
            perror("P loop"); 
            break;
        }

        int c = sh->counter; // Read current shared counter
        int m = sh->multiple;  // Read current multiple/divisor

        // Print depending on whether c is a multiple of m.
        if(c % m == 0){
            printf("[P1] counter=%d - %d is a multiple of %d\n", c, c, m);
        } else {
            printf("[P1] counter=%d\n", c);
        }

        // increment and store back
        sh->counter = c + 1;

        int done = (sh->counter > 500); // Both should stop >500
        if(V(semid) < 0){ 
            perror("V loop"); 
            break; 
        }

        if(done) break;
        nap(100);  // Sleep for 100 milliseconds before next cycle
    }


    printf("[Process1] finished. Cleaning up resources…\n");
    shmdt(sh);  // Detach shared memory from parent process
    nap(200);   // Allow child process to detach first  
    shmctl(shmid, IPC_RMID, NULL); // Mark shared memory segment for deletion
    semctl(semid, 0, IPC_RMID);  // Remove semaphore set from the system  
    return 0;    
}