/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename dhcp_filtering_api.h
*
* @purpose DHCP Filtering API functions
*
* @component DHCP Filtering
*
* @comments none
*
* @create 07/18/2005
*
* @author sbasu
* @end
*
**********************************************************************/

#include "comm_mask.h"

/*********************************************************************
*
* @purpose  Gets the DHCP Filtering Admin mode
*
* @param    L7_uint32  *adminMode   @b((output)) DHCP Filtering admin mode
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpFilterAdminModeGet( L7_uint32 *adminMode );

/*********************************************************************
*
* @purpose  Sets the DHCP Filtering Admin mode
*
* @param    L7_uint32  adminMode   @b((input)) DHCP Filtering admin mode
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpFilterAdminModeSet( L7_uint32 adminMode );

/*********************************************************************
*
* @purpose  Gets the DHCP Filtering mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uchar8  *mode     @b((output)) DHCP Filtering intf mode
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpFilterIntfModeGet( L7_uint32 intIfNum, L7_uint32 *mode );

/*********************************************************************
*
* @purpose  Sets the DHCP Filtering mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((output)) Internal interface number
* @param    L7_uint32  mode      @b((input))  DHCP Filtering intf mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface was invalid or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dhcpFilterIntfModeSet( L7_uint32 intIfNum, L7_uint32 mode );

/*********************************************************************
*
* @purpose  Given an interface, get the next interface that is enabled 
*           for DHCP Filtering
*
* @param    L7_uint32   intIfNum         @b((output)) interface Number
* @param    L7_uint32   *nextIntIfNum    @b((output)) Pointer to next 
*                                           internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t dhcpFilterIntfEnabledGetNext ( L7_uint32 intIfNum, L7_uint32 *nextIntIfNum);

/*********************************************************************
* @purpose  Determine if the interface is valid in DHCP Filtering
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
L7_BOOL dhcpFilterIntfIsValid( L7_uint32 intIfNum );

/*********************************************************************
* @purpose  Checks to see if the interface type is valid to perticipate 
*           dhcp Filtering
*                    
* @param    sysIntfType   @b{(input)} interface type
*
* @returns  L7_TRUE 
* @returns  L7_FALSE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t dhcpFilterIntfTypeIsValid(L7_uint32 sysIntfType);


/*********************************************************************
* @purpose   get all the Trusted ports which are members of
*           tht DHCP Filtering*                    
* @param    *intIfMask      @b{(output)}   Mask specifying the member ports
*
* @returns  L7_TRUE 
* @returns  L7_FALSE
*
* @comments
*        
* @end
*********************************************************************/
L7_RC_t dhcpFilterIntfMaskGet(L7_INTF_MASK_t *intIfMask);
/* L7_RC_t dhcpFilterIntfMaskGet(L7_INTF_MASK_t *intIfMask); */


