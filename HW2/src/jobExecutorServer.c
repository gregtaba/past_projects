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
#include "function.h"




#define BUFFER_SIZE 1024



int main(int ac, char *av[]) {
    if (ac != 4) {
        printf("In order for this program to be executed, it requires 3 command line auments\n");
        exit(1);
    }

    int portnum = atoi(av[1]);                                                              //creating the appropriate variables depending on the command line arguments
    int bufferSize = atoi(av[2]);
    int threadPoolSize = atoi(av[3]);
    if (threadPoolSize <= 0) {
        fprintf(stderr, "threadPoolSize must be a positive integer\n");
        return 1;
    }

    limit_of_queue=bufferSize;
    buffer=create_queue();                                                                  //creating the buffer queue and the signal handler
    setup_signal_handler();
    

    int cs;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    
    struct Thread_info *threads = malloc(threadPoolSize * sizeof(struct Thread_info));      //allocating memory for the worker threads
    if (threads == NULL) {
        fprintf(stderr, "Failed to allocate memory for threads\n");
        return 1;
    }
    

    for (int i = 0; i < threadPoolSize; i++) {

        threads[i].running = 0;                                                             //here we use the pthread_create() function and the struct Thread_info  to create worker threads  equal to threadPoolSize.In every wt_function we send &threads[i] as a function so that wt_function can access the struct Thread_info but only for its own specific thread 
        threads[i].waiting = 1;

        if (pthread_create(&threads[i].thread, NULL, wt_function, &threads[i]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            return 1;
        }
    }
    

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);                                     //in this part we use the function socket() to create a socket
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(portnum);


    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");                                          
        close(server_fd);                                           //in this part we use the function bind() to bind the socket we created to a port
        exit(EXIT_FAILURE);
    }

    
    if (listen(server_fd, threadPoolSize) < 0) {
        perror("Listen failed");                        //in this part we use the function listen() to check for possible incoming connections
        close(server_fd);
        exit(EXIT_FAILURE);
    }


    
    while (server_running) {
         
        if ((cs = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            if (errno == EINTR) {          //this part is simply to avoid an interrupted system call
                
                continue; 
            } else {
                perror("accept");
                close(server_fd);
                exit(EXIT_FAILURE);
            }
        }

                


        pthread_t ct;                    //when main reaches this part that means that main has accepted a connection ,therefore it has to create a controller thread
        int *a = malloc(sizeof(int));
        if (a == NULL) {
            fprintf(stderr, "Failed to allocate memory for thread aument\n");
            close(cs);
            continue;
        }
        *a = cs;
        lcs=cs;
        if (pthread_create(&ct, NULL, ct_function, a) != 0) {            //here we create the controller thread while sending the client socket as an argument in the conntroller thread function (ct_function)
            fprintf(stderr, "Error creating controller thread\n");
            close(cs);
            free(a);
            continue;
        }
        pthread_detach(ct);                                      // here we detach the thread so it can clean up after itself without us having to do anything
        
    }

    if (lcs != -1) {
        char fm[] = "SERVER TERMINATED";
        if (write(lcs, fm, strlen(fm)) == -1) {
            perror("write");
        }
        close(lcs); // Close the last client socket
    }
    
   
    for (int i = 0; i < threadPoolSize; i++) {
        pthread_join(threads[i].thread, NULL);              //here we use the pthread_join function in order to wait for the worker threads to complete
    }
     




    free(threads);          // Free the memory allocated for the worker threads
    destroy_queue(buffer);  //destroy the buffer queue

    return 0;
}