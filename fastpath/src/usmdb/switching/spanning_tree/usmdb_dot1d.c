
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\usmdb_dot1d.c
*
* @purpose Provide interface to hardware API's for RFC 1493 unitmgr components
*
* @component unitmgr
*
* @comments tba
*
* @create 03-Nov-2000
*
* @author gaunce
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include <l7_common.h>
#include <stdio.h>
#include <osapi.h>
#include <nimapi.h>
#include <simapi.h>


#include <stdlib.h>
#include <string.h>
#include <dot1d_api.h>
#include <statsapi.h>
#include <usmdb_counters_api.h>
#include <fdb_api.h>
#include "usmdb_nim_api.h"
#include "usmdb_common.h"
#include "usmdb_counters_api.h"
#include "usmdb_dot1d_api.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_util_api.h"



/*********************************************************************
*
* @purpose Get the unique MAC address used to refer to the bridge.
*          
* @param UnitIndex @b{(input)} the unit for this operation
* @param BridgeAddress @b{(output)} the pointer to the MAC address
* @param BridgeAddress_len @b{(output)} 
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			               found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		   values are invalid
*
* @comments It is recommended that this be the numerically smallest MAC 
*           address of all ports that belong to this bridge.  However 
*           it is only required to be unique.
*
* @end
*********************************************************************/
L7_RC_t
usmDbDot1dBaseBridgeAddressGet ( L7_uint32 UnitIndex, L7_char8 *BridgeAddress, L7_uint32 *BridgeAddress_len )
{
  /* dot1dBase */
  simGetSystemIPBurnedInMac(BridgeAddress);
  *BridgeAddress_len = L7_MAC_ADDR_LEN;
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose Get number of ports controlled by bridging entity.
*          
* @param UnitIndex @b{(input)} the unit for this operation
* @param NumPorts @b{(output)} number of ports
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			               found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		   values are invalid
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
usmDbDot1dBaseNumPortsGet ( L7_uint32 UnitIndex, L7_int32 *NumPorts )
{
  L7_uint32 intf = 0, ports = 0;

  while (usmDbGetNextVisibleIntIfNumber(intf, &intf) == L7_SUCCESS)
  {
    if (usmDbDot1dIsValidIntf(UnitIndex, intf) == L7_TRUE)
    {
      ports++;
    }
  }

  *NumPorts = ports;

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose Get dot1d Base type of bridging.
*          
* @param UnitIndex @b{(input)} the unit for this operation
* @param Type @b{(output)} type of brigding
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			               found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		   values are invalid
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
usmDbDot1dBaseTypeGet ( L7_uint32 UnitIndex, L7_int32 *Type )
{
  *Type = L7_DOT1D_BRIDGING_TYPE;
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose Get list of information for each port of bridge.
*          
* @param UnitIndex @b{(input)} the unit for this operation
* @param Port @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			               found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		   values are invalid
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
usmDbDot1dBasePortEntryGet ( L7_uint32 UnitIndex, L7_int32 Port )
{
  /* dot1dBasePort */ 
  L7_uint32 adminState;

  if (((nimGetIntfAdminState(Port, &adminState)) == L7_SUCCESS) && adminState != L7_DIAG_DISABLE)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Get Internal Interface Number of next valid port.
*          
* @param UnitIndex @b{(input)} the unit for this operation
* @param Port @b{(output)} next valid internal interface number
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			               found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		   values are invalid
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
usmDbDot1dBasePortEntryNextGet ( L7_uint32 UnitIndex, L7_int32 *Port )
{
  L7_uint32 nextIntIfNum, rc;

  if ((rc = nimNextValidIntfNumber(*Port, &nextIntIfNum)) == L7_SUCCESS)
    *Port = nextIntIfNum;

  return rc;
}

/*********************************************************************
*
* @purpose Get ifIndex associated with the internal interface number.
*          
* @param UnitIndex @b{(input)} the unit for this operation
* @param Port @b{(input)} internal interface number
* @param IfIndex @b{(output)} ptr to the IfIndex
*
* @returns  L7_SUCCESS     If the counter was successfully 
*			               found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		   values are invalid
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t
usmDbDot1dBasePortIfIndexGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_int32 *IfIndex )
{
  return nimGetIntfIfIndex(Port, IfIndex);
}

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
*			               found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		   values are invalid
*
* @comments This function is used for a port which (potentially) has 
*           the same value of dot1dBasePortIfIndex as another port on 
*           the same bridge.
*
* @end
*********************************************************************/
L7_RC_t
usmDbDot1dBasePortCircuitGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *buf, L7_uint32 *buf_len )
{
  buf[0] = L7_NULL;
  buf[1] = L7_NULL;
  *buf_len = 2;
  return L7_SUCCESS;
}


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
*			               found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		   values are invalid
*
* @comments It is incremented by both transparent and source route bridges.
*
* @end
*********************************************************************/
L7_RC_t
usmDbDot1dBasePortDelayExceededDiscardsGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *DelayExceededDiscards )
{
  *DelayExceededDiscards = 0;
  return L7_NOT_IMPLEMENTED_YET;
}


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
*			               found and stored in the space provided
* @returns  L7_ERROR       If the counter requested does not exisit 
* @returns  L7_FAILURE     Catastrophic failure i.e. all the returned 
*			    		   values are invalid
*
* @comments It is incremented by both transparent and source route bridges.
*
* @end
*********************************************************************/
L7_RC_t
usmDbDot1dBasePortMtuExceededDiscardsGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *MtuExceededDiscards )
{
  *MtuExceededDiscards = 0;
  return L7_NOT_IMPLEMENTED_YET;
}

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
L7_RC_t
usmDbDot1dTpLearnedEntryDiscardsGet ( L7_uint32 UnitIndex, L7_uint32 *val )
{
  return usmDbFdbBadAddsGet(UnitIndex, val);
}


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
L7_RC_t
usmDbDot1dTpAgingTimeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  *val = fdbGetAddressAgingTimeOut(FD_CNFGR_FDB_DEFAULT_FID);

  return L7_SUCCESS;
}


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
L7_RC_t
usmDbDot1dTpLearnedEntryDiscardsSet ( L7_uint32 UnitIndex, L7_uint32 val )
{
  return L7_NOT_IMPLEMENTED_YET;
}


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
L7_RC_t
usmDbDot1dTpAgingTimeSet ( L7_uint32 UnitIndex, L7_int32 val )
{
  if ((val < L7_FDB_MIN_AGING_TIMEOUT) || (val > L7_FDB_MAX_AGING_TIMEOUT))
  {
    return L7_FAILURE;
  }

  return fdbSetAddressAgingTimeOut(val, FD_CNFGR_FDB_DEFAULT_FID);
}


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
L7_RC_t
usmDbDot1dTpFdbEntryGet ( L7_uint32 UnitIndex, L7_char8 *Address )
{
  /* dot1dTpFdb */
  /* @p0375 start */
  usmdbFdbEntry_t fdbEntry;
  L7_FDB_TYPE_t      type;

  if (Address != NULL)
  {
    bzero(fdbEntry.usmdbFdbMac, L7_FDB_KEY_SIZE);

    fdbGetTypeOfVL(&type);
    if (type == L7_SVL)
    {
      memcpy(fdbEntry.usmdbFdbMac, Address, L7_MAC_ADDR_LEN);
    }
    else
    {
      memcpy(fdbEntry.usmdbFdbMac, Address, L7_FDB_KEY_SIZE);
    }

    return usmDbFDBEntryGet(UnitIndex, &fdbEntry);
  }

  return L7_FAILURE;
/* @p0375 end */
}


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
L7_RC_t
usmDbDot1dTpFdbEntryNextGet ( L7_uint32 UnitIndex, L7_char8 *Address )
{
/* @p0375 start */
  usmdbFdbEntry_t fdbEntry;
  L7_FDB_TYPE_t      type;
  L7_RC_t rc;

  if (Address != NULL)
  {
    bzero(fdbEntry.usmdbFdbMac, L7_FDB_KEY_SIZE);
    fdbGetTypeOfVL(&type);
    if (type == L7_SVL)
    {
      memcpy(fdbEntry.usmdbFdbMac, Address, L7_MAC_ADDR_LEN);
    }
    else
    {
      memcpy(fdbEntry.usmdbFdbMac, Address, L7_FDB_KEY_SIZE);
    }

    rc = usmDbFDBEntryNextGet(UnitIndex, &fdbEntry);

    if (rc == L7_SUCCESS)
    {
      memcpy(Address, fdbEntry.usmdbFdbMac, L7_MAC_ADDR_LEN);
    }

    return rc;
  }

  return L7_FAILURE;
/* @p0375 end */
}


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
L7_RC_t
usmDbDot1dTpFdbPortGet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 *val )
{
/* @p0375 start */
  usmdbFdbEntry_t fdbEntry;
  L7_FDB_TYPE_t      type;
  L7_RC_t rc;

  if (Address != NULL)
  {
    bzero(fdbEntry.usmdbFdbMac, L7_FDB_KEY_SIZE);

    fdbGetTypeOfVL(&type);
    if (type == L7_SVL)
    {
      memcpy(fdbEntry.usmdbFdbMac, Address, L7_MAC_ADDR_LEN);
    }
    else
    {
      memcpy(fdbEntry.usmdbFdbMac, Address, L7_FDB_KEY_SIZE);
    }

    rc = usmDbFDBEntryGet(UnitIndex, &fdbEntry);

    if (rc == L7_SUCCESS)
    {
      *val = fdbEntry.usmdbFdbIntIfNum;
    }

    return rc;
  }

  return L7_FAILURE;
/* @p0375 end */
}


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
L7_RC_t
usmDbDot1dTpFdbStatusGet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 *val )
{
/* @p0375 start */
  usmdbFdbEntry_t fdbEntry;
  L7_FDB_TYPE_t      type;  
  L7_RC_t rc;

  if (Address != NULL)
  {
    bzero(fdbEntry.usmdbFdbMac, L7_FDB_KEY_SIZE);

    fdbGetTypeOfVL(&type);
    if (type == L7_SVL)
    {
      memcpy(fdbEntry.usmdbFdbMac, Address, L7_MAC_ADDR_LEN);
    }
    else
    {
      memcpy(fdbEntry.usmdbFdbMac, Address, L7_FDB_KEY_SIZE);
    }

    rc = usmDbFDBEntryGet(UnitIndex, &fdbEntry);

    if (rc == L7_SUCCESS)
    {
      *val = fdbEntry.usmdbFdbEntryType;
    }

    return rc;
  }

  return L7_FAILURE;
/* @p0375 end */
}


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
L7_RC_t usmDbDot1dTpPortMaxInfoGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *val )
{
  return nimGetIntfConfigMaxFrameSize(Port, val);
}


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
L7_RC_t
usmDbDot1dTpPortInFramesGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *val )
{
  return usmDbStatGet(UnitIndex, L7_CTR_RX_TOTAL_FRAMES, Port, val);
}


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
L7_RC_t
usmDbDot1dTpPortOutFramesGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *val )
{
  return usmDbStatGet(UnitIndex, L7_CTR_TX_TOTAL_FRAMES, Port, val);
}


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
L7_RC_t
usmDbDot1dTpPortInDiscardsGet ( L7_uint32 UnitIndex, L7_int32 Port, L7_uint32 *val )
{
  return usmDbStatGet(UnitIndex, L7_CTR_POLICY_RX_TOTAL_DISCARDS, Port, val);
}


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
L7_RC_t
usmDbDot1dStaticEntryGet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 Port )
{
  return L7_NOT_SUPPORTED;
}


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
L7_RC_t
usmDbDot1dStaticEntryNextGet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 *Port )
{
  return L7_NOT_SUPPORTED;
}

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
L7_RC_t
usmDbDot1dStaticAllowedToGoToGet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 Port, L7_char8 *buf )
{
  return L7_NOT_SUPPORTED;
}


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
L7_RC_t
usmDbDot1dStaticStatusGet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 Port, L7_int32 *val )
{
  return L7_NOT_SUPPORTED;
}


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
L7_RC_t
usmDbDot1dStaticAllowedToGoToSet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 Port, L7_char8 *buf )
{
  return L7_NOT_SUPPORTED;
}

/*********************************************************************
*
* @purpose Set status of this entry... other, invalid, permanent, deleteOnReset,
*          deleteOnTimeout.
*          
* @param UnitIndex @b{(input)} the unit for this operation
* @param Address @b{(input)}
* @param Port @b{(input)}
* @param val @b{(input)}
*
* @returns  L7_SUCCESS     If success
* @returns  L7_FAILURE     If failure 
*
* @comments none 	
*
* @end
*********************************************************************/
L7_RC_t
usmDbDot1dStaticStatusSet ( L7_uint32 UnitIndex, L7_char8 *Address, L7_int32 Port, L7_int32 val )
{
  return L7_NOT_SUPPORTED;
}



/*********************************************************************
* @purpose  Check if an interface is valid for dot1d
*
* @param    UnitIndex      unit
* @param    intIfNum       internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*                                 
* @end
*********************************************************************/
L7_BOOL usmDbDot1dIsValidIntf(L7_uint32 unitIndex, L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if ( (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS) ||
       (sysIntfType == L7_LOOPBACK_INTF) ||
       (sysIntfType == L7_WIRELESS_INTF) ||
       (sysIntfType == L7_CAPWAP_TUNNEL_INTF) ||
       (sysIntfType == L7_TUNNEL_INTF) )
    return L7_FALSE;

  return L7_TRUE;
}
/*********************************************************************
*
* @purpose Returns the number of local packets dropped
*          
* @param   UnitIndex  Unit
* @param   interface  interface number
* @param   *val       returned value
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbLocalPktsDroppedGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val)
{
  *val=0;
  return L7_NOT_IMPLEMENTED_YET;
}


