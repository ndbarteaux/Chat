#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
typedef int bool;
#define true 1
#define false 0
#define MAXBYTES 1400

void DieWithError(char *errorMessage) {
    perror(errorMessage);
       exit(1);
}

int main(int argc, char *argv[]) {

	if(argc == 1) {

		//SERVER
		int sockfd, new_fd;
		struct sockaddr_in address; //address
		struct sockaddr_in client_address; //client address
		struct hostent *h;
		int max_len = 141;
		int sin_size;
		char host[100];
		char message[max_len];
		printf("Welcome to chat!\n");

		if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
			DieWithError( "socket () failed");
		}

		int yes=1;
		// lose the pesky "Address already in use" error message
		if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
		}
	
		memset(&address, 0, sizeof(address)); //Reset Memory
		address.sin_family = AF_INET;
		address.sin_port = htons(0); // choose an unused port at random
		address.sin_addr.s_addr = htonl(INADDR_ANY); // use my IP address

		if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
			 DieWithError ( "bind () failed");
		}

		 if (listen(sockfd, 1) < 0) {   	//Listen for connections
		 	 DieWithError("listen() failed");
		 }

		 socklen_t len = sizeof(address);
		 int i = gethostname(host, sizeof(host));
		 h = gethostbyname(host);
		 getsockname(sockfd, (struct sockaddr *)&address, &len);

		 printf("Waiting for a connection on %s port %d\n", inet_ntoa(*((struct in_addr *)h->h_addr)), (int) ntohs(address.sin_port));

		 sin_size = sizeof(struct sockaddr_in);
		 new_fd = accept(sockfd, (struct sockaddr *)&client_address, &sin_size);

		 printf("Found a friend! You receive first\n");
		 
		 int numInBytes;
		 char array[MAXBYTES];
		 int numbytes;
 		unsigned int Servlen;
 		size_t length;
 		unsigned int version;
 		unsigned char outBuf[MAXBYTES];
 		uint16_t neVersion;
 		uint16_t neLen;
 		char inBuf[MAXBYTES];
 		char *inMsg;
 		int bufLen;
		bool longInput = 0;
		 
		 // Continually loops
		 while(1) {
          
			 if(!longInput) {
				 char inBuf[150];
             	if ((numInBytes=recv(new_fd, inBuf, sizeof(inBuf), 0)) == -1) {
			 	   perror("recv"); exit(1); 
		  		} 
			 	 char *inMsg = inBuf + 4; 
			 	 printf("Received: %s\n",inMsg);
				 longInput = 0;
		 	 }
			  
  			// Clear Array and copy client message
  			memset(array, 0, MAXBYTES);
  			printf("You: ");
			longInput = 0;
  			fgets(array, MAXBYTES, stdin);
  			length = strlen(array);
  			if(array[length - 1] == '\n') {
  				array[length-1] = '\0';
  			}
  			char buffer[length];
  			strncpy(buffer, array, length);
  			buffer[length] = '\0';
  			// Error if over 140 characters
  			if(strlen(array) > 140) {
  				printf("Error: Input too long.\n");
				longInput = 1;
  			} else {

  				Servlen = strlen(buffer);
  				version = 457;
  				bufLen = sizeof(version)+len+sizeof(Servlen);
  				unsigned char outBuf[bufLen];
  				memset(outBuf, 0, bufLen);
  						//	char * p = outBuf;
  				neVersion = htons(version);
  				outBuf[0] = neVersion & 0xFF;
  				outBuf[1] = neVersion >> 8;

  						//	memcpy(p, &neVersion, sizeof(neVersion));
  						//	p += sizeof(neVersion);

  				neLen = htons(Servlen);
  				outBuf[2] = neLen & 0xFF;
  				outBuf[3] = neLen >> 8;

  				memmove(outBuf + 4, buffer, strlen(buffer));

  						//	memcpy(p, &neLen, sizeof(neLen));
  						//	p+= sizeof(neLen);

  						//	memcpy(p, buffer, sizeof(buffer));
  						//	p+= sizeof(buffer);
  			  numbytes = send(new_fd, outBuf, sizeof(outBuf), 0);
		 	}
		}

	}
	
	else if (argc == 2 && (strcmp(argv[1], "-h") == 0)) {
		printf("Usage:\nServer: ./chat\nClient: ./chat -s server_ip -p port\n");
	}

	else if(argc == 5) {
		//CLIENT

		int sockfd, numbytes, numInBytes;
		char array[MAXBYTES];
		char* servIP;
		struct sockaddr_in their_addr;

		printf("Connecting to server...\n");

		if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
			perror("socket");
			exit(1);
		}
		
		memset(&their_addr, 0, sizeof(their_addr));
		if((strcmp(argv[1], "-s") == 0) && (strcmp(argv[3], "-p") == 0)) {
			servIP = argv[2];
			their_addr.sin_port = htons(atoi(argv[4])); // short, network
		} else if ((strcmp(argv[1], "-p") == 0) && (strcmp(argv[3], "-s") == 0)) {
			servIP = argv[4];
			their_addr.sin_port = htons(atoi(argv[2])); // short, network
		}
		their_addr.sin_family = AF_INET; // host byte order
		their_addr.sin_addr.s_addr = inet_addr(servIP);

		if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(their_addr)) == -1) {
			perror("connect");
			exit(1);
		}

		printf("Connected!\n");
		printf("Connected to a friend! You send first.\n");


		unsigned int len;
		size_t length;
		unsigned int version;
		unsigned char outBuf[MAXBYTES];
		uint16_t neVersion;
		uint16_t neLen;
		char inBuf[MAXBYTES];
		char *inMsg;
		int bufLen;

		while(1) {

			// Clear Array and copy client message
			memset(array, 0, MAXBYTES);
			printf("You: ");
			fgets(array, MAXBYTES, stdin);
			length = strlen(array);
			if(array[length - 1] == '\n') {
				array[length-1] = '\0';
			}
			char buffer[length];
			strncpy(buffer, array, length);
			buffer[length] = '\0';
			// Error if over 140 characters
			if(strlen(array) > 140) {
				printf("Error: Input too long.\n");
			} else {

				len = strlen(buffer);
				version = 457;
				bufLen = sizeof(version)+len+sizeof(len);
				unsigned char outBuf[bufLen];
				memset(outBuf, 0, bufLen);
						//	char * p = outBuf;
				neVersion = htons(version);
				outBuf[0] = neVersion & 0xFF;
				outBuf[1] = neVersion >> 8;

						//	memcpy(p, &neVersion, sizeof(neVersion));
						//	p += sizeof(neVersion);

				neLen = htons(len);
				outBuf[2] = neLen & 0xFF;
				outBuf[3] = neLen >> 8;

				memmove(outBuf + 4, buffer, strlen(buffer));

						//	memcpy(p, &neLen, sizeof(neLen));
						//	p+= sizeof(neLen);

						//	memcpy(p, buffer, sizeof(buffer));
						//	p+= sizeof(buffer);
			  numbytes = send(sockfd, outBuf, sizeof(outBuf), 0);
			  
              char inBuf[150];
              if ((numInBytes=recv(sockfd, inBuf, sizeof(inBuf), 0)) == -1) {
				  perror("recv"); exit(1); 
			  } 
				  char *inMsg = inBuf + 4; printf("Received: %s\n",inMsg);



			}
		}
	}

}
