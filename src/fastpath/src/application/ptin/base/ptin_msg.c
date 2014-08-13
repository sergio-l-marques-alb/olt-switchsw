/**
 * ptin_msg.c
 *
 * Created on:
 * Author:
 * 
 * Modified on 2011/06/14 
 * By: Alexandre Santos (alexandre-r-santos@ptinovacao.pt) 
 * Notes: 
 */

#include <unistd.h>
#include <math.h>
#include <string.h>

#include "ptin_msg.h"
#include "ptin_intf.h"
#include "ptin_evc.h"
#include "ptin_igmp.h"
#include "ptin_dhcp.h"
#include "ptin_pppoe.h"
#include "ptin_prot_typeb.h"
#include "ptin_l2.h"
#include "ptin_fieldproc.h"
#include "ptin_cfg.h"
#include "ptin_control.h"
#include "tty_ptin.h"
#include "ipc.h"
#include "ptin_msghandler.h"
#include "ptin_cnfgr.h"
#include "nimapi.h"
#include <ptin_prot_oam_eth.h>
#include "ptin_prot_erps.h"
#include "ptin_hal_erps.h"
#include "ptin_intf.h"
#include "ptin_xconnect_api.h"
#include "fdb_api.h"
#include "usmdb_ip_api.h"
#include "ptin_xlate_api.h"

#include "ptin_acl.h"
#include "ptin_routing.h"

#ifndef SNOOPING_API_H
#include "snooping_api.h" //To interact with SNOOP
#endif

#ifndef DHCP_SNOOPING_API_H
#include "dhcp_snooping_api.h"//To interact with IPSG
#endif

#include "ptin_fpga_api.h"//To interact with CPLD register

#define CMD_MAX_LEN   200   /* Shell command maximum length */

/******************************************************** 
 * STATIC FUNCTIONS PROTOTYPES
 ********************************************************/
static L7_RC_t ptin_msg_ptinPort_get(L7_uint8 intf_type, L7_uint8 intf_id, L7_int *ptin_port);

static L7_RC_t ptin_shell_command_run(L7_char8 *tty, L7_char8 *type, L7_char8 *cmd);

static void ptin_msg_PortStats_convert(msg_HWEthRFC2819_PortStatistics_t  *msgPortStats,
                                       ptin_HWEthRFC2819_PortStatistics_t *ptinPortStats);

static L7_RC_t ptin_msg_bwProfileStruct_fill(msg_HwEthBwProfile_t *msgBwProfile, ptin_bw_profile_t *profile);
static L7_RC_t ptin_msg_evcStatsStruct_fill(msg_evcStats_t *msg_evcStats, ptin_evcStats_profile_t *evcStats_profile);

/******************************************************** 
 * EXTERNAL FUNCTIONS IMPLEMENTATION
 ********************************************************/

/* FastPath Misc Functions ****************************************************/
/**
 * Execute drivshell or devshell command string
 * 
 * @param str Input string used to call driv or devshell
 * 
 * @return L7_RC_t Return code L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_ShellCommand_run(L7_char8 *str)
{
  L7_char8 *tty, *type, *cmd;
  L7_uint i   = 0;
  L7_uint len = strlen(str);
  L7_RC_t rc;

  if (len > CMD_MAX_LEN)
    len = CMD_MAX_LEN;

  /* TTY */
  tty = str;

  /* Validate command */
  if (*tty == '\0' || i >= len)
    return L7_FAILURE;

  /* TYPE */
  /* Search for a space */
  for (type = str; (i < len) && (*type != '\0') && (*type != ' '); type++, i++);
  if (*type == ' ')
  {
    *(type++) = '\0';
    i++;
  }
  /* Validate command */
  if (*type == '\0' || i >= len)
    return L7_FAILURE;

  /* COMMAND */
  /* Search for a space */
  for (cmd = type; (i < len) && (*cmd != '\0') && (*cmd != ' '); cmd++, i++);
  if (*cmd == ' ')
  {
    *(cmd++) = '\0';
    i++;
  }

  /* Validate command */
  if (*cmd == '\0' || i >= len)
    return L7_FAILURE;

  rc = ptin_shell_command_run(tty,type,cmd);

  fflush(stdout);

  return rc;
}

/**
 * Gets general info about FastPath firmware
 * 
 * @param msgFPInfo Pointer to output structure
 * 
 * @return L7_RC_t L7_SUCCESS (always)
 */
L7_RC_t ptin_msg_FPInfo_get(msg_FWFastpathInfo *msgFPInfo)
{
  memset(msgFPInfo, 0x00, sizeof(msg_FWFastpathInfo));

  msgFPInfo->SlotIndex    = ptin_fgpa_board_slot();
  msgFPInfo->BoardPresent = (ptin_state == PTIN_LOADED);

  osapiStrncpySafe(msgFPInfo->BoardSerialNumber, "FASTPATH 6.3.0.2", 20);

  return L7_SUCCESS;
}

/* Reset Functions ************************************************************/

/**
 * Reset to default configuration 
 *  
 * Actions: 
 *  - EVCs are destroyed (including counter, bw profiles, clientes, etc)
 *  - ERPS intances are destroyed
 */
void ptin_msg_defaults_reset(L7_char8 mode)
{
  LOG_INFO(LOG_CTX_PTIN_MSG, "Resetting to default configuration (mode %x)", mode);

#ifdef __Y1731_802_1ag_OAM_ETH__
  eth_srv_oam_msg_defaults_reset();
#endif

  /* Remove all IGMP instances */
  ptin_igmp_clean_all();

  /* Reset MGMD */
  ptin_igmp_mgmd_resetdefaults();

  /* EVCs */
  ptin_evc_destroy_all();

  /* ERPS */
#ifdef PTIN_ENABLE_ERPS
  ptin_erps_clear();
  ptin_hal_erps_clear();
#endif

  ptin_aclCleanAll();

  if (mode == DEFATUL_RESET_MODE_FULL)
  {
    ptin_NtwConnectivity_t ptinNtwConn;

    /* Unconfig Connectivity */
    memset(&ptinNtwConn, 0x00, sizeof(ptin_NtwConnectivity_t));
    ptinNtwConn.mask = PTIN_NTWCONN_MASK_IPADDR;
    ptin_cfg_ntw_connectivity_set(&ptinNtwConn);

    ptin_intf_Lag_delete_all();
  }

  return;
}

/**
 * Reset Multicast Machine
 * 
 * @param msg : (no meaning)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_multicast_reset(msg_HwGenReq_t *msg)
{
  schedule_matrix_query_send();

  return L7_SUCCESS;
}

/**
 * TYPE B Protection interface switch notification
 * 
 * @param msg : (no meaning)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_typeBprotIntfSwitchNotify(msg_HwTypeBProtSwitchNotify_t *msg)
{
  L7_RC_t   rc;
  L7_uint32 intIfNum;
  L7_uint8  status;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Type-B Protection switch notification");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " slotId = %u"   , msg->slotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " portId = %u", msg->portId); //ptin_port format
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " cmd    = %08X" , msg->cmd);

  /* Convert portId to intfNum */
  if (ptin_intf_port2intIfNum(msg->portId, &intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Non existent port");
    return L7_FAILURE;
  }

  /* Get interface status from the first bit of msg->cmd */
  status = msg->cmd & 0x0001;

  /* Update interface configurations */
  rc = ptin_prottypeb_intf_switch_notify(intIfNum, status);
  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to set interface's type-b protection configurations");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * TYPE B Protection Interface Configuration
 * 
 * @param msg : (no meaning)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_typeBprotIntfConfig(msg_HwTypeBProtIntfConfig_t *msg)
{
  L7_RC_t                      rc;
  L7_int                       ptin_port;
  ptin_prottypeb_intf_config_t ptin_intfConfig;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Configurations");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " slotId     = %u"   , msg->slotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " intfId     = %u/%u", msg->intfId.intf_type, msg->intfId.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " pairSlotId = %u"   , msg->pairSlotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " pairIntfId = %u/%u", msg->pairIntfId.intf_type, msg->pairIntfId.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " intfRole   = %u"   , msg->intfRole);

  memset(&ptin_intfConfig, 0x00, sizeof(ptin_intfConfig));

  /* Convert intfId to intfNum */
  if (ptin_msg_ptinPort_get(msg->intfId.intf_type, msg->intfId.intf_id, &ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid port");
    return L7_FAILURE;
  }
  if (ptin_intf_port2intIfNum(ptin_port, &ptin_intfConfig.intfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Non existent port");
    return L7_FAILURE;
  }

  /* Convert pairIntfId to intfNum */
  if (ptin_msg_ptinPort_get(msg->pairIntfId.intf_type, msg->pairIntfId.intf_id, &ptin_port)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid port");
    return L7_FAILURE;
  }
  if (ptin_intf_port2intIfNum(ptin_port, &ptin_intfConfig.pairIntfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Non existent port");
    return L7_FAILURE;
  }

  ptin_intfConfig.pairSlotId = msg->pairSlotId;
  ptin_intfConfig.intfRole   = msg->intfRole;
  ptin_intfConfig.slotId     = msg->slotId;

  /* Save interface configurations */
  rc = ptin_prottypeb_intf_config_set(&ptin_intfConfig);
  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to set interface's type-b protection configurations");
    return L7_FAILURE;
  }
 
  return L7_SUCCESS;
}

/**
 * TYPE B Protection Switching
 * 
 * @param msg : (no meaning)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_typeBprotSwitch(msg_HwTypeBprot_t *msg)
{
  L7_RC_t   rc = L7_SUCCESS;
#if (PTIN_BOARD_IS_MATRIX)
  L7_uint32 lag_idx;
  L7_uint32 intIfNum;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ptin_msg_typeBprotSwitch(slot %d)", msg->slot);

  rc = ptin_intf_slot2lagIdx(msg->slot, &lag_idx);
  if (rc==L7_SUCCESS)
  {
    rc = ptin_intf_lag2intIfNum(lag_idx, &intIfNum);
    if (rc==L7_SUCCESS)
    {
      rc = fdbFlushByPort(intIfNum);
    }
  }
  #endif

#if PTIN_BOARD_IS_LINECARD
  /* Reset MGMD General Querier state */
  rc = ptin_igmp_generalquerier_reset();
  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to reset MGMD General Queriers");
  }
#endif

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "return %d", rc);
  }
  

  return L7_SUCCESS;
}

/**
 * Apply linkscan procedure
 * 
 * @param msg : message
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_board_action(msg_HwGenReq_t *msg)
{
  L7_RC_t rc = L7_SUCCESS;

  LOG_INFO(LOG_CTX_PTIN_MSG, "ptin_msg_board_action");
  LOG_DEBUG(LOG_CTX_PTIN_MSG," slot       = %u", msg->slot_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," generic_id = %u", msg->generic_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," type       = 0x%02x", msg->type);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," param      = 0x%02x", msg->param);

  #if (PTIN_BOARD_IS_MATRIX)

  /* insertion action */
  if (msg->type == 0x03)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Insertion detected (slot %u, board_id=%u)", msg->generic_id, msg->param);

    rc = ptin_slot_action_insert(msg->generic_id, msg->param);
    if ( rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error inserting card (%d)", rc);
    }
    else
    {
      LOG_INFO(LOG_CTX_PTIN_MSG, "Card inserted successfully");
    }
  }
  /* Board removed */
  else if (msg->type == 0x00)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Remotion detected (slot %u)", msg->generic_id);

    rc = ptin_slot_action_remove(msg->generic_id);
    if ( rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error removing card (%d)", rc);
    }
    else
    {
      LOG_INFO(LOG_CTX_PTIN_MSG, "Card removed successfully");
    }
  }
  #endif

  return rc;
}

/**
 * Apply linkscan procedure
 * 
 * @param msg : message
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_link_action(msg_HwGenReq_t *msg)
{
  L7_RC_t rc = L7_SUCCESS;

  LOG_INFO(LOG_CTX_PTIN_MSG, "ptin_msg_link_action");
  LOG_DEBUG(LOG_CTX_PTIN_MSG," slot       = %u", msg->slot_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," generic_id = %u", msg->generic_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," type       = 0x%02x", msg->type);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," param      = 0x%02x", msg->param);

  #if 0
  #if (PTIN_BOARD_IS_MATRIX)
  #ifdef MAP_CPLD
  L7_uint16 board_type;
  L7_uint32 ptin_port, intIfNum;

  /* Only active matrix will process these messages */
  if (!cpld_map->reg.mx_is_active)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "I am not active matrix");
    return L7_SUCCESS;
  }

  osapiSemaTake(ptin_boardaction_sem, L7_WAIT_FOREVER);

  /* Get board id for this interface */
  rc = ptin_slot_boardid_get(msg->generic_id, &board_type);
  if (rc != L7_SUCCESS)
  {
    osapiSemaGive(ptin_boardaction_sem);
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting board_id for slot id %u (rc=%d)", msg->generic_id, rc);
    return L7_FAILURE;
  }

  /* Only consider uplink boards */
  if (!PTIN_BOARD_IS_UPLINK(board_type))
  {
    osapiSemaGive(ptin_boardaction_sem);
    LOG_ERR(LOG_CTX_PTIN_MSG, "Not an uplink board (board_id=%u)", board_type);
    return L7_FAILURE;
  }

  /* Get ptin_port and intIfNum */
  if (ptin_intf_slotPort2port(msg->generic_id, msg->param, &ptin_port) != L7_SUCCESS)
  {
    osapiSemaGive(ptin_boardaction_sem);
    LOG_ERR(LOG_CTX_PTIN_MSG, "No ptin_port related to slot/port %u/%u", msg->generic_id, msg->param);
    return L7_FAILURE;
  }
  if (ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
  {
    osapiSemaGive(ptin_boardaction_sem);
    LOG_ERR(LOG_CTX_PTIN_MSG, "No intIfNum related to ptin_port %u", ptin_port);
    return L7_FAILURE;
  }

  /* Should be a protection port */
  if (!ptin_intf_is_uplinkProtection(ptin_port))
  {
    osapiSemaGive(ptin_boardaction_sem);
    LOG_ERR(LOG_CTX_PTIN_MSG, "ptin_port %u is not a protection port", ptin_port);
    return L7_FAILURE;
  }

  /* When link is up, disable linkscan */
  if (msg->type == 0x01)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Link-up detected at ptin_port %u", ptin_port);

    #ifdef PTIN_LINKSCAN_CONTROL
    /* Linkscan control should be enabled */
    if (linkscan_update_control)
    {
      /* Force link-up */
      /* It it is going to force a link up, it is importante to avoid loops during that procedure.
         To guarantee that, this port will be removed from all vlans.
         Only protection ports at inactive state, don't need this procedure */
      if (ptin_intf_is_uplinkProtectionActive(ptin_port))
      {
        ptin_vlan_port_remove(ptin_port, 0);
      }

      rc = ptin_intf_link_force(intIfNum, L7_TRUE, L7_ENABLE);
      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_API, "Error enabling force linkup for port %u (%d)", ptin_port, rc);
      }

      /* Add port to vlans again */
      if (ptin_intf_is_uplinkProtectionActive(ptin_port))
      {
        ptin_vlan_port_add(ptin_port, 0);
      }

      LOG_DEBUG(LOG_CTX_PTIN_API, "Forced linkup for port %u", ptin_port);
    }
    #endif
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Link-down detected at ptin_port %u", ptin_port);

    #ifdef PTIN_LINKSCAN_CONTROL
    if (linkscan_update_control)
    {
      /* Remove forced link-up */
      rc = ptin_intf_link_force(intIfNum, L7_TRUE, L7_DISABLE);
      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_API, "Error disabling force linkup for port %u (%d)", ptin_port, rc);
      }

      /* Cause link-down */
      rc = ptin_intf_link_force(intIfNum, L7_FALSE, 0);
      if (rc != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_API, "Error disabling force linkup for port %u (%d)", ptin_port, rc);
      }
      LOG_DEBUG(LOG_CTX_PTIN_API, "Force link-up disabled for port %u", ptin_port);
    }
    #endif
  }

  osapiSemaGive(ptin_boardaction_sem);
  #endif
  #endif
  #endif

  return rc;
}

/**
 * Reset alarms state
 *  
 */
void ptin_msg_alarms_reset(void)
{
  LOG_INFO(LOG_CTX_PTIN_MSG, "Resetting alarms states");

  /* Init alarms state */
  ptin_alarms_init();

  return;
}

/* Resources ******************************************************************/ 
/**
 * Read hardware resources
 * 
 * @param msgResources : structure with the availanle resources 
 *  
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_hw_resources_get(msg_ptin_policy_resources *msgResources)
{
  L7_uint8 slotId, group_idx, stage_idx;
  st_ptin_policy_resources resources;
  L7_RC_t rc = L7_SUCCESS;

  /* Clear output structure */
  slotId = msgResources->SlotId;
  memset(msgResources,0x00,sizeof(msg_ptin_policy_resources));
  msgResources->SlotId = slotId;

  /* Read hardware available resources */
  if ((rc=ptin_hw_resources_get(&resources))!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error consulting hardware resources");
    return rc;
  }

  /* Copy data to output structure */
  for (stage_idx=0; stage_idx<PTIN_POLICY_MAX_STAGES; stage_idx++)
  {
    for (group_idx=0; group_idx<PTIN_POLICY_MAX_GROUPS; group_idx++)
    {
      msgResources->cap[group_idx][stage_idx].inUse             = resources.cap[group_idx][stage_idx].inUse;
      msgResources->cap[group_idx][stage_idx].group_id          = resources.cap[group_idx][stage_idx].group_id;

      msgResources->cap[group_idx][stage_idx].total.counters    = resources.cap[group_idx][stage_idx].total.counters;
      msgResources->cap[group_idx][stage_idx].total.meters      = resources.cap[group_idx][stage_idx].total.meters;
      msgResources->cap[group_idx][stage_idx].total.rules       = resources.cap[group_idx][stage_idx].total.rules;
      msgResources->cap[group_idx][stage_idx].total.slice_width = resources.cap[group_idx][stage_idx].total.slice_width;

      msgResources->cap[group_idx][stage_idx].free.counters     = resources.cap[group_idx][stage_idx].free.counters;
      msgResources->cap[group_idx][stage_idx].free.meters       = resources.cap[group_idx][stage_idx].free.meters;
      msgResources->cap[group_idx][stage_idx].free.rules        = resources.cap[group_idx][stage_idx].free.rules;
      msgResources->cap[group_idx][stage_idx].free.slice_width  = resources.cap[group_idx][stage_idx].free.slice_width;

      msgResources->cap[group_idx][stage_idx].count.counters    = resources.cap[group_idx][stage_idx].count.counters;
      msgResources->cap[group_idx][stage_idx].count.meters      = resources.cap[group_idx][stage_idx].count.meters;
      msgResources->cap[group_idx][stage_idx].count.rules       = resources.cap[group_idx][stage_idx].count.rules;
      msgResources->cap[group_idx][stage_idx].count.slice_width = resources.cap[group_idx][stage_idx].count.slice_width;
    }
  }

  return L7_SUCCESS;
}


/* Physical Interfaces Functions **********************************************/
/**
 * Set physical port configuration
 * 
 * @param msgPhyConf Structure with the configuration to be set
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_PhyConfig_set(msg_HWEthPhyConf_t *msgPhyConf)
{
  ptin_HWEthPhyConf_t phyConf;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Port # %u",           msgPhyConf->Port);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Enable        = %u", msgPhyConf->PortEnable );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Speed         = %u", msgPhyConf->Speed );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Duplex        = %u", msgPhyConf->Duplex );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Media         = %u", msgPhyConf->Media );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " MaxFrame      = %u", msgPhyConf->MaxFrame );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Loopback      = %u", msgPhyConf->LoopBack );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " MACLearn Prio = %u", msgPhyConf->MacLearning );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Mask          = 0x%04X", msgPhyConf->Mask );

  /* Copy the message data to a new structure (*/
  phyConf.Port         = msgPhyConf->Port;
  phyConf.Mask         = msgPhyConf->Mask;
  phyConf.PortEnable   = msgPhyConf->PortEnable;
  phyConf.Speed        = msgPhyConf->Speed;
  phyConf.Duplex       = msgPhyConf->Duplex;
  phyConf.Media        = msgPhyConf->Media;
  phyConf.MaxFrame     = msgPhyConf->MaxFrame;
  phyConf.LoopBack     = msgPhyConf->LoopBack;

  /* Apply config */
  if ( ptin_intf_PhyConfig_set(&phyConf) != L7_SUCCESS )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error applying configurations on port# %u", phyConf.Port);

    memset(msgPhyConf, 0x00, sizeof(msg_HWEthPhyConf_t));
    msgPhyConf->Mask = phyConf.Mask;  /* Restore mask */
    msgPhyConf->Port = phyConf.Port;

    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/**
 * Get physical port configuration
 * 
 * @param msgPhyConf Structure to save port configuration (Port 
 * field MUST be set; Output mask bits reflect the updated fields)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_PhyConfig_get(msg_HWEthPhyConf_t *msgPhyConf)
{
  ptin_HWEthPhyConf_t phyConf;

  phyConf.Port = msgPhyConf->Port;
  phyConf.Mask = msgPhyConf->Mask;

  if (ptin_intf_PhyConfig_get(&phyConf) != L7_SUCCESS )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting configurations of port# %u", phyConf.Port);
    memset(msgPhyConf, 0x00, sizeof(msg_HWEthPhyConf_t));

    return L7_FAILURE;
  }

  /* Copy the message data to the msg structure (*/
  msgPhyConf->Port        = phyConf.Port;
  msgPhyConf->Mask        = phyConf.Mask;
  msgPhyConf->PortEnable  = phyConf.PortEnable;
  msgPhyConf->Speed       = phyConf.Speed;
  msgPhyConf->Duplex      = phyConf.Duplex;
  msgPhyConf->Media       = phyConf.Media;
  msgPhyConf->MaxFrame    = phyConf.MaxFrame;
  msgPhyConf->LoopBack    = phyConf.LoopBack;

  /* Output info read */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Port # %u",           msgPhyConf->Port);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Enable        = %u", msgPhyConf->PortEnable );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Speed         = %u", msgPhyConf->Speed );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Duplex        = %u", msgPhyConf->Duplex );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Media         = %u", msgPhyConf->Media );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " MaxFrame      = %u", msgPhyConf->MaxFrame );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Loopback      = %u", msgPhyConf->LoopBack );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Mask          = 0x%04X", msgPhyConf->Mask );

  return L7_SUCCESS;
}


/**
 * Get physical port state
 * 
 * @param msgPhyState Structure to save port state (Port 
 * field MUST be set; Outut mask bits reflect the updated fields)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_PhyState_get(msg_HWEthPhyState_t *msgPhyState)
{
  L7_uint port;
  ptin_HWEthPhyConf_t                 phyConf;
  ptin_HWEthPhyState_t                phyState;
  ptin_HWEthRFC2819_PortStatistics_t  portStats;

  port = msgPhyState->Port;

  /* Clear structure */
  memset(msgPhyState, 0x00, sizeof(msg_HWEthPhyState_t));
  msgPhyState->Port = port;

  /* Read some configurations: MaxFrame & Media */
  phyConf.Port = port;
  phyConf.Mask = 0xFFFF;
  if (ptin_intf_PhyConfig_get(&phyConf) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting configurations of port# %u", phyConf.Port);
    memset(msgPhyState, 0x00, sizeof(ptin_HWEthPhyState_t));

    return L7_FAILURE;
  }

  /* Read some state parameters: Link-Up and AutoNeg-Complete */
  phyState.Port = port;
  phyState.Mask = 0xFFFF;
  if (ptin_intf_PhyState_read(&phyState))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting state of port# %u", phyState.Port);
    memset(msgPhyState, 0x00, sizeof(msg_HWEthPhyState_t));

    return L7_FAILURE;
  }

  /* Read statistics */
  portStats.Port = port;
  portStats.Mask = 0xFF;
  portStats.RxMask = 0xFFFFFFFF;
  portStats.TxMask = 0xFFFFFFFF;
  if (ptin_intf_counters_read(&portStats) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting statistics of port# %u", portStats.Port);
    memset(msgPhyState, 0x00, sizeof(msg_HWEthPhyState_t));

    return L7_FAILURE;
  }

  /* Compose message with all the gathered data */
  msgPhyState->Mask               = 0x1C7F;   /* Do not include TxFault, RemoteFault and LOS */
  msgPhyState->Speed              = phyState.Speed;
  msgPhyState->Duplex             = phyState.Duplex;
  msgPhyState->LinkUp             = phyState.LinkUp;
  msgPhyState->AutoNegComplete    = phyState.AutoNegComplete;

  msgPhyState->Collisions         = portStats.Tx.etherStatsCollisions > 0;
  msgPhyState->RxActivity         = portStats.Rx.Throughput > 0;
  msgPhyState->TxActivity         = portStats.Tx.Throughput > 0;

  msgPhyState->Media              = phyConf.Media;
  msgPhyState->MTU_mismatch       = phyConf.MaxFrame > PHY_MAX_MAXFRAME;
  msgPhyState->Supported_MaxFrame = PHY_MAX_MAXFRAME;

//msgPhyState->TxFault            = 0;    /* Always FALSE */
//msgPhyState->RemoteFault        = 0;    /* Always FALSE */
//msgPhyState->LOS                = 0;    /* Always FALSE */

  /* Output info read */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Port # %u",                   msgPhyState->Port);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Mask             = 0x%04X",  msgPhyState->Mask );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Speed            = %u",      msgPhyState->Speed );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Duplex           = %u",      msgPhyState->Duplex );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " LinkUp           = %s",      msgPhyState->LinkUp?"Yes":"No" );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " AutoNegComplete  = %s",      msgPhyState->AutoNegComplete?"Yes":"No" );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Collisions       = %s",      msgPhyState->Collisions?"Yes":"No" );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " RxActivity       = %s",      msgPhyState->RxActivity?"Yes":"No" );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " TxActivity       = %s",      msgPhyState->TxActivity?"Yes":"No" );
//LOG_DEBUG(LOG_CTX_PTIN_MSG, " TxFault          = %s",      msgPhyState->TxFault?"Yes":"No" );
//LOG_DEBUG(LOG_CTX_PTIN_MSG, " RemoteFault      = %s",      msgPhyState->RemoteFault?"Yes":"No" );
//LOG_DEBUG(LOG_CTX_PTIN_MSG, " LOS              = %s",      msgPhyState->LOS?"Yes":"No" );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Media            = %u",      msgPhyState->Media );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " MTU_mismatch     = %s",      msgPhyState->MTU_mismatch?"Yes":"No" );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Support.MaxFrame = %u",      msgPhyState->Supported_MaxFrame );

  return L7_SUCCESS;
}


/**
 * Get physical port activity
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_PhyActivity_get(msg_HWEthPhyActivity_t *msgPhyAct)
{
#if (PTIN_BOARD_IS_MATRIX)
  L7_uint ptin_port;
  ptin_HWEthRFC2819_PortStatistics_t  portStats;

  /* Get ptin port */
  if (ptin_intf_slotPort2port(msgPhyAct->intf.slot, msgPhyAct->intf.port, &ptin_port) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unknown interface (slot=%u/%u)", msgPhyAct->intf.slot, msgPhyAct->intf.port);
    return L7_FAILURE;
  }

  /* Read statistics */
  portStats.Port = ptin_port;
  portStats.Mask = 0xFF;
  portStats.RxMask = 0xFFFFFFFF;
  portStats.TxMask = 0xFFFFFFFF;
  if (ptin_intf_counters_read(&portStats) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting statistics of port# %u", portStats.Port);
    return L7_FAILURE;
  }

  /* Compose message with all the gathered data */
  msgPhyAct->Mask = 0xff;
  msgPhyAct->RxActivity = (L7_uint32) portStats.Rx.Throughput;
  msgPhyAct->TxActivity = (L7_uint32) portStats.Tx.Throughput;

  /* Output info read */
  LOG_TRACE(LOG_CTX_PTIN_MSG, "Slot/Port # %u/%u",           msgPhyAct->intf.slot, msgPhyAct->intf.port);
  LOG_TRACE(LOG_CTX_PTIN_MSG, " Mask             = 0x%02x",  msgPhyAct->Mask );
  LOG_TRACE(LOG_CTX_PTIN_MSG, " RX Activity      = %u",      msgPhyAct->RxActivity );
  LOG_TRACE(LOG_CTX_PTIN_MSG, " TX Activity      = %u",      msgPhyAct->TxActivity );

  return L7_SUCCESS;
#else
  return L7_NOT_SUPPORTED;
#endif
}


/**
 * Get physical port state
 * 
 * @param msgPhyState Structure to save port state (Port 
 * field MUST be set; Outut mask bits reflect the updated fields)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_PhyStatus_get(msg_HWEthPhyStatus_t *msgPhyStatus)
{
  L7_uint port;
  ptin_HWEthPhyConf_t                 phyConf;
  ptin_HWEthPhyState_t                phyState;
  ptin_HWEthRFC2819_PortStatistics_t  portStats;

  port = msgPhyStatus->Port;

  /* Clear structure */
  memset(msgPhyStatus, 0x00, sizeof(msg_HWEthPhyStatus_t));
  msgPhyStatus->Port = port;

  /* Read some configurations: MaxFrame & Media */
  phyConf.Port = port;
  phyConf.Mask = 0xFFFF;
  if (ptin_intf_PhyConfig_get(&phyConf) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting configurations of port# %u", phyConf.Port);
    memset(msgPhyStatus, 0x00, sizeof(msg_HWEthPhyStatus_t));

    return L7_FAILURE;
  }

  /* Read statistics */
  portStats.Port = port;
  portStats.Mask = 0xFF;
  portStats.RxMask = 0xFFFFFFFF;
  portStats.TxMask = 0xFFFFFFFF;
  if (ptin_intf_counters_read(&portStats) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting statistics of port# %u", portStats.Port);
    memset(msgPhyStatus, 0x00, sizeof(msg_HWEthPhyStatus_t));

    return L7_FAILURE;
  }

  /* Read some state parameters: Link-Up and AutoNeg-Complete */
  phyState.Port = port;
  phyState.Mask = 0xFFFF;
  if (ptin_intf_PhyState_read(&phyState))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting state of port# %u", phyState.Port);
    memset(msgPhyStatus, 0x00, sizeof(msg_HWEthPhyStatus_t));

    return L7_FAILURE;
  }

  /* Compose message with all the gathered data */
  msgPhyStatus->phy.alarmes = 0;

  /* Acquired speed */
  if (phyState.Speed == PHY_PORT_100_MBPS)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_SPEED100_BIT;
  }
  if (phyState.Speed == PHY_PORT_1000_MBPS)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_SPEED1000_BIT;
  }
  if (phyState.Speed == PHY_PORT_10_GBPS)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_SPEED10G_BIT;
  }

  /* Traffic activity */
  if (portStats.Tx.Throughput > 0)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_TX_BIT;
  }
  if (portStats.Rx.Throughput > 0)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_RX_BIT;
  }
  if (portStats.Tx.etherStatsCollisions > 0)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_COLLISION_BIT;
  }

  /* Link down alarm  */
  if (!phyState.LinkUp)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_LINK_BIT;
  }

  /* Autoneg complete? */
  if (phyState.AutoNegComplete)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_AUTONEG_BIT;
  }

  /* Other parameters */
  if (phyState.Duplex)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_FULLDUPLEX_BIT;
  }
  if (phyConf.Media == PHY_PORT_MEDIA_OPTICAL)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_MEDIAX_BIT;
  }

  msgPhyStatus->phy.alarmes_mask =  HW_ETHERNET_STATUS_MASK_SPEED100_BIT | HW_ETHERNET_STATUS_MASK_TX_BIT | HW_ETHERNET_STATUS_MASK_RX_BIT | 
                                    HW_ETHERNET_STATUS_MASK_COLLISION_BIT | HW_ETHERNET_STATUS_MASK_LINK_BIT | /* HW_ETHERNET_STATUS_MASK_AUTONEG_BIT | */
                                    HW_ETHERNET_STATUS_MASK_FULLDUPLEX_BIT | HW_ETHERNET_STATUS_MASK_SPEED1000_BIT | HW_ETHERNET_STATUS_MASK_MEDIAX_BIT;

  /* Output info read */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Port # %u",                   msgPhyStatus->Port);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " alarmes          = 0x%04X",  msgPhyStatus->phy.alarmes );
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " alarmes_mask     = 0x%04X",  msgPhyStatus->phy.alarmes_mask );

  return L7_SUCCESS;
}


/* Phy Counters Functions *****************************************************/
/**
 * Read PHY counters
 * 
 * @param msgPortStats : Array of stats (one for each port) 
 * @param msgRequest   : Array of requests (one for each port) 
 * @param numElems     : Number of elements to be read
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_PhyCounters_read(msg_HwGenReq_t *msgRequest, msg_HWEthRFC2819_PortStatistics_t *msgPortStats, L7_uint nElems)
{
  L7_uint port, i;
  msg_HwGenReq_t                    *p_msgRequest;
  msg_HWEthRFC2819_PortStatistics_t *p_msgPortStats;
  ptin_HWEthRFC2819_PortStatistics_t portStats;

  for (i=0; i<nElems; i++)
  {
    p_msgRequest   = &msgRequest[i];    /* Pointer to request */
    p_msgPortStats = &msgPortStats[i];  /* Pointer to output */

    /* Port to be read */
    port = p_msgRequest->generic_id;

    /* Clear output structure */
    memset(p_msgPortStats, 0x00, sizeof(msg_HWEthRFC2819_PortStatistics_t));
    /* Update slot_id and port_id */
    p_msgPortStats->SlotId = p_msgRequest->slot_id;
    p_msgPortStats->Port   = port;

    /* Read statistics */
    portStats.Port = port;
    portStats.Mask = 0xFF;
    portStats.RxMask = 0xFFFFFFFF;
    portStats.TxMask = 0xFFFFFFFF;
    if (ptin_intf_counters_read(&portStats) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting statistics of port# %u", portStats.Port);
      memset(p_msgPortStats, 0x00, sizeof(msg_HWEthRFC2819_PortStatistics_t));
      return L7_FAILURE;
    }

    /* Copy data from ptin to msg structure */
    ptin_msg_PortStats_convert(p_msgPortStats, &portStats);

    /* Output info read */
    LOG_TRACE(LOG_CTX_PTIN_MSG, "Slotid=%u, Port # %2u", p_msgPortStats->SlotId, p_msgPortStats->Port);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.DropEvents           = %15llu  | Tx.DropEvents           = %15llu", p_msgPortStats->Rx.etherStatsDropEvents,           msgPortStats->Tx.etherStatsDropEvents);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.Octets               = %15llu  | Tx.Octets               = %15llu", p_msgPortStats->Rx.etherStatsOctets,               msgPortStats->Tx.etherStatsOctets);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.Pkts                 = %15llu  | Tx.Pkts                 = %15llu", p_msgPortStats->Rx.etherStatsPkts,                 msgPortStats->Tx.etherStatsPkts);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.BroadcastPkts        = %15llu  | Tx.BroadcastPkts        = %15llu", p_msgPortStats->Rx.etherStatsBroadcastPkts,        msgPortStats->Tx.etherStatsBroadcastPkts);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.MulticastPkts        = %15llu  | Tx.MulticastPkts        = %15llu", p_msgPortStats->Rx.etherStatsMulticastPkts,        msgPortStats->Tx.etherStatsMulticastPkts);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.CRCAlignErrors       = %15llu  | Tx.CRCAlignErrors       = %15llu", p_msgPortStats->Rx.etherStatsCRCAlignErrors,       msgPortStats->Tx.etherStatsCRCAlignErrors);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.UndersizePkts        = %15llu  | Tx.OversizePkts         = %15llu", p_msgPortStats->Rx.etherStatsUndersizePkts,        msgPortStats->Tx.etherStatsOversizePkts);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.OversizePkts         = %15llu  | Tx.Fragments            = %15llu", p_msgPortStats->Rx.etherStatsOversizePkts,         msgPortStats->Tx.etherStatsFragments);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.Fragments            = %15llu  | Tx.Jabbers              = %15llu", p_msgPortStats->Rx.etherStatsFragments,            msgPortStats->Tx.etherStatsJabbers);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.Jabbers              = %15llu  | Tx.Collisions           = %15llu", p_msgPortStats->Rx.etherStatsJabbers,              msgPortStats->Tx.etherStatsCollisions);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.Pkts64Octets         = %15llu  | Tx.Pkts64Octets         = %15llu", p_msgPortStats->Rx.etherStatsPkts64Octets,         msgPortStats->Tx.etherStatsPkts64Octets);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.Pkts65to127Octets    = %15llu  | Tx.Pkts65to127Octets    = %15llu", p_msgPortStats->Rx.etherStatsPkts65to127Octets,    msgPortStats->Tx.etherStatsPkts65to127Octets);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.Pkts128to255Octets   = %15llu  | Tx.Pkts128to255Octets   = %15llu", p_msgPortStats->Rx.etherStatsPkts128to255Octets,   msgPortStats->Tx.etherStatsPkts128to255Octets);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.Pkts256to511Octets   = %15llu  | Tx.Pkts256to511Octets   = %15llu", p_msgPortStats->Rx.etherStatsPkts256to511Octets,   msgPortStats->Tx.etherStatsPkts256to511Octets);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.Pkts512to1023Octets  = %15llu  | Tx.Pkts512to1023Octets  = %15llu", p_msgPortStats->Rx.etherStatsPkts512to1023Octets,  msgPortStats->Tx.etherStatsPkts512to1023Octets);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.Pkts1024to1518Octets = %15llu  | Tx.Pkts1024to1518Octets = %15llu", p_msgPortStats->Rx.etherStatsPkts1024to1518Octets, msgPortStats->Tx.etherStatsPkts1024to1518Octets);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.Pkts1519toMaxOctets  = %15llu  | Tx.Pkts1519toMaxOctets  = %15llu", p_msgPortStats->Rx.etherStatsPkts1519toMaxOctets,  msgPortStats->Tx.etherStatsPkts1519toMaxOctets);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " Rx.Throughput (bps)     = %15llu  | Tx.Throughput (bps)     = %15llu", p_msgPortStats->Rx.Throughput,                     msgPortStats->Tx.Throughput);
  }

  return L7_SUCCESS;
}


/**
 * Clear PHY counters
 * 
 * @param portStats portStats.Port must defined the port#
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_PhyCounters_clear(msg_HWEthRFC2819_PortStatistics_t *msgPortStats)
{
  ptin_HWEthRFC2819_PortStatistics_t portStats;

  /* Clear statistics */
  portStats.Port = msgPortStats->Port;
  portStats.Mask = msgPortStats->Mask;
  portStats.RxMask = msgPortStats->RxMask;
  portStats.TxMask = msgPortStats->TxMask;

  if (ptin_intf_counters_clear(&portStats) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error clearing statistics of port# %u", portStats.Port);
    memset(msgPortStats, 0x00, sizeof(msg_HWEthRFC2819_PortStatistics_t));

    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Port# %u counters cleared", portStats.Port);

  return L7_SUCCESS;
}


/* Slot mode configuration ****************************************************/

/**
 * Get all interfaces info
 * 
 * @param intf_info 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_intfInfo_get(msg_HwIntfInfo_t *intf_info)
{
  #if (PTIN_BOARD_IS_MATRIX)
  L7_int ptin_port;
  ptin_intf_t ptin_intf;
  L7_uint16 admin, link, board_id;

  /* Run all physical ports */
  for (ptin_port = 0; ptin_port < ptin_sys_number_of_ports; ptin_port++)
  {
    ptin_intf.intf_type = PTIN_EVC_INTF_PHYSICAL;
    ptin_intf.intf_id   = ptin_port;

    /* Get intf data */
    if (ptin_intf_info_get(&ptin_intf, &admin, &link, &board_id) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error reading info for ptin_port %d", ptin_port);
      return L7_FAILURE;
    }

    intf_info->port[ptin_port].board_id = board_id;
    intf_info->port[ptin_port].enable   = admin;
    intf_info->port[ptin_port].link     = link;

    LOG_TRACE(LOG_CTX_PTIN_MSG, "port=%u: boardId=%u admin=%u link=%u", ptin_port, board_id, admin, link);
  }
  /* Number of ports */
  intf_info->number_of_ports = ptin_sys_number_of_ports;

  #ifdef MAP_CPLD
  if (!ptin_fgpa_mx_is_matrixactive())
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "I am inactive matrix");
    return L7_FAILURE;
  }
  #endif
  #endif

  return L7_SUCCESS;
}

/**
 * Get slot mode configuration
 * 
 * @param slotMode 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_slotMode_get(msg_slotModeCfg_t *slotMode)
{
  L7_uint i;
  L7_uint32 slot_list[PTIN_SYS_SLOTS_MAX];

  memset(slotMode->slot_list, 0x00, sizeof(slotMode->slot_list));

  /* Get list of slot modes */
  if ( ptin_intf_slotMode_get(slot_list)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error obtaining list of slot modes");
    return L7_FAILURE;
  }

  /* Run all slots */
  for (i=0; i<MSG_SLOTMODECFG_NSLOTS && i<PTIN_SYS_SLOTS_MAX; i++)
  {
    slotMode->slot_list[i].slot_index  = i+1;
    slotMode->slot_list[i].slot_config = 1;
    slotMode->slot_list[i].slot_mode   = slot_list[i];
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Slot mode list:");
  for (i=0; i<PTIN_SYS_SLOTS_MAX; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Slot %02u: Mode=%u", i+1, slot_list[i]);
  }
  LOG_INFO(LOG_CTX_PTIN_MSG,"Success");

  return L7_SUCCESS;
}

/**
 * Validate slot mode configuration
 * 
 * @param slotMode 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_slotMode_validate(msg_slotModeCfg_t *slotMode)
{
  L7_uint i, slot, mode;
  L7_uint32 slot_list[PTIN_SYS_SLOTS_MAX];
  L7_RC_t rc = L7_SUCCESS;

  memset(slot_list, 0x00, sizeof(slot_list));

  /* Run all slots */
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Slot mode list:");
  for (i=0; i<MSG_SLOTMODECFG_NSLOTS; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Idx%02u: Slot %02u Active=%u Mode=%u", i,
              slotMode->slot_list[i].slot_index, slotMode->slot_list[i].slot_config, slotMode->slot_list[i].slot_mode);

    /* Valid slot? */
    if (!slotMode->slot_list[i].slot_config)
      continue;

    slot = slotMode->slot_list[i].slot_index;
    mode = slotMode->slot_list[i].slot_mode;
    /* Validate slot */
    if (slot>=PTIN_SYS_LC_SLOT_MIN && slot<=PTIN_SYS_LC_SLOT_MAX)
    {
      slot_list[slot-1] = mode;
    }
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Slot mode list to be validated:");
  for (i=0; i<PTIN_SYS_SLOTS_MAX; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Slot %02u: Mode=%u", i+1, slot_list[i]);
  }

  /* Validate list of slot modes */
  if ((rc=ptin_intf_slotMode_validate(slot_list)) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Slot mode list is not valid! (rc=%d)",rc);
    return rc;
  }
  LOG_INFO(LOG_CTX_PTIN_MSG, "Slot mode list is valid!");

  return L7_SUCCESS;
}

/**
 * Apply new slot mode configuration by rebboting application
 *  
 * @param slotMode
 *  
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_slotMode_apply(void)
{
  LOG_INFO(LOG_CTX_PTIN_MSG,"Success");

  return L7_SUCCESS;
}

/* Port Type Functions ********************************************************/ 

/**
 * Define Port Type settings
 * 
 * @param mefExt : Mef Extension definitions
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_portExt_set(msg_HWPortExt_t *portExt, L7_uint nElems)
{
  L7_uint          i;
  ptin_intf_t      ptin_intf;
  ptin_HWPortExt_t portExt_conf;

  for (i=0; i<nElems; i++)
  {
    memset(&portExt_conf,0x00,sizeof(ptin_HWPortExt_t));
    portExt_conf.Mask                          = portExt[i].Mask;
    portExt_conf.defVid                        = portExt[i].defVid;
    portExt_conf.defPrio                       = portExt[i].defPrio;
    portExt_conf.acceptable_frame_types        = portExt[i].acceptable_frame_types;
    portExt_conf.ingress_filter                = portExt[i].ingress_filter;
    portExt_conf.restricted_vlan_reg           = portExt[i].restricted_vlan_reg;
    portExt_conf.vlan_aware                    = portExt[i].vlan_aware;
    portExt_conf.type                          = portExt[i].type;
    portExt_conf.doubletag                     = portExt[i].doubletag;
    portExt_conf.inner_tpid                    = portExt[i].inner_tpid;
    portExt_conf.outer_tpid                    = portExt[i].outer_tpid;
    portExt_conf.egress_type                   = portExt[i].egress_type;
    portExt_conf.macLearn_enable               = portExt[i].macLearn_enable;
    portExt_conf.macLearn_stationMove_enable   = portExt[i].macLearn_stationMove_enable;
    portExt_conf.macLearn_stationMove_prio     = portExt[i].macLearn_stationMove_prio;
    portExt_conf.macLearn_stationMove_samePrio = portExt[i].macLearn_stationMove_samePrio;
    portExt_conf.maxChannels                   = portExt[i].maxChannels;
    portExt_conf.maxBandwidth                  = portExt[i].maxBandwidth;
    portExt_conf.dhcp_trusted                  = portExt[i].protocol_trusted;

    ptin_intf.intf_type = portExt[i].intf.intf_type;
    ptin_intf.intf_id   = portExt[i].intf.intf_id;

    /* Set MEF parameters */
    if (ptin_intf_portExt_set(&ptin_intf, &portExt_conf)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error setting MEF EXT configurations");
      return L7_FAILURE;
    }
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success setting MEF EXT configurations",__FUNCTION__);

  return L7_SUCCESS;
}

/**
 * Read Port Type settings
 * 
 * @param mefExt : Mef Extension definitions
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_portExt_get(msg_HWPortExt_t *portExt, L7_uint *nElems)
{
  L7_uint8        slotId;
  L7_uint         index, port, port_start, port_end;
  ptin_intf_t     ptin_intf;
  ptin_HWPortExt_t portExt_conf;

  slotId              = portExt->SlotId;
  ptin_intf.intf_type = portExt->intf.intf_type;
  ptin_intf.intf_id   = portExt->intf.intf_id;

  if (portExt->intf.intf_type==0xff)
  {
    port_start = 0;
    port_end   = PTIN_SYSTEM_N_INTERF-1;
  }
  else if (portExt->intf.intf_type==PTIN_EVC_INTF_PHYSICAL && portExt->intf.intf_id==0xff)
  {
    port_start = 0;
    port_end   = PTIN_SYSTEM_N_PORTS-1;
  }
  else if (portExt->intf.intf_type==PTIN_EVC_INTF_LOGICAL && portExt->intf.intf_id==0xff)
  {
    port_start = PTIN_SYSTEM_N_PORTS;
    port_end   = PTIN_SYSTEM_N_INTERF-1;
  }
  else if (ptin_intf_ptintf2port(&ptin_intf,&port)==L7_SUCCESS)
  {
    port_start = port;
    port_end   = port;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error reading interface %u/%u",ptin_intf.intf_type,ptin_intf.intf_id);
    return L7_FAILURE;
  }

  index = 0;
  for (port=port_start; port<=port_end; port++)
  {
    if (ptin_intf_port2ptintf(port,&ptin_intf)!=L7_SUCCESS)
    {
      LOG_WARNING(LOG_CTX_PTIN_MSG,"Error reading port %u",port);
      continue;
    }

    memset(&portExt_conf,0x00,sizeof(ptin_HWPortExt_t));

    /* Get MEF parameters */
    if (ptin_intf_portExt_get(&ptin_intf, &portExt_conf)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error getting MEF EXT parameters for interface=%u/%u",ptin_intf.intf_type,ptin_intf.intf_id);
      return L7_FAILURE;
    }

    /* Copy values to output */
    portExt[index].SlotId                        = slotId;
    portExt[index].intf.intf_type                = ptin_intf.intf_type;
    portExt[index].intf.intf_id                  = ptin_intf.intf_id;
    portExt[index].Mask                          = portExt_conf.Mask;
    portExt[index].defVid                        = portExt_conf.defVid;
    portExt[index].defPrio                       = portExt_conf.defPrio;
    portExt[index].acceptable_frame_types        = portExt_conf.acceptable_frame_types;
    portExt[index].ingress_filter                = portExt_conf.ingress_filter;
    portExt[index].restricted_vlan_reg           = portExt_conf.restricted_vlan_reg;
    portExt[index].vlan_aware                    = portExt_conf.vlan_aware;
    portExt[index].type                          = portExt_conf.type;
    portExt[index].doubletag                     = portExt_conf.doubletag;
    portExt[index].inner_tpid                    = portExt_conf.inner_tpid;
    portExt[index].outer_tpid                    = portExt_conf.outer_tpid;
    portExt[index].egress_type                   = portExt_conf.egress_type;
    portExt[index].macLearn_enable               = portExt_conf.macLearn_enable;
    portExt[index].macLearn_stationMove_enable   = portExt_conf.macLearn_stationMove_enable;
    portExt[index].macLearn_stationMove_prio     = portExt_conf.macLearn_stationMove_prio;
    portExt[index].macLearn_stationMove_samePrio = portExt_conf.macLearn_stationMove_samePrio;
    portExt[index].maxChannels                   = portExt_conf.maxChannels;
    portExt[index].maxBandwidth                  = portExt_conf.maxBandwidth;
    portExt[index].protocol_trusted              = portExt_conf.dhcp_trusted;

    index++;
  }

  if (nElems!=L7_NULLPTR)  *nElems = index;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success reading MEF EXT parameters for %u interfaces",index);

  return L7_SUCCESS;
}

/**
 * Set MAC address to interfaces
 * 
 * @param portMac : MAC definitions 
 * @param nElems  : Number of MACs to be attributed
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_portMAC_set(msg_HWPortMac_t *portMac, L7_uint nElems)
{
  L7_uint          i;
  ptin_intf_t      ptin_intf;
  ptin_HWPortMac_t portMac_conf;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Processing %u structures",nElems);

  for (i=0; i<nElems; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Structure %u",i);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," SlotId = %u",portMac[i].SlotId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Intf   = %u/%u",portMac[i].intf.intf_type,portMac[i].intf.intf_id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Mask   = 0x%04x",portMac[i].Mask);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," MAC    = %02x:%02x:%02x:%02x:%02x:%02x",
              portMac[i].macAddr[0],portMac[i].macAddr[1],portMac[i].macAddr[2],portMac[i].macAddr[3],portMac[i].macAddr[4],portMac[i].macAddr[5]);

    memset(&portMac_conf,0x00,sizeof(ptin_HWPortMac_t));
    portMac_conf.Mask = portMac[i].Mask;
    memcpy(portMac_conf.macAddr, portMac[i].macAddr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);

    ptin_intf.intf_type = portMac[i].intf.intf_type;
    ptin_intf.intf_id   = portMac[i].intf.intf_id;

    /* Set MEF parameters */
    if (ptin_intf_portMAC_set(&ptin_intf, &portMac_conf)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error setting MAC address");
      return L7_FAILURE;
    }
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success setting MAC address",__FUNCTION__);

  return L7_SUCCESS;
}

/**
 * Get MAC addresses for the given interfaces
 * 
 * @param portMac : MAC definitions 
 * @param nElems  : Number of MACs to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_portMAC_get(msg_HWPortMac_t *portMac, L7_uint *nElems)
{
  L7_uint8         slotId;
  L7_uint          index, port, port_start, port_end;
  ptin_intf_t      ptin_intf;
  ptin_HWPortMac_t portMac_conf;

  slotId = portMac->SlotId;
  ptin_intf.intf_type = portMac->intf.intf_type;
  ptin_intf.intf_id   = portMac->intf.intf_id;

  if (portMac->intf.intf_type==0xff)
  {
    port_start = 0;
    port_end   = PTIN_SYSTEM_N_INTERF-1;
  }
  else if (portMac->intf.intf_type==PTIN_EVC_INTF_PHYSICAL && portMac->intf.intf_id==0xff)
  {
    port_start = 0;
    port_end   = PTIN_SYSTEM_N_PORTS-1;
  }
  else if (portMac->intf.intf_type==PTIN_EVC_INTF_LOGICAL && portMac->intf.intf_id==0xff)
  {
    port_start = PTIN_SYSTEM_N_PORTS;
    port_end   = PTIN_SYSTEM_N_INTERF-1;
  }
  else if (ptin_intf_ptintf2port(&ptin_intf,&port)==L7_SUCCESS)
  {
    port_start = port;
    port_end   = port;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error reading interface %u/%u",ptin_intf.intf_type,ptin_intf.intf_id);
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Going to read %u structures",port_end-port_start+1);

  index = 0;
  for (port=port_start; port<=port_end; port++)
  {
    if (ptin_intf_port2ptintf(port,&ptin_intf)!=L7_SUCCESS)
    {
      LOG_WARNING(LOG_CTX_PTIN_MSG,"Error reading port %u",port);
      continue;
    }

    memset(&portMac_conf,0x00,sizeof(ptin_HWPortMac_t));
    portMac_conf.Mask = PTIN_HWPORTMAC_MASK_MACADDR;

    /* Get MEF parameters */
    if (ptin_intf_portMAC_get(&ptin_intf, &portMac_conf)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error getting MAC address for interface=%u/%u",ptin_intf.intf_type,ptin_intf.intf_id);
      return L7_FAILURE;
    }

    /* Copy values to output */
    portMac[index].SlotId                      = slotId;
    portMac[index].intf.intf_type              = ptin_intf.intf_type;
    portMac[index].intf.intf_id                = ptin_intf.intf_id;
    portMac[index].Mask                        = portMac_conf.Mask;
    memcpy(portMac[index].macAddr,portMac_conf.macAddr,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);

    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Structure %u",index);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," SlotId = %u",portMac[index].SlotId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Intf   = %u/%u",portMac[index].intf.intf_type,portMac[index].intf.intf_id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Mask   = 0x%04x",portMac[index].Mask);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," MAC    = %02x:%02x:%02x:%02x:%02x:%02x",
              portMac[index].macAddr[0],portMac[index].macAddr[1],portMac[index].macAddr[2],portMac[index].macAddr[3],portMac[index].macAddr[4],portMac[index].macAddr[5]);

    index++;
  }

  if (nElems!=L7_NULLPTR)  *nElems = index;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success reading MEF EXT parameters for %u interfaces",index);

  return L7_SUCCESS;
}

/* CoS Functions *************************************************************/ 

/**
 * Get CoS configuration
 * 
 * @param qos_config : CoS configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_CoS_get(msg_QoSConfiguration_t *qos_msg)
{
  L7_int i;
  L7_uint8                slot_id;
  ptin_intf_t             ptin_intf;
  ptin_QoS_intf_t         qos_intf;
  ptin_QoS_cos_t          qos_cos[8];

  /* Save slot id and interface */
  slot_id = qos_msg->SlotId;
  ptin_intf.intf_type = qos_msg->intf.intf_type;
  ptin_intf.intf_id   = qos_msg->intf.intf_id;

  /* Clear and initialize message structure */
  memset(qos_msg,0x00,sizeof(msg_QoSConfiguration_t));
  qos_msg->SlotId = slot_id;
  qos_msg->intf.intf_type = ptin_intf.intf_type;
  qos_msg->intf.intf_id   = ptin_intf.intf_id;

  /* Clear temporary structures */
  memset(&qos_intf,0x00,sizeof(ptin_QoS_intf_t));
  memset(qos_cos,0x00,sizeof(ptin_QoS_cos_t)*8);

  /* Get Interface configuration */
  if (ptin_QoS_intf_config_get(&ptin_intf,&qos_intf)==L7_SUCCESS)
  {
    /* Trust mode */
    if (qos_intf.mask & PTIN_QOS_INTF_TRUSTMODE_MASK)
    {
      qos_msg->trust_mode = qos_intf.trust_mode;
      qos_msg->mask |= MSG_QOS_CONFIGURATION_TRUSTMODE_MASK;
    }
    /* Bandwidth units */
    if (qos_intf.mask & PTIN_QOS_INTF_BANDWIDTHUNIT_MASK)
    {
      qos_msg->bandwidth_unit = qos_intf.bandwidth_unit;
      qos_msg->mask |= MSG_QOS_CONFIGURATION_BANDWIDTHUNIT_MASK;
    }
    /* Shaping rate */
    if (qos_intf.mask & PTIN_QOS_INTF_SHAPINGRATE_MASK)
    {
      qos_msg->shaping_rate = qos_intf.shaping_rate;
      qos_msg->mask |= MSG_QOS_CONFIGURATION_SHAPINGRATE_MASK;
    }
    /* Priority map to CoS */
    if (qos_intf.mask & PTIN_QOS_INTF_PACKETPRIO_MASK)
    {
      /* Run all priorities */
      for (i=0; i<8; i++)
      {
        /* Is priority i to be defined? If so define CoS */
        if ((qos_intf.pktprio.mask >> i) & 1)
        {
          qos_msg->pktprio.mask  |= (L7_uint8) 1<<i;
          qos_msg->pktprio.cos[i] = qos_intf.pktprio.cos[i];
        }
      }
      if (qos_msg->pktprio.mask)
      {
        qos_msg->mask |= MSG_QOS_CONFIGURATION_PACKETPRIO_MASK;
      }
    }
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error reading interface QoS configuration");
    return L7_FAILURE;
  }

  /* QoS configuration */
  if (ptin_QoS_cos_config_get(&ptin_intf,(L7_uint8)-1,qos_cos)==L7_SUCCESS)
  {
    /* Run all QoS */
    for (i=0; i<8; i++)
    {
      /* QoS i is to be configured? */
      if (qos_cos[i].mask==0)  continue;

      qos_msg->cos_config.mask |= (L7_uint8) 1<<i;

      /* Scheduler type */
      if (qos_cos[i].mask & PTIN_QOS_COS_SCHEDULER_MASK)
      {
        qos_msg->cos_config.cos[i].scheduler = qos_cos[i].scheduler_type;
        qos_msg->cos_config.cos[i].mask |= MSG_QOS_CONFIGURATION_QOSCONF_SCHEDULER_MASK;
      }
      /* Minimum mandwidth */
      if (qos_cos[i].mask & PTIN_QOS_COS_BW_MIN_MASK)
      {
        qos_msg->cos_config.cos[i].min_bandwidth = qos_cos[i].min_bandwidth;
        qos_msg->cos_config.cos[i].mask |= MSG_QOS_CONFIGURATION_QOSCONF_BW_MIN_MASK;
      }
      /* Maximum bandwidth */
      if (qos_cos[i].mask & PTIN_QOS_COS_BW_MAX_MASK)
      {
        qos_msg->cos_config.cos[i].max_bandwidth = qos_cos[i].max_bandwidth;
        qos_msg->cos_config.cos[i].mask |= MSG_QOS_CONFIGURATION_QOSCONF_BW_MAX_MASK;
      }
    }
    if (qos_msg->cos_config.mask)
    {
      qos_msg->mask |= MSG_QOS_CONFIGURATION_QOSCONF_MASK;
    }
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error reading QoS configuration for all 8 CoS");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Slotid         = %u",qos_msg->SlotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Interface      = %u/%u",qos_msg->intf.intf_type,qos_msg->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "General Mask   = 0x%02X",qos_msg->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Trust mode     = %u",qos_msg->trust_mode);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Bandwidth unit = %u",qos_msg->bandwidth_unit);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Shaping rate   = %u",qos_msg->shaping_rate);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "pktprio.cos    = [ %Xh %Xh %Xh %Xh %Xh %Xh %Xh %Xh ]",
            qos_msg->pktprio.cos[0],qos_msg->pktprio.cos[1],qos_msg->pktprio.cos[2],qos_msg->pktprio.cos[3],qos_msg->pktprio.cos[4],qos_msg->pktprio.cos[5],qos_msg->pktprio.cos[6],qos_msg->pktprio.cos[7]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Priority Mask  = 0x%02X",qos_msg->pktprio.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "CoS Mask       = 0x%02X",qos_msg->cos_config.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "CoS Spec. Mask = [ 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X ]",
            qos_msg->cos_config.cos[0].mask,qos_msg->cos_config.cos[1].mask,qos_msg->cos_config.cos[2].mask,qos_msg->cos_config.cos[3].mask,qos_msg->cos_config.cos[4].mask,qos_msg->cos_config.cos[5].mask,qos_msg->cos_config.cos[6].mask,qos_msg->cos_config.cos[7].mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Queue_scheduler = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].scheduler, qos_msg->cos_config.cos[1].scheduler, qos_msg->cos_config.cos[2].scheduler, qos_msg->cos_config.cos[3].scheduler, qos_msg->cos_config.cos[4].scheduler, qos_msg->cos_config.cos[5].scheduler, qos_msg->cos_config.cos[6].scheduler, qos_msg->cos_config.cos[7].scheduler);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Queue_min_bw    = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].min_bandwidth, qos_msg->cos_config.cos[1].min_bandwidth, qos_msg->cos_config.cos[2].min_bandwidth, qos_msg->cos_config.cos[3].min_bandwidth, qos_msg->cos_config.cos[4].min_bandwidth, qos_msg->cos_config.cos[5].min_bandwidth, qos_msg->cos_config.cos[6].min_bandwidth, qos_msg->cos_config.cos[7].min_bandwidth);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Queue_max_bw    = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].max_bandwidth, qos_msg->cos_config.cos[1].max_bandwidth, qos_msg->cos_config.cos[2].max_bandwidth, qos_msg->cos_config.cos[3].max_bandwidth, qos_msg->cos_config.cos[4].max_bandwidth, qos_msg->cos_config.cos[5].max_bandwidth, qos_msg->cos_config.cos[6].max_bandwidth, qos_msg->cos_config.cos[7].max_bandwidth);

  return L7_SUCCESS;
}

/**
 * Redefine CoS configuration
 * 
 * @param qos_config : CoS configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_CoS_set(msg_QoSConfiguration_t *qos_msg)
{
  L7_int i;
  ptin_intf_t             ptin_intf;
  ptin_QoS_intf_t         qos_intf;
  ptin_QoS_cos_t          qos_cos[8];
  L7_RC_t                 rc = L7_SUCCESS;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Slotid         = %u",qos_msg->SlotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Interface      = %u/%u",qos_msg->intf.intf_type,qos_msg->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "General Mask   = 0x%02X",qos_msg->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Trust mode     = %u",qos_msg->trust_mode);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Bandwidth unit = %u",qos_msg->bandwidth_unit);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Shaping rate   = %u",qos_msg->shaping_rate);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "pktprio.cos    = [ %Xh %Xh %Xh %Xh %Xh %Xh %Xh %Xh ]",
            qos_msg->pktprio.cos[0],qos_msg->pktprio.cos[1],qos_msg->pktprio.cos[2],qos_msg->pktprio.cos[3],qos_msg->pktprio.cos[4],qos_msg->pktprio.cos[5],qos_msg->pktprio.cos[6],qos_msg->pktprio.cos[7]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Priority Mask  = 0x%02X",qos_msg->pktprio.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "CoS Mask       = 0x%02X",qos_msg->cos_config.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "CoS Spec. Mask = [ 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X ]",
            qos_msg->cos_config.cos[0].mask,qos_msg->cos_config.cos[1].mask,qos_msg->cos_config.cos[2].mask,qos_msg->cos_config.cos[3].mask,qos_msg->cos_config.cos[4].mask,qos_msg->cos_config.cos[5].mask,qos_msg->cos_config.cos[6].mask,qos_msg->cos_config.cos[7].mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Queue_scheduler = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].scheduler, qos_msg->cos_config.cos[1].scheduler, qos_msg->cos_config.cos[2].scheduler, qos_msg->cos_config.cos[3].scheduler, qos_msg->cos_config.cos[4].scheduler, qos_msg->cos_config.cos[5].scheduler, qos_msg->cos_config.cos[6].scheduler, qos_msg->cos_config.cos[7].scheduler);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Queue_min_bw    = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].min_bandwidth, qos_msg->cos_config.cos[1].min_bandwidth, qos_msg->cos_config.cos[2].min_bandwidth, qos_msg->cos_config.cos[3].min_bandwidth, qos_msg->cos_config.cos[4].min_bandwidth, qos_msg->cos_config.cos[5].min_bandwidth, qos_msg->cos_config.cos[6].min_bandwidth, qos_msg->cos_config.cos[7].min_bandwidth);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Queue_max_bw    = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].max_bandwidth, qos_msg->cos_config.cos[1].max_bandwidth, qos_msg->cos_config.cos[2].max_bandwidth, qos_msg->cos_config.cos[3].max_bandwidth, qos_msg->cos_config.cos[4].max_bandwidth, qos_msg->cos_config.cos[5].max_bandwidth, qos_msg->cos_config.cos[6].max_bandwidth, qos_msg->cos_config.cos[7].max_bandwidth);

  /* Clear structures */
  memset(&qos_intf,0x00,sizeof(ptin_QoS_intf_t));
  memset(qos_cos,0x00,sizeof(ptin_QoS_cos_t)*8);

  /* Interface */
  ptin_intf.intf_type = qos_msg->intf.intf_type;
  ptin_intf.intf_id   = qos_msg->intf.intf_id;

  /* Interface configuration */

  /* Trust mode */
  if (qos_msg->mask & MSG_QOS_CONFIGURATION_TRUSTMODE_MASK)
  {
    qos_intf.trust_mode = qos_msg->trust_mode;
    qos_intf.mask |= PTIN_QOS_INTF_TRUSTMODE_MASK;
  }
  /* Bandwidth units */
  if (qos_msg->mask & MSG_QOS_CONFIGURATION_BANDWIDTHUNIT_MASK)
  {
    qos_intf.bandwidth_unit = qos_msg->bandwidth_unit;
    qos_intf.mask |= PTIN_QOS_INTF_BANDWIDTHUNIT_MASK;
  }
  /* Shaping rate */
  if (qos_msg->mask & MSG_QOS_CONFIGURATION_SHAPINGRATE_MASK)
  {
    qos_intf.shaping_rate = qos_msg->shaping_rate;
    qos_intf.mask |= PTIN_QOS_INTF_SHAPINGRATE_MASK;
  }
  /* Priority map to CoS */
  if (qos_msg->mask & MSG_QOS_CONFIGURATION_PACKETPRIO_MASK)
  {
    /* Run all priorities */
    for (i=0; i<8; i++)
    {
      /* Is priority i to be defined? If so define CoS */
      if ((qos_msg->pktprio.mask>>i) & 1)
      {
        qos_intf.pktprio.mask  |= 1<<i;
        qos_intf.pktprio.cos[i] = qos_msg->pktprio.cos[i];
      }
    }
    if (qos_intf.pktprio.mask)
    {
      qos_intf.mask |= PTIN_QOS_INTF_PACKETPRIO_MASK;
    }
  }

  /* Is there any configuration to be applied? */
  if (qos_intf.mask)
  {
    /* Execute priority map configuration */
    if (ptin_QoS_intf_config_set(&ptin_intf,&qos_intf)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error configuring priority map");
      rc = L7_FAILURE;
    }
  }

  /* QoS configuration */
  if (qos_msg->mask & MSG_QOS_CONFIGURATION_QOSCONF_MASK)
  {
    /* Run all QoS */
    for (i=0; i<8; i++)
    {
      /* QoS i is to be configured? */
      if ( !((qos_msg->cos_config.mask>>i) & 1) )  continue;

      /* Scheduler type */
      if (qos_msg->cos_config.cos[i].mask & MSG_QOS_CONFIGURATION_QOSCONF_SCHEDULER_MASK)
      {
        qos_cos[i].scheduler_type = qos_msg->cos_config.cos[i].scheduler;
        qos_cos[i].mask |= PTIN_QOS_COS_SCHEDULER_MASK;
      }
      /* Minimum mandwidth */
      if (qos_msg->cos_config.cos[i].mask & MSG_QOS_CONFIGURATION_QOSCONF_BW_MIN_MASK)
      {
        qos_cos[i].min_bandwidth = qos_msg->cos_config.cos[i].min_bandwidth;
        qos_cos[i].mask |= PTIN_QOS_COS_BW_MIN_MASK;
      }
      /* Maximum bandwidth */
      if (qos_msg->cos_config.cos[i].mask & MSG_QOS_CONFIGURATION_QOSCONF_BW_MAX_MASK)
      {
        qos_cos[i].max_bandwidth = qos_msg->cos_config.cos[i].max_bandwidth;
        qos_cos[i].mask |= PTIN_QOS_COS_BW_MAX_MASK;
      }
    }

    /* Is there any configuration to be applied? */
    if (qos_msg->cos_config.mask)
    {
      /* Apply configuration */
      if (ptin_QoS_cos_config_set(&ptin_intf, (L7_uint8)-1, qos_cos)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG,"Error configuring QoS %d",i);
        rc =  L7_FAILURE;
      }
    }
  }

  if (rc==L7_SUCCESS)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Success applying QoS configurations to all CoS");
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error applying QoS configurations to all CoS");
  }

  return rc;
}


/* LAGs Manipulation Functions ************************************************/
/**
 * Gets one or all LAGs info
 * 
 * @param lagInfo Pointer to the output structure (or array)
 * @param nElems  Number of elements returned (array of structures)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_Lag_get(msg_LACPLagInfo_t *lagInfo, L7_uint *nElems)
{
  ptin_LACPLagConfig_t ptinLagConf;
  L7_uint32 lag_idx;
  L7_uint32 start, end, array_idx;
  L7_uint8  slot_id;

  *nElems = 0;

  slot_id = lagInfo->SlotId;

  /* Determine loop range (LAG index [0..PTIN_SYSTEM_N_LAGS[) */
  if (lagInfo->id >= PTIN_SYSTEM_N_LAGS)
  {
    start = 0;
    end   = PTIN_SYSTEM_N_LAGS-1;
  }
  else
  {
    start = lagInfo->id;
    end   = lagInfo->id;
  }

  /* Loop that works for one LAG or for all of them */
  for (lag_idx=start, array_idx=0; lag_idx<=end; lag_idx++)
  {
    if (!ptin_intf_lag_exists(lag_idx))
      continue;

    ptinLagConf.lagId = lag_idx;

    if (ptin_intf_LagConfig_get(&ptinLagConf) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error reading LAG# %u", lag_idx);
      return L7_FAILURE;
    }

    /* Copy data to the output message structure */
    lagInfo[array_idx].SlotId           = slot_id;
    lagInfo[array_idx].id               = ptinLagConf.lagId;
    lagInfo[array_idx].admin            = ptinLagConf.admin;
    lagInfo[array_idx].static_enable    = ptinLagConf.static_enable;
    lagInfo[array_idx].stp_enable       = ptinLagConf.stp_enable;
    lagInfo[array_idx].loadBalance_mode = ptinLagConf.loadBalance_mode;
    lagInfo[array_idx].members_pbmp32[0]= (L7_uint32) ptinLagConf.members_pbmp64;
    lagInfo[array_idx].members_pbmp32[1]= (L7_uint32) (ptinLagConf.members_pbmp64 >> 32);

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "LAG# %2u", (L7_uint32)lagInfo[array_idx].id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .admin            = %u", lagInfo[array_idx].admin);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .static_enable    = %u", lagInfo[array_idx].static_enable);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .stp_enable       = %u", lagInfo[array_idx].stp_enable);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .loadBalance_mode = %u", lagInfo[array_idx].loadBalance_mode);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .members_pbmp[0]  = 0x%08X", lagInfo[array_idx].members_pbmp32[0]);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .members_pbmp[1]  = 0x%08X", lagInfo[array_idx].members_pbmp32[1]);

    array_idx++;
  }

  *nElems = array_idx;
  return L7_SUCCESS;
}

/**
 * Creates a LAG
 * 
 * @param lagInfo Pointer to the structure with LAG info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_Lag_create(msg_LACPLagInfo_t *lagInfo)
{
  ptin_LACPLagConfig_t ptinLagConf;

  /* Only for TA48GE boards */
#if ( PTIN_BOARD == PTIN_BOARD_TA48GE )
  /* For iLag 0 allow just initialization, otherwise it'll mess with protection. Only monitor_matrix_commutation() can touch it. */
  if (0==lagInfo->id)
  {
    if (ptin_intf_lag_exists(lagInfo->id)) return L7_SUCCESS;
    ptinLagConf.stp_enable       = 0;//DISABLED;
    ptinLagConf.members_pbmp64   = 1ULL<<(PTIN_SYSTEM_N_ETH+1) |   1ULL<<PTIN_SYSTEM_N_ETH;;
  }
  else
#endif
  {
   ptinLagConf.stp_enable       = lagInfo->stp_enable;
   ptinLagConf.members_pbmp64   = (L7_uint64)lagInfo->members_pbmp32[0];
   ptinLagConf.members_pbmp64  |= ((L7_uint64)lagInfo->members_pbmp32[1]) << 32;
  }

  /* Copy data from msg to ptin structure */
  ptinLagConf.lagId            = (L7_uint32) lagInfo->id;
  ptinLagConf.admin            = lagInfo->admin;
  ptinLagConf.static_enable    = lagInfo->static_enable;
  //ptinLagConf.stp_enable       = lagInfo->stp_enable;
  ptinLagConf.loadBalance_mode = lagInfo->loadBalance_mode;
  //ptinLagConf.members_pbmp64   = (L7_uint64)lagInfo->members_pbmp32[0];
  //ptinLagConf.members_pbmp64  |= ((L7_uint64)lagInfo->members_pbmp32[1]) << 32;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "LAG# %2u",                    (L7_uint32)lagInfo->id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .admin            = %u",     lagInfo->admin);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .static_enable    = %u",     lagInfo->static_enable);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .stp_enable       = %u",     lagInfo->stp_enable);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .loadBalance_mode = %u",     lagInfo->loadBalance_mode);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .members_pbmp[0]  = 0x%08X", lagInfo->members_pbmp32[0]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .members_pbmp[1]  = 0x%08X", lagInfo->members_pbmp32[1]);

  if (ptin_intf_Lag_create(&ptinLagConf) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Failed to create LAG# %u", (L7_uint32)lagInfo->id);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Deletes a LAG
 * 
 * @param lagInfo Structure that references the LAG to destroy
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_Lag_destroy(msg_LACPLagInfo_t *lagInfo)
{
  ptin_LACPLagConfig_t ptinLagConf;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "LAG# %2u", (L7_uint32) lagInfo->id);

  ptinLagConf.lagId = (L7_uint32) lagInfo->id;

  if (ptin_intf_Lag_delete(&ptinLagConf) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Failed to delete LAG# %u", (L7_uint32)lagInfo->id);
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "LAG# %u: successfully deleted", (L7_uint32)lagInfo->id);

  return L7_SUCCESS;
}

/**
 * Gets one or all LAGs status
 * 
 * @param lagStatus Pointer to the output structure (or array)
 * @param nElems    Number of elements returned (array of structures)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_LagStatus_get(msg_LACPLagStatus_t *lagStatus, L7_uint *nElems)
{
  ptin_LACPLagStatus_t ptinLagStatus;
  L7_uint32 lag_idx;
  L7_uint32 start, end, array_idx;
  L7_uint8  slot_id;

  *nElems = 0;

  slot_id = lagStatus->SlotId;

  /* Determine loop range (LAG index [1..PTIN_SYSTEM_N_LAGS]) */
  if (lagStatus->id >= PTIN_SYSTEM_N_LAGS)
  {
    start = 0;
    end   = PTIN_SYSTEM_N_LAGS-1;
  }
  else
  {
    start = lagStatus->id;
    end   = lagStatus->id;
  }

  /* Loop that works for one LAG or for all of them */
  for (lag_idx=start, array_idx=0; lag_idx<=end; lag_idx++)
  {
    if (!ptin_intf_lag_exists(lag_idx))
      continue;

    ptinLagStatus.lagId = lag_idx;

    if (ptin_intf_LagStatus_get(&ptinLagStatus) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting status of LAG# %u", lag_idx);
      return L7_FAILURE;
    }

    /* Copy data to the output message structure */
    lagStatus[array_idx].SlotId               = slot_id;
    lagStatus[array_idx].id                   = ptinLagStatus.lagId;
    lagStatus[array_idx].admin                = ptinLagStatus.admin;
    lagStatus[array_idx].link_status          = ptinLagStatus.link_status;
    lagStatus[array_idx].port_channel_type    = ptinLagStatus.port_channel_type;
    lagStatus[array_idx].members_pbmp32[0]        = (L7_uint32)ptinLagStatus.members_pbmp64;
    lagStatus[array_idx].members_pbmp32[1]        = (L7_uint32)(ptinLagStatus.members_pbmp64 >> 32);
    lagStatus[array_idx].active_members_pbmp32[0] = (L7_uint32)ptinLagStatus.active_members_pbmp64;
    lagStatus[array_idx].active_members_pbmp32[1] = (L7_uint32)(ptinLagStatus.active_members_pbmp64 >> 32);

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "LAG# %2u", (L7_uint32)lagStatus[array_idx].id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .admin                  = %s",     lagStatus[array_idx].admin?"ON":"OFF");
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .link_status            = %s",     lagStatus[array_idx].link_status?"UP":"DOWN");
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .port_channel_type      = %s",     lagStatus[array_idx].port_channel_type?"STATIC":"DYNAMIC");
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .members_pbmp[0]        = 0x%08X", lagStatus[array_idx].members_pbmp32[0]);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .members_pbmp[1]        = 0x%08X", lagStatus[array_idx].members_pbmp32[1]);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .active_members_pbmp[0] = 0x%08X", lagStatus[array_idx].active_members_pbmp32[0]);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .active_members_pbmp[1] = 0x%08X", lagStatus[array_idx].active_members_pbmp32[1]);

    array_idx++;
  }

  *nElems = array_idx;
  return L7_SUCCESS;
}

/**
 * Sets one or all Ports LACP Admin State
 * 
 * @param adminState Pointer to the input structure (or array)
 * @param nElems     Number of elements in the array of structures
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_LACPAdminState_set(msg_LACPAdminState_t *adminState, L7_uint nElems)
{
  ptin_LACPAdminState_t ptinAdminState;
  L7_uint i;

  for (i=0; i<nElems; i++)
  {
    /* Copy data into ptin struct */
    ptinAdminState.port              = (L7_uint32) adminState[i].id;
    ptinAdminState.state_aggregation = adminState[i].state_aggregation;
    ptinAdminState.lacp_activity     = adminState[i].lacp_activity;
    ptinAdminState.lacp_timeout      = adminState[i].lacp_timeout;

    /* Apply settings */
    if (ptin_intf_LACPAdminState_set(&ptinAdminState) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error setting Port# %u LACP Admin State", ptinAdminState.port);
      return L7_FAILURE;
    }

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Port# %2u LACP Admin State configured", ptinAdminState.port);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .state_aggregation = %s", ptinAdminState.state_aggregation?"Enabled":"Disabled");
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .lacp_activity     = %s", ptinAdminState.lacp_activity?"True":"False");
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .lacp_timeout      = %s", ptinAdminState.lacp_timeout?"Short":"Long");
  }

  return L7_SUCCESS;
}

/**
 * Gets one or all Ports LACP Admin State
 * 
 * @param adminState Pointer to the output structure (or array)
 * @param nElems     Number of elements returned (array of structures)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_LACPAdminState_get(msg_LACPAdminState_t *adminState, L7_uint *nElems)
{
  ptin_LACPAdminState_t ptinAdminState;
  L7_uint32 port_idx;
  L7_uint32 start, end, array_idx;
  L7_uint8  slot_id;

  *nElems = 0;

  slot_id = adminState->SlotId;

  /* Determine loop range (Port index [0..PTIN_SYSTEM_N_PORTS[) */
  if (adminState->id >= PTIN_SYSTEM_N_PORTS)
  {
    start = 0;
    end   = PTIN_SYSTEM_N_PORTS-1;
  }
  else
  {
    start = adminState->id;
    end   = adminState->id;
  }

  /* Loop that works for one Port or for all of them */
  for (port_idx=start, array_idx=0; port_idx<=end; port_idx++)
  {
    ptinAdminState.port = port_idx;

    if (ptin_intf_LACPAdminState_get(&ptinAdminState) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting admin status of Port# %u", port_idx);
      return L7_FAILURE;
    }

    /* Copy data to the output message structure */
    adminState[array_idx].SlotId            = slot_id;
    adminState[array_idx].id                = ptinAdminState.port;
    adminState[array_idx].state_aggregation = ptinAdminState.state_aggregation;
    adminState[array_idx].lacp_activity     = ptinAdminState.lacp_activity;
    adminState[array_idx].lacp_timeout      = ptinAdminState.lacp_timeout;

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Port# %2u", (L7_uint32)adminState[array_idx].id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .state_aggregation = %s", adminState[array_idx].state_aggregation?"Enabled":"Disabled");
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .lacp_activity     = %s", adminState[array_idx].lacp_activity?"True":"False");
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .lacp_timeout      = %s", adminState[array_idx].lacp_timeout?"Short":"Long");

    array_idx++;
  }

  *nElems = array_idx;
  return L7_SUCCESS;
}

/**
 * Gets one or all Ports LACP statistics info
 * 
 * @param lagStats Pointer to the output structure (or array)
 * @param nElems   Number of elements returned (array of structures)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_LACPStats_get(msg_LACPStats_t *lagStats, L7_uint *nElems)
{
  ptin_LACPStats_t ptinLagStats;
  L7_uint32 port_idx;
  L7_uint32 start, end, array_idx;
  L7_uint8  slot_id;

  *nElems = 0;

  slot_id = lagStats->SlotId;

  /* Determine loop range (Port index [0..PTIN_SYSTEM_N_PORTS[) */
  if (lagStats->id >= PTIN_SYSTEM_N_PORTS)
  {
    start = 0;
    end   = PTIN_SYSTEM_N_PORTS-1;
  }
  else
  {
    start = lagStats->id;
    end   = lagStats->id;
  }

  /* Loop that works for one Port or for all of them */
  for (port_idx=start, array_idx=0; port_idx<=end; port_idx++)
  {
    ptinLagStats.port = port_idx;

    if (ptin_intf_LACPStats_get(&ptinLagStats) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting status of Port# %u", port_idx);
      return L7_FAILURE;
    }

    /* Copy data to the output message structure */
    lagStats[array_idx].SlotId     = slot_id;
    lagStats[array_idx].id         = ptinLagStats.port;
    lagStats[array_idx].LACPdus_rx = ptinLagStats.LACPdus_rx;
    lagStats[array_idx].LACPdus_tx = ptinLagStats.LACPdus_tx;

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Port# %2u", (L7_uint32)lagStats[array_idx].id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .LACPdus_rx = %u", lagStats[array_idx].LACPdus_rx);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .LACPdus_tx = %u", lagStats[array_idx].LACPdus_tx);

    array_idx++;
  }

  *nElems = array_idx;
  return L7_SUCCESS;
}

/**
 * Clears one or all Ports LACP statistics info
 * 
 * @param lagStats Structure that references the Port# to clear
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_LACPStats_clear(msg_LACPStats_t *lagStats)
{
  ptin_LACPStats_t ptinLagStats;
  L7_uint32 port_idx;
  L7_uint32 start, end, array_idx;

  /* Determine loop range (Port index [0..PTIN_SYSTEM_N_PORTS[) */
  if (lagStats->id >= PTIN_SYSTEM_N_PORTS)
  {
    start = 0;
    end   = PTIN_SYSTEM_N_PORTS-1;
  }
  else
  {
    start = lagStats->id;
    end   = lagStats->id;
  }

  /* Loop that works for one Port or for all of them */
  for (port_idx=start, array_idx=0; port_idx<=end; port_idx++)
  {
    ptinLagStats.port = port_idx;

    if (ptin_intf_LACPStats_clear(&ptinLagStats) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error clearing LACP statistics of Port# %u", port_idx);
      return L7_FAILURE;
    }

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Port# %2u: LACP statistics successfully cleared", port_idx);
  }

  return L7_SUCCESS;
}

/* L2 Manipulation functions **************************************************/

/**
 * Sets L2 aging time
 * 
 * @param switch_config Pointer to configuration structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_l2_switch_config_set(msg_switch_config_t *switch_config)
{
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Message function '%s' is being executed",__FUNCTION__);

  /* Check mask */
  if (switch_config->mask & 0x0001)
  {
    /* Set aging time */
    if (ptin_l2_mac_aging_set(switch_config->aging_time)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error setting aging time to %u seconds",switch_config->aging_time);
      return L7_FAILURE;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success setting aging time to %u seconds",switch_config->aging_time);
    }

    /* Set ARP timeout to be the same as MAC age time */
    if(L7_SUCCESS != usmDbIpArpAgeTimeSet(1, switch_config->aging_time))
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error setting ARP timeout to %u seconds", switch_config->aging_time);
      return L7_FAILURE;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success setting ARP timeout to %u seconds", switch_config->aging_time);
    }
  }

  return L7_SUCCESS;
}

/**
 * Gets L2 aging time
 * 
 * @param switch_config Pointer to configuration structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_l2_switch_config_get(msg_switch_config_t *switch_config)
{
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Message function '%s' is being executed",__FUNCTION__);

  switch_config->mask = 0x0000;

  if (ptin_l2_mac_aging_get(&switch_config->aging_time)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting aging time");
    return L7_FAILURE;
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success getting aging time (%u seconds)",switch_config->aging_time);
    switch_config->mask |= 0x0001;
  }

  return L7_SUCCESS;
}

/**
 * Shows L2 table
 * 
 * @param mac_table: Mac list structure
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_l2_macTable_get(msg_switch_mac_table_t *mac_table, int struct1or2)
{
  L7_uint32 i, numEntries;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Message function '%s' is being executed",__FUNCTION__);

  /* Validate arguments */
  if (mac_table==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid argument");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG," SlotId       = %u",mac_table->intro.slotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," StartEntryId = %u",mac_table->intro.startEntryId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," NumEntries   = %u",mac_table->intro.numEntries);

#if 0
  L7_uint8  slotId;
  L7_uint32 startId;
  L7_uint64 macAddr;
  L7_uint32 maxSize;

  slotId  = mac_table->intro.slotId;
  maxSize = ((L7_uint32) (slotId+2)*100)%4095+1;

  /* Read start id */
  startId     = mac_table->intro.startEntryId;
  numEntries  = mac_table->intro.numEntries;

  /* Validate start id */
  if (startId>=maxSize)
  {
    mac_table->intro.numEntries = 0;
    LOG_WARNING(LOG_CTX_PTIN_MSG," StartEntryId is higher than 999");
    return L7_SUCCESS;
  }

  /* Validate number of entries */
  if (startId+numEntries>=maxSize)
  {
    numEntries = maxSize - startId;
    LOG_WARNING(LOG_CTX_PTIN_MSG," First majoration of Number of entries to %u",numEntries);
  }
  if (numEntries>MSG_CMDGET_MAC_TABLE_MAXENTRIES)
  {
    numEntries = MSG_CMDGET_MAC_TABLE_MAXENTRIES;
    LOG_WARNING(LOG_CTX_PTIN_MSG," Second majoration of entries limited to %u",numEntries);
  }

  mac_table->intro.numEntries = numEntries;
  for (i=0; i<numEntries; i++)
  {
    macAddr = (startId+slotId+i)%0x010000000000ULL;

    mac_table->entry[i].evcId  = (L7_uint16)-1;
    mac_table->entry[i].vlanId = (startId+slotId+i)%4096;
    mac_table->entry[i].intf.intf_type = 0;
    mac_table->entry[i].intf.intf_id = (startId+i)%PTIN_SYSTEM_N_PORTS;
    mac_table->entry[i].addr[0] = (L7_uint8) ((macAddr>>40) & 0xff);
    mac_table->entry[i].addr[1] = (L7_uint8) ((macAddr>>32) & 0xff);
    mac_table->entry[i].addr[2] = (L7_uint8) ((macAddr>>24) & 0xff);
    mac_table->entry[i].addr[3] = (L7_uint8) ((macAddr>>16) & 0xff);
    mac_table->entry[i].addr[4] = (L7_uint8) ((macAddr>>8) & 0xff);
    mac_table->entry[i].addr[5] = (L7_uint8) (macAddr & 0xff);
  }
#else

  ptin_switch_mac_entry *entries_list;

  /* Load table */
  if (mac_table->intro.startEntryId==0)
  {
    if (ptin_l2_mac_table_load()!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error loading table");
      return L7_FAILURE;
    }
  }

  /* Get input number of MAC itemns, and majorate them */

  numEntries = mac_table->intro.numEntries;

  if (numEntries>MSG_CMDGET_MAC_TABLE_MAXENTRIES)
  {
    numEntries = MSG_CMDGET_MAC_TABLE_MAXENTRIES;
  }

  /* Read table */
  if (ptin_l2_mac_table_get(mac_table->intro.startEntryId, &numEntries, &entries_list)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error getting MAC list");
    return L7_FAILURE;
  }

  /* Copy MAC list to output message */
  if (2==struct1or2) {
      msg_switch_mac_table_t *p;

      p= (msg_switch_mac_table_t*)mac_table;

      for (i=0; i<numEntries; i++)
      {
        memcpy(p->entry[i].addr, entries_list[i].addr, sizeof(L7_uint8)*6);
        p->entry[i].evcId          = entries_list[i].evcId;
        p->entry[i].vlanId         = entries_list[i].vlanId;
        p->entry[i].intf.intf_type = entries_list[i].intf.intf_type;
        p->entry[i].intf.intf_id   = entries_list[i].intf.intf_id;
        p->entry[i].gem_id         = entries_list[i].gem_id;
        p->entry[i].static_entry   = entries_list[i].static_entry;
      }
  }
  else
  for (i=0; i<numEntries; i++)
  {
    memcpy(mac_table->entry[i].addr, entries_list[i].addr, sizeof(L7_uint8)*6);
    mac_table->entry[i].evcId          = entries_list[i].evcId;
    mac_table->entry[i].vlanId         = entries_list[i].vlanId;
    mac_table->entry[i].intf.intf_type = entries_list[i].intf.intf_type;
    mac_table->entry[i].intf.intf_id   = entries_list[i].intf.intf_id;
    mac_table->entry[i].static_entry   = entries_list[i].static_entry;
  }

  /* Update number of entries */
  mac_table->intro.numEntries = numEntries;

#endif

  return L7_SUCCESS;
}

/**
 * Remove an address from the L2 table
 * 
 * @param mac_table: Mac list structure
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_l2_macTable_remove(msg_switch_mac_table_t *mac_table)
{
  L7_uint32 i, numEntries;
  ptin_switch_mac_entry entry;
  L7_RC_t rc = L7_SUCCESS;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Message function '%s' being executed",__FUNCTION__);

  /* Validate arguments */
  if (mac_table==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid argument");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG," SlotId       = %u",mac_table->intro.slotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," StartEntryId = %u",mac_table->intro.startEntryId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," NumEntries   = %u",mac_table->intro.numEntries);

  /* Get input number of MAC itemns, and majorate them */

  numEntries = mac_table->intro.numEntries;

  /* If numEntries is -1, flush all L2 MAC entries */
  if (numEntries==(L7_uint32)-1)
  {
    if (ptin_l2_mac_table_flush()!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error flushing MAC table");
      return L7_FAILURE;
    }
    else
    {
      LOG_WARNING(LOG_CTX_PTIN_MSG,"MAC table will flushed");
      return L7_SUCCESS;
    }
  }
  /* Consider only a maximum of 256 elements */
  else if (numEntries>MSG_CMDGET_MAC_TABLE_MAXENTRIES)
  {
    LOG_WARNING(LOG_CTX_PTIN_MSG,"numEntries limited from %u to %u",numEntries,MSG_CMDGET_MAC_TABLE_MAXENTRIES);
    numEntries = MSG_CMDGET_MAC_TABLE_MAXENTRIES;
  }

  /* Remove all elements */
  for (i=0; i<numEntries; i++)
  {
    entry.entryId = 0;
    memcpy(entry.addr, mac_table->entry[i].addr, sizeof(L7_uint8)*6);
    entry.evcId          = mac_table->entry[i].evcId;
    entry.vlanId         = mac_table->entry[i].vlanId;
    entry.intf.intf_type = mac_table->entry[i].intf.intf_type;
    entry.intf.intf_id   = mac_table->entry[i].intf.intf_id;
    entry.static_entry   = mac_table->entry[i].static_entry;

    if (ptin_l2_mac_table_entry_remove(&entry)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error removing index entry %u",i);
      rc = L7_FAILURE;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_MSG,"Success adding index entry %u",i);
    }
  }

  return rc;
}

/**
 * Add an address to the L2 table
 * 
 * @param mac_table: Mac list structure
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_l2_macTable_add(msg_switch_mac_table_t *mac_table)
{
  L7_uint32 i, numEntries;
  ptin_switch_mac_entry entry;
  L7_RC_t rc = L7_SUCCESS;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Message function '%s' being executed",__FUNCTION__);

  /* Validate arguments */
  if (mac_table==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid argument");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG," SlotId       = %u",mac_table->intro.slotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," StartEntryId = %u",mac_table->intro.startEntryId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," NumEntries   = %u",mac_table->intro.numEntries);

  /* Get input number of MAC itemns, and majorate them */

  numEntries = mac_table->intro.numEntries;

  /* Consider only a maximum of 256 elements */
  if (numEntries>MSG_CMDGET_MAC_TABLE_MAXENTRIES)
  {
    LOG_WARNING(LOG_CTX_PTIN_MSG,"numEntries limited from %u to %u",numEntries,MSG_CMDGET_MAC_TABLE_MAXENTRIES);
    numEntries = MSG_CMDGET_MAC_TABLE_MAXENTRIES;
  }

  /* Remove all elements */
  for (i=0; i<numEntries; i++)
  {
    entry.entryId = 0;
    memcpy(entry.addr, mac_table->entry[i].addr, sizeof(L7_uint8)*6);
    entry.evcId          = mac_table->entry[i].evcId;
    entry.vlanId         = mac_table->entry[i].vlanId;
    entry.intf.intf_type = mac_table->entry[i].intf.intf_type;
    entry.intf.intf_id   = mac_table->entry[i].intf.intf_id;
    entry.static_entry   = mac_table->entry[i].static_entry;

    if (ptin_l2_mac_table_entry_add(&entry)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error adding index entry %u",i);
      rc = L7_FAILURE;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_MSG,"Success adding index entry %u",i);
    }
  }

  return rc;
}

/* EVCs Manipulation Functions ************************************************/
/**
 * Gets an EVC configuration
 * 
 * NOTE: It allowed to read EVC# PTIN_EVC_INBAND, although it CANNOT be created or deleted
 * using CLI or Manager interface
 * 
 * @param msgEvcConf Pointer to the output struct (index field is used as input param)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_EVC_get(msg_HwEthMef10Evc_t *msgEvcConf)
{
  L7_uint i;
  ptin_HwEthMef10Evc_t ptinEvcConf;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if (msgEvcConf->id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "EVC# %u is out of range [0..%u]", msgEvcConf->id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  ptinEvcConf.index = msgEvcConf->id;

  if (ptin_evc_get(&ptinEvcConf) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting EVC# %u configuration", ptinEvcConf.index);
    return L7_FAILURE;
  }

  /* Copy data to message struct */
  msgEvcConf->id    = ptinEvcConf.index;
  msgEvcConf->flags    = ptinEvcConf.flags;
  msgEvcConf->type     = ptinEvcConf.type;
  msgEvcConf->mc_flood = ptinEvcConf.mc_flood;
  msgEvcConf->n_intf   = ptinEvcConf.n_intf;
  memcpy(msgEvcConf->ce_vid_bmp, ptinEvcConf.ce_vid_bmp, sizeof(msgEvcConf->ce_vid_bmp));

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "EVC# %u",              msgEvcConf->id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Flags    = 0x%08X",  msgEvcConf->flags);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Type     = %u",      msgEvcConf->type);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .MC Flood = %u (%s)", msgEvcConf->mc_flood, msgEvcConf->mc_flood==0?"All":msgEvcConf->mc_flood==1?"Unknown":"None");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Nr.Intf  = %u",      msgEvcConf->n_intf);

  for (i=0; i < ptinEvcConf.n_intf; i++)
  {
    msgEvcConf->intf[i].intf_id   = ptinEvcConf.intf[i].intf_id;
    msgEvcConf->intf[i].intf_type = ptinEvcConf.intf[i].intf_type;
    msgEvcConf->intf[i].mef_type  = ptinEvcConf.intf[i].mef_type;
    msgEvcConf->intf[i].vid       = ptinEvcConf.intf[i].vid;

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "   %s# %02u %s VID=%04u",
              msgEvcConf->intf[i].intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
              msgEvcConf->intf[i].intf_id,
              msgEvcConf->intf[i].mef_type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf",
              msgEvcConf->intf[i].vid);
  }

  return L7_SUCCESS;
}

/**
 * Creates or reconfigures an EVC
 * 
 * @param msgEvcConf Pointer to the input struct
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_EVC_create(msg_HwEthMef10Evc_t *msgEvcConf)
{
  L7_uint i;
  ptin_HwEthMef10Evc_t ptinEvcConf;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if ((msgEvcConf->id == PTIN_EVC_INBAND) || (msgEvcConf->id >= PTIN_SYSTEM_N_EXTENDED_EVCS))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "EVC# %u is out of range [0..%u]", msgEvcConf->id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Copy data to ptin struct */
  ptinEvcConf.index    = msgEvcConf->id;
  ptinEvcConf.flags    = msgEvcConf->flags;
  ptinEvcConf.type     = msgEvcConf->type;
  ptinEvcConf.mc_flood = msgEvcConf->mc_flood;
  ptinEvcConf.n_intf   = msgEvcConf->n_intf;
  memcpy(ptinEvcConf.ce_vid_bmp, msgEvcConf->ce_vid_bmp, sizeof(ptinEvcConf.ce_vid_bmp));

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "EVC# %u",              ptinEvcConf.index);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Flags    = 0x%08X",  ptinEvcConf.flags);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Type     = %u",      ptinEvcConf.type);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .MC Flood = %u (%s)", ptinEvcConf.mc_flood, ptinEvcConf.mc_flood==0?"All":ptinEvcConf.mc_flood==1?"Unknown":"None");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Nr.Intf  = %u",      ptinEvcConf.n_intf);

  for (i=0; i < ptinEvcConf.n_intf; i++)
  {
    #if (0)
    /* PTP: Workaround */

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "ptin_sys_number_of_ports (%d)", ptin_sys_number_of_ports);

    if ( (msgEvcConf->intf[i].intf_id == (ptin_sys_number_of_ports - 1)) /*&& (msgEvcConf->intf[i].vid >= 1065)*/ )
    {

      ptinEvcConf.flags = 0x18;
      ptinEvcConf.n_intf--;
      LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Flags    = 0x%08X",  ptinEvcConf.flags);
      
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "PTP EVC (%u)",              ptinEvcConf.index);
      break;
    }

    ptinEvcConf.flags &= ~PTIN_EVC_MASK_IGMP_PROTOCOL;
    ptinEvcConf.flags &= ~PTIN_EVC_MASK_MC_IPTV;     
    #endif

    ptinEvcConf.intf[i].intf_id   = msgEvcConf->intf[i].intf_id;
    ptinEvcConf.intf[i].intf_type = msgEvcConf->intf[i].intf_type;
    ptinEvcConf.intf[i].mef_type  = msgEvcConf->intf[i].mef_type /*PTIN_EVC_INTF_ROOT*/;
    ptinEvcConf.intf[i].vid       = msgEvcConf->intf[i].vid;
    ptinEvcConf.intf[i].vid_inner = msgEvcConf->intf[i].inner_vid;

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "   %s# %02u %s VID=%04u/%-04u",
             ptinEvcConf.intf[i].intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
             ptinEvcConf.intf[i].intf_id,
             ptinEvcConf.intf[i].mef_type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf",
             ptinEvcConf.intf[i].vid,ptinEvcConf.intf[i].vid_inner);
  }

  if (ptin_evc_create(&ptinEvcConf) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error creating/reconfiguring EVC# %u", ptinEvcConf.index);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Deletes an EVC
 * 
 * @param msgEvcConf Pointer to the input struct (index field must be set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_EVC_delete(msg_HwEthMef10EvcRemove_t *msgEvcConf, L7_uint16 n_structs)
{
  L7_uint16 i;
  L7_RC_t rc_global = L7_SUCCESS;

  if (msgEvcConf == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid argument");
    return L7_FAILURE;
  }

  for (i=0; i<n_structs; i++)
  {
    /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
    if ((msgEvcConf[i].id == PTIN_EVC_INBAND) || (msgEvcConf[i].id >= PTIN_SYSTEM_N_EXTENDED_EVCS))
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "EVC# %u is out of range [0..%u]", msgEvcConf[i].id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
      rc_global = L7_FAILURE;
      continue;
    }

    if (ptin_evc_delete(msgEvcConf[i].id) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error deleting EVC# %u", msgEvcConf[i].id);
      rc_global = L7_FAILURE;
      continue;
    }

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "EVC# %u successfully deleted", msgEvcConf[i].id);
  }

  return rc_global;
}

/**
 * Add/remove port to/from an EVC
 * 
 * @param msgEvcPort : Pointer to the input struct 
 * @param n_size     : Number of structures 
 * @param oper       : Operation type
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_evc_port(msg_HWevcPort_t *msgEvcPort, L7_uint16 n_size, ptin_msg_oper_t oper)
{
  L7_uint i;
  ptin_HwEthMef10Intf_t ptinEvcPort;
  L7_RC_t rc, rc_global = L7_SUCCESS;

  /* Validate arguments */
  if (msgEvcPort == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "No data provided");
    return L7_FAILURE;
  }

  /* Run all structures */
  for (i=0; i<n_size; i++)
  {
    /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
    if (/*(msgEvcPort[i].evcId == PTIN_EVC_INBAND) ||*/ (msgEvcPort[i].evcId >= PTIN_SYSTEM_N_EXTENDED_EVCS))
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "EVC# %u is out of range [0..%u]", msgEvcPort[i].evcId, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
      return L7_FAILURE;
    }

    /* Copy data to ptin struct */
    ptinEvcPort.intf_type = msgEvcPort[i].intf.intf_type;
    ptinEvcPort.intf_id   = msgEvcPort[i].intf.intf_id;
    ptinEvcPort.mef_type  = msgEvcPort[i].intf.mef_type;
    ptinEvcPort.vid       = msgEvcPort[i].intf.vid;
    ptinEvcPort.vid_inner = msgEvcPort[i].intf.inner_vid;

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "EVC# %u - oper %s",     msgEvcPort[i].evcId,
              ((oper==PTIN_MSG_OPER_ADD) ? "ADD" : ((oper==PTIN_MSG_OPER_REMOVE) ? "REMOVE" : "UNKNOWN")));
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Intf      = %u/%u",   ptinEvcPort.intf_type, ptinEvcPort.intf_id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .IntfType  = %s",     (ptinEvcPort.mef_type == PTIN_EVC_INTF_LEAF) ? "LEAF" : "ROOT");
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .OuterVlan = %u",      ptinEvcPort.vid);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " .InnerVlan = %u",      ptinEvcPort.vid_inner);

    /* Add/remove port */
    switch (oper)
    {
    case PTIN_MSG_OPER_ADD:
      if ((rc=ptin_evc_port_add(msgEvcPort[i].evcId, &ptinEvcPort)) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "Error adding port %u/%u to EVC# %u", ptinEvcPort.intf_type, ptinEvcPort.intf_id, msgEvcPort[i].evcId);
        rc_global = rc;
      }
      LOG_TRACE(LOG_CTX_PTIN_MSG, "Added port %u/%u to EVC# %u", ptinEvcPort.intf_type, ptinEvcPort.intf_id, msgEvcPort[i].evcId);
      break;
    case PTIN_MSG_OPER_REMOVE:
      if ((rc=ptin_evc_port_remove(msgEvcPort[i].evcId, &ptinEvcPort)) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "Error removing port %u/%u to EVC# %u", ptinEvcPort.intf_type, ptinEvcPort.intf_id, msgEvcPort[i].evcId);
        rc_global = rc;
      }
      LOG_TRACE(LOG_CTX_PTIN_MSG, "Removed port %u/%u from EVC# %u", ptinEvcPort.intf_type, ptinEvcPort.intf_id, msgEvcPort[i].evcId);
      break;
    default:
      LOG_ERR(LOG_CTX_PTIN_MSG, "Unknown operation %u", oper);
      rc_global = L7_FAILURE;
    }
  }

  return rc_global;
}

/**
 * Adds a bridge to a stacked EVC between the root and a particular interface
 * 
 * @param msgEvcBridge Bridge info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_EVCBridge_add(msg_HwEthEvcBridge_t *msgEvcBridge)
{
  ptin_HwEthEvcBridge_t ptinEvcBridge;

  /* Copy data */
  ptinEvcBridge.index          = msgEvcBridge->evcId;
  ptinEvcBridge.inn_vlan       = msgEvcBridge->inn_vlan;
  ptinEvcBridge.intf.intf_id   = msgEvcBridge->intf.intf_id;
  ptinEvcBridge.intf.intf_type = msgEvcBridge->intf.intf_type;
  ptinEvcBridge.intf.mef_type  = msgEvcBridge->intf.mef_type;   /* must be Leaf */
  ptinEvcBridge.intf.vid       = msgEvcBridge->intf.vid;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "EVC# %u Bridge",         ptinEvcBridge.index);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " %s# %u",                ptinEvcBridge.intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
            ptinEvcBridge.intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Inner VID       = %u", ptinEvcBridge.inn_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Outer VID [NEW] = %u", ptinEvcBridge.intf.vid);

  if (ptin_evc_p2p_bridge_add(&ptinEvcBridge) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error adding EVC# %u bridge", ptinEvcBridge.index);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Removes a bridge from a stacked EVC between the root and a particular interface
 * 
 * @param msgEvcBridge Bridge info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_EVCBridge_remove(msg_HwEthEvcBridge_t *msgEvcBridge)
{
  ptin_HwEthEvcBridge_t ptinEvcBridge;
  L7_RC_t rc;

  /* Copy data */
  ptinEvcBridge.index          = msgEvcBridge->evcId;
  ptinEvcBridge.inn_vlan       = msgEvcBridge->inn_vlan;
  ptinEvcBridge.intf.intf_id   = msgEvcBridge->intf.intf_id;
  ptinEvcBridge.intf.intf_type = msgEvcBridge->intf.intf_type;
  ptinEvcBridge.intf.mef_type  = msgEvcBridge->intf.mef_type;   /* must be Leaf */
  ptinEvcBridge.intf.vid       = msgEvcBridge->intf.vid;        /* not used on remove oper. */

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "EVC# %u Bridge",         ptinEvcBridge.index);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " %s# %u",                ptinEvcBridge.intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
            ptinEvcBridge.intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Inner VID       = %u", ptinEvcBridge.inn_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Outer VID [NEW] = %u", ptinEvcBridge.intf.vid);

  rc = ptin_evc_p2p_bridge_remove(&ptinEvcBridge);

  if ( rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error removing EVC# %u bridge", ptinEvcBridge.index);
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Adds a flow to an EVC
 * 
 * @param msgEvcFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_EVCFlow_add(msg_HwEthEvcFlow_t *msgEvcFlow)
{
  ptin_HwEthEvcFlow_t ptinEvcFlow;
  L7_RC_t rc;

  /* Copy data */
  ptinEvcFlow.evc_idx             = msgEvcFlow->evcId;
  ptinEvcFlow.flags               = msgEvcFlow->flags;
  ptinEvcFlow.int_ivid            = msgEvcFlow->nni_cvlan;
  ptinEvcFlow.ptin_intf.intf_type = msgEvcFlow->intf.intf_type;
  ptinEvcFlow.ptin_intf.intf_id   = msgEvcFlow->intf.intf_id;
  ptinEvcFlow.uni_ovid            = msgEvcFlow->intf.outer_vid; /* must be a leaf */
  ptinEvcFlow.uni_ivid            = msgEvcFlow->intf.inner_vid;
  ptinEvcFlow.macLearnMax         = msgEvcFlow->macLearnMax;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "EVC# %u Flow",     ptinEvcFlow.evc_idx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Flags = 0x%08x",  ptinEvcFlow.flags);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " %s# %u",          ptinEvcFlow.ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
                                                  ptinEvcFlow.ptin_intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Int.IVID    = %u", ptinEvcFlow.int_ivid);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " UNI-OVID    = %u", ptinEvcFlow.uni_ovid);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " UNI-IVID    = %u", ptinEvcFlow.uni_ivid);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " macLearnMax = %u", ptinEvcFlow.macLearnMax);  

#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
  if (ptinEvcFlow.flags & PTIN_EVC_MASK_IGMP_PROTOCOL)
  {
    if ( (msgEvcFlow->mask > PTIN_IGMP_ADMISSION_CONTROL_MASK_VALID) || 
        ( ( (msgEvcFlow->mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH ) &&
          (msgEvcFlow->maxBandwidth != PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_DISABLE_UINT64 && msgEvcFlow->maxBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_KBPS) ) ||
         ( ( (msgEvcFlow->mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS ) &&
          (msgEvcFlow->maxChannels != PTIN_IGMP_ADMISSION_CONTROL_MAX_CHANNELS_DISABLE && msgEvcFlow->maxChannels > PTIN_IGMP_ADMISSION_CONTROL_MAX_CHANNELS) ) )
        
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid Admission Control Parameters [mask:0x%02x maxBandwidth:%llu kbits/s maxChannels:%hu",msgEvcFlow->mask, msgEvcFlow->maxBandwidth, msgEvcFlow->maxChannels);
      return L7_FAILURE;
    }
    ptinEvcFlow.onuId               = msgEvcFlow->onuId;
    ptinEvcFlow.mask                = msgEvcFlow->mask;
    ptinEvcFlow.maxBandwidth        = msgEvcFlow->maxBandwidth;
    ptinEvcFlow.maxChannels         = msgEvcFlow->maxChannels;
    
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " onuId       = %u", ptinEvcFlow.onuId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " mask        = 0x%x", ptinEvcFlow.mask);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " maxChannels = %u", ptinEvcFlow.maxChannels);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, " maxBandwidth= %llu kbit/s", ptinEvcFlow.maxBandwidth);
  }
#endif

  if ((rc=ptin_evc_flow_add(&ptinEvcFlow)) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error adding EVC# %u flow", ptinEvcFlow.evc_idx);
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Removes a flow from an EVC
 * 
 * @param msgEvcFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_EVCFlow_remove(msg_HwEthEvcFlow_t *msgEvcFlow)
{
  ptin_HwEthEvcFlow_t ptinEvcFlow;
  L7_RC_t rc;

  /* Copy data */
  ptinEvcFlow.evc_idx             = msgEvcFlow->evcId;
  ptinEvcFlow.ptin_intf.intf_type = msgEvcFlow->intf.intf_type;
  ptinEvcFlow.ptin_intf.intf_id   = msgEvcFlow->intf.intf_id;
  ptinEvcFlow.int_ivid            = msgEvcFlow->nni_cvlan;
  ptinEvcFlow.uni_ovid            = msgEvcFlow->intf.outer_vid; /* must be a leaf */
  ptinEvcFlow.uni_ivid            = msgEvcFlow->intf.inner_vid;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "EVC# %u Flow",   ptinEvcFlow.evc_idx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " %s# %u",        ptinEvcFlow.ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
                                                ptinEvcFlow.ptin_intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " Int.IVID = %u", ptinEvcFlow.int_ivid);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " UNI-OVID = %u", ptinEvcFlow.uni_ovid);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " UNI-IVID = %u", ptinEvcFlow.uni_ivid);

  if ((rc=ptin_evc_flow_remove(&ptinEvcFlow)) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error removing EVC# %u flow", ptinEvcFlow.evc_idx);
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Adds a flooding vlan applied to an EVC
 * 
 * @param msgEvcFlood : Flooding vlan info 
 * @param n_clients   : Number of vlans to be added
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_EvcFloodVlan_add(msg_HwEthEvcFloodVlan_t *msgEvcFlood, L7_uint n_clients)
{
  L7_uint     i;
  ptin_intf_t ptin_intf;
  L7_RC_t     rc = L7_SUCCESS;

  if ( msgEvcFlood == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid params");
    return L7_FAILURE;
  }

  /* Run all clients */
  for ( i=0; i<n_clients; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"EVC flood vlan %u:",i);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Slot    = %u",    msgEvcFlood[i].SlotId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," EVC_idx = %u",    msgEvcFlood[i].evcId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Mask    = 0x%02x",msgEvcFlood[i].mask);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Intf    = %u/%u", msgEvcFlood[i].intf.intf_type, msgEvcFlood[i].intf.intf_id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," CVlan   = %u",    msgEvcFlood[i].client_vlan);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Outer Vlan : %u", msgEvcFlood[i].oVlanId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Inner Vlan : %u", msgEvcFlood[i].iVlanId);

    ptin_intf.intf_type = msgEvcFlood[i].intf.intf_type;
    ptin_intf.intf_id   = msgEvcFlood[i].intf.intf_id;

    if (ptin_evc_flood_vlan_add( msgEvcFlood[i].evcId,
                                 ((msgEvcFlood[i].mask & 0x01) ? &ptin_intf : L7_NULLPTR),
                                 ((msgEvcFlood[i].mask & 0x02) ? msgEvcFlood[i].client_vlan : 0),
                                 msgEvcFlood[i].oVlanId,
                                 msgEvcFlood[i].iVlanId ) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error adding EVC# %u flooding vlan", msgEvcFlood[i].evcId);
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**
 * Removes a flooding vlan applied to an EVC
 * 
 * @param msgEvcFlood : Flooding vlan info 
 * @param n_clients   : Number of vlans to be removed
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_EvcFloodVlan_remove(msg_HwEthEvcFloodVlan_t *msgEvcFlood, L7_uint n_clients)
{
  L7_uint     i;
  ptin_intf_t ptin_intf;
  L7_RC_t     rc = L7_SUCCESS;

  if ( msgEvcFlood == L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid params");
    return L7_FAILURE;
  }

  /* Run all clients */
  for ( i=0; i<n_clients; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"EVC flood vlan %u:",i);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Slot    = %u",    msgEvcFlood[i].SlotId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," EVC_idx = %u",    msgEvcFlood[i].evcId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Mask    = 0x%02x",msgEvcFlood[i].mask);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Intf    = %u/%u", msgEvcFlood[i].intf.intf_type, msgEvcFlood[i].intf.intf_id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," CVlan   = %u",    msgEvcFlood[i].client_vlan);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Outer Vlan : %u", msgEvcFlood[i].oVlanId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Inner Vlan : %u", msgEvcFlood[i].iVlanId);

    ptin_intf.intf_type = msgEvcFlood[i].intf.intf_type;
    ptin_intf.intf_id   = msgEvcFlood[i].intf.intf_id;

    if (ptin_evc_flood_vlan_remove( msgEvcFlood[i].evcId,
                                    ((msgEvcFlood[i].mask & 0x01) ? &ptin_intf : L7_NULLPTR),
                                    ((msgEvcFlood[i].mask & 0x02) ? msgEvcFlood[i].client_vlan : 0),
                                    msgEvcFlood[i].oVlanId,
                                    msgEvcFlood[i].iVlanId ) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error removing EVC# %u flooding vlan", msgEvcFlood[i].evcId);
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/* Bandwidth profiles *********************************************************/

/**
 * Get data of an existent bandwidth profile
 * 
 * @param msgBwProfile : Bw profile
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_bwProfile_get(msg_HwEthBwProfile_t *msgBwProfile)
{
  L7_uint32 evcId;
  ptin_bw_profile_t profile;
  L7_RC_t   rc;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgBwProfile==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG," evcId  = %u",msgBwProfile->evcId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," mask   = 0x%02x",msgBwProfile->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," SVID   = %u",msgBwProfile->service_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," CVID   = %u",msgBwProfile->client_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," SrcIntf= %u/%u",msgBwProfile->intf_src.intf_type,msgBwProfile->intf_src.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," DstIntf= %u/%u",msgBwProfile->intf_dst.intf_type,msgBwProfile->intf_dst.intf_id);

  /* Extract EVC id */
  evcId = msgBwProfile->evcId;

  /* Copy data */
  if (ptin_msg_bwProfileStruct_fill(msgBwProfile,&profile)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error with ptin_msg_bwProfileStruct_fill");
    return L7_FAILURE;
  }

  /* Add bandwidth profile */
  if ((rc=ptin_evc_bwProfile_get(evcId,&profile))!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error reading profile!");
    return rc;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Message processing finished!");
  return L7_SUCCESS;
}

/**
 * Add a new bandwidth profile
 * 
 * @param msgBwProfile : Bw profile
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_bwProfile_set(msg_HwEthBwProfile_t *msgBwProfile, unsigned int msgId)
{
  L7_uint32 evcId;
  ptin_bw_profile_t profile;
  L7_RC_t   rc;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgBwProfile==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG," evcId  = %u",msgBwProfile->evcId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," mask   = 0x%02x",msgBwProfile->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," SVID   = %u",msgBwProfile->service_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," CVID   = %u",msgBwProfile->client_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," SrcIntf= %u/%u",msgBwProfile->intf_src.intf_type,msgBwProfile->intf_src.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," DstIntf= %u/%u",msgBwProfile->intf_dst.intf_type,msgBwProfile->intf_dst.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Meter {CIR,CBS}={%llu,%llu}",msgBwProfile->profile.cir,msgBwProfile->profile.cbs);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Meter {EIR,EBS}={%llu,%llu}",msgBwProfile->profile.eir,msgBwProfile->profile.ebs);

  /* Extract EVC id */
  evcId = msgBwProfile->evcId;

  /* Copy data */
  if (ptin_msg_bwProfileStruct_fill(msgBwProfile,&profile)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error with ptin_msg_bwProfileStruct_fill");
    return L7_FAILURE;
  }

  /* Add bandwidth profile */
  switch (msgId) {
  case CCMSG_ETH_BW_PROFILE_SET:
      profile.cos=-1;   //Set to ignore
      if ((rc=ptin_evc_bwProfile_set(evcId,&profile))!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG,"Error applying profile!");
        return rc;
      }
      break;
  case CCMSG_ETH_BW_PROFILE_SET_II:
      {
       //L7_uint16 i;
       msg_HwEthBwProfile_II_t *msgBwProfile_II;
       ptin_bw_profile_t p;

       msgBwProfile_II = (msg_HwEthBwProfile_II_t *) msgBwProfile;

       profile.cos = msgBwProfile_II->cos;
       //profile.meter.cir/=8;
       //profile.meter.eir/=8;
       //for (i=0; i<8; i++) {
           //profile.cos=i;
           p=profile;
           if ((rc=ptin_evc_bwProfile_set(evcId,&p))!=L7_SUCCESS)
           {
             LOG_ERR(LOG_CTX_PTIN_MSG,"Error applying profile!");
             return rc;
           }
       //}
      }
      break;
  }//switch

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Message processing finished!");
  return L7_SUCCESS;
}

/**
 * Remove an existent bandwidth profile
 * 
 * @param msgBwProfile : Bw profile
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_bwProfile_delete(msg_HwEthBwProfile_t *msgBwProfile, unsigned int msgId)
{
  L7_uint32 evcId;
  ptin_bw_profile_t profile;
  L7_RC_t   rc;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgBwProfile==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG," evcId  = %u",msgBwProfile->evcId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," mask   = 0x%02x",msgBwProfile->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," SVID   = %u",msgBwProfile->service_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," CVID   = %u",msgBwProfile->client_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," SrcIntf= %u/%u",msgBwProfile->intf_src.intf_type,msgBwProfile->intf_src.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," DstIntf= %u/%u",msgBwProfile->intf_dst.intf_type,msgBwProfile->intf_dst.intf_id);

  /* Extract EVC id */
  evcId = msgBwProfile->evcId;

  /* Copy data */
  if (ptin_msg_bwProfileStruct_fill(msgBwProfile,&profile)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error with ptin_msg_bwProfileStruct_fill");
    return L7_FAILURE;
  }

  switch (msgId) {
  case CCMSG_ETH_BW_PROFILE_DELETE:
      profile.cos=-1;   //Set to ignore
      if ((rc=ptin_evc_bwProfile_delete(evcId,&profile))!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG,"Error removing profile!");
        return rc;
      }
      break;
  case CCMSG_ETH_BW_PROFILE_DELETE_II:
      {
       //L7_uint16 i;
       msg_HwEthBwProfile_II_t *msgBwProfile_II;
       ptin_bw_profile_t p;

       msgBwProfile_II = (msg_HwEthBwProfile_II_t *) msgBwProfile;

       profile.cos = msgBwProfile_II->cos;
       //profile.meter.cir/=8;
       //profile.meter.eir/=8;
       //for (i=0; i<8; i++) {
           //profile.cos=i;
           p=profile;
           if ((rc=ptin_evc_bwProfile_delete(evcId,&p))!=L7_SUCCESS)
           {
             LOG_ERR(LOG_CTX_PTIN_MSG,"Error removing profile!");
             return rc;
           }
       //}
      }
      break;
  }//switch

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Message processing finished!");
  return L7_SUCCESS;
}

/**
 * Get data of an existent storm control profile
 * 
 * @param msgStormControl : Storm control profile
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_stormControl_get(msg_HwEthStormControl_t *msgStormControl)
{
  ptin_stormControl_t stormControl;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgStormControl==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG," slotId = %u", msgStormControl->SlotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," id     = %u", msgStormControl->id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," flags  = 0x%04x", msgStormControl->flags);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," mask   = 0x%04x", msgStormControl->mask);

  /* Input data */
  memset(&stormControl, 0x00, sizeof(ptin_stormControl_t));
  /* Traffic Type */
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_BCAST)
  {
    stormControl.flags |= PTIN_STORMCONTROL_MASK_BCAST;
  }
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_MCAST)
  {
    stormControl.flags |= PTIN_STORMCONTROL_MASK_MCAST;
  }
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_UCUNK)
  {
    stormControl.flags |= PTIN_STORMCONTROL_MASK_UCUNK;
  }

  /* Read bandwidth profile */
  if ((rc=ptin_evc_stormControl_get(&stormControl))!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error reading storm control data!");
    return rc;
  }

  /* Init Output data */
  msgStormControl->mask = 0;
  msgStormControl->bcast_rate = 0;
  msgStormControl->mcast_rate = 0;
  msgStormControl->ucast_unknown_rate = 0;
  /* Fill output data */
  if (stormControl.flags & PTIN_STORMCONTROL_MASK_BCAST)
  {
    msgStormControl->bcast_rate = stormControl.bcast_rate;
    msgStormControl->mask = MSG_STORMCONTROL_MASK_BCAST;
  }
  if (stormControl.flags & PTIN_STORMCONTROL_MASK_MCAST)
  {
    msgStormControl->mcast_rate = stormControl.mcast_rate;
    msgStormControl->mask = MSG_STORMCONTROL_MASK_MCAST;
  }
  if (stormControl.flags & PTIN_STORMCONTROL_MASK_UCUNK)
  {
    msgStormControl->ucast_unknown_rate = stormControl.ucunk_rate;
    msgStormControl->mask = MSG_STORMCONTROL_MASK_UCUNK;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Message processing finished! (rc=%d)", rc);
  return rc;
}

/**
 * Configure storm control profile
 * 
 * @param msgStormControl : Storm control profile
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_stormControl_set(msg_HwEthStormControl_t *msgStormControl)
{
  ptin_stormControl_t stormControl;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgStormControl==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG," slotId = %u", msgStormControl->SlotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," id     = %u", msgStormControl->id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," flags  = 0x%04x", msgStormControl->flags);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," mask   = 0x%04x", msgStormControl->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," bcast_rate = %u bps", msgStormControl->bcast_rate);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," mcast_rate = %u bps", msgStormControl->mcast_rate);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," ucunk_rate = %u bps", msgStormControl->ucast_unknown_rate);

  /* Input data */
  memset(&stormControl, 0x00, sizeof(ptin_stormControl_t));
  /* Traffic type */
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_BCAST)
  {
    stormControl.bcast_rate = msgStormControl->bcast_rate;
    stormControl.flags |= PTIN_STORMCONTROL_MASK_BCAST;
  }
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_MCAST)
  {
    stormControl.mcast_rate = msgStormControl->mcast_rate;
    stormControl.flags |= PTIN_STORMCONTROL_MASK_MCAST;
  }
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_UCUNK)
  {
    stormControl.ucunk_rate = msgStormControl->ucast_unknown_rate;
    stormControl.flags |= PTIN_STORMCONTROL_MASK_UCUNK;
  }

  /* Add bandwidth profile */
  if ((rc=ptin_evc_stormControl_set(L7_ENABLE, &stormControl))!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error applying storm control profile!");
    return rc;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Message processing finished!  (rc=%d)", rc);
  return rc;
}

/**
 * Clear storm control profile
 * 
 * @param msgStormControl : Storm control profile
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_stormControl_clear(msg_HwEthStormControl_t *msgStormControl)
{
  ptin_stormControl_t stormControl;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgStormControl==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG," slotId = %u", msgStormControl->SlotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," id     = %u", msgStormControl->id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," flags  = 0x%04x", msgStormControl->flags);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," mask   = 0x%04x", msgStormControl->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," bcast_rate = %u bps", msgStormControl->bcast_rate);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," mcast_rate = %u bps", msgStormControl->mcast_rate);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," ucunk_rate = %u bps", msgStormControl->ucast_unknown_rate);

  /* Input data */
  memset(&stormControl, 0x00, sizeof(ptin_stormControl_t));
  /* Traffic type */
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_BCAST)
  {
    stormControl.flags |= PTIN_STORMCONTROL_MASK_BCAST;
  }
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_MCAST)
  {
    stormControl.flags |= PTIN_STORMCONTROL_MASK_MCAST;
  }
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_UCUNK)
  {
    stormControl.flags |= PTIN_STORMCONTROL_MASK_UCUNK;
  }

  /* Add bandwidth profile */
  if ((rc=ptin_evc_stormControl_set(L7_DISABLE, &stormControl))!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error disabling storm control profile!");
    return rc;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Message processing finished!  (rc=%d)", rc);
  return rc;
}

/**
 * Clear storm control profile
 * 
 * @param msgStormControl : 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_stormControl_reset(msg_HwEthStormControl_t *msgStormControl)
{
  ptin_stormControl_t stormControl;
  L7_RC_t   rc = L7_SUCCESS;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgStormControl==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG," slotId = %u", msgStormControl->SlotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," id     = %u", msgStormControl->id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," flags  = 0x%04x", msgStormControl->flags);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," mask   = 0x%04x", msgStormControl->mask);

  /* Input data */
  memset(&stormControl, 0x00, sizeof(ptin_stormControl_t));
  /* Traffic type */
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_BCAST)
  {
    stormControl.flags |= PTIN_STORMCONTROL_MASK_BCAST;
  }
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_MCAST)
  {
    stormControl.flags |= PTIN_STORMCONTROL_MASK_MCAST;
  }
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_UCUNK)
  {
    stormControl.flags |= PTIN_STORMCONTROL_MASK_UCUNK;
  }

  /* Add bandwidth profile */
  if ((rc=ptin_evc_stormControl_reset(&stormControl))!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error clearing storm control profile!");
    return rc;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Message processing finished! (rc=%d)", rc);
  return rc;
}

/* EVC Statistics *********************************************************/

/**
 * Get Traffic Statistics of a specific EVC
 * 
 * @param evcStats : Statistics structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_evcStats_get(msg_evcStats_t *msg_evcStats)
{
  L7_uint32 evcId;
  ptin_evcStats_profile_t  profile;
  ptin_evcStats_counters_t counters;
  L7_RC_t rc=L7_SUCCESS;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msg_evcStats==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG," evcId    = %u",    msg_evcStats->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," mask     = 0x%02x",msg_evcStats->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Intf     = %u/%u", msg_evcStats->intf.intf_type,msg_evcStats->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," SVID     = %u",    msg_evcStats->service_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," CVID     = %u",    msg_evcStats->client_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," ChannelIP= %u",    msg_evcStats->channel_ip);

  /* Clear counters */
  memset(&msg_evcStats->stats,0x00,sizeof(msg_evcStats_counters_t));
  msg_evcStats->mask &= ~((L7_uint8) MSG_EVC_COUNTERS_MASK_STATS);

  /* Extract EVC id */
  evcId = msg_evcStats->evc_id;

  /* Copy data */
  if (ptin_msg_evcStatsStruct_fill(msg_evcStats,&profile)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error filling structure");
    return L7_FAILURE;
  }

  /* Get statistics data */
  rc=ptin_evc_evcStats_get(evcId,&profile,&counters);

  /* Add bandwidth profile */
  if (rc==L7_SUCCESS)
  {
    /* Copy statistics data to message */
    msg_evcStats->stats.mask_stat        = counters.mask;
    /* RX */
    msg_evcStats->stats.rx.pktTotal      = counters.rx.pktTotal;
    msg_evcStats->stats.rx.pktUnicast    = counters.rx.pktUnicast;
    msg_evcStats->stats.rx.pktMulticast  = counters.rx.pktMulticast;
    msg_evcStats->stats.rx.pktBroadcast  = counters.rx.pktBroadcast;
    msg_evcStats->stats.rx.pktDropped    = counters.rx.pktDropped;
    /* TX */
    msg_evcStats->stats.tx.pktTotal      = counters.tx.pktTotal;
    msg_evcStats->stats.tx.pktUnicast    = counters.tx.pktUnicast;
    msg_evcStats->stats.tx.pktMulticast  = counters.tx.pktMulticast;
    msg_evcStats->stats.tx.pktBroadcast  = counters.tx.pktBroadcast;
    msg_evcStats->stats.tx.pktDropped    = counters.tx.pktDropped;
    /* Activate bit in mask: this indicates that the counters are valid */
    msg_evcStats->mask |= MSG_EVC_COUNTERS_MASK_STATS;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"No policy defined to read stats!");
    /* Null values to message */
    //memset(&(msg_evcStats->stats),0x00,sizeof(msg_evcStats_counters_t));
    /* Deactivate bit in mask: this indicates that the counters are NOT valid */
    //msg_evcStats->mask &= ~((L7_uint8) MSG_EVC_COUNTERS_MASK_STATS);
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG," evcId    = %u",    msg_evcStats->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," mask     = 0x%02x",msg_evcStats->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Intf     = %u/%u", msg_evcStats->intf.intf_type,msg_evcStats->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," SVID     = %u",    msg_evcStats->service_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," CVID     = %u",    msg_evcStats->client_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," ChannelIP= %u",    msg_evcStats->channel_ip);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Stat_mask  = 0x%02x",msg_evcStats->stats.mask_stat);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Stats_RX:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"   Total    =%10u", msg_evcStats->stats.rx.pktTotal);
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"   Unicast  =%10u", msg_evcStats->stats.rx.pktUnicast);
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"   Multicast=%10u", msg_evcStats->stats.rx.pktMulticast);
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"   Broadcast=%10u", msg_evcStats->stats.rx.pktBroadcast);
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"   Dropped  =%10u", msg_evcStats->stats.rx.pktDropped);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Stats_TX:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"   Total    =%10u", msg_evcStats->stats.tx.pktTotal);
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"   Unicast  =%10u", msg_evcStats->stats.tx.pktUnicast);
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"   Multicast=%10u", msg_evcStats->stats.tx.pktMulticast);
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"   Broadcast=%10u", msg_evcStats->stats.tx.pktBroadcast);
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"   Dropped  =%10u", msg_evcStats->stats.tx.pktDropped);

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Message processing finished!");
  return L7_SUCCESS;
}

/**
 * Allocate statistics counting for a specific EVC
 * 
 * @param evcStats : Statistics structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_evcStats_set(msg_evcStats_t *msg_evcStats)
{
  L7_uint32 evcId;
  ptin_evcStats_profile_t profile;
  L7_RC_t   rc=L7_SUCCESS;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msg_evcStats==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG," evcId    = %u",    msg_evcStats->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," mask     = 0x%02x",msg_evcStats->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Intf     = %u/%u", msg_evcStats->intf.intf_type,msg_evcStats->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," SVID     = %u",    msg_evcStats->service_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," CVID     = %u",    msg_evcStats->client_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," ChannelIP= 0x%08x",    msg_evcStats->channel_ip);

  /* Extract EVC id */
  evcId = msg_evcStats->evc_id;

  /* Copy data */
  if (ptin_msg_evcStatsStruct_fill(msg_evcStats,&profile)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error filling structure");
    return L7_FAILURE;
  }

  /* Add bandwidth profile */
  if ((rc=ptin_evc_evcStats_set(evcId,&profile))!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error allocating statistics!");
    return rc;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Message processing finished!");
  return L7_SUCCESS;
}

/**
 * Deallocate statistics counting for a specific EVC
 * 
 * @param evcStats : Statistics structure
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_evcStats_delete(msg_evcStats_t *msg_evcStats)
{
  L7_uint32 evcId;
  ptin_evcStats_profile_t profile;
  L7_RC_t   rc=L7_SUCCESS;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msg_evcStats==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG," evcId    = %u",    msg_evcStats->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," mask     = 0x%02x",msg_evcStats->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Intf     = %u/%u", msg_evcStats->intf.intf_type,msg_evcStats->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," SVID     = %u",    msg_evcStats->service_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," CVID     = %u",    msg_evcStats->client_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," ChannelIP= %u",    msg_evcStats->channel_ip);

  /* Extract EVC id */
  evcId = msg_evcStats->evc_id;

  /* Copy data */
  if (ptin_msg_evcStatsStruct_fill(msg_evcStats,&profile)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error filling structure");
    return L7_FAILURE;
  }

  /* Add bandwidth profile */
  rc = ptin_evc_evcStats_delete(evcId,&profile);

  if ( rc != L7_SUCCESS )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error deallocating statistics!");
    return rc;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Message processing finished!");
  return L7_SUCCESS;
}

/* Network Connectivity (inBand) Functions ************************************/
/**
 * Gets Network Connectivity configuration
 * 
 * @param msgNtwConn Pointer to the output data (mask defines what to read)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_ntw_connectivity_get(msg_NtwConnectivity_t *msgNtwConn)
{
  ptin_NtwConnectivity_t ptinNtwConn;
  L7_uint i;
  L7_RC_t rc;

  /* Copy data */
  memset(&ptinNtwConn, 0x00, sizeof(ptinNtwConn));
  ptinNtwConn.mask = msgNtwConn->mask;

  /* Get config */
  rc = ptin_cfg_ntw_connectivity_get(&ptinNtwConn);

  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting inband management config");
    return L7_FAILURE;
  }

  /* Output data */
  LOG_INFO(LOG_CTX_PTIN_MSG, "Network Connectivity (mask=0x%08X)", ptinNtwConn.mask);
  LOG_INFO(LOG_CTX_PTIN_MSG, "  IP Addr         = %u.%u.%u.%u",    (ptinNtwConn.ipaddr  >> 24) & 0xFF, (ptinNtwConn.ipaddr  >> 16) & 0xFF,
           (ptinNtwConn.ipaddr  >>  8) & 0xFF,  ptinNtwConn.ipaddr         & 0xFF);
  LOG_INFO(LOG_CTX_PTIN_MSG, "  Mask            = %u.%u.%u.%u",    (ptinNtwConn.netmask >> 24) & 0xFF, (ptinNtwConn.netmask >> 16) & 0xFF,
           (ptinNtwConn.netmask >>  8) & 0xFF,  ptinNtwConn.netmask        & 0xFF);
  LOG_INFO(LOG_CTX_PTIN_MSG, "  Gateway         = %u.%u.%u.%u",    (ptinNtwConn.gateway >> 24) & 0xFF, (ptinNtwConn.gateway >> 16) & 0xFF,
           (ptinNtwConn.gateway >>  8) & 0xFF,  ptinNtwConn.gateway        & 0xFF);
  LOG_INFO(LOG_CTX_PTIN_MSG, "  Mgmt VLAN ID    = %u",             ptinNtwConn.mgmtVlanId);
  LOG_INFO(LOG_CTX_PTIN_MSG, "  Interfaces (%u)", ptinNtwConn.n_intf);
  for (i=0; i<ptinNtwConn.n_intf; i++)
    LOG_INFO(LOG_CTX_PTIN_MSG, "    %s Intf #    = %u", ptinNtwConn.intf[i].intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", ptinNtwConn.intf[i].intf_id);

  /* Copy data */
  msgNtwConn->mask            = ptinNtwConn.mask;
  msgNtwConn->ipaddr          = ptinNtwConn.ipaddr;
  msgNtwConn->netmask         = ptinNtwConn.netmask;
  msgNtwConn->gateway         = ptinNtwConn.gateway;
  msgNtwConn->mgmtVlanId      = ptinNtwConn.mgmtVlanId;
  msgNtwConn->n_intf          = ptinNtwConn.n_intf;
  for (i=0; i<ptinNtwConn.n_intf; i++)
  {
    msgNtwConn->intf[i].intf_type = ptinNtwConn.intf[i].intf_type;
    msgNtwConn->intf[i].intf_id   = ptinNtwConn.intf[i].intf_id;
  }

  return L7_SUCCESS;
}

/**
 * Sets Network Connectivity configuration
 * 
 * @param msgNtwConn Pointer to the config data (mask defines what to set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_ntw_connectivity_set(msg_NtwConnectivity_t *msgNtwConn)
{
  ptin_NtwConnectivity_t ptinNtwConn;
  L7_uint i;
  L7_RC_t rc;

  /* Copy data */
  ptinNtwConn.mask            = msgNtwConn->mask;
  ptinNtwConn.ipaddr          = msgNtwConn->ipaddr;
  ptinNtwConn.netmask         = msgNtwConn->netmask;
  ptinNtwConn.gateway         = msgNtwConn->gateway;
  ptinNtwConn.mgmtVlanId      = msgNtwConn->mgmtVlanId;
  ptinNtwConn.n_intf          = msgNtwConn->n_intf;
  for (i=0; i<ptinNtwConn.n_intf; i++)
  {
    ptinNtwConn.intf[i].intf_type = msgNtwConn->intf[i].intf_type;
    ptinNtwConn.intf[i].intf_id   = msgNtwConn->intf[i].intf_id;
  }

  /* Output data */
  LOG_INFO(LOG_CTX_PTIN_MSG, "Network Connectivity (mask=0x%08X)", ptinNtwConn.mask);
  LOG_INFO(LOG_CTX_PTIN_MSG, "  IP Addr         = %u.%u.%u.%u",    (ptinNtwConn.ipaddr  >> 24) & 0xFF, (ptinNtwConn.ipaddr  >> 16) & 0xFF,
           (ptinNtwConn.ipaddr  >>  8) & 0xFF,  ptinNtwConn.ipaddr         & 0xFF);
  LOG_INFO(LOG_CTX_PTIN_MSG, "  Mask            = %u.%u.%u.%u",    (ptinNtwConn.netmask >> 24) & 0xFF, (ptinNtwConn.netmask >> 16) & 0xFF,
           (ptinNtwConn.netmask >>  8) & 0xFF,  ptinNtwConn.netmask        & 0xFF);
  LOG_INFO(LOG_CTX_PTIN_MSG, "  Gateway         = %u.%u.%u.%u",    (ptinNtwConn.gateway >> 24) & 0xFF, (ptinNtwConn.gateway >> 16) & 0xFF,
           (ptinNtwConn.gateway >>  8) & 0xFF,  ptinNtwConn.gateway        & 0xFF);
  LOG_INFO(LOG_CTX_PTIN_MSG, "  Mgmt VLAN ID    = %u",             ptinNtwConn.mgmtVlanId);
  LOG_INFO(LOG_CTX_PTIN_MSG, "  Interfaces (%u)", ptinNtwConn.n_intf);
  for (i=0; i<ptinNtwConn.n_intf; i++)
    LOG_INFO(LOG_CTX_PTIN_MSG, "    %s Intf #    = %u", ptinNtwConn.intf[i].intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", ptinNtwConn.intf[i].intf_id);

  /* Apply config */
  rc = ptin_cfg_ntw_connectivity_set(&ptinNtwConn);

  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error setting inband management config");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/* DHCP Management Functions **************************************************/

/**
 * Reconfigure a global DHCP EVC
 *
 * @param dhcpEvcInfo: DHCP EVC info
 *
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_DHCP_evc_reconf(msg_DhcpEvcReconf_t *dhcpEvcInfo)
{
  L7_RC_t   rc;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Processing message");

  /* Validate input parameters */
  if (dhcpEvcInfo==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid parameters");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  ID Type    = %u",      dhcpEvcInfo->idType);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  ID         = %u",      dhcpEvcInfo->id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask       = 0x%04X",  dhcpEvcInfo->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  DHCP Flag  = %u",      dhcpEvcInfo->dhcp_flag);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Options    = 0x%04X",  dhcpEvcInfo->options);

  if (dhcpEvcInfo->idType == MSG_ID_DEF_TYPE ||
      dhcpEvcInfo->idType == MSG_ID_EVC_TYPE)
  {
    rc = ptin_dhcp_reconf_evc(dhcpEvcInfo->id, dhcpEvcInfo->dhcp_flag, dhcpEvcInfo->options);
    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error reconfiguring global DHCP EVC");
      return rc;
    }
    rc = ptin_pppoe_reconf_evc(dhcpEvcInfo->id, dhcpEvcInfo->dhcp_flag, dhcpEvcInfo->options);
  }
  else if (dhcpEvcInfo->idType == MSG_ID_NNIVID_TYPE)
  {
    rc = ptin_dhcp_reconf_rootVid(dhcpEvcInfo->id, dhcpEvcInfo->dhcp_flag, dhcpEvcInfo->options);
    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error reconfiguring global DHCP Instance");
      return rc;
    }
    rc = ptin_pppoe_reconf_rootVid(dhcpEvcInfo->id, dhcpEvcInfo->dhcp_flag, dhcpEvcInfo->options);
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid id %u", dhcpEvcInfo->idType);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Set DHCP circuit-id global data
 *
 * @param profile: DHCP profile
 *
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_DHCP_circuitid_set(msg_AccessNodeCircuitId_t *circuitid)
{
  L7_RC_t rc;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Processing message");

  /* Validate input parameters */
  if (circuitid==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid parameters");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Id_type            = %u",      circuitid->id_ref.id_type);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Id value           = %u",      circuitid->id_ref.id_val.evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Template           = %s",      circuitid->template_str);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  CircuitId Mask     = 0x%04X",  circuitid->mask_circuitid);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  AccessNode ID      = %s",      circuitid->access_node_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Chassis            = %u",      circuitid->chassis);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Rack               = %u",      circuitid->rack);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Frame              = %u",      circuitid->frame);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Ethernet Priority  = %u",      circuitid->ethernet_priority);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  S-VID              = %u",      circuitid->s_vid);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Flags Mask         = 0x%02X",  circuitid->mask_flags);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Flags              = 0x%02X",  circuitid->broadcast_flag);

  /* TODO: To be reworked */

  /* Set circuit-id global data */
  if (circuitid->id_ref.id_type == MSG_ID_DEF_TYPE ||
      circuitid->id_ref.id_type == MSG_ID_EVC_TYPE)
  {
    /* Circuit id */
    rc = ptin_dhcp_circuitid_set_evc(circuitid->id_ref.id_val.evc_id, circuitid->template_str, circuitid->mask_circuitid, circuitid->access_node_id, circuitid->chassis, circuitid->rack,
                                     circuitid->frame, circuitid->ethernet_priority, circuitid->s_vid);
    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error configuring DHCP circuit-id");
      return rc;
    }
    /* Flags */
    rc = ptin_dhcp_evc_flags_set(circuitid->id_ref.id_val.evc_id, circuitid->mask_flags, circuitid->broadcast_flag);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error configuring DHCP flags");
      return rc;
    }

    rc = ptin_pppoe_circuitid_set_evc(circuitid->id_ref.id_val.evc_id, circuitid->template_str, circuitid->mask_circuitid, circuitid->access_node_id, circuitid->chassis, circuitid->rack,
                                      circuitid->frame, circuitid->ethernet_priority, circuitid->s_vid);

  }
  else if (circuitid->id_ref.id_type == MSG_ID_NNIVID_TYPE)
  {
    rc = ptin_dhcp_circuitid_set_nniVid(circuitid->id_ref.id_val.nni_vid, circuitid->template_str, circuitid->mask_circuitid, circuitid->access_node_id, circuitid->chassis, circuitid->rack,
                                        circuitid->frame, circuitid->ethernet_priority, circuitid->s_vid);
    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error configuring DHCP circuit-id");
      return rc;
    }
    /* Flags */
    rc = ptin_dhcp_nniVid_flags_set(circuitid->id_ref.id_val.nni_vid, circuitid->mask_flags, circuitid->broadcast_flag);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error configuring DHCP flags");
      return rc;
    }

    rc = ptin_pppoe_circuitid_set_nniVid(circuitid->id_ref.id_val.nni_vid, circuitid->template_str, circuitid->mask_circuitid, circuitid->access_node_id, circuitid->chassis, circuitid->rack,
                                         circuitid->frame, circuitid->ethernet_priority, circuitid->s_vid);    
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid id %u", circuitid->id_ref.id_type);
    return L7_FAILURE;
  }

  /* TODO */
#if 0
  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error configuring circuit-id global data");
    return rc;
  }
#endif

  return L7_SUCCESS;
}

/**
 * Get DHCP circuit-id global data
 *
 * @param profile: DHCP profile
 *
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_DHCP_circuitid_get(msg_AccessNodeCircuitId_t *circuitid)
{
  L7_RC_t rc;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Processing message");

  /* Validate input parameters */
  if (circuitid==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid parameters");
    return L7_FAILURE;
  }

  /* Set circuit-id global data */
  if (circuitid->id_ref.id_type == MSG_ID_DEF_TYPE ||
      circuitid->id_ref.id_type == MSG_ID_EVC_TYPE)
  {
    /* Circuit id */
    rc = ptin_dhcp_circuitid_get(circuitid->id_ref.id_val.evc_id, circuitid->template_str, &circuitid->mask_circuitid,
                                 circuitid->access_node_id, &circuitid->chassis, &circuitid->rack,
                                 &circuitid->frame, &circuitid->ethernet_priority, &circuitid->s_vid);
    /* DHCP flags */
    if (rc == L7_SUCCESS)
    {
      rc = ptin_dhcp_evc_flags_get(circuitid->id_ref.id_val.evc_id, &circuitid->mask_flags, &circuitid->broadcast_flag);
    }
  }
  else if (circuitid->id_ref.id_type == MSG_ID_NNIVID_TYPE)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Not supported yet");
    return L7_FAILURE;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid id %u", circuitid->id_ref.id_type);
    return L7_FAILURE;
  }

  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error configuring circuit-id global data");
    return rc;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  ID type            = %u",      circuitid->id_ref.id_type);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  ID value           = %u",      circuitid->id_ref.id_val);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Template           = %s",      circuitid->template_str);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  CircuitId Mask     = 0x%04X",  circuitid->mask_circuitid);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  AccessNode ID      = %s",      circuitid->access_node_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Chassis            = %u",      circuitid->chassis);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Rack               = %u",      circuitid->rack);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Frame              = %u",      circuitid->frame);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Ethernet Priority  = %u",      circuitid->ethernet_priority);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  S-VID              = %u",      circuitid->s_vid);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Flags Mask         = 0x%02X",  circuitid->mask_flags);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Flags              = 0x%02X",  circuitid->broadcast_flag);

  return L7_SUCCESS;
}

/**
 * Get DHCP profile data
 * 
 * @param profile: DHCP profile
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_DHCP_profile_get(msg_HwEthernetDhcpOpt82Profile_t *profile)
{
  L7_uint32               evc_idx;
  ptin_client_id_t        client;
  ptin_clientCircuitId_t  circuitId_data;
  L7_RC_t           rc;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Processing message");

  /* Validate input parameters */
  if (profile==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid parameters");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Slot Id      = %u",     profile->SlotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  evc_idx      = %u",     profile->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask         = 0x%02x", profile->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Interface    = %u/%u",  profile->intf.intf_type,profile->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Mask  = 0x%02x", profile->client.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.OVlan = %u",     profile->client.outer_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.IVlan = %u",     profile->client.inner_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Intf  = %u/%u",  profile->client.intf.intf_type, profile->client.intf.intf_id);

  /* Extract input data */
  evc_idx = profile->evc_id;

  memset(&client,0x00,sizeof(ptin_client_id_t));
  if (profile->client.mask & MSG_CLIENT_OVLAN_MASK)
  {
    client.outerVlan = profile->client.outer_vlan;
    client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  }
  if (profile->client.mask & MSG_CLIENT_IVLAN_MASK)
  {
    client.innerVlan = profile->client.inner_vlan;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
  }
  if (profile->client.mask & MSG_CLIENT_INTF_MASK)
  {
    client.ptin_intf.intf_type  = profile->client.intf.intf_type;
    client.ptin_intf.intf_id    = profile->client.intf.intf_id;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  }

  /* Get circuit and remote ids */
  rc = ptin_dhcp_client_get(evc_idx, &client, &profile->options, &circuitId_data, L7_NULLPTR, profile->remoteId);

  profile->circuitId.onuid  = circuitId_data.onuid;
  profile->circuitId.slot   = circuitId_data.slot;
  profile->circuitId.port   = circuitId_data.port;
  profile->circuitId.q_vid  = circuitId_data.q_vid;
  profile->circuitId.c_vid  = circuitId_data.c_vid;

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error obtaining circuit and remote ids");
    return rc;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Options                      = %02x",  profile->options);
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"CircuitId.onuid              = %u",    profile->circuitId.onuid);
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"CircuitId.slot               = %u",    profile->circuitId.slot);
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"CircuitId.port               = %u",    profile->circuitId.port);
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"CircuitId.q_vid              = %u",    profile->circuitId.q_vid);
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"CircuitId.c_vid              = %u",    profile->circuitId.c_vid);
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"RemoteId                     = \"%s\"",profile->remoteId);

  return L7_SUCCESS;
}

/**
 * Add a new DHCP profile
 * 
 * @param profile: DHCP profile
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_DHCP_profile_add(msg_HwEthernetDhcpOpt82Profile_t *profile, L7_uint n_clients)
{
  L7_uint32               i, evc_idx;
  ptin_client_id_t        client;
  ptin_clientCircuitId_t  circuitId;
  L7_RC_t                 rc = L7_SUCCESS;

  /* Validate input parameters */
  if (profile==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid parameters");
    return L7_FAILURE;
  }

  for (i=0; i<n_clients; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Processing message %u",i);

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Slot Id                      = %u",     profile[i].SlotId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  evc_idx                      = %u",     profile[i].evc_id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask                         = 0x%02x", profile[i].mask);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Interface                    = %u/%u",  profile[i].intf.intf_type,profile[i].intf.intf_id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Mask                  = 0x%02x", profile[i].client.mask);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.OVlan                 = %u",     profile[i].client.outer_vlan);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.IVlan                 = %u",     profile[i].client.inner_vlan);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Intf                  = %u/%u",  profile[i].client.intf.intf_type, profile[i].client.intf.intf_id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Options                      = %04x",   profile[i].options);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  CircuitId.onuid              = %u",     profile[i].circuitId.onuid);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  CircuitId.slot               = %u",     profile[i].circuitId.slot);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  CircuitId.port               = %u",     profile[i].circuitId.port);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  CircuitId.q_vid              = %u",     profile[i].circuitId.q_vid);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Remote Id                    = \"%s\"", profile[i].remoteId);

    /* Check if all UseGlobal_DHCP_options match */
    if ( ((profile[i].options & 0x02) >> 1) != ((profile[i].options & 0x08) >> 3) )
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error: UseGlobal_DHCP_options do not match");
      return L7_FAILURE;
    }
    if ( ((profile[i].options & 0x08) >> 3) != ((profile[i].options & 0x20) >> 5) )
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error: UseGlobal_DHCP_options do not match");
      return L7_FAILURE;
    }

    /* Extract input data */
    evc_idx = profile[i].evc_id;

    memset(&client,0x00,sizeof(ptin_client_id_t));
    if (profile[i].client.mask & MSG_CLIENT_OVLAN_MASK)
    {
      client.outerVlan = profile[i].client.outer_vlan;
      client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
    }
    if (profile[i].client.mask & MSG_CLIENT_IVLAN_MASK)
    {
      client.innerVlan = profile[i].client.inner_vlan;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
    }
    if (profile[i].client.mask & MSG_CLIENT_INTF_MASK)
    {
      client.ptin_intf.intf_type  = profile[i].client.intf.intf_type;
      client.ptin_intf.intf_id    = profile[i].client.intf.intf_id;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
    }

    /* TODO: To be reworked */
    circuitId.onuid   = profile[i].circuitId.onuid;
    circuitId.slot    = profile[i].circuitId.slot;
    circuitId.port    = profile[i].circuitId.port;
    circuitId.q_vid   = profile[i].circuitId.q_vid;
    circuitId.c_vid   = profile[i].circuitId.c_vid;

    /* Add circuit and remote ids */
    rc = ptin_dhcp_client_add(evc_idx, &client, 0, 0, profile[i].options, &circuitId, profile[i].remoteId);

    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error adding DHCP circuitId+remoteId entry");
      return rc;
    }

    rc = ptin_pppoe_client_add(evc_idx, &client, 0, 0, profile[i].options, &circuitId, profile[i].remoteId);
    /* TODO */
#if 0
    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error adding PPPoE circuitId+remoteId entry");
      return rc;
    }
#endif
  }

  return L7_SUCCESS;
}

/**
 * Remove DHCP profile
 * 
 * @param profile: DHCP profile
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_DHCP_profile_remove(msg_HwEthernetDhcpOpt82Profile_t *profile, L7_uint n_clients)
{
  L7_uint32         i, evc_idx;
  ptin_client_id_t  client;
  L7_RC_t           rc;

  /* Validate input parameters */
  if (profile==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid parameters");
    return L7_FAILURE;
  }

  for (i=0; i<n_clients; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Processing message %u",i);

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Slot Id      = %u",     profile[i].SlotId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  evc_idx      = %u",     profile[i].evc_id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask         = 0x%02x", profile[i].mask);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Interface    = %u/%u",  profile[i].intf.intf_type,profile[i].intf.intf_id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Mask  = 0x%02x", profile[i].client.mask);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.OVlan = %u",     profile[i].client.outer_vlan);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.IVlan = %u",     profile[i].client.inner_vlan);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Intf  = %u/%u",  profile[i].client.intf.intf_type, profile[i].client.intf.intf_id);

    /* Extract input data */
    evc_idx = profile[i].evc_id;

    memset(&client,0x00,sizeof(ptin_client_id_t));
    if (profile[i].client.mask & MSG_CLIENT_OVLAN_MASK)
    {
      client.outerVlan = profile[i].client.outer_vlan;
      client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
    }
    if (profile[i].client.mask & MSG_CLIENT_IVLAN_MASK)
    {
      client.innerVlan = profile[i].client.inner_vlan;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
    }
    if (profile[i].client.mask & MSG_CLIENT_INTF_MASK)
    {
      client.ptin_intf.intf_type  = profile[i].client.intf.intf_type;
      client.ptin_intf.intf_id    = profile[i].client.intf.intf_id;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
    }

    /* TODO: To be reworked */

    /* Remove circuitId+remoteId entry */
    rc = ptin_dhcp_client_delete(evc_idx,&client);
    if ( rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error removing DHCP circuitId+remoteId entry");
      return rc;
    }
    rc = ptin_pppoe_client_delete(evc_idx,&client);
    /* TODO */
#if 0
    if ( rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error removing PPPoE circuitId+remoteId entry");
      return rc;
    }
#endif
  }

  return L7_SUCCESS;
}

/**
 * Get DHCP counters at a specific client
 * 
 * @param stats: statistics information
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_DHCP_clientStats_get(msg_DhcpClientStatistics_t *dhcp_stats)
{
  ptin_client_id_t        client;
  ptin_DHCP_Statistics_t  stats;
  L7_RC_t                 rc;

  if (dhcp_stats==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Reading client DHCP stats:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  evc_idx      = %u",     dhcp_stats->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask         = 0x%02x", dhcp_stats->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Interface    = %u/%u",  dhcp_stats->intf.intf_type,dhcp_stats->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Mask  = 0x%02x", dhcp_stats->client.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.OVlan = %u",     dhcp_stats->client.outer_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.IVlan = %u",     dhcp_stats->client.inner_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Intf  = %u/%u",  dhcp_stats->client.intf.intf_type, dhcp_stats->client.intf.intf_id);

  /* Evaluate provided data */
  if ( dhcp_stats->evc_id==(L7_uint16)-1 ||
       !(dhcp_stats->mask & MSG_CLIENT_MASK) ||
       (dhcp_stats->client.mask == 0x00) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "MC EVC and client reference must be provided");
    return L7_FAILURE;
  }

  memset(&client,0x00,sizeof(ptin_client_id_t));
  if (dhcp_stats->client.mask & MSG_CLIENT_OVLAN_MASK)
  {
    client.outerVlan = dhcp_stats->client.outer_vlan;
    client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  }
  if (dhcp_stats->client.mask & MSG_CLIENT_IVLAN_MASK)
  {
    client.innerVlan = dhcp_stats->client.inner_vlan;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
  }
  if (dhcp_stats->client.mask & MSG_CLIENT_INTF_MASK)
  {
    client.ptin_intf.intf_type  = dhcp_stats->client.intf.intf_type;
    client.ptin_intf.intf_id    = dhcp_stats->client.intf.intf_id;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  }

  /* Get statistics */
  rc = ptin_dhcp_stat_client_get(dhcp_stats->evc_id,&client,&stats);

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting client statistics");
    return rc;
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success getting client statistics");
  }

  /* Return data */
  dhcp_stats->stats.dhcp_rx_intercepted                           = stats.dhcp_rx_intercepted;
  dhcp_stats->stats.dhcp_rx                                       = stats.dhcp_rx;
  dhcp_stats->stats.dhcp_rx_filtered                              = stats.dhcp_rx_filtered;
  dhcp_stats->stats.dhcp_tx_forwarded                             = stats.dhcp_tx_forwarded;
  dhcp_stats->stats.dhcp_tx_failed                                = stats.dhcp_tx_failed;

  dhcp_stats->stats.dhcp_rx_client_requests_without_options       = stats.dhcp_rx_client_requests_without_options;
#if 0 /* PTin Daniel OLTTS-4141 - Removed to ensure API compatibility with manager in 3.3.0 */
  dhcp_stats->stats.dhcp_tx_client_requests_without_options       = stats.dhcp_tx_client_requests_without_options;
#endif
  dhcp_stats->stats.dhcp_tx_client_requests_with_option82         = stats.dhcp_tx_client_requests_with_option82;
  dhcp_stats->stats.dhcp_tx_client_requests_with_option37         = stats.dhcp_tx_client_requests_with_option37;
  dhcp_stats->stats.dhcp_tx_client_requests_with_option18         = stats.dhcp_tx_client_requests_with_option18;
  dhcp_stats->stats.dhcp_rx_server_replies_with_option82          = stats.dhcp_rx_server_replies_with_option82;
  dhcp_stats->stats.dhcp_rx_server_replies_with_option37          = stats.dhcp_rx_server_replies_with_option37;
  dhcp_stats->stats.dhcp_rx_server_replies_with_option18          = stats.dhcp_rx_server_replies_with_option18;
#if 0 /* PTin Daniel OLTTS-4141 - Removed to ensure API compatibility with manager in 3.3.0 */
  dhcp_stats->stats.dhcp_rx_server_replies_without_options        = stats.dhcp_rx_server_replies_without_options;
#endif
  dhcp_stats->stats.dhcp_tx_server_replies_without_options        = stats.dhcp_tx_server_replies_without_options;

  dhcp_stats->stats.dhcp_rx_client_pkts_onTrustedIntf             = stats.dhcp_rx_client_pkts_onTrustedIntf;
  dhcp_stats->stats.dhcp_rx_client_pkts_withOps_onUntrustedIntf   = stats.dhcp_rx_client_pkts_withOps_onUntrustedIntf;
  dhcp_stats->stats.dhcp_rx_server_pkts_onUntrustedIntf           = stats.dhcp_rx_server_pkts_onUntrustedIntf;
#if 1 /* PTin Daniel OLTTS-4141 - Added to ensure API compatibility with manager in 3.3.0 */
  dhcp_stats->stats.dhcp_rx_server_pkts_withoutOps_onTrustedIntf  = 0;
#endif

  return L7_SUCCESS;
}

/**
 * Clear DHCP counters at a specific client
 * 
 * @param stats: statistics information
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_DHCP_clientStats_clear(msg_DhcpClientStatistics_t *dhcp_stats)
{
  ptin_client_id_t  client;
  L7_RC_t           rc;

  if (dhcp_stats==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Clearing client DHCP stats:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  evc_idx      = %u",     dhcp_stats->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask         = 0x%02x", dhcp_stats->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Interface    = %u/%u",  dhcp_stats->intf.intf_type,dhcp_stats->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Mask  = 0x%02x", dhcp_stats->client.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.OVlan = %u",     dhcp_stats->client.outer_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.IVlan = %u",     dhcp_stats->client.inner_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Intf  = %u/%u",  dhcp_stats->client.intf.intf_type, dhcp_stats->client.intf.intf_id);

  /* Evaluate provided data */
  if ( dhcp_stats->evc_id==(L7_uint16)-1 ||
       !(dhcp_stats->mask & MSG_CLIENT_MASK) ||
       (dhcp_stats->client.mask == 0x00) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "MC EVC and client reference must be provided");
    return L7_FAILURE;
  }

  memset(&client,0x00,sizeof(ptin_client_id_t));
  if (dhcp_stats->client.mask & MSG_CLIENT_OVLAN_MASK)
  {
    client.outerVlan = dhcp_stats->client.outer_vlan;
    client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  }
  if (dhcp_stats->client.mask & MSG_CLIENT_IVLAN_MASK)
  {
    client.innerVlan = dhcp_stats->client.inner_vlan;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
  }
  if (dhcp_stats->client.mask & MSG_CLIENT_INTF_MASK)
  {
    client.ptin_intf.intf_type  = dhcp_stats->client.intf.intf_type;
    client.ptin_intf.intf_id    = dhcp_stats->client.intf.intf_id;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  }

  /* Clear client stats */
  rc = ptin_dhcp_stat_client_clear(dhcp_stats->evc_id,&client);

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error clearing client statistics");
    return rc;
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success clearing client statistics");
  }

  return L7_SUCCESS;
}

/**
 * Get DHCP counters at a specific interface
 * 
 * @param stats: statistics information
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_DHCP_intfStats_get(msg_DhcpClientStatistics_t *dhcp_stats)
{
  ptin_intf_t             ptin_intf;
  ptin_DHCP_Statistics_t  stats;
  L7_RC_t                 rc;

  if (dhcp_stats==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Reading interface DHCP stats:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  evc_idx      = %u",     dhcp_stats->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask         = 0x%02x", dhcp_stats->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Interface    = %u/%u",  dhcp_stats->intf.intf_type,dhcp_stats->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Mask  = 0x%02x", dhcp_stats->client.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.OVlan = %u",     dhcp_stats->client.outer_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.IVlan = %u",     dhcp_stats->client.inner_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Intf  = %u/%u",  dhcp_stats->client.intf.intf_type, dhcp_stats->client.intf.intf_id);

  /* Evaluate provided data */
  if ( !(dhcp_stats->mask & MSG_INTERFACE_MASK) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "At least, interface must be provided");
    return L7_FAILURE;
  }

  ptin_intf.intf_type = dhcp_stats->intf.intf_type;
  ptin_intf.intf_id   = dhcp_stats->intf.intf_id;

  /* Get statistics */
  if (dhcp_stats->evc_id==(L7_uint16)-1)
  {
    rc = ptin_dhcp_stat_intf_get(&ptin_intf,&stats);

    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting global interface statistics");
      return rc;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success getting global interface statistics");
    }
  }
  else
  {
    rc = ptin_dhcp_stat_instanceIntf_get(dhcp_stats->evc_id,&ptin_intf,&stats);

    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting statistics struct");
      return rc;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success getting interface statistics of one DHCP instance");
    }
  }

  /* Return data */
  dhcp_stats->stats.dhcp_rx_intercepted                           = stats.dhcp_rx_intercepted;
  dhcp_stats->stats.dhcp_rx                                       = stats.dhcp_rx;
  dhcp_stats->stats.dhcp_rx_filtered                              = stats.dhcp_rx_filtered;
  dhcp_stats->stats.dhcp_tx_forwarded                             = stats.dhcp_tx_forwarded;
  dhcp_stats->stats.dhcp_tx_failed                                = stats.dhcp_tx_failed;

  dhcp_stats->stats.dhcp_rx_client_requests_without_options       = stats.dhcp_rx_client_requests_without_options;
#if 0 /* PTin Daniel OLTTS-4141 - Removed to ensure API compatibility with manager in 3.3.0 */
  dhcp_stats->stats.dhcp_tx_client_requests_without_options       = stats.dhcp_tx_client_requests_without_options;
#endif
  dhcp_stats->stats.dhcp_tx_client_requests_with_option82         = stats.dhcp_tx_client_requests_with_option82;
  dhcp_stats->stats.dhcp_tx_client_requests_with_option37         = stats.dhcp_tx_client_requests_with_option37;
  dhcp_stats->stats.dhcp_tx_client_requests_with_option18         = stats.dhcp_tx_client_requests_with_option18;
  dhcp_stats->stats.dhcp_rx_server_replies_with_option82          = stats.dhcp_rx_server_replies_with_option82;
  dhcp_stats->stats.dhcp_rx_server_replies_with_option37          = stats.dhcp_rx_server_replies_with_option37;
  dhcp_stats->stats.dhcp_rx_server_replies_with_option18          = stats.dhcp_rx_server_replies_with_option18;
#if 0 /* PTin Daniel OLTTS-4141 - Removed to ensure API compatibility with manager in 3.3.0 */
  dhcp_stats->stats.dhcp_rx_server_replies_without_options        = stats.dhcp_rx_server_replies_without_options;
#endif
  dhcp_stats->stats.dhcp_tx_server_replies_without_options        = stats.dhcp_tx_server_replies_without_options;

  dhcp_stats->stats.dhcp_rx_client_pkts_onTrustedIntf             = stats.dhcp_rx_client_pkts_onTrustedIntf;
  dhcp_stats->stats.dhcp_rx_client_pkts_withOps_onUntrustedIntf   = stats.dhcp_rx_client_pkts_withOps_onUntrustedIntf;
  dhcp_stats->stats.dhcp_rx_server_pkts_onUntrustedIntf           = stats.dhcp_rx_server_pkts_onUntrustedIntf;
#if 1 /* PTin Daniel OLTTS-4141 - Added to ensure API compatibility with manager in 3.3.0 */
  dhcp_stats->stats.dhcp_rx_server_pkts_withoutOps_onTrustedIntf  = 0;
#endif

  return L7_SUCCESS;
}

/**
 * Clear DHCP counters at a specific interface
 * 
 * @param stats: statistics information
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_DHCP_intfStats_clear(msg_DhcpClientStatistics_t *dhcp_stats)
{
  ptin_intf_t ptin_intf;
  L7_RC_t     rc;

  if (dhcp_stats==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Clearing interface DHCP stats:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  evc_idx      = %u",     dhcp_stats->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask         = 0x%02x", dhcp_stats->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Interface    = %u/%u",  dhcp_stats->intf.intf_type,dhcp_stats->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Mask  = 0x%02x", dhcp_stats->client.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.OVlan = %u",     dhcp_stats->client.outer_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.IVlan = %u",     dhcp_stats->client.inner_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Intf  = %u/%u",  dhcp_stats->client.intf.intf_type, dhcp_stats->client.intf.intf_id);

  ptin_intf.intf_type = dhcp_stats->intf.intf_type;
  ptin_intf.intf_id   = dhcp_stats->intf.intf_id;

  /* MC EVC not provided */
  if (dhcp_stats->evc_id==(L7_uint16)-1)
  {
    /* Interface not provided */
    if ( !(dhcp_stats->mask & MSG_INTERFACE_MASK) )
    {
      /* Clear all stats */
      rc = ptin_dhcp_stat_clearAll();

      if (rc!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "Error clearing all statistics data");
        return rc;
      }
      else
      {
        LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success clearing all statistics data");
      }
    }
    /* Interface provided */
    else
    {
      /* Clear all stats of one interface */
      rc = ptin_dhcp_stat_intf_clear(&ptin_intf);

      if (rc!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "Error clearing statistics of one complete interface");
        return rc;
      }
      else
      {
        LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success clearing statistics of one complete interface");
      }
    }
  }
  /* MC EVC provided */
  else
  {
    /* Interface not provided */
    if ( !(dhcp_stats->mask & MSG_INTERFACE_MASK) )
    {
      /* Clear stats of one dhcp instance */
      rc = ptin_dhcp_stat_instance_clear(dhcp_stats->evc_id);

      if (rc!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "Error clearing statistics of one DHCP instance");
        return rc;
      }
      else
      {
        LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success clearing statistics of one DHCP instance");
      }
    }
    /* Interface provided */
    else
    {
      /* Clear stats of one dhcp instance and one interface */
      rc = ptin_dhcp_stat_instanceIntf_clear(dhcp_stats->evc_id,&ptin_intf);

      if (rc!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "Error clearing statistics of one DHCP instance + interface");
        return rc;
      }
      else
      {
        LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success clearing statistics of one DHCP instance + interface");
      }
    }
  }

  return L7_SUCCESS;
}

/**
 * Get DHCP bind table
 * 
 * @param table: bind table entries
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
static L7_uint16 dhcp_bindtable_entries = 0;
static ptin_DHCPv4v6_bind_entry dhcpv4v6_bindtable[PLAT_MAX_FDB_MAC_ENTRIES];

L7_RC_t ptin_msg_DHCPv4v6_bindTable_get(msg_DHCP_bind_table_request_t *input, msg_DHCPv4v6_bind_table_t *output)
{
  L7_uint32 i, page, first, entries, size;
  L7_RC_t   rc;

  /* Debug */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Binding table get:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  SlotId = %u",   input->slotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Page   = %u",   input->page);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask   = %02X", input->mask);

  page = input->page;

  // For index null, read all mac entries
  if (page==0)
  {
    size = PLAT_MAX_FDB_MAC_ENTRIES;

    rc = ptin_dhcpv4v6_bindtable_get(dhcpv4v6_bindtable,&size);

    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error reading binding table");
      return rc;
    }
    // Total number of entries
    dhcp_bindtable_entries = size;
  }

  // Validate page index
  if ((page*128)>dhcp_bindtable_entries)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Requested page exceeds binding table size (table size=%u, page=%u)",dhcp_bindtable_entries,page);
    return L7_FAILURE;
  }

  first   = page*128;
  entries = dhcp_bindtable_entries-first;   // Calculate remaining entries to be read
  if (entries>128)  
  {
    entries = 128;          // Overgoes 128? If so, limit to 128
  }
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "There at least %u entries left", entries);

  output->bind_table_msg_size      = entries;
  output->bind_table_total_entries = dhcp_bindtable_entries;

  // Copy binding table entries
  for (i=0; i<entries; ++i)
  {
//  memset(&output->bind_table[i],0x00,sizeof(msg_DHCP_bind_entry));

    output->bind_table[i].entry_index    = dhcpv4v6_bindtable[first+i].entry_index;
    output->bind_table[i].evc_idx        = dhcpv4v6_bindtable[first+i].evc_idx;
    output->bind_table[i].intf.intf_type = dhcpv4v6_bindtable[first+i].ptin_intf.intf_type;
    output->bind_table[i].intf.intf_id   = dhcpv4v6_bindtable[first+i].ptin_intf.intf_id;
    output->bind_table[i].outer_vlan     = dhcpv4v6_bindtable[first+i].outer_vlan;
    output->bind_table[i].inner_vlan     = dhcpv4v6_bindtable[first+i].inner_vlan;
    memcpy(output->bind_table[i].macAddr, dhcpv4v6_bindtable[first+i].macAddr, sizeof(L7_uint8)*6);
    memcpy(&output->bind_table[i].ipAddr, &dhcpv4v6_bindtable[first+i].ipAddr, sizeof(chmessage_ip_addr_t));
    output->bind_table[i].remLeave       = dhcpv4v6_bindtable[first+i].remLeave;
    output->bind_table[i].bindingType    = dhcpv4v6_bindtable[first+i].bindingType;

    LOG_TRACE(LOG_CTX_PTIN_MSG, "Entry %u:", first+i);
    LOG_TRACE(LOG_CTX_PTIN_MSG, "  entry_index = %u",    output->bind_table[i].entry_index);
    LOG_TRACE(LOG_CTX_PTIN_MSG, "  evc_idx     = %u",    output->bind_table[i].evc_idx);
    LOG_TRACE(LOG_CTX_PTIN_MSG, "  intf        = %u/%u", output->bind_table[i].intf.intf_type, output->bind_table[i].intf.intf_id);
    LOG_TRACE(LOG_CTX_PTIN_MSG, "  outer_vlan  = %u",    output->bind_table[i].outer_vlan);
    LOG_TRACE(LOG_CTX_PTIN_MSG, "  inner_vlan  = %u",    output->bind_table[i].inner_vlan);
    LOG_TRACE(LOG_CTX_PTIN_MSG, "  macAddr     = %02X:%02X:%02X:%02X:%02X:%02X", output->bind_table[i].macAddr[0], output->bind_table[i].macAddr[1], 
              output->bind_table[i].macAddr[2], output->bind_table[i].macAddr[3], output->bind_table[i].macAddr[4], output->bind_table[i].macAddr[5]);
    LOG_TRACE(LOG_CTX_PTIN_MSG, "  ipAddr      = %08X",  output->bind_table[i].ipAddr);
    LOG_TRACE(LOG_CTX_PTIN_MSG, "  remLeave    = %u",    output->bind_table[i].remLeave);
    LOG_TRACE(LOG_CTX_PTIN_MSG, "  bindingType = %u",    output->bind_table[i].bindingType);
  }

  return L7_SUCCESS;
}

/**
 * Remove a DHCP bind table entry
 * 
 * @param table: bind table entries
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_DHCP_bindTable_remove(msg_DHCPv4v6_bind_table_t *table)
{
  L7_uint16           i, i_max;
  dhcpSnoopBinding_t  dsBinding;
  L7_RC_t             rc;

  i_max = table->bind_table_msg_size;
  if (i_max>128)  i_max = 128;

  for (i=0; i<i_max ; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Evc_idx=%u",table->bind_table[i].evc_idx);
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Port   = %u/%u",table->bind_table[i].intf.intf_type,table->bind_table[i].intf.intf_id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"OVlan  = %u",table->bind_table[i].outer_vlan);
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"IVlan  = %u",table->bind_table[i].inner_vlan);
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"MacAddr=%02X:%02X:%02X:%02X:%02X:%02X",
              table->bind_table[i].macAddr[0],
              table->bind_table[i].macAddr[1],
              table->bind_table[i].macAddr[2],
              table->bind_table[i].macAddr[3],
              table->bind_table[i].macAddr[4],
              table->bind_table[i].macAddr[5]);

    memset(&dsBinding,0x00,sizeof(dhcpSnoopBinding_t));
    memcpy(dsBinding.macAddr,table->bind_table[i].macAddr,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);

    rc = ptin_dhcp82_bindtable_remove(&dsBinding);

    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error removing entry");
      return rc;
    }
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Success removing entry");
  }

  return L7_SUCCESS;
}


/*IP Source Guard Management Functions **************************************************/

L7_RC_t fp_to_ptin_ip_notation(L7_inet_addr_t *fpIpAddr, chmessage_ip_addr_t *ptinIpAddr)
{
   if ( fpIpAddr->family == L7_AF_INET)
   {
      ptinIpAddr->family = PTIN_AF_INET;
      ptinIpAddr->addr.ipv4 = fpIpAddr->addr.ipv4.s_addr;      

      return L7_SUCCESS;
   }
   else if ( fpIpAddr->family == L7_AF_INET6)
   {
      ptinIpAddr->family = PTIN_AF_INET6;
      memcpy(ptinIpAddr->addr.ipv6, fpIpAddr->addr.ipv6.in6.addr8, L7_IP6_ADDR_LEN*sizeof(L7_uchar8));

      return L7_SUCCESS;
   }
   else
   {
     LOG_ERR(LOG_CTX_PTIN_MSG, "IP Family Address of FP not Supported:%u",fpIpAddr->family);
     return L7_NOT_SUPPORTED;
   }   
}

L7_RC_t ptin_to_fp_ip_notation(chmessage_ip_addr_t *ptinIpAddr, L7_inet_addr_t *fpIpAddr)
{
   if ( ptinIpAddr->family == PTIN_AF_INET )
   {      
     inetAddressSet(L7_AF_INET, &ptinIpAddr->addr.ipv4, fpIpAddr);          
     return L7_SUCCESS;
   }
   else if ( ptinIpAddr->family == PTIN_AF_INET6 )
   { 
     fpIpAddr->family = L7_AF_INET6;
     memcpy(fpIpAddr->addr.ipv6.in6.addr8, ptinIpAddr->addr.ipv6, L7_IP6_ADDR_LEN*sizeof(L7_uchar8));               
     return L7_SUCCESS;
   }
   else
   {
     LOG_ERR(LOG_CTX_PTIN_MSG, "IP Family Address of PTIN not Supported:%u",fpIpAddr->family);
     return L7_NOT_SUPPORTED;
   }   
}

/**
 * Configure IP Source Guard on Ptin Port
 * 
 * @param msgIpsgVerifySource Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_ipsg_verify_source_set(msg_IPSG_set_t* msgIpsgVerifySource)
{
  ptin_intf_t  ptin_intf;
  L7_uint32    intIfNum;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "slotId         = %u"   , msgIpsgVerifySource->slotId);  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ptinPort       = %u/%u",msgIpsgVerifySource->intf.intf_type,msgIpsgVerifySource->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "verifySource   = %s"   ,msgIpsgVerifySource->enable==L7_FALSE?"No":"Yes");
 
  /* Get intIfNum */
  ptin_intf.intf_id=msgIpsgVerifySource->intf.intf_id;
  ptin_intf.intf_type=msgIpsgVerifySource->intf.intf_type;

  if (ptin_intf_ptintf2intIfNum(&ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error converting port %u/%u to intIfNum",ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_MSG, "Port# %u/%u: intIfNum# %2u", ptin_intf.intf_type, ptin_intf.intf_id, intIfNum);

#ifdef L7_IPSG_PACKAGE
  /*Despite the IPSG API having two input parameters: IP filtering and MAC filtering. 
    It does not support enabling just one!
    */
  if( (msgIpsgVerifySource->enable & IPSG_ENABLE) == IPSG_ENABLE )
  {
    return (ipsgVerifySourceSet(intIfNum, IPSG_ENABLE, IPSG_ENABLE));
  }
  else
  {
    if( (msgIpsgVerifySource->enable & IPSG_DISABLE) == IPSG_DISABLE )
    {
      return (ipsgVerifySourceSet(intIfNum, IPSG_DISABLE, IPSG_DISABLE));
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid value for enable:%u", msgIpsgVerifySource->enable);
      return L7_FAILURE;
    }
  }  
#else
  LOG_ERR(LOG_CTX_PTIN_MSG, "IP Source Guard not Supported!");
  return L7_FAILURE;
#endif
}

/**
 * Configure an IP Source Guard  static entry
 * 
 * @param msg_IPSG_static_entry_t Structure with config 
 *                                parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_ipsg_static_entry_set(msg_IPSG_static_entry_t* msgIpsgStaticEntry)
{  
  ptin_intf_t       ptin_intf;
  L7_uint32         intIfNum;
  L7_uint16         vlanId;
  L7_inet_addr_t    ipAddr;
  L7_uchar8         ipAddrStr[IPV6_DISP_ADDR_LEN]; 
  L7_enetMacAddr_t  macAddr;
  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "slotId        = %u"  , msgIpsgStaticEntry->slotId);  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "iDType        = %u"  , msgIpsgStaticEntry->idType);  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "iD            = %u"  , msgIpsgStaticEntry->id);  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ptinP         = %u/%u", msgIpsgStaticEntry->intf.intf_type,msgIpsgStaticEntry->intf.intf_id);  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "action        = %s"  , msgIpsgStaticEntry->action==L7_FALSE?"Remove":"Add");  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "MAC Addr      = %02X:%02X:%02X:%02X:%02X:%02X",msgIpsgStaticEntry->macAddr[0],msgIpsgStaticEntry->macAddr[1],
            msgIpsgStaticEntry->macAddr[2],msgIpsgStaticEntry->macAddr[3],msgIpsgStaticEntry->macAddr[4],msgIpsgStaticEntry->macAddr[5]);
  
  if (ptin_to_fp_ip_notation(&msgIpsgStaticEntry->ipAddr,&ipAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  memcpy(&macAddr, msgIpsgStaticEntry->macAddr, sizeof(macAddr));
  
  inetAddrPrint(&ipAddr, ipAddrStr);    

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "IP Address    = %s",ipAddrStr);
   
  /* Get intIfNum */
  ptin_intf.intf_id   = msgIpsgStaticEntry->intf.intf_id;
  ptin_intf.intf_type = msgIpsgStaticEntry->intf.intf_type;

  if (ptin_intf_ptintf2intIfNum(&ptin_intf, &intIfNum)!= L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error converting port %u/%u to intIfNum",ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_MSG, "Port# %u/%u: intIfNum# %2u", ptin_intf.intf_type, ptin_intf.intf_id, intIfNum);

  if( (msgIpsgStaticEntry->idType & IPSG_ID_ALL) == IPSG_EVC_ID)
  {
    /* Get Internal root vlan */
    if (ptin_evc_intRootVlan_get(msgIpsgStaticEntry->id, &vlanId)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error getting internal root vlan for eEVCId=%u", msgIpsgStaticEntry->id);
      return L7_NOT_EXIST;
    }
    LOG_TRACE(LOG_CTX_PTIN_MSG, "EVCidx# %u: internalRootVlan# %u",msgIpsgStaticEntry->id,vlanId);
  }
  else
  {
    if ( (msgIpsgStaticEntry->idType & IPSG_ID_ALL) == IPSG_ROOT_VLAN )
    {
#if 0
      L7_uint32         eEVCId;
#endif
      if ((vlanId = (0x0000FFFF & msgIpsgStaticEntry->id)) != msgIpsgStaticEntry->id && vlanId >= 4096)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid root vlan given:%u", msgIpsgStaticEntry->id);
        return L7_FAILURE;
      }
/*Disabled this verification to support MAC Bridge Services*/
#if 0
      if (ptin_evc_get_evcIdfromIntVlan(vlanId, &eEVCId) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_IGMP,"Invalid root VLAN:%u", vlanId);
        return L7_NOT_EXIST;
      }
      LOG_TRACE(LOG_CTX_PTIN_MSG, "EVCidx# %u: internalRootVlan# %u",eEVCId, vlanId);
#endif
    }
    else
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid IdType:%u", msgIpsgStaticEntry->idType);
      return L7_FAILURE;
    }
  }

#ifdef L7_IPSG_PACKAGE
  if ((msgIpsgStaticEntry->action&IPSG_ACTION_ADD) == IPSG_ACTION_ADD)
  {
    return (ipsgStaticEntryAdd(intIfNum, vlanId, &macAddr, &ipAddr));
  }
  else
  {
    return (ipsgStaticEntryRemove(intIfNum, vlanId, &macAddr, &ipAddr));  
  }   
#else
  LOG_ERR(LOG_CTX_IPSG, "IP Source Guard not Supported!");
  return L7_FAILURE;
#endif
}

/**
 * Get IP Source Guard binding table
 *
 * @param table: bind table entries
 *
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_ipsg_binding_table_get(msg_ipsg_binding_table_request_t *input, msg_ipsg_binding_table_response_t *output)
{
#ifdef L7_IPSG_PACKAGE
  LOG_NOTICE(LOG_CTX_IPSG, "Not Implemented Yet!");
  return L7_NOT_IMPLEMENTED_YET;
#else
  LOG_ERR(LOG_CTX_IPSG, "IP Source Guard not Supported!");
  return L7_FAILURE;
#endif

}

/*End IP Source Guard Management Functions **************************************************/

/* IGMP Management Functions **************************************************/

/**
* @purpose Set the IGMP Admission Control 
*          Configuration
*  
* @param  msg_IgmpAdmissionControl : Structure with config 
*                                  parameters
*
* @return L7_RC_t L7_SUCCESS/L7_FAILURE
*
* @notes This routine will support configuring the admission 
*        control parameters on the interface, on the evc id, and
*        on the igmp client
*/
L7_RC_t ptin_msg_igmp_admission_control_set(msg_IgmpAdmissionControl *igmpAdmissionControl)
{
  LOG_NOTICE(LOG_CTX_IPSG, "Not Implemented Yet!");
  return L7_SUCCESS;
}


/**
 * Applies IGMP Proxy configuration
 * 
 * @param msgIgmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_igmp_proxy_set(msg_IgmpProxyCfg_t *msgIgmpProxy)
{
  PTIN_MGMD_CTRL_MGMD_CONFIG_t ptinMgmdProxy;
  L7_RC_t rc;

  /* Copy data */
  ptinMgmdProxy.mask                                   = msgIgmpProxy->mask;
  ptinMgmdProxy.admin                                  = msgIgmpProxy->admin;
  ptinMgmdProxy.networkVersion                         = msgIgmpProxy->networkVersion;
  ptinMgmdProxy.clientVersion                          = msgIgmpProxy->clientVersion;
  ptinMgmdProxy.ipv4Addr                               = msgIgmpProxy->ipv4_addr.s_addr;
  ptinMgmdProxy.igmpCos                                = msgIgmpProxy->igmp_cos;
  ptinMgmdProxy.fastLeave                              = msgIgmpProxy->fast_leave;

  ptinMgmdProxy.querier.mask                           = msgIgmpProxy->querier.mask;
  ptinMgmdProxy.querier.flags                          = msgIgmpProxy->querier.flags;
  ptinMgmdProxy.querier.robustness                     = msgIgmpProxy->querier.robustness;
  ptinMgmdProxy.querier.queryInterval                  = msgIgmpProxy->querier.query_interval;
  ptinMgmdProxy.querier.queryResponseInterval          = msgIgmpProxy->querier.query_response_interval;
  ptinMgmdProxy.querier.groupMembershipInterval        = msgIgmpProxy->querier.group_membership_interval;
  ptinMgmdProxy.querier.otherQuerierPresentInterval    = msgIgmpProxy->querier.other_querier_present_interval;
  ptinMgmdProxy.querier.startupQueryInterval           = msgIgmpProxy->querier.startup_query_interval;
  ptinMgmdProxy.querier.startupQueryCount              = msgIgmpProxy->querier.startup_query_count;
  ptinMgmdProxy.querier.lastMemberQueryInterval        = msgIgmpProxy->querier.last_member_query_interval;
  ptinMgmdProxy.querier.lastMemberQueryCount           = msgIgmpProxy->querier.last_member_query_count;
  ptinMgmdProxy.querier.olderHostPresentTimeout        = msgIgmpProxy->querier.older_host_present_timeout;

  ptinMgmdProxy.host.mask                              = msgIgmpProxy->host.mask;
  ptinMgmdProxy.host.flags                             = msgIgmpProxy->host.flags;
  ptinMgmdProxy.host.robustness                        = msgIgmpProxy->host.robustness;
  ptinMgmdProxy.host.unsolicitedReportInterval         = msgIgmpProxy->host.unsolicited_report_interval;
  ptinMgmdProxy.host.olderQuerierPresentTimeout        = msgIgmpProxy->host.older_querier_present_timeout;
  ptinMgmdProxy.host.maxRecordsPerReport               = msgIgmpProxy->host.max_records_per_report;

  ptinMgmdProxy.bandwidthControl                       = msgIgmpProxy->bandwidthControl;
  ptinMgmdProxy.channelsControl                        = msgIgmpProxy->channelsControl;

  //Force the whitelist feauture to DISABLE
  ptinMgmdProxy.whiteList                              = PTIN_MGMD_DISABLE;
  ptinMgmdProxy.mask                                  |= PTIN_MGMD_CONFIG_WHITELIST_MASK;

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "IGMP Proxy (mask=0x%08X)", ptinMgmdProxy.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Admin #                          = %u", ptinMgmdProxy.admin);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Network Version                  = %u", ptinMgmdProxy.networkVersion);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client Version                   = %u", ptinMgmdProxy.clientVersion);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  IP Addr                          = %u.%u.%u.%u", (ptinMgmdProxy.ipv4Addr>>24)&0xFF, (ptinMgmdProxy.ipv4Addr>>16)&0xFF, 
                                                                                  (ptinMgmdProxy.ipv4Addr>>8)&0xFF, ptinMgmdProxy.ipv4Addr&0xFF);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  COS                              = %u", ptinMgmdProxy.igmpCos);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  FastLeave                        = %s", ptinMgmdProxy.fastLeave != 0 ? "ON":"OFF");  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Querier (mask=0x%08X)", ptinMgmdProxy.querier.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Flags                          = 0x%04X", ptinMgmdProxy.querier.flags);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Robustness                     = %u", ptinMgmdProxy.querier.robustness);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Query Interval                 = %u", ptinMgmdProxy.querier.queryInterval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Query Response Interval        = %u", ptinMgmdProxy.querier.queryResponseInterval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Group Membership Interval      = %u", ptinMgmdProxy.querier.groupMembershipInterval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Other Querier Present Interval = %u", ptinMgmdProxy.querier.otherQuerierPresentInterval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Startup Query Interval         = %u", ptinMgmdProxy.querier.startupQueryInterval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Startup Query Count            = %u", ptinMgmdProxy.querier.startupQueryCount);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Last Member Query Interval     = %u", ptinMgmdProxy.querier.lastMemberQueryInterval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Last Member Query Count        = %u", ptinMgmdProxy.querier.lastMemberQueryCount);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Older Host Present Timeout     = %u", ptinMgmdProxy.querier.olderHostPresentTimeout);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Host (mask=0x%08X)", ptinMgmdProxy.host.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Flags                          = 0x%02X", ptinMgmdProxy.host.flags);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Robustness                     = %u", ptinMgmdProxy.host.robustness);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Unsolicited Report Interval    = %u", ptinMgmdProxy.host.unsolicitedReportInterval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Older Querier Present Timeout  = %u", ptinMgmdProxy.host.olderQuerierPresentTimeout);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Max Group Records per Packet   = %u", ptinMgmdProxy.host.maxRecordsPerReport);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Bandwidth Control                = %s", ptinMgmdProxy.bandwidthControl != 0 ? "ON":"OFF");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Channels Control                 = %s", ptinMgmdProxy.channelsControl != 0 ? "ON":"OFF");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  WhiteList                        = %s", ptinMgmdProxy.whiteList != 0 ? "ON":"OFF");
  
  /* Apply config */
  rc = ptin_igmp_proxy_config_set(&ptinMgmdProxy);

  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error setting IGMP Proxy config");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Gets IGMP Proxy configuration
 * 
 * @param msgIgmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_igmp_proxy_get(msg_IgmpProxyCfg_t *msgIgmpProxy)
{
  PTIN_MGMD_CTRL_MGMD_CONFIG_t ptinIgmpProxy;
  L7_RC_t rc;

  memset(&ptinIgmpProxy, 0x00, sizeof(ptinIgmpProxy));

  ptinIgmpProxy.mask         = 0xFF;
  ptinIgmpProxy.querier.mask = 0xFFFF;
  ptinIgmpProxy.host.mask    = 0xFF;

  /* Get config */
  rc = ptin_igmp_proxy_config_get(&ptinIgmpProxy);

  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting IGMP Proxy config");
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "IGMP Proxy (mask=0x%08X)", ptinIgmpProxy.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Admin #                          = %u", ptinIgmpProxy.admin);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Network Version                  = %u", ptinIgmpProxy.networkVersion);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client Version                   = %u", ptinIgmpProxy.clientVersion);  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  IP Addr                          = %u.%u.%u.%u", (ptinIgmpProxy.ipv4Addr >> 24) & 0xFF, (ptinIgmpProxy.ipv4Addr >> 16) & 0xFF,
                                                                                  (ptinIgmpProxy.ipv4Addr >>  8) & 0xFF,  ptinIgmpProxy.ipv4Addr        & 0xFF);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  COS                              = %u", ptinIgmpProxy.igmpCos);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  FastLeave                        = %s", ptinIgmpProxy.fastLeave != 0 ? "ON":"OFF");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Querier (mask=0x%08X)", ptinIgmpProxy.querier.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Flags                          = 0x%08X", ptinIgmpProxy.querier.flags);  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Robustness                     = %u", ptinIgmpProxy.querier.robustness);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Query Interval                 = %u", ptinIgmpProxy.querier.queryInterval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Query Response Interval        = %u", ptinIgmpProxy.querier.queryResponseInterval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Group Membership Interval      = %u", ptinIgmpProxy.querier.groupMembershipInterval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Other Querier Present Interval = %u", ptinIgmpProxy.querier.otherQuerierPresentInterval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Startup Query Interval         = %u", ptinIgmpProxy.querier.startupQueryInterval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Startup Query Count            = %u", ptinIgmpProxy.querier.startupQueryCount);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Last Member Query Interval     = %u", ptinIgmpProxy.querier.lastMemberQueryInterval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Last Member Query Count        = %u", ptinIgmpProxy.querier.lastMemberQueryCount);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Older Host Present Timeout     = %u", ptinIgmpProxy.querier.olderHostPresentTimeout);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Host (mask=0x%08X)", ptinIgmpProxy.host.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Flags                          = 0x%02X", ptinIgmpProxy.host.flags);  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Robustness                     = %u", ptinIgmpProxy.host.robustness);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Unsolicited Report Interval    = %u", ptinIgmpProxy.host.unsolicitedReportInterval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Older Querier Present  Timeout = %u", ptinIgmpProxy.host.olderQuerierPresentTimeout);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Bandwidth Control                = %s", ptinIgmpProxy.bandwidthControl != 0 ? "ON":"OFF");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Channels Control                 = %s", ptinIgmpProxy.channelsControl != 0 ? "ON":"OFF");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  WhiteList                        = %s", ptinIgmpProxy.whiteList != 0 ? "ON":"OFF");

  /* Copy data */
  msgIgmpProxy->mask                                   = ptinIgmpProxy.mask;
  msgIgmpProxy->admin                                  = ptinIgmpProxy.admin;
  msgIgmpProxy->networkVersion                         = ptinIgmpProxy.networkVersion;
  msgIgmpProxy->clientVersion                          = ptinIgmpProxy.clientVersion;
  msgIgmpProxy->ipv4_addr.s_addr                       = ptinIgmpProxy.ipv4Addr;
  msgIgmpProxy->igmp_cos                               = ptinIgmpProxy.igmpCos;
  msgIgmpProxy->fast_leave                             = ptinIgmpProxy.fastLeave;

  msgIgmpProxy->querier.mask                           = ptinIgmpProxy.querier.mask;
  msgIgmpProxy->querier.flags                          = ptinIgmpProxy.querier.flags;  
  msgIgmpProxy->querier.robustness                     = ptinIgmpProxy.querier.robustness;
  msgIgmpProxy->querier.query_interval                 = ptinIgmpProxy.querier.queryInterval;
  msgIgmpProxy->querier.query_response_interval        = ptinIgmpProxy.querier.queryResponseInterval;
  msgIgmpProxy->querier.group_membership_interval      = ptinIgmpProxy.querier.groupMembershipInterval;
  msgIgmpProxy->querier.other_querier_present_interval = ptinIgmpProxy.querier.otherQuerierPresentInterval;
  msgIgmpProxy->querier.startup_query_interval         = ptinIgmpProxy.querier.startupQueryInterval;
  msgIgmpProxy->querier.startup_query_count            = ptinIgmpProxy.querier.startupQueryCount;
  msgIgmpProxy->querier.last_member_query_interval     = ptinIgmpProxy.querier.lastMemberQueryInterval;
  msgIgmpProxy->querier.last_member_query_count        = ptinIgmpProxy.querier.lastMemberQueryCount;
  msgIgmpProxy->querier.older_host_present_timeout     = ptinIgmpProxy.querier.olderHostPresentTimeout;

  msgIgmpProxy->host.mask                              = ptinIgmpProxy.host.mask;
  msgIgmpProxy->host.flags                             = ptinIgmpProxy.host.flags;  
  msgIgmpProxy->host.robustness                        = ptinIgmpProxy.host.robustness;
  msgIgmpProxy->host.unsolicited_report_interval       = ptinIgmpProxy.host.unsolicitedReportInterval;
  msgIgmpProxy->host.older_querier_present_timeout     = ptinIgmpProxy.host.olderQuerierPresentTimeout;

  msgIgmpProxy->bandwidthControl                       = ptinIgmpProxy.bandwidthControl;
  msgIgmpProxy->channelsControl                        = ptinIgmpProxy.channelsControl;

  return L7_SUCCESS;
}

/**
 * Creates/updates an IGMP instance (interfaces/VLANs)
 *
 * @param msgIgmpIntf Structure with config parameters
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_igmp_instance_add(msg_IgmpMultcastUnicastLink_t *msgIgmpInst)
{
  L7_RC_t rc;

  if (msgIgmpInst==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Going to add IGMP Instance:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  MC evc_idx = %u", msgIgmpInst->multicastEvcId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  UC evc_idx = %u", msgIgmpInst->unicastEvcId);

  /* Apply config */
  rc = ptin_igmp_instance_add(msgIgmpInst->multicastEvcId,msgIgmpInst->unicastEvcId);

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error creating/updating IGMP instance");
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Deletes an IGMP instance (interfaces/VLANs)
 *
 * @param msgIgmpIntf Structure with config parameters (router VLAN must be set)
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_igmp_instance_remove(msg_IgmpMultcastUnicastLink_t *msgIgmpInst)
{
  L7_RC_t rc;

  if (msgIgmpInst==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Going to remove IGMP Instance:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  MC evc_idx = %u", msgIgmpInst->multicastEvcId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  UC evc_idx = %u", msgIgmpInst->unicastEvcId);

  /* Apply config */
  rc = ptin_igmp_instance_remove(msgIgmpInst->multicastEvcId,msgIgmpInst->unicastEvcId);

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error removing IGMP instance");
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Add a Multicast client to a MC EVC
 *
 * @param McastClient : Multicast client info 
 * @param n_clients   : Number of clients 
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_igmp_client_add(msg_IgmpClient_t *McastClient, L7_uint16 n_clients)
{
  L7_uint16 i;
  ptin_client_id_t client;
  L7_RC_t rc;

  if (McastClient==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  for (i=0; i<n_clients; i++)
  {
    /* Output data */
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Going to add MC client");
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  MC evc_idx = %u", McastClient[i].mcEvcId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "   Client.Mask         = 0x%02x", McastClient[i].client.mask);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "   Client.OVlan        = %u", McastClient[i].client.outer_vlan);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "   Client.IVlan        = %u", McastClient[i].client.inner_vlan);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "   Client.Intf         = %u/%u", McastClient[i].client.intf.intf_type,McastClient[i].client.intf.intf_id);
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT                                     
  
    if ( (McastClient[i].mask > PTIN_IGMP_ADMISSION_CONTROL_MASK_VALID) || 
        ( ( (McastClient[i].mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH ) &&
          (McastClient[i].maxBandwidth != PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_DISABLE_UINT64 && McastClient[i].maxBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_KBPS) ) ||
         ( ( (McastClient[i].mask & PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS) == PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS ) &&
          (McastClient[i].maxChannels != PTIN_IGMP_ADMISSION_CONTROL_MAX_CHANNELS_DISABLE && McastClient[i].maxChannels > PTIN_IGMP_ADMISSION_CONTROL_MAX_CHANNELS) ) )
        
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid Admission Control Parameters [mask:0x%02x maxBandwidth:%llu kbits/s maxChannels:%hu",McastClient[i].mask, McastClient[i].maxBandwidth, McastClient[i].maxChannels);
      return L7_FAILURE;
    }

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "   onuId        = %u", McastClient[i].onuId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "   mask         = %u", McastClient[i].mask);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "   maxChannels  = %u", McastClient[i].maxChannels);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "   maxBandwidth = %llu kbit/s ", McastClient[i].maxBandwidth);
#endif

    memset(&client,0x00,sizeof(ptin_client_id_t));
    if (McastClient[i].client.mask & MSG_CLIENT_OVLAN_MASK)
    {
      client.outerVlan = McastClient[i].client.outer_vlan;
      client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
    }
    if (McastClient[i].client.mask & MSG_CLIENT_IVLAN_MASK)
    {
      client.innerVlan = McastClient[i].client.inner_vlan;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
    }
    if (McastClient[i].client.mask & MSG_CLIENT_INTF_MASK)
    {
      client.ptin_intf.intf_type  = McastClient[i].client.intf.intf_type;
      client.ptin_intf.intf_id    = McastClient[i].client.intf.intf_id;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
    }

    /* Apply config */
    rc = ptin_igmp_client_add(McastClient[i].mcEvcId, &client, 0, 0, McastClient[i].onuId, McastClient[i].mask, McastClient[i].maxBandwidth, McastClient[i].maxChannels);

    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error adding MC client");
      return rc;
    }
  }

  return L7_SUCCESS;
}

/**
 * Remove a Multicast client from a MC EVC
 *
 * @param McastClient : Multicast client info 
 * @param n_clients   : Number of clients 
 *
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_igmp_client_delete(msg_IgmpClient_t *McastClient, L7_uint16 n_clients)
{
  L7_uint16 i;
  ptin_client_id_t client;
  L7_RC_t rc = L7_SUCCESS;

  if (McastClient==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  for (i=0; i<n_clients; i++)
  {
    /* Output data */
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Going to remove MC client");
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  MC evc_idx = %u", McastClient[i].mcEvcId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "   Client.Mask  = 0x%02x", McastClient[i].client.mask);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "   Client.OVlan = %u", McastClient[i].client.outer_vlan);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "   Client.IVlan = %u", McastClient[i].client.inner_vlan);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "   Client.Intf  = %u/%u", McastClient[i].client.intf.intf_type,McastClient[i].client.intf.intf_id);

    memset(&client,0x00,sizeof(ptin_client_id_t));
    if (McastClient[i].client.mask & MSG_CLIENT_OVLAN_MASK)
    {
      client.outerVlan = McastClient[i].client.outer_vlan;
      client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
    }
    if (McastClient[i].client.mask & MSG_CLIENT_IVLAN_MASK)
    {
      client.innerVlan = McastClient[i].client.inner_vlan;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
    }
    if (McastClient[i].client.mask & MSG_CLIENT_INTF_MASK)
    {
      client.ptin_intf.intf_type  = McastClient[i].client.intf.intf_type;
      client.ptin_intf.intf_id    = McastClient[i].client.intf.intf_id;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
    }

    /* Apply config */
    rc = ptin_igmp_client_delete(McastClient[i].mcEvcId,&client);

    if ( rc != L7_SUCCESS )
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error removing MC client");
      return rc;
    }
  }

  return L7_SUCCESS;
}

/**
 * Get Client IGMP statistics
 * 
 * @param igmp_stats : IGMP statistics information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_clientStats_get(msg_IgmpClientStatistics_t *igmp_stats)
{
  ptin_client_id_t                client;
  PTIN_MGMD_CTRL_STATS_RESPONSE_t stats;
  L7_RC_t                         rc;

  if (igmp_stats==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Reading client IGMP stats:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  MC evc_idx   = %u", igmp_stats->mcEvcId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask         = 0x%02x", igmp_stats->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Interface    = %u/%u", igmp_stats->intf.intf_type,igmp_stats->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Mask  = 0x%02x", igmp_stats->client.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.OVlan = %u", igmp_stats->client.outer_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.IVlan = %u", igmp_stats->client.inner_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Intf  = %u/%u", igmp_stats->client.intf.intf_type, igmp_stats->client.intf.intf_id);

  //Short Fix to Support Mac Bridge Services and Unicast Services
  #if PTIN_BOARD_IS_LINECARD
  {
    #if 0
    L7_BOOL isMacBridge;    
    if (ptin_evc_mac_bridge_check(igmp_stats->mcEvcId, &isMacBridge)==L7_SUCCESS && isMacBridge==L7_TRUE)
    #else
    if (igmp_stats->client.outer_vlan==0) 
    #endif
    {      
      igmp_stats->client.outer_vlan=igmp_stats->client.inner_vlan;      
    }
    igmp_stats->client.mask|=MSG_CLIENT_OVLAN_MASK;    
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Converted [client.Mask:%u Client.OVlan:%u]",igmp_stats->client.mask,igmp_stats->client.outer_vlan);
  }
  #endif

  /* Evaluate provided data */
  if ( igmp_stats->mcEvcId==(L7_uint16)-1 ||
       !(igmp_stats->mask & MSG_CLIENT_MASK) ||
       (igmp_stats->client.mask == 0x00) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "MC EVC and client reference must be provided");
    return L7_FAILURE;
  }

  memset(&client,0x00,sizeof(ptin_client_id_t));
  if (igmp_stats->client.mask & MSG_CLIENT_OVLAN_MASK)
  {
    client.outerVlan = igmp_stats->client.outer_vlan;
    client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  }
  if (igmp_stats->client.mask & MSG_CLIENT_IVLAN_MASK)
  {
    client.innerVlan = igmp_stats->client.inner_vlan;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
  }
  if (igmp_stats->client.mask & MSG_CLIENT_INTF_MASK)
  {
    client.ptin_intf.intf_type  = igmp_stats->client.intf.intf_type;
    client.ptin_intf.intf_id    = igmp_stats->client.intf.intf_id;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  }

  /* Get statistics */
  rc = ptin_igmp_stat_client_get(igmp_stats->mcEvcId,&client,&stats);
  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting client statistics");
    return rc;
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success getting client statistics");
  }

  igmp_stats->stats.active_groups                                                    = stats.activeGroups;            
  igmp_stats->stats.active_clients                                                   = stats.activeClients; 
            
  igmp_stats->stats.igmp_tx                                                          = stats.igmpTx;
  igmp_stats->stats.igmp_valid_rx                                                    = stats.igmpValidRx;
  igmp_stats->stats.igmp_invalid_rx                                                  = stats.igmpInvalidRx;    
  igmp_stats->stats.igmp_dropped_rx                                                  = stats.igmpDroppedRx; 
  igmp_stats->stats.igmp_total_rx                                                    = stats.igmpTotalRx;  
  
  igmp_stats->stats.HWIgmpv2Statistics.join_tx                                       = stats.v2.joinTx;               
  igmp_stats->stats.HWIgmpv2Statistics.join_valid_rx                                 = stats.v2.joinValidRx;   
  igmp_stats->stats.HWIgmpv2Statistics.join_invalid_rx                               = stats.v2.joinInvalidRx;    
  igmp_stats->stats.HWIgmpv2Statistics.leave_tx                                      = stats.v2.leaveTx;              
  igmp_stats->stats.HWIgmpv2Statistics.leave_valid_rx                                = stats.v2.leaveValidRx;    
  
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_tx                          = stats.v3.membershipReportTx; 
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_valid_rx                    = stats.v3.membershipReportValidRx;      
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_invalid_rx                  = stats.v3.membershipReportInvalidRx;          
  
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_tx              = stats.v3.groupRecords.allowTx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_valid_rx        = stats.v3.groupRecords.allowValidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_invalid_rx      = stats.v3.groupRecords.allowInvalidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_tx              = stats.v3.groupRecords.blockTx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_valid_rx        = stats.v3.groupRecords.blockValidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_invalid_rx      = stats.v3.groupRecords.blockInvalidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_tx         = stats.v3.groupRecords.isIncludeTx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_valid_rx   = stats.v3.groupRecords.isIncludeValidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_invalid_rx = stats.v3.groupRecords.isIncludeInvalidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_tx         = stats.v3.groupRecords.isExcludeTx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_valid_rx   = stats.v3.groupRecords.isExcludeValidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_invalid_rx = stats.v3.groupRecords.isExcludeInvalidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_tx         = stats.v3.groupRecords.toIncludeTx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_valid_rx   = stats.v3.groupRecords.toIncludeValidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_invalid_rx = stats.v3.groupRecords.toIncludeInvalidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_tx         = stats.v3.groupRecords.toExcludeTx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_valid_rx   = stats.v3.groupRecords.toExcludeValidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_invalid_rx = stats.v3.groupRecords.toExcludeInvalidRx;                                  
         
  igmp_stats->stats.HWQueryStatistics.general_query_tx                               = stats.query.generalQueryTx;     
  igmp_stats->stats.HWQueryStatistics.general_query_valid_rx                         = stats.query.generalQueryValidRx;
  igmp_stats->stats.HWQueryStatistics.group_query_tx                                 = stats.query.groupQueryTx;       
  igmp_stats->stats.HWQueryStatistics.group_query_valid_rx                           = stats.query.groupQueryValidRx;  
  igmp_stats->stats.HWQueryStatistics.source_query_tx                                = stats.query.sourceQueryTx;      
  igmp_stats->stats.HWQueryStatistics.source_query_valid_rx                          = stats.query.sourceQueryValidRx; 

  return L7_SUCCESS;
}

/**
 * Clear Client IGMP statistics
 * 
 * @param igmp_stats : IGMP statistics information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_clientStats_clear(msg_IgmpClientStatistics_t *igmp_stats, uint16 n_clients)
{
  ptin_client_id_t client;
  L7_RC_t rc;

  if (igmp_stats==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Clearing client IGMP stats:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  MC evc_idx   = %u", igmp_stats->mcEvcId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask         = 0x%02x", igmp_stats->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Interface    = %u/%u", igmp_stats->intf.intf_type,igmp_stats->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Mask  = 0x%02x", igmp_stats->client.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.OVlan = %u", igmp_stats->client.outer_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.IVlan = %u", igmp_stats->client.inner_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Intf  = %u/%u", igmp_stats->client.intf.intf_type, igmp_stats->client.intf.intf_id);

  /* Evaluate provided data */
  if ( igmp_stats->mcEvcId==(L7_uint16)-1 ||
       !(igmp_stats->mask & MSG_CLIENT_MASK) ||
       (igmp_stats->client.mask == 0x00) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "MC EVC and client reference must be provided");
    return L7_FAILURE;
  }

  memset(&client,0x00,sizeof(ptin_client_id_t));
  if (igmp_stats->client.mask & MSG_CLIENT_OVLAN_MASK)
  {
    client.outerVlan = igmp_stats->client.outer_vlan;
    client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  }
  if (igmp_stats->client.mask & MSG_CLIENT_IVLAN_MASK)
  {
    client.innerVlan = igmp_stats->client.inner_vlan;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
  }
  if (igmp_stats->client.mask & MSG_CLIENT_INTF_MASK)
  {
    client.ptin_intf.intf_type  = igmp_stats->client.intf.intf_type;
    client.ptin_intf.intf_id    = igmp_stats->client.intf.intf_id;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  }

  /* Clear client stats */
  rc = ptin_igmp_stat_client_clear(igmp_stats->mcEvcId,&client);

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error clearing client statistics");
    return rc;
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success clearing client statistics");
  }

  return L7_SUCCESS;
}

/**
 * Get interface IGMP statistics
 * 
 * @param igmp_stats : IGMP statistics information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_intfStats_get(msg_IgmpClientStatistics_t *igmp_stats)
{
  ptin_intf_t                     ptin_intf;
  PTIN_MGMD_CTRL_STATS_RESPONSE_t stats;
  L7_RC_t                         rc;

  if (igmp_stats==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Reading interface IGMP stats:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  MC evc_idx   = %u", igmp_stats->mcEvcId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask         = 0x%02x", igmp_stats->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Interface    = %u/%u", igmp_stats->intf.intf_type,igmp_stats->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Mask  = 0x%02x", igmp_stats->client.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.OVlan = %u", igmp_stats->client.outer_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.IVlan = %u", igmp_stats->client.inner_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Intf  = %u/%u", igmp_stats->client.intf.intf_type, igmp_stats->client.intf.intf_id);

  /* Evaluate provided data */
  if ( !(igmp_stats->mask & MSG_INTERFACE_MASK) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "At least, interface must be provided");
    return L7_FAILURE;
  }

  ptin_intf.intf_type = igmp_stats->intf.intf_type;
  ptin_intf.intf_id   = igmp_stats->intf.intf_id;

  /* Get statistics */
  if (igmp_stats->mcEvcId==(L7_uint16)-1)
  {
    rc = ptin_igmp_stat_intf_get(&ptin_intf,&stats);

    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting global interface statistics");
      return rc;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success getting global interface statistics");
    }
  }
  else
  {
    rc = ptin_igmp_stat_instanceIntf_get(igmp_stats->mcEvcId, &ptin_intf, &stats);

    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error getting statistics struct");
      return rc;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success getting interface statistics of one IGMP instance");
    }
  }

  igmp_stats->stats.active_groups                                                    = stats.activeGroups;            
  igmp_stats->stats.active_clients                                                   = stats.activeClients; 
            
  igmp_stats->stats.igmp_tx                                                          = stats.igmpTx;
  igmp_stats->stats.igmp_valid_rx                                                    = stats.igmpValidRx;
  igmp_stats->stats.igmp_invalid_rx                                                  = stats.igmpInvalidRx;    
  igmp_stats->stats.igmp_dropped_rx                                                  = stats.igmpDroppedRx; 
  igmp_stats->stats.igmp_total_rx                                                    = stats.igmpTotalRx;  
  
  igmp_stats->stats.HWIgmpv2Statistics.join_tx                                       = stats.v2.joinTx;               
  igmp_stats->stats.HWIgmpv2Statistics.join_valid_rx                                 = stats.v2.joinValidRx;   
  igmp_stats->stats.HWIgmpv2Statistics.join_invalid_rx                               = stats.v2.joinInvalidRx;    
  igmp_stats->stats.HWIgmpv2Statistics.leave_tx                                      = stats.v2.leaveTx;              
  igmp_stats->stats.HWIgmpv2Statistics.leave_valid_rx                                = stats.v2.leaveValidRx;    
  
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_tx                          = stats.v3.membershipReportTx; 
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_valid_rx                    = stats.v3.membershipReportValidRx;      
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_invalid_rx                  = stats.v3.membershipReportInvalidRx;          
  
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_tx              = stats.v3.groupRecords.allowTx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_valid_rx        = stats.v3.groupRecords.allowValidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_invalid_rx      = stats.v3.groupRecords.allowInvalidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_tx              = stats.v3.groupRecords.blockTx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_valid_rx        = stats.v3.groupRecords.blockValidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_invalid_rx      = stats.v3.groupRecords.blockInvalidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_tx         = stats.v3.groupRecords.isIncludeTx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_valid_rx   = stats.v3.groupRecords.isIncludeValidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_invalid_rx = stats.v3.groupRecords.isIncludeInvalidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_tx         = stats.v3.groupRecords.isExcludeTx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_valid_rx   = stats.v3.groupRecords.isExcludeValidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_invalid_rx = stats.v3.groupRecords.isExcludeInvalidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_tx         = stats.v3.groupRecords.toIncludeTx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_valid_rx   = stats.v3.groupRecords.toIncludeValidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_invalid_rx = stats.v3.groupRecords.toIncludeInvalidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_tx         = stats.v3.groupRecords.toExcludeTx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_valid_rx   = stats.v3.groupRecords.toExcludeValidRx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_invalid_rx = stats.v3.groupRecords.toExcludeInvalidRx;                                  
         
  igmp_stats->stats.HWQueryStatistics.general_query_tx                               = stats.query.generalQueryTx;     
  igmp_stats->stats.HWQueryStatistics.general_query_valid_rx                         = stats.query.generalQueryValidRx;
  igmp_stats->stats.HWQueryStatistics.group_query_tx                                 = stats.query.groupQueryTx;       
  igmp_stats->stats.HWQueryStatistics.group_query_valid_rx                           = stats.query.groupQueryValidRx;  
  igmp_stats->stats.HWQueryStatistics.source_query_tx                                = stats.query.sourceQueryTx;      
  igmp_stats->stats.HWQueryStatistics.source_query_valid_rx                          = stats.query.sourceQueryValidRx; 

  return L7_SUCCESS;
}

/**
 * Clear interface IGMP statistics
 * 
 * @param igmp_stats : IGMP statistics information
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_intfStats_clear(msg_IgmpClientStatistics_t *igmp_stats, uint16 n_clients)
{
  ptin_intf_t ptin_intf;
  L7_RC_t rc;

  if (igmp_stats==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Clearing interface IGMP stats:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  MC evc_idx   = %u", igmp_stats->mcEvcId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask         = 0x%02x", igmp_stats->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Interface    = %u/%u", igmp_stats->intf.intf_type,igmp_stats->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Mask  = 0x%02x", igmp_stats->client.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.OVlan = %u", igmp_stats->client.outer_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.IVlan = %u", igmp_stats->client.inner_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client.Intf  = %u/%u", igmp_stats->client.intf.intf_type, igmp_stats->client.intf.intf_id);

  ptin_intf.intf_type = igmp_stats->intf.intf_type;
  ptin_intf.intf_id   = igmp_stats->intf.intf_id;

  /* MC EVC not provided */
  if (igmp_stats->mcEvcId==(L7_uint16)-1)
  {
    /* Interface not provided */
    if ( !(igmp_stats->mask & MSG_INTERFACE_MASK) )
    {
      /* Clear all stats */
      rc = ptin_igmp_stat_clearAll();

      if (rc!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "Error clearing all statistics data");
        return rc;
      }
      else
      {
        LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success clearing all statistics data");
      }
    }
    /* Interface provided */
    else
    {
      /* Clear all stats of one interface */
      rc = ptin_igmp_stat_intf_clear(&ptin_intf);

      if (rc!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "Error clearing statistics of one complete interface");
        return rc;
      }
      else
      {
        LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success clearing statistics of one complete interface");
      }
    }
  }
  /* MC EVC provided */
  else
  {
    /* Interface not provided */
    if ( !(igmp_stats->mask & MSG_INTERFACE_MASK) )
    {
      /* Clear stats of one igmp instance */
      rc = ptin_igmp_stat_instance_clear(igmp_stats->mcEvcId);

      if (rc!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "Error clearing statistics of one IGMP instance");
        return rc;
      }
      else
      {
        LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success clearing statistics of one IGMP instance");
      }
    }
    /* Interface provided */
    else
    {
      /* Clear stats of one igmp instance and one interface */
      rc = ptin_igmp_stat_instanceIntf_clear(igmp_stats->mcEvcId, &ptin_intf);

      if (rc!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "Error clearing statistics of one IGMP instance + interface");
        return rc;
      }
      else
      {
        LOG_DEBUG(LOG_CTX_PTIN_MSG, "Success clearing statistics of one IGMP instance + interface");
      }
    }
  }

  return L7_SUCCESS;
}

/**
 * Get list of channels contained in the white list
 * 
 * @param channel_list : Channel list array
 * @param n_channels : Number of channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  #define IGMPASSOC_MAX_CHANNELS_IN_MESSAGE   100   /* Maximum number of channels in one message */
static igmpAssoc_entry_t igmpAssoc_list[IGMPASSOC_CHANNELS_MAX];
static L7_uint16 igmpAssoc_channels_max = 0;
#endif

L7_RC_t ptin_msg_IGMP_ChannelAssoc_get(msg_MCAssocChannel_t *channel_list, L7_uint16 *n_channels)
{
  if (channel_list==L7_NULLPTR || n_channels==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Reading White list channel list:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Slot   = %d",channel_list->SlotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," EVC_MC = %d",channel_list->evcid_mc);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Entry_idx = %d",channel_list->entry_idx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," DstIP_Channel = 0x%08x (ipv6=%u) / %u",channel_list->channel_dstIp.addr.ipv4, channel_list->channel_dstIp.family, channel_list->channel_dstmask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," SrcIP_Channel = 0x%08x (ipv6=%u) / %u",channel_list->channel_srcIp.addr.ipv4, channel_list->channel_srcIp.family, channel_list->channel_srcmask);

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  L7_uint16 i, i_start;
  L7_uint16 number_of_channels;
  L7_uint8  slotId;

  /* For null entry idex, read and store all channels */
  if (channel_list->entry_idx==0)
  {
    /* Clear channel list */
    memset(igmpAssoc_list, 0x00, sizeof(igmpAssoc_list));
    igmpAssoc_channels_max = 0;

    number_of_channels = IGMPASSOC_CHANNELS_MAX;
    if (igmp_assoc_channelList_get(0, channel_list->evcid_mc, igmpAssoc_list, &number_of_channels)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error reading list of channels");
      return L7_FAILURE;
    }
    /* Update number of channels */
    igmpAssoc_channels_max = number_of_channels;
  }

  /* Save important data of input message */
  slotId  = channel_list->SlotId;
  i_start = channel_list->entry_idx;

  /* Determine max number of messages */
  /* First index is beyond max index: no channels to retrieve */
  if (i_start >= igmpAssoc_channels_max)
  {
    number_of_channels = 0;
  }
  /* Remaining channels, is lower than maximum message capacity */
  else if ( (igmpAssoc_channels_max - i_start) < IGMPASSOC_MAX_CHANNELS_IN_MESSAGE )
  {
    number_of_channels = igmpAssoc_channels_max - i_start;
  }
  /* Use maximum capacity */
  else
  {
    number_of_channels = IGMPASSOC_MAX_CHANNELS_IN_MESSAGE;
  }

  for (i=0; i<=number_of_channels; i++)
  {
    /* Constant information to be replicated in all channels */
    channel_list[i].SlotId    = slotId;
    channel_list[i].entry_idx = i_start + i;

    /* Group address (prepared for IPv6) */
    if ( igmpAssoc_list[i_start+i].groupAddr.family == L7_AF_INET6 )
    {
      channel_list[i].channel_dstIp.family = PTIN_AF_INET6;
      memcpy( channel_list[i].channel_dstIp.addr.ipv6, igmpAssoc_list[i_start+i].groupAddr.addr.ipv6.in6.addr8, sizeof(L7_uchar8)*16);
      channel_list[i].channel_dstmask = 128;
    }
    else
    {
      channel_list[i].channel_dstIp.family = PTIN_AF_INET;
      channel_list[i].channel_dstIp.addr.ipv4 = igmpAssoc_list[i_start+i].groupAddr.addr.ipv4.s_addr;
      channel_list[i].channel_dstmask = 32;
    }

    /* Source address (prepared for IPv6) */
    if ( igmpAssoc_list[i_start+i].sourceAddr.family == L7_AF_INET6 )
    {
      channel_list[i].channel_srcIp.family = PTIN_AF_INET6;
      memcpy( channel_list[i].channel_srcIp.addr.ipv6, igmpAssoc_list[i_start+i].sourceAddr.addr.ipv6.in6.addr8, sizeof(L7_uchar8)*16);
      channel_list[i].channel_srcmask = 128;
    }
    else
    {
      channel_list[i].channel_srcIp.family = PTIN_AF_INET;
      channel_list[i].channel_srcIp.addr.ipv4 = igmpAssoc_list[i_start+i].sourceAddr.addr.ipv4.s_addr;
      channel_list[i].channel_srcmask = 32;
    }

    //channel_list[i].???       = igmpAssoc_list[i_start+i].evc_uc;
    channel_list[i].evcid_mc  = igmpAssoc_list[i_start+i].evc_mc;
    //channel_list[i].???       = igmpAssoc_list[i_start+i].is_static;
  }

  /* Return number of channels */
  *n_channels = number_of_channels;

#else

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Not supported!");
  return L7_NOT_SUPPORTED;

#endif

  return L7_SUCCESS;
}

/**
 * Add channels to White list
 * 
 * @param channel_list : Channel list array
 * @param n_channels : Number of channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_ChannelAssoc_add(msg_MCAssocChannel_t *channel_list, L7_uint16 n_channels)
{
  L7_uint16 i;
  L7_inet_addr_t groupAddr, sourceAddr;
  L7_RC_t rc = L7_SUCCESS;

  if (channel_list==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  for (i=0; i<n_channels; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Adding channel index %u:",i);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Slot   = %d",channel_list[i].SlotId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," EVC_MC = %d",channel_list[i].evcid_mc);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Entry_idx = %d",channel_list[i].entry_idx);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," DstIP_Channel = 0x%08x (ipv6=%u) / %u",channel_list[i].channel_dstIp.addr.ipv4, channel_list[i].channel_dstIp.family, channel_list[i].channel_dstmask);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," SrcIP_Channel = 0x%08x (ipv6=%u) / %u",channel_list[i].channel_srcIp.addr.ipv4, channel_list[i].channel_srcIp.family, channel_list[i].channel_srcmask);

    #if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
    LOG_DEBUG(LOG_CTX_PTIN_MSG," channelBandwidth = %llu kbits/s", channel_list[i].channelBandwidth);

    if (channel_list[i].channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_KBPS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid channelBandwidth:%llu kbits/s",channel_list[i].channelBandwidth);
      return L7_FAILURE;
    }
    #endif
    /* Prepare group address */
    memset(&groupAddr, 0x00, sizeof(L7_inet_addr_t));
    if (channel_list[i].channel_dstIp.family == PTIN_AF_INET6)
    {
      groupAddr.family = L7_AF_INET6;
      memcpy(groupAddr.addr.ipv6.in6.addr8, channel_list[i].channel_dstIp.addr.ipv6, sizeof(L7_uint8)*16);
    }
    else
    {
      groupAddr.family = L7_AF_INET;
      groupAddr.addr.ipv4.s_addr = channel_list[i].channel_dstIp.addr.ipv4;
    }
    /* Prepare source address */
    memset(&sourceAddr, 0x00, sizeof(L7_inet_addr_t));
    if (channel_list[i].channel_srcIp.family == PTIN_AF_INET6)
    {
      sourceAddr.family = L7_AF_INET6;
      memcpy(sourceAddr.addr.ipv6.in6.addr8, channel_list[i].channel_srcIp.addr.ipv6, sizeof(L7_uint8)*16);
    }
    else
    {
      sourceAddr.family = L7_AF_INET;
      sourceAddr.addr.ipv4.s_addr = channel_list[i].channel_srcIp.addr.ipv4;
    }

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED

    if ((rc=igmp_assoc_channel_add( 0, channel_list[i].evcid_mc,
                                    &groupAddr , channel_list[i].channel_dstmask,
                                    &sourceAddr, channel_list[i].channel_srcmask, L7_FALSE, channel_list[i].channelBandwidth )) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error adding group address 0x%08x/%u, source address 0x%08x/%u to MC EVC %u",
              channel_list[i].channel_dstIp.addr.ipv4, channel_list[i].channel_dstmask,
              channel_list[i].channel_srcIp.addr.ipv4, channel_list[i].channel_srcmask,
              channel_list[i].evcid_mc);
      return rc;
    }

#else
    rc = L7_NOT_SUPPORTED;
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Not supported!");
    break;
#endif
  }

  return rc;
}

/**
 * Remove channels to white list
 * 
 * @param channel_list : Channel list array
 * @param n_channels : Number of channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_ChannelAssoc_remove(msg_MCAssocChannel_t *channel_list, L7_uint16 n_channels)
{
  L7_uint16 i;
  L7_inet_addr_t groupAddr, sourceAddr;
  L7_RC_t rc = L7_SUCCESS;
  L7_RC_t rc_global = L7_SUCCESS;

  if (channel_list==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  for (i=0; i<n_channels; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Removing channel index %u:",i);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Slot   = %d",channel_list[i].SlotId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," EVC_MC = %d",channel_list[i].evcid_mc);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Entry_idx = %d",channel_list[i].entry_idx);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," DstIP_Channel = 0x%08x (ipv6=%u) / %u",channel_list[i].channel_dstIp.addr.ipv4, channel_list[i].channel_dstIp.family, channel_list[i].channel_dstmask);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," SrcIP_Channel = 0x%08x (ipv6=%u) / %u",channel_list[i].channel_srcIp.addr.ipv4, channel_list[i].channel_srcIp.family, channel_list[i].channel_srcmask);

    /* Prepare group address */
    memset(&groupAddr, 0x00, sizeof(L7_inet_addr_t));
    if (channel_list[i].channel_dstIp.family == PTIN_AF_INET6)
    {
      groupAddr.family = L7_AF_INET6;
      memcpy(groupAddr.addr.ipv6.in6.addr8, channel_list[i].channel_dstIp.addr.ipv6, sizeof(L7_uint8)*16);
    }
    else
    {
      groupAddr.family = L7_AF_INET;
      groupAddr.addr.ipv4.s_addr = channel_list[i].channel_dstIp.addr.ipv4;
    }
    /* Prepare source address */
    memset(&sourceAddr, 0x00, sizeof(L7_inet_addr_t));
    if (channel_list[i].channel_srcIp.family == PTIN_AF_INET6)
    {
      sourceAddr.family = L7_AF_INET6;
      memcpy(sourceAddr.addr.ipv6.in6.addr8, channel_list[i].channel_srcIp.addr.ipv6, sizeof(L7_uint8)*16);
    }
    else
    {
      sourceAddr.family = L7_AF_INET;
      sourceAddr.addr.ipv4.s_addr = channel_list[i].channel_srcIp.addr.ipv4;
    }

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED

    if ((rc = igmp_assoc_channel_remove( 0,
                                   &groupAddr , channel_list[i].channel_dstmask,
                                   &sourceAddr, channel_list[i].channel_srcmask )) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error (%d) removing group address 0x%08x/%u, source address 0x%08x/%u to MC EVC %u",
              rc,
              channel_list[i].channel_dstIp.addr.ipv4, channel_list[i].channel_dstmask,
              channel_list[i].channel_srcIp.addr.ipv4, channel_list[i].channel_srcmask,
              channel_list[i].evcid_mc);
      rc_global = rc;
      continue;
    }

#else
    rc = L7_NOT_SUPPORTED;
    rc_global = L7_NOT_SUPPORTED;
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Not supported!");
    break;
#endif
  }

  return rc_global;
}

/**
 * Remove all channels to white list
 * 
 * @param channel_list : Channel list array
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_ChannelAssoc_remove_all(msg_MCAssocChannel_t *channel_list, L7_uint16 n_channels)
{
  L7_uint16 i;
  L7_RC_t rc = L7_SUCCESS;
  L7_RC_t rc_global = L7_SUCCESS;

  if (channel_list==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  for (i=0; i<n_channels; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Removing channel index %u:",i);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Slot   = %d",channel_list[i].SlotId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," EVC_MC = %d",channel_list[i].evcid_mc);

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED

    if ((rc = igmp_assoc_channel_clear(-1, channel_list[i].evcid_mc)) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error (%d) removing groups to MC EVC %u", rc, channel_list[i].evcid_mc);
      rc_global = rc;
      continue;
    }

#else
    rc = L7_NOT_SUPPORTED;
    rc_global = L7_NOT_SUPPORTED;
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Not supported!");
    break;
#endif
  }

  return rc_global;
}

/**
 * Add a static group channel to MFDB table
 * 
 * @param channel : static group channel
 * @param n_channels : Number of channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_staticChannel_add(msg_MCStaticChannel_t *channel, L7_uint16 n_channels)
{
  PTIN_MGMD_CTRL_STATICGROUP_t staticGroup={0};
  L7_uint16 i;
  L7_RC_t rc = L7_SUCCESS;

  if (channel==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }
    

  for (i=0; i<n_channels; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Static channel addition index %u:",i);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," SlotId           = %u",channel[i].SlotId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," EvcId            = %u",channel[i].evc_id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Channel          = %u.%u.%u.%u",
              (channel[i].channelIp.s_addr>>24) & 0xff,(channel[i].channelIp.s_addr>>16) & 0xff,(channel[i].channelIp.s_addr>>8) & 0xff,channel[i].channelIp.s_addr & 0xff);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," SourceIP         = %u.%u.%u.%u",
              (channel[i].sourceIp.s_addr>>24) & 0xff,(channel[i].sourceIp.s_addr>>16) & 0xff,(channel[i].sourceIp.s_addr>>8) & 0xff,channel[i].sourceIp.s_addr & 0xff);

    #if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
    LOG_DEBUG(LOG_CTX_PTIN_MSG," channelBandwidth = %llu", channel[i].channelBandwidth);
                               
    if (channel[i].channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_KBPS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid channelBandwidth = %llu", channel[i].channelBandwidth);
      return L7_FAILURE;
    }
    #endif

    staticGroup.serviceId = channel[i].evc_id;
    staticGroup.groupIp   = channel[i].channelIp.s_addr;
    staticGroup.sourceIp  = channel[i].sourceIp.s_addr;
    staticGroup.portType  = PTIN_MGMD_PORT_TYPE_LEAF;

    if ((rc = ptin_igmp_static_channel_add(&staticGroup)) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error (%d) adding static channel", rc);
      return rc;
    }

    #ifdef IGMPASSOC_MULTI_MC_SUPPORTED//Add Static Channel to (WhiteList) Group List     
    msg_MCAssocChannel_t channel_list;  

    channel_list.SlotId=channel[i].SlotId;
    channel_list.evcid_mc=channel[i].evc_id;

    channel_list.channel_dstIp.family = PTIN_AF_INET;
    channel_list.channel_dstIp.addr.ipv4 = channel[i].channelIp.s_addr;
    channel_list.channel_dstmask=32;//32 Bits of Mask

    channel_list.channel_srcIp.family=PTIN_AF_INET;
    channel_list.channel_srcIp.addr.ipv4 = channel[i].sourceIp.s_addr;
    channel_list.channel_srcmask=32;

    channel_list.channelBandwidth = channel[i].channelBandwidth;
    
    ptin_msg_IGMP_ChannelAssoc_add(&channel_list,1);   
    #endif//End Static Channel Add

  }

  return rc;
}

/**
 * Remove a static group channel from MFDB table
 * 
 * @param channel : static group channel
 * @param n_channels : Number of channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_channel_remove(msg_MCStaticChannel_t *channel, L7_uint16 n_channels)
{
  PTIN_MGMD_CTRL_STATICGROUP_t staticGroup={0};
  L7_uint16 i;
  L7_RC_t rc;
  L7_RC_t rc_global = L7_SUCCESS;

  if (channel==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  for (i=0; i<n_channels; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Channel remotion index %u:",i);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," SlotId =%u",channel[i].SlotId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," EvcId  =%u",channel[i].evc_id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Channel=%u.%u.%u.%u",
              (channel[i].channelIp.s_addr>>24) & 0xff,(channel[i].channelIp.s_addr>>16) & 0xff,(channel[i].channelIp.s_addr>>8) & 0xff,channel[i].channelIp.s_addr & 0xff);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," SourceIP=%u.%u.%u.%u",
              (channel[i].sourceIp.s_addr>>24) & 0xff,(channel[i].sourceIp.s_addr>>16) & 0xff,(channel[i].sourceIp.s_addr>>8) & 0xff,channel[i].sourceIp.s_addr & 0xff);

    staticGroup.serviceId = channel[i].evc_id;
    staticGroup.groupIp   = channel[i].channelIp.s_addr;
    staticGroup.sourceIp  = channel[i].sourceIp.s_addr;
    staticGroup.portType  = PTIN_MGMD_PORT_TYPE_LEAF;

    if ((rc = ptin_igmp_channel_remove(&staticGroup)) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error (%d) removing channel", rc);
      rc_global = rc;
      continue;
    }

    #ifdef IGMPASSOC_MULTI_MC_SUPPORTED//Remove Static Channel from (WhiteList) Group List     
    msg_MCAssocChannel_t channel_list;  

    channel_list.SlotId=channel[i].SlotId;
    channel_list.evcid_mc=channel[i].evc_id;

    channel_list.channel_dstIp.family=PTIN_AF_INET;
    channel_list.channel_dstIp.addr.ipv4=channel[i].channelIp.s_addr;
    channel_list.channel_dstmask=32;//32 Bits of Mask

    channel_list.channel_srcIp.family=PTIN_AF_INET;
    channel_list.channel_srcIp.addr.ipv4 = channel[i].sourceIp.s_addr;
    channel_list.channel_srcmask=32;

    channel_list.channelBandwidth = channel[i].channelBandwidth;
    
    ptin_msg_IGMP_ChannelAssoc_remove(&channel_list,1);   
    #endif//End Static Channel Remove
    
  }

  return rc_global;
}

/**
 * Consult list of multicast channels
 * 
 * @param channel_list : list of multicast channels
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_channelList_get(msg_MCActiveChannelsRequest_t *inputPtr, msg_MCActiveChannelsReply_t *outputPtr, L7_uint16 *numberOfChannels)
{
  ptin_igmpChannelInfo_t clist[*numberOfChannels];
  L7_uint16              i, number_of_channels, total_channels;
  ptin_client_id_t       client;
  L7_RC_t                rc;

  if (numberOfChannels == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid parameters");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Going to retrieve list of channels");
  LOG_DEBUG(LOG_CTX_PTIN_MSG," slotId = %u",             inputPtr->SlotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," EvcId  = %u",             inputPtr->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Client.Mask  = 0x%02x",   inputPtr->client.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Client.OVlan = %u",       inputPtr->client.outer_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Client.IVlan = %u",       inputPtr->client.inner_vlan);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Client.Intf  = %u/%u",    inputPtr->client.intf.intf_type,inputPtr->client.intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Entry_idx=%u",            inputPtr->entryId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Max Number of Channels=%u",*numberOfChannels);

  //Short Fix to Support Mac Bridge Services and Unicast Services
  #if PTIN_BOARD_IS_LINECARD
  {
    #if 0
    L7_BOOL isMacBridge;    
    if(ptin_evc_mac_bridge_check(inputPtr->evc_id, &isMacBridge)==L7_SUCCESS && isMacBridge==L7_TRUE)
    #else
    if (inputPtr->client.outer_vlan==0) 
    #endif
    {        
      inputPtr->client.outer_vlan=inputPtr->client.inner_vlan;        
    }
    inputPtr->client.mask|=MSG_CLIENT_OVLAN_MASK;
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Converted [client.Mask:%u Client.OVlan:%u]",inputPtr->client.mask,inputPtr->client.outer_vlan);
  }
  #endif
  
  /* Client info */
  memset(&client,0x00,sizeof(ptin_client_id_t));
  if (inputPtr->client.mask & MSG_CLIENT_INTF_MASK)
  {
    client.ptin_intf.intf_type = inputPtr->client.intf.intf_type;
    client.ptin_intf.intf_id   = inputPtr->client.intf.intf_id;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  }
  if (inputPtr->client.mask & MSG_CLIENT_IVLAN_MASK)
  {
    client.innerVlan = inputPtr->client.inner_vlan;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
  }
  if (inputPtr->client.mask & MSG_CLIENT_OVLAN_MASK)
  {
    client.outerVlan = inputPtr->client.outer_vlan;
    client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  }

  /* Get list of channels */
  number_of_channels = *numberOfChannels;
  rc = ptin_igmp_channelList_get(inputPtr->evc_id, &client, inputPtr->entryId, &number_of_channels, clist, &total_channels);

  if (rc==L7_SUCCESS)
  {
     /* Copy channels to message */
    for (i=0; i<(*numberOfChannels) && i<number_of_channels; i++)
    {
      LOG_TRACE(LOG_CTX_PTIN_MSG,"Client[%u] -> Group:[%08X] Source[%08X]", i, clist[i].groupAddr.addr.ipv4.s_addr, clist[i].sourceAddr.addr.ipv4.s_addr);
      outputPtr[i].entryId = i + inputPtr->entryId;
      outputPtr[i].chIP    = clist[i].groupAddr.addr.ipv4.s_addr;
      outputPtr[i].srcIP   = clist[i].sourceAddr.addr.ipv4.s_addr;
      outputPtr[i].chType  = clist[i].static_type;
    }
     *numberOfChannels = i;
     LOG_DEBUG(LOG_CTX_PTIN_MSG, "Read %u channels and retrieving %u channels.",number_of_channels,*numberOfChannels);
  }
  else if (rc==L7_NOT_EXIST)
  {
    *numberOfChannels = 0;
    LOG_NOTICE(LOG_CTX_PTIN_MSG, "No channels to retrieve");
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error with ptin_igmp_channelList_get");
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Process Snoop Sync Message Request
 * 
 * @param msg_SnoopSyncRequest_t : 
 *  
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_snoop_sync_request(msg_SnoopSyncRequest_t *snoopSyncRequest)
{
  L7_uint16 mcastRootVlan = 0;
  L7_RC_t   rc;

  if (snoopSyncRequest==L7_NULLPTR )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid input parameters snoopSyncRequest=%p",snoopSyncRequest);
    return L7_FAILURE;
  }
  
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Received Snoop Sync Request Message");     
  LOG_DEBUG(LOG_CTX_PTIN_MSG," serviceId=%u",snoopSyncRequest->serviceId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," groupAddr=%08X",snoopSyncRequest->groupAddr);
#if !PTIN_BOARD_IS_MATRIX  
  LOG_DEBUG(LOG_CTX_PTIN_MSG," portId=%u",snoopSyncRequest->portId);
#endif

  if( (snoopSyncRequest->serviceId != 0 &&  snoopSyncRequest->groupAddr != 0)
       && (L7_SUCCESS != (rc=ptin_evc_intRootVlan_get(snoopSyncRequest->serviceId, &mcastRootVlan))))
  {
    if( rc != L7_NOT_EXIST)
    {
      LOG_ERR(LOG_CTX_PTIN_IGMP, "Unable to get mcastRootVlan from serviceId:%u",snoopSyncRequest->serviceId);
      return rc;
    }
#if PTIN_BOARD_IS_MATRIX 
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Evc Id is not yet created. Silently Ignoring Snoop Sync Request! [serviceId:%u groupAddr:%08X]", snoopSyncRequest->serviceId, snoopSyncRequest->groupAddr);
#else
    LOG_NOTICE(LOG_CTX_PTIN_IGMP, "Evc Id is not yet created. Silently Ignoring Snoop Sync Request! [serviceId:%u portId:%u groupAddr:%08X]", snoopSyncRequest->serviceId, snoopSyncRequest->portId, snoopSyncRequest->groupAddr);
#endif
    return rc;
  }

   
#if PTIN_BOARD_IS_MATRIX    
  return (ptin_snoop_sync_mx_process_request(mcastRootVlan, snoopSyncRequest->groupAddr));            
#else    
  return (ptin_snoop_sync_port_process_request(mcastRootVlan, snoopSyncRequest->groupAddr,snoopSyncRequest->portId));           
#endif                
}

/**
 * Process Snoop Sync Message Reply
 * 
 * @param msg_SnoopSyncReply_t : 
 * @param numberOfSnoopEntries :
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_snoop_sync_reply(msg_SnoopSyncReply_t *snoopSyncReply, L7_uint32 numberOfSnoopEntries)
{
  L7_uint32  maxNumberOfSnoopEntries  =  IPCLIB_MAX_MSGSIZE/sizeof(msg_SnoopSyncReply_t); //IPC buffer size / struct size 
  L7_uint32  iterator; 
  L7_uint32  sourceAddr = 0x0;
  
  if (snoopSyncReply==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid parameters: snoopSyncReply=%p",snoopSyncReply);
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Received Snoop Sync Reply Message: numberOfSnoopEntries=%u | maxNumberOfSnoopEntries:%u",numberOfSnoopEntries,maxNumberOfSnoopEntries);
  
  if(numberOfSnoopEntries==0)
  {
    LOG_NOTICE(LOG_CTX_PTIN_MSG,"The number of snoop entries is equal to zero. Silently ignoring this reply message.");
    return L7_SUCCESS;
  }
    
  #if PTIN_BOARD_IS_MATRIX
  {
    L7_uint32 intIfNum;
    L7_uint32 numberOfActivePorts;

    for(iterator=0;iterator < numberOfSnoopEntries; iterator++)
    {
      LOG_TRACE(LOG_CTX_PTIN_MSG," serviceId=%u",snoopSyncReply[iterator].serviceId);
      LOG_TRACE(LOG_CTX_PTIN_MSG," groupAddr=%08X",snoopSyncReply[iterator].groupAddr);
      LOG_TRACE(LOG_CTX_PTIN_MSG," StaticEntry=%s",snoopSyncReply[iterator].isStatic?"Yes":"No");
      LOG_TRACE(LOG_CTX_PTIN_MSG," numberOfActivePorts=%u",snoopSyncReply[iterator].numberOfActivePorts);
      numberOfActivePorts=0;
      if(snoopSyncReply[iterator].numberOfActivePorts>0)
      {
        for (intIfNum=1;intIfNum<L7_MAX_INTERFACE_COUNT;intIfNum++)
        {   
          if (L7_INTF_ISMASKBITSET(snoopSyncReply[iterator].snoopGrpMemberList,intIfNum))
          {
            LOG_DEBUG(LOG_CTX_PTIN_PROTB, "Snoop Port Open :%u", intIfNum);
            if(snooping_port_open(snoopSyncReply[iterator].serviceId, intIfNum, snoopSyncReply[iterator].groupAddr, sourceAddr, snoopSyncReply[iterator].isStatic)!=L7_SUCCESS)
            {
              LOG_ERR(LOG_CTX_PTIN_PROTB, "Failed to open port");
              return L7_FAILURE;
            }

            if(++numberOfActivePorts>=snoopSyncReply[iterator].numberOfActivePorts)
            {
              break;
            }
          }
        }
      }
    }
  }
  #else
  {
    for(iterator=0;iterator < numberOfSnoopEntries; iterator++)
    { 
      LOG_TRACE(LOG_CTX_PTIN_MSG," serviceId=%u",snoopSyncReply[iterator].serviceId);
      LOG_TRACE(LOG_CTX_PTIN_MSG," groupAddr=%08X",snoopSyncReply[iterator].groupAddr);
      LOG_TRACE(LOG_CTX_PTIN_MSG," StaticEntry=%s",snoopSyncReply[iterator].isStatic?"Yes":"No");   
      LOG_TRACE(LOG_CTX_PTIN_MSG," portId=%u",snoopSyncReply->portId);
      
      if(snooping_port_open(snoopSyncReply[iterator].serviceId, snoopSyncReply[iterator].portId, snoopSyncReply[iterator].groupAddr, sourceAddr, snoopSyncReply[iterator].isStatic)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_PROTB, "Failed to open port");
        return L7_FAILURE;
      }
    }
  }
  #endif

  //Request the remaining snoop entries
  if(numberOfSnoopEntries!=maxNumberOfSnoopEntries)
  {
    LOG_TRACE(LOG_CTX_PTIN_MSG, "This is the Last Snoop Sync Reply Message Received");
    return L7_SUCCESS;
  }

  msg_SnoopSyncRequest_t   snoopSyncRequest;
  L7_uint32                ipAddr;

  snoopSyncRequest.groupAddr    = snoopSyncReply[numberOfSnoopEntries-1].groupAddr;
  snoopSyncRequest.serviceId    = snoopSyncReply[numberOfSnoopEntries-1].serviceId;

#if PTIN_BOARD_IS_MATRIX    
  if(ptin_fgpa_mx_is_matrixactive())//If I'm a Working Matrix
  {
    LOG_NOTICE(LOG_CTX_PTIN_MSG, "Not sending Another Snoop Sync Request Message to Sync the Remaining Snoop Entries. I'm a Working Matrix on slotId:%u",ptin_fgpa_board_slot());
    return SUCCESS;
  }

  /* MX board IP address */
  ipAddr = IPC_MX_IPADDR;
  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Sending Snoop Sync Request Message [groupAddr:%08X | serviceId:%u] to ipAddr:%08X to Sync the Remaining Snoop Entries", snoopSyncRequest.groupAddr, snoopSyncRequest.serviceId, ipAddr);         
#else
  ptin_prottypeb_intf_config_t protTypebIntfConfig = {0};     

  /*  Get the configuration of this portId for the Type B Scheme Protection */
  ptin_prottypeb_intf_config_get(snoopSyncReply[numberOfSnoopEntries-1].portId, &protTypebIntfConfig);    

  if(protTypebIntfConfig.status==L7_ENABLE)//If I'm a Protection
  {
    LOG_NOTICE(LOG_CTX_PTIN_MSG, "Not sending Another Snoop Sync Request Message to Sync the Remaining Snoop Entries. I'm a Working slotId/intfNum:%u/%u",protTypebIntfConfig.pairSlotId, protTypebIntfConfig.intfNum);
    return SUCCESS;
  }
    
  /* Determine the IP address of the working port/slot */
  ipAddr = 0xC0A8C800 /*192.168.200.X*/ | ((protTypebIntfConfig.pairSlotId+1) & 0x000000FF); 

  snoopSyncRequest.portId    = protTypebIntfConfig.pairIntfNum;     

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Sending Snoop Sync Request Message [groupAddr:%08X | serviceId:%u | portId:%u] to ipAddr:%08X to Sync the Remaining Snoop Entries", snoopSyncRequest.groupAddr, snoopSyncRequest.serviceId, snoopSyncRequest.portId, ipAddr);
#endif
              
  
  /*Send the snoop sync request to the protection matrix */  
  if (send_ipc_message(IPC_HW_FASTPATH_PORT, ipAddr, CCMSG_MGMD_SNOOP_SYNC_REQUEST, (char *)(&snoopSyncRequest), NULL, sizeof(snoopSyncRequest), NULL) < 0)
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Failed to Send Snoop Sync Request Message");
    return L7_FAILURE;
  }
  
  return L7_SUCCESS;
}

/**
 * Consult list of clients of a particular multicast channel
 * 
 * @param client_list : list of client vlans
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_clientList_get(msg_MCActiveChannelClients_t *client_list)
{
  L7_in_addr_t     channelIp;
  ptin_client_id_t clist[MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX];
  L7_uint32        extended_evc_id[MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX];
  L7_uint16        i, number_of_clients, total_clients;
  L7_RC_t rc;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Going to retrieve list of clients");
  LOG_DEBUG(LOG_CTX_PTIN_MSG," slotId     =%u",client_list->SlotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," EvcId      =%u",client_list->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," IPv4Channel=%u.%u.%u.%u",(client_list->channelIp.s_addr>>24) & 0xff,(client_list->channelIp.s_addr>>16) & 0xff,(client_list->channelIp.s_addr>>8) & 0xff,client_list->channelIp.s_addr & 0xff);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Page_idx=%u",client_list->page_index);

  /* Get list of channels */
  channelIp.s_addr    = client_list->channelIp.s_addr;
  number_of_clients   = MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX;

  rc = ptin_igmp_clientList_get(client_list->evc_id, &channelIp, client_list->page_index*MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX, &number_of_clients, clist, extended_evc_id,&total_clients);
  LOG_DEBUG(LOG_CTX_PTIN_MSG,"number_of_clients=%u total_clients=%u", number_of_clients, total_clients);
  if (rc==L7_SUCCESS)
  {
    /* Copy channels to message */
    for (i=0; i<MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX && i<number_of_clients; i++)
    {
      client_list->clients_list[i].mask           = clist[i].mask;
      client_list->clients_list[i].outer_vlan     = clist[i].outerVlan;
      client_list->clients_list[i].inner_vlan     = clist[i].innerVlan;
      client_list->clients_list[i].intf.intf_type = clist[i].ptin_intf.intf_type;
      client_list->clients_list[i].intf.intf_id   = clist[i].ptin_intf.intf_id;
      //MAC Bridge Services Support
      client_list->clients_list[i].evc_id         = extended_evc_id[i];
      //End MAC Bridge Services Support
    }
    client_list->n_pages_total   = (total_clients==0) ? 1 : ((total_clients-1)/MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX+1);
    client_list->n_clients_total = total_clients;
    client_list->n_clients_msg   = number_of_clients;
  }
  else if (rc==L7_NOT_EXIST)
  {
    LOG_WARNING(LOG_CTX_PTIN_MSG, "No channels to retrieve");
    client_list->n_pages_total   = 1;
    client_list->n_clients_total = 0;
    client_list->n_clients_msg   = 0;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error with ptin_igmp_clientList_get");
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Remove all static group channel from MFDB table
 * 
 * @param channel : static group channel
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_channel_remove_all(msg_MCStaticChannel_t *channel, L7_uint16 n_channels)
{
  PTIN_MGMD_CTRL_STATICGROUP_t staticGroup;
  L7_uint16 i;
  L7_RC_t rc = L7_SUCCESS;
  L7_RC_t rc_global = L7_SUCCESS;

  if (channel==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }
  for (i=0; i<n_channels; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Channel remotion index %u:",i);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," SlotId =%u",channel[i].SlotId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," EvcId  =%u",channel[i].evc_id);

    staticGroup.serviceId = channel[i].evc_id;
    
    if ((ptin_igmp_mgmd_service_remove(channel[i].evc_id)) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error (%d) removing channel", rc);
      rc_global = rc;
      continue;
    }
  }

  return rc_global;
}

/**
 * Uplink protection command
 * 
 * @param cmd : command array
 * @param n : number of commands 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_uplink_protection_cmd(msg_uplinkProtCmd *cmd, L7_int n)
{
  L7_RC_t rc = L7_SUCCESS;

#ifdef PTIN_SYSTEM_PROTECTION_LAGID_BASE
  L7_int i, i2rem, i2add;

  i2add = i2rem = -1;

  /* Verify plan to be followed */
  for (i = 0; i < n; i++) 
  {
    LOG_TRACE(LOG_CTX_PTIN_MSG, "Received protection command: "); 
    LOG_TRACE(LOG_CTX_PTIN_MSG, " slot = %u", cmd[i].slotId);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " port = %u", cmd[i].port);
    LOG_TRACE(LOG_CTX_PTIN_MSG, " cmd  = %u", cmd[i].protCmd);

    if (n > 1)
    {
      if      ( (cmd[i].protCmd & 1) && i2add < 0)  i2add = i;
      else if (!(cmd[i].protCmd & 1) && i2rem < 0)  i2rem = i;
    }
  }

  /* If provided a port to be removed, and to be added, follow plan D for those ports */
  if (i2add >= 0 && i2rem >= 0)
  {
    LOG_TRACE(LOG_CTX_PTIN_MSG, "Applying plan D for slot/ports %d/%d -> %d/%d",
              cmd[i2rem].slotId, cmd[i2rem].port, cmd[i2add].slotId, cmd[i2add].port);
    /* PLAN D */
    if (ptin_intf_protection_cmd_planD(cmd[i2rem].slotId, cmd[i2rem].port,
                                       cmd[i2add].slotId, cmd[i2add].port) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Plan D failed for slot/ports %d/%d -> %d/%d",
              cmd[i2rem].slotId, cmd[i2rem].port, cmd[i2add].slotId, cmd[i2add].port);
      rc = L7_FAILURE;
    }
  }

  /* Apply plan C for other ports */
  for (i = 0; i < n; i++) 
  {
    /* Skip ports used for plan D */
    if (i == i2rem || i == i2add)  continue;

    LOG_TRACE(LOG_CTX_PTIN_MSG, "Applying plan C for slot/port %d/%d", cmd[i].slotId, cmd[i].port);
    /* PLAN C */
    if (ptin_intf_protection_cmd_planC(cmd[i].slotId, cmd[i].port, cmd[i].protCmd) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Plan C failed for slot/port %d/%d", cmd[i].slotId, cmd[i].port);
      rc = L7_FAILURE;
    }
  }

  /* Check for result */
  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Protection switch failed"); 
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Successfull protection switch!"); 
  }
#else
  LOG_ERR(LOG_CTX_PTIN_MSG, "Uplink protection not supported for this board!");
  rc = L7_NOT_SUPPORTED;
#endif

  return rc;
}

/**
 * Sync MGMD open ports
 * 
 * @param port_sync_data : MGMD port to sync
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_mgmd_sync_ports(msg_HwMgmdPortSync *port_sync_data)
{
  LOG_TRACE(LOG_CTX_PTIN_MSG, "Received request to sync MGMD port: ");
  LOG_TRACE(LOG_CTX_PTIN_MSG, " admin      = %u",   port_sync_data->admin);
  LOG_TRACE(LOG_CTX_PTIN_MSG, " serviceId  = %u",   port_sync_data->serviceId);
  LOG_TRACE(LOG_CTX_PTIN_MSG, " portId     = %u",   port_sync_data->portId);
  LOG_TRACE(LOG_CTX_PTIN_MSG, " groupAddr  = %08X", port_sync_data->groupAddr);
  LOG_TRACE(LOG_CTX_PTIN_MSG, " sourceAddr = %08X", port_sync_data->sourceAddr);
  LOG_TRACE(LOG_CTX_PTIN_MSG, " groupType  = %u",   port_sync_data->groupType);

  ptin_igmp_mgmd_port_sync(port_sync_data->admin, port_sync_data->serviceId, port_sync_data->portId, port_sync_data->groupAddr, port_sync_data->sourceAddr, port_sync_data->groupType);

  return L7_SUCCESS;
}

/**
 * Enable PRBS tx/rx
 * 
 * @param msg : PRBS configuration
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_pcs_prbs_enable(msg_ptin_pcs_prbs *msg, L7_int n_msg)
{
  ptin_intf_t ptin_intf;
  L7_uint32 i, intIfNum;
  L7_RC_t rc;

  for (i=0; i<n_msg; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"PRBS configuration:");
    LOG_DEBUG(LOG_CTX_PTIN_MSG," slotId = %u",msg[i].SlotId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Port   = %u/%u",msg[i].intf.intf_type,msg[i].intf.intf_id);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Enable = %u",msg[i].enable);

    ptin_intf.intf_type = msg[i].intf.intf_type;
    ptin_intf.intf_id   = msg[i].intf.intf_id;

    /* Get intIfNum */
    if (ptin_intf_ptintf2intIfNum(&ptin_intf, &intIfNum)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Non existent port (%u/%u)",ptin_intf.intf_type,ptin_intf.intf_id);
      return L7_FAILURE;
    }

    rc = ptin_pcs_prbs_enable(intIfNum,msg[i].enable);
    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error settings PRBS enable of port %u/%u to %u",ptin_intf.intf_type,ptin_intf.intf_id,msg[i].enable);
      return rc;
    }

    LOG_TRACE(LOG_CTX_PTIN_MSG,"Success setting PRBS enable of port %u/%u to %u",ptin_intf.intf_type,ptin_intf.intf_id,msg[i].enable);
  }

  return L7_SUCCESS;
}

/**
 * Read PRBS errors
 * 
 * @param msg : PRBS configuration
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_pcs_prbs_status(msg_ptin_pcs_prbs *msg, L7_int n_msg)
{
  ptin_intf_t ptin_intf;
  L7_uint32 i, intIfNum, rxStatus;
  L7_RC_t   rc;

  for (i=0; i<n_msg; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"PRBS status:");
    LOG_DEBUG(LOG_CTX_PTIN_MSG," slotId = %u",msg[i].SlotId);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Port   = %u/%u",msg[i].intf.intf_type,msg[i].intf.intf_id);

    ptin_intf.intf_type = msg[i].intf.intf_type;
    ptin_intf.intf_id   = msg[i].intf.intf_id;

    /* Get intIfNum */
    if (ptin_intf_ptintf2intIfNum(&ptin_intf, &intIfNum)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Non existent port (%u/%u)",ptin_intf.intf_type,ptin_intf.intf_id);
      return L7_FAILURE;
    }

    /* Read number of PRBS errors */
    rc = ptin_pcs_prbs_errors_get(intIfNum, &rxStatus);
    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error getting PRBS errors from port %u/%u",ptin_intf.intf_type,ptin_intf.intf_id);
      return rc;
    }

    LOG_TRACE(LOG_CTX_PTIN_MSG,"Success getting PRBS errors from port %u/%u",ptin_intf.intf_type,ptin_intf.intf_id);

    if (rxStatus<=0xffff)
    {
      msg[i].rxStatus.lock = L7_TRUE;
      msg[i].rxStatus.rxErrors = rxStatus;
    }
  }

  return L7_SUCCESS;
}

/****************************************************************************** 
 * STATIC FUNCTIONS IMPLEMENTATION
 ******************************************************************************/

/**
 * Convert manager interface representation to ptin_port
 * 
 * @param intf_type : interface type (0:physical, 1:Lag)
 * @param intf_id : Interface id 
 * @param ptin_port : ptin representation of port 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE;
 */
static L7_RC_t ptin_msg_ptinPort_get(L7_uint8 intf_type, L7_uint8 intf_id, L7_int *ptin_port)
{
  L7_int port = -1;

  if (intf_type)
  {
    port = (L7_int) intf_id + PTIN_SYSTEM_N_PORTS;
    if (port<PTIN_SYSTEM_N_PORTS || port>=PTIN_SYSTEM_N_INTERF)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    port = (L7_int) intf_id;
    if (port>=PTIN_SYSTEM_N_PORTS)
    {
      return L7_FAILURE;
    }
  }

  if (ptin_port!=L7_NULLPTR)  *ptin_port = port;

  return L7_SUCCESS;
}

/**
 * Execute drivshell or devshell command
 * 
 * @param tty  tty caller
 * @param type is a driv or a devshell command?
 * @param cmd  command string
 * 
 * @return L7_RC_t Return code L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_shell_command_run(L7_char8 *tty, L7_char8 *type, L7_char8 *cmd)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_char8 *prevtty = ttyname(1);

  ptin_PitHandler(tty);

  if (strcmp(type, "driv") == 0)
  {
    dtlDriverShell(cmd);
  }
  else if (strcmp(type, "dev") == 0)
  {
    if (osapiDevShellExec(cmd) != 0)
      rc = L7_FAILURE;
  }

  ptin_PitHandler(prevtty);

  return rc;
}


/**
 * Copy ptin_HWEthRFC2819_PortStatistics_t (unpacked) to msg_HWEthRFC2819_PortStatistics_t (packed)
 * 
 * @param msgPortStats  Destination
 * @param ptinPortStats Source
 */
static void ptin_msg_PortStats_convert(msg_HWEthRFC2819_PortStatistics_t  *msgPortStats,
                                       ptin_HWEthRFC2819_PortStatistics_t *ptinPortStats)
{
  msgPortStats->Port                              = ptinPortStats->Port;
  msgPortStats->Mask                              = ptinPortStats->Mask;
  msgPortStats->RxMask                            = ptinPortStats->RxMask;
  msgPortStats->TxMask                            = ptinPortStats->TxMask;
  /* Rx */
  msgPortStats->Rx.etherStatsDropEvents           = ptinPortStats->Rx.etherStatsDropEvents;
  msgPortStats->Rx.etherStatsOctets               = ptinPortStats->Rx.etherStatsOctets;
  msgPortStats->Rx.etherStatsPkts                 = ptinPortStats->Rx.etherStatsPkts;
  msgPortStats->Rx.etherStatsBroadcastPkts        = ptinPortStats->Rx.etherStatsBroadcastPkts;
  msgPortStats->Rx.etherStatsMulticastPkts        = ptinPortStats->Rx.etherStatsMulticastPkts;
  msgPortStats->Rx.etherStatsCRCAlignErrors       = ptinPortStats->Rx.etherStatsCRCAlignErrors;
  msgPortStats->Rx.etherStatsUndersizePkts        = ptinPortStats->Rx.etherStatsUndersizePkts;
  msgPortStats->Rx.etherStatsOversizePkts         = ptinPortStats->Rx.etherStatsOversizePkts;
  msgPortStats->Rx.etherStatsFragments            = ptinPortStats->Rx.etherStatsFragments;
  msgPortStats->Rx.etherStatsJabbers              = ptinPortStats->Rx.etherStatsJabbers;
  msgPortStats->Rx.etherStatsCollisions           = ptinPortStats->Rx.etherStatsCollisions;
  msgPortStats->Rx.etherStatsPkts64Octets         = ptinPortStats->Rx.etherStatsPkts64Octets;
  msgPortStats->Rx.etherStatsPkts65to127Octets    = ptinPortStats->Rx.etherStatsPkts65to127Octets;
  msgPortStats->Rx.etherStatsPkts128to255Octets   = ptinPortStats->Rx.etherStatsPkts128to255Octets;
  msgPortStats->Rx.etherStatsPkts256to511Octets   = ptinPortStats->Rx.etherStatsPkts256to511Octets;
  msgPortStats->Rx.etherStatsPkts512to1023Octets  = ptinPortStats->Rx.etherStatsPkts512to1023Octets;
  msgPortStats->Rx.etherStatsPkts1024to1518Octets = ptinPortStats->Rx.etherStatsPkts1024to1518Octets;
  msgPortStats->Rx.etherStatsPkts1519toMaxOctets  = ptinPortStats->Rx.etherStatsPkts1519toMaxOctets;
  msgPortStats->Rx.Throughput                     = ptinPortStats->Rx.Throughput;
  /* Tx */
  msgPortStats->Tx.etherStatsDropEvents           = ptinPortStats->Tx.etherStatsDropEvents;
  msgPortStats->Tx.etherStatsOctets               = ptinPortStats->Tx.etherStatsOctets;
  msgPortStats->Tx.etherStatsPkts                 = ptinPortStats->Tx.etherStatsPkts;
  msgPortStats->Tx.etherStatsBroadcastPkts        = ptinPortStats->Tx.etherStatsBroadcastPkts;
  msgPortStats->Tx.etherStatsMulticastPkts        = ptinPortStats->Tx.etherStatsMulticastPkts;
  msgPortStats->Tx.etherStatsCRCAlignErrors       = ptinPortStats->Tx.etherStatsCRCAlignErrors;
  msgPortStats->Tx.etherStatsUndersizePkts        = ptinPortStats->Tx.etherStatsUndersizePkts;
  msgPortStats->Tx.etherStatsOversizePkts         = ptinPortStats->Tx.etherStatsOversizePkts;
  msgPortStats->Tx.etherStatsFragments            = ptinPortStats->Tx.etherStatsFragments;
  msgPortStats->Tx.etherStatsJabbers              = ptinPortStats->Tx.etherStatsJabbers;
  msgPortStats->Tx.etherStatsCollisions           = ptinPortStats->Tx.etherStatsCollisions;
  msgPortStats->Tx.etherStatsPkts64Octets         = ptinPortStats->Tx.etherStatsPkts64Octets;
  msgPortStats->Tx.etherStatsPkts65to127Octets    = ptinPortStats->Tx.etherStatsPkts65to127Octets;
  msgPortStats->Tx.etherStatsPkts128to255Octets   = ptinPortStats->Tx.etherStatsPkts128to255Octets;
  msgPortStats->Tx.etherStatsPkts256to511Octets   = ptinPortStats->Tx.etherStatsPkts256to511Octets;
  msgPortStats->Tx.etherStatsPkts512to1023Octets  = ptinPortStats->Tx.etherStatsPkts512to1023Octets;
  msgPortStats->Tx.etherStatsPkts1024to1518Octets = ptinPortStats->Tx.etherStatsPkts1024to1518Octets;
  msgPortStats->Tx.etherStatsPkts1519toMaxOctets  = ptinPortStats->Tx.etherStatsPkts1519toMaxOctets;
  msgPortStats->Tx.Throughput                     = ptinPortStats->Tx.Throughput;
}

/**
 * Transfer data from msg_HwEthBwProfile_t structure to 
 * ptin_bw_profile_t. 
 * 
 * @param msgBwProfile : structure unsed in incoming message
 * @param profile : structure to be passed to further 
 *                processing.
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_msg_bwProfileStruct_fill(msg_HwEthBwProfile_t *msgBwProfile, ptin_bw_profile_t *profile)
{
  L7_int    ptin_port;
  L7_uint32 intIfNum;
  nimUSP_t  usp;

  /* Validate arguments */
  if (msgBwProfile==L7_NULLPTR || profile==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Null arguments");
    return L7_FAILURE;
  }

  /* SVID */
  profile->outer_vlan_in        = 0;
  profile->outer_vlan_internal  = 0;
  profile->outer_vlan_out       = 0;

  /* CVID */
  profile->inner_vlan_in  = 0;
  profile->inner_vlan_out = 0;

  /* Source interface */
  profile->ddUsp_src.unit = profile->ddUsp_src.slot = profile->ddUsp_src.port = -1;
  if (msgBwProfile->mask & MSG_HWETH_BWPROFILE_MASK_INTF_SRC)
  {
    /* SVID */
    if ((msgBwProfile->mask & MSG_HWETH_BWPROFILE_MASK_SVLAN) &&
        (msgBwProfile->service_vlan>0 && msgBwProfile->service_vlan<4096))
    {
      profile->outer_vlan_in = msgBwProfile->service_vlan;
      LOG_DEBUG(LOG_CTX_PTIN_MSG," SVID extracted!");
    }

    /* CVID */
    if ((msgBwProfile->mask & MSG_HWETH_BWPROFILE_MASK_CVLAN) &&
        (msgBwProfile->client_vlan>0 && msgBwProfile->client_vlan<4096))
    {
      profile->inner_vlan_in = msgBwProfile->client_vlan;
      LOG_DEBUG(LOG_CTX_PTIN_MSG," CVID extracted!");
    }

    /* Get ptin_port */
    if (ptin_msg_ptinPort_get(msgBwProfile->intf_src.intf_type,msgBwProfile->intf_src.intf_id, &ptin_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid port reference");
      return L7_FAILURE;
    }
    /* Get intIfNum */
    if (ptin_intf_port2intIfNum(ptin_port,&intIfNum)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Non existent port");
      return L7_FAILURE;
    }
    /* Get USP */
    if (nimGetUnitSlotPort(intIfNum,&usp)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error getting USP reference");
      return L7_FAILURE;
    }
    /* Calculate ddUSP */
    profile->ddUsp_src.unit = usp.unit;
    profile->ddUsp_src.slot = usp.slot;
    profile->ddUsp_src.port = usp.port - 1;
    LOG_DEBUG(LOG_CTX_PTIN_MSG," SrcIntf extracted!");
  }

  /* Destination interface */
  profile->ddUsp_dst.unit = profile->ddUsp_dst.slot = profile->ddUsp_dst.port = -1;
  if (msgBwProfile->mask & MSG_HWETH_BWPROFILE_MASK_INTF_DST)
  {
    /* SVID */
    if ((msgBwProfile->mask & MSG_HWETH_BWPROFILE_MASK_SVLAN) &&
        (msgBwProfile->service_vlan>0 && msgBwProfile->service_vlan<4096))
    {
      profile->outer_vlan_out = msgBwProfile->service_vlan;
      LOG_DEBUG(LOG_CTX_PTIN_MSG," SVID extracted!");
    }

    /* CVID */
    if ((msgBwProfile->mask & MSG_HWETH_BWPROFILE_MASK_CVLAN) &&
        (msgBwProfile->client_vlan>0 && msgBwProfile->client_vlan<4096))
    {
      profile->inner_vlan_out = msgBwProfile->client_vlan;
      LOG_DEBUG(LOG_CTX_PTIN_MSG," CVID extracted!");
    }

    /* Get ptin_port */
    if (ptin_msg_ptinPort_get(msgBwProfile->intf_dst.intf_type,msgBwProfile->intf_dst.intf_id, &ptin_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid port reference");
      return L7_FAILURE;
    }
    /* Get intIfNum */
    if (ptin_intf_port2intIfNum(ptin_port,&intIfNum)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Non existent port");
      return L7_FAILURE;
    }
    /* Get USP */
    if (nimGetUnitSlotPort(intIfNum,&usp)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Error getting USP reference");
      return L7_FAILURE;
    }
    /* Calculate ddUSP */
    profile->ddUsp_dst.unit = usp.unit;
    profile->ddUsp_dst.slot = usp.slot;
    profile->ddUsp_dst.port = usp.port - 1;
    LOG_DEBUG(LOG_CTX_PTIN_MSG," DstIntf extracted!");
  }

  if (msgBwProfile->mask & MSG_HWETH_BWPROFILE_MASK_PROFILE)
  {
    profile->meter.cir = (L7_uint32) (msgBwProfile->profile.cir/1000);   /* in kbps */
    profile->meter.cbs = (L7_uint32) msgBwProfile->profile.cbs;          /* in bytes */
    profile->meter.eir = (L7_uint32) (msgBwProfile->profile.eir/1000);   /* in kbps */
    profile->meter.ebs = (L7_uint32) msgBwProfile->profile.ebs;          /* in bytes */
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Profile data extracted!");
  }
  else
  {
    profile->meter.cir = (L7_uint32) -1;
    profile->meter.cbs = (L7_uint32) -1;
    profile->meter.eir = (L7_uint32) -1;
    profile->meter.ebs = (L7_uint32) -1;
  }

  return L7_SUCCESS;
}

/**
 * Transfer data from msg_HwEthBwProfile_t structure to 
 * ptin_bw_profile_t. 
 * 
 * @param msgBwProfile : structure unsed in incoming message
 * @param profile : structure to be passed to further 
 *                processing.
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t ptin_msg_evcStatsStruct_fill(msg_evcStats_t *msg_evcStats, ptin_evcStats_profile_t *evcStats_profile)
{
  L7_int    ptin_port;

  /* Validate arguments */
  if (msg_evcStats==L7_NULLPTR || evcStats_profile==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Null arguments");
    return L7_FAILURE;
  }

  /* Source interface */
  if (msg_evcStats->mask & MSG_EVC_COUNTERS_MASK_INTF)
  {
    /* Get ptin_port */
    if (ptin_msg_ptinPort_get(msg_evcStats->intf.intf_type,msg_evcStats->intf.intf_id, &ptin_port)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG,"Invalid port reference");
      return L7_FAILURE;
    }
    evcStats_profile->ptin_port = ptin_port;
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Intf extracted!");
  }

  /* SVID */
  evcStats_profile->outer_vlan_in       = 0;
  evcStats_profile->outer_vlan_internal = 0;
  evcStats_profile->outer_vlan_out      = 0;
  if ((msg_evcStats->mask & MSG_EVC_COUNTERS_MASK_SVLAN) &&
      (msg_evcStats->service_vlan>0 && msg_evcStats->service_vlan<4096))
  {
    evcStats_profile->outer_vlan_in = msg_evcStats->service_vlan;
    LOG_DEBUG(LOG_CTX_PTIN_MSG," SVID extracted!");
  }

  /* CVID */
  evcStats_profile->inner_vlan_in  = 0;
  evcStats_profile->inner_vlan_out = 0;
  if ((msg_evcStats->mask & MSG_EVC_COUNTERS_MASK_CVLAN) &&
      (msg_evcStats->client_vlan>0 && msg_evcStats->client_vlan<4096))
  {
    evcStats_profile->inner_vlan_in = msg_evcStats->client_vlan;
    LOG_DEBUG(LOG_CTX_PTIN_MSG," CVID extracted!");
  }

  /* Channel IP */
  evcStats_profile->dst_ip = 0;
  if ((msg_evcStats->mask & MSG_EVC_COUNTERS_MASK_CHANNEL) &&
      (msg_evcStats->channel_ip!=0 && msg_evcStats->channel_ip!=(L7_uint32)-1))
  {
    evcStats_profile->dst_ip = msg_evcStats->channel_ip;
    LOG_DEBUG(LOG_CTX_PTIN_MSG," ChannelIP extracted!");
  }

  return L7_SUCCESS;
}



#ifdef __Y1731_802_1ag_OAM_ETH__
  #include <sirerrors.h>
  #include <ptin_prot_oam_eth.h>
/************************************************************************** 
* OAM MEPs Configuration
**************************************************************************/

/**
 * Used to create a new MEP
 * 
 * @author joaom (5/31/2013)
 * 
 * @param inbuff 
 * @param outbuff 
 * @param i 
 * 
 * @return int 
 */
L7_RC_t ptin_msg_wr_MEP(ipc_msg *inbuff, ipc_msg *outbuff, L7_uint32 i)
{
  msg_bd_mep_t              *pi;
  msg_generic_prefix_t      *po;
  L7_uint16                 r = S_OK;
  L7_uint32                 porta;
  //L7_uint8                  changing_trap;
  //L7_uint16                 old_prt=-1, old_vid=-1, old_level=-1;
  T_MEP                     *p;


  pi=(msg_bd_mep_t *)inbuff->info;
  po=(msg_generic_prefix_t *)outbuff->info;
  po[i].index = pi[i].index;

  porta = pi[i].bd.prt;

  if ((pi[i].flags & 0x01)) {
  L7_uint16 slot, port;

    if (L7_SUCCESS!=ptin_intf_port2SlotPort(porta, &slot, &port, L7_NULLPTR)) return L7_FAILURE;
    if (slot!=pi[i].tu_slot) {
        LOG_ERR(LOG_CTX_PTIN_MSG, "ptin_intf_port=%lu => (slot,port)=(%u,%u) struct_passed_slot=%u", porta, slot, port, pi[i].tu_slot);
        //return L7_FAILURE;
    }
    if (send_also_uplinkprot_traps(1, slot, port, pi[i].bd.vid));// return L7_FAILURE;
  }


  if (valid_mep_index(pi[i].index)) {
      p = &oam.db[pi[i].index].mep;
      if (EMPTY_T_MEP(*p)) ;//changing_trap=0;
      else {
          //old_prt=p->prt;       old_vid=p->vid;     old_level=p->level;
          //changing_trap=1;  //if (old_prt!=porta || old_vid!=pi[i].bd.vid || old_level!=pi[i].bd.level) changing_trap=1;    else changing_trap=0;
          ptin_msg_del_MEP(inbuff, outbuff, i);
      }
  }
  //else changing_trap=0;


  switch (wr_mep(pi[i].index, (T_MEP_HDR*)&pi[i].bd, &oam))
  {
  case 0:    r=S_OK;
    //if (changing_trap)  ptin_ccm_packet_trap(old_prt, old_vid, old_level, 0);
    ptin_ccm_packet_trap(porta, pi[i].bd.vid, pi[i].bd.level, 1);
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "i_MEP#%llu\tporta=%lu\tvid=%llu\tlevel=%lu", pi[i].index, porta, pi[i].bd.vid, pi[i].bd.level);
    break;
  case 2:    r=ERROR_CODE_FULLTABLE;    break;
  case 3:    r=  CCMSG_FLUSH_MEP==inbuff->msgId?   S_OK:   ERROR_CODE_FULLTABLE; break;
  case 4:    r=ERROR_CODE_NOTPRESENT;  break;
  default:   r=ERROR_CODE_INVALIDPARAM; break;
  }//switch


  if (r==S_OK)
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }

}//msg_wr_MEP


/**
 * Used to remove a MEP
 * 
 * @author joaom (5/31/2013)
 * 
 * @param inbuff 
 * @param outbuff 
 * @param i 
 * 
 * @return int 
 */
L7_RC_t ptin_msg_del_MEP(ipc_msg *inbuff, ipc_msg *outbuff, L7_uint32 i)
{
  msg_bd_mep_t *pi;
  msg_generic_prefix_t *po;
  L7_uint32 i_mep;
  L7_uint16 r=L7_HARDWARE_ERROR;
  L7_uint16 prt=-1, vid=-1, level=-1;

  pi=(msg_bd_mep_t *)inbuff->info;   po=(msg_generic_prefix_t *)outbuff->info;
  i_mep=po[i].index=pi[i].index;

  if (i_mep<N_MEPs)
  {
    prt=oam.db[i_mep].mep.prt;
    vid=oam.db[i_mep].mep.vid;
    level=oam.db[i_mep].mep.level;
  }


  switch (del_mep(i_mep, &oam))
  {
  case 0:    r=S_OK;
    ptin_ccm_packet_trap(prt, vid, level, 0);
    {
     L7_uint16 slot, port;

      if (L7_SUCCESS==ptin_intf_port2SlotPort(prt, &slot, &port, L7_NULLPTR)) send_also_uplinkprot_traps(0, slot, port, vid);
    }

    break;
    //case 2:    r=HW_RESOURCE_UNAVAILABLE;  break;
  default:   r=ERROR_CODE_INVALIDPARAM; break;
  }//switch


  if (r==S_OK)
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }

}//msg_del_MEP



/**
 * Used to create a new RMEP
 * 
 * @author joaom (5/31/2013)
 * 
 * @param inbuff 
 * @param outbuff 
 * @param i 
 * 
 * @return int 
 */
L7_RC_t ptin_msg_wr_RMEP(ipc_msg *inbuff, ipc_msg *outbuff, L7_uint32 i)
{
  msg_bd_rmep_t *pi;
  msg_generic_prefix_t *po;
  T_ETH_SRV_OAM *p_oam;
  L7_uint32 i_mep, i_rmep;
  L7_uint16 r=L7_HARDWARE_ERROR;

  pi=(msg_bd_rmep_t *)inbuff->info;   po=(msg_generic_prefix_t *)outbuff->info;
  po[i].index=pi[i].index;

  i_mep=     MEP_INDEX_TO_iMEP(pi[i].index);

  if (!valid_mep_index(i_mep))
  {
    return L7_FAILURE;
  }

  i_rmep=    MEP_INDEX_TO_iRMEP(pi[i].index);


  p_oam= &oam;
  switch (wr_rmep(i_mep, i_rmep, &pi[i].bd, (T_MEP_HDR*)&p_oam->db[i_mep].mep, p_oam))
  {
  case 0:
    r=S_OK;
    break;
  case 4:
    if (CCMSG_FLUSH_RMEP==inbuff->msgId)
    {
      r=S_OK;
      break;
    }
    r=ERROR_CODE_FULLTABLE;
    break;
  case 5:
  case 6:
    r=ERROR_CODE_NOTPRESENT;
    break;
  case 7:
    r=ERROR_CODE_FULLTABLE;
    break;
  default:
    r=ERROR_CODE_INVALIDPARAM;
  }


  if (r==S_OK)
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }

}//msg_wr_RMEP


/**
 * Used to remove a RMEP
 * 
 * @author joaom (5/31/2013)
 * 
 * @param inbuff 
 * @param outbuff 
 * @param i 
 * 
 * @return int 
 */
L7_RC_t ptin_msg_del_RMEP(ipc_msg *inbuff, ipc_msg *outbuff, L7_uint32 i)
{
  msg_bd_rmep_t *pi;
  msg_generic_prefix_t *po;
  T_ETH_SRV_OAM *p_oam;
  L7_uint32 i_mep, i_rmep;
  L7_uint16 r=L7_HARDWARE_ERROR;

  pi=(msg_bd_rmep_t *)inbuff->info;   po=(msg_generic_prefix_t *)outbuff->info;
  po[i].index=pi[i].index;

  i_mep=     MEP_INDEX_TO_iMEP(pi[i].index);
  i_rmep=    MEP_INDEX_TO_iRMEP(pi[i].index);


  p_oam= &oam;

  switch (del_rmep(i_mep, i_rmep, p_oam))
  {
  case 0:    r=S_OK;             break;
    //case 2:    r=HW_RESOURCE_UNAVAILABLE; break;
  default:   r=ERROR_CODE_INVALIDPARAM;
  }


  if (r==S_OK)
  {
    return L7_SUCCESS;
  }
  else
  {
    return L7_FAILURE;
  }

}//msg_del_RMEP


/**
 * Used to dump MEPs
 * 
 * @author joaom (5/31/2013)
 * 
 * @param inbuff 
 * @param outbuff 
 * @param i 
 * 
 * @return int 
 */
L7_RC_t ptin_msg_dump_MEPs(ipc_msg *inbuff, ipc_msg *outbuff)
{
  msg_generic_prefix_t *pi;
  msg_bd_mep_t *po;
  L7_uint32 n=0;
  L7_uint32 i;
  T_ETH_SRV_OAM   *p_oam;

  pi=(msg_generic_prefix_t *)inbuff->info;   po=(msg_bd_mep_t *)outbuff->info;

  if (pi->index>=N_MEPs)
  {
    return(L7_FAILURE);
  }


  p_oam= &oam;

  for (i=pi->index, n=0; i<N_MEPs; i++)
  {
    po[n].index = i;
    po[n].err_code = S_OK;

    //set_active_to_(p_oam->mep_db);
    //if (!active_is_used(p_oam->mep_db))
    //_p_mep= pointer2active_node_info(*p_mep_db);

    if (!EMPTY_T_MEP(p_oam->db[i].mep)  ||  N_MEPs-1==i)
      po[n++].bd=  *((T_MEP_HDR *) &p_oam->db[i].mep);

    if (n+1 > 15   ||  (n+1)*sizeof(msg_bd_mep_t) >= IPCLIB_MAX_MSGSIZE) break;// if (n+1 > 100) break;// if ((n+1)*sizeof(msg_bd_mep_t) >= INFO_DIM_MAX) break;
  }//for


  outbuff->infoDim = n*sizeof(msg_bd_mep_t);
  return L7_SUCCESS;

}//msg_dump_MEPs


/**
 * Used to dump MPs
 * 
 * @author joaom (5/31/2013)
 * 
 * @param inbuff 
 * @param outbuff 
 * @param i 
 * 
 * @return int 
 */
L7_RC_t ptin_msg_dump_MEs(ipc_msg *inbuff, ipc_msg *outbuff) {
  msg_bd_me_t *pi;//Exception: in and out are of the same type
  msg_bd_me_t *po;
  L7_uint32 n=0, i_mep, i_rmep;
  L7_uint32 i;
  T_ETH_SRV_OAM   *p_oam;

  pi=(msg_bd_me_t *)inbuff->info;   po=(msg_bd_me_t *)outbuff->info;

  i_mep=     MEP_INDEX_TO_iMEP(pi->index);
  i_rmep=    MEP_INDEX_TO_iRMEP(pi->index);

  if (!valid_mep_index(i_mep) || !valid_rmep_index(i_rmep))
  {
    return(L7_FAILURE);
  }


  p_oam= &oam;

  if (EMPTY_T_MEP(p_oam->db[i_mep].mep))
  {
    return(L7_FAILURE);
  }

  for (i=i_rmep, n=0; i<N_MAX_MEs_PER_MEP; i++)
  {
    po[n].index = iMEP_iRMEP_TO_MEP_INDEX(i_mep, i);//i_mep*0x10000L+i;
    po[n].err_code = S_OK;

    if (   !EMPTY_T_MEP(p_oam->db[i_mep].mep.ME[i])
           ||  N_MAX_MEs_PER_MEP-1==i)
    {
      po[n].bd.me=     p_oam->db[i_mep].mep.ME[i];
      //if (0L-1==po[n].bd.me.LOC_timer) po[n].bd.me.RDI=0;
      n++;
    }

    if (n+1 > 17   ||  (n+1)*sizeof(msg_bd_me_t) >= IPCLIB_MAX_MSGSIZE) break;// if (n+1 > 100) break;// if ((n+1)*sizeof(msg_bd_me_t) >= INFO_DIM_MAX) break;
  }//for


  outbuff->infoDim = n*sizeof(msg_bd_me_t);

  return L7_SUCCESS;

}//msg_dump_MEs


/**
 * Used to dump LUT MEPs
 * 
 * @author joaom (5/31/2013)
 * 
 * @param inbuff 
 * @param outbuff 
 * @param i 
 * 
 * @return int 
 */
L7_RC_t ptin_msg_dump_LUT_MEPs(ipc_msg *inbuff, ipc_msg *outbuff) {
  msg_generic_prefix_t *pi;
  msg_bd_lut_mep_t *po;
  L7_uint32 n=0;
  L7_uint32 i;
  T_ETH_SRV_OAM   *p_oam;

  pi=(msg_generic_prefix_t *)inbuff->info;   po=(msg_bd_lut_mep_t *)outbuff->info;

  if (pi->index>=N_MAX_LOOKUP_MEPs)
  {
    return(L7_FAILURE);
  }


  p_oam= &oam;

  for (i=pi->index, n=0; i<N_MAX_LOOKUP_MEPs; i++)
  {
    po[n].index = i;
    po[n].err_code = S_OK;

    if (!EMPTY_T_MEP(p_oam->mep_lut[i]))
    {
      po[n++].bd= p_oam->mep_lut[i];
    }

    if (n+1 > 15   ||  (n+1)*sizeof(msg_bd_lut_mep_t) >= IPCLIB_MAX_MSGSIZE) break;// if (n+1 > 100) break;// if ((n+1)*sizeof(msg_bd_lut_mep_t) >= INFO_DIM_MAX) break;
  }//for


  outbuff->infoDim = n*sizeof(msg_bd_lut_mep_t);

  return L7_SUCCESS;

}//msg_dump_LUT_MEPs
#endif //__Y1731_802_1ag_OAM_ETH__



/****************************************************************************** 
 * ERPS Configuration
 ******************************************************************************/

/**
 * ERPS Creation
 * 
 * @author joaom (6/4/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_erps_set(msg_erps_t *msgErpsConf)
{

#ifdef PTIN_ENABLE_ERPS

  erpsProtParam_t ptinErpsConf;

  /* Validate ERPS# range (idx [0..MAX_PROT_PROT_ERPS[) */
  if (msgErpsConf->idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ERPS#%u is out of range [0..%u]", msgErpsConf->idx, MAX_PROT_PROT_ERPS-1);
    return L7_FAILURE;
  }


  /* Copy data to ptin struct */
  ptinErpsConf.ringId               = msgErpsConf->ringId;
  ptinErpsConf.isOpenRing           = msgErpsConf->isOpenRing;

  ptinErpsConf.controlVid           = msgErpsConf->controlVid;
  ptinErpsConf.megLevel             = msgErpsConf->megLevel;

  ptinErpsConf.port0.slot           = msgErpsConf->port0.slot;
  ptinErpsConf.port0.type           = msgErpsConf->port0.type;
  ptinErpsConf.port0.idx            = msgErpsConf->port0.idx;
  ptinErpsConf.port1.slot           = msgErpsConf->port1.slot;
  ptinErpsConf.port1.type           = msgErpsConf->port1.type;
  ptinErpsConf.port1.idx            = msgErpsConf->port1.idx;
  ptinErpsConf.port0Role            = msgErpsConf->port0Role;
  ptinErpsConf.port1Role            = msgErpsConf->port1Role;
  ptinErpsConf.port0CfmIdx          = msgErpsConf->port0CfmIdx;
  ptinErpsConf.port1CfmIdx          = msgErpsConf->port1CfmIdx;

  ptinErpsConf.revertive            = msgErpsConf->revertive;
  ptinErpsConf.guardTimer           = msgErpsConf->guardTimer;
  ptinErpsConf.holdoffTimer         = msgErpsConf->holdoffTimer;
  ptinErpsConf.waitToRestoreTimer   = msgErpsConf->waitToRestoreTimer;

  ptinErpsConf.continualTxInterval  = 5;  // 5 seconds
  ptinErpsConf.rapidTxInterval      = 0;  // 3.33 ms

  memcpy(ptinErpsConf.vid_bmp, msgErpsConf->vid_bmp, sizeof(ptinErpsConf.vid_bmp));

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ERPS#%u",                    msgErpsConf->idx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .ringId             = %d",  ptinErpsConf.ringId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .isOpenRing         = %d",  ptinErpsConf.isOpenRing);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .controlVid         = %d",  ptinErpsConf.controlVid);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .megLevel           = %d",  ptinErpsConf.megLevel);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .port0.slot         = %d",  ptinErpsConf.port0.slot);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .port0.type         = %d",  ptinErpsConf.port0.type);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .port0.idx          = %d",  ptinErpsConf.port0.idx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .port1.slot         = %d",  ptinErpsConf.port1.slot);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .port1.type         = %d",  ptinErpsConf.port1.type);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .port1.idx          = %d",  ptinErpsConf.port1.idx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .port0Role          = %d",  ptinErpsConf.port0Role);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .port1Role          = %d",  ptinErpsConf.port1Role);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .port0CfmIdx        = %d",  ptinErpsConf.port0CfmIdx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .port1CfmIdx        = %d",  ptinErpsConf.port1CfmIdx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .revertive          = %d",  ptinErpsConf.revertive);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .guardTimer         = %d",  ptinErpsConf.guardTimer);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .holdoffTimer       = %d",  ptinErpsConf.holdoffTimer);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .waitToRestoreTimer = %d",  ptinErpsConf.waitToRestoreTimer);

  if (ptin_erps_add_entry(msgErpsConf->idx, (erpsProtParam_t *) &ptinErpsConf) != msgErpsConf->idx)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error Creating ERPS#%u", msgErpsConf->idx);
    return L7_FAILURE;
  }

  ptin_hal_erps_entry_init(msgErpsConf->idx);

#endif  // PTIN_ENABLE_ERPS

  return L7_SUCCESS;

}


/**
 * ERPS removal
 * 
 * @author joaom (6/22/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_erps_del(msg_erps_t *msgErpsConf)
{

#ifdef PTIN_ENABLE_ERPS

  /* Validate ERPS# range (idx [0..MAX_PROT_PROT_ERPS[) */
  if (msgErpsConf->idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ERPS#%u is out of range [0..%u]", msgErpsConf->idx, MAX_PROT_PROT_ERPS-1);
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ERPS#%u", msgErpsConf->idx);

  if (ptin_erps_remove_entry(msgErpsConf->idx) != msgErpsConf->idx)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error Removing ERPS#%u", msgErpsConf->idx);
    ptin_hal_erps_entry_deinit(msgErpsConf->idx);
    return L7_FAILURE;
  }

  ptin_hal_erps_entry_deinit(msgErpsConf->idx);

#endif  // PTIN_ENABLE_ERPS

  return L7_SUCCESS;

}


/**
 * ERPS Reconfiguration
 * 
 * @author joaom (6/22/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_erps_config(msg_erps_t *msgErpsConf)
{

#ifdef PTIN_ENABLE_ERPS

  erpsProtParam_t ptinErpsConf;

  /* Validate ERPS# range (idx [0..MAX_PROT_PROT_ERPS[) */
  if (msgErpsConf->idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ERPS#%u is out of range [0..%u]", msgErpsConf->idx, MAX_PROT_PROT_ERPS-1);
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ERPS#%u",                     msgErpsConf->idx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .mask               = 0x%x", msgErpsConf->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .isOpenRing         = %d",   msgErpsConf->isOpenRing);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .port0CfmIdx        = %d",   msgErpsConf->port0CfmIdx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .port1CfmIdx        = %d",   msgErpsConf->port1CfmIdx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .revertive          = %d",   msgErpsConf->revertive);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .guardTimer         = %d",   msgErpsConf->guardTimer);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .holdoffTimer       = %d",   msgErpsConf->holdoffTimer);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .waitToRestoreTimer = %d",   msgErpsConf->waitToRestoreTimer);

  /* Copy data to ptin struct */

  if (msgErpsConf->mask & ERPS_CONF_MASK_BIT_ISOPENRING)    ptinErpsConf.isOpenRing         = msgErpsConf->isOpenRing;

  if (msgErpsConf->mask & ERPS_CONF_MASK_BIT_PORT0CFMIDX)   ptinErpsConf.port0CfmIdx        = msgErpsConf->port0CfmIdx;
  if (msgErpsConf->mask & ERPS_CONF_MASK_BIT_PORT1CFMIDX)   ptinErpsConf.port1CfmIdx        = msgErpsConf->port1CfmIdx;

  if (msgErpsConf->mask & ERPS_CONF_MASK_BIT_REVERTIVE)     ptinErpsConf.revertive          = msgErpsConf->revertive;
  if (msgErpsConf->mask & ERPS_CONF_MASK_BIT_GUARDTIMER)    ptinErpsConf.guardTimer         = msgErpsConf->guardTimer;
  if (msgErpsConf->mask & ERPS_CONF_MASK_BIT_HOLDOFFTIMER)  ptinErpsConf.holdoffTimer       = msgErpsConf->holdoffTimer;
  if (msgErpsConf->mask & ERPS_CONF_MASK_BIT_WAITTORESTORE) ptinErpsConf.waitToRestoreTimer = msgErpsConf->waitToRestoreTimer;

  if (msgErpsConf->mask & ERPS_CONF_MASK_BIT_VIDBMP)        memcpy(ptinErpsConf.vid_bmp, msgErpsConf->vid_bmp, sizeof(ptinErpsConf.vid_bmp));

  if (ptin_erps_conf_entry(msgErpsConf->idx, msgErpsConf->mask, (erpsProtParam_t *) &ptinErpsConf) != msgErpsConf->idx)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error creating/reconfiguring ERPS#%u", msgErpsConf->idx);
    return L7_FAILURE;
  }

  ptin_hal_erps_convert_vid_init(msgErpsConf->idx);

#endif  // PTIN_ENABLE_ERPS

  return L7_SUCCESS;

}


/**
 * ERPS status
 * 
 * @author joaom (6/24/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_erps_status(msg_erps_status_t *msgErpsStatus)
{

#ifdef PTIN_ENABLE_ERPS

  erpsStatus_t status;

  /* Validate ERPS# range (idx [0..MAX_PROT_PROT_ERPS[) */
  if (msgErpsStatus->idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ERPS#%u is out of range [0..%u]", msgErpsStatus->idx, MAX_PROT_PROT_ERPS-1);
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ERPS#%u", msgErpsStatus->idx);

  if (ptin_erps_get_status(msgErpsStatus->idx, &status) != msgErpsStatus->idx)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error Retrieving Status ERPS#%u", msgErpsStatus->idx);
    return L7_FAILURE;
  }

  msgErpsStatus->slotId             = msgErpsStatus->slotId;
  msgErpsStatus->idx                = msgErpsStatus->idx;
  msgErpsStatus->port0_SF           = status.port0_SF;
  msgErpsStatus->port1_SF           = status.port1_SF;
  msgErpsStatus->port0State         = status.port0State;
  msgErpsStatus->port1State         = status.port1State;

  msgErpsStatus->apsReqStatusTx     = status.apsReqStatusTx;
  msgErpsStatus->apsReqStatusRxP0   = status.apsReqStatusRxP0;
  msgErpsStatus->apsReqStatusRxP1   = status.apsReqStatusRxP1;

  memcpy(msgErpsStatus->apsNodeIdRxP0, status.apsNodeIdRxP0, PROT_ERPS_MAC_SIZE);
  memcpy(msgErpsStatus->apsNodeIdRxP1, status.apsNodeIdRxP1, PROT_ERPS_MAC_SIZE);

  msgErpsStatus->state_machine      = status.state_machine;
  msgErpsStatus->dnfStatus          = status.dnfStatus;

  msgErpsStatus->guard_timer        = status.guard_timer;
  msgErpsStatus->wtr_timer          = status.wtr_timer;
  msgErpsStatus->wtb_timer          = status.wtb_timer;
  msgErpsStatus->holdoff_timer      = status.holdoff_timer;

#endif  // PTIN_ENABLE_ERPS

  return L7_SUCCESS;

}


/**
 * ERPS status
 * 
 * @author joaom (6/24/2013)
 * 
 * @param ptr 
 * @param n
 * 
 * @return L7_RC_t 
 */
int ptin_msg_erps_status_next(msg_erps_status_t *msgErpsStatus, L7_int *n)
{

#ifdef PTIN_ENABLE_ERPS

  erpsStatus_t  status;
  L7_uint32     nextIdx, i;

  L7_uint8      slotId = msgErpsStatus->slotId;

  *n = 0;
  i  = 0;

  nextIdx = msgErpsStatus->idx + 1;
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ERPS Next Index %d", nextIdx);

  while ( i < CCMSG_ERPS_STATUS_PAGESIZE )
  {

    /* Validate ERPS# range (idx [0..MAX_PROT_PROT_ERPS[) */
    if (nextIdx >= MAX_PROT_PROT_ERPS)
    {
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "ERPS#%u is out of range [0..%u]", nextIdx, MAX_PROT_PROT_ERPS-1);
      break;
    }

    if (ptin_erps_get_status(nextIdx, &status) != nextIdx)
    {
      nextIdx++;
      continue;
    }

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "ERPS#%d status retrieved", nextIdx);

    msgErpsStatus[i].slotId             = slotId;
    msgErpsStatus[i].idx                = nextIdx;
    msgErpsStatus[i].port0_SF           = status.port0_SF;
    msgErpsStatus[i].port1_SF           = status.port1_SF;
    msgErpsStatus[i].port0State         = status.port0State;
    msgErpsStatus[i].port1State         = status.port1State;

    msgErpsStatus[i].apsReqStatusTx     = status.apsReqStatusTx;
    msgErpsStatus[i].apsReqStatusRxP0   = status.apsReqStatusRxP0;
    msgErpsStatus[i].apsReqStatusRxP1   = status.apsReqStatusRxP1;

    memcpy(msgErpsStatus[i].apsNodeIdRxP0, status.apsNodeIdRxP0, PROT_ERPS_MAC_SIZE);
    memcpy(msgErpsStatus[i].apsNodeIdRxP1, status.apsNodeIdRxP1, PROT_ERPS_MAC_SIZE);

    msgErpsStatus[i].state_machine      = status.state_machine;
    msgErpsStatus[i].dnfStatus          = status.dnfStatus;

    msgErpsStatus[i].guard_timer        = status.guard_timer;
    msgErpsStatus[i].wtr_timer          = status.wtr_timer;
    msgErpsStatus[i].wtb_timer          = status.wtb_timer;
    msgErpsStatus[i].holdoff_timer      = status.holdoff_timer;

    i++;
    nextIdx++;
    *n += 1;

  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "n=%d", *n);

#endif  // PTIN_ENABLE_ERPS

  return L7_SUCCESS;
}


/**
 * ERPS Commands
 * 
 * @author joaom (7/12/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_erps_cmd(msg_erps_cmd_t *msgErpsCmd)
{

#ifdef PTIN_ENABLE_ERPS

  int ret;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ERPS#%u: CMD %d, Port %d", msgErpsCmd->idx, msgErpsCmd->cmd, msgErpsCmd->port);

  /* Validate ERPS# range (idx [0..MAX_PROT_PROT_ERPS[) */
  if (msgErpsCmd->idx >= MAX_PROT_PROT_ERPS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ERPS#%u is out of range [0..%u]", msgErpsCmd->idx, MAX_PROT_PROT_ERPS-1);
    return L7_FAILURE;
  }

  if ( (msgErpsCmd->port != 0) && (msgErpsCmd->port != 1) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Port %d is out of range [0,1]", msgErpsCmd->port);
    return L7_FAILURE;
  }

  switch ( msgErpsCmd->cmd )
  {
  case PROT_ERPS_OPCMD_FS:
    ret = ptin_erps_cmd_force(msgErpsCmd->idx, msgErpsCmd->port);
    break;
  case PROT_ERPS_OPCMD_MS:
    ret = ptin_erps_cmd_manual(msgErpsCmd->idx, msgErpsCmd->port);
    break;
  case PROT_ERPS_OPCMD_OC:
    ret = ptin_erps_cmd_clear(msgErpsCmd->idx);
    break;
  case PROT_ERPS_OPCMD_LO:            //// The following command is for further study ///
    ret = ptin_erps_cmd_lockout(msgErpsCmd->idx);
    break;
  case PROT_ERPS_OPCMD_ReplaceRPL:    //// The following command is for further study ///
    ret = ptin_erps_cmd_replaceRpl(msgErpsCmd->idx, msgErpsCmd->port);
    break;
  case PROT_ERPS_OPCMD_ExeSignal:     //// The following command is for further study ///
    ret = ptin_erps_cmd_exercise(msgErpsCmd->idx, msgErpsCmd->port);
    break;
  default:
    return L7_FAILURE;
  }

  if (ret != msgErpsCmd->idx)
  {
    return L7_FAILURE;
  }

#endif  // PTIN_ENABLE_ERPS

  return L7_SUCCESS;

}



/***************************************************************************** 
 * ACL Configuration
 *****************************************************************************/


/**
 * MAC ACL Rule Configuration
 * 
 * @author joaom (10/29/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_mac_acl_rule_config(msg_mac_acl_t *msgMacAcl, ACL_OPERATION_t operation)
{
  L7_uint8  *aclTypeStr[] = {"MAC", "IP STANDARD", "IP EXTENDED", "IP NAMED", "IPv6 EXTENDED"};
  L7_uint8  *actionStr[] =  {"DENY", "PERMIT"};
  L7_uint8  *operationStr[] =  {"CREATE RULE", "REMOVE RULE"};

  L7_RC_t rc = L7_FAILURE;

  if (msgMacAcl->aclType != ACL_TYPE_MAC)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "aclType Invalid (%d)", msgMacAcl->aclType);
    return L7_FAILURE;
  }

  if (msgMacAcl->aclId >= L7_MAX_ACL_LISTS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid ACL ID (%d)", msgMacAcl->aclId);
    return L7_FAILURE;
  }

  if (msgMacAcl->aclRuleId > L7_MAX_NUM_RULES_PER_ACL)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "aclRuleId Invalid (%d)", msgMacAcl->aclRuleId);
    return L7_FAILURE;
  }

  if (msgMacAcl->action > ACL_ACTION_PERMIT)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "action Invalid (%d)", msgMacAcl->action);
    return L7_FAILURE;
  }
  

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Slot Id        %d",                              msgMacAcl->slotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Type       %s",                              aclTypeStr[msgMacAcl->aclType]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Id         %d",                              msgMacAcl->aclId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Name       %s",                              msgMacAcl->name);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Rule Id    %d",                              msgMacAcl->aclRuleId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Action         %s",                              actionStr[msgMacAcl->action]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Rule Mask  0x%x",                            msgMacAcl->aclRuleMask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Src Mac Addr   %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",   msgMacAcl->srcMacAddr[0], 
                                                                                msgMacAcl->srcMacAddr[1],
                                                                                msgMacAcl->srcMacAddr[2],
                                                                                msgMacAcl->srcMacAddr[3],
                                                                                msgMacAcl->srcMacAddr[4],
                                                                                msgMacAcl->srcMacAddr[5]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Src Mac Mask   %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",   msgMacAcl->srcMacMask[0], 
                                                                                msgMacAcl->srcMacMask[1],
                                                                                msgMacAcl->srcMacMask[2],
                                                                                msgMacAcl->srcMacMask[3],
                                                                                msgMacAcl->srcMacMask[4],
                                                                                msgMacAcl->srcMacMask[5]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Dst Mac Addr   %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",   msgMacAcl->dstMacAddr[0], 
                                                                                msgMacAcl->dstMacAddr[1],
                                                                                msgMacAcl->dstMacAddr[2],
                                                                                msgMacAcl->dstMacAddr[3],
                                                                                msgMacAcl->dstMacAddr[4],
                                                                                msgMacAcl->dstMacAddr[5]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Dst Mac Mask   %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",   msgMacAcl->dstMacMask[0], 
                                                                                msgMacAcl->dstMacMask[1],
                                                                                msgMacAcl->dstMacMask[2],
                                                                                msgMacAcl->dstMacMask[3],
                                                                                msgMacAcl->dstMacMask[4],
                                                                                msgMacAcl->dstMacMask[5]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "EtherType      0x%.4x",                          msgMacAcl->eType);

  if (msgMacAcl->startVlan == msgMacAcl->endVlan)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Vlan           %d",                            msgMacAcl->startVlan);
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Vlan Range     %d-%d",                         msgMacAcl->startVlan, msgMacAcl->endVlan);
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "COS            %d",                              msgMacAcl->cosVal);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Operation      %s",                              operationStr[operation]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");

  rc = ptin_aclMacRuleConfig(msgMacAcl, operation);

  return rc;
}



/**
 * IP ACL Rule Configuration
 * 
 * @author joaom (10/29/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_ip_acl_rule_config(msg_ip_acl_t *msgIpAcl, ACL_OPERATION_t operation)
{
  L7_uint8 *aclTypeStr[] = {"MAC", "IP STANDARD", "IP EXTENDED", "IP NAMED", "IPv6 EXTENDED"};
  L7_uint8 *actionStr[] =  {"DENY", "PERMIT"};
  L7_uint8 *operationStr[] =  {"CREATE RULE", "REMOVE RULE"};
  L7_uint8 ipAddr[] = "255.255.255.255";

  L7_RC_t rc = L7_FAILURE;

  if ( (msgIpAcl->aclType != ACL_TYPE_IP_STANDARD) && (msgIpAcl->aclType != ACL_TYPE_IP_EXTENDED) && (msgIpAcl->aclType != ACL_TYPE_IP_NAMED) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "aclType Invalid (%d)", msgIpAcl->aclType);
    return L7_FAILURE;
  }

  if (msgIpAcl->aclType == ACL_TYPE_IP_STANDARD)
  {
    if ( (msgIpAcl->aclId == 0) || (msgIpAcl->aclId > 99) ) /* [1..99] */
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid ACL ID (%d)", msgIpAcl->aclId);
      return L7_FAILURE;
    }
  }
  else if ( (msgIpAcl->aclType == ACL_TYPE_IP_EXTENDED) || (msgIpAcl->aclType == ACL_TYPE_IP_NAMED) )
  {
    if ( (msgIpAcl->aclId < 100) || (msgIpAcl->aclId > 199) ) /* [100..199] */
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid ACL ID (%d)", msgIpAcl->aclId);
      return L7_FAILURE;
    }
  }

  if (msgIpAcl->aclRuleId > L7_MAX_NUM_RULES_PER_ACL)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "aclRuleId Invalid (%d)", msgIpAcl->aclRuleId);
    return L7_FAILURE;
  }

  if (msgIpAcl->action > ACL_ACTION_PERMIT)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "action Invalid (%d)", msgIpAcl->action);
    return L7_FAILURE;
  }

  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Slot Id        %d",                              msgIpAcl->slotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Type       %s",                              aclTypeStr[msgIpAcl->aclType]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Id         %d",                              msgIpAcl->aclId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Name       %s",                              msgIpAcl->name);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Rule Id    %d",                              msgIpAcl->aclRuleId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Action         %s",                              actionStr[msgIpAcl->action]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Rule Mask  0x%x",                            msgIpAcl->aclRuleMask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Protocol       %d",                              msgIpAcl->protocol);

  usmDbInetNtoa(msgIpAcl->srcIpAddr,  ipAddr);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Src IP Addr    %s",                              ipAddr);

  usmDbInetNtoa(msgIpAcl->srcIpMask,  ipAddr);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Src IP Mask    %s",                              ipAddr);
  
  usmDbInetNtoa(msgIpAcl->dstIpAddr,  ipAddr);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Dst IP Addr    %s",                              ipAddr);
  
  usmDbInetNtoa(msgIpAcl->dstIpMask,  ipAddr);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Dst IP Mask    %s",                              ipAddr);  

  if (msgIpAcl->srcStartPort == msgIpAcl->srcEndPort)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Src L4 Port    %d",                            msgIpAcl->srcStartPort);
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Src L4 Port Range   %d-%d",                    msgIpAcl->srcStartPort, msgIpAcl->srcEndPort);
  }

  if (msgIpAcl->dstStartPort == msgIpAcl->dstEndPort)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Dst L4 Port    %d",                            msgIpAcl->dstStartPort);
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Dst L4 Port Range   %d-%d",                    msgIpAcl->dstStartPort, msgIpAcl->dstEndPort);
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "TOS            0x%.2x",                          msgIpAcl->tosVal);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "TOS Mask       0x%.2x",                          msgIpAcl->tosMask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "DSCP           %d",                              msgIpAcl->dscpVal);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Prec           %d",                              msgIpAcl->precVal);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Operation      %s",                              operationStr[operation]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");

  rc = ptin_aclIpRuleConfig(msgIpAcl, operation);

  return rc;
}



/**
 * IPv6 ACL Rule Configuration
 * 
 * @author joaom (10/29/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_ipv6_acl_rule_config(msg_ipv6_acl_t *msgIpv6Acl, ACL_OPERATION_t operation)
{
  L7_uint8 *aclTypeStr[] = {"MAC", "IP STANDARD", "IP EXTENDED", "IP NAMED", "IPv6 EXTENDED"};
  L7_uint8 *actionStr[] =  {"DENY", "PERMIT"};
  L7_uint8 *operationStr[] =  {"CREATE RULE", "REMOVE RULE"};
  L7_uint8 ipAddr[] = "fe80:0000:0206:91ff:fe06:f69e";

  L7_RC_t rc = L7_FAILURE;

  if (msgIpv6Acl->aclType != ACL_TYPE_IPv6_EXTENDED)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "aclType Invalid (%d)", msgIpv6Acl->aclType);
    return L7_FAILURE;
  }

  if (msgIpv6Acl->aclId > L7_MAX_ACL_LISTS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid ACL ID (%d)", msgIpv6Acl->aclId);
    return L7_FAILURE;
  }

  if (msgIpv6Acl->aclRuleId > L7_MAX_NUM_RULES_PER_ACL)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "aclRuleId Invalid (%d)", msgIpv6Acl->aclRuleId);
    return L7_FAILURE;
  }

  if (msgIpv6Acl->action > ACL_ACTION_PERMIT)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "action Invalid (%d)", msgIpv6Acl->action);
    return L7_FAILURE;
  }

  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Slot Id        %d",                              msgIpv6Acl->slotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Type       %s",                              aclTypeStr[msgIpv6Acl->aclType]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Id         %d",                              msgIpv6Acl->aclId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Name       %s",                              msgIpv6Acl->name);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Rule Id    %d",                              msgIpv6Acl->aclRuleId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Action         %s",                              actionStr[msgIpv6Acl->action]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Rule Mask  0x%x",                            msgIpv6Acl->aclRuleMask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Protocol       %d",                              msgIpv6Acl->protocol);

  if (osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)msgIpv6Acl->src6Addr, ipAddr, sizeof(ipAddr)) != L7_NULLPTR)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Src IP Addr    %s/%d",                         ipAddr, msgIpv6Acl->src6PrefixLen);
  }

  if (osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)msgIpv6Acl->dst6Addr, ipAddr, sizeof(ipAddr)) != L7_NULLPTR)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Dst IP Addr    %s/%d",                         ipAddr, msgIpv6Acl->dst6PrefixLen);
  }

  if (msgIpv6Acl->srcStartPort == msgIpv6Acl->srcEndPort)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Src L4 Port    %d",                            msgIpv6Acl->srcStartPort);
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Src L4 Port Range   %d-%d",                    msgIpv6Acl->srcStartPort, msgIpv6Acl->srcEndPort);
  }

  if (msgIpv6Acl->dstStartPort == msgIpv6Acl->dstEndPort)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Dst L4 Port    %d",                            msgIpv6Acl->dstStartPort);
  }
  else
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Dst L4 Port Range   %d-%d",                    msgIpv6Acl->dstStartPort, msgIpv6Acl->dstEndPort);
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "DSCP           %d",                              msgIpv6Acl->dscpVal);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Flow Label     %d",                              msgIpv6Acl->flowLabelVal);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Operation      %s",                              operationStr[operation]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");

  rc = ptin_aclIpv6RuleConfig(msgIpv6Acl, operation);

  return rc;
}


/**
 * ACL Rule Configuration
 * 
 * @author joaom (11/01/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_acl_rule_config(void *msgAcl, L7_uint msgId)
{
  L7_uint8        *msg;
  ACL_OPERATION_t operation = ACL_OPERATION_REMOVE;
  L7_RC_t         rc = L7_FAILURE;

  msg = (L7_uint8 *) msgAcl;  

  if (msgId == CCMSG_ACL_RULE_ADD)
  {
    operation = ACL_OPERATION_CREATE;
  }
  else if (msgId == CCMSG_ACL_RULE_DEL)
  {
    operation = ACL_OPERATION_REMOVE;
  }

  if (msg[1] == ACL_TYPE_MAC)
  {
    rc = ptin_msg_mac_acl_rule_config((msg_mac_acl_t*) msgAcl, operation);
  }
  else if ( (msg[1] == ACL_TYPE_IP_STANDARD) || (msg[1] == ACL_TYPE_IP_EXTENDED) || (msg[1] == ACL_TYPE_IP_NAMED) )
  {
    rc = ptin_msg_ip_acl_rule_config((msg_ip_acl_t*) msgAcl, operation);
  }
  else if (msg[1] == ACL_TYPE_IPv6_EXTENDED)
  {
    rc = ptin_msg_ipv6_acl_rule_config((msg_ipv6_acl_t*) msgAcl, operation);
  }

  return rc;
}



/**
 * Apply ACL Configuration
 * 
 * @author joaom (10/29/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_acl_apply(msg_apply_acl_t *msgAcl, ACL_OPERATION_t operation, L7_uint8 aclType)
{
  L7_uint8  *aclTypeStr[] = {"MAC", "IP STANDARD", "IP EXTENDED", "IP NAMED", "IPv6 EXTENDED"};
  L7_uint8  *directionStr[] =  {"IN", "OUT"};
  L7_uint8  *operationStr[] =  {"APPLY ACL", "UNAPPLY ACL"};

  L7_uint16 intRootVlan;
  L7_RC_t   rc = L7_FAILURE;

  if (msgAcl->aclType > ACL_TYPE_IPv6_EXTENDED)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "aclType Invalid (%d)", msgAcl->aclType);
    return L7_FAILURE;
  }

  if (msgAcl->direction > ACL_DIRECTION_OUT)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "direction Invalid (%d)", msgAcl->direction);
    return L7_FAILURE;
  }


  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Slot Id        %u",                              msgAcl->slotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Type       %s",                              aclTypeStr[msgAcl->aclType]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Id         %u",                              msgAcl->aclId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ACL Name       %s",                              msgAcl->name);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "interface      %u",                              msgAcl->interface);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "vlanId         %u",                              msgAcl->vlanId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "evcId          %u",                              msgAcl->evcId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "direction      %s",                              directionStr[msgAcl->direction]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Operation      %s",                              operationStr[operation]);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "-------------------------------------------");

  if ( (msgAcl->interface == L7_ACL_INVALID_IFACE_ID) && (msgAcl->evcId == L7_ACL_INVALID_EVC_ID) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Neither interface neither evcId is valid");
    return L7_FAILURE;
  }

  if (msgAcl->evcId != L7_ACL_INVALID_EVC_ID)
  {
    /* Gets the root vlan (internal) for a particular evc */
    rc = ptin_evc_intRootVlan_get(msgAcl->evcId, &intRootVlan);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error while retrieving VLAN ID(rc=%d)", rc);
      return rc;
    }

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Retrieved VLAN ID %d", (L7_uint32) intRootVlan);
    msgAcl->vlanId = (L7_uint32) intRootVlan;
  }
  else
  {
    L7_uint32 intIfNum;

    msgAcl->vlanId = L7_ACL_INVALID_VLAN_ID;

    ptin_intf_port2intIfNum(msgAcl->interface, &intIfNum);

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Converting Interface %d to intIfNum %d", msgAcl->interface, intIfNum);
    msgAcl->interface = intIfNum;
  }

  
  if (aclType == ACL_TYPE_MAC)
  {
    rc = ptin_aclMacApply(msgAcl, operation);
  }
  else if ( (aclType == ACL_TYPE_IP_STANDARD) || (aclType == ACL_TYPE_IP_EXTENDED) || (aclType == ACL_TYPE_IP_NAMED) )
  {
    rc = ptin_aclIpApply(msgAcl, operation);
  }
  else if (aclType == ACL_TYPE_IPv6_EXTENDED)
  {
    rc = ptin_aclIpv6Apply(msgAcl, operation);
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "rc=%d", rc);
  return rc;
}


/**
 * ACL Enable/Disable
 * 
 * @author joaom (11/01/2013)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_acl_enable(void *msgAcl, L7_uint msgId)
{
  L7_uint8        *msg;
  L7_uint8        aclType;
  ACL_OPERATION_t operation = ACL_OPERATION_REMOVE;
  L7_RC_t         rc = L7_FAILURE;

  msg = (L7_uint8 *) msgAcl;  

  if (msgId == CCMSG_ACL_APPLY)
  {
    operation = ACL_OPERATION_CREATE;
  }
  else if (msgId == CCMSG_ACL_UNAPPLY)
  {
    operation = ACL_OPERATION_REMOVE;
  }

  aclType = msg[1];

  if (aclType == ACL_TYPE_MAC)
  {
    rc = ptin_msg_acl_apply(msgAcl, operation, aclType);
  }
  else if ( (aclType == ACL_TYPE_IP_STANDARD) || (aclType == ACL_TYPE_IP_EXTENDED) || (aclType == ACL_TYPE_IP_NAMED) )
  {
    rc = ptin_msg_acl_apply(msgAcl, operation, aclType);
  }
  else if (aclType == ACL_TYPE_IPv6_EXTENDED)
  {
    rc = ptin_msg_acl_apply(msgAcl, operation, aclType);
  }

  return rc;
}

/* ************************* MSG Debug Routines **************************** */

/* IP */
L7_RC_t ptin_msg_DEBUG_ip_acl_rule_config(L7_uint8 operation, L7_uchar8 protocol)
{
  msg_ip_acl_t msgIpAcl;

  memset(&msgIpAcl, 0, sizeof(msgIpAcl));

  msgIpAcl.slotId =      0;
  msgIpAcl.aclType =     ACL_TYPE_IP_NAMED;
  msgIpAcl.aclId =       1;
  sprintf(msgIpAcl.name, "ptin_ip_acl_001");
  msgIpAcl.aclRuleId =   1;
  msgIpAcl.action =      ACL_ACTION_DENY;

  msgIpAcl.aclRuleMask = ACL_IP_RULE_MASK_protocol | ACL_IP_RULE_MASK_srcIpAddr;

  msgIpAcl.protocol = protocol;

  usmDbInetAton("192.1.1.2", &msgIpAcl.srcIpAddr);
  usmDbInetAton("255.255.255.255", &msgIpAcl.srcIpMask);

  ptin_msg_acl_rule_config(&msgIpAcl, (operation==ACL_OPERATION_CREATE)? CCMSG_ACL_RULE_ADD : CCMSG_ACL_RULE_DEL);

  return L7_SUCCESS;
}


L7_RC_t ptin_msg_DEBUG_ip_acl_apply(L7_uint32 interface, L7_uint32 evcId, L7_uint8 operation)
{
  msg_apply_acl_t msgAcl;

  memset(&msgAcl, 0, sizeof(msgAcl));

  msgAcl.slotId =       0;
  msgAcl.aclType =      ACL_TYPE_IP_NAMED;
  msgAcl.aclId =        1;
  sprintf(msgAcl.name,  "ptin_ip_acl_001");

  msgAcl.interface =    interface;
  msgAcl.vlanId =       L7_ACL_INVALID_VLAN_ID;
  msgAcl.evcId =        evcId;
  msgAcl.direction =    ACL_DIRECTION_IN;

  ptin_msg_acl_enable(&msgAcl, (operation==ACL_OPERATION_CREATE)? CCMSG_ACL_APPLY : CCMSG_ACL_UNAPPLY);

  return L7_SUCCESS;
}



/* IPv6 */
L7_RC_t ptin_msg_DEBUG_ipv6_acl_rule_config(L7_uint8 operation, L7_uchar8 protocol)
{
  msg_ipv6_acl_t msgIpv6Acl;

  memset(&msgIpv6Acl, 0, sizeof(msgIpv6Acl));

  msgIpv6Acl.slotId =      0;
  msgIpv6Acl.aclType =     ACL_TYPE_IPv6_EXTENDED;
  msgIpv6Acl.aclId =       1;
  sprintf(msgIpv6Acl.name, "ptin_ipv6_acl_001");
  msgIpv6Acl.aclRuleId =   1;
  msgIpv6Acl.action =      ACL_ACTION_DENY;

  msgIpv6Acl.aclRuleMask = ACL_IPv6_RULE_MASK_protocol | ACL_IPv6_RULE_MASK_src6Addr;

  msgIpv6Acl.protocol = protocol;

  /* fe80::250:56ff:fe99:123/64 */
  msgIpv6Acl.src6Addr[0] =  0xfe;
  msgIpv6Acl.src6Addr[1] =  0x80;

  msgIpv6Acl.src6Addr[2] =  0x00;
  msgIpv6Acl.src6Addr[3] =  0x00;
  msgIpv6Acl.src6Addr[4] =  0x00;
  msgIpv6Acl.src6Addr[5] =  0x00;
  msgIpv6Acl.src6Addr[6] =  0x00;
  msgIpv6Acl.src6Addr[7] =  0x00;

  msgIpv6Acl.src6Addr[8] =  0x02;
  msgIpv6Acl.src6Addr[9] =  0x50;
  msgIpv6Acl.src6Addr[10] = 0x56;
  msgIpv6Acl.src6Addr[11] = 0xff;
  msgIpv6Acl.src6Addr[12] = 0xfe;
  msgIpv6Acl.src6Addr[13] = 0x99;
  msgIpv6Acl.src6Addr[14] = 0x01;
  msgIpv6Acl.src6Addr[15] = 0x23;
  
  msgIpv6Acl.src6PrefixLen = 64;

  ptin_msg_acl_rule_config(&msgIpv6Acl, (operation==ACL_OPERATION_CREATE)? CCMSG_ACL_RULE_ADD : CCMSG_ACL_RULE_DEL);

  return L7_SUCCESS;
}


L7_RC_t ptin_msg_DEBUG_ipv6_acl_apply(L7_uint32 interface, L7_uint32 evcId, L7_uint8 operation)
{
  msg_apply_acl_t msgAcl;

  memset(&msgAcl, 0, sizeof(msgAcl));

  msgAcl.slotId =       0;
  msgAcl.aclType =      ACL_TYPE_IPv6_EXTENDED;
  msgAcl.aclId =        1;
  sprintf(msgAcl.name,  "ptin_ipv6_acl_001");

  msgAcl.interface =    interface;
  msgAcl.vlanId =       L7_ACL_INVALID_VLAN_ID;
  msgAcl.evcId =        evcId;
  msgAcl.direction =    ACL_DIRECTION_IN;

  ptin_msg_acl_enable(&msgAcl, (operation==ACL_OPERATION_CREATE)? CCMSG_ACL_APPLY : CCMSG_ACL_UNAPPLY);

  return L7_SUCCESS;
}



/* MAC */
L7_RC_t ptin_msg_DEBUG_mac_acl_rule_config(L7_uint8 operation)
{
  msg_mac_acl_t msgMacAcl;


  /* Rule #1 */

  memset(&msgMacAcl, 0, sizeof(msgMacAcl));

  msgMacAcl.slotId =      0;
  msgMacAcl.aclType =     ACL_TYPE_MAC;
  msgMacAcl.aclId =       1;
  sprintf(msgMacAcl.name, "ptin_mac_acl_001");
  msgMacAcl.aclRuleId =   1;
  msgMacAcl.action =      ACL_ACTION_DENY;

  msgMacAcl.aclRuleMask = ACL_MAC_RULE_MASK_srcMacAddr | ACL_MAC_RULE_MASK_srcMacMask;

  msgMacAcl.srcMacAddr[0] = 0x00;
  msgMacAcl.srcMacAddr[1] = 0x00;
  msgMacAcl.srcMacAddr[2] = 0xC0;
  msgMacAcl.srcMacAddr[3] = 0x01;
  msgMacAcl.srcMacAddr[4] = 0x01;
  msgMacAcl.srcMacAddr[5] = 0x02;

  msgMacAcl.srcMacMask[0] = 0xFF;
  msgMacAcl.srcMacMask[1] = 0xFF;
  msgMacAcl.srcMacMask[2] = 0xFF;
  msgMacAcl.srcMacMask[3] = 0xFF;
  msgMacAcl.srcMacMask[4] = 0xFF;
  msgMacAcl.srcMacMask[5] = 0xFF;

  msgMacAcl.eType =     0;
  msgMacAcl.startVlan = 0;
  msgMacAcl.endVlan =   0;
  msgMacAcl.cosVal =    0;

  ptin_msg_acl_rule_config(&msgMacAcl, (operation==ACL_OPERATION_CREATE)? CCMSG_ACL_RULE_ADD : CCMSG_ACL_RULE_DEL);


  /* Rule #2 */

  memset(&msgMacAcl, 0, sizeof(msgMacAcl));

  msgMacAcl.slotId =      0;
  msgMacAcl.aclType =     ACL_TYPE_MAC;
  msgMacAcl.aclId =       1;
  sprintf(msgMacAcl.name, "ptin_mac_acl_001");
  msgMacAcl.aclRuleId =   2;
  msgMacAcl.action =      ACL_ACTION_DENY;

  msgMacAcl.aclRuleMask = ACL_MAC_RULE_MASK_srcMacAddr | ACL_MAC_RULE_MASK_srcMacMask;

  msgMacAcl.srcMacAddr[0] = 0x00;
  msgMacAcl.srcMacAddr[1] = 0x00;
  msgMacAcl.srcMacAddr[2] = 0xC0;
  msgMacAcl.srcMacAddr[3] = 0x01;
  msgMacAcl.srcMacAddr[4] = 0x01;
  msgMacAcl.srcMacAddr[5] = 0x03;

  msgMacAcl.srcMacMask[0] = 0xFF;
  msgMacAcl.srcMacMask[1] = 0xFF;
  msgMacAcl.srcMacMask[2] = 0xFF;
  msgMacAcl.srcMacMask[3] = 0xFF;
  msgMacAcl.srcMacMask[4] = 0xFF;
  msgMacAcl.srcMacMask[5] = 0xFF;


  msgMacAcl.eType =     0;
  msgMacAcl.startVlan = 0;
  msgMacAcl.endVlan =   0;
  msgMacAcl.cosVal =    0;

  ptin_msg_acl_rule_config(&msgMacAcl, (operation==ACL_OPERATION_CREATE)? CCMSG_ACL_RULE_ADD : CCMSG_ACL_RULE_DEL);

  return L7_SUCCESS;
}


L7_RC_t ptin_msg_DEBUG_mac_acl_apply(L7_uint32 interface, L7_uint32 evcId, L7_uint8 operation)
{
  msg_apply_acl_t msgAcl;

  memset(&msgAcl, 0, sizeof(msgAcl));

  msgAcl.slotId =       0;
  msgAcl.aclType =      ACL_TYPE_MAC;
  msgAcl.aclId =        1;
  sprintf(msgAcl.name,  "ptin_mac_acl_001");

  msgAcl.interface =    interface;
  msgAcl.vlanId =       L7_ACL_INVALID_VLAN_ID;
  msgAcl.evcId =        evcId;
  msgAcl.direction =    ACL_DIRECTION_IN;

  ptin_msg_acl_enable(&msgAcl, (operation==ACL_OPERATION_CREATE)? CCMSG_ACL_APPLY : CCMSG_ACL_UNAPPLY);

  return L7_SUCCESS;
}

/* ********************************************************************* */


#ifdef __802_1x__

#include <sirerrors.h>
#include <usmdb_dot1x_api.h>
#include <usmdb_dot1x_auth_serv_api.h>
#include <dot1x_auth_serv_exports.h>
#include <dot1x_auth_serv_api.h>


int msg_wr_802_1x_Genrc(ipc_msg *inbuff, ipc_msg *outbuff, L7_ulong32 i)
{
  msg_802_1x_Genrc * pi,*po;
  L7_RC_t r;

  pi = (msg_802_1x_Genrc * )inbuff->info;   po = (msg_802_1x_Genrc * )outbuff->info;

  switch (inbuff->msgId)
  {
  case CCMSG_WR_802_1X_ADMINMODE:
    r = usmDbDot1xAdminModeSet(1, pi[i].v);
    break;
  case CCMSG_WR_802_1X_TRACE:
    r = usmDbDot1xPacketDebugTraceFlagSet(pi[i].v >> 1, pi[i].v & 1);
    break;
  case CCMSG_WR_802_1X_VLANASSGNMODE:
    r = usmDbDot1xVlanAssignmentModeSet(1, pi[i].v);
    break;
  case CCMSG_WR_802_1X_MONMODE:
    r = usmDbDot1xMonitorModeSet(1, pi[i].v);
    break;
  case CCMSG_WR_802_1X_DYNVLANMODE:
    r = usmDbDot1xDynamicVlanCreationModeSet(1, pi[i].v);
    break;
  default:
    po[i].v = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
    return 1;
  }

  if (L7_SUCCESS != r)
  {
    po[i].v = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
    return 1;
  }
  else
  {
    po[i].v = ERROR_CODE_OK;
  }

  return 0;
} //msg_wr_802_1x_Genrc






int msg_wr_802_1x_Genrc2(ipc_msg *inbuff, ipc_msg *outbuff, L7_ulong32 i)
{
  msg_802_1x_Genrc2 *pi;
  msg_generic_prefix_t *po;
  ptin_intf_t ptinp;
  L7_RC_t r;
  L7_uint32 intIfNum;

  pi = (msg_802_1x_Genrc2 * )inbuff->info;   po = (msg_generic_prefix_t * )outbuff->info;

  ptinp.intf_type =   pi[i].index >> 8;
  ptinp.intf_id =     pi[i].index;

  if (L7_SUCCESS != ptin_intf_ptintf2intIfNum(&ptinp, &intIfNum))
  {
    po[i].err_code = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
    return 1;
  }

  switch (inbuff->msgId)
  {
  case CCMSG_WR_802_1X_ADMINCONTROLLEDDIRECTIONS:
    r = usmDbDot1xPortAdminControlledDirectionsSet(1, intIfNum, pi[i].v);
    break;
  case CCMSG_WR_802_1X_PORTCONTROLMODE:
    r = usmDbDot1xPortControlModeSet(1, intIfNum, pi[i].v);
    break;
  case CCMSG_WR_802_1X_QUIETPERIOD:
    r = usmDbDot1xPortQuietPeriodSet(1, intIfNum, pi[i].v);
    break;
  case CCMSG_WR_802_1X_TXPERIOD:
    r = usmDbDot1xPortTxPeriodSet(1, intIfNum, pi[i].v);
    break;
  case CCMSG_WR_802_1X_SUPPTIMEOUT:
    r = usmDbDot1xPortSuppTimeoutSet(1, intIfNum, pi[i].v);
    break;
  case CCMSG_WR_802_1X_SERVERTIMEOUT:
    r = usmDbDot1xPortServerTimeoutSet(1, intIfNum, pi[i].v);
    break;
  case CCMSG_WR_802_1X_MAXREQ:
    r = usmDbDot1xPortMaxReqSet(1, intIfNum, pi[i].v);
    break;
  case CCMSG_WR_802_1X_REAUTHPERIOD:
    if (1 + pi[i].v == 0)
    {
      r = usmDbDot1xPortReAuthEnabledSet(1, intIfNum, 0);
      break;
    }  //Forbidden period disables
    r = usmDbDot1xPortReAuthPeriodSet(1, intIfNum, pi[i].v);
    if (L7_SUCCESS != r)  break;
    r = usmDbDot1xPortReAuthEnabledSet(1, intIfNum, 1);
    break;
  case CCMSG_WR_802_1X_KEYTXENABLED:
    r = usmDbDot1xPortKeyTransmissionEnabledSet(1, intIfNum, pi[i].v);
    break;
  case CCMSG_WR_802_1X_GUESTVLANID:
    r = usmDbDot1xAdvancedGuestPortsCfgSet(1, intIfNum, pi[i].v);
    break;
  case CCMSG_WR_802_1X_GUSTVLANPERIOD:
    r = usmDbDot1xAdvancedPortGuestVlanPeriodSet(1, intIfNum, pi[i].v);
    break;
  case CCMSG_WR_802_1X_MAXUSERS:
    r = usmDbDot1xPortMaxUsersSet(1, intIfNum, pi[i].v);
    break;
  case CCMSG_WR_802_1X_UNAUTHENTICATEDVLAN:
    r = usmDbDot1xPortUnauthenticatedVlanSet(1, intIfNum, pi[i].v);
    break;
  default:
    po[i].err_code = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
    return 1;
  }

  if (L7_SUCCESS != r)
  {
    po[i].err_code = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
    return 1;
  }
  else
  {
    po[i].err_code = ERROR_CODE_OK;
  }

  return 0;
} //msg_wr_802_1x_Genrc2
















L7_RC_t usmDbDot1xAuthServUserDBUserIndexGet(L7_char8 *name, L7_uint32 *index)
{
  return dot1xAuthServUserDBUserIndexGet(name, index);
}

L7_RC_t usmDbDot1xAuthServUserDBUserNameSet(L7_uint32 index, L7_char8 *name)
{
  return dot1xAuthServUserDBUserNameSet(index, name);
}

L7_RC_t usmDbDot1xAuthServUserDBUserPasswordSet(L7_uint32 index, L7_char8 *password, L7_BOOL encrypted)
{
  return dot1xAuthServUserDBUserPasswordSet(index, password, encrypted);
}

L7_RC_t usmDbDot1xAuthServUserDBAvailableIndexGet(L7_uint32 *index)
{
  return dot1xAuthServUserDBAvailableIndexGet(index);
}

int msg_wr_802_1x_AuthServ(ipc_msg *inbuff, ipc_msg *outbuff, L7_ulong32 i)
{
  msg_802_1x_AuthServ *pi;
  msg_generic_prefix_t *po;
  L7_RC_t r;
  L7_ulong32 k;           //MNGMT preferred index
  L7_uint32 index = -1;     //index to eventual already in table entry
  L7_uchar8 e;

  pi = (msg_802_1x_AuthServ * )inbuff->info;   po = (msg_generic_prefix_t * )outbuff->info;

  k = pi[i].index; //64th bit's lost
  e = pi[i].index >> 63;

  if (L7_SUCCESS == usmDbDot1xAuthServUserDBUserIndexGet(pi[i].name, &index))
  {
    if (k < L7_MAX_IAS_USERS && k != index)  //Name already in table with different index
    {
      po[i].err_code = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_DUPLICATENAME);
      return 1;
    }
  }
  else index = -1;

  if (k < L7_MAX_IAS_USERS)
  {
    if (k != index && (L7_SUCCESS != (r = usmDbDot1xAuthServUserDBUserNameSet(k, pi[i].name))))   //index already used (or table full)
    {
      po[i].err_code = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_USED);
      return 1;
    }
  }
  else
  {
    if (index >= L7_MAX_IAS_USERS)
    {
      if (L7_SUCCESS != usmDbDot1xAuthServUserDBAvailableIndexGet(&index))
      {
        po[i].err_code = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_FULLTABLE);
        return 1;
      }
    }
    k = index;
  }

  if (L7_SUCCESS != (r = usmDbDot1xAuthServUserDBUserPasswordSet(k, pi[i].passwd, e)))
  {
    po[i].err_code = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
    return 1;
  }
  else po[i].err_code = ERROR_CODE_OK;

  return 0;
} //msg_wr_802_1x_AuthServ
#endif //__802_1x__

/**
 * Create new routing interface.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_intf_create(msg_RoutingIpv4Intf* data)
{
  ptin_intf_t routingIntf;
  L7_uint16   internalVlan;

  /* Debug */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Creating new routing interface:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  mask          = %08X",  data->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  routingIntf   = %u/%u", data->routingIntf.intf_type, data->routingIntf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  evcId         = %u",    data->evcId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  ipAddress     = %08X",  data->ipAddress);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  subnetMask    = %08X",  data->subnetMask);

  routingIntf.intf_type  = data->routingIntf.intf_type;
  routingIntf.intf_id    = data->routingIntf.intf_id;

  if(L7_SUCCESS != ptin_evc_intRootVlan_get(data->evcId, &internalVlan))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to convert evc_id to internal root vlan");
    return L7_FAILURE;
  }

  if(L7_SUCCESS != ptin_routing_intf_create(&routingIntf, internalVlan))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to create a new routing interface");
    return L7_FAILURE;
  }

  if(L7_SUCCESS != ptin_routing_intf_ipaddress_set(&routingIntf, L7_AF_INET, data->ipAddress, data->subnetMask))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to set interface IP address");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Modify an existing routing interface.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_intf_modify(msg_RoutingIpv4Intf* data)
{
  ptin_intf_t routingIntf;

  /* Debug */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Configuring routing interface:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  mask          = %08X",  data->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  routingIntf   = %u/%u", data->routingIntf.intf_type, data->routingIntf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  evcId         = %u",    data->evcId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  ipAddress     = %08X",  data->ipAddress);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  subnetMask    = %08X",  data->subnetMask);

  routingIntf.intf_type  = data->routingIntf.intf_type;
  routingIntf.intf_id    = data->routingIntf.intf_id;

  if(L7_SUCCESS != ptin_routing_intf_ipaddress_set(&routingIntf, L7_AF_INET, data->ipAddress, data->subnetMask))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to set interface IP address");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Remove an existing routing interface.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_intf_remove(msg_RoutingIpv4Intf* data)
{
  ptin_intf_t routingIntf;

  /* Debug */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Removing routing interface:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  mask          = %08X",  data->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  routingIntf   = %u/%u", data->routingIntf.intf_type, data->routingIntf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  evcId         = %u",    data->evcId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  ipAddress     = %08X",  data->ipAddress);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  subnetMask    = %08X",  data->subnetMask);

  routingIntf.intf_type = data->routingIntf.intf_type;
  routingIntf.intf_id   = data->routingIntf.intf_id;

  if(L7_SUCCESS != ptin_routing_intf_remove(&routingIntf))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to remove the existing routing interface");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get ARP table.
 * 
 * @param inBuffer
 * @param outBuffer
 * @param readEntries
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_arptable_get(msg_RoutingArpTableRequest* inBuffer, msg_RoutingArpTableResponse* outBuffer, L7_uint32* readEntries)
{
  ptin_intf_t intf;
  L7_uint32   intfNum;
  L7_uint32   maxEntries;

  if( (inBuffer == L7_NULLPTR) || (outBuffer == L7_NULLPTR) || (readEntries == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Abnormal context [inBuffer=%p outBuffer=%p readEntries=%p]", inBuffer, outBuffer, readEntries);
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Getting ARP table:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  mask       = %08X",  inBuffer->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  intf       = %u/%u", inBuffer->intf.intf_type, inBuffer->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  lastIndex  = %u",    inBuffer->lastIndex);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  maxEntries = %u",    inBuffer->maxEntries);

  if(inBuffer->mask & CCMSG_ROUTING_ARPTABLE_GET_MASK_INTF)
  {
    intf.intf_type = inBuffer->intf.intf_type;
    intf.intf_id   = inBuffer->intf.intf_id;
    if(L7_SUCCESS != ptin_intf_ptintf2intIfNum(&intf, &intfNum))
    {
      LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert intf %u/%u to intfNum", intf.intf_type, intf.intf_id);
      return L7_FAILURE;
    }
  }
  else
  {
    intfNum = (L7_uint32)-1;
  }

  if(inBuffer->maxEntries & CCMSG_ROUTING_ARPTABLE_GET_MASK_MAXENTRIES)
  {
    maxEntries = min(IPCLIB_MAX_MSGSIZE/sizeof(msg_RoutingArpTableResponse), inBuffer->maxEntries);
  }
  else
  {
    maxEntries = IPCLIB_MAX_MSGSIZE/sizeof(msg_RoutingArpTableResponse);
  }

  /*
   I know that passing a ptin_msghandler struct to a file other than ptin_msg breaks PTIN Fastpath's architecture.
   However, doing so here allows me to hide the complexity of interacting with usmDb completly inside of ptin_routing.                                                                                                           .
  */
  if(L7_SUCCESS != ptin_routing_arptable_getnext(intfNum, inBuffer->lastIndex, maxEntries, readEntries, outBuffer))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to get the ARP table");
    return L7_FAILURE;
  }
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Successfully read %u entries", *readEntries);

  return L7_SUCCESS;
}

/**
 * Delete ARP entry.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_arpentry_purge(msg_RoutingArpEntryPurge* data)
{
  ptin_intf_t intf;
  L7_uint32   intfNum;

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Removing ARP entry:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  mask       = %08X",  data->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Intf       = %u/%u", data->intf.intf_type, data->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  IP Address = %08X",  data->ipAddr);

  if(data->mask & CCMSG_ROUTING_ARPTABLE_GET_MASK_INTF)
  {
    intf.intf_type = data->intf.intf_type;
    intf.intf_id   = data->intf.intf_id;
    if(L7_SUCCESS != ptin_intf_ptintf2intIfNum(&intf, &intfNum))
    {
      LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert intf %u/%u to intfNum", intf.intf_type, intf.intf_id);
      return L7_FAILURE;
    }
  }
  else
  {
    intfNum = (L7_uint32)-1;
  }

  if(L7_SUCCESS != ptin_routing_arpentry_purge(intfNum, data->ipAddr))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to remove the existing ARP entry");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get route table.
 * 
 * @param inBuffer
 * @param outBuffer
 * @param maxEntries
 * @param readEntries
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_routetable_get(msg_RoutingRouteTableRequest* inBuffer, msg_RoutingRouteTableResponse* outBuffer, L7_uint32 maxEntries, L7_uint32* readEntries)
{
  ptin_intf_t intf;
  L7_uint32   intfNum;

  if( (inBuffer == L7_NULLPTR) || (outBuffer == L7_NULLPTR) || (readEntries == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Abnormal context [inBuffer=%p outBuffer=%p readEntries=%p]", inBuffer, outBuffer, readEntries);
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Getting route table:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  intf       = %u/%u", inBuffer->intf.intf_type, inBuffer->intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  lastIndex  = %u",    inBuffer->lastIndex);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  maxEntries = %u",    maxEntries);

  intf.intf_type = inBuffer->intf.intf_type;
  intf.intf_id   = inBuffer->intf.intf_id;

  if(L7_SUCCESS != ptin_intf_ptintf2intIfNum(&intf, &intfNum))
  {
    LOG_ERR(LOG_CTX_PTIN_ROUTING, "Unable to to convert intf %u/%u to intfNum", intf.intf_type, intf.intf_id);
    return L7_FAILURE;
  }

  /*
   I know that passing a ptin_msghandler struct to a file other than ptin_msg breaks PTIN Fastpath's architecture.
   However, doing so here allows me to hide the complexity of interacting with usmDb completly inside of ptin_routing.                                                                                                           .
  */
  if(L7_SUCCESS != ptin_routing_routetable_get(intfNum, inBuffer->lastIndex, maxEntries, readEntries, outBuffer))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to get the route table");
    return L7_FAILURE;
  }
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Successfully read %u entries", *readEntries);

  return L7_SUCCESS;
}

/**
 * Configure a static route.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_staticroute_add(msg_RoutingStaticRoute* data)
{
  if( (data == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Configuring static route:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  dstIpAddr   = %08X", data->dstIpAddr);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  subnetMask  = %08X", data->subnetMask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  nextHopRtr  = %08X", data->nextHopRtr);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  pref        = %u",   data->pref);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  isNullRoute = %u",   data->isNullRoute);

  if(L7_SUCCESS != ptin_routing_staticroute_add(data->dstIpAddr, data->subnetMask, data->nextHopRtr, data->pref, (L7_BOOL)data->isNullRoute))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to configure static route");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Delete an existing static route.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_staticroute_delete(msg_RoutingStaticRoute* data)
{
  if( (data == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Removing an existing static route:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  dstIpAddr   = %08X", data->dstIpAddr);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  subnetMask  = %08X", data->subnetMask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  nextHopRtr  = %08X", data->nextHopRtr);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  pref        = %u",   data->pref);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  isNullRoute = %u",   data->isNullRoute);

  if(L7_SUCCESS != ptin_routing_staticroute_delete(data->dstIpAddr, data->subnetMask, data->nextHopRtr, (L7_BOOL)data->isNullRoute))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to remove static route");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Start a ping request.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_pingsession_create(msg_RoutingPingSessionCreate* data)
{
  if( (data == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Creating new ping session:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  sessionIdx    = %u",   data->sessionIdx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  dstIpAddr     = %08X", data->dstIpAddr);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  probeCount    = %u",   data->probeCount);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  probeSize     = %u",   data->probeSize);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  probeInterval = %u",   data->probeInterval);

  if(L7_SUCCESS != ptin_routing_pingsession_create(data->sessionIdx, data->dstIpAddr, data->probeCount, data->probeSize, data->probeInterval))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to create new ping session");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get ping session status.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_pingsession_query(msg_RoutingPingSessionQuery* data)
{
  if( (data == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Querying ping session:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  sessionIdx = %u", data->sessionIdx);

  /*
   I know that passing a ptin_msghandler struct to a file other than ptin_msg breaks PTIN Fastpath's architecture.
   However, doing so here allows me to hide the complexity of interacting with usmDb completly inside of ptin_routing.                                                                                                           .
   */
  if(L7_SUCCESS != ptin_routing_pingsession_query(data))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to query ping session");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Free existing ping session.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_pingsession_free(msg_RoutingPingSessionFree* data)
{
  if( (data == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Freeing ping session:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  mask       = %02X", data->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  sessionIdx = %u",   data->sessionIdx);

  if(data->mask & CCMSG_ROUTING_PINGSESSION_MASK_SESSIONIDX)
  {
    if(L7_SUCCESS != ptin_routing_pingsession_free(data->sessionIdx))
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to free ping session");
      return L7_FAILURE;
    }
  }
  else
  {
    if(L7_SUCCESS != ptin_routing_pingsession_freeall())
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to free ping sessions");
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/**
 * Start a traceroute session.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_tracertsession_create(msg_RoutingTracertSessionCreate* data)
{
  if( (data == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Creating new traceroute session:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  sessionIdx    = %u",   data->sessionIdx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  dstIpAddr     = %08X", data->dstIpAddr);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  probePerHop   = %u",   data->probePerHop);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  probeSize     = %u",   data->probeSize);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  probeInterval = %u",   data->probeInterval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  dontFrag      = %u",   data->dontFrag);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  port          = %u",   data->port);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  maxTtl        = %u",   data->maxTtl);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  initTtl       = %u",   data->initTtl);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  maxFail       = %u",   data->maxFail);

  if(L7_SUCCESS != ptin_routing_traceroutesession_create(data->sessionIdx, data->dstIpAddr, data->probeSize, data->probePerHop, data->probeInterval, data->dontFrag, data->port, data->maxTtl, data->initTtl, data->maxFail))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to create new traceroute session");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get traceroute session status.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_tracertsession_query(msg_RoutingTracertSessionQuery* data)
{
  if( (data == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Querying traceroute session:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  sessionIdx = %u", data->sessionIdx);

  /*
   I know that passing a ptin_msghandler struct to a file other than ptin_msg breaks PTIN Fastpath's architecture.
   However, doing so here allows me to hide the complexity of interacting with usmDb completly inside of ptin_routing.                                                                                                           .
   */
  if(L7_SUCCESS != ptin_routing_traceroutesession_query(data))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to query traceroute session");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Get current hops of a given traceroute session.
 * 
 * @param inBuffer
 * @param outBuffer
 * @param maxEntries
 * @param readEntries
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_tracertsession_gethops(msg_RoutingTracertSessionHopsRequest* inBuffer, msg_RoutingTracertSessionHopsResponse* outBuffer, L7_uint32 maxEntries, L7_uint32* readEntries)
{
  if( (inBuffer == L7_NULLPTR) || (outBuffer == L7_NULLPTR) || (readEntries == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Abnormal context [inBuffer=%p outBuffer=%p readEntries=%p]", inBuffer, outBuffer, readEntries);
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Getting traceroute hops:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  sessionIdx = %u", inBuffer->sessionIdx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  lastIndex  = %u", inBuffer->lastIndex);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  maxEntries = %u", maxEntries);

  /*
   I know that passing a ptin_msghandler struct to a file other than ptin_msg breaks PTIN Fastpath's architecture.
   However, doing so here allows me to hide the complexity of interacting with usmDb completly inside of ptin_routing.                                                                                                           .
  */
  if(L7_SUCCESS != ptin_routing_traceroutesession_gethops(inBuffer->sessionIdx, inBuffer->lastIndex, maxEntries, readEntries, outBuffer))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to get traceroute session hops");
    return L7_FAILURE;
  }
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Successfully read %u entries", *readEntries);

  return L7_SUCCESS;
}

/**
 * Free existing traceroute session.
 * 
 * @param data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_routing_tracertsession_free(msg_RoutingTracertSessionFree* data)
{
  if( (data == L7_NULLPTR) )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Freeing traceroute session:");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  mask       = %02X", data->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  sessionIdx = %u",   data->sessionIdx);

  if(data->mask & CCMSG_ROUTING_TRACEROUTESESSION_MASK_SESSIONIDX)
  {
    if(L7_SUCCESS != ptin_routing_traceroutesession_free(data->sessionIdx))
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to free traceroute session");
      return L7_FAILURE;
    }
  }
  else
  {
    if(L7_SUCCESS != ptin_routing_traceroutesession_freeall())
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to free traceroute sessions");
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}



/**
 * This routine is a place holder to trigger events that require 
 * the protection matrix  to be in the state of end 
 * of flush configuration 
 * 
 */
void ptin_msg_protection_matrix_configuration_flush_end(void)
{
  /*Add your code here. In case of error please  let this routine process until the end*/
  {

  }

  { /*Trigger the Sync of the Snooping Table*/   
  #if PTIN_BOARD_IS_MATRIX    
    if(!ptin_fgpa_mx_is_matrixactive())//If I'm a Protection Matrix
    {
      msg_SnoopSyncRequest_t   snoopSyncRequest = {0};
      L7_uint32                ipAddr; 
    
      /* MX board IP address */
      ipAddr = IPC_MX_IPADDR;

      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Sending a Snoop Sync Request Message to ipAddr:%08X", ipAddr);

      /*Send the snoop sync request to the protection matrix */  
      if (send_ipc_message(IPC_HW_FASTPATH_PORT, ipAddr, CCMSG_MGMD_SNOOP_SYNC_REQUEST, (char *)(&snoopSyncRequest), NULL, sizeof(snoopSyncRequest), NULL) < 0)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "Failed to send Snoop Sync Request Message");
//      return;
      }         
    }
    else
    {
      LOG_NOTICE(LOG_CTX_PTIN_MSG, "Not sending Snoop Sync Request Message. Since, I'm not a protection matrix");      
//    return
    }
  #endif
  }

  return;
}
