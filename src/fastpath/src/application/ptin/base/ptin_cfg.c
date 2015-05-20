/*
 * ptin_cfg.c
 *
 * Created on: 2010/04/08
 * Author: Andre Brizido
 *
 * Modified on 2011/06/14 
 * By: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: The PTIN module works as an interface to configure other Fastpath
 *        modules, and therefore, does not require the same configuration
 *        structure as the typical fastpath modules.
 */

#include "ptin_cfg.h"
#include "ptin_evc.h"
#include "ptin_intf.h"
#include "ptin_fieldproc.h"
#include "usmdb_sim_api.h"

/* External VLAN used for inBand management purposes */
static L7_uint16 vlan_inband = 0xFFFF;

/**
 * Gets Network Connectivity configuration (inBand management)
 * 
 * @param ntwConn Pointer to the output data (mask defines what to read)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_cfg_ntw_connectivity_get(ptin_NtwConnectivity_t *ntwConn)
{
  L7_RC_t   rc;
  L7_uint32 mask;
  L7_uint   i;
  ptin_HwEthMef10Evc_t evcConf;

  mask = ntwConn->mask;
  ntwConn->mask = 0;

  /* IP Address */
  if (mask & PTIN_NTWCONN_MASK_IPADDR)
  {
    rc = usmDbAgentIpIfAddressGet(1, &ntwConn->ipaddr);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_API, "Error getting inBand management IPv4 Address");
      return L7_FAILURE;
    }
    ntwConn->mask |= PTIN_NTWCONN_MASK_IPADDR;
  }

  /* Network Mask */
  if (mask & PTIN_NTWCONN_MASK_NETMASK)
  {
    rc = usmDbAgentIpIfNetMaskGet(1, &ntwConn->netmask);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_API, "Error getting inBand management NetMask");
      return L7_FAILURE;
    }
    ntwConn->mask |= PTIN_NTWCONN_MASK_NETMASK;
  }

  /* Gateway Address */
  if (mask & PTIN_NTWCONN_MASK_GATEWAY)
  {
    rc = usmDbAgentIpIfDefaultRouterGet(1, &ntwConn->gateway);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_API, "Error getting inBand management Gateway Addr");
      return L7_FAILURE;
    }
    ntwConn->mask |= PTIN_NTWCONN_MASK_GATEWAY;
  }

  /* Interface & VLAN */
  if (mask & PTIN_NTWCONN_MASK_INTF ||
      mask & PTIN_NTWCONN_MASK_VLAN)
  {
    /* Read EVC */
    evcConf.index = PTIN_EVC_INBAND;
    if (ptin_evc_get(&evcConf) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_API, "Error getting inBand interface and VLAN info (EVC read error)");
    }
    else
    {
      /* Interface */
      if (mask & PTIN_NTWCONN_MASK_INTF)
      {
        for (i=0; i<evcConf.n_intf; i++)
        {
          ntwConn->intf[i].intf_id   = evcConf.intf[i].intf_id;
          ntwConn->intf[i].intf_type = evcConf.intf[i].intf_type;
        }
        ntwConn->mask |= PTIN_NTWCONN_MASK_INTF;
      }

      /* VLAN ID */
      if (mask & PTIN_NTWCONN_MASK_VLAN)
      {
        ntwConn->mgmtVlanId = evcConf.intf[0].vid;
        ntwConn->mask |= PTIN_NTWCONN_MASK_VLAN;
      }
    }
  }

  return L7_SUCCESS;
}

/**
 * Sets Network Connectivity configuration
 * 
 * @param ntwConn Pointer to the config data (mask defines what to set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_cfg_ntw_connectivity_set(ptin_NtwConnectivity_t *ntwConn)
{
  L7_RC_t   rc;
  L7_uint32 old_ipaddr  = 0;
  L7_uint32 old_netmask = 0;
  L7_uint32 old_gateway = 0;
  L7_uint32 old_vlan    = 0;
  L7_uint32 undo_mask   = 0;
  L7_uint   i;
  ptin_HwEthMef10Evc_t evcConf;
#if 0
  L7_uint32 intIfNum;
  L7_uint32 ipAddr;
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
#endif

  /* Get current config */
  rc = 0;
  rc |= usmDbAgentIpIfAddressGet(1, &old_ipaddr);
  rc |= usmDbAgentIpIfNetMaskGet(1, &old_netmask);
  rc |= usmDbAgentIpIfDefaultRouterGet(1, &old_gateway);
  rc |= usmDbMgmtVlanIdGet(1, &old_vlan);
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_API, "Error reading current inBand management config");
    return L7_FAILURE;
  }

  /* Apply config */
  rc = L7_SUCCESS;
  do
  {
    /* IP Address */
    if (ntwConn->mask & PTIN_NTWCONN_MASK_IPADDR)
    {
      rc = usmDbAgentIpIfAddressSet(1, ntwConn->ipaddr);
      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_API, "Error setting inBand management IPv4 Address");
        break;
      }
      undo_mask |= PTIN_NTWCONN_MASK_IPADDR;
    }

    /* Network Mask */
    if (ntwConn->mask & PTIN_NTWCONN_MASK_NETMASK)
    {
      rc = usmDbAgentIpIfNetMaskSet(1, ntwConn->netmask);
      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_API, "Error setting inBand management NetMask");
        break;
      }
      undo_mask |= PTIN_NTWCONN_MASK_NETMASK;
    }

    /* Gateway Address */
    if (ntwConn->mask & PTIN_NTWCONN_MASK_GATEWAY)
    {
      rc = usmDbAgentIpIfDefaultRouterSet(1, ntwConn->gateway);
      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_API, "Error setting inBand management Gateway Addr");
        break;
      }
      undo_mask |= PTIN_NTWCONN_MASK_GATEWAY;
    }

    /* IP Address */
    if (ntwConn->mask & PTIN_NTWCONN_MASK_IPADDR &&
        ntwConn->ipaddr == 0)
    {
      /* Delete EVC if was already created */
      evcConf.index = PTIN_EVC_INBAND;
      if (ptin_evc_get(&evcConf) == L7_SUCCESS)
        ptin_evc_delete(evcConf.index);
    }

    /* Create EVC for inBand (when either intf or vlan change */
    if (ntwConn->mask & PTIN_NTWCONN_MASK_INTF ||
        ntwConn->mask & PTIN_NTWCONN_MASK_VLAN)
    {
      /* Delete EVC if was already created */
      evcConf.index = PTIN_EVC_INBAND;
      if (ptin_evc_get(&evcConf) == L7_SUCCESS)
        ptin_evc_delete(evcConf.index);

      /* Create a new EVC */
      memset(&evcConf, 0x00, sizeof(evcConf));
      evcConf.index             = PTIN_EVC_INBAND;
      evcConf.flags             = PTIN_EVC_MASK_MACLEARNING | PTIN_EVC_MASK_CPU_TRAPPING;
      evcConf.mc_flood          = PTIN_EVC_MC_FLOOD_ALL;
      evcConf.n_intf            = ntwConn->n_intf;
      for (i=0; i<evcConf.n_intf; i++)
      {
        evcConf.intf[i].intf_id   = ntwConn->intf[i].intf_id;
        evcConf.intf[i].intf_type = ntwConn->intf[i].intf_type;
        evcConf.intf[i].mef_type  = PTIN_EVC_INTF_ROOT;
        evcConf.intf[i].vid       = ntwConn->mgmtVlanId;
      }

      rc = ptin_evc_create(&evcConf);
      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_API, "Error creating EVC# %u for inBand management purposes", PTIN_EVC_INBAND);
        break;
      }
      undo_mask |= PTIN_NTWCONN_MASK_INTF;

      vlan_inband = ntwConn->mgmtVlanId;  /* Save external VLAN */

      /* VLAN ID */
      rc = usmDbMgmtVlanIdSet(1, PTIN_VLAN_INBAND);
      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_API, "Error setting inBand management VLAN ID");
        break;
      }
      undo_mask |= PTIN_NTWCONN_MASK_VLAN;
    }
    else
    {
      /* Reset VLAN ID */
      rc = usmDbMgmtVlanIdSet(1, 0);
      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_API, "Error resetting inBand management VLAN ID to ZERO");
        break;
      }
      else
      {
        LOG_TRACE(LOG_CTX_API, "inBand VLAN cleared");
      }
    }
  } while (0);

  /* Errors ? Undo config... */
  if (rc != L7_SUCCESS)
  {
    if (undo_mask & PTIN_NTWCONN_MASK_IPADDR)
      usmDbAgentIpIfAddressSet(1, old_ipaddr);
    if (undo_mask & PTIN_NTWCONN_MASK_NETMASK)
      usmDbAgentIpIfNetMaskSet(1, old_netmask);
    if (undo_mask & PTIN_NTWCONN_MASK_GATEWAY)
      usmDbAgentIpIfDefaultRouterSet(1, old_gateway);
    if (undo_mask & PTIN_NTWCONN_MASK_INTF)
      ptin_evc_delete(PTIN_EVC_INBAND);
    if (undo_mask & PTIN_NTWCONN_MASK_VLAN)
      usmDbMgmtVlanIdSet(1, old_vlan);
    return rc;
  }

  /* Let hapiBroadFfpSysMacInstall function do the job, for trapping inband packets */
#if 0
  /* Create/remove inband rule to trap packets to the CPU */
  simGetSystemIPBurnedInMac(macAddr);
  LOG_TRACE(LOG_CTX_API,"MacAddr=%02X:%02X:%02X:%02X:%02X:%02X",macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
  if (usmDbAgentIpIfAddressGet(1,&ipAddr)!=L7_SUCCESS)
  {
    LOG_WARNING(LOG_CTX_API, "Error acquiring dtl0 ipaddress");
    ipAddr = 0;
  }
  LOG_TRACE(LOG_CTX_API,"IP_Addr=%u.%u.%u.%u",(ipAddr>>24) & 0xFF,(ipAddr>>16) & 0xFF,(ipAddr>>8) & 0xFF,ipAddr & 0xFF);
  if ( (ntwConn->intf_type!=PTIN_EVC_INTF_PHYSICAL && ntwConn->intf_type!=PTIN_EVC_INTF_LOGICAL) ||
       (ntwConn->intf_type==PTIN_EVC_INTF_PHYSICAL && ptin_intf_port2intf(ntwConn->intf_id,&intIfNum)!=L7_SUCCESS) ||
       (ntwConn->intf_type==PTIN_EVC_INTF_LOGICAL  && ptin_intf_lag2intf(ntwConn->intf_id,&intIfNum)!=L7_SUCCESS) )
  {
    LOG_WARNING(LOG_CTX_API, "Error calculating intIfNum");
    intIfNum = L7_ALL_INTERFACES;
  }
  LOG_TRACE(LOG_CTX_API,"intIfNum=%u",intIfNum);
  /* Create trap rule */
  if (ipAddr!=0)
  {
    if ((rc=ptin_inbandPkts_trap_set(intIfNum,PTIN_VLAN_INBAND,ipAddr,macAddr))!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_API, "Error activating rule to trap inband packets");
    }
    else
    {
      LOG_TRACE(LOG_CTX_API, "Success activating rule to trap inband packets");
    }
  }
  /* Remove rule */
  else
  {
    if ((rc=ptin_inbandPkts_trap_clear())!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_API, "Error deactivating rule to trap inband packets");
    }
    else
    {
      LOG_TRACE(LOG_CTX_API, "Success deactivating rule to trap inband packets");
    }
  }
#endif

  return L7_SUCCESS;
}

/**
 * Returns the external inBand VLAN ID
 * 
 * @return L7_uint16 inBand VID (zero if not configured)
 */
inline L7_uint16 ptin_cfg_inband_vlan_get(void)
{
  return vlan_inband;
}


/**
 * Creates a bridge between dtl0 interface and a virtual interface eth0.4093 
 *  
 * NOTE: 
 *  1. virtual interface eth0.4093 is created here 
 *  2. all operations are accomplished through an external shell script 
 *     '/usr/local/ptin/scripts/startBridge.sh'
 * 
 * @author alex (4/10/2012)
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_cfg_inband_bridge_set(void)
{
  int rc;

  rc = system(PTIN_INBAND_BRIDGE_SCRIPT);
  if (rc != 0)
  {
    LOG_ERR(LOG_CTX_API, "Error executing script " PTIN_INBAND_BRIDGE_SCRIPT " (rc=%d)", rc);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_API, "InBand bridge script successfully executed");

  return L7_SUCCESS;
}


