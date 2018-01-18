/*
*  C Implementation: switch_flow
*
* Description: 
*
*
* Author: Milton Ruas,,, <mruas@mruas-laptop>, (C) 2010
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#include <dapi_db.h>
#include <hpc_db.h>
#include <soc/counter.h>

#include "ptin_globaldefs.h"
#include "logger.h"
#include "ptin_hapi.h"
#include "ptin_hapi_xlate.h"
#include "ptin_hapi_xconnect.h"
#include "ptin_hapi_l2.h"
#include "ptin_hapi_l3.h"
#include "ptin_hapi_fp_bwpolicer.h"
#include "ptin_hapi_fp_counters.h"
#include "broad_policy.h"
#include "simapi.h"
#include "broad_group_bcm.h"
#include "bcm_int/esw/link.h"
#include <bcmx/switch.h>
#include <bcmx/port.h>
#include <bcmx/l2.h>
#if 0//Required to init L3 Modules. Not used since FP is already performing the init of those Modules
#include <bcm/init.h>
#endif

/********************************************************************
 * DEFINES
 ********************************************************************/

/********************************************************************
 * TYPES DEFINITION
 ********************************************************************/

/********************************************************************
 * GLOBAL VARIABLES
 ********************************************************************/

extern L7_uint64 hapiBroadReceive_packets_count;
extern L7_uint64 hapiBroadReceice_igmp_count;
extern L7_uint64 hapiBroadReceice_mld_count;
extern L7_uint64 hapiBroadReceice_dhcpv4_count;
extern L7_uint64 hapiBroadReceice_dhcpv6_count;
extern L7_uint64 hapiBroadReceice_pppoe_count;

BROAD_POLICY_t lacp_policyId   = BROAD_POLICY_INVALID;
BROAD_POLICY_t bl2cpu_policyId = BROAD_POLICY_INVALID;

/********************************************************************
 * INTERNAL VARIABLES
 ********************************************************************/

L7_int bcm_unit = 0;

/* Lookup map to provide internal port# based on physical port */
static DAPI_USP_t usp_map[PTIN_SYSTEM_N_PORTS];

BROAD_POLICY_t inband_policyId = 0;

#if (PTIN_BOARD_IS_MATRIX)
 int ptin_sys_slotport_to_intf_map[PTIN_SYS_SLOTS_MAX+1][PTIN_SYS_INTFS_PER_SLOT_MAX];
 int ptin_sys_intf_to_slot_map[PTIN_SYSTEM_N_PORTS];
 int ptin_sys_intf_to_port_map[PTIN_SYSTEM_N_PORTS];
#endif
int ptin_sys_number_of_ports = PTIN_SYSTEM_N_PORTS;

/* Root ports list for egress isolation purposes */
bcm_pbmp_t pbm_egress_all_ports;
bcm_pbmp_t pbm_egress_root_ports;
bcm_pbmp_t pbm_egress_community_ports;

/* Save storm control rate values */
ptin_stormControl_t stormControl_backup = { 0, 0, RATE_LIMIT_BCAST, RATE_LIMIT_MCAST, RATE_LIMIT_UCUNK, RATE_LIMIT_CPU };

/********************************************************************
 * MACROS AND INLINE FUNCTIONS
 ********************************************************************/

/********************************************************************
 * INTERNAL FUNCTIONS PROTOTYPES
 ********************************************************************/

static L7_RC_t hapi_ptin_portMap_init(void);

L7_RC_t hapi_ptin_egress_ports(L7_uint port_frontier);

L7_RC_t ptin_hapi_linkscan_execute(bcm_port_t bcm_port, L7_uint8 enable);

/**
 * Apply global switch configurations
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
//static L7_RC_t ptin_hapi_switch_init(void);


/********************************************************************
 * EXTERNAL FUNCTIONS IMPLEMENTATION
 ********************************************************************/

/**
 * Initializes PTin HAPI data structures
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_data_init(void)
{
  L7_RC_t rc;

  /* Port mapping initializations (PLD mapping must be ok first!) */
  rc = hapi_ptin_portMap_init();
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error: rc=%d", rc);
    return L7_FAILURE;
  }
  
  /* Field processor initializations */
  rc = hapi_ptin_bwPolicer_init();
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error: rc=%d", rc);
    return L7_FAILURE;
  }

  rc = hapi_ptin_fpCounters_init();
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error: rc=%d", rc);
    return L7_FAILURE;
  }

  rc = ptin_hapi_maclimit_init();
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error: rc=%d", rc);
    return L7_FAILURE;
  }
  /* PTin removed: Dune */
#if 0
  rc = ptin_hapi_l3_intf_init();
  if (rc != L7_SUCCESS)
    return L7_FAILURE;
#endif

  return rc;
}

/**
 * Initializes PTin HAPI configurations
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_config_init(void)
{
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_STARTUP,"Starting config_init...");

  /* Switch initializations */
  if (ptin_hapi_switch_init()!=L7_SUCCESS)
    rc = L7_FAILURE;

  /* PHY initializations */
  if (ptin_hapi_phy_init()!=L7_SUCCESS)
    rc = L7_FAILURE;

  /* ptin_hapi_xlate initializations */
  if (ptin_hapi_xlate_init()!=L7_SUCCESS)
    rc = L7_FAILURE;

  /* ptin_hapi_bridge initializations */
  if (ptin_hapi_bridge_init()!=L7_SUCCESS)
    rc = L7_FAILURE;

  PT_LOG_TRACE(LOG_CTX_STARTUP,"Config_init done! rc=%d", rc);

  return rc;
}

/**
 * Initialize Switch control parameters
 * 
 * @author mruas (5/31/2012)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_switch_init(void)
{
  return L7_SUCCESS;
}

/**
 * Init hash procedures
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_hash_init(void)
{
  L7_RC_t    rc = L7_SUCCESS;

#if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
  L7_uint32 i, banks;
  L7_int    hash_type;
  L7_uint32 hash_offset;

  /* Hash tables depth */
  if (bcm_switch_control_set(0, bcmSwitchHashDualMoveDepth, 8) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error setting bcmSwitchHashDualMoveDepth switch_control to 8");
    rc = L7_FAILURE;
  }
  if (bcm_switch_control_set(0, bcmSwitchHashDualMoveDepthEgressVlan, 8) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error setting bcmSwitchHashDualMoveDepthEgressVlan switch_control to 8");
    rc = L7_FAILURE;
  }

  /* Ingress tranlation hash tables */
  if (bcm_switch_hash_banks_max_get(0, bcmHashTableVlanTranslate, &banks) == BCM_E_NONE)
  {
    PT_LOG_INFO(LOG_CTX_HAPI,"bcmHashTableVlanTranslate: number of banks=%u", banks);

    for (i=0; i<banks; i++)
    {
      /* Show default */
      if (bcm_switch_hash_banks_config_get(0, bcmHashTableVlanTranslate, i, &hash_type, &hash_offset) == BCM_E_NONE)
      {
        PT_LOG_INFO(LOG_CTX_HAPI,"bcmHashTableVlanTranslate:bank%u => Default is hash_type=0x%x hash_offset=%u", i, hash_type, hash_offset);
      }
      /* Bank 0 */
      switch (i)
      {
        case 0:
          hash_type = BCM_HASH_CRC32U;
          hash_offset = 0;
          break;
        case 1:
          hash_type = BCM_HASH_CRC32L;
          hash_offset = 0;
          break;
        default:
          hash_type = BCM_HASH_OFFSET;
          hash_offset = (i-2)*4+4;
          break;
      }

      if (bcm_switch_hash_banks_config_set(0, bcmHashTableVlanTranslate, i, hash_type, hash_offset) != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"Error setting bcmHashTableVlanTranslate:bank%u attribute => hash_type=%d, hash_offset=%u", i, hash_type, hash_offset);
        rc = L7_FAILURE;
      }
      else
      {
        PT_LOG_INFO(LOG_CTX_HAPI,"Success setting bcmHashTableVlanTranslate:bank%u attribute => hash_type=%d, hash_offset=%u", i, hash_type, hash_offset);
      }
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error reading max number of banks for bcmHashTableVlanTranslate table");
    rc = L7_FAILURE;
  }

  /* Egress tranlation hash tables */
  if (bcm_switch_hash_banks_max_get(0, bcmHashTableEgressVlanTranslate, &banks) == BCM_E_NONE)
  {
    PT_LOG_INFO(LOG_CTX_HAPI,"bcmHashTableEgressVlanTranslate: number of banks=%u", banks);
    for (i=0; i<banks; i++)
    {
      /* Show default */
      if (bcm_switch_hash_banks_config_get(0, bcmHashTableEgressVlanTranslate, i, &hash_type, &hash_offset) == BCM_E_NONE)
      {
        PT_LOG_INFO(LOG_CTX_HAPI,"bcmHashTableEgressVlanTranslate:bank%u => Default is hash_type=0x%x hash_offset=%u", i, hash_type, hash_offset);
      }
      /* Bank 0 */
      switch (i)
      {
        case 0:
          hash_type = BCM_HASH_CRC32U;
          hash_offset = 0;
          break;
        case 1:
          hash_type = BCM_HASH_CRC32L;
          hash_offset = 0;
          break;
        default:
          hash_type = BCM_HASH_OFFSET;
          hash_offset = (i-2)*4+4;
          break;
      }

      if (bcm_switch_hash_banks_config_set(0, bcmHashTableEgressVlanTranslate, i, hash_type, hash_offset) != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"Error setting bcmHashTableEgressVlanTranslate:bank%u attribute => hash_type=%d, hash_offset=%u", i, hash_type, hash_offset);
        rc = L7_FAILURE;
      }
      else
      {
        PT_LOG_INFO(LOG_CTX_HAPI,"Success setting bcmHashTableEgressVlanTranslate:bank%u attribute => hash_type=%d, hash_offset=%u", i, hash_type, hash_offset);
      }
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error reading max number of banks for bcmHashTableEgressVlanTranslate table");
    rc = L7_FAILURE;
  }

  PT_LOG_INFO(LOG_CTX_HAPI,"Switch %u initialized!", bcm_unit);
#endif

  return rc;
}

/**
 * Initialize PHY control parameters
 * 
 * @author asantos (07/02/2013)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_phy_init(void)
{
  L7_uint i;
  bcm_port_t bcm_port;
  bcm_gport_t gport;
  bcm_error_t rv;

  /* Run all ports */
  for (i=0; i<ptin_sys_number_of_ports; i++)
  {
    /* Get bcm_port format */
    if (hapi_ptin_bcmPort_get(i, &bcm_port)!=BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP, "Error obtaining bcm_port for port %u", i);
      continue;
    }

    BCM_GPORT_LOCAL_SET(gport, bcm_port);

    PT_LOG_INFO(LOG_CTX_STARTUP, "Initializing port %u -> bcm_port %u / gport 0x%08x", i, bcm_port, gport);

    #if 0
    /* TAG mode */
    rv = bcm_port_dtag_mode_set(bcm_unit, bcm_port, BCM_PORT_DTAG_MODE_INTERNAL);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP, "error: bcm_port_dtag_mode_set for bcm_port %d: rv=%d (%s)", bcm_port, rv, bcm_errmsg(rv));
      return rv;
    }

    /* Filtering */
    rv = bcm_port_vlan_member_set(bcm_unit, bcm_port, BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS | BCM_PORT_VLAN_MEMBER_VP_VLAN_MEMBERSHIP);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP, "error: bcm_port_vlan_member_set for bcm_port %d: rv=%d (%s)", bcm_port, rv, bcm_errmsg(rv));
      return rv;
    }

    /* MAC learning */
    rv = bcm_port_learn_set(bcm_unit, gport, BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP, "error: bcm_port_tpid_set for gport 0x%x: rv=%d (%s)", gport, rv, bcm_errmsg(rv));
      return rv;
    }

    /* Default VLAN */
    rv = bcm_port_untagged_vlan_set(bcm_unit, bcm_port, 1);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP, "error: bcm_port_untagged_vlan_set for bcm_port %u: rv=%d (%s)", bcm_port, rv, bcm_errmsg(rv));
      return rv;
    }

    /* Remove port from VLAN 1 */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, bcm_port);
    rv = bcm_vlan_port_remove(bcm_unit, 1, pbmp);
    if (rv != BCM_E_NONE) {
      PT_LOG_ERR(LOG_CTX_STARTUP, "Error, bcm_vlan_port_remove with vlan 1, bcm_port %d: rv=%d (%s)", bcm_port, rv, bcm_errmsg(rv));
      return rv;
    }
    #endif

    /* TPIDs */
    rv = bcm_port_tpid_delete_all(bcm_unit, bcm_port);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP, "error: bcm_port_tpid_delete_all to bcm_port %d: rv=%d (%s)\n", bcm_port, rv, bcm_errmsg(rv));
      return rv;
    }
    rv = bcm_port_tpid_set(bcm_unit, bcm_port, PTIN_TPID_OUTER_DEFAULT);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP, "error: bcm_port_tpid_set to bcm_port %d: rv=%d (%s)", bcm_port, rv, bcm_errmsg(rv));
      return rv;
    }
    rv = bcm_port_inner_tpid_set(bcm_unit, bcm_port, PTIN_TPID_INNER_DEFAULT);
    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP, "error: bcm_port_inner_tpid_set for bcm_port %d: rv=%d (%s)", bcm_port, rv, bcm_errmsg(rv));
      return rv;
    }
  }

  PT_LOG_INFO(LOG_CTX_STARTUP,"All PHYs initialized!");

  return L7_SUCCESS;
}

/**
 * Post-Initialize PHY control parameters
 * 
 * @author asantos (07/02/2013)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_phy_post_init(void)
{
  return L7_SUCCESS;
}


/**
 * Reset a warpcore
 * 
 * @param slot_id : backplane slot id
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_warpcore_reset(L7_int slot_id)
{
  L7_RC_t    rc = L7_SUCCESS;

#if (PTIN_BOARD_IS_MATRIX)
  L7_uint    i;
  bcm_port_t bcm_port;
  bcm_pbmp_t pbm, pbm_out;

  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_WC_PORT_MAP_t           *hapiWCMapPtr;

  /* Get WC port map */
  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr          = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiWCMapPtr         = dapiCardPtr->wcPortMap;

  BCM_PBMP_CLEAR(pbm);

  /* Run all slots */
  for (i=0; i<ptin_sys_number_of_ports; i++)
  {
    /* Get bcm_port format */
    if (hapi_ptin_bcmPort_get(i, &bcm_port)!=BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error obtaining bcm_port for port %u", i);
      continue;
    }
    /* Skip non SFI ports */
    if (hapiWCMapPtr[i].slotNum < 0)
    {
      PT_LOG_TRACE(LOG_CTX_HAPI, "bcm_port %u (port %u) not considered", bcm_port, i);
      continue;
    }
    /* And add the ports associated to the provided slot_id */
    if (hapiWCMapPtr[i].slotNum == slot_id)
    {
      BCM_PBMP_PORT_ADD(pbm, bcm_port);
      PT_LOG_INFO(LOG_CTX_HAPI, "bcm_port %u (port %u) added to list of ports to be reseted", bcm_port, i);
    }
  }
  
  /* Check if list has ports */
  if (BCM_PBMP_IS_NULL(pbm))
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "List of selected ports is empty... nothing to be done!");
    return L7_NOT_EXIST;
  }

  /* Detach ports */
  if (bcm_port_detach(0, pbm, &pbm_out) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error dettaching selected ports");
    rc = L7_FAILURE;
  }
  PT_LOG_INFO(LOG_CTX_HAPI, "Selected ports detached");

  /* Probe ports */
  if (bcm_port_probe(0, pbm, &pbm_out) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error probing selected ports");
    rc = L7_FAILURE;
  }
  PT_LOG_INFO(LOG_CTX_HAPI, "Selected ports probed");

  /* Reenable ports */
  BCM_PBMP_ITER(pbm, bcm_port)
  {
    /* Disable ports */
    if (bcm_port_enable_set(0, bcm_port, L7_DISABLE) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error disabling bcm_port %u", bcm_port);
      rc = L7_FAILURE;
    }
    if (bcm_port_autoneg_set(0, bcm_port, L7_DISABLE) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_port_autoneg_set to bcm_port %u", bcm_port);
      rc = L7_FAILURE;
    }
    if (bcm_port_duplex_set(0, bcm_port, L7_ENABLE) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_port_duplex_set to bcm_port %u", bcm_port);
      rc = L7_FAILURE;
    }
    if (bcm_port_pause_set(0, bcm_port, L7_DISABLE, L7_DISABLE) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_port_pause_set to bcm_port %u", bcm_port);
      rc = L7_FAILURE;
    }
    if (bcm_port_interface_set(0, bcm_port, BCM_PORT_IF_SFI) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_port_interface_set to bcm_port %u", bcm_port);
      rc = L7_FAILURE;
    }
    /* Reenable ports */
    if (bcm_port_enable_set(0, bcm_port, L7_ENABLE) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error reenabling bcm_port %u", bcm_port);
      rc = L7_FAILURE;
    }
    if (bcm_port_stp_set(0, bcm_port, BCM_PORT_STP_FORWARD) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_port_stp_set to bcm_port %u", bcm_port);
      rc = L7_FAILURE;
    }
    PT_LOG_INFO(LOG_CTX_HAPI, "bcm_port %u reconfigured", bcm_port);
  }

  if (rc == L7_SUCCESS)
  {
    PT_LOG_NOTICE(LOG_CTX_HAPI, "Selected ports reseted successfully");
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error reseting selected ports");
  }

  /* Wait 100ms */
  osapiSleepMSec(100);
#endif

  return rc;
}

/**
 * Execute a linkscan procedure
 * 
 * @param bcm_port 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_hapi_linkscan_execute(bcm_port_t bcm_port, L7_uint8 enable)
{
#ifdef BCM_ESW_SUPPORT
  int fault;
#endif
  int linkStatus;
  bcm_pbmp_t pbmp;

  PT_LOG_NOTICE(LOG_CTX_HAPI, "Starting Linkscan procedure to bcm_port %u", bcm_port);

  /* Execute linkscan */
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_SET(pbmp, bcm_port);

  if (enable)
  {
  #ifdef BCM_ESW_SUPPORT
    /* Clear link faults */
    PT_LOG_INFO(LOG_CTX_HAPI, "_ptin_esw_link_fault_get from bcm_port %u", bcm_port);
    if (_ptin_esw_link_fault_get(0, bcm_port, &fault) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error clearing faults for bcm_port %u", bcm_port);
    }
  #endif
    /* Read link status once, to update its real value */
    PT_LOG_INFO(LOG_CTX_HAPI, "bcm_port_link_status_get from bcm_port %u", bcm_port);
    if (bcm_port_link_status_get(0, bcm_port, &linkStatus) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error reading link status for bcm_port %u", bcm_port);
    }
    /* Enable linkscan */
    PT_LOG_INFO(LOG_CTX_HAPI, "bcm_linkscan_mode_set:1 to bcm_port %u", bcm_port); 
    if (bcm_linkscan_mode_set(0, bcm_port, BCM_LINKSCAN_MODE_SW) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error enabling linkscan for bcm_port %u", bcm_port);
      return L7_FAILURE;
    }
  }

  #if 0
  /* Execute a linkscan update */
  PT_LOG_INFO(LOG_CTX_HAPI, "bcm_linkscan_update to bcm_port %u", bcm_port);
  if (bcm_linkscan_update(bcm_unit, pbmp) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error applying linkscan to bcm_port %u", bcm_port);
  }
  #endif

  if (!enable)
  {
    /* Disable linkscan */
    PT_LOG_INFO(LOG_CTX_HAPI, "bcm_linkscan_mode_set:0 to bcm_port %u", bcm_port); 
    if (bcm_linkscan_mode_set(0, bcm_port, BCM_LINKSCAN_MODE_NONE) != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error enabling linkscan for bcm_port %u", bcm_port);
      return L7_FAILURE;
    }
  }

  PT_LOG_NOTICE(LOG_CTX_HAPI, "Linkscan applied to bcm_port %u (enable=%u)", bcm_port, enable);

  return L7_SUCCESS;
}

/**
 * Initialize egress ports for all physical ports
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t hapi_ptin_egress_ports(L7_uint port_frontier)
{
  int i, unit=0;
  bcm_port_t bcm_port;
  bcmx_lport_t lport_cpu;
  bcm_port_t bcm_port_cpu;

  /* Validate arguments */
  if (port_frontier>=ptin_sys_number_of_ports)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Invalid port frontier (%u)",port_frontier);
    return L7_FAILURE;
  }

  /* Switch unit */
  if (hapi_ptin_bcmUnit_get(&unit) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"bcm_port map not initialized!");
    return L7_FAILURE;
  }

  /* Prepare port bitmaps */
  BCM_PBMP_CLEAR(pbm_egress_all_ports);
  BCM_PBMP_CLEAR(pbm_egress_root_ports);
  BCM_PBMP_CLEAR(pbm_egress_community_ports);

  /* Add CPU to port bitmaps */
  if (bcmx_lport_local_cpu_get(0, &lport_cpu) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error with bcmx_lport_local_cpu_get");
    return L7_FAILURE;
  }
  bcm_port_cpu = bcmx_lport_bcm_port(lport_cpu);
  if (bcm_port_cpu < 0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error with bcmx_lport_bcm_port");
    return L7_FAILURE;
  }
  BCM_PBMP_PORT_ADD(pbm_egress_all_ports, bcm_port_cpu);
  BCM_PBMP_PORT_ADD(pbm_egress_root_ports, bcm_port_cpu);
  BCM_PBMP_PORT_ADD(pbm_egress_community_ports, bcm_port_cpu);

  for (i=0; i<ptin_sys_number_of_ports; i++)
  {
    if (hapi_ptin_bcmPort_get(i, &bcm_port) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error getting bcm_port for port %u",i);
      return L7_FAILURE;
    }
    /* All ports */
    BCM_PBMP_PORT_ADD(pbm_egress_all_ports, bcm_port);
    /* Root/community ports */
    if (i>=port_frontier)
    {
      BCM_PBMP_PORT_ADD(pbm_egress_root_ports, bcm_port);
      BCM_PBMP_PORT_ADD(pbm_egress_community_ports, bcm_port);
    }
  }

  PT_LOG_TRACE(LOG_CTX_HAPI,"PBM_ALL:");
  for (i=0; i<_SHR_PBMP_WORD_MAX; i++)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"0x%08x",pbm_egress_all_ports.pbits[i]);
  }
  PT_LOG_TRACE(LOG_CTX_HAPI,"PBM_ROOT:");
  for (i=0; i<_SHR_PBMP_WORD_MAX; i++)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"0x%08x",pbm_egress_root_ports.pbits[i]);
  }
  PT_LOG_TRACE(LOG_CTX_HAPI,"PBM_COMMUNITY:");
  for (i=0; i<_SHR_PBMP_WORD_MAX; i++)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI,"0x%08x",pbm_egress_community_ports.pbits[i]);
  }

  /* PON ports: egress ports are the ethernet ones only */
  for (i=0; i<port_frontier; i++)
  {
    if (hapi_ptin_bcmPort_get(i, &bcm_port) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error getting bcm_port for port %u",i);
      return L7_FAILURE;
    }
    /* Configure egress ports list */
    if (bcm_port_egress_set(unit, bcm_port, 0, pbm_egress_root_ports)!=BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error setting egress bitmap for port %u",i);
      return L7_FAILURE;
    }
    PT_LOG_DEBUG(LOG_CTX_HAPI,"Egress bitmap configured for PON port %u (bcm_port=%u)",i, bcm_port);
  }
  /* ETH ports */
  for (i=port_frontier; i<ptin_sys_number_of_ports; i++)
  {
    if (hapi_ptin_bcmPort_get(i, &bcm_port) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error getting bcm_port for port %u",i);
      return L7_FAILURE;
    }
    /* Configure egress ports list */
    if (bcm_port_egress_set(unit, bcm_port, 0, pbm_egress_all_ports)!=BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error setting egress bitmap for port %u",i);
      return L7_FAILURE;
    }
    PT_LOG_DEBUG(LOG_CTX_HAPI,"Egress bitmap configured for ETH port %u (bcm_port=%u)",i,bcm_port);
  }

  PT_LOG_DEBUG(LOG_CTX_HAPI,"Egress bitmap configured for all ports");

  return L7_SUCCESS;
}

/** 
 * Get bcm unit id for this switch. 
 * Normally is ZERO, but nervertheless it's better to be sure 
 * 
 * @param bcm_unit: switch unit id
 * 
 * @return L7_RC_t: always L7_SUCCESS
 */
L7_RC_t hapi_ptin_bcmUnit_get(L7_int *bcm_unit)
{
  if (bcm_unit!=L7_NULLPTR)
  {
    *bcm_unit = usp_map[0].unit;
  }

  return L7_SUCCESS;
}

/**
 * Get sdk port reference
 * 
 * @param port: PTin port id 
 *        bcm_port: Pointer to the location where the bcm_port
 *        will be stored
 *  
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_bcmPort_get(L7_int port, L7_int *bcm_port)
{
  if (port >= ptin_sys_number_of_ports)
    return L7_FAILURE;

  if (bcm_port!=L7_NULLPTR)
  {
    *bcm_port = usp_map[port].port;
  }

  return L7_SUCCESS;
}

/**
 * Get pbmp value for a bitmap of ptin_ports
 * 
 * @param port_bmp 
 * @param bcm_pbm 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapi_ptin_bcmPbmPort_get(L7_uint64 port_bmp, pbmp_t *bcm_pbm)
{
  L7_int      port;
  bcm_port_t  bcm_port;

  if (bcm_pbm == L7_NULLPTR)
    return L7_SUCCESS;

  /* Clear port bitmap */
  BCM_PBMP_CLEAR(*bcm_pbm);

  /* Add all uplink ports */
  for (port=0; port<ptin_sys_number_of_ports; port++)
  {
    if (!((1ULL << port) & port_bmp))
      continue;

    if (hapi_ptin_bcmPort_get(port, &bcm_port) == L7_SUCCESS)
    {
      BCM_PBMP_PORT_ADD(*bcm_pbm, bcm_port);
    }
  }

  return L7_SUCCESS;
}

/**
 * Get port if, from the sdk port reference
 * 
 * @param bcm_port: SDK port number 
 *        port: PTin port id
 *  
 * @return L7_RC_t: 
 *        L7_SUCCESS if found
 *        L7_FAILURE otherwise
 */
L7_RC_t hapi_ptin_port_get(L7_int bcm_port, L7_int *port)
{
  L7_int p;

  /* Search for the referenced bcm_port */
  for ( p = 0 ; p < ptin_sys_number_of_ports && usp_map[p].port != bcm_port ; p++ );

  /* bcm_port was not found */
  if ( p >= ptin_sys_number_of_ports )
    return L7_FAILURE;

  /* Return port number */
  if ( port != L7_NULLPTR )
    *port = p;

  return L7_SUCCESS;
}

/**
 * Get port bitmap in pbmp_t format for all physical ports
 * 
 * @param pbmp_mask : port bitmap
 */
void hapi_ptin_allportsbmp_get(pbmp_t *pbmp_mask)
{
  L7_int ptin_port;
  bcm_port_t bcm_port;

  /* Argument must not be a null pointer */
  if (pbmp_mask==L7_NULLPTR)
    return;

  /* Interfaces mask (for inports field) */
  BCM_PBMP_CLEAR(*pbmp_mask);
  for (ptin_port=0; ptin_port<ptin_sys_number_of_ports; ptin_port++)
  {
    if (hapi_ptin_bcmPort_get(ptin_port,&bcm_port)==L7_SUCCESS)
    {
      BCM_PBMP_PORT_ADD(*pbmp_mask,bcm_port);
      //PT_LOG_TRACE(LOG_CTX_HAPI,"Ptin port %d added to pbm_mask",ptin_port);
    }
  }
}

/**
 * Get port descriptor from ddUsp interface
 * 
 * @param ddUsp : unit, slot and port reference
 * @param dapi_g
 * @param intf_desc : interface descriptor with lport, bcm_port 
 *                  (-1 if not physical) and trunk_id (-1 if not
 *                  trunk)
 * @param pbmp : If is a physical port, it will be ADDED to this
 *             port bitmap.
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_portDescriptor_get(DAPI_USP_t *ddUsp, DAPI_t *dapi_g, ptin_hapi_intf_t *intf_desc, pbmp_t *pbmp)
{
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  bcmx_lport_t  lport=-1;
  bcm_trunk_t   trunk_id=-1;
  bcm_port_t    bcm_port=-1;
  L7_uint32     efp_class_port=0, xlate_class_port=0;

  /* Validate interface */
  if (ddUsp==L7_NULLPTR || (ddUsp->unit<0 && ddUsp->slot<0 && ddUsp->port<0))
  {
    PT_LOG_WARN(LOG_CTX_HAPI,"No provided interface!");
    return L7_SUCCESS;
  }
  if (ddUsp->unit<0 || ddUsp->slot<0 || ddUsp->port<0)
  {
    PT_LOG_WARN(LOG_CTX_HAPI,"Invalid interface!");
    return L7_FAILURE;
  }

  dapiPortPtr = DAPI_PORT_GET( ddUsp, dapi_g );
  hapiPortPtr = HAPI_PORT_GET( ddUsp, dapi_g );

  /* Extract lport */
  lport = hapiPortPtr->bcmx_lport;
  PT_LOG_TRACE(LOG_CTX_HAPI,"Analysing interface {%d,%d,%d}: lport=0x%08x",ddUsp->unit,ddUsp->slot,ddUsp->port,lport);

  /* Extract Trunk id */
  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr))
  {
    trunk_id = hapiPortPtr->hapiModeparm.lag.tgid;
    PT_LOG_TRACE(LOG_CTX_HAPI,"Interface {%d,%d,%d} is a lag: trunk_id = %d",ddUsp->unit,ddUsp->slot,ddUsp->port,trunk_id);
  }
  /* Extract Physical port */
  else if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
  {
    bcm_port = hapiPortPtr->bcm_port;
    PT_LOG_TRACE(LOG_CTX_HAPI,"Interface {%d,%d,%d} is a port: bcm_port = %d",ddUsp->unit,ddUsp->slot,ddUsp->port,bcm_port);
  }
  /* Not valid type */
  else
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Interface has a not valid type: error!");
    return L7_FAILURE;
  }

  /* Class port */
  efp_class_port   = (ddUsp->slot*L7_MAX_PORTS_PER_SLOT) + ddUsp->port + 1 + EFP_STD_CLASS_ID_MAX;
  xlate_class_port = (ddUsp->slot*L7_MAX_PORTS_PER_SLOT) + ddUsp->port + 1;

  PT_LOG_TRACE(LOG_CTX_HAPI,"Interface {%d,%d,%d}: efp_class_port=%d xlate_class_port=%d",
            ddUsp->unit,ddUsp->slot,ddUsp->port,efp_class_port,xlate_class_port);

  /* Add physical interface to port bitmap */
  if (pbmp!=L7_NULLPTR && bcm_port>=0)
  {
    BCM_PBMP_PORT_ADD(*pbmp,bcm_port);
  }

  /* Update interface descriptor */
  if (intf_desc!=L7_NULLPTR)
  {
    intf_desc->lport            = lport;
    intf_desc->trunk_id         = trunk_id;
    intf_desc->bcm_port         = bcm_port;
    intf_desc->efp_class_port   = efp_class_port;
    intf_desc->xlate_class_port = xlate_class_port;
  }

  return L7_SUCCESS;
}

/**
 * get linkscan state
 * 
 * @param dapiPort: Port 
 * @param dapi_g 
 * @param enable: linkscan state (output) 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_linkscan_get(DAPI_USP_t *usp, DAPI_t *dapi_g, L7_uint8 *enable)
{
  PT_LOG_INFO(LOG_CTX_HAPI, "Linkscan read from usp {%d,%d,%d}", usp->unit, usp->slot, usp->port);

  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  L7_int mode;

  /* Validate dapiPort */
  if (usp->unit<0 || usp->slot<0 || usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  dapiPortPtr = DAPI_PORT_GET( usp, dapi_g );
  hapiPortPtr = HAPI_PORT_GET( usp, dapi_g );

  /* Accept only physical interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) )
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical", usp->unit, usp->slot, usp->port);
    return L7_NOT_SUPPORTED;
  }

  if (bcm_linkscan_mode_get(0, hapiPortPtr->bcm_port, &mode) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error reading linkscan state for port {%d,%d,%d}/bcm_port %u/port %u to %u",
            usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port);
    return L7_FAILURE;
  }

  if (enable != L7_NULLPTR)
  {
    *enable = (mode != BCM_LINKSCAN_MODE_NONE); 
  }

  return L7_SUCCESS;
}

/**
 * Apply a linkscan to this port
 * 
 * @param dapiPort: Port
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_linkscan_set(DAPI_USP_t *usp, DAPI_t *dapi_g, L7_uint8 enable)
{
  PT_LOG_INFO(LOG_CTX_HAPI, "Linkscan procedure to usp {%d,%d,%d}", usp->unit, usp->slot, usp->port);

  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  L7_int ptin_port;

#if (PTIN_BOARD_IS_MATRIX)
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_WC_PORT_MAP_t           *hapiWCMapPtr;
  bcm_port_if_t                 intf_type;
  bcm_error_t                   rv;
#endif

  /* Validate dapiPort */
  if (usp->unit<0 || usp->slot<0 || usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  dapiPortPtr = DAPI_PORT_GET( usp, dapi_g );
  hapiPortPtr = HAPI_PORT_GET( usp, dapi_g );

  /* Accept only physical interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) )
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical", usp->unit, usp->slot, usp->port);
    return L7_NOT_SUPPORTED;
  }

  /* Validate bcm port */
  if (hapi_ptin_port_get(hapiPortPtr->bcm_port, &ptin_port)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d}/bcm_port %u is not valid", usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port);
    return L7_FAILURE;
  }

#if (PTIN_BOARD_IS_MATRIX)
  /* Get WC port map */
  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr          = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiWCMapPtr         = dapiCardPtr->wcPortMap;

  if ((rv = bcm_port_interface_get(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, &intf_type)) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error retrieving interface type {%d,%d,%d}/bcm_port %u", usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port);
    return L7_FAILURE;
  }

  /* Speed of this interface should be 10G */
  if (ptin_port >= dapiCardPtr->numOfWCPortMapEntries ||    /* Invalid port */
      hapiWCMapPtr[ptin_port].wcSpeedG > 10 ||
      intf_type == BCM_PORT_IF_KR ||
      intf_type == BCM_PORT_IF_KR4)
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "Port {%d,%d,%d}/bcm_port %u/port %u cannot be considered",
                usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port);
    return L7_NOT_SUPPORTED;
  }
#endif

  /* Execute linkscan */
  if (ptin_hapi_linkscan_execute(hapiPortPtr->bcm_port, enable) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error setting linkscan for port {%d,%d,%d}/bcm_port %u/port %u to %u",
            usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port, enable);
    return L7_FAILURE;
  }

  PT_LOG_NOTICE(LOG_CTX_HAPI, "Linkscan applied for port {%d,%d,%d}/bcm_port %u/port %u to %u",
             usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port, enable);

  return L7_SUCCESS;
}

/**
 * Force link in an interface 
 * It is assumed linkscan is disabled for that interface 
 * 
 * @param usp 
 * @param dapi_g 
 * @param link_state 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_link_force(DAPI_USP_t *usp, DAPI_t *dapi_g, L7_uint8 link, L7_uint8 enable)
{
  PT_LOG_INFO(LOG_CTX_HAPI, "Force link procedure (enable=%u) for usp {%d,%d,%d}", enable, usp->unit, usp->slot, usp->port);

  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  L7_int ptin_port, i, link_status;
  bcm_pbmp_t pbmp;
  bcm_error_t rv;

#if (PTIN_BOARD_IS_MATRIX)
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_WC_PORT_MAP_t           *hapiWCMapPtr;
  bcm_port_if_t                 intf_type;
#endif

  /* Validate dapiPort */
  if (usp->unit<0 || usp->slot<0 || usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  dapiPortPtr = DAPI_PORT_GET( usp, dapi_g );
  hapiPortPtr = HAPI_PORT_GET( usp, dapi_g );

  /* Accept only physical interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) )
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical", usp->unit, usp->slot, usp->port);
    return L7_NOT_SUPPORTED;
  }

  /* Validate bcm port */
  if (hapi_ptin_port_get(hapiPortPtr->bcm_port, &ptin_port)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d}/bcm_port %u is not valid", usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port);
    return L7_FAILURE;
  }

#if (PTIN_BOARD_IS_MATRIX)
  /* Get WC port map */
  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr          = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiWCMapPtr         = dapiCardPtr->wcPortMap;

  if ((rv = bcm_port_interface_get(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, &intf_type)) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error retrieving interface type {%d,%d,%d}/bcm_port %u", usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port);
    return L7_FAILURE;
  }

  /* Speed of this interface should be 10G */
  if (ptin_port >= dapiCardPtr->numOfWCPortMapEntries ||  /* Invalid port */
      intf_type == BCM_PORT_IF_KR ||
      intf_type == BCM_PORT_IF_KR4)
  {
    PT_LOG_WARN(LOG_CTX_HAPI, "Port {%d,%d,%d}/bcm_port %u/port %u cannot be considered",
                usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port);
    return L7_NOT_SUPPORTED;
  }
#endif

  /* Port bitmap */
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_SET(pbmp, hapiPortPtr->bcm_port);

  /* If link is to be down, only */
  if (!link)
  {
    /* Disable loopback */
    if ((rv = bcm_port_loopback_set(0, hapiPortPtr->bcm_port, BCM_PORT_LOOPBACK_NONE)) != BCM_E_NONE) 
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error disabling PHY loopback, with link change -> port {%d,%d,%d}/bcm_port %u/port %u to %u (rv=%d)",
              usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port, enable, rv);
      return L7_FAILURE;
    }
    return L7_SUCCESS;
  }

  /* If is to force link up */
  if (enable)
  {
    /* Apply Phy loopback */
    PT_LOG_INFO(LOG_CTX_HAPI, "Going to enable PHY loopback (official) to port {%d,%d,%d}/bcm_port %u/port %u to %u",
             usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port, enable);
    if ((rv = bcm_port_loopback_set(0, hapiPortPtr->bcm_port, BCM_PORT_LOOPBACK_PHY)) != BCM_E_NONE)
    {
      bcm_port_loopback_set(0, hapiPortPtr->bcm_port, BCM_PORT_LOOPBACK_NONE);
      PT_LOG_ERR(LOG_CTX_HAPI, "Error enabling loopback for port {%d,%d,%d}/bcm_port %u/port %u to %u (rv=%d)",
              usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port, enable, rv);
      return L7_FAILURE;
    }

    /* Wait for link up */
    i = 0;
    do
    {
      osapiSleepUSec(10000);
      if ((rv = bcm_port_link_status_get(0, hapiPortPtr->bcm_port, &link_status)) != BCM_E_NONE)
      {
        bcm_port_loopback_set(0, hapiPortPtr->bcm_port, BCM_PORT_LOOPBACK_NONE);
        PT_LOG_ERR(LOG_CTX_HAPI, "Error reading link status for port {%d,%d,%d}/bcm_port %u/port %u to %u (rv=%d)",
                usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port, enable, rv);
        return L7_FAILURE;
      }
      i++;
    } while (i <= 20 && !link_status);

    if (!link_status)
    {
      bcm_port_loopback_set(0, hapiPortPtr->bcm_port, BCM_PORT_LOOPBACK_NONE);
      PT_LOG_ERR(LOG_CTX_HAPI, "Link is down for port {%d,%d,%d}/bcm_port %u/port %u to %u",
              usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port, enable);
      return L7_FAILURE;
    }

    osapiSleepUSec(20000);

//  /* Execute a linkscan update */
//  PT_LOG_INFO(LOG_CTX_HAPI, "bcm_linkscan_update to bcm_port %u", hapiPortPtr->bcm_port);
//  if (bcm_linkscan_update(0, pbmp) != BCM_E_NONE)
//  {
//    PT_LOG_ERR(LOG_CTX_HAPI, "Error applying linkscan to bcm_port %u", hapiPortPtr->bcm_port);
//  }

  #if 0
    /* Disable loopback */
    PT_LOG_INFO(LOG_CTX_HAPI, "Going to disable physical loopback to port {%d,%d,%d}/bcm_port %u/port %u to %u",
             usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port, enable);
    if ((rv = soc_phyctrl_loopback_set(0, hapiPortPtr->bcm_port, L7_DISABLE)) != SOC_E_NONE)
    {
      bcm_port_loopback_set(0, hapiPortPtr->bcm_port, BCM_PORT_LOOPBACK_NONE);
      PT_LOG_ERR(LOG_CTX_HAPI, "Error disabling physical loopback for port {%d,%d,%d}/bcm_port %u/port %u to %u (rv=%d)",
              usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port, enable, rv);
      return L7_FAILURE;
    }
  #else
   #ifdef BCM_ESW_SUPPORT
    if ((rv = ptin_esw_port_loopback_set(0, hapiPortPtr->bcm_port, BCM_PORT_LOOPBACK_NONE, L7_TRUE)) != BCM_E_NONE) 
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error disabling PHY loopback, with no link change -> port {%d,%d,%d}/bcm_port %u/port %u to %u (rv=%d)",
              usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port, enable, rv);
      return L7_FAILURE;
    }
   #endif
  #endif

    /* Wait more 20ms */
    osapiSleepUSec(20000);

    PT_LOG_NOTICE(LOG_CTX_HAPI, "Force link-up applied to port {%d,%d,%d}/bcm_port %u/port %u to %u",
               usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port, enable);
  }
  else
  {
    #if 0
    /* Undo Loopback */
    PT_LOG_INFO(LOG_CTX_HAPI, "Going to disable force link-up (with no link change) to port {%d,%d,%d}/bcm_port %u/port %u to %u",
             usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port, enable);
    if ((rv = ptin_esw_port_loopback_set(0, hapiPortPtr->bcm_port, BCM_PORT_LOOPBACK_NONE, L7_TRUE)) != BCM_E_NONE) 
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error disabling force link-up, with no link change -> port {%d,%d,%d}/bcm_port %u/port %u to %u (rv=%d)",
              usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port, enable, rv);
      return L7_FAILURE;
    }

    PT_LOG_NOTICE(LOG_CTX_HAPI, "Disable force link-up (with no link change) applied to port {%d,%d,%d}/bcm_port %u/port %u to %u",
               usp->unit, usp->slot, usp->port, hapiPortPtr->bcm_port, ptin_port, enable);
    #endif
  }

  return L7_SUCCESS;
}

/**
 * Configure main and backup recovery clocks
 * 
 * @param main_port 
 * @param backup_port 
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_clock_recovery_set(L7_int main_port, L7_int bckp_port, DAPI_t *dapi_g)
{
  bcm_port_t bcm_port=-1;

  /* Main clock */
  if (main_port >= 0)
  {
    /* Validate port */
    if (main_port >= ptin_sys_number_of_ports)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Invalid port: main_port=%d", main_port);
      return L7_FAILURE;
    }

    /* Get bcm_port reference */
    if (hapi_ptin_bcmPort_get(main_port, &bcm_port) != L7_SUCCESS) 
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Invalid port: main_port=%d", main_port);
      return L7_FAILURE;
    }

    /* Correct bcm_port id for 10G ports: some bug requires to use an offset of +2 (CSP 814123) */
  #if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
    if ((1ULL << main_port) & PTIN_SYSTEM_10G_PORTS_MASK)
    {
      bcm_port += 2;
    }
  #endif

    /* Configure main clock */
    if (bcm_switch_control_set(0, bcmSwitchSynchronousPortClockSource, bcm_port) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting main recovery clock from bcm_port=%d", bcm_port);
      return L7_FAILURE;
    }
  }

  /* Backup clock */
  if (bckp_port >= 0)
  {
    /* Validate port */
    if (bckp_port >= ptin_sys_number_of_ports)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Invalid port: backup_port=%d", bckp_port);
      return L7_FAILURE;
    }

    /* Get bcm_port reference */
    if (hapi_ptin_bcmPort_get(bckp_port, &bcm_port) != L7_SUCCESS) 
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Invalid port: backup_port=%d", bckp_port);
      return L7_FAILURE;
    }

    /* Correct bcm_port id for 10G ports: some bug requires to use an offset of +2 (CSP 814123) */
  #if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
    if ((1ULL << bckp_port) & PTIN_SYSTEM_10G_PORTS_MASK)
    {
      bcm_port += 2;
    }
  #endif

    /* Configure backup clock */
    if (bcm_switch_control_set(0, bcmSwitchSynchronousPortClockSourceBkup, bcm_port) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting backup recovery clock from bcm_port=%d", bcm_port);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/**
 * Configure maximum valid frame size
 * 
 * @param usp 
 * @param frame_size 
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_frame_oversize_set(DAPI_USP_t *usp, L7_uint32 frame_size, DAPI_t *dapi_g)
{
  /* PTin removed: Dune */
#if 0
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr, *hapiPortPtr_member;
  L7_int        i;
  bcm_error_t   rv;

  /* Validate dapiPort */
  if (usp->unit<0 || usp->slot<0 || usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  dapiPortPtr = DAPI_PORT_GET( usp, dapi_g );
  hapiPortPtr = HAPI_PORT_GET( usp, dapi_g );

  /* Validate pointers */
  if (dapiPortPtr ==L7_NULLPTR || hapiPortPtr == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Accept only physical interfaces */
  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr))
  {
    /* Apply to all member ports */
    for (i=0; i<L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (!dapiPortPtr->modeparm.lag.memberSet[i].inUse)  continue;

      hapiPortPtr_member = HAPI_PORT_GET( &dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g );
      if (hapiPortPtr_member==L7_NULLPTR)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error getting HAPI_PORT_GET for usp={%d,%d,%d}",
                dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
        return L7_FAILURE;
      }

      /* Set oversize packets limite */
      rv = bcm_port_control_set(hapiPortPtr_member->bcm_unit, hapiPortPtr_member->bcm_port, bcmPortControlStatOversize, frame_size);

      if (rv != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error setting oversize frame limite at port {%d,%d,%d} (rv=%d)",
                dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                dapiPortPtr->modeparm.lag.memberSet[i].usp.port,
                rv);
        return L7_FAILURE;
      }
    }
  }
  else if ( IS_PORT_TYPE_PHYSICAL(dapiPortPtr) )
  {
    /* Set oversize packets limite */
    rv = bcm_port_control_set(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, bcmPortControlStatOversize, frame_size);

    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting oversize frame limite at port {%d,%d,%d} (rv=%d)",
              usp->unit, usp->slot, usp->port, rv);
      return L7_FAILURE;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port type (usp={%d,%d,%d}) is not valid!", usp->unit, usp->slot, usp->port);
    return L7_NOT_SUPPORTED;
  }

  return L7_SUCCESS;
#else
  return L7_NOT_SUPPORTED;
#endif
}

/**
 * Read maximum valid frame size
 * 
 * @param usp 
 * @param frame_size (output)
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_frame_oversize_get(DAPI_USP_t *usp, L7_uint32 *frame_size, DAPI_t *dapi_g)
{
  /* PTin removed: Dune */
#if 0
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr, *hapiPortPtr_member;
  L7_int        fsize_res = L7_MAX_FRAME_SIZE, fsize;
  L7_int        i;
  bcm_error_t   rv;

  /* Validate dapiPort */
  if (usp->unit<0 || usp->slot<0 || usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  dapiPortPtr = DAPI_PORT_GET( usp, dapi_g );
  hapiPortPtr = HAPI_PORT_GET( usp, dapi_g );

  /* Validate pointers */
  if (dapiPortPtr ==L7_NULLPTR || hapiPortPtr == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Accept only physical interfaces */
  if (IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr))
  {
    fsize_res = L7_MAX_FRAME_SIZE;

    /* Apply to all member ports */
    for (i=0; i<L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (!dapiPortPtr->modeparm.lag.memberSet[i].inUse)  continue;

      hapiPortPtr_member = HAPI_PORT_GET( &dapiPortPtr->modeparm.lag.memberSet[i].usp, dapi_g );
      if (hapiPortPtr_member==L7_NULLPTR)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error getting HAPI_PORT_GET for usp={%d,%d,%d}",
                dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
        return L7_FAILURE;
      }

      /* Set oversize packets limite */
      rv = bcm_port_control_get(hapiPortPtr_member->bcm_unit, hapiPortPtr_member->bcm_port, bcmPortControlStatOversize, &fsize);

      if (rv != BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error setting oversize frame limite at port {%d,%d,%d} (rv=%d)",
                dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                dapiPortPtr->modeparm.lag.memberSet[i].usp.port,
                rv);
        return L7_FAILURE;
      }
      else if (fsize < fsize_res)
      {
        /* Update frame size (select minimum) */
        fsize_res = fsize;
      }
    }
  }
  else if ( IS_PORT_TYPE_PHYSICAL(dapiPortPtr) )
  {
    /* Set oversize packets limite */
    rv = bcm_port_control_get(hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port, bcmPortControlStatOversize, &fsize);

    if (rv != BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting oversize frame limite at port {%d,%d,%d} (rv=%d)",
              usp->unit, usp->slot, usp->port, rv);
      return L7_FAILURE;
    }
    else
    {
      fsize_res = fsize;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port type (usp={%d,%d,%d}) is not valid!", usp->unit, usp->slot, usp->port);
    return L7_NOT_SUPPORTED;
  }

  /* Return value */
  if (frame_size != L7_NULLPTR)
  {
    *frame_size = fsize_res;

    PT_LOG_TRACE(LOG_CTX_HAPI, "Returning Oversize frame limite for port {%d,%d,%d} as %u",
              usp->unit, usp->slot, usp->port, *frame_size);
  }

  return L7_SUCCESS;
#else
  return L7_NOT_SUPPORTED;
#endif
}


/**
 * Get Egress port type definition
 * 
 * @param dapiPort  : Physical interface
 * @param port_type : Port type (PROMISCUOUS/COMMUNITY/ISOLATED)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_egress_port_type_get(ptin_dapi_port_t *dapiPort, L7_int *port_type)
{
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  bcm_port_t    bcm_unit, bcm_port;
  L7_int        type;

  PT_LOG_TRACE(LOG_CTX_HAPI, "dapiPort={%d,%d,%d}",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_NOT_SUPPORTED;
  }

  /* Get bcm_unit and bcm_port */
  bcm_unit = hapiPortPtr->bcm_unit;
  bcm_port = hapiPortPtr->bcm_port;

  /* If this is a root port, is PROMISCUOUS type */
  if (BCM_PBMP_MEMBER(pbm_egress_root_ports, bcm_port))
  {
    type = PTIN_PORT_EGRESS_TYPE_PROMISCUOUS;
  }
  /* If this is a community port, is COMMUNITY type */
  else if (BCM_PBMP_MEMBER(pbm_egress_community_ports, bcm_port))
  {
    type = PTIN_PORT_EGRESS_TYPE_COMMUNITY;
  }
  /* Otherwise, is ISOLATED type */
  else
  {
    type = PTIN_PORT_EGRESS_TYPE_ISOLATED;
  }

  if (port_type != L7_NULLPTR)
  {
    *port_type = type;
  }

  return L7_SUCCESS;
}


/**
 * Egress port type definition
 * 
 * @param dapiPort  : Physical or logical interface
 * @param port_type : Port type (PROMISCUOUS/COMMUNITY/ISOLATED)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_egress_port_type_set(ptin_dapi_port_t *dapiPort, L7_int port_type)
{
  L7_int i;
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr, *hapiPortPtr_member;
  bcm_port_t    bcm_unit, bcm_port;

  PT_LOG_INFO(LOG_CTX_HAPI, "dapiPort={%d,%d,%d}",
              dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* Physical port */
  if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
  {
    if (port_type == PTIN_PORT_EGRESS_TYPE_ISOLATED)
    {
      BCM_PBMP_PORT_REMOVE(pbm_egress_root_ports, hapiPortPtr->bcm_port);
      BCM_PBMP_PORT_REMOVE(pbm_egress_community_ports, hapiPortPtr->bcm_port);
    }
    else if (port_type == PTIN_PORT_EGRESS_TYPE_COMMUNITY)
    {
      /* Port used only as community port */
      BCM_PBMP_PORT_REMOVE(pbm_egress_root_ports, hapiPortPtr->bcm_port);
      BCM_PBMP_PORT_ADD(pbm_egress_community_ports, hapiPortPtr->bcm_port);
    }
    else
    {
      /* Port used as root and community port */
      BCM_PBMP_PORT_ADD(pbm_egress_root_ports, hapiPortPtr->bcm_port);
      BCM_PBMP_PORT_ADD(pbm_egress_community_ports, hapiPortPtr->bcm_port);
    }
  }
  /* LAG port */
  else
  {
    /* Apply to all member ports */
    for (i=0; i<L7_MAX_MEMBERS_PER_LAG; i++)
    {
      if (!dapiPortPtr->modeparm.lag.memberSet[i].inUse)  continue;

      hapiPortPtr_member = HAPI_PORT_GET( &dapiPortPtr->modeparm.lag.memberSet[i].usp, dapiPort->dapi_g );
      if (hapiPortPtr_member==L7_NULLPTR)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error getting HAPI_PORT_GET for usp={%d,%d,%d}",
                dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
        return L7_FAILURE;
      }

      if (port_type == PTIN_PORT_EGRESS_TYPE_ISOLATED)
      {
        BCM_PBMP_PORT_REMOVE(pbm_egress_root_ports, hapiPortPtr_member->bcm_port);
        BCM_PBMP_PORT_REMOVE(pbm_egress_community_ports, hapiPortPtr_member->bcm_port);
      }
      else if (port_type == PTIN_PORT_EGRESS_TYPE_COMMUNITY)
      {
        /* Port used only as community port */
        BCM_PBMP_PORT_REMOVE(pbm_egress_root_ports, hapiPortPtr_member->bcm_port);
        BCM_PBMP_PORT_ADD(pbm_egress_community_ports, hapiPortPtr_member->bcm_port);
      }
      else
      {
        /* Port used as root and community port */
        BCM_PBMP_PORT_ADD(pbm_egress_root_ports, hapiPortPtr_member->bcm_port);
        BCM_PBMP_PORT_ADD(pbm_egress_community_ports, hapiPortPtr_member->bcm_port);
      }
    }
  }

  /* Get bcm_unit */
  if (hapi_ptin_bcmUnit_get(&bcm_unit) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error getting bcm_unit");
    return L7_FAILURE;
  }

  /* Run iteratively all ports, and apply new port map */
  for (i=0; i<ptin_sys_number_of_ports; i++)
  {
    /* Get bcm_port */
    if (hapi_ptin_bcmPort_get(i, &bcm_port) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error getting bcm_port for port %u",i);
      continue;
    }

    /* If this is a root port, apply all ports as egress port map */
    if (BCM_PBMP_MEMBER(pbm_egress_root_ports, bcm_port))
    {
      if (bcm_port_egress_set(bcm_unit, bcm_port, 0, pbm_egress_all_ports)!=BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"Error setting egress bitmap for port %u",i);
        return L7_FAILURE;
      }
    }
    /* If not root port, but is a community port, apply community ports list as egress port map */
    else if (BCM_PBMP_MEMBER(pbm_egress_community_ports, bcm_port))
    {
      if (bcm_port_egress_set(bcm_unit, bcm_port, 0, pbm_egress_community_ports)!=BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"Error setting egress bitmap for port %u",i);
        return L7_FAILURE;
      }
    }
    /* Otherwise, apply only root ports as egress port map */
    else
    {
      if (bcm_port_egress_set(bcm_unit, bcm_port, 0, pbm_egress_root_ports)!=BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"Error setting egress bitmap for port %u",i);
        return L7_FAILURE;
      }
    }
  }

  PT_LOG_NOTICE(LOG_CTX_HAPI, "New port type %u correctly set to port {%d,%d,%d}",
            port_type, dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);

  return L7_SUCCESS;
}


/**
 * Attribute L2 learning priority to specified port
 * 
 * @param dapiPort : Physical or logical interface
 * @param macLearn_enable : Enable MAC Learning 
 * @param stationMove_enable : Enable L2 Station Move 
 * @param stationMove_prio   : L2 Station Move priority
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_l2learn_port_set(ptin_dapi_port_t *dapiPort, L7_int macLearn_enable, L7_int stationMove_enable, L7_int stationMove_prio, L7_int stationMove_samePrio)
{
  L7_int    i, lclass;
  L7_uint32 flags;
  L7_BOOL   learn_class_move = L7_TRUE;
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
  BROAD_PORT_t *hapiPortPtr_member;
  bcm_error_t rv = BCM_E_NONE;

  PT_LOG_TRACE(LOG_CTX_HAPI, "dapiPort={%d,%d,%d}",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);
  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* MAC Learning enable */
  if (macLearn_enable>=0)
  {
    if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
    {
      /* LearnClass Enable/Disable */
      if ((rv=bcmx_port_control_set(hapiPortPtr->bcmx_lport, bcmPortControlLearnClassEnable,macLearn_enable & 1))!=BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error setting bcmPortControlLearnClassEnable in port {%d,%d,%d} to %u (rv=%d)",
                dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, macLearn_enable, rv);
        return L7_FAILURE;
      }
    }
    else
    {
      /* Apply to all member ports */
      for (i=0; i<L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (!dapiPortPtr->modeparm.lag.memberSet[i].inUse)  continue;

        hapiPortPtr_member = HAPI_PORT_GET( &dapiPortPtr->modeparm.lag.memberSet[i].usp, dapiPort->dapi_g );
        if (hapiPortPtr_member==L7_NULLPTR)
        {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error getting HAPI_PORT_GET for usp={%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }
        /* Get enable status for member port */
        if ((rv=bcmx_port_control_set(hapiPortPtr_member->bcmx_lport, bcmPortControlLearnClassEnable, macLearn_enable & 1))!=BCM_E_NONE)
        {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error getting bcmPortControlLearnClassEnable in port {%d,%d,%d} (rv=%d)",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port,
                  rv);
          return L7_FAILURE;
        }
      }
    }
  }

  /* L2 Station move */
  if (stationMove_enable>=0)
  {
    flags = (stationMove_enable) ? (BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD) : 0x00;

    if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
    {
      /* L2 Station move */
      if ((rv=bcmx_port_control_set(hapiPortPtr->bcmx_lport, bcmPortControlL2Move, flags))!=BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error setting bcmPortControlL2Move in port {%d,%d,%d} to 0x%02x (rv=%d)",
                dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, flags, rv);
        return L7_FAILURE;
      }
    }
    else
    {
      /* Apply to all member ports */
      for (i=0; i<L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (!dapiPortPtr->modeparm.lag.memberSet[i].inUse)  continue;

        hapiPortPtr_member = HAPI_PORT_GET( &dapiPortPtr->modeparm.lag.memberSet[i].usp, dapiPort->dapi_g );
        if (hapiPortPtr_member==L7_NULLPTR)
        {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error getting HAPI_PORT_GET for usp={%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }
        /* Get enable status for member port */
        if ((rv=bcmx_port_control_set(hapiPortPtr_member->bcmx_lport, bcmPortControlL2Move, flags))!=BCM_E_NONE)
        {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error setting bcmPortControlL2Move in port {%d,%d,%d} (rv=%d)",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port,
                  rv);
          return L7_FAILURE;
        }
      }
    }
  }

  /* Station Move with same priority ports */
  if (stationMove_samePrio>=0)
  {
    learn_class_move = stationMove_samePrio & 1;
  }

  /* Station move priority */
  if (stationMove_prio>=0)
  {
    /* Validate priority */
    if (stationMove_prio>3)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Invalid priority: should be between 0 and 3");
      return L7_FAILURE;
    }

    /* Priority flags */
    flags = (learn_class_move) ? BCM_L2_LEARN_CLASS_MOVE : 0;

    lclass = stationMove_prio;

    /* Attribute priority to a class */
    if ((rv=bcmx_l2_learn_class_set(lclass, stationMove_prio, flags))!=BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting prio %d to class %d (rv=%d)", stationMove_prio, lclass, rv);
      return L7_FAILURE;
    }

    /* Associate class to the specified interface */
    if ((rv=bcmx_l2_learn_port_class_set(hapiPortPtr->bcmx_lport,lclass))!=BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error setting class %d to port {%d,%d,%d} (rv=%d)",lclass,
              dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, rv);
      return L7_FAILURE;
    }
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "L2Learn parameters attributed correctly to port {%d,%d,%d} (rv=%d)",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port, rv);

  return L7_SUCCESS;
}



/**
 * Get L2 learning attributes
 * 
 * @param dapiPort : Physical or logical interface
 * @param macLearn_enable    : Enable MAC Learning (output)
 * @param stationMove_enable : Enable L2 Station Move (output)
 * @param stationMove_prio   : L2 Station Move priority (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_l2learn_port_get(ptin_dapi_port_t *dapiPort, L7_int *macLearn_enable, L7_int *stationMove_enable, L7_int *stationMove_prio, L7_int *stationMove_samePrio)
{
#if 0
  L7_int  lclass;
  L7_int  i, enable, enable_global, prio;
  L7_uint32     flags;
#endif
  DAPI_PORT_t  *dapiPortPtr;
  BROAD_PORT_t *hapiPortPtr;
#if 0
  BROAD_PORT_t *hapiPortPtr_member;
#endif

  PT_LOG_TRACE(LOG_CTX_HAPI, "dapiPort={%d,%d,%d}",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);

  /* Validate dapiPort */
  if (dapiPort->usp->unit<0 || dapiPort->usp->slot<0 || dapiPort->usp->port<0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid interface");
    return L7_FAILURE;
  }

  /* Get port pointers */
  DAPIPORT_GET_PTR(dapiPort, dapiPortPtr, hapiPortPtr);

  /* Accept only physical and lag interfaces */
  if ( !IS_PORT_TYPE_PHYSICAL(dapiPortPtr) && !IS_PORT_TYPE_LOGICAL_LAG(dapiPortPtr) )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Port {%d,%d,%d} is not physical neither logical lag",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
    return L7_FAILURE;
  }

  /* PTin removed: Dune */
#if 0
  /* MAC Learning enable */
  if (macLearn_enable!=L7_NULLPTR)
  {
    if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
    {
      /* LearnClass Enable/Disable */
      if (bcmx_port_control_get(hapiPortPtr->bcmx_lport, bcmPortControlLearnClassEnable,&enable_global)!=BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error getting bcmPortControlLearnClassEnable in port {%d,%d,%d}",
                dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
        return L7_FAILURE;
      }
    }
    /* If port is a lag, get all member port enables. If any has mac learning disabled, return disabled status */
    else
    {
      enable_global = 1;
      /* Run all member ports */
      for (i=0; i<L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (!dapiPortPtr->modeparm.lag.memberSet[i].inUse)  continue;

        hapiPortPtr_member = HAPI_PORT_GET( &dapiPortPtr->modeparm.lag.memberSet[i].usp, dapiPort->dapi_g );
        if (hapiPortPtr_member==L7_NULLPTR)
        {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error getting HAPI_PORT_GET for usp={%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }
        /* Get enable status for member port */
        if (bcmx_port_control_get(hapiPortPtr_member->bcmx_lport, bcmPortControlLearnClassEnable, &enable)!=BCM_E_NONE)
        {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error getting bcmPortControlLearnClassEnable in port {%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }
        /* If not enabled, set global enable to FALSE, and break cycle */
        if (!enable)
        {
          enable_global = 0;
          break;
        }
      }
    }
    /* Save global enable status */
    *macLearn_enable = enable_global;
  }

  if (stationMove_enable!=L7_NULLPTR)
  {
    if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr))
    {
      /* L2 Station move */
      if (bcmx_port_control_get(hapiPortPtr->bcmx_lport, bcmPortControlL2Move, &flags)!=BCM_E_NONE)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error getting bcmPortControlL2Move flags in port {%d,%d,%d}",
                dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
        return L7_FAILURE;
      }
      enable_global = (flags & BCM_PORT_LEARN_ARL);
    }
    /* If port is a lag, get all member port enables. If any has mac learning disabled, return disabled status */
    else
    {
      enable_global = 1;
      /* Run all member ports */
      for (i=0; i<L7_MAX_MEMBERS_PER_LAG; i++)
      {
        if (!dapiPortPtr->modeparm.lag.memberSet[i].inUse)  continue;

        hapiPortPtr_member = HAPI_PORT_GET( &dapiPortPtr->modeparm.lag.memberSet[i].usp, dapiPort->dapi_g );
        if (hapiPortPtr_member==L7_NULLPTR)
        {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error getting HAPI_PORT_GET for usp={%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }
        /* Get enable status for member port */
        if (bcmx_port_control_get(hapiPortPtr_member->bcmx_lport, bcmPortControlL2Move, &flags)!=BCM_E_NONE)
        {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error getting bcmPortControlL2Move in port {%d,%d,%d}",
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.unit,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.slot,
                  dapiPortPtr->modeparm.lag.memberSet[i].usp.port);
          return L7_FAILURE;
        }
        /* If not enabled, set global enable to FALSE, and break cycle */
        if (!(flags & BCM_PORT_LEARN_ARL))
        {
          enable_global = 0;
          break;
        }
      }
    }
    /* Save global enable status */
    *stationMove_enable = enable_global;
  }

  if (stationMove_prio!=L7_NULLPTR)
  {
    /* Get class id from the specified interface */
    if (bcmx_l2_learn_port_class_get(hapiPortPtr->bcmx_lport,&lclass)!=BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error getting classId from port {%d,%d,%d}",
              dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
      return L7_FAILURE;
    }
    /* Get priority attribute */
    if (bcmx_l2_learn_class_get(lclass, &prio, &flags)!=BCM_E_NONE)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error getting prio from classId %d",lclass);
      return L7_FAILURE;
    }
    *stationMove_prio = prio;

    if (stationMove_samePrio!=L7_NULLPTR)
    {
      *stationMove_samePrio = (flags & BCM_L2_LEARN_CLASS_MOVE);
    }
  }

  /* Station move for same priority ports? */

  PT_LOG_TRACE(LOG_CTX_HAPI, "L2Learn parameters read correctly from port {%d,%d,%d}",
            dapiPort->usp->unit, dapiPort->usp->slot, dapiPort->usp->port);
#endif

  return L7_SUCCESS;
}

#define REG_GET(unit, port, reg, reg_val)					   \
    if (SOC_REG_IS_VALID(unit, reg) && SOC_REG_IS_COUNTER(unit, reg)) {	              \
        int rv=0;                                                                     \
        if (SOC_CONTROL(unit)->counter_interval == 0 /*counter thread is off*/) {     \
            if ((rv=soc_reg_get(unit, reg, port, 0, &reg_val))<0)                     \
              PT_LOG_ERR(LOG_CTX_HAPI,"Port %u: Error with soc_reg_get(%u, %u, %u, 0, &reg_val): rv=%d", port, unit, reg, port, rv);      \
        } else {                                                                      \
            if ((rv=soc_counter_get(unit, port, reg, 0, &reg_val))<0)                 \
              PT_LOG_ERR(LOG_CTX_HAPI,"Port %u: Error with soc_counter_get(%u, %u, %u, 0, &reg_val): rv=%d", port, unit, port, reg, rv);  \
        }                                                                             \
    }                                                                                 \
    else                                                                              \
    {                                                                                 \
      PT_LOG_ERR(LOG_CTX_HAPI,"Port %u: Register %u is invalid", port, reg);          \
    }

#define REG_SET(unit, port, reg, reg_val)					   \
    if (SOC_REG_IS_VALID(unit, reg) && SOC_REG_IS_COUNTER(unit, reg)) {	              \
        int rv=0;                                                                     \
        if (SOC_CONTROL(unit)->counter_interval == 0 /*counter thread is off*/) {     \
            if ((rv=soc_reg_set(unit, reg, port, 0, reg_val))<0)                     \
              PT_LOG_ERR(LOG_CTX_HAPI,"Port %u: Error with soc_reg_set(%u, %u, %u, 0, %u): rv=%d", port, unit, reg, port, reg_val, rv);     \
        } else {                                                                      \
            if ((rv=soc_counter_set(unit, port, reg, 0, reg_val))<0)                 \
              PT_LOG_ERR(LOG_CTX_HAPI,"Port %u: Error with soc_counter_set(%u, %u, %u, 0, %u): rv=%d", port, unit, port, reg, reg_val, rv); \
        }                                                                             \
    }                                                                                 \
    else                                                                              \
    {                                                                                 \
      PT_LOG_ERR(LOG_CTX_HAPI,"Port %u: Register %u is invalid", port, reg);          \
    }

/**
 * Read counters (physical interfaces)
 *  
 * Note: currently masks are ignored, meaning that all values are read
 * 
 * @param portStats Structure with port# and masks (port# must be set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_counters_read(ptin_HWEthRFC2819_PortStatistics_t *portStats)
{
  ptin_HWEthRFC2819_StatisticsBlock_t *rx, *tx;
  //L7_uint64 tmp1=0, tmp2=0, tmp3=0;
  L7_uint64 mtuePkts=0;
  L7_uint64 pkts1519to2047, pkts2048to4095, pkts4096to9216, pkts9217to16383;
  L7_uint port, unit;

  if (portStats->Port >= ptin_sys_number_of_ports)
    return L7_FAILURE;

  /* Clear stats */
  memset(&portStats->Rx, 0x00, sizeof(portStats->Rx));
  memset(&portStats->Tx, 0x00, sizeof(portStats->Tx));

  port = usp_map[portStats->Port].port;
  unit = usp_map[portStats->Port].unit;
  rx = &portStats->Rx;
  tx = &portStats->Tx;

  /* Rx counters */
  REG_GET(unit, port, RMTUEr, mtuePkts);                              /* Packets > MTU bytes (good and bad) */
  //REG_GET(unit, port, RDROPr           , tmp1);
  //REG_GET(unit, port, DROP_PKT_CNT_INGr, tmp2);
  //rx->etherStatsDropEvents = tmp1 + tmp2;                                         /* Drop Events */
  REG_GET(unit, port, RBYTr , rx->etherStatsOctets);
  REG_GET(unit, port, RPKTr , rx->etherStatsPkts);                    /* Packets (>=64 bytes) */
  REG_GET(unit, port, RBCAr , rx->etherStatsBroadcastPkts);           /* Broadcasts */
  REG_GET(unit, port, RMCAr , rx->etherStatsMulticastPkts);           /* Muilticast */
  REG_GET(unit, port, RFCSr , rx->etherStatsCRCAlignErrors);          /* FCS Errors (64-1518 bytes)*/
  rx->etherStatsCollisions = 0;                                                   /* Collisions */
  REG_GET(unit, port, RUNDr , rx->etherStatsUndersizePkts);           /* Undersize */
  //REG_GET(unit, port, ROVRr,  tmp1);
  //REG_GET(unit, port, RMGVr,  tmp2);
  //rx->etherStatsOversizePkts = tmp1 + tmp2;                                       /* Oversize: 1519-MTU bytes */
  REG_GET(unit, port, ROVRr , rx->etherStatsOversizePkts);            /* Oversize: 1523-MTU bytes */
  rx->etherStatsOversizePkts += mtuePkts;                                         /* Oversize: >MTU bytes */
  REG_GET(unit, port, RFRGr , rx->etherStatsFragments);               /* Fragments */
  REG_GET(unit, port, RJBRr , rx->etherStatsJabbers);                 /* Jabbers */
  REG_GET(unit, port, R64r  , rx->etherStatsPkts64Octets);            /* 64B packets */
  REG_GET(unit, port, R127r , rx->etherStatsPkts65to127Octets);       /* 65-127B packets */
  REG_GET(unit, port, R255r , rx->etherStatsPkts128to255Octets);      /* 128-255B packets */
  REG_GET(unit, port, R511r , rx->etherStatsPkts256to511Octets);      /* 256-511B packets */
  REG_GET(unit, port, R1023r, rx->etherStatsPkts512to1023Octets);     /* 512-1023B packets */
  REG_GET(unit, port, R1518r, rx->etherStatsPkts1024to1518Octets);    /* 1024-1518B packets */

  REG_GET(unit, port, R2047r, pkts1519to2047);                        /* 1519-2047 Bytes packets */
  REG_GET(unit, port, R4095r, pkts2048to4095);                        /* 2048-4095 Bytes packets */
  REG_GET(unit, port, R9216r, pkts4096to9216);                        /* 4096-9216 Bytes packets */
  pkts9217to16383 = 0;
  rx->etherStatsPkts1519toMaxOctets = pkts1519to2047 + pkts2048to4095 + pkts4096to9216 + pkts9217to16383;

  if (soc_counter_get_rate(unit, port, RBYTr , 0, &rx->Throughput)<0)                   /* Throughput */
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error getting RX rate!");
  }

  // Tx counters
  //REG_GET(unit, port, DROP_PKT_CNTr, tmp1);
  //REG_GET(unit, port, HOLDROP_PKT_CNTr, tmp2);
  //REG_GET(unit, port, EGRDROPPKTCOUNTr, tmp3);
  //tx->etherStatsDropEvents = tmp1 + /*tmp2 +*/ tmp3;                              /* Drop Events */
  REG_GET(unit, port, TBYTr , tx->etherStatsOctets);                  /* Octets */                   
  REG_GET(unit, port, TPKTr , tx->etherStatsPkts);                    /* Packets */                  
  REG_GET(unit, port, TBCAr , tx->etherStatsBroadcastPkts);           /* Broadcasts */               
  REG_GET(unit, port, TMCAr , tx->etherStatsMulticastPkts);           /* Muilticast */               
  REG_GET(unit, port, TFCSr , tx->etherStatsCRCAlignErrors);          /* FCS Errors (64-1518 bytes)*/
  tx->etherStatsCollisions = 0;                                                   /* Collisions */               
  tx->etherStatsUndersizePkts = 0;                                                /* Undersize */                
  //REG_GET(unit, port, TOVRr, tmp1);
  //REG_GET(unit, port, TMGVr, tmp2);
  //tx->etherStatsOversizePkts = tmp1 + tmp2;                                       /* Oversize: 1519-MTU bytes */
  REG_GET(unit, port, TOVRr,  tx->etherStatsOversizePkts);            /* Oversize: 1523-MTU bytes */
  REG_GET(unit, port, TFRGr , tx->etherStatsFragments);               /* Fragments */               
  REG_GET(unit, port, TJBRr , tx->etherStatsJabbers);                 /* Jabbers */                 
  REG_GET(unit, port, T64r  , tx->etherStatsPkts64Octets);            /* 64B packets */             
  REG_GET(unit, port, T127r , tx->etherStatsPkts65to127Octets);       /* 65-127B packets */         
  REG_GET(unit, port, T255r , tx->etherStatsPkts128to255Octets);      /* 128-255B packets */        
  REG_GET(unit, port, T511r , tx->etherStatsPkts256to511Octets);      /* 256-511B packets */        
  REG_GET(unit, port, T1023r, tx->etherStatsPkts512to1023Octets);     /* 512-1023B packets */       
  REG_GET(unit, port, T1518r, tx->etherStatsPkts1024to1518Octets);    /* 1024-1518B packets */

  REG_GET(unit, port, T2047r, pkts1519to2047);                        /* 1519-2047 Bytes packets */
  REG_GET(unit, port, T4095r, pkts2048to4095);                        /* 2048-4095 Bytes packets */
  REG_GET(unit, port, T9216r, pkts4096to9216);                        /* 4096-9216 Bytes packets */
  pkts9217to16383 = 0;
  tx->etherStatsPkts1519toMaxOctets = pkts1519to2047 + pkts2048to4095 + pkts4096to9216 + pkts9217to16383;

  if (soc_counter_get_rate(unit, port, TBYTr , 0, &tx->Throughput)<0)                   /* Throughput */
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error getting TX rate!");
  }

/* PTin debug: drop events are not correctly read. Override with zero until this problem is solved! */
//PT_LOG_WARN(LOG_CTX_HAPI, "RX and TX drop counters are always zero! Known bug to be solved!");
//rx->etherStatsDropEvents = 0;
//tx->etherStatsDropEvents = 0;

  /* Only missing undersize and fragment packets in packet counting */
  rx->etherStatsPkts += rx->etherStatsUndersizePkts + rx->etherStatsFragments;

  /* Adjust Throughput to Mbps */
  rx->Throughput *= 8;
  tx->Throughput *= 8;

  portStats->Mask = 0x03;
  portStats->RxMask = 0x7FBFF;  /* without Collisions, but include Pkts1024to1518Octets (0x20000) and Throughput (0x40000) */
  portStats->TxMask = 0x7FC9F;  /* without UndersizePkts, CRCAlignErrors, Fragments, Jabbers,
                                 * but include Pkts1024to1518Octets (0x20000) and Throughput (0x40000) */

  return L7_SUCCESS;
}


/**
 * Clears counters from a physical interface
 * 
 * @param port Port # (physical interface)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_counters_clear(L7_uint phyPort)
{
  L7_uint port, unit;

  if (phyPort >= ptin_sys_number_of_ports)
    return L7_FAILURE;

  port = usp_map[phyPort].port;
  unit = usp_map[phyPort].unit;

  /* Rx counters */
  REG_SET(unit, port, RMTUEr, 0);
  //REG_SET(unit, port, RDROPr, 0);
  //REG_SET(unit, port, DROP_PKT_CNTr , 0);
  REG_SET(unit, port, RBYTr , 0);
  REG_SET(unit, port, RPKTr , 0);
  REG_SET(unit, port, RBCAr , 0);
  REG_SET(unit, port, RMCAr , 0);
  REG_SET(unit, port, RFCSr , 0);
  REG_SET(unit, port, RUNDr , 0);
  REG_SET(unit, port, ROVRr , 0);
  REG_SET(unit, port, RFRGr , 0);
  REG_SET(unit, port, RJBRr , 0);
  REG_SET(unit, port, R64r  , 0);
  REG_SET(unit, port, R127r , 0);
  REG_SET(unit, port, R255r , 0);
  REG_SET(unit, port, R511r , 0);
  REG_SET(unit, port, R1023r, 0);
  REG_SET(unit, port, R1518r, 0);

  /* Tx counters */
  //REG_SET(unit, port, DROP_PKT_CNTr, 0);
  //REG_SET(unit, port, EGRDROPPKTCOUNTr, 0);
  //REG_SET(unit, port, HOLDr,  0, 0);
  REG_SET(unit, port, TBYTr , 0);
  REG_SET(unit, port, TPKTr , 0);
  REG_SET(unit, port, TBCAr , 0);
  REG_SET(unit, port, TMCAr , 0);
  REG_SET(unit, port, TFCSr , 0);
  //REG_SETunit, port, TXCLr , 0);
  REG_SET(unit, port, TOVRr , 0);
  REG_SET(unit, port, TFRGr , 0);
  //REG_SET(unit, port, TJBRr, 0);
  REG_SET(unit, port, T64r  , 0);
  REG_SET(unit, port, T127r , 0);
  REG_SET(unit, port, T255r , 0);
  REG_SET(unit, port, T511r , 0);
  REG_SET(unit, port, T1023r, 0);
  REG_SET(unit, port, T1518r, 0);
  
  PT_LOG_TRACE(LOG_CTX_HAPI, "Port# %u counters cleared", phyPort);
  return L7_SUCCESS;
}


/**
 * Get counters activity (physical interfaces)
 *  
 * Note: at the moment, masks are ignored, therefore all values 
 * are read for all ports)
 * 
 * @param portsActivity Structure with port# and masks (port# must be set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_counters_activity_get(ptin_HWEth_PortsActivity_t *portsActivity)
{
  L7_uint port;
  L7_uint port_remap, unit;
  L7_uint64 old_mask;
  L7_uint64 rate;

  old_mask = portsActivity->ports_mask;
  portsActivity->ports_mask = 0;

  for (port=0; port<ptin_sys_number_of_ports && port<PTIN_SYSTEM_N_PORTS; port++)
  {
    if (! (old_mask & ((L7_uint64) 1<<port)))
      continue;

    port_remap = usp_map[port].port;
    unit       = usp_map[port].unit;
    portsActivity->ports_mask |= (L7_uint64) 1 << port;
    portsActivity->activity_bmap[port] = 0;

    /* The process used to read counters activity is to read its rate. This
     * may eventually fail to provide an accurate result in cases where rate
       it too low and return 0 */

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_RX_DROPPACKETS) {
      soc_counter_get_rate(unit, port_remap, DROP_PKT_CNTr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_RX_DROPPACKETS;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_RX_ACTIVITY) {
      soc_counter_get_rate(unit, port_remap, RBYTr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_RX_ACTIVITY;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_TX_ACTIVITY) {
      soc_counter_get_rate(unit, port_remap, TBYTr, 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_TX_ACTIVITY;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_TX_COLLISIONS) {
      soc_counter_get_rate(unit, port_remap, TXCLr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_TX_COLLISIONS;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_RX_CRC_ERRORS) {
      soc_counter_get_rate(unit, port_remap, RFCSr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_RX_CRC_ERRORS;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_RX_JABBERS) {
      soc_counter_get_rate(unit, port_remap, RJBRr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_RX_JABBERS;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_RX_FRAGMENTS) {
      soc_counter_get_rate(unit, port_remap, RFRGr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_RX_FRAGMENTS;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_RX_OVERSIZEPACKETS) {
      soc_counter_get_rate(unit, port_remap, ROVRr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_RX_OVERSIZEPACKETS;
    }

    if (portsActivity->activity_mask & PTIN_PORTACTIVITY_MASK_RX_UNDERSIZEPACKETS) {
      soc_counter_get_rate(unit, port_remap, RUNDr , 0, &rate);
      if (rate > 0)
        portsActivity->activity_bmap[port] |= PTIN_PORTACTIVITY_MASK_RX_UNDERSIZEPACKETS;
    }
  }

  return L7_SUCCESS;
}


L7_RC_t hapi_ptin_counters_read_debug(L7_uint phyPort)
{
  ptin_HWEthRFC2819_PortStatistics_t portStats;

  portStats.Port = phyPort;

  if (hapi_ptin_counters_read(&portStats) != L7_SUCCESS) {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error reading counter!");
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_HAPI, "Rx.etherStatsPkts = %llu", portStats.Rx.etherStatsPkts);
  PT_LOG_DEBUG(LOG_CTX_HAPI, "Rx.Throughput     = %llu", portStats.Rx.Throughput);
  PT_LOG_DEBUG(LOG_CTX_HAPI, "Tx.etherStatsPkts = %llu", portStats.Tx.etherStatsPkts);
  PT_LOG_DEBUG(LOG_CTX_HAPI, "Tx.Throughput     = %llu", portStats.Tx.Throughput);

  return L7_SUCCESS;
}

L7_RC_t hapi_ptin_counters_activity_get_debug(L7_uint phyPort)
{
  ptin_HWEth_PortsActivity_t portsActivity;

  portsActivity.ports_mask = 1 << phyPort;
  portsActivity.activity_mask = 0xFFFF;

  if (hapi_ptin_counters_activity_get(&portsActivity) != L7_SUCCESS) {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error on hapi_ptin_counters_activity_get()");
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_HAPI, "Port# %2u", phyPort);
  PT_LOG_DEBUG(LOG_CTX_HAPI, " .port_mask         = 0x%08X", portsActivity.ports_mask);
  PT_LOG_DEBUG(LOG_CTX_HAPI, " .activity_mask     = 0x%08X", portsActivity.activity_mask);
  PT_LOG_DEBUG(LOG_CTX_HAPI, " .activity_bmap[%02u] = 0x%08X", phyPort, portsActivity.activity_bmap[phyPort]);

  return L7_SUCCESS;
}

/****** STORM CONTROL ***********************************************/
static BROAD_POLICY_t policyId_storm_cpu = BROAD_POLICY_INVALID;

/**
 * Configure storm control for CPU packet processing
 * 
 * @author mruas (9/30/2014)
 * 
 * @param enable 
 * @param cir 
 * @param cbs 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapi_ptin_stormControl_cpu_set(L7_BOOL enable, L7_uint32 cir, L7_uint32 cbs)
{
  bcmx_lport_t  lport;
  bcm_port_t    bcm_port;
  bcm_port_t    bcm_port_mask = (bcm_port_t) -1;
  BROAD_POLICY_t      policyId;
  BROAD_POLICY_RULE_t ruleId;
  BROAD_METER_ENTRY_t meterInfo;
  L7_RC_t rc = L7_SUCCESS;

  /* Delete configured policy */
  if (policyId_storm_cpu != BROAD_POLICY_INVALID)
  {
    hapiBroadPolicyDelete(policyId_storm_cpu);
    policyId_storm_cpu = BROAD_POLICY_INVALID;
    PT_LOG_TRACE(LOG_CTX_HAPI,"CPU stormcontrol destroyed");
  }

  if (!enable || cir == 0 || cir == (L7_uint32)-1)
  {
    PT_LOG_WARN(LOG_CTX_HAPI,"Nothing done: enable=%u cir=%u cbs=%u", enable, cir, cbs);
    return L7_SUCCESS;
  }

  meterInfo.cir       = cir;
  meterInfo.cbs       = cbs;
  meterInfo.pir       = cir;
  meterInfo.pbs       = cbs;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;

  /* CPU port */
  if (bcmx_lport_local_cpu_get(0, &lport) != BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error with bcmx_lport_local_cpu_get");
    return L7_FAILURE;
  }
  bcm_port = bcmx_lport_bcm_port(lport);
  if (bcm_port < 0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI,"Error with bcmx_lport_bcm_port");
    return L7_FAILURE;
  }

  /* Create policy */
  rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PTIN);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Cannot create trap policy\r\n");
    return L7_FAILURE;
  }

  /* Egress stage */
  if (hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_EGRESS) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error creating a egress policy\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Create rule */
  rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error adding rule\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OUTPORT, (L7_uchar8 *)&bcm_port, (L7_uchar8 *)&bcm_port_mask);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error adding port qualifier (bcm_port=%d)\r\n",bcm_port);
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Drop red packets */
  rc = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error adding hard_drop action\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Define meter action, to rate limit packets */
  rc = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error adding rate limit\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Add counter */
  if (hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP,"Error with hapiBroadPolicyRuleCounterAdd");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Commit rule */
  if ((rc=hapiBroadPolicyCommit(&policyId)) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error commiting trap policy\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_STARTUP, "Trap policy commited successfully (policyId=%u)\r\n",policyId);

  /* Store policyId */
  policyId_storm_cpu = policyId;

  return L7_SUCCESS;
}

/**
 * Configures storm control
 * 
 * @param dapiPort : port 
 * @param enable   : Enable or diable
 * @param stormControl : storm control data 
 * @param egress_type : Egress type port 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t hapi_ptin_stormControl_set(ptin_dapi_port_t *dapiPort, L7_BOOL enable, ptin_stormControl_t *control, PORT_EGRESS_TYPE egress_type)
{
  L7_RC_t                 result_global = L7_SUCCESS, result = L7_SUCCESS;

  /* CPU storm control */
  if (control->flags & PTIN_STORMCONTROL_MASK_CPU)
  {
    result = hapi_ptin_stormControl_cpu_set(enable, control->cpu_rate, 128);

    if (result != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error committing CPU stormcontrol policy");
      result_global = result;
    }
  }

  /* Save configuration, if success */
  if (result_global == L7_SUCCESS)
  {
    stormControl_backup = *control;
  }

  PT_LOG_TRACE(LOG_CTX_HAPI, "Finished storm control processing: rc=%d", result_global);

  return result_global;
}

/**
 * Dump list of bw policers
 */
void ptin_stormcontrol_dump(void)
{
  L7_int        rule;
  BROAD_GROUP_t group_id;
  BROAD_ENTRY_t entry_id;
  int policer_id, counter_id;
  BROAD_POLICY_STATS_t stat;

  printf("Listing ingress rate limiters...\r\n");

  printf("\r\nListing counters of CPU processed packets...\r\n");

  /* LACP packets */
  if (lacp_policyId != 0 && lacp_policyId != BROAD_POLICY_INVALID)
  {
    rule = 0;
    while (l7_bcm_policy_hwInfo_get(0, lacp_policyId, rule, &group_id, &entry_id, &policer_id, &counter_id) == L7_SUCCESS)
    {
      /* Also print hw group id and entry id*/
      printf("LACPDU packets: policy=%-4u rule=%u -> group=%-2d, entry=%-4d (PolicerId=%-4d CounterId %-4d)",
             lacp_policyId, 0, group_id, entry_id, policer_id, counter_id);

      /* Check counter */
      if (counter_id > 0)
      {
        printf(": Packets=");
        /* Get stat data */
        if (hapiBroadPolicyStatsGet(lacp_policyId, 0, &stat) != L7_SUCCESS)
          printf("Error");
        else
          printf("%llu", stat.statMode.counter.count);
      }
      printf("\r\n");

      rule++;
    }
    if (rule == 0)
    {
      printf("LACPDU packets: Not configured\r\n");
    }
  }

  /* CPU storm control */
  if (policyId_storm_cpu != 0 && policyId_storm_cpu != BROAD_POLICY_INVALID)
  {
    printf("L2 CPU packets: ");
    if (l7_bcm_policy_hwInfo_get(0, policyId_storm_cpu, 0, &group_id, &entry_id, &policer_id, &counter_id) == L7_SUCCESS)
    {
      /* Also print hw group id and entry id*/
      printf("policy=%-4u rule=%u -> group=%-2d, entry=%-4d (PolicerId=%-4d CounterId %-4d)",
             policyId_storm_cpu, 0, group_id, entry_id, policer_id, counter_id);

      /* Check counter */
      if (counter_id > 0)
      {
        printf(": Packets=");
        /* Get stat data */
        if (hapiBroadPolicyStatsGet(policyId_storm_cpu, 0, &stat) != L7_SUCCESS)
          printf("Error");
        else
          printf("%llu", stat.statMode.counter.count);
      }
      printf("\r\n");
    }
    else
    {
      printf("Not configured\r\n");
    }
  }

  printf("\r\nIntercepted CPU packets at processing level: %llu\r\n", hapiBroadReceive_packets_count);
  printf("   IGMP   packets: %llu\r\n", hapiBroadReceice_igmp_count);
  printf("   MLD    packets: %llu\r\n", hapiBroadReceice_mld_count);
  printf("   DHCPv4 packets: %llu\r\n", hapiBroadReceice_dhcpv4_count);
  printf("   DHCPv6 packets: %llu\r\n", hapiBroadReceice_dhcpv6_count);
  printf("   PPPoE  packets: %llu\r\n", hapiBroadReceice_pppoe_count);
  printf("Done!\r\n");
  fflush(stdout);
}

L7_RC_t hapi_ptin_stormControl_test(L7_uint enable, L7_uint32 flags, L7_uint32 rate, PORT_EGRESS_TYPE egress_type)
{
  ptin_stormControl_t control;

  control.flags = flags;
  control.bcast_rate = rate;
  control.mcast_rate = rate;
  control.ucunk_rate = rate;
  control.cpu_rate   = rate;

  return hapi_ptin_stormControl_set(L7_NULLPTR, enable, &control, egress_type);
}

/********************************************************************
 * INTERNAL FUNCTIONS IMPLEMENTATION
 ********************************************************************/

/**
 * Initializes port mapping structures 
 *  
 * IMPORTANT: 
 *   On CXP360G, port mapping depends on the slot# (working vs protection)
 *   CPLD mapping MUST BE done prior to this function!
 * 
 * @return L7_RC_t L7_SUCCESS
 */
static L7_RC_t hapi_ptin_portMap_init(void)
{
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;
#if (PTIN_BOARD_IS_MATRIX)
  L7_uint32                     slot, lane;
  HAPI_WC_PORT_MAP_t           *hapiWCMapPtr;
#endif
  L7_uint i;
  
  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr          = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiSlotMapPtr       = dapiCardPtr->slotMap;
#if (PTIN_BOARD_IS_MATRIX)
  hapiWCMapPtr         = dapiCardPtr->wcPortMap;
#endif

  /* Initialize USP map */
  memset(usp_map, 0xff, sizeof(usp_map));   /* -1 for all values */

#if (PTIN_BOARD_IS_MATRIX)
  /* Initialize slot/lane map */
  memset(ptin_sys_slotport_to_intf_map, 0xff, sizeof(ptin_sys_slotport_to_intf_map));   /* -1 for all values */
  memset(ptin_sys_intf_to_slot_map, 0xff, sizeof(ptin_sys_intf_to_slot_map));
  memset(ptin_sys_intf_to_port_map, 0xff, sizeof(ptin_sys_intf_to_port_map));
  ptin_sys_number_of_ports = dapiCardPtr->numOfPortMapEntries;
#endif

  PT_LOG_INFO(LOG_CTX_HAPI, "Port mapping:");
  for (i = 0; i < ptin_sys_number_of_ports; i++)
  {
    /* Initialize port name */
    sprintf(hapiSlotMapPtr[i].portName, "%.10s", SOC_PORT_NAME(0, hapiSlotMapPtr[i].bcm_port));

    /* It is assumed that: i = hapiSlotMapPtr[i].portNum
     * (check dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1 */

    usp_map[i].slot = hapiSlotMapPtr[i].slotNum;
    usp_map[i].unit = hapiSlotMapPtr[i].bcm_cpuunit;
    usp_map[i].port = hapiSlotMapPtr[i].bcm_port;

  #if (PTIN_BOARD_IS_MATRIX)
    /* Only 10/140/100Gbps ports */
    if ( hapiWCMapPtr[i].slotNum >= 0 &&
         hapiWCMapPtr[i].wcLane >= 0 &&
         hapiWCMapPtr[i].wcSpeedG > 1 )
    {
      slot = hapiWCMapPtr[i].slotNum;
      lane = hapiWCMapPtr[i].wcLane;
      
      /* Update slot/lane to port map */
      if (slot <= PTIN_SYS_SLOTS_MAX && lane < PTIN_SYS_INTFS_PER_SLOT_MAX)
      {
        ptin_sys_slotport_to_intf_map[slot][lane] = i;
        ptin_sys_intf_to_slot_map[i] = slot;
        ptin_sys_intf_to_port_map[i] = lane;
      }
    }
  #endif

    PT_LOG_INFO(LOG_CTX_HAPI, " Port# %2u => Remapped# unit=%u slot=%u bcm_port=%2u (%s)", i, usp_map[i].unit, usp_map[i].slot, usp_map[i].port, hapiSlotMapPtr[i].portName);
  }

#if (PTIN_BOARD_IS_MATRIX)
  printf("Slot to intf mapping (%u interfaces):", ptin_sys_number_of_ports);
  for (slot=1; slot<=PTIN_SYS_SLOTS_MAX; slot++)
  {
    printf("\n Slot %02u: ",slot);
    for (lane=0; lane<PTIN_SYS_INTFS_PER_SLOT_MAX; lane++)
    {
      printf(" %2d",ptin_sys_slotport_to_intf_map[slot][lane]);
    }
  }
  printf("\n");
  PT_LOG_INFO(LOG_CTX_HAPI,"Intf to slot/port map (%u interfaces):",ptin_sys_number_of_ports);
  for (i=0; i<PTIN_SYSTEM_N_PORTS; i++)
  {
    PT_LOG_INFO(LOG_CTX_HAPI," Port# %2u => slot=%d/%d", i, ptin_sys_intf_to_slot_map[i], ptin_sys_intf_to_port_map[i]);
  }
#endif

  /* BCM unit is globally accessible */
  bcm_unit = usp_map[0].unit;

  return L7_SUCCESS;
}

/* Given a maximum speed, return the mask of bcm_port_ability_t speeds
 * while are less than or equal to the given speed. */
bcm_port_abil_t
static port_speed_max_mask(bcm_port_abil_t max_speed)
{
    bcm_port_abil_t speed_mask = 0;
    /* This is a giant fall through switch */
    switch (max_speed) {
        
    case 42000:
        speed_mask |= BCM_PORT_ABILITY_42GB;
    case 40000:
        speed_mask |= BCM_PORT_ABILITY_40GB;
    case 30000:
        speed_mask |= BCM_PORT_ABILITY_30GB;
    case 25000:
        speed_mask |= BCM_PORT_ABILITY_25GB;
    case 24000:
        speed_mask |= BCM_PORT_ABILITY_24GB;
    case 21000:
        speed_mask |= BCM_PORT_ABILITY_21GB;
    case 20000:
        speed_mask |= BCM_PORT_ABILITY_20GB;
    case 16000:
        speed_mask |= BCM_PORT_ABILITY_16GB;
    case 15000:
        speed_mask |= BCM_PORT_ABILITY_15GB;
    case 13000:
        speed_mask |= BCM_PORT_ABILITY_13GB;
    case 12500:
        speed_mask |= BCM_PORT_ABILITY_12P5GB;
    case 12000:
        speed_mask |= BCM_PORT_ABILITY_12GB;
    case 10000:
        speed_mask |= BCM_PORT_ABILITY_10GB;
    case 6000:
        speed_mask |= BCM_PORT_ABILITY_6000MB;
    case 5000:
        speed_mask |= BCM_PORT_ABILITY_5000MB;
    case 3000:
        speed_mask |= BCM_PORT_ABILITY_3000MB;
    case 2500:
        speed_mask |= BCM_PORT_ABILITY_2500MB;
    case 1000:
        speed_mask |= BCM_PORT_ABILITY_1000MB;
    case 100:
        speed_mask |= BCM_PORT_ABILITY_100MB;
    case 10:
        speed_mask |= BCM_PORT_ABILITY_10MB;
    default:
        break;
    }
    return speed_mask;
}

/**
 * Change a port interface type to KR4
 * 
 * @param port : ptin_port format
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_kr4_set(bcm_port_t bcm_port)
{
  bcm_error_t rc = BCM_E_NONE;
  bcm_port_ability_t port_ability, local_ability;

  /* Disable port */
  rc = bcm_port_enable_set(0, bcm_port, 0);
  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error initializing bcm_port %u", bcm_port);
    return L7_FAILURE;
  }

  /* Set 40G speed */
  rc = bcm_port_speed_set(0, bcm_port, 40000);
  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error initializing bcm_port %u", bcm_port);
    return L7_FAILURE;
  }

  /* Set Full duplex */
  rc = bcm_port_duplex_set(0, bcm_port, 1);
  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error initializing bcm_port %u", bcm_port);
    return L7_FAILURE;
  }

  /* Start of 'special' code: without this, we never get KR4 links! */
  rc = bcm_port_ability_local_get(0, bcm_port, &port_ability);
  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error initializing bcm_port %u", bcm_port);
    return L7_FAILURE;
  }

  memset(&local_ability,0x00,sizeof(local_ability));

  local_ability.speed_full_duplex  = port_ability.speed_full_duplex;
  local_ability.speed_full_duplex &= port_speed_max_mask(40000);

  local_ability.speed_half_duplex  = port_ability.speed_half_duplex;
  local_ability.speed_half_duplex &= port_speed_max_mask(40000);

  rc = bcm_port_ability_advert_set(0, bcm_port, &local_ability);
  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error initializing bcm_port %u", bcm_port);
    return L7_FAILURE;
  }
  /* End of 'special' code */

  rc = bcm_port_autoneg_set(0, bcm_port, 1);
  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error initializing bcm_port %u", bcm_port);
    return L7_FAILURE;
  }

  rc = bcm_port_interface_set(0, bcm_port, BCM_PORT_IF_KR4);
  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error initializing bcm_port %u", bcm_port);
    return L7_FAILURE;
  }

  rc = bcm_port_enable_set(0, bcm_port, 1);
  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error initializing bcm_port %u", bcm_port);
    return L7_FAILURE;
  }

  PT_LOG_INFO(LOG_CTX_HAPI, "Success initializing bcm_port %u", bcm_port);

  return L7_SUCCESS;
}


/**
 * Change a port interface type to XAUI
 * 
 * @param port : ptin_port format
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_xaui_set(bcm_port_t bcm_port)
{
  bcm_error_t rc = BCM_E_NONE;

  rc = bcm_port_interface_set(0, bcm_port, BCM_PORT_IF_XAUI);
  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error initializing bcm_port %u", bcm_port);
    return L7_FAILURE;
  }

  PT_LOG_INFO(LOG_CTX_HAPI, "Success initializing bcm_port %u", bcm_port);

  return L7_SUCCESS;
}

/**
 * System Default rules applied for PTIn 
 * 
 * @author mruas (9/21/2014)
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadSystemInstallPtin(void)
{
  BROAD_POLICY_t      policyId;
  BROAD_POLICY_RULE_t ruleId;
  L7_int              port;
  bcmx_lport_t        lport;
  bcm_port_t          bcm_port;
  L7_ushort16         vlanId, vlanMask;
  BROAD_METER_ENTRY_t meterInfo;
  L7_RC_t             rc = L7_SUCCESS;

  L7_uint8  prio, prio_mask  = 0x7;
  L7_uint8  vlanFormat_value = BROAD_VLAN_FORMAT_STAG | BROAD_VLAN_FORMAT_CTAG;
  L7_uint8  vlanFormat_mask  = 0xff;

  /* PTin added: packet trap - LACPdu's */
  /* Rate limit for LACPdu's */
  L7_ushort16 lacp_etherType = 0x8809;
  bcm_mac_t   lacp_dmac      = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x02 };
  L7_uchar8   exact_match[]  = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};

  meterInfo.cir       = RATE_LIMIT_LACP;
  meterInfo.cbs       = 128;
  meterInfo.pir       = RATE_LIMIT_LACP;
  meterInfo.pbs       = 128;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;

  /* Create policy */
  rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error creating policy");
    return L7_FAILURE;
  }

  /* Define qualifiers and actions */
  do
  {
    vlanId = 1;
    vlanMask = PTIN_SYSTEM_EVC_CPU_VLAN_MASK;

    rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGH);
    if (rc != L7_SUCCESS)  break;
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA,   (L7_uchar8 *)  lacp_dmac  , exact_match);
    if (rc != L7_SUCCESS)  break;
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID,    (L7_uchar8 *) &vlanId, (L7_uchar8 *) &vlanMask);
    if (rc != L7_SUCCESS)  break;
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *) &lacp_etherType, exact_match);
    if (rc != L7_SUCCESS)  break;
    rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_BPDU_COS, 0, 0);
    if (rc != L7_SUCCESS)  break;
    rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
    if (rc != L7_SUCCESS)  break;
    rc = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
    if (rc != L7_SUCCESS)  break;
    rc = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
    if (rc != L7_SUCCESS)  break;
    rc = hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS);
    if (rc != L7_SUCCESS)  break;

  #if PTIN_QUATTRO_FLOWS_FEATURE_ENABLED
    vlanId = PTIN_SYSTEM_EVC_QUATTRO_VLAN_MIN;
    vlanMask = PTIN_SYSTEM_EVC_QUATTRO_VLAN_MASK;

    rc = hapiBroadPolicyRuleCopy(ruleId, &ruleId);
    if (rc != L7_SUCCESS)  break;

    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *) &vlanId, (L7_uchar8 *) &vlanMask);
    if (rc != L7_SUCCESS)  break;
  #endif
  }
  while (0);
  /* Commit rule */
  if (rc == L7_SUCCESS)
  {
    rc = hapiBroadPolicyCommit(&policyId);
    if (L7_SUCCESS != rc)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP, "Error committing policy!");
      return L7_FAILURE;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error configurating rule.");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  /* Save policy */
  lacp_policyId = policyId;
  PT_LOG_NOTICE(LOG_CTX_STARTUP,"LACP rule added");

  /* For OLT1T0 */
#if (PTIN_BOARD_IS_STANDALONE)

  /* BroadLight packets should have high priority */
  vlanId = PTIN_VLAN_BL2CPU;

  /* Create policy */
  rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error creating policy");
    return L7_FAILURE;
  }
  /* Define qualifiers and actions */
  do
  {
    rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGH);
    if (rc != L7_SUCCESS)  break;
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *) &vlanId, exact_match);
    if (rc != L7_SUCCESS)  break;
    rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, 7, 0, 0);
    if (rc != L7_SUCCESS)  break;
  }
  while (0);
  /* Commit rule */
  if (rc == L7_SUCCESS)
  {
    rc = hapiBroadPolicyCommit(&policyId);
    if (L7_SUCCESS != rc)
    {
      PT_LOG_ERR(LOG_CTX_STARTUP, "Error committing policy!");
      return L7_FAILURE;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error configurating rule.");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  /* Save policy */
  bl2cpu_policyId = policyId;
  PT_LOG_NOTICE(LOG_CTX_STARTUP,"BL2CPU rule added");

#elif (PTIN_BOARD == PTIN_BOARD_TG16G)
  /* For TG16G, IPTV traffic (downstream direction) is going to egress with an extra inner tag with the UNI-VLAN.
     At egressing is important to guarantee PBIT value of inner vlan is the same as the outer tag: a copy operation will be done */

  /* Multicast services */
  L7_uint16 vlanId_value;
  L7_uint16 vlanId_mask;

  /** INGRESS STAGE **/

  /* Multicast services */
  vlanId_value = PTIN_SYSTEM_EVC_BCAST_VLAN_MIN;
  vlanId_mask  = PTIN_SYSTEM_EVC_BCAST_VLAN_MASK;

  /* Create Policy */
  rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM_PORT);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error creating policy");
    return L7_FAILURE;
  }

  /* Run all 8 priorities */
  for (prio = 0; prio < 8; prio++)
  {
    /* Priority higher than dot1p rules */
    rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_LOW);
    if (rc != L7_SUCCESS)  break;

    /* Multicast EVCs */
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *) &vlanId_value, (L7_uchar8 *) &vlanId_mask);
    if (rc != L7_SUCCESS)  break;
    /* Priority */
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OCOS, (L7_uchar8 *) &prio, (L7_uchar8 *) &prio_mask);
    if (rc != L7_SUCCESS)  break;

    /* Set COS equal to packet priority */
    rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, prio, 0, 0);
    if (rc != L7_SUCCESS)  break;
    /* Change packet priority to 0 */
    //rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_USERPRIO, 0, 0, 0);
    /* Change inner tag priority to prio */
    //rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_USERPRIO_INNERTAG, prio, 0, 0);

    if (rc != L7_SUCCESS)  break;
  }

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error configuring rule for priority %u", prio);
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Apply rules */
  rc = hapiBroadPolicyCommit(&policyId);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error commiting policy");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Add Upklink ports */
  for (port = 0; port < ptin_sys_number_of_ports; port++)
  {
    if (hapi_ptin_bcmPort_get(port, &bcm_port) == L7_SUCCESS)
    {
      lport = bcmx_unit_port_to_lport(0, bcm_port);

      if ((PTIN_SYSTEM_10G_PORTS_MASK >> port) & 1)
      {
        if (hapiBroadPolicyApplyToIface(policyId, lport) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_STARTUP, "Error adding port %u", port);
          hapiBroadPolicyDelete(policyId);
          return L7_FAILURE;
        }
      }
    }
  }
#endif

  /** COS REMARKING **/
  /** EGRESS STAGE **/

  /* Create Policy */
  rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error creating policy");
    return L7_FAILURE;
  }

  rc = hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_EGRESS);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error creating policy");
    return L7_FAILURE;
  }

  /* Run all 8 priorities */
  for (prio = 0; prio < 8; prio++)
  {
    /* ----- DOUBLE TAGGED PACKETS ----- */
    vlanFormat_value = BROAD_VLAN_FORMAT_STAG | BROAD_VLAN_FORMAT_CTAG;
    vlanFormat_mask  = 0xff;

    /* Priority higher than dot1p rules */
    rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_LOW);
    if (rc != L7_SUCCESS)  break;

    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_VLAN_FORMAT, (L7_uchar8 *) &vlanFormat_value, (L7_uchar8 *) &vlanFormat_mask);
    if (rc != L7_SUCCESS)  break;

    /* Priority */
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_INT_PRIO, (L7_uchar8 *) &prio, (L7_uchar8 *) &prio_mask);
    if (rc != L7_SUCCESS)  break;

    /* Change outer tag priority to prio */
    rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_USERPRIO, prio, 0, 0);

    /* Change inner tag priority to prio */
    rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_USERPRIO_INNERTAG, prio, 0, 0);

    /* ----- SINGLE TAGGED PACKETS ----- */
    vlanFormat_value = BROAD_VLAN_FORMAT_STAG;
    vlanFormat_mask  = 0xff;

    /* Priority higher than dot1p rules */
    rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_LOW);
    if (rc != L7_SUCCESS)  break;

    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_VLAN_FORMAT, (L7_uchar8 *) &vlanFormat_value, (L7_uchar8 *) &vlanFormat_mask);
    if (rc != L7_SUCCESS)  break;

    /* Priority */
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_INT_PRIO, (L7_uchar8 *) &prio, (L7_uchar8 *) &prio_mask);
    if (rc != L7_SUCCESS)  break;

    /* Change outer tag priority to prio */
    rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_USERPRIO, prio, 0, 0);

    if (rc != L7_SUCCESS)  break;
  }

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error configuring rule for priority %u", prio);
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Apply rules */
  rc = hapiBroadPolicyCommit(&policyId);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_STARTUP, "Error commiting policy");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Add all physical ports */
  for (port = 0; port < ptin_sys_number_of_ports; port++)
  {
    if (hapi_ptin_bcmPort_get(port, &bcm_port) == L7_SUCCESS)
    {
      lport = bcmx_unit_port_to_lport(0, bcm_port);

      if ((PTIN_SYSTEM_PORTS_MASK >> port) & 1)
      {
        if (hapiBroadPolicyApplyToIface(policyId, lport) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_STARTUP, "Error adding port %u", port);
          hapiBroadPolicyDelete(policyId);
          return L7_FAILURE;
        }
      }
    }
  }

  return L7_SUCCESS;
}



/* VCAP rules for Default VLAN */
static BROAD_POLICY_t policyId_pvid[PTIN_SYSTEM_N_PORTS]  = {[0 ... PTIN_SYSTEM_N_PORTS-1] = BROAD_POLICY_INVALID};
static BROAD_POLICY_t ruleId_pvid[PTIN_SYSTEM_N_PORTS]    = {[0 ... PTIN_SYSTEM_N_PORTS-1] = BROAD_POLICY_INVALID};

/**
 * Configure default (Outer+Inner) VLANs using VCAP
 * 
 * @param usp 
 * @param outerVlan 
 * @param innerVlan 
 * @param dapi_g  
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_vcap_defvid(DAPI_USP_t *usp, L7_uint16 outerVlan, L7_uint16 innerVlan, DAPI_t *dapi_g)
{
  L7_uint32           port;
  BROAD_POLICY_t      policyId;
  BROAD_POLICY_RULE_t ruleId;
  L7_uint8  vlan_format = BROAD_VLAN_FORMAT_UNTAG;
  L7_uint8  mask[]      = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  DAPI_PORT_t         *dapiPortPtr;
  BROAD_PORT_t        *hapiPortPtr;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate pointers */
  if (dapi_g == L7_NULLPTR || usp == L7_NULLPTR )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Null pointers");
    return L7_FAILURE;
  }

  /* Accept only physical ports */
  if (usp->unit != 1 && usp->slot != 0)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid USP {%d,%d,%d}", usp->unit, usp->slot, usp->port);
    return L7_FAILURE;
  }

  /* Validate port */
  port = usp->port;
  if (port >= PTIN_SYSTEM_N_PORTS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Invalid USP {%d,%d,%d}", usp->unit, usp->slot, usp->port);
    return L7_FAILURE;
  }

  /* Get port pointers */
  dapiPortPtr = DAPI_PORT_GET( usp, dapi_g );
  hapiPortPtr = HAPI_PORT_GET( usp, dapi_g );

  if (dapiPortPtr == L7_NULLPTR || hapiPortPtr == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Null pointers");
    return L7_FAILURE;
  }

  if (policyId_pvid[port] != BROAD_POLICY_INVALID)
  {
    hapiBroadPolicyDelete(policyId_pvid[port]);
    policyId_pvid[port] = BROAD_POLICY_INVALID;
    ruleId_pvid[port]   = BROAD_POLICY_INVALID;

    PT_LOG_TRACE(LOG_CTX_HAPI,"PVID cleared successfully");
  }

  /* Only consider valid VLANs between 2 and 4095 */
  if (outerVlan >= 2 && outerVlan <= 4095)
  {
    hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PORT);
    hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_LOOKUP);
    hapiBroadPolicyRuleAdd(&ruleId);

    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_VLAN_FORMAT, (L7_uchar8 * ) &vlan_format, mask);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error adding qualifier");
      hapiBroadPolicyCreateCancel();
      return rc;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"Vlan format qualifier added");

    /* Actions */
    /* Outer Vlan qualifier */
    rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_ADD_OUTER_VID, outerVlan, 0, 0);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error adding action");
      hapiBroadPolicyCreateCancel();
      return rc;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"Outer vlan add action added");

    /* Inner Vlan qualifier */
    if (innerVlan >= 1 && innerVlan <=4095)
    {
      rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_ADD_INNER_VID, innerVlan, 0, 0);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_HAPI,"Error adding action");
        hapiBroadPolicyCreateCancel();
        return rc;
      }
      PT_LOG_TRACE(LOG_CTX_HAPI,"Inner vlan add action added");
    }

    /* Commit */
    rc = hapiBroadPolicyCommit(&policyId);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error commiting policy");
      hapiBroadPolicyCreateCancel();
      return rc;
    }

    /* Apply to interface */
    rc = hapiBroadPolicyApplyToIface(policyId, hapiPortPtr->bcmx_lport);
    if (L7_SUCCESS != rc)
    {
      PT_LOG_ERR(LOG_CTX_HAPI,"Error applying policy to interface");
      hapiBroadPolicyDelete(policyId);
      return rc;
    }

    /* Save policy id */
    policyId_pvid[port] = policyId;
    ruleId_pvid[port]   = ruleId;

    PT_LOG_TRACE(LOG_CTX_HAPI,"PVID set successfully");
  }

  return L7_SUCCESS;
}

/**
 * Dump VCAP rules
 */
void ptin_vcap_defvid_dump(void)
{
  L7_uint i;
  BROAD_GROUP_t gid;
  BROAD_ENTRY_t eid;

  printf("Dumping VCAP defVID rules:\r\n");

  for (i = 0; i < PTIN_SYSTEM_N_PORTS; i++)
  {
    if (policyId_pvid[i] != BROAD_POLICY_INVALID && ruleId_pvid[i] != BROAD_POLICY_INVALID)
    {
      printf(" Port %2u: ", i);

      if (l7_bcm_policy_hwInfo_get(0, policyId_pvid[i], ruleId_pvid[i], &gid, &eid, L7_NULLPTR, L7_NULLPTR) != L7_SUCCESS)
      {
        printf("error\r\n");
      }
      else
      {
        printf("group=%-2d entry=%-5d\r\n", gid, eid);
      }
    }
  }
  
  fflush(stdout);
}

/**
 * Configure the global option L3UcastTtl1ToCpu on switchcontrol
 * 
 * @param usp 
 * @param enable 
 * @param dapi_g 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_hapi_L3UcastTtl1ToCpu_set(DAPI_USP_t *usp, L7_BOOL enable, DAPI_t *dapi_g)
{
  /* Configure L3UcastTtl1ToCpu */
  if (bcm_switch_control_set(0, bcmSwitchL3UcastTtl1ToCpu, enable) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error setting L3UcastTtl1ToCpu");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

BROAD_POLICY_t policyId_trap = BROAD_POLICY_INVALID;
L7_int    trap_port = -1;
L7_uint16 trap_ovlan=0, trap_ivlan=0;
L7_uint8  trap_only_drops = 0;

void ptin_debug_trap_packets_state( void )
{
  /* Remove current policy */
  if (policyId_trap == BROAD_POLICY_INVALID)
  {
    printf("No trap rule defined!\r\n");
    return;
  }

  printf("Trap rule defined:\r\n");

  if (trap_port>=0)
  {
    printf(" Inport = %d\r\n",trap_port);
  }
  if (trap_ovlan>0 && trap_ovlan<4096)
  {
    printf(" OVlan  = %u\r\n",trap_ovlan);
  }
  if (trap_ivlan>0 && trap_ivlan<4096)
  {
    printf(" IVlan  = %u\r\n",trap_ivlan);
  }
  if (trap_only_drops)
  {
    printf(" Only dropped packets are trapped.");
  }

  printf("Done!\r\n");
  fflush(stdout);
}


L7_RC_t ptin_debug_trap_packets_cancel( void )
{
  /* Remove current policy */
  if (policyId_trap == BROAD_POLICY_INVALID)
  {
    PT_LOG_TRACE(LOG_CTX_HAPI, "Trap Policy does not exist");
    return L7_SUCCESS;
  }

  /* Delete policy */
  if (hapiBroadPolicyDelete(policyId_trap)!=L7_SUCCESS)
  {
    printf("Cannot delete Trap Policy\r\n");
    return L7_FAILURE;
  }

  policyId_trap = BROAD_POLICY_INVALID;

  printf("Trap Policy deleted\r\n");
  fflush(stdout);

  return L7_SUCCESS;
}


L7_RC_t ptin_debug_trap_packets( L7_int port, L7_uint16 ovlan, L7_uint16 ivlan, L7_uint8 only_drops )
{
  BROAD_POLICY_t      policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t ruleId = BROAD_POLICY_RULE_INVALID;
  BROAD_METER_ENTRY_t meterInfo;
  bcm_port_t          bcm_port;
  pbmp_t              pbm, pbm_mask;
  L7_uint16           vlan_mask = 0x0fff;
  L7_uint8            drop = 1, drop_mask = 1;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if (port<0 &&
      (ovlan==0 || ovlan>=4096) &&
      (ivlan==0 || ivlan>=4096) &&
      !only_drops)
  {
    printf("No rule provided!\r\n");
    return L7_SUCCESS;
  }

  if (port>=0)
  {
    printf("Port %d was given\r\n",port);

    /* Validate port */
    if (hapi_ptin_bcmPort_get(port, &bcm_port)!=L7_SUCCESS)
    {
      printf("Error getting bcm_port of port %d\r\n",port);
      return L7_FAILURE;
    }

    printf("bcm_port = %d\r\n",bcm_port);

    /* Define port bitmap */
    BCM_PBMP_CLEAR(pbm);
    BCM_PBMP_PORT_ADD(pbm,bcm_port);
    hapi_ptin_allportsbmp_get(&pbm_mask);
  }
  else
  {
    port = -1;
    printf("No port provided\r\n");
  }

  /* Remove current policy */
  if (ptin_debug_trap_packets_cancel()!=L7_SUCCESS)
  {
    printf("Error removing current trap policy\r\n");
    return L7_FAILURE;
  }

  meterInfo.cir       = 256;
  meterInfo.cbs       = 256;
  meterInfo.pir       = 256;
  meterInfo.pbs       = 256;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;

  /* Clear saved paremeters */
  trap_port  = -1;
  trap_ovlan =  0;
  trap_ivlan =  0;
  trap_only_drops = 0;

  /* Create policy */
  rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
  if (rc != L7_SUCCESS)
  {
    printf("Cannot create trap policy\r\n");
    return L7_FAILURE;
  }
  printf("tRAP Policy created\r\n");

  /* Create rule */
  rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGHEST);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding rule\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Add source port qualifier */
  if (port>=0)
  {
    printf("Adding port qualifier (port=%u, bcm_port=%d)\r\n",port,bcm_port);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_INPORTS, (L7_uchar8 *)&pbm, (L7_uchar8 *)&pbm_mask);
    if (rc != L7_SUCCESS)
    {
      printf("Error adding port qualifier (port=%u, bcm_port=%d)\r\n",port,bcm_port);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_port = port;
    printf("Port qualifier (port=%u, bcm_port=%d) added\r\n",port,bcm_port);
  }
  /* Add outer vlan qualifier */
  if (ovlan>0 && ovlan<4096)
  {
    printf("Adding outer vlan qualifier (ovlan=%u)",ovlan);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&ovlan, (L7_uchar8 *)&vlan_mask);
    if (rc != L7_SUCCESS)
    {
      printf("Error adding outer vlan qualifier (ovlan=%u/0x%03x)\r\n",ovlan,vlan_mask);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_ovlan = ovlan;
    printf("Outer vlan qualifier added (ovlan=%u/0x%03x)\r\n",ovlan,vlan_mask);
  }
  /* Add inner vlan qualifier */
  if (ivlan>0 && ivlan<4096)
  {
    printf("Adding inner vlan qualifier (ivlan=%u)\r\n",ivlan);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IVID, (L7_uchar8 *)&ivlan, (L7_uchar8 *)&vlan_mask);
    if (rc != L7_SUCCESS)
    {
      printf("Error adding inner vlan qualifier (ivlan=%u/0x%03x)\r\n",ivlan,vlan_mask);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_ivlan = ivlan;
    printf("Inner vlan qualifier added (ivlan=%u/0x%03x)\r\n",ivlan,vlan_mask);
  }

  /* Add drop qualifer */
  if ( only_drops )
  {
    printf("Adding Drop qualifier (drop=%u)\r\n",drop);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DROP, (L7_uchar8 *)&drop, (L7_uchar8 *)&drop_mask);
    if (rc != L7_SUCCESS)
    {
      printf("Error adding drop qualifier (drop=%u/0x%02x)\r\n",drop,drop_mask);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_only_drops = drop;
    printf("Drop qualifier added (drop=%u/0x%02x)\r\n",drop,drop_mask);
  }

  #if 0
  /* Ingress priority */
  rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_LOWEST_PRIORITY_COS, 0, 0);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding action\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("Ingress priority action added\r\n");
  #endif

  /* Trap to cpu action */
  rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_COPY_TO_CPU, 0, 0, 0);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding copy_to_cpu action\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("copy_to_cpu action added\r\n");

  /* Do not cause drops (to not interfere with switching). CopyToCpu action, will only copy green packets */
  #if 0
  /* Drop all packets */
  rc = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding hard_drop action\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("hard_drop action added\r\n");
  #endif

  /* Define meter action, to classify packets */
  rc = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding rate limit\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("Rate limit added\r\n");

  printf("Commiting trap policy\r\n");
  if ((rc=hapiBroadPolicyCommit(&policyId)) != L7_SUCCESS)
  {
    printf("Error commiting trap policy\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("Trap policy commited successfully (policyId=%u)\r\n",policyId);

  /* Save policy id */
  policyId_trap = policyId;

  fflush(stdout);

  return L7_SUCCESS;
}

L7_RC_t ptin_debug_trap_packets_egress( L7_int port, L7_uint16 ovlan, L7_uint16 ivlan, L7_uint8 only_drops )
{
  BROAD_POLICY_t      policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t ruleId = BROAD_POLICY_RULE_INVALID;
  BROAD_METER_ENTRY_t meterInfo;
  bcm_port_t          bcm_port;
  L7_uint32           mask = 0xffffffff;
  L7_uint8            drop = 1;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if (port<0 &&
      (ovlan==0 || ovlan>=4096) &&
      (ivlan==0 || ivlan>=4096) &&
      !only_drops)
  {
    printf("No rule provided!\r\n");
    return L7_SUCCESS;
  }

  if (port>=0)
  {
    printf("Port %d was given\r\n",port);

    /* Validate port */
    if (hapi_ptin_bcmPort_get(port, &bcm_port)!=L7_SUCCESS)
    {
      printf("Error getting bcm_port of port %d\r\n",port);
      return L7_FAILURE;
    }

    printf("bcm_port = %d\r\n",bcm_port);
  }
  else
  {
    port = -1;
    printf("No port provided\r\n");
  }

  /* Remove current policy */
  if (ptin_debug_trap_packets_cancel()!=L7_SUCCESS)
  {
    printf("Error removing current trap policy\r\n");
    return L7_FAILURE;
  }

  meterInfo.cir       = 256;
  meterInfo.cbs       = 256;
  meterInfo.pir       = 256;
  meterInfo.pbs       = 256;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;

  /* Clear saved paremeters */
  trap_port  = -1;
  trap_ovlan =  0;
  trap_ivlan =  0;
  trap_only_drops = 0;

  /* Create policy */
  rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PTIN);
  if (rc != L7_SUCCESS)
  {
    printf("Cannot create trap policy\r\n");
    return L7_FAILURE;
  }
  printf("tRAP Policy created\r\n");

  /* Egress stage */
  if (hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_EGRESS) != L7_SUCCESS)
  {
    printf("Error creating a egress policy\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Create rule */
  rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGHEST);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding rule\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Add source port qualifier */
  if (port>=0)
  {
    printf("Adding port qualifier (port=%u, bcm_port=%d)\r\n",port,bcm_port);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OUTPORT, (L7_uchar8 *)&bcm_port, (L7_uchar8 *)&mask);
    if (rc != L7_SUCCESS)
    {
      printf("Error adding port qualifier (port=%u, bcm_port=%d)\r\n",port,bcm_port);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_port = port;
    printf("Port qualifier (port=%u, bcm_port=%d) added\r\n",port,bcm_port);
  }
  /* Add outer vlan qualifier */
  if (ovlan>0 && ovlan<4096)
  {
    printf("Adding outer vlan qualifier (ovlan=%u)",ovlan);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&ovlan, (L7_uchar8 *)&mask);
    if (rc != L7_SUCCESS)
    {
      printf("Error adding outer vlan qualifier (ovlan=%u/0x%03x)\r\n",ovlan,mask);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_ovlan = ovlan;
    printf("Outer vlan qualifier added (ovlan=%u/0x%03x)\r\n",ovlan,mask);
  }
  /* Add inner vlan qualifier */
  if (ivlan>0 && ivlan<4096)
  {
    printf("Adding inner vlan qualifier (ivlan=%u)\r\n",ivlan);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IVID, (L7_uchar8 *)&ivlan, (L7_uchar8 *)&mask);
    if (rc != L7_SUCCESS)
    {
      printf("Error adding inner vlan qualifier (ivlan=%u/0x%03x)\r\n",ivlan,mask);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_ivlan = ivlan;
    printf("Inner vlan qualifier added (ivlan=%u/0x%03x)\r\n",ivlan,mask);
  }

  /* Add drop qualifer */
  if ( only_drops )
  {
    printf("Adding Drop qualifier (drop=%u)\r\n",drop);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DROP, (L7_uchar8 *)&drop, (L7_uchar8 *)&mask);
    if (rc != L7_SUCCESS)
    {
      printf("Error adding drop qualifier (drop=%u/0x%02x)\r\n",drop,mask);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_only_drops = drop;
    printf("Drop qualifier added (drop=%u/0x%02x)\r\n",drop,mask);
  }

  #if 0
  /* Ingress priority */
  rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_LOWEST_PRIORITY_COS, 0, 0);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding action\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("Ingress priority action added\r\n");
  #endif

  /* Trap to cpu action */
  rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding trap_to_cpu action\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("trap_to_cpu action added\r\n");

  #if 0
  /* Drop all packets */
  rc = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding hard_drop action\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("hard_drop action added\r\n");
  #endif

  /* Define meter action, to rate limit packets */
  rc = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding rate limit\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("Rate limit added\r\n");

  printf("Commiting trap policy\r\n");
  if ((rc=hapiBroadPolicyCommit(&policyId)) != L7_SUCCESS)
  {
    printf("Error commiting trap policy\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("Trap policy commited successfully (policyId=%u)\r\n",policyId);

  /* Save policy id */
  policyId_trap = policyId;

  fflush(stdout);

  return L7_SUCCESS;
}

L7_RC_t ptin_debug_trap_packets_mirror(L7_int dst_port, L7_int port, L7_uint16 ovlan, L7_uint16 ivlan, L7_uint8 only_drops)
{
  BROAD_POLICY_t      policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t ruleId = BROAD_POLICY_RULE_INVALID;
  bcm_port_t          bcm_port;
  DAPI_USP_t          ddUsp;
  L7_uint16           mask = 0xffff;
  L7_uint8            drop = 1;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if (port<0 &&
      (ovlan==0 || ovlan>=4096) &&
      (ivlan==0 || ivlan>=4096) &&
      !only_drops)
  {
    printf("No rule provided!\r\n");
    return L7_SUCCESS;
  }

  /* Destination port */
  if (dst_port >= 0)
  {
    printf("Dst Port %d was given\r\n",dst_port);

    if (dst_port >= ptin_sys_number_of_ports)
    {
      printf("Invalid port\r\n");
      return L7_FAILURE;
    }
    ddUsp.unit = 1;
    ddUsp.slot = 0;
    ddUsp.port = dst_port;

    printf("ddUsp = {%d,%d,%d}\r\n",ddUsp.unit,ddUsp.slot,ddUsp.port);
  }
  else
  {
    printf("No destination port provided\r\n");
    return L7_FAILURE;
  }

  /* Source port */
  if (port>=0)
  {
    printf("Port %d was given\r\n",port);

    /* Validate port */
    if (hapi_ptin_bcmPort_get(port, &bcm_port)!=L7_SUCCESS)
    {
      printf("Error getting bcm_port of port %d\r\n",port);
      return L7_FAILURE;
    }

    printf("bcm_port = %d\r\n",bcm_port);
  }
  else
  {
    port = -1;
    printf("No port provided\r\n");
  }

  /* Remove current policy */
  if (ptin_debug_trap_packets_cancel()!=L7_SUCCESS)
  {
    printf("Error removing current trap policy\r\n");
    return L7_FAILURE;
  }

  /* Clear saved paremeters */
  trap_port  = -1;
  trap_ovlan =  0;
  trap_ivlan =  0;
  trap_only_drops = 0;

  /* Create policy */
  rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PTIN);
  if (rc != L7_SUCCESS)
  {
    printf("Cannot create trap policy\r\n");
    return L7_FAILURE;
  }
  printf("tRAP Policy created\r\n");

  /* Egress stage */
  if (hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_EGRESS) != L7_SUCCESS)
  {
    printf("Error creating a egress policy\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Create rule */
  rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGHEST);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding rule\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Add source port qualifier */
  if (port>=0)
  {
    printf("Adding port qualifier (port=%u, bcm_port=%d)\r\n",port,bcm_port);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OUTPORT, (L7_uchar8 *)&bcm_port, (L7_uchar8 *)&mask);
    if (rc != L7_SUCCESS)
    {
      printf("Error adding port qualifier (port=%u, bcm_port=%d)\r\n",port,bcm_port);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_port = port;
    printf("Port qualifier (port=%u, bcm_port=%d) added\r\n",port,bcm_port);
  }

  /* Add outer vlan qualifier */
  if (ovlan>0 && ovlan<4096)
  {
    printf("Adding outer vlan qualifier (ovlan=%u)\r\n",ovlan);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&ovlan, (L7_uchar8 *)&mask);
    if (rc != L7_SUCCESS)
    {
      printf("Error adding outer vlan qualifier (ovlan=%u/0x%03x)\r\n",ovlan,mask);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_ovlan = ovlan;
    printf("Outer vlan qualifier added (ovlan=%u/0x%03x)\r\n",ovlan,mask);
  }

  /* Add inner vlan qualifier */
  if (ivlan>0 && ivlan<4096)
  {
    printf("Adding inner vlan qualifier (ivlan=%u)\r\n",ivlan);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IVID, (L7_uchar8 *)&ivlan, (L7_uchar8 *)&mask);
    if (rc != L7_SUCCESS)
    {
      printf("Error adding inner vlan qualifier (ivlan=%u/0x%03x)\r\n",ivlan,mask);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_ivlan = ivlan;
    printf("Inner vlan qualifier added (ivlan=%u/0x%03x)\r\n",ivlan,mask);
  }

  /* Add drop qualifer */
  if ( only_drops )
  {
    printf("Adding Drop qualifier (drop=%u)\r\n",drop);
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DROP, (L7_uchar8 *)&drop, (L7_uchar8 *)&mask);
    if (rc != L7_SUCCESS)
    {
      printf("Error adding drop qualifier (drop=%u/0x%02x)\r\n",drop,mask);
      hapiBroadPolicyCreateCancel();
      return L7_FAILURE;
    }
    trap_only_drops = drop;
    printf("Drop qualifier added (drop=%u/0x%02x)\r\n",drop,mask);
  }

  /* Trap to cpu action */
  rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_REDIRECT, ddUsp.unit, ddUsp.slot, ddUsp.port);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding trap_to_cpu action\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("trap_to_cpu action added\r\n");

  #if 0
  /* Trap to cpu action */
  rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding trap_to_cpu action\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("trap_to_cpu action added\r\n");
  #endif

  printf("Commiting trap policy\r\n");
  if ((rc=hapiBroadPolicyCommit(&policyId)) != L7_SUCCESS)
  {
    printf("Error commiting trap policy\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("Trap policy commited successfully (policyId=%u)\r\n",policyId);

  /* Save policy id */
  policyId_trap = policyId;

  fflush(stdout);

  return L7_SUCCESS;
}

/**
 * Show trapped packets (to CPU) according to the configured 
 * trap rule 
 * 
 * @param bcm_port : Input port (bcm representation)
 * @param ovlan : Outer vlan
 * @param ivlan : Inner vlan
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_debug_trap_packets_show( L7_int bcm_port, bcm_pkt_t *bcm_pkt )
{
  int i;
  int trap_bcm_port;

  /* Validate arguments */
  if ( bcm_port < 0 || bcm_pkt == L7_NULLPTR )
  {
    return L7_SUCCESS;
  }

  /* Rule is defined? */
  if (policyId_trap == BROAD_POLICY_INVALID)
  {
    return L7_SUCCESS;
  }

  /* Check if packet properties match the defined rule */
  if (trap_port>=0)
  {
    /* Validate port */
    if (hapi_ptin_bcmPort_get(trap_port, &trap_bcm_port)!=L7_SUCCESS)
    {
      printf("Error getting bcm_port of trap_port %d\r\n",trap_port);
      return L7_FAILURE;
    }
    if (bcm_port != trap_bcm_port)
      return L7_SUCCESS;
  }
  if (trap_ovlan>0 && trap_ovlan<4096)
  {
    if (bcm_pkt->vlan != trap_ovlan)
      return L7_SUCCESS;
  }
  if (trap_ivlan>0 && trap_ivlan<4096)
  {
    if (bcm_pkt->inner_vlan != trap_ivlan)
      return L7_SUCCESS;
  }

  printf("Packet received on port %u (bcm_port %u), oVlan=%u.%d, iVlan=%u.%d (int.prio=%d cos=%d):\r\n",
         trap_port, bcm_port,
         bcm_pkt->vlan, bcm_pkt->vlan_pri,
         bcm_pkt->inner_vlan, bcm_pkt->inner_vlan_pri,
         bcm_pkt->prio_int, bcm_pkt->cos);

  for (i=0; i<bcm_pkt->pkt_len && i < 128; i++)
  {
    if (i%16==0)
    {
      if (i!=0)
        printf("\r\n");
      printf(" 0x%02x:",i);
    }
    printf(" %02x",bcm_pkt->pkt_data->data[i]);
  }
  printf("\r\n");
  fflush(stdout);

  return L7_SUCCESS;
}

L7_RC_t ptin_hapi_register_set(L7_int ptin_port, L7_uint16 block, L7_uint16 offset, L7_uint16 value)
{
  bcm_error_t rv;
  bcm_port_t bcm_port;

  if (hapi_ptin_bcmPort_get(ptin_port, &bcm_port) != L7_SUCCESS)
  {
    printf("Error getting bcm_port from ptin_port %u\r\n", ptin_port);
    return L7_FAILURE;
  }

  /* Force links to be up: bit 6 of register 0x8012 */
  rv = bcm_port_phy_set(0, bcm_port, BCM_PORT_PHY_INTERNAL, BCM_PORT_PHY_REG_INDIRECT_ADDR(0,block,offset), value);

  if (rv != BCM_E_NONE)
  {
    printf("Error writing to register 0x%04x/0x%x at port %u\r\n", block, offset, ptin_port);
    return L7_FAILURE;
  }

  printf("Success writing to register 0x%04x/0x%x at port %u\r\n", block, offset, ptin_port);
  fflush(stdout);

  return L7_SUCCESS;
}

/**
 * Generic HAPI example 
 * 
 * @author mruas (2/26/2015)
 * 
 * @param usp 
 * @param example
 * @param dapi_g 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_hapi_example(DAPI_USP_t *usp, ptin_dtl_example_t *example, DAPI_t *dapi_g)
{
  PT_LOG_INFO(LOG_CTX_HAPI, "Hello World: usp={%d,%d,%d}", usp->unit, usp->slot, usp->port);

  /* Validate data pointer */
  if (example == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Structure data pointer is null");
    return L7_FAILURE;
  }

  PT_LOG_INFO(LOG_CTX_HAPI, "Structure contents: param1=%u param2=%u", example->param1, example->param2); 

  return L7_SUCCESS;
}

