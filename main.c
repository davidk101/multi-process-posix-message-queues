/***************************************************************************
*
* File: main.c
* Author: David Kumar (DDK170002)
* Procedures:
    * main - the main function create a multi-process and then makes computations on the server side followed by displaying results to the client.
*
***************************************************************************/

#include <unistd.h> /* file needed for getdomainname*/
#include <sys/utsname.h> /* file needed for uname */
#include <fcntl.h>  /* file needed for mq_open*/
#include <sys/stat.h> /* file needed for O constants */
#include<fcntl.h> /* file needed for O flag definitions */
#include <mqueue.h> /* file needed for message queue */
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>                                                                                      

/***************************************************************************
* 
* int main( int argc, char *argv[] )
* Author: David Kumar
* Due: 02/05/2022
* Description: the main function creates two message queues in addition to a client and server process. 
* It then computes three function calls (getdomainname, gethostname, uname) in the server process followed by displaying their outputs in the client process.
*
* Parameters:
    * argc I/P int The number of arguments on the command line
    * argv I/P char *[] The arguments on the command line
    * main O/P int Status code (not currently used)
**************************************************************************/

int main(int argc, char *argv[]){  

    char argument[1024]; /* "argument" is used to store the command line argument from the user. */

    /* enusring only one approprate command line argument used out of the following: getdomainname, gethostname, uname */
    if (argc > 2){

        printf("Error. Too many command line arguments provided. Please provide only one.\n");
        return EXIT_FAILURE;
    }

    else if (argc == 0){

        printf("Error. Must add at least one command line argument.\n");
        return EXIT_FAILURE;
    }

    else if (strcmp(argv[1], "getdomainname") == 0){

        /* user chooses to getdomainname */
        (void)strcpy(argument, "getdomainname");
        printf("User attempts to getdomainname.\n");
    }

    else if (strcmp(argv[1], "gethostname") == 0){

        /* user chooses to gethostname */
        (void)strcpy(argument, "gethostname");
        printf("User attempts to gethostname.\n");
    }

    else if (strcmp(argv[1], "uname") == 0){

        /* user chooses to get uname */
        (void)strcpy(argument, "uname");
        printf("User attempts to get uname.\n");
    }

    else{

        printf("Error. Improper command line argument used.\n");
        return EXIT_FAILURE;
    }

    /* creating a message queue for the client to server communication. */

    mqd_t descriptor_queue1 = mq_open("/queue1", O_RDWR | O_CREAT, 0777, NULL);

    if (descriptor_queue1 == -1){

        printf("Error. Failed to create message queue.\n");
        return EXIT_FAILURE;
    }

    else{

        printf("Message queue for client to server communication created.\n");
    }

    /* creating a second message queue for server to client communication. */

    mqd_t descriptor_queue2 = mq_open("/queue2", O_RDWR | O_CREAT, 0777, NULL);

    if (descriptor_queue2 == -1){

        printf("Error. Failed to create message queue.\n");
        return EXIT_FAILURE;

    }

    else{

        printf("Message queue for server to client communication created.\n\n");

    }

    /* creating a fork to start a client and a server process. */

    pid_t pid = fork();

    /* entering the client process. */
    if (pid == 0) {

        /* sending a message from the client to the server through the first queue */

        if (mq_send(descriptor_queue1, argument, (strlen(argument) * sizeof(char)) + 1, 0) == -1){

            printf("Error. Failed to send message from client to server.\n");
            return EXIT_FAILURE;

        }

        else{

            printf("Client: Message containing command line argument sent to Server.\n");

        }

        char new_message[8192]; /* storing the final output. */

        /* receiving a message from the server through the second queue */

        if (mq_receive(descriptor_queue2, new_message, 8192, 0) == -1){

            printf("Error. Failed to retrieve message from server.");
            return EXIT_FAILURE;

        }

        else{

            printf("Client Output: \n%s", new_message);

        }
    }
    else {

        /* entering the server process. */

        char message[8192]; /* storing the output of the command line argument from the client. */

        /* receiving a message from the client through the first queue */

        if (mq_receive(descriptor_queue1, message, 8192, 0) == -1){

            printf("Error. Failed to retrieve message from client.\n");
            return EXIT_FAILURE;

        }

        else{

            printf("Server: Message received from Client.\n");
        }

        char new_message[8192]; /* storing the output of the final output after necessary function calls. */

        /* computing getdomainname in the server process. */
        if (strcmp(message, "getdomainname") == 0){

            char domainname[1024];
            domainname[1023] = '\0';
            getdomainname(domainname, 1023);
            (void)strcpy(new_message, domainname);
        }

        /* computing gethostname in the server process. */
        else if (strcmp(message, "gethostname") == 0){

            char hostname[1024];
            hostname[1023] = '\0';
            gethostname(hostname, 1023);
            (void)strcpy(new_message, hostname);

        }

        /* computing uname in the server process. */
        else if (strcmp(message, "uname") == 0){

            struct utsname my_uname;
            if (uname(&my_uname) == -1){

                printf("Error. Failed to call uname.\n");
                return EXIT_FAILURE;

            }
            else{
                
                /* computing the various attributes part of the uname method call. */
                char my_uname_text[16384];
                strcat(my_uname_text, "OS: ");
                strcat(my_uname_text, my_uname.sysname );
                strcat(my_uname_text, "\nNetwork node: ");
                strcat(my_uname_text, my_uname.nodename );
                strcat(my_uname_text, "\nRealease level: ");
                strcat(my_uname_text, my_uname.release);
                strcat(my_uname_text, "\nVersion level: ");
                strcat(my_uname_text, my_uname.version);
                strcat(my_uname_text, "\nHardware Type");
                strcat(my_uname_text, my_uname.machine);

                (void)strcpy(new_message, my_uname_text);
            }
        }
        else{

            printf("Error retrieving appropriate command line argument.\n");
            return EXIT_FAILURE;

        }

        /* sending message from the server to the client through second queue after computing getdomainname, gethostname, uname. */

        if (mq_send(descriptor_queue2, new_message, (strlen(new_message) * sizeof(char)) + 1, 0) == -1){

            printf("Error. Failed to send message from server to client.\n");
            return EXIT_FAILURE;
        }

        else{
            printf("Server: Message with output sent to Client.\n");
        }
       
        wait(NULL); /* suspending the calling process to ensure terminating child is not a zombie. */

        /* closing both message queues. */

        if (mq_close(descriptor_queue1) == -1){

            printf("Error. Failed to close message queue.\n");
            return EXIT_FAILURE;

        }

        else{

            printf("\n\nSuccessfully closed the first message queue.\n");

        }

        if (mq_close(descriptor_queue2) == -1){

            printf("Error. Failed to close message queue.\n");
            return EXIT_FAILURE;
        }

        else{

            printf("Successfully closed the second message queue.\n");
        }

        /* unlinking both message queues. */

        if (mq_unlink("/queue1") == -1){

            printf("Error. Failed to unlink message queue.\n");
            return EXIT_FAILURE;

        }

        else{

            printf("Successfully unlinked the first message queue.\n");
        }

        if (mq_unlink("/queue2") == -1){

            printf("Error. Failed to unlink message queue.\n");
            return EXIT_FAILURE;

        }

        else{

            printf("Successfully unlinked the second message queue.\n");
        }

    }

    return EXIT_SUCCESS;
}
