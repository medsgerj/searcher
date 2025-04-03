#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "main.h"
#include "searcher.h"
#include "devgeo.h"
#include "rndsamp.h"
#include "rndgen.h"
#include "eval.h"



int main(int argc, char **argv)
{
  FILE *fp = NULL;
  Main_Prefs Prefs;
  DevGeo_DeviceGeometry DevGeo;
  Searcher_SearcherObj *SearcherObj = NULL;
  int64_t Ret = 0;
  int64_t i = 0;
  Eval_EvalEntropyX EvalEntropyX;
  
  /* SEED RND NUMBER GENERATOR FOR PROGRAM */
  RndGen_SeedRand();
  
  /* CHECK CMD LINE ARGS */
  if (argc != 11) {
    printf("USAGE: %s <dev> <cL> <cI> <centProc> <subSz> <blkSz> <trn_cL> <trn_cI> <eLow> <eHigh>\n", argv[0]);
    exit(-1);
  }
  
  /* COPY CMD LINE ARGS TO VARS */
  strncpy(Prefs.DeviceName, argv[1], MAX_DEV_NAME_LEN_UCHARS-1);
  Prefs.DeviceName[MAX_DEV_NAME_LEN_UCHARS-1] = '\0';  
  Prefs.PhaseOneCl = atof(argv[2]);
  Prefs.PhaseOneCi = atof(argv[3]);
  Prefs.CentProc = atof(argv[4]);
  Prefs.SubpopSizeBytes = atoll(argv[5]); //TODO: SIGNED TO UNSIGNED
  Prefs.BlkSizeBytes = atoll(argv[6]); //TODO: SIGNED TO UNSIGNED
  Prefs.PhaseTwoCl = atof(argv[7]);
  Prefs.PhaseTwoCi = atof(argv[8]);
  
  EvalEntropyX.EntropyLowBpB = atof(argv[9]);
  EvalEntropyX.EntropyHighBpB = atof(argv[10]);
  
  /* PRINT USER SUPPLIED PARAMETERS */
  printf("SUPPLIED PARAMETERS:\n");
  printf("\tDEVICE NAME: %s\n", Prefs.DeviceName);
  printf("\tCL: %2.5f\n", Prefs.PhaseOneCl);
  printf("\tCI: %2.5f\n", Prefs.PhaseOneCi);
  printf("\tSUB SIZE BYTES:%llu\n", Prefs.SubpopSizeBytes);
  printf("\tBLOCK SIZE BYTES: %llu\n", Prefs.BlkSizeBytes);
  printf("\tTRAINING CL: %2.5f\n", Prefs.PhaseTwoCl);
  printf("\tTRAINING CI: %2.5f\n", Prefs.PhaseTwoCi);
  printf("\tENTROPY LOW BOUND: %2.5f bpB\n", EvalEntropyX.EntropyLowBpB);
  printf("\tENTROPY HIGH BOUND: %2.5f bpB\n", EvalEntropyX.EntropyHighBpB);
  
  /* OPEN DEVICE */
  fp = fopen(Prefs.DeviceName, "rb");
  if (fp == NULL) {
    printf("Error opening %s\n", Prefs.DeviceName);
    exit(-1);
  }
  
  /* GET THE DEVICE'S GEOMETRY */
  if (DevGeo_InitDeviceGeometry(fp, Prefs.SubpopSizeBytes, Prefs.BlkSizeBytes, &DevGeo) == -1) {
    printf("Error getting drive geometry.\n");
    exit(-1);
  }
  
  /* PRINT DEVICE SIZE BYTES */
  printf("DEVICE GEOMETRY:\n");
  printf("\tBLK SIZE: %llu BYTES\n", DevGeo.BlkSizeBytes);
  printf("\tDEVICE SIZE: %llu BYTES\n", DevGeo.DevSizeBytes);
  printf("\tDEVICE SIZE: %llu BLKS\n", DevGeo.DevSizeBlks);
  printf("\tSUBPOP SIZE: %llu BYTES\n", DevGeo.SubpopSizeBytes);
  printf("\tSUBPOP SIZE: %llu BLKS\n", DevGeo.SubpopSizeBlks);
  printf("\tSLD WND MAX OFFSET: %llu BLKS\n", DevGeo.SldSubpopWndMaxOffsetBlks);
  printf("\tNUM SUBPOPS: %llu\n", DevGeo.NumSubpops);
  
  
  
  Ret = Searcher_NewSearcherObj(&SearcherObj,
                          DevGeo.SubpopSizeBlks,
                          Prefs.PhaseOneCl,
                          Prefs.PhaseOneCi,
                          Eval_EvalEntropy,
                          &EvalEntropyX,
                          DevGeo.NumSubpops,
                          DevGeo.DevSizeBlks,
                          DevGeo.SldSubpopWndMaxOffsetBlks,
                          Prefs.PhaseTwoCl,
                          Prefs.PhaseTwoCi,
                          fp,
                          DevGeo.BlkSizeBytes);
  if (Ret == -1) {
    printf("Error creating searcher object.\n");
    exit(-1);
  }
  
  /*
  ** Array 1:
  ** Arr Pos     [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ] [ ]
  ** Blk Num     12  144 233 333 335 22  44  55  78  101
  ** Match Flag  0   0   1   0   0   0   0   1   1   1
  */
 
 printf("FILENO FP: %d\n", fileno(SearcherObj->Fp));
  
  Ret = Searcher_DoPhaseOneSampling(SearcherObj);
  
  if (Ret == -1) {
    printf("Error performing phase one sampling.\n");
    exit(-1);
  }
  
  for (i = 0; i < SearcherObj->PhaseOneSampObj->SampleSize; i++) {
    printf("%llu\n", SearcherObj->PhaseOneSampObj->SampledNumsArr[i]);
  }
  
  /* 
  ** Array 2:
  ** Sample x blk offsets in range 0 -> SldSubpopWndMaxOffsetBlks
  */
  
  Ret = Searcher_DoPhaseTwoSampling(SearcherObj);
  
  if (Ret == -1) {
    printf("Error in phase two sampling.\n");
    exit(-1);
  }
  
  /*
  ** Loop through Array 2 and Array 1 and count how
  ** many BlkNums in each sampled sliding window subpop
  ** have the match flag set.
  ** Calc percentages for each sliding window subpop 
  ** and put in Array 3. 
  */
  
  /*
  ** Sort Array 3 in ascending order.
  */ 
  
  /*
  ** Get the percent that makes top cent.
  */
  
  
  
  printf("PHASE 1 POPULATION SIZE: %llu\n", SearcherObj->PhaseOneSampObj->PopulationSize);
  printf("PHASE 1 CONFIDENCE LEVEL: %2.5f\n", SearcherObj->PhaseOneSampObj->ConfidenceLevel);
  printf("PHASE 1 CONFIDENCE INTERVAL: %2.5f\n", SearcherObj->PhaseOneSampObj->ConfidenceInterval);
  printf("PHASE 1 SAMPLE SIZE: %llu\n", SearcherObj->PhaseOneSampObj->SampleSize); 
  printf("PHASE 1 ACT MEAN SAMP NUM: %2.5f\n", SearcherObj->PhaseOneSampObj->ActualMeanSampledNum); 
  printf("PHASE 1 EST MEAN SAMP NUM: %2.5f\n", SearcherObj->PhaseOneSampObj->EstimatedMeanSampledNum);   
  printf("FILE: %s LINE: %d\n", __FILE__, __LINE__);
 
  Searcher_DestroySearcherObj(&SearcherObj);
  
  
  /*SearcherTcDS.NumPhaseOneSamples = SearcherTc_GetNumPhaseOneSamples(DevGeo.SubpopSizeBytes, 
                                  Prefs.PhaseOneCl, Prefs.PhaseOneCi, DevGeo.NumSubpops);
  if (SearcherTcDS.NumPhaseOneSamples == -1) {
    printf("Error getting num phase one samples.\n");
    exit(-1);
  }
  
  SearcherTcDS.NumPhaseTwoSamples = SearcherTc_GetNumPhaseTwoSamples(DevGeo.SldWndMaxOffsetBlks, 
                                                               Prefs.PhaseTwoCl, Prefs.PhaseTwoCi);
  if (SearcherTcDS.NumPhaseTwoSamples == -1) {
    printf("Error getting num phase two samples.\n");
    exit(-1);
  }
  
  printf("Num Phase One Samples: %llu\n", SearcherTcDS.NumPhaseOneSamples);
  printf("Num Phase Two Samples: %llu\n", SearcherTcDS.NumPhaseTwoSamples);
*/

  
  
  
  /* GET THE TOTAL NUMBER OF SAMPLES NECESSARY TO MEET THE
   * CL AND CI FOR EACH SUBPOP */
  /*total_samples = get_whole_dev_num_samples(&dev_geo, cl, ci);
   if (total_samples == -1) {
   printf("Error getting whole dev num samples.\n");
   exit(-1);
   }*/
  
  //printf("TOTAL SAMPLES WHOLE DEVICE: %llu\n", total_samples);
  
  /* RANDOMLY SAMPLE THE WHOLE DEVICE AND PUT SAMPLED 
   * BLOCK INFO IN SAMPLED_BLKS ARRAY */
  /*if (sample_whole_device(fp, &dev_geo, &sampled_blks, total_samples) == -1) {
   printf("Error sampling whole device.\n");
   exit(-1);
   }*/
  
  /*total_rnd_trn_subpop_samples = get_num_trn_samp_subpops(&dev_geo, trn_cl, trn_ci);
   if (total_rnd_trn_subpop_samples == -1) {
   printf("Error getting total rnd training subpop samples.\n");
   exit(-1);
   }*/
  
  //printf("TOTAL RND TRN SUBPOP SAMPLES: %llu\n", total_rnd_trn_subpop_samples);
  
  /*if (sample_rnd_training_subpop_offsets(&trn_samp_subpops, total_rnd_trn_subpop_samples, &dev_geo) == -1) {
   printf("Error sampling rnd subpop offsets for training.\n");
   exit(-1);
   }
   
   rnd_training_supop_top_percent = calc_rnd_training_supop_top_percent(trn_samp_subpops, total_rnd_trn_subpop_samples, 
   sampled_blks, total_samples, cent_proc,
   e_low, e_high);
   if (rnd_training_supop_top_percent == -1) {
   printf("Error calculating subpop top percent training set.\n");
   exit(-1);
   }*/
  
  //if (process_device_sld_subpop_wnds(fp, rnd_training_supop_top_percent, sampled_blks, total_samples, &dev_geo, e_low, e_high) == -1) {
  //  printf("Error processing the device using sld subpop wnds.\n");
  //  exit(-1);
  //}
  
  /*int64_t i;
   for (i = 0; i < total_samples; i++) {
   printf("BLK: %llu\t%2.5f\n", sampled_blks[i].blk_num , sampled_blks[i].entropy);
   }
   
   for (i = 0; i < total_rnd_trn_subpop_samples; i++) {
   printf("SUB: %llu %llu/%llu PERC: %2.5f\n", trn_samp_subpops[i].start_blk_num, 
   trn_samp_subpops[i].num_samp_blks_in_ent_range,
   trn_samp_subpops[i].tot_num_samp_blks_in_subpop,
   trn_samp_subpops[i].percent_samp_blks_in_ent_range);
   }*/
  
  //printf("PERCENT NECESSARY: %2.5f\n", rnd_training_supop_top_percent);
  
  //free(sampled_blks);
  //free(trn_samp_subpops);
  
  exit(0);
}
