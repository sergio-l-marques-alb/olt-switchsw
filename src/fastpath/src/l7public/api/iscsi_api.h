/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename   iscsi_api.h
*
* @purpose    Contains definitions for iSCSI API's
*
* @component  ISCSI
*
* @comments   This file contains all the defines, function prototypes
*             and include files required by iscsi_api.c
*
* @create     04/18/2008
* @end
*
**********************************************************************/

#ifndef ISCSI_API_H
#define ISCSI_API_H

#include "iscsi.h"
#include "cos_exports.h"
#include "osapi.h"

/*********************************************************************
* @purpose  Configures the global administrative mode iSCSI component
*
* @param  L7_uint32 adminMode      (input) Global enable mode for iSCSI
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiAdminModeSet(L7_uint32 adminMode);

/*********************************************************************
* @purpose  Retrieves the global administrative mode iSCSI component
*
* @param  L7_uint32 *adminMode      (input) Global enable mode for iSCSI
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiAdminModeGet(L7_uint32 *adminMode);

/*********************************************************************
* @purpose  Configures the VLAN priority for treatment of iSCSI packets
*
* @param  L7_uint32 priority      (input)  VLAN tag priority 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiVlanPrioritySet(L7_uint32 priority);

/*********************************************************************
* @purpose  Retrieve the VLAN priority value used for iSCSI session
*           data packets
*
* @param  L7_uint32 *priority      (output)  VLAN tag priority 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiVlanPriorityGet(L7_uint32 *priority);

/*********************************************************************
* @purpose  Configures the DSCP value for treatment of iSCSI packets
*
* @param  L7_uint32 dscp      (input)  DSCP field data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiDscpSet(L7_uint32 dscp);

/*********************************************************************
* @purpose  Retrieve the DSCP value used for iSCSI session
*           data packets
*
* @param  L7_uint32 *dscp      (output)  DSCP value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiDscpGet(L7_uint32 *dscp);

/*********************************************************************
* @purpose  Configures the IP precedence value for treatment of iSCSI packets
*
* @param  L7_uint32 prec      (input)  IP precedence data
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiPrecSet(L7_uint32 prec);

/*********************************************************************
* @purpose  Retrieve the PREC value used for iSCSI session
*           data packets
*
* @param  L7_uint32 *prec      (output)  PREC value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiPrecGet(L7_uint32 *prec);

/*********************************************************************
* @purpose  Configures whether iSCSI session packets will be marked
*
* @param  L7_uint32 adminMode      (input) tagging enable mode for iSCSI
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiMarkingModeSet(L7_uint32 adminMode);

/*********************************************************************
* @purpose  Retrieves the marking mode iSCSI component
*
* @param  L7_uint32 *adminMode      (input) Global enable mode for iSCSI
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiMarkingModeGet(L7_uint32 *adminMode);

/*********************************************************************
* @purpose  Configures which type of packet priority marking to use
*           for iSCSI session traffic
*
* @param  L7_QOS_COS_MAP_INTF_MODE_t selector      (input)  what type
*                    of marking to apply to session traffic
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiTagFieldSet(L7_QOS_COS_MAP_INTF_MODE_t selector);

/*********************************************************************
* @purpose  Retrieve the PREC value used for iSCSI session
*           data packets
*
* @param  L7_QOS_COS_MAP_INTF_MODE_t *selector  (output)  what type
*                    of marking to apply to session traffic
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiTagFieldGet(L7_QOS_COS_MAP_INTF_MODE_t *selector);

/*********************************************************************
* @purpose  Configures the session time out value used for iSCSI sessions
*
* @param  L7_uint32 seconds   (input) number of seconds to wait before
*                             deleting a dormant iSCSI session  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiTimeOutIntervalSet(L7_uint32 seconds);

/*********************************************************************
* @purpose  Retrieve the session time out value used for iSCSI sessions
*
* @param  L7_uint32 *seconds   (output) number of seconds to wait before
*                               deleting a dormant iSCSI session  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t iscsiTimeOutIntervalGet(L7_uint32 *seconds);

/*********************************************************************
* @purpose  Configures the TCP ports and (optionally) IP addresses for
*           traffic to monitor for iSCSI session setup packets.  The
*           target name can be configured for user convenience.
*
* @param  L7_uint32 tcpPort    (input) TCP port to be monitored  
* @param  L7_uint32 ipAddress  (input) IP address to be monitored on this port
* @param  L7_uint32 targetName (input) target's name 
*
* @returns  L7_SUCCESS 
*           L7_FAILURE
*           L7_ALREADY_CONFIGURED
*
* @notes    TCP ports can be specified without an IP address.  If so,
*           all TCP frames will sent to this TCP port will be examined for 
*           iSCSI protocol frames.  If the ipAddress is specified, only 
*           packets sent to the TCP port and this IP address will be 
*           examined.
*
*           The targetName is optional and only used for show commands as
*           a convenience to the user.  It is not used in the filtering of
*           which frames to examine.
*
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTcpPortAdd (L7_uint32 tcpPort, L7_inet_addr_t *ipAddress, L7_uchar8 *targetName);

/*********************************************************************
* @purpose  Removes the TCP ports and (optionally) IP addresses for
*           traffic to monitor for iSCSI session setup packets.  The
*           target name can be configured for user convenience.
*
* @param  L7_uint32 tcpPort    (input) TCP port to be monitored  
* @param  L7_inet_addr_t *ipAddress  (input) IP address to be monitored on this port
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*           L7_ERROR
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTcpPortDelete(L7_uint32 tcpPort, L7_inet_addr_t *ipAddress);

/*********************************************************************
* @purpose  Finds the entry in the Target TCP port table the matches
*           the specified port number and IP address.
*
* @param  L7_uint32 port  (input) TCP port number 
* @param  L7_uint32 ip_addr  (input) IP address, this parameter is
*                              ignored if set to 0
* @param  L7_uint32 *entryId  (output) the entry identifier, if found
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableLookUp(L7_uint32 port, L7_uint32 ip_addr, L7_uint32 *entryId);

/*********************************************************************
* @purpose  Retrieves the first entry in the Target TCP port table.
*           Entries are returned in a sorted order by TCP port number,
*           target IP address, and target name.
*
* @param  L7_uint32 *entryId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableFirstGet(L7_uint32 *entryId, L7_BOOL includeDeletedDfltCfgEntries);

/*********************************************************************
* @purpose  Retrieves the next entry in the Target TCP port table.
*           Entries are returned in a sorted order by TCP port number,
*           target IP address, and target name.
*
* @param  L7_uint32 entryId  (input) an identifier of the current table entry
* @param  L7_uint32 *nextEntryId  (output) an identifier of the retrieved
*                                table entry.  
*
* @returns  L7_SUCCESS  table not empty and next entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableNextGet(L7_uint32 entryId, L7_uint32 *nextEntryId, L7_BOOL includeDeletedDfltCfgEntries);

/*********************************************************************
* @purpose  Returns the entry in the Target TCP port table that contains
*           the specified TCP port number and target IP address if it 
*           exists.
*
* @param  L7_uint32 tcpPortNumber    (input) TCP port to be monitored  
* @param  L7_inet_addr_t ipAddress  (input) IP address to be monitored on this port
* @param  L7_uint32 *entryId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer 
* @returns  L7_FAILURE  not found
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableEntryGet(L7_uint32 tcpPortNumber, L7_inet_addr_t *ipAddress, L7_uint32 *entryId);

/*********************************************************************
* @purpose  Returns the next entry in the Target TCP port table after
*           the specified TCP port number and target IP address if it 
*           exists.
*
* @param  L7_uint32 tcpPortNumber    (input) TCP port to be monitored  
* @param  L7_inet_addr_t ipAddress  (input) IP address to be monitored on this port
* @param  L7_uint32 *nextEntryId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer 
* @returns  L7_FAILURE  not found
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableEntryNextGet(L7_uint32 tcpPortNumber, L7_inet_addr_t *ipAddress, L7_uint32 *nextEntryId);

/*********************************************************************
* @purpose  Returns whether the Target Table entry data matches one
*           of the Factory Default table entries.
*
* @param  L7_uint32 entryId  (input) an identifier of the current table entry
*
* @returns  L7_TRUE    table entry data matches an entry in the default config table 
* @returns  L7_FALSE   none match
*
* @notes    None
*
* @end
*********************************************************************/
L7_BOOL iscsiTargetTableIsEntryInDefaultConfig(L7_uint32 entryId);

/*********************************************************************
* @purpose  Returns the value of the deletedDefaultCfgEntry flag for the
*           Target Table entry.
*
* @param  L7_uint32 entryId  (input) an identifier of the current table entry
*
* @returns  L7_TRUE    table entry in the default config table 
* @returns  L7_FALSE   no match
*
* @notes    None
*
* @end
*********************************************************************/
L7_BOOL iscsiTargetTableEntryDeletedDefaultConfigFlagGet(L7_uint32 entryId);

/*********************************************************************
* @purpose  Retrieves the TCP port number for the specified
*           entry in the Target TCP port table.
*
* @param  L7_uint32 entryId  (input) an identifier of the current table entry
* @param  L7_uint32 *tcpPortNumber  (output) TCP port number value of the table entry.  
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableEntryTcpPortGet(L7_uint32 entryId, L7_uint32 *tcpPortNumber);

/*********************************************************************
* @purpose  Reports whether the  Target TCP port table entry corresponding with
*           entryId is in use.
*
* @param  L7_uint32 entryId  (input) an identifier of the current table entry
*
* @returns  L7_TRUE     table entry is in use 
* @returns  L7_FALSE    table entry empty or entryId out of range
*
* @notes    None
*
* @end
*********************************************************************/
L7_BOOL iscsiTargetTableEntryInUseStatusGet(L7_uint32 entryId);

/*********************************************************************
* @purpose  Retrieves the target IP address for the specified
*           entry in the Target TCP port table.
*
* @param  L7_uint32 entryId  (input) an identifier of the current table entry
* @param  L7_uint32 *tcpPortNumber  (output) TCP port number value of the table entry.  
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableEntryIpAddressGet(L7_uint32 entryId, L7_inet_addr_t *ipAddress);

/*********************************************************************
* @purpose  Retrieves the target name for the specified
*           entry in the Target TCP port table.
*
* @param  L7_uint32 entryId  (input) an identifier of the current table entry
* @param  L7_uint32 *tcpPortNumber  (output) TCP port number value of the table entry.  
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiTargetTableEntryTargetNameGet(L7_uint32 entryId, L7_uchar8 *name);

/*********************************************************************
* @purpose  Checks whether an entry associated with the given session
*           identifier is in use.
*
* @param  L7_uint32 sessionId    (input) an identifier of a
*                                table entry.  
*
* @returns  L7_SUCCESS  session ID is in use 
* @returns  L7_FAILURE  session ID is not in use
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionTableIdIsValid(L7_uint32 sessionId);

/*********************************************************************
* @purpose  Retrieves the first entry in the iSCSI session table.
*           Entries are returned in a sorted order by target IQN, 
*           iniator IQN, and session ISID.
*
* @param  L7_uint32 *sessionId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionFirstGet(L7_uint32 *sessionId);

/*********************************************************************
* @purpose  Retrieves the next entry in the iSCSI session table.
*           Entries are returned in a sorted order by target IQN, 
*           iniator IQN, and session ISID.
*
* @param  L7_uint32 sessionId  (input) an identifier of the current table entry
* @param  L7_uint32 *nextSessionId  (output) an identifier of the retrieved
*                                table entry.  
*
* @returns  L7_SUCCESS  table not empty and next entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionNextGet(L7_uint32 sessionId, L7_uint32 *nextSessionId);

/*********************************************************************
* @purpose  Retrieves the next entry in the iSCSI session table.
*           Entries are returned in order of increasing sessionId.
*
* @param  L7_uint32 sessionId  (input) an identifier of the current table entry
* @param  L7_uint32 *nextSessionId  (output) an identifier of the retrieved
*                                table entry.  
*
* @returns  L7_SUCCESS  table not empty and next entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionNextByEntryIdGet(L7_uint32 sessionId, L7_uint32 *nextSessionId);

/*********************************************************************
* @purpose  Retrieves the target IQN for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uchar8 *name     (output) the IQN 
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionTargetNameGet(L7_uint32 sessionId, L7_uchar8 *name);

/*********************************************************************
* @purpose  Retrieves the initiator IQN for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uchar8 *name     (output) the IQN 
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionInitiatorNameGet(L7_uint32 sessionId, L7_uchar8 *name);

/*********************************************************************
* @purpose  Retrieves the start time for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_clocktime *time   (output) the system time when the session 
*                                was detected
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionStartTimeGet(L7_uint32 sessionId, L7_clocktime *time);

/*********************************************************************
* @purpose  Retrieves the time since last data traffic for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uint32 *seconds   (output) seconds since last seen data for session 
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionSilentTimeGet(L7_uint32 sessionId, L7_uint32 *seconds);

/*********************************************************************
* @purpose  Retrieves the ISID for the specified
*           entry in the iSCSI session table.
*
* @param  L7_uint32 sessionId  (input) an identifier of the table entry
* @param  L7_uchar8 *isid      (output) the ISID associated with the specified
*                                 session
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiSessionIsidGet(L7_uint32 sessionId, L7_uchar8 *isid);

/*********************************************************************
* @purpose  Checks whether an entry associated with the given connection
*           identifier is in use.
*
* @param  L7_uint32 connectionId    (input) an identifier of a
*                                table entry.  
*
* @returns  L7_SUCCESS  connection ID is in use 
* @returns  L7_FAILURE  connection ID is not in use
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionTableIdIsValid(L7_uint32 connectionId);

/*********************************************************************
* @purpose  Retrieves the first entry in the iSCSI connection table 
*           associated with the specified session.
*           Entries are returned in a sorted order by initiator IP
*           address, initiator TCP port, target IP address, target TCP
*           port, iSCSI CID (connection ID)
*
* @param  L7_uint32 sessionId  (input) an identifier of the session table entry
* @param  L7_uint32 *connectionId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or sessionId not found 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionFirstGet(L7_uint32 sessionId, L7_uint32 *connectionId);

/*********************************************************************
* @purpose  Retrieves the next entry in the iSCSI connection table 
*           associated with the specified session.
*           Entries are returned in a sorted order by initiator IP
*           address, initiator TCP port, target IP address, target TCP
*           port, iSCSI CID (connection ID)
*
* @param  L7_uint32 sessionId  (input) an identifier of the session table entry
* @param  L7_uint32 connectionId  (input) an identifier of the current connection table entry
* @param  L7_uint32 *nextConnectionId  (output) an identifier of the retrieved
*                                table entry.  This identifier is used
*                                for getting parameters for this entry.
*
* @returns  L7_SUCCESS  table not empty and first entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or sessionId not found 
* @returns  L7_FAILURE  table empty
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionNextGet(L7_uint32 sessionId, L7_uint32 connectionId, L7_uint32 *nextConnectionId);

/*********************************************************************
* @purpose  Retrieves the next entry in the iSCSI connection table.
*           Entries are returned in order of increasing connectionId.
*
* @param  L7_uint32 connectionId  (input) an identifier of the current table entry
* @param  L7_uint32 *nextConnectionId  (output) an identifier of the retrieved
*                                table entry.  
*
* @returns  L7_SUCCESS  table not empty and next entry retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionNextByEntryIdGet(L7_uint32 connectionId, L7_uint32 *nextConnectionId);

/*********************************************************************
* @purpose  Retrieves the target IP address for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_inet_addr_t *ipAddr  (output) the IP address associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionTargetIpAddressGet(L7_uint32 connectionId, L7_inet_addr_t *ipAddr);

/*********************************************************************
* @purpose  Retrieves the initiator IP address for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_inet_addr_t *ipAddr  (output) the IP address associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionInitiatorIpAddressGet(L7_uint32 connectionId, L7_inet_addr_t *ipAddr);

/*********************************************************************
* @purpose  Retrieves the target TCP port for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *port         (output) the TCP port associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionTargetTcpPortGet(L7_uint32 connectionId, L7_uint32 *port);

/*********************************************************************
* @purpose  Retrieves the initiator TCP port for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *port         (output) the TCP port associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionInitiatorTcpPortGet(L7_uint32 connectionId, L7_uint32 *port);

/*********************************************************************
* @purpose  Retrieves the iSCSI connection id (CID) for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *cid         (output) the CID associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionCidGet(L7_uint32 connectionId, L7_uint32 *cid);

/*********************************************************************
* @purpose  Retrieves the iSCSI Session table entry id for the specified
*           entry in the iSCSI connection table.
*
* @param  L7_uint32 connectionId  (input) an identifier of the table entry
* @param  L7_uint32 *sessionId    (output) the CID associated with the specified
*                                 connection
*
* @returns  L7_SUCCESS  table not empty and data retrieved with no errors 
* @returns  L7_ERROR    function called with NULL pointer or current entry not found
* @returns  L7_FAILURE  end of table reached
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t iscsiConnectionSessionIdGet(L7_uint32 connectionId, L7_uint32 *sessionId);

#endif
 
