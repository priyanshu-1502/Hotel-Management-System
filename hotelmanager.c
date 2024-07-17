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
#define EARN "earnings.txt"
#define BUF_SIZE (5 * 10 * sizeof(int))
#define MAX_TABLES 10

typedef struct{
    int num;
    int order[10];
} customerOrder;

typedef struct{
    int no;
    int amount[10];
} billamount;

int main(){
   
    //int MAX_ITEMS=0;
   
    



   
    printf("Enter the Total Number of Tables at the Hotel:");
    int totalTables;  
    scanf("%d",&totalTables);

    FILE *earn =  fopen(EARN, "w");
    if (earn == NULL) {
        perror("Error opening menu file");
        exit(EXIT_FAILURE);
    }
    
    key_t key;
    int shmid;
    int* exitflag;
    exitflag = (int *)malloc(sizeof(int));
    *exitflag = 0;
    
    if ((key = ftok("msgq.txt", 'z')) == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    
    if ((shmid = shmget(key, BUF_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    

    printf("Waiting for Admin...\n");
    
    while(*exitflag==0)
    {
    	shmid = shmget(key, BUF_SIZE, 0644 | IPC_CREAT);
    	if (shmid == -1) {
        	perror("Error in shmget in creating/accessing shared memory\n");
        	return 1;
        }
    	exitflag = shmat(shmid, NULL , 0);
	    if (exitflag == (void *) -1) {
		perror("shmat");
		exit(EXIT_FAILURE);
    		}
    	sleep(1);
    }
    
    if (shmdt(exitflag) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    
    key_t key3;
    system("touch msgq.txt");
    if ((key3 = ftok("msgq.txt", 'A'+totalTables)) == -1) {
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
    
   
    if (shmdt(loopflag) == -1) {
        perror("Error in shmdt in detaching the memory segment\n");
        return 1;
    }
    
    int earnings[MAX_TABLES] = {0};
   
   
    system("touch msgq.txt");

    key_t waiterKey;
    int waiterShmIds[MAX_TABLES];
    waiterKey = ftok("msgq.txt", 11);

    for (int i = 0; i < totalTables; i++) {
       
        if (waiterKey == -1) {
            perror("Error in ftok");
            return 1;
        }
        waiterShmIds[i] = shmget(waiterKey, BUF_SIZE, 0644);
        if (waiterShmIds[i] == -1) {
            perror("Error in shmget in creating/accessing shared memory");
            return 1;
        }
    }

    //int waiterShmIds[MAX_TABLES];
    // for (int i = 0; i < totalTables; i++) {
    //     waiterShmIds[i] = shmget(waiterKeys[i], BUF_SIZE, 0644);
    //     if (waiterShmIds[i] == -1) {
    //         perror("Error in shmget in creating/accessing shared memory");
    //         return 1;
    //     }
    // }

int waiterShmId = shmget(waiterKey, BUF_SIZE, 0644 | IPC_CREAT);



billamount *final = shmat(waiterShmId, NULL, 0);


      if (final == (void *) -1) {
            perror("Error in shmat in attaching the memory segment");
            return 1;
        }



    for (int i = 0; i < totalTables; i++) {
       
       //printf("%d",final->amount[i]);
       
       earnings[i] = final->amount[i];
       
    }

shmdt(final);

    for (int i = 0; i < totalTables; i++) {
        fprintf(earn, "Earning from Table %d: %d INR\n", i + 1, earnings[i]);
    }

    fclose(earn);



    int totalEarnings = 0;
    for (int i = 0; i < totalTables; i++) {
        totalEarnings += earnings[i];
    }
    int totalWages = totalEarnings * 0.4;
    int totalProfit = totalEarnings - totalWages;

    printf("Total Earnings of Hotel: %d INR\n", totalEarnings);
    printf("Total Wages of Waiters: %d INR\n", totalWages);
    printf("Total Profit: %d INR\n", totalProfit);

    earn =  fopen(EARN, "w");
    if (earn == NULL) {
        perror("Error opening menu file");
        exit(EXIT_FAILURE);
    }
		
    fprintf(earn, "Total Earnings of Hotel: %d INR\n", totalEarnings);
    fprintf(earn, "Total Wages of Waiters: %d INR\n", totalWages);
    fprintf(earn, "Total Profit: %d INR\n", totalProfit);

    fclose(earn);

    printf("Thank you for visiting the Hotel!\n");

    key_t kWaiter[MAX_TABLES],kSum,kWait[MAX_TABLES],kWait2[MAX_TABLES],kAdmin;
    
    int shmidWaiter[MAX_TABLES];   
    int shmidWait[MAX_TABLES];
    int shmidWait2[MAX_TABLES];
    
    for(int i1 = 0;i1 < totalTables;i1++){
    	if ((kWaiter[i1] = ftok("msgq.txt", i1)) == -1) {
        	perror("Error in ftok\n");
        	return 1;
    	}
    	shmidWaiter[i1] = shmget(kWaiter[i1], BUF_SIZE, 0644 | IPC_CREAT);
    	if(shmctl(shmidWaiter[i1], IPC_RMID, 0) == -1) {
    	printf("1Error while deleting memory.");
    	return 1;
    	}
    }
    
    for(int j1 = 0;j1 < totalTables;j1++){
    	if ((kWait[j1] = ftok("msgq.txt",'A'+j1)) == -1) {
        perror("Error in ftok\n");
        return 1;
    	}
    	if ((kWait2[j1] = ftok("msgq.txt",'a'+j1)) == -1) {
        perror("Error in ftok\n");
        return 1;
    	}
    	shmidWait[j1] = shmget(kWait[j1], BUF_SIZE, 0644 | IPC_CREAT);
    	shmidWait2[j1] = shmget(kWait2[j1], BUF_SIZE, 0644 | IPC_CREAT);
    	
    	if(shmctl(shmidWait[j1], IPC_RMID, 0) == -1) {
    	printf("2Error while deleting memory.");
    	return 1;
    	}
    	if(shmctl(shmidWait2[j1], IPC_RMID, 0) == -1) {
    	printf("3Error while deleting memory.");
    	return 1;
    	}
    	
    }
    
    if ((kSum = ftok("msgq.txt", 11)) == -1) {
        	perror("Error in ftok\n");
        	return 1;
    	}
    
     if ((kAdmin = ftok("msgq.txt", 'z')) == -1) {
        	perror("Error in ftok\n");
        	return 1;
    	}
    
    
    int shmidSum = shmget(kSum, BUF_SIZE, 0644 | IPC_CREAT);
    int shmidAdmin = shmget(kAdmin, BUF_SIZE, 0644 | IPC_CREAT);
    
    if(shmctl(shmidSum, IPC_RMID, 0) == -1) {
    	printf("4Error while deleting memory.");
    	return 1;
    	}
    if(shmctl(shmidAdmin, IPC_RMID, 0) == -1) {
    	printf("5Error while deleting memory.");
    	return 1;
    	}
   
    //system("rm msgq.txt");
    //free(loopflag);
    //free(exitflag);
    
    
    //memset(loopflag,0,sizeof(int));
    
    
   printf("Shared memory deleted...");

    return 0;
}
