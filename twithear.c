#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>  
#include <arpa/inet.h>  
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

int main(int argc, char *argv[]){

	while(1){	
		int sockdesc, port, n;
		struct sockaddr_in serv_addr;
		struct hostent *server;
		int count_chr;
		char buffer[140];

		if (argc < 3) {
			fprintf(stderr,"usage %s hostname port\n", argv[0]);
			exit(0);
		}

		port = atoi(argv[2]);
		sockdesc = socket(AF_INET, SOCK_STREAM, 0);

		if (sockdesc < 0) perror("socket error");

		server = gethostbyname(argv[1]);

		if (server == NULL) {
			fprintf(stderr,"wrong host\n");
			exit(0);
		}

		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)server->h_addr, 
              	      (char *)&serv_addr.sin_addr.s_addr, server->h_length);

		serv_addr.sin_port = htons(port);
		if (connect(sockdesc, &serv_addr, sizeof(serv_addr)) < 0) 
			perror("connection error");

		bzero(buffer,141);   

		n = read(sockdesc,buffer,140);
		if (n < 0) perror("socket read error");
		if (n > 0) printf("%s\n",buffer);
		
	}

    return 0;
}
