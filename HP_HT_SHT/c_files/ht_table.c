#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "ht_table.h"
#include "record.h"

#define CALL_OR_DIE(call)     \
  {                           \
    BF_ErrorCode code = call; \
    if (code != BF_OK) {      \
      BF_PrintError(code);    \
      exit(code);             \
    }                         \
  }


int HT_CreateFile(char *fileName,  int buckets){

  int fd;
  BF_Block *block;
  BF_Block_Init(&block);
  BF_Block *block2;
  BF_Block_Init(&block2);
  HT_info *info;
  HT_block_info *block_info;
  

  //create the data.db file
  if(BF_CreateFile(fileName)!=BF_OK){
    printf("error in create_file\n");
    return -1;
  };


  //open the data.db file
  CALL_OR_DIE(BF_OpenFile(fileName, &fd));
  

  //create a new block
  CALL_OR_DIE(BF_AllocateBlock(fd, block));

  void * data = BF_Block_GetData(block); 
  
  block_info = malloc(sizeof(HT_block_info));
  block_info->num_of_records=0;
  block_info->block_number=0;
  block_info->overflowed=0;
  block_info->next_block=-1;
  
  info = malloc(sizeof(HT_info));
  info->heap_or_hash_or_nothing=2;
  info->num_of_records=0;
  info->filedesc=fd;
  info->numBuckets=buckets; 
  

  HT_block_info *block_info2;
  block_info2 = malloc(sizeof(HT_block_info));
  block_info2->num_of_records=0;
  block_info2->block_number=1;
  block_info2->overflowed=0;
  block_info2->next_block=-1;
  
  //we give the first block the pointer to HT_info,so we can identify it as a hash file
  memcpy(data,info,sizeof(HT_info));
  //Allocate an array
  int* hashArray = (int*)malloc(buckets*sizeof(int));
	for(int i=0 ; i<buckets ;i++) hashArray[i] = 0;     // initialize with 0


  CALL_OR_DIE(BF_AllocateBlock(fd, block2));

  void * data2 = BF_Block_GetData(block2); 

  memcpy(data2, hashArray, buckets*sizeof(int));

  //we changed the block , therefore we set it as dirty
  BF_Block_SetDirty(block);
  BF_Block_SetDirty(block2);

  //we also unpin it
  BF_UnpinBlock(block);
  BF_UnpinBlock(block2);

  CALL_OR_DIE(BF_CloseFile(fd));
  printf("success create_file\n");
  free(info);
  free(block_info);
  free(block_info2);
  free(hashArray);
  return 0;
}

HT_info* HT_OpenFile(char *fileName){

  int fd;
  BF_Block *block;
	BF_Block_Init(&block);
  HT_info *info;
  info = malloc(sizeof(HT_info));

  //open the file
  CALL_OR_DIE(BF_OpenFile(fileName, &fd));
  
  //get 1st block
  //check if it's heap file
  CALL_OR_DIE(BF_GetBlock(fd, 0, block));

  void * data = BF_Block_GetData(block);
  memcpy(info,data,sizeof(HT_info));

  //unpin it
  CALL_OR_DIE(BF_UnpinBlock(block));	

  
  
  
  if(info->heap_or_hash_or_nothing==2){
    printf("success on openfile\n");
    return info;
  }
  else{
    printf("failure on openfile\n");
    return NULL;
    
  }

  return NULL;
}


int HT_CloseFile( HT_info* HT_info ){

    if(BF_CloseFile(HT_info->filedesc)!=BF_OK){
      printf("error in close_file\n");
      return -1;
    }
    else{
      printf("success on close file\n");
      free(HT_info);      
    }
    return 0;
}

int HT_InsertEntry(HT_info* ht_info, Record record){
  
  int ret_block_id;
  int* hashArray = (int*)malloc(ht_info->numBuckets*sizeof(int));
  BF_Block *block;
  BF_Block_Init(&block);
  
  HT_block_info *block_info;

  CALL_OR_DIE(BF_GetBlock(ht_info->filedesc, 1, block));
  void* data=BF_Block_GetData(block);
  memcpy(hashArray, data, ht_info->numBuckets*sizeof(int));
  int hash=record.id % ht_info->numBuckets;
  int block_counter;
  BF_GetBlockCounter(ht_info->filedesc,&block_counter);
  ht_info->num_of_records+=1;
  int block_id = hashArray[hash];

  if (block_id == 0){
    BF_Block *block2;
    BF_Block_Init(&block2);
    CALL_OR_DIE(BF_AllocateBlock(ht_info->filedesc, block2));
    void* data2=BF_Block_GetData(block2);
    memcpy(data2 , &record, sizeof(Record));
    void* new_record = data2 + BF_BLOCK_SIZE - sizeof(HT_block_info); 
    block_info = malloc(sizeof(HT_block_info));
    block_info->num_of_records=1;
    block_info->block_number=block_counter;
    block_info->overflowed=0;
    block_info->next_block=-1;
    memcpy(new_record, block_info, sizeof(HT_block_info));

    hashArray[hash] = block_counter;    //save block position to hashtable
    
    block_id = block_counter-1;         //we return block_id+1
    ret_block_id=block_info->block_number;
    
    free(block_info);
    BF_Block_SetDirty(block2);
    CALL_OR_DIE(BF_UnpinBlock(block2));
  }
  else{
    
    BF_Block* Block;
    BF_Block_Init(&Block);
    CALL_OR_DIE(BF_GetBlock(ht_info->filedesc, block_id, Block));
    void* d = BF_Block_GetData(Block);
    HT_block_info* block_info2 = d + BF_BLOCK_SIZE - sizeof(HT_block_info);
    //printf("getting block number %d \n", block_id);
    int rec_of_block;
    rec_of_block=block_info2->num_of_records;
    //printf("records in the block %d \n",rec_of_block );
    if(rec_of_block==6 && block_info2->overflowed==0){
      
      //printf("block is full,so we need to create a new one\n" );
      block_info2->overflowed=1;
      BF_Block* new_Block;
      BF_Block_Init(&new_Block);
      CALL_OR_DIE(BF_AllocateBlock(ht_info->filedesc, new_Block));
      BF_GetBlockCounter(ht_info->filedesc,&block_counter);
      //printf("creating block number %d\n",block_counter-1);
      block_info2->next_block=block_counter-1;
      //printf("next_block of block number %d is block number %d\n",block_id,block_counter-1);
      void* d2=BF_Block_GetData(new_Block);
      memcpy(d2 , &record, sizeof(Record));
      HT_block_info *new_block_info;
      new_block_info = malloc(sizeof(HT_block_info));
      new_block_info->num_of_records=1;
      new_block_info->block_number=block_counter-1;
      new_block_info->overflowed=0;
      new_block_info->next_block=-1;
      void* new_record = d2 + BF_BLOCK_SIZE - sizeof(HT_block_info); 
      memcpy(new_record, new_block_info, sizeof(HT_block_info));
      ret_block_id=new_block_info->block_number;
      free(new_block_info);
      
      BF_Block_SetDirty(new_Block);
      CALL_OR_DIE(BF_UnpinBlock(new_Block));
      
    }
    else if(rec_of_block==6 && block_info2->overflowed==1){
      int l=0;

      while(rec_of_block==6 || l==0){
        //printf("block_number=%d ",block_info2->block_number);
        //printf("number of records in block is %d \n ",block_info2->num_of_records);
        BF_Block* Block1;
        BF_Block_Init(&Block1);
        if(block_info2->num_of_records<6){
          BF_Block* Block4;
          BF_Block_Init(&Block4);
          CALL_OR_DIE(BF_GetBlock(ht_info->filedesc, block_info2->block_number, Block4));
          void* d5 = BF_Block_GetData(Block4);
          memcpy(d5+block_info2->num_of_records*sizeof(Record) , &record, sizeof(Record));
          block_info2->num_of_records++;
          ret_block_id=block_info2->block_number;
          BF_Block_SetDirty(Block4);
          CALL_OR_DIE(BF_UnpinBlock(Block4));
          l=1;
        }
        else if(block_info2->next_block==-1 && block_info2->num_of_records==6){
          block_info2->overflowed=1;
          BF_Block* new_Block;
          BF_Block_Init(&new_Block);
          CALL_OR_DIE(BF_AllocateBlock(ht_info->filedesc, new_Block));
          BF_GetBlockCounter(ht_info->filedesc,&block_counter);
          //printf("creating block number %d\n",block_counter-1);
          block_info2->next_block=block_counter-1;
          //printf("next_block of block number %d is block number %d\n",block_id,block_counter-1);
          void* d2=BF_Block_GetData(new_Block);
          memcpy(d2 , &record, sizeof(Record));
          HT_block_info *new_block_info;
          new_block_info = malloc(sizeof(HT_block_info));
          new_block_info->num_of_records=1;
          new_block_info->block_number=block_counter-1;
          new_block_info->overflowed=0;
          new_block_info->next_block=-1;
          ret_block_id=new_block_info->block_number;
          void* new_record = d2 + BF_BLOCK_SIZE - sizeof(HT_block_info); 
          memcpy(new_record, new_block_info, sizeof(HT_block_info));
          free(new_block_info);      
          BF_Block_SetDirty(new_Block);
          CALL_OR_DIE(BF_UnpinBlock(new_Block));   
          l=1;
        }
        else{
          CALL_OR_DIE(BF_GetBlock(ht_info->filedesc, block_info2->next_block, Block1));
          void* d3 = BF_Block_GetData(Block1);
          block_info2 = d3 + BF_BLOCK_SIZE - sizeof(HT_block_info);
          //printf("getting block number %d \n", block_id);
          rec_of_block=block_info2->num_of_records;
        }
        BF_Block_SetDirty(Block1);
        CALL_OR_DIE(BF_UnpinBlock(Block1));  
      }
      //printf("\n");
      
    }
    else{
      
      memcpy(d+rec_of_block*sizeof(Record) , &record, sizeof(Record));
      
      block_info2->num_of_records++;
      ret_block_id=block_info2->block_number;
      
      BF_Block_SetDirty(Block);
      CALL_OR_DIE(BF_UnpinBlock(Block));
       
    }
    //printf("\n");
  }
  
  memcpy(data, hashArray, ht_info->numBuckets*sizeof(int));
  free(hashArray);
  BF_Block_SetDirty(block);
  CALL_OR_DIE(BF_UnpinBlock(block));
  


  
  
  return ret_block_id;
}

int HT_GetAllEntries(HT_info* ht_info, void *value ){

    BF_Block *block, *block_c;
    BF_Block_Init(&block);
    BF_Block_Init(&block_c);

    //to store the records
    Record rec;
    int val=*((int*)value);
    


    //get number of blocks
    int num;
    CALL_OR_DIE(BF_GetBlockCounter(ht_info->filedesc, &num));

    //we will print all the blocks except the last one
    //if there are more than 2 blocks, we print them all exept the last
    int flag=0;
    int blocks_that_have_been_read=0;
    int k=1;
        //adjust the num
        num = num - 1;
        //printf("num is : %d\n", num);
        //for each block
        for(int i = 2; i<= num; i++){
            
            //get the block
            CALL_OR_DIE(BF_GetBlock(ht_info->filedesc, i, block));
            //get the data
            void *data = BF_Block_GetData(block);
            //since there are 76 bytes in every record there are 6 records in each full block
            for(int j =0; j<6; j++){
                memcpy(&rec, data + j *sizeof(Record), sizeof(Record)); 
                
                if(rec.id==val){
                  printRecord(rec);
                  flag=1;
                  blocks_that_have_been_read=k;
                  printf("blocks that have been read  %d \n", blocks_that_have_been_read);
                  printf("\n");
                }
                
            }
            
            CALL_OR_DIE(BF_UnpinBlock(block));
            k++;

        }

    BF_Block_Destroy(&block);
    BF_Block_Destroy(&block_c);

    if(flag==0){
      return -1;
    }
  
  return blocks_that_have_been_read;
  
}




