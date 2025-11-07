#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

// Helper function - Convert milliseconds to microseconds
static void nap(int ms){ usleep((useconds_t)ms*1000); }

int main(void){
    pid_t pid = fork();   // Create a new process using fork()
    // Check if fork failed
    if(pid < 0) { 
        perror("fork");
         exit(1); 
    } 
    if(pid == 0){
        // Replace child with external program: Process 2
        char *argv[] = {"./question2.3_process2_101289630_101287549", NULL};  // Argument list for execv()
        execv(argv[0], argv);  // Execute the external program
        perror("execv question2.3_process2_101289630_101287549");
        exit(1);
    }
    // Parent prints until child terminates, then exits
    long cycle = 0, // Tracks how many cycles have passed
    long value = 0;  // The current number being checked for multiples of 3
    int status = 0;  // Will hold child's termination status
    while(1){
        // Non-blocking check if child has finished
        pid_t r = waitpid(pid, &status, WNOHANG);
        if(r == pid){
            // Child has terminated
            printf("[Process1] Child finished (status=%d). Exiting Process1.\n", status);
            break; // Exit the loop; parent will end
        }
        // Check if current value is a multiple of 3
        if(value % 3 == 0){
            printf("[Process1] Cycle %ld – %ld is a multiple of 3\n", cycle, value);
        } else {
            printf("[Process1] Cycle %ld\n", cycle);
        }
        fflush(stdout);   // Force output to appear immediately
        ++cycle;          // Increment cycle counter
        ++value;          // Increment value
        nap(100);         // Sleep for 100 milliseconds before next cycle
    }
    return 0;
}