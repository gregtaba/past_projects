#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bf.h"
#include "ht_table.h"
#include "sht_table.h"

#define RECORDS_NUM 250 // you can change it if you want
#define FILE_NAME "data.db"
#define INDEX_NAME "index.db"

#define CALL_OR_DIE(call)     \
  {                           \
    BF_ErrorCode code = call; \
    if (code != BF_OK) {      \
      BF_PrintError(code);    \
      exit(code);             \
    }                         \
  }

int HashStatistics(char* filename ){

  int fd;
  int num_of_records=0;
  
  BF_Block *block;
	BF_Block_Init(&block);
  BF_Block *block1;
	BF_Block_Init(&block1);
  BF_Block *block2;
	BF_Block_Init(&block2);

  if(strcmp(filename,"data.db")==0){
    HT_info *info;
    info = malloc(sizeof(HT_info));
  
    int* hashArray = (int*)malloc(info->numBuckets*sizeof(int));
    if(BF_OpenFile(filename, &fd)!=BF_OK){
      printf("error in HashStatistics \n");
      return -1;
    };

    int num;
    CALL_OR_DIE(BF_GetBlockCounter(fd, &num));

    CALL_OR_DIE(BF_GetBlock(fd, 0, block));

    void * data = BF_Block_GetData(block);
    memcpy(info,data,sizeof(HT_info));
    CALL_OR_DIE(BF_GetBlock(info->filedesc, 1, block1));
    void* data2=BF_Block_GetData(block1);
    memcpy(hashArray, data2, info->numBuckets*sizeof(int));

    int current_block=0;
    int blocks_per_bucket=0;
    int overflowed_buckets=0;
    int total_overflowed_blocks=0;
    int total_blocks=0;
    int i,j;
    int record_MAX=-1;
    int record_MIN=1000000;
    int total_records=0;
    Record rec;

    for(i=0;i<info->numBuckets;i++){
      int records_per_bucket=0;
      int total_overflowed_blocks=0;


      current_block=hashArray[i];
      CALL_OR_DIE(BF_GetBlock(info->filedesc, current_block, block2));
      void* data3=BF_Block_GetData(block2);

      for(j =0; j<6; j++){
          memcpy(&rec, data3 + j *sizeof(Record), sizeof(Record)); 
          if(rec.id!=0){
            records_per_bucket++;
          }
      }

      HT_block_info* block_info2 ;
      block_info2 = malloc(sizeof(HT_block_info));
      block_info2= data3 + BF_BLOCK_SIZE - sizeof(HT_block_info);
      if(current_block!=1 && current_block!=0){
        total_blocks++;                           //because blocks 0 and 1 are for the SHT_info pointer and the hashArray
      }
      if (block_info2->overflowed==1){
        BF_Block* Block1;
        BF_Block_Init(&Block1);
        total_overflowed_blocks++;
        overflowed_buckets++;
        CALL_OR_DIE(BF_GetBlock(info->filedesc, block_info2->next_block, Block1));
        void* d3 = BF_Block_GetData(Block1);

        for(j =0; j<6; j++){
          memcpy(&rec, d3 + j *sizeof(Record), sizeof(Record)); 
          if(rec.id!=0){
            records_per_bucket++;
          }
        }       

        block_info2 = d3 + BF_BLOCK_SIZE - sizeof(HT_block_info);
        int l=0;
        while (l==0){
          total_blocks++;

          if(block_info2->overflowed==1){
            total_overflowed_blocks++;
          }

          if(block_info2->next_block==-1){
            l=1;
          }
          else{
            CALL_OR_DIE(BF_GetBlock(info->filedesc, block_info2->next_block, Block1));
            void* d3 = BF_Block_GetData(Block1);
            block_info2 = d3 + BF_BLOCK_SIZE - sizeof(HT_block_info);

            for(j =0; j<6; j++){
              memcpy(&rec, d3 + j *sizeof(Record), sizeof(Record)); 
              if(rec.id!=0){
                records_per_bucket++;
              }
            }


          }
        }
      }
      printf("bucket %d has %d overflowed blocks\n",i+1,total_overflowed_blocks);
      printf("bucket %d has %d records\n",i+1,records_per_bucket);
      if(records_per_bucket>record_MAX){
        record_MAX=records_per_bucket;
      }

      if(records_per_bucket<record_MIN){
        record_MIN=records_per_bucket;
      }

      total_records=total_records+records_per_bucket;
      
    }

    printf("\n");


    printf("number of smallest records number per buckets in file %s is %d\n",filename,record_MIN);
    printf("number of biggest records number per buckets in file %s is %d\n",filename,record_MAX);
    printf("number of average records per buckets in file %s is %ld\n",filename,total_records/info->numBuckets);
    printf("number of average blocks per buckets in file %s is %ld\n",filename,total_blocks/info->numBuckets);
    printf("number of buckets that have overflowed blocks in file %s is %d\n",filename,overflowed_buckets);
    printf("number of blocks in file %s is %d\n",filename,num);
    printf("buckets in file are %ld\n",info->numBuckets);


    CALL_OR_DIE(BF_CloseFile(fd));
  }
  else if (strcmp(filename,"index.db")==0){
    SHT_info *info;
    info = malloc(sizeof(SHT_info));
  
    int* hashArray = (int*)malloc(info->numBuckets*sizeof(int));
    if(BF_OpenFile(filename, &fd)!=BF_OK){
      printf("error in HashStatistics \n");
      return -1;
    };

    int num;
    CALL_OR_DIE(BF_GetBlockCounter(fd, &num));

    CALL_OR_DIE(BF_GetBlock(fd, 0, block));

    void * data = BF_Block_GetData(block);
    memcpy(info,data,sizeof(SHT_info));
    CALL_OR_DIE(BF_GetBlock(info->filedesc, 1, block1));
    void* data2=BF_Block_GetData(block1);
    memcpy(hashArray, data2, info->numBuckets*sizeof(int));

    int current_block=0;
    int blocks_per_bucket=0;
    int overflowed_buckets=0;
    int total_overflowed_blocks=0;
    int total_blocks=0;
    int i,j;
    int record_MAX=-1;
    int record_MIN=1000000;
    int total_records=0;
    SHT_Record rec;

    for(i=0;i<info->numBuckets;i++){
      int records_per_bucket=0;
      int total_overflowed_blocks=0;


      current_block=hashArray[i];
      CALL_OR_DIE(BF_GetBlock(info->filedesc, current_block, block2));
      void* data3=BF_Block_GetData(block2);

      for(j =0; j<24; j++){
          memcpy(&rec, data3 + j *sizeof(SHT_Record), sizeof(SHT_Record)); 
          if(rec.block!=0){
            records_per_bucket++;
          }
      }

      SHT_block_info* block_info2 ;
      block_info2 = malloc(sizeof(SHT_block_info));
      block_info2= data3 + BF_BLOCK_SIZE - sizeof(SHT_block_info);
      if(current_block!=1 && current_block!=0){
        total_blocks++;                           //because blocks 0 and 1 are for the SHT_info pointer and the hashArray
      }
      if (block_info2->overflowed==1){
        BF_Block* Block1;
        BF_Block_Init(&Block1);
        total_overflowed_blocks++;
        overflowed_buckets++;
        CALL_OR_DIE(BF_GetBlock(info->filedesc, block_info2->next_block, Block1));
        void* d3 = BF_Block_GetData(Block1);

        for(j =0; j<24; j++){
          memcpy(&rec, d3 + j *sizeof(SHT_Record), sizeof(SHT_Record)); 
          if(rec.block!=0){
            records_per_bucket++;
          }
        }       

        block_info2 = d3 + BF_BLOCK_SIZE - sizeof(SHT_block_info);
        int l=0;
        while (l==0){
          total_blocks++;

          if(block_info2->overflowed==1){
            total_overflowed_blocks++;
          }

          if(block_info2->next_block==-1){
            l=1;
          }
          else{
            CALL_OR_DIE(BF_GetBlock(info->filedesc, block_info2->next_block, Block1));
            void* d3 = BF_Block_GetData(Block1);
            block_info2 = d3 + BF_BLOCK_SIZE - sizeof(SHT_block_info);

            for(j =0; j<24; j++){
              memcpy(&rec, d3 + j *sizeof(SHT_Record), sizeof(SHT_Record)); 
              if(rec.block!=0){
                records_per_bucket++;
              }
            }


          }
        }
      }
      printf("bucket %d has %d overflowed blocks\n",i+1,total_overflowed_blocks);
      printf("bucket %d has %d records\n",i+1,records_per_bucket);
      if(records_per_bucket>record_MAX){
        record_MAX=records_per_bucket;
      }

      if(records_per_bucket<record_MIN){
        record_MIN=records_per_bucket;
      }

      total_records=total_records+records_per_bucket;
      
    }

    printf("\n");


    printf("number of smallest records number per buckets in file %s is %d\n",filename,record_MIN);
    printf("number of biggest records number per buckets in file %s is %d\n",filename,record_MAX);
    printf("number of average records per buckets in file %s is %ld\n",filename,total_records/info->numBuckets);
    printf("number of average blocks per buckets in file %s is %ld\n",filename,total_blocks/info->numBuckets);
    printf("number of buckets that have overflowed blocks in file %s is %d\n",filename,overflowed_buckets);
    printf("number of blocks in file %s is %d\n",filename,num);
    printf("buckets in file are %ld\n",info->numBuckets);
    printf("\n");
    printf("\n");
    

    CALL_OR_DIE(BF_CloseFile(fd));
  }

  return 0;
}


int main() {
    srand(12569874);
    BF_Init(LRU);
    // Αρχικοποιήσεις
    HT_CreateFile(FILE_NAME,10);
    SHT_CreateSecondaryIndex(INDEX_NAME,10,FILE_NAME);
    HT_info* info = HT_OpenFile(FILE_NAME);
    SHT_info* index_info = SHT_OpenSecondaryIndex(INDEX_NAME);
    // Θα ψάξουμε στην συνέχεια το όνομα searchName
    Record record=randomRecord();
    char searchName[15];
    strcpy(searchName, record.name);

    // Κάνουμε εισαγωγή τυχαίων εγγραφών τόσο στο αρχείο κατακερματισμού τις οποίες προσθέτουμε και στο δευτερεύον ευρετήριο
    printf("Insert Entries\n");
    for (int id = 0; id < RECORDS_NUM; ++id) {
        record = randomRecord();
        int block_id = HT_InsertEntry(info, record);
        //printf("adding {%s,block %d}\n",record.name,block_id);
        SHT_SecondaryInsertEntry(index_info, record, block_id);
    }
    // Τυπώνουμε όλες τις εγγραφές με όνομα searchName
    printf("RUN PrintAllEntries for name %s\n",searchName);

    SHT_SecondaryGetAllEntries(info,index_info,searchName);


    HashStatistics(INDEX_NAME );
    

    
    // Κλείνουμε το αρχείο κατακερματισμού και το δευτερεύον ευρετήριο
    SHT_CloseSecondaryIndex(index_info);
    HT_CloseFile(info);
    //
    BF_Close();
    
}
