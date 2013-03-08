/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_debug.h
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

#ifndef PIMDM_DEBUG_H
#define PIMDM_DEBUG_H

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "osapi.h"
#include "sysapi.h"
#include "l3_addrdefs.h"
#include "pimdm_main.h"

/*******************************************************************************
**                        General Definitions                                 **
*******************************************************************************/

/*********************************************************************
*                  Trace Messages Related Definitions
*********************************************************************/

#define PIMDM_DEBUG_PRINTF sysapiPrintf

#define PIMDM_TRACE(traceLevel, __fmt__, __args__...) \
  if (pimdmDebugFlagCheck (traceLevel) == L7_TRUE) \
    pimdmDebugPrintMsg ((L7_uchar8 *)__FUNCTION__, __LINE__, __fmt__, ##__args__)

/*******************************************************************************
**                        Data Structure Definitions                          **
*******************************************************************************/

/*********************************************************************
*                   Trace Level Definition
*********************************************************************/
typedef enum
{
  PIMDM_DEBUG_INIT = 0,          /* 0  - Initialization */
  PIMDM_DEBUG_API,               /* 1  - APIs */
  PIMDM_DEBUG_CTRL_PKT,          /* 2  - Control Packets Related except Hello */
  PIMDM_DEBUG_HELLO_PKT,         /* 3  - Control Packets only Hello */
  PIMDM_DEBUG_EVENTS,            /* 4  - Events both sent/recieved */
  PIMDM_DEBUG_TIMER,             /* 5  - Timers */
  PIMDM_DEBUG_MGMD,              /* 6  - MGMD Group Membership Details */
  PIMDM_DEBUG_MFC,               /* 7  - MFC Updation Details */
  PIMDM_DEBUG_FSM_UPSTRM,        /* 8  - FSM Related */
  PIMDM_DEBUG_FSM_DNSTRM,        /* 9  - FSM Related */
  PIMDM_DEBUG_FSM_STRFR,         /* 10 - FSM Related */
  PIMDM_DEBUG_FSM_ASSERT,        /* 11 - FSM Related */
  PIMDM_DEBUG_RTO,               /* 12 - RTO Related */
  PIMDM_DEBUG_INTF,              /* 13 - Interface/Neighbor Related */
  PIMDM_DEBUG_FAILURE,           /* 14 - Failures */
  PIMDM_DEBUG_LAST
}PIMDM_TRACE_LVL_t;

/*******************************************************************************
**                  Function Prototypes Declarations                          **
*******************************************************************************/
extern L7_RC_t
pimdmDebugEnable (void);

extern L7_RC_t
pimdmDebugDisable (void);

extern L7_RC_t
pimdmDebugAllSet (void);

extern L7_RC_t
pimdmDebugAllReset (void);

extern L7_RC_t
pimdmDebugFlagSet (PIMDM_TRACE_LVL_t flag);

extern L7_RC_t
pimdmDebugFlagReset (PIMDM_TRACE_LVL_t flag);

extern L7_BOOL
pimdmDebugFlagCheck (PIMDM_TRACE_LVL_t flag);

extern void
pimdmDebugFlagShow (void);

void
pimdmDebugHelp (void);

extern void
pimdmDebugPrintMsg (L7_uchar8 *func, L7_uint32 line, L7_uchar8 *msg, ...);

extern void
pimdmDebugByteDump (L7_uchar8 *msg, L7_uint32 msgLen, PIMDM_TRACE_LVL_t trcLvl);

extern void
pimdmDebugIntfStatsClear (L7_uint32 addrFamily, L7_uint32 rtrIfNum,
                   PIMDM_CTRL_PKT_TYPE_t msgType, PIMDM_STATS_TYPE_t statsType);

extern void
pimdmDebugCBShow (L7_uint32 addrFamily);

extern void
pimdmDebugIntfAllShow (L7_uint32 addrFamily);

extern void
pimdmDebugIntfShow (L7_uint32 addrFamily, L7_uint32 rtrIfNum);

extern void
pimdmDebugNbrAllShow (L7_uint32 addrFamily);

extern void
pimdmDebugNbrShow (L7_uint32 addrFamily, L7_uint32 nbrIndex);

extern void
pimdmDebugAdminScopeShow (L7_uint32 addrFamily);

extern void
pimdmDebugMRTShow (L7_uint32 addrFamily, L7_BOOL inBriefFlag,L7_uint32 count);

extern void
pimdmDebugIntfStatsShow (L7_uint32 addrFamily,
                         L7_uint32 rtrIfNum);

#endif /* PIMDM_DEBUG_H */
