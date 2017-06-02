/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename radius_debug.h
*
* @purpose RADIUS Client debug utilities
*
* @component radius
*
* @comments none
*
* @create 03/28/2003
*
* @author spetriccione
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_RADIUS_DEBUG_H
#define INCLUDE_RADIUS_DEBUG_H

#define RADIUS_ERROR_SEVERE(format,args...) \
{                                           \
  L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_RADIUS_COMPONENT_ID,format,##args);                   \
}

/*----------------------------------------*/
/*  Start Radius System Logging Options */
/*----------------------------------------*/

#define RADIUS_DLOG(rd_log_level, __fmt__, __args__...) \
 if (radiusDebugLevelCheck(rd_log_level) == L7_TRUE) \
 { \
   L7_LOGF (L7_LOG_SEVERITY_DEBUG, L7_RADIUS_COMPONENT_ID, __fmt__, ## __args__); \
 }

L7_BOOL radiusDebugLevelCheck(L7_uint32 level);

typedef enum
{
  RD_LEVEL_INFO = 0,     /* Info/Debug messages */
  RD_LEVEL_TX_PKT,       /* Pkt dump of radius messages sent to radius server */
  RD_LEVEL_RX_PKT,       /* Pkt dump of radius messages received from radius server */

 /* Radius cluster debug levels
  */
  RD_LEVEL_CLUSTER_INFO = 10,    /* Cluster Info/Debug messages */
  RD_LEVEL_CLUSTER_TX_PKT,       /* Pkt dump of TX radius cluster messages */
  RD_LEVEL_CLUSTER_RX_PKT,       /* Pkt dump of RX radius cluster messages */

 /* This must be the last element in the list.
  */
  RD_LEVEL_LAST
} radius_log_level_t;

/*
** Internal function prototypes
*/

L7_RC_t radiusDebugInit(void);

L7_RC_t radiusDebugHelp(void);
L7_RC_t radiusDebugUserAuth(L7_uint32 correlator,
                            L7_uint32 type,
                            L7_char8 *serverName,
                            L7_uint32 testCase,
                            L7_uint32 ipAddrType,
                            L7_uchar8 *ipAddr);


L7_RC_t radiusDebugCallback(L7_uint32 correlator,
                            L7_uint32 status,
                            L7_uchar8 *attributes,
                            L7_uint32 attributesLen);

/*
** Radius packet Dump debug functions
*/

/*********************************************************************
* @purpose  Hex dump of radius packet
*
* @param    level  @b((input))  If debug level enabled dump the packet
* @param    pkt    @b((input))  buffer containing the message
* @param    len    @b((input))  message length
*
* @returns  void
*
* @notes    This dumps packets only if radiusPktDump flag is enabled.
*
* @end
*********************************************************************/
void radiusDebugPktDump(L7_uint32 level, L7_uchar8 *pkt, L7_uint32 len);

/*********************************************************************
*
* @purpose  Display the list of All Accounting Servers configured or
*           next entry to the given server based on choice.
*
* @param serverAddr @b((input)) server address.
* @param Type @b((input))       server address type.
* @param choice @b((input))     Choice whether display all the entries
*                               or only next entry to the specified one.
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusDebugAcctServerNextIPHostNameGet(L7_uchar8 *serverAddr,L7_uchar8 Type, L7_uchar8 choice);

void radiusDebugAuthServerEntriesShow(void);
void radiusDebugAcctServerEntriesShow(void);

/*configure server with desired name.*/
void radiusDebugServerConfig(L7_uint32 type, L7_char8 *serverAddr, L7_char8 *serverName);

/*Configure desired Primary server*/
void radiusDebugServerTypeConfig(L7_char8 *serverAddr, L7_uint8 entryType);

/*Unconditional Walk through the nameArray[]*/
void radiusDebugServerNameEntriesShowAll(L7_uint32 type);

/* type indicates auth(1)/acct(2)*/
void radiusDebugServerNameSet(L7_uint8 type, L7_char8 *serverAddr, L7_char8 *serverName );

/*delete server with desired name.*/
/* 1 - Auth / 2- Acct */
void radiusDebugServerEntryRemove(L7_uint32 type, L7_char8 *serverAddr);

/* 1=Auth / 2=Acct */
/* Lists all the server names & corresponding current servers*/
void radiusDebugNamedServersWalk(L7_uint32 type);


/*********************************************************************
*
* @purpose Get the server name index held by the specified server.
*
* @param serverAddr @b((input)) IP address of the Auth Server
* @param L7_IP_ADDRESS_TYPE_t type @b{(input)} server address Type.
* @param L7_uint32 *index @b((output)) Pointer to the Index of the Auth Server
*
* @returns L7_SUCCESS - if there is a server corresponding to the
*                       specified index
* @returns L7_FAILURE - if no server for the index exists
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t radiusServerhostIndexGet(L7_uchar8 *serverAddr,
                     L7_IP_ADDRESS_TYPE_t type, L7_uint32 *index);

/* ONly for debug purpose*/
L7_RC_t radiusServerNameEntryDetailedGet(L7_uint32 ind,L7_char8 *serverName,L7_char8 *currentServer,L7_uint32 *count,void *currentEntry);
L7_RC_t radiusServerAcctNameEntryDetailedGet(L7_uint32 ind,L7_char8 *serverName,L7_uint32 *count);

/*********************************************************************
*
* @purpose To display constant values of parameters.
*
* @param L7_uint32 type @b((input))
*
* @returns none
*
* @comments
*
* @end
*
*********************************************************************/
void radiusDebugDumpConstants(L7_uint32 type);

void socketDbWalk(void);

/*********************************************************************
*
* @purpose  Debug function to show last function that took the
*           write lock.
*
* @param   none
*
* returns   none
*
* @comments
*
* @end
*
*********************************************************************/
int radiusDebugLock(void);

void radiusDebugNameOrderlyGetNext(L7_char8 *name, L7_uint32 type, L7_uint32 useAPI);

/*********************************************************************
* @purpose  This function sets specified debug flag.
*
* @param    level
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void radiusDebugLevelSet(L7_uint32 level);

/*********************************************************************
* @purpose  This function clears specified debug flag.
*
* @param    level
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void radiusDebugLevelClear(L7_uint32 level);

/*********************************************************************
* @purpose  This function clears all radius debug flags.
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t radiusDebugReset(void);

/*********************************************************************
*
* @purpose Display the IP address of the specified DNS server.
*
* @param host @b((input)) Pointer to the dns address string.
* @param radiusServerType_t type @b((input)) Auth/Acct.
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void radiusDebugServerHostIPAddrGet(L7_uchar8 *host, L7_uint32 type);

/*********************************************************************
* @purpose  This function configures the global parameters of radius.
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void radiusDebugGlobalConfig(void);

/*********************************************************************
* @purpose  This function displays the global parameters of radius.
*
* @param    none
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void radiusDebugGlobalShow(void);

#endif /* INCLUDE_RADIUS_DEBUG_H */

