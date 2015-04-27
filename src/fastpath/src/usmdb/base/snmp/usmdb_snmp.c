/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\usmdb_snmp.c
*
* @purpose Provide interface to SNMP API's for unitmgr components
*
* @component unitmgr
*
* @comments tba
*
* @create 01/10/2001
*
* @author cpverne
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/



#include "l7_common.h"
#include "usmdb_snmp_api.h"
#include "osapi.h"
#include "nimapi.h"
#include "snmp_api.h"
#include "log.h"


/* Begin Function Declarations: usmdb_snmp_api.h */

/*********************************************************************
*
* @purpose Get an SNMP community access entry based on its index.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 Index      interface number
*
* @returns L7_SUCCESS, if success
* @returns L7_ERROR, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpCommunityEntryGet(L7_uint32 UnitIndex, L7_uint32 Index)
{
  L7_uint32 status;

  /* check to see if it's a valid access entry */
  if (Index >= 0 && Index < L7_MAX_SNMP_COMM && 
      SnmpCommunityStatusGet(Index, &status) == L7_SUCCESS && status != L7_SNMP_COMMUNITY_STATUS_DELETE)
    return L7_SUCCESS;

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose Get the next SNMP community access entry based on its index.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 *Index     interface number
*
* @returns L7_SUCCESS, if success
* @returns L7_ERROR, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpCommunityEntryNext(L7_uint32 UnitIndex, L7_uint32 *Index)
{
  L7_uint32 status;
    
  for (*Index += 1; *Index < L7_MAX_SNMP_COMM; *Index += 1)
  {
    if (SnmpCommunityStatusGet(*Index, &status) == L7_SUCCESS && 
        status != L7_SNMP_COMMUNITY_STATUS_DELETE)
    return L7_SUCCESS;
  }
  return(L7_FAILURE);
}


/*********************************************************************
*
* @purpose Get the SNMP community name per index  
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 Index      table index   
* @param L7_char8  *buf       pointer to Community name   
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpCommunityNameGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf)
{
  return SnmpCommunityNameGet(Index, buf);
}


/*********************************************************************
*
* @purpose Set the SNMP community name per index if not a duplicate. 
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 Index      table index   
* @param L7_char8  *buf       pointer to Community name   
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpCommunityNameSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf)
{
  return SnmpCommunityNameSet(Index, buf);
}


/*********************************************************************
*
* @purpose Get the SNMP access owner per index.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 Index      table index   
* @param L7_char8  *buf       pointer to Community name   
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpCommunityOwnerGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf)
{
  return SnmpCommunityOwnerGet(Index, buf);
}


/*********************************************************************
*
* @purpose Set the SNMP access owner per index.  
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 Index      table index   
* @param L7_char8  *buf       pointer to Community name   
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpCommunityOwnerSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf)
{
  return SnmpCommunityOwnerSet(Index, buf);
}


/*********************************************************************
*
* @purpose Get the SNMP IP addr per index.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 Index      table index   
* @param L7_uint32 *val       pointer to Community name   
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpCommunityIpAddrGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val)
{
  return SnmpCommunityIpAddrGet(Index, val);
}


/*********************************************************************
*
* @purpose Set the SNMP IP addr per index.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 Index      table index   
* @param L7_uint32 val        the ip address
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpCommunityIpAddrSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val)
{
  return SnmpCommunityIpAddrSet(Index, val);
}


/*********************************************************************
*
* @purpose Get the SNMP IP mask per index.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 Index      table index   
* @param L7_uint32 val        pointer to IP mask   
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpCommunityIpMaskGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val)
{
  return SnmpCommunityIpMaskGet(Index, val);
}


/*********************************************************************
*
* @purpose Set the SNMP IP mask per index.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 Index      table index   
* @param L7_uint32 val        IP mask   
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpCommunityIpMaskSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val)
{
  return SnmpCommunityIpMaskSet(Index, val);
}


/*********************************************************************
*
* @purpose Get the SNMP access level per index.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 Index      table index   
* @param L7_uint32 *val       pointer to access level   
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpCommunityAccessLevelGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val)
{
  return SnmpCommunityAccessLevelGet(Index, val);
}


/*********************************************************************
*
* @purpose Set the SNMP access level per index.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 Index      table index   
* @param L7_uint32 val        access level   
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpCommunityAccessLevelSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val)
{
  return SnmpCommunityAccessLevelSet(Index, val);
}


/*********************************************************************
*
* @purpose get the SNMP community status per index.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 Index      table index   
* @param L7_uint32 *val       pointer to access status
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpCommunityStatusGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 *val)
{
  return SnmpCommunityStatusGet(Index, val);
}


/*********************************************************************
*
* @purpose Set the SNMP access status per index.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 Index      table index   
* @param L7_uint32 val        access status
*
* @returns L7_SUCCESS or
* @returns L7_FAILURE
*
* @notes none 
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpCommunityStatusSet(L7_uint32 UnitIndex, L7_uint32 Index, L7_uint32 val)
{
  return SnmpCommunityStatusSet(Index, val);
}

/*********************************************************************
* @purpose  Sets/Creates an SNMP User
*
* @param    UnitIndex      The unit for this operation
* @param    userName        pointer to user name @b((input))
* @param    accessLevel     L7_SNMP_USER_ACCESS_LEVEL_t @b((input))
* @param    authProtocol    L7_SNMP_USER_AUTH_PROTO_t @b((input))
* @param    privProtocol    L7_SNMP_USER_PRIV_PROTO_t @b((input))
* @param    authKey         pointer to user's authentication password @b((input))
* @param    privKey         pointer to user's privacy password @b((input))
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t 
usmDbSnmpUserSet(L7_uint32 UnitIndex, L7_uchar8 *userName, L7_uint32 accessLevel, L7_uint32 authProtocol, L7_uint32 privProtocol, L7_uchar8 *authKey, L7_uchar8 *privKey)
{
  return SnmpUserSet(userName, accessLevel, authProtocol, privProtocol, authKey, privKey, L7_FALSE);
}

/*********************************************************************
* @purpose  Deletes an SNMP User
*
* @param    UnitIndex      The unit for this operation
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
usmDbSnmpUserDelete(L7_uint32 UnitIndex, L7_uchar8 *userName, L7_uint32 accessLevel, L7_uint32 authProtocol, L7_uint32 privProtocol)
{
  return SnmpUserDelete(userName, accessLevel, authProtocol, privProtocol);
}

/*********************************************************************
*
* @purpose  Returns status of encryption code in the SNMPv3 User Table
*
* @param    UnitIndex      The unit for this operation
*
* @returns  L7_TRUE     if encryption code is present
* @returns  L7_FALSE    if not
*
* @notes    none 
*
* @end
*********************************************************************/
L7_BOOL usmDbSnmpUserTableHasEncryption(L7_uint32 UnitIndex)
{
  return SnmpUserTableHasEncryption();
}

/*********************************************************************
* @purpose  Gets the current status of SNMP
*
* @param    UnitIndex      The unit for this operation
* @returns  L7_ENABLE or L7_DISABLE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 usmDbSnmpStatusGet(L7_uint32 UnitIndex)
{
  return SnmpStatusGet();
}

/*********************************************************************
* @purpose  Checks to see if an entry in the Supported Mibs table exists
*
* @param    UnitIndex      The unit for this operation
* @parms    index         index to check
*
* @returns  L7_SUCCESS    if the entry exists
*           L7_FAILURE    if the entry doesn't exist
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t usmDbSnmpSupportedMibTableEntryGet(L7_uint32 UnitIndex, L7_uint32 index )
{
  return SnmpSupportedMibTableEntryGet(index);
}

/*********************************************************************
* @purpose  Finds the next valid index of an entry in the Supported Mibs table
*
* @param    UnitIndex      The unit for this operation
* @parms    *index        pointer to hold the next valid index
*
* @returns  L7_SUCCESS    if annother entry exists
*           L7_FAILURE    if there are no more entries
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t usmDbSnmpSupportedMibTableEntryNextGet(L7_uint32 UnitIndex, L7_uint32 *index )
{
  return SnmpSupportedMibTableEntryNextGet(index);
}

/*********************************************************************
* @purpose  Creates an entry in the Supported Mibs Table
*
* @param    UnitIndex      The unit for this operation
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
L7_RC_t usmDbSnmpSupportedMibTableEntryCreate(L7_uint32 UnitIndex, L7_char8 mibName[], L7_char8 mibDescription[], L7_char8 midOID[] )
{
  return SnmpSupportedMibTableEntryCreate(mibName,mibDescription,midOID);
}

/*********************************************************************
* @purpose  Returns the name of the Supported Mib entry
*
* @param    UnitIndex      The unit for this operation
* @parms    index         index to return
*
* @returns  L7_SUCCESS    if the entry exists
*           L7_FAILURE    if the entry doesn't exist
*
* @notes    buffer should be large enough to hold a string of size
*           L7_SNMP_SUPPORTED_MIB_NAME_SIZE-1
*       
* @end
*********************************************************************/
L7_RC_t usmDbSnmpSupportedMibTableMibNameGet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf)
{
  return SnmpSupportedMibTableMibNameGet(index, buf);
}

/*********************************************************************
* @purpose  Returns the description of the Supported Mib entry
*
* @param    UnitIndex      The unit for this operation
* @parms    index         index to return
*
* @returns  L7_SUCCESS    if the entry exists
*           L7_FAILURE    if the entry doesn't exist
*
* @notes    buffer should be large enough to hold a string of size
*           L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE-1
*       
* @end
*********************************************************************/
L7_RC_t usmDbSnmpSupportedMibTableMibDescriptionGet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf)
{
  return SnmpSupportedMibTableMibDescriptionGet(index, buf);
}

/*********************************************************************
* @purpose  Returns the OID of the Supported Mib entry
*
* @param    UnitIndex      The unit for this operation
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
usmDbSnmpSupportedMibTableMibOIDGet(L7_uint32 UnitIndex, L7_uint32 index, L7_char8 *buf)
{
  return SnmpSupportedMibTableMibOIDGet(index, buf);
}

/*********************************************************************
* @purpose  Returns the entry creation time of the Supported Mib entry
*
* @param    L7_uint32   UnitIndex  @b((input))  The unit for this operation
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
usmDbSnmpSupportedMibTableMibEntryCreationTimeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *createTime)
{
  return SnmpSupportedMibTableMibEntryCreationTimeGet(index, createTime);
}

/*********************************************************************
* @purpose  Returns the time of the last changed to the Supported Mib table
*
* @param    L7_uint32   UnitIndex  @b((input))  The unit for this operation
* @parms    *lastChangeTime       last change time to return
*
* @returns  L7_SUCCESS    
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t
usmDbSnmpSupportedMibTableLastChangeTimeGet(L7_uint32 UnitIndex, L7_uint32 *lastChangeTime)
{
  return SnmpSupportedMibTableLastChangeTimeGet(lastChangeTime);
}

/* End Function Declarations */


