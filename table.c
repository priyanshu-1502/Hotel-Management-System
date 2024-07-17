#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define READ 0
#define WRITE 1
#define MAX_CUSTOMERS 5
#define MENU_FILE "menu.txt"
#define BUF_SIZE (5 * 10 * sizeof(int))
#define MAX_TABLES 10

typedef struct {
    int num;
    int order[10];
} customerOrder;

typedef struct{
    int no;
    int amount[10];
} billamount;;


int main() {
    int MAX_ITEMS=0;
    printf("Enter Table Number: ");
    int tableNumber;
    scanf("%d", &tableNumber);
    if(tableNumber<=0||tableNumber>10)
    {
printf("Table Number should be between 1 and 10");
exit(EXIT_FAILURE);

    }
    printf("Enter Number of Customers at Table (maximum no. of customers can be 5): ");
    int numCustomers;
    scanf("%d", &numCustomers);

    while (numCustomers < 1 || numCustomers > MAX_CUSTOMERS) {
        printf("Invalid number of customers. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    int fd[numCustomers][2];
    for (int i = 0; i < numCustomers; i++) {
        if (pipe(fd[i]) == -1) {
            perror("Pipe creation failed");
            return 1;
        }
    }

    FILE *menuFile = fopen(MENU_FILE, "r");
    if (menuFile == NULL) {
        perror("Error opening menu file");
        exit(EXIT_FAILURE);
    }

    printf("\nMenu:\n");
    char menuLine[100];
    while (fgets(menuLine, sizeof(menuLine), menuFile) != NULL) {
        printf("%s", menuLine);
    }

    fclose(menuFile);

    // Rewind the file to read it again
    menuFile = fopen(MENU_FILE, "r");
    if (menuFile == NULL) {
        perror("Error opening menu file");
        exit(EXIT_FAILURE);
    }

    int costArray[10] = {0}; // Initialize array to 0

    // Read each line from the file
    for (int i = 0; i < 10; i++) {
        char line[100];
       
        // Read a line from the file
        if (fgets(line, sizeof(line), menuFile) == NULL) {
            break; // End of file or error
        }
        MAX_ITEMS++;
        // Parse the cost from the line
        int cost;
        if (sscanf(line, "%*d. %*[^0-9]%d", &cost) == 1) {
            costArray[i] = cost;
        } else {
            fprintf(stderr, "Error parsing menu file at line %d\n", i + 1);
            exit(EXIT_FAILURE);
        }
    }

    fclose(menuFile);

    for (int i = 0; i < numCustomers; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Error forking customer process");
        } else if (pid == 0) {
            customerOrder fullOrder;
            for(int j=0;j<10;j++)
            {
            fullOrder.order[j]=0;
            }
            close(fd[i][READ]);
            printf("Enter the serial number(s) of the item(s) to order from the menu. Enter -1 when done:\n");
            int flag = 1;

   
        
     

   while (flag == 1) {
	    flag = 0; // Initialize flag to 0 at the beginning of each iteration

	    for (int j = 0; j < 10; j++) {
		scanf("%d", &fullOrder.order[j]);

		if (fullOrder.order[j] == -1) {
		    break;
		} else if (fullOrder.order[j] > MAX_ITEMS || fullOrder.order[j] <= 0) {
		    printf("There are only 10 items in the menu! Please re-enter the values:\n");
		    flag = 1; // Set flag to 1 if an invalid value is encountered
		    break;
		}
	    }
    }


            // Calculate the total cost for this customer
    //         int totalCost = 0;
    //         for (int j = 0; j < 10; j++) {
    //             if (fullOrder.order[j] == -1)
    //                 break;
    //             totalCost += costArray[fullOrder.order[j] - 1];
    //         }

    //         printf("Customer %d Total Cost: %d INR\n", i + 1, totalCost);

            // Write the order to the pipe
            write(fd[i][WRITE], &fullOrder, sizeof(customerOrder));
            close(fd[i][WRITE]);

            exit(EXIT_SUCCESS);
         } else {
             waitpid(pid, NULL, 0);
         }
    }



    key_t key;
    system("touch msgq.txt");
    if ((key = ftok("msgq.txt", tableNumber)) == -1) {
        perror("Error in ftok\n");
        return 1;
    }

    int shmid;
    customerOrder *shmPtr;
        shmid = shmget(key, BUF_SIZE, 0644 | IPC_CREAT); // IPC_PRIVATE as 1st arg.
        if (shmid == -1) {
            perror("Error in shmget in creating/accessing shared memory\n");
            return 1;
        }

    // Attach the shared memory segment to the parent process
    shmPtr = shmat(shmid, NULL, 0);
    if (shmPtr == (void *) -1) {
        perror("Error in shmPtr in attaching the memory segment\n");
        return 1;
    }

    // Parent writing to shared memory
    int n = numCustomers;
    for (int i = 0; i < n; i++) {
        close(fd[i][WRITE]);
        customerOrder *final = malloc(sizeof(customerOrder));
        read(fd[i][READ], final, sizeof(customerOrder));
        final->num=numCustomers;
        close(fd[i][READ]);
        shmPtr[i] = *final;
    }

    wait(NULL);

    if (shmdt(shmPtr) == -1) {
        perror("Error in shmdt in detaching the memory segment\n");
        return 1;
    }
        // Mark the shared memory segment for deletion (A control operation)
        //if (shmctl(shmid, IPC_RMID, 0) == -1) // don't delete before reading is done
        //{
        //    perror("Error in shmctl\n");
        //    return 1;
        //}
    
    
    
    key_t keyWaitEnd;
    system("touch msgq.txt");
    if ((keyWaitEnd = ftok("msgq.txt", 'a'+tableNumber)) == -1) {
        perror("Error in ftok\n");
        return 1;
    }
	
	int shmidWaitEnd;
	int *loopflagWaitEnd;
   loopflagWaitEnd = (int *)malloc(sizeof(int));
   *loopflagWaitEnd = 0;
   //int *loopflag;
   *loopflagWaitEnd = 0;
   shmidWaitEnd = shmget(keyWaitEnd, BUF_SIZE, 0644 | IPC_CREAT);
   if (shmidWaitEnd == -1) {
        perror("Error in shmget in creating/accessing shared memory\n");
        return 1;
    }
    loopflagWaitEnd = shmat(shmidWaitEnd, NULL , 0);
    if (loopflagWaitEnd == (void *) -1) {
        perror("Error in shmPtr in attaching the memory segment\n");
        return 1;
    }
    
   
    
    *loopflagWaitEnd = 1;
    
    if (shmdt(loopflagWaitEnd) == -1) {
        perror("Error in shmdt in detaching the memory segment\n");
        return 1;
    }
    
    
    
    key_t key3;
    system("touch msgq.txt");
    if ((key3 = ftok("msgq.txt", 'A'+tableNumber)) == -1) {
        perror("Error in ftok\n");
        return 1;
    }
    
	
    int shmid3;
	    	    
    shmid3 = shmget(key3, BUF_SIZE, 0644 | IPC_CREAT);
    
    printf("Waiting for Waiter...\n");
    
    
    
    int* loopflag;
    loopflag = (int *)malloc(sizeof(int));
    *loopflag = 0;
    //key_t flagkey;
    loopflag = shmat(shmid3, NULL , 0);
	    if (loopflag == (void *) -1) {
		perror("Error in shmPtr in attaching the memory segment\n");
		return 1;
	    }
    //if (shmdt(loopflag) == -1) {
	//perror("Error in shmdt in detaching the memory segment\n");
	//return 1;
    	//}
    	
    	
    	
    	
    	while(*loopflag==0)
    {
    	shmid3 = shmget(key3, BUF_SIZE, 0644 | IPC_CREAT);
    	if (shmid3 == -1) {
        	perror("Error in shmget in creating/accessing shared memory\n");
        	return 1;
        }
    	loopflag = shmat(shmid3, NULL , 0);
	    if (loopflag == (void *) -1) {
		perror("Error in shmPtr in attaching the memory segment\n");
		return 1;
	    }
    	sleep(1);
    }
    
    //*loopflag = 0;
    
    //while(1){
    
    	
//	if(*loopflag)    break;
   // }
   
    if (shmdt(loopflag) == -1) {
        perror("Error in shmdt in detaching the memory segment\n");
        return 1;
    }
    
    system("touch msgq.txt");

    int sum = 0;

    key_t waiterKey;
    int waiterShmIds[MAX_TABLES];
    waiterKey = ftok("msgq.txt", 11);

    for (int i = 0; i < tableNumber; i++) { 
        if (waiterKey == -1) {
            perror("Error in ftok");
            return 1;
        }
        waiterShmIds[i] = shmget(waiterKey, BUF_SIZE, 0644);
        if (waiterShmIds[i] == -1) {
            perror("1");
            return 1;
        }
    }

    int waiterShmId = shmget(waiterKey, BUF_SIZE, 0644 | IPC_CREAT);

    
    billamount *final2 = shmat(waiterShmId, NULL, 0);


    if (final2 == (void *) -1) {
        perror("2");
        return 1;
    }
	



    for (int i = 0; i < tableNumber; i++) {
       
      sum = final2->amount[i];
       
    }

    shmdt(final2);
	
	
    
    	
    printf("The total bill amount is %d INR.\n", sum);

    return 0;
}
