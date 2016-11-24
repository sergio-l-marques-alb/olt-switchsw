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

#define DOT3AD_DEBUG_PACKET_RX_FORMAT "Pkt RX - Intf: %d/%d/%d(%d), Port: %d, Sys: %s, Key: 0x%x"
#define DOT3AD_DEBUG_PACKET_TX_FORMAT "Pkt TX - Intf: %d/%d/%d(%d), Type: %s, Sys: %s, State: 0x%x, Key: 0x%x"

static L7_BOOL dot3adDebugPacketTraceFlag = L7_FALSE;

extern dot3adDebugCfg_t dot3adDebugCfg;
extern dot3adOperPort_t dot3adOperPort[L7_MAX_PORT_COUNT + 1];

void dot3adDebugHelp(void);

extern dot3ad_agg_t dot3adAgg[L7_MAX_NUM_LAG_INTF];
extern dot3ad_port_t dot3adPort[L7_MAX_PORT_COUNT + 1];
extern L7_uint32 dot3adAggIdx[L7_MAX_INTERFACE_COUNT];
extern L7_uint32 dot3adPortIdx[L7_MAX_PORT_COUNT+1]; 
extern dot3ad_system_t dot3adSystem;


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

/******************************************DEBUG COMMANDS***********/

void dot3adDebugLag(L7_uint32 lag_intf)
{
  dot3ad_agg_t *a;
  L7_uint32    i;

  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
				"The traceCode used in devshell traceDump is %d\n", dot3adSystem.traceId);

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
void dot3adDebugPacketRxTrace(L7_uint32 intIfNum, dot3ad_pdu_t *pdu)
{
  L7_uint32 unit, slot, port;
  L7_uchar8 actorSys[20];

  if(dot3adDebugPacketTraceFlag != L7_TRUE)
      return;  

  /* Convert from intIfNum to USP */
  if (usmDbUnitSlotPortGet(intIfNum, &unit, &slot, &port) == L7_SUCCESS)
  {
      osapiSnprintf(actorSys, sizeof(actorSys), "%02x.%02x.%02x.%02x.%02x.%02x",
                    pdu->actorSys.addr[0],pdu->actorSys.addr[1],pdu->actorSys.addr[2],
                    pdu->actorSys.addr[3],pdu->actorSys.addr[4],pdu->actorSys.addr[5]);
    DOT3AD_USER_TRACE(DOT3AD_DEBUG_PACKET_RX_FORMAT,
                      unit, slot, port, intIfNum, pdu->actorPort, actorSys, pdu->actorKey);
  }

  return;
}

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
    osapiSnprintf(actorSys, sizeof(actorSys), "%02x.%02x.%02x.%02x.%02x.%02x",
                    pdu->actorSys.addr[0],pdu->actorSys.addr[1],pdu->actorSys.addr[2],
                    pdu->actorSys.addr[3],pdu->actorSys.addr[4],pdu->actorSys.addr[5]);

    DOT3AD_USER_TRACE(DOT3AD_DEBUG_PACKET_TX_FORMAT, 
                      unit, slot, port, intIfNum, subType, actorSys, pdu->actorState, pdu->actorKey);
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
  dot3adDebugPacketTraceFlag = flag;

  /* Indicate outstanding configuration change to be copied.
     The config will be copied from the operational structure
     into the config structure when the user requests a 
     debug config save.
    */ 

  dot3adDebugCfg.hdr.dataChanged = L7_TRUE;

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


