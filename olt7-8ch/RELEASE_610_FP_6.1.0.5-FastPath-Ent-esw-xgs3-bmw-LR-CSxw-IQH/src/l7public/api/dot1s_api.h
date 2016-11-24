/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  dot1s_api.h
*
* @purpose   Multiple Spanning tree APIs
*
* @component dot1s
*
* @comments 
*
* @create    9/19/2002
*
* @author    spetriccione
*
* @end
**********************************************************************/
#ifndef _DOT1SAPI_H_
#define _DOT1SAPI_H_

#include "dtlapi.h"
#include "dot1s_exports.h"

#define DOT1S_INVALID_HELLO_TIME 0

typedef struct dot1s_stateCB_s
{
  L7_ushort16 mpc;
  L7_RC_t rc;
}dot1s_stateCB_t;

/**************************************************************/
/* Defines for adaptive lag path cost calculation             */
/**************************************************************/

#define DOT1S_PATH_COST_LAG_CONSTANT        20000000000
#define DOT1S_DEFAULT_PATH_COST_WORSE       200000000
#define DOT1S_10M_PATH_COST                 2000000
#define DOT1S_100M_PATH_COST                200000
#define DOT1S_1G_PATH_COST                  20000
#define DOT1S_10G_PATH_COST                 2000
#define DOT1S_100G_PATH_COST                200
#define DOT1S_1T_PATH_COST                  20
#define DOT1S_10T_PATH_COST                 2
#define DOT1S_DEFAULT_PATH_COST_BETTER      0
#include "dot1s_exports.h"
#include "osapi.h"
/**************************************************************/
/* End changes for adaptive lag path cost calculation         */
/**************************************************************/

/*********************************************************************
* @purpose  Set the MSTP Force Protocol Version parameter	
*          
* @param    val @b{(input)} the new value of the MSTP parameter  
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sForceVersionSet(DOT1S_FORCE_VERSION_t val); 

/*********************************************************************
* @purpose  Get the value of the MSTP Force Protocol Version parameter	
*          
* @param    
*
* @returns  the current value of the MSTP parameter or L7_NULL in the
* @returns  event of an error
*
* @comments
*
* @end
*********************************************************************/
extern DOT1S_FORCE_VERSION_t dot1sForceVersionGet(); 

/*********************************************************************
* @purpose  Set the MSTP Operational Mode 
*          
* @param    val @b{(input)} the new value of the MSTP mode  
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sModeSet(L7_BOOL val); 

/*********************************************************************
* @purpose  Get the MSTP Operational Mode 
*          
* @param    
*
* @returns the current value of the MSTP mode  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_BOOL dot1sModeGet(); 

/*********************************************************************
* @purpose  Set the MSTP Configuration Format Selector 
*          
* @param    val @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sConfigFormatSelectorSet(L7_uint32 val); 

/*********************************************************************
* @purpose  Get the MSTP Configuration Format Selector 
*          
* @param    
*
* @returns the current value of the MSTP parameter
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_uchar8 dot1sConfigFormatSelectorGet(); 

/*********************************************************************
* @purpose  Set the MSTP Configuration Name
*          
* @param    buf @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sConfigNameSet(L7_uchar8 *buf); 

/*********************************************************************
* @purpose  Get the MSTP Configuration Name
*          
* @param    buf @b{(output)} the buffer where the Cfg Name is stored
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sConfigNameGet(L7_uchar8 buf[]);

/*********************************************************************
* @purpose  Set the MSTP Configuration Revision
*          
* @param    val @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sConfigRevisionSet(L7_uint32 val); 

/*********************************************************************
* @purpose  Get the MSTP Configuration Revision
*          
* @param    
*
* @returns  the current value of the MSTP parameter
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sConfigRevisionGet(); 

/*********************************************************************
* @purpose  Get the MSTP Configuration Digest Key
*          
* @param    
*
* @returns  a pointer to the current value of the MSTP parameter
* @returns  L7_NULLPTR bridge params not found
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_uchar8 *dot1sConfigDigestKeyGet(); 

/*********************************************************************
* @purpose  Set the MSTP Bridge Max Age parameter for the CIST
*          
* @param    val @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments the Bridge Max Age is stored as a multiple of 256
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sCistBridgeMaxAgeSet(L7_uint32 val);

/*********************************************************************
* @purpose  Set the MSTP Bridge parameters(Forward Delay,Hello time
            and Max age) for the CIST
*
* @param    fwdDelay,maxAge,helloTime @b{(input)} the new parameters of the MSTP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sCistBridgeParametersSet(L7_uint32 fwdDelay, L7_uint32 maxAge, L7_uint32 helloTime);
 

/*********************************************************************
* @purpose  Get the MSTP Bridge Max Age parameter for the CIST
*          
* @param    
*
* @returns  the current value of the MSTP parameter
*
* @comments the Bridge Max Age is stored as a multiple of 256
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sCistBridgeMaxAgeGet(); 

/*********************************************************************
* @purpose  Get the MSTP Root Port Max Age parameter for the CIST
*          
* @param    
*
* @returns  the current value of the MSTP parameter
*
* @comments the Root Port Max Age is stored as a multiple of 256
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sCistRootMaxAgeGet(); 

/*********************************************************************
* @purpose  Set the MSTP Bridge HelloTime parameter for the CIST
*          
* @param    val @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments the Bridge HelloTime is stored as a multiple of 256
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sCistBridgeHelloTimeSet(L7_uint32 val); 

/*********************************************************************
* @purpose  Get the MSTP Bridge HelloTime paramter for the CIST
*          
* @param    
*
* @returns  the current value of the MSTP parameter
*
* @comments the Bridge HelloTime is stored as a multiple of 256
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sCistBridgeHelloTimeGet(); 

/*********************************************************************
* @purpose  Get the MSTP Root Port HelloTime paramter for the CIST
*          
* @param    
*
* @returns  the current value of the MSTP parameter
*
* @comments the Root Port HelloTime is stored as a multiple of 256
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sCistRootHelloTimeGet(); 

/*********************************************************************
* @purpose  Set the MSTP Bridge Forward Delay for the CIST
*          
* @param    val @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments the Bridge Forward Delay is stored as a multiple of 256
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sCistBridgeFwdDelaySet(L7_uint32 val); 

/*********************************************************************
* @purpose  Get the MSTP Bridge Forward Delay for the CIST
*          
* @param    
*
* @returns  the current value of the MSTP parameter
*
* @comments the Bridge Forward Delay is stored as a multiple of 256
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sCistBridgeFwdDelayGet(); 

/*********************************************************************
* @purpose  Get the MSTP Root Port Forward Delay for the CIST
*          
* @param    
*
* @returns  the current value of the MSTP parameter
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sCistRootFwdDelayGet(); 

/*********************************************************************
* @purpose  Get the MSTP Bridge Hold Time for the CIST
*          
* @param    
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sCistBridgeHoldTimeGet(); 

/*********************************************************************
* @purpose  Set the MSTP Bridge Priority parameter in the specific
*           instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
* @param    val   @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE if val is out of range for a MSTP Bridge 
*                      Priority or the MSTID is not valid.
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sMstiBridgePrioritySet(L7_uint32 mstID, L7_uint32 val);

/*********************************************************************
* @purpose  Get the MSTP Bridge Priority parameter in the specific
*           instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  the current value of the MSTP parameter or L7_NULL if
*           not found.
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_ushort16 dot1sMstiBridgePriorityGet(L7_uint32 mstID); 

/*********************************************************************
* @purpose  Set the MSTP Administrative Edge Port parameter of a 
*           specific port for the CIST
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sCistPortEdgeSet(L7_uint32 intIfNum, L7_BOOL val); 

/*********************************************************************
* @purpose  Get the MSTP Administrative Edge Port parameter of a 
*           specific port for the CIST
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sCistPortEdgeGet(L7_uint32 intIfNum, L7_BOOL *val); 

/*********************************************************************
* @purpose  Get the MSTP Operational Edge Port parameter of a 
*           specific port for the CIST
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sCistPortOperEdgeGet(L7_uint32 intIfNum, L7_BOOL *val); 

/*********************************************************************
* @purpose  Get the MSTP Operational Point to Point Mac parameter 
*           of a specific port for the CIST
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP parameter
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_BOOL dot1sCistPortOperPointToPointGet(L7_uint32 intIfNum); 

/*********************************************************************
* @purpose  Force the specified port to transmit RSTP or MSTP BPDU's
*          
* @param    unitIndex @b{(input)} the unit for this operation
* @param    intIfNum  @b{(input)} interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sPortForceMigrationCheck(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Set the MSTP Port Administrative Mode variable
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments returns L7_FAILURE if the number of ports enabled will 
*           exceed the number that can be supported by MSTP.
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sPortAdminModeSet(L7_uint32 intIfNum, L7_BOOL val);

/*********************************************************************
* @purpose  Get the MSTP Port Administrative Mode variable
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP parameter. L7_TRUE for an
*           enabled port and L7_FALSE for a disabled port.
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_BOOL dot1sPortAdminModeGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Check if the port in a specific instance is valid for 
*           MSTP use.
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  L7_SUCCESS valid instance/port combination specified
* @returns  L7_FAILURE not a valid combination 
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sMstiPortCheck(L7_uint32 mstID, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the MSTP Forwarding State of the port in a specific
*           instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current forwarding state
*
* @comments an MSTID of zero will represent the CIST
*           Use dot1sPortForwardingCheck if it is required to know if this 
*           port is forwarding in any MSTP instance
*
* @end
*********************************************************************/
extern L7_uint32 dot1sMstiPortStateGet(L7_uint32 mstID, L7_uint32 intIfNum);

// PTin added
/*********************************************************************
* @purpose  Set MAC learning state in a port.
*          
* @param    intIfNum @b{(input)} interface number
*           learnEnable: Learn state (L7_TRUE or L7_FALSE)
*
* @returns  L7_SUCCESS change is sucessfull
* @returns  L7_FAILURE if error
*
* @end
*********************************************************************/
extern L7_RC_t dot1sPortMacLearningSet(L7_uint32 intIfNum, L7_BOOL learnEnable);

/*********************************************************************
* @purpose  Get MAC learning state in a port.
*          
* @param    intIfNum @b{(input)} interface number
*           learnEnable: Learn state (L7_TRUE or L7_FALSE)
*
* @returns  L7_SUCCESS change is sucessfull
* @returns  L7_FAILURE if error
*
* @end
*********************************************************************/
extern L7_RC_t dot1sPortMacLearningGet(L7_uint32 intIfNum, L7_BOOL *learnEnable);
// PTin end

/*********************************************************************
* @purpose  Check if this port is Forwarding in any MSTP instance or
*           Manual Forwarding by NIM when MSTP is disabled.
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns  L7_SUCCESS if forwarding in any instance or Nim Manual Forwarding
* @returns  L7_FAILURE if not forwarding in all instances
*
* @comments Use dot1sMstiPortStateGet if the state of a port in a specific 
*           instance is required. 
*
* @end
*********************************************************************/
extern L7_RC_t dot1sPortForwardingCheck(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the MSTP Port Role of the port in a specific
*           instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current forwarding state
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sMstiPortRoleGet(L7_uint32 mstID, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the MTSI associated with the specified VID
*          
* @param    VID @b{(input)} the desired VLAN ID
*
* @returns  the MSTID of the associated instance
*
* @comments All VLAN IDs by default will be associated with the CIST
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sVlanToMstiGet(L7_uint32 VID); 

/*********************************************************************
* @purpose  Set the MSTP Port Path Cost in a specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*           Setting the Port Path Cost to '0' or 'auto' will also cause
*           the Port Path Cost mode to be set to L7_TRUE. Setting it to 
*           any other value will set the Port Path Cost mode to L7_FALSE.
*
* @end
*********************************************************************/
extern L7_RC_t dot1sMstiPortPathCostSet(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 val); 

/*********************************************************************
* @purpose  Get the MSTP Port Path Cost in a specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP parameter
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sMstiPortPathCostGet(L7_uint32 mstID, L7_uint32 intIfNum); 

/*********************************************************************
* @purpose  Get the MSTP Port Path Cost Mode in a specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP parameter
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_BOOL dot1sMstiPortPathCostModeGet(L7_uint32 mstID, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Set the MSTP Port Priority in a specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sMstiPortPrioritySet(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get the MSTP Port Priority in a specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP parameter
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sMstiPortPriorityGet(L7_uint32 mstID, L7_uint32 intIfNum); 

/*********************************************************************
* @purpose  Get the MSTP Port Identifier of a specific port in a
*           specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the port ID
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sMstiPortIDGet(L7_uint32 mstID, L7_uint32 intIfNum); 

/*********************************************************************
* @purpose  Create a MSTP instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments total created should not exceed L7_MAX_MULTIPLE_STP_INSTANCES
*           0 is not allowed as an mstID
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sMstiCreate(L7_uint32 mstID); 

/*********************************************************************
* @purpose  Delete a MSTP instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments 0 is not allowed as an mstID
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sMstiDelete(L7_uint32 mstID); 

/*********************************************************************
* @purpose  Add an association between a VLAN and an MSTP instance
*          
* @param    mstID  @b{(input)} the MSTID for the desired MTSI
* @param    vlanID @b{(input)} the VLAN ID
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments The VLAN ID will no longer be associated with the CIST.
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sMstiVlanAdd(L7_uint32 mstID, L7_uint32 vlanID); 

/*********************************************************************
* @purpose  Remove an association between a VLAN and an MSTP instance
*          
* @param    mstID  @b{(input)} the MSTID for the desired MSTI
* @param    vlanID @b{(input)} the VLAN ID
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments The VLAN ID will again be associated with the CIST.
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sMstiVlanRemove(L7_uint32 mstID, L7_uint32 vlanID); 

/*********************************************************************
* @purpose  Check if the specified MSTP instance exists
*          
* @param    mstID  @b{(input)} MSTP instance number
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sInstanceGet(L7_uint32 mstID);

/*********************************************************************
* @purpose  Get the first MSTP instances number
*          
* @param    
*
* @returns  the first MSTP instance number
*
* @comments The first MSTP instance will always be the CIST
*
* @end
*********************************************************************/
extern L7_uint32 dot1sInstanceFirstGet();

/*********************************************************************
* @purpose  Get the next MSTP instances ID in sorted order
*          
* @param    prevInstNumber @b{(input)} previous instance number
*
* @returns  the next MSTP instance number or L7_NULL if none
*
* @comments the first MSTP instance ID will be returned if L7_NULL is
*           specified as the prevInstNumber
*           The CIST ID will not be returned from this function.
*
* @end
*********************************************************************/
extern L7_uint32 dot1sInstanceNextGet(L7_uint32 prevInstNumber);

/*********************************************************************
* @purpose  List all of the created MSTP instances
*          
* @param    buf @b{(output)} pointer to a buffer for a list of FIDs
* @param    val @b{(output)} count of the actual number of FIDs in the list
*
* @returns  L7_SUCCESS
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sInstanceList(L7_uint32 buf[], L7_uint32 *val);

/*********************************************************************
* @purpose  Get the first of the Forwarding Database ID's associated 
*           with the specified MSTP instance ID
*          
* @param    mstID @b{(input)} the MSTP instance in question
*
* @returns  the first FID associated with the MSTP 
*           instance ID or L7_NULL if none found.
*
* @comments an MSTID of zero will represent the CIST
*           VIDs and FIDs have a one to one mapping in regard to MSTP
*
* @end
*********************************************************************/
extern L7_uint32 dot1sMstiFIDFirstGet(L7_uint32 mstID);

/*********************************************************************
* @purpose  Get the next of the Forwarding Database ID's associated 
*           with the specified MSTP instance ID
*          
* @param    prevFID @b{(input)} previous FID
* @param    mstID   @b{(input)} the MSTP instance in question
*
* @returns  the next FID or L7_NULL if no more found
*
* @comments an MSTID of zero will represent the CIST
            VIDs and FIDs have a one to one mapping in regard to MSTP
*
* @end
*********************************************************************/
extern L7_uint32 dot1sMstiFIDNextGet(L7_uint32 mstID, L7_uint32 prevFID);

/*********************************************************************
* @purpose  List all of the Forwarding Database ID's associated with 
*           an MSTP instance
*          
* @param    mstID @b{(input)}  the MSTID for the desired MSTI
* @param    buf   @b{(output)} pointer to a buffer for a list of FIDs
* @param    val   @b{(output)} count of the actual number of FIDs in the list
*
* @returns  L7_SUCCESS  
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sMstiFIDList(L7_uint32 mstID, L7_uint32 buf[], L7_uint32 *val);

/*********************************************************************
* @purpose  Get the first of the VLAN ID's associated with the 
*           specified MSTP instance ID
*          
* @param    mstID @b{(input)} the MSTP instance in question
*
* @returns  the first VID associated with the MSTP 
*           instance ID or L7_NULL if none found.
*
* @comments an MSTID of zero will represent the CIST
*           VIDs and FIDs have a one to one mapping in regard to MSTP
*
* @end
*********************************************************************/
extern L7_uint32 dot1sMstiVIDFirstGet(L7_uint32 mstID);

/*********************************************************************
* @purpose  Get the next of the VLAN ID's associated with the 
*           specified MSTP instance ID
*          
* @param    prevVID @b{(input)} previous VID
* @param    mstID   @b{(input)} the MSTP instance in question
*
* @returns  the next VID, or L7_NULL if no more found
*
* @comments an MSTID of zero will represent the CIST
*           VIDs and FIDs have a one to one mapping in regard to MSTP
*
* @end
*********************************************************************/
extern L7_uint32 dot1sMstiVIDNextGet(L7_uint32 mstID, L7_uint32 prevVID);

/*********************************************************************
* @purpose  List all of the VLAN ID's associated with an MSTP instance
*          
* @param    mstID @b{(input)}  the MSTID for the desired MTSI
* @param    buf   @b{(output)} pointer to a buffer for a list of VIDs
* @param    val   @b{(output)} count of the actual number of VIDs in the list
*
* @returns  L7_SUCCESS  
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sMstiVIDList(L7_uint32 mstID, L7_ushort16 buf[], L7_uint32 *val); 

/*********************************************************************
* @purpose  Get the Base Mac Address
*          
* @param    buf @b{(output)} Base Mac Address
*
* @returns  
*
* @comments
*
*
* @end
*********************************************************************/
extern void dot1sBaseMacAddrGet(L7_uchar8 *buf);

/*********************************************************************
* @purpose  Get the MSTP Bridge Identifier in a specific instance
*          
* @param    mstID @b{(input)}  the MSTID for the desired MTSI
* @param    buf   @b{(output)} Bridge Identifier
* @param    len   @b{(output)} length of the Bridge Identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sMstiBridgeIdentifierGet(L7_uint32 mstID, L7_uchar8 *buf, L7_uint32 *len); 

/*********************************************************************
* @purpose  Get the MSTP CIST Root Bridge Identifier
*          
* @param    buf @b{(output)} Bridge Identifier
* @param    len @b{(output)} length of the Bridge Identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sCistRootIDGet(L7_uchar8 *buf, L7_uint32 *len);

/*********************************************************************
* @purpose  Get the MSTP Regional Root Bridge Identifier in a specific 
*           instance
*          
* @param    mstID @b{(input)}  the MSTID for the desired MTSI
* @param    buf   @b{(output)} Bridge Identifier
* @param    len   @b{(output)} length of the Bridge Identifier
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sMstiRegionalRootIDGet(L7_uint32 mstID, L7_uchar8 *buf, L7_uint32 *len);

/*********************************************************************
* @purpose  Get the MSTP CIST Root Path Cost
*          
* @param    
*
* @returns current value of the Path Cost
*
* @comments
*
* @end
*********************************************************************/
extern L7_uint32 dot1sCistRootPathCostGet();

/*********************************************************************
* @purpose  Get the MSTP Regional Root Path Cost in a specific 
*           instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  current value of the Path Cost or L7_NULL if the specified
*           instance is not found.
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sMstiRegionalRootPathCostGet(L7_uint32 mstID);

/*********************************************************************
* @purpose  Get the MSTP Time since the last Topology Change in a
*           specific instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  time in seconds since last TCN
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sMstiTimeSinceTopologyChangeGet(L7_uint32 mstID); 

/*********************************************************************
* @purpose  Get the MSTP count of Topology Changes in a specific 
*           instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  number of TCNs received
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sMstiTopologyChangeCountGet(L7_uint32 mstID); 

/*********************************************************************
* @purpose  Get the MSTP Topology Change parameter in a specific
*           instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  the current value of the TCN parm
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_BOOL dot1sMstiTopologyChangeParmGet(L7_uint32 mstID); 

/*********************************************************************
* @purpose  Get the MSTP Topology Change Acknowledge parameter for a
*           specific port in the CIST
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP parm
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_BOOL dot1sCistPortTopologyChangeAckGet(L7_uint32 intIfNum); 

/*********************************************************************
* @purpose  Get the MSTP Root Port ID in a specific instance
*          
* @param    mstID @b{(input)} the MSTID for the desired MTSI
*
* @returns  Port ID of the Root Port
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sMstiRootPortIDGet(L7_uint32 mstID); 

/*********************************************************************
* @purpose  Get the MSTP Designated Cost of a specific port in a 
*           specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns current value of the Path Cost
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sMstiDesignatedCostGet(L7_uint32 mstID, L7_uint32 intIfNum); 

/*********************************************************************
* @purpose  Get the MSTP Designated Bridge ID of a specific port in a 
*           specific instance
*          
* @param    mstID    @b{(input)}  the MSTID for the desired MTSI
* @param    intIfNum @b{(input)}  interface number
* @param    buf      @b{(output)} Bridge ID
* @param    len      @b{(output)} length of Bridge ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sMstiDesignatedBridgeIDGet(L7_uint32 mstID, L7_uint32 intIfNum, L7_uchar8 *buf, L7_uint32 *len);

/*********************************************************************
* @purpose  Get the MSTP Designated Port ID of a specific port in a 
*           specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the value of the port ID
*
* @comments an MSTID of zero will represent the CIST
*
*
* @end
*********************************************************************/
extern L7_ushort16 dot1sMstiDesignatedPortIDGet(L7_uint32 mstID, L7_uint32 intIfNum); 

/*********************************************************************
* @purpose  Get the MSTP Statistics for STP BPDU's Received on a 
*           specific port
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP stat  
*
* @returns  the current value of the MSTP stat  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sPortStatsSTPBPDUsReceivedGet(L7_uint32 intIfNum); 

/*********************************************************************
* @purpose  Get the MSTP Statistics for RSTP BPDU's Received on a 
*           specific port
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP stat  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sPortStatsRSTPBPDUsReceivedGet(L7_uint32 intIfNum); 

/*********************************************************************
* @purpose  Get the MSTP Statistics for MSTP BPDU's Received on a 
*           specific port
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns the current value of the MSTP stat  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sPortStatsMSTPBPDUsReceivedGet(L7_uint32 intIfNum); 

/*********************************************************************
* @purpose  Get the MSTP Statistics for STP BPDU's Sent on a specific
*           port
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns the current value of the MSTP stat  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sPortStatsSTPBPDUsSentGet(L7_uint32 intIfNum); 

/*********************************************************************
* @purpose  Get the MSTP Statistics for RSTP BPDU's Sent on a specific
*           port
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP stat  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sPortStatsRSTPBPDUsSentGet(L7_uint32 intIfNum); 

/*********************************************************************
* @purpose  Get the MSTP Statistics for MSTP BPDU's Sent on a specific
*           port
*          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP stat  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sPortStatsMSTPBPDUsSentGet(L7_uint32 intIfNum); 

/*********************************************************************
* @purpose  Get the MSTP Statistics for the number of times this interface 
*           has transitioned into loop inconsistent state
* 
* @param    mstID    @b((input)) MST Instance ID          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the Loop Inconsistent State Start stat  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sPortStatsTransitionsIntoLoopInconsistentStateGet(L7_uint32 mstID, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the MSTP Statistics for the number of times this interface 
*           has transitioned out of loop inconsistent state
* 
* @param    mstID    @b((input)) MST Instance ID          
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the Loop Inconsistent State End stat  
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_uint32 dot1sPortStatsTransitionsOutOfLoopInconsistentStateGet(L7_uint32 mstID, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Register a routine to be called when a link state changes.
*
* @param    registrar_ID   @b{(input)} routine registrar id  (See L7_COMPONENT_ID_t)      
* @param    *notify        @b{(input)} pointer to a routine to be invoked for link state     
*                                      changes.  Each routine has the following parameters:
*                                      (internal interface number, event(L7_UP, L7_DOWN,
*                                      etc.)). 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*                                 
* @end
*********************************************************************/
extern L7_RC_t dot1sRegisterIntfChange(L7_COMPONENT_IDS_t registrar_ID, 
                                       L7_RC_t (*notify)(L7_uint32 mstID, 
														   L7_uint32 intIfNum, 
														   L7_uint32 event));

/*********************************************************************
* @purpose  Set the MSTP Path Cost per port speed setting or continue 
*           to use the current configured path cost
*
* @param    intIfNum @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function will also zero out the path cost if auto
*           calculate is enabled and the port is disabled.
*
*       
* @end
*********************************************************************/
extern L7_RC_t dot1sCalcPortPathCost(L7_uint32 intIfNum);

/*********************************************************************
* @purpose 	Check if a particular MSTP instance ID is in use
*
* @param    instNumber	@b{(input)} Instance ID  
*
* @returns  L7_SUCCESS 	if the instance number is found and inUse is L7_TRUE  
* @returns  L7_FAILURE 	if the instance number is not found
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t	dot1sInstCheckInUse(L7_uint32 mstID);

/*********************************************************************
* @purpose 	Synchronize with the dot1s Queue.
*
* @param    @b{(input)} None 
*
* @returns  L7_SUCCESS 	  
* @returns  L7_FAILURE 	
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_RC_t	dot1sApiQueueSynchronize();

/********************************************************************
* @purpose 	Check if this intIf number is a member of a vlan associated
*			with the specified instance.
*
* @param    instId	 @b{(input)} Instance ID  
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_BOOL dot1sIsIntIfMemberOfInstanceVlans(L7_uint32 instId, 
                                                 L7_uint32 intIfNum);

/*********************************************************************
* @purpose 	Check if this int if number is forwarding or learning in any instance
*			other than the one supplied.
*
* @param    instId	 @b{(input)} Instance ID  
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS 	if the instance number is found and inUse is L7_TRUE  
* @returns  L7_FAILURE 	if the instance number is not found
*
* @comments
*
*
* @end
*********************************************************************/
extern L7_BOOL dot1sIsFwdInAnyOtherInst(L7_uint32 instId, 
                                 L7_uint32 intIfNum);
/*********************************************************************
* @purpose  callback function for interface state query 
*          
* @param    intfNum    @b{(input)} Internal Interface Number
* @param    dtlEvent   @b{(input)} DTL_EVENT_t DTL_EVENT_ADDR_INTF_MAC_QUERY
* @param    data       @b{(input)} pointer to MSTP instance ID 
* @param    stateQurey @b{(input)} pointer to bool to set on exit 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*********************************************************************/
extern L7_RC_t dot1sInstStateQueryCallback(L7_uint32 intIfNum, 
									       DTL_EVENT_t dtlEvent, 
									       void *data, 
									       L7_BOOL *stateQuery);

/*********************************************************************
* @purpose  Notify MSTP of a change to the Mac Address. 
*          
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments If the Base Mac Address has been locally admistrated or 
*           switched back to the burned in value, dot1s needs to be 
*           notified.
*
* @end
*********************************************************************/
extern L7_RC_t dot1sMacAddrChangeNotify();

/*********************************************************************
* @purpose  Clears the port statistics. 
*          
* @param    intIfNum @b{(input)} Internal Interface Number of the port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Clears all the BPDU stats associated with this port. (STP RSTP & MSTP)
*
* @end
*********************************************************************/
extern L7_RC_t dot1sPortStatsClear(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the MSTP Designated Root ID of a specific port in a 
*           specific instance
*          
* @param    mstID    @b((input))  the MSTID for the desired MTSI
* @param    intIfNum @b((input))  interface number
* @param    buf      @b((output)) Root Bridge ID
* @param    len      @b((output)) length of Bridge ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments an MSTID of zero will represent the CIST
*
* @end
*********************************************************************/
extern L7_RC_t dot1sMstiPortDesignatedRootIDGet(L7_uint32 mstID, L7_uint32 intIfNum, L7_uchar8 *buf, L7_uint32 *len);

/*********************************************************************
* @purpose  Get the CIST Regional Root ID of a specific port
*          
* @param    intIfNum @b((input))  interface number
* @param    buf      @b((output)) CIST Regional Root ID
* @param    len      @b((output)) length of Bridge ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t dot1sCistPortRegionalRootIDGet(L7_uint32 intIfNum, L7_uchar8 *buf, L7_uint32 *len);

/*********************************************************************
* @purpose  Get the MSTP Port Hello Time parameter of a 
*           specific port for the CIST
*          
* @param    intIfNum @b{(input)}  interface number
* @param    val      @b{(output)} pointer to hello time value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	Hello time is stored in multiples of 256
*
* @end
*********************************************************************/
extern L7_RC_t dot1sCistPortHelloTimeGet(L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the MSTP Port Path Cost parameter of a 
*           specific port for the CIST
*          
* @param    intIfNum @b{(input)}  interface number
* @param    val      @b{(output)} pointer to CIST Path Cost
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
extern L7_RC_t dot1sCistPortPathCostGet(L7_uint32 intIfNum, L7_uint32 *val);
/*********************************************************************
* @purpose  Get the MSTP Max Hop count parameter for the device
*          
* @param    val      @b{(output)} pointer to Max Hop Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
extern L7_RC_t dot1sBridgeMaxHopCountGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Set the MSTP Max Hop count parameter for the device
*          
* @param    val      @b{(output)} Max Hop Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
extern L7_RC_t dot1sBridgeMaxHopCountSet(L7_uint32 val);
/*********************************************************************
* @purpose  Get the MSTP Tx Hold count parameter for the device
*          
* @param    val      @b{(output)} pointer to Tx Hold Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
extern L7_RC_t dot1sBridgeTxHoldCountGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Set the MSTP Tx Hold count parameter for the device
*          
* @param    val      @b{(input)} Tx Hold Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
extern L7_RC_t dot1sBridgeTxHoldCountSet(L7_uint32 val);
/*********************************************************************
* @purpose  Get the Admin Hello Time for this port
*          
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} pointer to the adminHelloTime
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
extern L7_RC_t dot1sCistPortAdminHelloTimeGet(L7_uint32 intIfNum, 
									          L7_uint32 *val);
/*********************************************************************
* @purpose  Set the Admin Hello Time for this port
*          
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(input)} hello time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
extern L7_RC_t dot1sCistPortAdminHelloTimeSet(L7_uint32 intIfNum, 
									          L7_uint32 val);
/*********************************************************************
* @purpose  Get the CIST External Path Cost for this port
*          
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(output)} pointer to path cost
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	Use dot1sMstiPortPathCostGet with mstid 0 to get the 
*           internal port path cost for the CIST
*
* @end
*********************************************************************/
extern L7_RC_t dot1sCistPortExternalPathCostGet(L7_uint32 intIfNum,
										        L7_uint32 *val);
/*********************************************************************
* @purpose  Set the CIST External Path Cost for this port
*          
* @param    intIfNum  @b{(input)} interface number
* @param    val       @b{(input)} path cost
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	Use dot1sMstiPortPathCostSet with mstid 0 to set the 
*           internal port path cost for the CIST
*
* @end
*********************************************************************/
extern L7_RC_t dot1sCistPortExternalPathCostSet(L7_uint32 intIfNum,
										        L7_uint32 val);
/*********************************************************************
* @purpose  Gets the ports begin reset state machine time
*          
* @param    p @b{(output)} pointer to port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments	
*
* @end
*********************************************************************/
extern L7_RC_t dot1sPortStateMachineUpTimeGet(L7_uint32 intIfNum, 
											  L7_timespec *val);
/*********************************************************************
* @purpose  Get CIST External Port Path Cost Mode in a specific instance
*          
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
*
* @returns  the current value of the MSTP parameter
*
* @comments Use the dot1sMstiPortPathCostModeGet for the cist internal 
*           path cost mode.
*
*
* @end
*********************************************************************/
extern L7_BOOL dot1sCistExtPortPathCostModeGet(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Causes the switch to change state by telling DTL and NIM.
*
* @param    mstID    @b((input)) user assigned instance number  
* @param    intIfNum @b{(input)} interface to change the state of
* @param    state    @b{(input)} state to change to
*
* @returns  
*
* @comments     
*       
* @end
*********************************************************************/
extern void dot1sIhSetPortState(L7_uint32 mstID, L7_uint32 intIfNum, L7_uint32 state);

/*********************************************************************
* @purpose  Determine if the interface type is valid in dot1s
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL dot1sIsValidIntfType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid in dot1s
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL dot1sIsValidIntf(L7_uint32 intIfNum);


/*********************************************************************
* @purpose  Get the MSTP Operational Auto Edge parameter of a 
*           specific port
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sAutoEdgeGet(L7_uint32 intIfNum, L7_BOOL *val);

/*********************************************************************
* @purpose  Set the MSTP Auto Edge Port parameter of a 
*           specific port.
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sAutoEdgeSet(L7_uint32 intIfNum, L7_BOOL val);

/*********************************************************************
* @purpose  Get the MSTP Administrative value or the restricted role parameter
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sRestrictedRoleGet(L7_uint32 intIfNum, L7_BOOL *val);

/*********************************************************************
* @purpose  Set the MSTP Administrative value or the restricted role parameter
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* @returns  L7_ERROR  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sRestrictedRoleSet(L7_uint32 intIfNum, L7_BOOL val);

/*********************************************************************
* @purpose  Get the MSTP Administrative value of the loopguard parameter
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sLoopGuardGet(L7_uint32 intIfNum, L7_BOOL *val);

/*********************************************************************
* @purpose  Set the MSTP Administrative value of the loopguard parameter
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* @returns  L7_ERROR  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sLoopGuardSet(L7_uint32 intIfNum, L7_BOOL val);

/*********************************************************************
* @purpose  Get the MST Loop Inconsistent state of the port in a specific instance
*
* @param    mstID    @b{(input)} the MSTID for the desired MTSI
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sMstiPortLoopInconsistentStateGet(L7_uint32 mstID, L7_uint32 intIfNum, L7_BOOL *val);

/*********************************************************************
* @purpose  Get the MSTP Administrative value or the restricted TCN parameter
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(output)} the current value of the MSTP parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sRestrictedTcnGet(L7_uint32 intIfNum, L7_BOOL *val);

/*********************************************************************
* @purpose  Set the MSTP Administrative value or the restricted TCN parameter
*          
* @param    intIfNum @b{(input)} interface number
* @param    val      @b{(input)} the new value of the MSTP parameter
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*
*
* @end
*********************************************************************/
L7_RC_t dot1sRestrictedTcnSet(L7_uint32 intIfNum, L7_BOOL val);

/*********************************************************************
* @purpose  Set the BPDU Guard for the bridge
*
* @param    val      @b{(input)} the new value of the STP BPDU Guard
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sBpduGuardSet(L7_BOOL val);

/*********************************************************************
* @purpose  Get the BPDU Guard for the bridge
*
* @param    val      @b{(output)} the new value of the STP BPDU Guard
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sBpduGuardGet(L7_BOOL *val);

/*********************************************************************
* @purpose  Get the BPDU Guard effect for a port
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    val      @b{(output)} the new value of the STP BPDU Guard
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sBpduGuardEffectGet(L7_uint32 intIfNum,L7_BOOL *val);

/*********************************************************************
* @purpose  Set the BPDU filter for all the edge ports
*
* @param    val      @b{(input)} the new value of the BPDU filter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sBpduFilterSet(L7_BOOL val);

/*********************************************************************
* @purpose  Set the BPDU filter for a port
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    val      @b{(input)} the new value of the BPDU filter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sIntfBpduFilterSet(L7_uint32 intIfNum, L7_BOOL val);

/*********************************************************************
* @purpose  Get the BPDU filter for the switch
*
* @param    val      @b{(input)} the value of the BPDU filter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sBpduFilterGet(L7_BOOL *val);

/*********************************************************************
* @purpose  Get the BPDU filter for a port
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    val      @b{(input)} the value of the BPDU filter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sIntfBpduFilterGet(L7_uint32 intIfNum, L7_BOOL *val);

/*********************************************************************
* @purpose  Set the BPDU flood for a port
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    val      @b{(input)} the new value of the BPDU filter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sIntfBpduFloodSet(L7_uint32 intIfNum, L7_BOOL val);

/*********************************************************************
* @purpose  Get the BPDU flood for a port
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    val      @b{(input)} the value of the BPDU filter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t dot1sIntfBpduFloodGet(L7_uint32 intIfNum, L7_BOOL *val);
/*********************************************************************
* @purpose  Adaptive Path Cost Calculation for Link Aggregation Interfaces
*
* @param    intIfNum   @b{(input:}   interface number
* @param    *pathCost  @b{Output:}   pointer to adaptive pathCost
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/

extern L7_RC_t dot1sLagAutoPathCalculation(L7_uint32 intIfNum, L7_uint32 *pathCost);

/*********************************************************************
* @purpose Checks if the device is the root bridge.
*
* @param   mstID The MSTP instance ID
*
* @returns L7_TRUE  If the device is the root in the given instance
* @returns L7_FALSE  For all other cases
*
* @notes
*
* @end
*********************************************************************/
extern L7_BOOL dot1sIsBridgeRoot(L7_uint32 mstID);

#endif /* _DOT1SAPI_H_ */
