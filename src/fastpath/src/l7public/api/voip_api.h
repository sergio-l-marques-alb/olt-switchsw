/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename  voip_api.h
*
* @purpose    Contains definitons for VOIP API's
*
* @component  VOIP
*
* @comments   This file contains all the defines, function prototypes
*             and include files required by voip_api.c
*
* @create     05/12/2007
*
* @author     aprashant
* @end
*
**********************************************************************/

#ifndef VOIP_API_H
#define VOIP_API_H

#include "l7_common.h"
#include "voip.h"

/*********************************************************************
* @purpose  Determine if the interface type is valid for VOIP 
*           configuration
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
L7_BOOL voipIsValidIntfType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid for VOIP configuration 
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
L7_BOOL voipIsValidIntf(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Gets the status of VOIP profile on interface
*
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                       enabled or disabled
* @param  L7_uint32 *mode     (output)  Mode value
*
* @param  L7_uint32 *bandwidth (output)  Bandwidth value
*
* @param  L7_uint32 *CosQueue (output)  Cos Queue
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipIntfVoipProfileGet(L7_uint32 interface, L7_uint32 *mode, 
                               L7_uint32 *bandwidth, L7_uint32 *CosQueue);

/*********************************************************************
* @purpose  Enables or disables the VOIP profile on interface
*
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      enabled or disabled
* @param  L7_uint32 mode      (input)  Mode value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipIntfVoipModeSet(L7_uint32 interface, L7_uint32 mode);

/*********************************************************************
* @purpose  Gets the Mode of VOIP profile on interface
*
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      be checked 
* @param  L7_uint32 *mode     (output)  Mode value
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipIntfVoipModeGet(L7_uint32 interface, L7_uint32 *mode);

/*********************************************************************
* @purpose  Sets the VOIP profile Min Bandwidth on the interface
*
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      enabled or disabled
* @param  L7_uint32 bandwidth (input)  Bandwidth value
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipIntfVoipMinBandwidthSet(L7_uint32 interface, L7_uint32 bandwidth);

/*********************************************************************
* @purpose  Gets the Mode of VOIP profile Min bandwidth on the interface
*
* @param  L7_uint32 interface (input)  Interface on which mode needs to
*                                      be checked 
* @param  L7_uint32 *bandwidth (output)  Bandwidth value
*
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t voipIntfVoipMinBandwidthGet(L7_uint32 interface, L7_uint32 *bandwidth);

/*********************************************************************
*
* @purpose  Return VoIP Config entry
*
* @param  L7_uint32 interface (input)  Internal Interface
*                            
* @returns  VoIP Config entry or NULL
*
* @notes    given a intIfNum return the cfg structure pointer 
*
* @end
*********************************************************************/
voipIntfCfgData_t *voipIntfCfgEntryGet(L7_uint32 intIfNum);

#endif
 
