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



FILE* server_log;
cache_entry_t* CACHE;
int clen;
request_t* REQUESTS;
int qin,qout,qcount,qmax; //in is next empty location out is next filled location count is number of locations in q filled max is maximum queue_length
pthread_mutex_t lockx=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t free_slot=PTHREAD_COND_INTITIALIZER;
pthread_cont_t content_here=PTHREAD_COND_INTITIALIZER;


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
//LRU would be nice bit of extra credit

// Function to check whether the given request is present in cache
int getCacheIndex(char *request){
  /// return the index if the request is present in the cache
  for(int i=0;i<clen;i+=1){
    if(strcmp(CACHE[i].request,request)==0)
      return i;
  }
  return -1;
}

// Function to add the request and its file content into the cache
void addIntoCache(char *mybuf, char *memory , int memory_size){
  // It should add the request at an index according to the cache replacement policy
  // Make sure to allocate/free memeory when adding or replacing cache entries
  for(int i=0;i<clen;i+=1){
    /*****fill with inital search for empty slots*/
    if(CACHE[i].len<=0){
      CACHE[i]=malloc(sizeof(int)+sizeof(mybuf)+sizeof(memory));
      CACHE[i].len=memory_size;
      CACHE[i].request=mybuf;
      CACHE[i].content=memory;
      //add something for LRU
      return;
    }
  }
  int rand_idx=rand()%clen;
  CACHE[rand_idx]=malloc(sizeof(int)+sizeof(mybuf)+sizeof(memory));
  CACHE[rand_idx].len=memory_size;
  CACHE[rand_idx].request=mybuf;
  CACHE[rand_idx].content=memory;
}

// clear the memory allocated to the cache
void deleteCache(){
  free(CACHE);
  // De-allocate/free the cache memory
}
void deleteQ(){//created by noah again so might be unnecessary
  free(REQUESTS);
}
// Function to initialize the cache
void initCache(int size){
  CACHE=calloc(size,sizeof(cache_entry_t));
  clen=size;
  for(int i=0;i<size;i+=1){
    CACHE[i].len=0;
    CACHE[i].request=NULL;
    CACHE[i].content=NULL;
  }
  // Allocating memory and initializing the cache array
}
//Function to initialize queue CREATEDBYNOAH so possible it unnecessary and wrong
void initQ(int size){
  REQUESTS=calloc(size,sizeof(request_t));
  qin=qout=qcount=0;
  qmax=size;
  for(int i=0;i<size;i+=1){
    REQUESTS[i].fd=0;
    REQUESTS[i].request=NULL;
  }
}

// Function to open and read the file from the disk into the memory
// Add necessary arguments as needed
int readFromDisk(/*necessary arguments*/char* file, void* buf) {
  // Open and read the contents of file given the request
  FILE* f=fopen(file,"r");
  if(f==NULL){
    return -1;
  }
  fseek(f,0,SEEK_END);
  int filesize=ftell(f); //file size will be value of last byte index of f
  memset(buf,'\0',filesize+1);
  printf("filesize=%d\n",filesize);
  fseek(f,0,SEEK_SET);
  fread(buf,1,filesize,f);
  fclose(f);
  return filesize;
}

/**********************************************************************************/

/* ************************************ Utilities ********************************/
// Function to get the content type from the request
char* getContentType(char * mybuf) {
  // Should return the content type based on the file type in the request
  // (See Section 5 in Project description for more details)

  char* subbuf=strchr(mybuf,'.');
  if(subbuf==NULL){
    return "text/plain\0";
  }
  //printf("subbuf: %s\n",subbuf);
  else if(strncmp(subbuf,".htm\0",5)==0 || strncmp(subbuf,".html\0",6)==0){
    return "text/html\0";
  }
  else if(strncmp(subbuf,".jpg\0",5)==0){
    return "image/jpeg\0";
  }
  else if(strncmp(subbuf,".gif\0",5)==0){
    return "image/gif\0";
  }
  else{
    return "text/plain\0";
  }
}

// This function returns the current time in microseconds
long getCurrentTimeInMicro() {
  struct timeval curr_time;
  gettimeofday(&curr_time, NULL);
  return curr_time.tv_sec * 1000000 + curr_time.tv_usec;
}

/**********************************************************************************/

// Function to receive the request from the client and add to the queue
void * dispatch(void *arg) {

  while (1) {

/**/    // Accept client connection
    int ac_check=accept_connection(/*???*/); //returns a file descriptor for request processing to be used in get_request
    if(ac_check<0) //returns negative number on failure
      break; //not sure how to handle this error
    char buf_file_name[1024];
/**/    // Get request from the client
    int req_res=get_request(ac_check,buf_file_name);
    if(req_res!=0){ //get_request failed if this TRUE
      int error_code=return_error(ac_check,buf_file_name);
      printf("get_request failed in dispatch\n");
      break;
    }

/**/    // Add the request into the queue

    REQUESTS[qin].fd=ac_check;
    REQUESTS[qin].request=buf_file_name;
    qcount+=1;
    while(REQUESTS[qin].fd!=0){
      if(qcount==qmax){
        qin=0;
        break;
      }
      qin+=1;
      if(qin==qmax)
        qin=0;
      //not sure what to do if q full
    }

   }
   return NULL;
}

/**********************************************************************************/

// Function to retrieve the request from the queue, process it and then return a result to the client
void * worker(void *arg) {
  int reqs=0; //number of requests handled by this worker thread
   while (1) {

/**/    // Start recording time
    long start_time=getCurrentTimeInMicro(); //get initial time for difference
    char buf[BUFF_SIZE];
    int fsize;
    char* cache_hit="MISS\0";
/**/    // Get the request from the queue
    request_t cur_req=REQUESTS[qout];
/**/    // Get the data from the disk or the cache
    if(cres=getCacheIndex(cur_req.request)>=0){
       buf=CACHE[cres].content;
       fsize=CACHE[cres].len;
       cache_hit="HIT\0";
    }
    else if(fsize=readFromDisk(cur_req.request,buf)<0){//readFromDisk returns filesize, only enters on failure
        printf("readFromDisk failed in worker\n");
        break;
    }

    char* content_type=getContentType(buf);
    if(ret_res=return_result(cur_req.fd,content_type,buf,fsize)){
      printf("return_result failed in worker\n");
      return_error(cur_req.fd,buf);
      break;
    }

/**/    // Stop recording the time
    long end_time=getCurrentTimeInMicro(); //get final time for difference
    end_time-=start_time;
/**/    // Log the request into the file and terminal
    reqs+=1;
    server_log=fopen("web_server_log","a");
    printf("[%d][%d][%d][%s][%d][%dms][%s]\n",arg,reqs,cur_req.fd,cur_req.request,fsize,end_time,cache_hit);
    fprintf(server_log,"[%d][%d][%d][%s][%d][%dms][%s]\n",arg,reqs,cur_req.fd,cur_req.request,fsize,end_time,cache_hit); //write to web_server_log
    fclose(server_log);
    REQUESTS[qout].fd=0;
    REQUESTS[qout].request=NULL;
    qcount-=1;
    while(REQUESTS[qout].fd==0){
      if(qcount==0){
        qout=0;
        break;
      }
      qout+=1;
      if(qout==qmax)
        qout=0;
    }
/**/    // return the result
    //unlock and signal

  }
  return NULL;
}

/***************************************************************************************/
// I put /**/ wherever one of the steps they gave us are
int main(int argc, char **argv) {

  // Error check on number of arguments
  if(argc != 8){
    printf("usage: %s port path num_dispatcher num_workers dynamic_flag queue_length cache_size\n", argv[0]);
    return -1;
  }

  server_log=fopen("web_server_log","w");
  fclose(server_log);
  srand(time(NULL));
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
  pthread_t workers[worker_count];
  pthread_t dispatchers[dispatcher_count];
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
  initQ(ql);
  //init(prt); //undefined reference to 'init' ld returned 1 exit status

/**/  // Create dispatcher and worker threads
  for(int i=0;i<dispatcher_count;i+=1){
    //call to pthread_create
    pthread_create(dispatchers[i],NULL,dispatch,NULL);
  }
  for(int i=0;i<worker_count;i+=1){
    //call to pthread_create
    int idx[i]=i; //worker needs it's threadid for when it writes to web_server_log
    pthread_create(workers[i],NULL,worker,&idx[i]);
  }
/**/  // Clean up
  for(int i=0;i<dispatcher_count;i+=1){
    //call to pthread_join or possibly exit
    pthread_join(dispatchers[i],NULL);
  }
  for(int i=0;i<worker_count;i+=1){
    //call to pthread_join or possibly exit
    pthread_join(workers[i],NULL);
  }
  deleteCache();
  deleteQ();
  return 0;
}
