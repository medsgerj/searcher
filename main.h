
/*
 ** File:
 ** $Id:$
 **
 ** Purpose:
 ** Main.
 **
 ** References:
 ** None
 **
 ** Notes:
 ** None
 */

#ifndef __main__
#define __main__

/*
 ** Includes
 */
#include <stdint.h>

/*
 ** Macro Definitions
 */
#define MAX_DEV_NAME_LEN_UCHARS 512

/*
 ** Type Definitions
 */
struct Main_PrefsT {
  char DeviceName[MAX_DEV_NAME_LEN_UCHARS]; /* NAME OF DEVICE */
  float         PhaseOneCl;                 /* CONFIDENCE LEVEL */
  float         PhaseOneCi;                 /* CONFIDENCE INTERVAL */
  float         CentProc;                   /* PERCENT OF SUBPOPS TO FULLY PROCESS */
  int64_t       SubpopSizeBytes;
  int64_t       BlkSizeBytes;
  float         PhaseTwoCl;
  float         PhaseTwoCi;
  float         EntropyLowBpB;
  float         EntropyHighBpB;
};
typedef struct Main_PrefsT Main_Prefs;

/*
 ** Exported Functions
 */


#endif