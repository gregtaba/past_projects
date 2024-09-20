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


#define BUFFER_SIZE 1024





int main(int argc, char *argv[]) {
    
    int s;
    struct sockaddr_in server_addr;
    struct hostent *server;


    if (argc < 4) {
        printf("In order for this program to be executed, it requires 3 command line arguments\n");
        exit(1);
    }

    char *serverName=argv[1];                                       //creating variables suited for the command line arguments
    int portnum = atoi(argv[2]);
    
    

    
    
    int t = 0;
    for (int i = 3; i < argc; i++) {
        t=t+strlen(argv[i]) + 1; // +1 for the space or null terminator
    }

    
    char *jcc = malloc(t);
    if (jcc == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 1;
    }

    
    jcc[0] = '\0';
    for (int i = 3; i < argc; i++) {
        strcat(jcc, argv[i]);                                         //transfering all the arguments needed for the command in one string
        strcat(jcc, " ");
    }

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {               //here we create the socket with the function socket()
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }


    if ((server = gethostbyname(serverName)) == NULL) {                 //here we check if the host server can be resolved
        fprintf(stderr, "No such host: %s\n", serverName);
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));                       // here we zero out the server_addr structure
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);             // Copy the IP address
    server_addr.sin_port = htons(portnum);                                              // Set the port number

    
    if (connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {        //here we connect to the server
        perror("Connection failed");
        close(s);
        exit(EXIT_FAILURE);
    }

    
    if (write(s, jcc, strlen(jcc)) < 0) {                              //writing the command to the jobExecutorServer
        perror("Write to socket failed");
        close(s);
        exit(EXIT_FAILURE);
    }

    
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);

                                                                        //in this part the jobCommander will check the third command line argument and depending on what it is will enter the correct if statement
    
    if (strcmp(argv[3], "issueJob") == 0) {                                 
        int br;
        int tbr = 0;
        int bmc = BUFFER_SIZE;
        char *df = malloc(bmc);

        if (df == NULL) {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }

        while (1) {
            if ((br = read(s, buffer, BUFFER_SIZE - 1)) == -1) {                   //first we read the command given by jobCommander
                perror("read");
                exit(EXIT_FAILURE);
            }
            if (strstr(buffer, "SERVER TERMINATED BEFORE EXECUTION") != NULL) {         //checking if the server terminated before executing the job
                strcpy(df,buffer);
                break;
            }    

            if (strstr(buffer, "job deleted from buffer because of stop") != NULL) {        //checking if the job was stopped
                strcpy(df,buffer);
                break;
            }    

            if (br < 0) {
                perror("Error reading from socket");
                free(df);
                break;
            } else if (br == 0) {
                break;
            } else {                                //if we have not completed reading from the socket and the server has not closed the connection
                buffer[br] = '\0';
                if (tbr + br >= bmc) {                      //checking if we need to increase the allocated memory for the buffer we use to store the data we read
                    bmc=bmc*2;
                    char *n = realloc(df, bmc);
                    if (n == NULL) {
                        perror("Failed to reallocate memory");
                        free(df);
                        exit(EXIT_FAILURE);
                    }
                    df = n;
                }

                
                memcpy(df + tbr, buffer, br);                       // copying the data to the buffer we use
                tbr =tbr+br;
                df[tbr] = '\0';



                
                if (strstr(buffer, "output end------") != NULL) {               //checking if we have reached the end of the input from the server
                    
                    break;
                }
            }
        }

        
        printf("%s\n", df);

        free(df);  // freeing the allocated memory
    }
    else if ((strcmp(argv[3], "setConcurrency") == 0))
    {       
        int br;
        if ((br = read(s, buffer, BUFFER_SIZE - 1)) == -1) {                   //read the input from jobExecutorServer and print it
            perror("read");
            exit(EXIT_FAILURE);
        }
        printf("%s\n",buffer);     
    }
    else if ((strcmp(argv[3], "stop") == 0))
    {   
        int br;
        if ((br = read(s, buffer, BUFFER_SIZE - 1)) == -1) {                   //read the input from jobExecutorServer and print it
            perror("read");
            exit(EXIT_FAILURE);
        }
        printf("%s\n",buffer);    
    }
    else if ((strcmp(argv[3], "poll") == 0) )                           //almost identical to the issueJob if statement 
    { 
        int br;
        int tbr = 0;
        int bmc = BUFFER_SIZE;
        char *df = malloc(bmc);

        if (df == NULL) {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }

        while (1) {
            if ((br = read(s, buffer, BUFFER_SIZE - 1)) == -1) {                   //first we read the command given by jobCommander
                perror("read");
                exit(EXIT_FAILURE);
            }
            if (strstr(buffer, "no jobs in buffer") != NULL) {                          //checking if there are no more jobs in the buffer queue of the server 
                break;
            }    

            if (br < 0) {
                perror("Error reading from socket");
                free(df);
                break;
            } else if (br == 0) {
                break;
            } else {                                //if we have not completed reading from the socket and the server has not closed the connection
                buffer[br] = '\0';
                if (tbr + br >= bmc) {                      //checking if we need to increase the allocated memory for the buffer we use to store the data we read
                    bmc=bmc*2;
                    char *n = realloc(df, bmc);
                    if (n == NULL) {
                        perror("Failed to reallocate memory");
                        free(df);
                        exit(EXIT_FAILURE);
                    }
                    df = n;
                }

                
                memcpy(df + tbr, buffer, br);                       // copying the data to the buffer we use
                tbr =tbr+br;
                df[tbr] = '\0';


            }
        }

        
        printf("%s\n", df);

        free(df);  // freeing the allocated memory
    }
    else if ((strcmp(argv[3], "exit") == 0))
    {
        int br;
        if ((br = read(s, buffer, BUFFER_SIZE - 1)) == -1) {                   //read the input from jobExecutorServer and print it
            perror("read");
            exit(EXIT_FAILURE);
        }
        printf("%s\n",buffer);
    }

    close(s);
    

    return EXIT_SUCCESS;
}