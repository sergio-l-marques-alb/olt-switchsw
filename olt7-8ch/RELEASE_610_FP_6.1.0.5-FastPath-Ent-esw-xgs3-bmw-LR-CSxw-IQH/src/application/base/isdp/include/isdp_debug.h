/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  isdp_debug.h
*
* @purpose   isdp debug utilities
*
* @component isdp
*
* @comments
*
* @create    29/11/2007
*
* @author    dgaryachy
*
* @end
*
**********************************************************************/
#ifndef ISDP_DEBUG_H
#define ISDP_DEBUG_H

#include "isdp_include.h"

typedef enum
{
  ISDP_DBG_FLAG_FIRST_ENTRY = 0,                      /* 0 */

  ISDP_DBG_FLAG_ConfigGroup,                          /* 1 */
  ISDP_DBG_FLAG_DatabaseGroup,                        /* 2 */
  ISDP_DBG_FLAG_NimGroup,                             /* 3 */
  ISDP_DBG_FLAG_PduGroup,                             /* 4 */
  ISDP_DBG_FLAG_ControlGroup,                         /* 5 */
  ISDP_DBG_FLAG_CnfgrGroup,                           /* 6 */
  ISDP_DBG_FLAG_ApiGroup,                             /* 7 */
  ISDP_DBG_FLAG_DebugGroup,                           /* 8 */

  ISDP_DBG_FLAG_LAST_ENTRY                            /* 9 */
} isdpDebugTraceFlags_t;

/***************************************************************************
 **************************FUNCTION PROTOTYPES******************************
 ***************************************************************************
 */

L7_RC_t isdpDebugCfgDump(void);
L7_RC_t isdpDebugEventTrace(isdpMsg_t msg);
L7_RC_t isdpDebugPacketTxTrace(L7_uint32 intIfNum, isdpPacket_t *pdu);
L7_RC_t isdpDebugPacketRxTrace(L7_uint32 intIfNum, isdpPacket_t *pdu);
L7_RC_t isdpDebugIntfDump(L7_uint32 intIfNum);
L7_RC_t isdpDebugStatsIntfDump(L7_uint32 intIfNum);
L7_char8* isdpDebugEventStringGet(L7_uint32 event);
void isdpDebugTrace(L7_uint32 event_flag, L7_char8 * format, ...);
void isdpDebugTraceSet(L7_uint32 event_flag);
void isdpDebugTraceClear(L7_uint32 event_flag);
void isdpDebugTraceDump(void);
void isdpDebugTraceLinkerAssist(void);

void isdpDebugRegister(void);
extern L7_RC_t isdpDebugSave(void);
extern L7_RC_t isdpDebugRestore(void);
extern void isdpDebugBuildDefaultConfigData(L7_uint32 ver);
extern L7_BOOL isdpDebugHasDataChanged(void);
extern L7_RC_t isdpDebugApplyConfigData(void);
void isdpDebugCfgRead(void);
L7_RC_t isdpDebugPacketTraceFlagSave();
void isdpDebugCfgUpdate(void);

void isdpDebugPacketTraceTxFlagSet(L7_uint32 flag);
void isdpDebugPacketTraceRxFlagSet(L7_uint32 flag);
void isdpDebugEventTraceFlagSet(L7_uint32 flag);
L7_uint32 isdpDebugPacketTraceRxFlagGet(void);
L7_uint32 isdpDebugPacketTraceTxFlagGet(void);
L7_uint32 isdpDebugEventTraceFlagGet(void);

#endif /* ISDP_DEBUG_H*/
