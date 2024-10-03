#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "sht_table.h"
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



int SHT_CreateSecondaryIndex(char *sfileName,  int buckets, char* fileName){
  int fd,fd2;
  BF_Block *block;
  BF_Block_Init(&block);
  BF_Block *block2;
  BF_Block_Init(&block2);
  SHT_info *info;
  SHT_block_info *block_info;
  

  
  if(BF_CreateFile(sfileName)!=BF_OK){
    printf("error in create_file of SHT\n");
    return -1;
  };


  CALL_OR_DIE(BF_OpenFile(fileName, &fd2));
  

  CALL_OR_DIE(BF_OpenFile(sfileName, &fd));

  //create a new block
  CALL_OR_DIE(BF_AllocateBlock(fd, block));

  void * data = BF_Block_GetData(block); 
  
  block_info = malloc(sizeof(SHT_block_info));
  block_info->num_of_records=0;
  block_info->block_number=0;
  block_info->overflowed=0;
  block_info->next_block=-1;
  
  info = malloc(sizeof(SHT_info));
  info->Shash_or_nothing=2;
  info->num_of_records=0;
  info->filedesc=fd;
  info->numBuckets=buckets; 
  

  SHT_block_info *block_info2;
  block_info2 = malloc(sizeof(SHT_block_info));
  block_info2->num_of_records=0;
  block_info2->block_number=1;
  block_info2->overflowed=0;
  block_info2->next_block=-1;
  
  //we give the first block the pointer to SHT_info
  memcpy(data,info,sizeof(SHT_info));
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
  CALL_OR_DIE(BF_CloseFile(fd2));
  printf("success create_file of SHT\n");
  free(info);
  free(block_info);
  free(block_info2);
  free(hashArray);
  return 0;
}

SHT_info* SHT_OpenSecondaryIndex(char *indexName){
  int fd;
  BF_Block *block;
	BF_Block_Init(&block);
  SHT_info *info;
  info = malloc(sizeof(SHT_info));

  //open the file 
  CALL_OR_DIE(BF_OpenFile(indexName, &fd));
  
  //get 1st block
  
  CALL_OR_DIE(BF_GetBlock(fd, 0, block));

  void * data = BF_Block_GetData(block);
  memcpy(info,data,sizeof(SHT_info));

  //unpin it
  CALL_OR_DIE(BF_UnpinBlock(block));	

  
  
  
  if(info->Shash_or_nothing==2){
    printf("success on openfile in SHT\n");
    return info;
  }
  else{
    printf("failure on openfile in SHT\n");
    return NULL;
    
  }

  return NULL;
}


int SHT_CloseSecondaryIndex( SHT_info* SHT_info ){
    
    if(BF_CloseFile(SHT_info->filedesc)!=BF_OK){
      printf("error in close_file in SHT\n");
      return -1;
    }
    else{
      printf("success on close file in SHT\n");
      free(SHT_info);      
    }
    return 0;
}

int SHT_SecondaryInsertEntry(SHT_info* sht_info, Record record, int block_id){
  
  SHT_Record* rec;
  rec = malloc(sizeof(SHT_Record));
  strcpy(rec->name,record.name);
  rec->block=block_id;
  
  int ret_block_id;
  int* hashArray = (int*)malloc(sht_info->numBuckets*sizeof(int));
  BF_Block *block;
  BF_Block_Init(&block);



  SHT_block_info *block_info;

  CALL_OR_DIE(BF_GetBlock(sht_info->filedesc, 1, block));
  void* data=BF_Block_GetData(block);
  memcpy(hashArray, data, sht_info->numBuckets*sizeof(int));
  int hash=record.name[0] % sht_info->numBuckets;
  int block_counter;
  BF_GetBlockCounter(sht_info->filedesc,&block_counter);


  sht_info->num_of_records+=1;
  int block_id2 = hashArray[hash];
  //printf("getting block counter %d with block_id2=%d \n", block_counter,block_id2);
  
  if (block_id2 == 0){
    BF_Block *block2;
    BF_Block_Init(&block2);
    CALL_OR_DIE(BF_AllocateBlock(sht_info->filedesc, block2));
    void* data2=BF_Block_GetData(block2);
    memcpy(data2 , rec, sizeof(SHT_Record));
    void* new_record = data2 + BF_BLOCK_SIZE - sizeof(SHT_block_info); 
    block_info = malloc(sizeof(SHT_block_info));
    block_info->num_of_records=1;
    //printf("records in the block 1 \n" );
    block_info->block_number=block_counter;
    block_info->overflowed=0;
    block_info->next_block=-1;
    memcpy(new_record, block_info, sizeof(SHT_block_info));

    hashArray[hash] = block_counter;    //save block position to hashtable

    block_id2 = block_counter-1;         //we return block_id+1
    ret_block_id=block_info->block_number;
    
    free(block_info);
    BF_Block_SetDirty(block2);
    CALL_OR_DIE(BF_UnpinBlock(block2));
  }
  else{
    BF_Block* Block;
    BF_Block_Init(&Block);
    CALL_OR_DIE(BF_GetBlock(sht_info->filedesc, block_id2, Block));
    void* d = BF_Block_GetData(Block);
    SHT_block_info* block_info2 ;
    block_info2 = malloc(sizeof(SHT_block_info));
    block_info2= d + BF_BLOCK_SIZE - sizeof(SHT_block_info);    
    int rec_of_block;
    rec_of_block=block_info2->num_of_records;
    //printf("records in the block %d \n",rec_of_block );
    if(rec_of_block==24 && block_info2->overflowed==0){
      
      //printf("block is full,so we need to create a new one\n" );
      block_info2->overflowed=1;
      BF_Block* new_Block;
      BF_Block_Init(&new_Block);
      CALL_OR_DIE(BF_AllocateBlock(sht_info->filedesc, new_Block));
      BF_GetBlockCounter(sht_info->filedesc,&block_counter);
      //printf("creating block number %d\n",block_counter-1);
      block_info2->next_block=block_counter-1;
      //printf("next_block of block number %d is block number %d\n",block_id,block_counter-1);
      void* d2=BF_Block_GetData(new_Block);
      memcpy(d2 , rec, sizeof(SHT_Record));
      SHT_block_info *new_block_info;
      new_block_info = malloc(sizeof(SHT_block_info));
      new_block_info->num_of_records=1;
      new_block_info->block_number=block_counter-1;
      new_block_info->overflowed=0;
      new_block_info->next_block=-1;
      void* new_record = d2 + BF_BLOCK_SIZE - sizeof(SHT_block_info); 
      memcpy(new_record, new_block_info, sizeof(SHT_block_info));
      ret_block_id=new_block_info->block_number;
      free(new_block_info);
      
      BF_Block_SetDirty(new_Block);
      CALL_OR_DIE(BF_UnpinBlock(new_Block));
      
    }
    else if(rec_of_block==24 && block_info2->overflowed==1){
      int l=0;

      while(rec_of_block==24 || l==0){
        //printf("block_number=%d ",block_info2->block_number);
        //printf("number of records in block is %d \n ",block_info2->num_of_records);
        BF_Block* Block1;
        BF_Block_Init(&Block1);
        if(block_info2->num_of_records<24){
          BF_Block* Block4;
          BF_Block_Init(&Block4);
          CALL_OR_DIE(BF_GetBlock(sht_info->filedesc, block_info2->block_number, Block4));
          void* d5 = BF_Block_GetData(Block4);
          memcpy(d5+block_info2->num_of_records*sizeof(SHT_Record) , rec, sizeof(SHT_Record));
          block_info2->num_of_records++;
          ret_block_id=block_info2->block_number;
          BF_Block_SetDirty(Block4);
          CALL_OR_DIE(BF_UnpinBlock(Block4));
          l=1;
        }
        else if(block_info2->next_block==-1 && block_info2->num_of_records==24){
          block_info2->overflowed=1;
          BF_Block* new_Block;
          BF_Block_Init(&new_Block);
          CALL_OR_DIE(BF_AllocateBlock(sht_info->filedesc, new_Block));
          BF_GetBlockCounter(sht_info->filedesc,&block_counter);
          //printf("creating block number %d\n",block_counter-1);
          block_info2->next_block=block_counter-1;
          //printf("next_block of block number %d is block number %d\n",block_id,block_counter-1);
          void* d2=BF_Block_GetData(new_Block);
          memcpy(d2 , rec, sizeof(SHT_Record));
          SHT_block_info *new_block_info;
          new_block_info = malloc(sizeof(SHT_block_info));
          new_block_info->num_of_records=1;
          new_block_info->block_number=block_counter-1;
          new_block_info->overflowed=0;
          new_block_info->next_block=-1;
          ret_block_id=new_block_info->block_number;
          void* new_record = d2 + BF_BLOCK_SIZE - sizeof(SHT_block_info); 
          memcpy(new_record, new_block_info, sizeof(SHT_block_info));
          free(new_block_info);      
          BF_Block_SetDirty(new_Block);
          CALL_OR_DIE(BF_UnpinBlock(new_Block));   
          l=1;
        }
        else{
          CALL_OR_DIE(BF_GetBlock(sht_info->filedesc, block_info2->next_block, Block1));
          void* d3 = BF_Block_GetData(Block1);
          block_info2 = d3 + BF_BLOCK_SIZE - sizeof(SHT_block_info);
          //printf("getting block number %d \n", block_id);
          rec_of_block=block_info2->num_of_records;
        }
        BF_Block_SetDirty(Block1);
        CALL_OR_DIE(BF_UnpinBlock(Block1));  
      }
      //printf("\n");
      
    }
    else{
      
      memcpy(d+rec_of_block*sizeof(SHT_Record) , rec, sizeof(SHT_Record));
      
      block_info2->num_of_records++;
      ret_block_id=block_info2->block_number;
      
      BF_Block_SetDirty(Block);
      CALL_OR_DIE(BF_UnpinBlock(Block));
       
    }
    //printf("\n");
  }
  memcpy(data, hashArray, sht_info->numBuckets*sizeof(int));
  BF_Block_SetDirty(block);
  CALL_OR_DIE(BF_UnpinBlock(block));
  

  
  
  
  return ret_block_id;
}

int SHT_SecondaryGetAllEntries(HT_info* ht_info, SHT_info* sht_info, char* name){
    
  BF_Block *block, *block_c;
  BF_Block_Init(&block);
  BF_Block_Init(&block_c);

  //to store the records
  SHT_Record rec;
  
  


  //get number of blocks
  int num;
  CALL_OR_DIE(BF_GetBlockCounter(sht_info->filedesc, &num));

  //we will print all the blocks except the last one
  //if there are more than 2 blocks, we print them all exept the last
  int flag=0;
  int blocks_that_have_been_read=0;
  int k=1;
  //adjust the num
  num = num - 1;

  //for each block
  for(int i = 2; i<= num; i++){
      
      //get the block
      CALL_OR_DIE(BF_GetBlock(sht_info->filedesc, i, block));
      //get the data
      void *data = BF_Block_GetData(block);
      //every block has at best 24 SHT_records
      for(int j =0; j<24; j++){
          memcpy(&rec, data + j *sizeof(SHT_Record), sizeof(SHT_Record)); 
          
          if(strcmp(name,rec.name)==0){
            printf("(%s,%d)\n",rec.name,rec.block);
            
            flag=1;
            blocks_that_have_been_read=k;
            
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
  printf("\n");
  return blocks_that_have_been_read;
  

}



