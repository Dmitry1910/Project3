#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <time.h>
#include "util.h"

/*
This file exists to test functions for server.c while the rest of the program gets built
printf("@fucktyler\n");
*/


char* getContentType(char * mybuf) {
  // Should return the content type based on the file type in the request
  // (See Section 5 in Project description for more details)
  /* line  83 through 99*/
  /*int n=strchr(mybuf,'.');
  printf("hi\n");
  printf(n);
  char subbuf[5];
  memcpy(subbuf,mybuf[n-1],4); //subbuf holds everything in buffer after it finds a '.'
  */
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

// Function to open and read the file from the disk into the memory
// Add necessary arguments as needed
int readFromDisk(/*necessary arguments*/char* file, void* buf) {
  // Open and read the contents of file given the request
  FILE* f=fopen(file,"r");
  if(f==NULL){
    //printf("fnull\n");
    return 1;
  }
  //printf("f not null\n");
  fseek(f,0,SEEK_END);
  int filesize=ftell(f); //file size will be value of last byte index of f
  memset(buf,'\0',filesize+1);
  printf("filesize=%d\n",filesize);
  fseek(f,0,SEEK_SET);
  fread(buf,1,filesize,f);
  //(char*)buf[filesize]='\0';
  //memcpy(buf[filesize],'\0',1);
  fclose(f);
  return 0;
}

int main(){
/*  char* a=getContentType("superduper.gif");
  char* b=getContentType("stupidass.html");
  char* c=getContentType("sucks");
  printf("%s\n%s\n%s\n",a,b,c);
  //This block tests getContentType */

  char str[128];
  char buf[1024];
  for(int i=0;i<3;i+=1){
    memset(str,'\0',128);

    if(i==0)
      memcpy(str,"stupid0.txt\0",12);
    else if(i==1)
      memcpy(str,"stupid1.txt\0",12);
    else
      memcpy(str,"stupid2.txt\0",12);
    if(readFromDisk(str,buf))
      printf("error opening file str\n");
    else
      printf("buf: %s",buf);
  }


  return 0;
}
