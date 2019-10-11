#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>
#include <conio.h>
#define BLOCK_SIZE 16
#define MEMORY_INIT_CALL_TIME_ERROR 1
#define SIZE_OF_REGION_ERROR 2
#define NOT_ENOUGH_SPACE 3


typedef struct list{

  struct list* next;

  int size;

}list;

list* head = NULL;

typedef struct header{

  int size;

}header;

//////////////////////////////////////////////////////

int memory_error;
int mem_init_call_time = 0;

//////////////////////////////////////////////////////

int Mem_Init(int sizeOfRegion){
int pageSize;

if(mem_init_call_time!=0){

memory_error = MEMORY_INIT_CALL_TIME_ERROR;
return -1;
}

if(sizeOfRegion<=0){

memory_error = SIZE_OF_REGION_ERROR;
return -1;
}

pageSize = getpagesize();
printf("Page Size: %d\n", pageSize);

int padsize = sizeOfRegion % pageSize;
padsize = (pageSize - padsize) % pageSize;

int alloc_size = sizeOfRegion + padsize;
printf("Size of Region: %d\n", alloc_size);


int fileDevice=open("/dev/zero", O_RDWR);
	 void *space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fileDevice, 0);
		if (space_ptr == MAP_FAILED) { 
			printf("MAP_FAILED\n");
			exit(1); 
			}

head = (list*)space_ptr;
head->next = NULL;

head->size = alloc_size - (int)sizeof(list);


mem_init_call_time=1;
return 0;
}

void *TEDU_alloc(int size){

if(size%BLOCK_SIZE !=0){
	size = size + (BLOCK_SIZE - size % BLOCK_SIZE);
}

printf("Allocating %d bytes\n", size);
header *hPointer = NULL;
int newSize=0;
void *memPointer=NULL;
list *tmp = head;
int bestSize;

//make FirstFit
while(tmp){
if(tmp->size >= size+sizeof(header)){
memPointer = tmp;
break;
}
tmp = tmp ->next;
} //First bitti
if(memPointer!=NULL){

tmp = (void *) memPointer;
list *tmpNext = tmp ->next;
newSize = tmp->size -size - sizeof(header);
memPointer = memPointer + tmp->size -size;
hPointer = (void *) memPointer - sizeof(header);
hPointer->size=size;
tmp->size = newSize;
tmp->next = tmpNext;
}

else if(memPointer == NULL){
memory_error = NOT_ENOUGH_SPACE;
return NULL;
}

printf("returned ptr from alloc: %p\n", memPointer);
return (void *)memPointer;

}

int TEDU_Free(void *ptr)
{

  list* itr;

  list* prev;

  int sizeCheck;

  int temp;

  if(ptr == NULL){
    return -1;
  }

  if(((list*)(ptr - sizeof(list)))->size <= 0){
    return -1;
  }


  sizeCheck =( (list*)(ptr - sizeof(list)) )->size;

  temp = (void*)(( (list*)(ptr - sizeof(list)) )->next) - ptr;

  if(sizeCheck-1 != temp){
  	return -1;
  }




  itr =  head;

  prev = NULL;


  if(itr == NULL){
    fprintf(stderr,"Error: Please Call Mem_Init before Mem_Alloc\n");
    return -1;    
  }


  while(itr != NULL){


    if((void*)((char*)itr + sizeof(list)) == ptr){

     
      if((itr->size) & 1){


        itr->size = itr->size - 1;

  
        if( itr->next != NULL && (!(itr->next->size & 1))){
         
          itr->size = itr->size + (int)sizeof(list)
             + (itr->next->size);

          itr->next = itr->next->next;
        }

      
        if(prev != NULL && (!(prev->size & 1))){

          prev->size = itr->size + sizeof(list) 
            + (prev->size);

          prev->next = itr->next;
        }

        return 0;
      }      

      else{

        return -1;
      }
    }

    prev = itr;
    itr = itr->next;
  }

  return -1;
}



int main() {
      assert(Mem_Init(4096) == 0);
   void* ptr[4];

   ptr[0] = TEDU_alloc(4);
   TEDU_GetStats();
   ptr[1] = TEDU_alloc(8);

   printf("\nBEFORE\n");
   assert(TEDU_Free(ptr[0])==0);

   assert(TEDU_Free(ptr[1])==0);

   printf("\nPASSSSSS\n");
   ptr[2] = TEDU_alloc(16);
   ptr[3] = TEDU_alloc(4);

   printf("\nBEFORE\n");
   assert(TEDU_Free(ptr[2])==0);
   assert(TEDU_Free(ptr[3])==0);

   exit(0);
   }

