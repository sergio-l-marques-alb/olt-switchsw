/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename vlan_port.h
*
* @purpose VLAN PORT Interface Manager NIM interactions
*
* @component VLAN PORT Interface Manager
*
* @comments
*
* @create 01/10/2012
*
* @author mruas
* @end
*
**********************************************************************/

#ifndef VLAN_PORT__H
#define VLAN_PORT__H

/*********************************************************************
* @purpose  Create a NIM instance for a VLAN_PORT interface
*
* @param    intfType    Interface type
* @param    intfId      Interface ID
* @param    pIntIfNum       Pointer to Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t vlan_port_intIfNum_create(L7_uint32 intfId, L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Delete an VLAN_PORT intf in NIM
*
* @param    intIfNum    internal interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes
*
* @end
*********************************************************************/
extern L7_RC_t vlan_port_intIfNum_delete(L7_uint32 intIfNum);

#endif /* VLAN_PORT__H */
