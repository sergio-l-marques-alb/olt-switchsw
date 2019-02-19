/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_ctrl.h
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

#ifndef _PIMDM_CTRL_H
#define _PIMDM_CTRL_H

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "l3_addrdefs.h"
#include "l3_mcast_commdefs.h"
#include "pimdm_main.h"
#include "pimdm_defs.h"
#include "pimdm_mrt.h"
#include "pimdm_pkt.h"
#include "pimdm_intf.h"

/*******************************************************************************
**                             General Definitions                            **
*******************************************************************************/

/* State Refresh Message 'reserved' field Bit Definitions */
#define PIMDM_STRFR_ASRT_OVERRIDE_BIT   5
#define PIMDM_STRFR_PRUNE_NOW_BIT       6
#define PIMDM_STRFR_PRUNE_INDICATOR_BIT 7

/*******************************************************************************
**                        Data Structure Definitions                          **
*******************************************************************************/
/* None */

/*******************************************************************************
**                  Function Prototypes Declarations                          **
*******************************************************************************/
extern L7_RC_t
pimdmAssertMsgSend (pimdmCB_t *pimdmCB, L7_inet_addr_t *destnAddr,
         L7_uint32 rtrIfNum, L7_inet_addr_t *srcAddr, L7_inet_addr_t *grpAddr,
         L7_uint32 asrtMetricPref, L7_uint32 asrtMetric);


extern L7_RC_t
pimdmStateRefreshMsgSend (pimdmCB_t *pimdmCB, L7_inet_addr_t *srcAddr,
          L7_inet_addr_t *grpAddr, L7_inet_addr_t *originatorAddr,
          L7_uint32 rtrIfNum, L7_uint32 stRfrTTL, L7_uint32 stRfrInterval,
          L7_uint32 rtMetricPref, L7_uint32 rtMetric, L7_uint32 rtMaskLen,
          L7_BOOL pruneIndFlag, L7_BOOL asrtOverrideFlag, L7_BOOL pruneNowFlag);

extern L7_RC_t
pimdmBundleJPGMsgSend (pimdmCB_t *pimdmCB,
                            L7_inet_addr_t *destnAddr,
                            L7_uint32 rtrIfNum,
                            L7_inet_addr_t *srcAddr,
                            L7_inet_addr_t *grpAddr,
                            L7_inet_addr_t *upstrmNbrAddr,
                            L7_uint32 jpgHoldtime,
                            PIMDM_CTRL_PKT_TYPE_t pimdmPktType);
extern L7_RC_t
pimdmHelloMsgSend (pimdmCB_t *pimdmCB, L7_uint32 rtrIfNum,
                   pimdmIntfNbrInfo_t *pNbrInfo);

extern L7_RC_t
pimdmCtrlPktReceive (pimdmCB_t *pimdmCB,
                     L7_uint32 rtrIfNum,
                     L7_inet_addr_t *srcAddr,
                     L7_inet_addr_t *destAddr,
                     L7_uchar8 *recvdBuff,
                     L7_uint32 recvdBuffLen);

extern void
pimdmJPBundleTimerExpiryHandler(void *pParam);

extern void
pimdmJPWorkingBuffReturn (pimdmCB_t *pimdmCB,
                          pimdmNeighbor_t *pNbrEntry);

#endif  /* _PIMDM_CTRL_H */
