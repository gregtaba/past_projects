#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "hp_file.h"
#include "record.h"

#define CALL_BF(call)       \
{                           \
  BF_ErrorCode code = call; \
  if (code != BF_OK) {      \
    BF_PrintError(code);    \
    exit(code);             \
  }                         \
}

int HP_CreateFile(char *fileName){
    

  int fd;
  BF_Block *block;
  BF_Block_Init(&block);
  HP_info *info;
  
  
  //create the data.db file
  if(BF_CreateFile(fileName)!=BF_OK){
    printf("error in create_file\n");
    return -1;
  };


  //open the data.db file
  CALL_BF(BF_OpenFile(fileName, &fd));


  info = malloc(sizeof(HP_info));
  info->heap_or_hash_or_nothing=1;
  info->number_of_records=0;
  info->filedisc=fd;

  //create a new block
  CALL_BF(BF_AllocateBlock(fd, block));

  void * data = BF_Block_GetData(block);  

  //we give the first block the pointer to HP_info,so we can identify it as a heap file
  memcpy(data,info,sizeof(HP_info));

  //we changed the block , therefore we set it as dirty
  BF_Block_SetDirty(block);

  //we also unpin it
  BF_UnpinBlock(block);

  //finally we close the file
  CALL_BF(BF_CloseFile(fd));
  


  printf("success create_file\n"); 
  free(info);
  return 0;
  
}


HP_info* HP_OpenFile(char *fileName){
  

  int fd;
  BF_Block *block;
	BF_Block_Init(&block);
  HP_info *info;
  info = malloc(sizeof(HP_info));
    
  //open the file
  CALL_BF(BF_OpenFile(fileName, &fd));
  
  //get 1st block
  //check if it's heap file
  CALL_BF(BF_GetBlock(fd, 0, block));

  void * data = BF_Block_GetData(block);
  memcpy(info,data,sizeof(HP_info));

  //unpin it
  CALL_BF(BF_UnpinBlock(block));	

  
  
  //if True = heapfile
  if(info->heap_or_hash_or_nothing==1){
    printf("success on openfile\n");
    return info;
  }
  else{
    printf("failure on openfile\n");
    return NULL;
    
  }
  
        

}


int HP_CloseFile( HP_info* hp_info ){


    if(BF_CloseFile(hp_info->filedisc)!=BF_OK){
      printf("error in close_file\n");
      return -1;
    }
    else{
      printf("success on close file\n");
      free(hp_info);      
    }
    return 0;
}


int HP_InsertEntry(HP_info* hp_info, Record record){

    BF_Block *block, *block_c;
    BF_Block_Init(&block);
    BF_Block_Init(&block_c);

    //get number of last block
    int num = 0;
    CALL_BF(BF_GetBlockCounter(hp_info->filedisc, &num));
    //we start counting from 0, so we need to adjust
    num = num - 1;
    

    

    CALL_BF(BF_GetBlock(hp_info->filedisc, 0, block_c));
    void * data_c = BF_Block_GetData(block_c);

    CALL_BF(BF_GetBlock(hp_info->filedisc, num, block));
    void * data = BF_Block_GetData(block);

    int freespace = BF_BLOCK_SIZE - hp_info->number_of_records *(sizeof(Record));
    

    if (freespace >= sizeof(Record)){

      memcpy((data + BF_BLOCK_SIZE - freespace), &record, sizeof(Record));

      hp_info->number_of_records+=1;
    }
    else{
      
      CALL_BF(BF_UnpinBlock(block));
      BF_AllocateBlock(hp_info->filedisc, block);
      data = BF_Block_GetData(block);
      hp_info->number_of_records=1;
      memcpy(data, &record, sizeof(Record));
      num=num+1;
      
    }
    BF_Block_SetDirty(block);
    CALL_BF(BF_UnpinBlock(block));
    BF_Block_Destroy(&block);
    BF_Block_Destroy(&block_c);
    
    return num;

}

int HP_GetAllEntries(HP_info* hp_info, int value){

    BF_Block *block, *block_c;
    BF_Block_Init(&block);
    BF_Block_Init(&block_c);

    //to store the records
    Record rec;

    //get number of records in the last block
    //CALL_BF(BF_GetBlock(fileDesc, 0, block_c));
    //char * data_c = BF_Block_GetData(block_c);
    //int counter = data_c[4];
    //printf("counter is: %d\n", counter);

    //get number of blocks
    int num;
    CALL_BF(BF_GetBlockCounter(hp_info->filedisc, &num));

    //we will print all the blocks except the last one
    //if there are more than 2 blocks, we print them all exept the last
    int flag=0;
    int blocks_that_have_been_read=0;
    int k=1;
        //adjust the num
        num = num - 1;
        //printf("num is : %d\n", num);
        //for each block
        for(int i = 0; i<= num; i++){
            
            //get the block
            CALL_BF(BF_GetBlock(hp_info->filedisc, i, block));
            //get the data
            void *data = BF_Block_GetData(block);
            //since there are 76 bytes in every record there are 6 records in each full block
            for(int j =0; j<6; j++){
                memcpy(&rec, data + j *sizeof(Record), sizeof(Record));
                if(rec.id==value){
                  printRecord(rec);
                  flag=1;
                  blocks_that_have_been_read=k;
                  printf("blocks that have been read  %d \n", blocks_that_have_been_read);
                }
                
            }
            //printf("\n");
            CALL_BF(BF_UnpinBlock(block));
            k++;

        }

    BF_Block_Destroy(&block);
    BF_Block_Destroy(&block_c);

    if(flag==0){
      return -1;
    }
  
  return blocks_that_have_been_read;
}

