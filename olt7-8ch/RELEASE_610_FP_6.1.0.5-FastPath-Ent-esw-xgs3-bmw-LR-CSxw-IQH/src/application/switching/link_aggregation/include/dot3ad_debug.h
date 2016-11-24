/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename    dot3ad_debug.h
* @purpose     802.3ad link aggregation, LAC debug functions
* @component   dot3ad
* @comments    none
* @create      08/02/2006
* @author      cpverne
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/
#ifndef DOT3AD_DEBUG_H
#define DOT3AD_DEBUG_H

#include "l7_common.h"
#include "dot3ad_include.h"
#include "log.h"

extern L7_BOOL dot3adPacketDebugFlag;

#define DOT3AD_USER_TRACE(__fmt__, __args__... )                              \
          if (dot3adDebugPacketTraceFlag == L7_TRUE)                          \
          {                                                                 \
            LOG_USER_TRACE(L7_DOT3AD_COMPONENT_ID, __fmt__,##__args__);        \
          }

/*********************************************************************
*
* @purpose Trace dot3ad packets received
*
* @param   intIfNum	@b{(input)} Internal Interface Number
* @param   *buff	@b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dot3adDebugPacketRxTrace(L7_uint32 intIfNum, dot3ad_pdu_t *pdu);

/*********************************************************************
*
* @purpose Trace dot3ad packets transmitted
*
* @param   intIfNum	@b{(input)} Internal Interface Number
* @param   *buff	@b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dot3adDebugPacketTxTrace(L7_uint32 intIfNum, dot3ad_pdu_t *pdu);

#endif /* DOT3AD_DEBUG_H */
