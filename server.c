#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <time.h>
#include "util.h"
#include <stdbool.h>

#include <unistd.h> //this wasn't included in initial file but needed for "chdir" function

#define MAX_THREADS 100
#define MAX_queue_len 100
#define MAX_CE 100
#define INVALID -1
#define BUFF_SIZE 1024

/*
  THE CODE STRUCTURE GIVEN BELOW IS JUST A SUGESSTION. FEEL FREE TO MODIFY AS NEEDED
*/
// I put /**/ wherever one of the steps they gave us are

// structs:
typedef struct request_queue {
   int fd;
   void *request;
} request_t;

typedef struct cache_entry {
    int len;
    char *request;
    char *content;
} cache_entry_t;

cache_entry_t* CACHE;

/* ************************ Dynamic Pool Code ***********************************/
// Extra Credit: This function implements the policy to change the worker thread pool dynamically
// depending on the number of requests
void * dynamic_pool_size_update(void *arg) {
  while(1) {
    // Run at regular intervals
    // Increase / decrease dynamically based on your policy
  }
}
/**********************************************************************************/

/* ************************************ Cache Code ********************************/
//LRU cache replacement policy is least recently used and shouldn't be hard to implement if decided later

// Function to check whether the given request is present in cache
int getCacheIndex(char *request){
  /// return the index if the request is present in the cache
}

// Function to add the request and its file content into the cache
void addIntoCache(char *mybuf, char *memory , int memory_size){
  // It should add the request at an index according to the cache replacement policy
  // Make sure to allocate/free memeory when adding or replacing cache entries
}

// clear the memory allocated to the cache
void deleteCache(){
  free(CACHE);
  // De-allocate/free the cache memory
}

// Function to initialize the cache
void initCache(int size){
  CACHE=calloc(size,sizeof(cache_entry_t));
  for(int i=0;i<size;i+=1){
    CACHE[i].len=0;
    CACHE[i].request=NULL;
    CACHE[i].content=NULL;
  }
  // Allocating memory and initializing the cache array
}

// Function to open and read the file from the disk into the memory
// Add necessary arguments as needed
int readFromDisk(/*necessary arguments*/char* file, void* buf) {
  // Open and read the contents of file given the request
  FILE* f=fopen(file,"r");
  if(f==NULL){
    return 1;
  }
  fseek(f,0,SEEK_END);
  int filesize=ftell(f); //file size will be value of last byte index of f
  memset(buf,'\0',filesize+1);
  printf("filesize=%d\n",filesize);
  fseek(f,0,SEEK_SET);
  fread(buf,1,filesize,f);
  fclose(f);
  return 0;
}

/**********************************************************************************/

/* ************************************ Utilities ********************************/
// Function to get the content type from the request
char* getContentType(char * mybuf) {
  // Should return the content type based on the file type in the request
  // (See Section 5 in Project description for more details)
  /* line  83 through 100*/
  char* subbuf=strchr(mybuf,'.');
  if(subbuf==NULL){
    return "text/plain";
  }
  //printf("subbuf: %s\n",subbuf);
  else if(strncmp(subbuf,".htm\0",5)==0 || strncmp(subbuf,".html\0",6)==0){
    return "text/html";
  }
  else if(strncmp(subbuf,".jpg\0",5)==0){
    return "image/jpeg";
  }
  else if(strncmp(subbuf,".gif\0",5)==0){
    return "image/gif";
  }
  else{
    return "text/plain";
  }
}

// This function returns the current time in milliseconds
int getCurrentTimeInMills() {
  struct timeval curr_time;
  gettimeofday(&curr_time, NULL);
  return curr_time.tv_usec;
}

/**********************************************************************************/

// Function to receive the request from the client and add to the queue
void * dispatch(void *arg) {

  printf("%s\n","New thread.");
/*
  while (0) {

    // Accept client connection

    // Get request from the client

    // Add the request into the queue

   }
   */
   return NULL;
}

/**********************************************************************************/

// Function to retrieve the request from the queue, process it and then return a result to the client
void * worker(void *arg) {

   while (1) {

/**/    // Start recording time
    int start_time=getCurrentTimeInMills(); //get initial time for difference

/**/    // Get the request from the queue

/**/    // Get the data from the disk or the cache

/**/    // Stop recording the time
    int end_time=getCurrentTimeInMills(); //get final time for difference

/**/    // Log the request into the file and terminal

/**/    // return the result
  }
  return NULL;
}

/**********************************************************************************/
// I put /**/ wherever one of the steps they gave us are
int main(int argc, char **argv) {

  // Error check on number of arguments
  if(argc != 8){
    printf("usage: %s port path num_dispatcher num_workers dynamic_flag queue_length cache_size\n", argv[0]);
    return -1;
  }

/**/  // Get the input args
  int prt=atoi(argv[1]); //port number, which should be in range 1025 through 65535
  char* pth=argv[2]; //the path to the top directory where the files will all be
  int dispatcher_count=atoi(argv[3]); //max amount of dispatcher threads that will be initialized
  int worker_count=atoi(argv[4]); //max amount of worker threads and how many to start up upon initialization
  int dyf=atoi(argv[5]); //determines static or dynamic worker thread pool size
  int ql=atoi(argv[6]); // fixed length of request queue
  int ce=atoi(argv[7]); //number of ENTRIES cache can hold. Not memory_size
  //printf("prt=%d, pth=%s, dispatcher_count=%d, worker_count=%d, dyf=%d, ql=%d, cs=%d\n",prt,pth,dispatcher_count,worker_count,dyf,ql,cs); //everything gets values that it should get
/**/  // Perform error checks on the input arguments
  if(prt<1025 || prt>65535){
    printf("invalid port\n");
    exit(-1);
  }
  if(worker_count>MAX_THREADS || dispatcher_count>MAX_THREADS){
    printf("too many threads\n");
    exit(-1);
  }
  if(ce>MAX_CE){
    printf("too many cache entries\n");
    exit(-1);
  }
  if(ql>MAX_queue_len){
    printf("too big of a q requested\n");
    exit(-1);
  }
/**/  // Change the current working directory to server root directory
  if(chdir(pth)){
    printf("invalid directory, %s requested\n",pth);
    exit(-1);
  }
/**/  // Start the server and initialize cache
  //I think our best option with implementing cache is with a global variable
  //I declare global variable CACHE up where structs are defined and it is allocated
  //in initCache and freed in deleteCache
  initCache(ce);
  // Initialize dispatcher thread array.
  pthread_t dispatcher_threads[dispatcher_count];
  // Start the server.
  init(prt);
  printf("%s\n","Ready to accept connections.");
  int n = 0;
  
  // Keep server up and running to wait for incoming connections.
  while (n < 2) {
	/**/  // Create dispatcher and worker threads
  	for(int i=0;i<dispatcher_count;i+=1){
    		// Create dispatcher threads.
		int thread_status = pthread_create(dispatcher_threads[i],NULL, dispatch, NULL);
		printf("%s%d\n","Created new thread: ",thread_status);
  	}
	
  	for(int i=0;i<worker_count;i+=1){
    	//call to pthread_create
  	}	
	/**/  // Clean up
  	for(int i=0;i<dispatcher_count;i+=1){
    		//call to pthread_join or possibly exit
  	}
  	for(int i=0;i<worker_count;i+=1){
    	//call to pthread_join or possibly exit
  	} 
	n+=1;
  }

  deleteCache();
  return 0;
}
