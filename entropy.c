
#include <string.h>
#include <math.h>
#include <stdint.h>
#include "entropy.h"

float Entropy_CalcEntropy(unsigned char *Buf, int64_t LenBytes)
{
  int64_t   i;
  int64_t   n;
  int64_t   Size;
  float Ent;
  float Temp;
  int64_t  Symbols[256];
    
  n = LenBytes;
  
  if (n == 0) {
    return ERROR;
  }
    
  Temp = 0;
  Size = 0;
  Ent = 0.0;
  memset(Symbols, 0, sizeof(long) * 256);

  for (i = 0; i < n; i++) 
  {
    Symbols[(int64_t) Buf[i]]++;
    Size++;
  }
  for (i = 0; i < 256; i++)
  {
    if (Symbols[i] != 0)
    {
      Temp = (float) Symbols[i] / (float) Size;
      Ent += -Temp * (log2f(Temp)); 
    }
  }

  return Ent;
}
