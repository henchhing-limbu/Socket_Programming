#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */
#include "helper.h"           /*  Our own helper functions  */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


//  Global constant
#define MAX_LINE           (1000)

//  Function declarations
// int ParseCmdLine(int argc, char *argv[]);

int main(int argc, char *argv[]) {

    int       	  conn_s;                 //  connection socket         
    short int 	  port;                   //  port number               
    struct    	  sockaddr_in servaddr;   //  socket address structure  
    char          buffer[MAX_LINE];       // character buffer          
    char*         ip_address;             //  Holds remote IP address   
    char*         remote_port;            //  Holds remote port         
    char*     	  endptr;                 //  for strtol()              
	unsigned long filesize;			 	  // size of the file
	unsigned long received_filesize;
	FILE* 		  file_to_send;
	int 		  format;

	/*
    //  Get command line arguments 
    ParseCmdLine(argc, argv, &ip_address, &remote_port);
	
    //  Set the remote port
    port = strtol(remote_port, &endptr, 0);
    if ( *endptr ) {
		printf("ECHOCLNT: Invalid port supplied.\n");
		exit(EXIT_FAILURE);
    }
	*/
	if (argc > 6) {
		printf("More than necessary arguments.\n");
		exit(EXIT_SUCCESS);
	}
	else if (argc < 6) {
		printf("Not enough arguments.\n");
		exit(EXIT_SUCCESS);
	}

	ip_address = argv[1];
	port = atoi(argv[2]);
	int filePathsize = strlen(argv[3]);
	char filePath[filePathsize];
	memcpy(filePath, argv[3], filePathsize);
	printf("filename: %s\n", filePath);
	format = atoi(argv[4]);
	if (format < 0 || format > 3) {
		printf("Incorrect format number.\n");
		exit(EXIT_SUCCESS);
	}
	int filenamesize = strlen(argv[5]);
	char outputfile[filenamesize];
	memcpy(outputfile, argv[5], filenamesize);

	
    //  Create the listening socket
    if ( (conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		fprintf(stderr, "ECHOCLNT: Error creating listening socket.\n");
		exit(EXIT_FAILURE);
    }


    //  Set all bytes in socket address structure to
    //  zero, and fill in the relevant data members
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);


    // Set the remote IP address 
    if ( inet_aton(ip_address, &servaddr.sin_addr) <= 0 ) {
		printf("ECHOCLNT: Invalid remote IP address.\n");
		exit(EXIT_FAILURE);
    }

    
    //  connect() to the remote echo server
    if ( connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) {
		printf("ECHOCLNT: Error calling connect()\n");
		exit(EXIT_FAILURE);
    }
	
	// TODO: temporary fix
	// TODO: need to get arguments from the cmd 
	
	
	// TODO: need change here 
	// TODO: stop hard-coding here
	file_to_send = fopen(filePath,"rb");
	if (file_to_send == NULL) {
		printf("Failed to open the file.\n");
	}
	// moving the pointer to the end of the file
	fseek(file_to_send, 0, SEEK_END);
	
	// finding the size of the file
	// ftell returns current value of the position indicator of the stream
	filesize = ftell(file_to_send);
	// resetting the position indicator to the original state
	rewind(file_to_send);
	
	// read data file to buffer
	fread(buffer,1,filesize, file_to_send);
	printf("Filesize in client: %lu\n", filesize);
	
	// Send file size to server
	printf("Sending file size to the server.\n");
	Writeline(conn_s, &filesize, sizeof(long));
	
	// receive file size from the server
	printf("Receiving file size from the server.\n");
	Readline(conn_s, &received_filesize, sizeof(long));
	
	// Send data to echo server, and retrieve response
	printf("Sending the buffer to the server.\n");
    Writeline(conn_s, buffer, filesize);
	printf("Finished sending buffer to the server.\n");
    // Readline(conn_s, buffer, filesize);
	// printf("Finished reading response from the server.\n");
	
	// sending the format to the server
	Writeline(conn_s, &format, sizeof(int));
	printf("Sending format number to the server.\n");
	
	// sending the output file name to the server
	// sending the filenamesize to the server
	Writeline(conn_s, &filenamesize, sizeof(int));

	// sending the filename to the server
	Writeline(conn_s, outputfile, filenamesize);
	printf("Send filename to the server.\n");
	
	// Getting the error message from the server
	int errorMessage = -1;
	Readline(conn_s, &errorMessage, sizeof(int));
	if (errorMessage < 0)
		printf("Format error\n");
	else
		printf("Success\n");
	if (close(conn_s) < 0) {
		printf("Error closing the connection.\n");
		exit(EXIT_FAILURE);
	}
	
    return EXIT_SUCCESS;
}


