#include "function.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>


volatile sig_atomic_t sigusr2_received = 0;
volatile sig_atomic_t sigchild_received = 0;

void sigusr2_handler(int signum) {
    
    sigusr2_received = 1;               //signal handler
}

void sigchld_handler(int signum) {
    
    sigchild_received = 1;              //signal handler
}


Queue* initializeQueue() {

    Queue* new_queue = (Queue*)malloc(sizeof(Queue));
    
    if (new_queue == NULL) {
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    new_queue->concurrency = 1;                 //initialization of the awaiting queue
    new_queue->number_of_jobs = 0;
    new_queue->front = NULL;
    new_queue->rear = NULL;

    return new_queue;
}

int isEmpty(Queue* queue) {

    if (queue->front == NULL){
        return 1;                               //checking if the awaiting queue is empty
    }
    else{
        return 0;
    }
    
}

JOBS* createNode(int queue_position,char *jobID,char *job,char *tripleta,JOBS* last) {

    JOBS* new = (JOBS*)malloc(sizeof(JOBS));
    

    if (new == NULL) {
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
   
    strcpy(new->tripleta, tripleta);                    //creating a new node for the waiting queue
    strcpy(new->jobID, jobID);
    strcpy(new->job, job);
    new->queue_position = queue_position;
    new->ahead_of = NULL;
    new->behind_of = last;
    new->active=0;
    return new;
}

void enqueue(Queue* queue, int queue_position,char *jobID,char *job,char *tripleta) {

    JOBS* new = createNode(queue_position,jobID,job,tripleta,queue->rear);
    if (isEmpty(queue)) {
        queue->front = new;
        queue->rear = new;
        queue->number_of_jobs = 1;
    } else {                                            //adding a new node to the waiting queue
        queue->rear->ahead_of = new;
        queue->rear = new;
        queue->number_of_jobs++;
    }
}

void dequeue(Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty.\n");
        return; 
    }

    JOBS* temp = queue->front;
    JOBS* temp2 = queue->front;

    while (temp != NULL) {
        temp->queue_position--;
        temp = temp->ahead_of;                      //deleting the first node of the awaiting queue while changing the queue position of the other nodes and the total number of jobs variable in the queue
    }

    queue->front = queue->front->ahead_of;

    if (queue->front == NULL) {
        queue->rear = NULL;
    } else {
        queue->front->behind_of = NULL;
    }
    queue->number_of_jobs--;

    free(temp2); 
}



char* get_command(Queue* queue) {
    JOBS* temp = queue->front;
    if (temp == NULL) {
        return NULL; 
    }

    char* job = (char*)malloc(strlen(temp->job) + 1);
    if (job == NULL) {
        printf("Memory allocation failed for job string\n");                //getting the command from the first node of the awaiting queue
        return NULL;
    }

    strcpy(job, temp->job);

    return job;
}

char* get_jobID(Queue* queue) {
    JOBS* temp = queue->front;
    if (temp == NULL) {
        return NULL; 
    }

    char* jobID = (char*)malloc(strlen(temp->jobID) + 1);                   //getting the jobID from the first node of the awaiting queue
    if (jobID == NULL) {
        printf("Memory allocation failed for job string\n");
        return NULL;
    }

    strcpy(jobID, temp->jobID);

    return jobID;
}



int get_concurrency(Queue* queue){
    return queue->concurrency;
}

void printQueue(Queue* queue) {

    JOBS* node = queue->front;
    
    while (node != NULL) {
        printf("%s \n",node->tripleta);
        node = node->ahead_of;
    }


}

void freeQueue(Queue* queue) {

    while (isEmpty(queue) == 0) {
        dequeue(queue);
    }
    free(queue);
}

int number_of_active_commands(active_Queue* A) {
    
    int i,j;
    j=0;
    i=0;
    active_JOBS* temp2 = A->front;
    while(j==0){
        
        if(temp2 == NULL){ 
            break;
        }                                                           //returning the number of commands that are active at the moment
        else if(temp2->active == 1){
            i++; 
        }

        if(temp2->ahead_of == NULL){
            break; 
        }
        else{    
            temp2=temp2->ahead_of;
        }
    }
    
    return i;
}

        
        
void poll_queued(int fd2,char *message_2,Queue *Q){

    const char sub5[] = "poll queued";
    char message[BUFFER_SIZE];
    strcpy(message,message_2);


    
    if ((strstr(message, sub5) != NULL)&&(message[0] == 'p')) {         //if the words poll queued are part of the command that was given by the jobcommander and the command starts with p (thats to avoid commands like stop poll running or issuejob poll queued)         
                    
        char final_message[BUFFER_SIZE]="DONE";
        int bytesWritten;
        int k,l,b;
        k=0;
        JOBS* temp2 = Q->front;
        if(isEmpty(Q) == 0){ //if queue not empty then we start sending the tripletes of every node in the awaiting queue
            while(k==0 ){
                
                if(temp2 == NULL){
                    
                    k=1;
                    bytesWritten = write(fd2, final_message,  strlen(final_message) + 1);
                    if (bytesWritten == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    }  

                    b=0;
                    while (b==0) {
                        if (sigusr2_received) {
                            b=1;
                            sigusr2_received = 0; 
                        }
                            
                    }                      
                }
                else{                       
                                              
                    bytesWritten = write(fd2, temp2->tripleta, strlen(temp2->tripleta) + 1);
                    if (bytesWritten == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    } 
                    b=0;
                    while (b==0) {
                        if (sigusr2_received) {
                            
                            b=1;
                            sigusr2_received = 0;                   //awaiting the signal sigurs2 from jobCommander
                        }                                
                    }
                    
                    temp2=temp2->ahead_of;
                }
            }
        }
        else{      //if queue empty then we send the message done to the commander and await a signal from him to know that he got it.
                                                                                  
            bytesWritten = write(fd2, final_message,  strlen(final_message) + 1);
            if (bytesWritten == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }  
            b=0;
            while (b==0) {
                if (sigusr2_received) {
                    
                    b=1;
                    sigusr2_received = 0;                           //awaiting the signal sigurs2 from jobCommander
                }
            }              
        }
    }
}


void set_concurrency(char *message_2,Queue *Q){

    char message[BUFFER_SIZE];
    strcpy(message,message_2);
    const char sub2[] = "setConcurrency"; 
    int len = strlen(sub2);
    char *p = strstr(message, sub2);


    if ((p != NULL)&&(message[0] == 's')) {                       //if the word setConcurrency is part of the command that was given by the jobcommander
        
        memmove(p, p + len, strlen(p + len) + 1);
        Q->concurrency=atoi(message);               //changing the value of the concurrency in the queue
        
    }
}

int exit_command(char *message,int fd2){

    char file[]="jobExecutorServer.txt";
    char final[BUFFER_SIZE]="jobExecutorServer terminated";
    const char sub3[] = "exit";
    int b=0;


    if (strstr(message, sub3)!=NULL) {                       //if the word exit is part of the command that was given by the jobcommander
        

        
        if (remove(file) == 0) {                            //deleting jobExecutorServer.txt
            
        } else {
            perror("Error deleting file");  
        }
        int bytesWritten = write(fd2, final, strlen(final) + 1);      //sending the jobexecutorserver terminated message to the commander
        if (bytesWritten == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }  
        b=0;
        while (b==0) {
            if (sigusr2_received) {
                
                b=1;
                sigusr2_received = 0;                           //awaiting the signal sigurs2 from jobCommander
            }
        }  
        exit(EXIT_SUCCESS);
        
    }
    else{
        return 0;
    }
}



void poll_running(int fd2,char *message_2,active_Queue* A){                     //if the words poll running are part of the command that was given by the jobcommander and the command starts with p (thats to avoid commands like stop poll running or issuejob poll queued)   
//this function works almost identically with the poll queued one the only difference is that this one searches the active_JOBS struct and the other searches the JOBS struct
    const char sub8[] = "poll running";
    char message[BUFFER_SIZE];
    strcpy(message,message_2);

    
    if ((strstr(message, sub8) != NULL)&&(message[0] == 'p')) {                       
                    
        char final_message[BUFFER_SIZE]="DONE";
        int bytesWritten;


        int k,l,b;
        k=0;
        active_JOBS* temp2 = A->front;
        
        if(isEmpty_active(A) == 0){                         //if the active queue is empty
            while(k==0 ){
                
                if(temp2 == NULL){
                    
                    k=1;
                    bytesWritten = write(fd2, final_message,  strlen(final_message) + 1);
                    if (bytesWritten == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    }  

                    b=0;
                    while (b==0) {
                        if (sigusr2_received) {
                            
                            b=1;
                            sigusr2_received = 0;               //awaiting the signal sigurs2 from jobCommander
                        }
                            
                    }                      
                }
                else{                       
                                             
                    bytesWritten = write(fd2, temp2->tripleta, strlen(temp2->tripleta) + 1);                //writing the tripleta in the pipe
                    
                    if (bytesWritten == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    } 
                    b=0;
                    while (b==0) {
                        
                        if (sigusr2_received) {
                            
                            b=1;
                            sigusr2_received = 0;                   //awaiting the signal sigurs2 from jobCommander
                        }                                
                    }
                
                    temp2=temp2->ahead_of;
                    
                }
            }
        }
        else{
            bytesWritten = write(fd2, final_message,  strlen(final_message) + 1);           //writing the message that lets the jobcommander know that there no other active
            if (bytesWritten == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }  
            b=0;
            while (b==0) {
                if (sigusr2_received) {
                    
                    b=1;
                    sigusr2_received = 0;                           //awaiting the signal sigurs2 from jobCommander
                }
            }              
        }
    }
}



int issueJob(Queue* Q,active_Queue* A,char *message_2,int jobCounter,int fd2){



    char message[BUFFER_SIZE];
    strcpy(message,message_2);
    const char sub[] = "issueJob ";
    int len = strlen(sub);
    char* p = strstr(message, sub);
    int b;

    
    
    if ((p != NULL)&&(message[0]=='i')) {                          //if the word issueJob is part of the command that was given by the jobcommander
        
        jobCounter++;

        
        memmove(p, p + len, strlen(p + len) + 1);  // Shift characters to the left so that i  can get rid of the "issuejob" part of the message string

        char *args[200];  // Maximum number of arguments
        
        int numArgs = 0;
        
                                                            //creation of the args array that contains every word of the command
        char *token = strtok(message, " ");
        while (token != NULL && numArgs < 199) {
            args[numArgs++] = token;
            token = strtok(NULL, " ");
        }
        args[numArgs] = NULL;  

        
        char jobID[BUFFER_SIZE];                            //creation of the necessary arrays to store the information of the tripleta
        memset(jobID, 0, sizeof(jobID));
        char queue_position[20];
        memset(queue_position, 0, sizeof(queue_position));
        char job[BUFFER_SIZE];
        memset(job, 0, sizeof(job));
        
        sprintf(jobID, "job_%d", jobCounter);

        char tripleta[BUFFER_SIZE];
        memset(tripleta, 0, sizeof(tripleta));

        strcat(tripleta, jobID);                            //first we store the jobID in the tripleta
        strcat(tripleta, ",");

                                                                                                    
        for (int i = 0; args[i] != NULL; i++) {

            strcat(job, args[i]); 
            strcat(tripleta, args[i]);                      //then we store the job in the tripleta
            if (args[i+1] !=NULL){
                strcat(tripleta, " "); 
                strcat(job, " "); 
            }

        }
        
        strcat(tripleta, ",");   
        sprintf(queue_position, "%d", jobCounter);          //finally we store the queue position in tripleta
        strcat(tripleta, queue_position); 


        
                                                                                      
        int bytesWritten = write(fd2, tripleta, strlen(tripleta) + 1);          //now we write the tripleta to the jobcommander and also add the command to the queue and wait for a signal to 
        if (bytesWritten == -1) {                                               //know that it was received from the jobcommander
            perror("read");
            exit(EXIT_FAILURE);
        }


        b=0;
        while (b==0) {
            if (sigusr2_received) {
                
                b=1;
                sigusr2_received = 0;                       //awaiting the signal sigurs2 from jobCommander
            }
            
        }


        
        enqueue(Q,jobCounter,jobID,job,tripleta);

        
    }


    //after we save the command given by jobCommander we do a routine check to see if we can start any additional commands
    int remaining_commands;
    int concurrency = get_concurrency(Q);
    int active_commands = number_of_active_commands(A);



    if((concurrency>active_commands) && (isEmpty(Q) == 0 )){
        remaining_commands = concurrency - active_commands;
        while((remaining_commands != 0) && (isEmpty(Q) == 0)) {

        
            char *a;
            char b[BUFFER_SIZE];
            char *c;
            char d[BUFFER_SIZE];
            char e[BUFFER_SIZE];
            char f[BUFFER_SIZE];
            a=get_command(Q);                       //gives us the command of the first node in the wait queue
            c=get_jobID(Q);                         //gives us the jobID of the first node in the wait queue
            
            strcpy(d,a);
            strcpy(f,a);                            //creating duplicates to make sure we dont affect the memory of the original strings
            strcpy(e,c);
            char *args2[200];  
            int numArgs2 = 0;
                                                                                 //creation of the args2 array that contains every word of the command like in the beginning of the issuejob function where we created args array

            char *token2 = strtok(d, " ");
            while (token2 != NULL && numArgs2 < 199) {
                args2[numArgs2++] = token2;
                token2 = strtok(NULL, " ");
            }
            args2[numArgs2] = NULL;  


                                                                                                        //creation of the necessary arrays to store the information of the tripleta2
            char queue_position2[20];
            memset(queue_position2, 0, sizeof(queue_position2));
            char tripleta2[BUFFER_SIZE];
            memset(tripleta2, 0, sizeof(tripleta2));

            strcat(tripleta2, e); 
            strcat(tripleta2, ",");
                                                                                                        
            for (int i = 0; args2[i] != NULL; i++) {

                
                strcat(tripleta2, args2[i]); 
                if (args2[i+1] !=NULL){
                    strcat(tripleta2, " ");                                                 //creation of tripleta2
                    
                }

            }
            strcat(tripleta2, ",");   
            sprintf(queue_position2, "%d", A->number_of_active_JOBS +1);
            strcat(tripleta2, queue_position2); 


            


            pid_t pid = fork();

            if (pid == 0) {
                // Child process
                execvp(args2[0], args2);
                perror("exec");                                                    //procedure to execute the command that is stored in args2 array
                exit(EXIT_FAILURE);
            } else if (pid > 0) {
                // Parent process
                
                int status;
                pid_t result;
                
                result = waitpid(pid, &status, WNOHANG);                            //checking whether the command has terminated
                if (result == -1) {
                    perror("waitpid");
                    exit(EXIT_FAILURE);
                } else if (result > 0) {
                    // Child process with PID result has exited
                    if (WIFEXITED(status)) {    
                    } 
                }
                else if (result == 0) {
                    
                    enactive_Queue(A,A->number_of_active_JOBS +1,e,d,tripleta2,pid);                  //add the tripleta to the active commands struct
                    
                }
                
            } else {
                perror("fork"); 
            }
            
            dequeue(Q);                     //delete the first node of the await queue since it has gone in the active command struct
            
            free(a);                        //freeing the results of get_command and get_jobid functions
            free(c);
            remaining_commands--;
        }
        
    }
    

    return jobCounter;
}


active_Queue* initializeactive_Queue() {

    active_Queue* new_active_Queue = (active_Queue*)malloc(sizeof(active_Queue));
    
    if (new_active_Queue == NULL) {
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);                                             //initialization of the struct for active commands
    }
    new_active_Queue->concurrency = 1;
    new_active_Queue->number_of_active_JOBS = 0;
    new_active_Queue->front = NULL;
    new_active_Queue->rear = NULL;

    return new_active_Queue;
}

int isEmpty_active(active_Queue* active_Queue) {

    if (active_Queue->front == NULL){
        return 1;
    }                                                                   //checking whether its empty
    else{
        return 0;
    }
    
}

active_JOBS* createNode_active(int active_Queue_position,char *jobID,char *job,char *tripleta,active_JOBS* last,pid_t pid) {

    active_JOBS* new = (active_JOBS*)malloc(sizeof(active_JOBS));
    

    if (new == NULL) {
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
   
    strcpy(new->tripleta, tripleta);                                    //creating a new node for the active commands struct
    strcpy(new->jobID, jobID);
    strcpy(new->job, job);
    new->active_Queue_position = active_Queue_position;
    new->ahead_of = NULL;
    new->behind_of = last;
    new->active=1;
    new->pid=pid;
    return new;
}

void enactive_Queue(active_Queue* active_Queue, int active_Queue_position,char *jobID,char *job,char *tripleta,pid_t pid) {

    active_JOBS* new = createNode_active(active_Queue_position,jobID,job,tripleta,active_Queue->rear,pid);
    if (isEmpty_active(active_Queue)) {
        active_Queue->front = new;
        active_Queue->rear = new;
        active_Queue->number_of_active_JOBS = 1;
    } else {                                                        //adding a new node to the active commands struct
        active_Queue->rear->ahead_of = new;
        active_Queue->rear = new;
        active_Queue->number_of_active_JOBS++;
    }

}

void active_deQueue(active_Queue* active_Queue) {                   //this function is responsible for deleting commands from the active queue struct
    if (isEmpty_active(active_Queue)) {
        
        return; 
    }
    
    active_JOBS* temp = active_Queue->front;
    active_JOBS* temp2 = active_Queue->front;

    while (temp != NULL) {

        int status;
        pid_t result = waitpid(temp->pid, &status, WNOHANG);
        if (result == -1) {
            
        } else if (result > 0) {
            // Child process with PID result has exited
            int fin;
            fin = WEXITSTATUS(status);


            if((temp == active_Queue->front) && (active_Queue->front->ahead_of !=NULL)){                        //making sure that no matter the position of the deleted command the connections between the remaining active commands remain intact
                active_Queue->front = active_Queue->front->ahead_of;                                            //this part is in case some of the commands finish their execution
                
            }

            if((temp == active_Queue->rear)  && (active_Queue->rear->behind_of !=NULL))  {
                active_Queue->rear = active_Queue->rear->behind_of;
                
            }

            
            if (active_Queue->front == NULL) {
                active_Queue->rear = NULL;
            } else {
                active_Queue->front->behind_of = NULL;
            }

            if((temp == active_Queue->front) && (temp == active_Queue->rear)){
                active_Queue->front = NULL;
                active_Queue->rear = NULL;
                
            }

            

            if(temp->ahead_of !=NULL){
                temp->ahead_of->behind_of=temp->behind_of;
            }
            if(temp->behind_of !=NULL){
                temp->behind_of->ahead_of=temp->ahead_of;
            }   
            active_Queue->number_of_active_JOBS--;
            free(temp);   
                
            break;
        }

        temp = temp->ahead_of;
    }

    while (temp2 != NULL) {
        
        if(temp2->active==0){                                                       //like before we are making sure that no matter the position of the deleted command the connections between the remaining active commands remain intact
                                                                                    //this part is in case the stop command has stopped one of the active commands

            if((temp2 == active_Queue->front) && (active_Queue->front->ahead_of !=NULL)){
                active_Queue->front = active_Queue->front->ahead_of;
                
            }

            if((temp2 == active_Queue->rear)  && (active_Queue->rear->behind_of !=NULL))  {
                active_Queue->rear = active_Queue->rear->behind_of;
                
            }

            
            if (active_Queue->front == NULL) {
                active_Queue->rear = NULL;
            } else {
                active_Queue->front->behind_of = NULL;
            }

            if((temp2 == active_Queue->front) && (temp2 == active_Queue->rear)){
                active_Queue->front = NULL;
                active_Queue->rear = NULL;
                
            }

             

            if(temp2->ahead_of !=NULL){
                temp2->ahead_of->behind_of=temp2->behind_of;
            }
            if(temp2->behind_of !=NULL){
                temp2->behind_of->ahead_of=temp2->ahead_of;
            }   
             
            free(temp2); 
                  
            break;            
        }

        
        temp2 = temp2->ahead_of;
    }



}

void printactive_Queue(active_Queue* active_Queue) {

    active_JOBS* node = active_Queue->front;
    

    if (isEmpty_active(active_Queue)) {
        
        return; 
    }                                                                   //printing the contents of the active struct
    while (node != NULL) {
        printf("%s,pid %d=\n",node->tripleta,node->pid);
        if(node->ahead_of ==NULL){
            break;
        }
        node = node->ahead_of;
    }
    printf("\n");

}

void freeactive_Queue(active_Queue* active_Queue) {

    
    while (isEmpty_active(active_Queue) == 0) {
        active_deQueue(active_Queue);                                   //freeing the memory of the active struct
    }
    free(active_Queue);
}




void Stop(Queue* Q,active_Queue *A,char *message_2,int fd2){

    char message[BUFFER_SIZE];
    char commander_message[BUFFER_SIZE];
    char final_message[BUFFER_SIZE]="DONE";
    strcpy(message,message_2);
    const char sub[] = "stop ";
    int len = strlen(sub);
    char* p = strstr(message, sub);
    int b;
    int x;
    int bytesWritten;
    strcpy(commander_message,"");
    
    if (p != NULL) {                          




        active_JOBS* temp = A->front;
        while (temp != NULL) {
            strcpy(message,message_2);
            char *args[3];  
            char jobID[BUFFER_SIZE];
            int numArgs = 0;
            memset(jobID, 0, sizeof(jobID));                                                 //creation of the args array that contains every word of the command
            char *token = strtok(message, " ");
            while (token != NULL && numArgs < 2) {
                args[numArgs++] = token;
                token = strtok(NULL, " ");
            }
            args[numArgs] = NULL;  
            strcat(jobID, args[1]); 

            if (strstr( temp->jobID , jobID)  != NULL){                                      //checking if the job_id matches with the one we are looking for
                
                if (kill(temp->pid, SIGTERM) == 0) {                                        //sending the sigterm signal to terminate the process
                    int status;
                    if (waitpid(temp->pid, &status, 0) != -1) {
                        if (WIFEXITED(status)) {
                            int exit_status = WEXITSTATUS(status);
                        } else if (WIFSIGNALED(status)) {
                            int term_signal = WTERMSIG(status);    
                        } 
                    } else {
                        perror("waitpid");
                    }

                    strcat(commander_message, temp->jobID); 
                    strcat(commander_message, " "); 
                    strcat(commander_message, "terminated"); 

                    bytesWritten = write(fd2, commander_message, strlen(commander_message) + 1);        //sending the job_xx terminated message to jobCommander

                    if (bytesWritten == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    } 
                    b=0;
                    while (b==0) {
                        
                        if (sigusr2_received) {
                            
                            b=1;
                            sigusr2_received = 0;                                           //awaiting for the sigusr2 signal
                        }                                
                    }
                    temp->active=0;                                             //changing this variable to 0 means that active_deQueue will remove it from the struct of active commands 
                    active_deQueue(A);
                    return;
                } else {
                    perror("kill");
                    exit(EXIT_FAILURE);
                }
                
            }

            temp=temp->ahead_of;
        }





        strcpy(commander_message,"");                                           //making the commander_message string empty again so it can be used a second time






        JOBS* temp2 = Q->front;                                                    //this is basically the same approach but instead of the active command struct now we are searching in the waiting queue
        while (temp2 != NULL) {
            strcpy(message,message_2);

            char *args2[3];  
            char jobID2[BUFFER_SIZE];
            int numArgs = 0;
            memset(jobID2, 0, sizeof(jobID2));                                                //creation of the args2 array that contains every word of the command

            char *token2 = strtok(message, " ");
            while (token2 != NULL && numArgs < 2) {

                args2[numArgs++] = token2;
                token2 = strtok(NULL, " ");
            }
            args2[numArgs] = NULL;  
            strcat(jobID2, args2[1]); 

            if (strstr( temp2->jobID , jobID2)  != NULL){                               //checking if the job_id matches with the one we are looking for


                if((temp2 == Q->front) && (Q->front->ahead_of !=NULL)){
                    Q->front = Q->front->ahead_of;
                    
                }

                if((temp2 == Q->rear)  && (Q->rear->behind_of !=NULL))  {
                    Q->rear = Q->rear->behind_of;
                    
                }                                                                       //deleting the node that we found while making sure that no connections between the waiting queue are broken


                if (Q->front == NULL) {
                    Q->rear = NULL;
                } else {
                    Q->front->behind_of = NULL;
                }

                if((temp2 == Q->front) && (temp2 == Q->rear)){
                    Q->front = NULL;
                    Q->rear = NULL;
                    
                }

                

                if(temp2->ahead_of !=NULL){
                    temp2->ahead_of->behind_of=temp2->behind_of;
                }
                if(temp2->behind_of !=NULL){
                    temp2->behind_of->ahead_of=temp2->ahead_of;
                }   
                  
                
                strcat(commander_message, temp2->jobID); 
                strcat(commander_message, " "); 
                strcat(commander_message, "removed"); 

                bytesWritten = write(fd2, commander_message, strlen(commander_message) + 1);                //sending the job_xx removed message to jobCommander
                
                if (bytesWritten == -1) {
                    perror("write");
                    exit(EXIT_FAILURE);
                } 
                b=0;
                while (b==0) {
                    
                    if (sigusr2_received) {
                        
                        b=1;
                        sigusr2_received = 0;                                               //awaiting for the sigusr2 signal
                    }                                
                }
                free(temp2); 
                return;

            }

            temp2=temp2->ahead_of;
        }


        bytesWritten = write(fd2, final_message,  strlen(final_message) + 1);               //this message is sent to the jobCommander when the job_id didnt exist in the active struct or the waiting queue
        if (bytesWritten == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }  
        b=0;
        while (b==0) {
            if (sigusr2_received) {
                
                b=1;
                sigusr2_received = 0;                                                       //awaiting for the sigusr2 signal
            }
        }  
        return;            
        
    }
    
}


void addJobIDs(Queue* queue, char* jobIDsString) {
    JOBS* temp = queue->front;
    while (temp != NULL) {
        
        strcat(jobIDsString, temp->jobID);

        
        if (temp->ahead_of != NULL) {
            strcat(jobIDsString, ",");
        }

        
        temp = temp->ahead_of;
    }
}

void addactiveJobIDs(active_Queue* A, char* jobIDsString) {
    active_JOBS* temp = A->front;
    while (temp != NULL) {
        
        strcat(jobIDsString, temp->jobID);

        
        if (temp->ahead_of != NULL) {
            strcat(jobIDsString, ",");
        }

        
        temp = temp->ahead_of;
    }
}