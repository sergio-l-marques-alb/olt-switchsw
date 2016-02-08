/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename  broad_l2_vlan_mac.c
*
* @purpose   This file contains the entry points to the Layer 2 interface
*
* @component hapi vlanMac
*
* @comments
*
* @create    05/20/2005
*
* @author    tsrikanth,jflack,grantc
*
* @end
*
**********************************************************************/

#include "dapi.h"
#include "log.h"
#include "dapi_struct.h"
#include "broad_common.h"
#include "l7_usl_bcm_vlan_mac.h"

static int initialized = 0;

/*********************************************************************
* @purpose Adds an MAC based VLAN
*
* @param   *usp	    @b{(input)}  [unit/slot/port]
* @param   cmd      @b{(input)}  [DAPI_CMD_MAC_VLAN_CREATE]
* @param   *data    @b{(input)}  [Data for command operation]
* @param   *dapi_g  @b{(input)}  [The driver object]
*
* @returns L7_SUCCESS 
* @returns L7_FAILURE
*
* @notes   none
*
* @end
*
**********************************************************************/
L7_RC_t hapiBroadL2VlanMacEntryAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, 
                                  void *data,DAPI_t *dapi_g)
{
    int rc; 
    DAPI_QVLAN_MGMT_CMD_t *dapiCmd;

    /*
     * Prepare arguments
     *  a. MAC address
     *  c. vlan id
     *  d. priority
     */
    dapiCmd = (DAPI_QVLAN_MGMT_CMD_t *) data;
    if (initialized == 0)
    {
        rc = usl_mac_bcmx_vlan_control_port_set(BCMX_LPORT_ETHER_ALL,
                                                bcmVlanLookupMACEnable, 1);
        if (L7_BCMX_OK(rc) != L7_TRUE)
        {
            printf("Error calling  bcm_vlan_control_port_set: %d\n", rc); 
            return(L7_FAILURE);
        }
        initialized = 1;
    }

    rc = usl_bcmx_vlan_mac_add(dapiCmd->cmdData.macVlanConfig.mac.addr,
                           (bcm_vlan_t)dapiCmd->cmdData.macVlanConfig.vlanId, 0);

    if (L7_BCMX_OK(rc) != L7_TRUE)
    {
        /* Hash bucket full condition may occur even before table is full.
         * Silent the error for full condition and return error.
         */
        if (rc != BCM_E_FULL)
        {
           SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR, "\n%s %d: Failure in %s! rc =%d !\n",
                   __FILE__, __LINE__, __FUNCTION__,rc);
        }
        rc = L7_FAILURE;
    }
    else
       rc = L7_SUCCESS;

    return((L7_RC_t)rc);
}

/********************************************************************
* @purpose Deletes an MAC VLAN
*
* @param   *usp    @b{(input)} [unit/slot/port]
* @param   cmd     @b{(input)} [DAPI_CMD_MAC_VLAN_DELETE]
* @param   *data   @b{(input)} [Data for command operation]
* @param   *dapi_g @b{(input)} [The driver object]
*
* @returns L7_SUCCESS 
* @returns L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t hapiBroadL2VlanMacEntryDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, 
                                     void *data,DAPI_t *dapi_g)
{
    int rc;
    DAPI_QVLAN_MGMT_CMD_t *dapiCmd;

    /*
     * [1] Prepare arguments:
     *   a. MAC address
     */
    dapiCmd = (DAPI_QVLAN_MGMT_CMD_t *) data;
    
    if(initialized == 0)
       return L7_SUCCESS;
    rc = usl_bcmx_vlan_mac_delete(dapiCmd->cmdData.macVlanConfig.mac.addr);

    if (L7_BCMX_OK(rc) != L7_TRUE)
    {
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR, "\n%s %d: Failure in %s! rc =%d !\n",
                   __FILE__, __LINE__, __FUNCTION__,rc);
        rc = L7_FAILURE;
    }
    else
       rc = L7_SUCCESS;

    return((L7_RC_t)rc);
}
