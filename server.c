#include "server.h" //contains all the neccessary headers inclusions

char twit[MAX_TWIT];

/* We are using twits as a round buffer, writing linearly to the right(tail++) 
 * and continuouing from the beginning(0, 1, 2 ... N-1, N, 0, 1 ...) and updating
 * head to point to the first message not transfered yet */
char* twitsDB[MAX_TWITS_NUM]; int tail, head;

/* hearers socket fds */
int hearers[MAX_HEARERS];

struct statistics{
	int threads;
	int sayers;		//connected aka 0 since sayers exit after sending twit
	int hearers;	//overall not connected!
	int twits_stored;
	int twits_delivered;
	int twits_arrived;
} stats;


void sigint_handler(int k) {
	char a; 
	printf("are you sure you want to exit? y/n");
	scanf("%c", &a);
	
	if (a=='y') {exit(0);}
	else return;

}

void sigquit_handler(){

	printf("\n *****\t Server statistics *****\n");
	printf("--------------------------------------\n");
	printf("Active threads\t=\t%d\n", stats.threads);
	printf("Hear connections\t=\t%d\n", stats.hearers);
	printf("Say connections\t=\t%d\n", stats.sayers);
	printf("Twits currently stored\t=\t%d\n", stats.twits_stored);
	printf("Total twits arrived\t=\t%d\n", stats.twits_arrived);
	printf("Total twits delivered\t=\t%d\n", stats.twits_delivered);
}


void init_sig_handlers(){

	struct sigaction sa; 
	
	//when ctrl-C pressed function sigint_handler is called
	sa.sa_handler = sigint_handler;
  	sigemptyset(&sa.sa_mask);
  	sa.sa_flags = SA_RESTART;

	sigaction(SIGINT, &sa, NULL); //SIGQUIT

	//when ctrl-\ is pressed function sigquit_handler is called
	sa.sa_handler = sigquit_handler;
  	sigemptyset(&sa.sa_mask);
  	sa.sa_flags = SA_RESTART;

	sigaction(SIGQUIT, &sa, NULL); //SIGQUIT
}

void init_statistics(){

	stats.twits_stored = 0;
	stats.twits_arrived = 0;
	stats.twits_delivered = 0;
	stats.threads = 0;
	stats.sayers = 0;
	stats.hearers = 0;
}


/* threaded func called by sayers incoming thread, to handle a new say */
void *sayers_handler (int fd) {	 

	int twitlen;
	stats.threads++;
	stats.sayers++;
	
	printf("Accepting incoming messages from sayer with fd %d\n", fd);

	if ((twitlen = read(fd,twit,sizeof(twit)-1)) > 0) { 	
		
		twit[twitlen] = '\0'; 
		//LOCK
		strcpy(twitsDB[tail],twit);
		tail = (tail+1) % MAX_TWITS_NUM;
		stats.twits_stored++;
		stats.twits_arrived++;
		printf("received a new message\n");
		//UNLOCK
	}
	
	close(fd);
	stats.sayers--;
	return(0);
}



void * sayers_thread(void* prama){

	int socket_fd, sayer_fd;
	pthread_t threadID;
	
	//create a socket to hear for incoming connections from sayers (port 3331)
	CHECK_ERRNO (socket_fd = create_server_socket (SAYPORT));

	tail = 0;	//haven't stored any twit yet
	
	/* Continuously accept new sayers and call the corresponded function
	 * handler for each of them */
	while (1) {

		//establishing the connection..
		CHECK_ERRNO (sayer_fd = accept(socket_fd, NULL, NULL));					

		if (pthread_create(&threadID, NULL, (void*)sayers_handler, 
		                                    (void*)sayer_fd) !=0){		
			perror("Thread create error");
			close(sayer_fd);
			close(socket_fd);
			exit(1);
		}
		pthread_join(threadID, NULL);
	}

	close(socket_fd);
}


void * hearers_thread(void* prama){

	int socket_fd, hearer_fd;
	
	//create a socket to hear for incoming connections from hearers (port 3332)
	CHECK_ERRNO (socket_fd = create_server_socket (HEARPORT));

	/* Continuously accept new hearers and call the corresponded function
	 * handler for each of them */
	while (1) {

		//establishing the connection..
		CHECK_ERRNO (hearer_fd = accept(socket_fd, NULL, NULL));					
		if(stats.hearers < MAX_HEARERS){
			/* add hearer to their array so that send_twit func can find it */
			hearers[stats.hearers++] = hearer_fd;	
			printf("enstablished connection with new hearer in fd %d\n", hearer_fd);
		}else{
			perror("Reached maximum number of hearers\n");
		}
	}
	close(socket_fd);
}


/* The threaded function used to deliver twits stored in the twits buffer 
 * to the hearers listening on the socket fds stored in hearers buffer */
void send_twits(){
	int i;

	head = 0;	
	for(;;){
		/* Send every message to all the recipients and then go on
		 * to the next message (head++) */
		if(stats.twits_stored > 0){
			for(i=0; i<stats.hearers; i++){
				if(hearers[i] != -1){
					printf("hearers[%d] = %d\n", i, hearers[i]);
					write_socket(hearers[i], twitsDB[head]);
				}
			}
			printf("twits stored = %d\n", stats.twits_stored);
			stats.twits_delivered++;
			stats.twits_stored--;
			head = (head+1) % MAX_TWITS_NUM;
		}
	}
}


/* write twit to the socket with sock_fd. A hearer should hearing on them
 * corresponding socket */
void write_socket(int sock_fd, char * twit){

	int pos, len,rc;

	pos = 0;
	len = strlen(twit);
	while(pos!=len) {
		dowrite:
			rc = write(sock_fd, twit+pos, len-pos);
			if(rc<0) {
				/* ooh, an error has occurred */
				if(errno==EINTR) goto dowrite;

				/* Report the error and break */
				perror("server_handler");
				break;
			}

			/* No error */
			printf("message delivered to hearer listening to %d\n", sock_fd);
			pos+=rc;
	}

}


int main() {

	int i;
	pthread_t sayers_tid, hearers_tid, send_twits_tid;

	init_sig_handlers();
	init_statistics();

	//allocate memory, for twits 
	for (i=0; i<MAX_TWITS_NUM; i++) 
		twitsDB[i]= malloc(sizeof(char)*(MAX_TWIT+1));  //+1 for '\0'

	//initialize hearers database with -1 (empty) - aka: no connections
	for (i=0; i<MAX_HEARERS; i++)
		hearers[i] = -1;

	CHECK_ERRNO(pthread_create(&hearers_tid, 
	                            NULL, (void*)hearers_thread , (void*)NULL));

	CHECK_ERRNO(pthread_create(&sayers_tid, 
	                            NULL, (void*)sayers_thread, (void*)NULL));

	CHECK_ERRNO(pthread_create(&send_twits_tid, 
	                            NULL, (void*)send_twits, (void*)NULL));

	stats.threads +=3; //recall that main thread waits until the 3's above terminate
	
	pthread_join(sayers_tid, NULL);
	pthread_join(hearers_tid, NULL);
	pthread_join(send_twits_tid, NULL);

	for(i=0; i<MAX_HEARERS; i++)
		close(hearers[i]);
	
	return 0;
}





/*
  Create, configure and return the file descriptor for a listening
  tcp socket on the given port, on every interface of this host. 
  The returned file descriptor can be used by accept to create new TCP
  connections. 

  Parameters:
  listening_port - the port on which the returned socket will listen

  Return:
  A server socket, ready to be used by accept.

  Side effects:
  In case of error, the program will abort(), printing a diagnostic
  message. 
 */
int create_server_socket(int listening_port)
{
  int server_socket;	/* the file descriptor of the listening socket
			   */  
  struct sockaddr_in listening_address; /* the network address of the
					   listening socket */
  int rc;		/* used to call setsockopt */
  
  /* create the listening socket, as an INET (internet) socket with
     TCP (stream) data transfer */
  CHECK_ERRNO(server_socket=socket(AF_INET, SOCK_STREAM, 0));
  
  
  /* we need to set a socket option (Google for it, or ask in class) */
  rc = 1;
  CHECK_ERRNO(setsockopt(server_socket, 
			 SOL_SOCKET, SO_REUSEADDR, 
			 &rc, sizeof(int)));
  
  /* Prepare address for a call to bind.
   The specified address will be the INADDR_ANY address (meaning "every
   address on this machine" and the port.
  */
  memset(&listening_address, 0, sizeof(listening_address));
  listening_address.sin_family      = AF_INET;
  listening_address.sin_addr.s_addr = htonl(INADDR_ANY);
  listening_address.sin_port        = htons(listening_port);

  /* Bind listening socket to the listening address */
  CHECK_ERRNO(bind(server_socket, 
		   (struct sockaddr*) &listening_address, 
		   sizeof(listening_address)));

  /* Make server_socket a listening socket (aka server socket) */
  CHECK_ERRNO(listen(server_socket, 15));

  return server_socket;
}

