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

	char *buffer= malloc(sizeof(char)*141);
	int k;

 
	pid_t pid;
 

	while(1){
		pid=fork();
	 	char *args[4];
		
		if(pid==0){

		
		fgets(buffer,140,stdin);
		buffer[strlen(buffer)] = '\0';
		//k=strlen(buffer);
		
 
		args[0]="./twitsay";
		args[1]="localhost";
		args[2]="3331";
		args[3]=buffer;
		execv("./twitsay",args);


		} else if(pid==-1){
		  perror("error");
		  }
	
		  else if(pid>0){
		  wait(pid);}

	}

	return 0;
}
