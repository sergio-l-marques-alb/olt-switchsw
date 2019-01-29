/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\public\usmdb_nim_api.h
*
* @purpose externs for USMDB layer
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


#ifndef USMDB_NIM_API_H
#define USMDB_NIM_API_H

#include "l7_common.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_common.h"

/*********************************************************************
*
* @purpose  Sets the link trap configuration of the specified interface.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
*                  L7_ALL_INTERFACES to apply to all
* @param val       L7_uint32 Link Trap Config 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Link Trap Configurations:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
/*extern L7_RC_t usmDbIfLinkUpDownTrapEnableSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);
*/
extern L7_RC_t usmDbSwPortCrtlEntryNext(L7_uint32 *UnitIndex, L7_uint32 *interface);





/*********************************************************************
* @purpose  Gets the management administrative state of the specified interface.
*
* @param    intIfNum Internal Interface Number
*
* @param    adminState admin state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIfMgmtAdminStateGet(L7_uint32 intIfNum, L7_uint32 *adminState);

/*********************************************************************
* @purpose  Gets the admin state of the specified interface.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number 
* @param    adminState  pointer to Admin State, 
*                       (@b{Returns: NIM_DISABLE,
*                                    NIM_ENABLE 
*                                    or NIM_DIAG_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIfAdminStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Sets the administrative state of the specified interface.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
*                  L7_ALL_INTERFACES to apply to all
* @param val      L7_uint32  Admin State
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfAdminStateSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);



/*********************************************************************
* @purpose  Gets the active state of the specified interface.
*
* @param    UnitIndex   L7_uint32   Unit Number
* @param    intIfNum    Internal    Interface Number 
* @param    val         pointer to Active State, 
*                       (@b{Returns: L7_ACTIVE,
*                                    or L7_INACTIVE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    An interface is considered active if it both has link up and is in forwarding state.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIfActiveStateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Sets the administrative state of all interfaces in the
*           specified slot.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param SlotNum   L7_uint32 the slot number
* @param val      L7_uint32  Admin State
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Admin States:
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfAdminStateSlotSet(L7_uint32 UnitIndex, L7_uint32 SlotNum, L7_uint32 val);


/*********************************************************************
* @purpose  Gets the next entry in the Switch port control table 
*
* @param *UnitIndex  a ptr to a L7_uint32 the unit for this operation
* @param *intIfNum   a ptr to L7_uint32 
*  
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbSwPortCtrlEntryNext(L7_uint32 *UnitIndex, L7_uint32 *intIfNum); 


/*********************************************************************
*
* @purpose  Returns the ifIndex associated with the 
*           internal interface number
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param *val      L7_uint32 pointer to the ifIndex 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfIndexGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);



/*********************************************************************
*
* @purpose  Gets the link configured state of the specified interface.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param *val      L7_uint32 pointer to link configured state 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Possible Link States returned:
* @table{@row{@cell{L7_PORTCTRL_PORTSPEED_AUTO_NEG}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100FX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_1000SX}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfSpeedGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Gets the status of the speed if congigured to auto.
*
* @param intIfNum  L7_uint32 the internal interface number
* @param *val      L7_uint32 pointer to link configured state
*
* @returns  L7_TRUE, if speed is configured to auto
* @returns  L7_FAILURE, if other failure
*
* @end
*********************************************************************/
L7_BOOL usmDbIsIntfSpeedAuto(L7_uint32 intIfNum);

/*********************************************************************

@purpose Gets the default type of the specified interface.

* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param *val      L7_uint32 pointer to link configured state
*
*@returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Possible Link States returned:
* @table{@row{@cell{L7_PORTCTRL_PORTSPEED_AUTO_NEG}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100FX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_1000SX}}}
*
* @end
*********************************************************************/

extern L7_RC_t usmDbIfDefaultSpeedGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
*
* @purpose  Gets the auto-negotiation status of the specified interface.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param *val      L7_uint32 pointer to auto-negotiation admin status
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Possible types returned: 
* @table{@row{@cell{L7_ENABLE}}
*        @row{@cell{L7_DISABLE}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfAutoNegAdminStatusGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Sets the speed of the specified interface 
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
*                  L7_ALL_INTERFACES to apply to all
* @param val       L7_uint32 port speed 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Port Speeds:
* @table{@row{@cell{L7_PORTCTRL_PORTSPEED_AUTO_NEG}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100FX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_1000SX}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfSpeedSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
*
* @purpose  Sets the default speed of the specified interface 
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
*                  L7_ALL_INTERFACES to apply to all
* @param val       L7_uint32 default port speed 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Unlike usmDbIfSpeedSet(), this function does not
*        call DTL to actually set the speed of the interface.
*        It only sets the ifSpeed field in the NIM port config structure.
*        Valid Port Speeds:
* @table{@row{@cell{L7_PORTCTRL_PORTSPEED_HALF_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100FX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_1000SX}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfDefaultSpeedSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
*
* @purpose  Gets the physical link data rate of the specified interface.
*
* @param UnitIndex  L7_uint32 the unit for this operation
* @param intIfNum   L7_uint32 the internal interface number
* @param *intfSpeed L7_uint32 pointer to an interface Data Rate 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes The data rate is determined from the ifSpeed encode maintained 
*        by NIM.  
*        Possible Link States returned:   
* @table{@row{@cell{L7_PORTCTRL_PORTSPEED_FULL_1000SX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100FX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_10T}}}  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfSpeedSimplexDataRateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *intfSpeed);

/*********************************************************************
*
* @purpose  Gets the physical link data rate of the specified interface.
*
* @param UnitIndex  L7_uint32 the unit for this operation
* @param intIfNum   L7_uint32 the internal interface number
* @param *intfSpeed L7_uint32 pointer to an interface Data Rate 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes The data rate is determined from the ifSpeed encode maintained 
*        by NIM.  Full duplex links are reported as having twice the
*        data rate as their half duplex counterparts.
*        Possible Link States returned:   
* @table{@row{@cell{L7_PORTCTRL_PORTSPEED_FULL_1000SX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100FX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_10T}}}  
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfSpeedDataRateGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *intfSpeed);

/*********************************************************************
* @purpose  Indicates whether a physical link is full duplex
*
* @param UnitIndex  L7_uint32 the unit for this operation
* @param intIfNum    Internal Interface Number 
*
* @returns  L7_TRUE     if link is full duplex
* @returns  L7_FALSE    if link is half duplex, or if interface is invalid
*
* @notes    none
*       
* @end
*********************************************************************/
extern L7_BOOL usmDbIsIntfSpeedFullDuplex(L7_uint32 UnitIndex, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Retrieves the duplex value of the port i.e., auto | full | half
*
* @param UnitIndex  L7_uint32 the unit for this operation
* @param intIfNum    Internal Interface Number
*
* @returns  L7_TRUE     if link is full duplex
* @returns  L7_FALSE    if link is half duplex, or if interface is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIfDuplexGet(L7_uint32 intIfNum,L7_uint32 *val);

/*********************************************************************
* @purpose  Retrieves the speed value of the port
*
* @param UnitIndex  L7_uint32 the unit for this operation
* @param intIfNum    Internal Interface Number
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbNimIntfSpeedGet(L7_uint32 intIfNum,L7_uint32 *val);

/*********************************************************************
*
* @purpose  Sets the speed of all interfaces in the specified slot.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param SlotNum   L7_uint32 the slot number
* @param val      L7_uint32  L7_uint32 port speed
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Port Speeds:
* @table{@row{@cell{L7_PORTCTRL_PORTSPEED_AUTO_NEG}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100FX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_1000SX}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfSpeedSlotSet(L7_uint32 UnitIndex, L7_uint32 SlotNum, L7_uint32 val);


/*********************************************************************
*
* @purpose Get the special port type
*          
* @param UnitIndex  L7_uint32 Unit Number
* @param i          L7_uint32 Internal interface number
* @param *val       L7_SPECIAL_PORT_TYPE_t pointer to the special port type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes Special Port Types:
* @table{@row{@cell{L7_PORT_MONITOR}@cell{Monitoring port}}
*        @row{@cell{L7_MONITORED_PORT}@cell{Port being monitored}}
*        @row{@cell{L7_TRUNK_MEMBER}@cell{Port is a member of a trunk}}
*        @row{@cell{L7_PORT_NORMAL}@cell{No special characteristics}}}
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbIfSpecialPortTypeGet(L7_uint32 UnitIndex, L7_uint32 i,L7_SPECIAL_PORT_TYPE_t *val);

/*********************************************************************
* @purpose  Set the interface in loopback mode.
*
* @param    intIfNum    Internal Interface Number 
* @param    mode        link state, L7_ENABLE,DISABLE?    
*                       (@b{  L7_PORTCTRL_LOOPBACK_MAC,
*                             L7_PORTCTRL_LOOPBACK_PHY,
*                             L7_PORTCTRL_LOOPBACK_NONE }}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
*
* @notes    None.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIfLoopbackModeSet(L7_uint32 intIfNum, L7_uint32 mode);


/*********************************************************************
* @purpose  Get the interface's loopback mode.
*
* @param    intIfNum    Internal Interface Number 
* @param    mode        link state    
*                       (@b{  L7_PORTCTRL_LOOPBACK_MAC,
*                             L7_PORTCTRL_LOOPBACK_PHY,
*                             L7_PORTCTRL_LOOPBACK_NONE }}
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
*
* @notes    None.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIfLoopbackModeGet(L7_uint32 intIfNum, L7_uint32 *mode);


/*********************************************************************
* @purpose  Gets the mtu size of the specified interface.
*
* @param    intIfNum    Internal Interface Number 
* @param    mtu         mtu size 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIfMtuSizeGet(L7_uint32 intIfNum, L7_uint32 *mtu);


/*********************************************************************
* @purpose  Gets the encapsulation type for the specified interface.
*
* @param    intIfNum    Internal Interface Number 
* @param    encapType   the encapsulation type 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIfEncapsulationTypeGet(L7_uint32 intIfNum, L7_uint32 *encapType);

/*********************************************************************
* @purpose  Sets the encapsulation type for the specified interface.
*
* @param    intIfNum    Internal Interface Number 
* @param    encapType   the encapsulation type 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIfEncapsulationTypeSet(L7_uint32 intIfNum, L7_uint32 encapType);


/* End R2 */

/*********************************************************************
* @purpose  Sets the (Layer 2) Maximum Framesize for the interface, 
*           based upon the type of interface and any submembers
*
* @param    intIfNum    Internal Interface Number
* @param    Maximum frame size of the interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfConfigMaxFrameSizeSet(L7_uint32 intIfNum, L7_uint32 maxSize);


/*********************************************************************
* @purpose  Sets either the burned-in or locally administered address
*           of the specified interface.
*
* @param    intIfNum    Internal Interface Number 
* @param    addrType    address type, 
*                       (@b{  NIM_BIA or
*                             NIM_LAA})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIfSetIntfAddr(L7_uint32 intIfNum, L7_uint32 addrType);

/*********************************************************************
* @purpose  Sets the locally administered address
*           of the specified interface.
*
* @param    intIfNum    Internal Interface Number 
* @param    macAddr     Pointer to LAA MAC Address.   
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfSetIntfLAA(L7_uint32 intIfNum, L7_uchar8 *macAddr);


/*********************************************************************
* @purpose  Set the promiscuous mode   
*
* @param    UnitIndex   Unit
* @param    intIfNum    Interface
* @param    mode        Mode           
*
* @returns  L7_FAILURE  all the time
*
* @notes    
* @end
*********************************************************************/
extern L7_RC_t usmDbIfPromiscuousModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose Determine if the port is in promiscous mode
*
* @param    UnitIndex   Unit
* @param    intIfNum    Interface
* @param    *mode       Status, L7_TRUE or L7_FALSE           
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    
* @end
*********************************************************************/
extern L7_RC_t usmDbIfPromiscuousModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *mode);


/*********************************************************************
*
* @purpose  Gets the physical link state of the specified interface.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param *val      L7_uint32 pointer to the physical link state 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Admin Speed States returned:
* @table{@row{@cell{L7_PORTCTRL_PORTSPEED_AUTO_NEG}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100FX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_1000SX}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfHighSpeedGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);


/*********************************************************************
*
* @purpose  Gets the physical link state of the specified interface.
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param intIfNum  L7_uint32 the internal interface number
* @param *val      L7_uint32 pointer to the physical link state 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes Valid Admin Speed States returned:
* @table{@row{@cell{L7_PORTCTRL_PORTSPEED_AUTO_NEG}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100TX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_HALF_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_10T}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_100FX}}
*        @row{@cell{L7_PORTCTRL_PORTSPEED_FULL_1000SX}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfSpeedStatusGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the negotiated Tx and Rx pause status of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    rxPauseStatus  L7_TRUE if autonegotiation result allows Rx pause
* @param    txPauseStatus  L7_TRUE if autonegotiation result allows Tx pause
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIfPauseStatusGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_BOOL *rxPauseStatus, L7_BOOL *txPauseStatus);

/*********************************************************************
*
* @purpose  Determine if a physical connector is present
*
* @param    UnitIndex   Unit
* @param    interface   interface number       
* @param    *val        returned value, L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS, if success
*                       
*           L7_ERROR, if interface does not exist
*           L7_FAILURE, other failure
*
* @notes   
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfConnectorPresentGet(L7_uint32 UnitIndex, L7_int32 Index, L7_int32 *val);


/*********************************************************************
*
* @purpose  Determine time of last reset
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    interface   L7_uint32 the internal interface number       
* @param    *ts        returns time of type usmDbTimeSpec_t
*
* @returns  L7_SUCCESS, if success
*           L7_ERROR, if interface does not exist
*           L7_FAILURE, other failure
*
* @notes   
*
* @end
*********************************************************************/
extern L7_RC_t usmDbTimeSinceLastStatsResetGet(L7_uint32 UnitIndex, L7_uint32 interface, usmDbTimeSpec_t *ts);


/*********************************************************************
*
* @purpose  Get the ifAlias of the object
*
* @param    UnitIndex   Unit
* @param    intIfNum    internal interface number       
* @param    *ifName        pointer to string to contain name
*
* @returns  L7_SUCCESS, if success
*                       
*           L7_ERROR, if interface does not exist
*           L7_FAILURE, other failure
*
* @notes    This object is not currently supported
*
* @end
*********************************************************************/
L7_RC_t usmDbIfAliasGet(L7_uint32 UnitIndex, L7_int32 intIfNum, L7_uchar8 *ifName);

/*********************************************************************
*
* @purpose  Get the ifTableLastChange of the object
*
* @param    UnitIndex   Unit
* @param    *ifLastChange        pointer to integer to contain value
*
* @returns  L7_SUCCESS, if success
*                       
*           L7_FAILURE, other failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbIfTableLastChangeGet(L7_uint32 UnitIndex, L7_uint32 *ifLastChange);

/*********************************************************************
*
* @purpose  Set the ifAlias of the object
*
* @param    UnitIndex   Unit
* @param    intIfNum    internal interface number       
* @param    *ifAlias    pointer to string to contain name
*
* @returns  L7_SUCCESS, if success
*                       
*           L7_ERROR, if interface does not exist
*           L7_FAILURE, other failure
*
* @notes    This object is not currently supported
*
* @end
*********************************************************************/
L7_RC_t usmDbIfAliasSet(L7_uint32 UnitIndex, L7_int32 intIfNum, L7_uchar8 *ifAlias);


/*********************************************************************
*
* @purpose  Get the ifTableLastChange of the object
*
* @param    UnitIndex   Unit
* @param    *ifLastChange        pointer to integer to contain value
*
* @returns  L7_SUCCESS, if success
*                       
*           L7_FAILURE, other failure
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbIfTableLastChangeGet(L7_uint32 UnitIndex, L7_uint32 *ifLastChange);


/*********************************************************************
*
* @purpose  Get ifCounterDiscontinuityTime
*
* @param    UnitIndex   Unit
* @param    intIfNum    internal interface number       
* @param    *val        pointer to string to contain name
*
* @returns  L7_SUCCESS, if success
*                       
*           L7_ERROR, if interface does not exist
*           L7_FAILURE, other failure
*
* @notes    This object is not currently supported
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfCounterDiscontinuityGet(L7_uint32 UnitIndex, L7_int32 intIfNum, L7_int32 *val);



/* start located in  usmdb_1213.c  look for the end of this stmt. */ 

/*********************************************************************
*
* @purpose  Returns a textual string containing information about the
*           interface
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    interface   interface
* @param    *buf        description
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIfDescrGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_char8 *buf);


/*********************************************************************
*
* @purpose  Returns the internal interface type (IANAifType)
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    interface   interface
* @param    *val        pointer to type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    The type of interface, distinguished according to
*           the physical/link protocol(s) immediately `below'
*           the network layer in the protocol stack
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIfTypeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

#if L7_FEAT_SF10GBT
/*********************************************************************
*
* @purpose  Gets the firmware revision of port transceiver. 
*           Currently applies only to 10GBaseT Solarflare 
*           SFX7101 transceiver
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    interface   @b{(input)} interface
* @param    *ifFwRevision  @b{(output)} string buffer containing FW revision, 
*                         should be at least 13 bytes long plus terminating zero char
* @param    *ifFwPartNumber  @b{(output)} string buffer containing FW revision, 
*                         should be at least 13 bytes long plus terminating zero char
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist, or operation is not supported 
* @returns  L7_FAILURE  if other failure
*
* @comments Valid only for 10G non-stacking ports with 10GBaseT 
*           Solarflare SFX7101 transceiver. If firmware is not present or being upgraded, 
*           corresponding status is written to output params
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfTransceiverFwRevisionStrGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uchar8 *ifFwRevision, L7_uchar8 *ifFwPartNumber);

/*********************************************************************
*
* @purpose  Gets the firmware revision of port transceiver. 
*           Currently applies only to 10GBaseT Solarflare 
*           SFX7101 transceiver
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    interface   @b{(input)} interface
* @param    *ifFwRevision  @b{(output)} string buffer containing FW revision, 
*                         should be at least 4 bytes long 
* @param    *ifFwPartNumber  @b{(output)} string buffer containing FW revision, 
*                         should be at least 8 bytes long 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist, or operation is not supported 
* @returns  L7_FAILURE  if other failure
*
* @comments Valid only for 10G non-stacking ports with 10GBaseT 
*           Solarflare SFX7101 transceiver
*
* @end
*********************************************************************/
L7_RC_t usmDbIfTransceiverFwRevisionGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uchar8 *ifFwRevision, L7_uchar8 *ifFwPartNumber);
#endif

/*********************************************************************
* @purpose  Gets either the burned-in or locally administered address
*           of the specified interface, as requested
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    interface   Internal Interface Number 
* @param    buf         pointer to MAC Address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIfPhysAddressGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_char8 *buf);


/*********************************************************************
* @purpose  Gets the operational status of the specified interface
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    interface   Internal Interface Number 
* @param    *val        pointer to value, set to L7_UP or L7_DOWN
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIfOperStatusGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);


/*********************************************************************
* @purpose  Gets the time of last link state change, in seconds
*
* @param    UnitIndex   L7_uint32 the unit for this operation
* @param    interface   Internal Interface Number 
* @param    *val        pointer to value, set to L7_UP or L7_DOWN
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIfLastChangeGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/* end located in  usmdb_1213.c */ 

extern L7_RC_t usmDbIfIndexSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 val);


/*********************************************************************
* @purpose  Gets either the burned-in or locally administered address
*           of the specified interface, as requested
*
* @param    intIfNum    Internal Interface Number 
* @param    addrType    address type requested (L7_SYSMAC_BIA, L7_SYSMAC_LAA,
*                       or L7_NULL) L7_NULL will return currently configured 
*                       MAC Address
* @param    macAddr     pointer to MAC Address,
*                       (@b{Returns: 6 byte mac address})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIfIntfAddressGet(L7_uint32 intIfNum, L7_uint32 addrType, L7_uchar8 *macAddr);

/*********************************************************************
* @purpose  Gets the L3 router MAC Address of the specified interface
*
* @param    intIfNum    Internal Interface Number
* @param    addrType    address type requested (L7_SYSMAC_BIA, L7_SYSMAC_LAA,
*                       or L7_NULL) L7_NULL will return currently configured
*                       MAC Address
* @param    macAddr     pointer to MAC Address,
*                       (@b{Returns: 6 byte mac address})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    L3 applications use the router MAC address as the source address.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfIntfL3MacAddressGet(L7_uint32 intIfNum, L7_uint32 addrType,
L7_uchar8 *macAddr);


/*********************************************************************
* @purpose  Return Internal Interface Number of the first valid interface for
*           the specified interface type.
*
* @param    unit        The Unit ID 
* @param    intIfNum    internal interface number of the switch
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbMgtSwitchintIfNumGet(L7_uint32 unit, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Return LAG Interface speed 
*
* @param    intIfNum    internal interface number of the switch
* @param    speed       interface speed 
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIfLAGSpeedGet(L7_uint32 intIfNum,L7_uint32 *speed );


/*********************************************************************
* @purpose  Returns the internal interface type
*           associated with the internal interface number
*
* @param    intIfNum    internal interface number
* @param    sysIntfType pointer to internal interface type,  
*                       (@b{Returns: Internal Interface Type}).
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t usmDbIntfTypeGet(L7_uint32 intIfNum, L7_uint32 *sysIntfType);

/*********************************************************************
* @purpose  Check the physical capability of this port
*
* @param    intIfNum    Internal interface number
* @param    capability  Pointer to port capability
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t usmDbIntfPhyCapabilityGet(L7_uint32 intIfNum, L7_uint32 *capability);

/*********************************************************************
* @purpose  Check the connector type of this port
*
* @param    intIfNum    Internal interface number
* @param    capability  Pointer to connector type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t usmDbIntfConnectorTypeGet(L7_uint32 intIfNum, L7_uint32 *connType);

/*********************************************************************
* @purpose  Gets the configured (Layer 2) Framesize for the interface, 
*           based upon the type of interface and any submembers
*
* @param    intIfNum    Internal Interface Number
* @param    maxSize     pointer to max frame size,  
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfConfigMaxFrameSizeGet(L7_uint32 intIfNum, L7_uint32 *maxSize);
/*********************************************************************
* @purpose  Gets the (Layer 2) Maximum Framesize for the interface, 
*           based upon the type of interface and any submembers
*
* @param    intIfNum    Internal Interface Number
* @param    maxSize     pointer to max frame size,  
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if other failure
*
* @notes    none 
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIfMaxFrameSizeGet(L7_uint32 intIfNum, L7_uint32 *maxSize);

/*********************************************************************
* @purpose  Get the status of the specified interface
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    interface   @b{(input)} internal interface number 
* @param    val         @b{(output)} pointer to state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIntfStatusGet(L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  Test whether a parameter is settable on an interface
*
* @param    intIfNum    Internal Interface Number
* @param    setParm     Mask of parameters
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  
*       
* @end
*********************************************************************/
extern L7_BOOL usmDbIntfParmCanSet(L7_uint32 intIfNum, L7_uint32 setParm);

/*********************************************************************
* @purpose  To get the bit offset value for the interface 
*
* @param    intIfNum    Internal Interface Number
* @param    setParm     Mask of parameters
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t usmDbConfigIdMaskOffsetGet( L7_uint32 intIfNum,L7_uint32 *maskOffset );

/*********************************************************************
* @purpose  Set whether the Captive Portal Supported. 
*
* @param    L7_uint32 intIfNum      Internal Interface Number  
* @param    L7_uint32 cpCap         CP Supported flag to be set
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t usmDbCpSupportedSet(L7_uint32 intIfNum, L7_uint32 cpCap);

/*********************************************************************
* @purpose  Get the Captive Portal Supported
*
* @param    L7_uint32 intIfNum        Internal Interface Number 
* @param    L7_uint32 *cpCap          CP Supported flag
*
* returns   L7_SUCCESS
* returns   L7_FAILURE
*
* @notes    none    
*
* @end
*********************************************************************/
L7_RC_t usmDbCpSupportedGet(L7_uint32 intIfNum, L7_uint32 *cpCap);

/**********************************************************************
*
* @purpose  Get auto-negotiation status and the negotiation capabilities
*           of the specified interface
*
* @param intIfNum  L7_uint32 the internal interface number
* @param *val      L7_uint32 pointer to negotiation capabilities that is
*                   a mask of ORed bit values from enum
*                   L7_PORT_NEGO_CAPABILITIES_t
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
***********************************************************************/
L7_RC_t usmDbIfAutoNegoStatusCapabilitiesGet(L7_uint32 intIfNum, L7_uint32 *val);

/*******************************************************************************
*
* @purpose  Enable/disable auto-negotiation and set the negotiation capabilities
*           of the specified interface
*
* @param intIfNum L7_uint32 the internal interface number
* @param val      L7_uint32 negotiation capabilities that is a mask of ORed bit
*                   values from enum L7_PORT_NEGO_CAPABILITIES_t
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*******************************************************************************/
L7_RC_t usmDbIfAutoNegoStatusCapabilitiesSet(L7_uint32 intIfNum, L7_uint32 val);

/*********************************************************************
* @purpose  Get the interface type from internal interface number
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} internal interface number
* @param    val         @b{(output)} pointer to type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbIntfIfTypeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *val);

/*****************************************************************************
* @purpose  Get the interface auto negotiation values
*
* @param    speed_mode  @b{(input)} whether speed is enabled for auto negotiation
* @param    duplex_mode @b{(input)} whether duplex is enabled for auto negotiation
* @param    intIfNum    @b{(input)} internal interface number
* @param    val         @b{(output)} pointer to type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*******************************************************************************/
L7_RC_t usmDbIntfAutoNegValueGet(L7_uint32 intfSpeed,L7_uint32 intIfNum, L7_uint32 *autoneg_val);

#endif
