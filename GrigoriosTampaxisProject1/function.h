#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define BUFFER_SIZE 512


extern volatile sig_atomic_t sigusr2_received;
extern volatile sig_atomic_t sigchild_received;

typedef struct JOBS {
    char tripleta[BUFFER_SIZE];     //contains the tripleta of the command 
    char jobID[BUFFER_SIZE] ;       //contains the job_id of the command
    char job[BUFFER_SIZE] ;         //contains the job of the command  
    int queue_position;             //position of the command in the waiting queue
    int active;
    struct JOBS* ahead_of;          //pointer to the command that is behind this node in the waiting queue
    struct JOBS* behind_of;         //pointer to the command that is in front of this node in the waiting queue
} JOBS;


typedef struct Queue {
    JOBS* front; 
    JOBS* rear;  
    int number_of_jobs;           //number of jobs in queue
    int concurrency;              //concurrency value
} Queue;


typedef struct active_JOBS {
    char tripleta[BUFFER_SIZE];     //contains the tripleta of the command 
    char jobID[BUFFER_SIZE] ;       //contains the job_id of the command
    char job[BUFFER_SIZE] ;         //contains the job of the command  
    int active_Queue_position;      //position of the command in the active queue
    int active;                     //shows whether the command is still active
    pid_t pid;                      //pid of the command
    struct active_JOBS* ahead_of;   //pointer to the command that is behind this node in the active queue
    struct active_JOBS* behind_of;  //pointer to the command that is in front of this node in the active queue
} active_JOBS;


typedef struct active_Queue {
    active_JOBS* front;         //the variables of this struct basically have the same role as the variables in the Queue struct
    active_JOBS* rear;  
    int number_of_active_JOBS;
    int concurrency;
} active_Queue;


Queue* initializeQueue() ;

int isEmpty(Queue* ) ;

JOBS* createNode(int ,char *,char *,char *tripleta,JOBS* ) ;

void enqueue(Queue* , int ,char *,char *,char *) ;
void dequeue(Queue* ) ;

char* get_command(Queue* ) ;
char* get_jobID(Queue* );
int get_concurrency(Queue* );
void printQueue(Queue* ) ;

void freeQueue(Queue* ) ;

void poll_queued(int ,char *,Queue*);
void poll_running(int ,char *,active_Queue* );


int number_of_active_commands(active_Queue*);

void set_concurrency(char *,Queue*);

void sigusr2_handler(int);

void sigchld_handler(int);

int exit_command(char *,int);

int issueJob(Queue* ,active_Queue*, char *,int ,int );

active_Queue* initializeactive_Queue() ;

int isEmpty_active(active_Queue* );

active_JOBS* createNode_active(int ,char *,char *,char *,active_JOBS* ,pid_t) ;

void enactive_Queue(active_Queue* , int ,char *,char *,char *,pid_t) ;

void active_deQueue(active_Queue* ) ;

void printactive_Queue(active_Queue* ) ;

void freeactive_Queue(active_Queue* ) ;

void Stop(Queue* ,active_Queue *,char *,int);

void addactiveJobIDs(active_Queue* , char* );

void addJobIDs(Queue* , char* );