#include "devgeo.h"
#include <stdio.h>
#include <math.h>

/*
 ** Gets the size of the device in bytes.
 ** This function contains code that 
 ** is platform independent.
 */
static int64_t DevGeo_GetDeviceSizeBytes(FILE *fp);

/* 
 ** Converts the size of the device in bytes
 ** to its size in blocks.
 */
static int64_t DevGeo_GetDeviceSizeBlks(DevGeo_DeviceGeometry *DevGeo);

/*
 ** Converts the size of the subpopulation in bytes
 ** to the size in blocks.
 */
static int64_t DevGeo_GetSubpopSizeBlks(DevGeo_DeviceGeometry *DevGeo);

/* 
 ** Function to compute the maximum offset of the beginning
 ** block of a subpopulation if it was slid from the beginning
 ** of the device to the end.
 */
static int64_t DevGeo_GetSldSubpopWndMaxOffsetBlks(DevGeo_DeviceGeometry *DevGeo);

/*
 ** Function to calculate the number of non-overlapping
 ** subpopulations the device can be divided into.
 */
static int64_t DevGeo_GetNumSubpops(DevGeo_DeviceGeometry *DevGeo);

/*
 ** Function to check if the device is greater than or
 ** equal to the subpopulation size.
 */
static int64_t DevGeo_IsDeviceGTESubpopSize(DevGeo_DeviceGeometry *DevGeo);

/* 
 ** Function to check if the device size in bytes
 ** is a multiple of the blocks size.
 */
static int64_t DevGeo_IsDeviceSizeBytesMultipleOfBlkSize(DevGeo_DeviceGeometry *DevGeo);

/*
 ** Function to check if the subpopulation size in bytes
 ** is a multiple of the block size.
 */
static int64_t DevGeo_IsSubpopSizeBytesMultipleOfBlkSize(DevGeo_DeviceGeometry *DevGeo);


static int64_t DevGeo_GetDeviceSizeBytes(FILE *fp)
{
  int8_t OSSupportedFlag = 0;
  int64_t DevSizeBytes = 0;

  #ifdef __APPLE__
  
    #include <sys/disk.h>
    #include <sys/ioctl.h>
    #include <fcntl.h>

    uint32_t BlockSize;
    int64_t BlockCount;
    
    ioctl(fileno(fp), DKIOCGETBLOCKSIZE, &BlockSize); /* TODO: UINT32_T MAY NOT WORK IN ALL CASES */
    ioctl(fileno(fp), DKIOCGETBLOCKCOUNT, &BlockCount); /* TODO: int64_t MAY NOT WORK IN ALL CASES */

    DevSizeBytes = BlockSize * BlockCount;
    OSSupportedFlag = 1;

  #endif
  
  #ifdef __linux__
    printf("Linux not supported yet.\n");
    return ERROR;
    OSSupportedFlag = 1;
  #endif
  
  if (OSSupportedFlag == 0) {
    printf("OS not supported.\n");
    return ERROR;
  }
  
  return DevSizeBytes;
}


static int64_t DevGeo_GetDeviceSizeBlks(DevGeo_DeviceGeometry *DevGeo)
{
  return DevGeo->DevSizeBytes / DevGeo->BlkSizeBytes;
}


static int64_t DevGeo_GetSubpopSizeBlks(DevGeo_DeviceGeometry *DevGeo)
{
  return DevGeo->SubpopSizeBytes / DevGeo->BlkSizeBytes;
}


static int64_t DevGeo_GetSldSubpopWndMaxOffsetBlks(DevGeo_DeviceGeometry *DevGeo)
{
  return DevGeo->DevSizeBlks - DevGeo->SubpopSizeBlks;
}


static int64_t DevGeo_GetNumSubpops(DevGeo_DeviceGeometry *DevGeo)
{
  double NumSubpops = 0;
  
  NumSubpops = (double)DevGeo->DevSizeBlks / (double)DevGeo->SubpopSizeBlks;

  return (int64_t)ceil(NumSubpops);
}


static int64_t DevGeo_IsDeviceGTESubpopSize(DevGeo_DeviceGeometry *DevGeo)
{
  if (DevGeo->DevSizeBytes >= DevGeo->SubpopSizeBytes) {
    return TRUE;
  } else {
    return FALSE;
  }
}


static int64_t DevGeo_IsDeviceSizeBytesMultipleOfBlkSize(DevGeo_DeviceGeometry *DevGeo)
{
  int64_t rem;
  
  rem = DevGeo->DevSizeBytes % DevGeo->BlkSizeBytes;
  if (rem == 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}


static int64_t DevGeo_IsSubpopSizeBytesMultipleOfBlkSize(DevGeo_DeviceGeometry *DevGeo)
{
  int64_t rem;
  
  rem = DevGeo->SubpopSizeBytes % DevGeo->BlkSizeBytes;
  if (rem == 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}


int64_t DevGeo_InitDeviceGeometry(FILE *fp, 
                                  int64_t SubSzBytes, 
                                  int64_t BlkSzBytes, 
                                  DevGeo_DeviceGeometry *DevGeo) 
{
  /* GET THE DEVICE'S SIZE IN BYTES */
  DevGeo->DevSizeBytes = DevGeo_GetDeviceSizeBytes(fp);
  
  if (DevGeo->DevSizeBytes == ERROR) {
    printf("Error getting size of device.\n");
    return ERROR;
  }
  
  /* USER SET BLOCK SIZE BYTES */
  DevGeo->BlkSizeBytes = BlkSzBytes;
  
  /* USER SET SUBPOP SIZE BYTES */
  DevGeo->SubpopSizeBytes = SubSzBytes;
  
  if (DevGeo_IsDeviceGTESubpopSize(DevGeo) == FALSE) {
    printf("DevGeo_IsDeviceGTESubpopSize() == FALSE.\n");
    return ERROR;
  }
  
  if (DevGeo_IsDeviceSizeBytesMultipleOfBlkSize(DevGeo) == FALSE) {
    printf("DevGeo_IsDeviceSizeBytesMultipleOfBlkSize() == FALSE.\n");
    return ERROR;
  }
  
  DevGeo->DevSizeBlks = DevGeo_GetDeviceSizeBlks(DevGeo);
  
  if (DevGeo_IsSubpopSizeBytesMultipleOfBlkSize(DevGeo) == FALSE) {
    printf("DevGeo_IsSubpopSizeBytesMultipleOfBlkSize() == FALSE.\n");
    return ERROR;
  }
  
  DevGeo->SubpopSizeBlks = DevGeo_GetSubpopSizeBlks(DevGeo);
  
  DevGeo->SldSubpopWndMaxOffsetBlks = DevGeo_GetSldSubpopWndMaxOffsetBlks(DevGeo);
  
  DevGeo->NumSubpops = DevGeo_GetNumSubpops(DevGeo);
  
  return 0;
}
