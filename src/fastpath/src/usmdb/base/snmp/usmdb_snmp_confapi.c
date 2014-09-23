#include "l7_common.h"
#include "usmdb_snmp_confapi_api.h"
#include "snmp_confapi.h"
#include <string.h>

/**********************************************************************************/

/*********************************************************************
*
* @purpose Checks that the row exists for the given instances
*
* @returns  L7_SUCCESS    if the row exists
*           L7_FAILURE    if no row exists
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunityTableCheckValid(L7_char8 *snmpCommunityIndex)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunityTableCheckValid(snmpCommunityIndex);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Retrieves the next valid instance for the table
*
* @returns  L7_SUCCESS    if annother row exists
*           L7_FAILURE    if no more rows exist
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunityTableNextGet(L7_char8 *snmpCommunityIndex)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunityTableNextGet(snmpCommunityIndex);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Retrieves the row index for a given community name
*
* @returns  L7_SUCCESS    if the row exists
*           L7_FAILURE    if no row exists
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunityTableCheckCommunity(L7_char8 *snmpCommunityName, L7_char8 *snmpCommunityIndex)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunityTableCheckCommunity(snmpCommunityName, snmpCommunityIndex);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The community string for which a row in this table
*        represents a configuration.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunityNameGet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunityNameGet(snmpCommunityIndex, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The community string for which a row in this table
*        represents a configuration.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunityNameSet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunityNameSet(snmpCommunityIndex, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunitySecurityName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes A human readable string representing the corresponding
*        value of snmpCommunityName in a Security Model
*        independent format.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunitySecurityNameGet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunitySecurityNameGet(snmpCommunityIndex, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunitySecurityName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes A human readable string representing the corresponding
*        value of snmpCommunityName in a Security Model
*        independent format.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunitySecurityNameSet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunitySecurityNameSet(snmpCommunityIndex, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityContextEngineID
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The contextEngineID indicating the location of the
*        context in which management information is accessed
*        when using the community string specified by the
*        corresponding instance of snmpCommunityName.
*        
*        The default value is the snmpEngineID of the entity in
*        which this object is instantiated.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunityContextEngineIDGet(L7_char8* snmpCommunityIndex, L7_char8 *val, L7_uint32 *len)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunityContextEngineIDGet(snmpCommunityIndex, val, len);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityContextEngineID
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The contextEngineID indicating the location of the
*        context in which management information is accessed
*        when using the community string specified by the
*        corresponding instance of snmpCommunityName.
*        
*        The default value is the snmpEngineID of the entity in
*        which this object is instantiated.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunityContextEngineIDSet(L7_char8* snmpCommunityIndex, L7_char8 *val, L7_uint32 len)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunityContextEngineIDSet(snmpCommunityIndex, val, len);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityContextName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The context in which management information is accessed
*        when using the community string specified by the corresponding
*        instance of snmpCommunityName.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunityContextNameGet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunityContextNameGet(snmpCommunityIndex, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityContextName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The context in which management information is accessed
*        when using the community string specified by the corresponding
*        instance of snmpCommunityName.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunityContextNameSet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunityContextNameSet(snmpCommunityIndex, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityTransportTag
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object specifies a set of transport endpoints
*        from which an agent will accept management requests.
*        If a management request containing this community
*        is received on a transport endpoint other than the
*        transport endpoints identified by this object, the
*        request is deemed unauthentic.
*        
*        The transports identified by this object are specified
*        in the snmpTargetAddrTable.  Entries in that table
*        whose snmpTargetAddrTagList contains this tag value
*        are identified.
*        
*        If the value of this object has zero-length, transport
*        endpoints are not checked when authenticating messages
*        containing this community string.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunityTransportTagGet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunityTransportTagGet(snmpCommunityIndex, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityTransportTag
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object specifies a set of transport endpoints
*        from which an agent will accept management requests.
*        If a management request containing this community
*        is received on a transport endpoint other than the
*        transport endpoints identified by this object, the
*        request is deemed unauthentic.
*        
*        The transports identified by this object are specified
*        in the snmpTargetAddrTable.  Entries in that table
*        whose snmpTargetAddrTagList contains this tag value
*        are identified.
*        
*        If the value of this object has zero-length, transport
*        endpoints are not checked when authenticating messages
*        containing this community string.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunityTransportTagSet(L7_char8* snmpCommunityIndex, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunityTransportTagSet(snmpCommunityIndex, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type for this conceptual row in the
*        snmpCommunityTable.  Conceptual rows having the value
*        columnar object in the row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunityStorageTypeGet(L7_char8* snmpCommunityIndex, l7_snmpStorageType_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunityStorageTypeGet(snmpCommunityIndex, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type for this conceptual row in the
*        snmpCommunityTable.  Conceptual rows having the value
*        columnar object in the row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunityStorageTypeSet(L7_char8* snmpCommunityIndex, l7_snmpStorageType_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunityStorageTypeSet(snmpCommunityIndex, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row in the snmpCommunityTable.
*        
*        An entry in this table is not qualified for activation
*        until instances of all corresponding columns have been
*        initialized, either through default values, or through
*        Set operations.  The snmpCommunityName and
*        snmpCommunitySecurityName objects must be explicitly set.
*        
*        There is no restriction on setting columns in this table
*        when the value of snmpCommunityStatus is active(1).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunityStatusGet(L7_char8* snmpCommunityIndex, l7_snmpRowStatus_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunityStatusGet(snmpCommunityIndex, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpCommunityStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row in the snmpCommunityTable.
*        
*        An entry in this table is not qualified for activation
*        until instances of all corresponding columns have been
*        initialized, either through default values, or through
*        Set operations.  The snmpCommunityName and
*        snmpCommunitySecurityName objects must be explicitly set.
*        
*        There is no restriction on setting columns in this table
*        when the value of snmpCommunityStatus is active(1).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpCommunityStatusSet(L7_char8* snmpCommunityIndex, l7_snmpRowStatus_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpCommunityStatusSet(snmpCommunityIndex, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/**********************************************************************************/


/*********************************************************************
*
* @purpose Checks that the row exists for the given instances
*
* @returns  L7_SUCCESS    if the row exists
*           L7_FAILURE    if no row exists
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterProfileTableCheckValid(L7_char8 *snmpTargetParamsName)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterProfileTableCheckValid(snmpTargetParamsName);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Retrieves the next valid instance for the table
*
* @returns  L7_SUCCESS    if annother row exists
*           L7_FAILURE    if no more rows exist
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterProfileTableNextGet(L7_char8 *snmpTargetParamsName)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterProfileTableNextGet(snmpTargetParamsName);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterProfileName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The name of the filter profile to be used when generating
*        notifications using the corresponding entry in the
*        snmpTargetAddrTable.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterProfileNameGet(L7_char8* snmpTargetParamsName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterProfileNameGet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterProfileName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The name of the filter profile to be used when generating
*        notifications using the corresponding entry in the
*        snmpTargetAddrTable.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterProfileNameSet(L7_char8* snmpTargetParamsName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterProfileNameSet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterProfileStorType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type of this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterProfileStorTypeGet(L7_char8* snmpTargetParamsName, l7_snmpStorageType_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterProfileStorTypeGet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterProfileStorType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type of this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterProfileStorTypeSet(L7_char8* snmpTargetParamsName, l7_snmpStorageType_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterProfileStorTypeSet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterProfileRowStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the
*        snmpNotifyFilterProfileRowStatus column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until the corresponding instance of
*        snmpNotifyFilterProfileName has been set.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterProfileRowStatusGet(L7_char8* snmpTargetParamsName, l7_snmpRowStatus_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterProfileRowStatusGet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterProfileRowStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the
*        snmpNotifyFilterProfileRowStatus column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until the corresponding instance of
*        snmpNotifyFilterProfileName has been set.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterProfileRowStatusSet(L7_char8* snmpTargetParamsName, l7_snmpRowStatus_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterProfileRowStatusSet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/**********************************************************************************/

/*********************************************************************
*
* @purpose Checks that the row exists for the given instances
*
* @returns  L7_SUCCESS    if the row exists
*           L7_FAILURE    if no row exists
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterTableCheckValid(L7_char8 *snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterTableCheckValid(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Retrieves the next valid instance for the table
*
* @returns  L7_SUCCESS    if annother row exists
*           L7_FAILURE    if no more rows exist
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterTableNextGet(L7_char8 *snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterTableNextGet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterMask
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The bit mask which, in combination with the corresponding
*        instance of snmpNotifyFilterSubtree, defines a family of
*        subtrees which are included in or excluded from the
*        filter profile.
*        
*        Each bit of this bit mask corresponds to a
*        sub-identifier of snmpNotifyFilterSubtree, with the
*        most significant bit of the i-th octet of this octet
*        string value (extended if necessary, see below)
*        corresponding to the (8*i - 7)-th sub-identifier, and
*        the least significant bit of the i-th octet of this
*        octet string corresponding to the (8*i)-th
*        sub-identifier, where i is in the range 1 through 16.
*        
*        Each bit of this bit mask specifies whether or not
*        the corresponding sub-identifiers must match when
*        determining if an OBJECT IDENTIFIER matches this
*        family of filter subtrees; a '1' indicates that an
*        exact match must occur; a '0' indicates 'wild card',
*        i.e., any sub-identifier value matches.
*        
*        Thus, the OBJECT IDENTIFIER X of an object instance
*        is contained in a family of filter subtrees if, for
*        each sub-identifier of the value of
*        snmpNotifyFilterSubtree, either:
*        
*          the i-th bit of snmpNotifyFilterMask is 0, or
*        
*          the i-th sub-identifier of X is equal to the i-th
*          sub-identifier of the value of
*          snmpNotifyFilterSubtree.
*        
*        If the value of this bit mask is M bits long and
*        there are more than M sub-identifiers in the
*        corresponding instance of snmpNotifyFilterSubtree,
*        then the bit mask is extended with 1's to be the
*        required length.
*        
*        Note that when the value of this object is the
*        zero-length string, this extension rule results in
*        a mask of all-1's being used (i.e., no 'wild card'),
*        and the family of filter subtrees is the one
*        subtree uniquely identified by the corresponding
*        instance of snmpNotifyFilterSubtree.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterMaskGet(L7_char8* snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, L7_char8 *val, L7_uint32 *len)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterMaskGet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, val, len);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterMask
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The bit mask which, in combination with the corresponding
*        instance of snmpNotifyFilterSubtree, defines a family of
*        subtrees which are included in or excluded from the
*        filter profile.
*        
*        Each bit of this bit mask corresponds to a
*        sub-identifier of snmpNotifyFilterSubtree, with the
*        most significant bit of the i-th octet of this octet
*        string value (extended if necessary, see below)
*        corresponding to the (8*i - 7)-th sub-identifier, and
*        the least significant bit of the i-th octet of this
*        octet string corresponding to the (8*i)-th
*        sub-identifier, where i is in the range 1 through 16.
*        
*        Each bit of this bit mask specifies whether or not
*        the corresponding sub-identifiers must match when
*        determining if an OBJECT IDENTIFIER matches this
*        family of filter subtrees; a '1' indicates that an
*        exact match must occur; a '0' indicates 'wild card',
*        i.e., any sub-identifier value matches.
*        
*        Thus, the OBJECT IDENTIFIER X of an object instance
*        is contained in a family of filter subtrees if, for
*        each sub-identifier of the value of
*        snmpNotifyFilterSubtree, either:
*        
*          the i-th bit of snmpNotifyFilterMask is 0, or
*        
*          the i-th sub-identifier of X is equal to the i-th
*          sub-identifier of the value of
*          snmpNotifyFilterSubtree.
*        
*        If the value of this bit mask is M bits long and
*        there are more than M sub-identifiers in the
*        corresponding instance of snmpNotifyFilterSubtree,
*        then the bit mask is extended with 1's to be the
*        required length.
*        
*        Note that when the value of this object is the
*        zero-length string, this extension rule results in
*        a mask of all-1's being used (i.e., no 'wild card'),
*        and the family of filter subtrees is the one
*        subtree uniquely identified by the corresponding
*        instance of snmpNotifyFilterSubtree.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterMaskSet(L7_char8* snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, L7_char8 *val, L7_uint32 len)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterMaskSet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, val, len);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object indicates whether the family of filter subtrees
*        defined by this entry are included in or excluded from a
*        filter.  A more detailed discussion of the use of this
*        object can be found in section 6. of [RFC2573].
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterTypeGet(L7_char8* snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, l7_snmpNotifyFilterType_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterTypeGet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object indicates whether the family of filter subtrees
*        defined by this entry are included in or excluded from a
*        filter.  A more detailed discussion of the use of this
*        object can be found in section 6. of [RFC2573].
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterTypeSet(L7_char8* snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, l7_snmpNotifyFilterType_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterTypeSet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type of this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterStorageTypeGet(L7_char8* snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, l7_snmpStorageType_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterStorageTypeGet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type of this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterStorageTypeSet(L7_char8* snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, l7_snmpStorageType_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterStorageTypeSet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterRowStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterRowStatusGet(L7_char8* snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, l7_snmpRowStatus_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterRowStatusGet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyFilterRowStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyFilterRowStatusSet(L7_char8* snmpNotifyFilterProfileName, L7_char8 *snmpNotifyFilterSubtree, l7_snmpRowStatus_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyFilterRowStatusSet(snmpNotifyFilterProfileName, snmpNotifyFilterSubtree, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/**********************************************************************************/

/*********************************************************************
*
* @purpose Checks that the row exists for the given instances
*
* @returns  L7_SUCCESS    if the row exists
*           L7_FAILURE    if no row exists
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyTableCheckValid(L7_char8 *snmpNotifyName)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyTableCheckValid(snmpNotifyName);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Retrieves the next valid instance for the table
*
* @returns  L7_SUCCESS    if annother row exists
*           L7_FAILURE    if no more rows exist
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyTableNextGet(L7_char8 *snmpNotifyName)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyTableNextGet(snmpNotifyName);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyTag
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object contains a single tag value which is used
*        to select entries in the snmpTargetAddrTable.  Any entry
*        in the snmpTargetAddrTable which contains a tag value
*        which is equal to the value of an instance of this
*        object is selected.  If this object contains a value
*        of zero length, no entries are selected.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyTagGet(L7_char8* snmpNotifyName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyTagGet(snmpNotifyName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyTag
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object contains a single tag value which is used
*        to select entries in the snmpTargetAddrTable.  Any entry
*        in the snmpTargetAddrTable which contains a tag value
*        which is equal to the value of an instance of this
*        object is selected.  If this object contains a value
*        of zero length, no entries are selected.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyTagSet(L7_char8* snmpNotifyName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyTagSet(snmpNotifyName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object determines the type of notification to
*        be generated for entries in the snmpTargetAddrTable
*        selected by the corresponding instance of
*        snmpNotifyTag.  This value is only used when
*        generating notifications, and is ignored when
*        using the snmpTargetAddrTable for other purposes.
*        
*        If the value of this object is trap(1), then any
*        messages generated for selected rows will contain
*        Unconfirmed-Class PDUs.
*        
*        If the value of this object is inform(2), then any
*        messages generated for selected rows will contain
*        Confirmed-Class PDUs.
*        
*        Note that if an SNMP entity only supports
*        generation of Unconfirmed-Class PDUs (and not
*        Confirmed-Class PDUs), then this object may be
*        read-only.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyTypeGet(L7_char8* snmpNotifyName, l7_snmpNotifyType_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyTypeGet(snmpNotifyName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object determines the type of notification to
*        be generated for entries in the snmpTargetAddrTable
*        selected by the corresponding instance of
*        snmpNotifyTag.  This value is only used when
*        generating notifications, and is ignored when
*        using the snmpTargetAddrTable for other purposes.
*        
*        If the value of this object is trap(1), then any
*        messages generated for selected rows will contain
*        Unconfirmed-Class PDUs.
*        
*        If the value of this object is inform(2), then any
*        messages generated for selected rows will contain
*        Confirmed-Class PDUs.
*        
*        Note that if an SNMP entity only supports
*        generation of Unconfirmed-Class PDUs (and not
*        Confirmed-Class PDUs), then this object may be
*        read-only.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyTypeSet(L7_char8* snmpNotifyName, l7_snmpNotifyType_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyTypeSet(snmpNotifyName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type for this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyStorageTypeGet(L7_char8* snmpNotifyName, l7_snmpStorageType_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyStorageTypeGet(snmpNotifyName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type for this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyStorageTypeSet(L7_char8* snmpNotifyName, l7_snmpStorageType_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyStorageTypeSet(snmpNotifyName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyRowStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyRowStatusGet(L7_char8* snmpNotifyName, l7_snmpRowStatus_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyRowStatusGet(snmpNotifyName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpNotifyRowStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpNotifyRowStatusSet(L7_char8* snmpNotifyName, l7_snmpRowStatus_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpNotifyRowStatusSet(snmpNotifyName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/**********************************************************************************/

/*********************************************************************
*
* @purpose Checks that the row exists for the given instances
*
* @returns  L7_SUCCESS    if the row exists
*           L7_FAILURE    if no row exists
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrTableCheckValid(L7_char8 *snmpTargetAddrName)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrTableCheckValid(snmpTargetAddrName);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Retrieves the next valid instance for the table
*
* @returns  L7_SUCCESS    if annother row exists
*           L7_FAILURE    if no more rows exist
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrTableNextGet(L7_char8 *snmpTargetAddrName)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrTableNextGet(snmpTargetAddrName);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTDomain
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object indicates the transport type of the address
*        contained in the snmpTargetAddrTAddress object.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrTDomainGet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrTDomainGet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTDomain
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object indicates the transport type of the address
*        contained in the snmpTargetAddrTAddress object.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrTDomainSet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrTDomainSet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTAddress
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object contains a transport address.  The format of
*        this address depends on the value of the
*        snmpTargetAddrTDomain object.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrTAddressGet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrTAddressGet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTAddress
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object contains a transport address.  The format of
*        this address depends on the value of the
*        snmpTargetAddrTDomain object.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrTAddressSet(L7_char8* snmpTargetAddrTAddress, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  L7_RC_t rc;

  rc = l7_snmpTargetAddrTAddressSet(snmpTargetAddrTAddress, val);
  if (rc == L7_SUCCESS)
  {
    if (strcmp(val, "0.0.0.0:0") == 0 || strcmp(val, "0.0.0.0:162") == 0)
    {
      (void)l7_snmpTargetAddrTMaskSet(snmpTargetAddrTAddress, "0.0.0.0:0");
    }
    else
    {
      (void)l7_snmpTargetAddrTMaskSet(snmpTargetAddrTAddress, "255.255.255.255:0");
    }
  }
  return rc;
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTimeout
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object should reflect the expected maximum round
*        trip time for communicating with the transport address
*        defined by this row.  When a message is sent to this
*        address, and a response (if one is expected) is not
*        received within this time period, an implementation
*        may assume that the response will not be delivered.
*        
*        Note that the time interval that an application waits
*        for a response may actually be derived from the value
*        of this object.  The method for deriving the actual time
*        interval is implementation dependent.  One such method
*        is to derive the expected round trip time based on a
*        particular retransmission algorithm and on the number
*        of timeouts which have occurred.  The type of message may
*        also be considered when deriving expected round trip
*        times for retransmissions.  For example, if a message is
*        being sent with a securityLevel that indicates both
*        authentication and privacy, the derived value may be
*        increased to compensate for extra processing time spent
*        during authentication and encryption processing.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrTimeoutGet(L7_char8* snmpTargetAddrName, L7_uint32 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrTimeoutGet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTimeout
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object should reflect the expected maximum round
*        trip time for communicating with the transport address
*        defined by this row.  When a message is sent to this
*        address, and a response (if one is expected) is not
*        received within this time period, an implementation
*        may assume that the response will not be delivered.
*        
*        Note that the time interval that an application waits
*        for a response may actually be derived from the value
*        of this object.  The method for deriving the actual time
*        interval is implementation dependent.  One such method
*        is to derive the expected round trip time based on a
*        particular retransmission algorithm and on the number
*        of timeouts which have occurred.  The type of message may
*        also be considered when deriving expected round trip
*        times for retransmissions.  For example, if a message is
*        being sent with a securityLevel that indicates both
*        authentication and privacy, the derived value may be
*        increased to compensate for extra processing time spent
*        during authentication and encryption processing.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrTimeoutSet(L7_char8* snmpTargetAddrName, L7_uint32 val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrTimeoutSet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrRetryCount
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object specifies a default number of retries to be
*        attempted when a response is not received for a generated
*        message.  An application may provide its own retry count,
*        in which case the value of this object is ignored.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrRetryCountGet(L7_char8* snmpTargetAddrName, L7_uint32 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrRetryCountGet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrRetryCount
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object specifies a default number of retries to be
*        attempted when a response is not received for a generated
*        message.  An application may provide its own retry count,
*        in which case the value of this object is ignored.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrRetryCountSet(L7_char8* snmpTargetAddrName, L7_uint32 val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrRetryCountSet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTagList
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object contains a list of tag values which are
*        used to select target addresses for a particular
*        operation.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrTagListGet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrTagListGet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTagList
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object contains a list of tag values which are
*        used to select target addresses for a particular
*        operation.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrTagListSet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrTagListSet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrParams
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The value of this object identifies an entry in the
*        snmpTargetParamsTable.  The identified entry
*        contains SNMP parameters to be used when generating
*        messages to be sent to this transport address.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrParamsGet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrParamsGet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrParams
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The value of this object identifies an entry in the
*        snmpTargetParamsTable.  The identified entry
*        contains SNMP parameters to be used when generating
*        messages to be sent to this transport address.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrParamsSet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrParamsSet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type for this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrStorageTypeGet(L7_char8* snmpTargetAddrName, l7_snmpStorageType_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrStorageTypeGet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type for this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrStorageTypeSet(L7_char8* snmpTargetAddrName, l7_snmpStorageType_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrStorageTypeSet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrRowStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the snmpTargetAddrRowStatus
*        column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until the corresponding instances of
*        snmpTargetAddrTDomain, snmpTargetAddrTAddress, and
*        snmpTargetAddrParams have all been set.
*        
*        The following objects may not be modified while the
*        value of this object is active(1):
*        An attempt to set these objects while the value of
*        snmpTargetAddrRowStatus is active(1) will result in
*        an inconsistentValue error.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrRowStatusGet(L7_char8* snmpTargetAddrName, l7_snmpRowStatus_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrRowStatusGet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrRowStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the snmpTargetAddrRowStatus
*        column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until the corresponding instances of
*        snmpTargetAddrTDomain, snmpTargetAddrTAddress, and
*        snmpTargetAddrParams have all been set.
*        
*        The following objects may not be modified while the
*        value of this object is active(1):
*        An attempt to set these objects while the value of
*        snmpTargetAddrRowStatus is active(1) will result in
*        an inconsistentValue error.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetAddrRowStatusSet(L7_char8* snmpTargetAddrName, l7_snmpRowStatus_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrRowStatusSet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTMask
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes This object contains a transport address.  The format of
*        this address depends on the value of the
*        snmpTargetAddrTDomain object.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpTargetAddrTMaskGet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrTMaskGet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetAddrTMask
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes This object contains a transport address.  The format of
*        this address depends on the value of the
*        snmpTargetAddrTDomain object.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpTargetAddrTMaskSet(L7_char8* snmpTargetAddrName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetAddrTMaskSet(snmpTargetAddrName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/**********************************************************************************/

/*********************************************************************
*
* @purpose Checks that the row exists for the given instances
*
* @returns  L7_SUCCESS    if the row exists
*           L7_FAILURE    if no row exists
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetParamsTableCheckValid(L7_char8 *snmpTargetParamsName)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetParamsTableCheckValid(snmpTargetParamsName);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Retrieves the next valid instance for the table
*
* @returns  L7_SUCCESS    if annother row exists
*           L7_FAILURE    if no more rows exist
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetParamsTableNextGet(L7_char8 *snmpTargetParamsName)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetParamsTableNextGet(snmpTargetParamsName);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsMPModel
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The Message Processing Model to be used when generating
*        SNMP messages using this entry.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetParamsMPModelGet(L7_char8* snmpTargetParamsName, L7_uint32 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetParamsMPModelGet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsMPModel
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The Message Processing Model to be used when generating
*        SNMP messages using this entry.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetParamsMPModelSet(L7_char8* snmpTargetParamsName, L7_uint32 val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetParamsMPModelSet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsSecurityModel
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The Security Model to be used when generating SNMP
*         messages using this entry.  An implementation may
*         choose to return an inconsistentValue error if an
*         attempt is made to set this variable to a value
*         for a security model which the implementation does
*         not support.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetParamsSecurityModelGet(L7_char8* snmpTargetParamsName, L7_uint32 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetParamsSecurityModelGet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsSecurityModel
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The Security Model to be used when generating SNMP
*         messages using this entry.  An implementation may
*         choose to return an inconsistentValue error if an
*         attempt is made to set this variable to a value
*         for a security model which the implementation does
*         not support.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetParamsSecurityModelSet(L7_char8* snmpTargetParamsName, L7_uint32 val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetParamsSecurityModelSet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsSecurityName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The securityName which identifies the Principal on
*        whose behalf SNMP messages will be generated using
*        this entry.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetParamsSecurityNameGet(L7_char8* snmpTargetParamsName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetParamsSecurityNameGet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsSecurityName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The securityName which identifies the Principal on
*        whose behalf SNMP messages will be generated using
*        this entry.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetParamsSecurityNameSet(L7_char8* snmpTargetParamsName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetParamsSecurityNameSet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsSecurityLevel
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The Level of Security to be used when generating
*        SNMP messages using this entry.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetParamsSecurityLevelGet(L7_char8* snmpTargetParamsName, L7_uint32 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetParamsSecurityLevelGet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsSecurityLevel
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The Level of Security to be used when generating
*        SNMP messages using this entry.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetParamsSecurityLevelSet(L7_char8* snmpTargetParamsName, L7_uint32 val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetParamsSecurityLevelSet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type for this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetParamsStorageTypeGet(L7_char8* snmpTargetParamsName, l7_snmpStorageType_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetParamsStorageTypeGet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type for this conceptual row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetParamsStorageTypeSet(L7_char8* snmpTargetParamsName, l7_snmpStorageType_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetParamsStorageTypeSet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsRowStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the snmpTargetParamsRowStatus
*        column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until the corresponding
*        snmpTargetParamsMPModel,
*        snmpTargetParamsSecurityModel,
*        snmpTargetParamsSecurityName,
*        and snmpTargetParamsSecurityLevel have all been set.
*        The following objects may not be modified while the
*        value of this object is active(1):
*        An attempt to set these objects while the value of
*        snmpTargetParamsRowStatus is active(1) will result in
*        an inconsistentValue error.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetParamsRowStatusGet(L7_char8* snmpTargetParamsName, l7_snmpRowStatus_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetParamsRowStatusGet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object snmpTargetParamsRowStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        
*        To create a row in this table, a manager must
*        set this object to either createAndGo(4) or
*        createAndWait(5).
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the snmpTargetParamsRowStatus
*        column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until the corresponding
*        snmpTargetParamsMPModel,
*        snmpTargetParamsSecurityModel,
*        snmpTargetParamsSecurityName,
*        and snmpTargetParamsSecurityLevel have all been set.
*        The following objects may not be modified while the
*        value of this object is active(1):
*        An attempt to set these objects while the value of
*        snmpTargetParamsRowStatus is active(1) will result in
*        an inconsistentValue error.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiSnmpTargetParamsRowStatusSet(L7_char8* snmpTargetParamsName, l7_snmpRowStatus_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpTargetParamsRowStatusSet(snmpTargetParamsName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/**********************************************************************************/

/*********************************************************************
*
* @purpose Checks that the row exists for the given instances
*
* @returns  L7_SUCCESS    if the row exists
*           L7_FAILURE    if no row exists
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserTableCheckValid(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserTableCheckValid(usmUserEngineID, usmUserEngineID_len, usmUserName);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Retrieves the next valid instance for the table
*
* @returns  L7_SUCCESS    if annother row exists
*           L7_FAILURE    if no more rows exist
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserTableNextGet(L7_char8 *usmUserEngineID, L7_uint32 *usmUserEngineID_len, L7_char8 *usmUserName)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserTableNextGet(usmUserEngineID, usmUserEngineID_len, usmUserName);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserSecurityName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes A human readable string representing the user in
*        Security Model independent format.
*        
*        The default transformation of the User-based Security
*        Model dependent security ID to the securityName and
*        vice versa is the identity function so that the
*        securityName is the same as the userName.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserSecurityNameGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserSecurityNameGet(usmUserEngineID, usmUserEngineID_len, usmUserName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserSecurityName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes A human readable string representing the user in
*        Security Model independent format.
*        
*        The default transformation of the User-based Security
*        Model dependent security ID to the securityName and
*        vice versa is the identity function so that the
*        securityName is the same as the userName.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserSecurityNameSet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserSecurityNameSet(usmUserEngineID, usmUserEngineID_len, usmUserName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserAuthProtocol
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be authenticated, and if so,
*        the type of authentication protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoAuthProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoAuthProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        If a set operation tries to set the value to the
*        usmNoAuthProtocol while the usmUserPrivProtocol value
*        in the same row is not equal to usmNoPrivProtocol,
*        then an 'inconsistentValue' error must be returned.
*        That means that an SNMP command generator application
*        must first ensure that the usmUserPrivProtocol is set
*        to the usmNoPrivProtocol value before it can set
*        the usmUserAuthProtocol value to usmNoAuthProtocol.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserAuthProtocolGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserAuthProtocol_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserAuthProtocolGet(usmUserEngineID, usmUserEngineID_len, usmUserName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Sets the value of the object usmUserAuthProtocol
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be authenticated, and if so,
*        the type of authentication protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoAuthProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoAuthProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        If a set operation tries to set the value to the
*        usmNoAuthProtocol while the usmUserPrivProtocol value
*        in the same row is not equal to usmNoPrivProtocol,
*        then an 'inconsistentValue' error must be returned.
*        That means that an SNMP command generator application
*        must first ensure that the usmUserPrivProtocol is set
*        to the usmNoPrivProtocol value before it can set
*        the usmUserAuthProtocol value to usmNoAuthProtocol.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserAuthProtocolSet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserAuthProtocol_t val, L7_char8 *pass)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserAuthProtocolSet(usmUserEngineID, usmUserEngineID_len, usmUserName, val, pass);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserAuthProtocol 
*          plus the generated key
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be authenticated, and if so,
*        the type of authentication protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoAuthProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoAuthProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        If a set operation tries to set the value to the
*        usmNoAuthProtocol while the usmUserPrivProtocol value
*        in the same row is not equal to usmNoPrivProtocol,
*        then an 'inconsistentValue' error must be returned.
*        That means that an SNMP command generator application
*        must first ensure that the usmUserPrivProtocol is set
*        to the usmNoPrivProtocol value before it can set
*        the usmUserAuthProtocol value to usmNoAuthProtocol.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserAuthProtocolKeyGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserAuthProtocol_t *val, L7_char8 *key, L7_uint32 *key_len)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserAuthProtocolKeyGet(usmUserEngineID, usmUserEngineID_len, usmUserName, val, key, key_len);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Sets the value of the object usmUserAuthProtocol 
*          plus the pregenerated key
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be authenticated, and if so,
*        the type of authentication protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoAuthProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoAuthProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        If a set operation tries to set the value to the
*        usmNoAuthProtocol while the usmUserPrivProtocol value
*        in the same row is not equal to usmNoPrivProtocol,
*        then an 'inconsistentValue' error must be returned.
*        That means that an SNMP command generator application
*        must first ensure that the usmUserPrivProtocol is set
*        to the usmNoPrivProtocol value before it can set
*        the usmUserAuthProtocol value to usmNoAuthProtocol.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserAuthProtocolKeySet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserAuthProtocol_t val, L7_char8 *key, L7_uint32 key_len)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserAuthProtocolKeySet(usmUserEngineID, usmUserEngineID_len, usmUserName, val, key, key_len);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserPrivProtocol
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be protected from disclosure,
*        and if so, the type of privacy protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoPrivProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoPrivProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        Note that if any privacy protocol is used, then you
*        must also use an authentication protocol. In other
*        words, if usmUserPrivProtocol is set to anything else
*        than usmNoPrivProtocol, then the corresponding instance
*        of usmUserAuthProtocol cannot have a value of
*        usmNoAuthProtocol. If it does, then an
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserPrivProtocolGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserPrivProtocol_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserPrivProtocolGet(usmUserEngineID, usmUserEngineID_len, usmUserName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserPrivProtocol
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be protected from disclosure,
*        and if so, the type of privacy protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoPrivProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoPrivProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        Note that if any privacy protocol is used, then you
*        must also use an authentication protocol. In other
*        words, if usmUserPrivProtocol is set to anything else
*        than usmNoPrivProtocol, then the corresponding instance
*        of usmUserAuthProtocol cannot have a value of
*        usmNoAuthProtocol. If it does, then an
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserPrivProtocolSet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserPrivProtocol_t val, L7_char8 *pass)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserPrivProtocolSet(usmUserEngineID, usmUserEngineID_len, usmUserName, val, pass);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserPrivProtocol
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be protected from disclosure,
*        and if so, the type of privacy protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoPrivProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoPrivProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        Note that if any privacy protocol is used, then you
*        must also use an authentication protocol. In other
*        words, if usmUserPrivProtocol is set to anything else
*        than usmNoPrivProtocol, then the corresponding instance
*        of usmUserAuthProtocol cannot have a value of
*        usmNoAuthProtocol. If it does, then an
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserPrivProtocolKeyGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserPrivProtocol_t *val, L7_char8 *key, L7_uint32 *key_len)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserPrivProtocolKeyGet(usmUserEngineID, usmUserEngineID_len, usmUserName, val, key, key_len);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserPrivProtocol
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes An indication of whether messages sent on behalf of
*        this user to/from the SNMP engine identified by
*        usmUserEngineID, can be protected from disclosure,
*        and if so, the type of privacy protocol which is used.
*        
*        An instance of this object is created concurrently
*        with the creation of any other object instance for
*        the same user (i.e., as part of the processing of
*        the set operation which creates the first object
*        instance in the same conceptual row).
*        
*        If an initial set operation (i.e. at row creation time)
*        tries to set a value for an unknown or unsupported
*        protocol, then a 'wrongValue' error must be returned.
*        
*        The value will be overwritten/set when a set operation
*        is performed on the corresponding instance of
*        usmUserCloneFrom.
*        
*        Once instantiated, the value of such an instance of
*        this object can only be changed via a set operation to
*        the value of the usmNoPrivProtocol.
*        
*        If a set operation tries to change the value of an
*        existing instance of this object to any value other
*        than usmNoPrivProtocol, then an 'inconsistentValue'
*        error must be returned.
*        
*        Note that if any privacy protocol is used, then you
*        must also use an authentication protocol. In other
*        words, if usmUserPrivProtocol is set to anything else
*        than usmNoPrivProtocol, then the corresponding instance
*        of usmUserAuthProtocol cannot have a value of
*        usmNoAuthProtocol. If it does, then an
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserPrivProtocolKeySet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_usmUserPrivProtocol_t val, L7_char8 *key, L7_uint32 key_len)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserPrivProtocolKeySet(usmUserEngineID, usmUserEngineID_len, usmUserName, val, key, key_len);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type for this conceptual row.
*        
*        Conceptual rows having the value 'permanent' must
*        allow write-access at a minimum to:
*        
*          and usmUserPublic for a user who employs
*          authentication, and
*          and usmUserPublic for a user who employs
*          privacy.
*        
*        Note that any user who employs authentication or
*        privacy must allow its secret(s) to be updated and
*        thus cannot be 'readOnly'.
*        
*        If an initial set operation tries to set the value to
*        privacy, then an 'inconsistentValue' error must be
*        returned.  Note that if the value has been previously
*        set (implicit or explicit) to any value, then the rules
*        as defined in the StorageType Textual Convention apply.
*        
*        It is an implementation issue to decide if a SET for
*        a readOnly or permanent row is accepted at all. In some
*        contexts this may make sense, in others it may not. If
*        a SET for a readOnly or permanent row is not accepted
*        at all, then a 'wrongValue' error must be returned.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserStorageTypeGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_snmpStorageType_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserStorageTypeGet(usmUserEngineID, usmUserEngineID_len, usmUserName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type for this conceptual row.
*        
*        Conceptual rows having the value 'permanent' must
*        allow write-access at a minimum to:
*        
*          and usmUserPublic for a user who employs
*          authentication, and
*          and usmUserPublic for a user who employs
*          privacy.
*        
*        Note that any user who employs authentication or
*        privacy must allow its secret(s) to be updated and
*        thus cannot be 'readOnly'.
*        
*        If an initial set operation tries to set the value to
*        privacy, then an 'inconsistentValue' error must be
*        returned.  Note that if the value has been previously
*        set (implicit or explicit) to any value, then the rules
*        as defined in the StorageType Textual Convention apply.
*        
*        It is an implementation issue to decide if a SET for
*        a readOnly or permanent row is accepted at all. In some
*        contexts this may make sense, in others it may not. If
*        a SET for a readOnly or permanent row is not accepted
*        at all, then a 'wrongValue' error must be returned.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserStorageTypeSet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_snmpStorageType_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserStorageTypeSet(usmUserEngineID, usmUserEngineID_len, usmUserName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the usmUserStatus column
*        is 'notReady'.
*        
*        In particular, a newly created row for a user who
*        employs authentication, cannot be made active until the
*        corresponding usmUserCloneFrom and usmUserAuthKeyChange
*        have been set.
*        
*        Further, a newly created row for a user who also
*        employs privacy, cannot be made active until the
*        usmUserPrivKeyChange has been set.
*        
*        The RowStatus TC [RFC2579] requires that this
*        DESCRIPTION clause states under which circumstances
*        other objects in this row can be modified:
*        
*        The value of this object has no effect on whether
*        other objects in this conceptual row can be modified,
*        except for usmUserOwnAuthKeyChange and
*        usmUserOwnPrivKeyChange. For these 2 objects, the
*        value of usmUserStatus MUST be active.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserStatusGet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_snmpRowStatus_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserStatusGet(usmUserEngineID, usmUserEngineID_len, usmUserName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object usmUserStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the usmUserStatus column
*        is 'notReady'.
*        
*        In particular, a newly created row for a user who
*        employs authentication, cannot be made active until the
*        corresponding usmUserCloneFrom and usmUserAuthKeyChange
*        have been set.
*        
*        Further, a newly created row for a user who also
*        employs privacy, cannot be made active until the
*        usmUserPrivKeyChange has been set.
*        
*        The RowStatus TC [RFC2579] requires that this
*        DESCRIPTION clause states under which circumstances
*        other objects in this row can be modified:
*        
*        The value of this object has no effect on whether
*        other objects in this conceptual row can be modified,
*        except for usmUserOwnAuthKeyChange and
*        usmUserOwnPrivKeyChange. For these 2 objects, the
*        value of usmUserStatus MUST be active.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiUsmUserStatusSet(L7_char8 *usmUserEngineID, L7_uint32 usmUserEngineID_len, L7_char8 *usmUserName, l7_snmpRowStatus_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_usmUserStatusSet(usmUserEngineID, usmUserEngineID_len, usmUserName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/**********************************************************************************/

/*********************************************************************
*
* @purpose Checks that the row exists for the given instances
*
* @returns  L7_SUCCESS    if the row exists
*           L7_FAILURE    if no row exists
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmAccessTableCheckValid(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, L7_uint32 vacmAccessSecurityModel, L7_uint32 vacmAccessSecurityLevel)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmAccessTableCheckValid(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Retrieves the next valid instance for the table
*
* @returns  L7_SUCCESS    if annother row exists
*           L7_FAILURE    if no more rows exist
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmAccessTableNextGet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, L7_uint32 *vacmAccessSecurityModel, L7_uint32 *vacmAccessSecurityLevel)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmAccessTableNextGet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessContextMatch
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes If the value of this object is exact(1), then all
*        rows where the contextName exactly matches
*        vacmAccessContextPrefix are selected.
*        
*        If the value of this object is prefix(2), then all
*        rows where the contextName whose starting octets
*        exactly match vacmAccessContextPrefix are selected.
*        This allows for a simple form of wildcarding.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmAccessContextMatchGet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, L7_uint32 vacmAccessSecurityModel, L7_uint32 vacmAccessSecurityLevel, l7_vacmAccessContextMatch_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmAccessContextMatchGet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessContextMatch
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes If the value of this object is exact(1), then all
*        rows where the contextName exactly matches
*        vacmAccessContextPrefix are selected.
*        
*        If the value of this object is prefix(2), then all
*        rows where the contextName whose starting octets
*        exactly match vacmAccessContextPrefix are selected.
*        This allows for a simple form of wildcarding.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmAccessContextMatchSet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, L7_uint32 vacmAccessSecurityModel, L7_uint32 vacmAccessSecurityLevel, l7_vacmAccessContextMatch_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmAccessContextMatchSet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessReadViewName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The value of an instance of this object identifies
*        the MIB view of the SNMP context to which this
*        conceptual row authorizes read access.
*        
*        The identified MIB view is that one for which the
*        vacmViewTreeFamilyViewName has the same value as the
*        instance of this object; if the value is the empty
*        string or if there is no active MIB view having this
*        value of vacmViewTreeFamilyViewName, then no access
*        is granted.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmAccessReadViewNameGet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, L7_uint32 vacmAccessSecurityModel, L7_uint32 vacmAccessSecurityLevel, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmAccessReadViewNameGet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessReadViewName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The value of an instance of this object identifies
*        the MIB view of the SNMP context to which this
*        conceptual row authorizes read access.
*        
*        The identified MIB view is that one for which the
*        vacmViewTreeFamilyViewName has the same value as the
*        instance of this object; if the value is the empty
*        string or if there is no active MIB view having this
*        value of vacmViewTreeFamilyViewName, then no access
*        is granted.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmAccessReadViewNameSet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, L7_uint32 vacmAccessSecurityModel, L7_uint32 vacmAccessSecurityLevel, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmAccessReadViewNameSet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessWriteViewName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The value of an instance of this object identifies
*        the MIB view of the SNMP context to which this
*        conceptual row authorizes write access.
*        
*        The identified MIB view is that one for which the
*        vacmViewTreeFamilyViewName has the same value as the
*        instance of this object; if the value is the empty
*        string or if there is no active MIB view having this
*        value of vacmViewTreeFamilyViewName, then no access
*        is granted.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmAccessWriteViewNameGet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, L7_uint32 vacmAccessSecurityModel, L7_uint32 vacmAccessSecurityLevel, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmAccessWriteViewNameGet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessWriteViewName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The value of an instance of this object identifies
*        the MIB view of the SNMP context to which this
*        conceptual row authorizes write access.
*        
*        The identified MIB view is that one for which the
*        vacmViewTreeFamilyViewName has the same value as the
*        instance of this object; if the value is the empty
*        string or if there is no active MIB view having this
*        value of vacmViewTreeFamilyViewName, then no access
*        is granted.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmAccessWriteViewNameSet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, L7_uint32 vacmAccessSecurityModel, L7_uint32 vacmAccessSecurityLevel, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmAccessWriteViewNameSet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessNotifyViewName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The value of an instance of this object identifies
*        the MIB view of the SNMP context to which this
*        conceptual row authorizes access for notifications.
*        
*        The identified MIB view is that one for which the
*        vacmViewTreeFamilyViewName has the same value as the
*        instance of this object; if the value is the empty
*        string or if there is no active MIB view having this
*        value of vacmViewTreeFamilyViewName, then no access
*        is granted.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmAccessNotifyViewNameGet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, L7_uint32 vacmAccessSecurityModel, L7_uint32 vacmAccessSecurityLevel, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmAccessNotifyViewNameGet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessNotifyViewName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The value of an instance of this object identifies
*        the MIB view of the SNMP context to which this
*        conceptual row authorizes access for notifications.
*        
*        The identified MIB view is that one for which the
*        vacmViewTreeFamilyViewName has the same value as the
*        instance of this object; if the value is the empty
*        string or if there is no active MIB view having this
*        value of vacmViewTreeFamilyViewName, then no access
*        is granted.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmAccessNotifyViewNameSet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, L7_uint32 vacmAccessSecurityModel, L7_uint32 vacmAccessSecurityLevel, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmAccessNotifyViewNameSet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type for this conceptual row.
*        
*        Conceptual rows having the value 'permanent' need not
*        allow write-access to any columnar objects in the row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmAccessStorageTypeGet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, L7_uint32 vacmAccessSecurityModel, L7_uint32 vacmAccessSecurityLevel, l7_snmpStorageType_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmAccessStorageTypeGet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type for this conceptual row.
*        
*        Conceptual rows having the value 'permanent' need not
*        allow write-access to any columnar objects in the row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmAccessStorageTypeSet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, L7_uint32 vacmAccessSecurityModel, L7_uint32 vacmAccessSecurityLevel, l7_snmpStorageType_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmAccessStorageTypeSet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        The  RowStatus TC [RFC2579] requires that this
*        DESCRIPTION clause states under which circumstances
*        other objects in this row can be modified:
*        
*        The value of this object has no effect on whether
*        other objects in this conceptual row can be modified.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmAccessStatusGet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, L7_uint32 vacmAccessSecurityModel, L7_uint32 vacmAccessSecurityLevel, l7_snmpRowStatus_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmAccessStatusGet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmAccessStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        The  RowStatus TC [RFC2579] requires that this
*        DESCRIPTION clause states under which circumstances
*        other objects in this row can be modified:
*        
*        The value of this object has no effect on whether
*        other objects in this conceptual row can be modified.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmAccessStatusSet(L7_char8 *vacmGroupName, L7_char8 *vacmAccessContextPrefix, L7_uint32 vacmAccessSecurityModel, L7_uint32 vacmAccessSecurityLevel, l7_snmpRowStatus_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmAccessStatusSet(vacmGroupName, vacmAccessContextPrefix, vacmAccessSecurityModel, vacmAccessSecurityLevel, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/**********************************************************************************/

/*********************************************************************
*
* @purpose Checks that the row exists for the given instances
*
* @returns  L7_SUCCESS    if the row exists
*           L7_FAILURE    if no row exists
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmSecurityToGroupTableCheckValid(L7_uint32 vacmSecurityModel, L7_char8 *vacmSecurityName)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmSecurityToGroupTableCheckValid(vacmSecurityModel, vacmSecurityName);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Retrieves the next valid instance for the table
*
* @returns  L7_SUCCESS    if annother row exists
*           L7_FAILURE    if no more rows exist
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmSecurityToGroupTableNextGet(L7_uint32 *vacmSecurityModel, L7_char8 *vacmSecurityName)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmSecurityToGroupTableNextGet(vacmSecurityModel, vacmSecurityName);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmGroupName
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The name of the group to which this entry (e.g., the
*        combination of securityModel and securityName)
*        belongs.
*        
*        This groupName is used as index into the
*        vacmAccessTable to select an access control policy.
*        However, a value in this table does not imply that an
*        instance with the value exists in table vacmAccesTable.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmGroupNameGet(L7_uint32 vacmSecurityModel, L7_char8 *vacmSecurityName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmGroupNameGet(vacmSecurityModel, vacmSecurityName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmGroupName
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The name of the group to which this entry (e.g., the
*        combination of securityModel and securityName)
*        belongs.
*        
*        This groupName is used as index into the
*        vacmAccessTable to select an access control policy.
*        However, a value in this table does not imply that an
*        instance with the value exists in table vacmAccesTable.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmGroupNameSet(L7_uint32 vacmSecurityModel, L7_char8 *vacmSecurityName, L7_char8 *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmGroupNameSet(vacmSecurityModel, vacmSecurityName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmSecurityToGroupStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type for this conceptual row.
*        Conceptual rows having the value 'permanent' need not
*        allow write-access to any columnar objects in the row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmSecurityToGroupStorageTypeGet(L7_uint32 vacmSecurityModel, L7_char8 *vacmSecurityName, l7_snmpStorageType_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmSecurityToGroupStorageTypeGet(vacmSecurityModel, vacmSecurityName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmSecurityToGroupStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type for this conceptual row.
*        Conceptual rows having the value 'permanent' need not
*        allow write-access to any columnar objects in the row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmSecurityToGroupStorageTypeSet(L7_uint32 vacmSecurityModel, L7_char8 *vacmSecurityName, l7_snmpStorageType_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmSecurityToGroupStorageTypeSet(vacmSecurityModel, vacmSecurityName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmSecurityToGroupStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the vacmSecurityToGroupStatus
*        column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until a value has been set for vacmGroupName.
*        
*        The  RowStatus TC [RFC2579] requires that this
*        DESCRIPTION clause states under which circumstances
*        other objects in this row can be modified:
*        
*        The value of this object has no effect on whether
*        other objects in this conceptual row can be modified.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmSecurityToGroupStatusGet(L7_uint32 vacmSecurityModel, L7_char8 *vacmSecurityName, l7_snmpRowStatus_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmSecurityToGroupStatusGet(vacmSecurityModel, vacmSecurityName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmSecurityToGroupStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        
*        Until instances of all corresponding columns are
*        appropriately configured, the value of the
*        corresponding instance of the vacmSecurityToGroupStatus
*        column is 'notReady'.
*        
*        In particular, a newly created row cannot be made
*        active until a value has been set for vacmGroupName.
*        
*        The  RowStatus TC [RFC2579] requires that this
*        DESCRIPTION clause states under which circumstances
*        other objects in this row can be modified:
*        
*        The value of this object has no effect on whether
*        other objects in this conceptual row can be modified.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmSecurityToGroupStatusSet(L7_uint32 vacmSecurityModel, L7_char8 *vacmSecurityName, l7_snmpRowStatus_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmSecurityToGroupStatusSet(vacmSecurityModel, vacmSecurityName, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/**********************************************************************************/

/*********************************************************************
*
* @purpose Checks that the row exists for the given instances
*
* @returns  L7_SUCCESS    if the row exists
*           L7_FAILURE    if no row exists
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmViewTreeFamilyTableCheckValid(L7_char8 *vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmViewTreeFamilyTableCheckValid(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Retrieves the next valid instance for the table
*
* @returns  L7_SUCCESS    if annother row exists
*           L7_FAILURE    if no more rows exist
*           
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmViewTreeFamilyTableNextGet(L7_char8 *vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmViewTreeFamilyTableNextGet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmViewTreeFamilyMask
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The bit mask which, in combination with the
*        corresponding instance of vacmViewTreeFamilySubtree,
*        defines a family of view subtrees.
*        
*        Each bit of this bit mask corresponds to a
*        sub-identifier of vacmViewTreeFamilySubtree, with the
*        most significant bit of the i-th octet of this octet
*        string value (extended if necessary, see below)
*        corresponding to the (8*i - 7)-th sub-identifier, and
*        the least significant bit of the i-th octet of this
*        octet string corresponding to the (8*i)-th
*        sub-identifier, where i is in the range 1 through 16.
*        
*        Each bit of this bit mask specifies whether or not
*        the corresponding sub-identifiers must match when
*        determining if an OBJECT IDENTIFIER is in this
*        family of view subtrees; a '1' indicates that an
*        exact match must occur; a '0' indicates 'wild card',
*        i.e., any sub-identifier value matches.
*        
*        Thus, the OBJECT IDENTIFIER X of an object instance
*        is contained in a family of view subtrees if, for
*        each sub-identifier of the value of
*        vacmViewTreeFamilySubtree, either:
*        
*          the i-th bit of vacmViewTreeFamilyMask is 0, or
*        
*          the i-th sub-identifier of X is equal to the i-th
*          sub-identifier of the value of
*          vacmViewTreeFamilySubtree.
*        
*        If the value of this bit mask is M bits long and
*        there are more than M sub-identifiers in the
*        corresponding instance of vacmViewTreeFamilySubtree,
*        then the bit mask is extended with 1's to be the
*        required length.
*        
*        Note that when the value of this object is the
*        zero-length string, this extension rule results in
*        a mask of all-1's being used (i.e., no 'wild card'),
*        and the family of view subtrees is the one view
*        subtree uniquely identified by the corresponding
*        instance of vacmViewTreeFamilySubtree.
*        
*        Note that masks of length greater than zero length
*        do not need to be supported. In this case this
*        object is made read-only.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmViewTreeFamilyMaskGet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, L7_char8 *val, L7_uint32 *len)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmViewTreeFamilyMaskGet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, val, len);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmViewTreeFamilyMask
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The bit mask which, in combination with the
*        corresponding instance of vacmViewTreeFamilySubtree,
*        defines a family of view subtrees.
*        
*        Each bit of this bit mask corresponds to a
*        sub-identifier of vacmViewTreeFamilySubtree, with the
*        most significant bit of the i-th octet of this octet
*        string value (extended if necessary, see below)
*        corresponding to the (8*i - 7)-th sub-identifier, and
*        the least significant bit of the i-th octet of this
*        octet string corresponding to the (8*i)-th
*        sub-identifier, where i is in the range 1 through 16.
*        
*        Each bit of this bit mask specifies whether or not
*        the corresponding sub-identifiers must match when
*        determining if an OBJECT IDENTIFIER is in this
*        family of view subtrees; a '1' indicates that an
*        exact match must occur; a '0' indicates 'wild card',
*        i.e., any sub-identifier value matches.
*        
*        Thus, the OBJECT IDENTIFIER X of an object instance
*        is contained in a family of view subtrees if, for
*        each sub-identifier of the value of
*        vacmViewTreeFamilySubtree, either:
*        
*          the i-th bit of vacmViewTreeFamilyMask is 0, or
*        
*          the i-th sub-identifier of X is equal to the i-th
*          sub-identifier of the value of
*          vacmViewTreeFamilySubtree.
*        
*        If the value of this bit mask is M bits long and
*        there are more than M sub-identifiers in the
*        corresponding instance of vacmViewTreeFamilySubtree,
*        then the bit mask is extended with 1's to be the
*        required length.
*        
*        Note that when the value of this object is the
*        zero-length string, this extension rule results in
*        a mask of all-1's being used (i.e., no 'wild card'),
*        and the family of view subtrees is the one view
*        subtree uniquely identified by the corresponding
*        instance of vacmViewTreeFamilySubtree.
*        
*        Note that masks of length greater than zero length
*        do not need to be supported. In this case this
*        object is made read-only.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmViewTreeFamilyMaskSet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, L7_char8 *val, L7_uint32 len)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmViewTreeFamilyMaskSet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, val, len);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmViewTreeFamilyStorageType
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The storage type for this conceptual row.
*        
*        Conceptual rows having the value 'permanent' need not
*        allow write-access to any columnar objects in the row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmViewTreeFamilyStorageTypeGet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, l7_snmpStorageType_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmViewTreeFamilyStorageTypeGet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmViewTreeFamilyStorageType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The storage type for this conceptual row.
*        
*        Conceptual rows having the value 'permanent' need not
*        allow write-access to any columnar objects in the row.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmViewTreeFamilyStorageTypeSet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, l7_snmpStorageType_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmViewTreeFamilyStorageTypeSet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmViewTreeFamilyType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes Indicates whether the corresponding instances of
*        vacmViewTreeFamilySubtree and vacmViewTreeFamilyMask
*        define a family of view subtrees which is included in
*        or excluded from the MIB view.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmViewTreeFamilyTypeGet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, l7_vacmViewTreeFamilyType_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmViewTreeFamilyTypeGet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}


/*********************************************************************
*
* @purpose Sets the value of the object vacmViewTreeFamilyType
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes Indicates whether the corresponding instances of
*        vacmViewTreeFamilySubtree and vacmViewTreeFamilyMask
*        define a family of view subtrees which is included in
*        or excluded from the MIB view.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmViewTreeFamilyTypeSet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, l7_vacmViewTreeFamilyType_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmViewTreeFamilyTypeSet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmViewTreeFamilyStatus
*
* @returns  L7_SUCCESS    if the object was retrieved successfully
*           L7_FAILURE    if the instance doesn't exist
*
* @notes The status of this conceptual row.
*        
*        The  RowStatus TC [RFC2579] requires that this
*        DESCRIPTION clause states under which circumstances
*        other objects in this row can be modified:
*        
*        The value of this object has no effect on whether
*        other objects in this conceptual row can be modified.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmViewTreeFamilyStatusGet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, l7_snmpRowStatus_t *val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmViewTreeFamilyStatusGet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Gets the value of the object vacmViewTreeFamilyStatus
*
* @returns  L7_SUCCESS    if the value was set successfully
*           L7_FAILURE    if the set request was invalid
*           L7_ERROR      if there was a problem in the code
*
* @notes The status of this conceptual row.
*        
*        The  RowStatus TC [RFC2579] requires that this
*        DESCRIPTION clause states under which circumstances
*        other objects in this row can be modified:
*        
*        The value of this object has no effect on whether
*        other objects in this conceptual row can be modified.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiVacmViewTreeFamilyStatusSet(L7_char8* vacmViewTreeFamilyViewName, L7_char8 *vacmViewTreeFamilySubtree, l7_snmpRowStatus_t val)
{
#ifdef L7_SNMP_PACKAGE
  return l7_vacmViewTreeFamilyStatusSet(vacmViewTreeFamilyViewName, vacmViewTreeFamilySubtree, val);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/**********************************************************************************/

L7_RC_t usmDbSnmpConfapiSnmpServerLocalEngineIDSet(L7_char8* engineId, L7_uint32 len, L7_uint32 alg)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpEngineIDSet(engineId,len,alg);
#else
  return L7_NOT_SUPPORTED;
#endif
}
L7_RC_t usmDbSnmpConfapiSnmpServerLocalEngineIDGet(L7_char8* engineId, L7_uint32 *len, L7_uint32 *alg)
{
#ifdef L7_SNMP_PACKAGE
  return l7_snmpEngineIDGet(engineId,len,alg);
#else
  return L7_NOT_SUPPORTED;
#endif
}


/*********************************************************************
*
* @purpose Converts a dotted OID into a readable string
*
* @returns  L7_SUCCESS    if the oid was valid
*           L7_FAILURE    if the oid was invalid
*
* @notes   This converts dotted notation OIDs into strings that
*          are human readable.  
*
*          Example:
*          1.3.6.1.2.1.1.1.0 -> sysDescr.0
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiObjectFromOID(L7_char8* oid, L7_char8* object)
{
#ifdef L7_SNMP_PACKAGE
  return l7_ObjectFromOID(oid, object);
#else
  return L7_NOT_SUPPORTED;
#endif
}

/*********************************************************************
*
* @purpose Converts a readable OID into a dotted OID
*
* @returns  L7_SUCCESS    if the oid was valid
*           L7_FAILURE    if the oid was invalid
*
* @notes   This converts a human readable OID into dotted notation.  
*
*          Example:
*          sysDescr.0 -> 1.3.6.1.2.1.1.1.0
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSnmpConfapiOIDFromObject(L7_char8* object, L7_char8* oid)
{
#ifdef L7_SNMP_PACKAGE
  return l7_OIDFromObject(object, oid);
#else
  return L7_NOT_SUPPORTED;
#endif
}
