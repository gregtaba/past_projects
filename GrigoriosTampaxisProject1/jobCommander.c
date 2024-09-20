#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define PIPE "commander"
#define PIPE2 "server"
#define BUFFER_SIZE 512
#define FILE_PATH "jobExecutorServer.txt"






int main(int argc, char *argv[]) {
    
    int bytesRead,bytesWritten;
    char commandString[BUFFER_SIZE];
    memset(commandString, 0, sizeof(commandString));
    int fd,fd2;
    
    FILE *file;
    char pid_server[BUFFER_SIZE]; 
    int server_pid;
    int max_retries = 5;
    int retries = 0;
    int success = 0;





    if (access(FILE_PATH, F_OK) != -1) {                        //checking whether jobExecutorServer is active
        
    } else {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        } else if (pid == 0) {   
            if (execl("./jobExecutorServer", "jobExecutorServer", NULL) == -1) {                //if not we start it now      
                perror("execl");
                exit(EXIT_FAILURE);
            }
        } else {
            printf("Waiting for jobExecutorServer to start...\n");
            sleep(1);

        }
    }

    file = fopen("jobExecutorServer.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }
    if (fgets(pid_server, sizeof(pid_server), file) == NULL) {
        fprintf(stderr, "Error reading PID from file\n");                           //in this part we retrieve the pid of the server
        fclose(file);
        return 1;
    }
    fclose(file);
    server_pid = atoi(pid_server);


    if (server_pid == 0 && pid_server[0] != '0') {
        fprintf(stderr, "Error converting PID to integer\n");
        return 1;
    }
    


    mkfifo(PIPE, 0666);
    mkfifo(PIPE2, 0666);

    
    fd = open(PIPE, O_WRONLY );                                             //creating and opening the named pipes
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    fd2 = open(PIPE2, O_RDONLY );
    if (fd2 == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }    

    while (retries < max_retries && !success) {
    if (kill(server_pid, SIGUSR2) == -1) {
        perror("Error sending SIGUSR2");
        return 1;
    } else {
        
        success = 1;                                                        //sending a signal to jobExecutorServer so that it knows that the jobCommander is ready to send the command
    }
    retries++;
    usleep(100000); 
    }

    if (!success) {
        fprintf(stderr, "Failed to send SIGUSR2 after %d retries\n", max_retries);
        return 1;
    }


    for (int i = 1; i < argc; i++) {
        strcat(commandString, argv[i]);                                         //transfering all the arguments in one string
        strcat(commandString, " ");
    }

    bytesWritten = write(fd, commandString, strlen(commandString) + 1);         //writing the command in the named pipe
    if (bytesWritten == -1) {                                       
        perror("read");
        exit(EXIT_FAILURE);
    }


    //depending on the first argument we will know what action will be taken.After that we go to the appropriate if statement
    //where we will either receive and print a response from the jobExecutorserver and then send them a signal to notify them that we got the message,or we dont 
    //have to do anything since nothing came back at (setConcurrency).


    if (strcmp(argv[1], "issueJob") == 0) {                                 

        bytesRead = read(fd2, commandString, BUFFER_SIZE);
        if (bytesRead == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (kill(server_pid, SIGUSR2) == -1) {
            perror("Error sending SIGUSR2");
            return 1;
        }
        printf("%s\n", commandString);
        sleep(1); 
    }
    else if ((strcmp(argv[1], "setConcurrency") == 0))
    {

             
    }
    else if ((strcmp(argv[1], "stop") == 0))
    {
        int k;
        k=0;
        char sub[4]="job";
        while(k==0){ 
            bytesRead = read(fd2, commandString, BUFFER_SIZE);
            if (bytesRead == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }  

            
            if (strcmp(commandString, "DONE") == 0){
                printf("job doesnt exist in the commands that are being executed or in the waiting queue\n");
                break;
            }
            else if (strstr(commandString, sub) != NULL){
                printf("%s\n", commandString);
                break;
            }
            
            
        }

        max_retries = 5;                //in order to help with synchronization we create this so that in case of failed signal commander can try again and make sure that the signal gets through
        retries = 0;
        success = 0;

        while (retries < max_retries && !success) {
            
            if (kill(server_pid, SIGUSR2) == -1) {
                perror("Error sending SIGUSR2");
                return 1;
            } else {
                
                success = 1;
            }
            retries++;
            usleep(100000); 
        }

        if (!success) {
            fprintf(stderr, "Failed to send SIGUSR2 after %d retries\n", max_retries);
            return 1;
        }
            
    }
    else if ((strcmp(argv[1], "poll") == 0) && (strcmp(argv[2], "running") == 0))
    {
        int k;
        k=0;
        
        while(k==0){
             
            bytesRead = read(fd2, commandString, BUFFER_SIZE);
            if (bytesRead == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }  
            if (kill(server_pid, SIGUSR2) == -1) {
                perror("Error sending SIGUSR2");
                return 1;
            }
            
            if (strcmp(commandString, "DONE") == 0){
                k=1;
            }
            else{
                printf("%s\n", commandString);
            }
        }
        

        max_retries = 5;                                        //we use the same tactic
        retries = 0;
        success = 0;

        while (retries < max_retries && !success) {
            if (kill(server_pid, SIGUSR2) == -1) {
                perror("Error sending SIGUSR2");
                return 1;
            } else {
                
                success = 1;
            }
            retries++;
            usleep(100000); 
        }

        if (!success) {
            fprintf(stderr, "Failed to send SIGUSR2 after %d retries\n", max_retries);
            return 1;
        }
    }
    else if ((strcmp(argv[1], "poll") == 0) && (strcmp(argv[2], "queued") == 0))
    {
        int k;
        k=0;
        
        
        while(k==0){

             
             
            bytesRead = read(fd2, commandString, BUFFER_SIZE);
            
            if (bytesRead == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }  
            
            if (kill(server_pid, SIGUSR2) == -1) {
                perror("Error sending SIGUSR2");
                return 1;
            }

            if (strcmp(commandString, "DONE") == 0){
                break;
            }
            else{
                printf("%s\n", commandString);
            }

            
            
        }
        
        int max_retries = 5;
        int retries = 0;
        int success = 0;

        while (retries < max_retries && !success) {
            if (kill(server_pid, SIGUSR2) == -1) {
                perror("Error sending SIGUSR2");
                return 1;
            } else {
                
                success = 1;
            }
            retries++;
            usleep(100000); 
        }

        if (!success) {
            fprintf(stderr, "Failed to send SIGUSR2 after %d retries\n", max_retries);
            return 1;
        }
    }
    else if ((strcmp(argv[1], "exit") == 0))
    {
        bytesRead = read(fd2, commandString, BUFFER_SIZE);
        if (bytesRead == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (kill(server_pid, SIGUSR2) == -1) {
            perror("Error sending SIGUSR2");
            return 1;
        }
        printf("%s\n", commandString);
        sleep(1); 
        return EXIT_SUCCESS;
               
    }

    //after the if statement we send a final signal to the jobexecutorserver so that it knows that this jobCommander has fulfilled its purpose and its about to finish

    max_retries = 5;
    retries = 0;
    success = 0;

    while (retries < max_retries && !success) {
        if (kill(server_pid, SIGUSR2) == -1) {
            perror("Error sending SIGUSR2");
            return 1;
        } else {
            
            success = 1;
        }
        retries++;
        usleep(100000); 
    }

    if (!success) {
        fprintf(stderr, "Failed to send SIGUSR2 after %d retries\n", max_retries);
        return 1;
    }

    if (close(fd) == -1) {
        perror("close");                                                //closing the file descriptors and ending the programme
        exit(EXIT_FAILURE);
    }

    if (close(fd2) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    
    

    return EXIT_SUCCESS;
}