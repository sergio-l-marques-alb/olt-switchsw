/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  dot3ad_debug.c
*
* @purpose   Dot3ad Debug functions
*
* @component Dot3ad
*
* @comments  none
*
* @create
*
* @author    w. jacobs
*
* @end
*
**********************************************************************/

#include <dot3ad_include.h>
#include "support_api.h"
#include "l7_common.h"
#include "usmdb_util_api.h"
#include "osapi.h"

#include "log.h"
/*#include "debug.h"*/

#include "dot3ad_debug.h"
#include "dot3ad_debug_api.h"
#include "sysapi.h"
#include "osapi_support.h"

#define DOT3AD_DEBUG_PACKET_RX_FORMAT "Pkt RX - Intf: %d/%d/%d(%d,%s), Port: %d, Sys: %s, Key: 0x%x"
#define DOT3AD_DEBUG_PACKET_TX_FORMAT "Pkt TX - Intf: %d/%d/%d(%d,%s), Type: %s, Sys: %s, State: 0x%x, Key: 0x%x"

static L7_BOOL dot3adDebugPacketTraceFlag = L7_FALSE;

L7_uint32 dot3adNsfDebugFlag = DOT3AD_DEBUG_NSF_NONE;
extern dot3adDebugCfg_t dot3adDebugCfg;
extern dot3adOperPort_t dot3adOperPort[L7_MAX_PORT_COUNT + 1];

void dot3adDebugHelp(void);

extern dot3ad_agg_t dot3adAgg[L7_MAX_NUM_LAG_INTF];
extern dot3ad_port_t dot3adPort[L7_MAX_PORT_COUNT + 1];
extern L7_uint32 dot3adAggIdx[L7_MAX_INTERFACE_COUNT];
extern L7_uint32 dot3adPortIdx[L7_MAX_PORT_COUNT+1];
extern dot3ad_system_t dot3adSystem;
extern dot3adOperPort_t dot3adOperPort[L7_MAX_PORT_COUNT + 1];

static L7_INTF_MASK_t traceIntfMask;
static L7_BOOL perIntfTrace= L7_FALSE, intfEventsTrace = L7_FALSE;


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
void dot3adDebugRegister(void)
{
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_DOT3AD_COMPONENT_ID;

    /* Register User Control Parms */
    supportDebugDescr.userControl.notifySave = dot3adDebugSave;
    supportDebugDescr.userControl.hasDataChanged = dot3adDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = dot3adDebugRestore;

    /* Register Internal Parms */
    supportDebugDescr.internal.debugHelp = dot3adDebugHelp;
    strncpy(supportDebugDescr.internal.debugHelpRoutineName,
            "dot3adDebugHelp",SUPPORT_DEBUG_HELP_NAME_SIZE);
    strncpy(supportDebugDescr.internal.debugHelpDescr,
            "Help Menu for LACP",SUPPORT_DEBUG_HELP_DESCR_SIZE);

    (void) supportDebugRegister(supportDebugDescr);
}


/*********************************************************************
*
* @purpose Display help menu for component dot3ad
*
* @param   void
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
void dot3adDebugHelp()
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "dot3adDebugLag(L7_uint32 lag_intf)\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "dot3adDebugNumOfLagsShow()\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "dot3adDebugStaticLagIntfStatShow(L7_uint32 intIfNum)\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "dot3adDebugTimeoutSet(L7_uint32 intIfNum, L7_uint32 timeout, L7_uint32 actor)\nSend 0 for all interfaces\n Send 0 for Partner and non zero for Actor\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "dot3adDebugIntfStatePrint(L7_uint32 intIfNum) Send 0 for all interfaces\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Use nimDebugPortTableShow to get mapping of usp to intIfNum\n");
}

void dot3adDebugLag(L7_uint32 lag_intf)
{
  dot3ad_agg_t *a;
  L7_uint32    i;

  a = dot3adAggIntfFind(lag_intf);

  if (a == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Incorrect lag id!\n");
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "LAG Id Range from %u to %u\n",
                  FD_CNFGR_NIM_MIN_LAG_INTF_NUM,
                  FD_CNFGR_NIM_MAX_LAG_INTF_NUM);
    return;
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Lag Internal Interface Number: %u \n",
                a->aggId);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "isStatic: %u\n", a->isStatic);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "\tWaitSelected Member List: ");
  for (i = 0; i< a->currNumWaitSelectedMembers; i++)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%u, ",
                  a->aggWaitSelectedPortList[i]);
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "\n\tSelected Member List: ");
  for (i = 0; i< a->currNumSelectedMembers; i++)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%u, ",
                  a->aggSelectedPortList[i]);
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "\n\tAttached Member List: ");
  for (i = 0; i< a->currNumMembers; i++)
  {
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%u, ",
                  a->aggPortList[i]);
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "\n\tActive Member List: ");
  for (i = 0; i< a->activeNumMembers; i++)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%u, ",
                  a->aggActivePortList[i]);
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "\n\tConfigured Member List: ");
  for (i = 0; i< a->currNumWaitSelectedMembers; i++)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "%u/%u/%u, ",
                  a->aggPortListUsp[i].unit,
                  a->aggPortListUsp[i].slot,
                  a->aggPortListUsp[i].port);
  }

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
          "\nAgg Mac Addr: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X \n",
         a->aggMacAddr.addr[0],a->aggMacAddr.addr[1],
         a->aggMacAddr.addr[2],a->aggMacAddr.addr[3],
         a->aggMacAddr.addr[4],a->aggMacAddr.addr[5]);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "individualAgg: %u\n", a->individualAgg);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "actorAdminAggKey: %u\n", a->actorAdminAggKey);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "actorOperAggKey: %u\n", a->actorOperAggKey);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
          "Partner Sys: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X \n",
         a->partnerSys.addr[0],a->partnerSys.addr[1],
         a->partnerSys.addr[2],a->partnerSys.addr[3],
         a->partnerSys.addr[4],a->partnerSys.addr[5]);


  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "partnerSysPri: %u\n", a->partnerSysPri);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "partnerOperAggKey: %u\n", a->partnerOperAggKey);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "collectorMaxDelay: %u\n", a->collectorMaxDelay);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "rxState: %u\n", a->rxState);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "txState: %u\n", a->txState);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "ready: %u\n", a->ready);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "inuse: %u\n", a->inuse);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "adminMode: %u\n", a->adminMode);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "stpMode: %u\n", a->stpMode);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "hashMode: %u\n", a->hashMode);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "name: %s\n", a->name);

  return;
}


void dot3adDebugLagMember(L7_uint32 intIfNum)
{
  dot3ad_port_t *pPort = L7_NULLPTR;

  pPort = dot3adPortIntfFind(intIfNum);
  if (pPort == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Port not a member of any lag");
  return;
}

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Actor port number: %u \n", pPort->actorPortNum);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Actor port priority: %u \n", pPort->actorPortPri);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Actor port SelectedAggId: %u \n", pPort->actorPortSelectedAggId);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Actor port WaitSelectedAggId: %u \n", pPort->actorPortWaitSelectedAggId);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Actor port ntt: %u \n", pPort->ntt);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Actor port AdminKey: %u \n", pPort->actorAdminPortKey);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Actor port AdminState: %u \n", pPort->actorAdminPortState);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Actor port OperKey: %u \n", pPort->actorOperPortKey);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Actor port OperState: %u \n", pPort->actorOperPortState);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n");

  /* MAC*/
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
          "Partner Admin Sys: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X \n",
         pPort->partnerAdminSys.addr[0],pPort->partnerAdminSys.addr[1],
         pPort->partnerAdminSys.addr[2],pPort->partnerAdminSys.addr[3],
         pPort->partnerAdminSys.addr[4],pPort->partnerAdminSys.addr[5]);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "partnerAdminPortState: %u \n", pPort->partnerAdminPortState);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "partnerAdminSysPri: %u \n", pPort->partnerAdminSysPri);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "partnerAdminKey: %u \n", pPort->partnerAdminKey);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "partnerAdminPortNumber: %u \n", pPort->partnerAdminPortNumber);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "partnerAdminPortPri: %u \n", pPort->partnerAdminPortPri);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
         "Partner Oper Sys: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X \n",
         pPort->partnerOperSys.addr[0],pPort->partnerOperSys.addr[1],
         pPort->partnerOperSys.addr[2],pPort->partnerOperSys.addr[3],
         pPort->partnerOperSys.addr[4],pPort->partnerOperSys.addr[5]);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "partnerOperPortState: %u \n", pPort->partnerOperPortState);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "partnerOperSysPri: %u \n", pPort->partnerOperSysPri);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "partnerOperKey: %u \n", pPort->partnerOperKey);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "partnerOperPortNumber: %u \n", pPort->partnerOperPortNumber);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "partnerOperPortPri: %u \n", pPort->partnerOperPortPri);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "portEnabled: %u \n", pPort->portEnabled);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "begin: %u \n", pPort->begin);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "lacpEnabled: %u \n", pPort->lacpEnabled);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "readyN: %u \n", pPort->readyN);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "selected: %u \n", pPort->selected);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "aggPortMoved: %u \n", pPort->aggPortMoved);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "portIndividualAgg: %u \n", pPort->portIndividualAgg);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "portAttached: %u \n", pPort->portAttached);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "txCount: %u \n", pPort->txCount);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n");

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Actor port rxState: %u \n", pPort->rxState);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Actor port perState: %u \n", pPort->perState);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Actor port muxState: %u \n", pPort->muxState);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Actor port Full duplex: %u \n",
                dot3adOperPort[intIfNum].fullDuplex);

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Actor port linkSpeed: %u \n",
                dot3adOperPort[intIfNum].linkSpeed);


}


void dot3adDebugTimeoutSet(L7_uint32 intIfNum, L7_uint32 timeOut, L7_uint32 actor)
{
  L7_uchar8 state = 0x07;
  L7_uint32 intf, nextIntf;
  L7_RC_t rc = L7_FAILURE;

  if (intIfNum == 0)
  {
    /* All interfaces */
    rc = nimFirstValidIntfNumberByType(L7_PHYSICAL_INTF, &intf);
    nextIntf = intf;
    do
    {
      intf = nextIntf;
      if (actor != 0)
      {
        dot3adAggPortActorAdminStateGet(intf, &state);
      }
      else
      {
        dot3adAggPortPartnerAdminStateGet(intf, &state);
      }

      if (timeOut == 0)
      {
        state = state & 0xfd;
      }
      else
      {
        state = state | 0x02;
      }
      if (actor != 0)
      {
        dot3adAggPortActorAdminStateSet(intf, state);
      }
      else
      {
        dot3adAggPortPartnerAdminStateSet(intf, state);
      }



    } while (nimNextValidIntfNumberByType(L7_PHYSICAL_INTF, intf,  &nextIntf) == L7_SUCCESS );
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "All Physical interfaces Set to timeout %d.'0' is long '1' is short\n", timeOut);
  }
  else
  {
    /* Just this port alone*/
    if (nimCheckIfNumber(intIfNum) == L7_SUCCESS)
    {
      if (actor != 0)
      {
        dot3adAggPortActorAdminStateGet(intIfNum, &state);
      }
      else
      {
        dot3adAggPortPartnerAdminStateGet(intIfNum, &state);
      }
      if (timeOut == 0)
      {
        state = state & 0xfd;
      }
      else
      {
        state = state | 0x02;
      }
      if (actor != 0)
      {
        dot3adAggPortActorAdminStateSet(intIfNum, state);
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Physical interface %d Set to Actor timeout %d.'0' is long '1' is short\n", intIfNum, timeOut);
      }
      else
      {
        dot3adAggPortPartnerAdminStateSet(intIfNum, state);
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "Physical interface %d Set to Partner timeout %d.'0' is long '1' is short\n", intIfNum, timeOut);
      }

    }
    else
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "Unknown Internal Interface number %d'0' is long '1' is short\n", intIfNum);
    }
  }
  return;

}
void dot3adDebugIntfStatePrint(L7_uint32 intIfNum)
{
  dot3ad_port_t *p;
  L7_RC_t rc;
  L7_uint32 intf, nextIntf;



  /*intIfNum 0 represents all interfaces*/
  if (intIfNum == 0)
  {
    /* All interfaces */
    rc = nimFirstValidIntfNumberByType(L7_PHYSICAL_INTF, &intf);
    nextIntf = intf;
    do
    {
      intf = nextIntf;
      p = dot3adPortIntfFind(intf);
      if (p != L7_NULLPTR)
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Port %d\nActor Admin State: 0x%2x, Partner Admin State: 0x%2x\n Actor Oper State : 0x%2x, Partner Oper  State: 0x%2x\n",
                      p->actorPortNum,p->actorAdminPortState, p->partnerAdminPortState,
                      p->actorOperPortState, p->partnerOperPortState);

      }
      else
      {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "Unknown port number %d\n", intf);
      }

    } while (nimNextValidIntfNumberByType(L7_PHYSICAL_INTF, intf,  &nextIntf) == L7_SUCCESS );
  }
  else
  {
    p = dot3adPortIntfFind(intIfNum);
    if (p != L7_NULLPTR)
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      "Port %d\nActor Admin State: 0x%2x, Partner Admin State: 0x%2x\nActor Oper  State: 0x%2x, Partner Oper  State: 0x%2x\n",
                      p->actorPortNum, p->actorAdminPortState, p->partnerAdminPortState,
                      p->actorOperPortState, p->partnerOperPortState);
    }
    else
    {
      SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                    "Unknown port number %d\n", intIfNum);
    }
  }

  return;
}

/*********************************************************************
*
* @purpose Trace dot3ad packets received
*
* @param   intIfNum @b{(input)} Internal Interface Number
* @param   *buff    @b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dot3adDebugPacketRxTrace(L7_uint32 intIfNum, dot3ad_pdu_t *pdu)
{
  L7_uint32 unit, slot, port;
  L7_uchar8 actorSys[20];

  if(dot3adDebugPacketTraceFlag != L7_TRUE)
      return;

  /* Convert from intIfNum to USP */
  if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      osapiSnprintf(actorSys, sizeof(actorSys), "%02x.%02x.%02x.%02x.%02x.%02x",
                    pdu->actorSys.addr[0],pdu->actorSys.addr[1],pdu->actorSys.addr[2],
                    pdu->actorSys.addr[3],pdu->actorSys.addr[4],pdu->actorSys.addr[5]);
    DOT3AD_USER_TRACE(DOT3AD_DEBUG_PACKET_RX_FORMAT,
                      unit, slot, port, intIfNum, ifName, pdu->actorPort, actorSys, pdu->actorKey);
  }

  return;
}

/*********************************************************************
*
* @purpose Trace dot3ad packets transmitted
*
* @param   intIfNum @b{(input)} Internal Interface Number
* @param   *buff    @b{(input)} pointer to packet
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void dot3adDebugPacketTxTrace(L7_uint32 intIfNum, dot3ad_pdu_t *pdu)
{
  L7_uint32 unit, slot, port;
  L7_uchar8 actorSys[20];
  L7_char8 subType[12];

  if(dot3adDebugPacketTraceFlag != L7_TRUE)
      return;

  if (pdu->subType == DOT3AD_LACP_SUBTYPE)
  {
    osapiSnprintf(subType, sizeof(subType), "LACP");
  }
  else if (pdu->subType == DOT3AD_MARKER_SUBTYPE)
  {
    if (pdu->tvlTypeActor == DOT3AD_MARKER_RESP_INFO)
    {
      osapiSnprintf(subType, sizeof(subType), "Marker Resp");
    }
    else if (pdu->tvlTypeActor == DOT3AD_MARKER_INFO)
    {
      osapiSnprintf(subType, sizeof(subType), "Marker");
    }
    else
    {
      osapiSnprintf(subType, sizeof(subType), "Unknown");
    }
  }

  /* Convert from intIfNum to USP */
  if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    osapiSnprintf(actorSys, sizeof(actorSys), "%02x.%02x.%02x.%02x.%02x.%02x",
                    pdu->actorSys.addr[0],pdu->actorSys.addr[1],pdu->actorSys.addr[2],
                    pdu->actorSys.addr[3],pdu->actorSys.addr[4],pdu->actorSys.addr[5]);

    DOT3AD_USER_TRACE(DOT3AD_DEBUG_PACKET_TX_FORMAT,
                      unit, slot, port, intIfNum, ifName, subType, actorSys, pdu->actorState, pdu->actorKey);
  }

  return;
}
/*********************************************************************
* @purpose  Get the current status of displaying lacp packet debug info
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
L7_BOOL dot3adDebugPacketTraceFlagGet()
{
  return dot3adDebugPacketTraceFlag;
}


/*********************************************************************
* @purpose  Turns on/off the displaying of lacp packet debug info
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
L7_RC_t dot3adDebugPacketTraceFlagSet(L7_BOOL flag)
{
    if (dot3adDebugPacketTraceFlag != flag)
    {
  dot3adDebugPacketTraceFlag = flag;
  dot3adDebugCfg.hdr.dataChanged = L7_TRUE;
    }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Save configuration settings for dot3ad trace data
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
L7_RC_t dot3adDebugPacketTraceFlagSave()
{
  dot3adDebugCfg.cfg.dot3adDebugPacketTraceFlag = dot3adDebugPacketTraceFlag;
  return L7_SUCCESS;
}

void dot3adNsfDebugFlagsSet(L7_uint32 flag)
{
   dot3adNsfDebugFlag = flag;
}

void dot3adNsfDebugPktDump(L7_uchar8 *pkt, L7_uint32 length)
{
  int row, column;
  L7_uint32 numRows = 4, numCols = 16;

  if (length != 0)
  {
      numRows = length/numCols;
  }

  printf("===================\n");
  for (row = 0; row <= numRows; row++)
  {
    printf("%04x ", row * numCols);
    for (column = 0; column < numCols; column++)
    {
      printf("%2.2x ",pkt[row*numCols + column]);
    }
    printf("\n");
  }
  printf("===================\n");
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
void dot3adDebugCfgUpdate(void)
{
  dot3adDebugPacketTraceFlagSave();

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
void dot3adDebugCfgRead(void)
{
      /* reset the debug flags*/
    memset((void*)&dot3adDebugCfg, 0 ,sizeof(dot3adDebugCfg_t));

    (void)sysapiSupportCfgFileGet(L7_DOT3AD_COMPONENT_ID, DOT3AD_DEBUG_CFG_FILENAME,
                         (L7_char8 *)&dot3adDebugCfg, (L7_uint32)sizeof(dot3adDebugCfg_t),
                         &dot3adDebugCfg.checkSum, DOT3AD_DEBUG_CFG_VER_CURRENT,
                         dot3adDebugBuildDefaultConfigData, L7_NULL);

    dot3adDebugCfg.hdr.dataChanged = L7_FALSE;
}


/****************************DOT3AD TRACE *******************************/

/*
| traceCode(0-7)| ActorAgg ID(8-15) | Actor Port (16-23) | Partner port (23-31)|
| ActorState(0-7)| partner state(8-15)| selected(16-23) | PRX state (23-31)    |
| Mux state(0-7)| Per state (8-15) | end val (16-31) 
                                   |

*/

void dot3adIntfEventTrace(L7_uint32 intIfNum, L7_uint32 event)
{
  if (dot3adSystem.traceHandle == ~0)
      return;

  if ((intfEventsTrace == L7_FALSE) ||
     (perIntfTrace == L7_TRUE &&
      (L7_INTF_ISMASKBITSET(traceIntfMask, intIfNum) == L7_FALSE)))
  {
    return;
  }

  TRACE_BEGIN(dot3adSystem.traceHandle, L7_DOT3AD_COMPONENT_ID);
  TRACE_ENTER_2BYTE(osapiHtons(DOT3AD_NIMEVENTS_TRACEID));
  TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiUpTimeMillisecondsGet()));
  TRACE_ENTER_4BYTE(osapiHtonl(osapiTaskIdSelf()));

  TRACE_ENTER_4BYTE(event);
  TRACE_ENTER_4BYTE(intIfNum);

  TRACE_END();

}

void dot3adAggTrace(L7_uint32 aggId, L7_uchar8 traceCode)
{
  if (dot3adSystem.traceHandle == ~0)
      return;

  if (perIntfTrace == L7_TRUE &&
      (L7_INTF_ISMASKBITSET(traceIntfMask, aggId) == L7_FALSE))
  {
    return;
  }
  TRACE_BEGIN(dot3adSystem.traceHandle, L7_DOT3AD_COMPONENT_ID);
  TRACE_ENTER_2BYTE(osapiHtons(DOT3AD_AGG_TRACEID));
  TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiUpTimeMillisecondsGet()));
  TRACE_ENTER_4BYTE(osapiHtonl(osapiTaskIdSelf()));

  TRACE_ENTER_4BYTE(traceCode);
  TRACE_ENTER_4BYTE(aggId);

  TRACE_END();
}
void dot3adPortTrace(dot3ad_port_t *p, L7_uchar8 traceCode)
{
  if (dot3adSystem.traceHandle == ~0)
      return;

  if ((perIntfTrace == L7_TRUE) &&
      (((p->actorPortAggId == 0) ||
       (L7_INTF_ISMASKBITSET(traceIntfMask,p->actorPortAggId) == L7_FALSE)) &&
       (L7_INTF_ISMASKBITSET(traceIntfMask,p->actorPortNum) == L7_FALSE)))
  {
    return;
  }

  TRACE_BEGIN(dot3adSystem.traceHandle, L7_DOT3AD_COMPONENT_ID);
  TRACE_ENTER_2BYTE(osapiHtons(DOT3AD_PORT_TRACEID));
  TRACE_ENTER_4BYTE(osapiHtonl((L7_ulong32)osapiUpTimeMillisecondsGet()));
  TRACE_ENTER_4BYTE(osapiHtonl(osapiTaskIdSelf()));


  TRACE_ENTER_2BYTE(traceCode);
  TRACE_ENTER_2BYTE(p->actorPortAggId);
  TRACE_ENTER_2BYTE(p->actorPortNum);
  TRACE_ENTER_2BYTE(p->partnerOperPortNumber);
  TRACE_ENTER_BYTE(p->actorOperPortState);
  TRACE_ENTER_BYTE(p->partnerOperPortState);
  TRACE_ENTER_BYTE(p->selected);
  TRACE_ENTER_BYTE(p->rxState);
  TRACE_ENTER_BYTE(p->muxState);
  TRACE_ENTER_BYTE(p->perState);

  TRACE_END();
}


char *dot3adDisplayTraceCode(L7_uchar8 traceCode)
{
  switch (traceCode)
  {
    case DOT3AD_TRACE_RX_INIT:          return "RxInit";
    case DOT3AD_TRACE_RX_DISABLE:       return "RxDisable";
    case DOT3AD_TRACE_RX_EXPIRED:       return "RxExpired";
    case DOT3AD_TRACE_RX_LACP_DISABLED: return "RxLacpDis";
    case DOT3AD_TRACE_RX_DEFAULTED:     return "RxDefault";
    case DOT3AD_TRACE_RX_CURRENT:       return "RxCurr";
    case DOT3AD_TRACE_RX_CURRWHILE_EXP: return "RxCWhileExp";
    case DOT3AD_TRACE_PER_NO_PERIODIC:  return "PerNo";
    case DOT3AD_TRACE_PER_FAST_PERIODIC:return "PerFast";
    case DOT3AD_TRACE_PER_SLOW_PERIODIC:return "PerSlow";
    case DOT3AD_TRACE_RX_PERWHILE_EXP:  return "RxPWhileExp";
    case DOT3AD_TRACE_MUX_DETACHED:     return "MuxDetach";
    case DOT3AD_TRACE_MUX_WAITING:      return "MuxWait";
    case DOT3AD_TRACE_MUX_ATTACHED:     return "MuxAttach";
    case DOT3AD_TRACE_MUX_COLLDIST:     return "MuxCollDist";
    case DOT3AD_TRACE_MUX_CD_EN_FAIL:   return "MuxEnFail";
    case DOT3AD_TRACE_MUX_CD_DIS_FAIL:  return "MuxDisFail";
    case DOT3AD_TRACE_RX_WAITWHILE_EXP: return "RxWwhileExp";
    case DOT3AD_TRACE_SEL_SELECT_BEGIN: return "SelBegin";
    case DOT3AD_TRACE_SEL_UNSELECT:     return "SelUnselect";
    case DOT3AD_TRACE_SEL_READY:        return "SEL_READY";
    case DOT3AD_TRACE_SEL_SELECT_END:   return "SelEnd";
    case DOT3AD_TRACE_TX_LACPDU:        return "TxPdu";
    case DOT3AD_TRACE_LAG_DOWN:         return "LagDown";
    case DOT3AD_TRACE_LAG_UP:           return "LagUp";

    default: return "Unknown ";
  }
}

L7_uchar8* dot3adDisplayState(L7_uchar8 state, L7_uchar8 *buf, L7_uint32 size)
{
 memset(buf,0,sizeof(buf));

 if (state & DOT3AD_STATE_EXPIRED)
 {
  osapiSnprintfcat(buf,size,"Exp,");
 }
 if (state & DOT3AD_STATE_DEFAULTED)
 {
  osapiSnprintfcat(buf,size,"Def,");
 }
 if (state & DOT3AD_STATE_DISTRIBUTING)
 {
  osapiSnprintfcat(buf,size,"Dist,");
 }
 if (state & DOT3AD_STATE_COLLECTING)
 {
  osapiSnprintfcat(buf,size,"Col,");
 }
 if (state & DOT3AD_STATE_SYNCHRONIZATION)
 {
  osapiSnprintfcat(buf,size,"Syn,");
 }
 if (state & DOT3AD_STATE_AGGREGATION)
 {
  osapiSnprintfcat(buf,size,"Agg,");
 }
 if (state & DOT3AD_STATE_LACP_TIMEOUT)
 {
  osapiSnprintfcat(buf,size,"ShrtTo,");
 }
 if (state & DOT3AD_STATE_LACP_ACTIVITY)
 {
  osapiSnprintfcat(buf,size,"Act ");
 }

 return buf;

}

char* dot3adDebugRxStateGet(L7_uint32 rxState)
{
  switch (rxState)
  {
    case INITIALIZE: return "Init";
    case PORT_DISABLED: return "PrtDisabled";
    case EXPIRED: return "Exp";
    case LACP_DISABLED: return "LacpDisabled";
    case DEFAULTED: return "Def";
    case CURRENT: return "Cur";
    default: return "Unknown";
  }
}

char* dot3adDebugMuxStateGet(L7_uint32 muxState)
{
  switch (muxState)
  {
    case DETACHED: return "Detached";
    case WAITING: return "Wait";
    case ATTACHED: return "Attached";
    case COLL_DIST: return "Coll_dist";
    default: return "Unknown";
  }
}

char* dot3adDebugPerStateGet(L7_uint32 perState)
{
  switch (perState)
  {
    case NO_PERIODIC: return "No";
    case FAST_PERIODIC: return "Fast";
    case SLOW_PERIODIC: return "Slow";
    case PERIODIC_TX: return "Tx";
    default: return "Unknown";
  }
}

static void dot3adPortTraceFmt(dot3adTrace_t *pTrace)
{
  L7_char8 buf[64];
  dot3adPortTrace_t *ptrPortTrace = &(pTrace->u.portTrace);
  
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"%-12s ",
                dot3adDisplayTraceCode(ptrPortTrace->traceCode));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"AggId(%3u) ",
                (L7_uint32)osapiHtonl(ptrPortTrace->aggId));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"Actor(%3u) ",
                (L7_uint32)osapiHtonl(ptrPortTrace->actorPortNum));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"State %s ", 
                dot3adDisplayState(ptrPortTrace->actorState,buf, sizeof(buf)));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"Partner(%3u) ",
                (L7_uint32)osapiHtonl(ptrPortTrace->partnerPortNum));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"State %s ",
                dot3adDisplayState(ptrPortTrace->partnerState,
                                   buf, sizeof(buf)));

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "%s ", (ptrPortTrace->selected == L7_TRUE) ? "Selected" : "");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "RX->%s ", dot3adDebugRxStateGet(ptrPortTrace->rxState));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "PER->%s ", dot3adDebugPerStateGet(ptrPortTrace->perState));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "MUX->%s ", dot3adDebugMuxStateGet(ptrPortTrace->muxState));

}

static void dot3adAggTraceFmt(dot3adTrace_t *pTrace)
{
  dot3adAggTrace_t *aggTrace = &(pTrace->u.aggTrace);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"%-12s ", dot3adDisplayTraceCode(aggTrace->traceCode));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"AggId=%3u ",
                (L7_uint32)osapiHtonl(aggTrace->aggId));
}

static void dot3adIntfTraceFmt(dot3adTrace_t *pTrace)
{
  dot3adIntfTrace_t *intfTrace = &(pTrace->u.intfTrace);
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"%-12s ", nimGetIntfEvent(intfTrace->intfEvent));
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                (char *)"Intf=%3u ",
                (L7_uint32)osapiHtonl(intfTrace->intIfNum));
}

typedef void fmtFunc(dot3adTrace_t *pTrace);

void dot3adTraceInit(L7_uint32 numEntries, L7_uint32 width)
{
  L7_RC_t rc;
  /* make sure the sizes make sense */
  if (numEntries == 0) numEntries = DOT3AD_TRACE_ENTRY_MAX;
  if (width == 0 ) width = DOT3AD_TRACE_ENTRY_SIZE_MAX;


  /* clean up if the handle is already allocated */
  if (dot3adSystem.traceHandle != ~0) (void)dot3adTraceFini();

  /* acquire a unitMgr trace buffer */
  if (traceBlockCreate(numEntries, width,
                       (L7_uchar8 *)"DOT3AD TRACE",
                       &dot3adSystem.traceHandle) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT3AD_COMPONENT_ID,
            "DOT3AD: unable to create the trace buffer\n");
    dot3adSystem.traceHandle = ~0;
  }
  else
  {
    /* set the debug flag such that state machine events would get traced*/
    rc = traceBlockStart(dot3adSystem.traceHandle);
    if (rc != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT3AD_COMPONENT_ID,
              "DOT3AD: Cannot start trace \n");
    }
  }

  return ;
}

void dot3adTraceFini()
{
  L7_RC_t rc = L7_SUCCESS;

  if (dot3adSystem.traceHandle != ~0)
  {
    /* disable at the mask level */

    /* disable at the trace util */
    traceBlockStop(dot3adSystem.traceHandle);

    if (traceBlockDelete(dot3adSystem.traceHandle) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT3AD_COMPONENT_ID,
              "Dot3ad: unable to delete the event trace buffer\n");
      rc = L7_FAILURE;
    }
    else
    {
      dot3adSystem.traceHandle = ~0;
    }
  }
  return ;

}

void dot3adTraceShow(L7_BOOL unFormatted)
{
  traceData_t   *traceData;
  L7_uchar8     *pEntry, *pOffset;
  L7_uint32     count;
  L7_ulong32    entIndex;
  L7_uint32     i, j, jmax;
  L7_ushort16   traceId;

  traceData = traceBlockFind(dot3adSystem.traceHandle);
  if (traceData == L7_NULLPTR)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                  "\n\nCould not locate DOT3AD trace block for handle "
                  "0x%8.8x\n", dot3adSystem.traceHandle);
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
        dot3adTraceHeader_t     *pHdr;
        dot3adTrace_t            *pTrace;
        L7_BOOL               printRecord = L7_TRUE;
        fmtFunc               *funcPtr;

        pTrace = (dot3adTrace_t *)pOffset;
        pOffset += sizeof(*pTrace);
        pHdr = &pTrace->hdr;

        /* pick up the trace point identifier from the entry contents */
        traceId = osapiNtohs(pHdr->traceId);

        /* translate trace code to a display string */
        switch (traceId)
        {

          case DOT3AD_PORT_TRACEID:
            pIdStr = (L7_uchar8 *)"DOT3AD PORT";
            funcPtr = dot3adPortTraceFmt;
            break;

          case DOT3AD_AGG_TRACEID:
            pIdStr = (L7_uchar8 *)"DOT3AD AGG ";
            funcPtr = dot3adAggTraceFmt;
            break;
          
          case DOT3AD_NIMEVENTS_TRACEID:
            pIdStr = (L7_uchar8 *)"NIM EVENTS";
            funcPtr = dot3adIntfTraceFmt;
            break;

          default:
            pIdStr = (L7_uchar8 *)"??????";
            funcPtr = dot3adAggTraceFmt;
            break;
        }

        if (printRecord)
        {

          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, "%4lu:  ", entIndex);

          /*SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        (char *)"%-12s: ", pIdStr);
           */
          /* show the entry timestamp */
          SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        (char *)"ts=%8.8lu ",
                        (L7_uint32)(pHdr->timeStamp));

          /*SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                        (char *)"tid=0x%.8x ",
                        (L7_uint32)osapiHtonl(pHdr->task));
        
          */
          funcPtr(pTrace);
          
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

void dot3adTraceIntfNumSet(L7_uint32 intIfNum)
{
  perIntfTrace = L7_TRUE;
  L7_INTF_SETMASKBIT(traceIntfMask,intIfNum);
}

void dot3adTraceIntIfNumClear()
{
  perIntfTrace = L7_FALSE;
  memset(&traceIntfMask, 0, sizeof(traceIntfMask));
}

void dot3adTraceIntfEvents(L7_BOOL intfTraceFlag)
{
  intfEventsTrace = intfTraceFlag;
}

void dot3adTraceHelp()
{

  
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "\nPer Intf trace mode %sset Intf event trace mode %sset",
                (perIntfTrace == L7_TRUE) ? "": "not ",
                (intfEventsTrace == L7_TRUE) ? "": "not ");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "\nvoid dot3adTraceIntfNumSet(L7_uint32 intIfNum)");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "\nvoid dot3adTraceIntIfNumClear()");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS, 
                "\nvoid dot3adTraceIntfEvents(L7_BOOL intfTraceFlag)");

}
