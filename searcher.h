/*
 ** File:
 ** $Id:$
 **
 ** Purpose:
 ** Main program definitions and functions.
 **
 ** References:
 ** None
 **
 ** Notes:
 ** None
 */
 
#ifndef __searcher__
#define __searcher__

/*
 ** Includes
 */
#include "rndsamp.h"
#include "blkio.h"
#include <stdint.h>
#include <stdio.h>

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

struct Searcher_SearcherObjT {
  RndSamp_RndSampObj *PhaseOneSampObj;
  RndSamp_RndSampObj *PhaseTwoSampObj;
  int8_t (*PhaseOneCmpFunc)(void *Ptr, BlkIo_BlkObj *BlkObj);
  void *PhaseOneCmpFuncParam;
  FILE *Fp; 
  int64_t BlkSize;
};
typedef struct Searcher_SearcherObjT Searcher_SearcherObj; 
 
/*
 ** Exported Functions
 */

/*
** CONSTRUCTOR:
** Create a new SearcherObj.
*/
int64_t Searcher_NewSearcherObj(Searcher_SearcherObj **SearcherObj, 
                          int64_t SubpopSize,
                          float PhaseOneCl,
                          float PhaseOneCi,
                          int8_t (*PhaseOneCmpFunc)
                            (void *Ptr, BlkIo_BlkObj *BlkObj),
                          void *PhaseOneCmpFuncParam,
                          int64_t NumSubpops,
                          int64_t PopulationSize,
                          int64_t SldSubpopWndMaxOffset,
                          float PhaseTwoCl,
                          float PhaseTwoCi,
                          FILE *Fp,
                          int64_t BlkSize);

/*
** Perform Phase One Sampling, which is used to
** determine the percentage the number of blocks on 
** average which a subpopulation must have to be
** considered in the top X % of subpopulations
** with matches.
*/
int64_t Searcher_DoPhaseOneSampling(Searcher_SearcherObj *SearcherObj);

int64_t Searcher_DoPhaseTwoSampling(Searcher_SearcherObj *SearcherObj);
                          
/*
** DESTRUCTOR:
** Destroy a SearcherObj.
*/
void Searcher_DestroySearcherObj(Searcher_SearcherObj **SearcherObj);

#endif
