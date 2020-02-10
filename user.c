/*-------------------------------------------------------
File: user.c

Name: Jonas Danko
Student Number: 8704351

Description: This program is designed to test the severs
             program using pipes.
             Please follow the instructions in the comments
--------------------------------------------------------*/

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <signal.h>

// Utilities

// Time Information Structure
typedef struct TIME_INFO
{
    int hour;
    int minute;
    int second;
}TimeInfo;

// This function would close the pipe
void closePipe(int *pipe)
{
    close(pipe[0]);
    close(pipe[1]);
}

// This function will get current time and store it in TimeInfo struct
TimeInfo getTimeInfo()
{
    time_t now;
    struct tm *tm_now;
    time(&now);
    tm_now = localtime(&now);
    
    TimeInfo timeInfo = {tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec};
    return timeInfo;
}

// This function is used for print the time information storing in the TimeInfo
void printTimeInfo(TimeInfo timeInfo)
{
    printf("%d@%d:%d:%d\n", getpid(), timeInfo.hour, timeInfo.minute, timeInfo.second);
}

// This function is to print the current time
void printTime()
{
    TimeInfo timeInfo = getTimeInfo();
    printTimeInfo(timeInfo);
}

// read function in block mode, same usage as read(int fd, const void *buf, size_t count)
int blockRead(int fd, const void *buf, size_t count)
{
    int readNum = 0;
    for(;readNum < count;)
    {
        char data = 0;
        int retVal = read(fd, &data, sizeof(data));
        if(retVal > 0)
        {
            readNum += sizeof(data);
            *(char *)buf = data;
            (char *)buf++;
        }
    }
    return readNum;
}


// define your parameters here
int queryTimesMax = 0;
int queryInterval = 0;
int serverPrintTimesMax = 0;
int serverPrintInterval = 0;

#define QueryCommand 't' // Used for Querying Time
#define QuitCommand 'q'

void *callTimeFunction(){
    for(int i = 0 ; i<2 ; ++i){
        printTime();
        sleep(2);
    }
}

/*---------------------------------------------------------------
Function: main

Description: Complete the functions listed in the comments.
             Each function is 10 points.

Assignment: Complete this function to ensure proper calls to 
            the other functions.
---------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    // You may liek to use the utility functions provided above //

    int userpid, routerpid, serverpid;

    char readFDRouter[5];
    char writeFDRouter[5];
    char readFromServer[5];
    char writeToServer[5];

    int pipeUser2Routerfd[2];
    int pipeRouter2Userfd[2];
    int pipeRouter2Serverfd[2];
    int pipeServer2Routerfd[2];
    
    char *routerArgs[6];

    char messageFromRouter[10];
    


    //printf("Everything is working fine");
    
    // Create pipes
	if(pipe(pipeUser2Routerfd) == -1){
        printf("Pipe 1 creation failed\n");
        exit(-1);
    }
    if(pipe(pipeRouter2Userfd) == -1){
        printf("Pipe 1 creation failed\n");
        exit(-1);
    }
    if(pipe(pipeRouter2Serverfd) == -1){
        printf("Pipe 1 creation failed\n");
        exit(-1);
    }
    if(pipe(pipeServer2Routerfd) == -1){
        printf("Pipe 1 creation failed\n");
        exit(-1);
    }
    
    // Create user process
    /* Stuff for router */
    //FD's for user - router pipes
    sprintf(readFDRouter, "%d", pipeUser2Routerfd[0]);
    sprintf(writeFDRouter, "%d", pipeRouter2Userfd[1]);

    sprintf(readFromServer, "%d", pipeServer2Routerfd[0]);
    sprintf(writeToServer, "%d", pipeRouter2Serverfd[1]);

    routerArgs[0] = "router" ;
    routerArgs[1] = readFDRouter;
    routerArgs[2] = writeFDRouter;
    routerArgs[3] = readFromServer;
    routerArgs[4] = writeToServer;
    routerArgs[5] = NULL;

    routerpid = fork();
    if(routerpid == -1){
        printf("First fork failed (router)");
        exit(-1);
    }
    else if(routerpid == 0){   /* child (start router) */
        //Start router process. May need to connect pipe here also.
        close(pipeUser2Routerfd[1]);
        //close(pipeRouter2Userfd[1]);
        //close(pipeRouter2Userfd[0]);

        printf("about to start process: router\n");
        execv("router", routerArgs);
        exit(-1); /* In case router fails */
    }

    
    close(pipeUser2Routerfd[0]);
    //close(pipeUser2Routerfd[1]);
    //close(pipeRouter2Userfd[1]);
    //close(pipeRouter2Userfd[0]);

    printf("Sending msg from user: hello\n");
    write(pipeUser2Routerfd[1], "hello", strlen("hello")+1);

    //wait();
    //read(pipeRouter2Userfd[0], messageFromRouter, 10);
    //printf("(User) Message from router: %s\n", messageFromRouter);
    //close(pipeRouter2Userfd[0]);


    serverpid = fork();
    if(serverpid == -1){
        printf("Second fork failed (server)");
    }
    else if(serverpid == 0){
        char routerMsg[10];
        char response[] = "heyfromserver";
        close(pipeRouter2Serverfd[1]);
        printf("About to start process: server\n");
        /*
        for(int t=0 ;t<100; ++t){
            if(read(pipeRouter2Serverfd[0], routerMsg, 10)>0){
                break;
            }
        }
        */

        pthread_t timeThread;
        pthread_create(&timeThread, NULL, callTimeFunction(), NULL);

        read(pipeRouter2Serverfd[0], routerMsg, 10);
        printf("(Server) message from router: %s\n", routerMsg); 
        write(pipeServer2Routerfd[1], response, 12);
        close(pipeRouter2Serverfd[0]);
        close(pipeServer2Routerfd[1]);
    }

    //sleep(8);

    //Start server process
    
    // Create Server process
    // 1. Server process should create a thread to display current time for at least 2 times
    // 2. Server should respond the following command from Router
    //     -"t": send current time to the router
    //     -"q": stop waiting for the command, kill thread and exit.
    // 3. Server should end the process by itself instead of User process
    
    // Create User process
    // 4. User queries time for at least 2 times by sending 't' command to Router
    // 5. Then User sends 'q' command to Router
    // 6. User waits for the Server ending itself
    // 7. User kills the Router process
    

    //Create server process
    /*
    serverpid = fork();
    if(serverpid == -1){
        printf("Second fork failed (server process).");
        exit(-1);
    }else if(serverpid == 0){
        execlp();

        printf();
    }
    */
    
    
    return 0;
}
