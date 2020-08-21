/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename  broad_l2_vlan_ipsubnet.c
*
* @purpose   This file contains the entry points to the Layer 2 interface
*
* @component hapi
*
* @comments
*
* @create    05/20/2005
*
* @author    tsrikanth,grantc
*
* @end
*
**********************************************************************/
#include "dapi.h"
#include "log.h"
#include "dapi_struct.h"
#include "broad_common.h"
#include "l7_usl_bcmx_l2.h"

static int initialized = 0;

/********************************************************************
* @purpose Adds an IP VLAN
*
* @param   *usp    @b{(input)} [unit/slot/port]
* @param   cmd     @b{(input)} [DAPI_CMD_QVLAN_IPVLAN_CONFIG]
* @param   *data   @b{(input)} [Data for command operation]
* @param   *dapi_g @b{(input)} [The driver object]
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
******************************************************************/
L7_RC_t hapiBroadL2VlanIpSubnetEntryAdd(DAPI_USP_t *usp,
                               DAPI_CMD_t cmd,
                               void *data,
                               DAPI_t *dapi_g)
{
    int rc; 
    DAPI_QVLAN_MGMT_CMD_t *dapiCmd;

    /*
     * Prepare arguments
     *  a. IP address
     *  b. netmask
     *  c. vlan id
     *  d. priority
     */
    dapiCmd = (DAPI_QVLAN_MGMT_CMD_t *) data;
    if (initialized == 0)
    {
        int gp_idx, gport;
        int bcm_unit, bcm_port;

        /* Iterate all local gports */
        BCMY_GPORT_LOCAL_ITER(gp_idx, gport)
        {
            /* Convert gport to unit+port */
            rc = bcmy_gport_to_unit_port(gport, &bcm_unit, &bcm_port);
            if (rc != BCM_E_NONE)
            {
                PT_LOG_ERR(LOG_CTX_L2, "idx %d, gport=0x%x: Error converting to bcm_unit/port: %d\n",
                           gp_idx, gport, rc);
                return (L7_FAILURE);
            }

            rc = bcm_vlan_control_port_set(bcm_unit, bcm_port, bcmVlanLookupIPEnable, 1);
            if (L7_BCMX_OK(rc) != L7_TRUE)
            {
                PT_LOG_ERR(LOG_CTX_L2, "idx %d, gport=0x%x, bcm_unit %d, bcm_port %d: Error calling bcm_vlan_control_port_set: %d\n",
                           gp_idx, gport, bcm_unit, bcm_port, rc); 
                return (L7_FAILURE);
            }
        }

        initialized = 1;
    }

    rc = usl_bcmx_vlan_ip4_add(dapiCmd->cmdData.ipSubnetVlanConfig.ipSubnet,
                                dapiCmd->cmdData.ipSubnetVlanConfig.netMask,
                                dapiCmd->cmdData.ipSubnetVlanConfig.vlanId, 0);

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

/*********************************************************************
* @purpose Deletes an IP VLAN
*
* @param   *usp    @b{(input)} [unit/slot/port]
* @param   cmd     @b{(input)} [DAPI_CMD_QVLAN_IPVLAN_CONFIG]
* @param   *data   @b{(input)} [Data for command operation]
* @param   *dapi_g @b{(input)} [The driver object]
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
********************************************************************/
L7_RC_t hapiBroadL2VlanIpSubnetEntryDelete(DAPI_USP_t *usp,
                                  DAPI_CMD_t cmd,
                                  void *data,
                                  DAPI_t *dapi_g)
{
    int rc;
    DAPI_QVLAN_MGMT_CMD_t *dapiCmd;

    /*
     * [1] Prepare arguments:
     *   a. IP address
     *   b. netmask
     */
    dapiCmd = (DAPI_QVLAN_MGMT_CMD_t *) data;
     
    if(initialized == 0)
       return L7_SUCCESS;
    rc = usl_bcmx_vlan_ip4_delete(dapiCmd->cmdData.ipSubnetVlanConfig.ipSubnet,
                                   dapiCmd->cmdData.ipSubnetVlanConfig.netMask);

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
