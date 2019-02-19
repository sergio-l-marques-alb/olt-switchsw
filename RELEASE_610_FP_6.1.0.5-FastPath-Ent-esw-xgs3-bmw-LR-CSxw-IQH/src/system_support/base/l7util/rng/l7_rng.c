/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename l7_rng.c
*
* @purpose Random Number Generator
*
* @component util
*
* @comments none
*
* @create 04/22/2003
*
* @author pmurthy
*
* @end
*             
**********************************************************************/

/* -------------------------------------------------------------------------
 * This is an ANSI C library for multi-stream random number generation.  
 * The use of this library is recommended as a replacement for the ANSI C 
 * rand() and srand() functions, particularly in simulation applications 
 * where the statistical 'goodness' of the random number generator is 
 * important.  The library supplies 256 streams of random numbers; use 
 * SelectStream(s) to switch between streams indexed s = 0,1,...,255.
 *
 * The streams must be initialized.  The recommended way to do this is by
 * using the function PlantSeeds(x) with the value of x used to initialize 
 * the default stream and all other streams initialized automatically with
 * values dependent on the value of x.  The following convention is used 
 * to initialize the default stream:
 *    if x > 0 then x is the state
 *    if x < 0 then the state is obtained from the system clock
 *    if x = 0 then the state is to be supplied interactively.
 *
 * The generator used in this library is a so-called 'Lehmer random number
 * generator' which returns a pseudo-random number uniformly distributed
 * 0.0 and 1.0.  The period is (m - 1) where m = 2,147,483,647 and the
 * smallest and largest possible values are (1 / m) and 1 - (1 / m)
 * respectively.  For more details see:
 * 
 *       "Random Number Generators: Good Ones Are Hard To Find"
 *                   Steve Park and Keith Miller
 *              Communications of the ACM, October 1988
 *
 * Name            : rngs.c  (Random Number Generation - Multiple Streams)
 * Authors         : Steve Park & Dave Geyer
 * Language        : ANSI C
 * Latest Revision : 09-22-98
 * ------------------------------------------------------------------------- 
 */


#include <stdio.h>
#include <time.h>

#include "l7_common.h"
#include "rng_api.h"

#define RNG_MODULUS    2147483647 /* DON'T CHANGE THIS VALUE                  */
#define RNG_MULTIPLIER 48271      /* DON'T CHANGE THIS VALUE                  */
#define RNG_CHECK      399268537  /* DON'T CHANGE THIS VALUE                  */
#define RNG_STREAMS    256        /* # of streams, DON'T CHANGE THIS VALUE    */
#define RNG_A256       22925      /* jump multiplier, DON'T CHANGE THIS VALUE */
#define RNG_DEFAULT    123456789  /* initial seed, use 0 < RNG_DEFAULT < RNG_MODULUS  */
      
static L7_long32 rng_seed[RNG_STREAMS] = {RNG_DEFAULT};  /* current state of each stream   */
static L7_long32  rng_stream        = 0;          /* stream index, 0 is the default */
static L7_long32  rng_initialized       = 0;          /* test for stream initialization */


/*********************************************************************
*
* @purpose Return a pseudo-random real number uniformly distributed 
* between 0.0 and 1.0
*
* @param 
*
* @returns a real number between 0.0 and 1.0
*
* @comments 
*       
* @end
*
*********************************************************************/

L7_double64 L7_Random(void)
{
  const L7_long32 Q = RNG_MODULUS / RNG_MULTIPLIER;
  const L7_long32 R = RNG_MODULUS % RNG_MULTIPLIER;
        L7_long32 t;

  t = RNG_MULTIPLIER * (rng_seed[rng_stream] % Q) - R * (rng_seed[rng_stream] / Q);
  if (t > 0) 
    rng_seed[rng_stream] = t;
  else 
    rng_seed[rng_stream] = t + RNG_MODULUS;
  return ((L7_double64) rng_seed[rng_stream] / RNG_MODULUS);
}

/*********************************************************************
*
* @purpose Set the state of all the random number generator 
* streams by "planting" a sequence of states (seeds), one per stream, 
* with all states dictated by the state of the default stream. 
* The sequence of planted states is separated one from the next by 
* 8,367,782 calls to Random().
*
* @param x @b((input))  seed that initializes state of all the streams
*
* @returns None
*
* @comments 
*       
* @end
*
*********************************************************************/

void randPlantSeeds(L7_long32 x)
{
  const L7_long32 Q = RNG_MODULUS / RNG_A256;
  const L7_long32 R = RNG_MODULUS % RNG_A256;
        L7_int32  j;
        L7_long32  s;

  rng_initialized = 1;
  s = rng_stream;                            /* remember the current stream */
  randSelectStream(0);                       /* change to stream 0          */
  randPutSeed(x);                            /* set rng_seed[0]                 */
  rng_stream = s;                            /* reset the current stream    */
  for (j = 1; j < RNG_STREAMS; j++) {
    x = RNG_A256 * (rng_seed[j - 1] % Q) - R * (rng_seed[j - 1] / Q);
    if (x > 0)
      rng_seed[j] = x;
    else
      rng_seed[j] = x + RNG_MODULUS;
   }
}


/*********************************************************************
*
* @purpose to set the state of the current random number 
* generator stream according to the following conventions:
*    if x > 0 then x is the state (unless too large)
*    if x < 0 then the state is obtained from the system clock
*    if x = 0 then the state is to be supplied interactively
*
* @param x @b((input))  seed 
*
* @returns None
*
* @comments 
*       
* @end
*
*********************************************************************/

void randPutSeed(L7_long32 x)
{
  L7_long32 ok = 0;
                                                
  if (x > 0)
    x = x % RNG_MODULUS;                       /* correct if x is too large  */
  if (x < 0)                                 
    x = ((L7_ulong32) time((time_t *) L7_NULLPTR)) % RNG_MODULUS;              
  if (x == 0)                                
    while (!ok) {
      printf("\nEnter a positive integer seed (9 digits or less) >> ");
      scanf("%ld", &x);
      ok = (0 < x) && (x < RNG_MODULUS);
      if (!ok)
        printf("\nInput out of range ... try again\n");
    }
  rng_seed[rng_stream] = x;
}

/*********************************************************************
*
* @purpose get the state of the current random number 
*   generator stream.
*
* @param x @b((output))  seed 
*
* @returns None
*
* @comments 
*       
* @end
*
*********************************************************************/

void randGetSeed(L7_long32 *x)
{
  *x = rng_seed[rng_stream];
}

/*********************************************************************
*
* @purpose set the current random number generator
* stream - stream from which the next random number will come
*
* @param index @b((input))  index of the stream
*
* @returns None
*
* @comments 
*       
* @end
*
*********************************************************************/

void randSelectStream(L7_long32 index)
{
  rng_stream = ((L7_long32) index) % RNG_STREAMS;
  if ((rng_initialized == 0) && (rng_stream != 0))   /* protect against        */
    randPlantSeeds(RNG_DEFAULT);                     /* un-initialized streams */
}

/*********************************************************************
*
* @purpose Initialize the random number generator using the
* random seed
*
* @param seed @b((input))  seed that initializes the RNG
*
* @returns None
*
* @comments 
*       
* @end
*
*********************************************************************/

void randInitRNG(L7_long32 seed)
{
    randPlantSeeds(seed);
    randSelectStream(seed);
}
