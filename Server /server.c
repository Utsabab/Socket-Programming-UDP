
/*

  ECHOSERV.C
  ==========
  (c) Paul Griffiths, 1999
  Email: mail@paulgriffiths.net
  
  Simple TCP/IP echo server.

*/


#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "helper.h"           /*  our own helper functions  */


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


/*  Global constants  */

#define ECHO_PORT          (2002)
#define MAX_LINE           (1000)


int main(int argc, char *argv[]) {
    int       list_TCP;                /*  listening socket  TCP         */
    //int       nBytes;
    int       list_UDP; 
    int       i;
    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    struct    sockaddr_in clientaddr;
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *endptr;                /*  for strtol()              */
    char      buffer_send[MAX_LINE];
    char      capital_buffer[MAX_LINE];
    char      file_buffer[MAX_LINE];
    FILE      *final_file;
    long      lSize;
    size_t    result;
    int       bytesReceived;




    /*  Get port number from the command line, and
        set to default port if no arguments were supplied  */

    if ( argc == 2 ) {
	port = strtol(argv[1], &endptr, 0);
	if ( *endptr ) {
	    fprintf(stderr, "ECHOSERV: Invalid port number.\n");
	    exit(EXIT_FAILURE);
	}
    }
    else if ( argc < 2 ) {
	port = ECHO_PORT;
    }
    else {
	fprintf(stderr, "ECHOSERV: Invalid arguments.\n");
	exit(EXIT_FAILURE);
    }

	
    /*  Create the listening socket for TCP  */

    if ( (list_TCP = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
	fprintf(stderr, "ECHOSERV: Error creating listening socket.\n");
	exit(EXIT_FAILURE);
    }

    /* Create the listening socket for UDP */
    if ( (list_UDP = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    fprintf(stderr, "ECHOSERV: Error creating listening socket.\n");
    exit(EXIT_FAILURE);
    }



    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);


    /*  Bind our socket addresss to the 
	listening socket, and call listen()  */

    if ( bind(list_TCP, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
	fprintf(stderr, "ECHOSERV: Error calling bind()\n");
	exit(EXIT_FAILURE);
    }

    if ( listen(list_TCP, LISTENQ) < 0 ) {
	fprintf(stderr, "ECHOSERV: Error calling listen()\n");
	exit(EXIT_FAILURE);
    }

    /* Bind UDP on the socket */
    if ( bind(list_UDP, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
    fprintf(stderr, "ECHOSERV: Error calling bind()\n");
    exit(EXIT_FAILURE);
    }

    
    /*  Enter an infinite loop to respond
        to client requests and echo input  */

    while ( 1 ) {

	/*  Wait for a connection, then accept() it  */

	if ( (conn_s = accept(list_TCP, NULL, NULL) ) < 0 ) {
	    fprintf(stderr, "ECHOSERV: Error calling accept()\n");
	    exit(EXIT_FAILURE);
	}

   


	/*  Retrieve an input line from the connected socket
	    then simply write it back to the same socket.     */

	   

       bytesReceived = recvfrom(list_UDP, buffer, strlen(buffer), 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr));


       //printf("%s", buffer);



     
        int temp_1 = strncmp(buffer, "CAP", 3);
        if (temp_1 == 0){
            memcpy(capital_buffer, buffer + 4, strlen(buffer) - 6);
            //fprintf(stderr, "Real message: %s\n", capital_buffer);
            
        
            for (i=0;i<strlen(capital_buffer);i++){
              capital_buffer[i] = toupper(capital_buffer[i]);
            }

            sprintf(buffer_send, "%d", strlen(capital_buffer));
            strcat(buffer_send, "\n");
            strcat(buffer_send, capital_buffer);

            sendto(list_UDP, buffer, strlen(buffer), 0, (struct sockaddr *)&clientaddr, sizeof(clientaddr));  

        }

        int temp_2 = strncmp(buffer_send, "FILE", 4);
        if (temp_2 == 0){
            memcpy(file_buffer, buffer + 5, strlen(buffer) - 7);

            final_file = fopen(file_buffer, "rb");
            if (final_file==NULL) {
                fputs ("FIle error", stderr);
                exit (1);
            }

            //obtain file size
            fseek (final_file, 0, SEEK_END);
            lSize = ftell (final_file);
            rewind (final_file);

            //allocate memory to contain the whole file
            *buffer = (char*) malloc (sizeof(char)*lSize);
            if (buffer == NULL)
            {
                fputs ("Memory error", stderr);
                exit (2);
            }

            //copy the file into the buffer
            result = fread (buffer, 1, lSize, final_file);
            if (result != lSize)
            {
                fputs ("Reading error", stderr);
                exit (3);
            }

            fclose (final_file);

            sprintf(buffer_send, "%d", lSize);
            strcat(buffer_send, "\n");
            strcat(buffer_send, buffer);


            
            write(conn_s, buffer_send, lSize);

            free (buffer);
        }
        

            else
            {
                strcpy (buffer, "NOT FOUND");
                sprintf(buffer_send, "%d", strlen(buffer));
                strcat(buffer_send, "\n");
                strcat(buffer_send, buffer);
                write(conn_s, buffer_send, strlen(buffer_send));
            }

            memset(buffer, 0, (sizeof buffer[0]) * MAX_LINE);
            memset(buffer_send, 0, (sizeof buffer_send[0]) * MAX_LINE);
            


        }







/*
     if (!fork()){
        nBytes = 1;
        while(nBytes!=0){
            nBytes = recv(conn_s,buffer,nBytes,0);


            for (i=0;i<nBytes-1;i++){
                buffer[i] = toupper(buffer[i]);
            }

            send(conn_s,buffer,nBytes,0);
        }
        close (conn_s);
        exit(0);
    }
*/   




	/*  Close the connected socket  */

	if ( close(conn_s) < 0 ) {
	    fprintf(stderr, "ECHOSERV: Error calling close()\n");
	    exit(EXIT_FAILURE);
	}
}


    