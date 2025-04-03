#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE

#include "searcher.h"
#include "rndsamp.h"
#include "blkio.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


static int64_t Searcher_AllocateMemForSearcherObj(Searcher_SearcherObj **SearcherObj);

static int64_t Searcher_GetNumPhaseOneSamples(int64_t SubpopSizeBlks, 
                                    float ConfidenceLevel, 
                                    float ConfidenceInterval, 
                                    int64_t NumSubpops);
                                    

static int64_t Searcher_GetNumPhaseOneSamples(int64_t SubpopSizeBlks, 
                                    float ConfidenceLevel, 
                                    float ConfidenceInterval, 
                                    int64_t NumSubpops)
{
/* This function approximates the number of phase one samples
** that needs to be sampled.  Note: This is an approximation.
** For simplicity, this is calculated by multiplying
** the number of non-overlapping subpoplations
** by the sample size for a single full size subpopulation.
** For more accuracy, the implementation could have determined
** if the last subpopulation is full or partial (Device might
** not be a multiple of the subpop size in blks) and calculated
** a different sample size for the partial subpop.
*/
  int64_t Ret = 0;
  RndSamp_RndSampObj *RndSampObj;
  int64_t NumPhaseOneSamples = 0;

  Ret = RndSamp_NewRndSampObjSampSizeUnknown(&RndSampObj, 
                                             SubpopSizeBlks, 
                                             ConfidenceLevel, 
                                             ConfidenceInterval);
  if (Ret == ERROR) {
    printf("Error initializing RndSamp in GetNumPhaseOneSamples.\n");
    return ERROR;
  }
  
  NumPhaseOneSamples = RndSampObj->SampleSize * NumSubpops;
  
  RndSamp_DestroyRndSampObj(&RndSampObj);
  
  return NumPhaseOneSamples;
}


int64_t Searcher_DoPhaseOneSampling(Searcher_SearcherObj *SearcherObj)
{
  int64_t Ret = 0;
  int64_t i = 0;
  unsigned char *Buf;
  BlkIo_BlkObj *BlkObj = NULL;
  int64_t BlkNum = 0;

  Ret = RndSamp_DoReplacementRndSamp(SearcherObj->PhaseOneSampObj);

  if (Ret == ERROR) {
    printf("Error performing replacement rnd samp in phase one.\n");
    return ERROR;
  }
  
  //Buf = BlkIo_AllocateMemForBuf(SearcherObj->BlkSize);

  //if (Buf == (unsigned char *)ERROR) {
  //  printf("Error allocated mem for block buf.\n");
  //  return ERROR;
  //}

  for (i = 0; i < SearcherObj->PhaseOneSampObj->SampleSize; i++) {
  
    BlkNum = SearcherObj->PhaseOneSampObj->SampledNumsArr[i];

    /* NEW BLK OBJ */
    Ret = BlkIo_NewBlkObj(SearcherObj->Fp, &BlkObj, BlkNum, SearcherObj->BlkSize);
    
    if (Ret == ERROR) {
      printf("Error creating blk obj.\n");
      return ERROR;
    }
    
    /* PHASE ONE CMP FUNC CALL */
    Ret = SearcherObj->PhaseOneCmpFunc(SearcherObj->PhaseOneCmpFuncParam, BlkObj);
    
    //Ret = TRUE;
    
    if (Ret == TRUE) {
      //SearcherObj->PhaseOneSampObj->SampledNumsArr[i].MatchFlag = TRUE;
      //printf("MATCH.\n");
    } else if (Ret == FALSE){
      //SearcherObj->PhaseOneSampObj->SampledNumsArr[i].MatchFlag = FALSE;
      //printf("NOT.\n");
    } else {
      printf("Error in phaseonecmpfunc.\n");
      return ERROR;
    }
    
    if (i % 50 == 0) {
      printf("%llu\n", i);
    }
    
    /* DESTROY BLK OBJ */
    BlkIo_DestroyBlkObj(&BlkObj);
    
  }

  return SUCCESS;

}


int64_t Searcher_DoPhaseTwoSampling(Searcher_SearcherObj *SearcherObj) {

  int64_t Ret = 0;
  int64_t i = 0;

  Ret = RndSamp_DoReplacementRndSamp(SearcherObj->PhaseTwoSampObj);

  if (Ret == ERROR) {
    printf("Error performing replacement rnd samp in phase one.\n");
    return ERROR;
  }

  return SUCCESS;
}


int64_t Searcher_NewSearcherObj(Searcher_SearcherObj **SearcherObj, 
                          int64_t SubpopSize,
                          float PhaseOneCl,
                          float PhaseOneCi,
                          int8_t (*PhaseOneCmpFunc)
                                 (void *Ptr,
                                 BlkIo_BlkObj *BlkObj),
                          void *PhaseOneCmpFuncParam,
                          int64_t NumSubpops,
                          int64_t PopulationSize,
                          int64_t SldSubpopWndMaxOffset,
                          float PhaseTwoCl,
                          float PhaseTwoCi,
                          FILE *Fp,
                          int64_t BlkSize)
{
  int64_t Ret = 0;
  Searcher_SearcherObj *TmpObj = NULL;
  int64_t NumPhaseOneSamples = 0;

  Ret = Searcher_AllocateMemForSearcherObj(&TmpObj);
  
  if (Ret == ERROR) {
    return ERROR;
  }

  NumPhaseOneSamples = Searcher_GetNumPhaseOneSamples(SubpopSize,
                                                   PhaseOneCl, 
                                                   PhaseOneCi, 
                                                   NumSubpops);
  
  printf("Num phase one samples: %llu\n", NumPhaseOneSamples);
  
  if (NumPhaseOneSamples == ERROR) {
    printf("Error getting num phase one samples.\n");
    return ERROR;
  }
  
  Ret = RndSamp_NewRndSampObjSampSizeKnown( &(TmpObj->PhaseOneSampObj),
                                            PopulationSize,
                                            NumPhaseOneSamples );
  
  if (Ret == ERROR) {
    printf("Error initializing PhaseOneSampObj.\n");
    return ERROR;
  }
  
  Ret = RndSamp_NewRndSampObjSampSizeUnknown( &(TmpObj->PhaseTwoSampObj), 
                                              SldSubpopWndMaxOffset,
                                              PhaseTwoCl,
                                              PhaseTwoCi);
                                              
  if (Ret == ERROR) {
    printf("Error initializing PhaseTwoSampObj.\n");
    return ERROR;
  }

  TmpObj->PhaseOneCmpFunc = PhaseOneCmpFunc;
  TmpObj->PhaseOneCmpFuncParam = PhaseOneCmpFuncParam;
  TmpObj->Fp = Fp;
  TmpObj->BlkSize = BlkSize;

  *SearcherObj = TmpObj;

  return SUCCESS;
}



static int64_t Searcher_AllocateMemForSearcherObj(Searcher_SearcherObj **SearcherObj)
{
  *SearcherObj = calloc(1, sizeof(Searcher_SearcherObj));
  
  if (*SearcherObj == NULL) {
    return ERROR;
  }
  
  return SUCCESS;
}

void Searcher_DestroySearcherObj(Searcher_SearcherObj **SearcherObj)
{
  RndSamp_DestroyRndSampObj( &(*SearcherObj)->PhaseOneSampObj );
  RndSamp_DestroyRndSampObj( &(*SearcherObj)->PhaseTwoSampObj );
  free( *SearcherObj );
}
