/*
 ** File:
 ** $Id:$
 **
 ** Purpose:
 ** Simplify reading blocks from a device.
 **
 ** References:
 ** None
 **
 ** Notes:
 ** None
 */

#ifndef __blkio__
#define __blkio__

/*
 ** Includes
 */
#include <stdint.h>
#include <stdio.h>

/*
 ** Macro Definitions
 */
#define ERROR   -1
#define SUCCESS  0


/*
 ** Type Definitions
 */
struct BlkIo_BlkObjT {
  int64_t       BlkNum;
  unsigned char *Buf;
  int64_t       BlkSizeBytes;
  int8_t        MatchFlag;
};

typedef struct BlkIo_BlkObjT BlkIo_BlkObj; 

/*
 ** Exported Functions
 */
 
/*
** CONSTRUCTOR:
** Creates a new BlkIo_Blk object.
*/
int64_t BlkIo_NewBlkObj(FILE *Fp, BlkIo_BlkObj **BlkObj, int64_t BlkNum, int64_t BlkSize);

/*
** DESTRUCTOR:
** Destroys a BlkIo_Blk object.
*/
void BlkIo_DestroyBlkObj(BlkIo_BlkObj **BlkObj);
 
#endif
