/* ----------------------------------------------------------------------- 
 * Name            : rngs.h  (header file for the library file rngs.c) 
 * Author          : Steve Park & Dave Geyer
 * Language        : ANSI C
 * Latest Revision : 09-22-98
 * ----------------------------------------------------------------------- 
 */

#if !defined( _RNGS_ )
#define _RNGS_

L7_double64 L7_Random(void);
void   randPlantSeeds(L7_long32 x);
void   randGetSeed(L7_long32 *x);
void   randPutSeed(L7_long32 x);
void   randSelectStream(L7_long32 index);
void   randInitRNG(L7_long32 seed);
void   randTestRandom(void);

#endif
