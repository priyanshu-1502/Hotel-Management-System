#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdbool.h>

#define BUF_SIZE 1024

int main() {
    // Shared memory key for communication with hotel manager
    key_t key;
    int shmid;
    bool closeHotel = false;

    // Create a unique key for shared memory
    
    if(system("rm msgq.txt") == -1){}
    
    system("touch msgq.txt");
    if ((key = ftok("msgq.txt", 'z')) == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Create shared memory segment
    if ((shmid = shmget(key, BUF_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    int* exitflag;
    exitflag = (int *)malloc(sizeof(int));
    *exitflag = 0;
    
    
    
    while (1) {
        // Prompt user for inputf
        printf("Do you want to close the hotel? Enter Y for Yes and N for No: ");
        char input;
        scanf(" %c", &input);

        // Check if input is valid
        if (input == 'Y' || input == 'y') {
            // Set flag to close hotel
            closeHotel = true;
            // Inform hotel manager
            //strcpy(shm, "CloseHotel");
            exitflag = shmat(shmid, NULL , 0);
                    
            *exitflag = 1;
            break;
        } else if (input == 'N' || input == 'n') {
            // Continue running admin process
            continue;
        } else {
            printf("Invalid input. Please enter Y or N.\n");
        }
    }

    // Detach shared memory segment
    if (shmdt(exitflag) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    // If hotel needs to close, terminate admin process
    if (closeHotel) {
        printf("Hotel closing. Admin process terminating.\n");
    }

    return 0;
}
