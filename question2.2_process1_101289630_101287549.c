#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function - Convert milliseconds to microseconds
static void nap(int ms) { usleep((useconds_t)ms*1000); }

int main(void){
    pid_t pid = fork(); // Create a new process using fork()
    // Check if fork failed
    if(pid < 0) { 
        perror("fork");
         exit(1); 
    } 

    if(pid == 0){
        // Child: exec Process 2 (separate program)
        char *argv[] = {"./question2.2_process2_101289630_101287549", NULL}; // Argument list for execv()
        execv(argv[0], argv);  // Execute the external program
        perror("execv question2.2_process2_101289630_101287549");
        exit(1);
    }

    // Parent: Process 1 – multiples of 3, show cycle number
    long cycle = 0; // Tracks how many cycles have passed
    long value = 0;  // The current number being checked for multiples of 3
    while(1){
        // Check if current value is a multiple of 3
        if(value % 3 == 0){
            printf("[Process1 pid=%d] Cycle %ld – %ld is a multiple of 3\n", getpid(), cycle, value);
        } else {
            printf("[Process1 pid=%d] Cycle %ld\n", getpid(), cycle);
        }
        fflush(stdout);   // Force output to appear immediately
        ++cycle;          // Increment cycle counter
        ++value;          // Increment value
        nap(100);         // Sleep for 100 milliseconds before next cycle
    }
}