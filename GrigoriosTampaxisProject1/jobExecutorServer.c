#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "function.h"

#define PIPE "commander"
#define PIPE2 "server"
#define FILE_PATH "jobExecutorServer.txt"
#define BUFFER_SIZE 512


int main() {
    static int jobCounter = 0;
    int fd,fd2,i;
    int bytesRead;
    char message[BUFFER_SIZE];
    Queue *Q;
    active_Queue *A;
    int b;

    Q=initializeQueue();
    A=initializeactive_Queue();
    i=0;

    struct sigaction sa_usr2, sa_chld;

    sa_usr2.sa_handler = sigusr2_handler;
    sigemptyset(&sa_usr2.sa_mask);
    sa_usr2.sa_flags = 0;

    // making the SIGUSR2 handler using sigaction
    if (sigaction(SIGUSR2, &sa_usr2, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    sa_chld.sa_handler = sigchld_handler;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART; 

    // making the SIGCHLD handler using sigaction
    if (sigaction(SIGCHLD, &sa_chld, NULL) == -1) {
        perror("sigaction for SIGCHLD");
        return 1;
    }

    while (i==0)
    {

        if (access(FILE_PATH, F_OK) == -1) {                            //checking the existence of jobExecutorServer.txt
            
            FILE *file = fopen(FILE_PATH, "w");
            if (file == NULL) {
                perror("Error opening file");
                return 1;
            }
            pid_t pid = getpid();
                                                                                                        
            fprintf(file, "%d\n", pid); 
            fclose(file);
        }

        mkfifo(PIPE, 0666);
        mkfifo(PIPE2, 0666);
        

        fd = open(PIPE, O_RDONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);                                         //creating (if they have not already been created) and opening the named pipes
        }

        fd2 = open(PIPE2, O_WRONLY);
        if (fd2 == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        

        b=0;
        
        while (b==0) {
            
            if (sigchild_received) {
                active_deQueue(A);
                sigchild_received = 0;
            }                                                           //awaiting for the signal.This signal lets us know that the jobcommander is ready to send us a message
            
            if (sigusr2_received) {
                
                b=1;
                sigusr2_received = 0; 
            }
            usleep(100000);
            
        }

                                                                                                               //reading the command from the pipe
        bytesRead = read(fd, message, BUFFER_SIZE);
        if (bytesRead == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        //every function covers a different possibility,once the  first five are over we go into issuejob that apart from its own job also has the responsibility to check
        //whether there are any free spaces for new commands to be executed.


        i=exit_command(message,fd2);                                                

        poll_queued(fd2,message,Q);
        
        poll_running(fd2,message,A);
        
        set_concurrency(message,Q);
        
        Stop(Q,A,message,fd2);

        jobCounter=issueJob(Q,A,message,jobCounter,fd2);

        

        if (close(fd) == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }
                                                                                        //after the functions we close the descriptors and erase the message string
        if (close(fd2) == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }
        memset(message, 0, sizeof(message));
        b=0;
        
        while (b==0) {
            
            if (sigchild_received) {
                active_deQueue(A);
                sigchild_received = 0;
            }
            
            if (sigusr2_received) {                                             //awaiting the signal that will let us know that jobCompiler has completed its purpose and its ready to terminate
                
                b=1;
                sigusr2_received = 0; 
            }
            usleep(100000);
            
        }

    }
    freeQueue(Q);
    freeactive_Queue(A);

    unlink(PIPE);
    unlink(PIPE2);

    return 0;
}