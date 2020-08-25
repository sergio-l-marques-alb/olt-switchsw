/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2008
*
**********************************************************************
*
* @filename    usmdb_qos_iscsi.c
* @purpose     usmdb iSCSI Flow Acceleration API functions
* @component   QoS iSCSI Flow Acceleration
* @comments    none
* @create      05/12/2008
* @end
*             
**********************************************************************/

#include "l7_common.h"
#include "usmdb_qos_iscsi.h"
#include "iscsi_api.h"
#include "osapi_support.h"

/*********************************************************************
* @purpose  Configures the global administative mode for the
* iSCSI component
*
* @param  L7_uint32 adminMode      (input) Global enable mode for iSCSI
*
* @returns  L7_SUCCESS   the requested admin mode was set with no errors
*                        or the requested setting matched the current 
*                        setting
* @returns  L7_FAILURE   input parameters were out of range or an error 
*                        was encountered while processing the set
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIscsiAdminModeSet(L7_uint32 adminMode)
{
  return(iscsiAdminModeSet(adminMode));
}

/*********************************************************************
* @purpose  Retrieves the global administative mode iSCSI component
*
* @param  L7_uint32 *adminMode      (output) Global enable mode for iSCSI
*
* @returns  L7_SUCCESS  current setting successfully retrieved
* @returns  L7_FAILURE  function was called with a NULL pointer
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIscsiAdminModeGet(L7_uint32 *adminMode)
{
  return(iscsiAdminModeGet(adminMode));
}

/*********************************************************************
* @purpose  Configures the VLAN priority for treatment of iSCSI packets
*
* @param  L7_uint32 priority      (input)  VLAN tag priority 
*
* @returns  L7_SUCCESS  requested set completed with no errors or 
*                       matched current setting
* @returns  L7_ERROR    parameter out of range
* @returns  L7_FAILURE  error encountered while processing set
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIscsiVlanPrioritySet(L7_uint32 priority)
{
  return(iscsiVlanPrioritySet(priority));
}

/*********************************************************************
* @purpose  Retrieve the VLAN priority value used for iSCSI session
*           data packets
*
* @param  L7_uint32 *priority      (output)  VLAN tag priority 
*
* @returns  L7_SUCCESS  current setting successfully retrieved
* @returns  L7_FAILURE  function was called with a NULL pointer
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIscsiVlanPriorityGet(L7_uint32 *priority)
{
  return(iscsiVlanPriorityGet(priority));
}

/*********************************************************************
* @purpose  Configures the DSCP value for treatment of iSCSI packets
*
* @param  L7_uint32 dscp      (input)  DSCP field data
*
* @returns  L7_SUCCESS  requested set completed with no errors or 
*                       matched current setting
* @returns  L7_ERROR    parameter out of range
* @returns  L7_FAILURE  error encountered while processing set
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIscsiDscpSet(L7_uint32 dscp)
{
  return(iscsiDscpSet(dscp));
}

/*********************************************************************
* @purpose  Retrieve the DSCP value used for iSCSI session
*           data packets
*
* @param  L7_uint32 *dscp      (output)  DSCP value
*
* @returns  L7_SUCCESS  current setting successfully retrieved
* @returns  L7_FAILURE  function was called with a NULL pointer
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIscsiDscpGet(L7_uint32 *dscp)
{
  return(iscsiDscpGet(dscp));
}

/*********************************************************************
* @purpose  Configures the IP precedence value for treatment of iSCSI packets
*
* @param  L7_uint32 prec      (input)  IP precedence data
*
* @returns  L7_SUCCESS  requested set completed with no errors or 
*                       matched current setting
* @returns  L7_ERROR    parameter out of range
* @returns  L7_FAILURE  error encountered while processing set
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIscsiPrecSet(L7_uint32 prec)
{
  return(iscsiPrecSet(prec));
}

/*********************************************************************
* @purpose  Retrieve the PREC value used for iSCSI session
*           data packets
*
* @param  L7_uint32 *prec      (output)  PREC value
*
* @returns  L7_SUCCESS  current setting successfully retrieved
* @returns  L7_FAILURE  function was called with a NULL pointer
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIscsiPrecGet(L7_uint32 *prec)
{
  return(iscsiPrecGet(prec));
}

/*********************************************************************
* @purpose  Configures whether to mark iSCSI packets
*
* @param  L7_uint32 mode  (input) enable mode for iSCSI marking
*
* @returns  L7_SUCCESS   the requested mode was set with no errors
*                        or the requested setting matched the current 
*                        setting
* @returns  L7_FAILURE   input parameters were out of range or an error 
*                        was encountered while processing the set
*
* @end
*********************************************************************/
L7_RC_t usmDbIscsiMarkingModeSet(L7_uint32 mode)
{
  return(iscsiMarkingModeSet(mode));
}

/*********************************************************************
* @purpose  Retrieve mode for iSCSI packet marking
*
* @param  L7_uint32 *mode      (output)  value
*
* @returns  L7_SUCCESS  current setting successfully retrieved
* @returns  L7_FAILURE  function was called with a NULL pointer
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIscsiMarkingModeGet(L7_uint32 *mode)
{
  return(iscsiMarkingModeGet(mode));
}

/*********************************************************************
* @purpose  Configures which type of packet priority marking to use
*           for iSCSI session traffic
*
* @param  L7_QOS_COS_MAP_INTF_MODE_t selector      (input)  what type
*                    of marking to apply to session traffic
*
* @returns  L7_SUCCESS  requested set completed with no errors or 
*                       matched current setting
* @returns  L7_ERROR    parameter out of range
* @returns  L7_FAILURE  error encountered while processing set
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIscsiTagFieldSet(L7_QOS_COS_MAP_INTF_MODE_t selector)
{
  return(iscsiTagFieldSet(selector));
}

/*********************************************************************
* @purpose  Retrieve the PREC value used for iSCSI session
*           data packets
*
* @param  L7_QOS_COS_MAP_INTF_MODE_t *selector  (output)  what type
*                    of marking to apply to session traffic
*
* @returns  L7_SUCCESS  current setting successfully retrieved
* @returns  L7_FAILURE  function was called with a NULL pointer
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIscsiTagFieldGet(L7_QOS_COS_MAP_INTF_MODE_t *selector)
{
  return(iscsiTagFieldGet(selector));
}

/*********************************************************************
* @purpose  Configures the session time out value used for iSCSI sessions
*
* @param  L7_uint32 seconds   (input) number of seconds to wait before
*                             deleting a dormant iSCSI session  
*
* @returns  L7_SUCCESS  requested set completed with no errors or 
*                       matched current setting
* @returns  L7_ERROR    parameter out of range
* @returns  L7_FAILURE  error encountered while processing set
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIscsiTimeOutIntervalSet(L7_uint32 seconds)
{
  return(iscsiTimeOutIntervalSet(seconds));
}

/*********************************************************************
* @purpose  Retrieve the session time out value used for iSCSI sessions
*
* @param  L7_uint32 *seconds   (output) number of seconds to wait before
*                               deleting a dormant iSCSI session  
*
* @returns  L7_SUCCESS  current setting successfully retrieved
* @returns  L7_FAILURE  function was called with a NULL pointer
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIscsiTimeOutIntervalGet(L7_uint32 *seconds)
{
  return(iscsiTimeOutIntervalGet(seconds));
}

/*********************************************************************
* @purpose  Configures the TCP ports and (optionally) IP addresses for
*           traffic to monitor for iSCSI session setup packets.  The
*           target name can be configured for user convenience.
*
* @param  L7_uint32 tcpPort    (input) TCP port to be monitored  
* @param  L7_inet_addr_t ipAddress  (input) IP address to be monitored on this port
* @param  L7_uint32 targetName (input) target's name 
*
* @returns  L7_SUCCESS 
*           L7_FAILURE
*           L7_TABLE_IS_FULL
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
* @end
*********************************************************************/
L7_RC_t usmDbIscsiTargetTcpPortAdd(L7_uint32 tcpPort, L7_inet_addr_t *ipAddress, L7_uchar8 *targetName)
{
  return(iscsiTargetTcpPortAdd(tcpPort, ipAddress, targetName));
}

/*********************************************************************
* @purpose  Removes the TCP ports and (optionally) IP addresses for
*           traffic to monitor for iSCSI session setup packets.  The
*           target name can be configured for user convenience.
*
* @param  L7_uint32 tcpPort    (input) TCP port to be monitored  
* @param  L7_uint32 ipAddress  (input) IP address to be monitored on this port
*
* @returns  L7_SUCCESS  deletion completed with no errors 
* @returns  L7_ERROR    incorrect parameter 
* @returns  L7_FAILURE  error encountered while processing deletion
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t usmDbIscsiTargetTcpPortDelete(L7_uint32 tcpPort, L7_inet_addr_t *ipAddress)
{
  return(iscsiTargetTcpPortDelete(tcpPort, ipAddress));
}

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
L7_RC_t usmDbIscsiTargetTableFirstGet(L7_uint32 *entryId, L7_BOOL includeDeletedDfltCfgEntries)
{
  return(iscsiTargetTableFirstGet(entryId, includeDeletedDfltCfgEntries));
}

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
L7_RC_t usmDbIscsiTargetTableNextGet(L7_uint32 entryId, L7_uint32 *nextEntryId, L7_BOOL includeDeletedDfltCfgEntries)
{
  return(iscsiTargetTableNextGet(entryId, nextEntryId, includeDeletedDfltCfgEntries));
}

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
L7_RC_t usmDbIscsiTargetTableEntryGet(L7_uint32 tcpPortNumber, L7_inet_addr_t *ipAddress, L7_uint32 *entryId)
{
  return(iscsiTargetTableEntryGet(tcpPortNumber, ipAddress, entryId));
}

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
L7_RC_t usmDbIscsiTargetTableEntryNextGet(L7_uint32 tcpPortNumber, L7_inet_addr_t *ipAddress, L7_uint32 *nextEntryId)
{
  return(iscsiTargetTableEntryNextGet(tcpPortNumber, ipAddress, nextEntryId));
}

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
L7_BOOL usmDbIscsiTargetTableIsEntryInDefaultConfig(L7_uint32 entryId)
{
  return(iscsiTargetTableIsEntryInDefaultConfig(entryId));
}

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
L7_BOOL usmDbIscsiTargetTableEntryDeletedDefaultConfigFlagGet(L7_uint32 entryId)
{
  return(iscsiTargetTableEntryDeletedDefaultConfigFlagGet(entryId));
}

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
L7_RC_t usmDbIscsiTargetTableEntryTcpPortGet(L7_uint32 entryId, L7_uint32 *tcpPortNumber)
{
  return(iscsiTargetTableEntryTcpPortGet(entryId, tcpPortNumber));
}

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
L7_BOOL usmDbIscsiTargetTableEntryInUseStatusGet(L7_uint32 entryId)
{
  return(iscsiTargetTableEntryInUseStatusGet(entryId));
}

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
L7_RC_t usmDbIscsiTargetTableEntryIpAddressGet(L7_uint32 entryId, L7_inet_addr_t *ipAddress)
{
  return(iscsiTargetTableEntryIpAddressGet(entryId, ipAddress));
}

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
L7_RC_t usmDbIscsiTargetTableEntryTargetNameGet(L7_uint32 entryId, L7_uchar8 *name)
{
  return(iscsiTargetTableEntryTargetNameGet(entryId, name));
}

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
L7_RC_t usmDbIscsiSessionFirstGet(L7_uint32 *sessionId)
{
  return(iscsiSessionFirstGet(sessionId));
}

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
L7_RC_t usmDbIscsiSessionNextGet(L7_uint32 sessionId, L7_uint32 *nextSessionId)
{
  return(iscsiSessionNextGet(sessionId, nextSessionId));
}

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
L7_RC_t usmDbIscsiSessionNextByEntryIdGet(L7_uint32 sessionId, L7_uint32 *nextSessionId)
{
  return(iscsiSessionNextByEntryIdGet(sessionId, nextSessionId));
}

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
L7_RC_t usmDbIscsiSessionTargetNameGet(L7_uint32 sessionId, L7_uchar8 *name)
{
  return(iscsiSessionTargetNameGet(sessionId, name));
}

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
L7_RC_t usmDbIscsiSessionInitiatorNameGet(L7_uint32 sessionId, L7_uchar8 *name)
{
  return(iscsiSessionInitiatorNameGet(sessionId, name));
}

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
L7_RC_t usmDbIscsiSessionStartTimeGet(L7_uint32 sessionId, L7_clocktime *time)
{
  return(iscsiSessionStartTimeGet(sessionId, time));
}

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
L7_RC_t usmDbIscsiSessionSilentTimeGet(L7_uint32 sessionId, L7_uint32 *seconds)
{
  return(iscsiSessionSilentTimeGet(sessionId, seconds));
}

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
L7_RC_t usmDbIscsiSessionIsidGet(L7_uint32 sessionId, L7_uchar8 *isid)
{
  return(iscsiSessionIsidGet(sessionId, isid));
}

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
L7_RC_t usmDbIscsiConnectionFirstGet(L7_uint32 sessionId, L7_uint32 *connectionId)
{
  return(iscsiConnectionFirstGet(sessionId, connectionId));
}

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
L7_RC_t usmDbIscsiConnectionNextGet(L7_uint32 sessionId, L7_uint32 connectionId, L7_uint32 *nextConnectionId)
{
  return(iscsiConnectionNextGet(sessionId, connectionId, nextConnectionId));
}

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
L7_RC_t usmDbIscsiConnectionNextByEntryIdGet(L7_uint32 connectionId, L7_uint32 *nextConnectionId)
{
  return(iscsiConnectionNextByEntryIdGet(connectionId, nextConnectionId));
}

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
L7_RC_t usmDbIscsiConnectionTargetIpAddressGet(L7_uint32 connectionId, L7_inet_addr_t *ipAddr)
{
  return(iscsiConnectionTargetIpAddressGet(connectionId, ipAddr));
}

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
L7_RC_t usmDbIscsiConnectionInitiatorIpAddressGet(L7_uint32 connectionId, L7_inet_addr_t *ipAddr)
{
  return(iscsiConnectionInitiatorIpAddressGet(connectionId, ipAddr));
}

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
L7_RC_t usmDbIscsiConnectionTargetTcpPortGet(L7_uint32 connectionId, L7_uint32 *port)
{
  return(iscsiConnectionTargetTcpPortGet(connectionId, port));
}

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
L7_RC_t usmDbIscsiConnectionInitiatorTcpPortGet(L7_uint32 connectionId, L7_uint32 *port)
{
  return(iscsiConnectionInitiatorTcpPortGet(connectionId, port));
}

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
L7_RC_t usmDbIscsiConnectionCidGet(L7_uint32 connectionId, L7_uint32 *cid)
{
  return(iscsiConnectionCidGet(connectionId, cid));
}

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
L7_RC_t usmDbIscsiConnectionSessionIdGet(L7_uint32 connectionId, L7_uint32 *sessionId)
{
  return(iscsiConnectionSessionIdGet(connectionId, sessionId));
}
