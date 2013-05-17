
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dot1d_api.h
*
* @purpose dot1d APIs
*
* @component dot1d
*
* @comments none
*
* @create 08/14/2000
*
* @author wjacobs
*
* @end
*             
**********************************************************************/

#ifndef _DOT1DAPI_H_
#define _DOT1DAPI_H_

/* need to fix */  
#include "l7_product.h"
#include "statsapi.h"
#include "osapi.h"

typedef struct
{
  L7_ushort16 dot1dPrio;
  L7_uchar8 mac[L7_MAC_ADDR_LEN];

} dot1dIdentifier_t;

/*--------------------------------------*/
/*  dot1Q  Public Structures            */
/*--------------------------------------*/


typedef L7_ushort16  dot1d_portID_t;
typedef L7_uint32    dot1d_bridgeID_t;

/*********************************************************************
*
* @purpose  Get Port ID
*
* @param intIfNum   physical or logical interface
*
* @returns the port ID
*
* @notes none
*
* @end
*********************************************************************/
L7_ushort16 dot1dSTPPortIdGet(L7_uint32 intIfNum); 

/*********************************************************************
*
* @purpose  Set the status of Spanning Tree for the switch
*
* @param    status      L7_ENABLE or L7_DISABLE   
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPStatusSet(L7_uint32 status);

/*********************************************************************
*
* @purpose  Set the status of Spanning Tree for an STP instance
*
* @param    instance    STP instance
* @param    status      L7_ENABLE or L7_DISABLE   
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1dSTPInstanceStatusSet(L7_uint32 instance, L7_uint32 status);

/*********************************************************************
*
* @purpose  Set Spanning Tree Hello Time
*
* @param    time    time, in seconds
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR, if out of range
* @returns  L7_FAILURE, if other failure
*
*
* @notes    LLD_NOTE: Provide ranges and interdependencies
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPBridgeHelloTimeSet(L7_uint32 time);

/*********************************************************************
*
* @purpose  Set Spanning Tree Max Age
*
* @param    time    time, in seconds
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR, if out of range
* @returns  L7_FAILURE, if other failure
*
*
* @notes    LLD_NOTE: Provide ranges and interdependencies
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPBridgeMaxAgeSet(L7_uint32 time);

/*********************************************************************
*
* @purpose  Set Spanning Tree Forward Delay
*
* @param    time    time, in seconds
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR, if out of range
* @returns  L7_FAILURE, if other failure
*
*
* @notes    LLD_NOTE: Provide ranges and interdependencies
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPBridgeFwdDelaySet(L7_uint32 time);

/*********************************************************************
*
* @purpose Set Spanning Tree Bridge Priority
*
* @param prio port priority
*
* @return     L7_SUCCESS, if success
* @return     L7_ERROR, if out of range
* @return     L7_FAILURE, if other failure
*
* @notes none, maybe see notes for port priority funciton below?
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPBridgePrioSet(L7_uint32 prio);

/*********************************************************************
*
* @purpose  Set Spanning Tree Port Priority
*            
* @param    intf        physical or logical interface to be configured
*                       or MAX_INTERFACE_NUM to apply configuration
*                       to all interfaces
* @param    priority    port priority
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR, if out of range
* @returns  L7_FAILURE, if other failure
*
*
* @notes    LLD_NOTE: Provide ranges and interdependencies
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPPortPrioSet(L7_uint32 intIfNum, L7_uint32 prio);

/*********************************************************************
*
* @purpose  Set Spanning Tree Port Cost
*            
* @param    intf        physical or logical interface to be configured
*                       or MAX_INTERFACE_NUM to apply configuration
*                       to all interfaces
* @param    cost        port cost    
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR, if out of range
* @returns  L7_FAILURE, if other failure
*
*
* @notes    LLD_NOTE: Provide ranges and interdependencies
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPPortCostSet(L7_uint32 intIfNum, L7_uint32 cost);

/*********************************************************************
*
* @purpose  See if the port cost is set to be determined automatically
*            
* @param    intf        physical or logical interface 
*
* @returns  L7_ENABLE, if the port cost is configured automatically
* @returns  L7_DISABLE, if the port cost was set manually
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPPortPathCostAutoModeGet(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Set Spanning Tree Port mode
*            
* @param    intf        physical or logical interface to be configured
*                       or MAX_INTERFACE_NUM to apply configuration
*                       to all interfaces
* @param    mode        L7_DOT1D_8021D, L7_DOT1D_RAPID, L7_DOT1D_OFF
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
*
* @notes    LLD_NOTE: Provide ranges and interdependencies
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPPortModeSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Get the Status of Spanning Tree for the switch
*
* @param    void   
*
* @returns  L7_ENABLE, if spanning tree is enabled for the switch
* @returns  L7_DISABLE, if spanning tree is disables for the switch
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPStatusGet();

/*********************************************************************
*
* @purpose  Get the Status of Spanning Tree for an STP instance
*
* @param    instance    STP instance
*
* @returns  L7_ENABLE, if spanning tree is enabled for the instance
* @returns  L7_DISABLE, if spanning tree is disables for the isntance
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPInstanceStatusGet(L7_uint32 instance);

/*********************************************************************
*
* @purpose  Get the Status of the Spanning Tree trap flag for an STP instance
*
* @param    instance    STP instance
*
* @returns  L7_ENABLE, if spanning trap flag is enabled for the instance
* @returns  L7_DISABLE, if spanning trap flag is disabled for the isntance
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPTrapFlagGet(L7_uint32 instance);

/*********************************************************************
*
* @purpose  Set the Status of the Spanning Tree trap flag for an STP instance
*
* @param    instance    STP instance
* @param    val         STP trap flag status
*
* @returns  L7_SUCCESS
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1dSTPTrapFlagSet(L7_uint32 instance, L7_uint32 val);

/*********************************************************************
*
* @purpose  Get Spanning Tree Bridge Priority
*
* @param    none
*
* @return   bridge priority
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPBridgePrioGet();

/*********************************************************************
*
* @purpose  Get Spanning Tree Bridge Hello Time
*
* @param    void
*
* @returns  time,  in seconds
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPBridgeHelloTimeGet();

/*********************************************************************
*
* @purpose Get Spanning Tree Hello Time
*
* @param void
*
* @return time, in seconds
*
* @notes none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPHelloTimeGet();

/*********************************************************************
*
* @purpose  Get Spanning Tree Bridge Max Age
*
* @param    void
*
* @returns  time, in seconds
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPBridgeMaxAgeGet();

/*********************************************************************
*
* @purpose Get Spanning Tree Max Age
*
* @param none
*
* @return time, in seconds
*
* @notes none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPMaxAgeGet();

/*********************************************************************
*
* @purpose  Get Spanning Tree Bridge Forward Delay
*
* @param    void
*
* @returns  time, in seconds
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPBridgeFwdDelayGet();

/*********************************************************************
*
* @purpose Get Spanning Tree Forward Delay
*
* @param   none
*
* @return  time, in seconds
*
* @notes   none 
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPFwdDelayGet();

/*********************************************************************
*
* @purpose Get Spanning Tree Hold Time
*
* @param none
*
* @return hold time
*
* @notes none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPHoldTimeGet();

/*********************************************************************
*
* @purpose Get the Root Port for the switch
*
* @param none
*
* @returns root port for the switch
*
* @notes none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPRootPortGet();

/*********************************************************************
*
* @purpose Get the Root Cost for the switch
*
* @param none
*
* @returns root cost for switch
*
* @notes none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPRootCostGet();

/*********************************************************************
*
* @purpose  Get Spanning Tree Port Priority
*            
* @param    intf        physical or logical interface to be configured
*
* @returns  port priority
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPPortPrioGet(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Get Spanning Tree Port Cost
*            
* @param    intf        physical or logical interface 
*
* @returns  port cost
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPPortCostGet(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Get Spanning Tree Port Mode
*            
* @param    intf        physical or logical interface 
*                      
* @returns  L7_DOT1D_8021D
* @returns  L7_DOT1D_RAPID
* @returns  L7_DOT1D_OFF
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPPortModeGet(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Get Spanning Tree Designated Root
*            
* @param    intf        physical or logical interface 
*                      
* @returns  Designated Root Bridge ID
*
* @notes    none
*
* @end
*********************************************************************/
void dot1dSTPPortDesignatedRootGet(L7_uint32 intIfNum, dot1dIdentifier_t *designatedRoot);

/*********************************************************************
*
* @purpose  Get Spanning Tree Designated Bridge
*            
* @param    intf        physical or logical interface 
*                      
* @returns  Designated Bridge ID
*
* @notes    none
*
* @end
*********************************************************************/
void dot1dSTPPortDesignatedBridgeGet(L7_uint32 intIfNum, dot1dIdentifier_t *designatedBridge);

/*********************************************************************
*
* @purpose  Get Spanning Tree Designated Root for the switch
*            
* @param    none
*                      
* @returns  Designated Root Bridge ID
*
* @notes    none
*
* @end
*********************************************************************/
void dot1dSTPDesignatedRootGet(dot1dIdentifier_t *designatedRoot);

/*********************************************************************
*
* @purpose  Get Spanning Tree Designated Port ID
*            
* @param    intf        physical or logical interface 
*                      
* @returns  Designated Port ID
*
* @notes    none
*
* @end
*********************************************************************/
L7_ushort16 dot1dSTPPortDesignatedPortGet(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Get Spanning Tree Designated Cost
*            
* @param    intf        physical or logical interface 
*                      
* @returns  Designated Cost for the port
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPPortDesignatedCostGet(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Get Spanning Tree Port State
*            
* @param    intf        physical or logical interface 
*                      
* @returns  L7_DOT1D_BLOCKING
* @returns  L7_DOT1D_LISTENING
* @returns  L7_DOT1D_LEARNING
* @returns  L7_DOT1D_FORWARDING
* @returns  L7_DOT1D_DISABLED
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPPortState(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Get the number of times this port has transitioned to the forwarding state
*            
* @param    intIfNum 
*                      
* @returns  number of forward transitions
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1d_get_stpPortForwardTransitions(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Get Spanning Tree Topology Change Count
*            
* @param    none
*                      
* @returns  total number of topology changes
*
* @notes   none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPTopoChangeCountGet();

/*********************************************************************
*
* @purpose  Get the time since the last Spanning Tree Topology Change
*            
* @param    none
*                      
* @returns  time since last spanning tree topo. change
*
* @notes    LLD_NOTE:  calculation needed to supply value
*
* @end
*********************************************************************/
L7_RC_t dot1dSTPTopoChangeTimeGet(L7_timespec *ts);

/*********************************************************************
*
* @purpose  Set the address aging time
*            
* @param    time, in seconds
*                      
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR, if out of range
* @returns  L7_FAILURE, if other failure
*
*
* @notes    LLD_NOTE:  This belongs to the address table component
*
* @end
*********************************************************************/
L7_RC_t dot1dAgingTimeSet(L7_uint32 time);

/*********************************************************************
*
* @purpose  Get the address aging time
*            
* @param    time, in seconds
*                      
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR, if out of range
* @returns  L7_FAILURE, if other failure
*
*
* @notes    LLD_NOTE:  This belongs to the address table component
*
* @end
*********************************************************************/
L7_uint32 dot1dAgingTimeGet();

/*********************************************************************
*
* @purpose  Get the version of the spanning tree standard used
*            
* @param    void
*                      
* @returns  *dot1dSpecification
*
* @notes    none
*
* @end
*********************************************************************/
L7_char8 *dot1dSpecificationGet();

/*********************************************************************
*
* @purpose  Get the dot1d Base Mac Address
*            
* @param    mac   base mac address
*                      
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dot1dBaseMacGet(L7_uchar8 *mac);

/*********************************************************************
*
* @purpose  Get the dot1d Base Number of ports
*            
* @param    void
*                      
* @returns  number of ports
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dBaseNumPortsGet();

/*********************************************************************
*
* @purpose  Get the dot1d Base type of bridging
*            
* @param    void
*                      
* @returns  type of brigding
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dBaseTypeGet();

/*********************************************************************
*
* @purpose  Get the dot1d Base port circuit
*            
* @param    intIfNum    port internal interface number
* @param    buf         return data buffer
* @param    lenOfData   length of data in buffer
*                      
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dBasePortCircuitGet(L7_uint32 intIfNum, L7_uint32 *buf, L7_uint32 *lenOfData);

/*********************************************************************
*
* @purpose  Get the dot1d Base port discards for delay
*            
* @param    intIfNum    port internal interface number
* @param    val         return value
*                      
* @returns  L7_SUCCESS
*
* @notes    hardware does not support counter
*
* @end
*********************************************************************/
L7_uint32 dot1dBasePortDelayExceededDiscardsGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the dot1d Base port discards for mtu
*            
* @param    intIfNum    port internal interface number
* @param    val         return value
*                      
* @returns  L7_SUCCESS
*
* @notes    hardware does not support counter
*
* @end
*********************************************************************/
L7_uint32 dot1dBasePortMtuExceededDiscardsGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Get the dot1d protocol Specification
*            
* @param    val         return value
*                      
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_uint32 dot1dStpProtocolSpecificationGet(L7_uint32 *val);

/*********************************************************************
*
* @purpose  See if an individual port is participating in STP or not
*            
* @param    intIfNum  interface number
*                      
* @returns  L7_FALSE  if the port IS NOT participating in spanning tree
* @returns  L7_TRUE   if the port IS participating in spanning tree
*
* @notes    several things can cause a port to not be participating
*           such as when the port is part of a trunk or when the port
*           has STP manually turned off
*
* @end
*********************************************************************/
L7_uint32 dot1dIsPortParticipating(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Set the dot1d instance to work with
*            
* @param    instanceID  spanning tree instance
*                      
* @returns  L7_SUCCESS  if the ID is valid and was set
* @returns  L7_FAILURE   if the ID is invalid and not set
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dInstanceToConfigureSet(L7_uint32 instanceID);

/*********************************************************************
*
* @purpose  Get the dot1d instance to work with
*            
* @param    instanceID  spanning tree instance
*                      
* @returns  configInstanceID  values of the current instance being wored with
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dInstanceToConfigureGet();

/*********************************************************************
*
* @purpose  Add or move an interface to a spanning tree instance
*            
* @param    intIfNum    interface to add
* @param    instanceID  spanning tree instance
*                      
* @returns  L7_SUCCESS  member was added
* @returns  L7_FAILURE  member was not added
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dInstanceMemberAdd(L7_uint32 intIfNum, L7_uint32 instanceID);

/*********************************************************************
*
* @purpose  Remove an interface from a spanning tree instance
*            
* @param    intIfNum    interface to remove
*                      
* @returns  L7_SUCCESS  member was removed
* @returns  L7_FAILURE  member was not removed
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dInstanceMemberRemove(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Get next interface for current instance
*            
* @param    intIfNum       starting interface
* @param    *nextIntIfNum  return value - next interface
*                      
* @returns  L7_SUCCESS  next member was found
* @returns  L7_FAILURE  next member was not found
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1dInstanceMemberGetNext(L7_uint32 intIfNum, L7_uint32 *nextIntIfNum);

/*********************************************************************
*
* @purpose  Remove all interfaces from a spanning tree instance
*            
* @param    instance    instance to remove members from
*                      
* @returns  L7_SUCCESS  members were removed
* @returns  L7_FAILURE  members were not removed
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dInstanceMemberRemoveAll(L7_uint32 instance);

/*********************************************************************
*
* @purpose  Return the instance of a specific interface
*            
* @param    intIfNum    interface to remove
*                      
* @returns  L7_SUCCESS  member was removed
* @returns  L7_FAILURE  member was not removed
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dInterfaceInstanceGet(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Returns L7_TRUE if the interface is in the currently configured stp instance 
*            
* @param    intIfNum    interface to remove
*                      
* @returns  L7_TRUE   interface is in the current config instance
* @returns  L7_FALSE  interface is NOT in the current config instance
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dot1dIsInterfaceInCurrentInstance(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Set the status of flow control in the switch
*            
* @param    status  L7_ENABLE or L7_DISABLE
*                      
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
*
* @notes    LLD_NOTE:  This belongs to the some other component (system ?)
*                      or misc_l2 ??????
*
* @end
*********************************************************************/
L7_uint32 bridgeFlowCtrlStatusSet(L7_uint32 status);


/*********************************************************************
* @purpose  Allow stats manager to get a bpdu related statistic
*
* @param    c           Pointer to storage allocated by stats
                        manager to hold the results
*
* @returns  none
*
* @notes    Only called by stats manager
*       
* @end
*********************************************************************/
void bpduStatGet(pCounterValue_entry_t c);


/*********************************************************************
* @purpose  Callback function to process VLAN changes.
*
* @param    vlanId      VLAN ID
* @param    intIfnum    internal interface whose state has changed
* @param    event       VLAN event (see vlanNotifyEvent_t for list)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dot1dVlanChangeCallback(L7_uint32 vlanId, L7_uint32 intIfNum, L7_uint32 event);

/*********************************************************************
*
* @purpose  Get the FID for an STP instance
*
* @param    instance    STP instance
*
* @returns  FID
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1dSTPFIDGet(L7_uint32 instance);

/*********************************************************************
*
* @purpose  Set the FID for an STP instance
*
* @param    instance    STP instance
* @param    val         STP FID
*
* @returns  L7_SUCCESS  FID was set
* @returns  L7_FAILURE  FID was not set
*
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1dSTPFIDSet(L7_uint32 instance, L7_uint32 val);

/*********************************************************************
* @purpose  Check to see if the port is the appropriate type on which
*           the parm can be set
*
* @param    intIfNum          Internal interface number
*
* @returns  L7_SUCCESS        The parm can be set on this port type
* @returns  L7_NOT_SUPPORTED  The port does not support this parm to be set
*
* @notes
*                                 
* @end
*********************************************************************/
L7_RC_t dot1dPortParmCanSet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Check if an interface is valid for dot1d
*
* @param 	intIfNum    internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*                                 
* @end
*********************************************************************/
L7_BOOL dot1dIsValidIntf(L7_uint32 intIfNum);


#endif
