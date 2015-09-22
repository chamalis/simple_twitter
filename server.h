#ifndef SERVER
#define SERVER

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

#define SAYPORT 3331
#define HEARPORT 3332
#define MAX_TWIT 140
#define MAX_TWITS_NUM 12000
#define MAX_HEARERS 100000

/**
  Useful Preprocessor macros
**/

/* Report an error and abort */
#define FATAL_ERROR(message)						\
  {									\
    fprintf(stderr,"In %s(%d) [function %s]: %s\n",			\
	    __FILE__, __LINE__, __FUNCTION__ , (message)  );		\
    abort();								\
  }									\

/* Report a posix error (similar to perror) and abort */
#define FATAL_PERROR(errcode) FATAL_ERROR(strerror(errcode))

/* check return code of command and abort on error with suitable
   message */
#define CHECK_ERRNO(command) { if((command) == -1) FATAL_PERROR(errno); }

/* Return a copy of obj (an lvalue) in a newly malloc'd area. */
#define OBJDUP(obj) memcpy(Malloc(sizeof(obj)), &(obj), sizeof(obj))

/* Copy to obj from src, using sizeof(obj). Note that obj is an
   lvalue but src is a pointer! */
#define COPY(obj, src) (void)memcpy(&(obj), (src), sizeof(obj))

/* Allocate a new object of the given type */
#define NEW(type) ((type *) Malloc(sizeof(type)))

/***** declarations *****/

void sigint_handler(int k);
void sigquit_handler();
void init_sig_handlers();
void init_statistics();
int create_server_socket(int listening_port);
void *sayers_handler (int);
void *hearer_handler(int fd);
void * sayers_thread(void* prama);
void * hearers_thread(void* prama);
void send_twits();
void write_socket(int sock_fd, char * twit);


#endif