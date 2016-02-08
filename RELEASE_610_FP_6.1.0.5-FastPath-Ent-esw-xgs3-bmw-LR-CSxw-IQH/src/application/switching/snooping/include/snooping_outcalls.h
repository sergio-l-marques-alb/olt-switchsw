/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007-2006
*
**********************************************************************
* @filename  snooping_outcalls.h
*
* @purpose   Prototypes of snooping outcalls
*
* @component Snooping
*
* @comments
*
* @create    10/27/2006
*
* @author    drajendra 
*
* @end
*
**********************************************************************/
#ifndef SNOOPING_OUTCALLS_H
#define SNOOPING_OUTCALLS_H

#include "datatypes.h"
#include "commdefs.h"

/* Begin Function Prototypes */
/*********************************************************************
* @purpose  Determine if the interface type is valid for Snooping
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE   Valid interface type
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL snoopIsValidIntfType(L7_uint32 sysIntfType);
/*********************************************************************
* @purpose  Determine if the interface is valid for Snooping
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE   Supported interface
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL snoopIsValidIntf(L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Get next valid vlan id
*
* @param    vid      @b{(input)}  current vlan id
* @param    nextvid  @b{(output)} next valid vlan id
*
* @returns  L7_SUCCESS  When a valid vid is found
* @returns  L7_FAILURE  no more vlans
*
* @comments none
*       
* @end
*********************************************************************/
L7_RC_t snoopNextVlanGet(L7_uint32 vid, L7_uint32 *nextvid);
/*********************************************************************
* @purpose  Check if VLAN is eligible for snooping configuration
*
* @param    vid      @b{(input)}  VLAN ID in question
*
* @returns  L7_SUCCESS  Valid VLAN
* @returns  L7_FAILURE  Invalid VLAN for configuration
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t snoopVlanCheckValid(L7_uint32 vid);
/*********************************************************************
* @purpose  Checks if the interface/VLAN ID can be enabled for Snooping
*
* @param    intIfNum  @b{(input)} Internal Interface Number
* @param    vlanId    @b{(input)} VLAN ID
*
* @returns  L7_TRUE, if interface can be enabled
* @returns  L7_FALSE, if interface cannot be enabled
*
* @notes    Acceptability criteria
*              1. Must not be the mirror dest port   &&
*              2. Must not be a LAG member           &&
*              3. Must not be enabled for routing    &&
*              4. Must be in the Forwarding state
*           This function only checks the above criteria.  All other
*           checking (i.e. validity of interface number) can be done
*           in snoopIntfCheck() or snoopIntfValidate().  This function
*           can be used to determine when an interface can be configured,
*           but, not enabled.
*
*
* @end
*********************************************************************/
L7_BOOL snoopIntfCanBeEnabled(L7_uint32 intIfNum, L7_uint32 vlanId);

/* End Function Prototypes */
#endif /* SNOOPING_OUTCALLS_H */
