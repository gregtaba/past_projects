#include "function.h"
#include <stdio.h>
#include <sys/wait.h> /* sockets */
#include <sys/types.h> /* sockets */
#include <sys/socket.h> /* sockets */
#include <netinet/in.h> /* internet sockets */
#include <netdb.h> /* gethostbyaddr */
#include <unistd.h> /* fork */
#include <stdlib.h> /* exit */
#include <ctype.h> /* toupper */
#include <signal.h> /* signal */
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>


volatile sig_atomic_t server_running = 1;

#define BUFFER_SIZE 1024

pthread_mutex_t bm;
pthread_cond_t job_available;
pthread_cond_t av_buff;

int limit_of_queue;
Queue* buffer;
int server_fd;
int busy_t=0;
int lcs = -1;
int u_ID=0;

void handle_exit_signal() {       //changing the server running vatiable so  that the server will start shutting down
    server_running = 0;
    pthread_mutex_lock(&bm);
    pthread_cond_broadcast(&job_available); 
    close(server_fd);       //we close the server
    pthread_mutex_unlock(&bm); 
    
}

void setup_signal_handler() {       //creation of the signal
    struct sigaction sa;
    sa.sa_handler = handle_exit_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
}

void signal_exit() {                //this function is used to send the SIGURS1 signal to the server,therefore calling the handle_exit_signal function to start the procedure of shutting down the server
    pid_t pid = getpid();
    if (kill(pid, SIGUSR1) == -1) {
        perror("kill");
        exit(EXIT_FAILURE);
    }
}

void exit_command(char *message){           //this function checks if the command from the jobCommander is "exit",if thats the case it uses the signal_exit function to start the procedure of shutting down the server
    
    if (strncmp(message, "exit", 4) == 0) {                      //if the word exit is part of the command that was given by the jobcommander
        
        signal_exit();
    }
}

void set_concurrency(char *message_2,Queue *Q,int cs){

    char message[BUFFER_SIZE];
    strcpy(message,message_2);
    const char sub2[] = "setConcurrency"; 
    char fm[40]="CONCURRENCY SET AT ";
    int len = strlen(sub2);
    int l2= strlen(fm);
    char *p = strstr(message, sub2);
    int k,l,d;

    if (strncmp(message, "setConcurrency", 14) == 0) {                      //if the word setConcurrency is part of the command that was given by the jobcommander
        l=Q->concurrency;
        memmove(p, p + len, strlen(p + len) + 1);                           //moving the string to the point that it goes over the "setConcurrency" and right into the number of the new concurrency
        Q->concurrency=atoi(message);                                       //changing the value of the concurrency in the queue
        
        k=Q->concurrency;
        sprintf(fm+l2, "%d", k);                                            //creating the CONCURRENCY SET AT N string
        if (send(cs, fm, strlen(fm), 0) == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }

        if(k>l){
            
            pthread_mutex_lock(&bm);
            
            d=k-busy_t;
            for (int i = 0; i < d; i++){
                pthread_cond_signal(&job_available);                            //alerting the worker threads that the concurrency has risen compared to what it was
            }
            
            pthread_mutex_unlock(&bm);
        }
        
    }
}

void poll(int clientSocket) {
    char fm[20]="no jobs in buffer";
    char res[BUFFER_SIZE];
    strcpy(res,"");                 // this is to make sure that res is originally empty

    pthread_mutex_lock(&bm);      //we lock the mutex and then for every job inside the buffer queue we create <jobID,job>\n and add it to res
    Job *cj = buffer->front;

    if(cj == NULL){
        if (send(clientSocket, fm, strlen(fm), 0) == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }
    }

    while (cj != NULL) {
        
        strcat(res, "<");
        strcat(res, cj->jobID);
        strcat(res, ",");
        strcat(res, cj->job);
        strcat(res, ">\n");
        if (send(clientSocket, res, strlen(res), 0) == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }
    
        cj = cj->next;
        strcpy(res,""); 
        
    }

    if (send(clientSocket, fm, strlen(fm), 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }


    pthread_mutex_unlock(&bm);        //afterwards we unlock the mutex and write the res string in the client socket


    
}


void Stop(char *message_2,int clientSocket){
    
    char message[BUFFER_SIZE];
    char commander_message[BUFFER_SIZE];
    char *a;
    const char c = 'j';
    strcpy(message,message_2);
    const char sub[] = "stop ";
    int len = strlen(sub);
    char* p = strstr(message, sub);
    int bw,k,counter;
    int bj;
    bj=buffer->buffer_jobs;
    
    strcpy(commander_message,"");
    strcpy(message,"");
    k=0;
    char fm[40]="job deleted from buffer because of stop";
    
    if (p != NULL) {                   
               
        pthread_mutex_lock(&bm);                                              //locking the mutex so that we can alter the buffer queue
        Job* t = buffer->front; 
        Job* t2 = NULL;                                                              //this will be used so that we have a way to change the "next" pointer to the previous job of the job we will erase                                                
        
        if (bj != 0){
            
            while (t != NULL ) {
                
                strcpy(message,message_2);
                counter++;
                char *args[3];  
                char jobID[BUFFER_SIZE];
                int numArgs = 0;
                memset(jobID, 0, sizeof(jobID));                                                //creation of the args array that contains every word of the command
                char *token = strtok(message, " ");
                while (token != NULL && numArgs < 2) {

                    args[numArgs++] = token;
                    token = strtok(NULL, " ");
                }
                args[numArgs] = NULL;  
                strcat(jobID, args[1]); 
                
                if (strstr( t->jobID , jobID)  != NULL){                               //checking if the job_id matches with the one we are looking for
                    k=1;

                    if((t == buffer->front) && (buffer->front->next !=NULL)){        //checking if the job we are looking for is the first job of the queue
                        buffer->front = buffer->front->next;   
                    }
                    else if((t == buffer->front) && (t == buffer->rear)){              //checking if the job we are looking for is the only job of the queue
                        buffer->front = NULL;
                        buffer->rear = NULL;  
                              
                    }else if (t == buffer->rear) {                                         //checking if the job we are looking for is the last job of the queue
                        buffer->rear = t2;
                    }else if (t2 != NULL) {                                                //if there is a previous job to the one we erased then that jobs "next" pointer becomes the "next" pointer of the job we will erase 
                        t2->next = t->next;
                    }

                    if (buffer->front == NULL) {
                        buffer->rear = NULL;    
                    }



                    if (t2 != NULL) {                                                //if there is a previous job to the one we erased then that jobs "next" pointer becomes the "next" pointer of the job we will erase 
                        t2->next = t->next;
                    }

                    strcat(commander_message, t->jobID); 
                    strcat(commander_message, " "); 
                    strcat(commander_message, "REMOVED"); 
                    
                    bw = write(clientSocket, commander_message, strlen(commander_message) + 1);                //sending the job_xx removed message to jobCommander
                    
                    if (bw == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    } 

                    bw = write(t->clientSocket, fm, strlen(fm) + 1);                                       //sending a message to the jobcommander whose job was stopped                
                    
                    if (bw == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    } 
                    buffer->buffer_jobs--;                                                                  //changing the number of jobs in the buffer and freeing the memory allocated for the job that was stopped

                    if( (buffer->buffer_jobs+1) == limit_of_queue){      //checking if the buffer queue was full before before stopping because if it was then we need to alert the controller threads that are waiting that there is available room in the buffer
                        pthread_cond_signal(&av_buff);
                    }
                    free(t); 
                    pthread_mutex_unlock(&bm);
                    return;

                } 
                                                                  
                t2=t;
                t=t->next;
                
            }
        }
         
        if(k==0){
            strcpy(message,message_2);                                                              //using strcpy(),strchr and strcat() we create the jobID NOTFOUND string we wil send back to the jobCommander in case the jobID didnt exist in the buffer queue
            a = strchr(message, c);
            strcat(commander_message, a); 
            strcat(commander_message, " "); 
            strcat(commander_message, "NOTFOUND"); 
            
            bw = write(clientSocket, commander_message, strlen(commander_message) + 1);                //sending the job_xx removed message to jobCommander
            
            if (bw == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            } 
        }
        pthread_mutex_unlock(&bm);
         
        return;            
        
    }
    
}


void *wt_function(void *arg) {
    struct Thread_info *thread_data = (struct Thread_info *)arg;

    while (server_running) {
        pthread_mutex_lock(&bm);                                                  //locking the mutex to use pthread_cond_wait and alter busy_t

        while ((buffer->front == NULL || busy_t >= buffer->concurrency) && (server_running==1)) {                    //while buffer is empty or busy threads are bigger or equal to concurrency no worker thread can begin a new job
            pthread_cond_wait(&job_available, &bm);                               //waiting for a change in job_available condition available so that we can check if a job is available or if the busy threads are now lower than the conurrency
            
        }
        if(server_running==0){
            
            pthread_mutex_unlock(&bm);
            return NULL;
        }
        busy_t++;                                               //once we leave the while loop the thread is now busy therefore we increase the busy_t variable that shows how many busy threads we have
        
        
        pthread_mutex_unlock(&bm);
        Job *tn = dequeue(buffer);                      //using dequeue we get the job that this thread will complete

        if(tn == NULL){
            pthread_mutex_lock(&bm);
            busy_t--;                                                                               
            pthread_mutex_unlock(&bm);
            continue;
        }
        
        char tj[BUFFER_SIZE];
        strcpy(tj,tn->job);
        
        char fm[BUFFER_SIZE];
        strcpy(fm,"");
        if (tn != NULL) {
            
            char *args[200];  // Maximum number of arguments
            int numArgs = 0;                                                
            char *token = strtok(tj, " ");                 //creation of the args array that contains every word of the command
            while (token != NULL && numArgs < 199) {
                args[numArgs++] = token;
                token = strtok(NULL, " ");
            }
            args[numArgs] = NULL; 

            strcat(fm, "JOB ");                             //creation of the <JOB <jobID,job> SUBMITTED string
            strcat(fm, "<");
            strcat(fm, tn->jobID);
            strcat(fm, ",");
            strcat(fm, tn->job);
            strcat(fm, "> ");
            strcat(fm, "SUBMITTED \n");

            if (send(tn->clientSocket, fm, strlen(fm), 0) == -1) {
                perror("send");
                exit(EXIT_FAILURE);
            }
            
            pid_t pid = fork();                                     //here we use fork so that the child process can execute the job
            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (pid == 0) {                                  //child process
                int a;
                char file[BUFFER_SIZE];
                a=getpid();
                snprintf(file, sizeof(file), "%d.output", a);
                int fd = open(file, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);             //creating a file named "pid.output" with the apropriate rights and abilities
                if (fd < 0) {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                
                if (dup2(fd, STDOUT_FILENO) < 0) {                              //here we use dup2 so that STDOUT file descriptor will point to the new file that we created
                    perror("dup2");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
                close(fd);
                
                if (execvp(args[0], args) == -1) {                             //executing the new command
                    perror("execlp");
                    exit(EXIT_FAILURE);
                }
                
            } else {                                                            //parent process
                int status;
                if (waitpid(pid, &status, 0) == -1) {                           //waiting for the child process to complete
                    perror("waitpid");
                    exit(EXIT_FAILURE);
                }
                
                char file[BUFFER_SIZE];
                snprintf(file, sizeof(file), "%d.output", pid);

                FILE *f = fopen(file, "r");                                     //opening the file "pid.output" that we created in the child process
                if (!f) {
                    perror("fopen");
                    exit(EXIT_FAILURE);
                }

                char buffer[BUFFER_SIZE];
                char start_message[BUFFER_SIZE];
                char end_message[BUFFER_SIZE];

                snprintf(start_message, sizeof(start_message), "-----%s output start------\n", tn->jobID);              //here we create the start_message and end_message strings
                snprintf(end_message, sizeof(end_message), "-----%s output end------\n", tn->jobID);

                if (send(tn->clientSocket, start_message, strlen(start_message), 0) == -1) {                    //initially we send the start_message string to the jobCommander
                    perror("send start_message");
                    exit(EXIT_FAILURE);
                }

                while (fgets(buffer, sizeof(buffer), f)) {                                              //after that using the fgrts() function we send the result of the command that the child process completed to the jobCommander
                    if (send(tn->clientSocket, buffer, strlen(buffer), 0) == -1) {
                        perror("send");
                        exit(EXIT_FAILURE);
                    }
                }

                if (send(tn->clientSocket, end_message, strlen(end_message), 0) == -1) {                //finally we send the end_message string to the jobCommander
                    perror("send end_message");
                    exit(EXIT_FAILURE);
                }

                fclose(f);
                remove(file);
                close(tn->clientSocket);                                                            //we delete the pid.output file and close the client socket

            }
            pthread_mutex_lock(&bm);
            busy_t--;                                                                               //finally after we lock the mutex we update the busy_t variable since this thread is no longer busy and after that we free the memory used for the now completed job
            pthread_mutex_unlock(&bm);
            
            free(tn);
            
        }
        
    }

    return NULL;
}


void *ct_function(void *arg) {
    int cs = *((int *)arg);
    free(arg);                                                  // this happens because we need to free the dynamically allocated memory for the socket

    char d[BUFFER_SIZE];
    int br;
    
    
    if ((br = read(cs, d, BUFFER_SIZE - 1)) == -1) {                   //first we read the command given by jobCommander
        perror("read");
        close(cs);
        return NULL;
    }
    
    d[br] = '\0';           //making sure the d string is empty
    pthread_mutex_lock(&bm);
    
    if (strstr(d, "issueJob") != NULL) {

        while (buffer->buffer_jobs >= limit_of_queue) {
            
            pthread_cond_wait(&av_buff, &bm);         //if the total jobs of the buffer are equal to the limit_of_queue (bufferSize) then no controller thread can begin until one of the jobs is removed from the queue.When that happens a signal will be received by pthread_cond_wait() making us check the while loop condition again
        }

    }

    pthread_mutex_unlock(&bm);

    
    
    char fm[50]="SERVER TERMINATED BEFORE EXECUTION";               

    
    if (strstr(d, "issueJob") != NULL) {                                        //in this point the controller thread function will search the d string and go to the aproppriate function
        enqueue(buffer,d,cs);
    } else if (strstr(d, "exit") != NULL) {
        pthread_mutex_lock(&bm);                                                //in case of exit we also have to dequeue all the jobs that are in the buffer and send the SERVER TERMINATED BEFORE EXECUTION mesage to each jobCommander whose job was never executed due to server termination
        while(buffer->front!=NULL){
            pthread_mutex_unlock(&bm);
            Job *tn = dequeue(buffer); 
            if (send(tn->clientSocket, fm, strlen(fm), 0) == -1) {                
                perror("send end_message");
                exit(EXIT_FAILURE);
            }
            pthread_mutex_lock(&bm);
        }
        pthread_mutex_unlock(&bm);
        exit_command(d);
    } else if (strstr(d, "poll") != NULL) {                                                      
        poll(cs);
    } else if (strstr(d, "setConcurrency") != NULL) {  
        set_concurrency(d,buffer,cs);
    } else if (strstr(d, "stop") != NULL) {
        Stop(d,cs);
    }

    return NULL;
}


Job* create_job(const char* tripleta, const char* jobID, const char* job, int clientSocket) {               //in this function we create a new job
    Job* j = (Job*)malloc(sizeof(Job));
    if (j == NULL) {
        perror("Failed to allocate memory for new job");
        exit(EXIT_FAILURE);
    }
    strcpy(j->tripleta, tripleta);
    strcpy(j->jobID, jobID);
    strcpy(j->job, job);
    j->clientSocket = clientSocket;
    j->next = NULL;
    return j;
}


Queue* create_queue() {                                         // this function is used to create a new queue and initialize the mutex and the 2 condition variables                     
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (q == NULL) {
        perror("Failed to allocate memory for new queue");
        exit(EXIT_FAILURE);
    }
    q->front = NULL;
    q->rear = NULL;
    q->buffer_jobs = 0;
    q->concurrency = 1;
    pthread_mutex_init(&bm, NULL);
    pthread_cond_init(&job_available, NULL);
    pthread_cond_init(&av_buff, NULL);
    return q;
}


void destroy_queue(Queue* queue) {                             // this function destroys the queue,frees the allocated memory and also destroys the mutex and the 2 condition variables 
    Job* j = queue->front;
    while (j != NULL) {
        Job* temp = j;
        j = j->next;
        free(temp);
    }
    free(queue);
    pthread_mutex_destroy(&bm);
    pthread_cond_destroy(&job_available);
    pthread_cond_destroy(&av_buff);
}


void enqueue(Queue* q,const char *d,int a) {


    pthread_mutex_lock(&bm);

    char jobID[BUFFER_SIZE];   
    char client_Socket[BUFFER_SIZE];                         //creation of the necessary arrays to store the information of the tripleta
    memset(jobID, 0, sizeof(jobID));

    int client_sock = a;     
         
    sprintf(jobID, "job_%d", u_ID+1);         //this will be the job_ID in the tripleta
    sprintf(client_Socket, "%d", client_sock);              //this will be the client socket in the tripleta

    char j[BUFFER_SIZE];
    strcpy(j,d);
    const char sub[] = "issueJob ";
    int len = strlen(sub);
    char* p = strstr(j, sub);

    if ((p != NULL)&&(j[0]=='i')) {                          //if the word issueJob is part of the command that was given by the jobcommander
        
        memmove(p, p + len, strlen(p + len) + 1);  // Shift characters to the left so that i  can get rid of the "issuejob" part of the j string 
    

        char tripleta[BUFFER_SIZE]="<";                             //creating the <jobID,job,client_Socket> tripleta

        strcat(tripleta,jobID);
        strcat(tripleta,",");
        strcat(tripleta,j);
        strcat(tripleta,",");
        strcat(tripleta,client_Socket);
        strcat(tripleta,">");


        
        if (q->buffer_jobs < limit_of_queue) {
            u_ID++;
            Job* nj=create_job(tripleta,jobID,j,client_sock);           //creating a new job for the queue
            if (q->rear == NULL) {
                q->front = nj;
                q->rear = nj;
            } else {
                q->rear->next = nj;
                q->rear = nj;
            }
            q->buffer_jobs++;
        } else {
            printf("Buffer is full, cannot enqueue.\n");
            
        }
    }
    pthread_cond_signal(&job_available);                            //alerting the worker threads that are not busy that there is a job available for them
    pthread_mutex_unlock(&bm);
    
}


Job* dequeue(Queue* q) {
    pthread_mutex_lock(&bm);
    if (q->front == NULL) {
        printf("Buffer is empty, cannot dequeue.\n");
        return NULL;
    }
    Job* a = q->front;
    q->front = q->front->next;              //changing the front of the queue
    
    if (q->front == NULL) {                 //covering the possibility of having only one job in the queue
        q->rear = NULL;
    }
    q->buffer_jobs--;
    
    
    if( (q->buffer_jobs+1) == limit_of_queue){      //checking if the buffer queue was full before before dequeuing because if it was then we need to alert the controller threads that are waiting that there is available room in the buffer
        pthread_cond_signal(&av_buff);
    }
    
    pthread_mutex_unlock(&bm);
    return a;
}

