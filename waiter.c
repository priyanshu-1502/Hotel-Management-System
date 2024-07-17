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
#define MAX_ORDER_ITEMS 10
#define MENU_FILE "menu.txt"
#define BUF_SIZE (5 * 10 * sizeof(int))
#define MAX_TABLES 10

typedef struct{
    int num;
    int order[10];
} customerOrder;

typedef struct{
    int no;
    int amount[10];
    int finish;
} billamount;;


int main(){

/***
	key_t key4;
    system("touch msgq.txt");
    if ((key4 = ftok("msgq.txt", 12)) == -1) {
        perror("Error in ftok\n");
        return 1;
    }
	
   int shmid4;
   int *loopflag;
   loopflag = (int *)malloc(sizeof(int));
   *loopflag = 0;
   shmid4 = shmget(key4, BUF_SIZE, 0644 | IPC_CREAT);
   if (shmid4 == -1) {
        perror("Error in shmget in creating/accessing shared memory\n");
        return 1;
    }
    loopflag = shmat(shmid4, NULL , 0);
    if (loopflag == (void *) -1) {
        perror("Error in shmPtr in attaching the memory segment\n");
        return 1;
        
        *loopflag = 0;
        
        if (shmdt(loopflag) == -1) {
        perror("Error in shmdt in detaching the memory segment\n");
        return 1;
    }
    }
    **/
   
    
    
    
    
    
        int MAX_ITEMS=0;
    printf("Enter Waiter ID:");
    int waiterID;  
    scanf("%d",&waiterID);
    
    if(waiterID<=0||waiterID>10)
    {
        printf("Waiter ID can be between 1 and 10. Exiting");
        exit(EXIT_FAILURE);
       
    }
    FILE *menuFile = fopen(MENU_FILE, "r");
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
    
    
    key_t keyWait;
    system("touch msgq.txt");
    if ((keyWait = ftok("msgq.txt", 'a'+waiterID)) == -1) {
        perror("Error in ftok\n");
        return 1;
    }
    
	
    int shmidWait;
	    	    
    shmidWait = shmget(keyWait, BUF_SIZE, 0644 | IPC_CREAT);
    
    printf("Waiting for Orders from customers...\n");
    
    
    
    int* loopflagWait;
    loopflagWait = (int *)malloc(sizeof(int));
    *loopflagWait = 0;
    //key_t flagkey;
    loopflagWait = shmat(shmidWait, NULL , 0);
	    if (loopflagWait == (void *) -1) {
		perror("Error in shmPtr in attaching the memory segment\n");
		return 1;
	    }
    //if (shmdt(loopflag) == -1) {
	//perror("Error in shmdt in detaching the memory segment\n");
	//return 1;
    	//}
    	
    	
    	
    	
    	while(*loopflagWait==0)
    {
    	shmidWait = shmget(keyWait, BUF_SIZE, 0644 | IPC_CREAT);
    	if (shmidWait == -1) {
        	perror("Error in shmget in creating/accessing shared memory\n");
        	return 1;
        }
    	loopflagWait = shmat(shmidWait, NULL , 0);
	    if (loopflagWait == (void *) -1) {
		perror("Error in shmPtr in attaching the memory segment\n");
		return 1;
	    }
    	sleep(1);
    }
    
    //*loopflag = 0;
    
    //while(1){
    
    	
//	if(*loopflag)    break;
   // }
   
    if (shmdt(loopflagWait) == -1) {
        perror("Error in shmdt in detaching the memory segment\n");
        return 1;
    }
    
    
    key_t key;
    if ((key = ftok("msgq.txt", waiterID)) == -1) {
        perror("Error in ftok\n");
        return 1;
    }
    int shmid;
    customerOrder *shmPtr;
    shmid = shmget(key, BUF_SIZE, 0644);
    if (shmid == -1) {
        perror("Error in shmget in creating/accessing shared memory\n");
        return 1;
    }

    shmPtr = shmat(shmid, NULL, 0);
    if (shmPtr == (void *) -1) {
        perror("Error in shmPtr in attaching the memory segment\n");
        return 1;
    }
    int n = shmPtr[0].num;  
    int sum = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 10; j++) {
            int serialNumber = shmPtr[i].order[j];
        if (serialNumber == -1)
            break;

        // Check if the serial number is valid
        if (serialNumber >= 1 && serialNumber <= MAX_ITEMS) {
            sum += costArray[serialNumber - 1];
        } else {
            fprintf(stderr, "Invalid serial number in order for customer %d\n", i + 1);
            return 1;
        }
    }
    }


    

    // printf("The total bill amount is %d INR.\n", sum);

    if (shmdt(shmPtr) == -1) {
        perror("Error in shmdt in detaching the memory segment\n");
        return 1;
    }

    key_t key2;
    system("touch msgq.txt");
    if ((key2 = ftok("msgq.txt", 11)) == -1) {
        perror("Error in ftok\n");
        return 1;
    }

    int shmid2;
    billamount *shmPtr2;
    shmid2 = shmget(key2, BUF_SIZE, 0644 | IPC_CREAT);
    if (shmid2 == -1) {
        perror("Error in shmget in creating/accessing shared memory\n");
        return 1;
    }

   
    shmPtr2 = shmat(shmid2, NULL , 0);
    if (shmPtr2 == (void *) -1) {
        perror("Error in shmPtr in attaching the memory segment\n");
        return 1;
    }



    shmPtr2->amount[waiterID - 1] = sum;
    shmPtr2->no = waiterID;

   

    if (shmdt(shmPtr2) == -1) {
        perror("Error in shmdt in detaching the memory segment\n");
        return 1;
    }
    
    
    
    
    
    key_t key3;
    system("touch msgq.txt");
    if ((key3 = ftok("msgq.txt", 'A'+waiterID)) == -1) {
        perror("Error in ftok\n");
        return 1;
    }
	
	int shmid3;
	int *loopflag;
   loopflag = (int *)malloc(sizeof(int));
   *loopflag = 0;
   //int *loopflag;
   *loopflag = 0;
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
    
   
    
    *loopflag = 1;
    
    if (shmdt(loopflag) == -1) {
        perror("Error in shmdt in detaching the memory segment\n");
        return 1;
    }
    
    printf("Serving table no. %d",waiterID);
    
    return 0;
}
