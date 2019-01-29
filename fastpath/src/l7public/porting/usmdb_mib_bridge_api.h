/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename usmdb_mib_bridge.h
*
* @purpose usmdb support for LVL7 extensions to BRIDGE support
*
* @component unitmgr
*
* @comments tba
*
* @create 01/10/2001
*
* @author rjindal
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/


#ifndef USMDB_MIB_BRIDGE_API_H
#define USMDB_MIB_BRIDGE_API_H

#include "usmdb_common.h"
#include "fdb_exports.h"

/*********************************************************************
*
* @structures usmDbIdentifier_t
*
* @purpose    dot1d information structure
*
* @notes none
*
*********************************************************************/
typedef struct
{
  L7_ushort16 dot1dPrio;
  L7_uchar8   mac[L7_MAC_ADDR_LEN];
}usmDbIdentifier_t;





/*********************************************************************
*
* @purpose Get the unique MAC address used to refer to the bridge.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param BridgeAddress @b{(output)} the pointer to the MAC address
* @param BridgeAddress_len @b{(output)}
*
* @returns  L7_SUCCESS     If the counter was successfully
*                    found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                values are invalid
*
* @comments It is recommended that this be the numerically smallest MAC
*           address of all ports that belong to this bridge.  However
*           it is only required to be unique.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dBaseBridgeAddressGet ( L7_uint32 UnitIndex, L7_char8 *BridgeAddress, L7_uint32 *BridgeAddress_len );
/*********************************************************************
*
* @purpose Get number of ports controlled by bridging entity.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param NumPorts @b{(output)} number of ports
*
* @returns  L7_SUCCESS     If the counter was successfully
*                    found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                values are invalid
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dBaseNumPortsGet ( L7_uint32 UnitIndex, L7_int32 *NumPorts );

/*********************************************************************
*
* @purpose Get dot1d Base type of bridging.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Type @b{(output)} type of brigding
*
* @returns  L7_SUCCESS     If the counter was successfully
*                    found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                values are invalid
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dBaseTypeGet ( L7_uint32 UnitIndex, L7_int32 *Type );



/*********************************************************************
*
* @purpose Get list of information for each port of bridge.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Port @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS     If the counter was successfully
*                    found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                values are invalid
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dBasePortEntryGet ( L7_uint32 UnitIndex, L7_int32 Port );

/*********************************************************************
*
* @purpose Get ifIndex associated with the internal interface number.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Port @b{(input)} internal interface number
* @param IfIndex @b{(output)} ptr to the IfIndex
*
* @returns  L7_SUCCESS     If the counter was successfully
*                    found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                values are invalid
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dBasePortIfIndexGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_int32 *IfIndex );

/*********************************************************************
*
* @purpose Get name of an object instance unique to this port.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Port @b{(input)} internal interface number
* @param buf @b{(output)} data buffer
* @param buf_len @{(output)} length of data in buffer
*
* @returns  L7_SUCCESS     If the counter was successfully
*                    found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                values are invalid
*
* @comments This function is used for a port which (potentially) has
*           the same value of dot1dBasePortIfIndex as another port on
*           the same bridge.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dBasePortCircuitGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *buf, L7_uint32 *buf_len );

/*********************************************************************
*
* @purpose Get number of frames discarded by this port due
*          to delay through the bridge.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Port @b{(input)} port internal interface number
* @param DelayExceededDiscards @b{(output)} return value
*
* @returns  L7_SUCCESS     If the counter was successfully
*                    found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                values are invalid
*
* @comments It is incremented by both transparent and source route bridges.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dBasePortDelayExceededDiscardsGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *DelayExceededDiscards );

/*********************************************************************
*
* @purpose Get number of frames discarded by this port due
*          to excessive size.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Port @b{(input)} port internal interface number
* @param MtuExceededDiscards @b{(output)} return value
*
* @returns  L7_SUCCESS     If the counter was successfully
*                    found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                values are invalid
*
* @comments It is incremented by both transparent and source route bridges.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dBasePortMtuExceededDiscardsGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *MtuExceededDiscards );

/*********************************************************************
*
* @purpose Get Internal Interface Number of next valid port.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Port @b{(output)} next valid internal interface number
*
* @returns  L7_SUCCESS     If the counter was successfully
*                    found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                values are invalid
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dBasePortEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *Port );







/*********************************************************************
*
* @purpose Set cost of the path to the root as seen from this bridge.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param val @b{(input)} root cost for switch
*
* @returns  L7_SUCCESS     If the counter was successfully
*                    found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned
*                values are invalid
*
* @comments none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dStpRootCostSet ( L7_uint32 UnitIndex, L7_uint32 val );


/*********************************************************************
*
* @purpose Get number of Forwarding Database entries that have been discarded
*          due to a lack of space to store them.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param val @b{(output)} number of discarded entries
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments none  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTpLearnedEntryDiscardsGet ( L7_uint32 UnitIndex, L7_uint32 *val );

/*********************************************************************
*
* @purpose Set number of Forwarding Database entries to discard.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param val @b{(input)} number of discarded entries
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments none  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTpLearnedEntryDiscardsSet ( L7_uint32 UnitIndex, L7_uint32 val );

/*********************************************************************
*
* @purpose Get timeout period for aging out dynamically learned forwarding information.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param val @b{(output)} timeout period
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments In seconds  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTpAgingTimeGet ( L7_uint32 UnitIndex, L7_int32 *val );

/*********************************************************************
*
* @purpose Set timeout period for aging out dynamically learned forwarding information.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param val @b{(intput)} timeout period
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments In seconds  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTpAgingTimeSet ( L7_uint32 UnitIndex, L7_int32 val );

/*********************************************************************
*
* @purpose Get info about a specific unicast MAC address which the bridge
*          has forwarding and/or filtering information.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Address @b{(output)}
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments none  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTpFdbEntryNextGet ( L7_uint32 UnitIndex, L7_char8 *Address );

/*********************************************************************
*
* @purpose Get info about a specific unicast MAC address which the bridge
*          has forwarding and/or filtering information.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Address @b{(output)}
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments none  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTpFdbEntryGet ( L7_uint32 UnitIndex, L7_char8 *Address );

/*********************************************************************
*
* @purpose Returns the port number
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Address @b{(output)}
* @param val @b{(output)}
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments " Either the value of '0', or the port number of the port
*             on which a frame having a source address equal to the
*             value of the corresponding instance of dot1dTpFdbAddress
*             has been seen.  A value of '0' indicates that the port
*             number has not been learned bu that the bridge does
*             have some forwarding/filtering information about this
*             address (e.g. in the dot1dStaticTable).  Implementors
*             are encouraged to assign the port value to this
*             object whenever it is learned even for addresses
*             for which the corresponding value of dot1dTpFdbStatus
*             is not learned(3)."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTpFdbPortGet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 *val );

/*********************************************************************
*
* @purpose Get the status of the dot1dTpFdbEntry.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Address @b{(output)}
* @param val @b{(output)}
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments Status types:
*               other(1)
*               invalid(2)
*               learned(3)
*               self(4)
*               mgmt(5)
*
*           " The status of this entry.  The meaning of the values are:
*
*             other(1): none of the following.  This would include the
*                       case where some other MIB object (not
*                       corresponding instance of dot1dTpFdbPort, nor
*                       an entry in the dot1dStaticTable) is being
*                       used to determine if and how frames addressed
*                       to the value of the corresponding instance of
*                       dot1dTpFdbAddress are being forwarded.
*
*            invalid(2): this entry is no longer valid (e.g. it was
*                        learned but has since aged-out), but has not
*                        yet been flushed from the table.
*
*            learned(3): the value of the corresponding instance
*                        of dot1dTpFdbPort was learned and is being
*                        used.
*
*            self(4):    the value of the corresponding instance of
*                        dot1dTpFdbAddress represents one of the
*                        bridge's addresses.  The corresponding
*                        instance of dot1dTpFdbPort indicates
*                        which of the bridge's ports has this address.
*
*            mgmt(5):    the value of this corresponding instance of
*                        dot1dTpFdbAddress is also the value of an
*                        existing instance of dot1dStaticAddress."
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTpFdbStatusGet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 *val );

/*********************************************************************
*
* @purpose Get maximum size of the INFO field this port will receive or transmit.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Port @b{(input)} interface number
* @param val @b{(output)} maximum size
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments none  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTpPortMaxInfoGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *val );

/*********************************************************************
*
* @purpose Get number of frames received by this port from its segment.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Port @b{(input)} interface number
* @param val @b{(output)} number of frames received
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments none  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTpPortInFramesGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *val );

/*********************************************************************
*
* @purpose Get number of frames transmitted by this port to its segment.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Port @b{(input)} interface number
* @param val @b{(output)} number of frames transmitted
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments none  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTpPortOutFramesGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *val );

/*********************************************************************
*
* @purpose Get count of valid frames received which were
*          discarded by the Forwarding Process.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Port @b{(input)} interface number
* @param val @b{(output)} valid frames received which were discarded
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments none  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dTpPortInDiscardsGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *val );

/*********************************************************************
*
* @purpose Get set of ports to which frames received from a specific
*          port and containing a specific destination address are allowed to be forwarded.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Address @b{(output)}
* @param Port @b{(input)}
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments none  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dStaticEntryGet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 Port );

/*********************************************************************
*
* @purpose Get the next set of ports to which frames received from a specific
*          port and containing a specific destination address are allowed to be forwarded.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Address @b{(output)}
* @param Port @b{(input)}
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments none  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dStaticEntryNextGet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 *Port );

/*********************************************************************
*
* @purpose Get set of ports to which frames received from a specific port
*          and destined for a specific MAC Address, are allowed to be forwarded.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Address @b{(output)}
* @param Port @b{(input)}
* @param buf @b{(output)}
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments none  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dStaticAllowedToGoToGet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 Port, L7_char8 *buf );

/*********************************************************************
*
* @purpose Set ports to which frames received from a specific port and
*          destined for a specific MAC Address, are allowed to be forwarded.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Address @b{(input)}
* @param Port @b{(input)}
* @param buf @b{(input)}
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments none  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dStaticAllowedToGoToSet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 Port, L7_char8 *buf );

/*********************************************************************
*
* @purpose Get status of this entry... other, invalid, permanent, deleteOnReset,
*          deleteOnTimeout.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Address @b{(output)}
* @param Port @b{(input)}
* @param val @b{(output)}
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments none  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dStaticStatusGet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 Port, L7_int32 *val );

/*********************************************************************
*
* @purpose Set status of this entry... other, invalid, permanent, deleteOnReset,
*          deleteOnTimeout.
*
* @param UnitIndex @b{(input)} the unit for this operation
* @param Address @b{(input)}
* @param Port @b{(input)}
* @param val @b{(input)} status of this entry
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure
*
* @comments none  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDot1dStaticStatusSet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 Port, L7_int32 val );

/*********************************************************************
* @purpose      Gets a entry from the forwarding database depending on the mac addr
*
* @param        UnitIndex   @b{(input)} unit index
* @param        *entry      @b{(output)} pointer to the structure
* 
* @returns      L7_SUCCESS
* @returns      L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbFDBEntryGet(L7_uint32 UnitIndex, usmdbFdbEntry_t *entry);

/*********************************************************************
* @purpose      Gets the next entry from the forwarding database
*
*
* @param        UnitIndex   @b{(input)} unit index
* @param        *entry      @b{(input)} pointer to the structure
* 
* @returns      L7_SUCCESS
* @returns      L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbFDBEntryNextGet(L7_uint32 UnitIndex, usmdbFdbEntry_t *entry);

/*********************************************************************
*
* @purpose Get the next entry from the FDB for a specific interface
*
* @param L7_uint32       intIfNum @b((input)) internal interface number
* @param usmdbFdbEntry_t *entry   @b((input/output)) pointer to the structure
*
* @returns L7_SUCCESS
* @returns L7_FAILURE, no more entries for this interface
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFDBIntfEntryNextGet(L7_uint32 intIfNum, usmdbFdbEntry_t *entry);

/*********************************************************************
*
* @purpose Get the next entry from the FDB for a specific vlan
*
* @param L7_ushort16      vlanId   @b((input)) vlan identifier
* @param usmdbFdbEntry_t *entry    @b((input/output)) pointer to the structure
*
* @returns L7_SUCCESS
* @returns L7_FAILURE, no more entries for this VLAN
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbFDBVlanEntryNextGet(L7_ushort16 vlanId, usmdbFdbEntry_t *entry);


/*********************************************************************
* @purpose  return the type of supported VL
*
* @param    type       pointer to the type
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbFDBTypeOfVLGet(L7_uint32 UnitIndex, L7_FDB_TYPE_t *type);

/*********************************************************************
* @purpose      get the next fdbID starting from a certain index.
*
* @param        UnitIndex   @b{(input)} unit index
* @param        startfrom   @b{(input)} index to start search from
* @param        *fdbId      @b{(output)} pointer to the return value
*
* @returns      L7_SUCCESS
* @returns      L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbFdbIdNextGet(L7_uint32 UnitIndex, L7_uint32 startfrom, L7_uint32 *fdbId);

/*********************************************************************
* @purpose      check to see if an FDB exists based on an FDB ID.
*
* @param        UnitIndex   @b{(input)} unit index
* @param        fdbId       @b{(input(} FDB ID to be checked
*
* @returns      L7_SUCCESS
* @returns      L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbFdbIdGet(L7_uint32 UnitIndex, L7_uint32 fdbId);

/*********************************************************************
* @purpose      Gets the aging timeout value for an FDB
*
*
* @param        UnitIndex   @b{(input)} unit index
* @param        fdbId       @b{(input)} FDB ID
* @param        *timeOut    @b{(output)} aging timeout value
* 
* @returns      L7_SUCCESS
* @returns      L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbFDBAddressAgingTimeoutGet(L7_uint32 UnitIndex, L7_uint32 fdbId, L7_uint32 *timeOut);

/*********************************************************************
* @purpose      Sets the aging timeout value for an FDB
*
*
* @param        UnitIndex   @b{(input)} unit index
* @param        fdbId       @b{(input)} FDB ID
* @param        timeOut     @b{(input)} aging timeout value
* 
* @returns      L7_SUCCESS
* @returns      L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbFDBAddressAgingTimeoutSet(L7_uint32 UnitIndex, L7_uint32 fdbId, L7_uint32 timeOut);

/*********************************************************************
* @purpose  Returns the number of failed FDB insert attempts due to
*           failure to allocate space.
*
* @param    *badAdds     pointer to the number of failed FDB inserts to
*                        be returned
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbFdbBadAddsGet(L7_uint32 UnitIndex, L7_uint32 *badAdds);

/*********************************************************************
*
* @purpose Retrieve the status of a SNMP save config request
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Enum L7_SAVE_CONFIG_CODE_t
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSaveConfigResultCodeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Set the status of a SNMP save config to not initiated
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSaveConfigResultCodeReset(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose  To get the maximum number of entries that the FDB table
*           can hold.
*
* @param    L7_uint32  UnitIndex    @b((input)) the operation unit
* @param    L7_uint32  *maxEntries  @b((output)) pointer to the number
*                                                of total entries
*
* @returns  L7_SUCCESS
*
* @comments None.
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbFdbMaxTableEntriesGet(L7_uint32 UnitIndex, L7_uint32 *maxEntries);
/*********************************************************************
* @purpose      gets a entry from the forwarding database
*               
*          
* @param        UnitIndex   @b{(input)} unit index
* @param        *val        @b{(output)} pointer to the value
*	
* @returns      L7_SUCCESS
* @returns      L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbActiveAddrEntriesGet(L7_uint32 UnitIndex, L7_uint32 *val);
/*********************************************************************
* @purpose      function to find the maximun number of entries in the forwarding database
*               
* @param        UnitIndex   @b{(input)} unit index
* @param        *val        @b{(output)}pointer to the value
*	
* @returns      L7_SUCCESS
* @returns      L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMostAddrEntriesGet(L7_uint32 UnitIndex, L7_uint32 *val);
/*********************************************************************
* @purpose      function to reset the maximun number of entries in the forwarding database
*               
* @param        UnitIndex   @b{(input)} unit index
*	
* @returns      L7_SUCCESS
* @returns      L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMostAddrEntriesReset(L7_uint32 UnitIndex);

/*********************************************************************
* @purpose  Flush all learned entries from the L2FDB.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t usmDbFdbFlush(void);

/*********************************************************************
* @purpose  Flushes all entries in fdb learnt on this interface 
*
* @param    intIfNum @b((input)) internal interface number  
*
* @returns  L7_SUCCESS on a successful operation 
* @returns  L7_FAILURE for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t usmDbFdbFlushByPort(L7_uint32 intfNum);

/*********************************************************************
* @purpose  Flushes specific VLAN entries in fdb.
*
* @param    vlanId @b((input)) VLAN number.
*
* @returns  L7_SUCCESS on a successful operation 
* @returns  L7_FAILURE for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t usmDbFdbFlushByVlan(L7_uint32 vlanId);

/*********************************************************************
* @purpose  Flushes All MAC specific entries in fdb.
*
* @param    mac @b((input)) MAC address
*
* @returns  L7_SUCCESS on a successful operation 
* @returns  L7_FAILURE for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t usmDbFdbFlushByMac(L7_enetMacAddr_t mac);

/*********************************************************************
* @purpose  Get count of dynamic entries in the L2FDB.
*
* @param    L7_uint32     *entries   @{{output}}  Number of dynamic entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t usmDbFdbDynamicEntriesGet(L7_uint32 *entries);

/*********************************************************************
* @purpose  Get count of internally added entries in the L2FDB.
*
* @param    L7_uint32     *entries   @{{output}}  Number of dynamic entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t usmDbFdbInternalEntriesGet(L7_uint32 *entries);

/*********************************************************************
* @purpose  Get count of dynamic entries in the L2FDB for a specific interface
*
* @param    L7_uint32     intIfNum   @((input))   Internal interface number
* @param    L7_uint32     *entries   @{{output}}  Number of static entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t usmDbFdbIntfDynamicEntriesGet(L7_uint32 intIfNum,
                                      L7_uint32 *entries);

/*********************************************************************
* @purpose  Get count of dynamic entries in the L2FDB for a specific VLAN
*
* @param    L7_ushort16    vlanId    @((input))   VLAN identifier
* @param    L7_uint32     *entries   @{{output}}  Number of static entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t usmDbFdbVlanDynamicEntriesGet(L7_ushort16 vlanId,
                                      L7_uint32   *entries);

/*********************************************************************
* @purpose  Get count of internal entries in the L2FDB for a specific interface
*
* @param    L7_uint32     intIfNum   @((input))   Internal interface number
* @param    L7_uint32     *entries   @{{output}}  Number of static entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t usmDbFdbIntfInternalEntriesGet(L7_uint32 intIfNum,
                                       L7_uint32 *entries);

/*********************************************************************
* @purpose  Get count of internal entries in the L2FDB for a specific VLAN
*
* @param    L7_ushort16    vlanId    @((input))   VLAN identifier
* @param    L7_uint32     *entries   @{{output}}  Number of static entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t usmDbFdbVlanInternalEntriesGet(L7_ushort16  vlanId,
                                       L7_uint32   *entries);

#endif
