
#include "blkio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*
** Allocate memory for the BlkIo_Blk object itself,
** but not other dynamically allocated memory
** associated with the structure.
*/
static int64_t BlkIo_AllocateBlkObj(BlkIo_BlkObj **BlkObj);

/*
** Free the BlkIo_Blk object, but
** not other dynamically allocated
** memory associated with it.
** BlkIo_DestroyBlkObj() does that. 
*/
static void BlkIo_FreeBlkObj(BlkIo_BlkObj **BlkObj);

/*
** Function which allocates memory which
** will hold the block read from the device.
*/
static unsigned char *BlkIo_AllocateMemForBuf(int64_t BlkSize);

/*
** Function to deallocate the memory used by
** the buffer which holds the block read
** from the device.
*/
static void BlkIo_BlkDeallocateMemForBuf(unsigned char **Buf);

/*
** Function to read a block from the device.
*/
static int64_t BlkIo_ReadBlock(FILE *fp, BlkIo_BlkObj *BlkObj);


static int64_t BlkIo_ConvertOffsetBlksToBytes(int64_t BlkNum, int64_t BlkSize)
{
  return BlkNum * BlkSize;
}

static unsigned char *BlkIo_AllocateMemForBuf(int64_t BlkSize)
{
  unsigned char *TmpBuf = NULL;

  TmpBuf = (unsigned char *)malloc(BlkSize * sizeof(unsigned char));
  if (TmpBuf == NULL) {
    printf("Error allocating memory for block.\n");
    return (unsigned char *)ERROR;
  }   
  
  //printf("%p\n", TmpBuf);
  
  return TmpBuf;
}

static void BlkIo_BlkDeallocateMemForBuf(unsigned char **Buf)
{
  free( *Buf );
}

static int64_t BlkIo_ReadBlock(FILE *fp, BlkIo_BlkObj *BlkObj)
{
  int64_t BlkOffsetBytes = 0;

  BlkOffsetBytes = BlkIo_ConvertOffsetBlksToBytes(BlkObj->BlkNum, BlkObj->BlkSizeBytes);

  //printf("%p\n", BlkObj->Buf);
  //printf("FILENO: %d\n", fileno(fp));
  //printf("BYTES: %llu\n", BlkOffsetBytes);
  if (fseeko(fp, BlkOffsetBytes, SEEK_SET) != 0) {
    printf("Error seeking to block %llu.\n", BlkObj->BlkNum);
    return -1;
  }
  
  if (fread(BlkObj->Buf, sizeof(unsigned char), BlkObj->BlkSizeBytes, fp) != BlkObj->BlkSizeBytes) {
    printf("Error reading block %llu from disk.\n", BlkObj->BlkNum);
    return -1;
  }
  
  //int64_t i;
  //for (i = 0; i < BlkObj->BlkSizeBytes; i++) {
    //printf("%c", BlkObj->Buf[i]);
  //}
  
  return SUCCESS;
}

static int64_t BlkIo_AllocateBlkObj(BlkIo_BlkObj **BlkObj)
{
  BlkIo_BlkObj *TmpBlk = NULL;
  
  TmpBlk = (BlkIo_BlkObj *)calloc(1, sizeof(BlkIo_BlkObj));
  
  if (TmpBlk == NULL) {
    return ERROR;
  }
  
  (*BlkObj) = TmpBlk;
  
  return SUCCESS;
}

static void BlkIo_FreeBlkObj(BlkIo_BlkObj **BlkObj)
{
  free(*BlkObj);
}

void BlkIo_DestroyBlkObj(BlkIo_BlkObj **BlkObj)
{
  (*BlkObj)->BlkNum = 0;
  
  BlkIo_BlkDeallocateMemForBuf( &((*BlkObj)->Buf) );
  
  (*BlkObj)->MatchFlag = 0;
  
  BlkIo_FreeBlkObj(BlkObj);
}

int64_t BlkIo_NewBlkObj(FILE *Fp, BlkIo_BlkObj **BlkObj, int64_t BlkNum, int64_t BlkSizeBytes)
{
  int64_t Ret = 0;
  BlkIo_BlkObj *TmpObj = NULL;

  Ret = BlkIo_AllocateBlkObj(&TmpObj);
  
  if (Ret == ERROR) {
    return ERROR;
  }

  TmpObj->BlkNum = BlkNum;
  
  TmpObj->BlkSizeBytes = BlkSizeBytes;
  
  TmpObj->Buf = BlkIo_AllocateMemForBuf(BlkSizeBytes);
  
  if ( TmpObj->Buf == NULL ) {
    return ERROR;
  }

  Ret = BlkIo_ReadBlock(Fp, TmpObj);

  if (Ret == ERROR) {
    printf("Error reading block.\n");
    return ERROR;
  }
  
  *BlkObj = TmpObj;
  
  return SUCCESS;
  
}