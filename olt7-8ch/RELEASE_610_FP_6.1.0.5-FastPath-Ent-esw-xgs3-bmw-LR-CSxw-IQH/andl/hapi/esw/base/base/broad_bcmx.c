/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_bcmx.c
*
* @purpose   Custom BCMX functions added to support Fastpath.
*
* @component hapi
*
* @comments
*
* @create    9/16/2004
*
* @author    Andrey Tsigler
*
* @end
*
**********************************************************************/
#include "broad_utils.h"
#include "bcm/stat.h"
#include "bcm/stack.h"
#include "bcmx/custom.h"
#include "sal/core/libc.h"
#include "bcmx/port.h"

#include "broad_policy_bcm.h"
#include "l7_usl_bcm_l3.h"

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
#include "broad_dot1ad_bcm.h"
#endif

/*********************************************************************
*
* @purpose Stats Handler. 
*
* @param
*
* @returns BCM Error Code
*
* @notes   
*   Get all of a port's stats in one call.
*   This is the custom handler that runs on the slave devices.
*   See companion calling routine custom_stat_get() that runs on the master.
*
*
* @end
*
*********************************************************************/
static int custom_stat_handler(int unit, bcm_port_t port, int setget,
            uint32 *args)
{
    int     rv = 0, argi, s;
    uint64  val;

    if (setget != BCM_CUSTOM_GET) {
    return BCM_E_PARAM;
    }

    argi = 0;
    for (s = 0; s < snmpValCount; s++) {
        rv = bcm_stat_get(unit, port, s, &val);
        if (rv < 0) {
        /* Not all stats may be supported on the port. Return zeroes for unsupported
        ** stats and set return code to OK.
        */
        rv = BCM_E_NONE;
        memset (&val, 0, sizeof (val));

        }
        if (argi >= BCM_CUSTOM_ARGS_MAX-2) {
        break;
        }
        args[argi++] = COMPILER_64_HI(val);
        args[argi++] = COMPILER_64_LO(val);
    }
    /* clear remaining arguments */
    sal_memset(&args[argi], 0,
           sizeof(uint32) * (BCM_CUSTOM_ARGS_MAX-argi));

    return rv;
}

/*********************************************************************
*
* @purpose Stats Handler. 
*
* @param
*
* @returns BCM Error Code
*
* @notes   
*   Get all of a port's stats in one call.
*   This is the custom handler that runs on the slave devices.
*   See companion calling routine custom_stat_get() that runs on the master.
*
*
* @end
*
*********************************************************************/
static int custom_port_stat_handler(int unit, bcm_port_t port, int setget,
            uint32 *args)
{
    int     rv = 0, argi, s;
    uint64  val;

    if (setget != BCM_CUSTOM_GET) {
    return BCM_E_PARAM;
    }

    argi = 0;
    for (s = 0; s < snmpValCount; s++) {
        rv = bcm_port_stat_get(unit, port, s, &val);
        if (rv < 0) {
        /* Not all stats may be supported on the port. Return zeroes for unsupported
        ** stats and set return code to OK.
        */
        rv = BCM_E_NONE;
        memset (&val, 0, sizeof (val));

        }
        if (argi >= BCM_CUSTOM_ARGS_MAX-2) {
        break;
        }
        args[argi++] = COMPILER_64_HI(val);
        args[argi++] = COMPILER_64_LO(val);
    }
    /* clear remaining arguments */
    sal_memset(&args[argi], 0,
           sizeof(uint32) * (BCM_CUSTOM_ARGS_MAX-argi));

    return rv;
}

/*********************************************************************
*
* @purpose Add to VLAN group handler.
*
* @param
*
* @returns BCM Error Code
*
* @notes   
*   Adds specified port to all VLANS in the list.
*
*
* @end
*
*********************************************************************/
static int custom_port_to_vlans_add(int unit, bcm_port_t port, int setget,
            uint32 *args)
{
  int     rv;
  bcm_vlan_t vid;
  HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t * vlan_cmd;
  bcm_pbmp_t pbmp, ubmp;
  bcm_pbmp_t old_pbmp, old_ubmp;
  int isuntagged = 1;

  if (setget != BCM_CUSTOM_SET) 
  {
    return BCM_E_PARAM;
  }

  vlan_cmd = (HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t *)  args;

  for (vid = 1; vid <= L7_PLATFORM_MAX_VLAN_ID; vid++)
  {
    if (BROAD_BCMX_CUSTOM_IS_VLAN_MEMBER(vid, vlan_cmd->vlan_membership_mask) != 0)
    {
      if (!BCM_GPORT_IS_WLAN_PORT(port))
      {
        BCM_PBMP_PORT_SET (pbmp, port);
      }
      if (BROAD_BCMX_CUSTOM_IS_VLAN_MEMBER(vid, vlan_cmd->vlan_tag_mask) != 0) 
      {
        if (!BCM_GPORT_IS_WLAN_PORT(port))
        {
          BCM_PBMP_CLEAR (ubmp);
        }
        isuntagged = 0;
      } 
      else
      {
        if (!BCM_GPORT_IS_WLAN_PORT(port))
        {
           BCM_PBMP_PORT_SET (ubmp, port);
        }
        isuntagged = 1;
      }

      /* If port is already in the VLAN then remove it. 
      */
      if (BCM_GPORT_IS_WLAN_PORT(port))
      {
        int temp;
        rv = bcm_vlan_gport_get(unit, vid, port, &temp);
        if (rv == BCM_E_NONE)
        {
          rv = bcm_vlan_gport_delete (unit, vid, port);
          if (rv != BCM_E_NONE)
          {
            /*LOG_ERROR (rv);*/
            printf(" bcm_vlan_gport_delete failed for port %d; vid %d; rv = %d \n", port, vid, rv);
          }
        }

      }
      else
      {
        rv = bcm_vlan_port_get (unit, vid, &old_pbmp, &old_ubmp);
        if (rv != BCM_E_NONE)
        {
          LOG_ERROR (rv);
        }
        if (BCM_PBMP_MEMBER(old_pbmp, port))
        {
          rv = bcm_vlan_port_remove (unit, vid, pbmp);
          if (rv != BCM_E_NONE)
          {
            LOG_ERROR (rv);
          }
        }
      }

      /* Add the port to the VLAN with specified tagging mode.
      */
      if (BCM_GPORT_IS_WLAN_PORT(port))
      {
        rv = bcm_vlan_gport_add (unit, vid, port, isuntagged);
        if (rv == BCM_E_PORT)
        {
          /* ignore this error for wlan ports since the vp may not be terminated on this unit */
          rv = BCM_E_NONE;
        }
      }
      else
      {
        rv = bcm_vlan_port_add (unit, vid, pbmp, ubmp);
      }
      if (rv != BCM_E_NONE)
      {
        LOG_ERROR (rv);
      }
    }

  }

  return BCM_E_NONE;
}

/*********************************************************************
*
* @purpose Remove from VLAN group handler.
*
* @param
*
* @returns BCM Error Code
*
* @notes   
*   Adds specified port to all VLANS in the list.
*
*
* @end
*
*********************************************************************/
static int custom_port_from_vlans_delete(int unit, bcm_port_t port, int setget,
            uint32 *args)
{
    int     rv;
    bcm_vlan_t vid;
    HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t * vlan_cmd;
    bcm_pbmp_t pbmp;

    if (setget != BCM_CUSTOM_SET) 
    {
      return BCM_E_PARAM;
    }

    vlan_cmd = (HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t *)  args;

    for (vid = 1; vid <= L7_PLATFORM_MAX_VLAN_ID; vid++)
    {
      if (BROAD_BCMX_CUSTOM_IS_VLAN_MEMBER(vid, vlan_cmd->vlan_membership_mask) != 0)
      {

        if (BCM_GPORT_IS_WLAN_PORT(port))
        {
          rv = bcm_vlan_gport_delete (unit, vid, port);
          if (rv == BCM_E_PORT)
          {
            /* ignore this error for wlan ports since the vp may not be terminated on this unit */
            rv = BCM_E_NONE;
          }
        }
        else
        {
          BCM_PBMP_PORT_SET (pbmp, port);
  
          rv = bcm_vlan_port_remove (unit, vid, pbmp);
        }
        if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND))
        {
          LOG_ERROR (rv);
        }
      }

    }

    return BCM_E_NONE;
}


/*********************************************************************
*
* @purpose dot1x client timeout get Handler. 
*
* @param
*
* @returns BCM Error Code
*
* @notes   
*   This is the custom handler that runs on the slave devices.
*   See companion calling routine customx_dot1x_client_timeout_get() that runs on the master.
*
*
* @end
*
*********************************************************************/
static int custom_dot1x_client_timeout_handler(int unit, bcm_port_t port, int setget,
            uint32 *args)
{
    int     rv;
    HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t *pClientCmd;
    bcm_l2_addr_t  l2addr;

    if (setget != BCM_CUSTOM_GET) 
    {
      return BCM_E_PARAM;
    }

    pClientCmd = (HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t *) args;

    rv = bcm_l2_addr_get(unit, pClientCmd->mac_addr, pClientCmd->vlan_id , &l2addr);
    if (rv == BCM_E_NONE) 
    {
        
        if (l2addr.flags & BCM_L2_SRC_HIT) 
        {
            /* Clear the HIT bit and add the entry on this unit */
            l2addr.flags &= ~(BCM_L2_SRC_HIT);
            (void)bcm_l2_addr_add(unit, &l2addr);
            pClientCmd->timedout_flg = L7_FALSE;
        }
        else
        {
            pClientCmd->timedout_flg = L7_TRUE;
        }
    }

  return rv;
}

/*********************************************************************
*
* @purpose dot1x client block Handler. 
*
* @param
*
* @returns BCM Error Code
*
* @notes   
*   This is the custom handler that runs on the slave devices.
*   See companion calling routine customx_dot1x_client_block() that runs on the master.
*
*
* @end
*
*********************************************************************/
static int custom_dot1x_client_block_handler(int unit, bcm_port_t port, int setget,
            uint32 *args)
{
    int     rv;
    HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t *pClientCmd;
    bcm_l2_addr_t  l2addr;

    if (setget != BCM_CUSTOM_SET) 
    {
      return BCM_E_PARAM;
    }

    pClientCmd = (HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t *) args;

    memset(&l2addr, 0, sizeof(l2addr));
    memcpy(l2addr.mac, pClientCmd->mac_addr, sizeof(bcm_mac_t));
    l2addr.vid = pClientCmd->vlan_id;
    l2addr.port = port;
    BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &l2addr.modid));
    l2addr.flags = BCM_L2_DISCARD_SRC | BCM_L2_DISCARD_DST | BCM_L2_HIT | BCM_L2_SRC_HIT;

    rv = bcm_l2_addr_add(unit, &l2addr);

    return rv;
}

/*********************************************************************
*
* @purpose dot1x client unblock Handler. 
*
* @param
*
* @returns BCM Error Code
*
* @notes   
*   This is the custom handler that runs on the slave devices.
*   See companion calling routine customx_dot1x_client_unblock() that runs on the master.
*
*
* @end
*
*********************************************************************/
static int custom_dot1x_client_unblock_handler(int unit, bcm_port_t port, int setget,
            uint32 *args)
{
    int     rv;
    HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t *pClientCmd;
    bcm_l2_addr_t  l2addr;

    if (setget != BCM_CUSTOM_SET) 
    {
      return BCM_E_PARAM;
    }

    pClientCmd = (HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t *) args;

    memset(&l2addr, 0, sizeof(l2addr));
    memcpy(l2addr.mac, pClientCmd->mac_addr, sizeof(bcm_mac_t));
    l2addr.vid = pClientCmd->vlan_id;

    rv = bcm_l2_addr_delete(unit, l2addr.mac, l2addr.vid);

    return rv;
}

#ifdef L7_MCAST_PACKAGE
/*********************************************************************
*
* @purpose Sets the L2 ports for the multicast group.
*
* @param   port - 
*
* @returns BCM Error Code
*
* @notes This function sets the L2 ports for the specified group and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
static int custom_ipmc_l2_port_set (int unit, bcm_port_t port, int setget,
            uint32 *args)
{
  int                                rv = BCM_E_NONE;
  HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *ipmc_cmd;
  bcm_ipmc_addr_t                    ipmc;
  int                                index;

  ipmc_cmd = (HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *) args;

  index = ipmc_cmd->ipmc_index;

  memset (&ipmc, 0, sizeof (ipmc));
  rv = mbcm_driver[unit]->mbcm_ipmc_get(unit, index, &ipmc);
  if (rv != BCM_E_NONE)
  {
    return rv;
  }

  if (!ipmc.v) {
    return BCM_E_NONE;
  }

  if (BCM_GPORT_IS_WLAN_PORT(port))
  {
    /*rv = usl_bcm_l2_ipmc_wlan_port_add(unit, index, port);*/
    printf("%s %d Fix for wlan ports \n", __FUNCTION__, __LINE__);
  }
  else
  {
    SOC_PBMP_ASSIGN (ipmc.l2_pbmp, ipmc_cmd->pbmp);
    SOC_PBMP_OR(ipmc.l2_pbmp, PBMP_HG_ALL(unit));
    SOC_PBMP_OR(ipmc.l2_pbmp, PBMP_HL_ALL(unit));
    SOC_PBMP_ASSIGN (ipmc.l2_ubmp, ipmc_cmd->ubmp);
  
    ipmc.flags |= BCM_IPMC_REPLACE;
    rv = mbcm_driver[unit]->mbcm_ipmc_add(unit, &ipmc);
    if (rv != BCM_E_NONE)
    {
      return rv;
    }
  }

  return BCM_E_NONE;
}

/*********************************************************************
*
* @purpose Add L2 ports to the multicast group.
*
* @param   port - 
*
* @returns BCM Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
static int custom_ipmc_l2_port_add (int unit, bcm_port_t port, int setget,
            uint32 *args)
{
  int rv = BCM_E_NONE;
  HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *ipmc_cmd;
  bcm_ipmc_addr_t  ipmc;
  int              index;
  bcm_pbmp_t       tagged_pbmp, untagged_pbmp;
  L7_uint32        i;

  ipmc_cmd = (HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *) args;

  for (i = 0; i < L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL; i++)
  {
    if (BROAD_BCMX_CUSTOM_IS_IPMC_INDEX_MEMBER(i, ipmc_cmd->ipmc_index_mask) == 0)
    {
      continue;
    }

    index = i;

    memset (&ipmc, 0, sizeof (ipmc));
    rv = mbcm_driver[unit]->mbcm_ipmc_get(unit, index, &ipmc);
    if (rv != BCM_E_NONE)
    {
      continue;
    }

    if (!ipmc.v) {
      continue;
    }

    if (BCM_GPORT_IS_WLAN_PORT(port))
    {
      rv = usl_bcm_l2_ipmc_wlan_port_add(unit, index, port);
    }
    else
    {
      BCM_PBMP_PORT_SET (tagged_pbmp, port);
      if (ipmc_cmd->tagged)
      {
        BCM_PBMP_CLEAR (untagged_pbmp);
      } else
      {
        BCM_PBMP_PORT_SET (untagged_pbmp, port);
      }

      SOC_PBMP_OR(ipmc.l2_pbmp, tagged_pbmp);
      SOC_PBMP_REMOVE(ipmc.l2_ubmp, tagged_pbmp);
      SOC_PBMP_OR(ipmc.l2_ubmp, untagged_pbmp);

      ipmc.flags |= BCM_IPMC_REPLACE;
      rv = mbcm_driver[unit]->mbcm_ipmc_add(unit, &ipmc);
      if (rv != BCM_E_NONE)
      {

        continue;
      }
    }

  }
  return BCM_E_NONE;
}

/*********************************************************************
*
* @purpose Delete L2 ports from the multicast group.
*
* @param   
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
static int custom_ipmc_l2_port_delete (int unit, bcm_port_t port, int setget,
            uint32 *args)
{
  int rv = BCM_E_NONE;
  HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *ipmc_cmd;
  bcm_ipmc_addr_t  ipmc;
  int              index;
  L7_uint32        i;


  ipmc_cmd = (HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *) args;

  for (i = 0; i < L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL; i++)
  {
    if (BROAD_BCMX_CUSTOM_IS_IPMC_INDEX_MEMBER(i, ipmc_cmd->ipmc_index_mask) == 0)
    {
      continue;
    }

    index = i;

    memset (&ipmc, 0, sizeof (ipmc));
    rv = mbcm_driver[unit]->mbcm_ipmc_get(unit, index, &ipmc);
    if (rv != BCM_E_NONE)
    {
      continue;
    }

    if (!ipmc.v) {
      continue;
    }

    if (BCM_GPORT_IS_WLAN_PORT(port))
    {
      rv = usl_bcm_l2_ipmc_wlan_port_delete(unit, index, port);
    }
    else
    {
      SOC_PBMP_PORT_REMOVE(ipmc.l2_pbmp, port);
      SOC_PBMP_PORT_REMOVE(ipmc.l2_ubmp, port);

      ipmc.flags |= BCM_IPMC_REPLACE;
      rv = mbcm_driver[unit]->mbcm_ipmc_add(unit, &ipmc);

      if (rv != BCM_E_NONE)
      {
        continue;
      }
    }

  }


  return rv;
}

/*********************************************************************
*
* @purpose Add L3 ports to the multicast group.
*
* @param   port - 
*
* @returns BCM Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
static int custom_ipmc_l3_port_add (int unit, bcm_port_t port, int setget,
            uint32 *args)
{
  int rv = BCM_E_NONE;
  HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *ipmc_cmd;
  bcm_ipmc_addr_t  ipmc;
  int              index;
  bcm_pbmp_t       l3_pbmp;
  L7_uint32        i, j;
  int              tagged;
  bcm_vlan_vector_t vlan_vec;
  int              ipmc_untag_flag;

  ipmc_cmd = (HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *) args;

  for (i = 0; i < L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL; i++)
  {
    if (BROAD_BCMX_CUSTOM_IS_IPMC_INDEX_MEMBER(i, ipmc_cmd->ipmc_index_mask) == 0)
    { 
      continue;
    }

    index = i;

    memset (&ipmc, 0, sizeof (ipmc));
    rv = mbcm_driver[unit]->mbcm_ipmc_get(unit, index, &ipmc);
    if (rv != BCM_E_NONE)
    {
      continue;
    }

    if (!ipmc.v) { 
      continue;
    }

    if (!BCM_GPORT_IS_WLAN_PORT(port))
    {
      BCM_PBMP_PORT_SET (l3_pbmp, port);
      SOC_PBMP_OR(ipmc.l3_pbmp, l3_pbmp);

      ipmc.flags |= BCM_IPMC_REPLACE;
      rv = mbcm_driver[unit]->mbcm_ipmc_add(unit, &ipmc); 
      if (rv != BCM_E_NONE)
      {
        continue;
      }
      /* If application requests no tagging then do it only if there is one VLAN 
      ** replicated on the port.
      ** If more than one VLAN is replicated on the port then force tagging mode.
      */
      tagged = ipmc_cmd->tagged;
      if (tagged == 0)
      {
        rv = mbcm_driver[unit]->mbcm_ipmc_repl_get(unit, index, port, vlan_vec);
        if (rv == BCM_E_NONE)
        {
          for (j = 1; j < _BCM_MAX_VLANS; j++)
          {
            if (BCM_VLAN_VEC_GET(vlan_vec, j))
            {
              break;
            }
          }
          if (j == _BCM_MAX_VLANS)
          {
            /* This is the only replicated VLAN.
            */
            tagged = ipmc_cmd->tagged;
          } else
          {
            /* Force tagging.
            */
            tagged = 1;
          }
        } 
      }


      /* Set up VLAN replication.
      */
      (void)mbcm_driver[unit]->mbcm_ipmc_repl_add(unit, index, port, ipmc_cmd->vlan_id);

      if (tagged)
      {
        ipmc_untag_flag = FALSE;
      } else
      {
        ipmc_untag_flag = TRUE;
      }

      (void)mbcm_driver[unit]->mbcm_ipmc_egress_port_set(unit, port, 
                                                        ipmc_cmd->mac,
                                                        ipmc_untag_flag, 
                                                        ipmc_cmd->vlan_id,
                                                        ipmc_cmd->ttl);
    }
    else
    {
      /* wlan gport add */
      rv = usl_bcm_l3_ipmc_wlan_port_add(unit, index, ipmc_cmd->vlan_id, port);
    }


  }
  return BCM_E_NONE;
}

/*********************************************************************
*
* @purpose Delete L3 ports from the multicast group.
*
* @param   
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
static int custom_ipmc_l3_port_delete (int unit, bcm_port_t port, int setget,
            uint32 *args)
{
  int rv = BCM_E_NONE;
  HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *ipmc_cmd;
  bcm_ipmc_addr_t  ipmc;
  int              index;
  L7_uint32        i, j;
  bcm_vlan_vector_t vlan_vec;
  L7_BOOL          remove_port;


  ipmc_cmd = (HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *) args;

  for (i = 0; i < L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL; i++)
  {
    if (BROAD_BCMX_CUSTOM_IS_IPMC_INDEX_MEMBER(i, ipmc_cmd->ipmc_index_mask) == 0)
    {
      continue;
    }

    index = i;

    memset (&ipmc, 0, sizeof (ipmc));
    rv = mbcm_driver[unit]->mbcm_ipmc_get(unit, index, &ipmc);
    if (rv != BCM_E_NONE)
    {
      continue;
    }

    if (!ipmc.v) {
      continue;
    }

    if (!BCM_GPORT_IS_WLAN_PORT(port))
    {
      /* Set up VLAN replication.
      */
      (void)mbcm_driver[unit]->mbcm_ipmc_repl_delete(unit, index, port, ipmc_cmd->vlan_id);

      /* If the egress port doesn't have any more replicated VLANs then remove the
      ** port from the L3 group.
      */
      remove_port = L7_TRUE;
      rv = mbcm_driver[unit]->mbcm_ipmc_repl_get(unit, index, port, vlan_vec);
      if (rv == BCM_E_NONE)
      {
        for (j = 1; j < _BCM_MAX_VLANS; j++)
        {
          if (BCM_VLAN_VEC_GET(vlan_vec, j))
          {
            remove_port = L7_FALSE;
            break;
          }
        }
      } 

      if (remove_port == L7_TRUE)
      {
        SOC_PBMP_PORT_REMOVE(ipmc.l3_pbmp, port);
        ipmc.flags |= BCM_IPMC_REPLACE;
        rv = mbcm_driver[unit]->mbcm_ipmc_add(unit, &ipmc);

      }
    }
    else
    {
      /* wlan gport delete */
      rv = usl_bcm_l3_ipmc_wlan_port_delete(unit, index, ipmc_cmd->vlan_id, port);
    }

  }

  return rv;
}
#endif

/*********************************************************************
 *
 * @purpose SFP diag Handler.
 *
 * @param
 *
 * @returns BCM Error Code
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
static int custom_diagnostic_handler(int unit, bcm_port_t port, int setget,
    uint32 *args)
{
  int     rv = 0;
  int32   temperature;
  uint32  voltage;
  uint32  current;
  uint32  txPower;
  uint32  rxPower;
  uint32  txFault;
  uint32  los;

  if (sysapiHpcDiagTempRead(unit, port, &temperature) != L7_SUCCESS)
  {
    rv = BCM_E_FAIL;
  }
  if (sysapiHpcDiagVoltageRead(unit, port, &voltage) != L7_SUCCESS)
  {
    rv = BCM_E_FAIL;
  }
  if (sysapiHpcDiagCurrentRead(unit, port, &current) != L7_SUCCESS)
  {
    rv = BCM_E_FAIL;
  }
  if (sysapiHpcDiagTxPwrRead(unit, port, &txPower) != L7_SUCCESS)
  {
    rv = BCM_E_FAIL;
  }
  if (sysapiHpcDiagRxPwrRead(unit, port, &rxPower) != L7_SUCCESS)
  {
    rv = BCM_E_FAIL;
  }
  if (sysapiHpcDiagTxFaultRead(unit, port, &txFault) != L7_SUCCESS)
  {
    rv = BCM_E_FAIL;
  }
  if (sysapiHpcDiagLosRead(unit, port, &los) != L7_SUCCESS)
  {
    rv = BCM_E_FAIL;
  }

  args[0] = temperature;
  args[1] = voltage;
  args[2] = current;
  args[3] = txPower;
  args[4] = rxPower;
  args[5] = txFault;
  args[6] = los;

  return rv;
}

/*********************************************************************
*
* @purpose Set the flow control mode for a port
*
* @param   
*
* @returns BCMX Error Code
*
*
* @end
*
*********************************************************************/
int custom_port_flow_control_set(int unit, bcm_port_t port, int setget, uint32 *args)
{
  int                                   rv = BCM_E_NONE, tmpRv;
  HAPI_BROAD_CUSTOM_BCMX_PAUSE_CMD_t   *pauseCmd;
  bcm_port_abil_t                       ability_mask;
  L7_BOOL                               jam = L7_FALSE;


  pauseCmd = (HAPI_BROAD_CUSTOM_BCMX_PAUSE_CMD_t *)args;

  tmpRv = bcm_port_advert_get(unit, port, &ability_mask);
  if (tmpRv != BCM_E_NONE)
  {
    rv = tmpRv;
  }

  if (pauseCmd->pauseRx) 
  {
    ability_mask |= BCM_PORT_ABIL_PAUSE_RX;
  }
  else
  {
    ability_mask &= ~BCM_PORT_ABIL_PAUSE_RX;
  }
  
  if (pauseCmd->pauseTx) 
  {
    ability_mask |= BCM_PORT_ABIL_PAUSE_TX;
  }
  else
  {
    ability_mask &= ~BCM_PORT_ABIL_PAUSE_TX;
  }


  tmpRv = bcm_port_advert_set(unit, port, ability_mask);
  if (tmpRv != BCM_E_NONE) 
  {
    rv = tmpRv;
  }


  tmpRv = bcm_port_pause_set(unit, port, pauseCmd->pauseTx, pauseCmd->pauseRx);
  if (tmpRv != BCM_E_NONE) 
  {
    rv = tmpRv;
  }

  /* pause_addr_set is not supported for RoBo */
  if(hapiBroadRoboCheck() != L7_TRUE)
  {
    tmpRv = bcm_port_pause_addr_set(unit, port, pauseCmd->pauseMacAddr);
    if (tmpRv != BCM_E_NONE) 
    {
      rv = tmpRv;
    }
  }

  if (pauseCmd->pauseTx || pauseCmd->pauseRx) 
  {
    jam = L7_TRUE;
  }
  else
  {
    jam = L7_FALSE;
  }

  /* Jam is only supported for certain port types */
  tmpRv = bcm_port_jam_set(unit, port, jam);
  if ((tmpRv != BCM_E_NONE) && (tmpRv != BCM_E_UNAVAIL))
  {
    rv = tmpRv;
  }

  return rv;
}

/*********************************************************************
*
* @purpose Set the admin mode for a port
*
* @param   
*
* @returns BCMX Error Code
*
*
* @end
*
*********************************************************************/
int custom_port_admin_mode_set(int unit, bcm_port_t port, int setget, uint32 *args)
{
  L7_uchar8                                       *msg, *msgPtr;
  HAPI_BROAD_CUSTOM_BCMX_PORT_ADMIN_MODE_t        *tmpElem;
  int                                              rv = BCM_E_NONE, tmpRv;
  L7_uint32                                        numElems;


  msg = (L7_uchar8 *) args;
  msgPtr = msg + sizeof(L7_uint32);
  numElems = *(L7_uint32 *)&msg[0];

  while (numElems-- > 0) 
  {
    tmpElem = (HAPI_BROAD_CUSTOM_BCMX_PORT_ADMIN_MODE_t *) msgPtr;

    tmpRv = bcm_port_enable_set(tmpElem->bcmUnit, tmpElem->bcmPort, tmpElem->adminMode);
    if (tmpRv != BCM_E_NONE) 
    {
      rv = tmpRv;
    }

    msgPtr += sizeof(HAPI_BROAD_CUSTOM_BCMX_PORT_ADMIN_MODE_t);
  }

  return rv;
}


#ifdef L7_QOS_FLEX_PACKAGE_COS
/* Helper function to convert queue-depth percentages to bytes. */
static uint32 custom_port_wred_percent_to_bytes(int unit, uint8 percent)
{
    static int conversion_factor = 0;

    if (conversion_factor == 0) 
    {
        /* The buffer usage allowed to a queue is: alpha*(B - omega)/(1+alpha*S)
           where alpha is the dynamic multiplier, omega is the total amount of buffer 
           used by uncontrolled queues, B is the total amount of buffer memory, 
           and S is the number of active queues. */
        /* Fastpath assumptions: omega=0, alpha=8. */
        /* We will set our conversion factor to: 100% = one queue active on 1/4 of the ports */
        unsigned int cellsize = 128;
        unsigned int totalmem = 32767; /* max cells that can be specified */
        
        if (SOC_IS_SC_CQ(unit)) 
        {
            totalmem = 16383;
        }
        /* subtract 1 from NUM_PORT to correct for CPU port, which never has WRED or taildrop */
        conversion_factor = cellsize * 8 * totalmem / (1 + 2 * (NUM_PORT(unit)-1));
    }
    return ((percent * conversion_factor) / 100);
}
/*********************************************************************
*
* @purpose Set WRED parameters on a port
*
* @param   unit - the local unit
* @param   port - BCMX Lport
* @param   setget - unused
* @param   args - Pointer to WRED params for all queues/colors
*
* @returns BCM Error Code
*
* @comments Companion to customx_port_wred_set(), this function runs 
*           on each unit.
* @end
*
*********************************************************************/
int custom_port_wred_set(int unit, bcm_port_t port, int setget, uint32 *args)                          
{
    HAPI_BROAD_CUSTOM_BCMX_PORT_WRED_SET_t *wredParams = 
        (HAPI_BROAD_CUSTOM_BCMX_PORT_WRED_SET_t *)args;
    int queueIndex, precIndex, rv;
    bcm_cosq_gport_discard_t discardParams;
    
    for (queueIndex=0; queueIndex<L7_MAX_CFG_QUEUES_PER_PORT; queueIndex++) 
    {
        discardParams.gain = wredParams->gain[queueIndex];
        for (precIndex=0; precIndex<(L7_MAX_CFG_DROP_PREC_LEVELS+1); precIndex++) 
        {
            discardParams.flags = BCM_COSQ_DISCARD_BYTES;
            if (wredParams->flags[queueIndex] & BCM_COSQ_DISCARD_CAP_AVERAGE) 
            {
                discardParams.flags |= BCM_COSQ_DISCARD_CAP_AVERAGE;
            }
            if (wredParams->flags[queueIndex] & BCM_COSQ_DISCARD_ENABLE)
            {
                  discardParams.flags |= BCM_COSQ_DISCARD_ENABLE;
            }
            discardParams.drop_probability = wredParams->dropProb[queueIndex][precIndex];
            discardParams.min_thresh = 
                custom_port_wred_percent_to_bytes(unit, wredParams->minThreshold[queueIndex][precIndex]);
            discardParams.max_thresh = 
                custom_port_wred_percent_to_bytes(unit, wredParams->maxThreshold[queueIndex][precIndex]);
            switch (precIndex) 
            {
            case 0:
                discardParams.flags |= BCM_COSQ_DISCARD_COLOR_GREEN;
                break;
            case 1:
                discardParams.flags |= BCM_COSQ_DISCARD_COLOR_YELLOW;
                break;
            case 2:
                discardParams.flags |= BCM_COSQ_DISCARD_COLOR_RED;
                break;
            case 3:
                discardParams.flags |= BCM_COSQ_DISCARD_NONTCP;
                break;
            default:
                return(BCM_E_PARAM);
            }
            rv = bcm_cosq_gport_discard_set(unit, wredParams->bcm_gport, 
                                            queueIndex, &discardParams);
            BCM_IF_ERROR_RETURN(rv);
        } /* End for each color */
    } /* End for each queue */
    return(BCM_E_NONE);
}
#endif


/*********************************************************************
*
* @purpose Add the given port to the list of multicast groups
*
* @param   unit - the local unit
* @param   port - BCM port
* @param   setget - unused
* @param   args - Pointer to list of multicast groups 
*
* @returns BCM Error Code
*
* @comments 
*
* @end
*
*********************************************************************/
int custom_multicast_join_groups_handler(int unit, 
                                         bcm_port_t port, 
                                         int setget, 
                                         uint32 *args)
{ 
  int rv = BCM_E_NONE;
  int i; 
  bcm_if_t encap_id = BCM_IF_INVALID;
  bcm_gport_t  gport;
 
  rv = bcm_port_gport_get(unit, port, &gport);

  if (rv != BCM_E_NONE)
  {
    return rv;
  }

  for (i=0; i < BCM_CUSTOM_ARGS_MAX; i++)
  {
    /* check whether the multicast group is valid */
    if (args[i] != 0)
    {
      rv = bcm_multicast_egress_add(unit, args[i], gport, encap_id);

      if (rv != BCM_E_NONE)
      {
        break;
      }
    }
  } 

  return rv;
}
 

/*********************************************************************
*
* @purpose Delete the given port from the list of multicast groups
*
* @param   unit - the local unit
* @param   port - BCM port
* @param   setget - unused
* @param   args - Pointer to list of multicast groups
*
* @returns BCM Error Code
*
* @comments
*
* @end
*
*********************************************************************/
int custom_multicast_leave_groups_handler(int unit,
                                          bcm_port_t port,
                                          int setget,
                                          uint32 *args)
{
  int rv = BCM_E_NONE;
  int i;
  bcm_if_t encap_id = BCM_IF_INVALID;
  bcm_gport_t  gport;

  rv = bcm_port_gport_get(unit, port, &gport);

  if (rv != BCM_E_NONE)
  {
    return rv;
  }


  for (i=0; i < BCM_CUSTOM_ARGS_MAX; i++)
  {
    /* check whether the multicast group is valid */
    if (args[i] != 0)
    {
      rv = bcm_multicast_egress_delete(unit, args[i], gport, encap_id);


      if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND))
      {
        break;
      }
    }
  }

  return rv;
}


/*********************************************************************
*
* @purpose Dispatch function to handle custom BCMX port requests.
*
* @param
*
* @returns BCM Error Code
*
* @notes   
*
*
* @end
*
*********************************************************************/
int custom_bcmx_port_handler(int unit, bcm_port_t port, int setget, int type,
            uint32 *args)
{
  int rv = BCM_E_NONE;

  switch (type)
  {
  case HAPI_BROAD_BCMX_PORT_STATS_GET:
      rv = custom_stat_handler (unit,port ,setget ,args );
    break;

  case HAPI_BROAD_BCMX_PORT_CUSTOM_STATS_GET:
      rv = custom_port_stat_handler (unit, port, setget,args );
    break;

  case HAPI_BROAD_BCMX_PORT_TO_VLANS_ADD:
    rv = custom_port_to_vlans_add (unit,port ,setget ,args );
    break;

  case HAPI_BROAD_BCMX_PORT_FROM_VLANS_DELETE:
    rv = custom_port_from_vlans_delete (unit,port ,setget ,args );
    break;

  case HAPI_BROAD_BCMX_PORT_FLOW_CONTROL_SET:
    rv = custom_port_flow_control_set (unit, port ,setget ,args );
    break;

  case HAPI_BROAD_BCMX_PORT_ADMIN_MODE_SET:
    rv = custom_port_admin_mode_set (unit, port , setget , args );
    break;
  
#ifdef L7_MCAST_PACKAGE

  case HAPI_BROAD_BCMX_IPMC_L2_PORT_SET:
    rv = custom_ipmc_l2_port_set (unit,port ,setget ,args );
    break;

  case HAPI_BROAD_BCMX_IPMC_L2_PORT_ADD:
    rv = custom_ipmc_l2_port_add (unit,port ,setget ,args );
    break;

  case HAPI_BROAD_BCMX_IPMC_L2_PORT_DELETE:
    rv = custom_ipmc_l2_port_delete (unit,port ,setget ,args );
    break;

  case HAPI_BROAD_BCMX_IPMC_L3_PORT_ADD:
    rv = custom_ipmc_l3_port_add (unit,port ,setget ,args );
    break;

  case HAPI_BROAD_BCMX_IPMC_L3_PORT_DELETE:
    rv = custom_ipmc_l3_port_delete (unit,port ,setget ,args );
    break;
#endif
  case HAPI_BROAD_BCMX_POLICY_SET_HANDLER:
    rv = custom_policy_set_handler(unit, port, setget, args);
    break;

  case HAPI_BROAD_BCMX_POLICY_GET_HANDLER:
    rv = custom_policy_get_handler(unit, port, setget, args);
    break;

  case HAPI_BROAD_BCMX_DOT1X_CLIENT_TIMEOUT_GET:
    rv=  custom_dot1x_client_timeout_handler(unit,port,setget, args);
    break;

  case HAPI_BROAD_BCMX_PORT_DIAGNOSTIC_GET:
    rv = custom_diagnostic_handler (unit,port, setget, args );
    break;

   case HAPI_BROAD_BCMX_DOT1X_CLIENT_BLOCK:
      rv = custom_dot1x_client_block_handler(unit, port, setget, args);
      break;

  case HAPI_BROAD_BCMX_DOT1X_CLIENT_UNBLOCK:
     rv = custom_dot1x_client_unblock_handler(unit, port, setget, args);
     break;

#ifdef L7_QOS_FLEX_PACKAGE_COS
  case HAPI_BROAD_BCMX_PORT_WRED_SET:
      rv = custom_port_wred_set(unit, port, setget, args);
      break;
#endif

#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  case HAPI_BROAD_BCMX_PORT_DOT1AD_RULE_ADD:
    rv = custom_dot1ad_rule_add_handler(unit, port, setget, args);
  break;

  case HAPI_BROAD_BCMX_PORT_DOT1AD_RULE_DELETE:
    rv = custom_dot1ad_rule_delete_handler(unit, port, setget, args);
  break;

#endif

  
  case HAPI_BROAD_BCMX_MULITCAST_JOIN_GROUPS:
     rv = custom_multicast_join_groups_handler(unit, port, setget, args);
     break;

  case HAPI_BROAD_BCMX_MULITCAST_LEAVE_GROUPS:
     rv = custom_multicast_leave_groups_handler(unit, port, setget, args);
     break;

  default:
      rv = BCM_E_PARAM;
    break;
  }
  return rv;
}

/*********************************************************************
**********************************************************************
**
** Custom BCMX APIs.
**
**********************************************************************
*********************************************************************/


/*********************************************************************
*
* @purpose Get all statistics for the specified port.
*
* @param   port - BCMX Lport
* @param   stats - 64-bit stats for the port.
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
customx_stat_get(bcmx_lport_t port,
                 uint64 stats[snmpValCount])
{
    int     rv, argi, s;
    uint32  args[BCM_CUSTOM_ARGS_MAX];

    rv = bcmx_custom_port_get(port, HAPI_BROAD_BCMX_PORT_STATS_GET, args);
    if (rv >= 0) {
    argi = 0;
    for (s = 0; s < snmpValCount; s++) {
        COMPILER_64_SET(stats[s], args[argi], args[argi+1]);
        argi += 2;
    }
    }
    return rv;
}

/*********************************************************************
*
* @purpose Get all statistics for the specified gport.
*
* @param   port - BCMX gport
* @param   stats - 64-bit stats for the port.
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
customx_port_stat_get(bcmx_lport_t port,
                 uint64 stats[snmpValCount])
{
    int     rv, argi, s;
    uint32  args[BCM_CUSTOM_ARGS_MAX];

    rv = bcmx_custom_port_get(port, HAPI_BROAD_BCMX_PORT_CUSTOM_STATS_GET, args);
    if (rv >= 0) {
    argi = 0;
    for (s = 0; s < snmpValCount; s++) {
        COMPILER_64_SET(stats[s], args[argi], args[argi+1]);
        argi += 2;
    }
    }
    return rv;
}

/*********************************************************************
*
* @purpose Add port to VLANs specified in the member list.
*
* @param   port - BCMX Lport
* @param   vlan_cmd - VLAN and Tagging mask.
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int
customx_port_vlan_member_add(bcmx_lport_t port,
                                HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t * vlan_cmd)

{
    int     rv;
    uint32  args[BCM_CUSTOM_ARGS_MAX];

    if (sizeof (HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t) > sizeof (args))
    {
      LOG_ERROR (sizeof (HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t));
    }

    memcpy (args, vlan_cmd, sizeof (HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t));

    rv = bcmx_custom_port_set(port, HAPI_BROAD_BCMX_PORT_TO_VLANS_ADD, args);


    return rv;
}

/*********************************************************************
*
* @purpose Remove port from VLANs specified in the member list.
*
* @param   port - BCMX Lport
* @param   vlan_cmd - VLAN and Tagging mask.
*
* @returns BCMX Error Code
*
* @notes 
*
* @end
*
*********************************************************************/
int
customx_port_vlan_member_delete(bcmx_lport_t port,
                                HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t * vlan_cmd)

{
    int     rv;
    uint32  args[BCM_CUSTOM_ARGS_MAX];

    if (sizeof (HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t) > sizeof (args))
    {
      LOG_ERROR (sizeof (HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t));
    }

    memcpy (args, vlan_cmd, sizeof (HAPI_BROAD_CUSTOM_BCMX_VLAN_CMD_t));

    rv = bcmx_custom_port_set(port, HAPI_BROAD_BCMX_PORT_FROM_VLANS_DELETE, args);


    return rv;
}

/*********************************************************************
*
* @purpose Get dot1x client timeout for the specified port.
*
* @param   port - BCMX Lport
* @param   client_cmd - Mac address, vlan Id 
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
customx_dot1x_client_timeout_get(bcmx_lport_t port,
                 HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t *client_cmd)
{
    int     rv;
    uint32  args[BCM_CUSTOM_ARGS_MAX];
    HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t *pReturnClient;

    if (sizeof(HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t) > sizeof(args)) 
    {
        LOG_ERROR(sizeof(HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t));
    }

    memcpy (args, client_cmd, sizeof (HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t));
    rv = bcmx_custom_port_get(port, HAPI_BROAD_BCMX_DOT1X_CLIENT_TIMEOUT_GET, args);
    if (rv >= 0) 
    {
        pReturnClient = (HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t *) args;
        client_cmd->timedout_flg = pReturnClient->timedout_flg;
    
    }
    return rv;
}

/*********************************************************************
*
* @purpose Block an unauthorized dot1x client for the specified port.
*
* @param   port - BCMX Lport
* @param   client_cmd - Mac address, vlan Id 
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
customx_dot1x_client_block(bcmx_lport_t port,
                           HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t *client_cmd)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];

  if (sizeof(HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t) > sizeof(args)) 
  {
      LOG_ERROR(sizeof(HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t));
  }

  memcpy (args, client_cmd, sizeof (HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t));
  rv = bcmx_custom_port_set(port, HAPI_BROAD_BCMX_DOT1X_CLIENT_BLOCK, args);
  return rv;
}

/*********************************************************************
*
* @purpose Unblock an unauthorized dot1x client for the specified port.
*
* @param   port - BCMX Lport
* @param   client_cmd - Mac address, vlan Id 
*
* @returns BCMX Error Code
*
* @notes   none
*
* @end
*
*********************************************************************/
int
customx_dot1x_client_unblock(bcmx_lport_t port,
                             HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t *client_cmd)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];

  if (sizeof(HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t) > sizeof(args)) 
  {
      LOG_ERROR(sizeof(HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t));
  }

  memcpy (args, client_cmd, sizeof (HAPI_BROAD_CUSTOM_BCMX_DOT1X_CLIENT_CMD_t));
  rv = bcmx_custom_port_set(port, HAPI_BROAD_BCMX_DOT1X_CLIENT_UNBLOCK, args);
  return rv;
}

#ifdef L7_MCAST_PACKAGE
/*********************************************************************
*
* @purpose Sets the L2 ports for a multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
* @param   tagged - Flag indicating whether this VLAN should be tagged.
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
customx_ipmc_l2_port_set(bcmx_lport_t port,
                         L7_uint32    ipmc_index,
                         bcm_pbmp_t   pbmp,
                         bcm_pbmp_t   ubmp,
                         L7_uint32    vlan_id)
{
  int rv = BCM_E_NONE;
  HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *ipmc_cmd;
  uint32  args[BCM_CUSTOM_ARGS_MAX];

  memset (&args, 0, sizeof (args));

  ipmc_cmd = (HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *) args;

  if (ipmc_index >= L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL)
  {
    LOG_ERROR (ipmc_index);
  }
  ipmc_cmd->ipmc_index = ipmc_index;

  memcpy(&ipmc_cmd->pbmp, &pbmp, sizeof(pbmp));
  memcpy(&ipmc_cmd->ubmp, &ubmp, sizeof(ubmp));

  ipmc_cmd->vlan_id = vlan_id;

  rv = bcmx_custom_port_set(port, HAPI_BROAD_BCMX_IPMC_L2_PORT_SET, args);

  return rv;
}

/*********************************************************************
*
* @purpose Add L2 ports to the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
* @param   tagged - Flag indicating whether this VLAN should be tagged.
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
customx_ipmc_l2_port_add (bcmx_lport_t port,
                          L7_uint32    *ipmc_index,
                          L7_uint32    num_groups,
                          L7_uint32    vlan_id,
                          L7_uint32    tagged)
{
  int rv = BCM_E_NONE;
  L7_uint32 i;
  HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *ipmc_cmd;
  uint32  args[BCM_CUSTOM_ARGS_MAX];

  memset (&args, 0, sizeof (args));

  if ((num_groups < 1) || (num_groups > L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL ))
  {
    LOG_ERROR (num_groups);
  }

  ipmc_cmd = (HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *) args;

  /* Pack the IPMC index list into a bit mask. We can do this because IPMC indexes
  ** are less or equal to the maximum IPMC table size.
  */
  for (i = 0; i < num_groups; i++)
  {
    if (ipmc_index[i] >= L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL)
    {
      LOG_ERROR (ipmc_index[i]);
    }

    BROAD_BCMX_CUSTOM_IPMC_INDEX_MEMBER_SET (ipmc_index[i], ipmc_cmd->ipmc_index_mask);
  }

  ipmc_cmd->vlan_id = vlan_id;
  ipmc_cmd->tagged = tagged;

  rv = bcmx_custom_port_set(port, HAPI_BROAD_BCMX_IPMC_L2_PORT_ADD, args);

  return rv;
}

/*********************************************************************
*
* @purpose Delete L2 ports from the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
*
* @returns BCMX Error Code
*
* @notes This function adds L2 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
customx_ipmc_l2_port_delete (bcmx_lport_t port,
                             L7_uint32    *ipmc_index,
                             L7_uint32    num_groups,
                             L7_uint32    vlan_id)
{
  int rv = BCM_E_NONE;
  L7_uint32 i;
  HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *ipmc_cmd;
  uint32  args[BCM_CUSTOM_ARGS_MAX];

  memset (&args, 0, sizeof (args));

  if ((num_groups < 1) || (num_groups > L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL ))
  {
    LOG_ERROR (num_groups);
  }

  ipmc_cmd = (HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *) args;

  /* Pack the IPMC index list into a bit mask. We can do this because IPMC indexes
  ** are less or equal to the maximum IPMC table size.
  */
  for (i = 0; i < num_groups; i++)
  {
    if (ipmc_index[i] >= L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL)
    {
      LOG_ERROR (ipmc_index[i]);
    }

    BROAD_BCMX_CUSTOM_IPMC_INDEX_MEMBER_SET (ipmc_index[i], ipmc_cmd->ipmc_index_mask);
  }

  ipmc_cmd->vlan_id = vlan_id;
  ipmc_cmd->tagged = 0;

  rv = bcmx_custom_port_set(port, HAPI_BROAD_BCMX_IPMC_L2_PORT_DELETE, args);


  return rv;
}


/*********************************************************************
*
* @purpose Add L3 ports to the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
* @param   tagged - Flag indicating whether this VLAN should be tagged.
*
* @returns BCMX Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
customx_ipmc_l3_port_add (bcmx_lport_t port,
                          L7_uint32    *ipmc_index,
                          L7_uint32    num_groups,
                          L7_uint32    vlan_id,
                          L7_uint32    tagged,
                          L7_uchar8    *mac,
                          L7_uint32    ttl)
{
  int rv = BCM_E_NONE;
  L7_uint32 i;
  HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *ipmc_cmd;
  uint32  args[BCM_CUSTOM_ARGS_MAX];
  memset (&args, 0, sizeof (args));

  if ((num_groups < 1) || (num_groups > L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL ))
  {
    LOG_ERROR (num_groups);
  }

  ipmc_cmd = (HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *) args;

  /* Pack the IPMC index list into a bit mask. We can do this because IPMC indexes
  ** are less or equal to the maximum IPMC table size.
  */
  for (i = 0; i < num_groups; i++)
  {
    if (ipmc_index[i] >= L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL)
    {
      LOG_ERROR (ipmc_index[i]);
    }

    BROAD_BCMX_CUSTOM_IPMC_INDEX_MEMBER_SET (ipmc_index[i], ipmc_cmd->ipmc_index_mask);
  }

  ipmc_cmd->vlan_id = vlan_id;
  ipmc_cmd->tagged = tagged;

  memcpy (ipmc_cmd->mac, mac, 6);
  ipmc_cmd->ttl = ttl;
  
  rv = bcmx_custom_port_set(port, HAPI_BROAD_BCMX_IPMC_L3_PORT_ADD, args);

  return rv;
}

/*********************************************************************
*
* @purpose Delete L3 ports from the multicast group.
*
* @param   port - BCMX Lport
* @param   *index - List of IPMC indexes to modify with this call.
* @param   num_groups - Number of IPMC groups in the *index array.
* @param   vlan_id - VLAN affected by this call.
*
* @returns BCMX Error Code
*
* @notes This function adds L3 ports to the specified groups and
*        sets up VLAN replication and tagging. 
*
* @end
*
*********************************************************************/
int
customx_ipmc_l3_port_delete (bcmx_lport_t port,
                          L7_uint32    *ipmc_index,
                          L7_uint32    num_groups,
                          L7_uint32    vlan_id)
{
  int rv = BCM_E_NONE;
  L7_uint32 i;
  HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *ipmc_cmd;
  uint32  args[BCM_CUSTOM_ARGS_MAX];

  memset (&args, 0, sizeof (args));

  if ((num_groups < 1) || (num_groups > L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL ))
  {
    LOG_ERROR (num_groups);
  }

  ipmc_cmd = (HAPI_BROAD_CUSTOM_BCMX_IPMC_CMD_t *) args;

  /* Pack the IPMC index list into a bit mask. We can do this because IPMC indexes
  ** are less or equal to the maximum IPMC table size.
  */
  for (i = 0; i < num_groups; i++)
  {
    if (ipmc_index[i] >= L7_L3_MCAST_ROUTE_TBL_SIZE_TOTAL)
    {
      LOG_ERROR (ipmc_index[i]);
    }

    BROAD_BCMX_CUSTOM_IPMC_INDEX_MEMBER_SET (ipmc_index[i], ipmc_cmd->ipmc_index_mask);
  }

  ipmc_cmd->vlan_id = vlan_id;

  rv = bcmx_custom_port_set(port, HAPI_BROAD_BCMX_IPMC_L3_PORT_DELETE, args);

  return rv;
}
#endif

/*********************************************************************
 *
 * @purpose Get SFP diagnostics for the specified port.
 *
 * @param   port - BCMX Lport
 * @param
 *
 * @returns BCMX Error Code
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
int
customx_diagnostic_get(bcmx_lport_t port,
    int32 *temperature,
    uint32 *voltage,
    uint32 *current,
    uint32 *txPower,
    uint32 *rxPower,
    uint32 *txFault,
    uint32 *los)
{
  int     rv;
  uint32  args[BCM_CUSTOM_ARGS_MAX];

  rv = bcmx_custom_port_get(port, HAPI_BROAD_BCMX_PORT_DIAGNOSTIC_GET, args);
  if (rv >= 0) {
    *temperature = args[0];
    *voltage     = args[1];
    *current     = args[2];
    *txPower     = args[3];
    *rxPower     = args[4];
    *txFault     = args[5];
    *los         = args[6];
  }
  return rv;
}



/*********************************************************************
*
* @purpose Enable/Disable flow control on a port
*
* @param   port - BCMX Lport
* @param   mode - Flow control mode
*
* @returns BCMX Error Code
*
*
* @end
*
*********************************************************************/
int customx_port_flow_control_set(bcmx_lport_t port,
                                  HAPI_BROAD_CUSTOM_BCMX_PAUSE_CMD_t pauseCmd)
{
  int     rv = BCM_E_NONE;
  uint32  args[BCM_CUSTOM_ARGS_MAX];

  memcpy(args, &pauseCmd, sizeof(HAPI_BROAD_CUSTOM_BCMX_PAUSE_CMD_t));

  rv = bcmx_custom_port_set(port, HAPI_BROAD_BCMX_PORT_FLOW_CONTROL_SET, args);

  return rv;
}

#ifdef L7_QOS_FLEX_PACKAGE_COS
/*********************************************************************
*
* @purpose Set WRED parameters on a port
*
* @param   port - BCMX Lport
* @param   wredParams - Pointer to WRED params for all queues/colors
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int customx_port_wred_set(bcmx_lport_t port,
                                  HAPI_BROAD_CUSTOM_BCMX_PORT_WRED_SET_t *wredParams)
{
    uint32  args[BCM_CUSTOM_ARGS_MAX];

    if (sizeof(HAPI_BROAD_CUSTOM_BCMX_PORT_WRED_SET_t) > BCM_CUSTOM_ARGS_MAX)
    {
        LOG_ERROR(sizeof(HAPI_BROAD_CUSTOM_BCMX_PORT_WRED_SET_t));
    }
    memcpy(args, wredParams, sizeof(HAPI_BROAD_CUSTOM_BCMX_PORT_WRED_SET_t));
    return(bcmx_custom_port_set(port, HAPI_BROAD_BCMX_PORT_WRED_SET, args));
}
#endif

/*********************************************************************
*
* @purpose Add the given port to the list of multicast groups
*
* @param   port - BCMX gport
* @param   groups - Pointer to list of multicast groups
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int customx_port_multicast_join_groups(bcm_gport_t port,
                                       int groups[BCM_CUSTOM_ARGS_MAX])
{
  return (bcmx_custom_port_set(port, 
                               HAPI_BROAD_BCMX_MULITCAST_JOIN_GROUPS, 
                               groups));
}

/*********************************************************************
*
* @purpose Delete the given port from the list of multicast groups
*
* @param   port - BCMX gport
* @param   groups - Pointer to list of multicast groups
*
* @returns BCMX Error Code
*
* @end
*
*********************************************************************/
int customx_port_multicast_leave_groups(bcm_gport_t port,
                                        int groups[BCM_CUSTOM_ARGS_MAX])
{
  return (bcmx_custom_port_set(port,
                               HAPI_BROAD_BCMX_MULITCAST_LEAVE_GROUPS, 
                               groups));
}
