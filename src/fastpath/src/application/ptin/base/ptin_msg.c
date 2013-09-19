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
#include "ptin_l2.h"
#include "ptin_fieldproc.h"
#include "ptin_cfg.h"
#include "ptin_control.h"
#include "tty_ptin.h"
#include "ipc.h"
#include "ptin_msghandler.h"
#include "nimapi.h"
#include <ptin_prot_oam_eth.h>
#include "ptin_prot_erps.h"
#include "ptin_hal_erps.h"
#include "ptin_intf.h"

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

  return ptin_shell_command_run(tty,type,cmd);
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

  msgFPInfo->SlotIndex    = ptin_board_slotId;
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
void ptin_msg_defaults_reset(void)
{
  LOG_INFO(LOG_CTX_PTIN_MSG, "Resetting to default configuration");

#ifdef __Y1731_802_1ag_OAM_ETH__
  eth_srv_oam_msg_defaults_reset();
#endif

  /* EVCs */
  ptin_evc_destroy_all();

  /* ERPS */
  #ifdef PTIN_ENABLE_ERPS
  ptin_erps_clear();
  ptin_hal_erps_clear();
  #endif

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

  if (phyState.Speed == PHY_PORT_100_MBPS)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_SPEED100_BIT;
  }
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
  if (!phyState.LinkUp)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_LINK_BIT;
  }
  if (phyState.AutoNegComplete)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_AUTONEG_BIT;
  }
  if (phyState.Duplex)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_FULLDUPLEX_BIT;
  }
  if (phyState.Speed == PHY_PORT_1000_MBPS)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_SPEED1000_BIT;
  }
  if (phyConf.Media == PHY_PORT_MEDIA_OPTICAL)
  {
    msgPhyStatus->phy.alarmes |= HW_ETHERNET_STATUS_MASK_MEDIAX_BIT;
  }

  msgPhyStatus->phy.alarmes_mask =  HW_ETHERNET_STATUS_MASK_SPEED100_BIT | HW_ETHERNET_STATUS_MASK_TX_BIT | HW_ETHERNET_STATUS_MASK_RX_BIT | 
                                    HW_ETHERNET_STATUS_MASK_COLLISION_BIT | HW_ETHERNET_STATUS_MASK_LINK_BIT | HW_ETHERNET_STATUS_MASK_AUTONEG_BIT |
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
  L7_uint         i;
  ptin_intf_t     ptin_intf;
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
    portExt_conf.macLearn_enable               = portExt[i].macLearn_enable;
    portExt_conf.macLearn_stationMove_enable   = portExt[i].macLearn_stationMove_enable;
    portExt_conf.macLearn_stationMove_prio     = portExt[i].macLearn_stationMove_prio;
    portExt_conf.macLearn_stationMove_samePrio = portExt[i].macLearn_stationMove_samePrio;

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
    portExt[index].macLearn_enable               = portExt_conf.macLearn_enable;
    portExt[index].macLearn_stationMove_enable   = portExt_conf.macLearn_stationMove_enable;
    portExt[index].macLearn_stationMove_prio     = portExt_conf.macLearn_stationMove_prio;
    portExt[index].macLearn_stationMove_samePrio = portExt_conf.macLearn_stationMove_samePrio;

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

    memset(&portMac_conf,0x00,sizeof(ptin_HWPortExt_t));
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

    memset(&portMac_conf,0x00,sizeof(ptin_HWPortExt_t));
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

    if (ptin_intf_LagConfig_get(&ptinLagConf) != L7_SUCCESS) {
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

  /* Copy data from msg to ptin structure */
  ptinLagConf.lagId            = (L7_uint32) lagInfo->id;
  ptinLagConf.admin            = lagInfo->admin;
  ptinLagConf.static_enable    = lagInfo->static_enable;
  ptinLagConf.stp_enable       = lagInfo->stp_enable;
  ptinLagConf.loadBalance_mode = lagInfo->loadBalance_mode;
  ptinLagConf.members_pbmp64   = (L7_uint64)lagInfo->members_pbmp32[0];
  ptinLagConf.members_pbmp64  |= ((L7_uint64)lagInfo->members_pbmp32[1]) << 32;

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

    if (ptin_intf_LagStatus_get(&ptinLagStatus) != L7_SUCCESS) {
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

    if (ptin_intf_LACPStats_get(&ptinLagStats) != L7_SUCCESS) {
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
L7_RC_t ptin_msg_l2_macTable_get(msg_switch_mac_table_t *mac_table)
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

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if (msgEvcConf->id >= PTIN_SYSTEM_N_EVCS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "EVC# %u is out of range [0..%u]", msgEvcConf->id, PTIN_SYSTEM_N_EVCS-1);
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

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if ((msgEvcConf->id == PTIN_EVC_INBAND) || (msgEvcConf->id >= PTIN_SYSTEM_N_EVCS))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "EVC# %u is out of range [0..%u]", msgEvcConf->id, PTIN_SYSTEM_N_EVCS-1);
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
    ptinEvcConf.intf[i].intf_id   = msgEvcConf->intf[i].intf_id;
    ptinEvcConf.intf[i].intf_type = msgEvcConf->intf[i].intf_type;
    ptinEvcConf.intf[i].mef_type  = msgEvcConf->intf[i].mef_type;
    ptinEvcConf.intf[i].vid       = msgEvcConf->intf[i].vid;

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "   %s# %02u %s VID=%04u",
             ptinEvcConf.intf[i].intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
             ptinEvcConf.intf[i].intf_id,
             ptinEvcConf.intf[i].mef_type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf",
             ptinEvcConf.intf[i].vid);
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
L7_RC_t ptin_msg_EVC_delete(msg_HwEthMef10Evc_t *msgEvcConf)
{
  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EVCS[) */
  if ((msgEvcConf->id == PTIN_EVC_INBAND) || (msgEvcConf->id >= PTIN_SYSTEM_N_EVCS))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "EVC# %u is out of range [0..%u]", msgEvcConf->id, PTIN_SYSTEM_N_EVCS-1);
    return L7_FAILURE;
  }

  if (ptin_evc_delete(msgEvcConf->id) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error deleting EVC# %u", msgEvcConf->id);
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "EVC# %u successfully deleted", msgEvcConf->id);

  return L7_SUCCESS;
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
 * Adds an GEM flow to an EVC
 * 
 * @param msgEvcFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_EVCFlow_add(msg_HwEthEvcFlow_t *msgEvcFlow)
{
  ptin_HwEthEvcFlow_t ptinEvcFlow;

  /* Copy data */
  ptinEvcFlow.evc_idx             = msgEvcFlow->evcId;
  ptinEvcFlow.ptin_intf.intf_type = msgEvcFlow->intf.intf_type;
  ptinEvcFlow.ptin_intf.intf_id   = msgEvcFlow->intf.intf_id;
  ptinEvcFlow.outer_vid           = msgEvcFlow->intf.outer_vid; /* must be a leaf */
  ptinEvcFlow.inner_vid           = msgEvcFlow->intf.inner_vid;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "EVC# %u Flow",     ptinEvcFlow.evc_idx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " %s# %u",          ptinEvcFlow.ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
                                                  ptinEvcFlow.ptin_intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Outer VID = %u", ptinEvcFlow.outer_vid);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Inner VID = %u", ptinEvcFlow.inner_vid);

  if (ptin_evc_gem_flow_add(&ptinEvcFlow) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error adding EVC# %u flow", ptinEvcFlow.evc_idx);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/**
 * Removes a GEM flow from an EVC
 * 
 * @param msgEvcFlow : Flow info
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_EVCFlow_remove(msg_HwEthEvcFlow_t *msgEvcFlow)
{
  ptin_HwEthEvcFlow_t ptinEvcFlow;

  /* Copy data */
  ptinEvcFlow.evc_idx             = msgEvcFlow->evcId;
  ptinEvcFlow.ptin_intf.intf_type = msgEvcFlow->intf.intf_type;
  ptinEvcFlow.ptin_intf.intf_id   = msgEvcFlow->intf.intf_id;
  ptinEvcFlow.outer_vid           = msgEvcFlow->intf.outer_vid; /* must be a leaf */
  ptinEvcFlow.inner_vid           = msgEvcFlow->intf.inner_vid;

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "EVC# %u Flow",     ptinEvcFlow.evc_idx);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " %s# %u",          ptinEvcFlow.ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
                                                  ptinEvcFlow.ptin_intf.intf_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Outer VID = %u", ptinEvcFlow.outer_vid);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, " .Inner VID = %u", ptinEvcFlow.inner_vid);

  if (ptin_evc_gem_flow_remove(&ptinEvcFlow) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error removing EVC# %u flow", ptinEvcFlow.evc_idx);
    return L7_FAILURE;
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
  L7_uint16 evcId;
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
L7_RC_t ptin_msg_bwProfile_set(msg_HwEthBwProfile_t *msgBwProfile)
{
  L7_uint16 evcId;
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
  if ((rc=ptin_evc_bwProfile_set(evcId,&profile))!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error applying profile!");
    return rc;
  }

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
L7_RC_t ptin_msg_bwProfile_delete(msg_HwEthBwProfile_t *msgBwProfile)
{
  L7_uint16 evcId;
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
  rc = ptin_evc_bwProfile_delete(evcId,&profile);

  if ( rc != L7_SUCCESS )
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error removing profile!");
    return rc;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Message processing finished!");
  return L7_SUCCESS;
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
  L7_uint16 evcId;
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
  L7_uint16 evcId;
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
  L7_uint16 evcId;
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
   L7_uint           evc_idx;
   L7_RC_t           rc;

   LOG_DEBUG(LOG_CTX_PTIN_MSG,"Processing message");

   /* Validate input parameters */
   if (dhcpEvcInfo==L7_NULLPTR)  {
     LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid parameters");
     return L7_FAILURE;
   }

   LOG_DEBUG(LOG_CTX_PTIN_MSG, "  EVC Id     = %u",      dhcpEvcInfo->evc_id);
   LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask       = 0x%04X",  dhcpEvcInfo->mask);
   LOG_DEBUG(LOG_CTX_PTIN_MSG, "  DHCP Flag  = %s",      dhcpEvcInfo->dhcp_flag);
   LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Options    = 0x%04X",  dhcpEvcInfo->options);

   /* Extract input data */
   evc_idx = dhcpEvcInfo->evc_id;

   /* TODO: To be reworked */
   rc = ptin_dhcp_evc_reconf(evc_idx, dhcpEvcInfo->dhcp_flag, dhcpEvcInfo->options);
   if (rc!=L7_SUCCESS)
   {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error reconfiguring global DHCP EVC");
      return rc;
   }
   rc = ptin_pppoe_evc_reconf(evc_idx, dhcpEvcInfo->dhcp_flag, dhcpEvcInfo->options);
   /* TODO */
   #if 0
   if (rc!=L7_SUCCESS)
   {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error reconfiguring global PPPoE EVC");
      return rc;
   }
   #endif

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
  L7_uint           evc_idx;
  L7_RC_t           rc;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Processing message");

  /* Validate input parameters */
  if (circuitid==L7_NULLPTR)  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid parameters");
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  EVC Id             = %u",      circuitid->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Template           = %s",      circuitid->template_str);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask               = 0x%04X",  circuitid->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  AccessNode ID      = %s",      circuitid->access_node_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Chassis            = %u",      circuitid->chassis);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Rack               = %u",      circuitid->rack);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Frame              = %u",      circuitid->frame);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Ethernet Priority  = %u",      circuitid->ethernet_priority);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  S-VID              = %u",      circuitid->s_vid);

  /* Extract input data */
  evc_idx = circuitid->evc_id;

  /* TODO: To be reworked */

  /* Set circuit-id global data */
  rc = ptin_dhcp_circuitid_set(evc_idx, circuitid->template_str, circuitid->mask, circuitid->access_node_id, circuitid->chassis, circuitid->rack,
      circuitid->frame, circuitid->ethernet_priority, circuitid->s_vid);
  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error configuring circuit-id global data");
    return rc;
  }
  rc = ptin_pppoe_circuitid_set(evc_idx, circuitid->template_str, circuitid->mask, circuitid->access_node_id, circuitid->chassis, circuitid->rack,
                                circuitid->frame, circuitid->ethernet_priority, circuitid->s_vid);
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
  L7_uint           evc_idx;
  L7_RC_t           rc;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Processing message");

  /* Validate input parameters */
  if (circuitid==L7_NULLPTR)  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid parameters");
    return L7_FAILURE;
  }

  /* Extract input data */
  evc_idx = circuitid->evc_id;

  /* Set circuit-id global data */
  rc = ptin_dhcp_circuitid_get(evc_idx, circuitid->template_str, &circuitid->mask, circuitid->access_node_id, &circuitid->chassis, &circuitid->rack,
      &circuitid->frame, &circuitid->ethernet_priority, &circuitid->s_vid);

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error configuring circuit-id global data");
    return rc;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  EVC Id             = %u",      circuitid->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Template           = %s",      circuitid->template_str);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Mask               = 0x%04X",  circuitid->mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  AccessNode ID      = %s",      circuitid->access_node_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Chassis            = %u",      circuitid->chassis);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Rack               = %u",      circuitid->rack);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Frame              = %u",      circuitid->frame);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Ethernet Priority  = %u",      circuitid->ethernet_priority);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  S-VID              = %u",      circuitid->s_vid);

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
  L7_uint           evc_idx;
  ptin_client_id_t  client;
  L7_RC_t           rc;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Processing message");

  /* Validate input parameters */
  if (profile==L7_NULLPTR)  {
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
  rc = ptin_dhcp_client_get(evc_idx, &client, &profile->options, &profile->circuitId.onuid, &profile->circuitId.slot, &profile->circuitId.port,
      &profile->circuitId.q_vid, &profile->circuitId.c_vid, L7_NULLPTR, profile->remoteId);

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
  L7_uint           i, evc_idx;
  ptin_client_id_t  client;
  L7_RC_t           rc = L7_SUCCESS;

  /* Validate input parameters */
  if (profile==L7_NULLPTR)  {
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
    if( ((profile[i].options & 0x02) >> 1) != ((profile[i].options & 0x08) >> 3) )
    {
       LOG_ERR(LOG_CTX_PTIN_MSG, "Error: UseGlobal_DHCP_options do not match");
       return L7_FAILURE;
    }
    if( ((profile[i].options & 0x08) >> 3) != ((profile[i].options & 0x20) >> 5) )
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

    /* Add circuit and remote ids */
    rc = ptin_dhcp_client_add(evc_idx, &client, profile[i].options, profile[i].circuitId.onuid, profile[i].circuitId.slot,
         profile[i].circuitId.port, profile[i].circuitId.q_vid, profile[i].circuitId.c_vid, profile[i].remoteId);
    if (rc!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error adding DHCP circuitId+remoteId entry");
      return rc;
    }

    rc = ptin_pppoe_client_add(evc_idx, &client, profile[i].options, profile[i].circuitId.onuid, profile[i].circuitId.slot,
                               profile[i].circuitId.port, profile[i].circuitId.q_vid, profile[i].circuitId.c_vid, profile[i].remoteId);
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
  L7_uint           i, evc_idx;
  ptin_client_id_t  client;
  L7_RC_t           rc;

  /* Validate input parameters */
  if (profile==L7_NULLPTR)  {
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
  dhcp_stats->stats.dhcp_tx_client_requests_with_option82         = stats.dhcp_tx_client_requests_with_option82;
  dhcp_stats->stats.dhcp_tx_client_requests_with_option37         = stats.dhcp_tx_client_requests_with_option37;
  dhcp_stats->stats.dhcp_tx_client_requests_with_option18         = stats.dhcp_tx_client_requests_with_option18;
  dhcp_stats->stats.dhcp_rx_server_replies_with_option82          = stats.dhcp_rx_server_replies_with_option82;
  dhcp_stats->stats.dhcp_rx_server_replies_with_option37          = stats.dhcp_rx_server_replies_with_option37;
  dhcp_stats->stats.dhcp_rx_server_replies_with_option18          = stats.dhcp_rx_server_replies_with_option18;
  dhcp_stats->stats.dhcp_tx_server_replies_without_options        = stats.dhcp_tx_server_replies_without_options;

  dhcp_stats->stats.dhcp_rx_client_pkts_onTrustedIntf             = stats.dhcp_rx_client_pkts_onTrustedIntf;
  dhcp_stats->stats.dhcp_rx_client_pkts_withOps_onUntrustedIntf   = stats.dhcp_rx_client_pkts_withOps_onUntrustedIntf;
  dhcp_stats->stats.dhcp_rx_server_pkts_onUntrustedIntf           = stats.dhcp_rx_server_pkts_onUntrustedIntf;
  dhcp_stats->stats.dhcp_rx_server_pkts_withoutOps_onTrustedIntf  = stats.dhcp_rx_server_pkts_withoutOps_onTrustedIntf;

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
  dhcp_stats->stats.dhcp_tx_client_requests_with_option82         = stats.dhcp_tx_client_requests_with_option82;
  dhcp_stats->stats.dhcp_tx_client_requests_with_option37         = stats.dhcp_tx_client_requests_with_option37;
  dhcp_stats->stats.dhcp_tx_client_requests_with_option18         = stats.dhcp_tx_client_requests_with_option18;
  dhcp_stats->stats.dhcp_rx_server_replies_with_option82          = stats.dhcp_rx_server_replies_with_option82;
  dhcp_stats->stats.dhcp_rx_server_replies_with_option37          = stats.dhcp_rx_server_replies_with_option37;
  dhcp_stats->stats.dhcp_rx_server_replies_with_option18          = stats.dhcp_rx_server_replies_with_option18;
  dhcp_stats->stats.dhcp_tx_server_replies_without_options        = stats.dhcp_tx_server_replies_without_options;

  dhcp_stats->stats.dhcp_rx_client_pkts_onTrustedIntf             = stats.dhcp_rx_client_pkts_onTrustedIntf;
  dhcp_stats->stats.dhcp_rx_client_pkts_withOps_onUntrustedIntf   = stats.dhcp_rx_client_pkts_withOps_onUntrustedIntf;
  dhcp_stats->stats.dhcp_rx_server_pkts_onUntrustedIntf           = stats.dhcp_rx_server_pkts_onUntrustedIntf;
  dhcp_stats->stats.dhcp_rx_server_pkts_withoutOps_onTrustedIntf  = stats.dhcp_rx_server_pkts_withoutOps_onTrustedIntf;

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
  L7_ushort16 i, page, first, entries, size;
  L7_RC_t     rc;

  page = input->page;

  // For index null, read all mac entries
  if (page==0) {
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
  if (entries>128)  entries = 128;          // Overgoes 128? If so, limit to 128

  output->bind_table_msg_size      = entries;
  output->bind_table_total_entries = dhcp_bindtable_entries;

  // Copy binding table entries
  for (i=0; i<entries; i++)
  {
    memset(&output->bind_table[i],0x00,sizeof(msg_DHCP_bind_entry));

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


/* IGMP Management Functions **************************************************/
/**
 * Applies IGMP Proxy configuration
 * 
 * @param msgIgmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_igmp_proxy_set(msg_IgmpProxyCfg_t *msgIgmpProxy)
{
  ptin_IgmpProxyCfg_t ptinIgmpProxy;
  L7_RC_t rc;

  /* Copy data */
  ptinIgmpProxy.mask                                   = msgIgmpProxy->mask;
  ptinIgmpProxy.admin                                  = msgIgmpProxy->admin;
  ptinIgmpProxy.networkVersion                         = msgIgmpProxy->networkVersion;
  ptinIgmpProxy.clientVersion                          = msgIgmpProxy->clientVersion;
  ptinIgmpProxy.ipv4_addr.s_addr                       = msgIgmpProxy->ipv4_addr.s_addr;
  ptinIgmpProxy.igmp_cos                               = msgIgmpProxy->igmp_cos;
  ptinIgmpProxy.fast_leave                             = msgIgmpProxy->fast_leave;

  ptinIgmpProxy.querier.mask                           = msgIgmpProxy->querier.mask;
  ptinIgmpProxy.querier.flags                          = msgIgmpProxy->querier.flags;
  ptinIgmpProxy.querier.robustness                     = msgIgmpProxy->querier.robustness;
  ptinIgmpProxy.querier.query_interval                 = msgIgmpProxy->querier.query_interval;
  ptinIgmpProxy.querier.query_response_interval        = msgIgmpProxy->querier.query_response_interval;
  ptinIgmpProxy.querier.group_membership_interval      = msgIgmpProxy->querier.group_membership_interval;
  ptinIgmpProxy.querier.other_querier_present_interval = msgIgmpProxy->querier.other_querier_present_interval;
  ptinIgmpProxy.querier.startup_query_interval         = msgIgmpProxy->querier.startup_query_interval;
  ptinIgmpProxy.querier.startup_query_count            = msgIgmpProxy->querier.startup_query_count;
  ptinIgmpProxy.querier.last_member_query_interval     = msgIgmpProxy->querier.last_member_query_interval;
  ptinIgmpProxy.querier.last_member_query_count        = msgIgmpProxy->querier.last_member_query_count;
  ptinIgmpProxy.querier.older_host_present_timeout     = msgIgmpProxy->querier.older_host_present_timeout;

  ptinIgmpProxy.host.mask                              = msgIgmpProxy->host.mask;
  ptinIgmpProxy.host.flags                             = msgIgmpProxy->host.flags;
  ptinIgmpProxy.host.robustness                        = msgIgmpProxy->host.robustness;
  ptinIgmpProxy.host.unsolicited_report_interval       = msgIgmpProxy->host.unsolicited_report_interval;
  ptinIgmpProxy.host.older_querier_present_timeout     = msgIgmpProxy->host.older_querier_present_timeout;
  ptinIgmpProxy.host.max_records_per_report            = msgIgmpProxy->host.max_records_per_report;

  /* Output data */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "IGMP Proxy (mask=0x%08X)", ptinIgmpProxy.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Admin #                          = %u", ptinIgmpProxy.admin);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Network Version                  = %u", ptinIgmpProxy.networkVersion);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Client Version                   = %u", ptinIgmpProxy.clientVersion);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  IP Addr                          = %u.%u.%u.%u", (ptinIgmpProxy.ipv4_addr.s_addr  >> 24) & 0xFF, (ptinIgmpProxy.ipv4_addr.s_addr  >> 16) & 0xFF,
                                                          (ptinIgmpProxy.ipv4_addr.s_addr  >>  8) & 0xFF,  ptinIgmpProxy.ipv4_addr.s_addr         & 0xFF);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  COS                              = %u", ptinIgmpProxy.igmp_cos);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  FastLeave                        = %s", ptinIgmpProxy.fast_leave != 0 ? "ON":"OFF");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Querier (mask=0x%08X)", ptinIgmpProxy.querier.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Flags                          = 0x%04X", ptinIgmpProxy.querier.flags);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Robustness                     = %u", ptinIgmpProxy.querier.robustness);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Query Interval                 = %u", ptinIgmpProxy.querier.query_interval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Query Response Interval        = %u", ptinIgmpProxy.querier.query_response_interval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Group Membership Interval      = %u", ptinIgmpProxy.querier.group_membership_interval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Other Querier Present Interval = %u", ptinIgmpProxy.querier.other_querier_present_interval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Startup Query Interval         = %u", ptinIgmpProxy.querier.startup_query_interval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Startup Query Count            = %u", ptinIgmpProxy.querier.startup_query_count);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Last Member Query Interval     = %u", ptinIgmpProxy.querier.last_member_query_interval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Last Member Query Count        = %u", ptinIgmpProxy.querier.last_member_query_count);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Older Host Present Timeout     = %u", ptinIgmpProxy.querier.older_host_present_timeout);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Host (mask=0x%08X)", ptinIgmpProxy.host.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Flags                          = 0x%02X", ptinIgmpProxy.host.flags);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Robustness                     = %u", ptinIgmpProxy.host.robustness);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Unsolicited Report Interval    = %u", ptinIgmpProxy.host.unsolicited_report_interval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Older Querier Present Timeout  = %u", ptinIgmpProxy.host.older_querier_present_timeout);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Max Group Records per Packet   = %u", ptinIgmpProxy.host.max_records_per_report);

  /* Apply config */
  rc = ptin_igmp_proxy_config_set(&ptinIgmpProxy);

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
  ptin_IgmpProxyCfg_t ptinIgmpProxy;
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
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  IP Addr                          = %u.%u.%u.%u", (ptinIgmpProxy.ipv4_addr.s_addr  >> 24) & 0xFF, (ptinIgmpProxy.ipv4_addr.s_addr  >> 16) & 0xFF,
                                                                                 (ptinIgmpProxy.ipv4_addr.s_addr  >>  8) & 0xFF,  ptinIgmpProxy.ipv4_addr.s_addr         & 0xFF);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  COS                              = %u", ptinIgmpProxy.igmp_cos);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  FastLeave                        = %s", ptinIgmpProxy.fast_leave != 0 ? "ON":"OFF");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Querier (mask=0x%08X)", ptinIgmpProxy.querier.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Flags                          = 0x%08X", ptinIgmpProxy.querier.flags);  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Robustness                     = %u", ptinIgmpProxy.querier.robustness);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Query Interval                 = %u", ptinIgmpProxy.querier.query_interval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Query Response Interval        = %u", ptinIgmpProxy.querier.query_response_interval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Group Membership Interval      = %u", ptinIgmpProxy.querier.group_membership_interval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Other Querier Present Interval = %u", ptinIgmpProxy.querier.other_querier_present_interval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Startup Query Interval         = %u", ptinIgmpProxy.querier.startup_query_interval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Startup Query Count            = %u", ptinIgmpProxy.querier.startup_query_count);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Last Member Query Interval     = %u", ptinIgmpProxy.querier.last_member_query_interval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Last Member Query Count        = %u", ptinIgmpProxy.querier.last_member_query_count);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Older Host Present Timeout     = %u", ptinIgmpProxy.querier.older_host_present_timeout);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Host (mask=0x%08X)", ptinIgmpProxy.host.mask);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Flags                          = 0x%02X", ptinIgmpProxy.host.flags);  
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Robustness                     = %u", ptinIgmpProxy.host.robustness);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Unsolicited Report Interval    = %u", ptinIgmpProxy.host.unsolicited_report_interval);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "    Older Querier Present  Timeout = %u", ptinIgmpProxy.host.older_querier_present_timeout);

  /* Copy data */
  msgIgmpProxy->mask                                   = ptinIgmpProxy.mask;
  msgIgmpProxy->admin                                  = ptinIgmpProxy.admin;
  msgIgmpProxy->networkVersion                         = ptinIgmpProxy.networkVersion;
  msgIgmpProxy->clientVersion                          = ptinIgmpProxy.clientVersion;
  msgIgmpProxy->ipv4_addr.s_addr                       = ptinIgmpProxy.ipv4_addr.s_addr;
  msgIgmpProxy->igmp_cos                               = ptinIgmpProxy.igmp_cos;
  msgIgmpProxy->fast_leave                             = ptinIgmpProxy.fast_leave;

  msgIgmpProxy->querier.mask                           = ptinIgmpProxy.querier.mask;
  msgIgmpProxy->querier.flags                          = ptinIgmpProxy.querier.flags;  
  msgIgmpProxy->querier.robustness                     = ptinIgmpProxy.querier.robustness;
  msgIgmpProxy->querier.query_interval                 = ptinIgmpProxy.querier.query_interval;
  msgIgmpProxy->querier.query_response_interval        = ptinIgmpProxy.querier.query_response_interval;
  msgIgmpProxy->querier.group_membership_interval      = ptinIgmpProxy.querier.group_membership_interval;
  msgIgmpProxy->querier.other_querier_present_interval = ptinIgmpProxy.querier.other_querier_present_interval;
  msgIgmpProxy->querier.startup_query_interval         = ptinIgmpProxy.querier.startup_query_interval;
  msgIgmpProxy->querier.startup_query_count            = ptinIgmpProxy.querier.startup_query_count;
  msgIgmpProxy->querier.last_member_query_interval     = ptinIgmpProxy.querier.last_member_query_interval;
  msgIgmpProxy->querier.last_member_query_count        = ptinIgmpProxy.querier.last_member_query_count;
  msgIgmpProxy->querier.older_host_present_timeout     = ptinIgmpProxy.querier.older_host_present_timeout;

  msgIgmpProxy->host.mask                              = ptinIgmpProxy.host.mask;
  msgIgmpProxy->host.flags                             = ptinIgmpProxy.host.flags;  
  msgIgmpProxy->host.robustness                        = ptinIgmpProxy.host.robustness;
  msgIgmpProxy->host.unsolicited_report_interval       = ptinIgmpProxy.host.unsolicited_report_interval;
  msgIgmpProxy->host.older_querier_present_timeout     = ptinIgmpProxy.host.older_querier_present_timeout;

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
    rc = ptin_igmp_client_add(McastClient[i].mcEvcId,&client);

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
  ptin_client_id_t client;
  ptin_IGMP_Statistics_t stats;
  L7_RC_t rc;

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

#if 0
  /* Return data */
  igmp_stats->stats.active_groups             = stats.active_groups;            
  igmp_stats->stats.active_clients            = stats.active_clients;           
  igmp_stats->stats.igmp_sent                 = stats.igmp_sent;                
  igmp_stats->stats.igmp_tx_failed            = stats.igmp_tx_failed;           
  igmp_stats->stats.igmp_intercepted          = stats.igmp_intercepted;         
  igmp_stats->stats.igmp_dropped              = stats.igmp_dropped;             
  igmp_stats->stats.igmp_received_valid       = stats.igmp_received_valid;      
  igmp_stats->stats.igmp_received_invalid     = stats.igmp_received_invalid;    
  igmp_stats->stats.joins_sent                = stats.joins_sent;               
  igmp_stats->stats.joins_received_success    = stats.joins_received_success;   
  igmp_stats->stats.joins_received_failed     = stats.joins_received_failed;    
  igmp_stats->stats.leaves_sent               = stats.leaves_sent;              
  igmp_stats->stats.leaves_received           = stats.leaves_received;          
  igmp_stats->stats.membership_report_v3      = stats.membership_report_v3;     
  igmp_stats->stats.general_queries_sent      = stats.general_queries_sent;     
  igmp_stats->stats.general_queries_received  = stats.general_queries_received; 
  igmp_stats->stats.specific_queries_sent     = stats.specific_queries_sent;    
  igmp_stats->stats.specific_queries_received = stats.specific_queries_received;
#else
 /* Return data */
  igmp_stats->stats.active_groups                                                       = stats.active_groups;            
  igmp_stats->stats.active_clients                                                      = stats.active_clients;           

  igmp_stats->stats.igmp_tx                                                             = stats.igmp_sent;                  
  igmp_stats->stats.igmp_total_rx                                                       = stats.igmp_intercepted;                     
  igmp_stats->stats.igmp_valid_rx                                                       = stats.igmp_received_valid;      
  igmp_stats->stats.igmp_invalid_rx                                                     = stats.igmp_received_invalid+stats.igmpv3.membership_report_invalid_rx+stats.igmpquery.generic_query_invalid_rx;    
  igmp_stats->stats.igmp_dropped_rx                                                     = stats.igmp_dropped+stats.igmpv3.membership_report_dropped_rx+stats.igmpquery.generic_query_dropped_rx; 

  igmp_stats->stats.HWIgmpv2Statistics.join_tx                                          = stats.joins_sent;               
  igmp_stats->stats.HWIgmpv2Statistics.join_valid_rx                                    = stats.joins_received_success;   
  igmp_stats->stats.HWIgmpv2Statistics.join_invalid_rx                                  = stats.joins_received_failed;    

  igmp_stats->stats.HWIgmpv2Statistics.leave_tx                                         = stats.leaves_sent;              
  igmp_stats->stats.HWIgmpv2Statistics.leave_valid_rx                                   = stats.leaves_received;        
  
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_tx                             =stats.igmpv3.membership_report_tx; 
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_valid_rx                       =stats.igmpv3.membership_report_valid_rx;      
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_invalid_rx                     =stats.igmpv3.membership_report_invalid_rx;           
  
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_tx                 =stats.igmpv3.group_record.allow_tx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_valid_rx           =stats.igmpv3.group_record.allow_valid_rx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_invalid_rx         =stats.igmpv3.group_record.allow_invalid_rx;

  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_tx                 =stats.igmpv3.group_record.block_tx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_valid_rx           =stats.igmpv3.group_record.block_valid_rx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_invalid_rx         =stats.igmpv3.group_record.block_invalid_rx;

  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_tx            =stats.igmpv3.group_record.is_include_tx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_valid_rx      =stats.igmpv3.group_record.is_include_valid_rx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_invalid_rx    =stats.igmpv3.group_record.is_include_invalid_rx;

  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_tx            =stats.igmpv3.group_record.is_exclude_tx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_valid_rx      =stats.igmpv3.group_record.is_exclude_valid_rx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_invalid_rx    =stats.igmpv3.group_record.is_exclude_invalid_rx;

  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_tx            =stats.igmpv3.group_record.to_include_tx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_valid_rx      =stats.igmpv3.group_record.to_include_valid_rx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_invalid_rx    =stats.igmpv3.group_record.to_include_invalid_rx;

  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_tx            =stats.igmpv3.group_record.to_exclude_tx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_valid_rx      =stats.igmpv3.group_record.to_exclude_valid_rx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_invalid_rx    =stats.igmpv3.group_record.to_exclude_invalid_rx;

  igmp_stats->stats.HWQueryStatistics.general_query_tx                                  =stats.igmpquery.general_query_tx;
  igmp_stats->stats.HWQueryStatistics.general_query_valid_rx                            =stats.igmpquery.general_query_valid_rx;

  igmp_stats->stats.HWQueryStatistics.group_query_tx                                    =stats.igmpquery.group_query_tx;   
  igmp_stats->stats.HWQueryStatistics.group_query_valid_rx                              =stats.igmpquery.group_query_valid_rx;

  igmp_stats->stats.HWQueryStatistics.source_query_tx                                   =stats.igmpquery.source_query_tx;   
  igmp_stats->stats.HWQueryStatistics.source_query_valid_rx                             =stats.igmpquery.group_query_valid_rx;  
#endif

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
  ptin_intf_t ptin_intf;
  ptin_IGMP_Statistics_t stats;
  L7_RC_t rc;

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

#if 0
  /* Return data */
  igmp_stats->stats.active_groups             = stats.active_groups;            
  igmp_stats->stats.active_clients            = stats.active_clients;           
  igmp_stats->stats.igmp_sent                 = stats.igmp_sent;                
  igmp_stats->stats.igmp_tx_failed            = stats.igmp_tx_failed;           
  igmp_stats->stats.igmp_intercepted          = stats.igmp_intercepted;         
  igmp_stats->stats.igmp_dropped              = stats.igmp_dropped;             
  igmp_stats->stats.igmp_received_valid       = stats.igmp_received_valid;      
  igmp_stats->stats.igmp_received_invalid     = stats.igmp_received_invalid;    
  igmp_stats->stats.joins_sent                = stats.joins_sent;               
  igmp_stats->stats.joins_received_success    = stats.joins_received_success;   
  igmp_stats->stats.joins_received_failed     = stats.joins_received_failed;    
  igmp_stats->stats.leaves_sent               = stats.leaves_sent;              
  igmp_stats->stats.leaves_received           = stats.leaves_received;          
  igmp_stats->stats.membership_report_v3      = stats.membership_report_v3;     
  igmp_stats->stats.general_queries_sent      = stats.general_queries_sent;     
  igmp_stats->stats.general_queries_received  = stats.general_queries_received; 
  igmp_stats->stats.specific_queries_sent     = stats.specific_queries_sent;    
  igmp_stats->stats.specific_queries_received = stats.specific_queries_received;
#else
  /* Return data */
  igmp_stats->stats.active_groups                                                       = stats.active_groups;            
  igmp_stats->stats.active_clients                                                      = stats.active_clients;           

  igmp_stats->stats.igmp_tx                                                             = stats.igmp_sent;                  
  igmp_stats->stats.igmp_total_rx                                                       = stats.igmp_intercepted;                     
  igmp_stats->stats.igmp_valid_rx                                                       = stats.igmp_received_valid;      
  igmp_stats->stats.igmp_invalid_rx                                                     = stats.igmp_received_invalid+stats.igmpv3.membership_report_invalid_rx+stats.igmpquery.generic_query_invalid_rx;    
  igmp_stats->stats.igmp_dropped_rx                                                     = stats.igmp_dropped+stats.igmpv3.membership_report_dropped_rx+stats.igmpquery.generic_query_dropped_rx; 

  igmp_stats->stats.HWIgmpv2Statistics.join_tx                                          = stats.joins_sent;               
  igmp_stats->stats.HWIgmpv2Statistics.join_valid_rx                                    = stats.joins_received_success;   
  igmp_stats->stats.HWIgmpv2Statistics.join_invalid_rx                                  = stats.joins_received_failed;    

  igmp_stats->stats.HWIgmpv2Statistics.leave_tx                                         = stats.leaves_sent;              
  igmp_stats->stats.HWIgmpv2Statistics.leave_valid_rx                                   = stats.leaves_received;        
  
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_tx                             =stats.igmpv3.membership_report_tx; 
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_valid_rx                       =stats.igmpv3.membership_report_valid_rx;      
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_invalid_rx                     =stats.igmpv3.membership_report_invalid_rx;           
  
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_tx                 =stats.igmpv3.group_record.allow_tx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_valid_rx           =stats.igmpv3.group_record.allow_valid_rx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_invalid_rx         =stats.igmpv3.group_record.allow_invalid_rx;

  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_tx                 =stats.igmpv3.group_record.block_tx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_valid_rx           =stats.igmpv3.group_record.block_valid_rx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_invalid_rx         =stats.igmpv3.group_record.block_invalid_rx;

  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_tx            =stats.igmpv3.group_record.is_include_tx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_valid_rx      =stats.igmpv3.group_record.is_include_valid_rx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_invalid_rx    =stats.igmpv3.group_record.is_include_invalid_rx;

  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_tx            =stats.igmpv3.group_record.is_exclude_tx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_valid_rx      =stats.igmpv3.group_record.is_exclude_valid_rx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_invalid_rx    =stats.igmpv3.group_record.is_exclude_invalid_rx;

  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_tx            =stats.igmpv3.group_record.to_include_tx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_valid_rx      =stats.igmpv3.group_record.to_include_valid_rx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_invalid_rx    =stats.igmpv3.group_record.to_include_invalid_rx;

  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_tx            =stats.igmpv3.group_record.to_exclude_tx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_valid_rx      =stats.igmpv3.group_record.to_exclude_valid_rx;
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_invalid_rx    =stats.igmpv3.group_record.to_exclude_invalid_rx;

  igmp_stats->stats.HWQueryStatistics.general_query_tx                                  =stats.igmpquery.general_query_tx;
  igmp_stats->stats.HWQueryStatistics.general_query_valid_rx                            =stats.igmpquery.general_query_valid_rx;

  igmp_stats->stats.HWQueryStatistics.group_query_tx                                    =stats.igmpquery.group_query_tx;   
  igmp_stats->stats.HWQueryStatistics.group_query_valid_rx                              =stats.igmpquery.group_query_valid_rx;

  igmp_stats->stats.HWQueryStatistics.source_query_tx                                   =stats.igmpquery.source_query_tx;   
  igmp_stats->stats.HWQueryStatistics.source_query_valid_rx                             =stats.igmpquery.group_query_valid_rx;  
#endif


  
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

    if (igmp_assoc_channel_add( 0, channel_list[i].evcid_mc,
                                &groupAddr , channel_list[i].channel_dstmask,
                                &sourceAddr, channel_list[i].channel_srcmask, L7_FALSE ) != L7_SUCCESS )
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error adding group address 0x%08x/%u, source address 0x%08x/%u to MC EVC %u",
              channel_list[i].channel_dstIp.addr.ipv4, channel_list[i].channel_dstmask,
              channel_list[i].channel_srcIp.addr.ipv4, channel_list[i].channel_srcmask,
              channel_list[i].evcid_mc);
      return L7_FAILURE;
    }
    #else
    rc = L7_NOT_SUPPORTED;
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Not supported!");
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

    if (igmp_assoc_channel_remove( 0,
                                   &groupAddr , channel_list[i].channel_dstmask,
                                   &sourceAddr, channel_list[i].channel_srcmask ) != L7_SUCCESS )
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error removing group address 0x%08x/%u, source address 0x%08x/%u to MC EVC %u",
              channel_list[i].channel_dstIp.addr.ipv4, channel_list[i].channel_dstmask,
              channel_list[i].channel_srcIp.addr.ipv4, channel_list[i].channel_srcmask,
              channel_list[i].evcid_mc);
      return L7_FAILURE;
    }

    #else
    rc = L7_NOT_SUPPORTED;
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Not supported!");
    #endif
  }

  return rc;
}

/**
 * Add a static group channel to MFDB table
 * 
 * @param channel : static group channel
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_staticChannel_add(msg_MCStaticChannel_t *channel)
{
  L7_in_addr_t in_addr;
  L7_RC_t rc;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Static channel addition");
  LOG_DEBUG(LOG_CTX_PTIN_MSG," SlotId =%u",channel->SlotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," EvcId  =%u",channel->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Channel=%u.%u.%u.%u",
            (channel->channelIp.s_addr>>24) & 0xff,(channel->channelIp.s_addr>>16) & 0xff,(channel->channelIp.s_addr>>8) & 0xff,channel->channelIp.s_addr & 0xff);

  #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  /* Add this channel to associations list */
  L7_inet_addr_t groupAddr, sourceAddr;

  memset(&groupAddr , 0x00, sizeof(sizeof(L7_inet_addr_t)));
  memset(&sourceAddr, 0x00, sizeof(sizeof(L7_inet_addr_t)));
  groupAddr.family = L7_AF_INET;
  groupAddr.addr.ipv4.s_addr = channel->channelIp.s_addr;

  /* Add channel */
  rc=igmp_assoc_channel_add(0, channel->evc_id, &groupAddr, 32, &sourceAddr, 32, L7_TRUE);

  if (rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error adding group 0x%08x to association list (MC EVC id %u)",groupAddr.addr.ipv4.s_addr,channel->evc_id);
    return rc;
  }
  #endif

  in_addr.s_addr = channel->channelIp.s_addr;

  rc = ptin_igmp_static_channel_add(channel->evc_id,&in_addr);

  if (rc!=L7_SUCCESS)
  {
    #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
    igmp_assoc_channel_remove(0, &groupAddr, 32, &sourceAddr, 32);    /* Undo */
    #endif
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error adding static channel");
    return rc;
  }

  return L7_SUCCESS;
}

/**
 * Remove a static group channel from MFDB table
 * 
 * @param channel : static group channel
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_channel_remove(msg_MCStaticChannel_t *channel)
{
  L7_in_addr_t in_addr;
  L7_RC_t rc;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Channel remotion");
  LOG_DEBUG(LOG_CTX_PTIN_MSG," SlotId =%u",channel->SlotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," EvcId  =%u",channel->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Channel=%u.%u.%u.%u",
            (channel->channelIp.s_addr>>24) & 0xff,(channel->channelIp.s_addr>>16) & 0xff,(channel->channelIp.s_addr>>8) & 0xff,channel->channelIp.s_addr & 0xff);

  in_addr.s_addr = channel->channelIp.s_addr;

  rc = ptin_igmp_channel_remove(channel->evc_id,&in_addr);

  if (rc!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Error removing channel");
    return rc;
  }

  #ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  /* Remove this channel from association list */
  L7_inet_addr_t groupAddr, sourceAddr;

  memset(&groupAddr , 0x00, sizeof(sizeof(L7_inet_addr_t)));
  memset(&sourceAddr, 0x00, sizeof(sizeof(L7_inet_addr_t)));
  groupAddr.family = L7_AF_INET;
  groupAddr.addr.ipv4.s_addr = channel->channelIp.s_addr;

  /* Remove channel */
  rc = igmp_assoc_channel_remove(0, &groupAddr, 32, &sourceAddr, 32);

  if ( rc != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_IGMP,"Error removing group 0x%08x from association list",groupAddr.addr.ipv4.s_addr);
    return rc;
  }
  #endif

  return L7_SUCCESS;
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

  if(numberOfChannels == L7_NULLPTR)
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
    *numberOfChannels = number_of_channels;
    
    /* Copy channels to message */
    for (i=0; i<(*numberOfChannels) && i<number_of_channels; i++)
    {
      LOG_TRACE(LOG_CTX_PTIN_MSG,"Client[%u] -> Group:[%08X] Source[%08X]", i, clist[i].groupAddr.addr.ipv4.s_addr, clist[i].sourceAddr.addr.ipv4.s_addr);
      outputPtr[i].entryId = i;
      outputPtr[i].chIP    = clist[i].groupAddr.addr.ipv4.s_addr;
      outputPtr[i].srcIP   = clist[i].sourceAddr.addr.ipv4.s_addr;
      outputPtr[i].chType  = clist[i].static_type;
    }
  }
  else if (rc==L7_NOT_EXIST)
  {
    *numberOfChannels = 0;
    LOG_WARNING(LOG_CTX_PTIN_MSG, "No channels to retrieve");
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Error with ptin_igmp_channelList_get");
    return rc;
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
  L7_in_addr_t channelIp;
  ptin_client_id_t clist[MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX];
  L7_uint16 i, number_of_clients, total_clients;
  L7_RC_t rc;

  LOG_DEBUG(LOG_CTX_PTIN_MSG,"Going to retrieve list of clients");
  LOG_DEBUG(LOG_CTX_PTIN_MSG," slotId     =%u",client_list->SlotId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," EvcId      =%u",client_list->evc_id);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," IPv4Channel=%u.%u.%u.%u",(client_list->channelIp.s_addr>>24) & 0xff,(client_list->channelIp.s_addr>>16) & 0xff,(client_list->channelIp.s_addr>>8) & 0xff,client_list->channelIp.s_addr & 0xff);
  LOG_DEBUG(LOG_CTX_PTIN_MSG," Page_idx=%u",client_list->page_index);

  /* Get list of channels */
  channelIp.s_addr    = client_list->channelIp.s_addr;
  number_of_clients   = MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX;

  rc = ptin_igmp_clientList_get(client_list->evc_id, &channelIp, client_list->page_index*MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX, &number_of_clients, clist, &total_clients);

  if (rc==L7_SUCCESS)
  {
    /* Copy channels to message */
    for (i=0; i<MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX && i<number_of_clients; i++)
    {
      client_list->clients_list[i].mask       = clist[i].mask;
      client_list->clients_list[i].outer_vlan = clist[i].outerVlan;
      client_list->clients_list[i].inner_vlan = clist[i].innerVlan;
      client_list->clients_list[i].intf.intf_type = clist[i].ptin_intf.intf_type;
      client_list->clients_list[i].intf.intf_id   = clist[i].ptin_intf.intf_id;
    }
    client_list->n_pages_total = (total_clients==0) ? 1 : ((total_clients-1)/MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX+1);
    client_list->n_clients_total = total_clients;
    client_list->n_clients_msg = number_of_clients;
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
  profile->outer_vlan_in  = 0;
  profile->outer_vlan_out = 0;
  if ((msgBwProfile->mask & MSG_HWETH_BWPROFILE_MASK_SVLAN) &&
      (msgBwProfile->service_vlan>0 && msgBwProfile->service_vlan<4096))
  {
    profile->outer_vlan_in = msgBwProfile->service_vlan;
    LOG_DEBUG(LOG_CTX_PTIN_MSG," SVID extracted!");
  }

  /* CVID */
  profile->inner_vlan_in  = 0;
  profile->inner_vlan_out = 0;
  if ((msgBwProfile->mask & MSG_HWETH_BWPROFILE_MASK_CVLAN) &&
      (msgBwProfile->client_vlan>0 && msgBwProfile->client_vlan<4096))
  {
    profile->inner_vlan_in = msgBwProfile->client_vlan;
    LOG_DEBUG(LOG_CTX_PTIN_MSG," CVID extracted!");
  }

  /* Source interface */
  profile->ddUsp_src.unit = profile->ddUsp_src.slot = profile->ddUsp_src.port = -1;
  if (msgBwProfile->mask & MSG_HWETH_BWPROFILE_MASK_INTF_SRC)
  {
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
  L7_uint32 intIfNum;
  nimUSP_t  usp;

  /* Validate arguments */
  if (msg_evcStats==L7_NULLPTR || evcStats_profile==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG,"Null arguments");
    return L7_FAILURE;
  }

  /* Source interface */
  evcStats_profile->ddUsp_src.unit = evcStats_profile->ddUsp_src.slot = evcStats_profile->ddUsp_src.port = -1;
  evcStats_profile->ddUsp_dst.unit = evcStats_profile->ddUsp_dst.slot = evcStats_profile->ddUsp_dst.port = -1;
  if (msg_evcStats->mask & MSG_EVC_COUNTERS_MASK_INTF)
  {
    /* Get ptin_port */
    if (ptin_msg_ptinPort_get(msg_evcStats->intf.intf_type,msg_evcStats->intf.intf_id, &ptin_port)!=L7_SUCCESS)
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
    evcStats_profile->ddUsp_src.unit = usp.unit;
    evcStats_profile->ddUsp_src.slot = usp.slot;
    evcStats_profile->ddUsp_src.port = usp.port - 1;
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Intf extracted!");
  }

  /* SVID */
  evcStats_profile->outer_vlan_in  = 0;
  evcStats_profile->outer_vlan_out = 0;
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

  pi=(msg_bd_mep_t *)inbuff->info;
  po=(msg_generic_prefix_t *)outbuff->info;
  po[i].index = pi[i].index;

  porta = pi[i].bd.prt;

  
  switch (wr_mep(pi[i].index, (T_MEP_HDR*)&pi[i].bd, &oam)) {
  case 0:    r=S_OK;
             ptin_ccm_packet_trap(porta, pi[i].bd.vid, pi[i].bd.level, 1);
             break;
  case 2:    r=ERROR_CODE_FULLTABLE;    break;
  case 3:    r=  CCMSG_FLUSH_MEP==inbuff->msgId?   S_OK:   ERROR_CODE_FULLTABLE; break;
  case 4:    r=ERROR_CODE_NOTPRESENT;  break;
  default:   r=ERROR_CODE_INVALIDPARAM; break;
  }//switch
  

  if (r==S_OK) {
    return L7_SUCCESS;
  } else {
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

  if (i_mep<N_MEPs) {
      prt=oam.mep_db[i_mep].prt;
      vid=oam.mep_db[i_mep].vid;
      level=oam.mep_db[i_mep].level;
  }

  
  switch (del_mep(i_mep, &oam)) {
  case 0:    r=S_OK;
             ptin_ccm_packet_trap(prt, vid, level, 0);
             break;
    //case 2:    r=HW_RESOURCE_UNAVAILABLE;  break;
  default:   r=ERROR_CODE_INVALIDPARAM; break;
  }//switch
  

  if (r==S_OK) {
    return L7_SUCCESS;
  } else {
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

  if (!valid_mep_index(i_mep)) {
    return L7_FAILURE;
  }

  i_rmep=    MEP_INDEX_TO_iRMEP(pi[i].index);

  
  p_oam= &oam;
  switch (wr_rmep(i_mep, i_rmep, &pi[i].bd, (T_MEP_HDR*)&p_oam->mep_db[i_mep], p_oam)) {
  case 0:
    r=S_OK;
    break;
  case 4:
    if (CCMSG_FLUSH_RMEP==inbuff->msgId) {
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
  

  if (r==S_OK) {
    return L7_SUCCESS;
  } else {
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

  switch (del_rmep(i_mep, i_rmep, p_oam)) {
  case 0:    r=S_OK;             break;
  //case 2:    r=HW_RESOURCE_UNAVAILABLE; break;
  default:   r=ERROR_CODE_INVALIDPARAM;
  }
  

  if (r==S_OK) {
    return L7_SUCCESS;
  } else {
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

  if (pi->index>=N_MEPs) {
    return(L7_FAILURE);
  }

  
  p_oam= &oam;

  for (i=pi->index, n=0; i<N_MEPs; i++) {
    po[n].index = i;
    po[n].err_code = S_OK;

    //set_active_to_(p_oam->mep_db);
    //if (!active_is_used(p_oam->mep_db))
    //_p_mep= pointer2active_node_info(*p_mep_db);

    if (!EMPTY_T_MEP(p_oam->mep_db[i])  ||  N_MEPs-1==i)
      po[n++].bd=  *((T_MEP_HDR *) &p_oam->mep_db[i]);

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

  if (!valid_mep_index(i_mep) || !valid_rmep_index(i_rmep)) {
    return(L7_FAILURE);
  }

  
  p_oam= &oam;

  if (EMPTY_T_MEP(p_oam->mep_db[i_mep])) {
    return(L7_FAILURE);
  }

  for (i=i_rmep, n=0; i<N_MAX_MEs_PER_MEP; i++) {
    po[n].index = iMEP_iRMEP_TO_MEP_INDEX(i_mep, i);//i_mep*0x10000L+i;
    po[n].err_code = S_OK;

    if (   !EMPTY_T_MEP(p_oam->mep_db[i_mep].ME[i])
           ||  N_MAX_MEs_PER_MEP-1==i) {
      po[n].bd.me=     p_oam->mep_db[i_mep].ME[i];
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

  if (pi->index>=N_MAX_LOOKUP_MEPs) {
    return(L7_FAILURE);
  }

  
  p_oam= &oam;

  for (i=pi->index, n=0; i<N_MAX_LOOKUP_MEPs; i++) {
    po[n].index = i;
    po[n].err_code = S_OK;

    if (!EMPTY_T_MEP(p_oam->mep_lut[i])) {
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
  if (msgErpsConf->idx >= MAX_PROT_PROT_ERPS) {
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

  if (ptin_erps_add_entry(msgErpsConf->idx, (erpsProtParam_t *) &ptinErpsConf) != msgErpsConf->idx) {
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
  if (msgErpsConf->idx >= MAX_PROT_PROT_ERPS) {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ERPS#%u is out of range [0..%u]", msgErpsConf->idx, MAX_PROT_PROT_ERPS-1);
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ERPS#%u", msgErpsConf->idx);

  if (ptin_erps_remove_entry(msgErpsConf->idx) != msgErpsConf->idx) {
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
  if (msgErpsConf->idx >= MAX_PROT_PROT_ERPS) {
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

  if (ptin_erps_conf_entry(msgErpsConf->idx, msgErpsConf->mask, (erpsProtParam_t *) &ptinErpsConf) != msgErpsConf->idx) {
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
  if (msgErpsStatus->idx >= MAX_PROT_PROT_ERPS) {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ERPS#%u is out of range [0..%u]", msgErpsStatus->idx, MAX_PROT_PROT_ERPS-1);
    return L7_FAILURE;
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "ERPS#%u", msgErpsStatus->idx);

  if (ptin_erps_get_status(msgErpsStatus->idx, &status) != msgErpsStatus->idx) {
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

  while ( i < CCMSG_ERPS_STATUS_PAGESIZE ) {

    /* Validate ERPS# range (idx [0..MAX_PROT_PROT_ERPS[) */
    if (nextIdx >= MAX_PROT_PROT_ERPS) {
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "ERPS#%u is out of range [0..%u]", nextIdx, MAX_PROT_PROT_ERPS-1);
      break;
    }

    if (ptin_erps_get_status(nextIdx, &status) != nextIdx) {
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
  if (msgErpsCmd->idx >= MAX_PROT_PROT_ERPS) {
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





#ifdef __802_1x__

#include <sirerrors.h>
#include <usmdb_dot1x_api.h>
#include <usmdb_dot1x_auth_serv_api.h>
#include <dot1x_auth_serv_exports.h>
#include <dot1x_auth_serv_api.h>


int msg_wr_802_1x_Genrc(ipc_msg *inbuff, ipc_msg *outbuff, L7_ulong32 i) {
msg_802_1x_Genrc *pi, *po;
L7_RC_t r;

 pi=(msg_802_1x_Genrc *)inbuff->info;   po=(msg_802_1x_Genrc *)outbuff->info;

 switch (inbuff->msgId) {
 case CCMSG_WR_802_1X_ADMINMODE:        r=usmDbDot1xAdminModeSet(1, pi[i].v); break;
 case CCMSG_WR_802_1X_TRACE:            r=usmDbDot1xPacketDebugTraceFlagSet(pi[i].v>>1, pi[i].v&1); break;
 case CCMSG_WR_802_1X_VLANASSGNMODE:    r=usmDbDot1xVlanAssignmentModeSet(1, pi[i].v); break;
 case CCMSG_WR_802_1X_MONMODE:          r=usmDbDot1xMonitorModeSet(1, pi[i].v); break;
 case CCMSG_WR_802_1X_DYNVLANMODE:      r=usmDbDot1xDynamicVlanCreationModeSet(1, pi[i].v); break;
 default:
     po[i].v = SIR_ERROR(ERROR_FAMILY_HARDWARE,ERROR_SEVERITY_ERROR,ERROR_CODE_INVALIDPARAM);
     return 1;
 }

 if (L7_SUCCESS!=r) {
   po[i].v = SIR_ERROR(ERROR_FAMILY_HARDWARE,ERROR_SEVERITY_ERROR,ERROR_CODE_INVALIDPARAM);
   return 1;
 }
 else po[i].v = ERROR_CODE_OK;

 return 0;
}//msg_wr_802_1x_Genrc

















int msg_wr_802_1x_Genrc2(ipc_msg *inbuff, ipc_msg *outbuff, L7_ulong32 i) {
msg_802_1x_Genrc2 *pi;
msg_generic_prefix_t *po;
ptin_intf_t ptinp;
L7_RC_t r;
L7_uint32 intIfNum;

 pi=(msg_802_1x_Genrc2 *)inbuff->info;   po=(msg_generic_prefix_t *)outbuff->info;

 ptinp.intf_type=   pi[i].index>>8;
 ptinp.intf_id=     pi[i].index;
 if (L7_SUCCESS!=ptin_intf_ptintf2intIfNum(&ptinp, &intIfNum)) {
     po[i].err_code = SIR_ERROR(ERROR_FAMILY_HARDWARE,ERROR_SEVERITY_ERROR,ERROR_CODE_INVALIDPARAM);
     return 1;
 }

 switch (inbuff->msgId) {
    case CCMSG_WR_802_1X_ADMINCONTROLLEDDIRECTIONS: r=usmDbDot1xPortAdminControlledDirectionsSet(1, intIfNum, pi[i].v); break;
    case CCMSG_WR_802_1X_PORTCONTROLMODE:           r=usmDbDot1xPortControlModeSet(1, intIfNum, pi[i].v);   break;
    case CCMSG_WR_802_1X_QUIETPERIOD:               r=usmDbDot1xPortQuietPeriodSet(1, intIfNum, pi[i].v);   break;
    case CCMSG_WR_802_1X_TXPERIOD:                  r=usmDbDot1xPortTxPeriodSet(1, intIfNum, pi[i].v);  break;
    case CCMSG_WR_802_1X_SUPPTIMEOUT:               r=usmDbDot1xPortSuppTimeoutSet(1, intIfNum, pi[i].v);   break;
    case CCMSG_WR_802_1X_SERVERTIMEOUT:             r=usmDbDot1xPortServerTimeoutSet(1, intIfNum, pi[i].v); break;
    case CCMSG_WR_802_1X_MAXREQ:                    r=usmDbDot1xPortMaxReqSet(1, intIfNum, pi[i].v);    break;
    case CCMSG_WR_802_1X_REAUTHPERIOD:
        if (1+pi[i].v==0) {r=usmDbDot1xPortReAuthEnabledSet(1, intIfNum, 0);   break;}  //Forbidden period disables
        r=usmDbDot1xPortReAuthPeriodSet(1, intIfNum, pi[i].v);
        if (L7_SUCCESS!=r) break;
        r=usmDbDot1xPortReAuthEnabledSet(1, intIfNum, 1);
        break;
    case CCMSG_WR_802_1X_KEYTXENABLED:              r=usmDbDot1xPortKeyTransmissionEnabledSet(1, intIfNum, pi[i].v); break;
    case CCMSG_WR_802_1X_GUESTVLANID:               r=usmDbDot1xAdvancedGuestPortsCfgSet(1, intIfNum, pi[i].v); break;
    case CCMSG_WR_802_1X_GUSTVLANPERIOD:            r=usmDbDot1xAdvancedPortGuestVlanPeriodSet(1, intIfNum, pi[i].v); break;
    case CCMSG_WR_802_1X_MAXUSERS:                  r=usmDbDot1xPortMaxUsersSet(1, intIfNum, pi[i].v); break;
    case CCMSG_WR_802_1X_UNAUTHENTICATEDVLAN:       r=usmDbDot1xPortUnauthenticatedVlanSet(1, intIfNum, pi[i].v); break;
    default:
        po[i].err_code = SIR_ERROR(ERROR_FAMILY_HARDWARE,ERROR_SEVERITY_ERROR,ERROR_CODE_INVALIDPARAM);
        return 1;
 }

 if (L7_SUCCESS!=r) {
   po[i].err_code = SIR_ERROR(ERROR_FAMILY_HARDWARE,ERROR_SEVERITY_ERROR,ERROR_CODE_INVALIDPARAM);
   return 1;
 }
 else po[i].err_code = ERROR_CODE_OK;

 return 0;
}//msg_wr_802_1x_Genrc2
















L7_RC_t usmDbDot1xAuthServUserDBUserIndexGet(L7_char8 *name, L7_uint32 *index) {
    return dot1xAuthServUserDBUserIndexGet(name, index);
}

L7_RC_t usmDbDot1xAuthServUserDBUserNameSet(L7_uint32 index, L7_char8 *name) {
    return dot1xAuthServUserDBUserNameSet(index, name);
}

L7_RC_t usmDbDot1xAuthServUserDBUserPasswordSet(L7_uint32 index, L7_char8 *password, L7_BOOL encrypted) {
    return dot1xAuthServUserDBUserPasswordSet(index, password, encrypted);
}

L7_RC_t usmDbDot1xAuthServUserDBAvailableIndexGet(L7_uint32 *index) {
    return dot1xAuthServUserDBAvailableIndexGet(index);
}

int msg_wr_802_1x_AuthServ(ipc_msg *inbuff, ipc_msg *outbuff, L7_ulong32 i) {
msg_802_1x_AuthServ *pi;
msg_generic_prefix_t *po;
L7_RC_t r;
L7_ulong32 k;           //MNGMT preferred index
L7_uint32 index=-1;     //index to eventual already in table entry
L7_uchar8 e;

 pi=(msg_802_1x_AuthServ *)inbuff->info;   po=(msg_generic_prefix_t *)outbuff->info;

 k=pi[i].index; //64th bit's lost
 e=pi[i].index>>63;

 if (L7_SUCCESS==usmDbDot1xAuthServUserDBUserIndexGet(pi[i].name, &index)) {
     if (k<L7_MAX_IAS_USERS && k!=index) {  //Name already in table with different index
         po[i].err_code = SIR_ERROR(ERROR_FAMILY_HARDWARE,ERROR_SEVERITY_ERROR,ERROR_CODE_DUPLICATENAME);
         return 1;
     }
 }
 else index=-1;

 if (k<L7_MAX_IAS_USERS) {
     if (k!=index && (L7_SUCCESS!=(r=usmDbDot1xAuthServUserDBUserNameSet(k, pi[i].name)))) {   //index already used (or table full)
       po[i].err_code = SIR_ERROR(ERROR_FAMILY_HARDWARE,ERROR_SEVERITY_ERROR,ERROR_CODE_USED);
       return 1;
     }
 }
 else {
     if (index>=L7_MAX_IAS_USERS) {
         if (L7_SUCCESS!=usmDbDot1xAuthServUserDBAvailableIndexGet(&index)) {
             po[i].err_code = SIR_ERROR(ERROR_FAMILY_HARDWARE,ERROR_SEVERITY_ERROR,ERROR_CODE_FULLTABLE);
             return 1;
         }
     }
     k=index;
 }

 if (L7_SUCCESS!=(r=usmDbDot1xAuthServUserDBUserPasswordSet(k, pi[i].passwd, e))) {
   po[i].err_code = SIR_ERROR(ERROR_FAMILY_HARDWARE,ERROR_SEVERITY_ERROR,ERROR_CODE_INVALIDPARAM);
   return 1;
 }
 else po[i].err_code = ERROR_CODE_OK;

 return 0;
}//msg_wr_802_1x_AuthServ
#endif //__802_1x__

