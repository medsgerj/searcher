
#include "rndgen.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void RndGen_SeedRand() {
  srand(time(NULL)); 
}

int64_t RndGen_RandRange(int64_t MaxNum) {
  double rnd_ratio;
  int64_t rnd_num;
  
  rnd_ratio = ((double)rand()) / (((double)RAND_MAX) + 1);
  rnd_num = rnd_ratio * MaxNum; 
  
  return rnd_num;
}