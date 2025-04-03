/*
 ** File:
 ** $Id:$
 **
 ** Purpose:
 ** Determine device geometry.
 **
 ** References:
 ** None
 **
 ** Notes:
 ** None
 */

#ifndef __devgeo__
#define __devgeo__

/*
 ** Includes
 */
#include <stdint.h>
#include <stdio.h>

/*
 ** Macro Definitions
 */
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE64_SOURCE 
 
#define ERROR -1
#define FALSE  0
#define TRUE   1

/*
 ** Type Definitions
 */

struct DevGeo_DeviceGeometryT {
  int64_t DevSizeBytes;
  int64_t DevSizeBlks;
  int64_t BlkSizeBytes;
  int64_t SubpopSizeBytes;
  int64_t SubpopSizeBlks;
  int64_t SldSubpopWndMaxOffsetBlks;
  int64_t NumSubpops;
};

typedef struct DevGeo_DeviceGeometryT DevGeo_DeviceGeometry;

/*
 ** Exported Functions
 */

/*
** Function to intialize the DevGeo_DeviceGeometry struct.
*/
int64_t DevGeo_InitDeviceGeometry(FILE *fp, int64_t SubSzBytes, int64_t BlkSzBytes, DevGeo_DeviceGeometry *DevGeo);

#endif
