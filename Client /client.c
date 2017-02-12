/*

  ECHOCLNT.C
  ==========
  (c) Paul Griffiths, 1999
  Email: mail@paulgriffiths.net
  
  Simple TCP/IP echo client.

*/


#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */
#include "helper.h"           /*  Our own helper functions  */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/*  Global constants  */

#define MAX_LINE           (1000)


/*  Function declarations  */

int ParseCmdLine(int argc, char *argv[], char **szAddress, char **szPort);


/*  main()  */

int main(int argc, char *argv[]) {

    int       conn_TCP;                /*  connection socket  for TCP */
    int 	  conn_UDP;				/* connection socket for UDP */
    int       nBytes;
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    struct 	  sockaddr_in clientaddr;
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *szAddress; //(char*) malloc(4*sizeof(char));             /*  Holds remote IP address   */
    char     *szPort; // (char*) malloc(4*sizeof(char));                /*  Holds remote port         */
    char     *endptr;                /*  for strtol()              */
    char      buffer_send[MAX_LINE];
    char      capital_buffer[MAX_LINE];
    char      file_buffer[MAX_LINE];
    FILE      *final_file;

    /*  Get command line arguments  */

    ParseCmdLine(argc, argv, &szAddress, &szPort);


    /*  Set the remote port  */
    port = strtol(szPort, &endptr, 0);
    if ( *endptr ) {
	printf("ECHOCLNT: Invalid port supplied.\n");
	exit(EXIT_FAILURE);
    }
	

    /*  Create the listening socket for TCP  */

    if ( (conn_TCP = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
	fprintf(stderr, "ECHOCLNT: Error creating listening socket.\n");
	exit(EXIT_FAILURE);
    }


    /* Create the listening socket for UDP */

    if ( (conn_UDP = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
	fprintf(stderr, "ECHOCLNT: Error creating listening socket.\n");
	exit(EXIT_FAILURE);
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);


    /*  Set the remote IP address  */

    if ( inet_aton(szAddress, &servaddr.sin_addr) <= 0 ) {
	printf("ECHOCLNT: Invalid remote IP address.\n");
	exit(EXIT_FAILURE);
    }

    
    /*  connect() to the remote echo server  */

    if ( connect(conn_TCP, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) {
	printf("ECHOCLNT: Error calling connect()\n");
	exit(EXIT_FAILURE);
    }


    /*  Get string to echo from user  */
   

    do{

        printf("Enter the command: ");
        fgets(buffer, MAX_LINE, stdin);
        

         if (strlen(buffer) > 2)
         {
             break;
         }

        int temp_1 = strncmp(buffer, "s", 1);
        int temp_2 = strncmp(buffer, "t", 1);
        int temp_3 = strncmp(buffer, "q", 1);

        //printf("Buffer is %s and its length is %s and strncmp gives %d", buffer, strlen(buffer), strncmp(buffer, "s",1));
        if (strlen(buffer) == 2 && temp_1 == 0)
        {
            printf("\nEnter the string to capitalize:");
            fgets(buffer, MAX_LINE, stdin);

            strcpy(buffer_send, "CAP\n");
            strcat(buffer_send,buffer);
            strcat(buffer_send,"\n");
            

            sendto(conn_UDP, buffer_send, strlen(buffer_send),
                       0, (struct sockaddr *)&servaddr,
                       sizeof(servaddr));

            recvfrom(conn_UDP, buffer, strlen(buffer), 0, (struct sockaddr *)&clientaddr, sizeof(servaddr));


            
            
            

            printf("The string in Uppercase: %s\n",buffer_send);
           
            
        }
        
        
        else if (temp_2 == 0)
        {
            printf("\nEnter the string to find the file:\n");
            fgets(buffer,MAX_LINE,stdin);

            strcpy(buffer_send, "FILE\n");
            strcat(buffer_send,buffer);
            strcat(buffer_send, "\n");


            printf("%s",buffer_send);

            write(conn_TCP, buffer_send, strlen(buffer_send));
            read(conn_TCP, buffer, MAX_LINE-1);
            
            if (strncmp(buffer_send + 2, "NOT FOUND", 9) == 0)
            {
                printf("%s", buffer_send + 2);
            }

            else
            {
                final_file = fopen(file_buffer, "wb");
                printf("%s", "The content of the file from server can be found in the clientfile named:\n", file_buffer);
                fclose(final_file);
                free(file_buffer);
            }
        } 
        
        
        else if (temp_3 == 0)
        {
            return EXIT_SUCCESS;
        }

        memset(buffer, 0, (sizeof buffer[0]) * MAX_LINE);
        memset(buffer_send, 0, (sizeof buffer_send[0]) *MAX_LINE);

    //printf("buffer is %s and length is %d ", buffer, strlen(buffer));

    } while ((strlen(buffer) == 2) && (strncmp (buffer , "q", 1) != 0));


   
    

    
    
    if  (close(conn_TCP) < 0) {
        printf("%s", "Error calling close()\n");
        exit(EXIT_FAILURE);


    }
    /*  Send string to echo server, and retrieve response  */


    /*  Output echoed string  */


    return EXIT_SUCCESS;
}


int ParseCmdLine(int argc, char *argv[], char **szAddress, char **szPort) {

    

    
	
	    *szAddress = argv[1];
	
	
	    *szPort = argv[2];
	
	
	

    return 0;
}
