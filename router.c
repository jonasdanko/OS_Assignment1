/*-------------------------------------------------------------------
File: router.c

Name: Jonas Danko
Student Number: 8704351

Description: This is a Router designed to foward messages between User and Server.
-------------------------------------------------------------------*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

/*-------------------------------------------------------------------
Function: main

Description: The main function processes the command line to get the
             pipes' file descriptors which it stores in a string.
             10 points each function.
-------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    //char *argv[]
    // 8. Router connects with User and Server via pipes
    // 9. Router receives any data from User and forwards the data to the Server
    // 10. Router receives any data from Server and forwards the data to the User

    char User2Routerfd[100];
    char test[5];
    int fdReadUser, fdWriteUser;
    int fdReadServer, fdWriteServer;
    char Router2Userfd[100];
    char test_string[10];
    char time_command[1];
    char server_response[12];

    
    if(argc != 5)
    {
      printf("Usage: router <param: \"int int int int\">\n");
      return 1;
    }
    
    fdReadUser = atoi(argv[1]);
    fdWriteUser = atoi(argv[2]);
    fdReadServer = atoi(argv[3]);
    fdWriteServer = atoi(argv[4]);

    read(fdReadUser, test_string, 10);
    close(fdReadUser);
    //printf("Working\n");
    printf("(router) Message from user: %s\n", test_string);
    //printf("%s\n", test_string);
    //write(fdWriteUser, test_string, 10);
    close(fdWriteUser);

    //sleep(2);

    //close(fdReadServer);
    write(fdWriteServer, test_string, 10);
    sleep(2);
    read(fdReadServer, server_response, 10);
    printf("Response from server: %s\n", server_response);
    close(fdWriteServer);

    return 0;
}
