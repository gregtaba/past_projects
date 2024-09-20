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


extern volatile sig_atomic_t server_running ;

#define BUFFER_SIZE 1024

// Job structure
typedef struct Job {
    char tripleta[BUFFER_SIZE];
    char jobID[BUFFER_SIZE];
    char job[BUFFER_SIZE];
    int clientSocket;
    struct Job* next;  
} Job;

// Queue structure
typedef struct Queue {
    Job* front; 
    Job* rear;  
    int buffer_jobs;           //number of jobs in queue
    int concurrency;              //concurrency value
} Queue;

extern pthread_mutex_t bm;
extern pthread_cond_t job_available;
extern pthread_cond_t av_buff;

// Thread data structure
struct Thread_info {
    pthread_t thread;
    int waiting;
    int running;
};

extern int limit_of_queue;
extern Queue* buffer;
extern int server_fd;
extern int busy_t;
extern int lcs;
extern int u_ID;

Job* dequeue(Queue* queue);
void *wt_function(void *arg);
void *ct_function(void *arg);
Job* create_job(const char* tripleta, const char* jobID, const char* job, int clientSocket);
Queue* create_queue();
void destroy_queue(Queue* queue);
void enqueue(Queue* queue, const char *data, int a);
void exit_command(char *message);
void handle_exit_signal();
void setup_signal_handler();
void signal_exit();
void set_concurrency(char *message_2,Queue *Q,int cs);
void poll(int clientSocket);
void Stop(char *message_2,int clientSocket);