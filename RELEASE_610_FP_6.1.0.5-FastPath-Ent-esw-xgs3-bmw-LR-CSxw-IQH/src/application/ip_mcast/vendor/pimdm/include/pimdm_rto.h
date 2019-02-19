/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_rto.h
*
* @purpose    
*
* @component  PIM-DM
*
* @comments   none
*
* @create     
*
* @author     Krishna Mohan CS
* @end
*
**********************************************************************/

#ifndef _PIMDM_RTO_H
#define _PIMDM_RTO_H

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "l3_mcast_commdefs.h"
#include "l7apptimer_api.h"
#include "pimdm_main.h"

/*******************************************************************************
**                             General Definitions                            **
*******************************************************************************/
/* None */

/*******************************************************************************
**                        Data Structure Definitions                          **
*******************************************************************************/
/* None */

/*******************************************************************************
**                  Function Prototypes Declarations                          **
*******************************************************************************/
extern L7_RC_t
pimdmRTOInit (pimdmCB_t* pimdmCB);

extern L7_RC_t
pimdmRTODeInit (pimdmCB_t* pimdmCB);

extern void
pimdmRTOTimerStart (MCAST_CB_HNDL_t cbHandle);

extern void
pimdmRTOTimerExpiryHandler (void *timerArg);

#endif  /* _PIMDM_RTO_H */

