#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// Helper function - Convert milliseconds to microseconds
static void nap(int ms){ usleep((useconds_t)ms*1000); }

int main(void){
    long cycle = 0;  // Tracks how many cycles (iterations) have passed
    long value = 0;  // The current number being checked for multiples of 3
    while(1){
        // Check if current value is a multiple of 3
        if(value % 3 == 0){
            printf("[P2 pid=%d] Cycle %ld – %ld is a multiple of 3\n", getpid(), cycle, value);
        } else {
            printf("[P2 pid=%d] Cycle %ld\n", getpid(), cycle);
        }
        fflush(stdout);   // Force output to appear immediately 
        ++cycle;          // Increment cycle counter
        ++value;          // Increment value
        nap(100);         // Sleep for 100 milliseconds before next cycle
    }
}