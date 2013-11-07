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
#define L7_BCM_SCORPION                            1
#define L7_BCM_TRIUMPH                             2
#define L7_BCM_TRIUMPH2                            3
#define L7_BCM_APOLLO                              4
#define L7_BCM_ENDURO                              5
#define L7_BCM_VALKYRIE2                           6  /* PTin added: new switch */
#define L7_BCM_TRIDENT                             7  /* PTin added: new switch */
#define L7_BCM_TRIUMPH3                            8  /* PTin added: new switch */

/* PTin added: boards definition */
#define PTIN_BOARD_OLT7_8CH_B                      0x01
#define PTIN_BOARD_TOLT8G                          0x02
#define PTIN_BOARD_TG16G                           0x04
#define PTIN_BOARD_CXP360G                         0x08
#define PTIN_BOARD_CXO640G                         0x10
#define PTIN_BOARD_TA48GE                          0x20

/* PTin added: board groups definition */
#define PTIN_BOARD_MATRIX_FAMILY      ( PTIN_BOARD_CXP360G | PTIN_BOARD_CXO640G)
#define PTIN_BOARD_LINECARD_FAMILY    ( PTIN_BOARD_TOLT8G | PTIN_BOARD_TG16G | PTIN_BOARD_TA48GE )
#define PTIN_BOARD_STANDALONE_FAMILY  ( PTIN_BOARD_OLT7_8CH_B )
#define PTIN_BOARD_GPON_FAMILY        ( PTIN_BOARD_TOLT8G | PTIN_BOARD_TG16G )
#define PTIN_BOARD_ACTIVETH_FAMILY    ( PTIN_BOARD_TA48GE )

#define PTIN_BOARD_IS_MATRIX          ( PTIN_BOARD & PTIN_BOARD_MATRIX_FAMILY )
#define PTIN_BOARD_IS_LINECARD        ( PTIN_BOARD & PTIN_BOARD_LINECARD_FAMILY )
#define PTIN_BOARD_IS_STANDALONE      ( PTIN_BOARD & PTIN_BOARD_STANDALONE_FAMILY )
#define PTIN_BOARD_IS_GPON            ( PTIN_BOARD & PTIN_BOARD_GPON_FAMILY )
#define PTIN_BOARD_IS_ACTIVETH        ( PTIN_BOARD & PTIN_BOARD_ACTIVETH_FAMILY )

#include "ptin_platform.h"                            /* PTin added: defines PLAT_BCM_CHIP and PTIN_BOARD */

#ifndef PTIN_BOARD
#error "PTIN: PTIN_BOARD is not defined!!!"
#endif
/* PTin end */

#ifndef PLAT_BCM_CHIP
//#define PLAT_BCM_CHIP                              L7_BCM_TRIUMPH
#error "PTIN: PLAT_BCM_CHIP is not defined!!!"
#endif

#include "xgs5.h"
/*********************************************************************
**  End of  CHIP Defines 
**********************************************************************/
/*********************************************************************
**  Start of  Base parameters
**********************************************************************/
/* PTin added: new switch */
#if (PTIN_BOARD == PTIN_BOARD_OLT7_8CH_B)
#define L7_MAX_PHYSICAL_SLOTS_PER_UNIT           1
#define L7_MAX_PORTS_PER_SLOT                    18    
#define L7_MAX_PHYSICAL_PORTS_PER_SLOT           18  
#define L7_MAX_PHYSICAL_PORTS_PER_UNIT           18

#elif (PTIN_BOARD == PTIN_BOARD_TOLT8G)
#define L7_MAX_PHYSICAL_SLOTS_PER_UNIT           1
#define L7_MAX_PORTS_PER_SLOT                    12
#define L7_MAX_PHYSICAL_PORTS_PER_SLOT           12
#define L7_MAX_PHYSICAL_PORTS_PER_UNIT           12

#elif (PTIN_BOARD == PTIN_BOARD_TG16G)
#define L7_MAX_PHYSICAL_SLOTS_PER_UNIT           1
#define L7_MAX_PORTS_PER_SLOT                    20
#define L7_MAX_PHYSICAL_PORTS_PER_SLOT           20
#define L7_MAX_PHYSICAL_PORTS_PER_UNIT           20

#elif (PTIN_BOARD == PTIN_BOARD_TA48GE)
#define L7_MAX_PHYSICAL_SLOTS_PER_UNIT           1
#define L7_MAX_PORTS_PER_SLOT                    52
#define L7_MAX_PHYSICAL_PORTS_PER_SLOT           52
#define L7_MAX_PHYSICAL_PORTS_PER_UNIT           52

#elif (PTIN_BOARD == PTIN_BOARD_CXP360G)
#define L7_MAX_PHYSICAL_SLOTS_PER_UNIT           1
#define L7_MAX_PORTS_PER_SLOT                    (36+1)
#define L7_MAX_PHYSICAL_PORTS_PER_SLOT           (36+1)
#define L7_MAX_PHYSICAL_PORTS_PER_UNIT           (36+1)

#elif (PTIN_BOARD == PTIN_BOARD_CXO640G)
#define L7_MAX_PHYSICAL_SLOTS_PER_UNIT           1
#define L7_MAX_PORTS_PER_SLOT                    64
#define L7_MAX_PHYSICAL_PORTS_PER_SLOT           64  
#define L7_MAX_PHYSICAL_PORTS_PER_UNIT           64

#else
/* PTin end */
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
