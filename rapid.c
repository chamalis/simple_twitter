#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
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

	char buffer[140];
	int i, j, status;
	char rew;
	char *args[4];
 
	pid_t pid;

	srand(time (NULL));
	while(1){
		
	 	printf("mpainoume sth while\n");
		
		//if(pid==0){

		puts("arxid");
		for (j=0; j<140; j++){

			rew = rand () % 27+97;

			buffer[j] = rew;

			//buffer[140] = '\0';

			printf("kai sth for\n");}
			buffer[140] = '\0';
 
		args[0]="./twitsay";
		args[1]="localhost";
		args[2]="3331";
		args[3]=buffer;	


		//}
 			if((pid=fork())!=0){
			waitpid(pid, &status, 0);
		}
	
		  else {execv("./twitsay",args);}

	}

	return 0;
}


