/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_debug.c
*
* @purpose   Multiple Spanning tree debug utilities
*
* @component dot1s
*
* @comments
*
* @create    12/02/2002
*
* @author    spetriccione
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_util_api.h"
#include "osapi.h"
#include "dot1s_include.h"
#include "support_api.h"

#include "log.h"

#include "dot1s_debug.h"
#include "dot1s_debug_api.h"

#define DOT1S_DEBUG_PACKET_RX_FORMAT "Pkt RX - Intf:%d/%d/%d(%d,%s), Source_Mac:%02x:%02x:%02x:%02x:%02x:%02x, Version:%d, Root_Mac:%02x:%02x:%02x:%02x:%02x:%02x, Root_Priority:0x%x Path_Cost:%d"
#define DOT1S_DEBUG_PACKET_TX_FORMAT "Pkt TX - Intf:%d/%d/%d(%d,%s), Source_Mac:%02x:%02x:%02x:%02x:%02x:%02x, Version:%d, Root_Mac:%02x:%02x:%02x:%02x:%02x:%02x, Root_Priority:0x%x Path_Cost:%d"


L7_BOOL dot1sDebugPacketTraceTxFlag = L7_FALSE;
L7_BOOL dot1sDebugPacketTraceRxFlag = L7_FALSE;
/* Flag to determine if the debug info is displayed on screen
   or in Trace : Currently used for NSF only.
*/
L7_BOOL traceConsoleMode = L7_FALSE;

extern dot1sDebugCfg_t dot1sDebugCfg;

extern void *dot1s_queue;
extern void *dot1s_stateCB_queue;
extern L7_uint32 MbufsFree;
extern  DOT1S_VLAN_MASK_t           dot1sVlanMask;
extern dot1sCfg_t                  *dot1sCfg;
extern DOT1S_PORT_COMMON_t          *dot1sPort;

L7_uint32 dot1sDebug = L7_NULL;
L7_uint32 dot1sDebugMsgs = L7_NULL;
L7_uint32 dot1sNsfDebug = DOT1S_DEBUG_NSF_NONE;
L7_uint32 debugInstIndex = 0;

static void dot1sPortTestConfigDataBuild (DOT1S_PORT_COMMON_CFG_t *portCfg, L7_uint32 seed);
static L7_RC_t dot1sCfgInstanceTestPopulate(DOT1S_INSTANCE_INFO_CFG_t *instance,
                                            L7_uint32 instIndex, L7_uint32 instId);
static void dot1sPortTestConfigDataBuild (DOT1S_PORT_COMMON_CFG_t *portCfg, L7_uint32 seed);
void dot1sCfgShow(L7_uint32 printFlag);
static L7_RC_t dot1sCfgInstanceShow(DOT1S_INSTANCE_INFO_CFG_t *instance, L7_uint32 instIndex,
                                    L7_uint32 instId);
static void dot1sPortCfgShow (DOT1S_PORT_COMMON_CFG_t *portCfg);


static void dot1sDebugRegister(void);

/* printing time spent in queue and time taken to process message*/
extern L7_uint32 dot1sQueueTime[10];     /* store time spent in dot1s Queue*/
extern L7_uint32 dot1sBPDUProcessTime[10]; /* store time taken to process last 10 bpdus*/

/*extern static L7_uint32 Qcnt;
extern static L7_uint32 MsgCnt; */

static L7_uint32 dot1sEventTraceHdl = ~0;
extern L7_BOOL *syncInProgress;
static L7_uint32 dot1sNsfTraceHdl = ~0;
static L7_uint32 dot1sNsfTraceWidth = 0;

void dot1sDebugMsgsSet(L7_BOOL debug)
{
  dot1sDebugMsgs = debug;
}

char *dot1sRoleStringGet(DOT1S_PORT_ROLE_t role)
{
  switch (role)
  {
    case ROLE_DISABLED:return "ROLE_DISABLED";
    case ROLE_ALTERNATE:return "ROLE_ALTERNATE";
    case ROLE_BACKUP:return "ROLE_BACKUP";
    case ROLE_ROOT:return "ROLE_ROOT";
    case ROLE_DESIGNATED:return "ROLE_DESIGNATED";
    case ROLE_MASTER:return "ROLE_MASTER";
    default: return "Unknown Role";
  }

}

char *dot1sStateStringGet(L7_uint32 state)
{
  switch (state)
  {
    case L7_DOT1S_DISCARDING:return "L7_DOT1S_DISCARDING";
    case L7_DOT1S_LEARNING:return "L7_DOT1S_LEARNING";
    case L7_DOT1S_FORWARDING:return "L7_DOT1S_FORWARDING";
    case L7_DOT1S_DISABLED:return "L7_DOT1S_DISABLED";
    case L7_DOT1S_MANUAL_FWD:return "L7_DOT1S_MANUAL_FWD";
    case L7_DOT1S_NOT_PARTICIPATE:return "L7_DOT1S_NOT_PARTICIPATE";
    default: return "Unknown State";
  }
}

char * dot1sRcvdInfoStringGet(DOT1S_RCVD_INFO_t rcvdInfo)
{
  switch (rcvdInfo)
  {
  case SuperiorDesignatedInfo:return "SUPERIOR_DEISGNATED_INFO";
  case RepeatedDesignatedInfo:return "REPEATED_DEISGNATED_INFO";
  case InferiorDesignatedInfo:return "INFERIOR_DEISGNATED_INFO";
  case InferiorRootAlternateInfo:return "INFERIOR_ROOTALT_INFO";
  case OtherInfo:return "OTHER_INFO";
  default:return "UNKNOWN INFO";
  }
}

/*********************************************************************
* @purpose  Register to general debug infrastructure
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dot1sDebugRegister(void)
{
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_DOT1S_COMPONENT_ID;

    /* Register User Control Parms */

    supportDebugDescr.userControl.notifySave = dot1sDebugSave;
    supportDebugDescr.userControl.hasDataChanged = dot1sDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = dot1sDebugRestore;

    /* Register Internal Parms */
    supportDebugDescr.internal.debugHelp = dot1sDebugHelp;
    strncpy(supportDebugDescr.internal.debugHelpRoutineName,
            "dot1sDebugHelp",SUPPORT_DEBUG_HELP_NAME_SIZE);
    strncpy(supportDebugDescr.internal.debugHelpDescr,
            "Help Menu for Dot1s",SUPPORT_DEBUG_HELP_DESCR_SIZE);

    (void) supportDebugRegister(supportDebugDescr);
}

/*********************************************************************
* @purpose  List the Debug commands availiable
*
* @param    @b{(input)}
* @param    @b{(output)}
*
* @returns  L7_SUCCESS
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1sDebugHelp()
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Available dot1s Debug commands:\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "-------------------------------\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sSmStates(L7_uint32 intIfNum, "
                "L7_uint32 instIndex)\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sSizesShow()\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sMsgQueue()\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sPortStates(L7_uint32 intIfNum)\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sPortDiscard(L7_uint32 intIfNum)\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "void dot1sPortParms(L7_uint32 intIfNum)\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sMbufs()\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sIsVlanSet(L7_uint32 vlanId)\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sDebugVidListGet(L7_uint32 mstId)\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sDebugFlagsHelp()\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sDebugFlagsSet(int debugFlag,int instIndex)\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sDebugMaskShow() Show the current value of mask\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sDebugNsf(int debugFlag)\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sDebugNsfShow()\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sDebugNsfHelp()\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sDebugSMFlagsSet(int debugFlag, int instIndex)\r\n - "
                "Same as dot1sDebugFlagsSet,expect that it sets the third and "
                "fourth bytes for flags greater than 16.\r\n  ");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sDebugPrintQueueTime() \r\n- Function to print the "
                "amount of time spent the last 10 messages in the queue \r\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void dot1sDebugPrintMsgProcTime()\r\n- Function to print the "
                "amount of time spent processing last 10 messages received \r\n");





}

/*********************************************************************
* @purpose  Do any initilization required for the debug utilities.
*
* @param    @b{(input)}
* @param    @b{(output)}
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sDebugInit()
{
  /* register for debug */
  dot1sDebugRegister();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Display the current state for an interface in a specific
*           instance in each of the MSTP state machines.
*
* @param    intIfNum @b{(input)} interface number
* @param    instIndex @b{(output)} MSTP instance Index
*
* @returns
*
* @comments  devshell command
*
* @end
*********************************************************************/
void dot1sSmStates(L7_uint32 intIfNum, L7_uint32 instIndex)
{
  DOT1S_PORT_COMMON_t *p;

  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                 "Port Number : %d Instance Index: %d\n", p->portNum, instIndex);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Port Enabled : %d\n", p->portEnabled);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Instance Num %d inUse : %d\n\n", dot1sInstanceMap[instIndex].instNumber,
                  dot1sInstanceMap[instIndex].inUse);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Sync In Progress for instance %d instIndex %d is %d\n", dot1sInstanceMap[instIndex].instNumber,
                instIndex, syncInProgress[instIndex]);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "PIM State : %d\n", p->portInstInfo[instIndex].pimState);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "PRT State : %d\n", p->portInstInfo[instIndex].prtState);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "PST State : %d\n", p->portInstInfo[instIndex].pstState);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "TCM State : %d\n", p->portInstInfo[instIndex].tcmState);

    if (instIndex == 0)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "PRS State : %d\n", dot1sInstance->cist.prsState);
    }
    else
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "PRS State : %d\n", dot1sInstance->msti[instIndex].prsState);

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "PRX State : %d\n", p->prxState);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "PTX State : %d\n", p->ptxState);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "PPM State : %d\n", p->ppmState);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "BDM State : %d\n\n", p->bdmState);

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "STP State of Port = %d\n", p->portInstInfo[instIndex].portState);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Port Role = %d\n", p->portInstInfo[instIndex].role);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Port Selected Role = %d\n", p->portInstInfo[instIndex].selectedRole);
  }

}


/*********************************************************************
* @purpose  Display the sizes of the MSTP structures
*
* @param    @b{(input)}
* @param    @b{(output)}
*
* @returns  void
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1sSizesShow()
{

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "DOT1S Data Structures:\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "----------------------\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof dot1sBridge = %d\r\n", sizeof(DOT1S_BRIDGE_t));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof DOT1S_INSTANCE_INFO_t = %d\r\n", sizeof(DOT1S_INSTANCE_INFO_t));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof dot1sInstance = %d\r\n", sizeof(dot1sInstance));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof DOT1S_PORT_COMMON_t = %d\r\n", sizeof(DOT1S_PORT_COMMON_t));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof dot1sPort = %d\r\n", sizeof(DOT1S_PORT_COMMON_t)*L7_DOT1S_MAX_INTERFACE_COUNT);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof DOT1S_PORT_COMMON_CFG_t = %d\r\n", sizeof(DOT1S_PORT_COMMON_CFG_t));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof dot1sCfgPort = %d\r\n", sizeof(DOT1S_PORT_COMMON_CFG_t) * (L7_MAX_INTERFACE_COUNT + 1));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof L7_MAX_INTERFACE_COUNT = %d\r\n", L7_MAX_INTERFACE_COUNT);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof DOT1S_INSTANCE_MAP_t = %d\r\n", sizeof(DOT1S_INSTANCE_MAP_t));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof dot1sInstanceMap = %d\r\n", sizeof(dot1sInstanceMap));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof DOT1S_INST_VLAN_MAP_t = %d\r\n", sizeof(DOT1S_INST_VLAN_MAP_t));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof dot1sInstVlanMap = %d\r\n", sizeof(DOT1S_INST_VLAN_MAP_t)*(L7_MAX_VLAN_ID+2));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof L7_DOT1Q_MAX_VLAN_ID+2 = %d\r\n", L7_DOT1Q_MAX_VLAN_ID+2);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "sizeof dot1sCfg_t = %d\r\n", sizeof(dot1sCfg_t));

}

/*********************************************************************
* @purpose  Display the number of messages in the MSTP message queue.
*
* @param    @b{(input)}
* @param    @b{(output)}
*
* @returns
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1sMsgQueue()
{
  L7_int32 num;

  if(osapiMsgQueueGetNumMsgs(dot1s_queue, &num) == L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Dot1s Messages in queue : %d\n", num);
  }

  if(osapiMsgQueueGetNumMsgs(dot1s_stateCB_queue, &num) == L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                  "Dot1s Timer Messages in queue : %d\n", num);
  }
}

/*********************************************************************
* @purpose  Display the Port state information
*
* @param    intIfNum @b{(input)} interface number
* @param    @b{(output)}
*
* @returns
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1sPortStates(L7_uint32 intIfNum)
{
  L7_uint32 adminState;
  DOT1S_PORT_COMMON_t *p;
  p = dot1sIntfFind(intIfNum);

  if (p == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Cannot find dot1s Port for interface %d ", intIfNum);
    return;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "dot1sBridge.Mode = %s\r\n", (dot1sBridge->Mode ? "Enabled" : "Disabled"));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "dot1sBridge.enabledPortCount = %x\r\n", dot1sBridge->enabledPortCount);

  if (nimCheckIfNumber(intIfNum) == L7_SUCCESS)
  {
    nimGetIntfAdminState(intIfNum, &adminState);

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  ".portNum = %d\r\n", p->portNum);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "nim Admin State = %s\r\n", (adminState ? "Enabled" : "Disabled"));
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\r\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  ".portAdminMode = %s\r\n", (p->portAdminMode ? "Enable" : "Disable"));
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  ".portEnabled = %s\r\n", (p->portEnabled ? "True" : "False"));
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  ".portLinkState = %x\r\n", p->portLinkState);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  ".ignoreLinkStateChanges = %s\r\n", (p->ignoreLinkStateChanges ? "True" : "False"));
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  ".notParticipating = %s\r\n", (p->notParticipating ? "True" : "False"));
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  ".adminEdge = %s\r\n", (p->adminEdge ? "True" : "False"));
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  ".operEdge = %s\r\n", (p->operEdge ? "True" : "False"));
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  ".bpduGuardEffect = %s\r\n", (p->bpduGuardEffect ? "True" : "False"));
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "nim Admin State = FAILED TO GET\r\n");
  }
}
/*********************************************************************
* @purpose  Display the Port discard counters
*
* @param    intIfNum @b{(input)} interface number
* @param    @b{(output)}
*
* @returns
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1sPortDiscard(L7_uint32 intIfNum)
{
  DOT1S_PORT_COMMON_t *p;
  L7_uint32 portIndex = dot1sPortIndexFromIntfNumGet(intIfNum);

  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                 "Port Number : %d BPDU Rx Discards: %d\n",
                  p->portNum, dot1sPortStats[portIndex].rxDiscards);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                 "Port Number : %d BPDU Tx Discards: %d\n",
                  p->portNum, dot1sPortStats[portIndex].txDiscards);


  }

}
/*********************************************************************
* @purpose  Display the Port parms
*
* @param    intIfNum @b{(input)} interface number
* @param    @b{(output)}
*
* @returns
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1sPortParms(L7_uint32 intIfNum)
{
  DOT1S_PORT_COMMON_t *p;
  L7_uint32 i;

  p = dot1sIntfFind(intIfNum);
  if (p != L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Port Number : %d\n", p->portNum);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Port UpEnabledNum : %d\n", p->portUpEnabledNum);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Port infoInternal: %d\n", p->infoInternal );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Port rcvdInternal: %d\n", p->rcvdInternal );

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Port rcvdMSTP: %d\n", p->rcvdMSTP );

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Port rcvdRSTP: %d\n", p->rcvdRSTP );

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Port rcvdSTP: %d\n", p->rcvdSTP );

        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                                  "Port sendRSTP: %d\n", p->sendRSTP );

        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                                  "Port loopGuard: %d\n", p->loopGuard );
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                                  "Port loopInconsistent: %d\n", p->loopInconsistent );


    for (i = 0; i<= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
    {
      if (dot1sInstanceMap[i].inUse == L7_TRUE)
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Instance Index: %d\n", i );
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Instance infoIs: %d\n", p->portInstInfo[i].infoIs );
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Instance rcvdInfo: %d\n", p->portInstInfo[i].rcvdInfo );
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Instance rcvdMsg: %d\n", p->portInstInfo[i].rcvdMsg );
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Instance reselect: %d\n", p->portInstInfo[i].reselect );
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Instance selected: %d\n", p->portInstInfo[i].selected );
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Instance updtInfo: %d\n", p->portInstInfo[i].updtInfo );
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Instance agree: %d\n", p->portInstInfo[i].agree );
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Instance agreed: %d\n", p->portInstInfo[i].agreed );
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Instance changedMaster: %d\n", p->portInstInfo[i].changedMaster );
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Instance proposed: %d\n", p->portInstInfo[i].proposed );
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Instance proposing: %d\n", p->portInstInfo[i].proposing );
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Instance sync: %d\n", p->portInstInfo[i].sync );
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Instance synced: %d\n", p->portInstInfo[i].synced );


      }
    }
  }
}

void dot1sDebugPortNums()
{
  L7_uint32 portIndex = 1;
  for (portIndex = 1; portIndex < L7_DOT1S_MAX_INTERFACE_COUNT; portIndex++)
  {
    printf("PortIndex %d PortNum %d \n", portIndex, dot1sPort[portIndex].portNum);
  }
}
/*********************************************************************
* @purpose  Display the Mbufs available in the system
*
* @param    @b{(input)}
* @param    @b{(output)}
*
* @returns
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1sMbufs()
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Free Mbufs: %d\n",MbufsFree  );

}
/*********************************************************************
* @purpose  Displays if a vlan bit is set in the vlan mask
*
* @param    vlanId @b{(input)} VLAN Id
*
* @returns
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1sIsVlanSet(L7_uint32 vlanId)
{
  if (vlanId == 0)
  {
    return;
  }
  if (DOT1S_VLAN_ISMASKBITSET(dot1sVlanMask, vlanId))
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "VLANID %u is Set in the mask\n",vlanId  );
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "VLANID %u is NOT Set in the mask\n",vlanId  );
  }
}
/*********************************************************************
* @purpose  Displays the VID list associated with a particular instId
*
* @param    mstId @b{(input)} Instance Id
*
* @returns
*
* @comments devshell command
*
* @end
*********************************************************************/
void dot1sDebugVidListGet(L7_uint32 mstId)
{
  L7_uint32 numVlan;
  L7_ushort16 vidList[L7_MAX_VLANS + 1];
  L7_uint32 i;

  dot1sMstiVIDList(mstId, vidList, &numVlan);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "MSTID %u has %u vlans associated\n", mstId, numVlan);
  for (i = 0; i < numVlan; i++)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "VLANID %u\n", vidList[i]);
  }
}




/*********************************************************************
* @purpose  Sets the state change debug flag to the passed value
*
* @param    debugFlag @b{(input)} Value of the flag to be set
*
* @param    instIndex @b{(input)} instIndex
*
* @returns
*
* @comments devshell command
*
* @end
*********************************************************************/

void dot1sDebugFlagsSet(L7_uint32 debugFlag, L7_uint32 instIndex)
{

    dot1sDebug = debugFlag;

    debugInstIndex = instIndex;

}


void dot1sDebugSMFlagsSet(L7_uint32 debugFlag, L7_uint32 instIndex)
{
    /* State machine flags sit on the next 2 bytes */
    L7_uint32 tmpDebug  = debugFlag << 16 ;

    dot1sDebug &= 0xffff; /* Clear the state machine bits */
    dot1sDebug |= tmpDebug;
    debugInstIndex = instIndex;

}


void dot1sDebugFlagsHelp()
{
    /* State machine flags sit on the next 2 bytes */
    L7_uint32 i =0;

    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Flag positions in the Mask\n" );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_PROTO_HANDSHAKE\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_PROTO_ROLE\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_PROTO_DISPUTE\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_PROTO_SYNC\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_RCVD_BPDU\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_RCVD_FLAGS\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_RCVD_INTERNAL\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_RCVD_INFO\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_RCVD_INFO_DETAIL\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_TIMER_EXPIRY\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_TIMER_EXPIRY_DETAIL\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_RCVD_TC\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_DETECTED_TC\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_FLUSH\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_STATE_CHANGE\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_DESIG_PRT\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_ROOT_PRT\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_MASTER_PRT\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_ALTBKUP_PRT\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_PIM\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_BDM\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_PPM\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_STATE_MACHINE\n", i++ );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%d DOT1S_DEBUG_MSG_PROC_TIME\n",i);
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "This flag records the time spent by the last 10 messages \n"
                  "in the dot1s queue and the time taken to process the last 10\n"
                  " BPDUs recieved.\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Note this flag is only operational for CIST instance\n");

}


void dot1sDebugMaskShow()
{
   SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "debug flag %x debug Instance is %d \n", dot1sDebug,debugInstIndex);

   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_PROTO_HANDSHAKE);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_PROTO_ROLE);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_PROTO_DISPUTE);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_PROTO_SYNC);

    /*DOT1S_DEBUG_RCVD_CLASS */
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_RCVD_BPDU);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_RCVD_FLAGS);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_RCVD_INTERNAL);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_RCVD_INFO);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_RCVD_INFO_DETAIL);

    /*DOT1S_DEBUG_TIMERS_CLASS */
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_TIMER_EXPIRY);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_TIMER_EXPIRY_DETAIL);

    /*DOT1S_DEBUG_TC_CLASS */
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_RCVD_TC);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_DETECTED_TC);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_FLUSH);

    /*DOT1S_DEBUG_STATE_CLASS */
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_STATE_CHANGE);

   /*DOT1S_DEBUG_DETAIL_CLASS,   */
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_DESIG_PRT);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_ROOT_PRT);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_MASTER_PRT);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_ALTBKUP_PRT);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_PIM);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_BDM);
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_PPM);




    /*DOT1S_DEBUG_MIGRATION_CLASS */
   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_STATE_MACHINE);

   DOT1S_DEBUG_FLAG_PRINT(DOT1S_DEBUG_MSG_PROC_TIME);

}

void dot1sDebugNsf(L7_uint32 flag)
{
  dot1sNsfDebug = flag;
}

void dot1sDebugNsfShow()
{
  DOT1S_DEBUG_NSF_FLAG_PRINT(DOT1S_DEBUG_NSF_NIM);
  DOT1S_DEBUG_NSF_FLAG_PRINT(DOT1S_DEBUG_NSF_CKPT);
  DOT1S_DEBUG_NSF_FLAG_PRINT(DOT1S_DEBUG_NSF_CKPT_DETAIL);
  DOT1S_DEBUG_NSF_FLAG_PRINT(DOT1S_DEBUG_NSF_HELPER);
  DOT1S_DEBUG_NSF_FLAG_PRINT(DOT1S_DEBUG_NSF_TRANSPORT);
  DOT1S_DEBUG_NSF_FLAG_PRINT(DOT1S_DEBUG_NSF_RECONCILE);
  DOT1S_DEBUG_NSF_FLAG_PRINT(DOT1S_DEBUG_NSF_HELPER_DETAIL);
  DOT1S_DEBUG_NSF_FLAG_PRINT(DOT1S_DEBUG_NSF_RECONCILE_EVENTS);
  DOT1S_DEBUG_NSF_FLAG_PRINT(DOT1S_DEBUG_NSF_RECONCILE_STATUS);
}

void dot1sDebugNsfHelp()
{
  /* State machine flags sit on the next 2 bytes */
  L7_uint32 i =0;

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Flag positions in the Mask\n" );
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%d DOT1S_DEBUG_NSF_NIM\n", i++ );
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%d DOT1S_DEBUG_NSF_CKPT\n", i++ );
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%d DOT1S_DEBUG_NSF_CKPT_DETAIL\n", i++ );
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%d DOT1S_DEBUG_NSF_HELPER\n", i++ );
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%d DOT1S_DEBUG_NSF_TRANSPORT\n", i++ );
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%d DOT1S_DEBUG_NSF_RECONCILE\n", i++ );
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%d DOT1S_DEBUG_NSF_HELPER_DETAIL\n", i++ );
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%d DOT1S_DEBUG_NSF_RECONCILE_EVENTS\n", i++ );
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%d DOT1S_DEBUG_NSF_RECONCILE_STATUS\n", i++ );

}

/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void dot1sBuildTestConfigData(void)
{
    L7_uint32 i = 0;
    L7_uchar8 mac[L7_MAC_ADDR_LEN];
    L7_uchar8 cfgName[DOT1S_MAX_CONFIG_NAME_SIZE];
    DOT1S_BRIDGE_CFG_t *cfgBridge;
    DOT1S_INSTANCE_INFO_CFG_t *cfgInstance;
    L7_uint32 instIndex;



  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/


     /* default values for the dot1sBridge structure */
    cfgBridge = &dot1sCfg->cfg.dot1sBridge;

    cfgBridge->Mode = L7_ENABLE;
    cfgBridge->ForceVersion = DOT1S_FORCE_VERSION_DOT1W;
    cfgBridge->FwdDelay = 18 * DOT1S_TIMER_UNIT;
    cfgBridge->TxHoldCount = 6;
    cfgBridge->MigrateTime = 6 * DOT1S_TIMER_UNIT;
    cfgBridge->MaxHops = 30;
    cfgBridge->instanceCount = 0;
    cfgBridge->MstConfigId.formatSelector = 0;
    dot1sBaseMacAddrGet(mac);
    sprintf((L7_char8 *)cfgName, "%02X-%02X-%02X-%02X-%02X-%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    memcpy((L7_uchar8 *)&cfgBridge->MstConfigId.configName, cfgName, sizeof(cfgName));
    cfgBridge->MstConfigId.revLevel = 1;

    /* default values for the dot1sInstVlanMap structure */
    instIndex = 1;
    for (i = 0; i < L7_DOT1Q_MAX_VLAN_ID + 2; i++)
    {

        dot1sCfg->cfg.dot1sInstVlanMap[i].instNumber = 0;
        instIndex++;

        /* Reset if going beyond allowable value of instance ID */
        if (instIndex%L7_MAX_MULTIPLE_STP_INSTANCES)
            instIndex = 1;
    }

    /* calculate the MSTP Configuration Digest after the Instance to Vlan Map is
     * initialized.
     */
    dot1sInstVlanMapEndianSafeMake(dot1sCfg->cfg.dot1sInstVlanMap);

    /* No need to store the digest key in the configuration file*/
    /*
    L7_hmac_md5((L7_uchar8 *)&dot1sInstVlanMapEndianSafe, (L7_uint32)sizeof(dot1sInstVlanMapEndianSafe),
                (L7_uchar8 *)&dot1sConfigDigSigKey, (L7_uint32)sizeof(dot1sConfigDigSigKey),
                (L7_uchar8 *)&dot1sCfg->cfg.dot1sBridge.MstConfigId.configDigest);
    */
    cfgInstance = &dot1sCfg->cfg.dot1sInstance;

    /* default values for the cist dot1sInstance structure */
    dot1sCfgInstanceTestPopulate(cfgInstance, DOT1S_CIST_INDEX, DOT1S_CIST_ID);

    /* default values for the msti dot1sInstance structures */
    for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
    {
      dot1sCfgInstanceTestPopulate(cfgInstance, i, L7_NULL);
    }


    /* default values for the dot1sInstanceMap structure */
    dot1sCfg->cfg.dot1sInstanceMap[0].instNumber = DOT1S_CIST_ID;
    dot1sCfg->cfg.dot1sInstanceMap[0].inUse = L7_TRUE;


    for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
    {
        dot1sCfg->cfg.dot1sInstanceMap[i].instNumber = i;
        dot1sCfg->cfg.dot1sInstanceMap[i].inUse = L7_TRUE;

    }

    for (i = 1; i < L7_DOT1S_MAX_INTERFACE_COUNT; i++)
    {

      dot1sPortTestConfigDataBuild (&dot1sCfg->cfg.dot1sCfgPort[i], i);

    }

 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   dot1sCfg->hdr.dataChanged = L7_TRUE;
   sysapiPrintf("Built test config data\n");

}

/*********************************************************************
* @purpose  Populate the Dot1s Instance Structure with Default Values
*
* @param    Instance  @b{(input)} pointer to a dot1s Instance structure
* @param    instIndex @b{(input)} instance Index
* @param    instId    @b{(input)} MSTP instance id
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sCfgInstanceTestPopulate(DOT1S_INSTANCE_INFO_CFG_t *instance,
                                        L7_uint32 instIndex,
                                        L7_uint32 instId)
{

  dot1sCfgInstanceMacAddrPopulate(instance, instIndex);

  if (instIndex == DOT1S_CIST_INDEX)
  {
    /* default values for the cist dot1sInstance structure */
    /* cist - Bridge Identifier */
    instance->cist.cistid = instId;
    instance->cist.BridgeIdentifier.priInstId = FD_DOT1S_BRIDGE_PRIORITY + instIndex;

    /* cist - Bridge Times */
    instance->cist.CistBridgeTimes.msgAge = 0;
    instance->cist.CistBridgeTimes.bridgeMaxAge = FD_DOT1S_BRIDGE_MAX_AGE * DOT1S_TIMER_UNIT + instIndex;
    instance->cist.CistBridgeTimes.bridgeFwdDelay = FD_DOT1S_BRIDGE_FWD_DELAY * DOT1S_TIMER_UNIT + instIndex;
    instance->cist.CistBridgeTimes.bridgeHelloTime = FD_DOT1S_BRIDGE_HELLO_TIME * DOT1S_TIMER_UNIT + instIndex;
    instance->cist.CistBridgeTimes.maxHops = FD_DOT1S_BRIDGE_MAX_HOP + instIndex;

  }/*end if (instIndex == DOT1S_CIST_INDEX)*/
  else
  {
    /* msti - Bridge Identifier */
    instance->msti[instIndex].mstid = instId;
    instance->msti[instIndex].BridgeIdentifier.priInstId = (L7_ushort16)(FD_DOT1S_BRIDGE_PRIORITY | instId);

    /* msti - Bridge Times */
    instance->msti[instIndex].MstiBridgeTimes.bridgeMaxHops = FD_DOT1S_BRIDGE_MAX_HOP + instIndex;

  }/*end else if (instIndex == DOT1S_CIST_INDEX)*/

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Initialize default values for a spanning tree port.
*
* @param    portCfg - Pointer the the port configuration data.
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void dot1sPortTestConfigDataBuild (DOT1S_PORT_COMMON_CFG_t *portCfg, L7_uint32 seed)
{
  L7_uint32 inst;
  L7_uint32 mode;

    portCfg->adminEdge = 0;
    portCfg->portAdminMode = L7_ENABLE;
    portCfg->HelloTime = FD_DOT1S_BRIDGE_HELLO_TIME * DOT1S_TIMER_UNIT + seed;
    for (inst = 0; inst <= L7_MAX_MULTIPLE_STP_INSTANCES; inst++)
    {
      portCfg->portInstInfo[inst].ExternalPortPathCost = FD_DOT1S_PORT_PATH_COST + seed;
      portCfg->portInstInfo[inst].InternalPortPathCost = FD_DOT1S_PORT_PATH_COST + seed;

  /* Update the Auto Port Path Cost setting */
#if 0
      if (FD_DOT1S_PORT_PATH_COST == L7_DOT1S_AUTO_CALC_PATH_COST)
      {
        portCfg->portInstInfo[inst].autoInternalPortPathCost = L7_TRUE;
        portCfg->portInstInfo[inst].autoExternalPortPathCost = L7_TRUE;
      }
      else
      {
        portCfg->portInstInfo[inst].autoInternalPortPathCost = L7_FALSE;
        portCfg->portInstInfo[inst].autoExternalPortPathCost = L7_FALSE;
      }
#else

      mode = L7_FALSE;
      if (seed%2 == 0)
          mode = L7_TRUE;

      portCfg->portInstInfo[inst].autoInternalPortPathCost = mode;
      portCfg->portInstInfo[inst].autoExternalPortPathCost = mode;

#endif
      portCfg->portInstInfo[inst].portPriority = (DOT1S_PORTID_t)(FD_DOT1S_PORT_PRIORITY) + seed;
    }

}


/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void dot1sCfgShow(L7_uint32 printFlag)
{

    /*L7_uint32 itype, nextIntf;*/
    L7_uint32 i = 0;
    DOT1S_INSTANCE_INFO_CFG_t *cfgInstance;
    DOT1S_BRIDGE_CFG_t *cfgBridge;


    cfgBridge = &dot1sCfg->cfg.dot1sBridge;

    sysapiPrintf("\n");
    sysapiPrintf("dot1sBridge structure\n");
    sysapiPrintf("---------------------\n");
    sysapiPrintf("cfgBridge = &dot1sCfg->cfg.dot1sBridge\n");
    sysapiPrintf("\n");

    sysapiPrintf(" cfgBridge->Mode                       = %d\n",  cfgBridge->Mode);
    sysapiPrintf(" cfgBridge->ForceVersion               = %d\n",  cfgBridge->ForceVersion);
    sysapiPrintf(" cfgBridge->FwdDelay                   = %d\n",  cfgBridge->FwdDelay);
    sysapiPrintf(" cfgBridge->TxHoldCount                = %d\n",  cfgBridge->TxHoldCount);
    sysapiPrintf(" cfgBridge->MigrateTime                = %d\n",  cfgBridge->MigrateTime);
    sysapiPrintf(" cfgBridge->MaxHops                    = %d\n",  cfgBridge->MaxHops);
    sysapiPrintf(" cfgBridge->instanceCount              = %d\n",  cfgBridge->instanceCount);
    sysapiPrintf(" cfgBridge->MstConfigId.formatSelector = %d\n",  cfgBridge->MstConfigId.formatSelector );
    sysapiPrintf(" cfgBridge->MstConfigId.configName     = %s\n",  cfgBridge->MstConfigId.configName);
    sysapiPrintf(" cfgBridge->MstConfigId.revLevel       = %d\n", cfgBridge->MstConfigId.revLevel);


    sysapiPrintf("\n");

    if (printFlag == 1)
    {
      sysapiPrintf("\n");
      sysapiPrintf("dot1sInstVlanMap structure\n");
      sysapiPrintf("--------------------------\n");
      sysapiPrintf("\n");


      for (i = 0; i < L7_DOT1Q_MAX_VLAN_ID + 2; i++)
      {

          sysapiPrintf("Vlan ID %d  ==> instanceNumber %d\n", i,
                       dot1sCfg->cfg.dot1sInstVlanMap[i].instNumber);
      }
    }


    if (printFlag == 2)
    {
      sysapiPrintf("\n");
      sysapiPrintf("dot1sInstance structure\n");
      sysapiPrintf("-----------------------\n");
      cfgInstance = &dot1sCfg->cfg.dot1sInstance;

      /* default values for the cist dot1sInstance structure */
      dot1sCfgInstanceShow(cfgInstance, DOT1S_CIST_INDEX, DOT1S_CIST_ID);

      /* default values for the msti dot1sInstance structures */
      for (i = 1; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
      {
        dot1sCfgInstanceShow(cfgInstance, i, L7_NULL);
      }

    }

    if (printFlag == 3)
    {
      sysapiPrintf("\n");
      sysapiPrintf("dot1sInstanceMap structures\n");
      sysapiPrintf("---------------------------\n");
      sysapiPrintf("\n");


      for (i = 0; i <= L7_MAX_MULTIPLE_STP_INSTANCES; i++)
      {

          sysapiPrintf(" dot1sCfg->cfg.dot1sInstanceMap[i].instNumber = %d\n",
                       dot1sCfg->cfg.dot1sInstanceMap[i].instNumber);
          sysapiPrintf(" dot1sCfg->cfg.dot1sInstanceMap[i].inUse      = %d\n",
                       dot1sCfg->cfg.dot1sInstanceMap[i].inUse);
          sysapiPrintf("\n");

      }
    }

    if (printFlag == 4)
    {
      sysapiPrintf("\n");
      sysapiPrintf("dot1sCfgPort structures\n");
      sysapiPrintf("-----------------------\n");
      sysapiPrintf("\n");


      for (i = 1; i < L7_DOT1S_MAX_INTERFACE_COUNT; i++)
      {

        dot1sPortCfgShow (&dot1sCfg->cfg.dot1sCfgPort[i]);

      }
    }

 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   dot1sCfg->hdr.dataChanged = L7_TRUE;
   sysapiPrintf("Built test config data\n");


}


/*********************************************************************
* @purpose  Dump the config information for the instance
*
* @param    Instance  @b{(input)} pointer to a dot1s Instance structure
* @param    instIndex @b{(input)} instance Index
* @param    instId    @b{(input)} MSTP instance id
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sCfgInstanceShow(DOT1S_INSTANCE_INFO_CFG_t *instance,
                                        L7_uint32 instIndex,
                                        L7_uint32 instId)
{

  sysapiPrintf("\n");
  sysapiPrintf("dot1sInstance %d:\n", instIndex);
  sysapiPrintf("\n");

  if (instIndex == DOT1S_CIST_INDEX)
  {

    /* cist - Bridge Identifier */
    sysapiPrintf(" instance->cist.cistid                        = %d\n",instance->cist.cistid);
    sysapiPrintf(" instance->cist.BridgeIdentifier.priInstId    = %d\n",instance->cist.BridgeIdentifier.priInstId);

    /* cist - Bridge Times */


    sysapiPrintf(" instance->cist.CistBridgeTimes.msgAge          = %d\n", instance->cist.CistBridgeTimes.msgAge);
    sysapiPrintf(" instance->cist.CistBridgeTimes.bridgeMaxAge    = %d\n", instance->cist.CistBridgeTimes.bridgeMaxAge);
    sysapiPrintf(" instance->cist.CistBridgeTimes.bridgeFwdDelay  = %d\n", instance->cist.CistBridgeTimes.bridgeFwdDelay);
    sysapiPrintf(" instance->cist.CistBridgeTimes.bridgeHelloTime = %d\n", instance->cist.CistBridgeTimes.bridgeHelloTime);
    sysapiPrintf(" instance->cist.CistBridgeTimes.maxHops         = %d\n", instance->cist.CistBridgeTimes.maxHops);

  }/*end if (instIndex == DOT1S_CIST_INDEX)*/
  else
  {
    /* msti - Bridge Identifier */
    sysapiPrintf(" msti[instIndex].mstid                        = %d\n", instance->msti[instIndex].mstid);
    sysapiPrintf(" msti[instIndex].BridgeIdentifier.priInstId    = %d\n", instance->cist.BridgeIdentifier.priInstId);
    /* msti - Bridge Times */

    sysapiPrintf(" instance->msti[instIndex].MstiBridgeTimes.bridgeMaxHops = %d\n",
                 instance->msti[instIndex].MstiBridgeTimes.bridgeMaxHops);

  }/*end else if (instIndex == DOT1S_CIST_INDEX)*/

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Dump the config information for the port
*
* @param    portCfg - Pointer the the port configuration data.
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void dot1sPortCfgShow (DOT1S_PORT_COMMON_CFG_t *portCfg)
{
  L7_uint32 inst;
  L7_uint32 intIfNum;
  nimUSP_t usp;

  if (nimIntIfFromConfigIDGet(&portCfg->configId, &intIfNum) != L7_SUCCESS)
      return;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
      return;

  sysapiPrintf("\n");
  sysapiPrintf("\n");

  sysapiPrintf(" INTERFACE %d/%d/%d:\n", usp.unit, usp.slot, usp.port);
  sysapiPrintf(" portCfg->adminEdge     = %d\n", portCfg->adminEdge);
  sysapiPrintf(" portCfg->portAdminMode = %d\n", portCfg->portAdminMode);
  sysapiPrintf(" portCfg->HelloTime     = %d\n", portCfg->HelloTime);

  sysapiPrintf("\n");
  sysapiPrintf(" Instance Info:\n");

    for (inst = 0; inst <= L7_MAX_MULTIPLE_STP_INSTANCES; inst++)
    {

        sysapiPrintf(" Instance %d\n",inst);
        sysapiPrintf(" ExternalPortPathCost     = %d\n",portCfg->portInstInfo[inst].ExternalPortPathCost);

        sysapiPrintf(" InternalPortPathCost = %d\n",portCfg->portInstInfo[inst].InternalPortPathCost);
        sysapiPrintf(" autoInternalPortPathCost = %d\n",portCfg->portInstInfo[inst].autoInternalPortPathCost);
        sysapiPrintf(" autoExternalPortPathCost = %d\n", portCfg->portInstInfo[inst].autoExternalPortPathCost);
        sysapiPrintf(" portPriority             = %d\n", portCfg->portInstInfo[inst].portPriority);

        sysapiPrintf("\n");

    }

}



/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void dot1sConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;


    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(dot1sCfg->hdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

   sysapiPrintf( "\n");

   dot1sCfgShow(0);

   /*-------------------------------*/
   /* Scaling Constants             */
   /*-------------------------------*/


   sysapiPrintf( "\n");
   sysapiPrintf( "Scaling Constants\n");
   sysapiPrintf( "-----------------\n");


    sysapiPrintf( "L7_MAX_MULTIPLE_STP_INSTANCES - %d\n", L7_MAX_MULTIPLE_STP_INSTANCES);
    sysapiPrintf( "L7_MAX_VLAN_ID - %d\n", L7_MAX_VLAN_ID);
    sysapiPrintf( "L7_DOT1S_MAX_INTERFACE_COUNT - %d\n", L7_DOT1S_MAX_INTERFACE_COUNT);
    sysapiPrintf( "DOT1S_MAX_CONFIG_NAME_SIZE - %d\n", DOT1S_MAX_CONFIG_NAME_SIZE);



    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("dot1sCfg->checkSum : %u\n", dot1sCfg->checkSum);


}



/*********************************************************************
* @purpose  Saves dot1s configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments dot1sCfg is the overlay
*
* @end
*********************************************************************/
void dot1sSaveTestConfigData(void)
{

    dot1sCfg->hdr.dataChanged = L7_FALSE;
    dot1sCfg->checkSum = nvStoreCrc32((L7_uchar8 *)dot1sCfg, (L7_uint32)(sizeof(dot1sCfg_t) - sizeof(dot1sCfg->checkSum)));

    if (sysapiCfgFileWrite(L7_DOT1S_COMPONENT_ID, DOT1S_CFG_FILENAME, (L7_char8 *)dot1sCfg,
                           (L7_int32)sizeof(dot1sCfg_t)) == L7_ERROR)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
              "Error on call to sysapiCfgFileWrite routine on config file %s\n", DOT1S_CFG_FILENAME);
    }

}

/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/

/*********************************************************************
*
* @purpose Trace dot1s packets received
*
* @param   intIfNum @b{(input)} Internal Interface Number
* @param   *bpdu    @b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dot1sDebugPacketRxTrace(L7_uint32 intIfNum, DOT1S_MSTP_ENCAPS_t *bpdu)
{
  L7_uint32 unit, slot, port;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  /* Convert from intIfNum to USP */
  if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
  {
    DOT1S_USER_TRACE_RX(DOT1S_DEBUG_PACKET_RX_FORMAT,
                     unit, slot, port, intIfNum, ifName, bpdu->srcMac[0], bpdu->srcMac[1],bpdu->srcMac[2],
                     bpdu->srcMac[3],bpdu->srcMac[4],bpdu->srcMac[5], bpdu->protocolVersion,
                     bpdu->cistRootId.macAddr[0],bpdu->cistRootId.macAddr[1],bpdu->cistRootId.macAddr[2],
                     bpdu->cistRootId.macAddr[3],bpdu->cistRootId.macAddr[4],bpdu->cistRootId.macAddr[5],
                     bpdu->cistRootId.priInstId,bpdu->cistExtRootPathCost);
  }

  return;
}

/*********************************************************************
*
* @purpose Trace dot1s packets transmitted
*
* @param   intIfNum @b{(input)} Internal Interface Number
* @param   *bpdu    @b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dot1sDebugPacketTxTrace(L7_uint32 intIfNum, DOT1S_MSTP_ENCAPS_t *bpdu)
{
  L7_uint32 unit, slot, port;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  /* Convert from intIfNum to USP */
  if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
  {
    DOT1S_USER_TRACE_TX(DOT1S_DEBUG_PACKET_TX_FORMAT,
                     unit, slot, port, intIfNum, ifName, bpdu->srcMac[0], bpdu->srcMac[1],bpdu->srcMac[2],
                     bpdu->srcMac[3],bpdu->srcMac[4],bpdu->srcMac[5], bpdu->protocolVersion,
                     bpdu->cistRootId.macAddr[0],bpdu->cistRootId.macAddr[1],bpdu->cistRootId.macAddr[2],
                     bpdu->cistRootId.macAddr[3],bpdu->cistRootId.macAddr[4],bpdu->cistRootId.macAddr[5],
                     bpdu->cistRootId.priInstId,bpdu->cistExtRootPathCost);
  }
  return;
}
/*********************************************************************
* @purpose  Get the current status of displaying dot1s packet debug info
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
void dot1sDebugPacketTraceFlagGet(L7_BOOL *transmitFlag,L7_BOOL *receiveFlag)
{
  *transmitFlag = dot1sDebugPacketTraceTxFlag;
  *receiveFlag = dot1sDebugPacketTraceRxFlag;
  return;
}


/*********************************************************************
* @purpose  Turns on/off the displaying of dot1s packet debug info
*
* @param    flag         new value of the Packet Debug flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1sDebugPacketTraceFlagSet(L7_BOOL transmitFlag,L7_BOOL receiveFlag)
{
  dot1sDebugPacketTraceTxFlag = transmitFlag;
  dot1sDebugPacketTraceRxFlag = receiveFlag;

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a
     debug config save.
    */

  dot1sDebugCfg.hdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Save configuration settings for dot1s trace data
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1sDebugPacketTraceFlagSave()
{
  dot1sDebugCfg.cfg.dot1sDebugPacketTraceTxFlag = dot1sDebugPacketTraceTxFlag;
  dot1sDebugCfg.cfg.dot1sDebugPacketTraceRxFlag = dot1sDebugPacketTraceRxFlag;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Copy the trace settings to the debug config file
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dot1sDebugCfgUpdate(void)
{
  dot1sDebugPacketTraceFlagSave();

}

/*********************************************************************
* @purpose  Read and apply the debug config
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dot1sDebugCfgRead(void)
{
      /* reset the debug flags*/
    memset((void*)&dot1sDebugCfg, 0 ,sizeof(dot1sDebugCfg_t));

    (void)sysapiSupportCfgFileGet(L7_DOT1S_COMPONENT_ID, DOT1S_DEBUG_CFG_FILENAME,
                         (L7_char8 *)&dot1sDebugCfg, (L7_uint32)sizeof(dot1sDebugCfg_t),
                         &dot1sDebugCfg.checkSum, DOT1S_DEBUG_CFG_VER_CURRENT,
                         dot1sDebugBuildDefaultConfigData, L7_NULL);

    dot1sDebugCfg.hdr.dataChanged = L7_FALSE;
}


/*********************************************************************
* @purpose  Print the time bpdu message spent in the queue
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dot1sDebugPrintQueueTime()
{
  L7_uint32 i;

  for (i=0;i<10;i++)
  {
    printf("\r\n Time msg %d spent in dot1s queue : %u (msec)",i,dot1sQueueTime[i]);
  }

}

/*********************************************************************
* @purpose  Print the time bpdu message spent in the queue
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dot1sDebugPrintMsgProcTime()
{
  L7_uint32 i;

  for(i=0;i<10;i++)
  {
    printf("\r\n Time taken to process BPDU %d : %u (msec)",i,dot1sBPDUProcessTime[i]);
  }
}

void dot1sDebugEventsTraceStart(L7_uint32 numEntries)
{
  L7_RC_t rc;
  /* make sure the sizes make sense */
  if (numEntries == 0) numEntries = DOT1S_EVENTS_TRACE_ENTRIES_MAX;

  /* clean up if the handle is already allocated */
  if (dot1sEventTraceHdl != ~0) (void)dot1sDebugEventsTraceFini();

  /* acquire a unitMgr trace buffer */
  if (traceBlockCreate(numEntries, DOT1S_DEBUG_EVENTS_TRACE_WIDTH,
                       (L7_uchar8 *)"DOT1S EVENTS TRACE",
                       &dot1sEventTraceHdl) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "DOT1S: unable to create the trace buffer\n");
    dot1sEventTraceHdl = ~0;
  }
  else
  {
    /* set the debug flag such that state machine events would get traced*/
    rc = traceBlockStart(dot1sEventTraceHdl);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
              "DOT1S: Cannot start dot1s events trace \n");
    }

    dot1sDebugFlagsSet(DOT1S_DEBUG_STATE_MACHINE,DOT1S_CIST_INDEX);
  }

  return ;
}

/*********************************************************************
* @purpose  Clean up resources used by the trace utility
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t dot1sDebugEventsTraceFini(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if (dot1sEventTraceHdl != ~0)
  {
    /* disable at the mask level */

    /* disable at the trace util */
    traceBlockStop(dot1sEventTraceHdl);

    if (traceBlockDelete(dot1sEventTraceHdl) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
              "Dot1s: unable to delete the event trace buffer\n");
      rc = L7_FAILURE;
    }
    else
    {
      dot1sEventTraceHdl = ~0;
    }
  }
  return rc;
}

/* Stop Tracing and Dump trace */
L7_RC_t dot1sDebugEventsTraceShow(L7_BOOL writeToFlash, L7_BOOL unFormatted)
{
  traceData_t   *traceData;
  L7_uchar8     *pEntry, *pOffset;
  L7_uint32     count;
  L7_ulong32    entIndex;
  L7_uint32     i, j, jmax;
  L7_ushort16   traceId;
  L7_uint32 fd;

  traceData = traceBlockFind(dot1sEventTraceHdl);
  if (traceData == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\n\nCould not locate DAPI trace block for handle "
                  "0x%8.8x\n", dot1sEventTraceHdl);
    return L7_ERROR;
  }



  if (traceData->entryCount <= traceData->numOfEntries)
  {
    pEntry = traceData->head;
    count = (L7_uint32)traceData->entryCount;
    entIndex = 0;
  }
  else
  {  /*trace dump will always begin from the oldest entry to the latest entry*/
    pEntry = traceData->next;
    count = traceData->numOfEntries;
    entIndex = traceData->entryCount % traceData->numOfEntries;
  }

  /* a count of 0 means 'display all' entries */


  if (writeToFlash != L7_FALSE)
  {
     unFormatted = L7_FALSE; /* can only write formatted to flash*/
     if (osapiFsFileCreate(DOT1S_EVENTS_TRACE_NAME, &fd) != L7_SUCCESS)
       return L7_FAILURE;
  }
  else
  {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
  }




  for (i = 0; i < count; i++)
  {
    pOffset = pEntry;

    /* only display the last N entries, indicated by the count parm */
    if (unFormatted == L7_TRUE)
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "%4lu:  ", entIndex);

        for (j = 1, jmax = traceData->bytesPerEntry; j <= jmax; j++)
        {
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "%2.2x ", *pOffset++);
          if (((j % 8) == 0) && (j < jmax))
          {
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "- ");
          }
        } /* endfor j */

        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");

      } /* endif unformatted */
      else  /* manually format output */
      {
        L7_uchar8             *pIdStr;
        do1sTraceHeader_t        *pHdr;
        dot1sEventsTrace_t        *pTrace;
        char buf[DOT1S_DEBUG_EVENTS_DISPLAY_BUF];

        memset(buf, 0, sizeof(buf));

                /* the first x bytes are the same for all dapi trace entries */
        pTrace = (dot1sEventsTrace_t *)pOffset;
        pOffset += sizeof(*pTrace);
        pHdr = &pTrace->hdr;

        /* pick up the trace point identifier from the entry contents */
        traceId = osapiNtohs(pHdr->traceId);

        /* translate trace code to a display string */
        switch (traceId)
        {
          case DOT1S_EVENTS_TRACEID:
            pIdStr = (L7_uchar8 *)"DOT1S EVENTS";
            break;

          default:
            pIdStr = (L7_uchar8 *)"???????";
            break;
        }

        osapiSnprintfcat(buf, DOT1S_DEBUG_EVENTS_DISPLAY_BUF," %4lu:  ", entIndex);

        osapiSnprintfcat(buf, DOT1S_DEBUG_EVENTS_DISPLAY_BUF, (char *)"%-12s: ", pIdStr);

        /*SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      (char *)"%-12s: ", pIdStr);*/

        /* show the entry timestamp */
        osapiSnprintfcat(buf, DOT1S_DEBUG_EVENTS_DISPLAY_BUF,
                      (char *)"ts=%8.8u ",
                      (L7_uint32)osapiHtonl(pHdr->timeStamp));

        /* show the task */
        osapiSnprintfcat(buf, DOT1S_DEBUG_EVENTS_DISPLAY_BUF,
                      (char *)"tid=0x%.8x ",
                      (L7_uint32)osapiHtonl(pHdr->task));

        osapiSnprintfcat(buf, DOT1S_DEBUG_EVENTS_DISPLAY_BUF,
                      (char *)"Depth=%3u ",
                      (L7_uint32)osapiHtonl(pTrace->depth));

        osapiSnprintfcat(buf, DOT1S_DEBUG_EVENTS_DISPLAY_BUF,
                      (char *)"Event=%3d ",
                      (L7_uint32)osapiHtonl(pTrace->event));

        osapiSnprintfcat(buf, DOT1S_DEBUG_EVENTS_DISPLAY_BUF,
                      (char *)"Intf=%3u ",
                      (L7_uint32)osapiHtonl(pTrace->portNum));

        osapiSnprintfcat(buf, DOT1S_DEBUG_EVENTS_DISPLAY_BUF,
                      (char *)"Instance Index=%2u ",
                      (L7_uint32)osapiHtonl(pTrace->instIndex));
        osapiSnprintfcat(buf, DOT1S_DEBUG_EVENTS_DISPLAY_BUF, "\n");

        if (writeToFlash != L7_TRUE)
        {
           SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"%s",buf);
        }
        else
        {
           osapiFsWriteNoClose(fd,buf,strlen(buf));
        }


      } /* endelse manually format output */

    /* watch for trace wrap */
    entIndex++;
    pEntry += traceData->bytesPerEntry;
    if (pEntry >= traceData->tail)
    {
      pEntry = traceData->head;
      entIndex = 0;
    }

  } /* endfor i */

  if (writeToFlash != L7_TRUE)
  {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
  }
  else
  {
      osapiFsClose(fd);
  }

  return L7_SUCCESS;
}

L7_RC_t dot1sDebugEventsTraceStop()
{

   /* disable at the trace util */
   traceBlockStop(dot1sEventTraceHdl);

   return dot1sDebugEventsTraceShow(L7_TRUE, L7_FALSE);


}

/* write the event to the trace */
L7_RC_t dot1sDebugEventsTrace(DOT1S_PORT_COMMON_t *p, dot1sEvents_t dot1sEvent,
                               L7_uint32 instIndex, L7_uint32 depth)
{
    L7_ushort16 traceId = DOT1S_EVENTS_TRACEID;

    if (dot1sEventTraceHdl != ~0)
    {

      TRACE_BEGIN(dot1sEventTraceHdl, L7_DOT1S_COMPONENT_ID);
      TRACE_ENTER_2BYTE(osapiHtons(traceId));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiUpTimeMillisecondsGet()));
      TRACE_ENTER_4BYTE(osapiHtonl(osapiTaskIdSelf()));
     /* End Trace Header*/



      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)depth));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)dot1sEvent));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)p->portNum));
      TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)instIndex));

      TRACE_END();

      return L7_SUCCESS;
    }

    return L7_FAILURE;

}

void dot1sNsfTraceInit(L7_uint32 numEntries, L7_uint32 width)
{
  L7_RC_t rc;
  /* make sure the sizes make sense */
  if (numEntries == 0) numEntries = DOT1S_EVENTS_TRACE_ENTRIES_MAX;
  if (width == 0 ) width = DOT1S_TRACE_WIDTH_MAX;


  /* clean up if the handle is already allocated */
  if (dot1sNsfTraceHdl != ~0) (void)dot1sNsfTraceFini();

  dot1sNsfTraceWidth = width;
  /* acquire a unitMgr trace buffer */
  if (traceBlockCreate(numEntries, dot1sNsfTraceWidth,
                       (L7_uchar8 *)"DOT1S NSF TRACE",
                       &dot1sNsfTraceHdl) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
            "DOT1S: unable to create the trace buffer\n");
    dot1sNsfTraceHdl = ~0;
  }
  else
  {
    /* set the debug flag such that state machine events would get traced*/
    rc = traceBlockStart(dot1sNsfTraceHdl);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
              "DOT1S: Cannot start dot1s NSF events trace \n");
    }
  }

  return ;

}

void dot1sNsfTraceMaskSet(L7_uint32 mask)
{
  dot1sDebugNsf(mask);
}

void dot1sNsfTraceConsoleModeSet(L7_uint32 consoleFlag)
{
  traceConsoleMode = consoleFlag;
}

void dot1sNsfTraceHelp()
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Debug info is displayed %s\n",
                traceConsoleMode ?"on screen" : "in trace");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Current Bits Set are ..\n");
  dot1sDebugNsfShow();
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Bit positions for Mask are ..\n");
  dot1sDebugNsfHelp();
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Use dot1sDebugNsfTraceShow() to display output\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Use dot1sNsfTraceConsoleModeSet() to "
                "direct output to console/screen \n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Use dot1sDebugNsfTraceInit(numEntries, width) to change "
                "trace parameters\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Use dot1sNsfTraceMaskSet() to set trace mask\n");



}


/*********************************************************************
* @purpose  Create a generic trace
*
* @param    pString  {(input)} A string to be placed in the trace buffer
*
* @returns  void
*
* @notes    Truncated to the max size of an entry
* @end
*********************************************************************/
L7_RC_t dot1sNsfGenericFormat(L7_ushort16 traceId,L7_uchar8 *pDataStart)
{
  L7_uint32 len;

  len = strlen(pDataStart);

  if (len > (dot1sNsfTraceWidth - (sizeof(do1sTraceHeader_t) + 1)))
    pDataStart[dot1sNsfTraceWidth - 1] = L7_NULL;

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%s",pDataStart);

  return L7_SUCCESS;
}
void dot1sNsfTraceShow(L7_BOOL unFormatted)
{
  traceData_t   *traceData;
  L7_uchar8     *pEntry, *pOffset;
  L7_uint32     count;
  L7_ulong32    entIndex;
  L7_uint32     i, j, jmax;
  L7_ushort16   traceId;

  traceData = traceBlockFind(dot1sNsfTraceHdl);
  if (traceData == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\n\nCould not locate DOT1S trace block for handle "
                  "0x%8.8x\n", dot1sNsfTraceHdl);
    return ;
  }

  if (traceData->entryCount <= traceData->numOfEntries)
  {
    pEntry = traceData->head;
    count = (L7_uint32)traceData->entryCount;
    entIndex = 0;
  }
  else
  {  /*trace dump will always begin from the oldest entry to the latest entry*/
    pEntry = traceData->next;
    count = traceData->numOfEntries;
    entIndex = traceData->entryCount % traceData->numOfEntries;
  }

  /* a count of 0 means 'display all' entries */

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");


  for (i = 0; i < count; i++)
  {
    pOffset = pEntry;

    /* only display the last N entries, indicated by the count parm */
    if (unFormatted == L7_TRUE)
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "%4lu:  ", entIndex);

        for (j = 1, jmax = traceData->bytesPerEntry; j <= jmax; j++)
        {
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        "%2.2x ", *pOffset++);
          if (((j % 8) == 0) && (j < jmax))
          {
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "- ");
          }
        } /* endfor j */

        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");

      } /* endif unformatted */
      else
      {
        L7_uchar8             *pIdStr;
        do1sTraceHeader_t        *pHdr;
        L7_BOOL                 printRecord = L7_TRUE;

        /* the first 8 bytes are the same for all Unitmgr trace entries */
        pHdr = (do1sTraceHeader_t *)pOffset;
        pOffset += sizeof(*pHdr);

        /* pick up the trace point identifier from the entry contents */
        traceId = osapiNtohs(pHdr->traceId);

        /* translate trace code to a display string */
        switch (traceId)
        {

          case DOT1S_NSF_TRACEID:
            pIdStr = (L7_uchar8 *)"DOT1S NSF";
            break;

          default:
            pIdStr = (L7_uchar8 *)"??????";
            break;
        }

        if (printRecord)
        {

          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "%4lu:  ", entIndex);

          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        (char *)"%-15s: ", pIdStr);

          /* show the entry timestamp */
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        (char *)"ts=%8.8lu ",
                        (L7_uint32)(pHdr->timeStamp));

          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        (char *)"tid=0x%.8x ",
                        (L7_uint32)osapiHtonl(pHdr->task));

          /* format the rest of the entry per the id value */
          if (dot1sNsfGenericFormat(traceId, pOffset) != L7_SUCCESS)
          {
            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                          "   *** ERROR (cannot format this entry) ***");
          }
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
        }

      } /* endelse manually format output */

    /* watch for trace wrap */
    entIndex++;
    pEntry += traceData->bytesPerEntry;
    if (pEntry >= traceData->tail)
    {
      pEntry = traceData->head;
      entIndex = 0;
    }

  } /* endfor i */

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "\n");
}

void dot1sNsfTraceFini()
{
  L7_RC_t rc = L7_SUCCESS;

  if (dot1sNsfTraceHdl != ~0)
  {
    /* disable at the mask level */

    /* disable at the trace util */
    traceBlockStop(dot1sNsfTraceHdl);

    if (traceBlockDelete(dot1sNsfTraceHdl) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1S_COMPONENT_ID,
              "Dot1s: unable to delete the event trace buffer\n");
      rc = L7_FAILURE;
    }
    else
    {
      dot1sNsfTraceHdl = ~0;
    }
  }
  return ;

}

L7_RC_t dot1sNsfTrace(L7_uchar8 *pString)
{
  L7_uint32 traceId = DOT1S_NSF_TRACEID;
  L7_uint32 i,max;
  if (dot1sNsfTraceHdl != ~0)
  {

    TRACE_BEGIN(dot1sNsfTraceHdl, L7_DOT1S_COMPONENT_ID);
    TRACE_ENTER_2BYTE(osapiHtons(traceId));
    TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiUpTimeMillisecondsGet()));
    TRACE_ENTER_4BYTE(osapiHtonl(osapiTaskIdSelf()));
   /* End Trace Header*/

    max = strlen(pString);
    /* make sure strlen is no bigger than the max - header + null term */
    if (max > (dot1sNsfTraceWidth - (sizeof(do1sTraceHeader_t) + 1)))
      max = (dot1sNsfTraceWidth - (sizeof(do1sTraceHeader_t) + 1));

    for (i=0;i < max;i++)
    {
      TRACE_ENTER_BYTE(pString[i]);
    }

    TRACE_ENTER_BYTE(0);

    TRACE_END();

    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

