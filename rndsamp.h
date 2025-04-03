/*
 ** File:
 ** $Id:$
 **
 ** Purpose:
 ** Calculation for random sampling.
 **
 ** References:
 ** None
 **
 ** Notes:
 ** None
 */

#ifndef __rndsamp__
#define __rndsamp__

/*
 ** Includes
 */
#include <stdint.h>

/*
 ** Macro Definitions
 */
#define ERROR   -1
#define SUCCESS  0
#define FALSE    0
#define TRUE     1

/*
 ** Type Definitions
 */
struct RndSamp_RndSampObjT {
  int64_t PopulationSize;
  float   ConfidenceLevel;
  float   ConfidenceInterval;
  int64_t SampleSize;
  int64_t *SampledNumsArr;
  double  ActualMeanSampledNum;
  double  EstimatedMeanSampledNum;
};

typedef struct RndSamp_RndSampObjT RndSamp_RndSampObj;

/*
 ** Exported Functions
 */
 
/*
** CONSTRUCTOR 1:
** Function to initialize RndSamp_RndSampDST struct and
** do random sampling when sample size is unknown.
*/
int64_t RndSamp_NewRndSampObjSampSizeUnknown(RndSamp_RndSampObj **RndSampObj,
                                             int64_t PopulationSize, 
                                             float ConfidenceLevel, 
                                             float ConfidenceInterval);
                                             
/*
 ** CONSTRUCTOR 2:
 ** Function to initialize RndSamp_RndSampDST struct and
 ** do random sampling when sample size is known.
 */                            
int64_t RndSamp_NewRndSampObjSampSizeKnown(RndSamp_RndSampObj **RndSampObj,
                                           int64_t PopulationSize, 
                                           int64_t SampleSize);
                                      
/*
** Performs random sampling without replacement and
** stores the sampled numbers in the SampledNumsArr
** in the RndSamp_RndSampObj.  The fuction samples
** SampleSize number of integers in range 0 to 
** PopulationSize and store them in an array of 
** int64_ts.
*/
int64_t RndSamp_DoReplacementRndSamp(RndSamp_RndSampObj *RndSampObj);

/*
** DESTRUCTOR:
** Destroys the RndSampObj and other dynamically allocated
** memory in the object.
*/
void RndSamp_DestroyRndSampObj(RndSamp_RndSampObj **RndSampObj);

#endif
