
#include "rndsamp.h"
#include "rndgen.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/*
** Function to allocate memory for the RndSampObj.
** Note this does not dynamically allocate memory
** for SampledNumsArr.  That occurs when RndSamp_DoReplacementRandSamp()
** is called.
*/
static int64_t RndSamp_AllocateMemForRndSampObj(RndSamp_RndSampObj **RndSampObj);

/*
** Function to check if the confidence level specified as input
** is a supported confidence level.
*/
static int64_t RndSamp_IsConfidenceLevelSupported(float ConfidenceLevel);

/*
 ** Function to check if the confidence interval specified as input
 ** is a supported confidence interval.
 */
static int64_t RndSamp_IsConfidenceIntervalSupported(float ConfidenceInterval);

/*
** Function to check if the population size appears to be a valid value.
*/
static int64_t RndSamp_IsPopulationSizeValid(int64_t PopulationSize);

/*
** Function to check if the sample size appears to be a valid value.
*/
static int64_t RndSamp_IsSampleSizeValid(int64_t SampleSize);

/*
** Function to allocate memory for the array of int64_ts
** which were sampled in range 0 to PopulationSize.
*/
static int64_t RndSamp_AllocateMemForSampledNumsArr(RndSamp_RndSampObj *RndSampObj);


/* 
 ** Calculate the sample size.
 */
static int64_t RndSamp_CalcSampleSize(RndSamp_RndSampObj *RndSampObj);

/*
** Function to calculate the actual mean of the
** numbers sampled.  This will be used
** to determine the random number generator
** if functioning properly.
*/
static int64_t RndSamp_CalcActualMeanSampledNum(RndSamp_RndSampObj *RndSampObj);

/*
 ** Function to calculate the estimated mean of the
 ** numbers sampled.  This will be used
 ** to determine the random number generator
 ** if functioning properly.
 */
static int64_t RndSamp_CalcEstimatedMeanSampledNum(RndSamp_RndSampObj *RndSampObj);

/* TODO: CMP Estimated and Actual Mean Sampled Num */

/* 
** Supported confidence levels and their number of std deviations.
*/
static float ConfidenceLevelConstant[5][2] = {{50, .67},{68, .99},{90, 1.64}, {95, 1.96}, {99, 2.57}}; 


static int64_t RndSamp_CalcSampleSize(RndSamp_RndSampObj *RndSampObj)
{
  float    Z; /* Z SCORE */
  float    p; /* VARIANCE */
  float    e; /* ERROR */
  int64_t  N; /* POPULATION SIZE */
  float    n_0; /*  */
  float    n;
  
  Z = 0.0;
  p = 0.5; /* MAX VARIANCE */
  e = (RndSampObj->ConfidenceInterval/100.0);
  N = RndSampObj->PopulationSize;
  n_0 = 0.0;
  n = 0.0;
  
  /* LOOP THROUGH SUPPORTED CONFIDENCE LEVELS AND FIND THE NUM STD DEVIATIONS FOR 
   * THAT CONFIDENCE LEVEL */
  int64_t i;
  for (i = 0; i < (sizeof(ConfidenceLevelConstant)/sizeof(float)/2); i++)
  {
    if (ConfidenceLevelConstant[i][0] == RndSampObj->ConfidenceLevel)
    {
      Z = ConfidenceLevelConstant[i][1];
    }
  }
  if (Z == 0.0) /* TODO: not sure if I like this may want no sampling 100% CL */
  {
    return ERROR;
  }
  
  /* CALC SAMPLE SIZE */
  n_0 = (pow(Z, 2) * p * (1-p))/pow(e, 2);
  /* ADJUST SAMPLE SIZE FOR FINITE POPULATION */
  n = n_0 / (1 + ((n_0 - 1)/(float)N));
  
  RndSampObj->SampleSize = ceil(n); /* THE SAMPLE SIZE */
  
  return SUCCESS;
}

static int64_t RndSamp_IsConfidenceLevelSupported(float ConfidenceLevel)
{
  uint8_t ConfidenceLevelSupportedFlag = 0;

  int64_t i;
  for (i = 0; i < (sizeof(ConfidenceLevelConstant)/sizeof(float)/2); i++)
  {
    if (ConfidenceLevelConstant[i][0] == ConfidenceLevel)
    {
      ConfidenceLevelSupportedFlag = 1;
    }
  }
  
  if (ConfidenceLevelSupportedFlag == 1) {
    return TRUE;
  } else {
    return FALSE;
  }
  
}

static int64_t RndSamp_IsConfidenceIntervalSupported(float ConfidenceInterval)
{
  if (ConfidenceInterval < 0.0 || ConfidenceInterval > 100.0) {
    return FALSE;
  } else {
    return TRUE;
  }
}

static int64_t RndSamp_IsPopulationSizeValid(int64_t PopulationSize)
{
  if (PopulationSize < 0) {
    return FALSE;
  } else {
    return TRUE;
  }
}

static int64_t RndSamp_IsSampleSizeValid(int64_t SampleSize)
{
  if (SampleSize < 0) {
    return FALSE;
  } else {
    return TRUE;
  }
}


static int64_t RndSamp_AllocateMemForSampledNumsArr(RndSamp_RndSampObj *RndSampObj)
{
  RndSampObj->SampledNumsArr = (int64_t *)calloc(RndSampObj->SampleSize, sizeof(int64_t));
  
  if (RndSampObj->SampledNumsArr == NULL) {
    printf("Error allocating memory for sampled nums arr: calloc.\n");
    return ERROR;
  }
  
  return SUCCESS;
}

int64_t RndSamp_DoReplacementRndSamp(RndSamp_RndSampObj *RndSampObj)
{
  int64_t i;
  int64_t ret;
  
  ret = RndSamp_AllocateMemForSampledNumsArr(RndSampObj);
  
  if (ret == ERROR) {
    printf("Error allocating memory for Sampled Nums Arr.\n");
    return ERROR;
  }
  
  for (i = 0; i < RndSampObj->SampleSize; i++) {
    //printf("SAMP SIZE: %llu\n", RndSampObj->SampleSize);
    
    ret = RndGen_RandRange(RndSampObj->PopulationSize);
    //printf("RET: %llu\n", i);
    if (ret < 0 || ret > RndSampObj->PopulationSize) {
      return ERROR;
    }
    
    RndSampObj->SampledNumsArr[i] = ret;
  }
  
  ret = RndSamp_CalcActualMeanSampledNum(RndSampObj);
  
  if (ret == ERROR) {
    printf("Error calculating mean of randomly sampled numbers.\n");
    return ERROR;
  }
  
  ret = RndSamp_CalcEstimatedMeanSampledNum(RndSampObj);
  
  if (ret == ERROR) {
    printf("Error calculating estimated mean.\n");
    return ERROR;
  }
  
  return SUCCESS;
}

static int64_t RndSamp_CalcActualMeanSampledNum(RndSamp_RndSampObj *RndSampObj)
{
  int64_t SumSampledNums = 0;
  double  ActualMeanSampledNum = 0;
  int64_t i;

  for (i = 0; i < RndSampObj->SampleSize; i++) {
    SumSampledNums += RndSampObj->SampledNumsArr[i];
  }
  
  if (RndSampObj->SampleSize <= 0) {
    return ERROR;
  }

  ActualMeanSampledNum = (double)SumSampledNums / (double)RndSampObj->SampleSize; 
  
  RndSampObj->ActualMeanSampledNum = ActualMeanSampledNum;

  return SUCCESS;
}


static int64_t RndSamp_CalcEstimatedMeanSampledNum(RndSamp_RndSampObj *RndSampObj)
{
  double EstimatedMeanSampledNum;
  double StdDev;
  
  int64_t i;
  for (i = 0; i < (sizeof(ConfidenceLevelConstant)/sizeof(float)/2); i++)
  {
    if (ConfidenceLevelConstant[i][0] == RndSampObj->ConfidenceLevel)
    {
      StdDev = ConfidenceLevelConstant[i][1];
    }
  }
  
  if (StdDev == 0) {
    return ERROR;
  }

  EstimatedMeanSampledNum = (double)RndSampObj->PopulationSize / (double)2.0;

  RndSampObj->EstimatedMeanSampledNum = EstimatedMeanSampledNum;

  return SUCCESS;
}

/*main()
{
  RndSamp_RndSampObj *RndSampObj;
  
  ret = RndSamp_NewRndSampObjSampSizeKnow(&RndSampObj, PopulationSize, SampleSize);
  if (ret == ERROR) {
    return ERROR;  
  }
  
  ret = RndSamp_DoReplacementRndSamp(RndSampObj);
  if (ret == ERROR) {
    return ERROR;
  }
  
  RndSamp_DestroyRndSampObj(&RndSampObj);
  
}*/

int64_t RndSamp_NewRndSampObjSampSizeUnknown(RndSamp_RndSampObj **RndSampObj,
                             int64_t PopulationSize, 
                             float ConfidenceLevel, 
                             float ConfidenceInterval)
{
  RndSamp_RndSampObj *TmpObj;
  int64_t ret;
  
  ret = RndSamp_AllocateMemForRndSampObj(&TmpObj);
  if (ret == ERROR) {
    printf("Error allocating memory for RndSampObj.\n");
    return ERROR;
  }

  TmpObj->PopulationSize = PopulationSize;
  
  ret = RndSamp_IsPopulationSizeValid(TmpObj->PopulationSize);
  if (ret == FALSE) {
    printf("Population size is not valid.\n");
    return ERROR;
  }
  
  TmpObj->ConfidenceLevel = ConfidenceLevel;
  
  ret = RndSamp_IsConfidenceLevelSupported(TmpObj->ConfidenceLevel);
  if (ret == FALSE) {
    printf("Confidence level is not supported.\n");
    return ERROR;
  }
  
  TmpObj->ConfidenceInterval = ConfidenceInterval;
  
  ret = RndSamp_IsConfidenceIntervalSupported(TmpObj->ConfidenceInterval);
  if (ret == FALSE) {
    printf("Confidence interval is not supported.\n");
    return ERROR;
  }
  
  ret = RndSamp_CalcSampleSize(TmpObj);
  if (ret == ERROR) {
    printf("Error calculating sample size.\n");
    return ERROR;
  }

  *RndSampObj = TmpObj;

  return SUCCESS;  
}

int64_t RndSamp_NewRndSampObjSampSizeKnown(RndSamp_RndSampObj **RndSampObj,
                                           int64_t PopulationSize, 
                                           int64_t SampleSize)
{
  RndSamp_RndSampObj *TmpObj;
  int64_t ret;
  
  ret = RndSamp_AllocateMemForRndSampObj(&TmpObj);
  if (ret == ERROR) {
    printf("Error allocating memory for RndSampObj.\n");
    return ERROR;
  }
  
  TmpObj->PopulationSize = PopulationSize;
  
  ret = RndSamp_IsPopulationSizeValid(TmpObj->PopulationSize);
  if (ret == FALSE) {
    printf("Population size is not valid.\n");
    return ERROR;
  }
  
  TmpObj->SampleSize = SampleSize;
  
  ret = RndSamp_IsSampleSizeValid(TmpObj->SampleSize);
  if (ret == FALSE) {
    printf("Sample size is not valid.\n");
    return ERROR;
  }
  
  *RndSampObj = TmpObj;
  
  return SUCCESS;  
}

static int64_t RndSamp_AllocateMemForRndSampObj(RndSamp_RndSampObj **RndSampObj)
{
  *RndSampObj = calloc(1, sizeof(RndSamp_RndSampObj));
  
  if (*RndSampObj == NULL) {
    return ERROR;
  }
  
  return SUCCESS;
}

void RndSamp_DestroyRndSampObj(RndSamp_RndSampObj **RndSampObj)
{
  free( (*RndSampObj)->SampledNumsArr );
  free( (*RndSampObj) );
}
