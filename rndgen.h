/*
** File:
** $Id:$
**
** Purpose:
** Wrappers to C Standard library random number generation functions.
**
** References:
** None
**
** Notes:
** None
*/

#ifndef __rndgen__
#define __rndgen__

/*
** Includes
*/
 
#include <stdint.h>

/*
** Macro Definitions
*/

/*
** Type Definitions
*/

/*
** Exported Functions
*/
 
/*
** Seed the random number generator
*/
void RndGen_SeedRand();

/* Generate a random number that has a maximum value
** of the MaxNum parameter specified
*/
int64_t RndGen_RandRange(int64_t MaxNum);

#endif
