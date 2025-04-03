/*
 ** File:
 ** $Id:$
 **
 ** Purpose:
 ** Interface for block content evaluation.
 **
 ** References:
 ** None
 **
 ** Notes:
 ** None
 */

#ifndef __eval__
#define __eval__

/*
 ** Includes
 */
#include "blkio.h"
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

struct Eval_EvalEntropyT {
  float EntropyLowBpB;
  float EntropyHighBpB;
};
typedef struct Eval_EvalEntropyT Eval_EvalEntropyX;



/*
 ** Exported Functions
 */
int8_t Eval_EvalEntropy(void *Ptr, BlkIo_BlkObj *BlkObj);


#endif