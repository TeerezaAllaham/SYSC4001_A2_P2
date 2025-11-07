#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// Helper function - Convert milliseconds to microseconds
static void nap(int ms){ usleep((useconds_t)ms*1000); }


int main(void){
    long cycle = 0, // Tracks how many cycles have passed
    long value = 0;  // The current number being checked for multiples of 3
    // Loop while value is between 0 and 500 (inclusive)
    while(value >= -500){
        // Check if current value is a multiple of 3
        if(value % 3 == 0){
            printf("[Process2] Cycle %ld - %ld is a multiple of 3\n", cycle, value);
        } else {
            printf("[Process2] Cycle %ld\n", cycle);
        }
        fflush(stdout);   // Force output to appear immediately
        ++cycle;          // Increment cycle counter
        ++value;          // Increment value
        nap(100);         // Sleep for 100 milliseconds before next cycle
    }
    printf("[P2] Reached value %ld < -500. Exiting child.\n", value);
    return 0;
}