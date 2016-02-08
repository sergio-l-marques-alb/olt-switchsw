/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    dot1dgarpapi.h
* @purpose     GARP API definitions and functions
* @component   GARP
* @comments    none
* @create      02/23/2001
* @author      Hassan
* @author      
* @end
*             
**********************************************************************/
/****************************************************************************/
/*                                                                          */
/* MODULE: dot1dgarpapi.h                                                   */
/*                                                                          */
/* DESCRIPTION: XAPI GARP Interface Class Header File.  This header file    */
/*              specifies the interface used to send messages to the Garp   */
/*              and its applications.                                       */
/*                                                                          */
/*              1) Initialization                                           */
/*                   GARPSysInit                                            */
/*                                                                          */
/*              2) GARP Command Interface:                                  */
/*                 a) GID commands                                          */
/*                      GarpJoinCmd                                         */
/*                      GarpLeaveCmd                                        */
/*                      GarpDeleteAttributeCmd                              */
/*                                                                          */
/*                 b) GARP Applicant Adminstrative Control                  */
/*                      GarpNormalParticipantCmd                            */
/*                      GarpNonParticipantCmd                               */
/*                                                                          */
/*                 d) GARP Registrar Adminstrative Control                  */
/*                      GarpNormalRegistrationCmd                           */
/*                      GarpRegistrationFixed                               */
/*                      GarpRegistrationForbidden                           */
/*                                                                          */
/*                 e) GARP Control                                          */
/*                      GarpEnableCmd                                       */
/*                      GarpDisableCmd                                      */
/*                      GarpEnablePortCmd                                   */
/*                      GarpDisablePortCmd                                  */
/*                      GarpSetJoinTimeCmd                                  */
/*                      GarpSetLeaveTimeCmd                                 */
/*                      GarpSetLeaveAllTimeCmd                              */
/*                                                                          */
/*              3) GARP PDU Transfer                                        */
/*                     GarpRcbToBuffCopy       (Garp use Only)              */
/*                     GARPPktIsGmrpPDU                                     */
/*                     GARPPktIsGvrpPDU                                     */
/*                     GARPPktIsGPDU                                        */
/*                                                                          */
/*              4) GARP Spanning Tree Control                               */
/*                   GarpLinkStateChange                                    */
/*                   GarpLinkIsActive        (Garp use Only                 */
/*                   GARPSpanInstallHook         (Garp use Only             */
/*                   GARPSpanUnInstallHook       (Garp use Only)            */
/*                                                                          */
/*              5) Miscellaneous                                            */
/*                   GarpIsReady                                            */
/*                   GarpSuspendTask         (Garp use Only)                */
/*                                                                          */
/*                                                                          */
/* SPEC STANDARD: 802.1D-1998 And 802.1Q-1998                               */
/*                                                                          */
/*****************************************************************************/

#ifndef INCLUDE_DOT1D_GARP_API_H
#define INCLUDE_DOT1D_GARP_API_H

#include "sysapi.h"
#include "platform_counters.h"
#include "statsapi.h"
#include "sysnet_api.h"
#include "nimapi.h"
#include "garp_exports.h"

/******************************************************************************/
/*                           Public Declarations                              */
/******************************************************************************/

/* GVRP attribute Type */ 
#define GARP_GVRP_VLAN_ATTRIBUTE              0x01
#define GARP_GMRP_ATTRIBUTE					  0x02 /*this include both group addr and service req*/
/* GMRP attribute type */
#define GARP_GMRP_MULTICAST_ATTRIBUTE         0x01  /*The Group Attribute*/                                        
#define GARP_GMRP_LEGACY_ATTRIBUTE            0x02  /*The Service Requirement Attribute*/                                        
/*  Legacy attribute value */
#define GARP_GMRP_FORWARD_ALL                 0x00    
#define GARP_GMRP_FORWARD_UNREGISTERED        0x01

typedef struct 
{
  L7_uint32 rJE;  /* Join Empty Received */
  L7_uint32 rJIn; /* Join In Received */
  L7_uint32 rEmp; /* Empty Received */
  L7_uint32 rLIn; /* Leave In Received */
  L7_uint32 rLE;  /* Leave Empty Received */
  L7_uint32 rLA;  /* Leave All Received */
  L7_uint32 sJE;  /* Join Empty Sent */
  L7_uint32 sJIn; /* Leave Empty Sent */
  L7_uint32 sEmp; /* Empty Sent */
  L7_uint32 sLIn; /* Join In Sent */
  L7_uint32 sLE;  /* Leave In Sent */
  L7_uint32 sLA;  /* Leave All Sent */
}garpStats_t;

typedef struct 
{
  L7_uint32 invalidProtocolId;  
  L7_uint32 invalidAttrType; 
  L7_uint32 invalidAttrLen; 
  L7_uint32 invalidAttrEvent; 
  L7_uint32 invalidAttrValue;  
}garpErrorStats_t;

typedef struct
{
  L7_uint32 received[L7_MAX_INTERFACE_COUNT];   /* GVRP pkts received   */
  L7_uint32 send[L7_MAX_INTERFACE_COUNT];       /* GVRP pkts send       */
  L7_uint32 failedReg[L7_MAX_INTERFACE_COUNT];  /* GVRP failed registrations */
  garpStats_t garpStats[L7_MAX_INTERFACE_COUNT];
  garpErrorStats_t garpErrorStats[L7_MAX_INTERFACE_COUNT];
}gvrpInfo_t;

typedef struct
{
  L7_uint32 received[L7_MAX_INTERFACE_COUNT];   /* GMRP pkts received   */
  L7_uint32 send[L7_MAX_INTERFACE_COUNT];       /* GMRP pkts send       */
  L7_uint32 failedReg[L7_MAX_INTERFACE_COUNT];  /* GMRP failed registrations */
}gmrpInfo_t;

/* Command Type Structure */
typedef enum
{
  GARP_UNDEFINED_COMMAND = -1,

  /* Garp_INF_DECL (GID) */
  GARP_JOIN = 0,               /* Join a new attribute to the given port         */
  GARP_LEAVE,                  /* Leave an old attribute fron the given port     */
  GARP_DELETE_ATTRIBUTE,       /* Delete the attribute from the database.        */

  /* GARP APP_ADMIN_CTRL */
  GARP_NORMAL_PARTICIPANT,     /* Enable protocol for an attribute (default)     */
  GARP_NON_PARTICIPANT,        /* Disable protocol for an attribute              */

  /* GARP_REG_ADMIN_CTRL */
  GARP_NORMAL_REGISTRATION,    /* Normal registration for an attribute (default) */
  GARP_REGISTRATION_FIXED,     /* Fixed registration for an attribute            */
  GARP_REGISTRATION_FORBIDDEN, /* Forbid registration for an attribute           */

  /* GARP_CTRL Command */
  GARP_ENABLE,                 /* Enable Garp Operation (default)                */
  GARP_DISABLE,                /* Disable Garp Operation                         */
  GARP_ENABLE_PORT,            /* Enable Garp Operation on a given port (default)*/
  GARP_DISABLE_PORT,           /* Disable Garp Operation on a given port         */

  GVRP_VLAN_CREATION_FORBID,   /* Set vlan-creation-forbid for a port            */
  GVRP_VLAN_REGISTRATION_FORBID,/* Set VLAN registration-forbid for a port       */

  /* GARP_SET_TIMEs */
  GARP_JOIN_TIME,              /* Set Join time in centiseconds                  */
  GARP_LEAVE_TIME,             /* Set Leave time in centiseconds                 */
  GARP_LEAVEALL_TIME           /* Set LeaveAll time in centiseconds              */
} GARPCommandNameTable;

typedef enum
{
  GARP_SUCCESS,
  GARP_FAILURE,
  GARP_SEVERE_FAILURE
} GARPStatus;

typedef enum
{
  GARP_SPAN_DISABLE = 0,
  GARP_SPAN_ENABLE
} GARPSpanState;


/* GARP Application */
typedef enum
{
  GARP_GVRP_APP = 0,
  GARP_GMRP_APP,
  UNDEFINED_GARP_APP
} GARPApplication;



/* Miscellaneous definitions */
typedef L7_uchar8   GARPAttrType;
typedef L7_uchar8  *GARPAttrValue;

typedef L7_uint32   GARPTime;

typedef L7_uint32   GARPPort;

/* GARP Control Point (tasks) Parameter Definition */
#define GARP_CCP_TASK			    "gccp_t"
#define GARP_STACK_SIZE_DEFAULT	    L7_DEFAULT_STACK_SIZE
#define GARP_DEFAULT_TASK_SLICE	    L7_DEFAULT_TASK_SLICE
#define GARP_CCP_TASK_PRI		    L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)    
#define GARP_TIMER_TASK_PRI		    L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY)    

/*--------------------------------------------------------------------------*/
/* GARP Receive Data Control Block GRCB    ****PRIVATE DATA *****           */
/* This control block contains GPDU and other information related to the    */
/* GPDU. Garp uses this control block to process the GPDU for a given port  */
/*--------------------------------------------------------------------------*/
typedef struct _dot1d_Garp_Receive_Control_Block_ 
{
  L7_uint32      msgId;           /* message id                          */
  GARPPort       port;            /* port that received the GPDU packet  */
  L7_uint32      pktLen;          /* number of bytes in the GPDU packet  */
  L7_uchar8      *pkt;            /* packet containing the GPDU          */

} GARPRcvCB, *GARPRcvCBPtr;


/*
* Garp GVRP/GMRP Initialization 
*/

/* The following entry point is needed to create garp application. This is 
*  the only compile time variable, all others are done at run time.  This 
*  way garp is a plugable module.
*/
#define GARP_CREATE_ENTRY GarpCreate
#define GARP_SEND_PDU     GarpSendPDU

void GarpInitDone(void);
GARPStatus GarpWaitInitDone(L7_int32 timeout);

/* milliseconds to wait for CCP to init */
enum
{
  garp_init_timeout = L7_WAIT_FOREVER
};



/*********************************************************************
* @purpose  Send command To Garp.
*
* @param    GARPApplication          Application
* @param    GARPCommandNameTable     Command 
* @param    GARPPort                 port
* @param    GARPAttrType             Attribute Type
* @param    GARPAttrValue            Attribute Value
* @param    L7_uint32                shall be used for GMRP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t GarpIssueCmd(GARPApplication app, GARPCommandNameTable cmd, 
                     GARPPort port, GARPAttrType attrType,
                     GARPAttrValue pAttrValue, L7_uint32 gmrpVID);

/*********************************************************************
* @purpose  Check if the PKT is a GVRP Packet.
*
* @param    L7_uchar8[]    Mac address
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL GarpPktIsGvrpPDU(const L7_uchar8 daMacAddr[]);

/*********************************************************************
* @purpose  Check if the PKT is a GMRP Packet.
*
* @param    L7_uchar8[]    Mac address
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL GarpPktIsGmrpPDU(const L7_uchar8 daMacAddr[]);

/*********************************************************************
* @purpose  Check if the PKT is a GARP Packet.
*
* @param    L7_uchar8[]    Mac address
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL GarpPktIsGPDU(const L7_uchar8 daMacAddr[]);

/*********************************************************************
* @purpose  Check if the PKT is VLAN tagged
*
* @param    daMacAddr    Mac address
*
* @returns  offset 0 if not tagged
* @returns  offset 4 if tagged
*
* @notes
*       
* @end
*********************************************************************/
L7_uint32 GarpPktIsTagged(L7_uchar8 *pPkt);

/*********************************************************************
* @purpose  Tell Garp the link status has changed 
*
* @param    intIfNum        interface number
* @param    event           event
* @param    correlator      correlator for event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t garpIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);


L7_RC_t gvrpVlanChangeCallback(L7_uint32 vlanId, 
                               L7_uint32 intIfNum, L7_uint32 event);


/*********************************************************************
* @purpose  Check if the port is active.
*
* @param    GARPPort    port
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL GarpLinkIsActive(GARPPort port);

/*********************************************************************
* @purpose  install a link state change hook 
*
* @param    GARPStatus *  pointer to untion that takes the following param
*                        @param GARPSpanState   port state
*                        &param GARPPort        port
*
* @returns  GARP_SUCCESS
* @returns  GARP_FAILURE
* @returns  GARP_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
GARPStatus GarpSpanInstallHook(GARPStatus (*)(GARPSpanState,GARPPort));

/*********************************************************************
* @purpose  Un install a link state change hook 
*
* @param    none
*
* @returns  none
*
* @notes    
*       
* @end
*********************************************************************/
void GarpSpanUnInstallHook(void );

/*********************************************************************
* @purpose  check if garp is active 
*
* @param    None
*
* @returns  L7_FALSE
* @returns  L7_TRUE
*
* @notes    
*       
* @end
*********************************************************************/
L7_BOOL GarpIsReady(void);


/*********************************************************************
* @purpose  Send a message to the Garp_Task to process the incomming packet.
*
* @param    bufHandle         buffer     handle to the bpdu received
* @param    sysnet_pdu_info_t *pduInfo   pointer to pdu info structure
*
* @returns  L7_SUCCESS  if the message was put on the queue
* @returns  L7_FAILURE  if the message was not put on the queue
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t GarpRecvPkt(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);

/*********************************************************************
* @purpose  Allow stats manager to get a garp related statistic
*
* @param    c           Pointer to storage allocated by stats
                        manager to hold the results
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Only called by stats manager
*       
* @end
*********************************************************************/
void garpStatGet(pCounterValue_entry_t c);

/*********************************************************************
* @purpose  Gets the GVRP admin mode
*
* @param    void
*
* @returns  L7_TRUE if enabled , L7_FALSE if disabled
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL garpGetGvrpEnabled();

/*********************************************************************
* @purpose  Gets the GMRP admin mode
*
* @param    void
*
* @returns  L7_TRUE if enabled , L7_FALSE if disabled
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL garpGetGmrpEnabled();

/*********************************************************************
* @purpose  returns the MAC address of last GVRP PDU received on a port
*
* @param    intIfNum    internal interface number
* @param    *macAddr    pointer to a structure where mac address is stored
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t garpPortGvrpLastPduOriginGet(L7_uint32 BasePort, L7_uchar8 *macAddr);

/*********************************************************************
* @purpose  returns the MAC address of last GMRP PDU received on a port
*
* @param    intIfNum    internal interface number
* @param    *macAddr    pointer to a structure where mac address is stored
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes     
*
* @end
*********************************************************************/
L7_RC_t garpPortGmrpLastPduOriginGet(L7_uint32 BasePort, L7_uchar8 *macAddr);

/*********************************************************************
* @purpose  initialize multiple GIP data strucyure.
*
* @param    void
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes   initialize the multiple GIP instance data structure
*
*
* @end
*********************************************************************/
L7_RC_t gipInitMultipleGIP();

/*********************************************************************
* @purpose  initialize multiple GIP data strucyure.
*
* @param    mstId    spanning tree ID
* @param    mstId    spanning tree ID
* @param    mstId    spanning tree ID
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes   initialize the multiple GIP instance data structure
*
*
* @end
*********************************************************************/
L7_uint32 gipIntfChangecallBack(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 event);

/*********************************************************************
* @purpose  check if a port is participating in a certain instance.
*
* @param    instanceID    instance ID
* @param    portNumber    port number
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes   check if a port is forwarding in a MSTP instance
*
*
* @end
*********************************************************************/
L7_RC_t gipIsPortBelongToInstance(L7_uint32 instanceID, L7_uint32 portNumber);

/*********************************************************************
* @purpose  Print the current GARP config values to 
*           serial port
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t GarpConfigDump(void);

/*********************************************************************
* @purpose  Get the value of vlan-creation-forbid flag for an interface
*
* @param    interface   (input) interface number
* @param    forbid      (output) vlan-creation-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t gvrpVlanCreationForbidGet(L7_uint32 interface, L7_BOOL *forbid);

/*********************************************************************
* @purpose  Set the value of vlan-creation-forbid flag for an interface
*
* @param    interface   (input) interface number
* @param    forbid      (input) vlan-creation-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t gvrpPortVlanCreationForbidSet(L7_uint32 interface, L7_BOOL forbid);

/*********************************************************************
* @purpose  Get the value of registration-forbid flag for an interface
*
* @param    interface   (input) interface number
* @param    forbid      (output) VLAN registration-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t gvrpPortRegistrationForbidGet(L7_uint32 interface, L7_BOOL *forbid);

/*********************************************************************
* @purpose  Set the value of registration-forbid flag for an interface
*
* @param    interface   (input) interface number
* @param    forbid      (input) VLAN registration-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t gvrpPortRegistrationForbidSet(L7_uint32 interface, L7_BOOL forbid);

/*********************************************************************
* @purpose  Get the GARP protocol related statistic
*
* @param    interface   interface number
* @param    counterType the type of statistic counter 
* @param    val         the statistic 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Stat Mgr is not used for these counters as that is unnecessarity
*           complex way.
*       
* @end
*********************************************************************/
L7_RC_t garpStatisticGet(L7_uint32 intIfNum, GARP_STAT_COUNTER_TYPE_t counterType, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the GARP protocol related error statistic
*
* @param    interface   interface number
* @param    counterType the type of error statistic counter 
* @param    val         the statistic 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Stat Mgr is not used for these counters as that is unnecessarity
*           complex way.
*       
* @end
*********************************************************************/
L7_RC_t garpErrorStatisticGet(L7_uint32 intIfNum, GARP_ERROR_STAT_COUNTER_TYPE_t counterType, L7_uint32 *val);

/*********************************************************************
* @purpose  Clear the GARP protocol related statistics
*
* @param    interface   interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Stat Mgr is not used for these counters as that is unnecessarity
*           complex way.
*       
* @end
*********************************************************************/
L7_RC_t garpStatisticsClear(L7_uint32 interface);

/*********************************************************************
* @purpose  Clear the GARP protocol related error statistics
*
* @param    interface   interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Stat Mgr is not used for these counters as that is unnecessarity
*           complex way.
*       
* @end
*********************************************************************/
L7_RC_t garpErrorStatisticsClear(L7_uint32 interface);

#endif
