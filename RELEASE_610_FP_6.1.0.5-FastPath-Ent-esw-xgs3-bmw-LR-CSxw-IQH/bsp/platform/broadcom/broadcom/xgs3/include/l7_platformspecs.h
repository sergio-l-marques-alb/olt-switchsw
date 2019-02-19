/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename l7_platspecs.h
*
* @purpose  System defines based on specific hardware
*
* @component hardware
*
*
* @comments Parameters which are dependent on the platform should
*           be defined here.
*
*
* @created 03/15/2007
*
* @author nshrivastav 

* @end
*
**********************************************************************/

#ifndef  __INC_L7_PLATSPECS_H
#define  __INC_L7_PLATSPECS_H

#include "flex.h"
#include "datatypes.h"

/*********************************************************************
**  Start of  CHIP Defines 
**********************************************************************/
#define L7_BCM_HELIX                               1
#define L7_BCM_FIREBOLT                            2
#define L7_BCM_FIREBOLT2                           3
#define L7_BCM_BRADLEY                             4

#ifndef PLAT_BCM_CHIP
#define PLAT_BCM_CHIP                              L7_BCM_BRADLEY   /* PTin modified (L7_BCM_FIREBOLT2) */
#endif
#include "xgs3.h"
/*********************************************************************
**  End of  CHIP Defines 
**********************************************************************/

/*********************************************************************
**  Start of  Base parameters
**********************************************************************/
#if PLAT_BCM_CHIP == L7_BCM_BRADLEY

#define L7_MAX_PHYSICAL_SLOTS_PER_UNIT           1
#define L7_MAX_PORTS_PER_SLOT                    20
#define L7_MAX_PHYSICAL_PORTS_PER_SLOT           20
#define L7_MAX_PHYSICAL_PORTS_PER_UNIT           20   

#else
#define L7_MAX_PHYSICAL_SLOTS_PER_UNIT           3
#define L7_MAX_PORTS_PER_SLOT                    52    
#define L7_MAX_PHYSICAL_PORTS_PER_SLOT           52  
#define L7_MAX_PHYSICAL_PORTS_PER_UNIT           52  
#endif


/* Start of SNTP (really local clock) specific define statements */

/* From RFC 1305:
*  Precision (sys.precision, peer.precision, pkt.precision): This is a
*  signed integer indicating the precision of the various clocks, in
*  seconds to the nearest power of two. The value must be rounded to the
*  next larger power of two; for instance, a 50-Hz (20 ms) or 60-Hz (16.67
*  ms) power-frequency clock would be assigned the value -5 (31.25 ms),
*  while a 1000-Hz (1 ms) crystal-controlled clock would be assigned the
*  value -9 (1.95 ms).
*/
#define L7_SNTP_LOCAL_CLOCK_PRECISION         (-5)
/*********************************************************************
**  End of Base parameters
**********************************************************************/


#endif /* __INC_L7_PLATSPECS_H */
