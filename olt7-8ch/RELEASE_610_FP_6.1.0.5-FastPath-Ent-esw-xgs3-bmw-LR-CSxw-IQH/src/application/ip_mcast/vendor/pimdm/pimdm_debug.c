/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   pimdm_debug.c
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

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/
#include "l7_common.h"
#include "l3_addrdefs.h"
#include "l7_pimdm_api.h"
#include "pimdm_main.h"
#include "pimdm_debug.h"
#include "pimdm_intf.h"
#include "pimdm_mrt.h"
#include "pimdm_mgmd.h"
#include "pimdm_init.h"
#include "pimdm_admin_scope.h"
#include "pimdm_map_vend_ctrl.h"
#include "usmdb_util_api.h"

/* for debugging purpose, store as strings */
L7_uchar8           pimdmUpStrmSGStateName[PIMDM_UPSTRM_MAX_STATES]
[MCAST_STRING_SIZE_MAX] =
{
  "FORWARD", 
  "PRUNED",
  "ACKPENDING"
};

L7_uchar8           pimdmOrigSGStateName[PIMDM_STATE_RFR_MAX_STATES]
[MCAST_STRING_SIZE_MAX] =
{
  "NOT-ORIGINATOR", 
  "ORIGINATOR"
};

L7_uchar8           pimdmDnStrmSGStateName[PIMDM_DNSTRM_MAX_STATES]
[MCAST_STRING_SIZE_MAX] =
{
  "NOINFO", 
  "PRUNE-PENDING",
  "PRUNED"
};

L7_uchar8           pimdmAssertSGStateName[PIMDM_ASSERT_MAX_STATES]
[MCAST_STRING_SIZE_MAX] =
{
  "NOINFO", 
  "WINNER",
  "LOSER"
};


/*******************************************************************************
**                        General Definitions                                 **
*******************************************************************************/
#define PIMDM_NUM_FLAG_BYTES     ((PIMDM_DEBUG_LAST + 7) / 8)

/*******************************************************************************
**                        Global Declarations                                 **
*******************************************************************************/
static L7_uchar8 pimdmDebugFlags[PIMDM_NUM_FLAG_BYTES];
static L7_BOOL pimdmIsDebugEnabled = L7_FALSE;

/*******************************************************************************
**                        Function Declarations                               **
*******************************************************************************/

/*******************************************************************************
**                        Function Definitions                                **
*******************************************************************************/

/*********************************************************************
*
* @purpose  Enable Debug Tracing in PIM-DM.
*
* @param    None.
*
* @returns  L7_SUCCESS  if Debug trace was successfully enabled.
* @returns  L7_FAILURE  if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
pimdmDebugEnable (void)
{
  if (pimdmIsDebugEnabled == L7_TRUE)
  {
    PIMDM_DEBUG_PRINTF ("PIM-DM Debug Tracing is already Enabled.\n");
    return (L7_FAILURE);
  }

  pimdmIsDebugEnabled = L7_TRUE;
  PIMDM_DEBUG_PRINTF ("PIM-DM Debug Tracing is Enabled.\n");

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing in PIM-DM
*
* @param    None.
*
* @returns  L7_SUCCESS  if Debug trace was successfully disabled.
* @returns  L7_FAILURE  if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
pimdmDebugDisable (void)
{
  if (pimdmIsDebugEnabled != L7_TRUE)
  {
    PIMDM_DEBUG_PRINTF ("PIM-DM Debug Tracing is already Disabled.\n");
    return (L7_FAILURE);
  }

  pimdmIsDebugEnabled = L7_FALSE;
  PIMDM_DEBUG_PRINTF ("PIM-DM Debug Tracing is Disabled.\n");

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing for all Trace Levels
*
* @param    None.
*
* @returns  L7_SUCCESS   if Debug trace was successfully enabled.
* @returns  L7_FAILURE   if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
pimdmDebugAllSet (void)
{
  memset (pimdmDebugFlags, 0xFF, sizeof (pimdmDebugFlags));
  PIMDM_DEBUG_PRINTF ("PIM-DM Debug Tracing is Enabled for All Trace Levels.\n");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for all Trace Levels
*
* @param    None.
*
* @returns  L7_SUCCESS if Debug trace was successfully disabled.
* @returns  L7_FAILURE if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
pimdmDebugAllReset (void)
{
  memset (pimdmDebugFlags, 0, sizeof (pimdmDebugFlags));
  PIMDM_DEBUG_PRINTF ("PIM-DM Debug Tracing is Disabled for All Trace Levels.\n");
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Enable Debug Tracing for a specific Trace Level
*
* @param    traceLevel @b{ (input) } Trace Level to enable debug tracing
*
* @returns  L7_SUCCESS   if Debug trace was successfully enabled.
* @returns  L7_FAILURE   if there was an error enabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
pimdmDebugFlagSet (PIMDM_TRACE_LVL_t traceLevel)
{
  if (traceLevel >= PIMDM_DEBUG_LAST)
  {
    PIMDM_DEBUG_PRINTF ("Invalid PIM-DM Trace Level.\n");
    return L7_FAILURE;
  }

  pimdmDebugFlags[traceLevel/8] |= (1 << (traceLevel % 8));
  PIMDM_DEBUG_PRINTF ("PIM-DM Debug Tracing is enabled for Trace Level - %d.\n",
                      traceLevel);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Disable Debug Tracing for a specific Trace Level
*
* @param    traceLevel @b{ (input) } Trace Level to disable debug tracing
*
* @returns  L7_SUCCESS   if Debug trace was successfully disabled.
* @returns  L7_FAILURE   if there was an error disabling Debug Trace.
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t
pimdmDebugFlagReset (PIMDM_TRACE_LVL_t traceLevel)
{
  if (traceLevel >= PIMDM_DEBUG_LAST)
  {
    PIMDM_DEBUG_PRINTF ("Invalid PIM-DM Trace Level.\n");
    return L7_FAILURE;
  }

  pimdmDebugFlags[traceLevel/8] &= (~(1 << (traceLevel % 8)));
  PIMDM_DEBUG_PRINTF ("PIM-DM Debug Tracing is disabled for Trace Level - %d.\n",
                      traceLevel);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose
*
* @param
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    
*
* @end
*********************************************************************/

L7_BOOL
pimdmDebugFlagCheck (PIMDM_TRACE_LVL_t traceLevel)
{
  if (pimdmIsDebugEnabled != L7_TRUE)
    return L7_FALSE;

  if (traceLevel >= PIMDM_DEBUG_LAST)
    return L7_FALSE;

  if ((pimdmDebugFlags[traceLevel/8] & (1 << (traceLevel % 8))) != 0)
    return L7_TRUE;

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Shows the current PIM-DM Debug Trace Level status.
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void
pimdmDebugFlagShow (void)
{
  L7_uint32 traceIndex = 0;

  if(pimdmIsDebugEnabled != L7_TRUE)
  {
    PIMDM_DEBUG_PRINTF ("PIM-DM Debug Tracing : Disabled.\n");
    return;
  }

  PIMDM_DEBUG_PRINTF ("PIM-DM Debug Tracing : Enabled.\n");
  for(traceIndex = 0;  traceIndex < PIMDM_DEBUG_LAST; traceIndex++)
  {
    if(pimdmDebugFlagCheck (traceIndex) == L7_TRUE)
      PIMDM_DEBUG_PRINTF ("PIM-DM Debug Trace Level [%d] : Enabled.\n", traceIndex);
    else
      PIMDM_DEBUG_PRINTF ("PIM-DM Debug Trace Level [%d] : Disabled.\n", traceIndex);
  }
}

/*********************************************************************
*
* @purpose  To Print a Trace Message
*
* @param    func   @b{ (input) } Function Name
*           line   @b{ (input) } Line Number
*           msg    @b{ (input) } Message to print
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void
pimdmDebugPrintMsg (L7_uchar8 *func,
                    L7_uint32 line,
                    L7_uchar8 *msg,
                    ...)
{
  L7_uchar8 buf[PIMDM_MAX_DBG_MSG_SIZE];
  L7_timespec time;
  va_list ap;

  memset (buf, 0, sizeof (buf));
  
  va_start (ap, msg);
  (void) vsprintf (buf, msg, ap);
  va_end (ap);

  osapiUpTime(&time);

  PIMDM_DEBUG_PRINTF ("\n[%2d:%2d:%2d][PIM-DM][%s()-%d]: %s.",
                      time.hours, time.minutes, time.seconds, func, line, buf);
}
/*********************************************************************
*
* @purpose
*
* @param
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    
*
* @end
*********************************************************************/
void
pimdmDebugByteDump (L7_uchar8 *msg, L7_uint32 msgLen, PIMDM_TRACE_LVL_t trcLvl)
{
  L7_uint32 index = 0;
  L7_uchar8 *buffer = msg;

  if (pimdmDebugFlagCheck (trcLvl) != L7_TRUE)
    return;

  PIMDM_DEBUG_PRINTF ("\n************************************************\n");

  while (index < msgLen)
  {
    PIMDM_DEBUG_PRINTF ("  %02x  ", buffer[index]);
    index++;
    if ((index % 8) == 0)
      PIMDM_DEBUG_PRINTF ("\n");
  }

  PIMDM_DEBUG_PRINTF ("\n*************************************************\n");
  return;
}

/*********************************************************************
*
* @purpose  Shows the list of PIM-DM Debug Devshell Functions
*
* @param    None.
*
* @returns  None.
*
* @notes
*
* @end
*
*********************************************************************/
void
pimdmDebugHelp (void)
{
  PIMDM_DEBUG_PRINTF ("pimdmDebugTraceEnable()  - Enable Debug Tracing in PIM-DM.\n");
  PIMDM_DEBUG_PRINTF ("pimdmDebugTraceDisable() - Disable Debug Tracing in PIM-DM.\n");
  PIMDM_DEBUG_PRINTF("pimdmDebugFlagSet(flag)\n");    
  PIMDM_DEBUG_PRINTF("pimdmDebugFlagReset(flag)\n");    
  PIMDM_DEBUG_PRINTF("pimdmDebugAllSet()\n");    
  PIMDM_DEBUG_PRINTF("pimdmDebugAllReset()\n");    
  PIMDM_DEBUG_PRINTF ("     Flags ...\n");
  PIMDM_DEBUG_PRINTF ("       0  -> To Trace the Protocol Initialization Path.\n");
  PIMDM_DEBUG_PRINTF ("       1  -> To Trace all the PIM-DM APIs invoked.\n");
  PIMDM_DEBUG_PRINTF ("       2  -> To Trace the Protocol Control Packets Path.\n");
  PIMDM_DEBUG_PRINTF ("       3  -> To Trace the Protocol Hello Packets Path.\n");
  PIMDM_DEBUG_PRINTF ("       4  -> To Trace the Events Received/Sent by PIM-DM.\n");
  PIMDM_DEBUG_PRINTF ("       5  -> To Trace the PIM-DM Timer Processing Path.\n");
  PIMDM_DEBUG_PRINTF ("       6  -> To Trace the PIM-DM MGMD Events Processing Path.\n");
  PIMDM_DEBUG_PRINTF ("       7  -> To Trace the PIM-DM MFC Updation Path.\n");
  PIMDM_DEBUG_PRINTF ("       8  -> To Trace the Upstream Interface FSM Path.\n");
  PIMDM_DEBUG_PRINTF ("       9  -> To Trace the Downstream Interface FSM Path.\n");
  PIMDM_DEBUG_PRINTF ("       10 -> To Trace the State Refresh FSM Path.\n");
  PIMDM_DEBUG_PRINTF ("       11 -> To Trace the Assert FSM Path.\n");
  PIMDM_DEBUG_PRINTF ("       12 -> To Trace the PIM-DM RTO Event Processing Path.\n");
  PIMDM_DEBUG_PRINTF ("       13 -> To Trace the PIM-DM Interface/Neighbor Processing Path.\n");
  PIMDM_DEBUG_PRINTF ("       14 -> To Trace all the Failures Path.\n");

  PIMDM_DEBUG_PRINTF ("pimdmDebugCBShow (L7_uint32 addrFamily).\n");
  PIMDM_DEBUG_PRINTF ("pimdmDebugTimersShow (L7_uint32 addrFamily).\n");
  PIMDM_DEBUG_PRINTF ("pimdmDebugMgmdShow (L7_uint32 addrFamily).\n");
  PIMDM_DEBUG_PRINTF ("pimdmDebugIntfAllShow (L7_uint32 addrFamily).\n");
  PIMDM_DEBUG_PRINTF ("pimdmDebugIntfShow (L7_uint32 addrFamily, L7_uint32 rtrIfNum).\n");
  PIMDM_DEBUG_PRINTF ("pimdmDebugNbrAllShow (L7_uint32 addrFamily).\n");
  PIMDM_DEBUG_PRINTF ("pimdmDebugNbrShow (L7_uint32 addrFamily, L7_uint32 nbrIndex).\n");
  PIMDM_DEBUG_PRINTF ("pimdmDebugAdminScopeShow (L7_uint32 addrFamily).\n");
  PIMDM_DEBUG_PRINTF ("pimdmDebugMRTTableShow (L7_uint32 addrFamily, L7_uint32 count).\n");
  PIMDM_DEBUG_PRINTF ("pimdmDebugIntfStatsShow (L7_uint32 addrFamily, L7_uint32 rtrIfNum).\n"
                      "          [Use rtrIfNum=0 to display Stats for all PIM-DM Enabled interfaces]");
  PIMDM_DEBUG_PRINTF ("pimdmDebugIntfStatsClear (L7_uint32 addrFamily, L7_uint32 rtrIfNum, PIMDM_CTRL_PKT_TYPE_t msgType, PIMDM_STATS_TYPE_t statsType).\n");

  return;
}

/*********************************************************************
*
* @purpose  To Clear the PIM-DM Statistics
*
* @param    addrFamily   @b{ (input) } L7_AF_INET or L7_AF_INET6 
*           rtrIfNum     @b{ (input) } Index of the Router Interface for
*                                      Stats has to be cleared
*                                      Specifying 0 would clear stats specified
*                                      by the statsType for the specified
*                                      message on all Interfaces
*           msgType      @b{ (input) } Type of the PIM-DM Control Pkt
*                                      Specifying 0 would clear stats specified
*                                      by the statsType for all messages.
*           statsType    @b{ (input) } Type of the Statistic
*                                      Specifying 0 would clear all types of
*                                      Statistics for the specified message.
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmDebugIntfStatsClear (L7_uint32 addrFamily,
                          L7_uint32 rtrIfNum,
                          PIMDM_CTRL_PKT_TYPE_t msgType,
                          PIMDM_STATS_TYPE_t statsType)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
  {
    PIMDM_DEBUG_PRINTF ("Bad Router If Index Specified.\n");
    return;
  }
  if (msgType >= PIMDM_CTRL_PKT_MAX)
  {
    PIMDM_DEBUG_PRINTF ("Bad Messsage Type Specified.\n");
    return;
  }
  if (statsType >= PIMDM_STATS_TYPE_MAX)
  {
    PIMDM_DEBUG_PRINTF ("Bad Statistic Type Specified.\n");
    return;
  }
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }

  pimdmIntfStatsClear (pimdmCB, rtrIfNum, msgType, statsType);
}

/*********************************************************************
*
* @purpose  To display the members of the PIM-DM Control Block
*
* @param    addrFamily   @b{ (input) } L7_AF_INET or L7_AF_INET6 
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmDebugCBShow (L7_uint32 addrFamily)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }

  /* Display the Control Block Contents */
  PIMDM_DEBUG_PRINTF ("\n******************************************\n");
  pimdmCtrlBlockMembersShow (pimdmCB);
  PIMDM_DEBUG_PRINTF ("******************************************\n");

  return;
}


/*********************************************************************
*
* @purpose  To display the info of all the PIM-DM Enabled Interfaces
*
* @param    addrFamily   @b{ (input) } L7_AF_INET or L7_AF_INET6 
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmDebugIntfAllShow (L7_uint32 addrFamily)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }

  /* Display the Interface Block Contents */
  PIMDM_DEBUG_PRINTF ("\n******************************************\n");
  pimdmIntfAllShow (pimdmCB);
  PIMDM_DEBUG_PRINTF ("******************************************\n");

  return;
}

/*********************************************************************
*
* @purpose  To display the info of the specified PIM-DM Interfaces
*
* @param    addrFamily   @b{ (input) } L7_AF_INET or L7_AF_INET6 
* @param    rtrIfNum     @b{ (input) } Index of the Router Interface
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmDebugIntfShow (L7_uint32 addrFamily,
                             L7_uint32 rtrIfNum)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }
  if ((rtrIfNum < 1) || (rtrIfNum >= PIMDM_MAX_INTERFACES))
  {
    PIMDM_DEBUG_PRINTF ("Bad Router If Index Specified.\n");
    return;
  }
  if (pimdmIntfIsEnabled (pimdmCB, rtrIfNum) != L7_TRUE)
  {
    PIMDM_DEBUG_PRINTF ("PIM-DM is not Enabled on this Interface.\n");
    return;
  }

  /* Display the Interface Block Contents */
  PIMDM_DEBUG_PRINTF ("\n******************************************\n");
  pimdmIntfShow (pimdmCB, rtrIfNum);
  PIMDM_DEBUG_PRINTF ("******************************************\n");

  return;
}

/*********************************************************************
*
* @purpose  To display the info of all the PIM-DM Neighbors
*
* @param    addrFamily   @b{ (input) } L7_AF_INET or L7_AF_INET6 
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmDebugNbrAllShow (L7_uint32 addrFamily)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }

  /* Display the Neighbor Block Contents */
  PIMDM_DEBUG_PRINTF ("\n******************************************\n");
  pimdmIntfNbrAllShow (pimdmCB);
  PIMDM_DEBUG_PRINTF ("******************************************\n");

  return;
}

/*********************************************************************
*
* @purpose  To display the info of the specified PIM-DM Neighbor
*
* @param    addrFamily   @b{ (input) } L7_AF_INET or L7_AF_INET6 
* @param    nbrIndex     @b{ (input) } Index of the Neighbor Bitmask
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmDebugNbrShow (L7_uint32 addrFamily,
                            L7_uint32 nbrIndex)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }
  if (nbrIndex > PIMDM_MAX_NEIGHBORS)
  {
    PIMDM_DEBUG_PRINTF ("Bad Neighbor Index Specified.\n");
    return;
  }

  /* Display the Neighbor Block Contents */
  PIMDM_DEBUG_PRINTF ("\n******************************************\n");
  pimdmIntfNbrShow (pimdmCB, nbrIndex);
  PIMDM_DEBUG_PRINTF ("******************************************\n");

  return;
}

/*********************************************************************
*
* @purpose  To display the info of the Admin Scope Boundary Database
*
* @param    addrFamily   @b{ (input) } L7_AF_INET or L7_AF_INET6 
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmDebugAdminScopeShow (L7_uint32 addrFamily)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }

  /* Display the Admin Scope Database Contents */
  PIMDM_DEBUG_PRINTF ("\n******************************************\n");
  pimdmAdminScopeBoundaryInfoShow (pimdmCB);
  PIMDM_DEBUG_PRINTF ("******************************************\n");

  return;
}

/******************************************************************************
* @purpose  Display PIMDM Mcast Routing Table (MRT)
*
* @param    family @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimdmDebugMRTTableShow ( L7_uchar8 addrFamily,L7_int32 count)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  L7_uint32             pimdmSGEntryCount;
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_uint32 localcount=L7_NULL;
  L7_uchar8 tmpStr[MCAST_STRING_SIZE_MAX];  
  L7_uchar8 pimdmAddr1[IPV6_DISP_ADDR_LEN];
  L7_uchar8 pimdmAddr2[IPV6_DISP_ADDR_LEN];
  L7_uchar8 pimdmAddr3[IPV6_DISP_ADDR_LEN];
  L7_uint32    timeLeftGRT = 0 , timeLeftOT = 0, timeLeftPLT = 0, 
               timeLeftSAT = 0, timeLeftSRT = 0, timeLeftExpiry =0 ;
  pimdmGraftPruneState_t *grfPrnState;
  pimdmOrigState_t *origState;
  L7_APP_TMR_CTRL_BLK_t *appTimer = L7_NULLPTR;
  L7_uint32     now;
  L7_uint32     uptime;
  
  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }
  appTimer = pimdmCB->appTimer;
  
  pimdmSGEntryCount = 0;     

  pimdmSGEntryCount = avlTreeCount(&pimdmCB->mrtSGTree);
  
  PIMDM_DEBUG_PRINTF( " Number of Entries (S,G)  = %d\n", pimdmSGEntryCount );  
  /* print info */
  PIMDM_DEBUG_PRINTF("-------------------------------------------------------\n" );
  PIMDM_DEBUG_PRINTF("SrcAddr, GrpAddr, IIF, UpNbrAddr, OIFList\n" );
  PIMDM_DEBUG_PRINTF("UpStrmFSM OrigFSM\n" );
  PIMDM_DEBUG_PRINTF("GRT, OT, PLT, SRT, SAT ExT\n" );
  PIMDM_DEBUG_PRINTF("UpTime, NextHop, Metric, MetricPref\n" );
  PIMDM_DEBUG_PRINTF("-------------------------------------------------------\n" );

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_DEBUG_PRINTF ("Failed to Acquire PIM-DM MRT Semaphore");
    return;
  }

  mrtEntry = pimdmMrtEntryNextGet (pimdmCB, L7_NULLPTR, L7_NULLPTR);
  while (mrtEntry != L7_NULLPTR)
  {
    if((count != L7_NULL) &&(localcount >= count))
    {
      break;
    }
    mcastIntfBitSetToString(&mrtEntry->oifList, tmpStr, MCAST_STRING_SIZE_MAX);
    PIMDM_DEBUG_PRINTF( "%s, %s, %d, %s, %s\n",
        inetAddrPrint( &(mrtEntry->srcAddr), pimdmAddr1),
        inetAddrPrint( &(mrtEntry->grpAddr), pimdmAddr2),
         mrtEntry->upstrmRtrIfNum,
        inetAddrPrint( &(mrtEntry->upstrmNbrInfo.assertWnrAddr),  pimdmAddr3),
        tmpStr);
    PIMDM_DEBUG_PRINTF( "%s, %s\n",
        pimdmUpStrmSGStateName[mrtEntry->upstrmGraftPruneStateInfo.grfPrnState],
        pimdmOrigSGStateName[mrtEntry->origStateInfo.origState]);
    grfPrnState = &(mrtEntry->upstrmGraftPruneStateInfo);
    origState = &(mrtEntry->origStateInfo);    
    appTimerTimeLeftGet (appTimer, grfPrnState->prnLmtTimer, &timeLeftPLT);
    appTimerTimeLeftGet (appTimer, grfPrnState->overrideTimer, &timeLeftOT);
    appTimerTimeLeftGet (appTimer, grfPrnState->grftRetryTimer, &timeLeftGRT);
    appTimerTimeLeftGet (appTimer, origState->stateRfrTimer, &timeLeftSRT);
    appTimerTimeLeftGet (appTimer, origState->srcActiveTimer, &timeLeftSAT);
    appTimerTimeLeftGet (appTimer, mrtEntry->mrtEntryExpiryTimer,
        &timeLeftExpiry);    
    PIMDM_DEBUG_PRINTF( "%d, %d, %d, %d, %d, %d\n",
          timeLeftGRT, timeLeftOT, timeLeftPLT, timeLeftSRT, timeLeftSAT,
          timeLeftExpiry);
    now = osapiUpTimeRaw();
    uptime = now - mrtEntry->entryUpTime;
    PIMDM_DEBUG_PRINTF( "%d, %s, 0x%x, 0x%x\n",    
        uptime,
        inetAddrPrint( &(mrtEntry->rpfInfo.rpfNextHop), pimdmAddr1),
        mrtEntry->rpfInfo.rpfRouteMetric,
       mrtEntry->rpfInfo.rpfRouteMetricPref);
    PIMDM_DEBUG_PRINTF( "\n");    

    localcount++;

    mrtEntry = pimdmMrtEntryNextGet (pimdmCB, &(mrtEntry->srcAddr),
                                     &(mrtEntry->grpAddr));
  }

  osapiSemaGive (pimdmCB->mrtSGTree.semId);
  
}
/******************************************************************************
* @purpose  Display PIMSM Mcast Routing Table (MRT)
*
* @param    family @b{(input)}
*
* @returns  void
*
* @comments      
*       
* @end
******************************************************************************/
void pimdmDebugMRTITableShow ( L7_uchar8 addrFamily, L7_int32 count )
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  L7_uint32             pimdmSGIEntryCount;
  pimdmMrtEntry_t *mrtEntry = L7_NULLPTR;
  L7_uint32 localcount=L7_NULL, index;
  L7_uchar8 pimdmAddr1[IPV6_DISP_ADDR_LEN];
  L7_uchar8 pimdmAddr2[IPV6_DISP_ADDR_LEN];
  L7_uint32    timeLeftPT = 0 , timeLeftPPT = 0, timeLeftAT = 0; 
  L7_APP_TMR_CTRL_BLK_t *appTimer = L7_NULLPTR;
  pimdmDownstrmIntfInfo_t *dnstrmIntfInfo = L7_NULLPTR;
  pimdmAssertInfo_t *asrtIntfInfo = L7_NULLPTR;  

  
  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }
  appTimer = pimdmCB->appTimer;
  
  pimdmSGIEntryCount = 0;     

 
  PIMDM_DEBUG_PRINTF( " Number of Entries (S,G)  = %d\n", pimdmSGIEntryCount );  
  /* print info */
  PIMDM_DEBUG_PRINTF("-------------------------------------------------------\n" );
  PIMDM_DEBUG_PRINTF("SrcAddr, GrpAddr\n" );
  PIMDM_DEBUG_PRINTF("DnStrmIndex, DnStrmFSM, AssertFSM\n" );
  PIMDM_DEBUG_PRINTF("PT, PPT, AT \n" );
  PIMDM_DEBUG_PRINTF("WinAddr, WinMetric, WinPref\n" );
  PIMDM_DEBUG_PRINTF("-------------------------------------------------------\n" );  

  if (osapiSemaTake (pimdmCB->mrtSGTree.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    PIMDM_DEBUG_PRINTF ("Failed to Acquire PIM-DM MRT Semaphore");
    return;
  }

  mrtEntry = pimdmMrtEntryNextGet (pimdmCB, L7_NULLPTR, L7_NULLPTR);
  while (mrtEntry != L7_NULLPTR)
  {
    if((count != L7_NULL) &&(localcount >= count))
    {
      break;
    }
    PIMDM_DEBUG_PRINTF( "%s, %s\n",
        inetAddrPrint( &(mrtEntry->srcAddr), pimdmAddr1),
        inetAddrPrint( &(mrtEntry->grpAddr), pimdmAddr2));
    
    for (index = 1; index < PIMDM_MAX_INTERFACES; index++)
    {  
      if (pimdmIntfIsEnabled (mrtEntry->pimdmCB, index) != L7_TRUE)
      {
        continue;
      }
      if (index == mrtEntry->upstrmRtrIfNum)
      {
        continue;
      }
  
      if ((dnstrmIntfInfo = mrtEntry->downstrmStateInfo[index]) == L7_NULLPTR)
      {
        continue;
      }
      ++pimdmSGIEntryCount;  
      asrtIntfInfo = &(dnstrmIntfInfo->downstrmAssertInfo);      
      PIMDM_DEBUG_PRINTF( "%d, %s, %s\n",index,
          pimdmDnStrmSGStateName[dnstrmIntfInfo->pruneState],
          pimdmAssertSGStateName[asrtIntfInfo->assertState]);

      appTimerTimeLeftGet (appTimer, dnstrmIntfInfo->pruneTimer, &timeLeftPT);
      appTimerTimeLeftGet (appTimer, dnstrmIntfInfo->prunePndTimer, &timeLeftPPT);
      appTimerTimeLeftGet (appTimer, asrtIntfInfo->assertTimer,
          &timeLeftAT);    
      PIMDM_DEBUG_PRINTF( "%d, %d, %d\n",
            timeLeftPT, timeLeftPPT, timeLeftAT);
      PIMDM_DEBUG_PRINTF( "%s, 0x%x, 0x%x\n",    
          inetAddrPrint( &(asrtIntfInfo->assertWnrAddr), pimdmAddr1),
          asrtIntfInfo->assertWnrAsrtMetric,
         asrtIntfInfo->assertWnrAsrtPref);
      PIMDM_DEBUG_PRINTF( "\n");    
    }
    localcount++;

    mrtEntry = pimdmMrtEntryNextGet (pimdmCB, &(mrtEntry->srcAddr),
                                     &(mrtEntry->grpAddr));
  }
  PIMDM_DEBUG_PRINTF( " Number of Total Entries (S,G,I)  = %d\n",
                        pimdmSGIEntryCount );
  osapiSemaGive (pimdmCB->mrtSGTree.semId);
  
}
/*********************************************************************
*
* @purpose  To display the info of the MRT (S,G) Tree Database
*
* @param    addrFamily  @b{ (input) } L7_AF_INET or L7_AF_INET6 
*           inBriefFlag @b{ (input) } Flag to indicate Display in Detail or Brief
*                                     0 - Detail ... 1 - Breif
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmDebugMRTShow (L7_uint32 addrFamily, L7_BOOL inBriefFlag,L7_uint32 count)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }
  if ((inBriefFlag != 0) && (inBriefFlag != 1))
  {
    PIMDM_DEBUG_PRINTF ("Invalid Flag Value specified ...\n"
                        "Usage: 0 - Detail, 1 - Brief.\n");
    return;
  }
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }

  /* Display the Admin Scope Database Contents */
  PIMDM_DEBUG_PRINTF ("\n******************************************\n");
  pimdmMrtTableInfoShow (pimdmCB, inBriefFlag,count);
  PIMDM_DEBUG_PRINTF ("******************************************\n");

  return;
}

/*********************************************************************
*
* @purpose  To display the info of the MRT (S,G) Tree Database
*           for a specific Group and Source combination
*
* @param    addrFamily  @b{ (input) } L7_AF_INET or L7_AF_INET6 
* @param    srcAddr     @b{ (input) } Source Address 
* @param    grpAddr     @b{ (input) } Group Address 
*           inBriefFlag @b{ (input) } Flag to indicate Display in Detail or Brief
*                                     0 - Detail ... 1 - Breif
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmDebugMRTSrcGrpShow (L7_uint32 addrFamily,
                         L7_uchar8 srcAddr[PIMDM_MAX_DBG_MSG_SIZE],
                         L7_uchar8 grpAddr[PIMDM_MAX_DBG_MSG_SIZE],
                         L7_BOOL inBriefFlag)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  L7_inet_addr_t srcAddrInet;
  L7_inet_addr_t grpAddrInet;

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }
  if ((inBriefFlag != 0) && (inBriefFlag != 1))
  {
    PIMDM_DEBUG_PRINTF ("Invalid Flag Value specified ...\n"
                        "Usage: 0 - Detail, 1 - Brief.\n");
    return;
  }

  switch (addrFamily)
  {
    case L7_AF_INET:
    {
      /* srcAddr */
      if (usmDbParseInetAddrFromStr (srcAddr, &srcAddrInet) != L7_SUCCESS)
      {
        sysapiPrintf("Invalid srcAddr.\n");
        return;
      }
    
      /* grpAddr */
      if (usmDbParseInetAddrFromStr (grpAddr, &grpAddrInet) != L7_SUCCESS)
      {
        sysapiPrintf("Invalid grpAddr.\n");
        return;
      }
    }
    break;

    case L7_AF_INET6:
    {
      L7_char8 srcAddrStr[L7_CLI_MAX_STRING_LENGTH];
      L7_uint32 maskLen;
      L7_in6_addr_t srcAddrV6;
      extern L7_RC_t cliValidPrefixPrefixLenCheck(const L7_char8 * buf, L7_in6_addr_t * prefix, L7_uint32 * prefixLen);

      /* srcAddr */
      memset (srcAddrStr, 0, sizeof(srcAddrStr));
      OSAPI_STRNCPY_SAFE (srcAddrStr, srcAddr);
      if (cliValidPrefixPrefixLenCheck (srcAddrStr, &srcAddrV6, &maskLen) != L7_SUCCESS)
      {
        sysapiPrintf("Invalid srcAddr.\n");
        return;
      }
      inetAddressSet (addrFamily, &srcAddrV6, &srcAddrInet);

      /* grpAddr */
      memset (srcAddrStr, 0, sizeof(srcAddrStr));
      OSAPI_STRNCPY_SAFE (srcAddrStr, srcAddr);
      if (cliValidPrefixPrefixLenCheck (srcAddrStr, &srcAddrV6, &maskLen) != L7_SUCCESS)
      {
        sysapiPrintf("Invalid srcAddr.\n");
        return;
      }
      inetAddressSet (addrFamily, &srcAddrV6, &grpAddrInet);
    }
    break;

    default:
    {
      sysapiPrintf("Invalid addrFamily.\n");
      return;
    }
  }

  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }

  /* Display the Admin Scope Database Contents */
  PIMDM_DEBUG_PRINTF ("\n******************************************\n");
  pimdmMrtTableSrcGrpInfoShow (pimdmCB, &srcAddrInet, &grpAddrInet, inBriefFlag);
  PIMDM_DEBUG_PRINTF ("******************************************\n");

  return;
}

/*********************************************************************
*
* @purpose  To display the info of the MRT (S,G) Tree Database
*
* @param    addrFamily  @b{ (input) } L7_AF_INET or L7_AF_INET6 
*           inBriefFlag @b{ (input) } Flag to indicate Display in Detail or Brief
*                                     0 - Detail ... 1 - Breif
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmDebugMRTTableCountShow (L7_uint32 addrFamily)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }

  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }

  /* Display the Admin Scope Database Contents */
  PIMDM_DEBUG_PRINTF ("\n******************************************\n");
  PIMDM_DEBUG_PRINTF ("Number of Route Entries: %d.\n", avlTreeCount (&(pimdmCB->mrtSGTree)));
  PIMDM_DEBUG_PRINTF ("******************************************\n");

  return;
}

#if 0
/*********************************************************************
*
* @purpose  To display the PIM-DM Packet Statistics
*
* @param    addrFamily   @b{ (input) } L7_AF_INET or L7_AF_INET6 
* @param    rtrIfNum     @b{ (input) } Index of the Router Interface
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmDebugIntfStatsShow (L7_uint32 addrFamily,
                         L7_uint32 rtrIfNum)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }
  if ((rtrIfNum < 1) || (rtrIfNum >= PIMDM_MAX_INTERFACES))
  {
    PIMDM_DEBUG_PRINTF ("Bad Router If Index Specified.\n");
    return;
  }
  if (pimdmIntfIsEnabled (pimdmCB, rtrIfNum) != L7_TRUE)
  {
    PIMDM_DEBUG_PRINTF ("PIM-DM is not Enabled on this Interface.\n");
    return;
  }

  /* Display the Interface Block Contents */
  PIMDM_DEBUG_PRINTF ("\n******** Interface %d Statistics *********\n", rtrIfNum);
  pimdmIntfStatsShow (pimdmCB, rtrIfNum);
  PIMDM_DEBUG_PRINTF ("******************************************\n\n");

  return;
}
#else
/*********************************************************************
*
* @purpose  To display the PIM-DM Packet Statistics
*
* @param    addrFamily   @b{ (input) } L7_AF_INET or L7_AF_INET6 
* @param    rtrIfNum     @b{ (input) } Index of the Router Interface
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmDebugIntfStatsShow (L7_uint32 addrFamily,
                         L7_uint32 rtrIfNum)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;
  L7_uint32 ifIndex = 0;

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family - %d Specified.\n", addrFamily);
    return;
  }
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Error in getting the Control Block for family - %d.\n", addrFamily);
    return;
  }
  if (rtrIfNum >= PIMDM_MAX_INTERFACES)
  {
    PIMDM_DEBUG_PRINTF ("Bad rtrIfNum - %d Specified.\n", rtrIfNum);
    return;
  }
  if (rtrIfNum != 0)
  {
    if (pimdmIntfIsEnabled (pimdmCB, rtrIfNum) != L7_TRUE)
    {
      PIMDM_DEBUG_PRINTF ("PIM-DM is not Enabled on Intf - %d.\n", rtrIfNum);
      return;
    }
  }

  PIMDM_DEBUG_PRINTF ("\nRx      - Packet Received in Protocol, Processing Successful.\n");
  PIMDM_DEBUG_PRINTF ("Tx      - Packet Sent from Protocol.\n");
  PIMDM_DEBUG_PRINTF ("Drop    - Packet Received in Protocol, Processing UnSuccessful.\n");
  PIMDM_DEBUG_PRINTF ("Invalid - Packet Received in Protocol, Basic Validations Failed.\n");

  PIMDM_DEBUG_PRINTF ("\n================================================================================\n");
  PIMDM_DEBUG_PRINTF ("Intf Stat    Hello Join/Pru   Assert    Graft GraftAcK StateRfr NoCache WrongIF\n");
  PIMDM_DEBUG_PRINTF ("================================================================================\n");

  if (rtrIfNum == 0)
  {
    for (ifIndex = 1; ifIndex < PIMDM_MAX_INTERFACES; ifIndex++)
    {
      pimdmIntfStatsDisplay (pimdmCB, ifIndex);
    }
  }
  else
  {
    pimdmIntfStatsDisplay (pimdmCB, rtrIfNum);
  }

  return;
}
#endif

/*********************************************************************
*
* @purpose  To display the info of PIM-DM Mgmd Database 
*
* @param    addrFamily   @b{ (input) } L7_AF_INET or L7_AF_INET6 
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void
pimdmDebugMgmdShow (L7_uint32 addrFamily, L7_int32 count)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, " Entry ");

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }

  /* Display the PIMDM mgmd database */
  PIMDM_DEBUG_PRINTF ("\n******************************************\n");
  pimdmMgmdShow(pimdmCB,count);
  PIMDM_DEBUG_PRINTF ("******************************************\n");

  PIMDM_TRACE (PIMDM_DEBUG_API, " Exit ");
  return;
}

/*********************************************************************
*
* @purpose  To configure TTL threshold used in PIMDM State efresh message
*
* @param    addrFamily   @b{ (input) } L7_AF_INET or L7_AF_INET6 
* @param    rtrIfNum     @b{ (input) } router Interface Number 
* @param    ttlThreshold @b{ (input) } ttl threshold value 
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    This API is provided only for debugging purpose and should
*           not be used anywhere in the code flow.
*
* @end
*********************************************************************/
void pimdmStubStateRefreshTTLThresholdConfigure(L7_uint32 addrFamily,
                                                L7_uint32 rtrIfNum,
                                                L7_uint32 ttlThreshold)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;

  PIMDM_TRACE (PIMDM_DEBUG_API, " Entry ");

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }

  pimdmIntfStateRefreshTTLThresholdSet(pimdmCB,rtrIfNum,ttlThreshold);

  
}

/*********************************************************************
*
* @purpose  
*
* @param    addrFamily   @b{ (input) } L7_AF_INET or L7_AF_INET6 
*
* @returns  void, if success
* @returns  void, if failure
*
* @notes    
*
* @end
*********************************************************************/
void
pimdmDebugTimersShow (L7_uint32 addrFamily)
{
  pimdmCB_t *pimdmCB = L7_NULLPTR;

  /* Basic validations */
  if ((addrFamily != L7_AF_INET) && (addrFamily != L7_AF_INET6))
  {
    PIMDM_DEBUG_PRINTF ("Bad Address Family Specified.\n");
    return;
  }
  if ((pimdmCB = (pimdmCB_t *) pimdmMapProtocolCtrlBlockGet (addrFamily))
              == L7_NULLPTR)
  {
    PIMDM_DEBUG_PRINTF ("Unknown Error in getting Protocol Control Block.\n");
    return;
  }

  appTimerDebugShow (pimdmCB->appTimer);
  return;
}

