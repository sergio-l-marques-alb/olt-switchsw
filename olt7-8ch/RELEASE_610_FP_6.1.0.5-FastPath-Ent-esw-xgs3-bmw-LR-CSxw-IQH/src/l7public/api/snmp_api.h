/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: snmp_api.h
*
* Purpose: API interface for SNMP Agent
*
* Created by: Colin Verne 02/16/2001
*
* Component: SNMP
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/



#ifndef SNMP_API_H
#define SNMP_API_H

#include "l7_common.h"
#include "snmp_exports.h"
#include "osapi.h"
#include "snmp_util_api.h"

extern L7_uint32 SnmpEnable;
extern L7_uint32 SnmpResetConfig;

/* Begin Function Prototypes */

/*********************************************************************
*
* @purpose  Initialize SNMP agent communities and trap receivers.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    This should only be called once per bootup, and only by
* @notes    the SNMP agent after it's completed it's initialization.
*
* @end
*********************************************************************/
L7_RC_t
SnmpInitialize();

/*********************************************************************
*
* @purpose  Sets the current SNMP state
*
* @returns  nothing
*
* @end
*********************************************************************/
void SnmpSetState(snmpState_t state);

/*********************************************************************
*
* @purpose  Waits until the SNMP component is in the specified state
*
* @returns  nothing
*
* @end
*********************************************************************/
void SnmpWaitUntilState(snmpState_t state);

/*********************************************************************
*
* @purpose  Waits until the SNMP component is in the RUN state
*
* @returns  nothing
*
* @notes    This should be called by all tasks before any USMDB calls are
*           issued.
* @end
*********************************************************************/
void
SnmpWaitUntilRun();

/*********************************************************************
*
* @purpose  Create/Update all present Communities in the SNMP Agent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
SnmpCommunityTableUpdate();

/*********************************************************************
*
* @purpose  Deletes an entry in the SNMP Agent Community Table
*
* @param    index    Index of community to add/set
* @param    comm     SNMP Community structure
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpCommunityTableDelete(L7_uint32 index);

/*********************************************************************
*
* @purpose  Create/Update all present Trap Recievers in the SNMP Agent
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t
SnmpTrapMgrTableUpdate();

/*********************************************************************
*
* @purpose  Add or set an entry in the SNMP Agent Trap Receiver table
*
* @param    index    Index of Trap Receiver to add/set
* @param    trap     SNMP Trap Receiver structure
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
L7_RC_t
SnmpTrapReceiverTableDelete(L7_uint32 index);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Community Table Functions
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*********************************************************************
* @purpose  Returns table index of SNMP community name   
*
* @param    commName    pointer to Community name   
* @param    index       pointer to table index   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityIndexGet(L7_char8 *commName, L7_uint32 *index);


/*********************************************************************
* @purpose  Returns SNMP community name per index  
*
* @param    index       table index   
* @param    commName    pointer to Community name   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityNameGet(L7_uint32 index, L7_char8 *commName);


/*********************************************************************
* @purpose  Sets a SNMP community name per index if not a duplicate 
*
* @param    index       table index   
* @param    commName    pointer to Community name   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityNameSet(L7_uint32 index, L7_char8 *commName);


/*********************************************************************
* @purpose  Return a SNMP IP addr per index  
*
* @param    index       table index   
* @param    ipAddr      pointer to IP address   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityIpAddrGet(L7_uint32 index, L7_uint32 *ipAddr);


/*********************************************************************
* @purpose  Sets a SNMP IP addr per index  
*
* @param    index       table index   
* @param    ipAddr      IP address   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityIpAddrSet(L7_uint32 index, L7_uint32 ipAddr);


/*********************************************************************
* @purpose  Return a SNMP IP mask per index  
*
* @param    index       table index   
* @param    ipMask      pointer to IP mask   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityIpMaskGet(L7_uint32 index, L7_uint32 *ipMask);


/*********************************************************************
* @purpose  Sets a SNMP IP mask per index  
*
* @param    index       table index   
* @param    ipMask      IP mask   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityIpMaskSet(L7_uint32 index, L7_uint32 ipMask);


/*********************************************************************
* @purpose  Return a SNMP access level per index  
*
* @param    index    table index   
* @param    level    pointer to access level   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityAccessLevelGet(L7_uint32 index, L7_uint32 *level);


/*********************************************************************
* @purpose  Sets a SNMP access level per index  
*
* @param    index    table index   
* @param    level    access level   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityAccessLevelSet(L7_uint32 index, L7_uint32 level);


/*********************************************************************
* @purpose  Return a SNMP access status per index  
*
* @param    index    table index   
* @param    status   pointer to access statu 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityStatusGet(L7_uint32 index, L7_uint32 *status);


/*********************************************************************
* @purpose  Sets a SNMP access status per index  
*
* @param    index    table index   
* @param    status   access status   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityStatusSet(L7_uint32 index, L7_uint32 status);


/*********************************************************************
* @purpose  Return a SNMP access owner per index  
*
* @param    index    table index   
* @param    owner    pointer to access owner   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityOwnerGet(L7_uint32 index, L7_char8 *owner);


/*********************************************************************
* @purpose  Sets a SNMP access owner per index  
*
* @param    index    table index   
* @param    owner    pointer to access owner   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpCommunityOwnerSet(L7_uint32 index, L7_char8 *owner);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Trap Receiver Table Functions
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*********************************************************************
* @purpose  Returns SNMP Trap Receiver community name per index  
*
* @param    index       table index   
* @param    commName    pointer to Community name   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrCommunityGet(L7_uint32 index, L7_char8 *commName);

/*********************************************************************
* @purpose  Sets a SNMP Trap Receiver community name by index
*
* @param    index       table index   
* @param    commName    pointer to Community name   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrCommunitySet(L7_uint32 index, L7_char8 *commName);

/*********************************************************************
* @purpose  Return a SNMP IP addr per index  
*
* @param    index       table index   
* @param    ipAddr      pointer to IP address   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrIpAddrGet(L7_uint32 index, L7_uint32 *ipAddr);

/*********************************************************************
* @purpose  Return a SNMP IPv6 addr per index  
*
* @param    index       table index   
* @param    ipv6Addr    pointer to IPv6 address   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrIPv6AddrGet(L7_uint32 index, L7_in6_addr_t *ipv6Addr);

/*********************************************************************
* @purpose  Sets a SNMP IP addr per index  
*
* @param    index       table index   
* @param    ipAddr      IP address   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrIpAddrSet(L7_uint32 index, L7_uint32 ipAddr);

/*********************************************************************
* @purpose  Sets a SNMP IPv6 addr per index  
*
* @param    index       table index   
* @param    ipv6Addr    IPv6 address   
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrIPv6AddrSet(L7_uint32 index, L7_in6_addr_t *ipv6Addr);

/*********************************************************************
* @purpose  Return a SNMP Trap Manager status per index  
*
* @param    index    table index   
* @param    status   pointer to trap manager status entry  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrStatusGet(L7_uint32 index, L7_uint32 *status);

/*********************************************************************
* @purpose  Sets a SNMP Trap Manager per index  
*
* @param    index    table index   
* @param    status   trap manager status entry  
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrStatusSet(L7_uint32 index, L7_uint32 status);

/*********************************************************************
* @purpose  Return a SNMP Version per index  
*
* @param    index       table index   
* @param    version     pointer to SNMP Version
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrVersionGet(L7_uint32 index, snmpTrapVer_t *version);

/*********************************************************************
* @purpose  Sets a SNMP IP addr per index  
*
* @param    index       table index   
* @param    version     SNMP Version
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpTrapMgrVersionSet(L7_uint32 index, snmpTrapVer_t version);

/*********************************************************************
* @purpose  Sets/Creates an SNMP User
*
* @param    userName        pointer to user name @b((input))
* @param    accessLevel     L7_SNMP_USER_ACCESS_LEVEL_t @b((input))
* @param    authProtocol    L7_SNMP_USER_AUTH_PROTO_t @b((input))
* @param    privProtocol    L7_SNMP_USER_PRIV_PROTO_t @b((input))
* @param    authKey         pointer to user's authentication password @b((input))
* @param    privKey         pointer to user's privacy password @b((input))
*
* @param    restore     L7_TRUE - Restore of the users from CLI, so shouldn't
*                       overwrite existing SNMP users
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpUserSet(L7_uchar8 *userName, L7_uint32 accessLevel, L7_uint32 authProtocol, L7_uint32 privProtocol, L7_uchar8 *authKey, L7_uchar8 *privKey, L7_BOOL restore);

/*********************************************************************
* @purpose  Deletes an SNMP User Name
*
* @param    userName        pointer to user name @b((input))
* @param    accessLevel     L7_SNMP_USER_ACCESS_LEVEL_t @b((input))
* @param    authProtocol    L7_SNMP_USER_AUTH_PROTO_t @b((input))
* @param    privProtocol    L7_SNMP_USER_PRIV_PROTO_t @b((input))
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
SnmpUserDelete(L7_uchar8 *userName, L7_uint32 accessLevel, L7_uint32 authProtocol, L7_uint32 privProtocol);

/*********************************************************************
*
* @purpose  Returns status of encryption code in the SNMPv3 User Table
*
* @param    void
*
* @returns  L7_TRUE     if encryption code is present
* @returns  L7_FALSE    if not
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL
SnmpUserTableHasEncryption();

/*********************************************************************
* @purpose  Gets the current status of SNMP
*
* @returns  L7_ENABLE or L7_DISABLE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32
SnmpStatusGet();

/*********************************************************************
* @purpose  Check to see if SNMP is present.
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL 
snmpPresent();


/*********************************************************************
* @purpose  Retrieve an unsigned integer MIB object
*
* @parms    *oid_buf      string buffer containing the OID to retreive
*           *val          unsigned integer to place the value into
*
* @returns  L7_SUCCESS    if the object could be retrieved successfully
*           L7_FAILURE    if the object couldn't be retrieved
*           L7_ERROR      if the object was not an unsigned integer
*
* @notes    OID is in the form of "0.1.2.3.4.5"
*       
* @end
*********************************************************************/
L7_RC_t
SnmpGetVarUInt( L7_char8 *oid_buf, L7_uint32 *val);

/*********************************************************************
* @purpose  Retrieve a signed integer MIB object
*
* @parms    *oid_buf      string buffer containing the OID to retreive
*           *val          signed integer to place the value into
*
* @returns  L7_SUCCESS    if the object could be retrieved successfully
*           L7_FAILURE    if the object couldn't be retrieved
*           L7_ERROR      if the object was not a signed integer
*
* @notes    OID is in the form of "0.1.2.3.4.5"
*       
* @end
*********************************************************************/
L7_RC_t
SnmpGetVarSInt( L7_char8 *oid_buf, L7_int32 *val);

/*********************************************************************
* @purpose  Retrieve an character buffer MIB object
*
* @parms    *oid_buf      string buffer containing the OID to retreive
*           *buf          character buffer to place the value into
*           *buf_len      unsigned integer to place the buffer length into
*
* @returns  L7_SUCCESS    if the object could be retrieved successfully
*           L7_FAILURE    if the object couldn't be retrieved
*           L7_ERROR      if the object was not an octet string
*
* @notes    OID is in the form of "0.1.2.3.4.5"
*       
* @end
*********************************************************************/
L7_RC_t
SnmpGetVarCharBuf( L7_char8 *oid_buf, L7_char8 *buf, L7_uint32 *buf_len);

/*********************************************************************
* @purpose  Checks to see if an entry in the Supported Mibs table exists
*
* @parms    index         index to check
*
* @returns  L7_SUCCESS    if the entry exists
*           L7_FAILURE    if the entry doesn't exist
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableEntryGet( L7_uint32 index );

/*********************************************************************
* @purpose  Finds the next valid index of an entry in the Supported Mibs table
*
* @parms    *index        pointer to hold the next valid index
*
* @returns  L7_SUCCESS    if annother entry exists
*           L7_FAILURE    if there are no more entries
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableEntryNextGet( L7_uint32 *index );

/*********************************************************************
* @purpose  Creates an entry in the Supported Mibs Table
*
* @parms    mibName         name of the mib (RFC number or short name)
*           mibDescription  string to describe the mib
*           mibOID          string buffer containing the OID to retreive
*
* @returns  L7_SUCCESS    if creation was successful
*           L7_FAILURE    if there are no empty positions available or
*                         if the provided strings are too long
*
* @notes    OID is in the form of "0.1.2.3.4.5"
* @notes    This function is used only by the SNMP Agent to add entries 
*           during initialization
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableEntryCreate( L7_char8 mibName[], L7_char8 mibDescription[], L7_char8  mibOID[] );

/*********************************************************************
* @purpose  Returns the name of the Supported Mib entry
*
* @parms    index         index to return
*
* @returns  L7_SUCCESS    if the entry exists
*           L7_FAILURE    if the entry doesn't exist
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableMibNameGet( L7_uint32 index, L7_char8 *buf );

/*********************************************************************
* @purpose  Returns the description of the Supported Mib entry
*
* @parms    index         index to return
*
* @returns  L7_SUCCESS    if the entry exists
*           L7_FAILURE    if the entry doesn't exist
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableMibDescriptionGet( L7_uint32 index, L7_char8 *buf );

/*********************************************************************
* @purpose  Returns the OID of the Supported Mib entry
*
* @parms    index         index into supported MIBs table
* @parms    *buf          OID to return
*
* @returns  L7_SUCCESS    if the entry exists
*           L7_FAILURE    if the entry doesn't exist
*
* @notes    buffer should be large enough to hold a string of size
*           L7_SNMP_SUPPORTED_MIB_OID_SIZE-1
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableMibOIDGet(L7_uint32 index, L7_char8 *buf);

/*********************************************************************
* @purpose  Returns the entry creation time of the Supported Mib entry
*
* @parms    index         index into supported MIBs table
* @parms    *createTime   creation time to return
*
* @returns  L7_SUCCESS    if the entry exists
*           L7_FAILURE    if the entry doesn't exist
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableMibEntryCreationTimeGet(L7_uint32 index, L7_uint32 *createTime);

/*********************************************************************
* @purpose  Returns the time of the last changed to the Supported Mib table
*
* @parms    *lastChangeTime       last change time to return
*
* @returns  L7_SUCCESS    
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t
SnmpSupportedMibTableLastChangeTimeGet(L7_uint32 *lastChangeTime);

/*********************************************************************
* @purpose  Gets the current status of Trap master flag
*
* @param    UnitIndex      The unit for this operation
* @param    val   Status of Trap flag (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t
SnmpEnableTrapsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the current status of Trap master flag
*
* @param    L7_uint32 UnitIndex      The unit for this operation
* @param    val   Status of Trap flag (L7_ENABLE or L7_DISABLE)
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t
SnmpEnableTrapsSet(L7_uint32 val);

/* End Function Prototypes */
#endif /* SNMP_API_H */





