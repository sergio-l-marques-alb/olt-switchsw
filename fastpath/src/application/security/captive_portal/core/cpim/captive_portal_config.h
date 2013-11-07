
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
*
* @filename     captive_portal_config.h
*
* @purpose      Captive portal configuration code
*
* @component    captive portal
*
* @comments     none
*
* @create       07/09/2007
* 
* @author       dcaugherty
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_CAPTIVE_PORTAL_CONFIG_H
#define INCLUDE_CAPTIVE_PORTAL_CONFIG_H

#include "captive_portal_include.h"

/* 
 *  These next two structures will, eventually, be rolled into the cdpmOprData_t 
 *  structure, but only once Dan A. has merged (as of 7/12) his stuff into CPDM.
 *  For now, we make do with temporary placeholders.
 */

typedef struct cpOprStatus_s
{
  L7_CP_MODE_STATUS_t      cpMode;  
  L7_CP_MODE_REASON_t      cpModeReason; 
} cpOprStatus_t;

typedef struct cpWirelessIPAddr_s
{
#ifdef L7_ROUTING_PACKAGE
  L7_uint32                   cpIntIfNum;        /* Loopback/Routing interface */
  L7_uint32                   cpRoutingMode;     /* Global routing mode */
  L7_BOOL                     cpLoopBackIFInUse; /* loopback interface use status */
#endif /* L7_ROUTING_PACKAGE */
  L7_IP_ADDR_t                addr;          /* IP addr of wireless system */

} cpWirelessIPAddr_t;



/*********************************************************************
*
* @purpose  Allocate and initialize operational info block for
*           Captive Portal task
*
* @param    void
*           
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    Calls CpDeleteOprData if anything should fail.
*
* @end
*********************************************************************/
L7_RC_t cpNewOprData(void);


/*********************************************************************
*
* @purpose  Finalize and deallocate operational info block for
*           Captive Portal task
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void    cpDeleteOprData(void);


/*********************************************************************
*
* @purpose  Return IP address used by Captive Portal
*
* @param    L7_IP_ADDR_t - output parameter, filled if successful
*           
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    Should be protected by semaphore
*
* @end    
*********************************************************************/
L7_RC_t cpIPAddress(L7_IP_ADDR_t * pAddr);


/*********************************************************************
*
* @purpose  Allocate and initialize configuration info block for
*           Captive Portal task
*
* @param    void
*           
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    Calls CpDeleteCfgData if anything should fail.
*
* @end
*********************************************************************/
L7_RC_t cpNewCfgData(void);


/*********************************************************************
*
* @purpose  Finalize and deallocate configuration info block for
*           Captive Portal task
*
* @param    void
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void    cpDeleteCfgData(void);




/*********************************************************************
*
* @purpose  Checks for IP address availability for CP task
*           
*
* @param    pIpAddr        @b{(input/output)} current/new IP address
* @param    pIpMask        @b{(input/output)} current/new IP subnet mask
* @param    pRtrMode       @b{(input/output)} current/new routing mode
* @param    pIntIfNum      @b{(input/output)} current/new internal interface number
* @param    pLoopbackInUse @b{(input/output)} indicates if application current/new using loopback
* @param    pCode          @b{(output)} reason code to indicate why no IP address selected
*
* @returns  L7_RC_t - L7_SUCCESS if IP address retrieved,
*           L7_FAILURE otherwise.
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t cpIPAddressCheck(L7_IP_ADDR_t        * pIpAddr,
                         L7_uint32           * pIpMask,
                         L7_uint32           * pRtrMode,
                         L7_uint32           * pIntIfNum,
                         L7_BOOL             * pLoopbackInUse,
                         L7_CP_MODE_REASON_t * pCode );



/*********************************************************************
*
* @purpose  Gets operational mode of Captive Portal task
*
* @param    void
*
* @returns  L7_CP_MODE_STATUS_t - either L7_CP_MODE_ENABLED 
*           or L7_CP_MODE_DISABLED.
*
* @notes    Call should be protected by semaphore. 
*
* @end
*********************************************************************/
L7_CP_MODE_STATUS_t  cpGetOprMode(void);


/*********************************************************************
*
* @purpose  Get the Interface MAC address 
*
* intfMAC   @b{(output)} pointer to mac address with length L7_MAC_ADDR_LEN
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalCPMACAddressGet(L7_uchar8 *intfMAC);


/*********************************************************************
*
* @purpose  Sets operational mode of Captive Portal task via the
*           configured mode
*
* @param    L7_uint32 - either L7_ENABLE or L7_DISABLE
*
* @returns  L7_RC_t - either L7_SUCCESS or L7_FAILURE
*
* @notes    Call should be protected by semaphore. 
*
* @end
*********************************************************************/
L7_RC_t  cpSetOprMode(L7_uint32 enableOrDisable);

/*********************************************************************
*
* @purpose  Set the CP mode
*
* @param    L7_uint32 mode @b{(input)} CP global mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments NOT TO BE EXPORTED OUTSIDE CAPTIVE PORTAL
*
* @end
*
*********************************************************************/

L7_RC_t cpdmOprGlobalModeSet(L7_uint32 mode);


/*********************************************************************
*
* @purpose  Set the Interface MAC address 
*
* intfMAC   @b{(input)} pointer to mac address with length L7_MAC_ADDR_LEN
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalCPMACAddressSet(L7_uchar8 *intfMAC);

/*********************************************************************
*
* @purpose  Check to see if the Interface MAC address has been changed
*
* intfMAC   @b{(input)} pointer to mac address with length L7_MAC_ADDR_LEN
*
* @returns  L7_SUCCESS if the MAC address is same.
* @returns  L7_FAILURE otherwise.
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t cpdmGlobalCPMACAddressCheck(L7_uchar8 *intfMAC);


#endif  /* INCLUDE_CAPTIVE_PORTAL_CONFIG_H */
