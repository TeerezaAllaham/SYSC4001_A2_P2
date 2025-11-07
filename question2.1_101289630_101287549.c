#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

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
        // Child = Process 2
        unsigned long counter = 0; // Counter variable that increases indefinitely
        while(1){
            printf("[Process2 pid=%d] counter=%lu\n", getpid(), counter++); // Print PID and counter
            fflush(stdout);     // Force output to appear immediately
            nap(100);      // Wait 100 milliseconds before repeating
        }
    } else {
        // Parent = Process 1
        unsigned long counter = 0; // Counter variable that increases indefinitely
        while(1){
            printf("[Process1 pid=%d] counter=%lu\n", getpid(), counter++); // Print PID and counter
            fflush(stdout);     // Force output to appear immediately
            nap(100);      // Wait 100 milliseconds before repeating
        }
    }
    return 0;
} // Stop with `ps` + `kill`