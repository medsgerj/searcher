#include "eval.h"
#include "blkio.h"
#include "entropy.h"

int8_t Eval_EvalEntropy(void *Ptr, BlkIo_BlkObj *BlkObj)
{
  float e = 0.0;
  Eval_EvalEntropyX *EvalEntropyX;

  EvalEntropyX = (Eval_EvalEntropyX *)Ptr;
  
  e = Entropy_CalcEntropy(BlkObj->Buf, BlkObj->BlkSizeBytes);
  
  if (e == ERROR) {
    return ERROR;
  } 
  
  //printf("ENT: %2.5f\n", e);
  
  if (e > EvalEntropyX->EntropyLowBpB && e <= EvalEntropyX->EntropyHighBpB) {
    return TRUE;
  } else {
    return FALSE;
  }
  
}