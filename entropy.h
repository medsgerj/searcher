/*
** File:
** $Id:$
**
** Purpose:
** Calculation of information entropy.
**
** References:
** None
**
** Notes:
** None
*/

#ifndef __entropy__
#define __entropy__

/*
** Includes
*/
#include <stdint.h>

/*
** Macro Definitions
*/
#define ERROR -1

/*
** Type Definitions
*/

/*
** Exported Functions
*/

/*
** Calculate the information entropy
** of a buffer of data of specified size.
** The result is in bits per byte,
** a float from 0.0 to 8.0.
*/
float Entropy_CalcEntropy(unsigned char *Buf, int64_t LenBytes);

#endif
