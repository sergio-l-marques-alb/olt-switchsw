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
#include "usmdb_dot1q_api.h"
#include "usmdb_dai_api.h"
#include "usmdb_dot3ad_api.h"
#include "mirror_api.h"
#include "usmdb_mirror_api.h"
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

#include "ptin_rfc2819_buffer.h"
#include "ptin_rfc2819.h"
#include "dai_api.h"
#include <dtl_ptin.h>

#include <ptin_ipdtl0_packet.h>

#include "sirerrors.h"

#define CMD_MAX_LEN   200   /* Shell command maximum length */

#define IS_FAILURE_ERROR(rc)  ((rc) != L7_NOT_EXIST          && \
                               (rc) != L7_ALREADY_CONFIGURED && \
                               (rc) != L7_NOT_SUPPORTED      && \
                               (rc) != L7_DEPENDENCY_NOT_MET && \
                               (rc) != L7_NOT_IMPLEMENTED_YET )

/******************************************************** 
 * STATIC FUNCTIONS PROTOTYPES
 ********************************************************/
static L7_RC_t ptin_msg_ptinPort_get(L7_uint8 intf_type, L7_uint8 intf_id, L7_int *ptin_port);

static L7_RC_t ptin_shell_command_run(L7_char8 *tty, L7_char8 *type, L7_char8 *cmd);

static void ptin_msg_PortStats_convert(msg_HWEthRFC2819_PortStatistics_t  *msgPortStats,
                                       ptin_HWEthRFC2819_PortStatistics_t *ptinPortStats);

static L7_RC_t ptin_msg_bwProfileStruct_fill(msg_HwEthBwProfile_t *msgBwProfile, ptin_bw_profile_t *profile, ptin_bw_meter_t *meter);
static L7_RC_t ptin_msg_evcStatsStruct_fill(msg_evcStats_t *msg_evcStats, ptin_evcStats_profile_t *evcStats_profile);

L7_RC_t ptin_to_fp_ip_notation(chmessage_ip_addr_t *ptinIpAddr, L7_inet_addr_t *fpIpAddr);


static L7_BOOL force_capture = L7_FALSE;

void ptin_force_capture(L7_BOOL force)
{
  force_capture = force;
}


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

  msgFPInfo->SlotIndex    = ENDIAN_SWAP8 (ptin_fpga_board_slot_get());
  msgFPInfo->BoardPresent = ENDIAN_SWAP32((ptin_state == PTIN_STATE_READY));

  snprintf(msgFPInfo->BoardSerialNumber, 19, "OLTSW-SDK-%u.%u.%u.%u", SDK_MAJOR_VERSION, SDK_MINOR_VERSION, SDK_REVISION_ID, SDK_PATCH_ID);
  msgFPInfo->BoardSerialNumber[19] = '\0';

  PT_LOG_NOTICE(LOG_CTX_MSG, "Board info: \"%s\"", msgFPInfo->BoardSerialNumber);

  return L7_SUCCESS;
}

L7_RC_t fp_to_ptin_ip_notation(L7_inet_addr_t *fpIpAddr, chmessage_ip_addr_t *ptinIpAddr)
{
   if ( ptinIpAddr == L7_NULLPTR || fpIpAddr == L7_NULLPTR)
   {
      return L7_FAILURE;
   } 

   memset(ptinIpAddr, 0, sizeof(*ptinIpAddr));

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
     PT_LOG_ERR(LOG_CTX_MSG, "IP Family Address of FP not Supported:%u",fpIpAddr->family);
     return L7_NOT_SUPPORTED;
   }   
}

L7_RC_t ptin_to_fp_ip_notation(chmessage_ip_addr_t *ptinIpAddr, L7_inet_addr_t *fpIpAddr)
{
   if ( ptinIpAddr == L7_NULLPTR || fpIpAddr == L7_NULLPTR)
   {
      return L7_FAILURE;
   }
  
   if ( ptinIpAddr->family == PTIN_AF_INET )
   {      
     inetAddressSet(L7_AF_INET, &ptinIpAddr->addr.ipv4, fpIpAddr);          
     return L7_SUCCESS;
   }
   else if ( ptinIpAddr->family == PTIN_AF_INET6 )
   {
     inetAddressSet(L7_AF_INET6, &ptinIpAddr->addr.ipv6, fpIpAddr);          
     return L7_SUCCESS;
   }
   else
   {
     PT_LOG_ERR(LOG_CTX_MSG, "IP Family Address of PTIN not Supported:%u",fpIpAddr->family);
     return L7_NOT_SUPPORTED;
   }   
}

/* Reset Functions ************************************************************/
/**
 * Reset to default configuration 
 *  
 * Actions: 
 *  - EVCs are destroyed (including counter, bw profiles, clientes, etc)
 */
extern void ptin_msg_defaults_reset(msg_HwGenReq_t *msgPtr)
{
  L7_uint8 mode;

  if (msgPtr == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid Parameters: msgPtr:%p", msgPtr);
    return;
  }
  mode = ENDIAN_SWAP8(msgPtr->param);
  
  PT_LOG_INFO(LOG_CTX_CONTROL,"Executing a reset defaults with mode=%u", mode);

  /*This Should be the First Module*/
  /* Reset IGMP Module */
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on IGMP...");
  ptin_igmp_default_reset();
  PT_LOG_INFO(LOG_CTX_MSG, "Done.");

   /* Reset DAI Module */
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on DAI...");
  daiRestore();
  PT_LOG_INFO(LOG_CTX_MSG, "Done.");

#ifdef __Y1731_802_1ag_OAM_ETH__
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on OAM...");
  eth_srv_oam_msg_defaults_reset();
  PT_LOG_INFO(LOG_CTX_MSG, "Done.");
#endif

  /* Reset Routing Module*/
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on Routing...");
  ptin_routing_intf_remove_all();
  PT_LOG_INFO(LOG_CTX_MSG, "Done.");

  /* ERPS */
#ifdef PTIN_ENABLE_ERPS
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on ERPS...");
  ptin_erps_clear();
  PT_LOG_INFO(LOG_CTX_MSG, "Done.");
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on HAL...");
  ptin_hal_erps_clear();
  PT_LOG_INFO(LOG_CTX_MSG, "Done.");
#endif

  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on ACL...");
  ptin_aclCleanAll();
  PT_LOG_INFO(LOG_CTX_MSG, "Done.");

  /* Reset EVC Module */
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on EVC...");
  ptin_evc_destroy_all();
  PT_LOG_INFO(LOG_CTX_MSG, "Done.");

  /* Reset Mirror */
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Mirror Reset...");
  ptin_mirror_reset();
  PT_LOG_INFO(LOG_CTX_MSG, "Done.");

  /* Reset RFC 2819 */
  PT_LOG_INFO(LOG_CTX_MSG, "Performing RFC 2819 reset...");
  ptin_rfc2819_init_buffers();
  ptin_rfc2819_init_all_probes();
  PT_LOG_INFO(LOG_CTX_MSG, "Done.");

  if (mode == DEFAULT_RESET_MODE_FULL)
  {
    ptin_NtwConnectivity_t ptinNtwConn;

    /* Unconfig Connectivity */
    memset(&ptinNtwConn, 0x00, sizeof(ptin_NtwConnectivity_t));
    ptinNtwConn.mask = PTIN_NTWCONN_MASK_IPADDR;
    PT_LOG_INFO(LOG_CTX_MSG, "(Re)Configure Inband...");
    ptin_cfg_ntw_connectivity_set(&ptinNtwConn);
    PT_LOG_INFO(LOG_CTX_MSG, "Done.");

    /*This Should be the Last Module*/
    PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on LAG...");
    ptin_intf_Lag_delete_all();
    PT_LOG_INFO(LOG_CTX_MSG, "Done.");
  }
}


/**
 * Routine to asynchronously handle message processing if a 
 * given timeout is reached.
 *  
 * @param msgId   : Message Identifier 
 * @param msgPtr  : Message Pointer 
 * @param msgSize : Message Size
 * @param timeOut : Time Out (milliseconds) 
 *  
 * @notes: timeOut supported values: 
 *  - -1 (L7_WAIT_FOREVER)
 *  -  0 (L7_NO_WAIT)
 *  - >0 (wait for a short amount of period - typically less
 *    then IPC_LIB timeout ~3 seconds)
 *  
 **/
void ptin_msg_task_process(L7_uint32 msgId, void *msgPtr, L7_uint32 msgSize, L7_int32 timeOut)
{
  static L7_uint32 msgBuffer[IPCLIB_MAX_MSGSIZE] = {0};
  L7_RC_t rc;
  
  PT_LOG_INFO(LOG_CTX_CONTROL,"Going to take ptin_ready_sem:%p waiting for it %d (ms)",ptin_ready_sem, timeOut);

  /* Lock Ready State */
  rc = osapiSemaTake(ptin_ready_sem, timeOut);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Failed to schedule a new task 0x%x within defined timeout:%d (ms)!", msgId, timeOut);
    return;
  }

  PT_LOG_INFO(LOG_CTX_MSG, "Scheduling a new task 0x%x to be handle within timeout:%d (ms)", msgId, timeOut);

  ptin_task_msg_id = msgId;
  memcpy(&msgBuffer, msgPtr, msgSize);
  ptin_task_msg_buffer = &msgBuffer;
 
  /* Unlock Busy State*/
  osapiSemaGive(ptin_busy_sem);

#if 0
    /* Only for non linecards: wait 3 seconds for fw to be ready again */
#if (!PTIN_BOARD_IS_LINECARD)
  L7_int8 cycles_100ms = 30;

  /* Wait until state goes back to ready again */
  while (ptin_state == PTIN_STATE_BUSY && (--cycles_100ms) >= 0)
  {
    osapiSleepMSec(100);
  }
  /* After 3 seconds, function should be terminated */
  if (cycles_100ms == 0)
  {
    PT_LOG_INFO(LOG_CTX_MSG, "Timeout... Leaving %s function.", __FUNCTION__); 
    return;
  }
  PT_LOG_INFO(LOG_CTX_MSG, "Operation completed! Leaving %s function.", __FUNCTION__); 
#else
  
#endif
#else
  /* Lock Ready State */
  rc = osapiSemaTake(ptin_ready_sem, timeOut);
  if (rc)
  {
    PT_LOG_INFO(LOG_CTX_MSG, "Timeout %d (ms) expired. Message Still Being Processed: 0x%x", timeOut, msgId); 
  }
  else
  {
    /* Unlock Ready State */
    osapiSemaGive(ptin_ready_sem);
    PT_LOG_INFO(LOG_CTX_MSG, "Message Processed 0x%x Within TimeOut %d (ms) ", msgId, timeOut);         
  }
#endif
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

  PT_LOG_DEBUG(LOG_CTX_MSG, "Type-B Protection switch notification");
  PT_LOG_DEBUG(LOG_CTX_MSG, " slotId = %u"   , ENDIAN_SWAP8(msg->slotId));
  PT_LOG_DEBUG(LOG_CTX_MSG, " portId = %u"   , ENDIAN_SWAP8(msg->portId)); //ptin_port format
  PT_LOG_DEBUG(LOG_CTX_MSG, " cmd    = %08X" , ENDIAN_SWAP8(msg->cmd));

  /* Convert portId to intfNum */
  if (ptin_intf_port2intIfNum(ENDIAN_SWAP8(msg->portId), &intIfNum)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Non existent port");
    return L7_FAILURE;
  }

  /* Get interface status from the first bit of msg->cmd */
  status = ENDIAN_SWAP8(msg->cmd) & 0x0001;

  /* Update interface configurations */
  rc = ptin_prottypeb_intf_switch_notify(intIfNum, status);
  if (rc!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unable to set interface's type-b protection configurations");
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

  PT_LOG_DEBUG(LOG_CTX_MSG, "Configurations");
  PT_LOG_DEBUG(LOG_CTX_MSG, " slotId     = %u"   , ENDIAN_SWAP8(msg->slotId));
  PT_LOG_DEBUG(LOG_CTX_MSG, " intfId     = %u/%u", ENDIAN_SWAP8(msg->intfId.intf_type), ENDIAN_SWAP8(msg->intfId.intf_id));
  PT_LOG_DEBUG(LOG_CTX_MSG, " pairSlotId = %u"   , ENDIAN_SWAP8(msg->pairSlotId));
  PT_LOG_DEBUG(LOG_CTX_MSG, " pairIntfId = %u/%u", ENDIAN_SWAP8(msg->pairIntfId.intf_type), ENDIAN_SWAP8(msg->pairIntfId.intf_id));
  PT_LOG_DEBUG(LOG_CTX_MSG, " intfRole   = %u"   , ENDIAN_SWAP8(msg->intfRole));

  memset(&ptin_intfConfig, 0x00, sizeof(ptin_intfConfig));

  /* Convert intfId to intfNum */
  if (ptin_msg_ptinPort_get(ENDIAN_SWAP8(msg->intfId.intf_type), ENDIAN_SWAP8(msg->intfId.intf_id), &ptin_port)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid port");
    return L7_FAILURE;
  }
  if (ptin_intf_port2intIfNum(ptin_port, &ptin_intfConfig.intfNum)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Non existent port");
    return L7_FAILURE;
  }

  /* Convert pairIntfId to intfNum */
  if (ptin_msg_ptinPort_get(ENDIAN_SWAP8(msg->pairIntfId.intf_type), ENDIAN_SWAP8(msg->pairIntfId.intf_id), &ptin_port)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid port");
    return L7_FAILURE;
  }
  if (ptin_intf_port2intIfNum(ptin_port, &ptin_intfConfig.pairIntfNum)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Non existent port");
    return L7_FAILURE;
  }

  if (ptin_intfConfig.intfNum == ptin_intfConfig.pairIntfNum && ENDIAN_SWAP8(msg->slotId) == ENDIAN_SWAP8(msg->pairSlotId))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid Parameters: slotId=pairSlotId=%u, intfNum=pairIntfNum=%u", ENDIAN_SWAP8(msg->slotId), ptin_intfConfig.intfNum);
    return L7_FAILURE;
  }
  
  ptin_intfConfig.pairSlotId = ENDIAN_SWAP8(msg->pairSlotId);
  ptin_intfConfig.intfRole   = ENDIAN_SWAP8(msg->intfRole);
  ptin_intfConfig.slotId     = ENDIAN_SWAP8(msg->slotId);

  /* Save interface configurations */
  rc = ptin_prottypeb_intf_config_set(&ptin_intfConfig);
  if (rc!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unable to set interface's type-b protection configurations");
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
  L7_uint32 intIfNum;
  L7_uint32 lag_idx;

  PT_LOG_DEBUG(LOG_CTX_MSG, "ptin_msg_typeBprotSwitch(slot %d, port %d)", ENDIAN_SWAP8(msg->slot), ENDIAN_SWAP8(msg->port));

  rc = ptin_intf_slot2lagIdx(ENDIAN_SWAP8(msg->slot), &lag_idx);
  if (rc==L7_SUCCESS)
  {
    rc = ptin_intf_lag2intIfNum(lag_idx, &intIfNum);
    if (rc==L7_SUCCESS)
    {
      rc = fdbFlushByPort(intIfNum);
    }
  }
#endif

#if (PTIN_BOARD_IS_STANDALONE)
  ptin_intf_t ptin_intf;
  L7_uint32 ptin_port;

  PT_LOG_DEBUG(LOG_CTX_MSG, "ptin_msg_typeBprotSwitch(slot %d, port %d)", ENDIAN_SWAP8(msg->slot), ENDIAN_SWAP8(msg->port));

  ptin_intf.intf_type = PTIN_EVC_INTF_PHYSICAL;
  ptin_intf.intf_id = ENDIAN_SWAP8(msg->port);

  ptin_intf_ptintf2port(&ptin_intf, &ptin_port);

  if (rc==L7_SUCCESS)
  {
    rc = switching_fdbFlushVlanByPort(ptin_port);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Non existent port");
  }
#endif


#if (PTIN_BOARD_IS_MATRIX)
  /* Reset MGMD General Querier state */
  rc = ptin_igmp_generalquerier_reset();
  if (rc!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unable to reset MGMD General Queriers");
  }
#endif

  if (rc!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "return %d", rc);
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

  PT_LOG_INFO(LOG_CTX_MSG, "ptin_msg_board_action");
  PT_LOG_DEBUG(LOG_CTX_MSG," slot       = %u",   ENDIAN_SWAP8(msg->slot_id));
  PT_LOG_DEBUG(LOG_CTX_MSG," generic_id = %u",    ENDIAN_SWAP8(msg->generic_id));
  PT_LOG_DEBUG(LOG_CTX_MSG," type       = 0x%02x", ENDIAN_SWAP8(msg->type));
  PT_LOG_DEBUG(LOG_CTX_MSG," param      = 0x%02x", ENDIAN_SWAP8(msg->param));

  #if (PTIN_BOARD_IS_MATRIX)

  /* insertion action */
  if (ENDIAN_SWAP8(msg->type) == 0x03)
  {
    PT_LOG_INFO(LOG_CTX_MSG,"Insertion detected (slot %u, board_id=%u)", ENDIAN_SWAP8(msg->generic_id), ENDIAN_SWAP8(msg->param));

    rc = ptin_slot_action_insert(ENDIAN_SWAP8(msg->generic_id), ENDIAN_SWAP8(msg->param));
    if ( rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error inserting card (%d)", rc);
    }
    else
    {
      PT_LOG_INFO(LOG_CTX_MSG, "Card inserted successfully");
    }
  }
  /* Board removed */
  else if (ENDIAN_SWAP8(msg->type) == 0x00)
  {
    PT_LOG_INFO(LOG_CTX_MSG,"Remotion detected (slot %u)", ENDIAN_SWAP8(msg->generic_id));

    rc = ptin_slot_action_remove(ENDIAN_SWAP8(msg->generic_id));
    if ( rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error removing card (%d)", rc);
    }
    else
    {
      PT_LOG_INFO(LOG_CTX_MSG, "Card removed successfully");
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

  PT_LOG_INFO(LOG_CTX_MSG, "ptin_msg_link_action");
  PT_LOG_DEBUG(LOG_CTX_MSG," slot       = %u", ENDIAN_SWAP8(msg->slot_id));
  PT_LOG_DEBUG(LOG_CTX_MSG," generic_id = %u", ENDIAN_SWAP8(msg->generic_id));
  PT_LOG_DEBUG(LOG_CTX_MSG," type       = 0x%02x", ENDIAN_SWAP8(msg->type));
  PT_LOG_DEBUG(LOG_CTX_MSG," param      = 0x%02x", ENDIAN_SWAP8(msg->param));

  #if 0
  #if (PTIN_BOARD_IS_MATRIX)
  #ifdef MAP_CPLD
  L7_uint16 board_type;
  L7_uint32 ptin_port, intIfNum;

  /* Only active matrix will process these messages */
  if (!cpld_map->reg.mx_is_active)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "I am not active matrix");
    return L7_SUCCESS;
  }

  osapiSemaTake(ptin_boardaction_sem, L7_WAIT_FOREVER);

  /* Get board id for this interface */
  rc = ptin_slot_boardid_get(ENDIAN_SWAP8(msg->generic_id), &board_type);
  if (rc != L7_SUCCESS)
  {
    osapiSemaGive(ptin_boardaction_sem);
    PT_LOG_ERR(LOG_CTX_MSG, "Error getting board_id for slot id %u (rc=%d)", ENDIAN_SWAP8(msg->generic_id), rc);
    return L7_FAILURE;
  }

  /* Only consider uplink boards */
  if (!PTIN_BOARD_IS_UPLINK(board_type))
  {
    osapiSemaGive(ptin_boardaction_sem);
    PT_LOG_ERR(LOG_CTX_MSG, "Not an uplink board (board_id=%u)", board_type);
    return L7_FAILURE;
  }

  /* Get ptin_port and intIfNum */
  if (ptin_intf_slotPort2port(ENDIAN_SWAP8(msg->generic_id), ENDIAN_SWAP8(msg->param), &ptin_port) != L7_SUCCESS)
  {
    osapiSemaGive(ptin_boardaction_sem);
    PT_LOG_ERR(LOG_CTX_MSG, "No ptin_port related to slot/port %u/%u", ENDIAN_SWAP8(msg->generic_id), ENDIAN_SWAP8(msg->param));
    return L7_FAILURE;
  }
  if (ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
  {
    osapiSemaGive(ptin_boardaction_sem);
    PT_LOG_ERR(LOG_CTX_MSG, "No intIfNum related to ptin_port %u", ptin_port);
    return L7_FAILURE;
  }

  /* Should be a protection port */
  if (!ptin_intf_is_uplinkProtection(ptin_port))
  {
    osapiSemaGive(ptin_boardaction_sem);
    PT_LOG_ERR(LOG_CTX_MSG, "ptin_port %u is not a protection port", ptin_port);
    return L7_FAILURE;
  }

  /* When link is up, disable linkscan */
  if (ENDIAN_SWAP8(msg->type == 0x01))
  {
    PT_LOG_DEBUG(LOG_CTX_MSG,"Link-up detected at ptin_port %u", ptin_port);

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
        PT_LOG_ERR(LOG_CTX_API, "Error enabling force linkup for port %u (%d)", ptin_port, rc);
      }

      /* Add port to vlans again */
      if (ptin_intf_is_uplinkProtectionActive(ptin_port))
      {
        ptin_vlan_port_add(ptin_port, 0);
      }

      PT_LOG_DEBUG(LOG_CTX_API, "Forced linkup for port %u", ptin_port);
    }
    #endif
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG,"Link-down detected at ptin_port %u", ptin_port);

    #ifdef PTIN_LINKSCAN_CONTROL
    if (linkscan_update_control)
    {
      /* Remove forced link-up */
      rc = ptin_intf_link_force(intIfNum, L7_TRUE, L7_DISABLE);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_API, "Error disabling force linkup for port %u (%d)", ptin_port, rc);
      }

      /* Cause link-down */
      rc = ptin_intf_link_force(intIfNum, L7_FALSE, 0);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_API, "Error disabling force linkup for port %u (%d)", ptin_port, rc);
      }
      PT_LOG_DEBUG(LOG_CTX_API, "Force link-up disabled for port %u", ptin_port);
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
  PT_LOG_INFO(LOG_CTX_MSG, "Resetting alarms states");

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
  slotId = ENDIAN_SWAP8(msgResources->SlotId);
  memset(msgResources,0x00,sizeof(msg_ptin_policy_resources));
  msgResources->SlotId = ENDIAN_SWAP8(slotId);

  /* Read hardware available resources */
  if ((rc=ptin_hw_resources_get(&resources))!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error consulting hardware resources");
    return rc;
  }

  /* Copy data to output structure */
  for (stage_idx=0; stage_idx<PTIN_POLICY_MAX_STAGES; stage_idx++)
  {
    for (group_idx=0; group_idx<PTIN_POLICY_MAX_GROUPS; group_idx++)
    {
      msgResources->cap[group_idx][stage_idx].inUse             = ENDIAN_SWAP8 (resources.cap[group_idx][stage_idx].inUse);
      msgResources->cap[group_idx][stage_idx].group_id          = ENDIAN_SWAP32(resources.cap[group_idx][stage_idx].group_id);

      msgResources->cap[group_idx][stage_idx].total.counters    = ENDIAN_SWAP16(resources.cap[group_idx][stage_idx].total.counters);
      msgResources->cap[group_idx][stage_idx].total.meters      = ENDIAN_SWAP16(resources.cap[group_idx][stage_idx].total.meters);
      msgResources->cap[group_idx][stage_idx].total.rules       = ENDIAN_SWAP16(resources.cap[group_idx][stage_idx].total.rules);
      msgResources->cap[group_idx][stage_idx].total.slice_width = ENDIAN_SWAP16(resources.cap[group_idx][stage_idx].total.slice_width);

      msgResources->cap[group_idx][stage_idx].free.counters     = ENDIAN_SWAP16(resources.cap[group_idx][stage_idx].free.counters);
      msgResources->cap[group_idx][stage_idx].free.meters       = ENDIAN_SWAP16(resources.cap[group_idx][stage_idx].free.meters);
      msgResources->cap[group_idx][stage_idx].free.rules        = ENDIAN_SWAP16(resources.cap[group_idx][stage_idx].free.rules);
      msgResources->cap[group_idx][stage_idx].free.slice_width  = ENDIAN_SWAP16(resources.cap[group_idx][stage_idx].free.slice_width);

      msgResources->cap[group_idx][stage_idx].count.counters    = ENDIAN_SWAP16(resources.cap[group_idx][stage_idx].count.counters);
      msgResources->cap[group_idx][stage_idx].count.meters      = ENDIAN_SWAP16(resources.cap[group_idx][stage_idx].count.meters);
      msgResources->cap[group_idx][stage_idx].count.rules       = ENDIAN_SWAP16(resources.cap[group_idx][stage_idx].count.rules);
      msgResources->cap[group_idx][stage_idx].count.slice_width = ENDIAN_SWAP16(resources.cap[group_idx][stage_idx].count.slice_width);
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

  PT_LOG_DEBUG(LOG_CTX_MSG, "Port # %u",           ENDIAN_SWAP8 (msgPhyConf->Port));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Enable        = %u", ENDIAN_SWAP8 (msgPhyConf->PortEnable));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Speed         = %u", ENDIAN_SWAP8 (msgPhyConf->Speed));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Duplex        = %u", ENDIAN_SWAP8 (msgPhyConf->Duplex));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Media         = %u", ENDIAN_SWAP8 (msgPhyConf->Media));
  PT_LOG_DEBUG(LOG_CTX_MSG, " MaxFrame      = %u", ENDIAN_SWAP16(msgPhyConf->MaxFrame));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Loopback      = %u", ENDIAN_SWAP8 (msgPhyConf->LoopBack));
  PT_LOG_DEBUG(LOG_CTX_MSG, " MACLearn Prio = %u", ENDIAN_SWAP8 (msgPhyConf->MacLearning));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Mask          = 0x%04X", ENDIAN_SWAP16(msgPhyConf->Mask));

  /* Copy the message data to a new structure (*/
  phyConf.Port         = ENDIAN_SWAP8 (msgPhyConf->Port);
  phyConf.Mask         = ENDIAN_SWAP16(msgPhyConf->Mask);
  phyConf.PortEnable   = ENDIAN_SWAP8 (msgPhyConf->PortEnable);
  phyConf.Speed        = ENDIAN_SWAP8 (msgPhyConf->Speed);
  phyConf.Duplex       = ENDIAN_SWAP8 (msgPhyConf->Duplex);
  phyConf.Media        = ENDIAN_SWAP8 (msgPhyConf->Media);
  phyConf.MaxFrame     = ENDIAN_SWAP16(msgPhyConf->MaxFrame);
  phyConf.LoopBack     = ENDIAN_SWAP8 (msgPhyConf->LoopBack);

  /* Apply config */
  if ( ptin_intf_PhyConfig_set(&phyConf) != L7_SUCCESS )
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error applying configurations on port# %u", phyConf.Port);

    memset(msgPhyConf, 0x00, sizeof(msg_HWEthPhyConf_t));
    msgPhyConf->Mask = ENDIAN_SWAP16(phyConf.Mask);  /* Restore mask */
    msgPhyConf->Port = ENDIAN_SWAP8 (phyConf.Port);

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

  phyConf.Port = ENDIAN_SWAP8 (msgPhyConf->Port);
  phyConf.Mask = ENDIAN_SWAP16(msgPhyConf->Mask);

  if (ptin_intf_PhyConfig_get(&phyConf) != L7_SUCCESS )
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error getting configurations of port# %u", phyConf.Port);
    memset(msgPhyConf, 0x00, sizeof(msg_HWEthPhyConf_t));

    return L7_FAILURE;
  }

  /* Copy the message data to the msg structure (*/
  msgPhyConf->Port        = ENDIAN_SWAP8 (phyConf.Port);
  msgPhyConf->Mask        = ENDIAN_SWAP16(phyConf.Mask);
  msgPhyConf->PortEnable  = ENDIAN_SWAP8 (phyConf.PortEnable);
  msgPhyConf->Speed       = ENDIAN_SWAP8 (phyConf.Speed);
  msgPhyConf->Duplex      = ENDIAN_SWAP8 (phyConf.Duplex);
  msgPhyConf->Media       = ENDIAN_SWAP8 (phyConf.Media);
  msgPhyConf->MaxFrame    = ENDIAN_SWAP16(phyConf.MaxFrame);
  msgPhyConf->LoopBack    = ENDIAN_SWAP8 (phyConf.LoopBack);

  /* Output info read */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Port # %u",               ENDIAN_SWAP8 (msgPhyConf->Port));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Enable        = %u",     ENDIAN_SWAP8 (msgPhyConf->PortEnable));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Speed         = %u",     ENDIAN_SWAP8 (msgPhyConf->Speed));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Duplex        = %u",     ENDIAN_SWAP8 (msgPhyConf->Duplex));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Media         = %u",     ENDIAN_SWAP8 (msgPhyConf->Media));
  PT_LOG_DEBUG(LOG_CTX_MSG, " MaxFrame      = %u",     ENDIAN_SWAP16(msgPhyConf->MaxFrame));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Loopback      = %u",     ENDIAN_SWAP8 (msgPhyConf->LoopBack));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Mask          = 0x%04X", ENDIAN_SWAP16(msgPhyConf->Mask));

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

  port = ENDIAN_SWAP8(msgPhyState->Port);

  /* Clear structure */
  memset(msgPhyState, 0x00, sizeof(msg_HWEthPhyState_t));
  msgPhyState->Port = ENDIAN_SWAP8(port);

  /* Read some configurations: MaxFrame & Media */
  phyConf.Port = port;
  phyConf.Mask = 0xFFFF;
  if (ptin_intf_PhyConfig_get(&phyConf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error getting configurations of port# %u", phyConf.Port);
    memset(msgPhyState, 0x00, sizeof(ptin_HWEthPhyState_t));

    return L7_FAILURE;
  }

  /* Read some state parameters: Link-Up and AutoNeg-Complete */
  phyState.Port = port;
  phyState.Mask = 0xFFFF;
  if (ptin_intf_PhyState_read(&phyState))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error getting state of port# %u", phyState.Port);
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
    PT_LOG_ERR(LOG_CTX_MSG, "Error getting statistics of port# %u", portStats.Port);
    memset(msgPhyState, 0x00, sizeof(msg_HWEthPhyState_t));

    return L7_FAILURE;
  }

  /* Compose message with all the gathered data */
  msgPhyState->Mask               = ENDIAN_SWAP16(0x1C7F);   /* Do not include TxFault, RemoteFault and LOS */
  msgPhyState->Speed              = ENDIAN_SWAP8 (phyState.Speed);
  msgPhyState->Duplex             = ENDIAN_SWAP8 (phyState.Duplex);
  msgPhyState->LinkUp             = ENDIAN_SWAP8 (phyState.LinkUp);
  msgPhyState->AutoNegComplete    = ENDIAN_SWAP8 (phyState.AutoNegComplete);

  msgPhyState->Collisions         = ENDIAN_SWAP8(portStats.Tx.etherStatsCollisions > 0);
  msgPhyState->RxActivity         = ENDIAN_SWAP8(portStats.Rx.Throughput > 0);
  msgPhyState->TxActivity         = ENDIAN_SWAP8(portStats.Tx.Throughput > 0);

  msgPhyState->Media              = ENDIAN_SWAP8(phyConf.Media);
  msgPhyState->MTU_mismatch       = ENDIAN_SWAP8(phyConf.MaxFrame > PHY_MAX_MAXFRAME);
  msgPhyState->Supported_MaxFrame = ENDIAN_SWAP16(PHY_MAX_MAXFRAME);

//msgPhyState->TxFault            = ENDIAN_SWAP8(0);    /* Always FALSE */
//msgPhyState->RemoteFault        = ENDIAN_SWAP8(0);    /* Always FALSE */
//msgPhyState->LOS                = ENDIAN_SWAP8(0);    /* Always FALSE */

  /* Output info read */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Port # %u",                   ENDIAN_SWAP8(msgPhyState->Port));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Mask             = 0x%04X",  ENDIAN_SWAP16(msgPhyState->Mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Speed            = %u",      ENDIAN_SWAP8(msgPhyState->Speed));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Duplex           = %u",      ENDIAN_SWAP8(msgPhyState->Duplex));
  PT_LOG_DEBUG(LOG_CTX_MSG, " LinkUp           = %s",      ENDIAN_SWAP8(msgPhyState->LinkUp) ? "Yes" : "No");
  PT_LOG_DEBUG(LOG_CTX_MSG, " AutoNegComplete  = %s",      ENDIAN_SWAP8(msgPhyState->AutoNegComplete) ? "Yes" : "No");
  PT_LOG_DEBUG(LOG_CTX_MSG, " Collisions       = %s",      ENDIAN_SWAP8(msgPhyState->Collisions) ? "Yes" : "No");
  PT_LOG_DEBUG(LOG_CTX_MSG, " RxActivity       = %s",      ENDIAN_SWAP8(msgPhyState->RxActivity) ? "Yes" : "No");
  PT_LOG_DEBUG(LOG_CTX_MSG, " TxActivity       = %s",      ENDIAN_SWAP8(msgPhyState->TxActivity) ? "Yes" : "No");
//PT_LOG_DEBUG(LOG_CTX_MSG, " TxFault          = %s",      ENDIAN_SWAP8(msgPhyState->TxFault) ? "Yes" : "No" );
//PT_LOG_DEBUG(LOG_CTX_MSG, " RemoteFault      = %s",      ENDIAN_SWAP8(msgPhyState->RemoteFault) ? "Yes" : "No" );
//PT_LOG_DEBUG(LOG_CTX_MSG, " LOS              = %s",      ENDIAN_SWAP8(msgPhyState->LOS) ? "Yes" : "No" );
  PT_LOG_DEBUG(LOG_CTX_MSG, " Media            = %u",      ENDIAN_SWAP8(msgPhyState->Media) );
  PT_LOG_DEBUG(LOG_CTX_MSG, " MTU_mismatch     = %s",      ENDIAN_SWAP8(msgPhyState->MTU_mismatch) ? "Yes" : "No" );
  PT_LOG_DEBUG(LOG_CTX_MSG, " Support.MaxFrame = %u",      ENDIAN_SWAP16(msgPhyState->Supported_MaxFrame));

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
  if (ptin_intf_slotPort2port(ENDIAN_SWAP8(msgPhyAct->intf.slot), ENDIAN_SWAP8(msgPhyAct->intf.port), &ptin_port) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unknown interface (slot=%u/%u)", ENDIAN_SWAP8(msgPhyAct->intf.slot), ENDIAN_SWAP8(msgPhyAct->intf.port));
    return L7_FAILURE;
  }

  /* Read statistics */
  portStats.Port = ptin_port;
  portStats.Mask = 0xFF;
  portStats.RxMask = 0xFFFFFFFF;
  portStats.TxMask = 0xFFFFFFFF;
  if (ptin_intf_counters_read(&portStats) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error getting statistics of port# %u", portStats.Port);
    return L7_FAILURE;
  }

  /* Compose message with all the gathered data */
  msgPhyAct->Mask = ENDIAN_SWAP8(0xff);
  msgPhyAct->RxActivity = ENDIAN_SWAP32((L7_uint32) portStats.Rx.Throughput);
  msgPhyAct->TxActivity = ENDIAN_SWAP32((L7_uint32) portStats.Tx.Throughput);

  /* Output info read */
  PT_LOG_TRACE(LOG_CTX_MSG, "Slot/Port # %u/%u",           ENDIAN_SWAP8(msgPhyAct->intf.slot), ENDIAN_SWAP8(msgPhyAct->intf.port));
  PT_LOG_TRACE(LOG_CTX_MSG, " Mask             = 0x%02x",  ENDIAN_SWAP8(msgPhyAct->Mask));
  PT_LOG_TRACE(LOG_CTX_MSG, " RX Activity      = %u",      ENDIAN_SWAP32(msgPhyAct->RxActivity));
  PT_LOG_TRACE(LOG_CTX_MSG, " TX Activity      = %u",      ENDIAN_SWAP32(msgPhyAct->TxActivity));

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

  port = ENDIAN_SWAP8(msgPhyStatus->Port);

  /* Clear structure */
  memset(msgPhyStatus, 0x00, sizeof(msg_HWEthPhyStatus_t));

  /* Read some configurations: MaxFrame & Media */
  phyConf.Port = port;
  phyConf.Mask = 0xFFFF;
  if (ptin_intf_PhyConfig_get(&phyConf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error getting configurations of port# %u", phyConf.Port);
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
    PT_LOG_ERR(LOG_CTX_MSG, "Error getting statistics of port# %u", portStats.Port);
    memset(msgPhyStatus, 0x00, sizeof(msg_HWEthPhyStatus_t));

    return L7_FAILURE;
  }

  /* Read some state parameters: Link-Up and AutoNeg-Complete */
  phyState.Port = port;
  phyState.Mask = 0xFFFF;
  if (ptin_intf_PhyState_read(&phyState))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error getting state of port# %u", phyState.Port);
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

  msgPhyStatus->Port              = ENDIAN_SWAP8(port);
  msgPhyStatus->phy.alarmes       = ENDIAN_SWAP32(msgPhyStatus->phy.alarmes);
  msgPhyStatus->phy.alarmes_mask  = ENDIAN_SWAP32(msgPhyStatus->phy.alarmes_mask);

  /* Output info read */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Port # %u",                   ENDIAN_SWAP8 (msgPhyStatus->Port));
  PT_LOG_DEBUG(LOG_CTX_MSG, " alarmes          = 0x%04X",  ENDIAN_SWAP32(msgPhyStatus->phy.alarmes));
  PT_LOG_DEBUG(LOG_CTX_MSG, " alarmes_mask     = 0x%04X",  ENDIAN_SWAP32(msgPhyStatus->phy.alarmes_mask));

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
    port = ENDIAN_SWAP8(p_msgRequest->generic_id);

    /* Clear output structure */
    memset(p_msgPortStats, 0x00, sizeof(msg_HWEthRFC2819_PortStatistics_t));
    /* Update slot_id and port_id */
    p_msgPortStats->SlotId = p_msgRequest->slot_id;
    p_msgPortStats->Port   = ENDIAN_SWAP8(port);

    /* Read statistics */
    portStats.Port = port;
    portStats.Mask = 0xFF;
    portStats.RxMask = 0xFFFFFFFF;
    portStats.TxMask = 0xFFFFFFFF;
    if (ptin_intf_counters_read(&portStats) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error getting statistics of port# %u", portStats.Port);
      memset(p_msgPortStats, 0x00, sizeof(msg_HWEthRFC2819_PortStatistics_t));
      return L7_FAILURE;
    }

    /* Copy data from ptin to msg structure */
    ptin_msg_PortStats_convert(p_msgPortStats, &portStats);

    /* Output info read */
    PT_LOG_TRACE(LOG_CTX_MSG, "Slotid=%u, Port # %2u", p_msgPortStats->SlotId, p_msgPortStats->Port);
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.DropEvents           = %15llu  | Tx.DropEvents           = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsDropEvents),           ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsDropEvents));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.Octets               = %15llu  | Tx.Octets               = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsOctets),               ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsOctets));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.Pkts                 = %15llu  | Tx.Pkts                 = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsPkts),                 ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsPkts));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.BroadcastPkts        = %15llu  | Tx.BroadcastPkts        = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsBroadcastPkts),        ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsBroadcastPkts));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.MulticastPkts        = %15llu  | Tx.MulticastPkts        = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsMulticastPkts),        ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsMulticastPkts));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.CRCAlignErrors       = %15llu  | Tx.CRCAlignErrors       = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsCRCAlignErrors),       ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsCRCAlignErrors));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.UndersizePkts        = %15llu  | Tx.OversizePkts         = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsUndersizePkts),        ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsOversizePkts));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.OversizePkts         = %15llu  | Tx.Fragments            = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsOversizePkts),         ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsFragments));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.Fragments            = %15llu  | Tx.Jabbers              = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsFragments),            ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsJabbers));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.Jabbers              = %15llu  | Tx.Collisions           = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsJabbers),              ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsCollisions));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.Pkts64Octets         = %15llu  | Tx.Pkts64Octets         = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsPkts64Octets),         ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsPkts64Octets));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.Pkts65to127Octets    = %15llu  | Tx.Pkts65to127Octets    = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsPkts65to127Octets),    ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsPkts65to127Octets));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.Pkts128to255Octets   = %15llu  | Tx.Pkts128to255Octets   = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsPkts128to255Octets),   ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsPkts128to255Octets));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.Pkts256to511Octets   = %15llu  | Tx.Pkts256to511Octets   = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsPkts256to511Octets),   ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsPkts256to511Octets));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.Pkts512to1023Octets  = %15llu  | Tx.Pkts512to1023Octets  = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsPkts512to1023Octets),  ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsPkts512to1023Octets));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.Pkts1024to1518Octets = %15llu  | Tx.Pkts1024to1518Octets = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsPkts1024to1518Octets), ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsPkts1024to1518Octets));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.Pkts1519toMaxOctets  = %15llu  | Tx.Pkts1519toMaxOctets  = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.etherStatsPkts1519toMaxOctets),  ENDIAN_SWAP64(p_msgPortStats->Tx.etherStatsPkts1519toMaxOctets));
    PT_LOG_TRACE(LOG_CTX_MSG, " Rx.Throughput (bps)     = %15llu  | Tx.Throughput (bps)     = %15llu", ENDIAN_SWAP64(p_msgPortStats->Rx.Throughput),                     ENDIAN_SWAP64(p_msgPortStats->Tx.Throughput));
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
  /* Clear statistics */
  if (ptin_intf_counters_clear(ENDIAN_SWAP8(msgPortStats->Port)) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error clearing statistics of port# %u", ENDIAN_SWAP8(msgPortStats->Port));
    memset(msgPortStats, 0x00, sizeof(msg_HWEthRFC2819_PortStatistics_t));

    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "Port# %u counters cleared", ENDIAN_SWAP8(msgPortStats->Port));

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
      PT_LOG_ERR(LOG_CTX_MSG, "Error reading info for ptin_port %d", ptin_port);
      return L7_FAILURE;
    }

    intf_info->port[ptin_port].board_id = ENDIAN_SWAP8(board_id);
    intf_info->port[ptin_port].enable   = ENDIAN_SWAP8(admin);
    intf_info->port[ptin_port].link     = ENDIAN_SWAP8(link);

    PT_LOG_TRACE(LOG_CTX_MSG, "port=%u: boardId=%u admin=%u link=%u", ptin_port, board_id, admin, link);
  }
  /* Number of ports */
  intf_info->number_of_ports = ENDIAN_SWAP8(ptin_sys_number_of_ports);

  #ifdef MAP_CPLD
  if (!ptin_fpga_mx_is_matrixactive())
  {
    PT_LOG_ERR(LOG_CTX_MSG, "I am inactive matrix");
    return L7_FAILURE;
  }
  #endif
#endif

  return L7_SUCCESS;
}

/**
 * Process linkStatus messages sent from linecards
 * 
 * @author mruas (10/1/2015)
 * 
 * @param inbuffer 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_intfLinkStatus(ipc_msg *inbuffer)
{
#if (PTIN_BOARD_IS_MATRIX)
  msg_HwIntfStatus_t *linkStatus = (msg_HwIntfStatus_t *) inbuffer->info;
  struct_linkStatus_t info;
  L7_uint16 index;
  L7_uint32 ptin_port;

  ENDIAN_SWAP8_MOD (linkStatus->slot_id);
  ENDIAN_SWAP16_MOD(linkStatus->generic_id);
  ENDIAN_SWAP8_MOD (linkStatus->number_of_ports);

  /* Validate slot */
  if (linkStatus->slot_id < PTIN_SYS_LC_SLOT_MIN || linkStatus->slot_id > PTIN_SYS_LC_SLOT_MAX)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid slot %u", linkStatus->slot_id);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_CONTROL, "Received linkStatus report from slot %u", linkStatus->slot_id);

  /* Run all given interfaces */
  for (index = 0; index < linkStatus->number_of_ports; index++)
  {
    ENDIAN_SWAP8_MOD (linkStatus->port[index].intf.intf_type);
    ENDIAN_SWAP8_MOD (linkStatus->port[index].intf.intf_id);
    ENDIAN_SWAP8_MOD (linkStatus->port[index].enable);
    ENDIAN_SWAP8_MOD (linkStatus->port[index].link);
    ENDIAN_SWAP64_MOD(linkStatus->port[index].tx_packets);
    ENDIAN_SWAP64_MOD(linkStatus->port[index].rx_packets);
    ENDIAN_SWAP64_MOD(linkStatus->port[index].rx_error);

    /* Convert to matrix's local ptin_port */
    if (ptin_intf_slotPort2port(linkStatus->slot_id, index, &ptin_port)!=L7_SUCCESS)
    {
      //PT_LOG_ERR(LOG_CTX_MSG, "Invalid slot=%u/port=%u", linkStatus->slot_id, index);
      continue;
    }

    PT_LOG_TRACE(LOG_CTX_CONTROL, "Updating linkStatus report of port %u", ptin_port);

    /* Update remote linkStatus */
    memset(&info, 0x00, sizeof(info)); 
    info.updated    = L7_TRUE;
    info.enable     = linkStatus->port[index].enable;
    info.link       = linkStatus->port[index].link;
    info.tx_packets = linkStatus->port[index].tx_packets;
    info.rx_packets = linkStatus->port[index].rx_packets;
    info.rx_error   = linkStatus->port[index].rx_error;

    ptin_control_remoteLinkstatus_update(ptin_port, &info);
  }

  PT_LOG_TRACE(LOG_CTX_CONTROL, "linkStatus from slot %u updated!", linkStatus->slot_id);

  return L7_SUCCESS;
#else
  PT_LOG_ERR(LOG_CTX_MSG, "This message is only supported by MATRIX board");
  return L7_NOT_SUPPORTED;
#endif
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
    PT_LOG_ERR(LOG_CTX_MSG, "Error obtaining list of slot modes");
    return L7_FAILURE;
  }

  /* Run all slots */
  for (i=0; i<MSG_SLOTMODECFG_NSLOTS && i<PTIN_SYS_SLOTS_MAX; i++)
  {
    slotMode->slot_list[i].slot_index  = ENDIAN_SWAP8(i+1);
    slotMode->slot_list[i].slot_config = ENDIAN_SWAP8(1);
    slotMode->slot_list[i].slot_mode   = ENDIAN_SWAP8(slot_list[i]);
  }

  PT_LOG_DEBUG(LOG_CTX_MSG,"Slot mode list:");
  for (i=0; i<PTIN_SYS_SLOTS_MAX; i++)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG,"Slot %02u: Mode=%u", i+1, slot_list[i]);
  }
  PT_LOG_INFO(LOG_CTX_MSG,"Success");

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
  PT_LOG_DEBUG(LOG_CTX_MSG,"Slot mode list:");
  for (i=0; i<MSG_SLOTMODECFG_NSLOTS; i++)
  {
    ENDIAN_SWAP8_MOD(slotMode->slot_list[i].slot_index);
    ENDIAN_SWAP8_MOD(slotMode->slot_list[i].slot_config);
    ENDIAN_SWAP8_MOD(slotMode->slot_list[i].slot_mode);

    PT_LOG_DEBUG(LOG_CTX_MSG,"Idx%02u: Slot %02u Active=%u Mode=%u", i,
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

  PT_LOG_DEBUG(LOG_CTX_MSG,"Slot mode list to be validated:");
  for (i=0; i<PTIN_SYS_SLOTS_MAX; i++)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG,"Slot %02u: Mode=%u", i+1, slot_list[i]);
  }

  /* Validate list of slot modes */
  if ((rc=ptin_intf_slotMode_validate(slot_list)) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Slot mode list is not valid! (rc=%d)",rc);
    return rc;
  }
  PT_LOG_INFO(LOG_CTX_MSG, "Slot mode list is valid!");

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
  PT_LOG_INFO(LOG_CTX_MSG,"Success");

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
    portExt_conf.Mask                          = ENDIAN_SWAP32(portExt[i].Mask);
    portExt_conf.defVid                        = ENDIAN_SWAP16(portExt[i].defVid);
    portExt_conf.defPrio                       = ENDIAN_SWAP8 (portExt[i].defPrio);
    portExt_conf.acceptable_frame_types        = ENDIAN_SWAP8 (portExt[i].acceptable_frame_types);
    portExt_conf.ingress_filter                = ENDIAN_SWAP8 (portExt[i].ingress_filter);
    portExt_conf.restricted_vlan_reg           = ENDIAN_SWAP8 (portExt[i].restricted_vlan_reg);
    portExt_conf.vlan_aware                    = ENDIAN_SWAP8 (portExt[i].vlan_aware);
    portExt_conf.type                          = ENDIAN_SWAP8 (portExt[i].type);
    portExt_conf.doubletag                     = ENDIAN_SWAP8 (portExt[i].doubletag);
    portExt_conf.inner_tpid                    = ENDIAN_SWAP16(portExt[i].inner_tpid);
    portExt_conf.outer_tpid                    = ENDIAN_SWAP16(portExt[i].outer_tpid);
    portExt_conf.egress_type                   = ENDIAN_SWAP8 (portExt[i].egress_type);
    portExt_conf.macLearn_enable               = ENDIAN_SWAP8 (portExt[i].macLearn_enable);
    portExt_conf.macLearn_stationMove_enable   = ENDIAN_SWAP8 (portExt[i].macLearn_stationMove_enable);
    portExt_conf.macLearn_stationMove_prio     = ENDIAN_SWAP8 (portExt[i].macLearn_stationMove_prio);
    portExt_conf.macLearn_stationMove_samePrio = ENDIAN_SWAP8 (portExt[i].macLearn_stationMove_samePrio);
    portExt_conf.maxChannels                   = ENDIAN_SWAP16(portExt[i].maxChannels);
    portExt_conf.maxBandwidth                  = ENDIAN_SWAP64(portExt[i].maxBandwidth);
    portExt_conf.dhcp_trusted                  = ENDIAN_SWAP8 (portExt[i].protocol_trusted);

    ptin_intf.intf_type = ENDIAN_SWAP8(portExt[i].intf.intf_type);
    ptin_intf.intf_id   = ENDIAN_SWAP8(portExt[i].intf.intf_id);

    /* Set MEF parameters */
    if (ptin_intf_portExt_set(&ptin_intf, &portExt_conf)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error setting MEF EXT configurations");
      return L7_FAILURE;
    }
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "Success setting MEF EXT configurations",__FUNCTION__);

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

  slotId              = ENDIAN_SWAP8(portExt->SlotId);
  ptin_intf.intf_type = ENDIAN_SWAP8(portExt->intf.intf_type);
  ptin_intf.intf_id   = ENDIAN_SWAP8(portExt->intf.intf_id);

  if (ptin_intf.intf_type==0xff)
  {
    port_start = 0;
    port_end   = PTIN_SYSTEM_N_INTERF-1;
  }
  else if (ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL && ptin_intf.intf_id == 0xff)
  {
    port_start = 0;
    port_end   = PTIN_SYSTEM_N_PORTS-1;
  }
  else if (ptin_intf.intf_type == PTIN_EVC_INTF_LOGICAL && ptin_intf.intf_id == 0xff)
  {
    port_start = PTIN_SYSTEM_N_PORTS;
    port_end   = PTIN_SYSTEM_N_INTERF-1;
  }
  else if (ptin_intf_ptintf2port(&ptin_intf, &port) == L7_SUCCESS)
  {
    port_start = port;
    port_end   = port;
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading interface %u/%u", ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  }

  index = 0;
  for (port = port_start; port <= port_end; port++)
  {
    if (ptin_intf_port2ptintf(port, &ptin_intf)!=L7_SUCCESS)
    {
      PT_LOG_WARN(LOG_CTX_MSG,"Error reading port %u", port);
      continue;
    }

    memset(&portExt_conf, 0x00, sizeof(ptin_HWPortExt_t));

    /* Get MEF parameters */
    if (ptin_intf_portExt_get(&ptin_intf, &portExt_conf)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error getting MEF EXT parameters for interface=%u/%u", ptin_intf.intf_type, ptin_intf.intf_id);
      return L7_FAILURE;
    }

    /* Copy values to output */
    portExt[index].SlotId                        = ENDIAN_SWAP8 (slotId);
    portExt[index].intf.intf_type                = ENDIAN_SWAP8 (ptin_intf.intf_type);
    portExt[index].intf.intf_id                  = ENDIAN_SWAP8 (ptin_intf.intf_id);
    portExt[index].Mask                          = ENDIAN_SWAP32(portExt_conf.Mask);
    portExt[index].defVid                        = ENDIAN_SWAP16(portExt_conf.defVid);
    portExt[index].defPrio                       = ENDIAN_SWAP8 (portExt_conf.defPrio);
    portExt[index].acceptable_frame_types        = ENDIAN_SWAP8 (portExt_conf.acceptable_frame_types);
    portExt[index].ingress_filter                = ENDIAN_SWAP8 (portExt_conf.ingress_filter);
    portExt[index].restricted_vlan_reg           = ENDIAN_SWAP8 (portExt_conf.restricted_vlan_reg);
    portExt[index].vlan_aware                    = ENDIAN_SWAP8 (portExt_conf.vlan_aware);
    portExt[index].type                          = ENDIAN_SWAP8 (portExt_conf.type);
    portExt[index].doubletag                     = ENDIAN_SWAP8 (portExt_conf.doubletag);
    portExt[index].inner_tpid                    = ENDIAN_SWAP16(portExt_conf.inner_tpid);
    portExt[index].outer_tpid                    = ENDIAN_SWAP16(portExt_conf.outer_tpid);
    portExt[index].egress_type                   = ENDIAN_SWAP8 (portExt_conf.egress_type);
    portExt[index].macLearn_enable               = ENDIAN_SWAP8 (portExt_conf.macLearn_enable);
    portExt[index].macLearn_stationMove_enable   = ENDIAN_SWAP8 (portExt_conf.macLearn_stationMove_enable);
    portExt[index].macLearn_stationMove_prio     = ENDIAN_SWAP8 (portExt_conf.macLearn_stationMove_prio);
    portExt[index].macLearn_stationMove_samePrio = ENDIAN_SWAP8 (portExt_conf.macLearn_stationMove_samePrio);
    portExt[index].maxChannels                   = ENDIAN_SWAP16(portExt_conf.maxChannels);
    portExt[index].maxBandwidth                  = ENDIAN_SWAP64(portExt_conf.maxBandwidth);
    portExt[index].protocol_trusted              = ENDIAN_SWAP8 (portExt_conf.dhcp_trusted);

    index++;
  }

  if (nElems!=L7_NULLPTR)  *nElems = index;

  PT_LOG_DEBUG(LOG_CTX_MSG, "Success reading MEF EXT parameters for %u interfaces",index);

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

  PT_LOG_DEBUG(LOG_CTX_MSG,"Processing %u structures",nElems);

  for (i=0; i<nElems; i++)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG,"Structure %u",i);
    PT_LOG_DEBUG(LOG_CTX_MSG," SlotId = %u", ENDIAN_SWAP8(portMac[i].SlotId));
    PT_LOG_DEBUG(LOG_CTX_MSG," Intf   = %u/%u", ENDIAN_SWAP8(portMac[i].intf.intf_type), ENDIAN_SWAP8(portMac[i].intf.intf_id));
    PT_LOG_DEBUG(LOG_CTX_MSG," Mask   = 0x%04x", ENDIAN_SWAP16(portMac[i].Mask));
    PT_LOG_DEBUG(LOG_CTX_MSG," MAC    = %02x:%02x:%02x:%02x:%02x:%02x",
                 portMac[i].macAddr[0], portMac[i].macAddr[1], portMac[i].macAddr[2], portMac[i].macAddr[3], portMac[i].macAddr[4], portMac[i].macAddr[5]);

    memset(&portMac_conf,0x00,sizeof(ptin_HWPortMac_t));
    portMac_conf.Mask = ENDIAN_SWAP16(portMac[i].Mask);
    memcpy(portMac_conf.macAddr, portMac[i].macAddr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);

    ptin_intf.intf_type = ENDIAN_SWAP8(portMac[i].intf.intf_type);
    ptin_intf.intf_id   = ENDIAN_SWAP8(portMac[i].intf.intf_id);

    /* Set MEF parameters */
    if (ptin_intf_portMAC_set(&ptin_intf, &portMac_conf)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error setting MAC address");
      return L7_FAILURE;
    }
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "Success setting MAC address",__FUNCTION__);

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

  slotId              = ENDIAN_SWAP8(portMac->SlotId);
  ptin_intf.intf_type = ENDIAN_SWAP8(portMac->intf.intf_type);
  ptin_intf.intf_id   = ENDIAN_SWAP8(portMac->intf.intf_id);

  if (ptin_intf.intf_type == 0xff)
  {
    port_start = 0;
    port_end   = PTIN_SYSTEM_N_INTERF-1;
  }
  else if (ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL && ptin_intf.intf_id == 0xff)
  {
    port_start = 0;
    port_end   = PTIN_SYSTEM_N_PORTS-1;
  }
  else if (ptin_intf.intf_type == PTIN_EVC_INTF_LOGICAL && ptin_intf.intf_id == 0xff)
  {
    port_start = PTIN_SYSTEM_N_PORTS;
    port_end   = PTIN_SYSTEM_N_INTERF-1;
  }
  else if (ptin_intf_ptintf2port(&ptin_intf, &port)==L7_SUCCESS)
  {
    port_start = port;
    port_end   = port;
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading interface %u/%u", ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG,"Going to read %u structures", port_end-port_start+1);

  index = 0;
  for (port = port_start; port <= port_end; port++)
  {
    if (ptin_intf_port2ptintf(port, &ptin_intf)!=L7_SUCCESS)
    {
      PT_LOG_WARN(LOG_CTX_MSG,"Error reading port %u", port);
      continue;
    }

    memset(&portMac_conf, 0x00, sizeof(ptin_HWPortMac_t));
    portMac_conf.Mask = PTIN_HWPORTMAC_MASK_MACADDR;

    /* Get MEF parameters */
    if (ptin_intf_portMAC_get(&ptin_intf, &portMac_conf)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error getting MAC address for interface=%u/%u", ptin_intf.intf_type, ptin_intf.intf_id);
      return L7_FAILURE;
    }

    /* Copy values to output */
    portMac[index].SlotId                      = ENDIAN_SWAP8 (slotId);
    portMac[index].intf.intf_type              = ENDIAN_SWAP8 (ptin_intf.intf_type);
    portMac[index].intf.intf_id                = ENDIAN_SWAP8 (ptin_intf.intf_id);
    portMac[index].Mask                        = ENDIAN_SWAP16(portMac_conf.Mask);
    memcpy(portMac[index].macAddr, portMac_conf.macAddr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);

    PT_LOG_DEBUG(LOG_CTX_MSG,"Structure %u", index);
    PT_LOG_DEBUG(LOG_CTX_MSG," SlotId = %u",    ENDIAN_SWAP8(portMac[index].SlotId));
    PT_LOG_DEBUG(LOG_CTX_MSG," Intf   = %u/%u", ENDIAN_SWAP8(portMac[index].intf.intf_type), ENDIAN_SWAP8(portMac[index].intf.intf_id));
    PT_LOG_DEBUG(LOG_CTX_MSG," Mask   = 0x%04x",ENDIAN_SWAP16(portMac[index].Mask));
    PT_LOG_DEBUG(LOG_CTX_MSG," MAC    = %02x:%02x:%02x:%02x:%02x:%02x",
                 ENDIAN_SWAP8(portMac[index].macAddr[0]),
                 ENDIAN_SWAP8(portMac[index].macAddr[1]),
                 ENDIAN_SWAP8(portMac[index].macAddr[2]),
                 ENDIAN_SWAP8(portMac[index].macAddr[3]),
                 ENDIAN_SWAP8(portMac[index].macAddr[4]),
                 ENDIAN_SWAP8(portMac[index].macAddr[5]));

    index++;
  }

  if (nElems!=L7_NULLPTR)  *nElems = index;

  PT_LOG_DEBUG(LOG_CTX_MSG, "Success reading MEF EXT parameters for %u interfaces", index);

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
  L7_RC_t rc;

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
  rc = ptin_QoS_intf_config_get(&ptin_intf,&qos_intf);
  if (rc == L7_SUCCESS)
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
        if (qos_intf.pktprio.mask[i] != 0)
        {
          qos_msg->pktprio.mask  |= (L7_uint8) 1<<i;
          qos_msg->pktprio.cos[i] = qos_intf.pktprio.cos[i];

          qos_msg->mask |= MSG_QOS_CONFIGURATION_PACKETPRIO_MASK;
        }
      }
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading interface QoS configuration");
    return rc;
  }

  /* QoS configuration */
  rc = ptin_QoS_cos_config_get(&ptin_intf,(L7_uint8)-1,qos_cos);
  if (rc == L7_SUCCESS)
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
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading QoS configuration for all 8 CoS");
    return rc;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "Slotid         = %u",qos_msg->SlotId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Interface      = %u/%u",qos_msg->intf.intf_type,qos_msg->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "General Mask   = 0x%02X",qos_msg->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Trust mode     = %u",qos_msg->trust_mode);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Bandwidth unit = %u",qos_msg->bandwidth_unit);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Shaping rate   = %u",qos_msg->shaping_rate);
  PT_LOG_DEBUG(LOG_CTX_MSG, "pktprio.cos    = [ %Xh %Xh %Xh %Xh %Xh %Xh %Xh %Xh ]",
            qos_msg->pktprio.cos[0],qos_msg->pktprio.cos[1],qos_msg->pktprio.cos[2],qos_msg->pktprio.cos[3],qos_msg->pktprio.cos[4],qos_msg->pktprio.cos[5],qos_msg->pktprio.cos[6],qos_msg->pktprio.cos[7]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Priority Mask  = 0x%02X",qos_msg->pktprio.mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "CoS Mask       = 0x%02X",qos_msg->cos_config.mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "CoS Spec. Mask = [ 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X ]",
            qos_msg->cos_config.cos[0].mask,qos_msg->cos_config.cos[1].mask,qos_msg->cos_config.cos[2].mask,qos_msg->cos_config.cos[3].mask,qos_msg->cos_config.cos[4].mask,qos_msg->cos_config.cos[5].mask,qos_msg->cos_config.cos[6].mask,qos_msg->cos_config.cos[7].mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Queue_scheduler = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].scheduler, qos_msg->cos_config.cos[1].scheduler, qos_msg->cos_config.cos[2].scheduler, qos_msg->cos_config.cos[3].scheduler, qos_msg->cos_config.cos[4].scheduler, qos_msg->cos_config.cos[5].scheduler, qos_msg->cos_config.cos[6].scheduler, qos_msg->cos_config.cos[7].scheduler);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Queue_min_bw    = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].min_bandwidth, qos_msg->cos_config.cos[1].min_bandwidth, qos_msg->cos_config.cos[2].min_bandwidth, qos_msg->cos_config.cos[3].min_bandwidth, qos_msg->cos_config.cos[4].min_bandwidth, qos_msg->cos_config.cos[5].min_bandwidth, qos_msg->cos_config.cos[6].min_bandwidth, qos_msg->cos_config.cos[7].min_bandwidth);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Queue_max_bw    = [ %u %u %u %u %u %u %u %u ]",
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
  L7_RC_t                 rc, rc_global = L7_SUCCESS;

  PT_LOG_DEBUG(LOG_CTX_MSG, "Slotid         = %u",qos_msg->SlotId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Interface      = %u/%u",qos_msg->intf.intf_type,qos_msg->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "General Mask   = 0x%02X",qos_msg->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Trust mode     = %u",qos_msg->trust_mode);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Bandwidth unit = %u",qos_msg->bandwidth_unit);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Shaping rate   = %u",qos_msg->shaping_rate);
  PT_LOG_DEBUG(LOG_CTX_MSG, "pktprio.cos    = [ %Xh %Xh %Xh %Xh %Xh %Xh %Xh %Xh ]",
            qos_msg->pktprio.cos[0],qos_msg->pktprio.cos[1],qos_msg->pktprio.cos[2],qos_msg->pktprio.cos[3],qos_msg->pktprio.cos[4],qos_msg->pktprio.cos[5],qos_msg->pktprio.cos[6],qos_msg->pktprio.cos[7]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Priority Mask  = 0x%02X",qos_msg->pktprio.mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "CoS Mask       = 0x%02X",qos_msg->cos_config.mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "CoS Spec. Mask = [ 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X ]",
            qos_msg->cos_config.cos[0].mask,qos_msg->cos_config.cos[1].mask,qos_msg->cos_config.cos[2].mask,qos_msg->cos_config.cos[3].mask,qos_msg->cos_config.cos[4].mask,qos_msg->cos_config.cos[5].mask,qos_msg->cos_config.cos[6].mask,qos_msg->cos_config.cos[7].mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Queue_scheduler = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].scheduler, qos_msg->cos_config.cos[1].scheduler, qos_msg->cos_config.cos[2].scheduler, qos_msg->cos_config.cos[3].scheduler, qos_msg->cos_config.cos[4].scheduler, qos_msg->cos_config.cos[5].scheduler, qos_msg->cos_config.cos[6].scheduler, qos_msg->cos_config.cos[7].scheduler);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Queue_min_bw    = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].min_bandwidth, qos_msg->cos_config.cos[1].min_bandwidth, qos_msg->cos_config.cos[2].min_bandwidth, qos_msg->cos_config.cos[3].min_bandwidth, qos_msg->cos_config.cos[4].min_bandwidth, qos_msg->cos_config.cos[5].min_bandwidth, qos_msg->cos_config.cos[6].min_bandwidth, qos_msg->cos_config.cos[7].min_bandwidth);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Queue_max_bw    = [ %u %u %u %u %u %u %u %u ]",
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
        qos_intf.pktprio.mask[i] = 0xff;
        qos_intf.pktprio.cos[i]  = qos_msg->pktprio.cos[i];

        qos_intf.mask |= PTIN_QOS_INTF_PACKETPRIO_MASK;
      }
    }
  }

  /* Is there any configuration to be applied? */
  if (qos_intf.mask)
  {
    /* Execute priority map configuration */
    rc = ptin_QoS_intf_config_set(&ptin_intf,&qos_intf);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error configuring priority map (rc=%d)", rc);
      rc_global = rc;
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
      rc = ptin_QoS_cos_config_set(&ptin_intf, (L7_uint8)-1, qos_cos);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Error configuring QoS (rc=%d)",rc);
        rc_global = rc;
      }
    }
  }

  if (rc_global == L7_SUCCESS)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG,"Success applying QoS configurations to all CoS");
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error applying QoS configurations to all CoS (rc_global=%d)", rc_global);
  }

  return rc_global;
}

/**
 * Get CoS configuration
 * 
 * @param qos_config : CoS configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_CoS2_get(msg_QoSConfiguration2_t *qos_msg)
{
  L7_int                  i, j;
  L7_uint8                slot_id;
  ptin_intf_t             ptin_intf;
  ptin_QoS_intf_t         qos_intf;
  ptin_QoS_cos_t          qos_cos[8];
  ptin_QoS_drop_t         qos_drop[8];
  L7_RC_t                 rc;

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
  memset(&qos_intf,0x00,sizeof(qos_intf));
  memset(qos_cos,0x00,sizeof(qos_cos));
  memset(qos_drop,0x00,sizeof(qos_drop));

  /* Get Interface configuration */
  rc = ptin_QoS_intf_config_get(&ptin_intf,&qos_intf);
  if (rc == L7_SUCCESS)
  {
    /* Trust mode */
    if (qos_intf.mask & PTIN_QOS_INTF_TRUSTMODE_MASK)
    {
      qos_msg->trust_mode = qos_intf.trust_mode;
      qos_msg->generic_mask |= MSG_QOS_CONFIGURATION_TRUSTMODE_MASK;
    }
    /* Bandwidth units */
    if (qos_intf.mask & PTIN_QOS_INTF_BANDWIDTHUNIT_MASK)
    {
      qos_msg->bandwidth_unit = qos_intf.bandwidth_unit;
      qos_msg->generic_mask |= MSG_QOS_CONFIGURATION_BANDWIDTHUNIT_MASK;
    }
    /* Shaping rate */
    if (qos_intf.mask & PTIN_QOS_INTF_SHAPINGRATE_MASK)
    {
      qos_msg->shaping_rate = qos_intf.shaping_rate;
      qos_msg->generic_mask |= MSG_QOS_CONFIGURATION_SHAPINGRATE_MASK;
    }
    /* Priority map to CoS */
    if (qos_intf.mask & PTIN_QOS_INTF_PACKETPRIO_MASK)
    {
      /* Run all priorities */
      for (i=0; i<8; i++)
      {
        /* Is priority i to be defined? If so define CoS */
        if (qos_intf.pktprio.mask[i] != 0)
        {
          qos_msg->pktprio.prio_mask[i] = qos_intf.pktprio.mask[i];
          qos_msg->pktprio.cos[i]       = qos_intf.pktprio.cos[i];

          qos_msg->generic_mask |= MSG_QOS_CONFIGURATION_PACKETPRIO_MASK;
        }
      }
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading interface QoS configuration");
    return rc;
  }

  /* QoS configuration */
  rc = ptin_QoS_cos_config_get(&ptin_intf,(L7_uint8)-1,qos_cos);
  if (rc == L7_SUCCESS)
  {
    /* Run all QoS */
    for (i=0; i<8; i++)
    {
      /* QoS i is to be configured? */
      if (qos_cos[i].mask==0)  continue;

      qos_msg->cos_config.cos_mask |= (L7_uint8) 1<<i;

      /* Scheduler type */
      if (qos_cos[i].mask & PTIN_QOS_COS_SCHEDULER_MASK)
      {
        qos_msg->cos_config.cos[i].scheduler = qos_cos[i].scheduler_type;
        qos_msg->cos_config.cos[i].local_mask |= MSG_QOS_CONFIGURATION_QOSCONF_SCHEDULER_MASK;
      }
      /* Minimum mandwidth */
      if (qos_cos[i].mask & PTIN_QOS_COS_BW_MIN_MASK)
      {
        qos_msg->cos_config.cos[i].min_bandwidth = qos_cos[i].min_bandwidth;
        qos_msg->cos_config.cos[i].local_mask |= MSG_QOS_CONFIGURATION_QOSCONF_BW_MIN_MASK;
      }
      /* Maximum bandwidth */
      if (qos_cos[i].mask & PTIN_QOS_COS_BW_MAX_MASK)
      {
        qos_msg->cos_config.cos[i].max_bandwidth = qos_cos[i].max_bandwidth;
        qos_msg->cos_config.cos[i].local_mask |= MSG_QOS_CONFIGURATION_QOSCONF_BW_MAX_MASK;
      }
      /* WRR Weights */
      if (qos_cos[i].mask & PTIN_QOS_COS_WRR_WEIGHT_MASK)
      {
        qos_msg->cos_config.cos[i].wrrSched_weight = qos_cos[i].wrrSched_weight;
        qos_msg->cos_config.cos[i].local_mask |= MSG_QOS_CONFIGURATION_QOSCONF_WRR_WEIGHT_MASK;
      }
    }
    if (qos_msg->cos_config.cos_mask)
    {
      qos_msg->generic_mask |= MSG_QOS_CONFIGURATION_QOSCONF_MASK;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading QoS configuration for all 8 CoS");
    return rc;
  }

  /* QoS drop configuration */
  rc = ptin_QoS_drop_config_get(&ptin_intf, (L7_uint8)-1, qos_drop);
  if (rc == L7_SUCCESS)
  {
    /* Run all QoS */
    for (i=0; i<8; i++)
    {
      /* QoS i is to be configured? */
      if (qos_cos[i].mask==0)  continue;

      qos_msg->cos_config.cos_mask |= (L7_uint8) 1<<i;

      /* Management type */
      if (qos_drop[i].mask & PTIN_QOS_COS_QUEUE_MANGM_MASK)
      {
        qos_msg->cos_config.cos[i].dropMgmtType = qos_drop[i].queue_management_type;
        qos_msg->cos_config.cos[i].local_mask |= MSG_QOS_CONFIGURATION_QOSCONF_MGMT_TYPE_MASK;
      }
      /* WRED decay exponent */
      if (qos_drop[i].mask & PTIN_QOS_COS_WRED_DECAY_EXP_MASK)
      {
        qos_msg->cos_config.cos[i].wred_decayExp = qos_drop[i].wred_decayExp;
        qos_msg->cos_config.cos[i].local_mask |= MSG_QOS_CONFIGURATION_QOSCONF_WRED_DECAYEXP_MASK;
      }
      /* Run all DP levels */
      for (j = 0; j < 4; j++)
      {
        if (qos_drop[i].dp[j].local_mask == 0)  continue;

        /* Taildrop threshold */
        if (qos_drop[i].dp[j].local_mask & PTIN_QOS_COS_DP_TAILDROP_THRESH_MASK)
        {
          qos_msg->cos_config.cos[i].dropThresholds[j].tailDrop_threshold = qos_drop[i].dp[j].taildrop_threshold;
          qos_msg->cos_config.cos[i].dropThresholds[j].local2_mask |= MSG_QOS_CONFIGURATION_QOSCONF_DROP_TAILDROP_THRES_MASK;
        }

        /* WRED min threshold */
        if (qos_drop[i].dp[j].local_mask & PTIN_QOS_COS_DP_WRED_THRESH_MIN_MASK)
        {
          qos_msg->cos_config.cos[i].dropThresholds[j].wred_minThreshold = qos_drop[i].dp[j].wred_min_threshold;
          qos_msg->cos_config.cos[i].dropThresholds[j].local2_mask |= MSG_QOS_CONFIGURATION_QOSCONF_DROP_WRED_MINTHRES_MASK;
        }

        /* WRED max threshold */
        if (qos_drop[i].dp[j].local_mask & PTIN_QOS_COS_DP_WRED_THRESH_MAX_MASK)
        {
          qos_msg->cos_config.cos[i].dropThresholds[j].wred_maxThreshold = qos_drop[i].dp[j].wred_max_threshold;
          qos_msg->cos_config.cos[i].dropThresholds[j].local2_mask |= MSG_QOS_CONFIGURATION_QOSCONF_DROP_WRED_MAXTHRES_MASK;
        }

        /* WRED drop probability */
        if (qos_drop[i].dp[j].local_mask & PTIN_QOS_COS_DP_WRED_DROP_PROB_MASK)
        {
          qos_msg->cos_config.cos[i].dropThresholds[j].wred_dropProb = qos_drop[i].dp[j].wred_drop_prob;
          qos_msg->cos_config.cos[i].dropThresholds[j].local2_mask |= MSG_QOS_CONFIGURATION_QOSCONF_DROP_WRED_DROPPROB_MASK;
        }
      }
    }
    if (qos_msg->cos_config.cos_mask)
    {
      qos_msg->generic_mask |= MSG_QOS_CONFIGURATION_QOSCONF_MASK;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading QoS drop configuration for all 8 CoS");
    return rc;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "Slotid            = %u",qos_msg->SlotId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Interface         = %u/%u",qos_msg->intf.intf_type,qos_msg->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "General Mask      = 0x%02X",qos_msg->generic_mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Trust mode        = %u",qos_msg->trust_mode);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Bandwidth unit    = %u",qos_msg->bandwidth_unit);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Shaping rate      = %u",qos_msg->shaping_rate);
  PT_LOG_DEBUG(LOG_CTX_MSG, "pktprio.prio_mask = [ 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]",
            qos_msg->pktprio.prio_mask[0],qos_msg->pktprio.prio_mask[1],qos_msg->pktprio.prio_mask[2],qos_msg->pktprio.prio_mask[3],qos_msg->pktprio.prio_mask[4],qos_msg->pktprio.prio_mask[5],qos_msg->pktprio.prio_mask[6],qos_msg->pktprio.prio_mask[7]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "pktprio.cos       = [ %Xh %Xh %Xh %Xh %Xh %Xh %Xh %Xh ]",
            qos_msg->pktprio.cos[0],qos_msg->pktprio.cos[1],qos_msg->pktprio.cos[2],qos_msg->pktprio.cos[3],qos_msg->pktprio.cos[4],qos_msg->pktprio.cos[5],qos_msg->pktprio.cos[6],qos_msg->pktprio.cos[7]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "CoS Mask       = 0x%02X",qos_msg->cos_config.cos_mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "CoS local Mask = [ 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X ]",
            qos_msg->cos_config.cos[0].local_mask, qos_msg->cos_config.cos[1].local_mask, qos_msg->cos_config.cos[2].local_mask, qos_msg->cos_config.cos[3].local_mask, qos_msg->cos_config.cos[4].local_mask, qos_msg->cos_config.cos[5].local_mask, qos_msg->cos_config.cos[6].local_mask, qos_msg->cos_config.cos[7].local_mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Queue_scheduler = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].scheduler, qos_msg->cos_config.cos[1].scheduler, qos_msg->cos_config.cos[2].scheduler, qos_msg->cos_config.cos[3].scheduler, qos_msg->cos_config.cos[4].scheduler, qos_msg->cos_config.cos[5].scheduler, qos_msg->cos_config.cos[6].scheduler, qos_msg->cos_config.cos[7].scheduler);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Queue_min_bw    = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].min_bandwidth, qos_msg->cos_config.cos[1].min_bandwidth, qos_msg->cos_config.cos[2].min_bandwidth, qos_msg->cos_config.cos[3].min_bandwidth, qos_msg->cos_config.cos[4].min_bandwidth, qos_msg->cos_config.cos[5].min_bandwidth, qos_msg->cos_config.cos[6].min_bandwidth, qos_msg->cos_config.cos[7].min_bandwidth);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Queue_max_bw    = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].max_bandwidth, qos_msg->cos_config.cos[1].max_bandwidth, qos_msg->cos_config.cos[2].max_bandwidth, qos_msg->cos_config.cos[3].max_bandwidth, qos_msg->cos_config.cos[4].max_bandwidth, qos_msg->cos_config.cos[5].max_bandwidth, qos_msg->cos_config.cos[6].max_bandwidth, qos_msg->cos_config.cos[7].max_bandwidth);
  PT_LOG_DEBUG(LOG_CTX_MSG, "wrrSched_weight = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].wrrSched_weight, qos_msg->cos_config.cos[1].wrrSched_weight, qos_msg->cos_config.cos[2].wrrSched_weight, qos_msg->cos_config.cos[3].wrrSched_weight, qos_msg->cos_config.cos[4].wrrSched_weight, qos_msg->cos_config.cos[5].wrrSched_weight, qos_msg->cos_config.cos[6].wrrSched_weight, qos_msg->cos_config.cos[7].wrrSched_weight);
  PT_LOG_DEBUG(LOG_CTX_MSG, "dropMgmttype    = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].dropMgmtType, qos_msg->cos_config.cos[1].dropMgmtType, qos_msg->cos_config.cos[2].dropMgmtType, qos_msg->cos_config.cos[3].dropMgmtType, qos_msg->cos_config.cos[4].dropMgmtType, qos_msg->cos_config.cos[5].dropMgmtType, qos_msg->cos_config.cos[6].dropMgmtType, qos_msg->cos_config.cos[7].dropMgmtType);
  PT_LOG_DEBUG(LOG_CTX_MSG, "wred_decayExp   = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].wred_decayExp, qos_msg->cos_config.cos[1].wred_decayExp, qos_msg->cos_config.cos[2].wred_decayExp, qos_msg->cos_config.cos[3].wred_decayExp, qos_msg->cos_config.cos[4].wred_decayExp, qos_msg->cos_config.cos[5].wred_decayExp, qos_msg->cos_config.cos[6].wred_decayExp, qos_msg->cos_config.cos[7].wred_decayExp);
  for (i=0; i<4; i++)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "DP%u: local_mask = [ 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X ]", i+1,
              qos_msg->cos_config.cos[0].dropThresholds[i].local2_mask,
              qos_msg->cos_config.cos[1].dropThresholds[i].local2_mask,
              qos_msg->cos_config.cos[2].dropThresholds[i].local2_mask,
              qos_msg->cos_config.cos[3].dropThresholds[i].local2_mask,
              qos_msg->cos_config.cos[4].dropThresholds[i].local2_mask,
              qos_msg->cos_config.cos[5].dropThresholds[i].local2_mask,
              qos_msg->cos_config.cos[6].dropThresholds[i].local2_mask,
              qos_msg->cos_config.cos[7].dropThresholds[i].local2_mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, "DP%u: Taildrop threshold = [ %3u %3u %3u %3u %3u %3u %3u %3u ]", i+1,
              qos_msg->cos_config.cos[0].dropThresholds[i].tailDrop_threshold,
              qos_msg->cos_config.cos[1].dropThresholds[i].tailDrop_threshold,
              qos_msg->cos_config.cos[2].dropThresholds[i].tailDrop_threshold,
              qos_msg->cos_config.cos[3].dropThresholds[i].tailDrop_threshold,
              qos_msg->cos_config.cos[4].dropThresholds[i].tailDrop_threshold,
              qos_msg->cos_config.cos[5].dropThresholds[i].tailDrop_threshold,
              qos_msg->cos_config.cos[6].dropThresholds[i].tailDrop_threshold,
              qos_msg->cos_config.cos[7].dropThresholds[i].tailDrop_threshold);
    PT_LOG_DEBUG(LOG_CTX_MSG, "DP%u: WRED min.threshold = [ %3u %3u %3u %3u %3u %3u %3u %3u ]", i+1,
              qos_msg->cos_config.cos[0].dropThresholds[i].wred_minThreshold,
              qos_msg->cos_config.cos[1].dropThresholds[i].wred_minThreshold,
              qos_msg->cos_config.cos[2].dropThresholds[i].wred_minThreshold,
              qos_msg->cos_config.cos[3].dropThresholds[i].wred_minThreshold,
              qos_msg->cos_config.cos[4].dropThresholds[i].wred_minThreshold,
              qos_msg->cos_config.cos[5].dropThresholds[i].wred_minThreshold,
              qos_msg->cos_config.cos[6].dropThresholds[i].wred_minThreshold,
              qos_msg->cos_config.cos[7].dropThresholds[i].wred_minThreshold);
    PT_LOG_DEBUG(LOG_CTX_MSG, "DP%u: WRED drop prob.    = [ %3u %3u %3u %3u %3u %3u %3u %3u ]", i+1,
              qos_msg->cos_config.cos[0].dropThresholds[i].wred_dropProb,
              qos_msg->cos_config.cos[1].dropThresholds[i].wred_dropProb,
              qos_msg->cos_config.cos[2].dropThresholds[i].wred_dropProb,
              qos_msg->cos_config.cos[3].dropThresholds[i].wred_dropProb,
              qos_msg->cos_config.cos[4].dropThresholds[i].wred_dropProb,
              qos_msg->cos_config.cos[5].dropThresholds[i].wred_dropProb,
              qos_msg->cos_config.cos[6].dropThresholds[i].wred_dropProb,
              qos_msg->cos_config.cos[7].dropThresholds[i].wred_dropProb);
  }

  return L7_SUCCESS;
}

/**
 * Redefine CoS configuration
 * 
 * @param qos_config : CoS configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_CoS2_set(msg_QoSConfiguration2_t *qos_msg)
{
  L7_int i, j;
  ptin_intf_t             ptin_intf;
  ptin_QoS_intf_t         qos_intf;
  ptin_QoS_cos_t          qos_cos[8];
  ptin_QoS_drop_t         qos_drop[8];
  L7_RC_t                 rc, rc_global = L7_SUCCESS;

  PT_LOG_DEBUG(LOG_CTX_MSG, "Slotid            = %u",qos_msg->SlotId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Interface         = %u/%u",qos_msg->intf.intf_type,qos_msg->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "General Mask      = 0x%02X",qos_msg->generic_mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Trust mode        = %u",qos_msg->trust_mode);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Bandwidth unit    = %u",qos_msg->bandwidth_unit);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Shaping rate      = %u",qos_msg->shaping_rate);
  PT_LOG_DEBUG(LOG_CTX_MSG, "pktprio.prio_mask = [ 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]",
            qos_msg->pktprio.prio_mask[0],qos_msg->pktprio.prio_mask[1],qos_msg->pktprio.prio_mask[2],qos_msg->pktprio.prio_mask[3],qos_msg->pktprio.prio_mask[4],qos_msg->pktprio.prio_mask[5],qos_msg->pktprio.prio_mask[6],qos_msg->pktprio.prio_mask[7]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "pktprio.cos       = [ %Xh %Xh %Xh %Xh %Xh %Xh %Xh %Xh ]",
            qos_msg->pktprio.cos[0],qos_msg->pktprio.cos[1],qos_msg->pktprio.cos[2],qos_msg->pktprio.cos[3],qos_msg->pktprio.cos[4],qos_msg->pktprio.cos[5],qos_msg->pktprio.cos[6],qos_msg->pktprio.cos[7]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "CoS Mask       = 0x%02X",qos_msg->cos_config.cos_mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "CoS local Mask = [ 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X ]",
            qos_msg->cos_config.cos[0].local_mask, qos_msg->cos_config.cos[1].local_mask, qos_msg->cos_config.cos[2].local_mask, qos_msg->cos_config.cos[3].local_mask, qos_msg->cos_config.cos[4].local_mask, qos_msg->cos_config.cos[5].local_mask, qos_msg->cos_config.cos[6].local_mask, qos_msg->cos_config.cos[7].local_mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Queue_scheduler = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].scheduler, qos_msg->cos_config.cos[1].scheduler, qos_msg->cos_config.cos[2].scheduler, qos_msg->cos_config.cos[3].scheduler, qos_msg->cos_config.cos[4].scheduler, qos_msg->cos_config.cos[5].scheduler, qos_msg->cos_config.cos[6].scheduler, qos_msg->cos_config.cos[7].scheduler);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Queue_min_bw    = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].min_bandwidth, qos_msg->cos_config.cos[1].min_bandwidth, qos_msg->cos_config.cos[2].min_bandwidth, qos_msg->cos_config.cos[3].min_bandwidth, qos_msg->cos_config.cos[4].min_bandwidth, qos_msg->cos_config.cos[5].min_bandwidth, qos_msg->cos_config.cos[6].min_bandwidth, qos_msg->cos_config.cos[7].min_bandwidth);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Queue_max_bw    = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].max_bandwidth, qos_msg->cos_config.cos[1].max_bandwidth, qos_msg->cos_config.cos[2].max_bandwidth, qos_msg->cos_config.cos[3].max_bandwidth, qos_msg->cos_config.cos[4].max_bandwidth, qos_msg->cos_config.cos[5].max_bandwidth, qos_msg->cos_config.cos[6].max_bandwidth, qos_msg->cos_config.cos[7].max_bandwidth);
  PT_LOG_DEBUG(LOG_CTX_MSG, "wrrSched_weight = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].wrrSched_weight, qos_msg->cos_config.cos[1].wrrSched_weight, qos_msg->cos_config.cos[2].wrrSched_weight, qos_msg->cos_config.cos[3].wrrSched_weight, qos_msg->cos_config.cos[4].wrrSched_weight, qos_msg->cos_config.cos[5].wrrSched_weight, qos_msg->cos_config.cos[6].wrrSched_weight, qos_msg->cos_config.cos[7].wrrSched_weight);
  PT_LOG_DEBUG(LOG_CTX_MSG, "dropMgmttype    = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].dropMgmtType, qos_msg->cos_config.cos[1].dropMgmtType, qos_msg->cos_config.cos[2].dropMgmtType, qos_msg->cos_config.cos[3].dropMgmtType, qos_msg->cos_config.cos[4].dropMgmtType, qos_msg->cos_config.cos[5].dropMgmtType, qos_msg->cos_config.cos[6].dropMgmtType, qos_msg->cos_config.cos[7].dropMgmtType);
  PT_LOG_DEBUG(LOG_CTX_MSG, "wred_decayExp   = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->cos_config.cos[0].wred_decayExp, qos_msg->cos_config.cos[1].wred_decayExp, qos_msg->cos_config.cos[2].wred_decayExp, qos_msg->cos_config.cos[3].wred_decayExp, qos_msg->cos_config.cos[4].wred_decayExp, qos_msg->cos_config.cos[5].wred_decayExp, qos_msg->cos_config.cos[6].wred_decayExp, qos_msg->cos_config.cos[7].wred_decayExp);
  for (i=0; i<4; i++)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "DP%u: local_mask = [ 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X ]", i+1,
              qos_msg->cos_config.cos[0].dropThresholds[i].local2_mask,
              qos_msg->cos_config.cos[1].dropThresholds[i].local2_mask,
              qos_msg->cos_config.cos[2].dropThresholds[i].local2_mask,
              qos_msg->cos_config.cos[3].dropThresholds[i].local2_mask,
              qos_msg->cos_config.cos[4].dropThresholds[i].local2_mask,
              qos_msg->cos_config.cos[5].dropThresholds[i].local2_mask,
              qos_msg->cos_config.cos[6].dropThresholds[i].local2_mask,
              qos_msg->cos_config.cos[7].dropThresholds[i].local2_mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, "DP%u: Taildrop threshold = [ %3u %3u %3u %3u %3u %3u %3u %3u ]", i+1,
              qos_msg->cos_config.cos[0].dropThresholds[i].tailDrop_threshold,
              qos_msg->cos_config.cos[1].dropThresholds[i].tailDrop_threshold,
              qos_msg->cos_config.cos[2].dropThresholds[i].tailDrop_threshold,
              qos_msg->cos_config.cos[3].dropThresholds[i].tailDrop_threshold,
              qos_msg->cos_config.cos[4].dropThresholds[i].tailDrop_threshold,
              qos_msg->cos_config.cos[5].dropThresholds[i].tailDrop_threshold,
              qos_msg->cos_config.cos[6].dropThresholds[i].tailDrop_threshold,
              qos_msg->cos_config.cos[7].dropThresholds[i].tailDrop_threshold);
    PT_LOG_DEBUG(LOG_CTX_MSG, "DP%u: WRED min.threshold = [ %3u %3u %3u %3u %3u %3u %3u %3u ]", i+1,
              qos_msg->cos_config.cos[0].dropThresholds[i].wred_minThreshold,
              qos_msg->cos_config.cos[1].dropThresholds[i].wred_minThreshold,
              qos_msg->cos_config.cos[2].dropThresholds[i].wred_minThreshold,
              qos_msg->cos_config.cos[3].dropThresholds[i].wred_minThreshold,
              qos_msg->cos_config.cos[4].dropThresholds[i].wred_minThreshold,
              qos_msg->cos_config.cos[5].dropThresholds[i].wred_minThreshold,
              qos_msg->cos_config.cos[6].dropThresholds[i].wred_minThreshold,
              qos_msg->cos_config.cos[7].dropThresholds[i].wred_minThreshold);
    PT_LOG_DEBUG(LOG_CTX_MSG, "DP%u: WRED drop prob.    = [ %3u %3u %3u %3u %3u %3u %3u %3u ]", i+1,
              qos_msg->cos_config.cos[0].dropThresholds[i].wred_dropProb,
              qos_msg->cos_config.cos[1].dropThresholds[i].wred_dropProb,
              qos_msg->cos_config.cos[2].dropThresholds[i].wred_dropProb,
              qos_msg->cos_config.cos[3].dropThresholds[i].wred_dropProb,
              qos_msg->cos_config.cos[4].dropThresholds[i].wred_dropProb,
              qos_msg->cos_config.cos[5].dropThresholds[i].wred_dropProb,
              qos_msg->cos_config.cos[6].dropThresholds[i].wred_dropProb,
              qos_msg->cos_config.cos[7].dropThresholds[i].wred_dropProb);
  }

  /* Clear structures */
  memset(&qos_intf,0x00,sizeof(ptin_QoS_intf_t));
  memset(qos_cos,0x00,sizeof(qos_cos));
  memset(qos_drop,0x00,sizeof(qos_drop));

  /* Interface */
  ptin_intf.intf_type = qos_msg->intf.intf_type;
  ptin_intf.intf_id   = qos_msg->intf.intf_id;

  /* Interface configuration */

  /* Trust mode */
  if (qos_msg->generic_mask & MSG_QOS_CONFIGURATION_TRUSTMODE_MASK)
  {
    qos_intf.trust_mode = qos_msg->trust_mode;
    qos_intf.mask |= PTIN_QOS_INTF_TRUSTMODE_MASK;
  }
  /* Bandwidth units */
  if (qos_msg->generic_mask & MSG_QOS_CONFIGURATION_BANDWIDTHUNIT_MASK)
  {
    qos_intf.bandwidth_unit = qos_msg->bandwidth_unit;
    qos_intf.mask |= PTIN_QOS_INTF_BANDWIDTHUNIT_MASK;
  }
  /* Shaping rate */
  if (qos_msg->generic_mask & MSG_QOS_CONFIGURATION_SHAPINGRATE_MASK)
  {
    qos_intf.shaping_rate = qos_msg->shaping_rate;
    qos_intf.mask |= PTIN_QOS_INTF_SHAPINGRATE_MASK;
  }
  /* Priority map to CoS */
  if (qos_msg->generic_mask & MSG_QOS_CONFIGURATION_PACKETPRIO_MASK)
  {
    /* Run all priorities */
    for (i=0; i<8; i++)
    {
      /* Is priority i to be defined? If so define CoS */
      if (qos_msg->pktprio.prio_mask[i] != 0)
      {
        qos_intf.pktprio.mask[i] = qos_msg->pktprio.prio_mask[i];
        qos_intf.pktprio.cos[i]  = qos_msg->pktprio.cos[i];

        qos_intf.mask |= PTIN_QOS_INTF_PACKETPRIO_MASK;
      }      
    }
  }
  if (qos_intf.mask)
  {
    /* Execute priority map configuration */
    rc = ptin_QoS_intf_config_set(&ptin_intf, &qos_intf);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error configuring priority map (rc=%d)", rc);
      rc_global = rc;
    }
  }

  /* QoS configuration */
  if (qos_msg->generic_mask & MSG_QOS_CONFIGURATION_QOSCONF_MASK)
  {
    /* Run all QoS */
    for (i=0; i<8; i++)
    {
      /* QoS i is to be configured? */
      if ( !((qos_msg->cos_config.cos_mask>>i) & 1) )  continue;

      /* Scheduler type */
      if (qos_msg->cos_config.cos[i].local_mask & MSG_QOS_CONFIGURATION_QOSCONF_SCHEDULER_MASK)
      {
        qos_cos[i].scheduler_type = qos_msg->cos_config.cos[i].scheduler;
        qos_cos[i].mask |= PTIN_QOS_COS_SCHEDULER_MASK;
      }
      /* Minimum mandwidth */
      if (qos_msg->cos_config.cos[i].local_mask & MSG_QOS_CONFIGURATION_QOSCONF_BW_MIN_MASK)
      {
        qos_cos[i].min_bandwidth = qos_msg->cos_config.cos[i].min_bandwidth;
        qos_cos[i].mask |= PTIN_QOS_COS_BW_MIN_MASK;
      }
      /* Maximum bandwidth */
      if (qos_msg->cos_config.cos[i].local_mask & MSG_QOS_CONFIGURATION_QOSCONF_BW_MAX_MASK)
      {
        qos_cos[i].max_bandwidth = qos_msg->cos_config.cos[i].max_bandwidth;
        qos_cos[i].mask |= PTIN_QOS_COS_BW_MAX_MASK;
      }
      /* WRR Weight */
      if (qos_msg->cos_config.cos[i].local_mask & MSG_QOS_CONFIGURATION_QOSCONF_WRR_WEIGHT_MASK)
      {
        qos_cos[i].wrrSched_weight = qos_msg->cos_config.cos[i].wrrSched_weight;
        qos_cos[i].mask |= PTIN_QOS_COS_WRR_WEIGHT_MASK;
      }
    }

    /* Apply configuration */
    rc = ptin_QoS_cos_config_set(&ptin_intf, (L7_uint8)-1, qos_cos);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error configuring QoS (rc=%d)",rc);
      rc_global = rc;
    }
  }

  /* DROP configuration */
  if (qos_msg->generic_mask & MSG_QOS_CONFIGURATION_QOSCONF_MASK)
  {
    /* Run all QoS */
    for (i=0; i<8; i++)
    {
      /* QoS i is to be configured? */
      if ( !((qos_msg->cos_config.cos_mask>>i) & 1) )  continue;

      /* Drop MGMT type */
      if (qos_msg->cos_config.cos[i].local_mask & MSG_QOS_CONFIGURATION_QOSCONF_MGMT_TYPE_MASK)
      {
        qos_drop[i].queue_management_type = qos_msg->cos_config.cos[i].dropMgmtType;
        qos_drop[i].mask |= PTIN_QOS_COS_QUEUE_MANGM_MASK;
      }
      /* WRED decay exponent */
      if (qos_msg->cos_config.cos[i].local_mask & MSG_QOS_CONFIGURATION_QOSCONF_WRED_DECAYEXP_MASK)
      {
        qos_drop[i].wred_decayExp = qos_msg->cos_config.cos[i].wred_decayExp;
        qos_drop[i].mask |= PTIN_QOS_COS_WRED_DECAY_EXP_MASK;
      }
      /* Thresholds */
      if (qos_msg->cos_config.cos[i].local_mask & MSG_QOS_CONFIGURATION_QOSCONF_THRESHOLDS_MASK)
      {
        /* Run all DP levels */
        for (j = 0; j < 4; j++)
        {
          if (qos_msg->cos_config.cos[i].dropThresholds[j].local2_mask == 0)  continue;

          qos_drop[i].mask |= PTIN_QOS_COS_WRED_THRESHOLDS_MASK;

          /* Taildrop threshold */
          if (qos_msg->cos_config.cos[i].dropThresholds[j].local2_mask & MSG_QOS_CONFIGURATION_QOSCONF_DROP_TAILDROP_THRES_MASK)
          {
            qos_drop[i].dp[j].taildrop_threshold = qos_msg->cos_config.cos[i].dropThresholds[j].tailDrop_threshold;
            qos_drop[i].dp[j].local_mask |= PTIN_QOS_COS_DP_TAILDROP_THRESH_MASK;
          }
          /* WRED min threshold */
          if (qos_msg->cos_config.cos[i].dropThresholds[j].local2_mask & MSG_QOS_CONFIGURATION_QOSCONF_DROP_WRED_MINTHRES_MASK)
          {
            qos_drop[i].dp[j].wred_min_threshold = qos_msg->cos_config.cos[i].dropThresholds[j].wred_minThreshold;
            qos_drop[i].dp[j].local_mask |= PTIN_QOS_COS_DP_WRED_THRESH_MIN_MASK;
          }
          /* WRED max threshold */
          if (qos_msg->cos_config.cos[i].dropThresholds[j].local2_mask & MSG_QOS_CONFIGURATION_QOSCONF_DROP_WRED_MAXTHRES_MASK)
          {
            qos_drop[i].dp[j].wred_max_threshold = qos_msg->cos_config.cos[i].dropThresholds[j].wred_maxThreshold;
            qos_drop[i].dp[j].local_mask |= PTIN_QOS_COS_DP_WRED_THRESH_MAX_MASK;
          }
          /* WRED drop probability */
          if (qos_msg->cos_config.cos[i].dropThresholds[j].local2_mask & MSG_QOS_CONFIGURATION_QOSCONF_DROP_WRED_DROPPROB_MASK)
          {
            qos_drop[i].dp[j].wred_drop_prob = qos_msg->cos_config.cos[i].dropThresholds[j].wred_dropProb;
            qos_drop[i].dp[j].local_mask |= PTIN_QOS_COS_DP_WRED_DROP_PROB_MASK;
          }
        }
      }
    }

    /* Apply configuration */
    rc = ptin_QoS_drop_config_set(&ptin_intf, (L7_uint8)-1, qos_drop);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error configuring QoS Drop management (rc=%d)",rc);
      rc_global = rc;
    }
  }

  if (rc_global==L7_SUCCESS)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG,"Success applying QoS configurations to all CoS");
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error applying QoS configurations to all CoS (rc_global=%d)", rc_global);
  }

  return rc_global;
}

/**
 * Get CoS configuration
 * 
 * @param qos_config : CoS configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_CoS3_get(msg_QoSConfiguration3_t *qos_msg)
{
  L7_int i, j;
  ptin_intf_t             ptin_intf;
  L7_uint                 trust_mode;
  ptin_QoS_intf_t         qos_intf;
  ptin_QoS_cos_t          qos_cos[8];
  ptin_QoS_drop_t         qos_drop[8];
  L7_RC_t                 rc;
  L7_uint8 slotId;

  /* Validate input */
  if (qos_msg == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Null pointer!");
    return L7_FAILURE;
  }

  /* Interface */
  ptin_intf.intf_type = qos_msg->intf.intf_type;
  ptin_intf.intf_id   = qos_msg->intf.intf_id;
  slotId = qos_msg->SlotId;

  /* Clear structure */
  memset(qos_msg,0x00,sizeof(msg_QoSConfiguration3_t));
  /* Pre-fill it */
  qos_msg->SlotId = slotId;
  qos_msg->intf.intf_type = ptin_intf.intf_type;
  qos_msg->intf.intf_id   = ptin_intf.intf_id;

  /* Get current interface configuration */
  memset(&qos_intf,0x00,sizeof(ptin_QoS_intf_t));
  rc = ptin_QoS_intf_config_get(&ptin_intf, &qos_intf);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading interface configuration (rc=%d)", rc);
    return rc;
  }
  trust_mode = qos_intf.trust_mode;

  /* Bandwidth units */
  qos_msg->bandwidth_unit = 0;
  qos_msg->main_mask |= MSG_QOS3_BANDWIDTH_UNITS_MASK;

  /* Interface configuration */

  /* Trust mode */
  if (qos_intf.mask & PTIN_QOS_INTF_TRUSTMODE_MASK)
  {
    qos_msg->ingress.trust_mode = qos_intf.trust_mode;
    qos_msg->ingress.ingress_mask |= MSG_QOS3_INGRESS_TRUST_MODE_MASK;
    qos_msg->main_mask |= MSG_QOS3_INGRESS_MASK;
  }

  /* COS classification */
  if (qos_intf.mask & PTIN_QOS_INTF_PACKETPRIO_MASK)
  {
    if (trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
    {
      /* Run all priorities */
      for (i=0; i<8; i++)
      {
        /* Is priority i to be defined? If so define CoS */
        if (qos_intf.pktprio.mask[i])
        {
          qos_msg->ingress.cos_classif.pcp_map.prio_mask |= (1 << i);
          qos_msg->ingress.cos_classif.pcp_map.cos[i] = qos_intf.pktprio.cos[i];

          qos_msg->ingress.ingress_mask |= MSG_QOS3_INGRESS_COS_CLASSIF_MASK;
          qos_msg->main_mask |= MSG_QOS3_INGRESS_MASK;
        }
      }
    }
    else if (trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC)
    {
      /* Run all priorities */
      for (i=0; i<8; i++)
      {
        /* Is priority i to be defined? If so define CoS */
        if (qos_intf.pktprio.mask[i])
        {
          qos_msg->ingress.cos_classif.ipprec_map.prio_mask |= (1 << i);
          qos_msg->ingress.cos_classif.ipprec_map.cos[i] = qos_intf.pktprio.cos[i];

          qos_msg->ingress.ingress_mask |= MSG_QOS3_INGRESS_COS_CLASSIF_MASK;
          qos_msg->main_mask |= MSG_QOS3_INGRESS_MASK;
        }
      }
    }
    else if (trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
    {
      /* Run all priorities */
      for (i=0; i<64; i++)
      {
        /* Is priority i to be defined? If so define CoS */
        if (qos_intf.pktprio.mask[i/8] >> (i%8))
        {
          qos_msg->ingress.cos_classif.dscp_map.prio_mask[i/32] |= 1ULL << i;
          qos_msg->ingress.cos_classif.dscp_map.cos[i] = (qos_intf.pktprio.cos[i/8] >> ((i%8)*4)) & 0xf;

          qos_msg->ingress.ingress_mask |= MSG_QOS3_INGRESS_COS_CLASSIF_MASK;
          qos_msg->main_mask |= MSG_QOS3_INGRESS_MASK;
        }
      }
    }
  }
  /* Shaping rate */
  if (qos_intf.mask & PTIN_QOS_INTF_SHAPINGRATE_MASK)
  {
    qos_msg->egress.shaping_rate = qos_intf.shaping_rate;
    qos_msg->egress.egress_mask |= MSG_QOS3_EGRESS_INTF_SHAPER_MASK;
    qos_msg->main_mask |= MSG_QOS3_EGRESS_MASK;
  }

  /* COS configuration */

  /* Clear structures */
  memset(qos_cos,0x00,sizeof(qos_cos));
  rc = ptin_QoS_cos_config_get(&ptin_intf, (L7_uint8)-1, qos_cos); 
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading QoS configurations (rc=%d)",rc);
    return rc;
  }

  /* Scheduler configurations */
  /* Run all QoS */
  for (i=0; i<8; i++)
  {
    /* Scheduler type */
    if (qos_cos[i].mask & PTIN_QOS_COS_SCHEDULER_MASK)
    {
      qos_msg->egress.cos_scheduler[i].schedulerType = qos_cos[i].scheduler_type;

      qos_msg->egress.cos_scheduler[i].local_mask |= MSG_QOS3_EGRESS_COS_SCHEDULER_TYPE_MASK;
      qos_msg->egress.egress_mask |= MSG_QOS3_EGRESS_COS_SCHEDULER_MASK;
      qos_msg->main_mask |= MSG_QOS3_EGRESS_MASK;
    }
    /* WRR Weight */
    if (qos_cos[i].mask & PTIN_QOS_COS_WRR_WEIGHT_MASK)
    {
      qos_msg->egress.cos_scheduler[i].wrrSched_weight = qos_cos[i].wrrSched_weight;

      qos_msg->egress.cos_scheduler[i].local_mask |= MSG_QOS3_EGRESS_COS_SCHEDULER_WRR_WEIGHT_MASK;
      qos_msg->egress.egress_mask |= MSG_QOS3_EGRESS_COS_SCHEDULER_MASK;
      qos_msg->main_mask |= MSG_QOS3_EGRESS_MASK;
    }
  }
  /* COS Shaper configurations */
  /* Run all QoS */
  for (i=0; i<8; i++)
  {
    /* Minimum mandwidth */
    if (qos_cos[i].mask & PTIN_QOS_COS_BW_MIN_MASK)
    {
      qos_msg->egress.cos_shaper[i].min_bandwidth = qos_cos[i].min_bandwidth;
      
      qos_msg->egress.cos_shaper[i].local_mask |= MSG_QOS3_EGRESS_COS_SHAPER_MIN_BW_MASK;
      qos_msg->egress.egress_mask |= MSG_QOS3_EGRESS_COS_SHAPER_MASK;
      qos_msg->main_mask |= MSG_QOS3_EGRESS_MASK;
    }
    /* Maximum bandwidth */
    if (qos_cos[i].mask & PTIN_QOS_COS_BW_MAX_MASK)
    {
      qos_msg->egress.cos_shaper[i].max_bandwidth = qos_cos[i].max_bandwidth;
      
      qos_msg->egress.cos_shaper[i].local_mask |= MSG_QOS3_EGRESS_COS_SHAPER_MAX_BW_MASK;
      qos_msg->egress.egress_mask |= MSG_QOS3_EGRESS_COS_SHAPER_MASK;
      qos_msg->main_mask |= MSG_QOS3_EGRESS_MASK;
    }
  }

  /* DROP configuration */

  /* Clear structures */
  memset(qos_drop,0x00,sizeof(qos_drop));
  rc = ptin_QoS_drop_config_get(&ptin_intf, (L7_uint8)-1, qos_drop);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading QoS Drop management configurations (rc=%d)",rc);
    return rc;
  }

  /* Run all QoS */
  for (i=0; i<8; i++)
  {
    /* Drop MGMT type */
    if (qos_drop[i].mask & PTIN_QOS_COS_QUEUE_MANGM_MASK)
    {
      qos_msg->egress.cos_dropmgmt[i].dropMgmtType = qos_drop[i].queue_management_type;
      
      qos_msg->egress.cos_dropmgmt[i].local_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_TYPE_MASK;
      qos_msg->egress.egress_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_MASK;
      qos_msg->main_mask |= MSG_QOS3_EGRESS_MASK;
    }
    /* WRED decay exponent */
    if (qos_drop[i].mask & PTIN_QOS_COS_WRED_DECAY_EXP_MASK)
    {
      qos_msg->egress.cos_dropmgmt[i].wred_decayExp = qos_drop[i].wred_decayExp;
      
      qos_msg->egress.cos_dropmgmt[i].local_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_WRED_DECAYEXP_MASK;
      qos_msg->egress.egress_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_MASK;
      qos_msg->main_mask |= MSG_QOS3_EGRESS_MASK;
    }

    if (qos_drop[i].mask & PTIN_QOS_COS_WRED_THRESHOLDS_MASK)
    {
      /* Run all DP levels */
      for (j = 0; j < 4; j++)
      {
        /* Taildrop threshold */
        if (qos_drop[i].dp[j].local_mask & PTIN_QOS_COS_DP_TAILDROP_THRESH_MASK)
        {
          qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].tailDrop_threshold = qos_drop[i].dp[j].taildrop_threshold;
          qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].local2_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_TAILDROP_MAX_MASK;
        }
        /* WRED min threshold */
        if (qos_drop[i].dp[j].local_mask & PTIN_QOS_COS_DP_WRED_THRESH_MIN_MASK)
        {
          qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].wred_minThreshold = qos_drop[i].dp[j].wred_min_threshold;
          qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].local2_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_WRED_MIN_MASK;
        }
        /* WRED max threshold */
        if (qos_drop[i].dp[j].local_mask & PTIN_QOS_COS_DP_WRED_THRESH_MAX_MASK)
        {
          qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].wred_maxThreshold = qos_drop[i].dp[j].wred_max_threshold;
          qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].local2_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_WRED_MAX_MASK;
        }
        /* WRED drop probability */
        if (qos_drop[i].dp[j].local_mask & PTIN_QOS_COS_DP_WRED_DROP_PROB_MASK)
        {
          qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].wred_dropProb = qos_drop[i].dp[j].wred_drop_prob;
          qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].local2_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_WRED_DROPPROB_MASK;
        }
      }
      qos_msg->egress.cos_dropmgmt[i].local_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLDS_MASK;
      qos_msg->egress.egress_mask |= MSG_QOS3_EGRESS_COS_DROPMGMT_MASK;
      qos_msg->main_mask |= MSG_QOS3_EGRESS_MASK;
    }
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "Slotid         = %u",qos_msg->SlotId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Interface      = %u/%u",qos_msg->intf.intf_type,qos_msg->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Main Mask      = 0x%02x",qos_msg->main_mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Bandwidth unit = %u",qos_msg->bandwidth_unit);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Ingress:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Ingress Mask = 0x%02x",qos_msg->ingress.ingress_mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Trust mode   = %u",qos_msg->ingress.trust_mode);
  if (qos_msg->ingress.trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_classif.pcp_map.prio_mask = 0x%02x",qos_msg->ingress.cos_classif.pcp_map.prio_mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_classif.pcp_map.cos = { %u %u %u %u %u %u %u %u } ",
              qos_msg->ingress.cos_classif.pcp_map.cos[0],
              qos_msg->ingress.cos_classif.pcp_map.cos[1],
              qos_msg->ingress.cos_classif.pcp_map.cos[2],
              qos_msg->ingress.cos_classif.pcp_map.cos[3],
              qos_msg->ingress.cos_classif.pcp_map.cos[4],
              qos_msg->ingress.cos_classif.pcp_map.cos[5],
              qos_msg->ingress.cos_classif.pcp_map.cos[6],
              qos_msg->ingress.cos_classif.pcp_map.cos[7]);
  }
  else if (qos_msg->ingress.trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_classif.ipprec_map.prio_mask = 0x%02x",qos_msg->ingress.cos_classif.ipprec_map.prio_mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_classif.ipprec_map.cos = { %u %u %u %u %u %u %u %u } ",
              qos_msg->ingress.cos_classif.ipprec_map.cos[0],
              qos_msg->ingress.cos_classif.ipprec_map.cos[1],
              qos_msg->ingress.cos_classif.ipprec_map.cos[2],
              qos_msg->ingress.cos_classif.ipprec_map.cos[3],
              qos_msg->ingress.cos_classif.ipprec_map.cos[4],
              qos_msg->ingress.cos_classif.ipprec_map.cos[5],
              qos_msg->ingress.cos_classif.ipprec_map.cos[6],
              qos_msg->ingress.cos_classif.ipprec_map.cos[7]);
  }
  else if (qos_msg->ingress.trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_classif.dscp_map.prio_mask = 0x%08x 0x%08x",
              qos_msg->ingress.cos_classif.dscp_map.prio_mask[0], qos_msg->ingress.cos_classif.dscp_map.prio_mask[1]);
    for (i = 0; i < 64; i+=8)
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_classif.dscp_map.cos[%2u-%2u] = { %u %u %u %u %u %u %u %u } ", i, i+7,
                qos_msg->ingress.cos_classif.dscp_map.cos[i+0],
                qos_msg->ingress.cos_classif.dscp_map.cos[i+1],
                qos_msg->ingress.cos_classif.dscp_map.cos[i+2],
                qos_msg->ingress.cos_classif.dscp_map.cos[i+3],
                qos_msg->ingress.cos_classif.dscp_map.cos[i+4],
                qos_msg->ingress.cos_classif.dscp_map.cos[i+5],
                qos_msg->ingress.cos_classif.dscp_map.cos[i+6],
                qos_msg->ingress.cos_classif.dscp_map.cos[i+7]);
    }
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_classif does not have usable information.");
  }
  for (i = 0; i < 8; i++)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Policer[%u]: mask=0x%02x - cir=%u eir=%u cbs=%u ebs=%u",
              i, qos_msg->ingress.cos_policer[i].local_mask,
              qos_msg->ingress.cos_policer[i].cir, qos_msg->ingress.cos_policer[i].eir,
              qos_msg->ingress.cos_policer[i].cbs, qos_msg->ingress.cos_policer[i].ebs);
  }
  PT_LOG_DEBUG(LOG_CTX_MSG, "Egress:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Egress Mask  = 0x%02x", qos_msg->egress.egress_mask); 
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Shaping rate = %u %%", qos_msg->egress.shaping_rate);

  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_scheduler->local_mask      = [ 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]",
            qos_msg->egress.cos_scheduler[0].local_mask,
            qos_msg->egress.cos_scheduler[1].local_mask,
            qos_msg->egress.cos_scheduler[2].local_mask,
            qos_msg->egress.cos_scheduler[3].local_mask,
            qos_msg->egress.cos_scheduler[4].local_mask,
            qos_msg->egress.cos_scheduler[5].local_mask,
            qos_msg->egress.cos_scheduler[6].local_mask,
            qos_msg->egress.cos_scheduler[7].local_mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_scheduler->schedulerType   = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->egress.cos_scheduler[0].schedulerType,
            qos_msg->egress.cos_scheduler[1].schedulerType,
            qos_msg->egress.cos_scheduler[2].schedulerType,
            qos_msg->egress.cos_scheduler[3].schedulerType,
            qos_msg->egress.cos_scheduler[4].schedulerType,
            qos_msg->egress.cos_scheduler[5].schedulerType,
            qos_msg->egress.cos_scheduler[6].schedulerType,
            qos_msg->egress.cos_scheduler[7].schedulerType);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_scheduler->wrrSched_weight = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->egress.cos_scheduler[0].wrrSched_weight,
            qos_msg->egress.cos_scheduler[1].wrrSched_weight,
            qos_msg->egress.cos_scheduler[2].wrrSched_weight,
            qos_msg->egress.cos_scheduler[3].wrrSched_weight,
            qos_msg->egress.cos_scheduler[4].wrrSched_weight,
            qos_msg->egress.cos_scheduler[5].wrrSched_weight,
            qos_msg->egress.cos_scheduler[6].wrrSched_weight,
            qos_msg->egress.cos_scheduler[7].wrrSched_weight);

  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_shaper->local_mask         = [ 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]",
            qos_msg->egress.cos_shaper[0].local_mask,
            qos_msg->egress.cos_shaper[1].local_mask,
            qos_msg->egress.cos_shaper[2].local_mask,
            qos_msg->egress.cos_shaper[3].local_mask,
            qos_msg->egress.cos_shaper[4].local_mask,
            qos_msg->egress.cos_shaper[5].local_mask,
            qos_msg->egress.cos_shaper[6].local_mask,
            qos_msg->egress.cos_shaper[7].local_mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_shaper->min_bandwidth      = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->egress.cos_shaper[0].min_bandwidth,
            qos_msg->egress.cos_shaper[1].min_bandwidth,
            qos_msg->egress.cos_shaper[2].min_bandwidth,
            qos_msg->egress.cos_shaper[3].min_bandwidth,
            qos_msg->egress.cos_shaper[4].min_bandwidth,
            qos_msg->egress.cos_shaper[5].min_bandwidth,
            qos_msg->egress.cos_shaper[6].min_bandwidth,
            qos_msg->egress.cos_shaper[7].min_bandwidth);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_shaper->max_bandwidth      = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->egress.cos_shaper[0].max_bandwidth,
            qos_msg->egress.cos_shaper[1].max_bandwidth,
            qos_msg->egress.cos_shaper[2].max_bandwidth,
            qos_msg->egress.cos_shaper[3].max_bandwidth,
            qos_msg->egress.cos_shaper[4].max_bandwidth,
            qos_msg->egress.cos_shaper[5].max_bandwidth,
            qos_msg->egress.cos_shaper[6].max_bandwidth,
            qos_msg->egress.cos_shaper[7].max_bandwidth);

  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_dropmgmt->local_mask       = [ 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]",
            qos_msg->egress.cos_dropmgmt[0].local_mask,
            qos_msg->egress.cos_dropmgmt[1].local_mask,
            qos_msg->egress.cos_dropmgmt[2].local_mask,
            qos_msg->egress.cos_dropmgmt[3].local_mask,
            qos_msg->egress.cos_dropmgmt[4].local_mask,
            qos_msg->egress.cos_dropmgmt[5].local_mask,
            qos_msg->egress.cos_dropmgmt[6].local_mask,
            qos_msg->egress.cos_dropmgmt[7].local_mask);

  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_dropmgmt->dropMgmttype     = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->egress.cos_dropmgmt[0].dropMgmtType,
            qos_msg->egress.cos_dropmgmt[1].dropMgmtType,
            qos_msg->egress.cos_dropmgmt[2].dropMgmtType,
            qos_msg->egress.cos_dropmgmt[3].dropMgmtType,
            qos_msg->egress.cos_dropmgmt[4].dropMgmtType,
            qos_msg->egress.cos_dropmgmt[5].dropMgmtType,
            qos_msg->egress.cos_dropmgmt[6].dropMgmtType,
            qos_msg->egress.cos_dropmgmt[7].dropMgmtType);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_dropmgmt->wred_decayExp    = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->egress.cos_dropmgmt[0].wred_decayExp,
            qos_msg->egress.cos_dropmgmt[1].wred_decayExp,
            qos_msg->egress.cos_dropmgmt[2].wred_decayExp,
            qos_msg->egress.cos_dropmgmt[3].wred_decayExp,
            qos_msg->egress.cos_dropmgmt[4].wred_decayExp,
            qos_msg->egress.cos_dropmgmt[5].wred_decayExp,
            qos_msg->egress.cos_dropmgmt[6].wred_decayExp,
            qos_msg->egress.cos_dropmgmt[7].wred_decayExp);

  for (i=0; i<6; i++)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_dropmgmt->dp_threshold[%u]: local_mask = [ 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]", i+1,
              qos_msg->egress.cos_dropmgmt[0].dp_thresholds[i].local2_mask,
              qos_msg->egress.cos_dropmgmt[1].dp_thresholds[i].local2_mask,
              qos_msg->egress.cos_dropmgmt[2].dp_thresholds[i].local2_mask,
              qos_msg->egress.cos_dropmgmt[3].dp_thresholds[i].local2_mask,
              qos_msg->egress.cos_dropmgmt[4].dp_thresholds[i].local2_mask,
              qos_msg->egress.cos_dropmgmt[5].dp_thresholds[i].local2_mask,
              qos_msg->egress.cos_dropmgmt[6].dp_thresholds[i].local2_mask,
              qos_msg->egress.cos_dropmgmt[7].dp_thresholds[i].local2_mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_dropmgmt->dp_threshold[%u]: Taildrop threshold = [ %3u %3u %3u %3u %3u %3u %3u %3u ]", i+1,
              qos_msg->egress.cos_dropmgmt[0].dp_thresholds[i].tailDrop_threshold,
              qos_msg->egress.cos_dropmgmt[1].dp_thresholds[i].tailDrop_threshold,
              qos_msg->egress.cos_dropmgmt[2].dp_thresholds[i].tailDrop_threshold,
              qos_msg->egress.cos_dropmgmt[3].dp_thresholds[i].tailDrop_threshold,
              qos_msg->egress.cos_dropmgmt[4].dp_thresholds[i].tailDrop_threshold,
              qos_msg->egress.cos_dropmgmt[5].dp_thresholds[i].tailDrop_threshold,
              qos_msg->egress.cos_dropmgmt[6].dp_thresholds[i].tailDrop_threshold,
              qos_msg->egress.cos_dropmgmt[7].dp_thresholds[i].tailDrop_threshold);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_dropmgmt->dp_threshold[%u]: WRED min.threshold = [ %3u %3u %3u %3u %3u %3u %3u %3u ]", i+1,
              qos_msg->egress.cos_dropmgmt[0].dp_thresholds[i].wred_minThreshold,
              qos_msg->egress.cos_dropmgmt[1].dp_thresholds[i].wred_minThreshold,
              qos_msg->egress.cos_dropmgmt[2].dp_thresholds[i].wred_minThreshold,
              qos_msg->egress.cos_dropmgmt[3].dp_thresholds[i].wred_minThreshold,
              qos_msg->egress.cos_dropmgmt[4].dp_thresholds[i].wred_minThreshold,
              qos_msg->egress.cos_dropmgmt[5].dp_thresholds[i].wred_minThreshold,
              qos_msg->egress.cos_dropmgmt[6].dp_thresholds[i].wred_minThreshold,
              qos_msg->egress.cos_dropmgmt[7].dp_thresholds[i].wred_minThreshold);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_dropmgmt->dp_threshold[%u]: WRED drop prob.    = [ %3u %3u %3u %3u %3u %3u %3u %3u ]", i+1,
              qos_msg->egress.cos_dropmgmt[0].dp_thresholds[i].wred_dropProb,
              qos_msg->egress.cos_dropmgmt[1].dp_thresholds[i].wred_dropProb,
              qos_msg->egress.cos_dropmgmt[2].dp_thresholds[i].wred_dropProb,
              qos_msg->egress.cos_dropmgmt[3].dp_thresholds[i].wred_dropProb,
              qos_msg->egress.cos_dropmgmt[4].dp_thresholds[i].wred_dropProb,
              qos_msg->egress.cos_dropmgmt[5].dp_thresholds[i].wred_dropProb,
              qos_msg->egress.cos_dropmgmt[6].dp_thresholds[i].wred_dropProb,
              qos_msg->egress.cos_dropmgmt[7].dp_thresholds[i].wred_dropProb);
  }
  PT_LOG_DEBUG(LOG_CTX_MSG, "Dump finished!");

  return L7_SUCCESS;
}

/**
 * Redefine CoS configuration
 * 
 * @param qos_config : CoS configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_CoS3_set(msg_QoSConfiguration3_t *qos_msg)
{
  L7_int i, j;
  ptin_intf_t             ptin_intf;
  L7_uint                 trust_mode;
  ptin_QoS_intf_t         qos_intf, qos_intf_curr;
  ptin_QoS_cos_t          qos_cos[8];
  ptin_QoS_drop_t         qos_drop[8];
  L7_RC_t                 rc, rc_global = L7_SUCCESS;
  L7_BOOL                 apply_conf;
  ptin_bw_meter_t         meter;

  if (qos_msg == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Null pointer!");
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "Slotid         = %u",qos_msg->SlotId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Interface      = %u/%u",qos_msg->intf.intf_type,qos_msg->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Main Mask      = 0x%02x",qos_msg->main_mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Bandwidth unit = %u",qos_msg->bandwidth_unit);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Ingress:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Ingress Mask = 0x%02x",qos_msg->ingress.ingress_mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Trust mode   = %u",qos_msg->ingress.trust_mode);
  if (qos_msg->ingress.trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_classif.pcp_map.prio_mask = 0x%02x",qos_msg->ingress.cos_classif.pcp_map.prio_mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_classif.pcp_map.cos = { %u %u %u %u %u %u %u %u } ",
              qos_msg->ingress.cos_classif.pcp_map.cos[0],
              qos_msg->ingress.cos_classif.pcp_map.cos[1],
              qos_msg->ingress.cos_classif.pcp_map.cos[2],
              qos_msg->ingress.cos_classif.pcp_map.cos[3],
              qos_msg->ingress.cos_classif.pcp_map.cos[4],
              qos_msg->ingress.cos_classif.pcp_map.cos[5],
              qos_msg->ingress.cos_classif.pcp_map.cos[6],
              qos_msg->ingress.cos_classif.pcp_map.cos[7]);
  }
  else if (qos_msg->ingress.trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_classif.ipprec_map.prio_mask = 0x%02x",qos_msg->ingress.cos_classif.ipprec_map.prio_mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_classif.ipprec_map.cos = { %u %u %u %u %u %u %u %u } ",
              qos_msg->ingress.cos_classif.ipprec_map.cos[0],
              qos_msg->ingress.cos_classif.ipprec_map.cos[1],
              qos_msg->ingress.cos_classif.ipprec_map.cos[2],
              qos_msg->ingress.cos_classif.ipprec_map.cos[3],
              qos_msg->ingress.cos_classif.ipprec_map.cos[4],
              qos_msg->ingress.cos_classif.ipprec_map.cos[5],
              qos_msg->ingress.cos_classif.ipprec_map.cos[6],
              qos_msg->ingress.cos_classif.ipprec_map.cos[7]);
  }
  else if (qos_msg->ingress.trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_classif.dscp_map.prio_mask = 0x%08x 0x%08x",
              qos_msg->ingress.cos_classif.dscp_map.prio_mask[0], qos_msg->ingress.cos_classif.dscp_map.prio_mask[1]);
    for (i = 0; i < 64; i+=8)
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_classif.dscp_map.cos[%2u-%2u] = { %u %u %u %u %u %u %u %u } ", i, i+7,
                qos_msg->ingress.cos_classif.dscp_map.cos[i+0],
                qos_msg->ingress.cos_classif.dscp_map.cos[i+1],
                qos_msg->ingress.cos_classif.dscp_map.cos[i+2],
                qos_msg->ingress.cos_classif.dscp_map.cos[i+3],
                qos_msg->ingress.cos_classif.dscp_map.cos[i+4],
                qos_msg->ingress.cos_classif.dscp_map.cos[i+5],
                qos_msg->ingress.cos_classif.dscp_map.cos[i+6],
                qos_msg->ingress.cos_classif.dscp_map.cos[i+7]);
    }
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_classif does not have usable information.");
  }
  for (i = 0; i < 8; i++)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Policer[%u]: mask=0x%02x - cir=%u eir=%u cbs=%u ebs=%u",
              i, qos_msg->ingress.cos_policer[i].local_mask,
              qos_msg->ingress.cos_policer[i].cir, qos_msg->ingress.cos_policer[i].eir,
              qos_msg->ingress.cos_policer[i].cbs, qos_msg->ingress.cos_policer[i].ebs);
  }
  PT_LOG_DEBUG(LOG_CTX_MSG, "Egress:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Egress Mask  = 0x%02x", qos_msg->egress.egress_mask); 
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Shaping rate = %u %%", qos_msg->egress.shaping_rate);

  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_scheduler->local_mask      = [ 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]",
            qos_msg->egress.cos_scheduler[0].local_mask,
            qos_msg->egress.cos_scheduler[1].local_mask,
            qos_msg->egress.cos_scheduler[2].local_mask,
            qos_msg->egress.cos_scheduler[3].local_mask,
            qos_msg->egress.cos_scheduler[4].local_mask,
            qos_msg->egress.cos_scheduler[5].local_mask,
            qos_msg->egress.cos_scheduler[6].local_mask,
            qos_msg->egress.cos_scheduler[7].local_mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_scheduler->schedulerType   = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->egress.cos_scheduler[0].schedulerType,
            qos_msg->egress.cos_scheduler[1].schedulerType,
            qos_msg->egress.cos_scheduler[2].schedulerType,
            qos_msg->egress.cos_scheduler[3].schedulerType,
            qos_msg->egress.cos_scheduler[4].schedulerType,
            qos_msg->egress.cos_scheduler[5].schedulerType,
            qos_msg->egress.cos_scheduler[6].schedulerType,
            qos_msg->egress.cos_scheduler[7].schedulerType);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_scheduler->wrrSched_weight = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->egress.cos_scheduler[0].wrrSched_weight,
            qos_msg->egress.cos_scheduler[1].wrrSched_weight,
            qos_msg->egress.cos_scheduler[2].wrrSched_weight,
            qos_msg->egress.cos_scheduler[3].wrrSched_weight,
            qos_msg->egress.cos_scheduler[4].wrrSched_weight,
            qos_msg->egress.cos_scheduler[5].wrrSched_weight,
            qos_msg->egress.cos_scheduler[6].wrrSched_weight,
            qos_msg->egress.cos_scheduler[7].wrrSched_weight);

  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_shaper->local_mask         = [ 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]",
            qos_msg->egress.cos_shaper[0].local_mask,
            qos_msg->egress.cos_shaper[1].local_mask,
            qos_msg->egress.cos_shaper[2].local_mask,
            qos_msg->egress.cos_shaper[3].local_mask,
            qos_msg->egress.cos_shaper[4].local_mask,
            qos_msg->egress.cos_shaper[5].local_mask,
            qos_msg->egress.cos_shaper[6].local_mask,
            qos_msg->egress.cos_shaper[7].local_mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_shaper->min_bandwidth      = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->egress.cos_shaper[0].min_bandwidth,
            qos_msg->egress.cos_shaper[1].min_bandwidth,
            qos_msg->egress.cos_shaper[2].min_bandwidth,
            qos_msg->egress.cos_shaper[3].min_bandwidth,
            qos_msg->egress.cos_shaper[4].min_bandwidth,
            qos_msg->egress.cos_shaper[5].min_bandwidth,
            qos_msg->egress.cos_shaper[6].min_bandwidth,
            qos_msg->egress.cos_shaper[7].min_bandwidth);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_shaper->max_bandwidth      = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->egress.cos_shaper[0].max_bandwidth,
            qos_msg->egress.cos_shaper[1].max_bandwidth,
            qos_msg->egress.cos_shaper[2].max_bandwidth,
            qos_msg->egress.cos_shaper[3].max_bandwidth,
            qos_msg->egress.cos_shaper[4].max_bandwidth,
            qos_msg->egress.cos_shaper[5].max_bandwidth,
            qos_msg->egress.cos_shaper[6].max_bandwidth,
            qos_msg->egress.cos_shaper[7].max_bandwidth);

  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_dropmgmt->local_mask       = [ 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]",
            qos_msg->egress.cos_dropmgmt[0].local_mask,
            qos_msg->egress.cos_dropmgmt[1].local_mask,
            qos_msg->egress.cos_dropmgmt[2].local_mask,
            qos_msg->egress.cos_dropmgmt[3].local_mask,
            qos_msg->egress.cos_dropmgmt[4].local_mask,
            qos_msg->egress.cos_dropmgmt[5].local_mask,
            qos_msg->egress.cos_dropmgmt[6].local_mask,
            qos_msg->egress.cos_dropmgmt[7].local_mask);
  
  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_dropmgmt->dropMgmttype     = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->egress.cos_dropmgmt[0].dropMgmtType,
            qos_msg->egress.cos_dropmgmt[1].dropMgmtType,
            qos_msg->egress.cos_dropmgmt[2].dropMgmtType,
            qos_msg->egress.cos_dropmgmt[3].dropMgmtType,
            qos_msg->egress.cos_dropmgmt[4].dropMgmtType,
            qos_msg->egress.cos_dropmgmt[5].dropMgmtType,
            qos_msg->egress.cos_dropmgmt[6].dropMgmtType,
            qos_msg->egress.cos_dropmgmt[7].dropMgmtType);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_dropmgmt->wred_decayExp    = [ %u %u %u %u %u %u %u %u ]",
            qos_msg->egress.cos_dropmgmt[0].wred_decayExp,
            qos_msg->egress.cos_dropmgmt[1].wred_decayExp,
            qos_msg->egress.cos_dropmgmt[2].wred_decayExp,
            qos_msg->egress.cos_dropmgmt[3].wred_decayExp,
            qos_msg->egress.cos_dropmgmt[4].wred_decayExp,
            qos_msg->egress.cos_dropmgmt[5].wred_decayExp,
            qos_msg->egress.cos_dropmgmt[6].wred_decayExp,
            qos_msg->egress.cos_dropmgmt[7].wred_decayExp);

  for (i=0; i<6; i++)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_dropmgmt->dp_threshold[%u]: local_mask = [ 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ]", i+1,
              qos_msg->egress.cos_dropmgmt[0].dp_thresholds[i].local2_mask,
              qos_msg->egress.cos_dropmgmt[1].dp_thresholds[i].local2_mask,
              qos_msg->egress.cos_dropmgmt[2].dp_thresholds[i].local2_mask,
              qos_msg->egress.cos_dropmgmt[3].dp_thresholds[i].local2_mask,
              qos_msg->egress.cos_dropmgmt[4].dp_thresholds[i].local2_mask,
              qos_msg->egress.cos_dropmgmt[5].dp_thresholds[i].local2_mask,
              qos_msg->egress.cos_dropmgmt[6].dp_thresholds[i].local2_mask,
              qos_msg->egress.cos_dropmgmt[7].dp_thresholds[i].local2_mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_dropmgmt->dp_threshold[%u]: Taildrop threshold = [ %3u %3u %3u %3u %3u %3u %3u %3u ]", i+1,
              qos_msg->egress.cos_dropmgmt[0].dp_thresholds[i].tailDrop_threshold,
              qos_msg->egress.cos_dropmgmt[1].dp_thresholds[i].tailDrop_threshold,
              qos_msg->egress.cos_dropmgmt[2].dp_thresholds[i].tailDrop_threshold,
              qos_msg->egress.cos_dropmgmt[3].dp_thresholds[i].tailDrop_threshold,
              qos_msg->egress.cos_dropmgmt[4].dp_thresholds[i].tailDrop_threshold,
              qos_msg->egress.cos_dropmgmt[5].dp_thresholds[i].tailDrop_threshold,
              qos_msg->egress.cos_dropmgmt[6].dp_thresholds[i].tailDrop_threshold,
              qos_msg->egress.cos_dropmgmt[7].dp_thresholds[i].tailDrop_threshold);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_dropmgmt->dp_threshold[%u]: WRED min.threshold = [ %3u %3u %3u %3u %3u %3u %3u %3u ]", i+1,
              qos_msg->egress.cos_dropmgmt[0].dp_thresholds[i].wred_minThreshold,
              qos_msg->egress.cos_dropmgmt[1].dp_thresholds[i].wred_minThreshold,
              qos_msg->egress.cos_dropmgmt[2].dp_thresholds[i].wred_minThreshold,
              qos_msg->egress.cos_dropmgmt[3].dp_thresholds[i].wred_minThreshold,
              qos_msg->egress.cos_dropmgmt[4].dp_thresholds[i].wred_minThreshold,
              qos_msg->egress.cos_dropmgmt[5].dp_thresholds[i].wred_minThreshold,
              qos_msg->egress.cos_dropmgmt[6].dp_thresholds[i].wred_minThreshold,
              qos_msg->egress.cos_dropmgmt[7].dp_thresholds[i].wred_minThreshold);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_dropmgmt->dp_threshold[%u]: WRED max.threshold = [ %3u %3u %3u %3u %3u %3u %3u %3u ]", i+1,
              qos_msg->egress.cos_dropmgmt[0].dp_thresholds[i].wred_maxThreshold,
              qos_msg->egress.cos_dropmgmt[1].dp_thresholds[i].wred_maxThreshold,
              qos_msg->egress.cos_dropmgmt[2].dp_thresholds[i].wred_maxThreshold,
              qos_msg->egress.cos_dropmgmt[3].dp_thresholds[i].wred_maxThreshold,
              qos_msg->egress.cos_dropmgmt[4].dp_thresholds[i].wred_maxThreshold,
              qos_msg->egress.cos_dropmgmt[5].dp_thresholds[i].wred_maxThreshold,
              qos_msg->egress.cos_dropmgmt[6].dp_thresholds[i].wred_maxThreshold,
              qos_msg->egress.cos_dropmgmt[7].dp_thresholds[i].wred_maxThreshold);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  cos_dropmgmt->dp_threshold[%u]: WRED drop prob.    = [ %3u %3u %3u %3u %3u %3u %3u %3u ]", i+1,
              qos_msg->egress.cos_dropmgmt[0].dp_thresholds[i].wred_dropProb,
              qos_msg->egress.cos_dropmgmt[1].dp_thresholds[i].wred_dropProb,
              qos_msg->egress.cos_dropmgmt[2].dp_thresholds[i].wred_dropProb,
              qos_msg->egress.cos_dropmgmt[3].dp_thresholds[i].wred_dropProb,
              qos_msg->egress.cos_dropmgmt[4].dp_thresholds[i].wred_dropProb,
              qos_msg->egress.cos_dropmgmt[5].dp_thresholds[i].wred_dropProb,
              qos_msg->egress.cos_dropmgmt[6].dp_thresholds[i].wred_dropProb,
              qos_msg->egress.cos_dropmgmt[7].dp_thresholds[i].wred_dropProb);
  }
  PT_LOG_DEBUG(LOG_CTX_MSG, "Dump finished!");

  /* Interface */
  ptin_intf.intf_type = qos_msg->intf.intf_type;
  ptin_intf.intf_id   = qos_msg->intf.intf_id;

  /* Get current interface configuration */
  memset(&qos_intf_curr,0x00,sizeof(ptin_QoS_intf_t));
  rc = ptin_QoS_intf_config_get(&ptin_intf, &qos_intf_curr);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading interface configuration (rc=%d)", rc);
    return L7_FAILURE;
  }
  trust_mode = qos_intf_curr.trust_mode;

  /* Interface configuration */

  /* Clear structures */
  memset(&qos_intf,0x00,sizeof(ptin_QoS_intf_t));
  apply_conf = L7_FALSE;

  if (qos_msg->main_mask & MSG_QOS3_INGRESS_MASK)
  {
    /* Trust mode */
    if (qos_msg->ingress.ingress_mask & MSG_QOS3_INGRESS_TRUST_MODE_MASK)
    {
      qos_intf.trust_mode = qos_msg->ingress.trust_mode;
      qos_intf.mask |= PTIN_QOS_INTF_TRUSTMODE_MASK;
      /* Save new trust mode */
      trust_mode = qos_intf.trust_mode;
    }

    /* COS classification */
    if (qos_msg->ingress.ingress_mask & MSG_QOS3_INGRESS_COS_CLASSIF_MASK)
    {
      if (trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
      {
        /* Run all priorities */
        for (i=0; i<8; i++)
        {
          /* Is priority i to be defined? If so define CoS */
          if ((qos_msg->ingress.cos_classif.pcp_map.prio_mask >> i) & 1)
          {
            qos_intf.pktprio.mask[i] = 1;
            qos_intf.pktprio.cos[i]  = qos_msg->ingress.cos_classif.pcp_map.cos[i];

            qos_intf.mask |= PTIN_QOS_INTF_PACKETPRIO_MASK;
          }
        }
      }
      else if (trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC)
      {
        /* Run all priorities */
        for (i=0; i<8; i++)
        {
          /* Is priority i to be defined? If so define CoS */
          if ((qos_msg->ingress.cos_classif.ipprec_map.prio_mask >> i) & 1)
          {
            qos_intf.pktprio.mask[i] = 1;
            qos_intf.pktprio.cos[i]  = qos_msg->ingress.cos_classif.pcp_map.cos[i];

            qos_intf.mask |= PTIN_QOS_INTF_PACKETPRIO_MASK;
          }
        }
      }
      else if (trust_mode == L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
      {
        /* Run all priorities */
        for (i=0; i<64; i++)
        {
          /* Is priority i to be defined? If so define CoS */
          if ((qos_msg->ingress.cos_classif.dscp_map.prio_mask[i/32] >> (i%32)) & 1)
          {
            qos_intf.pktprio.mask[i/8] |= 1 << (i%8);
            qos_intf.pktprio.cos[i/8]  |= (qos_msg->ingress.cos_classif.dscp_map.cos[i] & 0xf) << ((i%8)*4);

            qos_intf.mask |= PTIN_QOS_INTF_PACKETPRIO_MASK;
          }
        }
      }
    }
    /* COS Policer */
    if (qos_msg->ingress.ingress_mask & MSG_QOS3_INGRESS_COS_POLICER_MASK)
    {
      /* Run all CoS */
      for (i=0; i<8; i++)
      {
        if (qos_msg->ingress.cos_policer[i].local_mask == 0)  continue;

        memset(&meter, 0x00, sizeof(meter));
        meter.cir = qos_msg->ingress.cos_policer[i].cir; 
        meter.eir = qos_msg->ingress.cos_policer[i].eir;
        meter.cbs = qos_msg->ingress.cos_policer[i].cbs;
        meter.ebs = qos_msg->ingress.cos_policer[i].ebs;

        rc = ptin_QoS_intf_cos_policer_set(&ptin_intf, i, &meter);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_MSG,"Error applying interface configuration to ptin_intf %u/%u, cos=%u: cir=%u eir=%u cbs=%u ebs=%u (rc=%d)",
                  ptin_intf.intf_type,ptin_intf.intf_id, i, meter.cir, meter.eir, meter.cbs, meter.ebs, rc);
          rc_global = rc;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_MSG,"Interface successfully configured to ptin_intf %u/%u, cos=%u: cir=%u eir=%u cbs=%u ebs=%u",
                    ptin_intf.intf_type,ptin_intf.intf_id, i, meter.cir, meter.eir, meter.cbs, meter.ebs);
        }
      }
    }
  }
  if (qos_msg->main_mask & MSG_QOS3_EGRESS_MASK)
  {
    /* Shaping rate */
    if (qos_msg->egress.egress_mask & MSG_QOS3_EGRESS_INTF_SHAPER_MASK)
    {
      qos_intf.shaping_rate = qos_msg->egress.shaping_rate;
      qos_intf.mask |= PTIN_QOS_INTF_SHAPINGRATE_MASK;
    }
  }
  /* Apply configuration? */
  apply_conf = (qos_intf.mask != 0);
  /* Apply interface configuration */
  if (apply_conf)
  {
    PT_LOG_TRACE(LOG_CTX_MSG,"Applying Interface configurations...");
    /* Execute priority map configuration */
    rc = ptin_QoS_intf_config_set(&ptin_intf, &qos_intf);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error applying interface configuration (rc=%d)", rc);
      rc_global = rc;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_MSG,"Interface successfully configured");
    }
  }

  /* COS configuration */

  /* Clear structures */
  memset(qos_cos,0x00,sizeof(qos_cos));
  apply_conf = L7_FALSE;

  if (qos_msg->main_mask & MSG_QOS3_EGRESS_MASK)
  {
    /* Scheduler configurations */
    if (qos_msg->egress.egress_mask & MSG_QOS3_EGRESS_COS_SCHEDULER_MASK)
    {
      /* Run all QoS */
      for (i=0; i<8; i++)
      {
        /* Scheduler type */
        if (qos_msg->egress.cos_scheduler[i].local_mask & MSG_QOS3_EGRESS_COS_SCHEDULER_TYPE_MASK)
        {
          qos_cos[i].scheduler_type = qos_msg->egress.cos_scheduler[i].schedulerType;
          qos_cos[i].mask |= PTIN_QOS_COS_SCHEDULER_MASK;
          apply_conf = L7_TRUE;
        }
        /* WRR Weight */
        if (qos_msg->egress.cos_scheduler[i].local_mask & MSG_QOS3_EGRESS_COS_SCHEDULER_WRR_WEIGHT_MASK)
        {
          qos_cos[i].wrrSched_weight = qos_msg->egress.cos_scheduler[i].wrrSched_weight;
          qos_cos[i].mask |= PTIN_QOS_COS_WRR_WEIGHT_MASK;
          apply_conf = L7_TRUE;
        }
      }
    }
    /* COS Shaper configurations */
    if (qos_msg->egress.egress_mask & MSG_QOS3_EGRESS_COS_SHAPER_MASK)
    {
      /* Run all QoS */
      for (i=0; i<8; i++)
      {
        /* Minimum bandwidth */
        if (qos_msg->egress.cos_shaper[i].local_mask & MSG_QOS3_EGRESS_COS_SHAPER_MIN_BW_MASK)
        {
          qos_cos[i].min_bandwidth = qos_msg->egress.cos_shaper[i].min_bandwidth;
          qos_cos[i].mask |= PTIN_QOS_COS_BW_MIN_MASK;
          apply_conf = L7_TRUE;
        }
        /* Maximum bandwidth */
        if (qos_msg->egress.cos_shaper[i].local_mask & MSG_QOS3_EGRESS_COS_SHAPER_MAX_BW_MASK)
        {
          qos_cos[i].max_bandwidth = qos_msg->egress.cos_shaper[i].max_bandwidth;
          qos_cos[i].mask |= PTIN_QOS_COS_BW_MAX_MASK;
          PT_LOG_TRACE(LOG_CTX_MSG,"Interface successfully configured %u", qos_msg->egress.cos_shaper[i].max_bandwidth);
          apply_conf = L7_TRUE;
        }
      }
    }
  }
  /* Apply configuration */
  if (apply_conf)
  {
    PT_LOG_TRACE(LOG_CTX_MSG,"Applying QoS configurations...");
    rc = ptin_QoS_cos_config_set(&ptin_intf, (L7_uint8)-1, qos_cos); 
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error configuring QoS (rc=%d)",rc);
      rc_global = rc;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_MSG,"QoS successfully configured");
    }
  }

  /* DROP configuration */

  /* Clear structures */
  memset(qos_drop,0x00,sizeof(qos_drop));
  apply_conf = L7_FALSE;

  
  if (qos_msg->main_mask & MSG_QOS3_EGRESS_MASK)
  {
    if (qos_msg->egress.egress_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_MASK)
    {
      /* Run all QoS */
      for (i=0; i<8; i++)
      {

        /* Drop MGMT type */
        if (qos_msg->egress.cos_dropmgmt[i].local_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_TYPE_MASK)
        {
          qos_drop[i].queue_management_type = qos_msg->egress.cos_dropmgmt[i].dropMgmtType;
          qos_drop[i].mask |= PTIN_QOS_COS_QUEUE_MANGM_MASK;
          apply_conf = L7_TRUE;
        }
        /* WRED decay exponent */
        if (qos_msg->egress.cos_dropmgmt[i].local_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_WRED_DECAYEXP_MASK)
        {
          qos_drop[i].wred_decayExp = qos_msg->egress.cos_dropmgmt[i].wred_decayExp;
          qos_drop[i].mask |= PTIN_QOS_COS_WRED_DECAY_EXP_MASK;
          apply_conf = L7_TRUE;
        }

        /* Run all DP levels */
        for (j = 0; j < 4; j++)
        {
          /* Taildrop threshold */
          if (qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].local2_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_TAILDROP_MAX_MASK)
          {
            qos_drop[i].dp[j].taildrop_threshold = qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].tailDrop_threshold;
            qos_drop[i].dp[j].local_mask |= PTIN_QOS_COS_DP_TAILDROP_THRESH_MASK;
            qos_drop[i].mask |= PTIN_QOS_COS_TAIL_THRESHOLDS_MASK; 
            apply_conf = L7_TRUE;
            PT_LOG_TRACE(LOG_CTX_MSG,"Interface successfully configured %u", qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].tailDrop_threshold);
          }
          /* WRED min threshold */
          if (qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].local2_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_WRED_MIN_MASK)
          {
            qos_drop[i].dp[j].wred_min_threshold = qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].wred_minThreshold;
            qos_drop[i].dp[j].local_mask |= PTIN_QOS_COS_DP_WRED_THRESH_MIN_MASK;
            qos_drop[i].mask |= PTIN_QOS_COS_WRED_THRESHOLDS_MASK;
            apply_conf = L7_TRUE;
          }
          /* WRED max threshold */
          if (qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].local2_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_WRED_MAX_MASK)
          {
            qos_drop[i].dp[j].wred_max_threshold = qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].wred_maxThreshold;
            qos_drop[i].dp[j].local_mask |= PTIN_QOS_COS_DP_WRED_THRESH_MAX_MASK;
            qos_drop[i].mask |= PTIN_QOS_COS_WRED_THRESHOLDS_MASK;
            apply_conf = L7_TRUE;
          }
          /* WRED drop probability */
          if (qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].local2_mask & MSG_QOS3_EGRESS_COS_DROPMGMT_THRESHOLD_WRED_DROPPROB_MASK)
          {
            qos_drop[i].dp[j].wred_drop_prob = qos_msg->egress.cos_dropmgmt[i].dp_thresholds[j].wred_dropProb;
            qos_drop[i].dp[j].local_mask |= PTIN_QOS_COS_DP_WRED_DROP_PROB_MASK;
            qos_drop[i].mask |= PTIN_QOS_COS_WRED_THRESHOLDS_MASK;
            apply_conf = L7_TRUE;
          }
        }
      }
    }
  }
  /* Apply configuration */

  if (apply_conf)
  {
    PT_LOG_TRACE(LOG_CTX_MSG,"Applying Drop Management configurations...");
    rc = ptin_QoS_drop_config_set(&ptin_intf, (L7_uint8)-1, qos_drop);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error configuring QoS Drop management (rc=%d)",rc);
      rc_global = rc;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_MSG,"QoS Drop management successfully configurted");
    }
  }

  if (rc_global==L7_SUCCESS)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG,"Success applying QoS configurations to all CoS");
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error applying QoS configurations to all CoS (rc_global=%d)", rc_global);
  }

  return rc_global;
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

  slot_id = ENDIAN_SWAP8(lagInfo->SlotId);

  /* Determine loop range (LAG index [0..PTIN_SYSTEM_N_LAGS[) */
  if (ENDIAN_SWAP8(lagInfo->id) >= PTIN_SYSTEM_N_LAGS)
  {
    start = 0;
    end   = PTIN_SYSTEM_N_LAGS-1;
  }
  else
  {
    start = (L7_uint32) ENDIAN_SWAP8(lagInfo->id);
    end   = (L7_uint32) ENDIAN_SWAP8(lagInfo->id);
  }

  /* Loop that works for one LAG or for all of them */
  for (lag_idx=start, array_idx=0; lag_idx<=end; lag_idx++)
  {
    if (!ptin_intf_lag_exists(lag_idx))
      continue;

    ptinLagConf.lagId = lag_idx;

    if (ptin_intf_LagConfig_get(&ptinLagConf) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error reading LAG# %u", lag_idx);
      return L7_FAILURE;
    }

    /* Copy data to the output message structure */
    lagInfo[array_idx].SlotId           = ENDIAN_SWAP8(slot_id);
    lagInfo[array_idx].id               = ENDIAN_SWAP8((L7_uint8) ptinLagConf.lagId);
    lagInfo[array_idx].admin            = ENDIAN_SWAP8(ptinLagConf.admin);
    lagInfo[array_idx].static_enable    = ENDIAN_SWAP8(ptinLagConf.static_enable);
    lagInfo[array_idx].stp_enable       = ENDIAN_SWAP8(ptinLagConf.stp_enable);
    lagInfo[array_idx].loadBalance_mode = ENDIAN_SWAP8(ptinLagConf.loadBalance_mode);
    lagInfo[array_idx].members_pbmp32[0]= ENDIAN_SWAP32((L7_uint32) ptinLagConf.members_pbmp64);
    lagInfo[array_idx].members_pbmp32[1]= ENDIAN_SWAP32((L7_uint32) (ptinLagConf.members_pbmp64 >> 32));

    PT_LOG_DEBUG(LOG_CTX_MSG, "LAG# %2u", (L7_uint32) ENDIAN_SWAP8(lagInfo[array_idx].id));
    PT_LOG_DEBUG(LOG_CTX_MSG, " .admin            = %u", ENDIAN_SWAP8(lagInfo[array_idx].admin));
    PT_LOG_DEBUG(LOG_CTX_MSG, " .static_enable    = %u", ENDIAN_SWAP8(lagInfo[array_idx].static_enable));
    PT_LOG_DEBUG(LOG_CTX_MSG, " .stp_enable       = %u", ENDIAN_SWAP8(lagInfo[array_idx].stp_enable));
    PT_LOG_DEBUG(LOG_CTX_MSG, " .loadBalance_mode = %u", ENDIAN_SWAP8(lagInfo[array_idx].loadBalance_mode));
    PT_LOG_DEBUG(LOG_CTX_MSG, " .members_pbmp[0]  = 0x%08X", ENDIAN_SWAP32(lagInfo[array_idx].members_pbmp32[0]));
    PT_LOG_DEBUG(LOG_CTX_MSG, " .members_pbmp[1]  = 0x%08X", ENDIAN_SWAP32(lagInfo[array_idx].members_pbmp32[1]));

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
  if (0 == ENDIAN_SWAP8(lagInfo->id))
  {
    if (ptin_intf_lag_exists((L7_uint32) ENDIAN_SWAP8(lagInfo->id)))
    {
      return L7_SUCCESS;
    }
    ptinLagConf.stp_enable       = 0;//DISABLED;
    ptinLagConf.members_pbmp64   = 1ULL<<(PTIN_SYSTEM_N_ETH+1) |   1ULL<<PTIN_SYSTEM_N_ETH;;
  }
  else
#endif
  {
   ptinLagConf.stp_enable       = ENDIAN_SWAP8(lagInfo->stp_enable);
   ptinLagConf.members_pbmp64   = ENDIAN_SWAP32((L7_uint64)lagInfo->members_pbmp32[0]);
   ptinLagConf.members_pbmp64  |= ENDIAN_SWAP32(((L7_uint64)lagInfo->members_pbmp32[1]) << 32);
  }

  /* Copy data from msg to ptin structure */
  ptinLagConf.lagId            = (L7_uint32) ENDIAN_SWAP8(lagInfo->id);
  ptinLagConf.admin            = ENDIAN_SWAP8(lagInfo->admin);
  ptinLagConf.static_enable    = ENDIAN_SWAP8(lagInfo->static_enable);
  //ptinLagConf.stp_enable       = ENDIAN_SWAP8(lagInfo->stp_enable);
  ptinLagConf.loadBalance_mode = ENDIAN_SWAP8(lagInfo->loadBalance_mode);
  //ptinLagConf.members_pbmp64   = ENDIAN_SWAP32((L7_uint64)lagInfo->members_pbmp32[0]);
  //ptinLagConf.members_pbmp64  |= ENDIAN_SWAP32(((L7_uint64)lagInfo->members_pbmp32[1]) << 32);

  PT_LOG_DEBUG(LOG_CTX_MSG, "LAG# %2u", (L7_uint32) ENDIAN_SWAP8(lagInfo->id));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .admin            = %u",     ENDIAN_SWAP8(lagInfo->admin));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .static_enable    = %u",     ENDIAN_SWAP8(lagInfo->static_enable));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .stp_enable       = %u",     ENDIAN_SWAP8(lagInfo->stp_enable));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .loadBalance_mode = %u",     ENDIAN_SWAP8(lagInfo->loadBalance_mode));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .members_pbmp[0]  = 0x%08X", ENDIAN_SWAP32(lagInfo->members_pbmp32[0]));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .members_pbmp[1]  = 0x%08X", ENDIAN_SWAP32(lagInfo->members_pbmp32[1]));

  if (ptin_intf_Lag_create(&ptinLagConf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Failed to create LAG# %u", (L7_uint32) ENDIAN_SWAP8(lagInfo->id));
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
  PT_LOG_DEBUG(LOG_CTX_MSG, "LAG# %2u", (L7_uint32) ENDIAN_SWAP8(lagInfo->id));

  if (ptin_intf_Lag_delete((L7_uint32) ENDIAN_SWAP8(lagInfo->id)) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Failed to delete LAG# %u", (L7_uint32) ENDIAN_SWAP8(lagInfo->id));
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "LAG# %u: successfully deleted", (L7_uint32) ENDIAN_SWAP8(lagInfo->id));

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

  slot_id = ENDIAN_SWAP8(lagStatus->SlotId);

  /* Determine loop range (LAG index [1..PTIN_SYSTEM_N_LAGS]) */
  if (ENDIAN_SWAP8(lagStatus->id) >= PTIN_SYSTEM_N_LAGS)
  {
    start = 0;
    end   = PTIN_SYSTEM_N_LAGS-1;
  }
  else
  {
    start = ENDIAN_SWAP8(lagStatus->id);
    end   = ENDIAN_SWAP8(lagStatus->id);
  }

  /* Loop that works for one LAG or for all of them */
  for (lag_idx=start, array_idx=0; lag_idx<=end; lag_idx++)
  {
    if (!ptin_intf_lag_exists(lag_idx))
      continue;

    ptinLagStatus.lagId = lag_idx;

    if (ptin_intf_LagStatus_get(&ptinLagStatus) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error getting status of LAG# %u", lag_idx);
      return L7_FAILURE;
    }

    /* Copy data to the output message structure */
    lagStatus[array_idx].SlotId               = ENDIAN_SWAP8(slot_id);
    lagStatus[array_idx].id                   = ENDIAN_SWAP8((L7_uint8) ptinLagStatus.lagId);
    lagStatus[array_idx].admin                = ENDIAN_SWAP8(ptinLagStatus.admin);
    lagStatus[array_idx].link_status          = ENDIAN_SWAP8(ptinLagStatus.link_status);
    lagStatus[array_idx].port_channel_type    = ENDIAN_SWAP8(ptinLagStatus.port_channel_type);
    lagStatus[array_idx].members_pbmp32[0]        = ENDIAN_SWAP32((L7_uint32)ptinLagStatus.members_pbmp64);
    lagStatus[array_idx].members_pbmp32[1]        = ENDIAN_SWAP32((L7_uint32)(ptinLagStatus.members_pbmp64 >> 32));
    lagStatus[array_idx].active_members_pbmp32[0] = ENDIAN_SWAP32((L7_uint32)ptinLagStatus.active_members_pbmp64);
    lagStatus[array_idx].active_members_pbmp32[1] = ENDIAN_SWAP32((L7_uint32)(ptinLagStatus.active_members_pbmp64 >> 32));

    PT_LOG_DEBUG(LOG_CTX_MSG, "LAG# %2u", (L7_uint32) ENDIAN_SWAP8(lagStatus[array_idx].id));
    PT_LOG_DEBUG(LOG_CTX_MSG, " .admin                  = %s",     ENDIAN_SWAP8(lagStatus[array_idx].admin) ? "ON" : "OFF");
    PT_LOG_DEBUG(LOG_CTX_MSG, " .link_status            = %s",     ENDIAN_SWAP8(lagStatus[array_idx].link_status) ? "UP" : "DOWN");
    PT_LOG_DEBUG(LOG_CTX_MSG, " .port_channel_type      = %s",     ENDIAN_SWAP8(lagStatus[array_idx].port_channel_type) ? "STATIC" : "DYNAMIC");
    PT_LOG_DEBUG(LOG_CTX_MSG, " .members_pbmp[0]        = 0x%08X", ENDIAN_SWAP32(lagStatus[array_idx].members_pbmp32[0]));
    PT_LOG_DEBUG(LOG_CTX_MSG, " .members_pbmp[1]        = 0x%08X", ENDIAN_SWAP32(lagStatus[array_idx].members_pbmp32[1]));
    PT_LOG_DEBUG(LOG_CTX_MSG, " .active_members_pbmp[0] = 0x%08X", ENDIAN_SWAP32(lagStatus[array_idx].active_members_pbmp32[0]));
    PT_LOG_DEBUG(LOG_CTX_MSG, " .active_members_pbmp[1] = 0x%08X", ENDIAN_SWAP32(lagStatus[array_idx].active_members_pbmp32[1]));

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
    ptinAdminState.port              = (L7_uint32) ENDIAN_SWAP8(adminState[i].id);
    ptinAdminState.state_aggregation = ENDIAN_SWAP8(adminState[i].state_aggregation);
    ptinAdminState.lacp_activity     = ENDIAN_SWAP8(adminState[i].lacp_activity);
    ptinAdminState.lacp_timeout      = ENDIAN_SWAP8(adminState[i].lacp_timeout);

    /* Apply settings */
    if (ptin_intf_LACPAdminState_set(&ptinAdminState) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error setting Port# %u LACP Admin State", ptinAdminState.port);
      return L7_FAILURE;
    }

    PT_LOG_DEBUG(LOG_CTX_MSG, "Port# %2u LACP Admin State configured", ptinAdminState.port);
    PT_LOG_DEBUG(LOG_CTX_MSG, " .state_aggregation = %s", ptinAdminState.state_aggregation?"Enabled":"Disabled");
    PT_LOG_DEBUG(LOG_CTX_MSG, " .lacp_activity     = %s", ptinAdminState.lacp_activity?"True":"False");
    PT_LOG_DEBUG(LOG_CTX_MSG, " .lacp_timeout      = %s", ptinAdminState.lacp_timeout?"Short":"Long");
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

  slot_id = ENDIAN_SWAP8(adminState->SlotId);

  /* Determine loop range (Port index [0..PTIN_SYSTEM_N_PORTS[) */
  if (ENDIAN_SWAP8(adminState->id) >= PTIN_SYSTEM_N_PORTS)
  {
    start = 0;
    end   = PTIN_SYSTEM_N_PORTS-1;
  }
  else
  {
    start = ENDIAN_SWAP8(adminState->id);
    end   = ENDIAN_SWAP8(adminState->id);
  }

  /* Loop that works for one Port or for all of them */
  for (port_idx=start, array_idx=0; port_idx<=end; port_idx++)
  {
    ptinAdminState.port = port_idx;

    if (ptin_intf_LACPAdminState_get(&ptinAdminState) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error getting admin status of Port# %u", port_idx);
      return L7_FAILURE;
    }

    /* Copy data to the output message structure */
    adminState[array_idx].SlotId            = ENDIAN_SWAP8(slot_id);
    adminState[array_idx].id                = ENDIAN_SWAP8(ptinAdminState.port);
    adminState[array_idx].state_aggregation = ENDIAN_SWAP8(ptinAdminState.state_aggregation);
    adminState[array_idx].lacp_activity     = ENDIAN_SWAP8(ptinAdminState.lacp_activity);
    adminState[array_idx].lacp_timeout      = ENDIAN_SWAP8(ptinAdminState.lacp_timeout);

    PT_LOG_DEBUG(LOG_CTX_MSG, "Port# %2u", (L7_uint32) ENDIAN_SWAP8(adminState[array_idx].id));
    PT_LOG_DEBUG(LOG_CTX_MSG, " .state_aggregation = %s", ENDIAN_SWAP8(adminState[array_idx].state_aggregation) ? "Enabled" : "Disabled");
    PT_LOG_DEBUG(LOG_CTX_MSG, " .lacp_activity     = %s", ENDIAN_SWAP8(adminState[array_idx].lacp_activity) ? "True" : "False");
    PT_LOG_DEBUG(LOG_CTX_MSG, " .lacp_timeout      = %s", ENDIAN_SWAP8(adminState[array_idx].lacp_timeout) ? "Short" : "Long");

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

  slot_id = ENDIAN_SWAP8(lagStats->SlotId);

  /* Determine loop range (Port index [0..PTIN_SYSTEM_N_PORTS[) */
  if (ENDIAN_SWAP8(lagStats->id) >= PTIN_SYSTEM_N_PORTS)
  {
    start = 0;
    end   = PTIN_SYSTEM_N_PORTS-1;
  }
  else
  {
    start = ENDIAN_SWAP8(lagStats->id);
    end   = ENDIAN_SWAP8(lagStats->id);
  }

  /* Loop that works for one Port or for all of them */
  for (port_idx=start, array_idx=0; port_idx<=end; port_idx++)
  {
    ptinLagStats.port = port_idx;

    if (ptin_intf_LACPStats_get(&ptinLagStats) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error getting status of Port# %u", port_idx);
      return L7_FAILURE;
    }

    /* Copy data to the output message structure */
    lagStats[array_idx].SlotId     = ENDIAN_SWAP8(slot_id);
    lagStats[array_idx].id         = ENDIAN_SWAP8(ptinLagStats.port);
    lagStats[array_idx].LACPdus_rx = ENDIAN_SWAP32(ptinLagStats.LACPdus_rx);
    lagStats[array_idx].LACPdus_tx = ENDIAN_SWAP32(ptinLagStats.LACPdus_tx);

    PT_LOG_DEBUG(LOG_CTX_MSG, "Port# %2u", (L7_uint32) ENDIAN_SWAP8(lagStats[array_idx].id));
    PT_LOG_DEBUG(LOG_CTX_MSG, " .LACPdus_rx = %u", ENDIAN_SWAP32(lagStats[array_idx].LACPdus_rx));
    PT_LOG_DEBUG(LOG_CTX_MSG, " .LACPdus_tx = %u", ENDIAN_SWAP32(lagStats[array_idx].LACPdus_tx));

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
  if (ENDIAN_SWAP8(lagStats->id) >= PTIN_SYSTEM_N_PORTS)
  {
    start = 0;
    end   = PTIN_SYSTEM_N_PORTS-1;
  }
  else
  {
    start = ENDIAN_SWAP8(lagStats->id);
    end   = ENDIAN_SWAP8(lagStats->id);
  }

  /* Loop that works for one Port or for all of them */
  for (port_idx=start, array_idx=0; port_idx<=end; port_idx++)
  {
    ptinLagStats.port = port_idx;

    if (ptin_intf_LACPStats_clear(&ptinLagStats) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error clearing LACP statistics of Port# %u", port_idx);
      return L7_FAILURE;
    }

    PT_LOG_DEBUG(LOG_CTX_MSG, "Port# %2u: LACP statistics successfully cleared", port_idx);
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
  L7_uint32 aging_time;

  PT_LOG_DEBUG(LOG_CTX_MSG, "Message function '%s' is being executed",__FUNCTION__);

  /* Check mask */
  if (ENDIAN_SWAP32(switch_config->mask) & 0x0001)
  {
    aging_time = ENDIAN_SWAP32(switch_config->aging_time);

    /* Set aging time */
    if (ptin_l2_mac_aging_set(aging_time)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error setting aging time to %u seconds", aging_time);
      return L7_FAILURE;
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Success setting aging time to %u seconds", aging_time);
    }

    /* Set ARP timeout to be the same as MAC age time */
    if(L7_SUCCESS != usmDbIpArpAgeTimeSet(1, aging_time))
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error setting ARP timeout to %u seconds", aging_time);
      return L7_FAILURE;
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Success setting ARP timeout to %u seconds", aging_time);
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
  L7_uint32 aging_time;

  PT_LOG_DEBUG(LOG_CTX_MSG, "Message function '%s' is being executed",__FUNCTION__);

  switch_config->mask = ENDIAN_SWAP32(0x0000);

  if (ptin_l2_mac_aging_get(&aging_time) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error getting aging time");
    return L7_FAILURE;
  }
  else
  {
    switch_config->aging_time = ENDIAN_SWAP32(aging_time);
    switch_config->mask |= ENDIAN_SWAP32(0x0001);

    PT_LOG_DEBUG(LOG_CTX_MSG, "Success getting aging time (%u seconds)", ENDIAN_SWAP32(switch_config->aging_time));
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

  PT_LOG_DEBUG(LOG_CTX_MSG, "Message function '%s' is being executed",__FUNCTION__);

  /* Validate arguments */
  if (mac_table==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid argument");
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG," SlotId       = %u",mac_table->intro.slotId);
  PT_LOG_DEBUG(LOG_CTX_MSG," StartEntryId = %u",ENDIAN_SWAP32(mac_table->intro.startEntryId));
  PT_LOG_DEBUG(LOG_CTX_MSG," NumEntries   = %u",ENDIAN_SWAP32(mac_table->intro.numEntries));

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
    PT_LOG_WARN(LOG_CTX_MSG," StartEntryId is higher than 999");
    return L7_SUCCESS;
  }

  /* Validate number of entries */
  if (startId+numEntries>=maxSize)
  {
    numEntries = maxSize - startId;
    PT_LOG_WARN(LOG_CTX_MSG," First majoration of Number of entries to %u",numEntries);
  }
  if (numEntries>MSG_CMDGET_MAC_TABLE_MAXENTRIES)
  {
    numEntries = MSG_CMDGET_MAC_TABLE_MAXENTRIES;
    PT_LOG_WARN(LOG_CTX_MSG," Second majoration of entries limited to %u",numEntries);
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
      PT_LOG_ERR(LOG_CTX_MSG,"Error loading table");
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
    PT_LOG_ERR(LOG_CTX_MSG,"Error getting MAC list");
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
    mac_table->entry[i].gem_id         = entries_list[i].gem_id;
    mac_table->entry[i].static_entry   = entries_list[i].static_entry;

    ENDIAN_SWAP32_MOD(mac_table->entry[i].evcId);
    ENDIAN_SWAP16_MOD(mac_table->entry[i].vlanId);
    ENDIAN_SWAP16_MOD(mac_table->entry[i].gem_id);

  }

  /* Update number of entries */
  mac_table->intro.numEntries = numEntries;
  ENDIAN_SWAP32_MOD(mac_table->intro.numEntries);

#endif

  return L7_SUCCESS;
}

/**
 * Remove an address from the L2 table
 * 
 * @param mac_table: Mac list structure 
 * @param numEntries: Number of entries 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_l2_macTable_remove(msg_switch_mac_table_entry_t *mac_table, L7_uint16 numEntries)
{
  L7_uint32 i;
  ptin_switch_mac_entry entry;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_DEBUG(LOG_CTX_MSG, "Message function '%s' being executed",__FUNCTION__);

  /* Validate arguments */
  if (mac_table==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid argument");
    return L7_FAILURE;
  }

  /* Consider only a maximum of 256 elements */
  if (numEntries > MSG_CMDGET_MAC_TABLE_MAXENTRIES)
  {
    PT_LOG_WARN(LOG_CTX_MSG,"numEntries limited from %u to %u", numEntries, MSG_CMDGET_MAC_TABLE_MAXENTRIES);
    numEntries = MSG_CMDGET_MAC_TABLE_MAXENTRIES;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG," NumEntries   = %u", numEntries);

  /* If numEntries is -1, flush all L2 MAC entries */
  if (numEntries == 0)
  {
    if (ptin_l2_mac_table_flush()!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error flushing MAC table");
      return L7_FAILURE;
    }
    else
    {
      PT_LOG_WARN(LOG_CTX_MSG,"MAC table will flushed");
      return L7_SUCCESS;
    }
  }

  /* Remove all elements */
  for (i = 0; i < numEntries; i++)
  {
    entry.entryId = 0;
    memcpy(entry.addr, mac_table[i].entry.addr, sizeof(L7_uint8)*6);
    entry.evcId          = mac_table[i].entry.evcId;
    entry.vlanId         = mac_table[i].entry.vlanId;
    entry.intf.intf_type = mac_table[i].entry.intf.intf_type;
    entry.intf.intf_id   = mac_table[i].entry.intf.intf_id;
    entry.static_entry   = mac_table[i].entry.static_entry;

    if (ptin_l2_mac_table_entry_remove(&entry) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error removing index entry %u",i);
      rc = L7_FAILURE;
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_MSG,"Success adding index entry %u",i);
    }
  }

  return rc;
}

/**
 * Add an address to the L2 table
 * 
 * @param mac_table: Mac list structure 
 * @param numEntries: Number of entries
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_l2_macTable_add(msg_switch_mac_table_entry_t *mac_table, L7_uint16 numEntries)
{
  L7_uint32 i;
  ptin_switch_mac_entry entry;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_DEBUG(LOG_CTX_MSG, "Message function '%s' being executed",__FUNCTION__);

  /* Validate arguments */
  if (mac_table==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid argument");
    return L7_FAILURE;
  }

  /* Consider only a maximum of 256 elements */
  if (numEntries > MSG_CMDGET_MAC_TABLE_MAXENTRIES)
  {
    PT_LOG_WARN(LOG_CTX_MSG,"numEntries limited from %u to %u", numEntries, MSG_CMDGET_MAC_TABLE_MAXENTRIES);
    numEntries = MSG_CMDGET_MAC_TABLE_MAXENTRIES;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG," NumEntries   = %u", numEntries);

  /* Remove all elements */
  for (i = 0; i < numEntries; i++)
  {
    entry.entryId = 0;
    memcpy(entry.addr, mac_table[i].entry.addr, sizeof(L7_uint8)*6);
    entry.evcId          = mac_table[i].entry.evcId;
    entry.vlanId         = mac_table[i].entry.vlanId;
    entry.intf.intf_type = mac_table[i].entry.intf.intf_type;
    entry.intf.intf_id   = mac_table[i].entry.intf.intf_id;
    entry.static_entry   = mac_table[i].entry.static_entry;

    if (ptin_l2_mac_table_entry_add(&entry)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error adding index entry %u",i);
      rc = L7_FAILURE;
    }
    else
    { 
      PT_LOG_DEBUG(LOG_CTX_MSG,"Success adding index entry %u",i);
    }
  }

  return rc;
}

/**
 * Configure L2 MAC Learn limit
 * 
 * @param maclimit: Mac limiting structure
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_l2_maclimit_config(msg_l2_maclimit_config_t *maclimit)
{
  ptin_l2_maclimit_t entry;
  L7_RC_t rc = L7_SUCCESS;

  ptin_intf_t ptin_intf;
  L7_uint32   intIfNum;

  PT_LOG_DEBUG(LOG_CTX_MSG, "Message function '%s' being executed",__FUNCTION__);

  /* Validate arguments */
  if (maclimit==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid argument");
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG," slotId       = %u",      ENDIAN_SWAP8(maclimit->slotId));
  PT_LOG_DEBUG(LOG_CTX_MSG," interface    = %u/%u",   ENDIAN_SWAP8(maclimit->intf.intf_type), ENDIAN_SWAP8(maclimit->intf.intf_id));
  PT_LOG_DEBUG(LOG_CTX_MSG," mask         = 0x%.8X",  ENDIAN_SWAP32(maclimit->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG," vid          = %u",      ENDIAN_SWAP16(maclimit->vid));
  PT_LOG_DEBUG(LOG_CTX_MSG," system       = %u",      ENDIAN_SWAP8(maclimit->system));
  PT_LOG_DEBUG(LOG_CTX_MSG," limit        = %u",      ENDIAN_SWAP32(maclimit->limit));
  PT_LOG_DEBUG(LOG_CTX_MSG," action       = %u",      ENDIAN_SWAP8(maclimit->action));
  PT_LOG_DEBUG(LOG_CTX_MSG," trap         = %u",      ENDIAN_SWAP8(maclimit->send_trap));

  memset(&entry, 0x00, sizeof(ptin_l2_maclimit_t));

  if ((maclimit->mask & L2_MACLIMIT_MASK_SYSTEM) & (maclimit->system))
  {
    intIfNum = L7_ALL_INTERFACES;
  }
  else
  {
    /* Get intIfNum */
    ptin_intf.intf_type = ENDIAN_SWAP8(maclimit->intf.intf_type);
    ptin_intf.intf_id   = ENDIAN_SWAP8(maclimit->intf.intf_id);  

    if (ptin_intf_ptintf2intIfNum(&ptin_intf, &intIfNum) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC,"Invalid ptin_intf: %u/%u", ptin_intf.intf_type, ptin_intf.intf_id);
      return L7_FAILURE;
    }
  }

  if (maclimit->mask & L2_MACLIMIT_MASK_LIMIT)
  {
    entry.limit = ENDIAN_SWAP32(maclimit->limit);    
  }
  else
  {
    entry.limit = 0;
  }

  if (maclimit->mask & L2_MACLIMIT_MASK_ACTION)
  {
    entry.action = ENDIAN_SWAP8(maclimit->action);
  }
  else
  {
    entry.action = -1;
  }

  if (maclimit->mask & L2_MACLIMIT_MASK_SEND_TRAP)
  {
    entry.send_trap = ENDIAN_SWAP8(maclimit->send_trap);
  }
  else
  {
    entry.send_trap = -1;
  }  

  rc = dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_L2_MACLIMIT, DAPI_CMD_SET, sizeof(ptin_l2_maclimit_t), &entry);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Failed set limit on hardware: rc:%u!", rc);
    PT_LOG_DEBUG(LOG_CTX_MSG," slotId       = %u",      ENDIAN_SWAP8(maclimit->slotId));
    PT_LOG_DEBUG(LOG_CTX_MSG," interface    = %u/%u",   ENDIAN_SWAP8(maclimit->intf.intf_type), ENDIAN_SWAP8(maclimit->intf.intf_id));
    PT_LOG_DEBUG(LOG_CTX_MSG," mask         = 0x%.8X",  ENDIAN_SWAP32(maclimit->mask));
    PT_LOG_DEBUG(LOG_CTX_MSG," vid          = %u",      ENDIAN_SWAP16(maclimit->vid));
    PT_LOG_DEBUG(LOG_CTX_MSG," system       = %u",      ENDIAN_SWAP8(maclimit->system));
    PT_LOG_DEBUG(LOG_CTX_MSG," limit        = %u",      ENDIAN_SWAP32(maclimit->limit));
    PT_LOG_DEBUG(LOG_CTX_MSG," action       = %u",      ENDIAN_SWAP8(maclimit->action));
    PT_LOG_DEBUG(LOG_CTX_MSG," trap         = %u",      ENDIAN_SWAP8(maclimit->send_trap));

    return rc;
  }

  return rc;
}

/**
 * Get L2 MAC Learn limit status
 * 
 * @param maclimit: Mac limiting structure
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_l2_maclimit_status(msg_l2_maclimit_status_t *maclimit_status)
{
  ptin_l2_maclimit_status_t entry;  
  ptin_intf_t               ptin_intf;
  L7_uint32                 intIfNum;
  L7_RC_t                   rc = L7_SUCCESS;

  /* Validate arguments */
  if (maclimit_status==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid argument");
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG," slotId       = %u",      maclimit_status->slotId);
  PT_LOG_DEBUG(LOG_CTX_MSG," interface    = %u/%u",   maclimit_status->intf.intf_type, maclimit_status->intf.intf_id);

  ptin_intf.intf_type = ENDIAN_SWAP8(maclimit_status->intf.intf_type);
  ptin_intf.intf_id   = ENDIAN_SWAP8(maclimit_status->intf.intf_id);
  if (ptin_intf_ptintf2intIfNum(&ptin_intf, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Invalid ptin_intf: %u/%u", ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  } 

  memset(&entry, 0x00, sizeof(ptin_l2_maclimit_status_t));

  rc = dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_L2_MACLIMIT_STATUS, DAPI_CMD_GET, sizeof(ptin_l2_maclimit_status_t), &entry);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_EVC,"Failed to get status from hardware: rc:%u!", rc);
    PT_LOG_ERR(LOG_CTX_MSG," slotId       = %u",      maclimit_status->slotId);
    PT_LOG_ERR(LOG_CTX_MSG," interface    = %u/%u",   maclimit_status->intf.intf_type, maclimit_status->intf.intf_id);    
    return rc;
  }

  maclimit_status->number_mac_learned = entry.number_mac_learned;
  maclimit_status->status = entry.status;

  maclimit_status->mask = 0x03;

  PT_LOG_DEBUG(LOG_CTX_MSG," Status Response");
  PT_LOG_DEBUG(LOG_CTX_MSG," slotId       = %u",      maclimit_status->slotId);
  PT_LOG_DEBUG(LOG_CTX_MSG," interface    = %u/%u",   maclimit_status->intf.intf_type, maclimit_status->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," MacLearned   = %u",      maclimit_status->number_mac_learned);
  PT_LOG_DEBUG(LOG_CTX_MSG," Status       = %u",      maclimit_status->status);
  PT_LOG_DEBUG(LOG_CTX_MSG," Mask         = %u",      maclimit_status->mask);

  return rc;
}

/* Dynamic ARP Inspection *****************************************************/

/**
 * DAI global configurations
 * 
 * @param config 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_dai_global_config(msg_dai_global_settings_t *config)
{
  L7_RC_t rc, rc_global = L7_SUCCESS, rc_global_failure = L7_SUCCESS;

  /* Validate arguments */
  if (config == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Null pointer");
    return L7_FAILURE;
  }

  ENDIAN_SWAP8_MOD(config->slotId);
  ENDIAN_SWAP8_MOD(config->mask);
  ENDIAN_SWAP8_MOD(config->global_enable);
  ENDIAN_SWAP8_MOD(config->validate_smac);
  ENDIAN_SWAP8_MOD(config->validate_dmac);
  ENDIAN_SWAP8_MOD(config->validate_ipAddr);

  PT_LOG_DEBUG(LOG_CTX_MSG," SlotId = %u",          config->slotId);
  PT_LOG_DEBUG(LOG_CTX_MSG," Mask   = 0x%02x",      config->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG," Enable = %u",          config->global_enable);
  PT_LOG_DEBUG(LOG_CTX_MSG," SrcMAC Validate = %u", config->validate_smac);
  PT_LOG_DEBUG(LOG_CTX_MSG," DstMAC Validate = %u", config->validate_dmac);
  PT_LOG_DEBUG(LOG_CTX_MSG," IPAddr Validate = %u", config->validate_ipAddr);

  /* Global enable */
  if (config->mask & 0x01)
  {
    /* Always enabled */
  }

  /* SrcMAC Validation */
  if (config->mask & 0x02)
  {
    rc = usmDbDaiVerifySMacSet(config->validate_smac);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error setting SMAC validation (%u)", config->validate_smac);
      rc_global = rc;
      if (IS_FAILURE_ERROR(rc))
        rc_global_failure = rc;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_MSG, "SMAC validation set to %u", config->validate_smac);
    }
  }

  /* DstMAC Validation */
  if (config->mask & 0x04)
  {
    rc = usmDbDaiVerifyDMacSet(config->validate_dmac);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error setting DMAC validation (%u)", config->validate_smac);
      rc_global = rc;
      if (IS_FAILURE_ERROR(rc))
        rc_global_failure = rc;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_MSG, "DMAC validation set to %u", config->validate_dmac);
    }
  }

  /* IP validation */
  if (config->mask & 0x08)
  {
    rc = usmDbDaiVerifyIPSet(config->validate_ipAddr);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error setting IP validation (%u)", config->validate_ipAddr);
      rc_global = rc;
      if (IS_FAILURE_ERROR(rc))
        rc_global_failure = rc;
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_MSG, "IPAddr validation set to %u", config->validate_ipAddr);
    }
  }

  if (rc_global_failure != L7_SUCCESS)
    return rc_global_failure;

  return rc_global;
}

/**
 * DAI Interface configuration
 * 
 * @param config 
 * @param nElems 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_dai_intf_config(msg_dai_intf_settings_t *config, L7_uint nElems)
{
  L7_uint     i;
  ptin_intf_t ptin_intf;
  L7_uint32   intIfNum;
  L7_int32    rate;
  msg_dai_intf_settings_t *item;
  L7_RC_t     rc, rc_global = L7_SUCCESS, rc_global_failure = L7_SUCCESS;

  /* Validate arguments */
  if (config == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Null pointer");
    return L7_FAILURE;
  }

  /* Run all elements */
  for (i = 0; i < nElems; i++)
  {
    item = &config[i];

    ENDIAN_SWAP8_MOD (item->slotId);
    ENDIAN_SWAP8_MOD (item->intf.intf_type);
    ENDIAN_SWAP8_MOD (item->intf.intf_id);
    ENDIAN_SWAP8_MOD (item->mask);
    ENDIAN_SWAP8_MOD (item->trust);
    ENDIAN_SWAP32_MOD(item->rateLimit);
    ENDIAN_SWAP32_MOD(item->burstInterval);

    PT_LOG_DEBUG(LOG_CTX_MSG," SlotId = %u",            item->slotId);
    PT_LOG_DEBUG(LOG_CTX_MSG," Mask   = 0x%02x",        item->mask);
    PT_LOG_DEBUG(LOG_CTX_MSG," Interface      = %u/%u", item->intf.intf_type, item->intf.intf_id);
    PT_LOG_DEBUG(LOG_CTX_MSG," Rate Limit     = %u",    item->rateLimit);
    PT_LOG_DEBUG(LOG_CTX_MSG," Burst Interval = %u",    item->burstInterval);

    ptin_intf.intf_type = item->intf.intf_type;
    ptin_intf.intf_id   = item->intf.intf_id;

    /* Validate interface */
    rc = ptin_intf_ptintf2intIfNum(&ptin_intf, &intIfNum);
    if (rc != L7_SUCCESS)
    {
      rc_global = rc_global_failure = L7_FAILURE;
      continue;
    }

    /* Trust interface */
    if (item->mask & 0x01)
    {
      rc = usmDbDaiIntfTrustSet(intIfNum, item->trust);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error setting trust mode (%u) for ptin_intf %u/%u",
                   item->trust, ptin_intf.intf_type, ptin_intf.intf_id);
        rc_global = rc;
        if (IS_FAILURE_ERROR(rc))
          rc_global_failure = rc;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_MSG, "Trust mode of intf %u/%u set to %u",
                     item->intf.intf_type, item->intf.intf_id, item->trust);
      }
    }
    /* Rate Limit */
    if (item->mask & 0x02)
    {
      rc = usmDbDaiIntfRateLimitSet(intIfNum, (L7_int) item->rateLimit);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error setting rate limit (%u) for ptin_intf %u/%u",
                   item->rateLimit, ptin_intf.intf_type, ptin_intf.intf_id);
        rc_global = rc;
        if (IS_FAILURE_ERROR(rc))
          rc_global_failure = rc;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_MSG, "Rate Limit of intf %u/%u set to %u",
                     item->intf.intf_type, item->intf.intf_id, item->rateLimit);
      }
    }
    /* Burst interval */
    if (item->mask & 0x04)
    {
      if (usmDbDaiIntfRateLimitGet(intIfNum, &rate) == L7_SUCCESS && rate != -1)
      {
        rc = usmDbDaiIntfBurstIntervalSet(intIfNum, item->burstInterval);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_MSG, "Error setting burst interval (%u) for ptin_intf %u/%u",
                     item->burstInterval, ptin_intf.intf_type, ptin_intf.intf_id);
          rc_global = rc;
          if (IS_FAILURE_ERROR(rc))
            rc_global_failure = rc;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_MSG, "Burst interval of intf %u/%u set to %u",
                       item->intf.intf_type, item->intf.intf_id, item->burstInterval);
        }
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_MSG, "Burst interval of intf %u/%u ignored",
                     item->intf.intf_type, item->intf.intf_id);
      }
    }
  }

  if (rc_global_failure != L7_SUCCESS)
    return rc_global_failure;

  return rc_global;
}

/* List of VLANs to be used for local purposes */
static L7_uint16 dai_intVid_list[4096];
static L7_uint dai_maxVlans = 0;

/**
 * DAI VLANs configuration
 * 
 * @param config 
 * @param nElems 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_dai_vlan_config(msg_dai_vlan_settings_t *config, L7_uint nElems)
{
  L7_uint     i, vlan_index;
  L7_uint16   vlanId;
  msg_dai_vlan_settings_t *item;
  L7_RC_t     rc, rc_global = L7_SUCCESS, rc_global_failure = L7_SUCCESS;

  /* Validate arguments */
  if (config == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Null pointer");
    return L7_FAILURE;
  }

  /* Run all elements */
  for (i = 0; i < nElems; i++)
  {
    item = &config[i];

    ENDIAN_SWAP8_MOD (item->slotId);
    ENDIAN_SWAP8_MOD (item->service.id_type);
    ENDIAN_SWAP32_MOD(item->service.id_val.evc_id);
    ENDIAN_SWAP8_MOD (item->mask);
    ENDIAN_SWAP8_MOD (item->dai_enable);
    ENDIAN_SWAP8_MOD (item->staticFlag);

    PT_LOG_DEBUG(LOG_CTX_MSG," SlotId = %u",          item->slotId);
    PT_LOG_DEBUG(LOG_CTX_MSG," service_type = %u",    item->service.id_type);
    PT_LOG_DEBUG(LOG_CTX_MSG," service_id   = %u",    item->service.id_val.evc_id);
    PT_LOG_DEBUG(LOG_CTX_MSG," Mask         = 0x%02x",item->mask);
    PT_LOG_DEBUG(LOG_CTX_MSG," DAI enable   = %u",    item->dai_enable);
    PT_LOG_DEBUG(LOG_CTX_MSG," Static Flag  = %u",    item->staticFlag);

    /* Clear list of VLANs */
    memset(dai_intVid_list, 0x00, sizeof(dai_intVid_list));
    dai_maxVlans = 0;

    /* If EVC id is provided, get related VLAN */
    if (item->service.id_type == MSG_ID_EVC_TYPE)
    {
      /* Validate EVC id */
      if (item->service.id_val.evc_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "eEVC#%u is out of range!", item->service.id_val.evc_id);
        rc_global = rc_global_failure = L7_FAILURE;
        continue;
      }
      /* EVC must be active */
      if (!ptin_evc_is_in_use(item->service.id_val.evc_id)) 
      {
        PT_LOG_ERR(LOG_CTX_MSG, "eEVC#%u is not in use!", item->service.id_val.evc_id);
        rc_global = L7_NOT_EXIST;
        continue;
      }
      /* Get internal VLAN from eEVC# */
      dai_maxVlans = 1;
      if (ptin_evc_intRootVlan_get(item->service.id_val.evc_id, &dai_intVid_list[0]) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Cannot get intVlan from eEVC#%u!", item->service.id_val.evc_id, dai_intVid_list[0]);
        rc_global = rc_global_failure = L7_FAILURE;
        continue;
      }
    }
    /* Use given VLANs range */
    else if (item->service.id_type == MSG_ID_NNIVID_TYPE)
    {
      /* Validate NNI VLAN */
      if (item->service.id_val.nni_vid < PTIN_VLAN_MIN || item->service.id_val.nni_vid > PTIN_VLAN_MAX)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "NNI VLAN %u is out of range!", item->service.id_val.nni_vid);
        rc_global = rc_global_failure = L7_FAILURE;
        continue;
      }

      /* Get int VLAN list from NNI VLAN */
      dai_maxVlans = 4096;
      if (ptin_evc_get_intVlan_fromNNIvlan(item->service.id_val.nni_vid, dai_intVid_list, &dai_maxVlans) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "NNI VLAN %u is invalid, or doesn't belong to any EVC!", item->service.id_val.nni_vid);
        rc_global = L7_NOT_EXIST;
        continue;
      }
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Invalid service type %u", item->service.id_type);
      rc_global = L7_NOT_SUPPORTED;
      continue;
    }

    PT_LOG_TRACE(LOG_CTX_MSG, "Going to process %u VLANs", dai_maxVlans);

    /* Run VLANs range: if EVC id was provided, only one iteration will be executed with vlanId=0 */
    for (vlan_index = 0; vlan_index < dai_maxVlans; vlan_index++)
    {
      vlanId = dai_intVid_list[vlan_index];

      /* Validate NNI VLAN */
      if (vlanId < PTIN_VLAN_MIN || vlanId > PTIN_VLAN_MAX)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Int. VLAN %u is out of range!", vlanId);
        rc_global = rc_global_failure = L7_FAILURE;
        continue;
      }

      /* Check if VLAN is valid */
      rc = usmDbVlanIDGet(0, vlanId);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "VLAN %u is invalid, or don't belong to any EVC!", vlanId);
        rc_global = L7_NOT_EXIST;
        continue;
      }

      /* VLAN enable */
      if (item->mask & 0x01)
      {
        rc = usmDbDaiVlanEnableSet(vlanId, item->dai_enable);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_MSG, "Error setting enable state (%u) for VLAN %u", item->dai_enable, vlanId);
          rc_global = rc;
          if (IS_FAILURE_ERROR(rc))
            rc_global_failure = rc;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_MSG, "DAI enable for VLAN %u set to %u", vlanId, item->dai_enable);
        }
      }
      /* Static flag */
      if (item->mask & 0x02)
      {
        rc = usmDbDaiVlanArpAclStaticFlagSet(vlanId, item->staticFlag);
        if (rc != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_MSG, "Error setting static flag (%u) for VLAN %u", item->staticFlag, vlanId);
          rc_global = rc;
          if (IS_FAILURE_ERROR(rc))
            rc_global_failure = rc;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_MSG, "Static flag for VLAN %u set to %u", vlanId, item->staticFlag);
        }
      }
    }
  }

  if (rc_global_failure != L7_SUCCESS)
    return rc_global_failure;

  return rc_global;
}

/**
 * Get DAI statistics
 * 
 * @param stats 
 * @param nElems 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_dai_stats_get(msg_dai_statistics_t *msg_stats, L7_uint nElems)
{
  L7_uint     i, vlan_index;
  L7_uint32   val;
  L7_uint16   vlanId;
  msg_dai_statistics_t *item;
  daiVlanStats_t        stats;
  L7_RC_t     rc, rc_global = L7_SUCCESS, rc_global_failure = L7_SUCCESS;

  /* Validate arguments */
  if (msg_stats == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Null pointer");
    return L7_FAILURE;
  }

  /* Run all elements */
  for (i = 0; i < nElems; i++)
  {
    item = &msg_stats[i];

    PT_LOG_DEBUG(LOG_CTX_MSG,"Stats index %u:", i);
    PT_LOG_DEBUG(LOG_CTX_MSG," SlotId       = %u", ENDIAN_SWAP8 (item->slotId));
    PT_LOG_DEBUG(LOG_CTX_MSG," EVC id       = %u", ENDIAN_SWAP32(item->evc_idx));
    PT_LOG_DEBUG(LOG_CTX_MSG," VLAN id      = %u", ENDIAN_SWAP16(item->vlan_id));
    PT_LOG_DEBUG(LOG_CTX_MSG," Intf         = %u", ENDIAN_SWAP8 (item->intf.intf_type), ENDIAN_SWAP8 (item->intf.intf_id));

    /* Clear list of VLANs */
    memset(dai_intVid_list, 0x00, sizeof(dai_intVid_list));
    dai_maxVlans = 0;

    /* If EVC id is provided, get related VLAN */
    if (ENDIAN_SWAP32(item->evc_idx) != (L7_uint32) -1)
    {
      /* Check range */
      if (ENDIAN_SWAP32(item->evc_idx) >= PTIN_SYSTEM_N_EXTENDED_EVCS) 
      {
        PT_LOG_ERR(LOG_CTX_MSG, "eEVC#%u is out of range!", ENDIAN_SWAP32(item->evc_idx));
        rc_global = rc_global_failure = L7_FAILURE;
        continue;
      }
      /* EVC is active? */
      if (!ptin_evc_is_in_use(ENDIAN_SWAP32(item->evc_idx)))
      {
        PT_LOG_ERR(LOG_CTX_MSG, "eEVC#%u is not in use!", ENDIAN_SWAP32(item->evc_idx));
        rc_global = L7_NOT_EXIST;
        continue;
      }
      /* Get internal VLAN from eEVC# */
      dai_maxVlans = 1;
      if (ptin_evc_intRootVlan_get(ENDIAN_SWAP32(item->evc_idx), &dai_intVid_list[0]) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Cannot get intVlan from eEVC#%u!", ENDIAN_SWAP32(item->evc_idx));
        rc_global = rc_global_failure = L7_FAILURE;
        continue;
      }
    }
    /* Use given VLANs range */
    else
    {
      if (ENDIAN_SWAP16(item->vlan_id) < PTIN_VLAN_MIN || ENDIAN_SWAP16(item->vlan_id) > PTIN_VLAN_MAX)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "VLAN %u is out of valid range!", ENDIAN_SWAP16(item->vlan_id));
        rc_global = rc_global_failure = L7_FAILURE;
        continue;
      }
      /* Get eEVC id from NNI VLAN */
      dai_maxVlans = 4096;
      if (ptin_evc_get_intVlan_fromNNIvlan(ENDIAN_SWAP16(item->vlan_id), dai_intVid_list, &dai_maxVlans) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "NNI VLAN %u is invalid, or doesn't belong to any EVC!", ENDIAN_SWAP16(item->vlan_id));
        rc_global = L7_NOT_EXIST;
        continue;
      }
    }

    PT_LOG_TRACE(LOG_CTX_MSG, "Going to process %u VLANs", dai_maxVlans);

    /* Clear results for this request */
    memset(&item->stats, 0x00, sizeof(item->stats));

    /* Run all vlans */
    for (vlan_index = 0; vlan_index < dai_maxVlans; vlan_index++)
    {
      vlanId = dai_intVid_list[vlan_index];

      /* Validate NNI VLAN */
      if (vlanId < PTIN_VLAN_MIN || vlanId > PTIN_VLAN_MAX)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Int. VLAN %u is out of range!", vlanId);
        rc_global = rc_global_failure = L7_FAILURE;
        continue;
      }

      /* Check if VLAN is valid */
      rc = usmDbVlanIDGet(0, vlanId);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "VLAN %u is invalid, or don't belong to any EVC!", vlanId);
        rc_global = L7_NOT_EXIST;
        continue;
      }

      /* VLAN is DAI enabled? */
      rc = usmDbDaiVlanEnableGet(vlanId, &val);
      if (rc != L7_SUCCESS || !val)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "VLAN %u is not used by DAI!", vlanId);
        rc_global = rc;
        if (IS_FAILURE_ERROR(rc))
          rc_global_failure = rc;
        continue;
      }

      /* Get stats */
      rc = usmDbDaiVlanStatsGet(vlanId, &stats);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error getting Stats from VLAN %u!", vlanId);
        rc_global = rc;
        if (IS_FAILURE_ERROR(rc))
          rc_global_failure = rc;
        continue;
      }

      /* Sum statistics to be returned */
      item->stats.forwarded       += ENDIAN_SWAP32(stats.forwarded);
      item->stats.dropped         += ENDIAN_SWAP32(stats.dropped);
      item->stats.dhcpDrops       += ENDIAN_SWAP32(stats.dhcpDrops);
      item->stats.dhcpPermits     += ENDIAN_SWAP32(stats.dhcpPermits);
      item->stats.aclDrops        += ENDIAN_SWAP32(stats.aclDrops + stats.sMacFailures + stats.dMacFailures + stats.ipValidFailures) ;
      item->stats.aclPermits      += ENDIAN_SWAP32(stats.aclPermits);
      item->stats.sMacFailures    += ENDIAN_SWAP32(stats.sMacFailures);
      item->stats.dMacFailures    += ENDIAN_SWAP32(stats.dMacFailures);
      item->stats.ipValidFailures += ENDIAN_SWAP32(stats.ipValidFailures);
    }
  }

  if (rc_global_failure != L7_SUCCESS)
    return rc_global_failure;

  return rc_global;
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
  if (ENDIAN_SWAP32(msgEvcConf->id) >= PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "EVC# %u is out of range [0..%u]", ENDIAN_SWAP32(msgEvcConf->id), PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  ptinEvcConf.index = ENDIAN_SWAP32(msgEvcConf->id);

  if (ptin_evc_get(&ptinEvcConf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error getting EVC# %u configuration", ptinEvcConf.index);
    return L7_FAILURE;
  }

  /* Copy data to message struct */
  msgEvcConf->id       = ENDIAN_SWAP32(ptinEvcConf.index);
  msgEvcConf->flags    = ENDIAN_SWAP32(ptinEvcConf.flags);
  msgEvcConf->type     = ENDIAN_SWAP8 (ptinEvcConf.type);
  msgEvcConf->mc_flood = ENDIAN_SWAP8 (ptinEvcConf.mc_flood);
  msgEvcConf->n_intf   = ENDIAN_SWAP8 (ptinEvcConf.n_intf);
  //memcpy(msgEvcConf->ce_vid_bmp, ptinEvcConf.ce_vid_bmp, sizeof(msgEvcConf->ce_vid_bmp));

  PT_LOG_DEBUG(LOG_CTX_MSG, "EVC# %u",              ENDIAN_SWAP32(msgEvcConf->id));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .Flags    = 0x%08X",  ENDIAN_SWAP32(msgEvcConf->flags));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .Type     = %u",      ENDIAN_SWAP8 (msgEvcConf->type));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .MC Flood = %u (%s)", ENDIAN_SWAP8 (msgEvcConf->mc_flood), ((ENDIAN_SWAP8(msgEvcConf->mc_flood)==0) ? "All" : (ENDIAN_SWAP8(msgEvcConf->mc_flood)==1) ? "Unknown" : "None"));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .Nr.Intf  = %u",      ENDIAN_SWAP8 (msgEvcConf->n_intf));

  for (i=0; i < ptinEvcConf.n_intf; i++)
  {
    msgEvcConf->intf[i].intf_id   = ENDIAN_SWAP8 (ptinEvcConf.intf[i].intf.value.ptin_intf.intf_id);
    msgEvcConf->intf[i].intf_type = ENDIAN_SWAP8 (ptinEvcConf.intf[i].intf.value.ptin_intf.intf_type);
    msgEvcConf->intf[i].mef_type  = ENDIAN_SWAP8 (ptinEvcConf.intf[i].mef_type);
    msgEvcConf->intf[i].vid       = ENDIAN_SWAP16(ptinEvcConf.intf[i].vid);

    PT_LOG_DEBUG(LOG_CTX_MSG, "   %s# %02u %s VID=%04u",
                 ((ENDIAN_SWAP8(msgEvcConf->intf[i].intf_type) == PTIN_EVC_INTF_PHYSICAL) ? "PHY":"LAG"),
                   ENDIAN_SWAP8(msgEvcConf->intf[i].intf_id),
                 ((ENDIAN_SWAP8(msgEvcConf->intf[i].mef_type) == PTIN_EVC_INTF_ROOT) ? "Root":"Leaf"),
                   ENDIAN_SWAP16(msgEvcConf->intf[i].vid));
  }

  return L7_SUCCESS;
}

/**
 * Configure QoS at the VLAN level
 * 
 * @param evc_id 
 * @param downlink : Is this a downlink configuration? 
 * @param qos : Null to only update ports
 * 
 * @return L7_RC_t 
 */
static L7_RC_t ptin_msg_qosvlan_config(L7_uint32 evc_id, L7_uint16 nni_vlan, L7_BOOL downlink,
                                       msg_CoS_classification_t *qos)
{
  L7_uint16 i;
  L7_uint32 number_of_ports;
  L7_uint16 int_vlan = 0;
  ptin_HwEthMef10Evc_t  evcConf;
  ptin_qos_vlan_t       qos_apply;

  memset(&qos_apply, 0x00, sizeof(ptin_qos_vlan_t));

  /* Determine list of ports */
  number_of_ports = 0;

  /* Only if valid EVC id is provided, get list of ports from it */
  if (evc_id < PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    /* No NNI VLAN will be used */
    nni_vlan = 0;

    /* Get internal VLAN */
    if (ptin_evc_intRootVlan_get(evc_id, &int_vlan) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error obtaining internal VLAN from eEVC %u", evc_id);
      return L7_FAILURE;
    }

    /* Get list of ports */
    memset(&evcConf, 0x00, sizeof(ptin_HwEthMef10Evc_t));
    evcConf.index = evc_id;
    if (ptin_evc_get(&evcConf) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error obtaining EVC data");
      return L7_FAILURE;
    }
    /* Add ports */
    for (i = 0; i < evcConf.n_intf; i++)
    {
      if ((!downlink && evcConf.intf[i].mef_type == PTIN_EVC_INTF_ROOT) ||   /* Uplink interface */
          ( downlink && evcConf.intf[i].mef_type == PTIN_EVC_INTF_LEAF))     /* Downlink interface */
      {
        qos_apply.ptin_port[number_of_ports++] = evcConf.intf[i].intf.value.ptin_port;
        PT_LOG_DEBUG(LOG_CTX_MSG, "Port %u added", evcConf.intf[i].intf.value.ptin_port);
      }
    }
    for (i = 0; i < number_of_ports; i++)
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Port added: ptin_port=%u", qos_apply.ptin_port[i]);
    }
  }
  /* Use NNI VLAN */
  else if (nni_vlan >= 1 && nni_vlan <= 4095)
  {
    /* No internal VLAN will be used */
    int_vlan = 0;

    /* Validate NNI VLAN */
    if (ptin_evc_get_intVlan_fromNNIvlan(nni_vlan, L7_NULLPTR, L7_NULLPTR) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "NNI Vlan %u not valid!", nni_vlan);
      return L7_FAILURE;
    }

  #if (PTIN_BOARD_IS_LINECARD)
    if (!downlink)
    {
    #if (PTIN_BOARD == PTIN_BOARD_TG16G || PTIN_BOARD == PTIN_BOARD_TG16GF)
      for (i=PTIN_SYSTEM_N_PONS; i<PTIN_SYSTEM_N_PORTS; i++)
      {
        qos_apply.ptin_port[number_of_ports++] = i;
      }
    #elif (PTIN_BOARD == PTIN_BOARD_TA48GE)
      for (i=PTIN_SYSTEM_N_ETH; i<PTIN_SYSTEM_N_PORTS; i++)
      {
        qos_apply.ptin_port[number_of_ports++] = i;
      }
    #else
      PT_LOG_ERR(LOG_CTX_MSG, "Cannot use NNI VLAN for this board!");
      return L7_FAILURE;
    #endif
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Cannot use NNI VLAN for downlink interfaces!");
      return L7_FAILURE;
    }
  #else
    PT_LOG_ERR(LOG_CTX_MSG, "Cannot use NNI VLAN for non LC cards!");
    return L7_FAILURE;
  #endif
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "No inputs provided: eEVC %u, NNIVlan %u", evc_id, nni_vlan);
    return L7_FAILURE;
  }

  /* Update number of ports and other information */
  qos_apply.number_of_ports = number_of_ports;
  qos_apply.nni_vlan        = nni_vlan;
  qos_apply.int_vlan        = int_vlan;
  qos_apply.leaf_side       = downlink;

  PT_LOG_DEBUG(LOG_CTX_MSG, "Going to configure QoS for EVC %u / internalVlan %u", evc_id, int_vlan);

  /* Configure QoS? */
  if (qos != L7_NULLPTR)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Ingress: %s", (downlink) ? "Downlink" : "Uplink");
    PT_LOG_DEBUG(LOG_CTX_MSG, "Mask      = 0x%02x", ENDIAN_SWAP8(qos->mask));
    PT_LOG_DEBUG(LOG_CTX_MSG, "TrustMode = %u",     ENDIAN_SWAP8(qos->trust_mode));

    if (ENDIAN_SWAP8(qos->mask) == 0x00) 
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "No configurations to be done");
      return L7_SUCCESS;
    }

    PT_LOG_DEBUG(LOG_CTX_MSG, "Processing QoS data");

    /* Trust mode */
    qos_apply.trust_mode   = ENDIAN_SWAP8(qos->trust_mode);
    qos_apply.pbits_remark = ENDIAN_SWAP8(qos->pbits_remark);
    memset(qos_apply.cos_map, 0xff, sizeof(qos_apply.cos_map));

    switch (ENDIAN_SWAP8(qos->trust_mode))
    {
      case L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED:
        qos_apply.cos_map[0] = 0;     /* This is the default CoS (applied to all pbits) */
        qos_apply.cos_map_size = 1;
        break;

      case L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P:
        for (i = 0; i < 8; i++)
        {
          if ((ENDIAN_SWAP8(qos->cos_classif.pcp_map.prio_mask) >> i) & 1)
          {
            qos_apply.cos_map[i] = ENDIAN_SWAP8(qos->cos_classif.pcp_map.cos[i]);
          }
        }
        qos_apply.cos_map_size = 8;
        break;
      case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC:
        for (i = 0; i < 8; i++)
        {
          if ((ENDIAN_SWAP8(qos->cos_classif.ipprec_map.prio_mask) >> i) & 1)
          {
            qos_apply.cos_map[i] = ENDIAN_SWAP8(qos->cos_classif.ipprec_map.cos[i]);
          }
        }
        qos_apply.cos_map_size = 8;
        break;

      case L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP:
        for (i = 0; i < 64; i++)
        {
          if ((ENDIAN_SWAP32(qos->cos_classif.dscp_map.prio_mask[i/32]) >> (i%32)) & 1)
          {
            qos_apply.cos_map[i] = ENDIAN_SWAP8(qos->cos_classif.dscp_map.cos[i]);
          }
        }
        qos_apply.cos_map_size = 64;
        break;

      default:
        qos_apply.cos_map_size = 0;
        break;
    }

    PT_LOG_DEBUG(LOG_CTX_MSG, "NNIVlan=%u, intVlan=%u, trust_mode=%u, remark=%u, Number of ports=%u, Number of CoS=%u",
              qos_apply.nni_vlan, qos_apply.int_vlan, qos_apply.trust_mode, qos_apply.pbits_remark, qos_apply.number_of_ports, qos_apply.cos_map_size);

    for (i = 0; i < qos_apply.cos_map_size; i++)
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "CoS(%u)=%u", i, qos_apply.cos_map[i]);
    }

    /* Apply configuration */
    if (ptin_qos_vlan_add(&qos_apply) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error configuring QoS of intVlan %u / NNIVlan %u / leaf:%u",
              qos_apply.int_vlan, qos_apply.nni_vlan, qos_apply.leaf_side);
      return L7_FAILURE;
    }

    PT_LOG_DEBUG(LOG_CTX_MSG, "QoS configured successfully of intVlan %u / NNIVlan %u / leaf:%u",
              qos_apply.int_vlan, qos_apply.nni_vlan, qos_apply.leaf_side);
  }
  /* Only update list of ports */
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Updating list of ports");

    /* Apply configuration */
    if (ptin_qos_vlan_ports_update(&qos_apply) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error reconfiguring list ports");
      return L7_FAILURE;
    }
    PT_LOG_DEBUG(LOG_CTX_MSG, "Ports reconfigured!");
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
L7_RC_t ptin_msg_EVC_create(ipc_msg *inbuffer, ipc_msg *outbuffer)
{
  L7_uint16 i;
  L7_uint32 evc_id, flags;
  L7_uint16 nni_vlan;
  ptin_HwEthMef10Evc_t ptinEvcConf;
  msg_HwEthMef10EvcQoS_t *msgEvcConf = (msg_HwEthMef10EvcQoS_t *) inbuffer->info;

  /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
  if ((ENDIAN_SWAP32(msgEvcConf->evc.id) == PTIN_EVC_INBAND) || (ENDIAN_SWAP32(msgEvcConf->evc.id) >= PTIN_SYSTEM_N_EXTENDED_EVCS))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "EVC# %u is out of range [0..%u]", ENDIAN_SWAP32(msgEvcConf->evc.id), PTIN_SYSTEM_N_EXTENDED_EVCS-1);
    return L7_FAILURE;
  }

  /* Copy data to ptin struct */
  memset(&ptinEvcConf, 0x00, sizeof(ptinEvcConf));
  ptinEvcConf.index    = ENDIAN_SWAP32(msgEvcConf->evc.id);
  ptinEvcConf.flags    = ENDIAN_SWAP32(msgEvcConf->evc.flags);
  ptinEvcConf.type     = ENDIAN_SWAP8 (msgEvcConf->evc.type);
  ptinEvcConf.mc_flood = ENDIAN_SWAP8 (msgEvcConf->evc.mc_flood);
  ptinEvcConf.n_intf   = ENDIAN_SWAP8 (msgEvcConf->evc.n_intf);
  //memcpy(ptinEvcConf.ce_vid_bmp, msgEvcConf->evc.ce_vid_bmp, sizeof(ptinEvcConf.ce_vid_bmp));

  PT_LOG_DEBUG(LOG_CTX_MSG, "EVC# %u",              ptinEvcConf.index);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .Flags    = 0x%08X",  ptinEvcConf.flags);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .Type     = %u",      ptinEvcConf.type);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .MC Flood = %u (%s)", ptinEvcConf.mc_flood, ptinEvcConf.mc_flood==0?"All":ptinEvcConf.mc_flood==1?"Unknown":"None");
  PT_LOG_DEBUG(LOG_CTX_MSG, " .Nr.Intf  = %u",      ptinEvcConf.n_intf);

  #ifdef PTIN_ENABLE_ERPS
  if( (flags & PTIN_EVC_MASK_MC_IPTV) && ptin_erps_get_status_void(1) == 1)
  {
    ptinEvcConf.mc_flood = 1;
  }
  #endif

  for (i=0; i < ptinEvcConf.n_intf; i++)
  {
    #if (0)
    /* PTP: Workaround */

    PT_LOG_DEBUG(LOG_CTX_MSG, "ptin_sys_number_of_ports (%d)", ptin_sys_number_of_ports);

    if ( (msgEvcConf->intf[i].intf_id == ptin_sys_number_of_ports) && (msgEvcConf->intf[i].intf_type == PTIN_EVC_INTF_PHYSICAL))
    {
      ptinEvcConf.flags = 0x18;
      ptinEvcConf.n_intf--;
      PT_LOG_DEBUG(LOG_CTX_MSG, " .Flags    = 0x%08X",  ptinEvcConf.flags);
      
      PT_LOG_DEBUG(LOG_CTX_MSG, "PTP EVC (%u)",              ptinEvcConf.index);
      break;
    }

    ptinEvcConf.flags &= ~PTIN_EVC_MASK_IGMP_PROTOCOL;
    ptinEvcConf.flags &= ~PTIN_EVC_MASK_MC_IPTV;     
    #endif

    ptinEvcConf.intf[i].intf.format = PTIN_INTF_FORMAT_TYPEID;
    ptinEvcConf.intf[i].intf.value.ptin_intf.intf_type = ENDIAN_SWAP8(msgEvcConf->evc.intf[i].intf_type);
    ptinEvcConf.intf[i].intf.value.ptin_intf.intf_id   = ENDIAN_SWAP8(msgEvcConf->evc.intf[i].intf_id);
    ptinEvcConf.intf[i].mef_type    = ENDIAN_SWAP8 (msgEvcConf->evc.intf[i].mef_type) /*PTIN_EVC_INTF_ROOT*/;
    ptinEvcConf.intf[i].vid         = ENDIAN_SWAP16(msgEvcConf->evc.intf[i].vid);
    ptinEvcConf.intf[i].vid_inner   = ENDIAN_SWAP16(msgEvcConf->evc.intf[i].inner_vid);
    ptinEvcConf.intf[i].action_outer= PTIN_XLATE_ACTION_REPLACE;
    ptinEvcConf.intf[i].action_inner= PTIN_XLATE_ACTION_NONE;

    PT_LOG_DEBUG(LOG_CTX_MSG, "   %s %02u %s VID=%04u/%-04u",
             ptinEvcConf.intf[i].intf.value.ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
             ptinEvcConf.intf[i].intf.value.ptin_intf.intf_id,
             ptinEvcConf.intf[i].mef_type == PTIN_EVC_INTF_ROOT ? "Root":"Leaf",
             ptinEvcConf.intf[i].vid,ptinEvcConf.intf[i].vid_inner);
  }

  if (ptin_evc_create(&ptinEvcConf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error creating/reconfiguring EVC# %u", ptinEvcConf.index);
    return L7_FAILURE;
  }

  /* Get EVC flags */
  if (ptin_evc_flags_get(ptinEvcConf.index, &flags, L7_NULLPTR) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error obtaining flags for EVC %u", ptinEvcConf.index);
    return L7_FAILURE;
  }

  /* Does this message contains QoS information? */
  /* Uplink QoS */
  if (inbuffer->infoDim >= sizeof(msg_HwEthMef10Evc_t) + sizeof(msg_CoS_classification_t))
  {
    evc_id = ptinEvcConf.index;
    nni_vlan = 0;

    /* Only obtain NNI vlan for linecard uplink ports */
  #if (PTIN_BOARD_IS_LINECARD)
    if ((flags & PTIN_EVC_MASK_QUATTRO) && (flags & PTIN_EVC_MASK_STACKED))
    {
      /* Get NNI VLAN */
      if (ptin_evc_get_NNIvlan_fromEvcId(ptinEvcConf.index, &nni_vlan) == L7_SUCCESS)
      {
        evc_id = (L7_uint32)-1;   /* Use NNI VLAN, instead of EVC id */
      }
      else
      {
        PT_LOG_WARN(LOG_CTX_MSG, "Error obtaining NNI VLAN from eEVC %u", ptinEvcConf.index);
        nni_vlan = 0;
      }
    }
  #endif
    PT_LOG_DEBUG(LOG_CTX_MSG, "Going to configure uplink QoS for EVC %u / NNI VLAN %u", evc_id, nni_vlan);
    /* Uplink QoS */
    if (ptin_msg_qosvlan_config(evc_id, nni_vlan, L7_FALSE, &msgEvcConf->qos[0]) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error configuring uplink QoS for EVC %u / NNI VLAN %u", evc_id, nni_vlan);
      return L7_FAILURE;
    }
  }
  /* Downlink QoS */
  if (inbuffer->infoDim >= sizeof(msg_HwEthMef10EvcQoS_t))
  {
    evc_id = ptinEvcConf.index;
    nni_vlan = 0;

    PT_LOG_DEBUG(LOG_CTX_MSG, "Going to configure downlink QoS for EVC %u / NNI VLAN %u", evc_id, nni_vlan);
    /* Uplink QoS */
    if (ptin_msg_qosvlan_config(evc_id, nni_vlan, L7_TRUE, &msgEvcConf->qos[1]) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error configuring downlink QoS for EVC %u / NNI VLAN %u", evc_id, nni_vlan);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/**
 * Configures QoS for an EVC
 * 
 * @param inbuffer 
 * @param outbuffer 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_evc_qos_set(ipc_msg *inbuffer, ipc_msg *outbuffer)
{
  L7_uint16 nni_vlan;
  L7_uint32 evc_id, number_of_evcs, flags;
  msg_evc_qos_t *msgEvcQoS = (msg_evc_qos_t *) inbuffer->info;
  L7_uint16 i, size;
  L7_RC_t rc;

  size = inbuffer->infoDim / sizeof(msg_evc_qos_t);

  for (i = 0; i < size; i++)
  {
    evc_id = (L7_uint32)-1;
    nni_vlan = 0;

    /* Get NNI VLAN */
    if (ENDIAN_SWAP8(msgEvcQoS[i].id.id_type) == MSG_ID_NNIVID_TYPE)
    {
      nni_vlan = ENDIAN_SWAP32(msgEvcQoS[i].id.id_val.nni_vid);

      number_of_evcs = 1;
      if (ptin_evc_get_evcId_fromNNIvlan(nni_vlan, &evc_id, &number_of_evcs) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error obtaining EVC id from NNI_VLAN %u", nni_vlan);
        return L7_NOT_EXIST;
      }
    }
    else
    {
      evc_id = ENDIAN_SWAP32(msgEvcQoS[i].id.id_val.evc_id);
      if (ptin_evc_get_NNIvlan_fromEvcId(evc_id, &nni_vlan) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error obtaining NNI_VLAN %u from EVC %u", evc_id);
        return L7_NOT_EXIST;
      }
    }

    /* Get EVC flags */
    if (ptin_evc_flags_get(evc_id, &flags, L7_NULLPTR) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error obtaining flags for EVC %u / NNI_VLAN %u", evc_id, nni_vlan);
      return L7_FAILURE;
    }

  #if (PTIN_BOARD_IS_LINECARD)
    if ((flags & PTIN_EVC_MASK_QUATTRO) && (flags & PTIN_EVC_MASK_STACKED))
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Going to configure uplink QoS for NNI_VLAN %u", nni_vlan);
      rc = ptin_msg_qosvlan_config((L7_uint32) -1, nni_vlan, L7_FALSE, &msgEvcQoS[i].qos[0]);
    }
    else
  #endif
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Going to configure uplink QoS for EVC %u", evc_id);
      rc = ptin_msg_qosvlan_config(evc_id, 0, L7_FALSE, &msgEvcQoS[i].qos[0]);
    }
    if (rc != L7_SUCCESS) 
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error configuring uplink QoS for EVC %u / NNI_VLAN %u", evc_id, nni_vlan);
      return L7_FAILURE;
    }

    PT_LOG_DEBUG(LOG_CTX_MSG, "Going to configure downlink QoS for EVC %u / NNI VLAN %u", evc_id, nni_vlan);
    rc = ptin_msg_qosvlan_config(evc_id, 0, L7_TRUE, &msgEvcQoS[i].qos[1]);
    if (rc != L7_SUCCESS) 
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error configuring downlink QoS for EVC %u / NNI_VLAN %u", evc_id, nni_vlan);
      return L7_FAILURE;
    }
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
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid argument");
    return L7_FAILURE;
  }

  for (i=0; i<n_structs; i++)
  {
    /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
    if ((ENDIAN_SWAP32(msgEvcConf[i].id) == PTIN_EVC_INBAND) || (ENDIAN_SWAP32(msgEvcConf[i].id) >= PTIN_SYSTEM_N_EXTENDED_EVCS))
    {
      PT_LOG_ERR(LOG_CTX_MSG, "EVC# %u is out of range [0..%u]", ENDIAN_SWAP32(msgEvcConf[i].id), PTIN_SYSTEM_N_EXTENDED_EVCS-1);
      rc_global = L7_FAILURE;
      continue;
    }

    /* Remove EVC */
    if (ptin_evc_delete(ENDIAN_SWAP32(msgEvcConf[i].id)) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error deleting EVC# %u", ENDIAN_SWAP32(msgEvcConf[i].id));
      rc_global = L7_FAILURE;
      continue;
    }

    PT_LOG_DEBUG(LOG_CTX_MSG, "EVC# %u successfully deleted", ENDIAN_SWAP32(msgEvcConf[i].id));
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
  L7_uint i, ext_evc_id;
  ptin_HwEthMef10Intf_t ptinEvcPort;
  L7_RC_t rc, rc_global = L7_SUCCESS, rc_global_failure = L7_SUCCESS;

  /* Validate arguments */
  if (msgEvcPort == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "No data provided");
    return L7_FAILURE;
  }

  /* Run all structures */
  for (i=0; i<n_size; i++)
  {
    ext_evc_id = ENDIAN_SWAP32(msgEvcPort[i].evcId);

    /* Validate EVC# range (EVC index [0..PTIN_SYSTEM_N_EXTENDED_EVCS[) */
    if (/*(msgEvcPort[i].evcId == PTIN_EVC_INBAND) ||*/ (ext_evc_id >= PTIN_SYSTEM_N_EXTENDED_EVCS))
    {
      PT_LOG_ERR(LOG_CTX_MSG, "EVC# %u is out of range [0..%u]", ext_evc_id, PTIN_SYSTEM_N_EXTENDED_EVCS-1);
      return L7_FAILURE;
    }

    /* Copy data to ptin struct */
    ptinEvcPort.intf.format = PTIN_INTF_FORMAT_TYPEID;
    ptinEvcPort.intf.value.ptin_intf.intf_type = ENDIAN_SWAP8(msgEvcPort[i].intf.intf_type);
    ptinEvcPort.intf.value.ptin_intf.intf_id   = ENDIAN_SWAP8(msgEvcPort[i].intf.intf_id);
    ptinEvcPort.mef_type  = ENDIAN_SWAP8 (msgEvcPort[i].intf.mef_type);
    ptinEvcPort.vid       = ENDIAN_SWAP16(msgEvcPort[i].intf.vid);
    ptinEvcPort.vid_inner = ENDIAN_SWAP16(msgEvcPort[i].intf.inner_vid);
    ptinEvcPort.action_outer = PTIN_XLATE_ACTION_REPLACE;
    ptinEvcPort.action_inner = PTIN_XLATE_ACTION_NONE;

    PT_LOG_DEBUG(LOG_CTX_MSG, "EVC# %u - oper %s",     ext_evc_id,
              ((oper==PTIN_MSG_OPER_ADD) ? "ADD" : ((oper==PTIN_MSG_OPER_REMOVE) ? "REMOVE" : "UNKNOWN")));
    PT_LOG_DEBUG(LOG_CTX_MSG, " .Intf      = %u/%u",   ptinEvcPort.intf.value.ptin_intf.intf_type, ptinEvcPort.intf.value.ptin_intf.intf_id);
    PT_LOG_DEBUG(LOG_CTX_MSG, " .IntfType  = %s",     (ptinEvcPort.mef_type == PTIN_EVC_INTF_LEAF) ? "LEAF" : "ROOT");
    PT_LOG_DEBUG(LOG_CTX_MSG, " .OuterVlan = %u",      ptinEvcPort.vid);
    PT_LOG_DEBUG(LOG_CTX_MSG, " .InnerVlan = %u",      ptinEvcPort.vid_inner);

    /* Add/remove port */
    switch (oper)
    {
    case PTIN_MSG_OPER_ADD:
      if ((rc=ptin_evc_port_add(ext_evc_id, &ptinEvcPort)) != L7_SUCCESS)
      {        
        rc_global = rc;
        if (IS_FAILURE_ERROR(rc))
        {
          PT_LOG_ERR(LOG_CTX_MSG, "Error adding port %u/%u to EVC# %u (rc:%u)",
                     ptinEvcPort.intf.value.ptin_intf.intf_type, ptinEvcPort.intf.value.ptin_intf.intf_id, ext_evc_id, rc);
          rc_global_failure = rc;
        }
        else
        {
          //Notice Already Logged
        }
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_MSG, "Added port %u/%u to EVC# %u",
                     ptinEvcPort.intf.value.ptin_intf.intf_type, ptinEvcPort.intf.value.ptin_intf.intf_id, ext_evc_id);
      }
      break;
    case PTIN_MSG_OPER_REMOVE:
      if ((rc=ptin_evc_port_remove(ext_evc_id, &ptinEvcPort)) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error removing port %u/%u to EVC# %u",
                   ptinEvcPort.intf.value.ptin_intf.intf_type, ptinEvcPort.intf.value.ptin_intf.intf_id, ext_evc_id);
        rc_global = rc;
        if (IS_FAILURE_ERROR(rc))
          rc_global_failure = rc;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_MSG, "Removed port %u/%u from EVC# %u",
                     ptinEvcPort.intf.value.ptin_intf.intf_type, ptinEvcPort.intf.value.ptin_intf.intf_id, ext_evc_id);
      }
      break;
    default:
      PT_LOG_ERR(LOG_CTX_MSG, "Unknown operation %u", oper);
      rc_global = L7_FAILURE;
    }

#if 0
    /* Update VLAN-QoS ports */
    if (rc_global_failure == L7_SUCCESS)
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Going to reconfigure QoS for EVC %u", msgEvcPort[i].evcId);
      if (ptin_msg_qosvlan_config(msgEvcPort[i].evcId, ptinEvcPort.mef_type, L7_NULLPTR))
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error reconfiguring QoS for EVC %u", msgEvcPort[i].evcId);
      }
      PT_LOG_DEBUG(LOG_CTX_MSG, "QoS for EVC %u reconfigured", msgEvcPort[i].evcId);
    }
#endif
  }

  if (rc_global_failure != L7_SUCCESS)
    return rc_global_failure;

  return rc_global;
}

/**
 * Reconfigure EVC
 * 
 * @param msgEvcOptions : EVC options
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_evc_config(ipc_msg *inbuffer, ipc_msg *outbuffer)
{
  L7_uint i, evc_list_index, evc_id;
  ptin_HwEthMef10EvcOptions_t evcOptions;
  L7_RC_t rc, rc_global = L7_SUCCESS, rc_global_failure = L7_SUCCESS;

  msg_HwEthMef10EvcOptions_t *msgEvcOptions = (msg_HwEthMef10EvcOptions_t *) inbuffer->info;
  L7_uint16 n_size  = inbuffer->infoDim/sizeof(msg_HwEthMef10EvcOptions_t);

  /* List of EVCs to be used for local purposes */
  static L7_uint32 evcid_list[4096];
  static L7_uint max_evcs = 4096;

  #ifdef PTIN_ENABLE_ERPS

  msgEvcOptions->mc_flood = ENDIAN_SWAP8(2);

  #endif

  /* Validate arguments */
  if (msgEvcOptions == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "No data provided");
    return L7_FAILURE;
  }

  /* Run all structures */
  for (i=0; i<n_size; i++)
  {
    ENDIAN_SWAP8_MOD (msgEvcOptions[i].service_id.id_type);
    ENDIAN_SWAP32_MOD(msgEvcOptions[i].service_id.id_val.evc_id);
    ENDIAN_SWAP16_MOD(msgEvcOptions[i].mask);
    ENDIAN_SWAP32_MOD(msgEvcOptions[i].flags.value);
    ENDIAN_SWAP32_MOD(msgEvcOptions[i].flags.mask);
    ENDIAN_SWAP8_MOD (msgEvcOptions[i].type);
    ENDIAN_SWAP8_MOD (msgEvcOptions[i].mc_flood);

    PT_LOG_DEBUG(LOG_CTX_MSG," type = %u",                  msgEvcOptions[i].service_id.id_type);
    PT_LOG_DEBUG(LOG_CTX_MSG," id   = %u",                  msgEvcOptions[i].service_id.id_val.evc_id);
    PT_LOG_DEBUG(LOG_CTX_MSG, " .Mask      = 0x%04x",       msgEvcOptions[i].mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, " .Flags     = 0x%08x/0x%08x",msgEvcOptions[i].flags.value, msgEvcOptions[i].flags.mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, " .Type      = %u",           msgEvcOptions[i].type);
    PT_LOG_DEBUG(LOG_CTX_MSG, " .MC_flood  = %u",           msgEvcOptions[i].mc_flood);

    /* If EVC id is provided, get related VLAN */
    if (msgEvcOptions[i].service_id.id_type == MSG_ID_EVC_TYPE)
    {
      /* Validate EVC id */
      if (msgEvcOptions[i].service_id.id_val.evc_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "eEVC#%u is out of range!", msgEvcOptions[i].service_id.id_val.evc_id);
        rc_global = rc_global_failure = L7_FAILURE;
        continue;
      }
      /* EVC must be active */
      if (!ptin_evc_is_in_use(msgEvcOptions[i].service_id.id_val.evc_id)) 
      {
        PT_LOG_ERR(LOG_CTX_MSG, "eEVC#%u is not in use!", msgEvcOptions[i].service_id.id_val.evc_id);
        rc_global = L7_NOT_EXIST;
        continue;
      }

      /* Copy data to ptin struct */
      evcOptions.mask         = msgEvcOptions[i].mask;
      evcOptions.flags.value  = msgEvcOptions[i].flags.value;
      evcOptions.flags.mask   = msgEvcOptions[i].flags.mask;
      evcOptions.type         = msgEvcOptions[i].type;
      evcOptions.mc_flood     = msgEvcOptions[i].mc_flood;

      if ((rc=ptin_evc_config(msgEvcOptions[i].service_id.id_val.evc_id, &evcOptions)) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error configuring EVC# %u", msgEvcOptions[i].service_id.id_val.evc_id);
        rc_global = rc;
        if (IS_FAILURE_ERROR(rc))
          rc_global_failure = rc;
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_MSG, "EVC# %u configured successfully", msgEvcOptions[i].service_id.id_val.evc_id);
      }

    }
    /* Use given VLANs range */
    else if (msgEvcOptions[i].service_id.id_type == MSG_ID_NNIVID_TYPE)
    {
      /* Validate NNI VLAN */
      if (msgEvcOptions[i].service_id.id_val.nni_vid < PTIN_VLAN_MIN ||
          msgEvcOptions[i].service_id.id_val.nni_vid > PTIN_VLAN_MAX)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "NNI VLAN %u is out of range!", msgEvcOptions[i].service_id.id_val.nni_vid);
        rc_global = rc_global_failure = L7_FAILURE;
        continue;
      }

      /* Get EVC list from NNI VLAN */
      if (ptin_evc_get_evcId_fromNNIvlan(msgEvcOptions[i].service_id.id_val.nni_vid, evcid_list, &max_evcs) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "NNI VLAN %u is invalid, or doesn't belong to any EVC!", msgEvcOptions[i].service_id.id_val.nni_vid);
        rc_global = L7_NOT_EXIST;
        continue;
      }

      /* Copy data to ptin struct */
      evcOptions.mask         = msgEvcOptions[i].mask;
      evcOptions.flags.value  = msgEvcOptions[i].flags.value;
      evcOptions.flags.mask   = msgEvcOptions[i].flags.mask;
      evcOptions.type         = msgEvcOptions[i].type;
      evcOptions.mc_flood     = msgEvcOptions[i].mc_flood;

      /* Run EVCs range */
      for (evc_list_index = 0; evc_list_index < max_evcs; evc_list_index++)
      {
        evc_id = evcid_list[evc_list_index];

        /* Validate EVC id */
        if (evc_id >= PTIN_SYSTEM_N_EXTENDED_EVCS)
        {
          PT_LOG_ERR(LOG_CTX_MSG, "eEVC#%u is out of range!", evc_id);
          rc_global = rc_global_failure = L7_FAILURE;
          continue;
        }
        /* EVC must be active */
        if (!ptin_evc_is_in_use(evc_id)) 
        {
          PT_LOG_ERR(LOG_CTX_MSG, "eEVC#%u is not in use!", evc_id);
          rc_global = L7_NOT_EXIST;
          continue;
        }

        if ((rc=ptin_evc_config(evc_id, &evcOptions)) != L7_SUCCESS)
        {
          PT_LOG_ERR(LOG_CTX_MSG, "Error configuring EVC# %u", evc_id);
          rc_global = rc;
          if (IS_FAILURE_ERROR(rc))
            rc_global_failure = rc;
        }
        else
        {
          PT_LOG_TRACE(LOG_CTX_MSG, "EVC# %u configured successfully", evc_id);
        }
      }
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Invalid service type %u", msgEvcOptions[i].service_id.id_type);
      rc_global = L7_NOT_SUPPORTED;
      continue;
    }
  }

  if (rc_global_failure != L7_SUCCESS)
    return rc_global_failure;

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
  ptinEvcBridge.index          = ENDIAN_SWAP32(msgEvcBridge->evcId);
  ptinEvcBridge.inn_vlan       = ENDIAN_SWAP16(msgEvcBridge->inn_vlan);
  ptinEvcBridge.intf.intf.format = PTIN_INTF_FORMAT_TYPEID;
  ptinEvcBridge.intf.intf.value.ptin_intf.intf_id   = ENDIAN_SWAP8(msgEvcBridge->intf.intf_id);
  ptinEvcBridge.intf.intf.value.ptin_intf.intf_type = ENDIAN_SWAP8(msgEvcBridge->intf.intf_type);
  ptinEvcBridge.intf.mef_type  = ENDIAN_SWAP8 (msgEvcBridge->intf.mef_type);   /* must be Leaf */
  ptinEvcBridge.intf.vid       = ENDIAN_SWAP16(msgEvcBridge->intf.vid);

  PT_LOG_DEBUG(LOG_CTX_MSG, "EVC# %u Bridge",         ptinEvcBridge.index);
  PT_LOG_DEBUG(LOG_CTX_MSG, " %s# %u",
               ptinEvcBridge.intf.intf.value.ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
               ptinEvcBridge.intf.intf.value.ptin_intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .Inner VID       = %u", ptinEvcBridge.inn_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .Outer VID [NEW] = %u", ptinEvcBridge.intf.vid);

  if (ptin_evc_p2p_bridge_add(&ptinEvcBridge) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error adding EVC# %u bridge", ptinEvcBridge.index);
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
  ptinEvcBridge.index          = ENDIAN_SWAP32(msgEvcBridge->evcId);
  ptinEvcBridge.inn_vlan       = ENDIAN_SWAP16(msgEvcBridge->inn_vlan);
  ptinEvcBridge.intf.intf.format = PTIN_INTF_FORMAT_TYPEID;
  ptinEvcBridge.intf.intf.value.ptin_intf.intf_id   = ENDIAN_SWAP8(msgEvcBridge->intf.intf_id);
  ptinEvcBridge.intf.intf.value.ptin_intf.intf_type = ENDIAN_SWAP8(msgEvcBridge->intf.intf_type);
  ptinEvcBridge.intf.mef_type  = ENDIAN_SWAP8 (msgEvcBridge->intf.mef_type);   /* must be Leaf */
  ptinEvcBridge.intf.vid       = ENDIAN_SWAP16(msgEvcBridge->intf.vid);        /* not used on remove oper. */

  PT_LOG_DEBUG(LOG_CTX_MSG, "EVC# %u Bridge",         ptinEvcBridge.index);
  PT_LOG_DEBUG(LOG_CTX_MSG, " %s# %u",
               ptinEvcBridge.intf.intf.value.ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
               ptinEvcBridge.intf.intf.value.ptin_intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .Inner VID       = %u", ptinEvcBridge.inn_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .Outer VID [NEW] = %u", ptinEvcBridge.intf.vid);

  rc = ptin_evc_p2p_bridge_remove(&ptinEvcBridge);

  if ( rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error removing EVC# %u bridge", ptinEvcBridge.index);
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

  ENDIAN_SWAP8_MOD (msgEvcFlow->SlotId);
  ENDIAN_SWAP32_MOD(msgEvcFlow->evcId);
  ENDIAN_SWAP32_MOD(msgEvcFlow->flags);
  ENDIAN_SWAP16_MOD(msgEvcFlow->nni_cvlan);
  ENDIAN_SWAP8_MOD (msgEvcFlow->intf.intf_type);
  ENDIAN_SWAP8_MOD (msgEvcFlow->intf.intf_id);
  ENDIAN_SWAP16_MOD(msgEvcFlow->intf.outer_vid);
  ENDIAN_SWAP16_MOD(msgEvcFlow->intf.inner_vid);
  ENDIAN_SWAP8_MOD (msgEvcFlow->macLearnMax);
  ENDIAN_SWAP8_MOD (msgEvcFlow->onuId);
  ENDIAN_SWAP8_MOD (msgEvcFlow->mask);
  ENDIAN_SWAP16_MOD(msgEvcFlow->maxChannels);
  ENDIAN_SWAP64_MOD(msgEvcFlow->maxBandwidth);

  /*Initialize Structure*/
  memset(&ptinEvcFlow, 0x00, sizeof(ptinEvcFlow));

  /* Copy data */
  ptinEvcFlow.evc_idx             = msgEvcFlow->evcId;
  ptinEvcFlow.flags               = msgEvcFlow->flags;
  ptinEvcFlow.int_ivid            = msgEvcFlow->nni_cvlan;
  ptinEvcFlow.ptin_intf.intf_type = msgEvcFlow->intf.intf_type;
  ptinEvcFlow.ptin_intf.intf_id   = msgEvcFlow->intf.intf_id;
  ptinEvcFlow.uni_ovid            = msgEvcFlow->intf.outer_vid; /* must be a leaf */
  ptinEvcFlow.uni_ivid            = msgEvcFlow->intf.inner_vid;
  ptinEvcFlow.macLearnMax         = msgEvcFlow->macLearnMax;

  PT_LOG_DEBUG(LOG_CTX_MSG, "EVC# %u Flow",     ptinEvcFlow.evc_idx);
  PT_LOG_DEBUG(LOG_CTX_MSG, " Flags = 0x%08x",  ptinEvcFlow.flags);
  PT_LOG_DEBUG(LOG_CTX_MSG, " %s# %u",          ptinEvcFlow.ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
                                                ptinEvcFlow.ptin_intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, " Int.IVID    = %u", ptinEvcFlow.int_ivid);
  PT_LOG_DEBUG(LOG_CTX_MSG, " UNI-OVID    = %u", ptinEvcFlow.uni_ovid);
  PT_LOG_DEBUG(LOG_CTX_MSG, " UNI-IVID    = %u", ptinEvcFlow.uni_ivid);
  PT_LOG_DEBUG(LOG_CTX_MSG, " macLearnMax = %u", ptinEvcFlow.macLearnMax);  

  if (ptinEvcFlow.flags & PTIN_EVC_MASK_IGMP_PROTOCOL)
  {
    if  (msgEvcFlow->mask > PTIN_MSG_EVC_FLOW_MASK_VALID)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Invalid Mask [mask:0x%02x]", msgEvcFlow->mask);
      return L7_FAILURE;
    }

#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT    
    if  (( ((msgEvcFlow->mask & PTIN_MSG_EVC_FLOW_MASK_MAX_ALLOWED_BANDWIDTH) == PTIN_MSG_EVC_FLOW_MASK_MAX_ALLOWED_BANDWIDTH) &&
           ( msgEvcFlow->maxBandwidth != PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_BPS_DISABLE &&
             msgEvcFlow->maxBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_BPS) ) ||
         ( ((msgEvcFlow->mask & PTIN_MSG_EVC_FLOW_MASK_MAX_ALLOWED_CHANNELS) == PTIN_MSG_EVC_FLOW_MASK_MAX_ALLOWED_CHANNELS) &&
           ( msgEvcFlow->maxChannels != PTIN_IGMP_ADMISSION_CONTROL_MAX_CHANNELS_DISABLE &&
             msgEvcFlow->maxChannels > PTIN_IGMP_ADMISSION_CONTROL_MAX_CHANNELS) ) )
        
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Invalid Admission Control Parameters [mask:0x%02x maxBandwidth:%llu bits/s maxChannels:%hu]",
                 msgEvcFlow->mask, msgEvcFlow->maxBandwidth, msgEvcFlow->maxChannels);
      return L7_FAILURE;
    }
    ptinEvcFlow.mask          = msgEvcFlow->mask;
    ptinEvcFlow.onuId         = msgEvcFlow->onuId;
    ptinEvcFlow.maxBandwidth  = msgEvcFlow->maxBandwidth;
    ptinEvcFlow.maxChannels   = msgEvcFlow->maxChannels;
    
    PT_LOG_DEBUG(LOG_CTX_MSG, " onuId       = %u",        ptinEvcFlow.onuId);
    PT_LOG_DEBUG(LOG_CTX_MSG, " mask        = 0x%x",      ptinEvcFlow.mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, " maxChannels = %u",        ptinEvcFlow.maxChannels);
    PT_LOG_DEBUG(LOG_CTX_MSG, " maxBandwidth= %llu bit/s",ptinEvcFlow.maxBandwidth);
#endif
  
  }

  if ((rc=ptin_evc_flow_add(&ptinEvcFlow)) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error adding EVC# %u flow", ptinEvcFlow.evc_idx);
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
  ptinEvcFlow.evc_idx             = ENDIAN_SWAP32(msgEvcFlow->evcId);
  ptinEvcFlow.ptin_intf.intf_type = ENDIAN_SWAP8 (msgEvcFlow->intf.intf_type);
  ptinEvcFlow.ptin_intf.intf_id   = ENDIAN_SWAP8 (msgEvcFlow->intf.intf_id);
  ptinEvcFlow.int_ivid            = ENDIAN_SWAP16(msgEvcFlow->nni_cvlan);
  ptinEvcFlow.uni_ovid            = ENDIAN_SWAP16(msgEvcFlow->intf.outer_vid); /* must be a leaf */
  ptinEvcFlow.uni_ivid            = ENDIAN_SWAP16(msgEvcFlow->intf.inner_vid);

  PT_LOG_DEBUG(LOG_CTX_MSG, "EVC# %u Flow",   ptinEvcFlow.evc_idx);
  PT_LOG_DEBUG(LOG_CTX_MSG, " %s# %u",        ptinEvcFlow.ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
                                                ptinEvcFlow.ptin_intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, " Int.IVID = %u", ptinEvcFlow.int_ivid);
  PT_LOG_DEBUG(LOG_CTX_MSG, " UNI-OVID = %u", ptinEvcFlow.uni_ovid);
  PT_LOG_DEBUG(LOG_CTX_MSG, " UNI-IVID = %u", ptinEvcFlow.uni_ivid);

  if ((rc=ptin_evc_flow_remove(&ptinEvcFlow)) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error removing EVC# %u flow", ptinEvcFlow.evc_idx);
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
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid params");
    return L7_FAILURE;
  }

  /* Run all clients */
  for ( i=0; i<n_clients; i++)
  {
    ENDIAN_SWAP8_MOD (msgEvcFlood[i].SlotId);
    ENDIAN_SWAP32_MOD(msgEvcFlood[i].evcId);
    ENDIAN_SWAP8_MOD (msgEvcFlood[i].mask);
    ENDIAN_SWAP8_MOD (msgEvcFlood[i].intf.intf_type);
    ENDIAN_SWAP8_MOD (msgEvcFlood[i].intf.intf_id);
    ENDIAN_SWAP16_MOD(msgEvcFlood[i].client_vlan);
    ENDIAN_SWAP16_MOD(msgEvcFlood[i].oVlanId);
    ENDIAN_SWAP16_MOD(msgEvcFlood[i].iVlanId);

    PT_LOG_DEBUG(LOG_CTX_MSG,"EVC flood vlan %u:",i);
    PT_LOG_DEBUG(LOG_CTX_MSG," Slot    = %u",    msgEvcFlood[i].SlotId);
    PT_LOG_DEBUG(LOG_CTX_MSG," EVC_idx = %u",    msgEvcFlood[i].evcId);
    PT_LOG_DEBUG(LOG_CTX_MSG," Mask    = 0x%02x",msgEvcFlood[i].mask);
    PT_LOG_DEBUG(LOG_CTX_MSG," Intf    = %u/%u", msgEvcFlood[i].intf.intf_type, msgEvcFlood[i].intf.intf_id);
    PT_LOG_DEBUG(LOG_CTX_MSG," CVlan   = %u",    msgEvcFlood[i].client_vlan);
    PT_LOG_DEBUG(LOG_CTX_MSG," Outer Vlan : %u", msgEvcFlood[i].oVlanId);
    PT_LOG_DEBUG(LOG_CTX_MSG," Inner Vlan : %u", msgEvcFlood[i].iVlanId);

    ptin_intf.intf_type = msgEvcFlood[i].intf.intf_type;
    ptin_intf.intf_id   = msgEvcFlood[i].intf.intf_id;

    if (ptin_evc_flood_vlan_add( msgEvcFlood[i].evcId,
                                 ((msgEvcFlood[i].mask & 0x01) ? &ptin_intf : L7_NULLPTR),
                                 ((msgEvcFlood[i].mask & 0x02) ? msgEvcFlood[i].client_vlan : 0),
                                 msgEvcFlood[i].oVlanId,
                                 msgEvcFlood[i].iVlanId ) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error adding EVC# %u flooding vlan", msgEvcFlood[i].evcId);
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
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid params");
    return L7_FAILURE;
  }

  /* Run all clients */
  for ( i=0; i<n_clients; i++)
  {
    ENDIAN_SWAP8_MOD (msgEvcFlood[i].SlotId);
    ENDIAN_SWAP32_MOD(msgEvcFlood[i].evcId);
    ENDIAN_SWAP8_MOD (msgEvcFlood[i].mask);
    ENDIAN_SWAP8_MOD (msgEvcFlood[i].intf.intf_type);
    ENDIAN_SWAP8_MOD (msgEvcFlood[i].intf.intf_id);
    ENDIAN_SWAP16_MOD(msgEvcFlood[i].client_vlan);
    ENDIAN_SWAP16_MOD(msgEvcFlood[i].oVlanId);
    ENDIAN_SWAP16_MOD(msgEvcFlood[i].iVlanId);

    PT_LOG_DEBUG(LOG_CTX_MSG,"EVC flood vlan %u:",i);
    PT_LOG_DEBUG(LOG_CTX_MSG," Slot    = %u",    msgEvcFlood[i].SlotId);
    PT_LOG_DEBUG(LOG_CTX_MSG," EVC_idx = %u",    msgEvcFlood[i].evcId);
    PT_LOG_DEBUG(LOG_CTX_MSG," Mask    = 0x%02x",msgEvcFlood[i].mask);
    PT_LOG_DEBUG(LOG_CTX_MSG," Intf    = %u/%u", msgEvcFlood[i].intf.intf_type, msgEvcFlood[i].intf.intf_id);
    PT_LOG_DEBUG(LOG_CTX_MSG," CVlan   = %u",    msgEvcFlood[i].client_vlan);
    PT_LOG_DEBUG(LOG_CTX_MSG," Outer Vlan : %u", msgEvcFlood[i].oVlanId);
    PT_LOG_DEBUG(LOG_CTX_MSG," Inner Vlan : %u", msgEvcFlood[i].iVlanId);

    ptin_intf.intf_type = msgEvcFlood[i].intf.intf_type;
    ptin_intf.intf_id   = msgEvcFlood[i].intf.intf_id;

    if (ptin_evc_flood_vlan_remove( msgEvcFlood[i].evcId,
                                    ((msgEvcFlood[i].mask & 0x01) ? &ptin_intf : L7_NULLPTR),
                                    ((msgEvcFlood[i].mask & 0x02) ? msgEvcFlood[i].client_vlan : 0),
                                    msgEvcFlood[i].oVlanId,
                                    msgEvcFlood[i].iVlanId ) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error removing EVC# %u flooding vlan", msgEvcFlood[i].evcId);
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
  ptin_bw_meter_t   meter;
  L7_RC_t   rc;

  PT_LOG_DEBUG(LOG_CTX_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgBwProfile == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  ENDIAN_SWAP8_MOD (msgBwProfile->SlotId);
  ENDIAN_SWAP32_MOD(msgBwProfile->evcId);
  ENDIAN_SWAP8_MOD (msgBwProfile->mask);
  ENDIAN_SWAP16_MOD(msgBwProfile->service_vlan);
  ENDIAN_SWAP16_MOD(msgBwProfile->client_vlan);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_src.intf_type);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_src.intf_id);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_dst.intf_type);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_dst.intf_id);

  PT_LOG_DEBUG(LOG_CTX_MSG," evcId  = %u",    msgBwProfile->evcId);
  PT_LOG_DEBUG(LOG_CTX_MSG," mask   = 0x%02x",msgBwProfile->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG," SVID   = %u",    msgBwProfile->service_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," CVID   = %u",    msgBwProfile->client_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," SrcIntf= %u/%u", msgBwProfile->intf_src.intf_type, msgBwProfile->intf_src.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," DstIntf= %u/%u", msgBwProfile->intf_dst.intf_type, msgBwProfile->intf_dst.intf_id);

  /* Extract EVC id */
  evcId = msgBwProfile->evcId;

  /* Copy data */
  if (ptin_msg_bwProfileStruct_fill(msgBwProfile, &profile, &meter) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error with ptin_msg_bwProfileStruct_fill");
    return L7_FAILURE;
  }

  /* Add bandwidth profile */
  if ((rc=ptin_evc_bwProfile_get(evcId, &profile, &meter))!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading profile!");
    return rc;
  }

  msgBwProfile->profile.cir = meter.cir;
  msgBwProfile->profile.cbs = meter.cbs;
  msgBwProfile->profile.eir = meter.eir;
  msgBwProfile->profile.ebs = meter.ebs;

  ENDIAN_SWAP8_MOD (msgBwProfile->SlotId);
  ENDIAN_SWAP32_MOD(msgBwProfile->evcId);
  ENDIAN_SWAP8_MOD (msgBwProfile->mask);
  ENDIAN_SWAP16_MOD(msgBwProfile->service_vlan);
  ENDIAN_SWAP16_MOD(msgBwProfile->client_vlan);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_src.intf_type);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_src.intf_id);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_dst.intf_type);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_dst.intf_id);
  ENDIAN_SWAP64_MOD(msgBwProfile->profile.cir);
  ENDIAN_SWAP64_MOD(msgBwProfile->profile.cbs);
  ENDIAN_SWAP64_MOD(msgBwProfile->profile.eir);
  ENDIAN_SWAP64_MOD(msgBwProfile->profile.ebs);

  PT_LOG_DEBUG(LOG_CTX_MSG,"Message processing finished!");
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
  ptin_bw_meter_t   meter;
  L7_RC_t   rc;

  PT_LOG_DEBUG(LOG_CTX_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgBwProfile == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  ENDIAN_SWAP8_MOD (msgBwProfile->SlotId);
  ENDIAN_SWAP32_MOD(msgBwProfile->evcId);
  ENDIAN_SWAP8_MOD (msgBwProfile->mask);
  ENDIAN_SWAP16_MOD(msgBwProfile->service_vlan);
  ENDIAN_SWAP16_MOD(msgBwProfile->client_vlan);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_src.intf_type);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_src.intf_id);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_dst.intf_type);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_dst.intf_id);
  ENDIAN_SWAP64_MOD(msgBwProfile->profile.cir);
  ENDIAN_SWAP64_MOD(msgBwProfile->profile.cbs);
  ENDIAN_SWAP64_MOD(msgBwProfile->profile.eir);
  ENDIAN_SWAP64_MOD(msgBwProfile->profile.ebs);

  PT_LOG_DEBUG(LOG_CTX_MSG," evcId  = %u",    msgBwProfile->evcId);
  PT_LOG_DEBUG(LOG_CTX_MSG," mask   = 0x%02x",msgBwProfile->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG," SVID   = %u",    msgBwProfile->service_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," CVID   = %u",    msgBwProfile->client_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," SrcIntf= %u/%u", msgBwProfile->intf_src.intf_type, msgBwProfile->intf_src.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," DstIntf= %u/%u", msgBwProfile->intf_dst.intf_type, msgBwProfile->intf_dst.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," Meter {CIR,CBS}={%llu,%llu}",msgBwProfile->profile.cir, msgBwProfile->profile.cbs);
  PT_LOG_DEBUG(LOG_CTX_MSG," Meter {EIR,EBS}={%llu,%llu}",msgBwProfile->profile.eir, msgBwProfile->profile.ebs);

  /* Extract EVC id */
  evcId = msgBwProfile->evcId;

  /* Copy data */
  if (ptin_msg_bwProfileStruct_fill(msgBwProfile, &profile, &meter) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error with ptin_msg_bwProfileStruct_fill");
    return L7_FAILURE;
  }

  /* Add bandwidth profile */
  switch (msgId) {
  case CCMSG_ETH_BW_PROFILE_SET:
      profile.cos=-1;   //Set to ignore
      if ((rc=ptin_evc_bwProfile_set(evcId, &profile, &meter)) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Error applying profile!");
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
           if ((rc=ptin_evc_bwProfile_set(evcId, &p, &meter)) != L7_SUCCESS)
           {
             PT_LOG_ERR(LOG_CTX_MSG,"Error applying profile!");
             return rc;
           }
       //}
      }
      break;
  }//switch

  PT_LOG_DEBUG(LOG_CTX_MSG,"Message processing finished!");
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
  ptin_bw_meter_t   meter;
  L7_RC_t   rc;

  PT_LOG_DEBUG(LOG_CTX_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgBwProfile == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  ENDIAN_SWAP8_MOD (msgBwProfile->SlotId);
  ENDIAN_SWAP32_MOD(msgBwProfile->evcId);
  ENDIAN_SWAP8_MOD (msgBwProfile->mask);
  ENDIAN_SWAP16_MOD(msgBwProfile->service_vlan);
  ENDIAN_SWAP16_MOD(msgBwProfile->client_vlan);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_src.intf_type);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_src.intf_id);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_dst.intf_type);
  ENDIAN_SWAP8_MOD (msgBwProfile->intf_dst.intf_id);

  PT_LOG_DEBUG(LOG_CTX_MSG," evcId  = %u",    msgBwProfile->evcId);
  PT_LOG_DEBUG(LOG_CTX_MSG," mask   = 0x%02x",msgBwProfile->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG," SVID   = %u",    msgBwProfile->service_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," CVID   = %u",    msgBwProfile->client_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," SrcIntf= %u/%u", msgBwProfile->intf_src.intf_type, msgBwProfile->intf_src.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," DstIntf= %u/%u", msgBwProfile->intf_dst.intf_type, msgBwProfile->intf_dst.intf_id);

  /* Extract EVC id */
  evcId = msgBwProfile->evcId;

  /* Copy data */
  if (ptin_msg_bwProfileStruct_fill(msgBwProfile, &profile, &meter) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error with ptin_msg_bwProfileStruct_fill");
    return L7_FAILURE;
  }

  switch (msgId) {
  case CCMSG_ETH_BW_PROFILE_DELETE:
      profile.cos=-1;   //Set to ignore
      if ((rc=ptin_evc_bwProfile_delete(evcId, &profile)) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Error removing profile!");
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
           if ((rc=ptin_evc_bwProfile_delete(evcId, &p)) != L7_SUCCESS)
           {
             PT_LOG_ERR(LOG_CTX_MSG,"Error removing profile!");
             return rc;
           }
       //}
      }
      break;
  }//switch

  PT_LOG_DEBUG(LOG_CTX_MSG,"Message processing finished!");
  return L7_SUCCESS;
}

/**
 * Get data of an existent storm control profile
 * 
 * @param msgStormControl : Storm control profile
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_stormControl2_get(msg_HwEthStormControl2_t *msgStormControl)
{
  PT_LOG_DEBUG(LOG_CTX_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgStormControl == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG," slotId = %u",          ENDIAN_SWAP8 (msgStormControl->SlotId));
  PT_LOG_DEBUG(LOG_CTX_MSG," intf   = %u/%u",       ENDIAN_SWAP8 (msgStormControl->intf.intf_type), ENDIAN_SWAP8 (msgStormControl->intf.intf_id));
  PT_LOG_DEBUG(LOG_CTX_MSG," mask   = 0x%02x",      ENDIAN_SWAP8 (msgStormControl->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG," Broadcast = %ld (%u)", ENDIAN_SWAP32(msgStormControl->broadcast.rate_value),  ENDIAN_SWAP8 (msgStormControl->broadcast.rate_units));
  PT_LOG_DEBUG(LOG_CTX_MSG," Multicast = %ld (%u)", ENDIAN_SWAP32(msgStormControl->multicast.rate_value),  ENDIAN_SWAP8 (msgStormControl->multicast.rate_units));
  PT_LOG_DEBUG(LOG_CTX_MSG," UnknownUC = %ld (%u)", ENDIAN_SWAP32(msgStormControl->unknown_uc.rate_value), ENDIAN_SWAP8 (msgStormControl->unknown_uc.rate_units));
  PT_LOG_DEBUG(LOG_CTX_MSG," Block UC = %u",        ENDIAN_SWAP8 (msgStormControl->block_unicast));
  PT_LOG_DEBUG(LOG_CTX_MSG," Block MC = %u",        ENDIAN_SWAP8 (msgStormControl->block_multicast));

  return L7_SUCCESS;
}

/**
 * Configure storm control profile
 * 
 * @param msgStormControl : Storm control profile
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_stormControl2_set(msg_HwEthStormControl2_t *msgStormControl)
{
  ptin_intf_t     ptin_intf;
  L7_BOOL         enable;
  L7_uint32       rate_value;
  L7_RATE_UNIT_t  rate_units;
  L7_RC_t         rc, rc_global=L7_SUCCESS;

  PT_LOG_DEBUG(LOG_CTX_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgStormControl == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  ENDIAN_SWAP8_MOD (msgStormControl->SlotId);
  ENDIAN_SWAP8_MOD (msgStormControl->intf.intf_type);
  ENDIAN_SWAP8_MOD (msgStormControl->intf.intf_id);
  ENDIAN_SWAP8_MOD (msgStormControl->mask);
  ENDIAN_SWAP32_MOD(msgStormControl->broadcast.rate_value);
  ENDIAN_SWAP8_MOD (msgStormControl->broadcast.rate_units);
  ENDIAN_SWAP32_MOD(msgStormControl->multicast.rate_value);
  ENDIAN_SWAP8_MOD (msgStormControl->multicast.rate_units);
  ENDIAN_SWAP32_MOD(msgStormControl->unknown_uc.rate_value);
  ENDIAN_SWAP8_MOD (msgStormControl->unknown_uc.rate_units);
  ENDIAN_SWAP8_MOD (msgStormControl->block_unicast);
  ENDIAN_SWAP8_MOD (msgStormControl->block_multicast);

  PT_LOG_DEBUG(LOG_CTX_MSG," slotId = %u",          msgStormControl->SlotId);
  PT_LOG_DEBUG(LOG_CTX_MSG," intf   = %u/%u",       msgStormControl->intf.intf_type, msgStormControl->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," mask   = 0x%02x",      msgStormControl->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG," Broadcast = %ld (%u)", msgStormControl->broadcast.rate_value,  msgStormControl->broadcast.rate_units);
  PT_LOG_DEBUG(LOG_CTX_MSG," Multicast = %ld (%u)", msgStormControl->multicast.rate_value,  msgStormControl->multicast.rate_units);
  PT_LOG_DEBUG(LOG_CTX_MSG," UnknownUC = %ld (%u)", msgStormControl->unknown_uc.rate_value, msgStormControl->unknown_uc.rate_units);
  PT_LOG_DEBUG(LOG_CTX_MSG," Block UC = %u",        msgStormControl->block_unicast);
  PT_LOG_DEBUG(LOG_CTX_MSG," Block MC = %u",        msgStormControl->block_multicast);

  ptin_intf.intf_type = msgStormControl->intf.intf_type;
  ptin_intf.intf_id   = msgStormControl->intf.intf_id;


  /* -------- BROADCAST STORMCONTROL -------- */
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_BCAST)
  {
    PT_LOG_TRACE(LOG_CTX_MSG, "Processing Broadcast stormcontrol...");
    do
    {
      /* Data units */
      switch (msgStormControl->broadcast.rate_units)
      {
        case 0:   rate_units = L7_RATE_UNIT_PPS;      break;
        case 1:   rate_units = L7_RATE_UNIT_PERCENT;  break;
        case 2:   rate_units = L7_RATE_UNIT_KBPS;     break;
        default:  rate_units = L7_RATE_UNIT_NONE;     break;
      }
      if (rate_units == L7_RATE_UNIT_NONE)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Unknown units (%u)", msgStormControl->broadcast.rate_units);
        rc_global = L7_FAILURE;
        break;
      }
      enable     = (msgStormControl->broadcast.rate_value != (L7_uint32)-1);
      rate_value =  msgStormControl->broadcast.rate_value;

      /* Apply stormcontrol */
      rc = ptin_intf_bcast_stormControl_set(&ptin_intf, enable, rate_value, FD_POLICY_DEFAULT_BCAST_STORM_BURSTSIZE, rate_units);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Error configuring Broadcast stormcontrol for ptin_intf %u/%u", ptin_intf.intf_type, ptin_intf.intf_id);
        rc_global = rc;
        break;
      }
    } while (0);
  }

  /* -------- MULTICAST STORMCONTROL -------- */
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_MCAST)
  {
    PT_LOG_TRACE(LOG_CTX_MSG, "Processing Multicast stormcontrol...");
    do
    {
      /* Data units */
      switch (msgStormControl->multicast.rate_units)
      {
        case 0:   rate_units = L7_RATE_UNIT_PPS;      break;
        case 1:   rate_units = L7_RATE_UNIT_PERCENT;  break;
        case 2:   rate_units = L7_RATE_UNIT_KBPS;     break;
        default:  rate_units = L7_RATE_UNIT_NONE;     break;
      }
      if (rate_units == L7_RATE_UNIT_NONE)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Unknown units (%u)", msgStormControl->multicast.rate_units);
        rc_global = L7_FAILURE;
        break;
      }
      enable     = (msgStormControl->multicast.rate_value != (L7_uint32)-1);
      rate_value =  msgStormControl->multicast.rate_value;

      /* Apply stormcontrol */
      rc = ptin_intf_mcast_stormControl_set(&ptin_intf, enable, rate_value, FD_POLICY_DEFAULT_BCAST_STORM_BURSTSIZE, rate_units);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Error configuring Multicast stormcontrol for ptin_intf %u/%u", ptin_intf.intf_type, ptin_intf.intf_id);
        rc_global = rc;
        break;
      }
    } while (0);
  }

  /* -------- UNKNOWN UNICAST STORMCONTROL -------- */
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_UCUNK)
  {
    PT_LOG_TRACE(LOG_CTX_MSG, "Processing Unknown Unicast stormcontrol...");
    do
    {
      /* Data units */
      switch (msgStormControl->unknown_uc.rate_units)
      {
        case 0:   rate_units = L7_RATE_UNIT_PPS;      break;
        case 1:   rate_units = L7_RATE_UNIT_PERCENT;  break;
        case 2:   rate_units = L7_RATE_UNIT_KBPS;     break;
        default:  rate_units = L7_RATE_UNIT_NONE;     break;
      }
      if (rate_units == L7_RATE_UNIT_NONE)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Unknown units (%u)", msgStormControl->unknown_uc.rate_units);
        rc_global = L7_FAILURE;
        break;
      }
      enable     = (msgStormControl->unknown_uc.rate_value != (L7_uint32)-1);
      rate_value =  msgStormControl->unknown_uc.rate_value;

      /* Apply stormcontrol */
      rc = ptin_intf_ucast_stormControl_set(&ptin_intf, enable, rate_value, FD_POLICY_DEFAULT_BCAST_STORM_BURSTSIZE, rate_units);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Error configuring Unicast stormcontrol for ptin_intf %u/%u", ptin_intf.intf_type, ptin_intf.intf_id);
        rc_global = rc;
        break;
      }
    } while (0);
  }

  /* Final result */
  if (rc_global == L7_SUCCESS)
  {
    PT_LOG_TRACE(LOG_CTX_MSG, "Success applying stormcontrol to ptin_intf %u/%u", ptin_intf.intf_type, ptin_intf.intf_id);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error applying stormcontrol to ptin_intf %u/%u", ptin_intf.intf_type, ptin_intf.intf_id);
  }

  return rc_global;
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
  //ptin_stormControl_t stormControl;
  //L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_DEBUG(LOG_CTX_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgStormControl == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG," slotId = %u",     ENDIAN_SWAP8 (msgStormControl->SlotId));
  PT_LOG_DEBUG(LOG_CTX_MSG," id     = %u",     ENDIAN_SWAP32(msgStormControl->id));
  PT_LOG_DEBUG(LOG_CTX_MSG," flags  = 0x%04x", ENDIAN_SWAP16(msgStormControl->flags));
  PT_LOG_DEBUG(LOG_CTX_MSG," mask   = 0x%04x", ENDIAN_SWAP16(msgStormControl->mask));

  PT_LOG_WARN(LOG_CTX_MSG, "Obsolete Feature: not supported anymore!");

  return L7_NOT_SUPPORTED;

#if 0
  /* Input data */
  memset(&stormControl, 0x00, sizeof(ptin_stormControl_t));
  /* Traffic Type */
  if (ENDIAN_SWAP16(msgStormControl->mask) & MSG_STORMCONTROL_MASK_BCAST)
  {
    stormControl.flags |= PTIN_STORMCONTROL_MASK_BCAST;
  }
  if (ENDIAN_SWAP16(msgStormControl->mask) & MSG_STORMCONTROL_MASK_MCAST)
  {
    stormControl.flags |= PTIN_STORMCONTROL_MASK_MCAST;
  }
  if (ENDIAN_SWAP16(msgStormControl->mask) & MSG_STORMCONTROL_MASK_UCUNK)
  {
    stormControl.flags |= PTIN_STORMCONTROL_MASK_UCUNK;
  }
  if (ENDIAN_SWAP16(msgStormControl->mask) & MSG_STORMCONTROL_MASK_CPU)
  {
    stormControl.flags |= PTIN_STORMCONTROL_MASK_CPU;
  }

  /* Read bandwidth profile */
  if ((rc=ptin_evc_stormControl_get(&stormControl))!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading storm control data!");
    return rc;
  }

  /* Init Output data */
  msgStormControl->mask = ENDIAN_SWAP16(0);
  msgStormControl->bcast_rate = ENDIAN_SWAP32(0);
  msgStormControl->mcast_rate = ENDIAN_SWAP32(0);
  msgStormControl->ucast_unknown_rate = ENDIAN_SWAP32(0);
  /* Fill output data */
  if (stormControl.flags & PTIN_STORMCONTROL_MASK_BCAST)
  {
    msgStormControl->bcast_rate = ENDIAN_SWAP32(stormControl.bcast_rate);
    msgStormControl->mask |= ENDIAN_SWAP16(MSG_STORMCONTROL_MASK_BCAST);
  }
  if (stormControl.flags & PTIN_STORMCONTROL_MASK_MCAST)
  {
    msgStormControl->mcast_rate = ENDIAN_SWAP32(stormControl.mcast_rate);
    msgStormControl->mask |= ENDIAN_SWAP16(MSG_STORMCONTROL_MASK_MCAST);
  }
  if (stormControl.flags & PTIN_STORMCONTROL_MASK_UCUNK)
  {
    msgStormControl->ucast_unknown_rate = ENDIAN_SWAP32(stormControl.ucunk_rate);
    msgStormControl->mask |= ENDIAN_SWAP16(MSG_STORMCONTROL_MASK_UCUNK);
  }
  if (stormControl.flags & PTIN_STORMCONTROL_MASK_CPU)
  {
    msgStormControl->ucast_unknown_rate = ENDIAN_SWAP32(stormControl.cpu_rate);
    msgStormControl->mask |= ENDIAN_SWAP16(MSG_STORMCONTROL_MASK_CPU);
  }

  PT_LOG_DEBUG(LOG_CTX_MSG,"Message processing finished! (rc=%d)", rc);
  return rc;
#endif
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
  //ptin_stormControl_t stormControl;
  //L7_RC_t   rc = L7_SUCCESS;

  PT_LOG_DEBUG(LOG_CTX_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgStormControl == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  ENDIAN_SWAP8_MOD (msgStormControl->SlotId);
  ENDIAN_SWAP32_MOD(msgStormControl->id);
  ENDIAN_SWAP16_MOD(msgStormControl->flags);
  ENDIAN_SWAP16_MOD(msgStormControl->mask);
  ENDIAN_SWAP32_MOD(msgStormControl->bcast_rate);
  ENDIAN_SWAP32_MOD(msgStormControl->mcast_rate);
  ENDIAN_SWAP32_MOD(msgStormControl->ucast_unknown_rate);

  PT_LOG_DEBUG(LOG_CTX_MSG," slotId = %u",         msgStormControl->SlotId);
  PT_LOG_DEBUG(LOG_CTX_MSG," id     = %u",         msgStormControl->id);
  PT_LOG_DEBUG(LOG_CTX_MSG," flags  = 0x%04x",     msgStormControl->flags);
  PT_LOG_DEBUG(LOG_CTX_MSG," mask   = 0x%04x",     msgStormControl->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG," bcast_rate = %u bps", msgStormControl->bcast_rate);
  PT_LOG_DEBUG(LOG_CTX_MSG," mcast_rate = %u bps", msgStormControl->mcast_rate);
  PT_LOG_DEBUG(LOG_CTX_MSG," ucunk_rate = %u bps", msgStormControl->ucast_unknown_rate);

  PT_LOG_WARN(LOG_CTX_MSG, "Obsolete Feature: not supported anymore!");

  return L7_SUCCESS;

#if 0
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
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_CPU)
  {
    stormControl.cpu_rate = msgStormControl->ucast_unknown_rate;
    stormControl.flags |= PTIN_STORMCONTROL_MASK_CPU;
  }

  /* Add bandwidth profile */
  if ((rc=ptin_evc_stormControl_set(L7_ENABLE, &stormControl))!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error applying storm control profile!");
    return rc;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG,"Message processing finished!  (rc=%d)", rc);
  return rc;
#endif
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

  PT_LOG_DEBUG(LOG_CTX_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgStormControl == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  ENDIAN_SWAP8_MOD (msgStormControl->SlotId);
  ENDIAN_SWAP32_MOD(msgStormControl->id);
  ENDIAN_SWAP16_MOD(msgStormControl->flags);
  ENDIAN_SWAP16_MOD(msgStormControl->mask);
  ENDIAN_SWAP32_MOD(msgStormControl->bcast_rate);
  ENDIAN_SWAP32_MOD(msgStormControl->mcast_rate);
  ENDIAN_SWAP32_MOD(msgStormControl->ucast_unknown_rate);

  PT_LOG_DEBUG(LOG_CTX_MSG," slotId = %u",         msgStormControl->SlotId);
  PT_LOG_DEBUG(LOG_CTX_MSG," id     = %u",         msgStormControl->id);
  PT_LOG_DEBUG(LOG_CTX_MSG," flags  = 0x%04x",     msgStormControl->flags);
  PT_LOG_DEBUG(LOG_CTX_MSG," mask   = 0x%04x",     msgStormControl->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG," bcast_rate = %u bps", msgStormControl->bcast_rate);
  PT_LOG_DEBUG(LOG_CTX_MSG," mcast_rate = %u bps", msgStormControl->mcast_rate);
  PT_LOG_DEBUG(LOG_CTX_MSG," ucunk_rate = %u bps", msgStormControl->ucast_unknown_rate);

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
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_CPU)
  {
    stormControl.flags |= PTIN_STORMCONTROL_MASK_CPU;
  }

  /* Add bandwidth profile */
  if ((rc=ptin_evc_stormControl_set(L7_DISABLE, &stormControl))!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error disabling storm control profile!");
    return rc;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG,"Message processing finished!  (rc=%d)", rc);
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

  PT_LOG_DEBUG(LOG_CTX_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msgStormControl == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  ENDIAN_SWAP8_MOD (msgStormControl->SlotId);
  ENDIAN_SWAP32_MOD(msgStormControl->id);
  ENDIAN_SWAP16_MOD(msgStormControl->flags);
  ENDIAN_SWAP16_MOD(msgStormControl->mask);

  PT_LOG_DEBUG(LOG_CTX_MSG," slotId = %u",     msgStormControl->SlotId);
  PT_LOG_DEBUG(LOG_CTX_MSG," id     = %u",     msgStormControl->id);
  PT_LOG_DEBUG(LOG_CTX_MSG," flags  = 0x%04x", msgStormControl->flags);
  PT_LOG_DEBUG(LOG_CTX_MSG," mask   = 0x%04x", msgStormControl->mask);

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
  if (msgStormControl->mask & MSG_STORMCONTROL_MASK_CPU)
  {
    stormControl.flags |= PTIN_STORMCONTROL_MASK_CPU;
  }

  /* Add bandwidth profile */
  if ((rc=ptin_evc_stormControl_reset(&stormControl))!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error clearing storm control profile!");
    return rc;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG,"Message processing finished! (rc=%d)", rc);
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

  PT_LOG_DEBUG(LOG_CTX_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msg_evcStats == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  ENDIAN_SWAP8_MOD (msg_evcStats->SlotId);
  ENDIAN_SWAP32_MOD(msg_evcStats->evc_id);
  ENDIAN_SWAP8_MOD (msg_evcStats->mask);
  ENDIAN_SWAP8_MOD (msg_evcStats->intf.intf_type);
  ENDIAN_SWAP8_MOD (msg_evcStats->intf.intf_id);
  ENDIAN_SWAP16_MOD(msg_evcStats->service_vlan);
  ENDIAN_SWAP16_MOD(msg_evcStats->client_vlan);
  ENDIAN_SWAP32_MOD(msg_evcStats->channel_ip);

  PT_LOG_DEBUG(LOG_CTX_MSG," evcId    = %u",    msg_evcStats->evc_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," mask     = 0x%02x",msg_evcStats->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG," Intf     = %u/%u", msg_evcStats->intf.intf_type, msg_evcStats->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," SVID     = %u",    msg_evcStats->service_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," CVID     = %u",    msg_evcStats->client_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," ChannelIP= %u",    msg_evcStats->channel_ip);

  /* Clear counters */
  memset(&msg_evcStats->stats, 0x00, sizeof(msg_evcStats_counters_t));
  msg_evcStats->mask &= ~((L7_uint8) MSG_EVC_COUNTERS_MASK_STATS);

  /* Extract EVC id */
  evcId = msg_evcStats->evc_id;

  /* Copy data */
  if (ptin_msg_evcStatsStruct_fill(msg_evcStats, &profile)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error filling structure");
    return L7_FAILURE;
  }

  /* Get statistics data */
  rc = ptin_evc_evcStats_get(evcId, &profile, &counters);

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
    PT_LOG_ERR(LOG_CTX_MSG,"No policy defined to read stats!");
    /* Null values to message */
    //memset(&(msg_evcStats->stats),0x00,sizeof(msg_evcStats_counters_t));
    /* Deactivate bit in mask: this indicates that the counters are NOT valid */
    //msg_evcStats->mask &= ~((L7_uint8) MSG_EVC_COUNTERS_MASK_STATS);
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG," evcId    = %u",      msg_evcStats->evc_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," mask     = 0x%02x",  msg_evcStats->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG," Intf     = %u/%u",   msg_evcStats->intf.intf_type, msg_evcStats->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," SVID     = %u",      msg_evcStats->service_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," CVID     = %u",      msg_evcStats->client_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," ChannelIP= %u",      msg_evcStats->channel_ip);
  PT_LOG_DEBUG(LOG_CTX_MSG," Stat_mask  = 0x%02x",msg_evcStats->stats.mask_stat);
  PT_LOG_DEBUG(LOG_CTX_MSG," Stats_RX:");
  PT_LOG_DEBUG(LOG_CTX_MSG,"   Total    =%10u",   msg_evcStats->stats.rx.pktTotal);
  PT_LOG_DEBUG(LOG_CTX_MSG,"   Unicast  =%10u",   msg_evcStats->stats.rx.pktUnicast);
  PT_LOG_DEBUG(LOG_CTX_MSG,"   Multicast=%10u",   msg_evcStats->stats.rx.pktMulticast);
  PT_LOG_DEBUG(LOG_CTX_MSG,"   Broadcast=%10u",   msg_evcStats->stats.rx.pktBroadcast);
  PT_LOG_DEBUG(LOG_CTX_MSG,"   Dropped  =%10u",   msg_evcStats->stats.rx.pktDropped);
  PT_LOG_DEBUG(LOG_CTX_MSG," Stats_TX:");
  PT_LOG_DEBUG(LOG_CTX_MSG,"   Total    =%10u",   msg_evcStats->stats.tx.pktTotal);
  PT_LOG_DEBUG(LOG_CTX_MSG,"   Unicast  =%10u",   msg_evcStats->stats.tx.pktUnicast);
  PT_LOG_DEBUG(LOG_CTX_MSG,"   Multicast=%10u",   msg_evcStats->stats.tx.pktMulticast);
  PT_LOG_DEBUG(LOG_CTX_MSG,"   Broadcast=%10u",   msg_evcStats->stats.tx.pktBroadcast);
  PT_LOG_DEBUG(LOG_CTX_MSG,"   Dropped  =%10u",   msg_evcStats->stats.tx.pktDropped);

  ENDIAN_SWAP8_MOD (msg_evcStats->SlotId);
  ENDIAN_SWAP32_MOD(msg_evcStats->evc_id);
  ENDIAN_SWAP8_MOD (msg_evcStats->mask);
  ENDIAN_SWAP8_MOD (msg_evcStats->intf.intf_type);
  ENDIAN_SWAP8_MOD (msg_evcStats->intf.intf_id);
  ENDIAN_SWAP16_MOD(msg_evcStats->service_vlan);
  ENDIAN_SWAP16_MOD(msg_evcStats->client_vlan);
  ENDIAN_SWAP32_MOD(msg_evcStats->channel_ip);
  ENDIAN_SWAP8_MOD (msg_evcStats->stats.mask_stat);
  ENDIAN_SWAP32_MOD(msg_evcStats->stats.rx.pktTotal);
  ENDIAN_SWAP32_MOD(msg_evcStats->stats.rx.pktUnicast);
  ENDIAN_SWAP32_MOD(msg_evcStats->stats.rx.pktMulticast);
  ENDIAN_SWAP32_MOD(msg_evcStats->stats.rx.pktBroadcast);
  ENDIAN_SWAP32_MOD(msg_evcStats->stats.rx.pktDropped);
  ENDIAN_SWAP32_MOD(msg_evcStats->stats.tx.pktTotal);
  ENDIAN_SWAP32_MOD(msg_evcStats->stats.tx.pktUnicast);
  ENDIAN_SWAP32_MOD(msg_evcStats->stats.tx.pktMulticast);
  ENDIAN_SWAP32_MOD(msg_evcStats->stats.tx.pktBroadcast);
  ENDIAN_SWAP32_MOD(msg_evcStats->stats.tx.pktDropped);

  PT_LOG_DEBUG(LOG_CTX_MSG,"Message processing finished!");
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

  PT_LOG_DEBUG(LOG_CTX_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msg_evcStats == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  ENDIAN_SWAP8_MOD (msg_evcStats->SlotId);
  ENDIAN_SWAP32_MOD(msg_evcStats->evc_id);
  ENDIAN_SWAP8_MOD (msg_evcStats->mask);
  ENDIAN_SWAP8_MOD (msg_evcStats->intf.intf_type);
  ENDIAN_SWAP8_MOD (msg_evcStats->intf.intf_id);
  ENDIAN_SWAP16_MOD(msg_evcStats->service_vlan);
  ENDIAN_SWAP16_MOD(msg_evcStats->client_vlan);
  ENDIAN_SWAP32_MOD(msg_evcStats->channel_ip);

  PT_LOG_DEBUG(LOG_CTX_MSG," evcId    = %u",      msg_evcStats->evc_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," mask     = 0x%02x",  msg_evcStats->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG," Intf     = %u/%u",   msg_evcStats->intf.intf_type, msg_evcStats->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," SVID     = %u",      msg_evcStats->service_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," CVID     = %u",      msg_evcStats->client_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," ChannelIP= 0x%08x",  msg_evcStats->channel_ip);

  /* Extract EVC id */
  evcId = msg_evcStats->evc_id;

  /* Copy data */
  if (ptin_msg_evcStatsStruct_fill(msg_evcStats, &profile) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error filling structure");
    return L7_FAILURE;
  }

  /* Add bandwidth profile */
  if ((rc=ptin_evc_evcStats_set(evcId, &profile)) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error allocating statistics!");
    return rc;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG,"Message processing finished!");
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

  PT_LOG_DEBUG(LOG_CTX_MSG,"Starting message processing...");

  /* Validate arguments */
  if (msg_evcStats == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid Message Pointer");
    return L7_FAILURE;
  }

  ENDIAN_SWAP8_MOD (msg_evcStats->SlotId);
  ENDIAN_SWAP32_MOD(msg_evcStats->evc_id);
  ENDIAN_SWAP8_MOD (msg_evcStats->mask);
  ENDIAN_SWAP8_MOD (msg_evcStats->intf.intf_type);
  ENDIAN_SWAP8_MOD (msg_evcStats->intf.intf_id);
  ENDIAN_SWAP16_MOD(msg_evcStats->service_vlan);
  ENDIAN_SWAP16_MOD(msg_evcStats->client_vlan);
  ENDIAN_SWAP32_MOD(msg_evcStats->channel_ip);

  PT_LOG_DEBUG(LOG_CTX_MSG," evcId    = %u",    msg_evcStats->evc_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," mask     = 0x%02x",msg_evcStats->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG," Intf     = %u/%u", msg_evcStats->intf.intf_type, msg_evcStats->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," SVID     = %u",    msg_evcStats->service_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," CVID     = %u",    msg_evcStats->client_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," ChannelIP= %u",    msg_evcStats->channel_ip);

  /* Extract EVC id */
  evcId = msg_evcStats->evc_id;

  /* Copy data */
  if (ptin_msg_evcStatsStruct_fill(msg_evcStats, &profile) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error filling structure");
    return L7_FAILURE;
  }

  /* Add bandwidth profile */
  rc = ptin_evc_evcStats_delete(evcId, &profile);

  if ( rc != L7_SUCCESS )
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error deallocating statistics!");
    return rc;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG,"Message processing finished!");
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
  ptinNtwConn.mask = ENDIAN_SWAP32(msgNtwConn->mask);

  /* Get config */
  rc = ptin_cfg_ntw_connectivity_get(&ptinNtwConn);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error getting inband management config");
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_INFO(LOG_CTX_MSG, "Network Connectivity (mask=0x%08X)", ptinNtwConn.mask);
  PT_LOG_INFO(LOG_CTX_MSG, "  IP Addr         = %u.%u.%u.%u",    (ptinNtwConn.ipaddr  >> 24) & 0xFF, (ptinNtwConn.ipaddr  >> 16) & 0xFF,
           (ptinNtwConn.ipaddr  >>  8) & 0xFF,  ptinNtwConn.ipaddr         & 0xFF);
  PT_LOG_INFO(LOG_CTX_MSG, "  Mask            = %u.%u.%u.%u",    (ptinNtwConn.netmask >> 24) & 0xFF, (ptinNtwConn.netmask >> 16) & 0xFF,
           (ptinNtwConn.netmask >>  8) & 0xFF,  ptinNtwConn.netmask        & 0xFF);
  PT_LOG_INFO(LOG_CTX_MSG, "  Gateway         = %u.%u.%u.%u",    (ptinNtwConn.gateway >> 24) & 0xFF, (ptinNtwConn.gateway >> 16) & 0xFF,
           (ptinNtwConn.gateway >>  8) & 0xFF,  ptinNtwConn.gateway        & 0xFF);
  PT_LOG_INFO(LOG_CTX_MSG, "  Mgmt VLAN ID    = %u",             ptinNtwConn.mgmtVlanId);
  PT_LOG_INFO(LOG_CTX_MSG, "  Interfaces (%u)", ptinNtwConn.n_intf);
  for (i=0; i<ptinNtwConn.n_intf; i++)
    PT_LOG_INFO(LOG_CTX_MSG, "    %s Intf #    = %u", ptinNtwConn.intf[i].intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", ptinNtwConn.intf[i].intf_id);

  /* Copy data */
  msgNtwConn->mask            = ENDIAN_SWAP32(ptinNtwConn.mask);
  msgNtwConn->ipaddr          = ENDIAN_SWAP32(ptinNtwConn.ipaddr);
  msgNtwConn->netmask         = ENDIAN_SWAP32(ptinNtwConn.netmask);
  msgNtwConn->gateway         = ENDIAN_SWAP32(ptinNtwConn.gateway);
  msgNtwConn->mgmtVlanId      = ENDIAN_SWAP16(ptinNtwConn.mgmtVlanId);
  msgNtwConn->n_intf          = ENDIAN_SWAP8 (ptinNtwConn.n_intf);
  for (i=0; i<ptinNtwConn.n_intf; i++)
  {
    msgNtwConn->intf[i].intf_type = ENDIAN_SWAP8(ptinNtwConn.intf[i].intf_type);
    msgNtwConn->intf[i].intf_id   = ENDIAN_SWAP8(ptinNtwConn.intf[i].intf_id);
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
  ptinNtwConn.mask            = ENDIAN_SWAP32(msgNtwConn->mask);
  ptinNtwConn.ipaddr          = ENDIAN_SWAP32(msgNtwConn->ipaddr);
  ptinNtwConn.netmask         = ENDIAN_SWAP32(msgNtwConn->netmask);
  ptinNtwConn.gateway         = ENDIAN_SWAP32(msgNtwConn->gateway);
  ptinNtwConn.mgmtVlanId      = ENDIAN_SWAP16(msgNtwConn->mgmtVlanId);
  ptinNtwConn.n_intf          = ENDIAN_SWAP8 (msgNtwConn->n_intf);
  for (i=0; i<ptinNtwConn.n_intf; i++)
  {
    ptinNtwConn.intf[i].intf_type = ENDIAN_SWAP8(msgNtwConn->intf[i].intf_type);
    ptinNtwConn.intf[i].intf_id   = ENDIAN_SWAP8(msgNtwConn->intf[i].intf_id);
  }

  /* Output data */
  PT_LOG_INFO(LOG_CTX_MSG, "Network Connectivity (mask=0x%08X)", ptinNtwConn.mask);
  PT_LOG_INFO(LOG_CTX_MSG, "  IP Addr         = %u.%u.%u.%u",    (ptinNtwConn.ipaddr  >> 24) & 0xFF, (ptinNtwConn.ipaddr  >> 16) & 0xFF,
           (ptinNtwConn.ipaddr  >>  8) & 0xFF,  ptinNtwConn.ipaddr         & 0xFF);
  PT_LOG_INFO(LOG_CTX_MSG, "  Mask            = %u.%u.%u.%u",    (ptinNtwConn.netmask >> 24) & 0xFF, (ptinNtwConn.netmask >> 16) & 0xFF,
           (ptinNtwConn.netmask >>  8) & 0xFF,  ptinNtwConn.netmask        & 0xFF);
  PT_LOG_INFO(LOG_CTX_MSG, "  Gateway         = %u.%u.%u.%u",    (ptinNtwConn.gateway >> 24) & 0xFF, (ptinNtwConn.gateway >> 16) & 0xFF,
           (ptinNtwConn.gateway >>  8) & 0xFF,  ptinNtwConn.gateway        & 0xFF);
  PT_LOG_INFO(LOG_CTX_MSG, "  Mgmt VLAN ID    = %u",             ptinNtwConn.mgmtVlanId);
  PT_LOG_INFO(LOG_CTX_MSG, "  Interfaces (%u)", ptinNtwConn.n_intf);
  for (i=0; i<ptinNtwConn.n_intf; i++)
    PT_LOG_INFO(LOG_CTX_MSG, "    %s Intf #    = %u", ptinNtwConn.intf[i].intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG", ptinNtwConn.intf[i].intf_id);

  /* Apply config */
  rc = ptin_cfg_ntw_connectivity_set(&ptinNtwConn);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error setting inband management config");
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

  PT_LOG_DEBUG(LOG_CTX_MSG,"Processing message");

  /* Validate input parameters */
  if (dhcpEvcInfo == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid parameters");
    return L7_FAILURE;
  }

  ENDIAN_SWAP8_MOD (dhcpEvcInfo->slot_id);
  ENDIAN_SWAP8_MOD (dhcpEvcInfo->idType);
  ENDIAN_SWAP32_MOD(dhcpEvcInfo->id);
  ENDIAN_SWAP32_MOD(dhcpEvcInfo->mask);
  ENDIAN_SWAP8_MOD (dhcpEvcInfo->dhcp_flag);
  ENDIAN_SWAP32_MOD(dhcpEvcInfo->options);

  PT_LOG_DEBUG(LOG_CTX_MSG, "  ID Type    = %u",      dhcpEvcInfo->idType);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  ID         = %u",      dhcpEvcInfo->id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Mask       = 0x%04X",  dhcpEvcInfo->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  DHCP Flag  = %u",      dhcpEvcInfo->dhcp_flag);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Options    = 0x%04X",  dhcpEvcInfo->options);

  if (dhcpEvcInfo->idType == MSG_ID_EVC_TYPE)
  {
    rc = ptin_dhcp_reconf_evc(dhcpEvcInfo->id, dhcpEvcInfo->dhcp_flag, dhcpEvcInfo->options);
    if (rc!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error reconfiguring global DHCP EVC");
      return rc;
    }
    rc = ptin_pppoe_reconf_evc(dhcpEvcInfo->id, dhcpEvcInfo->dhcp_flag, dhcpEvcInfo->options);
  }
  else if (dhcpEvcInfo->idType == MSG_ID_NNIVID_TYPE)
  {
    rc = ptin_dhcp_reconf_rootVid(dhcpEvcInfo->id, dhcpEvcInfo->dhcp_flag, dhcpEvcInfo->options);
    if (rc!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error reconfiguring global DHCP Instance");
      return rc;
    }
    rc = ptin_pppoe_reconf_rootVid(dhcpEvcInfo->id, dhcpEvcInfo->dhcp_flag, dhcpEvcInfo->options);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid id %u", dhcpEvcInfo->idType);
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

  PT_LOG_DEBUG(LOG_CTX_MSG,"Processing message");

  /* Validate input parameters */
  if (circuitid==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid parameters");
    return L7_FAILURE;
  }

  ENDIAN_SWAP8_MOD (circuitid->slot_id);
  ENDIAN_SWAP8_MOD (circuitid->id_ref.id_type);
  ENDIAN_SWAP32_MOD(circuitid->id_ref.id_val.evc_id);
  ENDIAN_SWAP32_MOD(circuitid->mask_circuitid);
  ENDIAN_SWAP8_MOD (circuitid->chassis);
  ENDIAN_SWAP8_MOD (circuitid->rack);
  ENDIAN_SWAP8_MOD (circuitid->frame);
  ENDIAN_SWAP8_MOD (circuitid->ethernet_priority);
  ENDIAN_SWAP16_MOD(circuitid->s_vid);
  ENDIAN_SWAP8_MOD (circuitid->mask_flags);
  ENDIAN_SWAP8_MOD (circuitid->broadcast_flag);

  PT_LOG_DEBUG(LOG_CTX_MSG, "  Id_type            = %u",      circuitid->id_ref.id_type);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Id value           = %u",      circuitid->id_ref.id_val.evc_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Template           = %s",      circuitid->template_str);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  CircuitId Mask     = 0x%04X",  circuitid->mask_circuitid);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  AccessNode ID      = %s",      circuitid->access_node_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Chassis            = %u",      circuitid->chassis);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Rack               = %u",      circuitid->rack);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Frame              = %u",      circuitid->rack);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Ethernet Priority  = %u",      circuitid->ethernet_priority);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  S-VID              = %u",      circuitid->s_vid);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Flags Mask         = 0x%02X",  circuitid->mask_flags);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Flags              = 0x%02X",  circuitid->broadcast_flag);

  /* TODO: To be reworked */

  /* Set circuit-id global data */
  if (circuitid->id_ref.id_type == MSG_ID_EVC_TYPE)
  {
    /* Circuit id */
    rc = ptin_dhcp_circuitid_set_evc(circuitid->id_ref.id_val.evc_id, circuitid->template_str, circuitid->mask_circuitid,
                                     circuitid->access_node_id, circuitid->chassis, circuitid->rack,
                                     circuitid->rack, circuitid->ethernet_priority, circuitid->s_vid);
    if (rc!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error configuring DHCP circuit-id");
      return rc;
    }
    /* Flags */
    rc = ptin_dhcp_evc_flags_set(circuitid->id_ref.id_val.evc_id, circuitid->mask_flags, circuitid->broadcast_flag);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error configuring DHCP flags");
      return rc;
    }

    rc = ptin_pppoe_circuitid_set_evc(circuitid->id_ref.id_val.evc_id, circuitid->template_str, circuitid->mask_circuitid,
                                      circuitid->access_node_id, circuitid->chassis, circuitid->rack,
                                      circuitid->rack, circuitid->ethernet_priority, circuitid->s_vid);

  }
  else if (circuitid->id_ref.id_type == MSG_ID_NNIVID_TYPE)
  {
    rc = ptin_dhcp_circuitid_set_nniVid(circuitid->id_ref.id_val.nni_vid, circuitid->template_str, circuitid->mask_circuitid,
                                        circuitid->access_node_id, circuitid->chassis, circuitid->rack,
                                        circuitid->rack, circuitid->ethernet_priority, circuitid->s_vid);
    if (rc!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error configuring DHCP circuit-id");
      return rc;
    }
    /* Flags */
    rc = ptin_dhcp_nniVid_flags_set(circuitid->id_ref.id_val.nni_vid, circuitid->mask_flags, circuitid->broadcast_flag);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error configuring DHCP flags");
      return rc;
    }

    rc = ptin_pppoe_circuitid_set_nniVid(circuitid->id_ref.id_val.nni_vid, circuitid->template_str, circuitid->mask_circuitid,
                                         circuitid->access_node_id, circuitid->chassis, circuitid->rack,
                                         circuitid->rack, circuitid->ethernet_priority, circuitid->s_vid);    
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid id %u", circuitid->id_ref.id_type);
    return L7_FAILURE;
  }

  /* TODO */
#if 0
  if (rc!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error configuring circuit-id global data");
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
  L7_uint32 evc_id;

  PT_LOG_DEBUG(LOG_CTX_MSG,"Processing message");

  /* Validate input parameters */
  if (circuitid == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid parameters");
    return L7_FAILURE;
  }

  /* Set circuit-id global data */
  if (ENDIAN_SWAP8(circuitid->id_ref.id_type) == MSG_ID_EVC_TYPE)
  {
    evc_id = ENDIAN_SWAP32(circuitid->id_ref.id_val.evc_id);

    /* Circuit id */
    rc = ptin_dhcp_circuitid_get(evc_id, circuitid->template_str, &circuitid->mask_circuitid,
                                 circuitid->access_node_id, &circuitid->chassis, &circuitid->rack,
                                 &circuitid->frame, &circuitid->ethernet_priority, &circuitid->s_vid);
    /* DHCP flags */
    if (rc == L7_SUCCESS)
    {
      rc = ptin_dhcp_evc_flags_get(evc_id, &circuitid->mask_flags, &circuitid->broadcast_flag);
    }
  }
  else if (ENDIAN_SWAP8(circuitid->id_ref.id_type) == MSG_ID_NNIVID_TYPE)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Not supported yet");
    return L7_FAILURE;
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid id %u", ENDIAN_SWAP8(circuitid->id_ref.id_type));
    return L7_FAILURE;
  }

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error configuring circuit-id global data");
    return rc;
  }

  /* Invert bytes */
  circuitid->mask_circuitid   = ENDIAN_SWAP32(circuitid->mask_circuitid);
  circuitid->chassis          = ENDIAN_SWAP8 (circuitid->chassis);
  circuitid->rack             = ENDIAN_SWAP8 (circuitid->rack);
  circuitid->frame            = ENDIAN_SWAP8 (circuitid->frame);
  circuitid->ethernet_priority= ENDIAN_SWAP8 (circuitid->ethernet_priority);
  circuitid->s_vid            = ENDIAN_SWAP16(circuitid->s_vid);
  circuitid->mask_flags       = ENDIAN_SWAP8 (circuitid->mask_flags);
  circuitid->broadcast_flag   = ENDIAN_SWAP8 (circuitid->broadcast_flag);

  PT_LOG_DEBUG(LOG_CTX_MSG, "  ID type            = %u",      ENDIAN_SWAP8 (circuitid->id_ref.id_type));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  ID value           = %u",      ENDIAN_SWAP32(circuitid->id_ref.id_val.evc_id));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Template           = %s",      circuitid->template_str);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  CircuitId Mask     = 0x%04X",  ENDIAN_SWAP32(circuitid->mask_circuitid));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  AccessNode ID      = %s",      circuitid->access_node_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Chassis            = %u",      ENDIAN_SWAP8 (circuitid->chassis));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Rack               = %u",      ENDIAN_SWAP8 (circuitid->rack));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Frame              = %u",      ENDIAN_SWAP8 (circuitid->frame));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Ethernet Priority  = %u",      ENDIAN_SWAP8 (circuitid->ethernet_priority));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  S-VID              = %u",      ENDIAN_SWAP16(circuitid->s_vid));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Flags Mask         = 0x%02X",  ENDIAN_SWAP8 (circuitid->mask_flags));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Flags              = 0x%02X",  ENDIAN_SWAP8 (circuitid->broadcast_flag));

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

  PT_LOG_DEBUG(LOG_CTX_MSG,"Processing message");

  /* Validate input parameters */
  if (profile==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid parameters");
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "  Slot Id      = %u",     ENDIAN_SWAP8 (profile->SlotId));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  evc_idx      = %u",     ENDIAN_SWAP32(profile->evc_id));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Mask         = 0x%02x", ENDIAN_SWAP8 (profile->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Interface    = %u/%u",  ENDIAN_SWAP8 (profile->intf.intf_type), ENDIAN_SWAP8 (profile->intf.intf_id));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Mask  = 0x%02x", ENDIAN_SWAP8 (profile->client.mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.OVlan = %u",     ENDIAN_SWAP16(profile->client.outer_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.IVlan = %u",     ENDIAN_SWAP16(profile->client.inner_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Intf  = %u/%u",  ENDIAN_SWAP8 (profile->client.intf.intf_type), ENDIAN_SWAP8 (profile->client.intf.intf_id));

  /* Extract input data */
  evc_idx = ENDIAN_SWAP32(profile->evc_id);

  memset(&client,0x00,sizeof(ptin_client_id_t));
  if (ENDIAN_SWAP8(profile->client.mask) & MSG_CLIENT_OVLAN_MASK)
  {
    client.outerVlan = ENDIAN_SWAP16(profile->client.outer_vlan);
    client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  }
  if (ENDIAN_SWAP8(profile->client.mask) & MSG_CLIENT_IVLAN_MASK)
  {
    client.innerVlan = ENDIAN_SWAP16(profile->client.inner_vlan);
    client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
  }
  if (ENDIAN_SWAP8(profile->client.mask) & MSG_CLIENT_INTF_MASK)
  {
    client.ptin_intf.intf_type  = ENDIAN_SWAP8(profile->client.intf.intf_type);
    client.ptin_intf.intf_id    = ENDIAN_SWAP8(profile->client.intf.intf_id);
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  }

  /* Get circuit and remote ids */
  rc = ptin_dhcp_client_get(evc_idx, &client, &profile->options, &circuitId_data, L7_NULLPTR, profile->remoteId);

  profile->options          = ENDIAN_SWAP16(profile->options);
  profile->circuitId.onuid  = ENDIAN_SWAP16(circuitId_data.onuid);
  profile->circuitId.slot   = ENDIAN_SWAP8 (circuitId_data.slot);
  profile->circuitId.port   = ENDIAN_SWAP16(circuitId_data.port);
  profile->circuitId.q_vid  = ENDIAN_SWAP16(circuitId_data.q_vid);
  profile->circuitId.c_vid  = ENDIAN_SWAP16(circuitId_data.c_vid);

  if (rc!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error obtaining circuit and remote ids");
    return rc;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG,"Options                      = %02x",  ENDIAN_SWAP16(profile->options));
  PT_LOG_DEBUG(LOG_CTX_MSG,"CircuitId.onuid              = %u",    ENDIAN_SWAP16(profile->circuitId.onuid));
  PT_LOG_DEBUG(LOG_CTX_MSG,"CircuitId.slot               = %u",    ENDIAN_SWAP8 (profile->circuitId.slot));
  PT_LOG_DEBUG(LOG_CTX_MSG,"CircuitId.port               = %u",    ENDIAN_SWAP16(profile->circuitId.port));
  PT_LOG_DEBUG(LOG_CTX_MSG,"CircuitId.q_vid              = %u",    ENDIAN_SWAP16(profile->circuitId.q_vid));
  PT_LOG_DEBUG(LOG_CTX_MSG,"CircuitId.c_vid              = %u",    ENDIAN_SWAP16(profile->circuitId.c_vid));
  PT_LOG_DEBUG(LOG_CTX_MSG,"RemoteId                     = \"%s\"",profile->remoteId);

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
  if (profile == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid parameters");
    return L7_FAILURE;
  }

  for (i=0; i<n_clients; i++)
  {
    ENDIAN_SWAP8_MOD (profile[i].SlotId);
    ENDIAN_SWAP32_MOD(profile[i].evc_id);
    ENDIAN_SWAP8_MOD (profile[i].mask);
    ENDIAN_SWAP8_MOD (profile[i].intf.intf_type);
    ENDIAN_SWAP8_MOD (profile[i].intf.intf_id);
    ENDIAN_SWAP8_MOD (profile[i].client.mask);
    ENDIAN_SWAP16_MOD(profile[i].client.outer_vlan);
    ENDIAN_SWAP16_MOD(profile[i].client.inner_vlan);
    ENDIAN_SWAP8_MOD (profile[i].client.intf.intf_type);
    ENDIAN_SWAP8_MOD (profile[i].client.intf.intf_id);
    ENDIAN_SWAP16_MOD(profile[i].options);
    ENDIAN_SWAP16_MOD(profile[i].circuitId.onuid);
    ENDIAN_SWAP8_MOD (profile[i].circuitId.slot);
    ENDIAN_SWAP16_MOD(profile[i].circuitId.port);
    ENDIAN_SWAP16_MOD(profile[i].circuitId.q_vid);
    ENDIAN_SWAP16_MOD(profile[i].circuitId.c_vid);

    PT_LOG_DEBUG(LOG_CTX_MSG, "Processing message %u",i);

    PT_LOG_DEBUG(LOG_CTX_MSG, "  Slot Id                      = %u",     profile[i].SlotId);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  evc_idx                      = %u",     profile[i].evc_id);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Mask                         = 0x%02x", profile[i].mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Interface                    = %u/%u",  profile[i].intf.intf_type, profile[i].intf.intf_id);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Mask                  = 0x%02x", profile[i].client.mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.OVlan                 = %u",     profile[i].client.outer_vlan);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.IVlan                 = %u",     profile[i].client.inner_vlan);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Intf                  = %u/%u",  profile[i].client.intf.intf_type, profile[i].client.intf.intf_id);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Options                      = %04x",   profile[i].options);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  CircuitId.onuid              = %u",     profile[i].circuitId.onuid);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  CircuitId.slot               = %u",     profile[i].circuitId.slot);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  CircuitId.port               = %u",     profile[i].circuitId.port);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  CircuitId.q_vid              = %u",     profile[i].circuitId.q_vid);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Remote Id                    = \"%s\"", profile[i].remoteId);

    /* Check if all UseGlobal_DHCP_options match */
    if ( (profile[i].options & 0x02 >> 1) != ((profile[i].options & 0x08) >> 3) )
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error: UseGlobal_DHCP_options do not match");
      return L7_FAILURE;
    }
    if ( ((profile[i].options & 0x08) >> 3) != ((profile[i].options & 0x20) >> 5) )
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error: UseGlobal_DHCP_options do not match");
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
      PT_LOG_ERR(LOG_CTX_MSG, "Error adding DHCP circuitId+remoteId entry");
      return rc;
    }

    rc = ptin_pppoe_client_add(evc_idx, &client, 0, 0, profile[i].options, &circuitId, profile[i].remoteId);
    /* TODO */
#if 0
    if (rc!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error adding PPPoE circuitId+remoteId entry");
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
  if (profile == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid parameters");
    return L7_FAILURE;
  }

  for (i=0; i<n_clients; i++)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG,"Processing message %u",i);

    ENDIAN_SWAP8_MOD (profile[i].SlotId);
    ENDIAN_SWAP32_MOD(profile[i].evc_id);
    ENDIAN_SWAP8_MOD (profile[i].mask);
    ENDIAN_SWAP8_MOD (profile[i].intf.intf_type);
    ENDIAN_SWAP8_MOD (profile[i].intf.intf_id);
    ENDIAN_SWAP8_MOD (profile[i].client.mask);
    ENDIAN_SWAP16_MOD(profile[i].client.outer_vlan);
    ENDIAN_SWAP16_MOD(profile[i].client.inner_vlan);
    ENDIAN_SWAP8_MOD (profile[i].client.intf.intf_type);
    ENDIAN_SWAP8_MOD (profile[i].client.intf.intf_id);

    PT_LOG_DEBUG(LOG_CTX_MSG, "  Slot Id      = %u",     profile[i].SlotId);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  evc_idx      = %u",     profile[i].evc_id);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Mask         = 0x%02x", profile[i].mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Interface    = %u/%u",  profile[i].intf.intf_type, profile[i].intf.intf_id);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Mask  = 0x%02x", profile[i].client.mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.OVlan = %u",     profile[i].client.outer_vlan);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.IVlan = %u",     profile[i].client.inner_vlan);
    PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Intf  = %u/%u",  profile[i].client.intf.intf_type, profile[i].client.intf.intf_id);

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
    rc = ptin_dhcp_client_delete(evc_idx, &client);
    if ( rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error removing DHCP circuitId+remoteId entry");
      return rc;
    }
    rc = ptin_pppoe_client_delete(evc_idx, &client);
    /* TODO */
#if 0
    if ( rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error removing PPPoE circuitId+remoteId entry");
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

  if (dhcp_stats == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Reading client DHCP stats:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  evc_idx      = %u",     ENDIAN_SWAP32(dhcp_stats->evc_id));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Mask         = 0x%02x", ENDIAN_SWAP8 (dhcp_stats->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Interface    = %u/%u",  ENDIAN_SWAP8 (dhcp_stats->intf.intf_type), ENDIAN_SWAP8 (dhcp_stats->intf.intf_id));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Mask  = 0x%02x", ENDIAN_SWAP8 (dhcp_stats->client.mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.OVlan = %u",     ENDIAN_SWAP16(dhcp_stats->client.outer_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.IVlan = %u",     ENDIAN_SWAP16(dhcp_stats->client.inner_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Intf  = %u/%u",  ENDIAN_SWAP8 (dhcp_stats->client.intf.intf_type), ENDIAN_SWAP8 (dhcp_stats->client.intf.intf_id));

  /* Evaluate provided data */
  if ( ENDIAN_SWAP32(dhcp_stats->evc_id) == (L7_uint16)-1 ||
      !(ENDIAN_SWAP8(dhcp_stats->mask) & MSG_CLIENT_MASK) ||
       (ENDIAN_SWAP8(dhcp_stats->client.mask) == 0x00) )
  {
    PT_LOG_ERR(LOG_CTX_MSG, "MC EVC and client reference must be provided");
    return L7_FAILURE;
  }

  memset(&client,0x00,sizeof(ptin_client_id_t));
  if (ENDIAN_SWAP8(dhcp_stats->client.mask) & MSG_CLIENT_OVLAN_MASK)
  {
    client.outerVlan = ENDIAN_SWAP16(dhcp_stats->client.outer_vlan);
    client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  }
  if (ENDIAN_SWAP8(dhcp_stats->client.mask) & MSG_CLIENT_IVLAN_MASK)
  {
    client.innerVlan = ENDIAN_SWAP16(dhcp_stats->client.inner_vlan);
    client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
  }
  if (ENDIAN_SWAP8(dhcp_stats->client.mask) & MSG_CLIENT_INTF_MASK)
  {
    client.ptin_intf.intf_type  = ENDIAN_SWAP8(dhcp_stats->client.intf.intf_type);
    client.ptin_intf.intf_id    = ENDIAN_SWAP8(dhcp_stats->client.intf.intf_id);
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  }

  /* Get statistics */
  rc = ptin_dhcp_stat_client_get(dhcp_stats->evc_id, &client, &stats);

  if (rc!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error getting client statistics");
    return rc;
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Success getting client statistics");
  }

  /* Return data */
  dhcp_stats->stats.dhcp_rx_intercepted                           = ENDIAN_SWAP32(stats.dhcp_rx_intercepted);
  dhcp_stats->stats.dhcp_rx                                       = ENDIAN_SWAP32(stats.dhcp_rx);
  dhcp_stats->stats.dhcp_rx_filtered                              = ENDIAN_SWAP32(stats.dhcp_rx_filtered);
  dhcp_stats->stats.dhcp_tx_forwarded                             = ENDIAN_SWAP32(stats.dhcp_tx_forwarded);
  dhcp_stats->stats.dhcp_tx_failed                                = ENDIAN_SWAP32(stats.dhcp_tx_failed);

  dhcp_stats->stats.dhcp_rx_client_requests_without_options       = ENDIAN_SWAP32(stats.dhcp_rx_client_requests_without_options);
#if 0 /* PTin Daniel OLTTS-4141 - Removed to ensure API compatibility with manager in 3.3.0 */
  dhcp_stats->stats.dhcp_tx_client_requests_without_options       = ENDIAN_SWAP32(stats.dhcp_tx_client_requests_without_options);
#endif
  dhcp_stats->stats.dhcp_tx_client_requests_with_option82         = ENDIAN_SWAP32(stats.dhcp_tx_client_requests_with_option82);
  dhcp_stats->stats.dhcp_tx_client_requests_with_option37         = ENDIAN_SWAP32(stats.dhcp_tx_client_requests_with_option37);
  dhcp_stats->stats.dhcp_tx_client_requests_with_option18         = ENDIAN_SWAP32(stats.dhcp_tx_client_requests_with_option18);
  dhcp_stats->stats.dhcp_rx_server_replies_with_option82          = ENDIAN_SWAP32(stats.dhcp_rx_server_replies_with_option82);
  dhcp_stats->stats.dhcp_rx_server_replies_with_option37          = ENDIAN_SWAP32(stats.dhcp_rx_server_replies_with_option37);
  dhcp_stats->stats.dhcp_rx_server_replies_with_option18          = ENDIAN_SWAP32(stats.dhcp_rx_server_replies_with_option18);
#if 0 /* PTin Daniel OLTTS-4141 - Removed to ensure API compatibility with manager in 3.3.0 */
  dhcp_stats->stats.dhcp_rx_server_replies_without_options        = ENDIAN_SWAP32(stats.dhcp_rx_server_replies_without_options);
#endif
  dhcp_stats->stats.dhcp_tx_server_replies_without_options        = ENDIAN_SWAP32(stats.dhcp_tx_server_replies_without_options);

  dhcp_stats->stats.dhcp_rx_client_pkts_onTrustedIntf             = ENDIAN_SWAP32(stats.dhcp_rx_client_pkts_onTrustedIntf);
  dhcp_stats->stats.dhcp_rx_client_pkts_withOps_onUntrustedIntf   = ENDIAN_SWAP32(stats.dhcp_rx_client_pkts_withOps_onUntrustedIntf);
  dhcp_stats->stats.dhcp_rx_server_pkts_onUntrustedIntf           = ENDIAN_SWAP32(stats.dhcp_rx_server_pkts_onUntrustedIntf);
#if 1 /* PTin Daniel OLTTS-4141 - Added to ensure API compatibility with manager in 3.3.0 */
  dhcp_stats->stats.dhcp_rx_server_pkts_withoutOps_onTrustedIntf  = ENDIAN_SWAP32(0);
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

  if (dhcp_stats == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Clearing client DHCP stats:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  evc_idx      = %u",     ENDIAN_SWAP32(dhcp_stats->evc_id));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Mask         = 0x%02x", ENDIAN_SWAP8 (dhcp_stats->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Interface    = %u/%u",  ENDIAN_SWAP8 (dhcp_stats->intf.intf_type), ENDIAN_SWAP8 (dhcp_stats->intf.intf_id));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Mask  = 0x%02x", ENDIAN_SWAP8 (dhcp_stats->client.mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.OVlan = %u",     ENDIAN_SWAP16(dhcp_stats->client.outer_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.IVlan = %u",     ENDIAN_SWAP16(dhcp_stats->client.inner_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Intf  = %u/%u",  ENDIAN_SWAP8 (dhcp_stats->client.intf.intf_type), ENDIAN_SWAP8 (dhcp_stats->client.intf.intf_id));

  /* Evaluate provided data */
  if ( ENDIAN_SWAP32(dhcp_stats->evc_id) == (L7_uint16)-1 ||
      !(ENDIAN_SWAP8(dhcp_stats->mask) & MSG_CLIENT_MASK) ||
       (ENDIAN_SWAP8(dhcp_stats->client.mask) == 0x00) )
  {
    PT_LOG_ERR(LOG_CTX_MSG, "MC EVC and client reference must be provided");
    return L7_FAILURE;
  }

  memset(&client,0x00,sizeof(ptin_client_id_t));
  if (ENDIAN_SWAP8(dhcp_stats->client.mask) & MSG_CLIENT_OVLAN_MASK)
  {
    client.outerVlan = ENDIAN_SWAP16(dhcp_stats->client.outer_vlan);
    client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  }
  if (ENDIAN_SWAP8(dhcp_stats->client.mask) & MSG_CLIENT_IVLAN_MASK)
  {
    client.innerVlan = ENDIAN_SWAP16(dhcp_stats->client.inner_vlan);
    client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
  }
  if (ENDIAN_SWAP8(dhcp_stats->client.mask) & MSG_CLIENT_INTF_MASK)
  {
    client.ptin_intf.intf_type  = ENDIAN_SWAP8(dhcp_stats->client.intf.intf_type);
    client.ptin_intf.intf_id    = ENDIAN_SWAP8(dhcp_stats->client.intf.intf_id);
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  }

  /* Clear client stats */
  rc = ptin_dhcp_stat_client_clear(dhcp_stats->evc_id, &client);

  if (rc!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error clearing client statistics");
    return rc;
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Success clearing client statistics");
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

  if (dhcp_stats == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Reading interface DHCP stats:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  evc_idx      = %u",     ENDIAN_SWAP32(dhcp_stats->evc_id));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Mask         = 0x%02x", ENDIAN_SWAP8 (dhcp_stats->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Interface    = %u/%u",  ENDIAN_SWAP8 (dhcp_stats->intf.intf_type), ENDIAN_SWAP8 (dhcp_stats->intf.intf_id));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Mask  = 0x%02x", ENDIAN_SWAP8 (dhcp_stats->client.mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.OVlan = %u",     ENDIAN_SWAP16(dhcp_stats->client.outer_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.IVlan = %u",     ENDIAN_SWAP16(dhcp_stats->client.inner_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Intf  = %u/%u",  ENDIAN_SWAP8 (dhcp_stats->client.intf.intf_type), ENDIAN_SWAP8 (dhcp_stats->client.intf.intf_id));

  /* Evaluate provided data */
  if ( !(ENDIAN_SWAP8(dhcp_stats->mask) & MSG_INTERFACE_MASK) )
  {
    PT_LOG_ERR(LOG_CTX_MSG, "At least, interface must be provided");
    return L7_FAILURE;
  }

  ptin_intf.intf_type = ENDIAN_SWAP8(dhcp_stats->intf.intf_type);
  ptin_intf.intf_id   = ENDIAN_SWAP8(dhcp_stats->intf.intf_id);

  /* Get statistics */
  if (ENDIAN_SWAP32(dhcp_stats->evc_id) == (L7_uint16)-1)
  {
    rc = ptin_dhcp_stat_intf_get(&ptin_intf,&stats);

    if (rc!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error getting global interface statistics");
      return rc;
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Success getting global interface statistics");
    }
  }
  else
  {
    rc = ptin_dhcp_stat_instanceIntf_get(ENDIAN_SWAP32(dhcp_stats->evc_id), &ptin_intf, &stats);

    if (rc!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error getting statistics struct");
      return rc;
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Success getting interface statistics of one DHCP instance");
    }
  }

  /* Return data */
  dhcp_stats->stats.dhcp_rx_intercepted                           = ENDIAN_SWAP32(stats.dhcp_rx_intercepted);
  dhcp_stats->stats.dhcp_rx                                       = ENDIAN_SWAP32(stats.dhcp_rx);
  dhcp_stats->stats.dhcp_rx_filtered                              = ENDIAN_SWAP32(stats.dhcp_rx_filtered);
  dhcp_stats->stats.dhcp_tx_forwarded                             = ENDIAN_SWAP32(stats.dhcp_tx_forwarded);
  dhcp_stats->stats.dhcp_tx_failed                                = ENDIAN_SWAP32(stats.dhcp_tx_failed);

  dhcp_stats->stats.dhcp_rx_client_requests_without_options       = ENDIAN_SWAP32(stats.dhcp_rx_client_requests_without_options);
#if 0 /* PTin Daniel OLTTS-4141 - Removed to ensure API compatibility with manager in 3.3.0 */
  dhcp_stats->stats.dhcp_tx_client_requests_without_options       = ENDIAN_SWAP32(stats.dhcp_tx_client_requests_without_options);
#endif
  dhcp_stats->stats.dhcp_tx_client_requests_with_option82         = ENDIAN_SWAP32(stats.dhcp_tx_client_requests_with_option82);
  dhcp_stats->stats.dhcp_tx_client_requests_with_option37         = ENDIAN_SWAP32(stats.dhcp_tx_client_requests_with_option37);
  dhcp_stats->stats.dhcp_tx_client_requests_with_option18         = ENDIAN_SWAP32(stats.dhcp_tx_client_requests_with_option18);
  dhcp_stats->stats.dhcp_rx_server_replies_with_option82          = ENDIAN_SWAP32(stats.dhcp_rx_server_replies_with_option82);
  dhcp_stats->stats.dhcp_rx_server_replies_with_option37          = ENDIAN_SWAP32(stats.dhcp_rx_server_replies_with_option37);
  dhcp_stats->stats.dhcp_rx_server_replies_with_option18          = ENDIAN_SWAP32(stats.dhcp_rx_server_replies_with_option18);
#if 0 /* PTin Daniel OLTTS-4141 - Removed to ensure API compatibility with manager in 3.3.0 */
  dhcp_stats->stats.dhcp_rx_server_replies_without_options        = ENDIAN_SWAP32(stats.dhcp_rx_server_replies_without_options);
#endif
  dhcp_stats->stats.dhcp_tx_server_replies_without_options        = ENDIAN_SWAP32(stats.dhcp_tx_server_replies_without_options);

  dhcp_stats->stats.dhcp_rx_client_pkts_onTrustedIntf             = ENDIAN_SWAP32(stats.dhcp_rx_client_pkts_onTrustedIntf);
  dhcp_stats->stats.dhcp_rx_client_pkts_withOps_onUntrustedIntf   = ENDIAN_SWAP32(stats.dhcp_rx_client_pkts_withOps_onUntrustedIntf);
  dhcp_stats->stats.dhcp_rx_server_pkts_onUntrustedIntf           = ENDIAN_SWAP32(stats.dhcp_rx_server_pkts_onUntrustedIntf);
#if 1 /* PTin Daniel OLTTS-4141 - Added to ensure API compatibility with manager in 3.3.0 */
  dhcp_stats->stats.dhcp_rx_server_pkts_withoutOps_onTrustedIntf  = ENDIAN_SWAP32(0);
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

  if (dhcp_stats == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Clearing interface DHCP stats:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  evc_idx      = %u",     ENDIAN_SWAP32(dhcp_stats->evc_id));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Mask         = 0x%02x", ENDIAN_SWAP8 (dhcp_stats->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Interface    = %u/%u",  ENDIAN_SWAP8 (dhcp_stats->intf.intf_type), ENDIAN_SWAP8 (dhcp_stats->intf.intf_id));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Mask  = 0x%02x", ENDIAN_SWAP8 (dhcp_stats->client.mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.OVlan = %u",     ENDIAN_SWAP16(dhcp_stats->client.outer_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.IVlan = %u",     ENDIAN_SWAP16(dhcp_stats->client.inner_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Intf  = %u/%u",  ENDIAN_SWAP8 (dhcp_stats->client.intf.intf_type), ENDIAN_SWAP8 (dhcp_stats->client.intf.intf_id));

  ptin_intf.intf_type = ENDIAN_SWAP8(dhcp_stats->intf.intf_type);
  ptin_intf.intf_id   = ENDIAN_SWAP8(dhcp_stats->intf.intf_id);

  /* MC EVC not provided */
  if (ENDIAN_SWAP32(dhcp_stats->evc_id) == (L7_uint16)-1)
  {
    /* Interface not provided */
    if ( !(ENDIAN_SWAP8(dhcp_stats->mask) & MSG_INTERFACE_MASK) )
    {
      /* Clear all stats */
      rc = ptin_dhcp_stat_clearAll();

      if (rc!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error clearing all statistics data");
        return rc;
      }
      else
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "Success clearing all statistics data");
      }
    }
    /* Interface provided */
    else
    {
      /* Clear all stats of one interface */
      rc = ptin_dhcp_stat_intf_clear(&ptin_intf);

      if (rc!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error clearing statistics of one complete interface");
        return rc;
      }
      else
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "Success clearing statistics of one complete interface");
      }
    }
  }
  /* MC EVC provided */
  else
  {
    /* Interface not provided */
    if ( !(ENDIAN_SWAP8(dhcp_stats->mask) & MSG_INTERFACE_MASK) )
    {
      /* Clear stats of one dhcp instance */
      rc = ptin_dhcp_stat_instance_clear(ENDIAN_SWAP32(dhcp_stats->evc_id));

      if (rc!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error clearing statistics of one DHCP instance");
        return rc;
      }
      else
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "Success clearing statistics of one DHCP instance");
      }
    }
    /* Interface provided */
    else
    {
      /* Clear stats of one dhcp instance and one interface */
      rc = ptin_dhcp_stat_instanceIntf_clear(ENDIAN_SWAP32(dhcp_stats->evc_id), &ptin_intf);

      if (rc!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error clearing statistics of one DHCP instance + interface");
        return rc;
      }
      else
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "Success clearing statistics of one DHCP instance + interface");
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
  L7_uint8 intf;
  L7_RC_t   rc;

  /* Debug */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Binding table get:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  SlotId = %u",  ENDIAN_SWAP8 (input->slotId));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Page   = %u",  ENDIAN_SWAP16(input->page));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Mask   = %02X",ENDIAN_SWAP8 (input->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  intfId = %u",  ENDIAN_SWAP8 (input->intfId));

  page = ENDIAN_SWAP16(input->page);

  // For index null, read all mac entries
  if (page==0)
  {
    size = PLAT_MAX_FDB_MAC_ENTRIES;

    intf =(ENDIAN_SWAP8(input->mask) == 0x01) ? (ENDIAN_SWAP8(input->intfId) + 1) /* Convertion to ptinIntfNum)*/ : ((uint8) -1); //check if is slot or intf reading

    rc = ptin_dhcpv4v6_bindtable_get(dhcpv4v6_bindtable, &size, &intf);

    if (rc!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error reading binding table");
      return rc;
    }
    // Total number of entries
    dhcp_bindtable_entries = size;
  }

  // Validate page index
  if ((page*128)>dhcp_bindtable_entries)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Requested page exceeds binding table size (table size=%u, page=%u)",dhcp_bindtable_entries,page);
    return L7_FAILURE;
  }

  first   = page*128;
  entries = dhcp_bindtable_entries-first;   // Calculate remaining entries to be read
  if (entries>128)  
  {
    entries = 128;          // Overgoes 128? If so, limit to 128
  }
  PT_LOG_DEBUG(LOG_CTX_MSG, "There at least %u entries left", entries);

  output->bind_table_msg_size      = ENDIAN_SWAP16(entries);
  output->bind_table_total_entries = ENDIAN_SWAP16(dhcp_bindtable_entries);

  // Copy binding table entries
  for (i=0; i<entries; ++i)
  {
//  memset(&output->bind_table[i],0x00,sizeof(msg_DHCP_bind_entry));

    output->bind_table[i].entry_index    = ENDIAN_SWAP16(dhcpv4v6_bindtable[first+i].entry_index);
    output->bind_table[i].evc_idx        = ENDIAN_SWAP32(dhcpv4v6_bindtable[first+i].evc_idx);
    output->bind_table[i].intf.intf_type = ENDIAN_SWAP8 (dhcpv4v6_bindtable[first+i].ptin_intf.intf_type);
    output->bind_table[i].intf.intf_id   = ENDIAN_SWAP8 (dhcpv4v6_bindtable[first+i].ptin_intf.intf_id);
    output->bind_table[i].outer_vlan     = ENDIAN_SWAP16(dhcpv4v6_bindtable[first+i].outer_vlan);
    output->bind_table[i].inner_vlan     = ENDIAN_SWAP16(dhcpv4v6_bindtable[first+i].inner_vlan);
    memcpy(output->bind_table[i].macAddr, dhcpv4v6_bindtable[first+i].macAddr, sizeof(L7_uint8)*6);
    CHMSG_IP_ADDR_SWAP_COPY(output->bind_table[i].ipAddr, dhcpv4v6_bindtable[first+i].ipAddr);
    output->bind_table[i].remLeave       = ENDIAN_SWAP32(dhcpv4v6_bindtable[first+i].remLeave);
    output->bind_table[i].bindingType    = ENDIAN_SWAP8 (dhcpv4v6_bindtable[first+i].bindingType);

    PT_LOG_TRACE(LOG_CTX_MSG, "Entry %u:", first+i);
    PT_LOG_TRACE(LOG_CTX_MSG, "  entry_index = %u",    ENDIAN_SWAP16(output->bind_table[i].entry_index));
    PT_LOG_TRACE(LOG_CTX_MSG, "  evc_idx     = %u",    ENDIAN_SWAP32(output->bind_table[i].evc_idx));
    PT_LOG_TRACE(LOG_CTX_MSG, "  intf        = %u/%u", ENDIAN_SWAP8 (output->bind_table[i].intf.intf_type), ENDIAN_SWAP8(output->bind_table[i].intf.intf_id));
    PT_LOG_TRACE(LOG_CTX_MSG, "  outer_vlan  = %u",    ENDIAN_SWAP16(output->bind_table[i].outer_vlan));
    PT_LOG_TRACE(LOG_CTX_MSG, "  inner_vlan  = %u",    ENDIAN_SWAP16(output->bind_table[i].inner_vlan));
    PT_LOG_TRACE(LOG_CTX_MSG, "  macAddr     = %02X:%02X:%02X:%02X:%02X:%02X", output->bind_table[i].macAddr[0], output->bind_table[i].macAddr[1], 
                 output->bind_table[i].macAddr[2], output->bind_table[i].macAddr[3], output->bind_table[i].macAddr[4], output->bind_table[i].macAddr[5]);
    PT_LOG_TRACE(LOG_CTX_MSG, "  ipAddr      = %08X",  ENDIAN_SWAP32(output->bind_table[i].ipAddr.addr.ipv4));
    PT_LOG_TRACE(LOG_CTX_MSG, "  remLeave    = %u",    ENDIAN_SWAP32(output->bind_table[i].remLeave));
    PT_LOG_TRACE(LOG_CTX_MSG, "  bindingType = %u",    ENDIAN_SWAP8 (output->bind_table[i].bindingType));
    PT_LOG_TRACE(LOG_CTX_MSG, "  family      = %u",    ENDIAN_SWAP8 (output->bind_table[i].ipAddr.family));
  }

  return L7_SUCCESS;
}

/**
 * Remove a DHCP bind table entry
 * 
 * @param table: bind table entries 
 * @param numEntries: number of entries 
 * 
 * @return L7_RC_t: L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_DHCP_bindTable_remove(msg_DHCP_bind_table_entry_t *table, L7_uint16 numEntries)
{
  L7_uint16           i;
  dhcpSnoopBinding_t  dsBindingIpv4,dsBindingIpv6;
  L7_RC_t             rc_Ipv4, rc_Ipv6;

  if (numEntries > 128)  numEntries = 128;

  PT_LOG_DEBUG(LOG_CTX_MSG,"NumEntries=%u", numEntries);

  for (i=0; i<numEntries ; i++)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG,"Evc_idx=%u",    ENDIAN_SWAP32(table[i].bind_entry.evc_idx));
    PT_LOG_DEBUG(LOG_CTX_MSG,"Port   = %u/%u",ENDIAN_SWAP8 (table[i].bind_entry.intf.intf_type), ENDIAN_SWAP8 (table[i].bind_entry.intf.intf_id));
    PT_LOG_DEBUG(LOG_CTX_MSG,"OVlan  = %u",   ENDIAN_SWAP16(table[i].bind_entry.outer_vlan));
    PT_LOG_DEBUG(LOG_CTX_MSG,"IVlan  = %u",   ENDIAN_SWAP16(table[i].bind_entry.inner_vlan));
    PT_LOG_DEBUG(LOG_CTX_MSG,"MacAddr=%02X:%02X:%02X:%02X:%02X:%02X",
                 table[i].bind_entry.macAddr[0],
                 table[i].bind_entry.macAddr[1],
                 table[i].bind_entry.macAddr[2],
                 table[i].bind_entry.macAddr[3],
                 table[i].bind_entry.macAddr[4],
                 table[i].bind_entry.macAddr[5]);
    PT_LOG_DEBUG(LOG_CTX_MSG,"family = %u", ENDIAN_SWAP8(table[i].bind_entry.ipAddr.family));

    memset(&dsBindingIpv4,0x00,sizeof(dhcpSnoopBinding_t));
    memcpy(dsBindingIpv4.key.macAddr, table[i].bind_entry.macAddr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
    dsBindingIpv4.key.ipType = L7_AF_INET;  //(table[i].bind_entry.ipAddr.family==0) ;//? (L7_AF_INET) : (L7_AF_INET6);

    memset(&dsBindingIpv6,0x00,sizeof(dhcpSnoopBinding_t));
    memcpy(dsBindingIpv6.key.macAddr, table[i].bind_entry.macAddr, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
    dsBindingIpv6.key.ipType = L7_AF_INET6;  //(table[i].bind_entry.ipAddr.family==0) ;//? (L7_AF_INET) : (L7_AF_INET6);

    // Remove IPv6 and IPv4 entry

    rc_Ipv4 = ptin_dhcp82_bindtable_remove(&dsBindingIpv4);
    rc_Ipv6 = ptin_dhcp82_bindtable_remove(&dsBindingIpv6);

    if ((rc_Ipv4!= L7_SUCCESS) && (rc_Ipv6!=L7_SUCCESS))
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error removing entry");
      return L7_SUCCESS;
    }
    PT_LOG_DEBUG(LOG_CTX_MSG,"Success removing entry");
  }

  return L7_SUCCESS;
}

/*IP Source Guard Management Functions **************************************************/

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

  PT_LOG_DEBUG(LOG_CTX_MSG, "slotId         = %u"   , msgIpsgVerifySource->slotId);  
  PT_LOG_DEBUG(LOG_CTX_MSG, "ptinPort       = %u/%u",msgIpsgVerifySource->intf.intf_type,msgIpsgVerifySource->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "verifySource   = %s"   ,msgIpsgVerifySource->enable==L7_FALSE?"No":"Yes");
 
  /* Get intIfNum */
  ptin_intf.intf_id=msgIpsgVerifySource->intf.intf_id;
  ptin_intf.intf_type=msgIpsgVerifySource->intf.intf_type;

  if (ptin_intf_ptintf2intIfNum(&ptin_intf, &intIfNum)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error converting port %u/%u to intIfNum",ptin_intf.intf_type, ptin_intf.intf_id);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_MSG, "Port# %u/%u: intIfNum# %2u", ptin_intf.intf_type, ptin_intf.intf_id, intIfNum);

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
      PT_LOG_ERR(LOG_CTX_MSG, "Invalid value for enable:%u", msgIpsgVerifySource->enable);
      return L7_FAILURE;
    }
  }  
#else
  PT_LOG_ERR(LOG_CTX_MSG, "IP Source Guard not Supported!");
  return L7_FAILURE;
#endif
}

/**
 * Configure an IP Source Guard  static entry
 * 
 * @param msg_IPSG_static_entry_t Structure with config 
 *                                parameters
 * @param n_msg : number of structs 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_ipsg_static_entry_set(msg_IPSG_static_entry_t* msgIpsgStaticEntry, L7_uint16 n_msg)
{  
  L7_uint16         i;
  ptin_intf_t       ptin_intf;
  L7_uint32         intIfNum;
  L7_uint16         vlanId;
  L7_inet_addr_t    ipAddr;
  L7_uchar8         ipAddrStr[IPV6_DISP_ADDR_LEN]; 
  L7_enetMacAddr_t  macAddr;
  L7_RC_t           rc, rc_global = L7_SUCCESS;
  
#ifndef L7_IPSG_PACKAGE
  PT_LOG_ERR(LOG_CTX_IPSG, "IP Source Guard not Supported!");
  return L7_FAILURE;
#endif

  /* Run all structs */
  for (i = 0; i < n_msg; i++)
  {
    ENDIAN_SWAP32_MOD(msgIpsgStaticEntry[i].id);

    CHMSG_IP_ADDR_SWAP_MOD(ipAddr);

    PT_LOG_DEBUG(LOG_CTX_MSG, "slotId        = %u"   , msgIpsgStaticEntry[i].slotId);  
    PT_LOG_DEBUG(LOG_CTX_MSG, "iDType        = %u"   , msgIpsgStaticEntry[i].idType);  
    PT_LOG_DEBUG(LOG_CTX_MSG, "iD            = %u"   , msgIpsgStaticEntry[i].id);  
    PT_LOG_DEBUG(LOG_CTX_MSG, "ptinP         = %u/%u", msgIpsgStaticEntry[i].intf.intf_type,msgIpsgStaticEntry[i].intf.intf_id);  
    PT_LOG_DEBUG(LOG_CTX_MSG, "action        = %s"   , msgIpsgStaticEntry[i].action==L7_FALSE?"Remove":"Add");  
    PT_LOG_DEBUG(LOG_CTX_MSG, "MAC Addr      = %02X:%02X:%02X:%02X:%02X:%02X",msgIpsgStaticEntry[i].macAddr[0],msgIpsgStaticEntry[i].macAddr[1],
              msgIpsgStaticEntry[i].macAddr[2],msgIpsgStaticEntry[i].macAddr[3],msgIpsgStaticEntry[i].macAddr[4],msgIpsgStaticEntry[i].macAddr[5]);
    
    rc = ptin_to_fp_ip_notation(&msgIpsgStaticEntry[i].ipAddr,&ipAddr);
    if ( rc != L7_SUCCESS)
    {
      rc_global = rc;
      continue;
    }

    memcpy(&macAddr, msgIpsgStaticEntry[i].macAddr, sizeof(macAddr));
    
    inetAddrPrint(&ipAddr, ipAddrStr);    

    PT_LOG_DEBUG(LOG_CTX_MSG, "IP Address    = %s",ipAddrStr);
     
    /* Get intIfNum */
    ptin_intf.intf_id   = msgIpsgStaticEntry[i].intf.intf_id;
    ptin_intf.intf_type = msgIpsgStaticEntry[i].intf.intf_type;

    rc = ptin_intf_ptintf2intIfNum(&ptin_intf, &intIfNum);
    if (rc != L7_SUCCESS)
    {
      rc_global = rc;
      PT_LOG_ERR(LOG_CTX_MSG, "Error converting port %u/%u to intIfNum",ptin_intf.intf_type, ptin_intf.intf_id);
      continue;
    }
    PT_LOG_TRACE(LOG_CTX_MSG, "Port# %u/%u: intIfNum# %2u", ptin_intf.intf_type, ptin_intf.intf_id, intIfNum);

    if( (msgIpsgStaticEntry[i].idType & IPSG_ID_ALL) == IPSG_EVC_ID)
    {
      /* Get Internal root vlan */
      rc = ptin_evc_intRootVlan_get(msgIpsgStaticEntry[i].id, &vlanId);
      if (rc != L7_SUCCESS)
      {
        rc_global = L7_NOT_EXIST;
        PT_LOG_ERR(LOG_CTX_MSG,"Error getting internal root vlan for eEVCId=%u", msgIpsgStaticEntry[i].id);
        continue;
      }
      PT_LOG_TRACE(LOG_CTX_MSG, "EVCidx# %u: internalRootVlan# %u",msgIpsgStaticEntry[i].id,vlanId);
    }
    else
    {
      if ( (msgIpsgStaticEntry[i].idType & IPSG_ID_ALL) == IPSG_ROOT_VLAN )
      {
  #if 0
        L7_uint32         eEVCId;
  #endif
        if ((vlanId = (0x0000FFFF & msgIpsgStaticEntry[i].id)) != msgIpsgStaticEntry[i].id && vlanId >= 4096)
        {
          rc_global = L7_FAILURE;
          PT_LOG_ERR(LOG_CTX_MSG,"Invalid root vlan given:%u", msgIpsgStaticEntry[i].id);
          continue;
        }
  /*Disabled this verification to support MAC Bridge Services*/
  #if 0
        if (ptin_evc_get_evcIdfromIntVlan(vlanId, &eEVCId) != L7_SUCCESS)
        {
          rc_global = L7_NOT_EXIST;
          PT_LOG_ERR(LOG_CTX_IGMP,"Invalid root VLAN:%u", vlanId);
          continue;
        }
        PT_LOG_TRACE(LOG_CTX_MSG, "EVCidx# %u: internalRootVlan# %u",eEVCId, vlanId);
  #endif
      }
      else
      {
        rc_global = L7_FAILURE;
        PT_LOG_ERR(LOG_CTX_MSG,"Invalid IdType:%u", msgIpsgStaticEntry[i].idType);
        continue;
      }
    }

    /* IPSG entry add/remove */
    if ((msgIpsgStaticEntry[i].action & IPSG_ACTION_ADD) == IPSG_ACTION_ADD)
    {
      rc = ipsgStaticEntryAdd(intIfNum, vlanId, &macAddr, &ipAddr);
    }
    else
    {
      rc = ipsgStaticEntryRemove(intIfNum, vlanId, &macAddr, &ipAddr);
    }
    /* Check for errors */
    if (rc != L7_SUCCESS)
    {
      rc_global = rc;
      continue;
    }
  }

  return rc_global;
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
  PT_LOG_NOTICE(LOG_CTX_IPSG, "Not Implemented Yet!");
  return L7_NOT_IMPLEMENTED_YET;
#else
  PT_LOG_ERR(LOG_CTX_IPSG, "IP Source Guard not Supported!");
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
L7_RC_t ptin_msg_igmp_admission_control_set(msg_IgmpAdmissionControl_t *msgAdmissionControl)
{  
#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT                                     

  ptin_igmp_admission_control_t igmpAdmissionControl;                             
  ptin_intf_t                   intf;  

  if (msgAdmissionControl == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid Input Parameters: igmpAdmissionControl=%p", msgAdmissionControl);
    return L7_FAILURE;
  }
   /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "SlotId       = %u"        , msgAdmissionControl->SlotId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "mask         = 0x%02X"    , msgAdmissionControl->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "evcId        = %u"        , ENDIAN_SWAP32(msgAdmissionControl->evcId));
  PT_LOG_DEBUG(LOG_CTX_MSG, "intf         = %u/%u"     , msgAdmissionControl->intf.intf_type, msgAdmissionControl->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "onuId        = %u"        , msgAdmissionControl->onuId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "outer_vlan   = %u"        , ENDIAN_SWAP16(msgAdmissionControl->outer_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "inner_vlan   = %u"        , ENDIAN_SWAP16(msgAdmissionControl->inner_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "maxChannels  = %hu"       , ENDIAN_SWAP16(msgAdmissionControl->maxChannels));
  PT_LOG_DEBUG(LOG_CTX_MSG, "maxBandwidth = %llu bit/s", ENDIAN_SWAP64(msgAdmissionControl->maxBandwidth));

  if ( ((msgAdmissionControl->mask & PTIN_MSG_ADMISSION_CONTROL_MASK_INTF) !=  PTIN_MSG_ADMISSION_CONTROL_MASK_INTF) ||
       ((msgAdmissionControl->mask & PTIN_MSG_ADMISSION_CONTROL_MASK_EVCID) !=  PTIN_MSG_ADMISSION_CONTROL_MASK_EVCID) ||
      #if  !PTIN_BOARD_IS_ACTIVETH
       ((msgAdmissionControl->mask & PTIN_MSG_ADMISSION_CONTROL_MASK_ONUID) !=  PTIN_MSG_ADMISSION_CONTROL_MASK_ONUID) ||
       #endif         
      ( ( (msgAdmissionControl->mask & PTIN_MSG_ADMISSION_CONTROL_MASK_MAX_BANDWIDTH) == PTIN_MSG_ADMISSION_CONTROL_MASK_MAX_BANDWIDTH ) &&
        (ENDIAN_SWAP64(msgAdmissionControl->maxBandwidth) != PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_BPS_DISABLE && ENDIAN_SWAP64(msgAdmissionControl->maxBandwidth) > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_BPS) ) ||
       ( ( (msgAdmissionControl->mask & PTIN_MSG_ADMISSION_CONTROL_MASK_MAX_CHANNELS) == PTIN_MSG_ADMISSION_CONTROL_MASK_MAX_CHANNELS ) &&
        (ENDIAN_SWAP16(msgAdmissionControl->maxChannels) != PTIN_IGMP_ADMISSION_CONTROL_MAX_CHANNELS_DISABLE && ENDIAN_SWAP16(msgAdmissionControl->maxChannels) > PTIN_IGMP_ADMISSION_CONTROL_MAX_CHANNELS) ) )
      
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid Admission Control Parameters [mask:0x%02x maxChannels:%hu maxBandwidth:%llu bits/s", msgAdmissionControl->mask, ENDIAN_SWAP16(msgAdmissionControl->maxChannels), ENDIAN_SWAP64(msgAdmissionControl->maxBandwidth));
    return L7_FAILURE;
  }

  igmpAdmissionControl.mask = 0x00;
 
  if ( (msgAdmissionControl->mask & PTIN_MSG_ADMISSION_CONTROL_MASK_MAX_CHANNELS) == PTIN_MSG_ADMISSION_CONTROL_MASK_MAX_CHANNELS)
    igmpAdmissionControl.mask = PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_CHANNELS;

  if ( (msgAdmissionControl->mask & PTIN_MSG_ADMISSION_CONTROL_MASK_MAX_BANDWIDTH) == PTIN_MSG_ADMISSION_CONTROL_MASK_MAX_BANDWIDTH)
    igmpAdmissionControl.mask |= PTIN_IGMP_ADMISSION_CONTROL_MASK_MAX_ALLOWED_BANDWIDTH;

  /*If Mask Is Set */
  if (igmpAdmissionControl.mask != 0x00)
  {
    intf.intf_id = msgAdmissionControl->intf.intf_id;  
    intf.intf_type = msgAdmissionControl->intf.intf_type;  
    if (ptin_intf_ptintf2port(&intf, &igmpAdmissionControl.ptin_port) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Failed to obtain ptin_port from ptin_intf [ptin_intf.intf_type:%u ptin_intf:%u]",intf.intf_type, intf.intf_id);
      return L7_FAILURE;
    }
   
    igmpAdmissionControl.serviceId      = ENDIAN_SWAP32(msgAdmissionControl->evcId);    
#if  !PTIN_BOARD_IS_ACTIVETH
    igmpAdmissionControl.onuId         = msgAdmissionControl->onuId;
#else
    igmpAdmissionControl.onuId         = 0;
#endif  
    igmpAdmissionControl.maxAllowedChannels   = ENDIAN_SWAP16(msgAdmissionControl->maxChannels);
    igmpAdmissionControl.maxAllowedBandwidth  = ENDIAN_SWAP64(msgAdmissionControl->maxBandwidth);

    if (ptin_igmp_multicast_service_add(igmpAdmissionControl.ptin_port, igmpAdmissionControl.onuId, igmpAdmissionControl.serviceId) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Failed to add multicast service");
      return L7_FAILURE;
    }

    if (ptin_igmp_admission_control_multicast_service_set(&igmpAdmissionControl) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Failed to set multicast admission control parameters");
      return L7_FAILURE;
    }     
  }
  else
  {
    PT_LOG_NOTICE(LOG_CTX_MSG,"Ignoring Request - Admission Control Mask is 0x00!");    
  }
#endif 
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
  ptinMgmdProxy.mask                                   = ENDIAN_SWAP16(msgIgmpProxy->mask);
  ptinMgmdProxy.admin                                  = msgIgmpProxy->admin;
  ptinMgmdProxy.networkVersion                         = msgIgmpProxy->networkVersion;
  ptinMgmdProxy.clientVersion                          = msgIgmpProxy->clientVersion;
  ptinMgmdProxy.ipv4Addr                               = ENDIAN_SWAP32(msgIgmpProxy->ipv4_addr.s_addr);
  ptinMgmdProxy.igmpCos                                = msgIgmpProxy->igmp_cos;
  ptinMgmdProxy.fastLeave                              = msgIgmpProxy->fast_leave;

  ptinMgmdProxy.querier.mask                           = ENDIAN_SWAP16(msgIgmpProxy->querier.mask);
  ptinMgmdProxy.querier.flags                          = msgIgmpProxy->querier.flags;
  ptinMgmdProxy.querier.robustness                     = msgIgmpProxy->querier.robustness;
  ptinMgmdProxy.querier.queryInterval                  = ENDIAN_SWAP16(msgIgmpProxy->querier.query_interval);
  ptinMgmdProxy.querier.queryResponseInterval          = ENDIAN_SWAP16(msgIgmpProxy->querier.query_response_interval);
  ptinMgmdProxy.querier.groupMembershipInterval        = ENDIAN_SWAP16(msgIgmpProxy->querier.group_membership_interval);
  ptinMgmdProxy.querier.otherQuerierPresentInterval    = ENDIAN_SWAP16(msgIgmpProxy->querier.other_querier_present_interval);
  ptinMgmdProxy.querier.startupQueryInterval           = ENDIAN_SWAP16(msgIgmpProxy->querier.startup_query_interval);
  ptinMgmdProxy.querier.startupQueryCount              = ENDIAN_SWAP16(msgIgmpProxy->querier.startup_query_count);
  ptinMgmdProxy.querier.lastMemberQueryInterval        = ENDIAN_SWAP16(msgIgmpProxy->querier.last_member_query_interval);
  ptinMgmdProxy.querier.lastMemberQueryCount           = ENDIAN_SWAP16(msgIgmpProxy->querier.last_member_query_count);
  ptinMgmdProxy.querier.olderHostPresentTimeout        = ENDIAN_SWAP16(msgIgmpProxy->querier.older_host_present_timeout);

  ptinMgmdProxy.host.mask                              = msgIgmpProxy->host.mask;
  ptinMgmdProxy.host.flags                             = msgIgmpProxy->host.flags;
  ptinMgmdProxy.host.robustness                        = msgIgmpProxy->host.robustness;
  ptinMgmdProxy.host.unsolicitedReportInterval         = ENDIAN_SWAP16(msgIgmpProxy->host.unsolicited_report_interval);
  ptinMgmdProxy.host.olderQuerierPresentTimeout        = ENDIAN_SWAP16(msgIgmpProxy->host.older_querier_present_timeout);
  ptinMgmdProxy.host.maxRecordsPerReport               = msgIgmpProxy->host.max_records_per_report;

  ptinMgmdProxy.bandwidthControl                       = msgIgmpProxy->bandwidthControl;
  ptinMgmdProxy.channelsControl                        = msgIgmpProxy->channelsControl;

  
#if PTIN_BOARD_IS_MATRIX
  ptinMgmdProxy.whiteList                              = L7_DISABLE;
#else
  ptinMgmdProxy.whiteList                              = ENDIAN_SWAP32(1);//L7_ENABLE;
#endif
                         
  ptinMgmdProxy.mask                                  |= PTIN_MGMD_CONFIG_WHITELIST_MASK;

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "IGMP Proxy (mask=0x%08X)", ENDIAN_SWAP16(ptinMgmdProxy.mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Admin #                          = %u", ptinMgmdProxy.admin);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Network Version                  = %u", ptinMgmdProxy.networkVersion);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client Version                   = %u", ptinMgmdProxy.clientVersion);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  IP Addr                          = %u.%u.%u.%u", (ENDIAN_SWAP32(ptinMgmdProxy.ipv4Addr)>>24)&0xFF, (ENDIAN_SWAP32(ptinMgmdProxy.ipv4Addr)>>16)&0xFF, 
                                                                                  (ENDIAN_SWAP32(ptinMgmdProxy.ipv4Addr)>>8)&0xFF, ENDIAN_SWAP32(ptinMgmdProxy.ipv4Addr)&0xFF);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  COS                              = %u", ptinMgmdProxy.igmpCos);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  FastLeave                        = %s", ptinMgmdProxy.fastLeave != 0 ? "ON":"OFF");  
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Querier (mask=0x%08X)", ENDIAN_SWAP16(ptinMgmdProxy.querier.mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Flags                          = 0x%04X", ptinMgmdProxy.querier.flags);
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Robustness                     = %u", ptinMgmdProxy.querier.robustness);
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Query Interval                 = %u", ENDIAN_SWAP16(ptinMgmdProxy.querier.queryInterval));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Query Response Interval        = %u", ENDIAN_SWAP16(ptinMgmdProxy.querier.queryResponseInterval));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Group Membership Interval      = %u", ENDIAN_SWAP16(ptinMgmdProxy.querier.groupMembershipInterval));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Other Querier Present Interval = %u", ENDIAN_SWAP16(ptinMgmdProxy.querier.otherQuerierPresentInterval));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Startup Query Interval         = %u", ENDIAN_SWAP16(ptinMgmdProxy.querier.startupQueryInterval));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Startup Query Count            = %u", ENDIAN_SWAP16(ptinMgmdProxy.querier.startupQueryCount));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Last Member Query Interval     = %u", ENDIAN_SWAP16(ptinMgmdProxy.querier.lastMemberQueryInterval));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Last Member Query Count        = %u", ENDIAN_SWAP16(ptinMgmdProxy.querier.lastMemberQueryCount));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Older Host Present Timeout     = %u", ENDIAN_SWAP16(ptinMgmdProxy.querier.olderHostPresentTimeout));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Host (mask=0x%08X)", ptinMgmdProxy.host.mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Flags                          = 0x%02X", ptinMgmdProxy.host.flags);
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Robustness                     = %u", ptinMgmdProxy.host.robustness);
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Unsolicited Report Interval    = %u", ENDIAN_SWAP16(ptinMgmdProxy.host.unsolicitedReportInterval));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Older Querier Present Timeout  = %u", ENDIAN_SWAP16(ptinMgmdProxy.host.olderQuerierPresentTimeout));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Max Group Records per Packet   = %u", ptinMgmdProxy.host.maxRecordsPerReport);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Bandwidth Control                = %s", ptinMgmdProxy.bandwidthControl != 0 ? "ON":"OFF");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Channels Control                 = %s", ptinMgmdProxy.channelsControl != 0 ? "ON":"OFF");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  WhiteList                        = %s", ptinMgmdProxy.whiteList != 0 ? "ON":"OFF");
  
  /* Apply config */
  rc = ptin_igmp_proxy_config_set(&ptinMgmdProxy);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error setting IGMP Proxy config");
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
    PT_LOG_ERR(LOG_CTX_MSG, "Error getting IGMP Proxy config");
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "IGMP Proxy (mask=0x%08X)", ENDIAN_SWAP16(ptinIgmpProxy.mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Admin #                          = %u", ptinIgmpProxy.admin);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Network Version                  = %u", ptinIgmpProxy.networkVersion);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client Version                   = %u", ptinIgmpProxy.clientVersion);  
  PT_LOG_DEBUG(LOG_CTX_MSG, "  IP Addr                          = %u.%u.%u.%u", (ENDIAN_SWAP32(ptinIgmpProxy.ipv4Addr) >> 24) & 0xFF, (ENDIAN_SWAP32(ptinIgmpProxy.ipv4Addr) >> 16) & 0xFF,
                                                                                 (ENDIAN_SWAP32(ptinIgmpProxy.ipv4Addr) >>  8) & 0xFF,  (ENDIAN_SWAP32(ptinIgmpProxy.ipv4Addr)       & 0xFF));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  COS                              = %u", ptinIgmpProxy.igmpCos);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  FastLeave                        = %s", ptinIgmpProxy.fastLeave != 0 ? "ON":"OFF");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Querier (mask=0x%08X)", ENDIAN_SWAP16(ptinIgmpProxy.querier.mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Flags                          = 0x%08X", ptinIgmpProxy.querier.flags);  
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Robustness                     = %u", ENDIAN_SWAP16(ptinIgmpProxy.querier.robustness));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Query Interval                 = %u", ENDIAN_SWAP16(ptinIgmpProxy.querier.queryInterval));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Query Response Interval        = %u", ENDIAN_SWAP16(ptinIgmpProxy.querier.queryResponseInterval));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Group Membership Interval      = %u", ENDIAN_SWAP16(ptinIgmpProxy.querier.groupMembershipInterval));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Other Querier Present Interval = %u", ENDIAN_SWAP16(ptinIgmpProxy.querier.otherQuerierPresentInterval));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Startup Query Interval         = %u", ENDIAN_SWAP16(ptinIgmpProxy.querier.startupQueryInterval));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Startup Query Count            = %u", ENDIAN_SWAP16(ptinIgmpProxy.querier.startupQueryCount));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Last Member Query Interval     = %u", ENDIAN_SWAP16(ptinIgmpProxy.querier.lastMemberQueryInterval));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Last Member Query Count        = %u", ENDIAN_SWAP16(ptinIgmpProxy.querier.lastMemberQueryCount));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Older Host Present Timeout     = %u", ENDIAN_SWAP16(ptinIgmpProxy.querier.olderHostPresentTimeout));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Host (mask=0x%08X)", ptinIgmpProxy.host.mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Flags                          = 0x%02X", ptinIgmpProxy.host.flags);  
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Robustness                     = %u", ptinIgmpProxy.host.robustness);
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Unsolicited Report Interval    = %u", ENDIAN_SWAP16(ptinIgmpProxy.host.unsolicitedReportInterval));
  PT_LOG_DEBUG(LOG_CTX_MSG, "    Older Querier Present  Timeout = %u", ptinIgmpProxy.host.olderQuerierPresentTimeout);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Bandwidth Control                = %s", ptinIgmpProxy.bandwidthControl != 0 ? "ON":"OFF");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Channels Control                 = %s", ptinIgmpProxy.channelsControl != 0 ? "ON":"OFF");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  WhiteList                        = %s", ptinIgmpProxy.whiteList != 0 ? "ON":"OFF");

  /* Copy data */
  msgIgmpProxy->mask                                   = ENDIAN_SWAP16(ptinIgmpProxy.mask);
  msgIgmpProxy->admin                                  = ptinIgmpProxy.admin;
  msgIgmpProxy->networkVersion                         = ptinIgmpProxy.networkVersion;
  msgIgmpProxy->clientVersion                          = ptinIgmpProxy.clientVersion;
  msgIgmpProxy->ipv4_addr.s_addr                       = ENDIAN_SWAP32(ptinIgmpProxy.ipv4Addr);
  msgIgmpProxy->igmp_cos                               = ptinIgmpProxy.igmpCos;
  msgIgmpProxy->fast_leave                             = ptinIgmpProxy.fastLeave;

  msgIgmpProxy->querier.mask                           = ENDIAN_SWAP16(ptinIgmpProxy.querier.mask);
  msgIgmpProxy->querier.flags                          = ptinIgmpProxy.querier.flags;  
  msgIgmpProxy->querier.robustness                     = ptinIgmpProxy.querier.robustness;
  msgIgmpProxy->querier.query_interval                 = ENDIAN_SWAP16(ptinIgmpProxy.querier.queryInterval);
  msgIgmpProxy->querier.query_response_interval        = ENDIAN_SWAP16(ptinIgmpProxy.querier.queryResponseInterval);
  msgIgmpProxy->querier.group_membership_interval      = ENDIAN_SWAP16(ptinIgmpProxy.querier.groupMembershipInterval);
  msgIgmpProxy->querier.other_querier_present_interval = ENDIAN_SWAP16(ptinIgmpProxy.querier.otherQuerierPresentInterval);
  msgIgmpProxy->querier.startup_query_interval         = ENDIAN_SWAP16(ptinIgmpProxy.querier.startupQueryInterval);
  msgIgmpProxy->querier.startup_query_count            = ENDIAN_SWAP16(ptinIgmpProxy.querier.startupQueryCount);
  msgIgmpProxy->querier.last_member_query_interval     = ENDIAN_SWAP16(ptinIgmpProxy.querier.lastMemberQueryInterval);
  msgIgmpProxy->querier.last_member_query_count        = ENDIAN_SWAP16(ptinIgmpProxy.querier.lastMemberQueryCount);
  msgIgmpProxy->querier.older_host_present_timeout     = ENDIAN_SWAP16(ptinIgmpProxy.querier.olderHostPresentTimeout);

  msgIgmpProxy->host.mask                              = ptinIgmpProxy.host.mask;
  msgIgmpProxy->host.flags                             = ptinIgmpProxy.host.flags;  
  msgIgmpProxy->host.robustness                        = ptinIgmpProxy.host.robustness;
  msgIgmpProxy->host.unsolicited_report_interval       = ENDIAN_SWAP16(ptinIgmpProxy.host.unsolicitedReportInterval);
  msgIgmpProxy->host.older_querier_present_timeout     = ENDIAN_SWAP16(ptinIgmpProxy.host.olderQuerierPresentTimeout);

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
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Going to add IGMP Instance:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  MC evc_idx = %u", ENDIAN_SWAP32(msgIgmpInst->multicastEvcId));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  UC evc_idx = %u", ENDIAN_SWAP32(msgIgmpInst->unicastEvcId));

  /* Apply config */
  rc = ptin_igmp_instance_add(ENDIAN_SWAP32(msgIgmpInst->multicastEvcId),ENDIAN_SWAP32(msgIgmpInst->unicastEvcId));

  if (rc!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error creating/updating IGMP instance");
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
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Going to remove IGMP Instance:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  MC evc_idx = %u", ENDIAN_SWAP32(msgIgmpInst->multicastEvcId));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  UC evc_idx = %u", ENDIAN_SWAP32(msgIgmpInst->unicastEvcId));

  /* Apply config */
  rc = ptin_igmp_instance_remove(ENDIAN_SWAP32(msgIgmpInst->multicastEvcId),ENDIAN_SWAP32(msgIgmpInst->unicastEvcId));

  if (rc!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error removing IGMP instance");
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
  L7_uint16        i;  
  L7_uint32        intIfNum;
  ptin_client_id_t client;  
  L7_uint16        uni_ivid;
  L7_uint16        uni_ovid;
  L7_RC_t          rc;

  if (McastClient==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  for (i=0; i<n_clients; i++)
  {
    /* Output data */
    PT_LOG_DEBUG(LOG_CTX_MSG, "Going to remove MC client");
    PT_LOG_DEBUG(LOG_CTX_MSG, "  MC evc_idx = %u", ENDIAN_SWAP32(McastClient[i].mcEvcId));
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.Mask  = 0x%02x", McastClient[i].client.mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.OVlan = %u", ENDIAN_SWAP16(McastClient[i].client.outer_vlan));
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.IVlan = %u", ENDIAN_SWAP16(McastClient[i].client.inner_vlan));
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.Intf  = %u/%u", McastClient[i].client.intf.intf_type,McastClient[i].client.intf.intf_id);

    if (McastClient[i].mask > PTIN_MSG_IGMP_CLIENT_MASK_VALID)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Invalid Mask [mask:0x%02x]",McastClient[i].mask, ENDIAN_SWAP64(McastClient[i].maxBandwidth), ENDIAN_SWAP16(McastClient[i].maxChannels));
      return L7_FAILURE;
    }

#if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT                                         
      if ( ( ( (McastClient[i].mask & PTIN_MSG_IGMP_CLIENT_MASK_MAX_ALLOWED_BANDWIDTH) == PTIN_MSG_IGMP_CLIENT_MASK_MAX_ALLOWED_BANDWIDTH ) &&
            (ENDIAN_SWAP64(McastClient[i].maxBandwidth) != PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_BPS_DISABLE && ENDIAN_SWAP64(McastClient[i].maxBandwidth) > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_BPS) ) ||
           ( ( (McastClient[i].mask & PTIN_MSG_IGMP_CLIENT_MASK_MAX_ALLOWED_CHANNELS) == PTIN_MSG_IGMP_CLIENT_MASK_MAX_ALLOWED_CHANNELS ) &&
            (ENDIAN_SWAP16(McastClient[i].maxChannels) != PTIN_IGMP_ADMISSION_CONTROL_MAX_CHANNELS_DISABLE && ENDIAN_SWAP16(McastClient[i].maxChannels) > PTIN_IGMP_ADMISSION_CONTROL_MAX_CHANNELS) ) )
          
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Invalid Admission Control Parameters [mask:0x%02x maxBandwidth:%llu bits/s maxChannels:%hu",McastClient[i].mask, ENDIAN_SWAP64(McastClient[i].maxBandwidth), ENDIAN_SWAP16(McastClient[i].maxChannels));
        return L7_FAILURE;
      }

      PT_LOG_DEBUG(LOG_CTX_MSG, "   onuId        = %u", McastClient[i].onuId);
      PT_LOG_DEBUG(LOG_CTX_MSG, "   mask         = %u", McastClient[i].mask);
      PT_LOG_DEBUG(LOG_CTX_MSG, "   maxChannels  = %u", ENDIAN_SWAP16(McastClient[i].maxChannels));
      PT_LOG_DEBUG(LOG_CTX_MSG, "   maxBandwidth = %llu bit/s ", ENDIAN_SWAP64(McastClient[i].maxBandwidth));
#endif
 
    memset(&client,0x00,sizeof(ptin_client_id_t));
    if (McastClient[i].client.mask & MSG_CLIENT_OVLAN_MASK)
    {
      client.outerVlan = ENDIAN_SWAP16(McastClient[i].client.outer_vlan);
      client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
    }
    if (McastClient[i].client.mask & MSG_CLIENT_IVLAN_MASK)
    {
      client.innerVlan = ENDIAN_SWAP16(McastClient[i].client.inner_vlan);
      client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
    }
    if (McastClient[i].client.mask & MSG_CLIENT_INTF_MASK)
    {
      client.ptin_intf.intf_type  = McastClient[i].client.intf.intf_type;
      client.ptin_intf.intf_id    = McastClient[i].client.intf.intf_id;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
    }

    {
      rc = ptin_igmp_clientId_convert(ENDIAN_SWAP32(McastClient[i].mcEvcId), &client);
      if ( rc != L7_SUCCESS )
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error converting clientId");
        continue;
      }

      /* Get interface as intIfNum format */      
      if (ptin_intf_ptintf2intIfNum(&client.ptin_intf, &intIfNum)==L7_SUCCESS)
      {
        if (ptin_evc_extVlans_get(intIfNum, ENDIAN_SWAP32(McastClient[i].mcEvcId),(L7_uint32)-1, client.innerVlan, &uni_ovid, &uni_ivid) == L7_SUCCESS)
        {
          PT_LOG_TRACE(LOG_CTX_IGMP,"Ext vlans for ptin_intf %u/%u, cvlan %u: uni_ovid=%u, uni_ivid=%u",
                    client.ptin_intf.intf_type,client.ptin_intf.intf_id, client.innerVlan, uni_ovid, uni_ivid);
        }
        else
        {
          uni_ovid = uni_ivid = 0;
          PT_LOG_ERR(LOG_CTX_IGMP,"Cannot get ext vlans for ptin_intf %u/%u, cvlan %u",
                  client.ptin_intf.intf_type,client.ptin_intf.intf_id, client.innerVlan);
        }
      }
      else
      {
        PT_LOG_ERR(LOG_CTX_IGMP,"Invalid ptin_intf %u/%u", client.ptin_intf.intf_type, client.ptin_intf.intf_id);
      }
    }
    
    /* Apply config */
    rc = ptin_igmp_api_client_add(&client, uni_ovid, uni_ivid, McastClient[i].onuId, McastClient[i].mask, ENDIAN_SWAP64(McastClient[i].maxBandwidth), ENDIAN_SWAP16(McastClient[i].maxChannels), L7_FALSE, L7_NULLPTR/*McastClient[i].packageBmpList*/, 0/*McastClient[i].noOfPackages*/);          

    if (rc!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error adding MC client");
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
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  for (i=0; i<n_clients; i++)
  {
    /* Output data */
    PT_LOG_DEBUG(LOG_CTX_MSG, "Going to remove MC client");
    PT_LOG_DEBUG(LOG_CTX_MSG, "  MC evc_idx = %u", ENDIAN_SWAP32(McastClient[i].mcEvcId));
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.Mask  = 0x%02x", McastClient[i].client.mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.OVlan = %u", ENDIAN_SWAP16(McastClient[i].client.outer_vlan));
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.IVlan = %u", ENDIAN_SWAP16(McastClient[i].client.inner_vlan));
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.Intf  = %u/%u", McastClient[i].client.intf.intf_type,McastClient[i].client.intf.intf_id);

    memset(&client,0x00,sizeof(ptin_client_id_t));
    if (McastClient[i].client.mask & MSG_CLIENT_OVLAN_MASK)
    {
      client.outerVlan = ENDIAN_SWAP16(McastClient[i].client.outer_vlan);
      client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
    }
    if (McastClient[i].client.mask & MSG_CLIENT_IVLAN_MASK)
    {
      client.innerVlan = ENDIAN_SWAP16(McastClient[i].client.inner_vlan);
      client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
    }
    if (McastClient[i].client.mask & MSG_CLIENT_INTF_MASK)
    {
      client.ptin_intf.intf_type  = McastClient[i].client.intf.intf_type;
      client.ptin_intf.intf_id    = McastClient[i].client.intf.intf_id;
      client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
    }

    rc = ptin_igmp_clientId_convert(ENDIAN_SWAP32(McastClient[i].mcEvcId), &client);
    if ( rc != L7_SUCCESS )
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error converting clientId");
      continue;
    }

    /* Apply config */
    rc = ptin_igmp_api_client_remove(&client);
    if ( rc != L7_SUCCESS )
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error removing MC client");
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
  L7_uint32                       aux_mcEvcId;

  if (igmp_stats==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  aux_mcEvcId = ENDIAN_SWAP32(igmp_stats->mcEvcId);

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Reading interface IGMP stats:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  MC evc_idx   = %u", aux_mcEvcId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Mask         = 0x%02x", igmp_stats->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Interface    = %u/%u", igmp_stats->intf.intf_type,igmp_stats->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Mask  = 0x%02x", igmp_stats->client.mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.OVlan = %u", ENDIAN_SWAP16(igmp_stats->client.outer_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.IVlan = %u", ENDIAN_SWAP16(igmp_stats->client.inner_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Intf  = %u/%u", igmp_stats->client.intf.intf_type, igmp_stats->client.intf.intf_id);

  //Short Fix to Support Mac Bridge Services and Unicast Services 
  #if PTIN_BOARD_IS_LINECARD
  {
    #if 0
    L7_BOOL isMacBridge;    
    if (ptin_evc_mac_bridge_check(igmp_stats->mcEvcId, &isMacBridge)==L7_SUCCESS && isMacBridge==L7_TRUE)
    #else
    if (ENDIAN_SWAP16(igmp_stats->client.outer_vlan)==0) 
    #endif
    {      
      igmp_stats->client.outer_vlan= ENDIAN_SWAP16(igmp_stats->client.inner_vlan);      
    }    
    igmp_stats->client.mask|=MSG_CLIENT_OVLAN_MASK;    
    PT_LOG_DEBUG(LOG_CTX_MSG,"Converted [client.Mask:%u Client.OVlan:%u]",igmp_stats->client.mask, ENDIAN_SWAP16(igmp_stats->client.outer_vlan));
  }  
  #endif

  /* Evaluate provided data */
  if ( aux_mcEvcId ==(L7_uint16)-1 ||
       !(igmp_stats->mask & MSG_CLIENT_MASK) ||
       (igmp_stats->client.mask == 0x00) )
  {
    PT_LOG_ERR(LOG_CTX_MSG, "MC EVC and client reference must be provided");
    return L7_FAILURE;
  }

  memset(&client,0x00,sizeof(ptin_client_id_t));
  if (igmp_stats->client.mask & MSG_CLIENT_OVLAN_MASK)
  {
    client.outerVlan = ENDIAN_SWAP16(igmp_stats->client.outer_vlan);
    client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  }
  if (igmp_stats->client.mask & MSG_CLIENT_IVLAN_MASK)
  {
    client.innerVlan = ENDIAN_SWAP16(igmp_stats->client.inner_vlan);
    client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
  }
  if (igmp_stats->client.mask & MSG_CLIENT_INTF_MASK)
  {
    client.ptin_intf.intf_type  = igmp_stats->client.intf.intf_type;
    client.ptin_intf.intf_id    = igmp_stats->client.intf.intf_id;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  }

  /* Get statistics */
  rc = ptin_igmp_stat_client_get(aux_mcEvcId,&client,&stats);
  if (rc!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error getting client statistics");
    return rc;
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Success getting client statistics");
  }

  igmp_stats->stats.active_groups                                                    = ENDIAN_SWAP32(stats.activeGroups);            
  igmp_stats->stats.active_clients                                                   = ENDIAN_SWAP32(stats.activeClients); 
            
  igmp_stats->stats.igmp_tx                                                          = ENDIAN_SWAP32(stats.igmpTx);
  igmp_stats->stats.igmp_valid_rx                                                    = ENDIAN_SWAP32(stats.igmpValidRx);
  igmp_stats->stats.igmp_invalid_rx                                                  = ENDIAN_SWAP32(stats.igmpInvalidRx);    
  igmp_stats->stats.igmp_dropped_rx                                                  = ENDIAN_SWAP32(stats.igmpDroppedRx); 
  igmp_stats->stats.igmp_total_rx                                                    = ENDIAN_SWAP32(stats.igmpTotalRx);  
  
  igmp_stats->stats.HWIgmpv2Statistics.join_tx                                       = ENDIAN_SWAP32(stats.v2.joinTx);               
  igmp_stats->stats.HWIgmpv2Statistics.join_valid_rx                                 = ENDIAN_SWAP32(stats.v2.joinRx);   
  igmp_stats->stats.HWIgmpv2Statistics.join_invalid_rx                               = ENDIAN_SWAP32(stats.v2.joinInvalidRx);    
  igmp_stats->stats.HWIgmpv2Statistics.leave_tx                                      = ENDIAN_SWAP32(stats.v2.leaveTx);              
  igmp_stats->stats.HWIgmpv2Statistics.leave_valid_rx                                = ENDIAN_SWAP32(stats.v2.leaveRx) +    
                                                                                       ENDIAN_SWAP32(stats.v2.leaveInvalidRx);
  
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_tx                          = ENDIAN_SWAP32(stats.v3.membershipReportTx); 
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_valid_rx                    = ENDIAN_SWAP32(stats.v3.membershipReportRx);      
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_invalid_rx                  = ENDIAN_SWAP32(stats.v3.membershipReportInvalidRx);          
  
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_tx              = ENDIAN_SWAP32(stats.v3.groupRecords.allowTx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_valid_rx        = ENDIAN_SWAP32(stats.v3.groupRecords.allowRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_invalid_rx      = ENDIAN_SWAP32(stats.v3.groupRecords.allowInvalidRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_tx              = ENDIAN_SWAP32(stats.v3.groupRecords.blockTx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_valid_rx        = ENDIAN_SWAP32(stats.v3.groupRecords.blockRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_invalid_rx      = ENDIAN_SWAP32(stats.v3.groupRecords.blockInvalidRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_tx         = ENDIAN_SWAP32(stats.v3.groupRecords.isIncludeTx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_valid_rx   = ENDIAN_SWAP32(stats.v3.groupRecords.isIncludeRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_invalid_rx = ENDIAN_SWAP32(stats.v3.groupRecords.isIncludeInvalidRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_tx         = ENDIAN_SWAP32(stats.v3.groupRecords.isExcludeTx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_valid_rx   = ENDIAN_SWAP32(stats.v3.groupRecords.isExcludeRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_invalid_rx = ENDIAN_SWAP32(stats.v3.groupRecords.isExcludeInvalidRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_tx         = ENDIAN_SWAP32(stats.v3.groupRecords.toIncludeTx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_valid_rx   = ENDIAN_SWAP32(stats.v3.groupRecords.toIncludeRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_invalid_rx = ENDIAN_SWAP32(stats.v3.groupRecords.toIncludeInvalidRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_tx         = ENDIAN_SWAP32(stats.v3.groupRecords.toExcludeTx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_valid_rx   = ENDIAN_SWAP32(stats.v3.groupRecords.toExcludeRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_invalid_rx = ENDIAN_SWAP32(stats.v3.groupRecords.toExcludeInvalidRx);                                  
         
  igmp_stats->stats.HWQueryStatistics.general_query_tx                               = ENDIAN_SWAP32(stats.query.generalQueryTx);     
  igmp_stats->stats.HWQueryStatistics.general_query_valid_rx                         = ENDIAN_SWAP32(stats.query.generalQueryRx);
  igmp_stats->stats.HWQueryStatistics.group_query_tx                                 = ENDIAN_SWAP32(stats.query.groupQueryTx);       
  igmp_stats->stats.HWQueryStatistics.group_query_valid_rx                           = ENDIAN_SWAP32(stats.query.groupQueryRx);  
  igmp_stats->stats.HWQueryStatistics.source_query_tx                                = ENDIAN_SWAP32(stats.query.sourceQueryTx);      
  igmp_stats->stats.HWQueryStatistics.source_query_valid_rx                          = ENDIAN_SWAP32(stats.query.sourceQueryRx); 

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
  L7_uint32 aux_mcEvcId;

  if (igmp_stats==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  aux_mcEvcId = ENDIAN_SWAP32(igmp_stats->mcEvcId);
  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Reading interface IGMP stats:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  MC evc_idx   = %u", aux_mcEvcId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Mask         = 0x%02x", igmp_stats->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Interface    = %u/%u", igmp_stats->intf.intf_type,igmp_stats->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Mask  = 0x%02x", igmp_stats->client.mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.OVlan = %u", ENDIAN_SWAP16(igmp_stats->client.outer_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.IVlan = %u", ENDIAN_SWAP16(igmp_stats->client.inner_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Intf  = %u/%u", igmp_stats->client.intf.intf_type, igmp_stats->client.intf.intf_id);

  /* Evaluate provided data */
  if ( aux_mcEvcId==(L7_uint16)-1 ||
       !(igmp_stats->mask & MSG_CLIENT_MASK) ||
       (igmp_stats->client.mask == 0x00) )
  {
    PT_LOG_ERR(LOG_CTX_MSG, "MC EVC and client reference must be provided");
    return L7_FAILURE;
  }

  memset(&client,0x00,sizeof(ptin_client_id_t));
  if (igmp_stats->client.mask & MSG_CLIENT_OVLAN_MASK)
  {
    client.outerVlan = ENDIAN_SWAP16(igmp_stats->client.outer_vlan);
    client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
  }
  if (igmp_stats->client.mask & MSG_CLIENT_IVLAN_MASK)
  {
    client.innerVlan = ENDIAN_SWAP16(igmp_stats->client.inner_vlan);
    client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
  }
  if (igmp_stats->client.mask & MSG_CLIENT_INTF_MASK)
  {
    client.ptin_intf.intf_type  = igmp_stats->client.intf.intf_type;
    client.ptin_intf.intf_id    = igmp_stats->client.intf.intf_id;
    client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
  }

  /* Clear client stats */
  rc = ptin_igmp_stat_client_clear(aux_mcEvcId,&client);

  if (rc!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error clearing client statistics");
    return rc;
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Success clearing client statistics");
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
  L7_uint32                       aux_mcEvcId;

  if (igmp_stats==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  aux_mcEvcId = ENDIAN_SWAP32(igmp_stats->mcEvcId);
  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Reading interface IGMP stats:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  MC evc_idx   = %u", aux_mcEvcId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Mask         = 0x%02x", igmp_stats->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Interface    = %u/%u", igmp_stats->intf.intf_type,igmp_stats->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Mask  = 0x%02x", igmp_stats->client.mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.OVlan = %u", ENDIAN_SWAP16(igmp_stats->client.outer_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.IVlan = %u", ENDIAN_SWAP16(igmp_stats->client.inner_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Intf  = %u/%u", igmp_stats->client.intf.intf_type, igmp_stats->client.intf.intf_id);

  /* Evaluate provided data */
  if ( !(igmp_stats->mask & MSG_INTERFACE_MASK) )
  {
    PT_LOG_ERR(LOG_CTX_MSG, "At least, interface must be provided");
    return L7_FAILURE;
  }

  ptin_intf.intf_type = igmp_stats->intf.intf_type;
  ptin_intf.intf_id   = igmp_stats->intf.intf_id;

  /* Get statistics */
  if ( aux_mcEvcId ==(L7_uint16)-1)
  {
    rc = ptin_igmp_stat_intf_get(&ptin_intf,&stats);

    if (rc!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error getting global interface statistics");
      return rc;
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Success getting global interface statistics");
    }
  }
  else
  {
    rc = ptin_igmp_stat_instanceIntf_get(aux_mcEvcId, &ptin_intf, &stats);

    if (rc!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error getting statistics struct");
      return rc;
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Success getting interface statistics of one IGMP instance");
    }
  }

  igmp_stats->stats.active_groups                                                    = ENDIAN_SWAP32(stats.activeGroups);            
  igmp_stats->stats.active_clients                                                   = ENDIAN_SWAP32(stats.activeClients); 
            
  igmp_stats->stats.igmp_tx                                                          = ENDIAN_SWAP32(stats.igmpTx);
  igmp_stats->stats.igmp_valid_rx                                                    = ENDIAN_SWAP32(stats.igmpValidRx);
  igmp_stats->stats.igmp_invalid_rx                                                  = ENDIAN_SWAP32(stats.igmpInvalidRx);    
  igmp_stats->stats.igmp_dropped_rx                                                  = ENDIAN_SWAP32(stats.igmpDroppedRx); 
  igmp_stats->stats.igmp_total_rx                                                    = ENDIAN_SWAP32(stats.igmpTotalRx);  
  
  igmp_stats->stats.HWIgmpv2Statistics.join_tx                                       = ENDIAN_SWAP32(stats.v2.joinTx);               
  igmp_stats->stats.HWIgmpv2Statistics.join_valid_rx                                 = ENDIAN_SWAP32(stats.v2.joinRx);   
  igmp_stats->stats.HWIgmpv2Statistics.join_invalid_rx                               = ENDIAN_SWAP32(stats.v2.joinInvalidRx);    
  igmp_stats->stats.HWIgmpv2Statistics.leave_tx                                      = ENDIAN_SWAP32(stats.v2.leaveTx);              
  igmp_stats->stats.HWIgmpv2Statistics.leave_valid_rx                                = ENDIAN_SWAP32(stats.v2.leaveRx) +    
                                                                                       ENDIAN_SWAP32(stats.v2.leaveInvalidRx);
  
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_tx                          = ENDIAN_SWAP32(stats.v3.membershipReportTx); 
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_valid_rx                    = ENDIAN_SWAP32(stats.v3.membershipReportRx);      
  igmp_stats->stats.HWIgmpv3Statistics.membership_report_invalid_rx                  = ENDIAN_SWAP32(stats.v3.membershipReportInvalidRx);          
  
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_tx              = ENDIAN_SWAP32(stats.v3.groupRecords.allowTx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_valid_rx        = ENDIAN_SWAP32(stats.v3.groupRecords.allowRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.allow_invalid_rx      = ENDIAN_SWAP32(stats.v3.groupRecords.allowInvalidRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_tx              = ENDIAN_SWAP32(stats.v3.groupRecords.blockTx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_valid_rx        = ENDIAN_SWAP32(stats.v3.groupRecords.blockRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.block_invalid_rx      = ENDIAN_SWAP32(stats.v3.groupRecords.blockInvalidRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_tx         = ENDIAN_SWAP32(stats.v3.groupRecords.isIncludeTx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_valid_rx   = ENDIAN_SWAP32(stats.v3.groupRecords.isIncludeRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_include_invalid_rx = ENDIAN_SWAP32(stats.v3.groupRecords.isIncludeInvalidRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_tx         = ENDIAN_SWAP32(stats.v3.groupRecords.isExcludeTx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_valid_rx   = ENDIAN_SWAP32(stats.v3.groupRecords.isExcludeRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.is_exclude_invalid_rx = ENDIAN_SWAP32(stats.v3.groupRecords.isExcludeInvalidRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_tx         = ENDIAN_SWAP32(stats.v3.groupRecords.toIncludeTx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_valid_rx   = ENDIAN_SWAP32(stats.v3.groupRecords.toIncludeRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_include_invalid_rx = ENDIAN_SWAP32(stats.v3.groupRecords.toIncludeInvalidRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_tx         = ENDIAN_SWAP32(stats.v3.groupRecords.toExcludeTx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_valid_rx   = ENDIAN_SWAP32(stats.v3.groupRecords.toExcludeRx);
  igmp_stats->stats.HWIgmpv3Statistics.HWGroupRecordStatistics.to_exclude_invalid_rx = ENDIAN_SWAP32(stats.v3.groupRecords.toExcludeInvalidRx);                                  
         
  igmp_stats->stats.HWQueryStatistics.general_query_tx                               = ENDIAN_SWAP32(stats.query.generalQueryTx);     
  igmp_stats->stats.HWQueryStatistics.general_query_valid_rx                         = ENDIAN_SWAP32(stats.query.generalQueryRx);
  igmp_stats->stats.HWQueryStatistics.group_query_tx                                 = ENDIAN_SWAP32(stats.query.groupQueryTx);       
  igmp_stats->stats.HWQueryStatistics.group_query_valid_rx                           = ENDIAN_SWAP32(stats.query.groupQueryRx);  
  igmp_stats->stats.HWQueryStatistics.source_query_tx                                = ENDIAN_SWAP32(stats.query.sourceQueryTx);      
  igmp_stats->stats.HWQueryStatistics.source_query_valid_rx                          = ENDIAN_SWAP32(stats.query.sourceQueryRx); 

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
  L7_uint32 aux_mcEvcId;

  if (igmp_stats==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  aux_mcEvcId = ENDIAN_SWAP32(igmp_stats->mcEvcId);

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Clearing interface IGMP stats:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  MC evc_idx   = %u", aux_mcEvcId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Mask         = 0x%02x", igmp_stats->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Interface    = %u/%u", igmp_stats->intf.intf_type,igmp_stats->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Mask  = 0x%02x", igmp_stats->client.mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.OVlan = %u", ENDIAN_SWAP16(igmp_stats->client.outer_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.IVlan = %u", ENDIAN_SWAP16(igmp_stats->client.inner_vlan));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Client.Intf  = %u/%u", igmp_stats->client.intf.intf_type, igmp_stats->client.intf.intf_id);

  ptin_intf.intf_type = igmp_stats->intf.intf_type;
  ptin_intf.intf_id   = igmp_stats->intf.intf_id;

  /* MC EVC not provided */
  if (aux_mcEvcId ==(L7_uint16)-1)
  {
    /* Interface not provided */
    if ( !(igmp_stats->mask & MSG_INTERFACE_MASK) )
    {
      /* Clear all stats */
      rc = ptin_igmp_stat_clearAll();

      if (rc!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error clearing all statistics data");
        return rc;
      }
      else
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "Success clearing all statistics data");
      }
    }
    /* Interface provided */
    else
    {
      /* Clear all stats of one interface */
      rc = ptin_igmp_stat_intf_clear(&ptin_intf);

      if (rc!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error clearing statistics of one complete interface");
        return rc;
      }
      else
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "Success clearing statistics of one complete interface");
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
      rc = ptin_igmp_stat_instance_clear(aux_mcEvcId);

      if (rc!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error clearing statistics of one IGMP instance");
        return rc;
      }
      else
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "Success clearing statistics of one IGMP instance");
      }
    }
    /* Interface provided */
    else
    {
      /* Clear stats of one igmp instance and one interface */
      rc = ptin_igmp_stat_instanceIntf_clear(aux_mcEvcId, &ptin_intf);

      if (rc!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error clearing statistics of one IGMP instance + interface");
        return rc;
      }
      else
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "Success clearing statistics of one IGMP instance + interface");
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
static igmpAssoc_entry_t igmpAssoc_list[PTIN_IGMP_CHANNELS_MAX];
static L7_uint16 igmpAssoc_channels_max = 0;
#endif

L7_RC_t ptin_msg_IGMP_ChannelAssoc_get(msg_MCAssocChannel_t *channel_list, L7_uint16 *n_channels)
{
  if (channel_list==L7_NULLPTR || n_channels==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  //CHMSG_IP_ADDR_SWAP_COPY(channel_list->channel_dstIp,channel_list->channel_dstIp);
  //CHMSG_IP_ADDR_SWAP_COPY(channel_list->channel_srcIp,channel_list->channel_srcIp);

  ENDIAN_SWAP64_MOD(channel_list->channelBandwidth);
  ENDIAN_SWAP32_MOD(channel_list->evcid_mc);
  ENDIAN_SWAP16_MOD(channel_list->entry_idx);

  PT_LOG_DEBUG(LOG_CTX_MSG,"Reading White list channel list:");
  PT_LOG_DEBUG(LOG_CTX_MSG," Slot   = %d",channel_list->SlotId);
  PT_LOG_DEBUG(LOG_CTX_MSG," EVC_MC = %d",channel_list->evcid_mc);
  PT_LOG_DEBUG(LOG_CTX_MSG," Entry_idx = %d",channel_list->entry_idx);
  PT_LOG_DEBUG(LOG_CTX_MSG," DstIP_Channel = 0x%08x (ipv6=%u) / %u",channel_list->channel_dstIp.addr.ipv4, channel_list->channel_dstIp.family, channel_list->channel_dstmask);
  PT_LOG_DEBUG(LOG_CTX_MSG," SrcIP_Channel = 0x%08x (ipv6=%u) / %u",channel_list->channel_srcIp.addr.ipv4, channel_list->channel_srcIp.family, channel_list->channel_srcmask);

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

    number_of_channels = PTIN_IGMP_CHANNELS_MAX;
    if (ptin_igmp_channel_list_get(0, channel_list->evcid_mc, igmpAssoc_list, &number_of_channels)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error reading list of channels");
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


   CHMSG_IP_ADDR_SWAP_COPY(channel_list[i].channel_dstIp,channel_list[i].channel_dstIp);
   CHMSG_IP_ADDR_SWAP_COPY(channel_list[i].channel_srcIp,channel_list[i].channel_srcIp);

   ENDIAN_SWAP64_MOD(channel_list[i].channelBandwidth);
   ENDIAN_SWAP32_MOD(channel_list[i].evcid_mc);
   ENDIAN_SWAP16_MOD(channel_list[i].entry_idx);
  }

  /* Return number of channels */
  *n_channels = number_of_channels;

#else

  PT_LOG_DEBUG(LOG_CTX_MSG,"Not supported!");
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
L7_RC_t ptin_msg_group_list_add(msg_MCAssocChannel_t *channel_list, L7_uint16 n_channels, L7_uint8 isStatic)
{
  L7_uint16      i;
  L7_inet_addr_t groupAddr;
  L7_inet_addr_t sourceAddr;
  char           groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char           sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
  L7_RC_t        rc = L7_SUCCESS;
  L7_RC_t        rc_global = L7_SUCCESS;

  if (channel_list==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  for (i=0; i<n_channels; i++)
  {
    CHMSG_IP_ADDR_SWAP_COPY(channel_list[i].channel_dstIp,channel_list[i].channel_dstIp);
    CHMSG_IP_ADDR_SWAP_COPY(channel_list[i].channel_srcIp,channel_list[i].channel_srcIp);

    ENDIAN_SWAP64_MOD(channel_list[i].channelBandwidth);
    ENDIAN_SWAP32_MOD(channel_list[i].evcid_mc);
    ENDIAN_SWAP16_MOD(channel_list[i].entry_idx);

    PT_LOG_DEBUG(LOG_CTX_MSG,"Adding channel index %u:",i);
    PT_LOG_DEBUG(LOG_CTX_MSG," Slot   = %d",channel_list[i].SlotId);
    PT_LOG_DEBUG(LOG_CTX_MSG," EVC_MC = %d",channel_list[i].evcid_mc);
    PT_LOG_DEBUG(LOG_CTX_MSG," Entry_idx = %d",channel_list[i].entry_idx);
    PT_LOG_DEBUG(LOG_CTX_MSG," group_Channel = 0x%08x (ipv6=%u) / %u",channel_list[i].channel_dstIp.addr.ipv4, channel_list[i].channel_dstIp.family, channel_list[i].channel_dstmask);
    PT_LOG_DEBUG(LOG_CTX_MSG," source_Channel = 0x%08x (ipv6=%u) / %u",channel_list[i].channel_srcIp.addr.ipv4, channel_list[i].channel_srcIp.family, channel_list[i].channel_srcmask);

    #if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
    PT_LOG_DEBUG(LOG_CTX_MSG," channelBandwidth = %llu bits/s", channel_list[i].channelBandwidth);

    if (channel_list[i].channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_BPS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Invalid channelBandwidth:%llu bits/s",channel_list[i].channelBandwidth);
      return L7_FAILURE;
    }
    #endif
    /* Prepare group address */    
    rc = ptin_to_fp_ip_notation(&channel_list[i].channel_dstIp, &groupAddr);
    if ( rc != L7_SUCCESS)
    {
      return rc;
    }
   
    /* Prepare source address */
    rc = ptin_to_fp_ip_notation(&channel_list[i].channel_srcIp, &sourceAddr);
    if ( rc != L7_SUCCESS)
    {
      return rc;
    }

    inetAddrPrint(&groupAddr, groupAddrStr);
    inetAddrPrint(&sourceAddr, sourceAddrStr);

    /*Validate Group Address & Group Mask */
    /*Validate Source Address & Source Mask*/
    if (inetIsAddressZero(&groupAddr) == L7_FALSE) 
    {
      if (inetIsInMulticast(&groupAddr)== L7_FALSE || channel_list[i].channel_dstmask < PTIN_IGMP_GROUP_MASK_MIN || channel_list[i].channel_dstmask > 32 || (!inetIsAddressZero(&sourceAddr) && inetIsValidHostAddress(&sourceAddr)== L7_FALSE) )            
      {      
        PT_LOG_ERR(LOG_CTX_MSG," Invalid Parameters: evcid_mc:%u groupAddr=%s/%u sourceAddr:%s/%u ", channel_list[i].evcid_mc, groupAddrStr, channel_list[i].channel_dstmask, sourceAddrStr, channel_list[i].channel_srcmask);
        return L7_FAILURE;                           
      }

      if ( channel_list[i].channel_srcmask != 32 )
      {
//      PT_LOG_WARN(LOG_CTX_MSG," Invalid Parameters: evcid_mc:%u groupAddr=%s/%u sourceAddr:%s/%u ", channel_list[i].evcid_mc, groupAddrStr, channel_list[i].channel_dstmask, sourceAddrStr, channel_list[i].channel_srcmask);
        channel_list[i].channel_srcmask = 32;          
      }
    }
    else
    { /*Default Multicast Entry*/
      if (!inetIsAddressZero(&sourceAddr))
      {
        PT_LOG_ERR(LOG_CTX_MSG," Invalid Parameters: evcid_mc:%u groupAddr=%s/%u sourceAddr:%s/%u ", channel_list[i].evcid_mc, groupAddrStr, channel_list[i].channel_dstmask, sourceAddrStr, channel_list[i].channel_srcmask);
        return L7_FAILURE;                   
      }

      if ( channel_list[i].channel_dstmask != 32 )
      {
        PT_LOG_WARN(LOG_CTX_MSG," Invalid Parameters: evcid_mc:%u groupAddr=%s/%u sourceAddr:%s/%u ", channel_list[i].evcid_mc, groupAddrStr, channel_list[i].channel_dstmask, sourceAddrStr, channel_list[i].channel_srcmask);
        channel_list[i].channel_dstmask = 32;
      }

      if ( channel_list[i].channel_srcmask != 0 )
      {
        PT_LOG_WARN(LOG_CTX_MSG," Invalid Parameters: evcid_mc:%u groupAddr=%s/%u sourceAddr:%s/%u ", channel_list[i].evcid_mc, groupAddrStr, channel_list[i].channel_dstmask, sourceAddrStr, channel_list[i].channel_srcmask);
        channel_list[i].channel_srcmask = 0;
      }       
    }

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED

    if ((rc=igmp_assoc_channel_add( 0, channel_list[i].evcid_mc,
                                    &groupAddr , channel_list[i].channel_dstmask,
                                    &sourceAddr, channel_list[i].channel_srcmask, isStatic, channel_list[i].channelBandwidth )) != L7_SUCCESS)
    {
      if ( L7_REQUEST_DENIED == rc)
      {
        rc_global = rc;
        rc = L7_SUCCESS;
      }
      else
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error adding group address 0x%08x/%u, source address 0x%08x/%u to MC EVC %u",
                channel_list[i].channel_dstIp.addr.ipv4, channel_list[i].channel_dstmask,
                channel_list[i].channel_srcIp.addr.ipv4, channel_list[i].channel_srcmask,
                channel_list[i].evcid_mc);
        return rc;
      }
      
    }

#else
    rc = L7_NOT_SUPPORTED;
    PT_LOG_DEBUG(LOG_CTX_MSG,"Not supported!");
    break;
#endif
  }

  if (rc_global != L7_SUCCESS)
  {    
    if (rc != L7_SUCCESS)
      rc_global = rc;
    PT_LOG_WARN(LOG_CTX_MSG, "One or more channels were already added! rc:%u", rc_global);
  }  

  return rc_global;
}

/**
 * Remove channels to white list
 * 
 * @param channel_list : Channel list array
 * @param n_channels : Number of channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_group_list_remove(msg_MCAssocChannel_t *channel_list, L7_uint16 n_channels, L7_uint8 isStatic)
{
  L7_uint16      i;
  L7_inet_addr_t groupAddr; 
  L7_inet_addr_t sourceAddr;
  char           groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char           sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
  L7_RC_t        rc = L7_SUCCESS;
  L7_RC_t        rc_global = L7_SUCCESS;

  if (channel_list==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  for (i=0; i<n_channels; i++)
  {
    CHMSG_IP_ADDR_SWAP_COPY(channel_list[i].channel_dstIp,channel_list[i].channel_dstIp);
    CHMSG_IP_ADDR_SWAP_COPY(channel_list[i].channel_srcIp,channel_list[i].channel_srcIp);

    ENDIAN_SWAP64_MOD(channel_list[i].channelBandwidth);
    ENDIAN_SWAP32_MOD(channel_list[i].evcid_mc);
    ENDIAN_SWAP16_MOD(channel_list[i].entry_idx);

    PT_LOG_DEBUG(LOG_CTX_MSG,"Removing channel index %u:",i);
    PT_LOG_DEBUG(LOG_CTX_MSG," Slot   = %d",channel_list[i].SlotId);
    PT_LOG_DEBUG(LOG_CTX_MSG," EVC_MC = %d",channel_list[i].evcid_mc);
    PT_LOG_DEBUG(LOG_CTX_MSG," Entry_idx = %d",channel_list[i].entry_idx);
    PT_LOG_DEBUG(LOG_CTX_MSG," isStatic = %s", isStatic?"Yes":"No");
    PT_LOG_DEBUG(LOG_CTX_MSG," DstIP_Channel = 0x%08x (ipv6=%u) / %u",channel_list[i].channel_dstIp.addr.ipv4, channel_list[i].channel_dstIp.family, channel_list[i].channel_dstmask);
    PT_LOG_DEBUG(LOG_CTX_MSG," SrcIP_Channel = 0x%08x (ipv6=%u) / %u",channel_list[i].channel_srcIp.addr.ipv4, channel_list[i].channel_srcIp.family, channel_list[i].channel_srcmask);

   
    /* Prepare group address */    
    rc = ptin_to_fp_ip_notation(&channel_list[i].channel_dstIp, &groupAddr);
    if ( rc != L7_SUCCESS)
    {
      return rc;
    }
    /* Prepare source address */     
    rc = ptin_to_fp_ip_notation(&channel_list[i].channel_srcIp, &sourceAddr);
    if ( rc != L7_SUCCESS)
    {
      return rc;
    }

    inetAddrPrint(&groupAddr, groupAddrStr);
    inetAddrPrint(&sourceAddr, sourceAddrStr);

    /*Validate Group Address & Group Mask */
    /*Validate Source Address & Source Mask*/
    if (inetIsAddressZero(&groupAddr) == L7_FALSE) 
    {
      if (inetIsInMulticast(&groupAddr)== L7_FALSE || channel_list[i].channel_dstmask < PTIN_IGMP_GROUP_MASK_MIN || channel_list[i].channel_dstmask > 32 || (!inetIsAddressZero(&sourceAddr) && inetIsValidHostAddress(&sourceAddr)== L7_FALSE) )            
      {      
        PT_LOG_ERR(LOG_CTX_MSG," Invalid Parameters: evcid_mc:%u groupAddr=%s/%u sourceAddr:%s/%u ", channel_list[i].evcid_mc, groupAddrStr, channel_list[i].channel_dstmask, sourceAddrStr, channel_list[i].channel_srcmask);
        return L7_FAILURE;                           
      }

      if ( channel_list[i].channel_srcmask != 32 )
      {
//      PT_LOG_WARN(LOG_CTX_MSG," Invalid Parameters: evcid_mc:%u groupAddr=%s/%u sourceAddr:%s/%u ", channel_list[i].evcid_mc, groupAddrStr, channel_list[i].channel_dstmask, sourceAddrStr, channel_list[i].channel_srcmask);
        channel_list[i].channel_srcmask = 32;          
      }
    }
    else
    { /*Default Multicast Entry*/
      if (!inetIsAddressZero(&sourceAddr))
      {
        PT_LOG_ERR(LOG_CTX_MSG," Invalid Parameters: evcid_mc:%u groupAddr=%s/%u sourceAddr:%s/%u ", channel_list[i].evcid_mc, groupAddrStr, channel_list[i].channel_dstmask, sourceAddrStr, channel_list[i].channel_srcmask);
        return L7_FAILURE;                   
      }

      if ( channel_list[i].channel_dstmask != 32 )
      {
        PT_LOG_WARN(LOG_CTX_MSG," Invalid Parameters: evcid_mc:%u groupAddr=%s/%u sourceAddr:%s/%u ", channel_list[i].evcid_mc, groupAddrStr, channel_list[i].channel_dstmask, sourceAddrStr, channel_list[i].channel_srcmask);
        channel_list[i].channel_dstmask = 32;
      }

      if ( channel_list[i].channel_srcmask != 0 )
      {
        PT_LOG_WARN(LOG_CTX_MSG," Invalid Parameters: evcid_mc:%u groupAddr=%s/%u sourceAddr:%s/%u ", channel_list[i].evcid_mc, groupAddrStr, channel_list[i].channel_dstmask, sourceAddrStr, channel_list[i].channel_srcmask);
        channel_list[i].channel_srcmask = 0;
      }       
    }


#ifdef IGMPASSOC_MULTI_MC_SUPPORTED

    if ((rc = igmp_assoc_channel_remove( channel_list[i].evcid_mc, 0, 
                                   &groupAddr , channel_list[i].channel_dstmask,
                                   &sourceAddr, channel_list[i].channel_srcmask, isStatic )) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error (%d) removing group address 0x%08x/%u, source address 0x%08x/%u from the MC EVC %u",
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
    PT_LOG_DEBUG(LOG_CTX_MSG,"Not supported!");
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
L7_RC_t ptin_msg_IGMP_ChannelAssoc_remove_all(msg_MCAssocChannel_t *channel_list, L7_uint16 noOfMessages)
{
  L7_uint16 messageIterator;
  L7_RC_t rc = L7_SUCCESS;
  L7_RC_t rc_global = L7_SUCCESS;

  if (channel_list==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  for (messageIterator=0; messageIterator<noOfMessages; messageIterator++)
  {

    ENDIAN_SWAP32_MOD(channel_list[messageIterator].evcid_mc);

    PT_LOG_DEBUG(LOG_CTX_MSG,"Removing channel index %u:",messageIterator);
    PT_LOG_DEBUG(LOG_CTX_MSG," Slot   = %d",channel_list[messageIterator].SlotId);
    PT_LOG_DEBUG(LOG_CTX_MSG," EVC_MC = %d",channel_list[messageIterator].evcid_mc);

#ifdef IGMPASSOC_MULTI_MC_SUPPORTED

    if ((rc = igmp_assoc_channel_clear(-1, channel_list[messageIterator].evcid_mc)) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error (%d) removing groups to MC EVC %u", rc, channel_list[messageIterator].evcid_mc);
      rc_global = rc;
      continue;
    }

#else
    rc = L7_NOT_SUPPORTED;
    rc_global = L7_NOT_SUPPORTED;
    PT_LOG_DEBUG(LOG_CTX_MSG,"Not supported!");
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
L7_RC_t ptin_msg_static_channel_add(msg_MCStaticChannel_t *channel, L7_uint16 n_channels)
{
  PTIN_MGMD_CTRL_STATICGROUP_t staticGroup={0};  
  L7_uint16                    i;
  L7_RC_t                      rc = L7_SUCCESS;

  if (channel==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }    

  /*Pre-Validate that all services are already created*/
  for (i=0; i<n_channels; i++)
  {
    if( SUCCESS != ptin_evc_intRootVlan_get(ENDIAN_SWAP32(channel[i].evc_id), L7_NULLPTR))
    {
      PT_LOG_ERR(LOG_CTX_IGMP,"Unable to get mcastRootVlan from serviceId:%u", ENDIAN_SWAP32(channel[i].evc_id));    
      return L7_DEPENDENCY_NOT_MET;
    } 
  }

  for (i=0; i<n_channels; i++)
  {

    ENDIAN_SWAP32_MOD(channel[i].evc_id);
    ENDIAN_SWAP32_MOD(channel[i].channelIp.s_addr);
    ENDIAN_SWAP32_MOD(channel[i].sourceIp.s_addr);
    ENDIAN_SWAP64_MOD(channel[i].channelBandwidth);


    #ifdef IGMPASSOC_MULTI_MC_SUPPORTED//Add Static Channel to (WhiteList) Group List    
    msg_MCAssocChannel_t         channel_list;                                        
                                       
    memset(&channel_list, 0x00, sizeof(channel_list));

    channel_list.SlotId=channel[i].SlotId;
    channel_list.evcid_mc=channel[i].evc_id;

    channel_list.channel_dstIp.family = PTIN_AF_INET;  
    channel_list.channel_dstIp.addr.ipv4 = channel[i].channelIp.s_addr;
    channel_list.channel_dstmask=32;//32 Bits of Mask

    channel_list.channel_srcIp.family=PTIN_AF_INET;
    channel_list.channel_srcIp.addr.ipv4 = channel[i].sourceIp.s_addr;    
    channel_list.channel_srcmask=32;

    channel_list.channelBandwidth = channel[i].channelBandwidth;

    if ((rc =  ptin_msg_group_list_add(&channel_list,1, L7_TRUE)) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error (%d) adding static channel", rc);
      return rc;
    }
    #endif//End Static Channel Add

    PT_LOG_DEBUG(LOG_CTX_MSG,"Static channel addition index %u:",i);
    PT_LOG_DEBUG(LOG_CTX_MSG," SlotId           = %u",channel[i].SlotId);
    PT_LOG_DEBUG(LOG_CTX_MSG," EvcId            = %u",channel[i].evc_id);
    PT_LOG_DEBUG(LOG_CTX_MSG," Channel          = %u.%u.%u.%u",
              (channel[i].channelIp.s_addr>>24) & 0xff,(channel[i].channelIp.s_addr>>16) & 0xff,(channel[i].channelIp.s_addr>>8) & 0xff, channel[i].channelIp.s_addr & 0xff);
    PT_LOG_DEBUG(LOG_CTX_MSG," SourceIP         = %u.%u.%u.%u",
              (channel[i].sourceIp.s_addr>>24) & 0xff,(channel[i].sourceIp.s_addr>>16) & 0xff,(channel[i].sourceIp.s_addr>>8) & 0xff, channel[i].sourceIp.s_addr & 0xff);

    #if PTIN_SYSTEM_IGMP_ADMISSION_CONTROL_SUPPORT
    PT_LOG_DEBUG(LOG_CTX_MSG," channelBandwidth = %llu", channel_list.channelBandwidth);
                               
    if (channel_list.channelBandwidth > PTIN_IGMP_ADMISSION_CONTROL_MAX_BANDWIDTH_IN_BPS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Invalid channelBandwidth = %llu", channel_list.channelBandwidth);
      return L7_FAILURE;
    }
    #endif

    staticGroup.serviceId = channel[i].evc_id;
    staticGroup.groupIp   = channel[i].channelIp.s_addr;
    staticGroup.sourceIp  = channel[i].sourceIp.s_addr;
    staticGroup.portType  = PTIN_MGMD_PORT_TYPE_LEAF;

    if ((rc = ptin_igmp_static_channel_add(&staticGroup)) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error (%d) adding static channel", rc);
      return rc;
    }
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
L7_RC_t ptin_msg_static_channel_remove(msg_MCStaticChannel_t *channel, L7_uint16 n_channels)
{
  PTIN_MGMD_CTRL_STATICGROUP_t staticGroup={0};  
  L7_uint16                    i;
  L7_RC_t                      rc;
  L7_RC_t                      rc_global = L7_SUCCESS;

  if (channel==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  /*Pre-Validate that all services are already created*/
  for (i=0; i<n_channels; i++)
  {
    if( SUCCESS != ptin_evc_intRootVlan_get(ENDIAN_SWAP32(channel[i].evc_id), L7_NULLPTR))
    {
      PT_LOG_ERR(LOG_CTX_IGMP,"Unable to get mcastRootVlan from serviceId:%u", ENDIAN_SWAP32(channel[i].evc_id));    
      return L7_DEPENDENCY_NOT_MET;
    } 
  }

  for (i=0; i<n_channels; i++)
  {
    staticGroup.serviceId        = ENDIAN_SWAP32(channel[i].evc_id);
    staticGroup.groupIp          = ENDIAN_SWAP32(channel[i].channelIp.s_addr);
    staticGroup.sourceIp         = ENDIAN_SWAP32(channel[i].sourceIp.s_addr);
    staticGroup.portType         = PTIN_MGMD_PORT_TYPE_LEAF;
    ENDIAN_SWAP64_MOD(channel[i].channelBandwidth);

    PT_LOG_DEBUG(LOG_CTX_MSG,"Channel remotion index %u:",i);
    PT_LOG_DEBUG(LOG_CTX_MSG," SlotId =%u",channel[i].SlotId);
    PT_LOG_DEBUG(LOG_CTX_MSG," EvcId  =%u",ENDIAN_SWAP32(channel[i].evc_id));
    PT_LOG_DEBUG(LOG_CTX_MSG," Channel=%u.%u.%u.%u",
              (staticGroup.groupIp>>24) & 0xff, (staticGroup.groupIp>>16) & 0xff, (staticGroup.groupIp>>8) & 0xff, (staticGroup.groupIp) & 0xff);
    PT_LOG_DEBUG(LOG_CTX_MSG," SourceIP=%u.%u.%u.%u",
              (staticGroup.sourceIp>>24) & 0xff,(staticGroup.sourceIp>>16) & 0xff,(staticGroup.sourceIp>>8) & 0xff,staticGroup.sourceIp & 0xff);


    if ((rc = ptin_igmp_static_channel_remove(&staticGroup)) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error (%d) removing channel", rc);
      rc_global = rc;
      continue;
    }

    #ifdef IGMPASSOC_MULTI_MC_SUPPORTED//Remove Static Channel from (WhiteList) Group List     
    msg_MCAssocChannel_t         channel_list;  

    memset(&channel_list, 0x00, sizeof(channel_list));

    channel_list.SlotId=channel[i].SlotId;
    channel_list.evcid_mc=staticGroup.serviceId;

    channel_list.channel_dstIp.family=PTIN_AF_INET;
    channel_list.channel_dstIp.addr.ipv4=staticGroup.groupIp;
    channel_list.channel_dstmask=32;//32 Bits of Mask

    channel_list.channel_srcIp.family=PTIN_AF_INET;
    channel_list.channel_srcIp.addr.ipv4 = staticGroup.sourceIp;
    channel_list.channel_srcmask=32;

    channel_list.channelBandwidth = channel[i].channelBandwidth;
    
    ptin_msg_group_list_remove(&channel_list,1, L7_TRUE);   
    #endif//End Static Channel Remove
    
  }

  return rc_global;
}

static ptin_igmpChannelInfo_t clist[IPCLIB_MAX_MSGSIZE/sizeof(msg_MCActiveChannelsReply_t)]; 
/**
 * Consult list of multicast channels
 * 
 * @param channel_list : list of multicast channels
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note   TODO: This function is invoqued several times by the
 *         Manager if the buffer with the upper layers (e.g. WebTi,CLI, Agora-NG) is excedeed. In order to
 *         optimise this behaviour a new parameter should be
 *         added: maxChannels
 */
L7_RC_t ptin_msg_IGMP_channelList_get(msg_MCActiveChannelsRequest_t *inputPtr, msg_MCActiveChannelsReply_t *outputPtr, L7_uint16 *numberOfChannels)
{  
  L7_uint16              i, number_of_channels, total_channels;
  ptin_client_id_t       client;
  L7_RC_t                rc;

  if (numberOfChannels == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid parameters");
    return L7_FAILURE;
  }

  ENDIAN_SWAP32_MOD(inputPtr->evc_id);
  ENDIAN_SWAP16_MOD(inputPtr->client.outer_vlan);
  ENDIAN_SWAP16_MOD(inputPtr->client.inner_vlan);
  ENDIAN_SWAP16_MOD(inputPtr->entryId);

  PT_LOG_DEBUG(LOG_CTX_MSG,"Going to retrieve list of channels");
  PT_LOG_DEBUG(LOG_CTX_MSG," slotId = %u",             inputPtr->SlotId);
  PT_LOG_DEBUG(LOG_CTX_MSG," EvcId  = %u",             inputPtr->evc_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," Client.Mask  = 0x%02x",   inputPtr->client.mask);
  PT_LOG_DEBUG(LOG_CTX_MSG," Client.OVlan = %u",       inputPtr->client.outer_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," Client.IVlan = %u",       inputPtr->client.inner_vlan);
  PT_LOG_DEBUG(LOG_CTX_MSG," Client.Intf  = %u/%u",    inputPtr->client.intf.intf_type,inputPtr->client.intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG," Entry_idx=%u",            inputPtr->entryId);
  PT_LOG_DEBUG(LOG_CTX_MSG," Max Number of Channels=%u",*numberOfChannels);

  //Short Fix to Support Mac Bridge Services and Unicast Services
  #if (PTIN_BOARD_IS_LINECARD || PTIN_BOARD_IS_STANDALONE)
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
    if (inputPtr->client.mask != 0)
    {
      inputPtr->client.mask|=MSG_CLIENT_OVLAN_MASK;
    }    
    PT_LOG_DEBUG(LOG_CTX_MSG,"Converted [client.Mask:%u Client.OVlan:%u]",inputPtr->client.mask,inputPtr->client.outer_vlan);
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
      outputPtr[i].entryId = i + ENDIAN_SWAP16(inputPtr->entryId);      
      outputPtr[i].chIP    = ENDIAN_SWAP32(clist[i].groupAddr.addr.ipv4.s_addr);
      outputPtr[i].srcIP   = ENDIAN_SWAP32(clist[i].sourceAddr.addr.ipv4.s_addr);
      outputPtr[i].chType  = clist[i].static_type;
      PT_LOG_TRACE(LOG_CTX_MSG,"EntryId[%u] -> Group:[%08X] Source[%08X] isStatic[%s]", ENDIAN_SWAP16(outputPtr[i].entryId), ENDIAN_SWAP32(outputPtr[i].chIP), 
                                                                                                              ENDIAN_SWAP32(outputPtr[i].srcIP), outputPtr[i].chType?"Yes":"No");
    }
     *numberOfChannels = i;
     PT_LOG_DEBUG(LOG_CTX_MSG, "Read %u channels and retrieving %u channels.",number_of_channels, *numberOfChannels);
  }
  else if (rc==L7_NOT_EXIST)
  {
    *numberOfChannels = 0;
    PT_LOG_NOTICE(LOG_CTX_MSG, "No channels to retrieve");
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error with ptin_igmp_channelList_get");
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
#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)    
  L7_uint16      mcastRootVlan = 0;
#if PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD
  L7_inet_addr_t groupAddr;
  L7_inet_addr_t sourceAddr;
  char           groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char           sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
#endif
  L7_RC_t        rc;

  if (snoopSyncRequest==L7_NULLPTR )
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid input parameters snoopSyncRequest=%p",snoopSyncRequest);
    return L7_FAILURE;
  }

#if PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD    
  ptin_to_fp_ip_notation(&snoopSyncRequest->groupAddr, &groupAddr);
  ptin_to_fp_ip_notation(&snoopSyncRequest->sourceAddr, &sourceAddr);

  inetAddrPrint(&groupAddr, groupAddrStr);
  inetAddrPrint(&sourceAddr, sourceAddrStr);
#endif

  PT_LOG_DEBUG(LOG_CTX_MSG,"Received Snoop Sync Request Message");     
  PT_LOG_DEBUG(LOG_CTX_MSG," serviceId=%u",snoopSyncRequest->serviceId);
#if PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD
  PT_LOG_DEBUG(LOG_CTX_MSG," groupAddr=%s", groupAddrStr);
  PT_LOG_DEBUG(LOG_CTX_MSG," sourceAddr=%s", sourceAddrStr);
#else
  PT_LOG_DEBUG(LOG_CTX_MSG," groupAddr=%08X", snoopSyncRequest->groupAddr);
#endif
#if !PTIN_BOARD_IS_MATRIX  
  PT_LOG_DEBUG(LOG_CTX_MSG," portId=%u", snoopSyncRequest->portId);
#endif
  
  if( snoopSyncRequest->serviceId != 0 
      && (L7_SUCCESS != (rc=ptin_evc_intRootVlan_get(snoopSyncRequest->serviceId, &mcastRootVlan))) )
  {
    if( rc != L7_NOT_EXIST)
    {
      PT_LOG_ERR(LOG_CTX_IGMP, "Unable to get mcastRootVlan from serviceId:%u",snoopSyncRequest->serviceId);
      return rc;
    }
#if PTIN_BOARD_IS_MATRIX 
  #if PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD
    PT_LOG_NOTICE(LOG_CTX_IGMP, "Evc Id is not yet created. Silently Ignoring Snoop Sync Request! [serviceId:%u groupAddr:%08X sourceAddr:%08X]", snoopSyncRequest->serviceId, snoopSyncRequest->groupAddr.addr.ipv4, snoopSyncRequest->sourceAddr.addr.ipv4);
  #else
    PT_LOG_NOTICE(LOG_CTX_IGMP, "Evc Id is not yet created. Silently Ignoring Snoop Sync Request! [serviceId:%u groupAddr:%08X]", snoopSyncRequest->serviceId, snoopSyncRequest->groupAddr);
  #endif
#else
  #if PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD
    PT_LOG_NOTICE(LOG_CTX_IGMP, "Evc Id is not yet created. Silently Ignoring Snoop Sync Request! [serviceId:%u portId:%u groupAddr:%08X sourceAddr:%08X]", snoopSyncRequest->serviceId, snoopSyncRequest->portId, snoopSyncRequest->groupAddr.addr.ipv4, snoopSyncRequest->sourceAddr.addr.ipv4);
  #else
    PT_LOG_NOTICE(LOG_CTX_IGMP, "Evc Id is not yet created. Silently Ignoring Snoop Sync Request! [serviceId:%u portId:%u groupAddr:%08X]", snoopSyncRequest->serviceId, snoopSyncRequest->portId, snoopSyncRequest->groupAddr);
  #endif
#endif
    return rc;
  }
   
#if PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD
  #if PTIN_BOARD_IS_MATRIX    
    return (ptin_snoop_l3_sync_mx_process_request(mcastRootVlan, &groupAddr, &sourceAddr));            
  #else    
    return (ptin_snoop_l3_sync_port_process_request(mcastRootVlan, &groupAddr, &sourceAddr, snoopSyncRequest->portId));
  #endif                
#else
  #if PTIN_BOARD_IS_MATRIX    
    return (ptin_snoop_sync_mx_process_request(mcastRootVlan, snoopSyncRequest->groupAddr));
  #else    
    return (ptin_snoop_sync_port_process_request(mcastRootVlan, snoopSyncRequest->groupAddr, snoopSyncRequest->portId));
  #endif                
#endif

#else
    PT_LOG_NOTICE(LOG_CTX_IGMP, "Silently Ignoring Snoop Sync Request. I'm a standalone!");
    return L7_SUCCESS;
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
  L7_uint32      maxNumberOfSnoopEntries  =  IPCLIB_MAX_MSGSIZE/sizeof(msg_SnoopSyncReply_t); //IPC buffer size / struct size 
  L7_uint32      iterator; 
#if PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD
  L7_BOOL        isProtection = L7_TRUE;
  L7_inet_addr_t groupAddr;
  L7_inet_addr_t sourceAddr;
  char           groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char           sourceAddrStr[IPV6_DISP_ADDR_LEN]={};
#else
  L7_uint32      sourceAddr = 0x0;
#endif
  
  if (snoopSyncReply==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Invalid parameters: snoopSyncReply=%p",snoopSyncReply);
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG,"Received Snoop Sync Reply Message: numberOfSnoopEntries=%u | maxNumberOfSnoopEntries:%u",numberOfSnoopEntries,maxNumberOfSnoopEntries);
  
  if(numberOfSnoopEntries==0)
  {
    PT_LOG_NOTICE(LOG_CTX_MSG,"The number of snoop entries is equal to zero. Silently ignoring this reply message.");
    return L7_SUCCESS;
  }
    
  #if PTIN_BOARD_IS_MATRIX
  {
    L7_uint32 intIfNum;
    L7_uint32 numberOfActivePorts;

    for(iterator=0;iterator < numberOfSnoopEntries; iterator++)
    {
      PT_LOG_TRACE(LOG_CTX_MSG," serviceId=%u", snoopSyncReply[iterator].serviceId);
#if PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD
      ptin_to_fp_ip_notation(&snoopSyncReply[iterator].groupAddr, &groupAddr);
      ptin_to_fp_ip_notation(&snoopSyncReply[iterator].sourceAddr, &sourceAddr);

      inetAddrPrint(&groupAddr, groupAddrStr);
      inetAddrPrint(&sourceAddr, sourceAddrStr);
      PT_LOG_TRACE(LOG_CTX_MSG," groupAddr=%s", groupAddrStr);
      PT_LOG_TRACE(LOG_CTX_MSG," sourceAddr=%s", sourceAddrStr);
#else
      PT_LOG_TRACE(LOG_CTX_MSG," groupAddr=%08X", snoopSyncReply[iterator].groupAddr);
#endif
      PT_LOG_TRACE(LOG_CTX_MSG," StaticEntry=%s", (snoopSyncReply[iterator].isStatic) ? "Yes" : "No");
      PT_LOG_TRACE(LOG_CTX_MSG," numberOfActivePorts=%u", snoopSyncReply[iterator].numberOfActivePorts);
      numberOfActivePorts=0;
      if(snoopSyncReply[iterator].numberOfActivePorts > 0)
      {
        for (intIfNum=1;intIfNum<PTIN_SYSTEM_MAXINTERFACES_PER_GROUP;intIfNum++)
        {   
          if (PTIN_IS_MASKBITSET(snoopSyncReply[iterator].intIfNum_mask,intIfNum))
          {
            PT_LOG_DEBUG(LOG_CTX_MSG, "Snoop Port Open :%u", intIfNum);
            #if PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD
            if(snoopPortOpen(snoopSyncReply[iterator].serviceId, intIfNum, &groupAddr, &sourceAddr, snoopSyncReply[iterator].isStatic, isProtection)!=L7_SUCCESS)
            #else
            if(snooping_port_open(snoopSyncReply[iterator].serviceId, intIfNum, snoopSyncReply[iterator].groupAddr, sourceAddr, snoopSyncReply[iterator].isStatic)!=L7_SUCCESS)
            #endif
            {
              PT_LOG_ERR(LOG_CTX_MSG, "Failed to open port");
              return L7_FAILURE;
            }

            if(++numberOfActivePorts >= snoopSyncReply[iterator].numberOfActivePorts)
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
      PT_LOG_TRACE(LOG_CTX_MSG," serviceId=%u", snoopSyncReply[iterator].serviceId);
#if PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD
      ptin_to_fp_ip_notation(&snoopSyncReply[iterator].groupAddr, &groupAddr);
      ptin_to_fp_ip_notation(&snoopSyncReply[iterator].sourceAddr, &sourceAddr);

      inetAddrPrint(&groupAddr, groupAddrStr);
      inetAddrPrint(&sourceAddr, sourceAddrStr);
      PT_LOG_TRACE(LOG_CTX_MSG," groupAddr=%s", groupAddrStr);
      PT_LOG_TRACE(LOG_CTX_MSG," sourceAddr=%s", sourceAddrStr);
#else
      PT_LOG_TRACE(LOG_CTX_MSG," groupAddr=%08X", snoopSyncReply[iterator].groupAddr);
#endif
      PT_LOG_TRACE(LOG_CTX_MSG," StaticEntry=%s", (snoopSyncReply[iterator].isStatic) ? "Yes" : "No");
      PT_LOG_TRACE(LOG_CTX_MSG," portId=%u", snoopSyncReply->portId);
      #if PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD
      if(snoopPortOpen(snoopSyncReply[iterator].serviceId, snoopSyncReply[iterator].portId, &groupAddr, &sourceAddr, snoopSyncReply[iterator].isStatic, isProtection)!=L7_SUCCESS)
      #else
      if(snooping_port_open(snoopSyncReply[iterator].serviceId, snoopSyncReply[iterator].portId, snoopSyncReply[iterator].groupAddr, sourceAddr, snoopSyncReply[iterator].isStatic)!=L7_SUCCESS)
      #endif
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Failed to open port");
        return L7_FAILURE;
      }
    }
  }
  #endif

  //Request the remaining snoop entries
  if(numberOfSnoopEntries!=maxNumberOfSnoopEntries)
  {
    PT_LOG_TRACE(LOG_CTX_MSG, "This is the Last Snoop Sync Reply Message Received");
    return L7_SUCCESS;
  }

  msg_SnoopSyncRequest_t   snoopSyncRequest;
  L7_uint32                ipAddr;

  memset(&snoopSyncRequest, 0x00, sizeof(snoopSyncRequest));

#if PTIN_SYSTEM_IGMP_L3_MULTICAST_FORWARD
  memcpy(&snoopSyncRequest.groupAddr, &snoopSyncReply[numberOfSnoopEntries-1].groupAddr, sizeof(snoopSyncRequest.groupAddr));
  memcpy(&snoopSyncRequest.sourceAddr, &snoopSyncReply[numberOfSnoopEntries-1].sourceAddr, sizeof(snoopSyncRequest.sourceAddr));
#else
  snoopSyncRequest.groupAddr    = snoopSyncReply[numberOfSnoopEntries-1].groupAddr;
#endif
  snoopSyncRequest.serviceId    = snoopSyncReply[numberOfSnoopEntries-1].serviceId;

#if PTIN_BOARD_IS_MATRIX    
  if(ptin_fpga_mx_is_matrixactive_rt())//If I'm a Active Matrix
  {
    PT_LOG_NOTICE(LOG_CTX_MSG, "Not sending Another Snoop Sync Request Message to Sync the Remaining Snoop Entries. I'm a Active Matrix on slotId:%u",ptin_fpga_board_slot_get());
    return SUCCESS;
  }

  /* MX board IP address */
  ipAddr = IPC_MX_PAIR_IPADDR;
  
  PT_LOG_INFO(LOG_CTX_MSG, "Sending Snoop Sync Request Message [groupAddr:%08X | serviceId:%u] to ipAddr:%08X (%u) to Sync the Remaining Snoop Entries", snoopSyncRequest.groupAddr, snoopSyncRequest.serviceId, ipAddr, MX_PAIR_SLOT_ID);         
#else
  ptin_prottypeb_intf_config_t protTypebIntfConfig = {0};     

  /*  Get the configuration of this portId for the Type B Scheme Protection */
  ptin_prottypeb_intf_config_get(snoopSyncReply[numberOfSnoopEntries-1].portId, &protTypebIntfConfig);    

  if(protTypebIntfConfig.status==L7_ENABLE)//If I'm a Protection
  {
    PT_LOG_NOTICE(LOG_CTX_MSG, "Not sending Another Snoop Sync Request Message to Sync the Remaining Snoop Entries. I'm a Active slotId/intfNum:%u/%u",protTypebIntfConfig.pairSlotId, protTypebIntfConfig.intfNum);
    return SUCCESS;
  }
    
  #if PTIN_BOARD_IS_STANDALONE
  ipAddr = simGetIpcIpAddr();
  #else
  /* Determine the IP address of the working port/slot */   
  if (L7_SUCCESS != ptin_fpga_slot_ip_addr_get(protTypebIntfConfig.pairSlotId, &ipAddr))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Failed to obtain ipAddress of slotId:%u", protTypebIntfConfig.pairSlotId);
    return L7_FAILURE;
  }
  #endif
  snoopSyncRequest.portId = protTypebIntfConfig.pairIntfNum;

  PT_LOG_DEBUG(LOG_CTX_MSG, "Sending Snoop Sync Request Message [groupAddr:%08X | serviceId:%u | portId:%u] to ipAddr:%08X to Sync the Remaining Snoop Entries", snoopSyncRequest.groupAddr, snoopSyncRequest.serviceId, snoopSyncRequest.portId, ipAddr);
#endif
              
  
  /*Send the snoop sync request to the protection matrix */  
  if (send_ipc_message(IPC_HW_FASTPATH_PORT, ipAddr, CCMSG_MGMD_SNOOP_SYNC_REQUEST, (char *)(&snoopSyncRequest), NULL, sizeof(snoopSyncRequest), NULL) < 0)
  {
    PT_LOG_ERR(LOG_CTX_PROTB, "Failed to Send Snoop Sync Request Message");
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
L7_RC_t ptin_msg_IGMP_clientList_get(msg_MCActiveChannelClientsResponse_t *client_list)
{
  L7_in_addr_t     channelIp, sourceIp;
  ptin_client_id_t clist[MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX];
  L7_uint32        extended_evc_id[MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX];
  L7_uint16        i, number_of_clients, total_clients;
  L7_RC_t rc;

  PT_LOG_DEBUG(LOG_CTX_MSG,"Going to retrieve list of clients");
  PT_LOG_DEBUG(LOG_CTX_MSG," slotId     =%u",client_list->SlotId);
  PT_LOG_DEBUG(LOG_CTX_MSG," EvcId      =%u",ENDIAN_SWAP32(client_list->evc_id));
  PT_LOG_DEBUG(LOG_CTX_MSG," groupAddr=%u.%u.%u.%u",(ENDIAN_SWAP32(client_list->channelIp.s_addr)>>24) & 0xff,(ENDIAN_SWAP32(client_list->channelIp.s_addr)>>16) & 0xff,(ENDIAN_SWAP32(client_list->channelIp.s_addr)>>8) & 0xff,ENDIAN_SWAP32(client_list->channelIp.s_addr) & 0xff);
  
  PT_LOG_DEBUG(LOG_CTX_MSG," sourceAddr=%u.%u.%u.%u",(ENDIAN_SWAP32(client_list->sourceIp.s_addr)>>24) & 0xff,(ENDIAN_SWAP32(client_list->sourceIp.s_addr)>>16) & 0xff,(ENDIAN_SWAP32(client_list->sourceIp.s_addr)>>8) & 0xff,ENDIAN_SWAP32(client_list->sourceIp.s_addr) & 0xff);
  PT_LOG_DEBUG(LOG_CTX_MSG," Page_idx=%u",ENDIAN_SWAP16(client_list->page_index));

  /* Get list of channels */      
  channelIp.s_addr    = ENDIAN_SWAP32(client_list->channelIp.s_addr);
  sourceIp.s_addr     = ENDIAN_SWAP32(client_list->sourceIp.s_addr);
  number_of_clients   = MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX;

  rc = ptin_igmp_clientList_get(ENDIAN_SWAP32(client_list->evc_id), &channelIp, &sourceIp, ENDIAN_SWAP16(client_list->page_index)*MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX, &number_of_clients, clist, extended_evc_id,&total_clients);
  PT_LOG_DEBUG(LOG_CTX_MSG,"number_of_clients=%u total_clients=%u", number_of_clients, total_clients);
  if (rc==L7_SUCCESS)
  {
    /* Copy channels to message */
    for (i=0; i<MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX && i<number_of_clients; i++)
    {
      client_list->clients_list[i].mask           = clist[i].mask;
      client_list->clients_list[i].outer_vlan     = ENDIAN_SWAP16(clist[i].outerVlan);
      client_list->clients_list[i].inner_vlan     = ENDIAN_SWAP16(clist[i].innerVlan);
      client_list->clients_list[i].intf.intf_type = clist[i].ptin_intf.intf_type;
      client_list->clients_list[i].intf.intf_id   = clist[i].ptin_intf.intf_id;
      //MAC Bridge Services Support
      client_list->clients_list[i].evc_id         = extended_evc_id[i];
      //End MAC Bridge Services Support
    }
    client_list->n_pages_total   = (total_clients==0) ? 1 : ((total_clients-1)/MSG_MCACTIVECHANNELCLIENTS_CLIENTS_MAX+1);
    client_list->n_pages_total   = ENDIAN_SWAP16(client_list->n_pages_total);
    client_list->n_clients_total = ENDIAN_SWAP16(total_clients);
    client_list->n_clients_msg   = ENDIAN_SWAP16(number_of_clients);
  }
  else if (rc==L7_NOT_EXIST)
  {
    PT_LOG_WARN(LOG_CTX_MSG, "No channels to retrieve");
    client_list->n_pages_total   = 1;
    client_list->n_clients_total = 0;
    client_list->n_clients_msg   = 0;
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error with ptin_igmp_clientList_get");
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
L7_RC_t ptin_msg_igmp_static_channel_remove_all(msg_MCStaticChannel_t *channel, L7_uint16 noOfMessages)
{
  PTIN_MGMD_CTRL_STATICGROUP_t staticGroup;
  L7_uint16 messageIterator;
  L7_RC_t rc = L7_SUCCESS;
  L7_RC_t rc_global = L7_SUCCESS;

  if (channel==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid arguments");
    return L7_FAILURE;
  }
  for (messageIterator=0; messageIterator<noOfMessages; messageIterator++)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG,"Channel remotion index %u:",messageIterator);
    PT_LOG_DEBUG(LOG_CTX_MSG," SlotId =%u",channel[messageIterator].SlotId);
    PT_LOG_DEBUG(LOG_CTX_MSG," EvcId  =%u",ENDIAN_SWAP32(channel[messageIterator].evc_id));

    staticGroup.serviceId = ENDIAN_SWAP32(channel[messageIterator].evc_id);
    
    if ((ptin_igmp_mgmd_service_remove(ENDIAN_SWAP32(channel[messageIterator].evc_id))) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error (%d) removing channel", rc);
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
    PT_LOG_TRACE(LOG_CTX_MSG, "Received protection command: "); 
    PT_LOG_TRACE(LOG_CTX_MSG, " slot = %u", ENDIAN_SWAP8(cmd[i].slotId));
    PT_LOG_TRACE(LOG_CTX_MSG, " port = %u", ENDIAN_SWAP8(cmd[i].port));
    PT_LOG_TRACE(LOG_CTX_MSG, " cmd  = %u", ENDIAN_SWAP8(cmd[i].protCmd));

    if (n > 1)
    {
      if      ( (ENDIAN_SWAP8(cmd[i].protCmd) & 1) && i2add < 0)  i2add = i;
      else if (!(ENDIAN_SWAP8(cmd[i].protCmd) & 1) && i2rem < 0)  i2rem = i;
    }
  }

  /* If provided a port to be removed, and to be added, follow plan D for those ports */
  if (i2add >= 0 && i2rem >= 0)
  {
    PT_LOG_TRACE(LOG_CTX_MSG, "Applying plan D for slot/ports %d/%d -> %d/%d",
                 ENDIAN_SWAP8(cmd[i2rem].slotId), ENDIAN_SWAP8(cmd[i2rem].port), ENDIAN_SWAP8(cmd[i2add].slotId), ENDIAN_SWAP8(cmd[i2add].port));
    /* PLAN D */
    if (ptin_intf_protection_cmd_planD(ENDIAN_SWAP8(cmd[i2rem].slotId), ENDIAN_SWAP8(cmd[i2rem].port),
                                       ENDIAN_SWAP8(cmd[i2add].slotId), ENDIAN_SWAP8(cmd[i2add].port)) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Plan D failed for slot/ports %d/%d -> %d/%d",
                 ENDIAN_SWAP8(cmd[i2rem].slotId), ENDIAN_SWAP8(cmd[i2rem].port), ENDIAN_SWAP8(cmd[i2add].slotId), ENDIAN_SWAP8(cmd[i2add].port));
      rc = L7_FAILURE;
    }
  }

  /* Apply plan C for other ports */
  for (i = 0; i < n; i++) 
  {
    /* Skip ports used for plan D */
    if (i == i2rem || i == i2add)  continue;

    PT_LOG_TRACE(LOG_CTX_MSG, "Applying plan C for slot/port %d/%d", ENDIAN_SWAP8(cmd[i].slotId), ENDIAN_SWAP8(cmd[i].port));
    /* PLAN C */
    if (ptin_intf_protection_cmd_planC(ENDIAN_SWAP8(cmd[i].slotId), ENDIAN_SWAP8(cmd[i].port), ENDIAN_SWAP8(cmd[i].protCmd)) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Plan C failed for slot/port %d/%d", ENDIAN_SWAP8(cmd[i].slotId), ENDIAN_SWAP8(cmd[i].port));
      rc = L7_FAILURE;
    }
  }

  /* Check for result */
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Protection switch failed"); 
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Successfull protection switch!"); 
  }
#else
  PT_LOG_ERR(LOG_CTX_MSG, "Uplink protection not supported for this board!");
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
  PT_LOG_TRACE(LOG_CTX_MSG, "Received request to sync MGMD port: ");
  PT_LOG_TRACE(LOG_CTX_MSG, " admin      = %u",   port_sync_data->admin);
  PT_LOG_TRACE(LOG_CTX_MSG, " serviceId  = %u",   port_sync_data->serviceId);
  PT_LOG_TRACE(LOG_CTX_MSG, " portId     = %u",   port_sync_data->portId);
  PT_LOG_TRACE(LOG_CTX_MSG, " groupAddr  = %08X", port_sync_data->groupAddr);
  PT_LOG_TRACE(LOG_CTX_MSG, " sourceAddr = %08X", port_sync_data->sourceAddr);
  PT_LOG_TRACE(LOG_CTX_MSG, " groupType  = %u",   port_sync_data->groupType);

  ptin_igmp_mgmd_port_sync(port_sync_data->admin,
                           port_sync_data->serviceId,
                           port_sync_data->portId,
                           port_sync_data->groupAddr,
                           port_sync_data->sourceAddr,
                           port_sync_data->groupType);

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
    PT_LOG_DEBUG(LOG_CTX_MSG,"PRBS configuration:");
    PT_LOG_DEBUG(LOG_CTX_MSG," slotId = %u",    ENDIAN_SWAP8(msg[i].SlotId));
    PT_LOG_DEBUG(LOG_CTX_MSG," Port   = %u/%u", ENDIAN_SWAP8(msg[i].intf.intf_type), ENDIAN_SWAP8(msg[i].intf.intf_id));
    PT_LOG_DEBUG(LOG_CTX_MSG," Enable = %u",    ENDIAN_SWAP8(msg[i].enable));

    ptin_intf.intf_type = ENDIAN_SWAP8(msg[i].intf.intf_type);
    ptin_intf.intf_id   = ENDIAN_SWAP8(msg[i].intf.intf_id);

    /* Get intIfNum */
    if (ptin_intf_ptintf2intIfNum(&ptin_intf, &intIfNum)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Non existent port (%u/%u)",ptin_intf.intf_type,ptin_intf.intf_id);
      return L7_FAILURE;
    }

    rc = ptin_pcs_prbs_enable(intIfNum, ENDIAN_SWAP8(msg[i].enable));
    if (rc!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error settings PRBS enable of port %u/%u to %u", ptin_intf.intf_type, ptin_intf.intf_id, ENDIAN_SWAP8(msg[i].enable));
      return rc;
    }

    PT_LOG_TRACE(LOG_CTX_MSG,"Success setting PRBS enable of port %u/%u to %u", ptin_intf.intf_type, ptin_intf.intf_id, ENDIAN_SWAP8(msg[i].enable));
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
    PT_LOG_DEBUG(LOG_CTX_MSG,"PRBS status:");
    PT_LOG_DEBUG(LOG_CTX_MSG," slotId = %u",    ENDIAN_SWAP8(msg[i].SlotId));
    PT_LOG_DEBUG(LOG_CTX_MSG," Port   = %u/%u", ENDIAN_SWAP8(msg[i].intf.intf_type), ENDIAN_SWAP8(msg[i].intf.intf_id));

    ptin_intf.intf_type = ENDIAN_SWAP8(msg[i].intf.intf_type);
    ptin_intf.intf_id   = ENDIAN_SWAP8(msg[i].intf.intf_id);

    /* Get intIfNum */
    if (ptin_intf_ptintf2intIfNum(&ptin_intf, &intIfNum)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Non existent port (%u/%u)",ptin_intf.intf_type,ptin_intf.intf_id);
      return L7_FAILURE;
    }

    /* Read number of PRBS errors */
    rc = ptin_pcs_prbs_errors_get(intIfNum, &rxStatus);
    if (rc!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Error getting PRBS errors from port %u/%u",ptin_intf.intf_type,ptin_intf.intf_id);
      return rc;
    }

    PT_LOG_TRACE(LOG_CTX_MSG,"Success getting PRBS errors from port %u/%u",ptin_intf.intf_type,ptin_intf.intf_id);

    memset(&msg[i].rxStatus, 0x00, sizeof(msg[i].rxStatus));

    if (rxStatus<=0xffff)
    {
      msg[i].rxStatus.lock = ENDIAN_SWAP8(L7_TRUE);
      msg[i].rxStatus.rxErrors = ENDIAN_SWAP32(rxStatus);
    }
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
L7_RC_t ptin_msg_prbs_enable(msg_ptin_prbs_enable *msg, L7_int n_msg)
{
  L7_uint8  enable;
  L7_uint32 i, intIfNum, port;
  L7_RC_t rc, rc_global = L7_SUCCESS;

  if (n_msg == 0)
  {
    return L7_SUCCESS;
  }

  /* Apply to all ports */
  if (ENDIAN_SWAP8(msg->intf) == 0xff)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG,"PRBS configuration:");
    PT_LOG_DEBUG(LOG_CTX_MSG," slotId = %u", ENDIAN_SWAP8(msg->SlotId));
    PT_LOG_DEBUG(LOG_CTX_MSG," Port   = %u", ENDIAN_SWAP8(msg->intf));
    PT_LOG_DEBUG(LOG_CTX_MSG," Enable = %u", ENDIAN_SWAP8(msg->enable));

    enable = ENDIAN_SWAP8(msg->enable);

    /* Run all ports */
    for (port = 0; port < ptin_sys_number_of_ports; port++)
    {
      /* Skip non backplane ports */
      if (!(PTIN_SYSTEM_10G_PORTS_MASK & (1ULL << port)))
      {
        continue;
      }

      /* Get intIfNum */
      if (ptin_intf_port2intIfNum(port, &intIfNum)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Non existent port %u", port);
        rc_global = L7_FAILURE;
        continue;
      }

      rc = ptin_pcs_prbs_enable(intIfNum, enable);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Error settings PRBS enable of port %u to %u", port, enable);
        rc_global = rc;
        continue;
      }

      PT_LOG_TRACE(LOG_CTX_MSG, "Success setting PRBS enable of port %u to %u", port, enable);
    }
  }
  /* Apply to each port */
  else
  {
    for (i=0; i<n_msg; i++)
    {
      PT_LOG_DEBUG(LOG_CTX_MSG,"PRBS configuration:");
      PT_LOG_DEBUG(LOG_CTX_MSG," slotId = %u", ENDIAN_SWAP8(msg[i].SlotId));
      PT_LOG_DEBUG(LOG_CTX_MSG," Port   = %u", ENDIAN_SWAP8(msg[i].intf));
      PT_LOG_DEBUG(LOG_CTX_MSG," Enable = %u", ENDIAN_SWAP8(msg[i].enable));

      port = ENDIAN_SWAP8(msg[i].intf);
      enable = ENDIAN_SWAP8(msg[i].enable);

      /* Validate port */
      if (port >= ptin_sys_number_of_ports)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Non existent port %u", port);
        rc_global = L7_FAILURE;
        continue;
      }
      /* Get intIfNum */
      if (ptin_intf_port2intIfNum(port, &intIfNum)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Non existent port %u", port);
        rc_global = L7_FAILURE;
        continue;
      }

      rc = ptin_pcs_prbs_enable(intIfNum, enable);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Error settings PRBS enable of port %u to %u", port, enable);
        rc_global = rc;
        continue;
      }

      PT_LOG_TRACE(LOG_CTX_MSG, "Success setting PRBS enable of port %u to %u", port, enable); 
    }
  }

  /* Total success? */
  if (rc_global == L7_SUCCESS)
  {
    PT_LOG_TRACE(LOG_CTX_MSG, "Success setting PRBS enable of all ports"); 
  }

  return L7_SUCCESS;
}

/**
 * Read PRBS errors
 * 
 * @param msg_in : PRBS inputs 
 * @param msg_out : PRBS results 
 * @param n_msg : Number of structures
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_prbs_status(msg_ptin_prbs_request *msg_in, msg_ptin_prbs_status *msg_out, L7_int *n_msg)
{
  L7_uint32 i, port, intIfNum, rxStatus;
  L7_RC_t   rc, rc_global = L7_SUCCESS;

  if (*n_msg == 0)
  {
    return L7_SUCCESS;
  }

  if (ENDIAN_SWAP8(msg_in->intf) == 0xff)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG,"PRBS status:");
    PT_LOG_DEBUG(LOG_CTX_MSG," slotId = %u", ENDIAN_SWAP8(msg_in->SlotId));
    PT_LOG_DEBUG(LOG_CTX_MSG," Port   = %u", ENDIAN_SWAP8(msg_in->intf));

    /* Run all ports */
    for (port = 0, i = 0; port < ptin_sys_number_of_ports; port++)
    {
      /* Skip non backplane ports */
      if (!(PTIN_SYSTEM_10G_PORTS_MASK & (1ULL << port)))
      {
        continue;
      }

      /* Get intIfNum */
      if (ptin_intf_port2intIfNum(port, &intIfNum)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Non existent port %u", port);
        rc_global = L7_FAILURE;
        continue;
      }

      /* Read number of PRBS errors */
      rc = ptin_pcs_prbs_errors_get(intIfNum, &rxStatus);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Error getting PRBS errors from port %u/%u", port);
        rc_global = rc;
        continue;
      }

      PT_LOG_TRACE(LOG_CTX_MSG,"Success getting PRBS errors from port %u", port);

      /* Store result */
      msg_out[i++].rxErrors = ENDIAN_SWAP32(rxStatus);
    }

    /* Update number of results */
    *n_msg = i;
  }
  else
  {
    /* Run all messages */
    for (i=0; i<*n_msg; i++)
    {
      PT_LOG_DEBUG(LOG_CTX_MSG,"PRBS status:");
      PT_LOG_DEBUG(LOG_CTX_MSG," slotId = %u", ENDIAN_SWAP8(msg_in[i].SlotId));
      PT_LOG_DEBUG(LOG_CTX_MSG," Port   = %u", ENDIAN_SWAP8(msg_in[i].intf));

      /* Init output as -1 (error) */
      msg_out[i].rxErrors = ENDIAN_SWAP32(-1);

      port = ENDIAN_SWAP8(msg_in[i].intf);

      /* Validate port */
      if (port >= ptin_sys_number_of_ports)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Non existent port %u", port);
        rc_global = L7_FAILURE;
        continue;
      }
      /* Get intIfNum */
      if (ptin_intf_port2intIfNum(port, &intIfNum)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Non existent port %u", port);
        rc_global = L7_FAILURE;
        continue;
      }

      /* Read number of PRBS errors */
      rc = ptin_pcs_prbs_errors_get(intIfNum, &rxStatus);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG,"Error getting PRBS errors from port %u/%u", port);
        rc_global = rc;
        continue;
      }

      PT_LOG_TRACE(LOG_CTX_MSG,"Success getting PRBS errors from port %u", port);

      /* Store result */
      msg_out[i].rxErrors = ENDIAN_SWAP32(rxStatus);
    }

    /* Do not modify number of results */
  }

  /* Total success? */
  if (rc_global == L7_SUCCESS)
  {
    PT_LOG_TRACE(LOG_CTX_MSG, "Success getting PRBS errors for all ports");
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
  //L7_char8 *prevtty = ttyname(1);

  //ptin_PitHandler(tty);

  if (strcmp(type, "driv") == 0)
  {
    dtlDriverShell(cmd);
  }
  else if (strcmp(type, "dev") == 0)
  {
    if (osapiDevShellExec(cmd) != 0)
      rc = L7_FAILURE;
  }

  fflush(stdout);

  //ptin_PitHandler(prevtty);

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
  msgPortStats->Port                              = ENDIAN_SWAP8 (ptinPortStats->Port);
  msgPortStats->Mask                              = ENDIAN_SWAP8 (ptinPortStats->Mask);
  msgPortStats->RxMask                            = ENDIAN_SWAP32(ptinPortStats->RxMask);
  msgPortStats->TxMask                            = ENDIAN_SWAP32(ptinPortStats->TxMask);
  /* Rx */
  msgPortStats->Rx.etherStatsDropEvents           = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsDropEvents);
  msgPortStats->Rx.etherStatsOctets               = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsOctets);
  msgPortStats->Rx.etherStatsPkts                 = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsPkts);
  msgPortStats->Rx.etherStatsBroadcastPkts        = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsBroadcastPkts);
  msgPortStats->Rx.etherStatsMulticastPkts        = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsMulticastPkts);
  msgPortStats->Rx.etherStatsCRCAlignErrors       = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsCRCAlignErrors);
  msgPortStats->Rx.etherStatsUndersizePkts        = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsUndersizePkts);
  msgPortStats->Rx.etherStatsOversizePkts         = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsOversizePkts);
  msgPortStats->Rx.etherStatsFragments            = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsFragments);
  msgPortStats->Rx.etherStatsJabbers              = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsJabbers);
  msgPortStats->Rx.etherStatsCollisions           = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsCollisions);
  msgPortStats->Rx.etherStatsPkts64Octets         = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsPkts64Octets);
  msgPortStats->Rx.etherStatsPkts65to127Octets    = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsPkts65to127Octets);
  msgPortStats->Rx.etherStatsPkts128to255Octets   = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsPkts128to255Octets);
  msgPortStats->Rx.etherStatsPkts256to511Octets   = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsPkts256to511Octets);
  msgPortStats->Rx.etherStatsPkts512to1023Octets  = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsPkts512to1023Octets);
  msgPortStats->Rx.etherStatsPkts1024to1518Octets = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsPkts1024to1518Octets);
  msgPortStats->Rx.etherStatsPkts1519toMaxOctets  = ENDIAN_SWAP64(ptinPortStats->Rx.etherStatsPkts1519toMaxOctets);
  msgPortStats->Rx.Throughput                     = ENDIAN_SWAP64(ptinPortStats->Rx.Throughput);
  /* Tx */
  msgPortStats->Tx.etherStatsDropEvents           = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsDropEvents);
  msgPortStats->Tx.etherStatsOctets               = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsOctets);
  msgPortStats->Tx.etherStatsPkts                 = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsPkts);
  msgPortStats->Tx.etherStatsBroadcastPkts        = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsBroadcastPkts);
  msgPortStats->Tx.etherStatsMulticastPkts        = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsMulticastPkts);
  msgPortStats->Tx.etherStatsCRCAlignErrors       = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsCRCAlignErrors);
  msgPortStats->Tx.etherStatsUndersizePkts        = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsUndersizePkts);
  msgPortStats->Tx.etherStatsOversizePkts         = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsOversizePkts);
  msgPortStats->Tx.etherStatsFragments            = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsFragments);
  msgPortStats->Tx.etherStatsJabbers              = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsJabbers);
  msgPortStats->Tx.etherStatsCollisions           = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsCollisions);
  msgPortStats->Tx.etherStatsPkts64Octets         = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsPkts64Octets);
  msgPortStats->Tx.etherStatsPkts65to127Octets    = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsPkts65to127Octets);
  msgPortStats->Tx.etherStatsPkts128to255Octets   = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsPkts128to255Octets);
  msgPortStats->Tx.etherStatsPkts256to511Octets   = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsPkts256to511Octets);
  msgPortStats->Tx.etherStatsPkts512to1023Octets  = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsPkts512to1023Octets);
  msgPortStats->Tx.etherStatsPkts1024to1518Octets = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsPkts1024to1518Octets);
  msgPortStats->Tx.etherStatsPkts1519toMaxOctets  = ENDIAN_SWAP64(ptinPortStats->Tx.etherStatsPkts1519toMaxOctets);
  msgPortStats->Tx.Throughput                     = ENDIAN_SWAP64(ptinPortStats->Tx.Throughput);
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
static L7_RC_t ptin_msg_bwProfileStruct_fill(msg_HwEthBwProfile_t *msgBwProfile, ptin_bw_profile_t *profile, ptin_bw_meter_t *meter)
{
  L7_int    ptin_port;
  L7_uint8  mask;

  /* Validate arguments */
  if (msgBwProfile == L7_NULLPTR || profile == L7_NULLPTR || meter == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Null arguments");
    return L7_FAILURE;
  }

  /* SVID */
  profile->outer_vlan_lookup   = 0;
  profile->outer_vlan_ingress  = 0;
  profile->outer_vlan_egress   = 0;

  /* CVID */
  profile->inner_vlan_ingress  = 0;
  profile->inner_vlan_egress   = 0;

  /* No MAC address provided */
  memset(profile->macAddr, 0x00, sizeof(L7_uint8)*L7_MAC_ADDR_LEN);

  mask = msgBwProfile->mask;

  /* Source interface */
  if (mask & MSG_HWETH_BWPROFILE_MASK_INTF_SRC)
  {
    /* SVID */
    if ((mask & MSG_HWETH_BWPROFILE_MASK_SVLAN) &&
        (msgBwProfile->service_vlan > 0 && msgBwProfile->service_vlan < 4096))
    {
      profile->outer_vlan_lookup = msgBwProfile->service_vlan;
      PT_LOG_DEBUG(LOG_CTX_MSG," SVID extracted!");
    }

    /* CVID */
    if ((mask & MSG_HWETH_BWPROFILE_MASK_CVLAN) &&
        (msgBwProfile->client_vlan > 0 && msgBwProfile->client_vlan < 4096))
    {
      profile->inner_vlan_ingress = msgBwProfile->client_vlan;
      PT_LOG_DEBUG(LOG_CTX_MSG," CVID extracted!");
    }

    /* Get ptin_port */
    if (ptin_msg_ptinPort_get(msgBwProfile->intf_src.intf_type, msgBwProfile->intf_src.intf_id, &ptin_port) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Invalid port reference");
      return L7_FAILURE;
    }

    /* Calculate ddUSP */
    profile->ptin_port = ptin_port;

    PT_LOG_DEBUG(LOG_CTX_MSG," SrcIntf extracted!");
  }

  /* Destination interface */
  if (mask & MSG_HWETH_BWPROFILE_MASK_INTF_DST)
  {
    /* SVID */
    if ((mask & MSG_HWETH_BWPROFILE_MASK_SVLAN) &&
        (msgBwProfile->service_vlan > 0 && msgBwProfile->service_vlan < 4096))
    {
      profile->outer_vlan_egress = msgBwProfile->service_vlan;
      PT_LOG_DEBUG(LOG_CTX_MSG," SVID extracted!");
    }

    /* CVID */
    if ((mask & MSG_HWETH_BWPROFILE_MASK_CVLAN) &&
        (msgBwProfile->client_vlan > 0 && msgBwProfile->client_vlan < 4096))
    {
      profile->inner_vlan_egress = msgBwProfile->client_vlan;
      PT_LOG_DEBUG(LOG_CTX_MSG," CVID extracted!");
    }

    /* Get ptin_port */
    if (ptin_msg_ptinPort_get(msgBwProfile->intf_dst.intf_type, msgBwProfile->intf_dst.intf_id, &ptin_port) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Invalid port reference");
      return L7_FAILURE;
    }

    /* Calculate ddUSP */
    profile->ptin_port = ptin_port;

    PT_LOG_DEBUG(LOG_CTX_MSG," DstIntf extracted!");
  }

  if (mask & MSG_HWETH_BWPROFILE_MASK_PROFILE)
  {
    meter->cir = (L7_uint32) (msgBwProfile->profile.cir)/1000;   /* in kbps */
    meter->cbs = (L7_uint32)  msgBwProfile->profile.cbs;         /* in bytes */
    meter->eir = (L7_uint32) (msgBwProfile->profile.eir)/1000;   /* in kbps */
    meter->ebs = (L7_uint32)  msgBwProfile->profile.ebs;         /* in bytes */
    PT_LOG_DEBUG(LOG_CTX_MSG," Profile data extracted!");
  }
  else
  {
    meter->cir = (L7_uint32) -1;
    meter->cbs = (L7_uint32) -1;
    meter->eir = (L7_uint32) -1;
    meter->ebs = (L7_uint32) -1;
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
  if (msg_evcStats == L7_NULLPTR || evcStats_profile == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Null arguments");
    return L7_FAILURE;
  }

  /* Source interface */
  if (msg_evcStats->mask & MSG_EVC_COUNTERS_MASK_INTF)
  {
    /* Get ptin_port */
    if (ptin_msg_ptinPort_get(msg_evcStats->intf.intf_type, msg_evcStats->intf.intf_id, &ptin_port) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG,"Invalid port reference");
      return L7_FAILURE;
    }
    evcStats_profile->ptin_port = ptin_port;
    PT_LOG_DEBUG(LOG_CTX_MSG," Intf extracted!");
  }

  /* SVID */
  evcStats_profile->outer_vlan_lookup  = 0;
  evcStats_profile->outer_vlan_ingress = 0;
  evcStats_profile->outer_vlan_egress  = 0;
  if ((msg_evcStats->mask & MSG_EVC_COUNTERS_MASK_SVLAN) &&
      (msg_evcStats->service_vlan > 0 && msg_evcStats->service_vlan < 4096))
  {
    evcStats_profile->outer_vlan_lookup = msg_evcStats->service_vlan;
    PT_LOG_DEBUG(LOG_CTX_MSG," SVID extracted!");
  }

  /* CVID */
  evcStats_profile->inner_vlan_ingress  = 0;
  evcStats_profile->inner_vlan_egress = 0;
  if ((msg_evcStats->mask & MSG_EVC_COUNTERS_MASK_CVLAN) &&
      (msg_evcStats->client_vlan > 0 && msg_evcStats->client_vlan < 4096))
  {
    evcStats_profile->inner_vlan_ingress = msg_evcStats->client_vlan;
    PT_LOG_DEBUG(LOG_CTX_MSG," CVID extracted!");
  }

  /* Channel IP */
  evcStats_profile->dst_ip = 0;
  if ((msg_evcStats->mask & MSG_EVC_COUNTERS_MASK_CHANNEL) &&
      (msg_evcStats->channel_ip != 0 && msg_evcStats->channel_ip != (L7_uint32)-1))
  {
    evcStats_profile->dst_ip = msg_evcStats->channel_ip;
    PT_LOG_DEBUG(LOG_CTX_MSG," ChannelIP extracted!");
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

#if MNGMT_DIFFERENT_ENDIANNESS
  {
   pi[i].index = ENDIAN_SWAP64(pi[i].index);
   pi[i].bd.vid = ENDIAN_SWAP64(pi[i].bd.vid);
   pi[i].bd.mep_id = ENDIAN_SWAP16(pi[i].bd.mep_id);
   pi[i].bd.prt = ENDIAN_SWAP16(pi[i].bd.prt);
  }
#endif

  porta = pi[i].bd.prt;

  if ((pi[i].flags & 0x01)) {
  L7_uint16 slot, port;

    if (L7_SUCCESS!=ptin_intf_port2SlotPort(porta, &slot, &port, L7_NULLPTR)) return L7_FAILURE;
    if (slot!=pi[i].tu_slot) {
        PT_LOG_ERR(LOG_CTX_MSG, "ptin_intf_port=%lu => (slot,port)=(%u,%u) struct_passed_slot=%u", porta, slot, port, pi[i].tu_slot);
        //return L7_FAILURE;
    }
    if (send_also_uplinkprot_traps(1, slot, port, pi[i].bd.vid));// return L7_FAILURE;
  }


  if (valid_mep_index(pi[i].index)) {
      p = &oam.db[pi[i].index].mep;
      if (EMPTY_T_MEP(*p)) PT_LOG_DEBUG(LOG_CTX_MSG, "MEP EMPTY");//changing_trap=0;

      else {
          //old_prt=p->prt;       old_vid=p->vid;     old_level=p->level;
          //changing_trap=1;  //if (old_prt!=porta || old_vid!=pi[i].bd.vid || old_level!=pi[i].bd.level) changing_trap=1;    else changing_trap=0;
          ptin_msg_del_MEP(inbuff, outbuff, i);
          PT_LOG_DEBUG(LOG_CTX_MSG, "MEP DEL");
      }
  }
  //else changing_trap=0;


  switch (wr_mep(pi[i].index, (T_MEP_HDR*)&pi[i].bd, &oam))
  {
  case 0:    r=S_OK;
    //if (changing_trap)  ptin_ccm_packet_trap(old_prt, old_vid, old_level, 0);
    ptin_ccm_packet_trap(porta, pi[i].bd.vid, pi[i].bd.level, 1);
#ifdef USING_SDK_OAM_FP_CREATE
    {
     L7_uint32 intIfNum;
     hapi_mep_t hm;
     L7_uint16 vidInternal;

     hm.imep=   pi[i].index;
     hm.m=      &pi[i].bd;

     if (L7_SUCCESS!=ptin_intf_port2intIfNum(porta, &intIfNum))
       PT_LOG_DEBUG(LOG_CTX_MSG, "Insucess port2intfNum");
     else
     if (L7_SUCCESS!=ptin_xlate_ingress_get(intIfNum, pi[i].bd.vid, PTIN_XLATE_NOT_DEFINED, &vidInternal, L7_NULLPTR)) 
       PT_LOG_DEBUG(LOG_CTX_MSG, "Insucess ingress get");
     else {
         pi[i].bd.vid=vidInternal;
         PT_LOG_DEBUG(LOG_CTX_MSG, "Sucess ingress get");
         if (L7_SUCCESS!=dtlPtinMEPControl(intIfNum, &hm))
           PT_LOG_DEBUG(LOG_CTX_MSG, "Insucess MEP CONTROL");
         else {
           PT_LOG_DEBUG(LOG_CTX_MSG, "Sucess MEP CONTROL");
         }
     }
    }
#endif
    PT_LOG_DEBUG(LOG_CTX_MSG, "i_MEP#%llu\tporta=%lu\tvid=%llu\tlevel=%lu", pi[i].index, porta, pi[i].bd.vid, pi[i].bd.level);
    break;
  case 2:    r=ERROR_CODE_FULLTABLE;    break;
  case 3:    r=  CCMSG_FLUSH_MEP==inbuff->msgId?   S_OK:   ERROR_CODE_FULLTABLE; break;
  case 4:    r=ERROR_CODE_NOTPRESENT;  break;
  default:   r=ERROR_CODE_INVALIDPARAM; break;
  }//switch


  if (r==S_OK)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Sucess");
    return L7_SUCCESS;
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Insucess");
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
  po[i].index=pi[i].index;

#if MNGMT_DIFFERENT_ENDIANNESS
  {
   pi[i].index = ENDIAN_SWAP64(pi[i].index);
   pi[i].bd.vid = ENDIAN_SWAP64(pi[i].bd.vid);
   pi[i].bd.mep_id = ENDIAN_SWAP16(pi[i].bd.mep_id);
   pi[i].bd.prt = ENDIAN_SWAP16(pi[i].bd.prt);
  }
#endif

  i_mep=pi[i].index;

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
#ifdef USING_SDK_OAM_FP_CREATE
    {
     L7_uint32 intIfNum;
     hapi_mep_t hm;

     hm.imep=   i_mep;
     hm.m=      (T_MEP_HDR*)&oam.db[i_mep].mep;

     if (L7_SUCCESS!=ptin_intf_port2intIfNum(prt, &intIfNum));
     else
     if (L7_SUCCESS!=dtlPtinMEPControl(intIfNum, &hm));
    }
#endif
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

#if MNGMT_DIFFERENT_ENDIANNESS
  {
   pi[i].index = ENDIAN_SWAP64(pi[i].index);
   pi[i].bd.vid = ENDIAN_SWAP64(pi[i].bd.vid);
   pi[i].bd.mep_id = ENDIAN_SWAP16(pi[i].bd.mep_id);
   pi[i].bd.prt = ENDIAN_SWAP16(pi[i].bd.prt);
  }
#endif

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
    ethsrv_oam_register_connection_loss((u8*)&pi[i].bd.meg_id, p_oam->db[i_mep].mep.mep_id, pi[i].bd.mep_id, pi[i].bd.prt, pi[i].bd.vid);
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

#if MNGMT_DIFFERENT_ENDIANNESS
  {
   pi[i].index = ENDIAN_SWAP64(pi[i].index);
   pi[i].bd.vid = ENDIAN_SWAP64(pi[i].bd.vid);
   pi[i].bd.mep_id = ENDIAN_SWAP16(pi[i].bd.mep_id);
   pi[i].bd.prt = ENDIAN_SWAP16(pi[i].bd.prt);
  }
#endif

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

#if MNGMT_DIFFERENT_ENDIANNESS
  pi->index = ENDIAN_SWAP64(pi->index);
#endif

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
      po[n].bd=  *((T_MEP_HDR *) &p_oam->db[i].mep);

#if MNGMT_DIFFERENT_ENDIANNESS
      {
       po[n].index = ENDIAN_SWAP64(po[n].index);
       po[n].err_code = ENDIAN_SWAP32(po[n].err_code);
       po[n].bd.vid = ENDIAN_SWAP64(po[n].bd.vid);
       po[n].bd.mep_id = ENDIAN_SWAP16(po[n].bd.mep_id);
       po[n].bd.prt = ENDIAN_SWAP16(po[n].bd.prt);

       po[n].bd.CCM_timer = ENDIAN_SWAP32(po[n].bd.CCM_timer);
       po[n].bd.mismerge_timer = ENDIAN_SWAP32(po[n].bd.mismerge_timer);
       po[n].bd.unxp_MEP_timer = ENDIAN_SWAP32(po[n].bd.unxp_MEP_timer);
       po[n].bd.unxp_lvl_timer = ENDIAN_SWAP32(po[n].bd.unxp_lvl_timer);
       po[n].bd.unxp_T_timer = ENDIAN_SWAP32(po[n].bd.unxp_T_timer);
       //{
       // L7_uint32 j,k,l;
       //
       // for (j=0; j<2; j++)
       //     for (k=0; k<2; k++)
       //         for (l=0; l<2; l++) po[n].bd.c[j][k][l] = ENDIAN_SWAP32(po[n].bd.c[j][k][l]);
       //}
      }
#endif

      n++;

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

#if MNGMT_DIFFERENT_ENDIANNESS
  pi->index = ENDIAN_SWAP64(pi->index);
#endif

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

#if MNGMT_DIFFERENT_ENDIANNESS
      {
       po[n].index = ENDIAN_SWAP64(po[n].index);
       po[n].err_code = ENDIAN_SWAP32(po[n].err_code);
       po[n].bd.me.mep_id = ENDIAN_SWAP16(po[n].bd.me.mep_id);
       po[n].bd.me.LOC_timer = ENDIAN_SWAP32(po[n].bd.me.LOC_timer);
      }
#endif

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

#if MNGMT_DIFFERENT_ENDIANNESS
  pi->index = ENDIAN_SWAP64(pi->index);
#endif

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
      po[n].bd= p_oam->mep_lut[i];

#if MNGMT_DIFFERENT_ENDIANNESS
      {
       po[n].index = ENDIAN_SWAP64(po[n].index);
       po[n].err_code = ENDIAN_SWAP32(po[n].err_code);

       po[n].bd.mep_index = ENDIAN_SWAP32(po[n].bd.mep_index);
       po[n].bd.mep_id = ENDIAN_SWAP16(po[n].bd.mep_id);
       po[n].bd.prt = ENDIAN_SWAP16(po[n].bd.prt);
       po[n].bd.vid = ENDIAN_SWAP64(po[n].bd.vid);
      }
#endif

      n++;
    }

    if (n+1 > 15   ||  (n+1)*sizeof(msg_bd_lut_mep_t) >= IPCLIB_MAX_MSGSIZE) break;// if (n+1 > 100) break;// if ((n+1)*sizeof(msg_bd_lut_mep_t) >= INFO_DIM_MAX) break;
  }//for


  outbuff->infoDim = n*sizeof(msg_bd_lut_mep_t);

  return L7_SUCCESS;

}//msg_dump_LUT_MEPs


L7_RC_t ptin_send_lmm(u8* sMAC , u8* dMAC, L7_uint32 intIfNum, u8 endpoint)
{
    ptin_send_lmm_t entry;

    memcpy(&entry.sMAC, (u8*) sMAC, 6);
    memcpy(&entry.dMAC, (u8*) dMAC, 6);
    memcpy(&entry.endpoint, &endpoint, sizeof(entry.endpoint));

    dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_LMM, DAPI_CMD_SET, sizeof(entry), &entry);

    return L7_SUCCESS;
}

L7_RC_t ptin_check_counters_lm(L7_uint32* lmr_TxFCb, L7_uint32 port)
{
  ptin_check_counters_lm_t entry;

  L7_int32 intIfNum; 
  ptin_intf_port2intIfNum( port, &intIfNum);
 
  dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_COUNTERS_LM, DAPI_CMD_GET, sizeof(entry), &entry);

  *lmr_TxFCb = entry.lmr_TxFCb;

  return L7_SUCCESS;
}
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
    PT_LOG_ERR(LOG_CTX_MSG, "ERPS#%u is out of range [0..%u]", ENDIAN_SWAP32(msgErpsConf->idx), MAX_PROT_PROT_ERPS-1);
    return L7_FAILURE;
  }


  /* Copy data to ptin struct */
  ptinErpsConf.ringId               = ENDIAN_SWAP32(msgErpsConf->ringId);
  ptinErpsConf.isOpenRing           = msgErpsConf->isOpenRing;

  ptinErpsConf.controlVid           = ENDIAN_SWAP16(msgErpsConf->controlVid);
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
  ptinErpsConf.guardTimer           = ENDIAN_SWAP16(msgErpsConf->guardTimer);
  ptinErpsConf.holdoffTimer         = msgErpsConf->holdoffTimer;
  ptinErpsConf.waitToRestoreTimer   = msgErpsConf->waitToRestoreTimer;

  ptinErpsConf.continualTxInterval  = ENDIAN_SWAP32(5);  // 5 seconds
  ptinErpsConf.rapidTxInterval      = 0;  // 3.33 ms

  memcpy(ptinErpsConf.vid_bmp, msgErpsConf->vid_bmp, sizeof(ptinErpsConf.vid_bmp));

  PT_LOG_DEBUG(LOG_CTX_MSG, "ERPS#%u",                    ENDIAN_SWAP32(msgErpsConf->idx));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .ringId             = %d",  ptinErpsConf.ringId);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .isOpenRing         = %d",  ptinErpsConf.isOpenRing);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .controlVid         = %d",  ENDIAN_SWAP16(ptinErpsConf.controlVid));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .megLevel           = %d",  ptinErpsConf.megLevel);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .port0.slot         = %d",  ptinErpsConf.port0.slot);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .port0.type         = %d",  ptinErpsConf.port0.type);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .port0.idx          = %d",  ptinErpsConf.port0.idx);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .port1.slot         = %d",  ptinErpsConf.port1.slot);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .port1.type         = %d",  ptinErpsConf.port1.type);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .port1.idx          = %d",  ptinErpsConf.port1.idx);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .port0Role          = %d",  ptinErpsConf.port0Role);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .port1Role          = %d",  ptinErpsConf.port1Role);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .port0CfmIdx        = %d",  ptinErpsConf.port0CfmIdx);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .port1CfmIdx        = %d",  ptinErpsConf.port1CfmIdx);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .revertive          = %d",  ptinErpsConf.revertive);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .guardTimer         = %d",  ENDIAN_SWAP16(ptinErpsConf.guardTimer));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .holdoffTimer       = %d",  ptinErpsConf.holdoffTimer);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .waitToRestoreTimer = %d",  ptinErpsConf.waitToRestoreTimer);

  if (ptin_erps_add_entry(ENDIAN_SWAP32(msgErpsConf->idx), (erpsProtParam_t *) &ptinErpsConf) != ENDIAN_SWAP32(msgErpsConf->idx))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error Creating ERPS#%u", ENDIAN_SWAP32(msgErpsConf->idx));
    return L7_FAILURE;
  } 

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

  ENDIAN_SWAP32_MOD(msgErpsConf->idx);

  /* Validate ERPS# range (idx [0..MAX_PROT_PROT_ERPS[) */
  if (msgErpsConf->idx >= MAX_PROT_PROT_ERPS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ERPS#%u is out of range [0..%u]", msgErpsConf->idx, MAX_PROT_PROT_ERPS-1);
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "ERPS#%u", msgErpsConf->idx);

  if (ptin_erps_remove_entry(msgErpsConf->idx) != msgErpsConf->idx)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error Removing ERPS#%u", msgErpsConf->idx);
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
    PT_LOG_ERR(LOG_CTX_MSG, "ERPS#%u is out of range [0..%u]", msgErpsConf->idx, MAX_PROT_PROT_ERPS-1);
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "ERPS#%u",                     ENDIAN_SWAP32(msgErpsConf->idx));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .mask               = 0x%x", ENDIAN_SWAP32(msgErpsConf->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .isOpenRing         = %d",   msgErpsConf->isOpenRing);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .port0CfmIdx        = %d",   msgErpsConf->port0CfmIdx);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .port1CfmIdx        = %d",   msgErpsConf->port1CfmIdx);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .revertive          = %d",   msgErpsConf->revertive);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .guardTimer         = %d",   ENDIAN_SWAP16(msgErpsConf->guardTimer));
  PT_LOG_DEBUG(LOG_CTX_MSG, " .holdoffTimer       = %d",   msgErpsConf->holdoffTimer);
  PT_LOG_DEBUG(LOG_CTX_MSG, " .waitToRestoreTimer = %d",   msgErpsConf->waitToRestoreTimer);

  /* Copy data to ptin struct */

  if (ENDIAN_SWAP32(msgErpsConf->mask) & ERPS_CONF_MASK_BIT_ISOPENRING)    ptinErpsConf.isOpenRing         = msgErpsConf->isOpenRing;

  if (ENDIAN_SWAP32(msgErpsConf->mask) & ERPS_CONF_MASK_BIT_PORT0CFMIDX)   ptinErpsConf.port0CfmIdx        = msgErpsConf->port0CfmIdx;
  if (ENDIAN_SWAP32(msgErpsConf->mask) & ERPS_CONF_MASK_BIT_PORT1CFMIDX)   ptinErpsConf.port1CfmIdx        = msgErpsConf->port1CfmIdx;

  if (ENDIAN_SWAP32(msgErpsConf->mask) & ERPS_CONF_MASK_BIT_REVERTIVE)     ptinErpsConf.revertive          = msgErpsConf->revertive;
  if (ENDIAN_SWAP16(msgErpsConf->mask) & ERPS_CONF_MASK_BIT_GUARDTIMER)    ptinErpsConf.guardTimer         = ENDIAN_SWAP16(msgErpsConf->guardTimer);
  if (ENDIAN_SWAP32(msgErpsConf->mask) & ERPS_CONF_MASK_BIT_HOLDOFFTIMER)  ptinErpsConf.holdoffTimer       = msgErpsConf->holdoffTimer;
  if (ENDIAN_SWAP32(msgErpsConf->mask) & ERPS_CONF_MASK_BIT_WAITTORESTORE) ptinErpsConf.waitToRestoreTimer = msgErpsConf->waitToRestoreTimer;

  if (ENDIAN_SWAP32(msgErpsConf->mask) & ERPS_CONF_MASK_BIT_VIDBMP)        memcpy(ptinErpsConf.vid_bmp, msgErpsConf->vid_bmp, sizeof(ptinErpsConf.vid_bmp));

  if (ptin_erps_conf_entry(ENDIAN_SWAP32(msgErpsConf->idx), ENDIAN_SWAP32(msgErpsConf->mask), (erpsProtParam_t *) &ptinErpsConf) != ENDIAN_SWAP32(msgErpsConf->idx))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error creating/reconfiguring ERPS#%u", ENDIAN_SWAP32(msgErpsConf->idx));
    return L7_FAILURE;
  }  

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
  if (ENDIAN_SWAP32(msgErpsStatus->idx) >= MAX_PROT_PROT_ERPS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ERPS#%u is out of range [0..%u]", ENDIAN_SWAP32(msgErpsStatus->idx), MAX_PROT_PROT_ERPS-1);
    return L7_FAILURE;
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "ERPS#%u", ENDIAN_SWAP32(msgErpsStatus->idx));

  if (ptin_erps_get_status(ENDIAN_SWAP32(msgErpsStatus->idx), &status) != ENDIAN_SWAP16(msgErpsStatus->idx))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error Retrieving Status ERPS#%u", ENDIAN_SWAP16(msgErpsStatus->idx));
    return L7_FAILURE;
  }

  //msgErpsStatus->slotId             = msgErpsStatus->slotId;
  //msgErpsStatus->idx                = msgErpsStatus->idx;
  msgErpsStatus->port0_SF           = status.port0_SF;
  msgErpsStatus->port1_SF           = status.port1_SF;
  msgErpsStatus->port0State         = status.port0State;
  msgErpsStatus->port1State         = status.port1State;

  msgErpsStatus->apsReqStatusTx     = ENDIAN_SWAP16(status.apsReqStatusTx);
  msgErpsStatus->apsReqStatusRxP0   = ENDIAN_SWAP16(status.apsReqStatusRxP0);
  msgErpsStatus->apsReqStatusRxP1   = ENDIAN_SWAP16(status.apsReqStatusRxP1);

  memcpy(msgErpsStatus->apsNodeIdRxP0, status.apsNodeIdRxP0, PROT_ERPS_MAC_SIZE);
  memcpy(msgErpsStatus->apsNodeIdRxP1, status.apsNodeIdRxP1, PROT_ERPS_MAC_SIZE);

  msgErpsStatus->state_machine      = status.state_machine;
  msgErpsStatus->dnfStatus          = status.dnfStatus;

  msgErpsStatus->guard_timer        = ENDIAN_SWAP16(status.guard_timer);
  msgErpsStatus->wtr_timer          = ENDIAN_SWAP32(status.wtr_timer);
  msgErpsStatus->wtb_timer          = ENDIAN_SWAP32(status.wtb_timer);
  msgErpsStatus->holdoff_timer      = ENDIAN_SWAP16(status.holdoff_timer);

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

  nextIdx = ENDIAN_SWAP32(msgErpsStatus->idx) + 1;
  PT_LOG_DEBUG(LOG_CTX_MSG, "ERPS Next Index %d", nextIdx);

  while ( i < CCMSG_ERPS_STATUS_PAGESIZE )
  {

    /* Validate ERPS# range (idx [0..MAX_PROT_PROT_ERPS[) */
    if (nextIdx >= MAX_PROT_PROT_ERPS)
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "ERPS#%u is out of range [0..%u]", nextIdx, MAX_PROT_PROT_ERPS-1);
      break;
    }

    if (ptin_erps_get_status(nextIdx, &status) != nextIdx)
    {
      nextIdx++;
      continue;
    }

    PT_LOG_DEBUG(LOG_CTX_MSG, "ERPS#%d status retrieved", nextIdx);

    msgErpsStatus[i].slotId             = slotId;
    msgErpsStatus[i].idx                = ENDIAN_SWAP32(nextIdx);
    msgErpsStatus[i].port0_SF           = status.port0_SF;
    msgErpsStatus[i].port1_SF           = status.port1_SF;
    msgErpsStatus[i].port0State         = status.port0State;
    msgErpsStatus[i].port1State         = status.port1State;

    msgErpsStatus[i].apsReqStatusTx     = ENDIAN_SWAP16(status.apsReqStatusTx);
    msgErpsStatus[i].apsReqStatusRxP0   = ENDIAN_SWAP16(status.apsReqStatusRxP0);
    msgErpsStatus[i].apsReqStatusRxP1   = ENDIAN_SWAP16(status.apsReqStatusRxP1);

    memcpy(msgErpsStatus[i].apsNodeIdRxP0, status.apsNodeIdRxP0, PROT_ERPS_MAC_SIZE);
    memcpy(msgErpsStatus[i].apsNodeIdRxP1, status.apsNodeIdRxP1, PROT_ERPS_MAC_SIZE);

    msgErpsStatus[i].state_machine      = status.state_machine;
    msgErpsStatus[i].dnfStatus          = status.dnfStatus;

    msgErpsStatus[i].guard_timer        = ENDIAN_SWAP16(status.guard_timer);
    msgErpsStatus[i].wtr_timer          = ENDIAN_SWAP32(status.wtr_timer);
    msgErpsStatus[i].wtb_timer          = ENDIAN_SWAP32(status.wtb_timer);
    msgErpsStatus[i].holdoff_timer      = ENDIAN_SWAP16(status.holdoff_timer);

    i++;
    nextIdx++;
    *n += 1;

  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "n=%d", *n);

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

  PT_LOG_DEBUG(LOG_CTX_MSG, "ERPS#%u: CMD %d, Port %d", ENDIAN_SWAP32(msgErpsCmd->idx), msgErpsCmd->cmd, msgErpsCmd->port);

  /* Validate ERPS# range (idx [0..MAX_PROT_PROT_ERPS[) */
  if (ENDIAN_SWAP32(msgErpsCmd->idx) >= MAX_PROT_PROT_ERPS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ERPS#%u is out of range [0..%u]", ENDIAN_SWAP32(msgErpsCmd->idx), MAX_PROT_PROT_ERPS-1);
    return L7_FAILURE;
  }

  if ( (msgErpsCmd->port != 0) && (msgErpsCmd->port != 1) )
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Port %d is out of range [0,1]", msgErpsCmd->port);
    return L7_FAILURE;
  }

  switch ( msgErpsCmd->cmd )
  {
  case PROT_ERPS_OPCMD_FS:
    ret = ptin_erps_cmd_force(ENDIAN_SWAP32(msgErpsCmd->idx), msgErpsCmd->port);
    break;
  case PROT_ERPS_OPCMD_MS:
    ret = ptin_erps_cmd_manual(ENDIAN_SWAP32(msgErpsCmd->idx), msgErpsCmd->port);
    break;
  case PROT_ERPS_OPCMD_OC:
    ret = ptin_erps_cmd_clear(ENDIAN_SWAP32(msgErpsCmd->idx));
    break;
  case PROT_ERPS_OPCMD_LO:            //// The following command is for further study ///
    ret = ptin_erps_cmd_lockout(ENDIAN_SWAP32(msgErpsCmd->idx));
    break;
  case PROT_ERPS_OPCMD_ReplaceRPL:    //// The following command is for further study ///
    ret = ptin_erps_cmd_replaceRpl(ENDIAN_SWAP32(msgErpsCmd->idx), msgErpsCmd->port);
    break;
  case PROT_ERPS_OPCMD_ExeSignal:     //// The following command is for further study ///
    ret = ptin_erps_cmd_exercise(ENDIAN_SWAP32(msgErpsCmd->idx), msgErpsCmd->port);
    break;
  default:
    return L7_FAILURE;
  }

  if (ret != ENDIAN_SWAP32(msgErpsCmd->idx))
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
 * ARP ACL Rule Configuration
 * 
 * @author mruas (02/16/2015)
 * 
 * @param ptr 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_arp_acl_rule_config(msg_arp_acl_t *msgArpAcl, ACL_OPERATION_t operation)
{
  L7_RC_t rc = L7_FAILURE;

  if (msgArpAcl->aclType != ACL_TYPE_ARP)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "aclType Invalid (%d)", msgArpAcl->aclType);
    return L7_FAILURE;
  }

  if (operation == ACL_OPERATION_CREATE && msgArpAcl->action != ACL_ACTION_PERMIT)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "action Invalid (%d) for rule creation", msgArpAcl->action);
    return L7_FAILURE;
  }
 
  
  msgArpAcl->srcIpAddr.addr.ipv4 = ( ((ENDIAN_SWAP32(msgArpAcl->srcIpAddr.addr.ipv4) >>24) & 0xff) | ((ENDIAN_SWAP32(msgArpAcl->srcIpAddr.addr.ipv4) >>16) & 0xff) |
                                   ((ENDIAN_SWAP32(msgArpAcl->srcIpAddr.addr.ipv4) >>8) & 0xff)  | ((ENDIAN_SWAP32(msgArpAcl->srcIpAddr.addr.ipv4)) & 0xff));
                  
   
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");
  PT_LOG_DEBUG(LOG_CTX_MSG, "Slot Id        %d",                              msgArpAcl->slotId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Type       %s",                              "ARP");
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Name       %s",                              msgArpAcl->name);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Action         %s",                              "PERMIT");
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");
  PT_LOG_DEBUG(LOG_CTX_MSG, "Src Mac Addr   %02x:%02x:%02x:%02x:%02x:%02x",   msgArpAcl->srcMacAddr[0], 
                                                                              msgArpAcl->srcMacAddr[1],
                                                                              msgArpAcl->srcMacAddr[2],
                                                                              msgArpAcl->srcMacAddr[3],
                                                                              msgArpAcl->srcMacAddr[4],
                                                                              msgArpAcl->srcMacAddr[5]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "SrcIPAddr      %03u.%03u.%03u.%03u (family=%u)", (msgArpAcl->srcIpAddr.addr.ipv4 >>24) & 0xff,
                                                                              (msgArpAcl->srcIpAddr.addr.ipv4 >>16) & 0xff,
                                                                              (msgArpAcl->srcIpAddr.addr.ipv4 >>8) & 0xff,
                                                                              (msgArpAcl->srcIpAddr.addr.ipv4) & 0xff,
                                                                               msgArpAcl->srcIpAddr.family);
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");

  rc = ptin_aclArpRuleConfig(msgArpAcl, operation);

  return rc;
}


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
  L7_uint8  *actionStr[] =  {"DENY", "PERMIT", "CAPTURE"};
  L7_uint8  *operationStr[] =  {"CREATE RULE", "REMOVE RULE"};

  L7_RC_t rc = L7_FAILURE;

  if (msgMacAcl->aclType != ACL_TYPE_MAC)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "aclType Invalid (%d)", msgMacAcl->aclType);
    return L7_FAILURE;
  }

  if (ENDIAN_SWAP16(msgMacAcl->aclId) >= L7_MAX_ACL_LISTS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid ACL ID (%d)", msgMacAcl->aclId);
    return L7_FAILURE;
  }

  if (msgMacAcl->aclRuleId > L7_MAX_NUM_RULES_PER_ACL)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "aclRuleId Invalid (%d)", msgMacAcl->aclRuleId);
    return L7_FAILURE;
  }

  if (msgMacAcl->action >= ACL_ACTION_MAX)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "action Invalid (%d)", msgMacAcl->action);
    return L7_FAILURE;
  }
  
  if (force_capture)
  {
    msgMacAcl->action = ACL_ACTION_CAPTURE;
    PT_LOG_DEBUG(LOG_CTX_MSG, "Packet capture will be configured for this rule!");
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");
  PT_LOG_DEBUG(LOG_CTX_MSG, "Slot Id        %d",                              msgMacAcl->slotId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Type       %s",                              aclTypeStr[msgMacAcl->aclType]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Id         %d",                              ENDIAN_SWAP16(msgMacAcl->aclId));
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Name       %s",                              msgMacAcl->name);
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Rule Id    %d",                              msgMacAcl->aclRuleId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Action         %s",                              actionStr[msgMacAcl->action]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Rule Mask  0x%x",                            ENDIAN_SWAP32(msgMacAcl->aclRuleMask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "Src Mac Addr   %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",   msgMacAcl->srcMacAddr[0], 
                                                                                msgMacAcl->srcMacAddr[1],
                                                                                msgMacAcl->srcMacAddr[2],
                                                                                msgMacAcl->srcMacAddr[3],
                                                                                msgMacAcl->srcMacAddr[4],
                                                                                msgMacAcl->srcMacAddr[5]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Src Mac Mask   %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",   msgMacAcl->srcMacMask[0], 
                                                                                msgMacAcl->srcMacMask[1],
                                                                                msgMacAcl->srcMacMask[2],
                                                                                msgMacAcl->srcMacMask[3],
                                                                                msgMacAcl->srcMacMask[4],
                                                                                msgMacAcl->srcMacMask[5]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Dst Mac Addr   %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",   msgMacAcl->dstMacAddr[0], 
                                                                                msgMacAcl->dstMacAddr[1],
                                                                                msgMacAcl->dstMacAddr[2],
                                                                                msgMacAcl->dstMacAddr[3],
                                                                                msgMacAcl->dstMacAddr[4],
                                                                                msgMacAcl->dstMacAddr[5]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Dst Mac Mask   %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",   msgMacAcl->dstMacMask[0], 
                                                                                msgMacAcl->dstMacMask[1],
                                                                                msgMacAcl->dstMacMask[2],
                                                                                msgMacAcl->dstMacMask[3],
                                                                                msgMacAcl->dstMacMask[4],
                                                                                msgMacAcl->dstMacMask[5]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "EtherType      0x%.4x",                          ENDIAN_SWAP16(msgMacAcl->eType));

  if (ENDIAN_SWAP16(msgMacAcl->startVlan) == ENDIAN_SWAP16(msgMacAcl->endVlan))
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Vlan           %d",                            ENDIAN_SWAP16(msgMacAcl->startVlan));
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Vlan Range     %d-%d",                         ENDIAN_SWAP16(msgMacAcl->startVlan), ENDIAN_SWAP16(msgMacAcl->startVlan));
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "COS            %d",                              msgMacAcl->cosVal);
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");
  PT_LOG_DEBUG(LOG_CTX_MSG, "Operation      %s",                              operationStr[operation]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");

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
  L7_uint8  *aclTypeStr[] = {"MAC", "IP STANDARD", "IP EXTENDED", "IP NAMED", "IPv6 EXTENDED"};
  L7_uint8  *actionStr[] =  {"DENY", "PERMIT", "CAPTURE"};
  L7_uint8  *operationStr[] =  {"CREATE RULE", "REMOVE RULE"};
  L7_uint8 ipAddr[] = "255.255.255.255";

  L7_RC_t rc = L7_FAILURE;

  if ( (msgIpAcl->aclType != ACL_TYPE_IP_STANDARD) && (msgIpAcl->aclType != ACL_TYPE_IP_EXTENDED) && (msgIpAcl->aclType != ACL_TYPE_IP_NAMED) )
  {
    PT_LOG_ERR(LOG_CTX_MSG, "aclType Invalid (%d)", msgIpAcl->aclType);
    return L7_FAILURE;
  }

  if (ENDIAN_SWAP16(msgIpAcl->srcStartPort) != 0 && ENDIAN_SWAP16(msgIpAcl->srcEndPort) == 0)
  {
    msgIpAcl->srcEndPort = msgIpAcl->srcStartPort ;
  }

  if (ENDIAN_SWAP16(msgIpAcl->dstStartPort) != 0 && ENDIAN_SWAP16(msgIpAcl->dstEndPort) == 0)
  {
    msgIpAcl->dstEndPort = msgIpAcl->dstStartPort ;
  }

  if (msgIpAcl->aclType == ACL_TYPE_IP_STANDARD)
  {
    if ( (ENDIAN_SWAP16(msgIpAcl->aclId) == 0) || (ENDIAN_SWAP16(msgIpAcl->aclId) > 99) ) /* [1..99] */
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Invalid ACL ID (%d)", ENDIAN_SWAP16(msgIpAcl->aclId));
      return L7_FAILURE;
    }
  }
  else if ( (msgIpAcl->aclType == ACL_TYPE_IP_EXTENDED) || (msgIpAcl->aclType == ACL_TYPE_IP_NAMED) )
  {
    if ( (msgIpAcl->aclId < 100) || (msgIpAcl->aclId > 199) ) /* [100..199] */
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Invalid ACL ID (%d)", ENDIAN_SWAP16(msgIpAcl->aclId));
      return L7_FAILURE;
    }
  }

  if (msgIpAcl->aclRuleId > L7_MAX_NUM_RULES_PER_ACL)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "aclRuleId Invalid (%d)", msgIpAcl->aclRuleId);
    return L7_FAILURE;
  }

  if (msgIpAcl->action >= ACL_ACTION_MAX)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "action Invalid (%d)", msgIpAcl->action);
    return L7_FAILURE;
  }

  if (force_capture)
  {
    msgIpAcl->action = ACL_ACTION_CAPTURE;
    PT_LOG_DEBUG(LOG_CTX_MSG, "Packet capture will be configured for this rule!");
  }
  
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");
  PT_LOG_DEBUG(LOG_CTX_MSG, "Slot Id        %d",                              msgIpAcl->slotId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Type       %s",                              aclTypeStr[msgIpAcl->aclType]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Id         %d",                              ENDIAN_SWAP16(msgIpAcl->aclId));
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Name       %s",                              msgIpAcl->name);
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Rule Id    %d",                              msgIpAcl->aclRuleId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Action         %s",                              actionStr[msgIpAcl->action]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Rule Mask  0x%x",                            ENDIAN_SWAP32(msgIpAcl->aclRuleMask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "Protocol       %d",                              msgIpAcl->protocol);

  usmDbInetNtoa(ENDIAN_SWAP32(msgIpAcl->srcIpAddr),  ipAddr);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Src IP Addr    %s",                              ipAddr);

  usmDbInetNtoa(ENDIAN_SWAP32(msgIpAcl->srcIpMask),  ipAddr);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Src IP Mask    %s",                              ipAddr);
  
  usmDbInetNtoa(ENDIAN_SWAP32(msgIpAcl->dstIpAddr),  ipAddr);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Dst IP Addr    %s",                              ipAddr);
  
  usmDbInetNtoa(ENDIAN_SWAP32(msgIpAcl->dstIpMask),  ipAddr);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Dst IP Mask    %s",                              ipAddr);  

  if (ENDIAN_SWAP16(msgIpAcl->srcStartPort) == ENDIAN_SWAP16(msgIpAcl->srcEndPort))
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Src L4 Port    %d",                            ENDIAN_SWAP16(msgIpAcl->srcStartPort));
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Src L4 Port Range   %d-%d",                    ENDIAN_SWAP16(msgIpAcl->srcStartPort), ENDIAN_SWAP16(msgIpAcl->srcEndPort));
  }

  if (ENDIAN_SWAP16(msgIpAcl->dstStartPort) == ENDIAN_SWAP16(msgIpAcl->dstEndPort))
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Dst L4 Port    %d",                            ENDIAN_SWAP16(msgIpAcl->dstStartPort));
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Dst L4 Port Range   %d-%d",                    ENDIAN_SWAP16(msgIpAcl->dstStartPort), ENDIAN_SWAP16(msgIpAcl->dstEndPort));
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "TOS            0x%.2x",                          msgIpAcl->tosVal);
  PT_LOG_DEBUG(LOG_CTX_MSG, "TOS Mask       0x%.2x",                          msgIpAcl->tosMask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "DSCP           %d",                              msgIpAcl->dscpVal);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Prec           %d",                              msgIpAcl->precVal);
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");
  PT_LOG_DEBUG(LOG_CTX_MSG, "Operation      %s",                              operationStr[operation]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");

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
    PT_LOG_ERR(LOG_CTX_MSG, "aclType Invalid (%d)", msgIpv6Acl->aclType);
    return L7_FAILURE;
  }

  if (ENDIAN_SWAP16(msgIpv6Acl->aclId) > L7_MAX_ACL_LISTS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid ACL ID (%d)", ENDIAN_SWAP16(msgIpv6Acl->aclId));
    return L7_FAILURE;
  }

  if (msgIpv6Acl->aclRuleId > L7_MAX_NUM_RULES_PER_ACL)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "aclRuleId Invalid (%d)", msgIpv6Acl->aclRuleId);
    return L7_FAILURE;
  }

  if (msgIpv6Acl->action >= ACL_ACTION_MAX)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "action Invalid (%d)", msgIpv6Acl->action);
    return L7_FAILURE;
  }

  if (force_capture)
  {
    msgIpv6Acl->action = ACL_ACTION_CAPTURE;
    PT_LOG_DEBUG(LOG_CTX_MSG, "Packet capture will be configured for this rule!");
  }
  
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");
  PT_LOG_DEBUG(LOG_CTX_MSG, "Slot Id        %d",                              msgIpv6Acl->slotId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Type       %s",                              aclTypeStr[msgIpv6Acl->aclType]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Id         %d",                              ENDIAN_SWAP16(msgIpv6Acl->aclId));
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Name       %s",                              msgIpv6Acl->name);
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Rule Id    %d",                              msgIpv6Acl->aclRuleId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Action         %s",                              actionStr[msgIpv6Acl->action]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Rule Mask  0x%x",                            ENDIAN_SWAP32(msgIpv6Acl->aclRuleMask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "Protocol       %d",                              msgIpv6Acl->protocol);

  if (osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)msgIpv6Acl->src6Addr, ipAddr, sizeof(ipAddr)) != L7_NULLPTR)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Src IP Addr    %s/%d",                         ipAddr, ENDIAN_SWAP32(msgIpv6Acl->src6PrefixLen));
  }

  if (osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)msgIpv6Acl->dst6Addr, ipAddr, sizeof(ipAddr)) != L7_NULLPTR)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Dst IP Addr    %s/%d",                         ipAddr, ENDIAN_SWAP32(msgIpv6Acl->dst6PrefixLen));
  }

  if (ENDIAN_SWAP16(msgIpv6Acl->srcStartPort) == ENDIAN_SWAP16(msgIpv6Acl->srcEndPort))
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Src L4 Port    %d",                            ENDIAN_SWAP16(msgIpv6Acl->srcStartPort));
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Src L4 Port Range   %d-%d",                    ENDIAN_SWAP16(msgIpv6Acl->srcStartPort), ENDIAN_SWAP16(msgIpv6Acl->srcEndPort));
  }

  if (ENDIAN_SWAP16(msgIpv6Acl->dstStartPort) == ENDIAN_SWAP16(msgIpv6Acl->dstEndPort))
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Dst L4 Port    %d",                            ENDIAN_SWAP16(msgIpv6Acl->dstStartPort));
  }
  else
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Dst L4 Port Range   %d-%d",                    ENDIAN_SWAP16(msgIpv6Acl->dstStartPort), ENDIAN_SWAP16(msgIpv6Acl->dstEndPort));
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "DSCP           %d",                              msgIpv6Acl->dscpVal);
  PT_LOG_DEBUG(LOG_CTX_MSG, "Flow Label     %d",                              ENDIAN_SWAP32(msgIpv6Acl->flowLabelVal));
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");
  PT_LOG_DEBUG(LOG_CTX_MSG, "Operation      %s",                              operationStr[operation]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");

  rc = ptin_aclIpv6RuleConfig(msgIpv6Acl, operation);

  return rc;
}


/**
 * ACL Rule Configuration
 * 
 * @author joaom (11/01/2013)
 * 
 * @param msgAcl : Pointer to beginning of data
 * @param msgId : operation 
 * @param msgDim : Dimension of data 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_acl_rule_config(void *msgAcl, L7_uint msgId, L7_uint msgDim)
{
  L7_uint8        *msg;
  ACL_OPERATION_t operation = ACL_OPERATION_REMOVE;
  L7_RC_t         rc, rc_global = L7_SUCCESS;

  /* Type of operation */
  if (msgId == CCMSG_ACL_RULE_ADD) 
  {
    operation = ACL_OPERATION_CREATE;
  }
  else if (msgId == CCMSG_ACL_RULE_DEL)
  {
    operation = ACL_OPERATION_REMOVE;
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid msgId: %u", msgId);
    return L7_FAILURE;
  }

  /* msg pointer starts pointing to the beginning of array */
  msg = (L7_uint8 *) msgAcl;  

  /* Run all entries, byte to byte */
  while (((L7_uint8 *) msg) < ((L7_uint8 *) msgAcl + msgDim))
  {
    rc = L7_SUCCESS;

    /* Type of entry */
    if (msg[1] == ACL_TYPE_MAC)
    {
      rc = ptin_msg_mac_acl_rule_config((msg_mac_acl_t*) msg, operation);
      msg += sizeof(msg_mac_acl_t);
    }
    else if ( (msg[1] == ACL_TYPE_IP_STANDARD) || (msg[1] == ACL_TYPE_IP_EXTENDED) || (msg[1] == ACL_TYPE_IP_NAMED) )
    {
      rc = ptin_msg_ip_acl_rule_config((msg_ip_acl_t*) msg, operation);
      msg += sizeof(msg_ip_acl_t);
    }
    else if (msg[1] == ACL_TYPE_IPv6_EXTENDED)
    {
      rc = ptin_msg_ipv6_acl_rule_config((msg_ipv6_acl_t*) msg, operation);
      msg += sizeof(msg_ipv6_acl_t);
    }
    else if (msg[1] == ACL_TYPE_ARP)
    {
      rc = ptin_msg_arp_acl_rule_config((msg_arp_acl_t*) msg, operation);
      msg += sizeof(msg_arp_acl_t);
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Invalid struct type: %u", msg[1]);
      return L7_FAILURE;
    }
    /* Update global result */
    if (rc != L7_SUCCESS)
    {
      rc_global = rc;
    }

    /* msg pointer was updated to the next entry */
  }

  /* Check if pointer is not out of position */
  if (msg != ((L7_uint8 *) msgAcl + msgDim))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Message pointer is out of the expected place... returning error");
    return L7_FAILURE;
  }

  return rc_global;
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
  ptin_acl_apply_t aclApply;
  L7_uint8  *aclTypeStr[] = {"MAC", "IP STANDARD", "IP EXTENDED", "IP NAMED", "IPv6 EXTENDED", "ARP"};
  L7_uint8  *directionStr[] =  {"IN", "OUT"};
  L7_uint8  *operationStr[] =  {"APPLY ACL", "UNAPPLY ACL"};
  L7_RC_t   rc = L7_SUCCESS;

  if (msgAcl->aclType >= ACL_TYPE_MAX)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "aclType Invalid (%d)", msgAcl->aclType);
    return L7_FAILURE;
  }

  if (msgAcl->direction >= ACL_DIRECTION_MAX)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "direction Invalid (%d)", msgAcl->direction);
    return L7_FAILURE;
  }
  
  ENDIAN_SWAP16_MOD(msgAcl->aclId);
  ENDIAN_SWAP32_MOD(msgAcl->interface);
  ENDIAN_SWAP16_MOD(msgAcl->vlanId);
  ENDIAN_SWAP32_MOD(msgAcl->evcId);

  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");
  PT_LOG_DEBUG(LOG_CTX_MSG, "Slot Id        %u",                              msgAcl->slotId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Type       %s",                              aclTypeStr[msgAcl->aclType]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Id         %u",                              msgAcl->aclId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "ACL Name       %s",                              msgAcl->name);
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");
  PT_LOG_DEBUG(LOG_CTX_MSG, "interface      %u",                              msgAcl->interface);
  PT_LOG_DEBUG(LOG_CTX_MSG, "vlanId         %u",                              msgAcl->vlanId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "evcId          %u",                              msgAcl->evcId);
  PT_LOG_DEBUG(LOG_CTX_MSG, "direction      %s",                              directionStr[msgAcl->direction]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");
  PT_LOG_DEBUG(LOG_CTX_MSG, "Operation      %s",                              operationStr[operation]);
  PT_LOG_DEBUG(LOG_CTX_MSG, "-------------------------------------------");

  if ( (msgAcl->interface == L7_ACL_INVALID_IFACE_ID) &&
       (msgAcl->evcId == L7_ACL_INVALID_EVC_ID) &&
       (msgAcl->vlanId == L7_ACL_INVALID_VLAN_ID) )
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Neither interface neither evcId is valid");
    return L7_FAILURE;
  }

  /* Copy data */
  memset(&aclApply, 0x00, sizeof(aclApply));
  aclApply.aclType    = msgAcl->aclType;
  aclApply.aclId      = msgAcl->aclId;
  memcpy(aclApply.name, msgAcl->name, sizeof(L7_uint8)*32);
  aclApply.direction  = msgAcl->direction;

  aclApply.number_of_vlans  = 0;
  aclApply.vlanId[0]        = L7_ACL_INVALID_VLAN_ID;
  aclApply.interface        = (L7_uint32) -1;

  if (msgAcl->vlanId > 0 && msgAcl->vlanId < 4096)
  {
    /* This is NNI VLAN. Get evc id from it, and use it to get the internal VLAN */
    aclApply.number_of_vlans = 4096;
    if (ptin_evc_get_intVlan_fromNNIvlan(msgAcl->vlanId, aclApply.vlanId, &aclApply.number_of_vlans) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Unable to get extEVCid from NNI VLAN %u", msgAcl->vlanId );
      return L7_DEPENDENCY_NOT_MET;
    }

    PT_LOG_DEBUG(LOG_CTX_MSG, "Retrieved VLAN ID %d", (L7_uint32) aclApply.vlanId[0]);
  }
  else if (msgAcl->evcId < PTIN_SYSTEM_N_EXTENDED_EVCS)
  {
    /* Gets the root vlan (internal) for a particular evc */
    aclApply.number_of_vlans = 1;
    rc = ptin_evc_intRootVlan_get(msgAcl->evcId, &aclApply.vlanId[0]);
    if (rc == L7_NOT_EXIST)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "eEVC %u does not exist", msgAcl->evcId);
      return L7_DEPENDENCY_NOT_MET;
    }
    else if (rc != L7_SUCCESS) 
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error while retrieving VLAN ID(rc=%d)", rc);
      return rc;
    }

    PT_LOG_DEBUG(LOG_CTX_MSG, "Retrieved VLAN ID %d", (L7_uint32) aclApply.vlanId[0]);
  }
  else
  {
    aclApply.interface = msgAcl->interface;

    PT_LOG_DEBUG(LOG_CTX_MSG, "Using Interface %d", aclApply.interface);
  }
  
  if (aclType == ACL_TYPE_MAC)
  {
    rc = ptin_aclMacApply(&aclApply, operation);
  }
  else if ( (aclType == ACL_TYPE_IP_STANDARD) || (aclType == ACL_TYPE_IP_EXTENDED) || (aclType == ACL_TYPE_IP_NAMED) )
  {
    rc = ptin_aclIpApply(&aclApply, operation);
  }
  else if (aclType == ACL_TYPE_IPv6_EXTENDED)
  {
    rc = ptin_aclIpv6Apply(&aclApply, operation);
  }
  else if (aclType == ACL_TYPE_ARP)
  {
    rc = ptin_aclArpApply(&aclApply, operation);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid ACL type: %d", aclType);
    return L7_FAILURE;
  }

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Error occurred: rc=%d", rc);
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "rc=%d", rc);
  return rc;
}


/**
 * ACL Enable/Disable
 * 
 * @author joaom (11/01/2013)
 * 
 * @param msgAcl : Pointer to data 
 * @param msgId : Operation 
 * @param n_msg : Number of structs
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_acl_enable(msg_apply_acl_t *msgAcl, L7_uint msgId, L7_uint n_msg)
{
  L7_uint         i;
  L7_uint8        aclType;
  ACL_OPERATION_t operation = ACL_OPERATION_REMOVE;
  L7_RC_t         rc, rc_global = L7_SUCCESS;


  msgAcl->aclId     = ENDIAN_SWAP16(msgAcl->aclId);
  msgAcl->interface = ENDIAN_SWAP32(msgAcl->interface);
  msgAcl->vlanId    = ENDIAN_SWAP16(msgAcl->vlanId);
  msgAcl->evcId     = ENDIAN_SWAP32(msgAcl->evcId);
       
  /* Operation */
  if (msgId == CCMSG_ACL_APPLY)
  {
    operation = ACL_OPERATION_CREATE;
  }
  else if (msgId == CCMSG_ACL_UNAPPLY)
  {
    operation = ACL_OPERATION_REMOVE;
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid msgId %u", msgId);
    return L7_FAILURE;
  }

  /* Run all entries */
  for (i = 0; i < n_msg; i++)
  {
    rc = L7_SUCCESS;

    aclType = msgAcl[i].aclType; 

    if (aclType == ACL_TYPE_MAC)
    {
      rc = ptin_msg_acl_apply(&msgAcl[i], operation, aclType);
    }
    else if ( (aclType == ACL_TYPE_IP_STANDARD) || (aclType == ACL_TYPE_IP_EXTENDED) || (aclType == ACL_TYPE_IP_NAMED) )
    {
      rc = ptin_msg_acl_apply(&msgAcl[i], operation, aclType);
    }
    else if (aclType == ACL_TYPE_IPv6_EXTENDED)
    {
      rc = ptin_msg_acl_apply(&msgAcl[i], operation, aclType);
    }
    else if (aclType == ACL_TYPE_ARP)
    {
      rc = ptin_msg_acl_apply(&msgAcl[i], operation, aclType);
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_MSG, "invalid entry type (i=%u): %d", i, aclType);
      rc_global = L7_FAILURE;
      continue;
    }

    /* Update final result */
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error occurred at i=%u (rc=%d)", i, rc);
      rc_global = rc;
    }
  }

  return rc_global;
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

  ptin_msg_acl_rule_config(&msgIpAcl, (operation==ACL_OPERATION_CREATE)? CCMSG_ACL_RULE_ADD : CCMSG_ACL_RULE_DEL, sizeof(msg_ip_acl_t));

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

  ptin_msg_acl_enable(&msgAcl, (operation==ACL_OPERATION_CREATE)? CCMSG_ACL_APPLY : CCMSG_ACL_UNAPPLY, 1);

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

  ptin_msg_acl_rule_config(&msgIpv6Acl, (operation==ACL_OPERATION_CREATE)? CCMSG_ACL_RULE_ADD : CCMSG_ACL_RULE_DEL, sizeof(msg_ipv6_acl_t));

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

  ptin_msg_acl_enable(&msgAcl, (operation==ACL_OPERATION_CREATE)? CCMSG_ACL_APPLY : CCMSG_ACL_UNAPPLY, 1);

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

  ptin_msg_acl_rule_config(&msgMacAcl, (operation==ACL_OPERATION_CREATE)? CCMSG_ACL_RULE_ADD : CCMSG_ACL_RULE_DEL, sizeof(msg_mac_acl_t));


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

  ptin_msg_acl_rule_config(&msgMacAcl, (operation==ACL_OPERATION_CREATE)? CCMSG_ACL_RULE_ADD : CCMSG_ACL_RULE_DEL, sizeof(msg_mac_acl_t));

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

  ptin_msg_acl_enable(&msgAcl, (operation==ACL_OPERATION_CREATE)? CCMSG_ACL_APPLY : CCMSG_ACL_UNAPPLY, 1);

  return L7_SUCCESS;
}

/* ********************************************************************* */


/**
 * Configure Session Monitor (Port Mirroring)
 * 
 * @author joaom (11/26/2015)
 * 
 * @param inbuffer 
 * @param outbuffer 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_mirror(ipc_msg *inbuffer, ipc_msg *outbuffer)
{
  static L7_uint32      listSrcPorts[L7_FILTER_MAX_INTF];
  static L7_uint32      listDstPorts[L7_FILTER_MAX_INTF];
  L7_uint32             numPorts;
  L7_uint32             sessionNum;
  L7_uint32             mode;
  L7_uint32             unit = 1;
  L7_int                ptin_port;
  L7_uint32             dstIntfNum;
  L7_uint32             srcIntfNum;
  L7_INTF_MASK_t        srcIntfMask;
  L7_MIRROR_DIRECTION_t type = L7_MIRROR_UNCONFIGURED;
  L7_RC_t               rc = L7_SUCCESS, rc_global = L7_SUCCESS;

  L7_uint8              n;
  const L7_uchar8       *dir[]={"None", "In & Out", "In", "Out"};

  msg_port_mirror_t *msg = (msg_port_mirror_t *) inbuffer->info;

  PT_LOG_DEBUG(LOG_CTX_MSG, "Mirror Configurations:");
  PT_LOG_DEBUG(LOG_CTX_MSG, " Slot Id       = %u",        msg->slotId);
  PT_LOG_DEBUG(LOG_CTX_MSG, " Session Id    = %u",        msg->sessionId);
  PT_LOG_DEBUG(LOG_CTX_MSG, " Mask          = %.8X (h)",  ENDIAN_SWAP16(msg->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, " Mode          = %u",        msg->sessionMode);
  PT_LOG_DEBUG(LOG_CTX_MSG, " Dst intfid    = %u/%u",     msg->dst_intf.intf_type, msg->dst_intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, " Src intf Num  = %u",        msg->n_intf);

  ENDIAN_SWAP16_MOD(msg->mask);

  if (msg->n_intf >= PTIN_SYSTEM_MAX_N_PORTS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Ups.. something is wrong! Too many Src interfaces");
    return L7_FAILURE;
  }

  for (n=0; n<msg->n_intf && n<PTIN_SYSTEM_MAX_N_PORTS; n++)
  {
    if (msg->src_intf[n].direction > 3)
    {
      PT_LOG_ERR(LOG_CTX_MSG, " Src intfid (Type/ID/Direction) = %u/%u/(%u?), with INVALID Direction! Ignoring this one!", msg->src_intf[n].intf.intf_type, msg->src_intf[n].intf.intf_id, msg->src_intf[n].direction);
      continue;
    }

    PT_LOG_DEBUG(LOG_CTX_MSG, " Src intfid (Type/ID/Direction) = %u/%u/%s", msg->src_intf[n].intf.intf_type, msg->src_intf[n].intf.intf_id, dir[msg->src_intf[n].direction]);
  }

  /* Validate Session ID */
  sessionNum = 1; // msg->sessionId;

  if( sessionNum > L7_MIRRORING_MAX_SESSIONS || sessionNum == 0)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid Session ID %u, sessionNum");
    return L7_FAILURE;
  }

  /* Check if Feature is supported */
  if(cnfgrIsFeaturePresent(L7_PORT_MIRROR_COMPONENT_ID, L7_MIRRORING_DIRECTION_PER_SOURCE_PORT_SUPPORTED_FEATURE_ID) != L7_TRUE)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Feature not supported");
    return L7_FAILURE;
  }

  /* Configure Probe interface */
  if (msg->mask & PORT_MIRROR_MASK_dst_intf)
  {
    /* Convert Dst intfId to intfNum */
    if (ptin_msg_ptinPort_get(msg->dst_intf.intf_type, msg->dst_intf.intf_id, &ptin_port)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Invalid port");
      return L7_FAILURE;
    }
    if (ptin_intf_port2intIfNum(ptin_port, &dstIntfNum)!=L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Non existent port");
      return L7_FAILURE;
    }

    /* check for portChannel members */
    if (usmDbDot3adIsMember(unit, dstIntfNum) == L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "This port is a portChannel member");
      return L7_FAILURE;
    }

    PT_LOG_TRACE(LOG_CTX_MSG, "Configuring Destination interface intfNum %d", dstIntfNum);

    rc = usmDbSwPortMonitorDestPortSet(unit, sessionNum, dstIntfNum);


    if(rc != L7_SUCCESS)
    {
      if (rc == L7_ALREADY_CONFIGURED)
      {
        L7_uint32 auxIntfNum;
        usmDbSwPortMonitorDestPortGet(unit, sessionNum, &auxIntfNum);

        if (auxIntfNum != dstIntfNum)
        {
          PT_LOG_TRACE(LOG_CTX_MSG, "Destination interface is already configured with a different value (current intfNum %d)", auxIntfNum);
          return L7_FAILURE;
        }
        //else
        //  Nothing to be done
      }
      else
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Some error occurred (%d)", rc); 
        return L7_FAILURE;
      }
    }    
  }

  /* Configure Source interface */
  if (msg->mask & PORT_MIRROR_MASK_src_intf)
  {
    for (n=0; n<msg->n_intf && n<PTIN_SYSTEM_MAX_N_PORTS; n++)
    {
      /* Convert Src intfId to intfNum */
      if (ptin_msg_ptinPort_get(msg->src_intf[n].intf.intf_type, msg->src_intf[n].intf.intf_id, &ptin_port)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Invalid port. Ignoring this one!");
        rc_global = L7_FAILURE;
        continue;
      }
      if (ptin_intf_port2intIfNum(ptin_port, &srcIntfNum)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Non existent port. Ignoring this one!");
        rc_global = L7_FAILURE;
        continue;
      }  

      /* check for portChannel members */
      if (usmDbDot3adIsMember(unit, srcIntfNum) == L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "This port is a portChannel member. Ignoring this one!");
        rc_global = L7_FAILURE;
        continue;
      }

      if (msg->src_intf[n].direction > 3)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "With INVALID (%u) Direction! Ignoring this port!", msg->src_intf[n].direction);
        rc_global = L7_FAILURE;
        continue;
      }

      type = msg->src_intf[n].direction;
      usmDbSwPortMonitorSourcePortsGet(unit, sessionNum, &srcIntfMask);
      usmDbConvertMaskToList(&srcIntfMask, listSrcPorts, &numPorts);
      listSrcPorts[numPorts++] = srcIntfNum;

      PT_LOG_TRACE(LOG_CTX_MSG, "Configuring Source interface intfNum %d with direction %u", dstIntfNum, type);
      
      if (type == L7_MIRROR_UNCONFIGURED)
      {
        PT_LOG_TRACE(LOG_CTX_MSG, "Removing intfNum %d", srcIntfNum);
        rc = usmDbSwPortMonitorSourcePortRemove(unit, sessionNum, srcIntfNum);      
      }
      else
      {
        L7_int32 ptin_port_aux;
        ptin_port_aux = msg->src_intf[n].intf.intf_id;

        rc = usmDbSwPortMonitorSourcePortAdd(unit, sessionNum, srcIntfNum, type);

        if (msg->src_intf[n].intf.intf_type == 1)
        {
          ptin_intf_intIfNum2port(srcIntfNum, &ptin_port_aux); 

          PT_LOG_TRACE(LOG_CTX_MSG, "Adding intfNum Src %d", ptin_port_aux);
        }

        /* Configure Egress XLATE on the destination interface */
        if (msg->src_intf[n].direction == 1 || msg->src_intf[n].direction == 3)
        {
          PT_LOG_TRACE(LOG_CTX_MSG, "Src intfNum %d", ptin_port_aux);
          PT_LOG_TRACE(LOG_CTX_MSG, "Dst intfNum %d", msg->dst_intf.intf_id);

          L7_uint32 auxIntfNum, ptin_port_dst;
          ptin_port_dst = msg->dst_intf.intf_id;

          if(msg->dst_intf.intf_id == 0)
          {         
            usmDbSwPortMonitorDestPortGet(unit, sessionNum, &auxIntfNum);
            ptin_intf_intIfNum2port(auxIntfNum, &ptin_port_dst); 
          }

          PT_LOG_TRACE(LOG_CTX_MSG, "Dst intfNum %d", msg->dst_intf.intf_id);
          xlate_outer_vlan_replicate_Dstport(1, ptin_port_aux, ptin_port_dst);
        }

      }

      if(rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Some error occurred (%d)", rc);
        rc_global = rc;
      }
    }
  }

  /* Enable/Disable this session */
  if (msg->mask & PORT_MIRROR_MASK_sessionMode)
  {
    mode = msg->sessionMode? L7_ENABLE:L7_DISABLE;
    
    PT_LOG_TRACE(LOG_CTX_MSG, "%s Mirror session", msg->sessionMode? "Enabling" : "Disabling");

    if (mode == L7_ENABLE)
    {
      /* Enable Monitor Session */
      rc = usmDbSwPortMonitorModeSet(unit, sessionNum, L7_ENABLE);
      if(rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Session do not exists");
        return L7_FAILURE;
      }
      /* Configure Egress XLATE on the destination interface */
      //if (msg->src_intf[n].direction != 3)
      //{
        //PT_LOG_ERR(LOG_CTX_MSG, "Dst intfNum %d", msg->dst_intf.intf_id);
        //xlate_outer_vlan_port(msg->sessionMode, msg->src_intf[n].intf.intf_id, msg->dst_intf.intf_id );
      //}
      // TODO
    }
    else 
    {
      L7_uint32 ptinSrc_aux, ptinDst_aux;

      /* Get the Src port(s) of the Monitor session*/
      usmDbSwPortMonitorSourcePortsGet(unit, sessionNum, &srcIntfMask);
      usmDbConvertMaskToList(&srcIntfMask, listSrcPorts, &numPorts);

      /* Convert to ptin format*/
      ptin_intf_intIfNum2port(listSrcPorts[0],&ptinSrc_aux); 

      /* Get the Dst port(s) of the Monitor session*/
      usmDbSwPortMonitorDestPortGet(unit, sessionNum, &listDstPorts[0]);

      /* Convert to ptin format*/
      ptin_intf_intIfNum2port(listDstPorts[0],&ptinDst_aux);

      // Remove egress translations
      xlate_outer_vlan_replicate_Dstport(mode, ptinSrc_aux, ptinDst_aux);

      /* Remove Monitor Session */
      rc = usmDbSwPortMonitorSessionRemove(unit, sessionNum);

      if(rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Some error occurred (%d)", rc);
        return L7_FAILURE;
      } 

      /* Disable Monitor Session */
      //rc = usmDbSwPortMonitorModeSet(unit, sessionNum, L7_DISABLE);
      //rc = usmDbSwPortMonitorSourcePortRemove(unit, sessionNum, intfNum);
      //rc = usmDbSwPortMonitorDestPortRemove(unit, sessionNum);
      
    }
  }

  PT_LOG_TRACE(LOG_CTX_MSG, "Return value is %u",rc_global);
  return rc_global;
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

  pi = (msg_802_1x_Genrc * )inbuff->info;   po = (msg_802_1x_Genrc * )ENDIAN_SWAP32(outbuff->info);

  switch (inbuff->msgId)
  {
  case CCMSG_WR_802_1X_ADMINMODE:
    r = usmDbDot1xAdminModeSet(1, ENDIAN_SWAP32(pi[i].v));
    break;
  case CCMSG_WR_802_1X_TRACE:
    r = usmDbDot1xPacketDebugTraceFlagSet(ENDIAN_SWAP32(pi[i].v) >> 1, ENDIAN_SWAP32(pi[i].v) & 1);
    break;
  case CCMSG_WR_802_1X_VLANASSGNMODE:
    r = usmDbDot1xVlanAssignmentModeSet(1, ENDIAN_SWAP32(pi[i].v));
    break;
  case CCMSG_WR_802_1X_MONMODE:
    r = usmDbDot1xMonitorModeSet(1, ENDIAN_SWAP32(pi[i].v));
    break;
  case CCMSG_WR_802_1X_DYNVLANMODE:
    r = usmDbDot1xDynamicVlanCreationModeSet(1, ENDIAN_SWAP32(pi[i].v));
    break;
  default:
    po[i].v = ENDIAN_SWAP32(SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM));
    return 1;
  }

  if (L7_SUCCESS != r)
  {
    po[i].v = ENDIAN_SWAP32(SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM));
    return 1;
  }
  else
  {
    po[i].v = ENDIAN_SWAP32(ERROR_CODE_OK);
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

  ptinp.intf_type =   ENDIAN_SWAP64(pi[i].index) >> 8;
  ptinp.intf_id =     ENDIAN_SWAP64(pi[i].index);

  if (L7_SUCCESS != ptin_intf_ptintf2intIfNum(&ptinp, &intIfNum))
  {
    po[i].err_code = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
    return 1;
  }

  switch (inbuff->msgId)
  {
  case CCMSG_WR_802_1X_ADMINCONTROLLEDDIRECTIONS:
    r = usmDbDot1xPortAdminControlledDirectionsSet(1, intIfNum, ENDIAN_SWAP64(pi[i].v));
    break;
  case CCMSG_WR_802_1X_PORTCONTROLMODE:
    r = usmDbDot1xPortControlModeSet(1, intIfNum, ENDIAN_SWAP64(pi[i].v));
    break;
  case CCMSG_WR_802_1X_QUIETPERIOD:
    r = usmDbDot1xPortQuietPeriodSet(1, intIfNum, ENDIAN_SWAP64(pi[i].v));
    break;
  case CCMSG_WR_802_1X_TXPERIOD:
    r = usmDbDot1xPortTxPeriodSet(1, intIfNum, ENDIAN_SWAP64(pi[i].v));
    break;
  case CCMSG_WR_802_1X_SUPPTIMEOUT:
    r = usmDbDot1xPortSuppTimeoutSet(1, intIfNum, ENDIAN_SWAP64(pi[i].v));
    break;
  case CCMSG_WR_802_1X_SERVERTIMEOUT:
    r = usmDbDot1xPortServerTimeoutSet(1, intIfNum, ENDIAN_SWAP64(pi[i].v));
    break;
  case CCMSG_WR_802_1X_MAXREQ:
    r = usmDbDot1xPortMaxReqSet(1, intIfNum, ENDIAN_SWAP64(pi[i].v));
    break;
  case CCMSG_WR_802_1X_REAUTHPERIOD:
    if (1 + ENDIAN_SWAP64(pi[i].v) == 0)
    {
      r = usmDbDot1xPortReAuthEnabledSet(1, intIfNum, 0);
      break;
    }  //Forbidden period disables
    r = usmDbDot1xPortReAuthPeriodSet(1, intIfNum, ENDIAN_SWAP64(pi[i].v));
    if (L7_SUCCESS != r)  break;
    r = usmDbDot1xPortReAuthEnabledSet(1, intIfNum, 1);
    break;
  case CCMSG_WR_802_1X_KEYTXENABLED:
    r = usmDbDot1xPortKeyTransmissionEnabledSet(1, intIfNum, ENDIAN_SWAP64(pi[i].v));
    break;
  case CCMSG_WR_802_1X_GUESTVLANID:
    r = usmDbDot1xAdvancedGuestPortsCfgSet(1, intIfNum, ENDIAN_SWAP64(pi[i].v));
    break;
  case CCMSG_WR_802_1X_GUSTVLANPERIOD:
    r = usmDbDot1xAdvancedPortGuestVlanPeriodSet(1, intIfNum, ENDIAN_SWAP64(pi[i].v));
    break;
  case CCMSG_WR_802_1X_MAXUSERS:
    r = usmDbDot1xPortMaxUsersSet(1, intIfNum, ENDIAN_SWAP64(pi[i].v));
    break;
  case CCMSG_WR_802_1X_UNAUTHENTICATEDVLAN:
    r = usmDbDot1xPortUnauthenticatedVlanSet(1, intIfNum, ENDIAN_SWAP64(pi[i].v));
    break;
  default:
    po[i].err_code = ENDIAN_SWAP32(SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM));
    return 1;
  }

  if (L7_SUCCESS != r)
  {
    po[i].err_code = ENDIAN_SWAP32(SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM));
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

  k = ENDIAN_SWAP64(pi[i].index); //64th bit's lost
  e = ENDIAN_SWAP64(pi[i].index) >> 63;

  if (L7_SUCCESS == usmDbDot1xAuthServUserDBUserIndexGet(pi[i].name, &index))
  {
    if (k < L7_MAX_IAS_USERS && k != index)  //Name already in table with different index
    {
      po[i].err_code = ENDIAN_SWAP32(SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_DUPLICATENAME));
      return 1;
    }
  }
  else index = -1;

  if (k < L7_MAX_IAS_USERS)
  {
    if (k != index && (L7_SUCCESS != (r = usmDbDot1xAuthServUserDBUserNameSet(k, pi[i].name))))   //index already used (or table full)
    {
      po[i].err_code = ENDIAN_SWAP32(SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_USED));
      return 1;
    }
  }
  else
  {
    if (index >= L7_MAX_IAS_USERS)
    {
      if (L7_SUCCESS != usmDbDot1xAuthServUserDBAvailableIndexGet(&index))
      {
        po[i].err_code = ENDIAN_SWAP32(SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_FULLTABLE));
        return 1;
      }
    }
    k = index;
  }

  if (L7_SUCCESS != (r = usmDbDot1xAuthServUserDBUserPasswordSet(k, pi[i].passwd, e)))
  {
    po[i].err_code = ENDIAN_SWAP32(SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM));
    return 1;
  }
  else po[i].err_code = ENDIAN_SWAP32(ERROR_CODE_OK);

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
L7_RC_t ptin_msg_routing_intf_create(msg_RoutingIntf* data)
{
  ptin_intf_t routingIntf;

  /* Debug */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Creating new routing interface:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  mask          = %08X",  ENDIAN_SWAP32(data->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  routingIntf   = %u/%u", data->routingIntf.intf_type, data->routingIntf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  evcId         = %u",    ENDIAN_SWAP32(data->evcId));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  ipAddress     = %08X",  ENDIAN_SWAP32(data->ipAddress));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  subnetMask    = %08X",  ENDIAN_SWAP32(data->subnetMask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  mtu           = %u",    ENDIAN_SWAP32(data->mtu));

  routingIntf.intf_type  = data->routingIntf.intf_type;
  routingIntf.intf_id    = data->routingIntf.intf_id;

  if(data->routingIntf.intf_type == PTIN_EVC_INTF_ROUTING)
  {
     L7_uint16 internalVlan;

     if(L7_SUCCESS != ptin_evc_intRootVlan_get(ENDIAN_SWAP32(data->evcId), &internalVlan))
     {
       PT_LOG_ERR(LOG_CTX_MSG, "Unable to convert evc_id to internal root vlan");
       return L7_FAILURE;
     }

     PT_LOG_TRACE(LOG_CTX_MSG, "Creating routing interface");
     if(L7_SUCCESS != ptin_routing_intf_create(&routingIntf, internalVlan))
     {
       PT_LOG_ERR(LOG_CTX_MSG, "Unable to create a new routing interface");
       return L7_FAILURE;
     }
  }
  else if(data->routingIntf.intf_type == PTIN_EVC_INTF_LOOPBACK)
  {
     PT_LOG_TRACE(LOG_CTX_MSG, "Creating loopback interface");
     if(L7_SUCCESS != ptin_routing_loopback_create(&routingIntf))
     {
       PT_LOG_ERR(LOG_CTX_MSG, "Unable to create a new loopback interface");
       return L7_FAILURE;
     }
  }

  PT_LOG_TRACE(LOG_CTX_MSG, "Configuring interface IP Address");
  if(L7_SUCCESS != ptin_routing_intf_ipaddress_set(&routingIntf, L7_AF_INET, ENDIAN_SWAP32(data->ipAddress), ENDIAN_SWAP32(data->subnetMask)))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unable to set interface IP address");
    return L7_FAILURE;
  }

  /* @note(Daniel): In v3.4.1, the loopback interfaces are not yet created in Linux. Hence, this command will fail. However, in future versions this MUST be fixed... */
  if(data->routingIntf.intf_type != PTIN_EVC_INTF_LOOPBACK)
  {
    PT_LOG_TRACE(LOG_CTX_MSG, "Configuring interface MTU");
    if(L7_SUCCESS != ptin_routing_intf_mtu_set(&routingIntf, ENDIAN_SWAP32(data->mtu)))
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Unable to set interface MTU");
      return L7_FAILURE;
    }
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
L7_RC_t ptin_msg_routing_intf_modify(msg_RoutingIntf* data)
{
  ptin_intf_t routingIntf;

  /* Debug */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Configuring routing interface:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  mask          = %08X",  ENDIAN_SWAP32(data->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  routingIntf   = %u/%u", data->routingIntf.intf_type, data->routingIntf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  evcId         = %u",    ENDIAN_SWAP32(data->evcId));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  ipAddress     = %08X",  ENDIAN_SWAP32(data->ipAddress));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  subnetMask    = %08X",  ENDIAN_SWAP32(data->subnetMask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  mtu           = %u",    ENDIAN_SWAP32(data->mtu));

  routingIntf.intf_type  = data->routingIntf.intf_type;
  routingIntf.intf_id    = data->routingIntf.intf_id;

  if( (ENDIAN_SWAP32(data->mask) & CCMSG_ROUTING_INTF_MASK_IPADDR) || (data->mask & CCMSG_ROUTING_INTF_MASK_SUBNETMASK) )
  {
     if(L7_SUCCESS != ptin_routing_intf_ipaddress_set(&routingIntf, L7_AF_INET, ENDIAN_SWAP32(data->ipAddress), ENDIAN_SWAP32(data->subnetMask)))
     {
       PT_LOG_ERR(LOG_CTX_MSG, "Unable to set interface IP address");
       return L7_FAILURE;
     }
  }

  if(ENDIAN_SWAP32(data->mask) & CCMSG_ROUTING_INTF_MASK_MTU)
  {
     PT_LOG_TRACE(LOG_CTX_MSG, "Configuring interface MTU");
     if(L7_SUCCESS != ptin_routing_intf_mtu_set(&routingIntf, ENDIAN_SWAP32(data->mtu)))
     {
       PT_LOG_ERR(LOG_CTX_MSG, "Unable to set interface MTU");
       return L7_FAILURE;
     }
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
L7_RC_t ptin_msg_routing_intf_remove(msg_RoutingIntf* data)
{
  ptin_intf_t routingIntf;

  /* Debug */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Removing routing interface:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  mask          = %08X",  ENDIAN_SWAP32(data->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  routingIntf   = %u/%u", data->routingIntf.intf_type, data->routingIntf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  evcId         = %u",    ENDIAN_SWAP32(data->evcId));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  ipAddress     = %08X",  ENDIAN_SWAP32(data->ipAddress));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  subnetMask    = %08X",  ENDIAN_SWAP32(data->subnetMask));

  routingIntf.intf_type = data->routingIntf.intf_type;
  routingIntf.intf_id   = data->routingIntf.intf_id;

  if(L7_SUCCESS != ptin_routing_intf_remove(&routingIntf))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unable to remove the existing routing interface");
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
    PT_LOG_ERR(LOG_CTX_MSG, "Abnormal context [inBuffer=%p outBuffer=%p readEntries=%p]", inBuffer, outBuffer, readEntries);
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Getting ARP table:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  mask       = %08X",  ENDIAN_SWAP32(inBuffer->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  intf       = %u/%u", inBuffer->intf.intf_type, inBuffer->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  lastIndex  = %u",    ENDIAN_SWAP32(inBuffer->lastIndex));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  maxEntries = %u",    ENDIAN_SWAP32(inBuffer->maxEntries));

  if(inBuffer->mask & CCMSG_ROUTING_ARPTABLE_GET_MASK_INTF)
  {
    intf.intf_type = inBuffer->intf.intf_type;
    intf.intf_id   = inBuffer->intf.intf_id;
    if(L7_SUCCESS != ptin_intf_ptintf2intIfNum(&intf, &intfNum))
    {
      PT_LOG_ERR(LOG_CTX_ROUTING, "Unable to to convert intf %u/%u to intfNum", intf.intf_type, intf.intf_id);
      return L7_FAILURE;
    }
  }
  else
  {
    intfNum = (L7_uint32)-1;
  }

  if(ENDIAN_SWAP32(inBuffer->maxEntries) & CCMSG_ROUTING_ARPTABLE_GET_MASK_MAXENTRIES)
  {
    maxEntries = min(IPCLIB_MAX_MSGSIZE/sizeof(msg_RoutingArpTableResponse), ENDIAN_SWAP32(inBuffer->maxEntries));
  }
  else
  {
    maxEntries = IPCLIB_MAX_MSGSIZE/sizeof(msg_RoutingArpTableResponse);
  }

  /*
   I know that passing a ptin_msghandler struct to a file other than ptin_msg breaks PTIN Fastpath's architecture.
   However, doing so here allows me to hide the complexity of interacting with usmDb completly inside of ptin_routing.                                                                                                           .
  */
  if(L7_SUCCESS != ptin_routing_arptable_getnext(intfNum, ENDIAN_SWAP32(inBuffer->lastIndex), maxEntries, readEntries, outBuffer))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unable to get the ARP table");
    return L7_FAILURE;
  }
  PT_LOG_DEBUG(LOG_CTX_MSG, "Successfully read %u entries", *readEntries);

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
  PT_LOG_DEBUG(LOG_CTX_MSG, "Removing ARP entry:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  mask       = %08X",  ENDIAN_SWAP32(data->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  Intf       = %u/%u", data->intf.intf_type, data->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  IP Address = %08X",  ENDIAN_SWAP32(data->ipAddr));

  if(ENDIAN_SWAP32(data->mask) & CCMSG_ROUTING_ARPTABLE_GET_MASK_INTF)
  {
    intf.intf_type = data->intf.intf_type;
    intf.intf_id   = data->intf.intf_id;
    if(L7_SUCCESS != ptin_intf_ptintf2intIfNum(&intf, &intfNum))
    {
      PT_LOG_ERR(LOG_CTX_ROUTING, "Unable to to convert intf %u/%u to intfNum", intf.intf_type, intf.intf_id);
      return L7_FAILURE;
    }
  }
  else
  {
    intfNum = (L7_uint32)-1;
  }

  if(L7_SUCCESS != ptin_routing_arpentry_purge(intfNum, ENDIAN_SWAP32(data->ipAddr)))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unable to remove the existing ARP entry");
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
    PT_LOG_ERR(LOG_CTX_MSG, "Abnormal context [inBuffer=%p outBuffer=%p readEntries=%p]", inBuffer, outBuffer, readEntries);
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Getting route table:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  intf       = %u/%u", inBuffer->intf.intf_type, inBuffer->intf.intf_id);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  lastIndex  = %u",    ENDIAN_SWAP32(inBuffer->lastIndex));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  maxEntries = %u",    maxEntries);

  intf.intf_type = inBuffer->intf.intf_type;
  intf.intf_id   = inBuffer->intf.intf_id;

  if(L7_SUCCESS != ptin_intf_ptintf2intIfNum(&intf, &intfNum))
  {
    PT_LOG_ERR(LOG_CTX_ROUTING, "Unable to to convert intf %u/%u to intfNum", intf.intf_type, intf.intf_id);
    return L7_FAILURE;
  }

  /*
   I know that passing a ptin_msghandler struct to a file other than ptin_msg breaks PTIN Fastpath's architecture.
   However, doing so here allows me to hide the complexity of interacting with usmDb completly inside of ptin_routing.                                                                                                           .
  */
  if(L7_SUCCESS != ptin_routing_routetable_get(intfNum, ENDIAN_SWAP32(inBuffer->lastIndex), maxEntries, readEntries, outBuffer))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unable to get the route table");
    return L7_FAILURE;
  }
  PT_LOG_DEBUG(LOG_CTX_MSG, "Successfully read %u entries", *readEntries);

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
  L7_RC_t rc;

  if( (data == L7_NULLPTR) )
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Configuring static route:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  dstIpAddr   = %08X", ENDIAN_SWAP32(data->dstIpAddr));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  subnetMask  = %08X", ENDIAN_SWAP32(data->subnetMask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  nextHopRtr  = %08X", ENDIAN_SWAP32(data->nextHopRtr));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  pref        = %u",   data->pref);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  isNullRoute = %u",   data->isNullRoute);

  rc = ptin_routing_staticroute_add(ENDIAN_SWAP32(data->dstIpAddr), ENDIAN_SWAP32(data->subnetMask), ENDIAN_SWAP32(data->nextHopRtr), 
                                    data->pref, (L7_BOOL)data->isNullRoute);

  if((rc != L7_SUCCESS) && (rc != L7_NOT_EXIST))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unable to configure static route [rc:%u]", rc);
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
    PT_LOG_ERR(LOG_CTX_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Removing an existing static route:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  dstIpAddr   = %08X", ENDIAN_SWAP32(data->dstIpAddr));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  subnetMask  = %08X", ENDIAN_SWAP32(data->subnetMask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  nextHopRtr  = %08X", ENDIAN_SWAP32(data->nextHopRtr));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  pref        = %u",   data->pref);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  isNullRoute = %u",   data->isNullRoute);

  if(L7_SUCCESS != ptin_routing_staticroute_delete(ENDIAN_SWAP32(data->dstIpAddr), ENDIAN_SWAP32(data->subnetMask), ENDIAN_SWAP32(data->nextHopRtr), 
                                                   (L7_BOOL)data->isNullRoute))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unable to remove static route");
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
    PT_LOG_ERR(LOG_CTX_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Creating new ping session:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  sessionIdx    = %u",   ENDIAN_SWAP16(data->sessionIdx));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  dstIpAddr     = %08X", ENDIAN_SWAP32(data->dstIpAddr));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  probeCount    = %u",   ENDIAN_SWAP16(data->probeCount));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  probeSize     = %u",   ENDIAN_SWAP16(data->probeSize));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  probeInterval = %u",   ENDIAN_SWAP16(data->probeInterval));

  if(L7_SUCCESS != ptin_routing_pingsession_create( ENDIAN_SWAP16(data->sessionIdx), ENDIAN_SWAP16(data->dstIpAddr), ENDIAN_SWAP16(data->probeCount),
                                                    ENDIAN_SWAP16(data->probeSize), ENDIAN_SWAP16(data->probeInterval)))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unable to create new ping session");
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
    PT_LOG_ERR(LOG_CTX_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Querying ping session:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  sessionIdx = %u", ENDIAN_SWAP16(data->sessionIdx));

  /*
   I know that passing a ptin_msghandler struct to a file other than ptin_msg breaks PTIN Fastpath's architecture.
   However, doing so here allows me to hide the complexity of interacting with usmDb completly inside of ptin_routing.                                                                                                           .
   */
  if(L7_SUCCESS != ptin_routing_pingsession_query(data))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unable to query ping session");
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
    PT_LOG_ERR(LOG_CTX_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Freeing ping session:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  mask       = %02X", data->mask);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  sessionIdx = %u",   ENDIAN_SWAP16(data->sessionIdx));

  if(data->mask & CCMSG_ROUTING_PINGSESSION_MASK_SESSIONIDX)
  {
    if(L7_SUCCESS != ptin_routing_pingsession_free(ENDIAN_SWAP16(data->sessionIdx)))
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Unable to free ping session");
      return L7_FAILURE;
    }
  }
  else
  {
    if(L7_SUCCESS != ptin_routing_pingsession_freeall())
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Unable to free ping sessions");
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
    PT_LOG_ERR(LOG_CTX_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Creating new traceroute session:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  sessionIdx    = %u",   ENDIAN_SWAP16(data->sessionIdx));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  dstIpAddr     = %08X", ENDIAN_SWAP32(data->dstIpAddr));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  probePerHop   = %u",   ENDIAN_SWAP16(data->probePerHop));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  probeSize     = %u",   ENDIAN_SWAP16(data->probeSize));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  probeInterval = %u",   ENDIAN_SWAP32(data->probeInterval));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  dontFrag      = %u",   data->dontFrag);
  PT_LOG_DEBUG(LOG_CTX_MSG, "  port          = %u",   ENDIAN_SWAP16(data->port));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  maxTtl        = %u",   ENDIAN_SWAP16(data->maxTtl));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  initTtl       = %u",   ENDIAN_SWAP16(data->initTtl));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  maxFail       = %u",   ENDIAN_SWAP16(data->maxFail));

  if(L7_SUCCESS != ptin_routing_traceroutesession_create(ENDIAN_SWAP16(data->sessionIdx), ENDIAN_SWAP32(data->dstIpAddr), ENDIAN_SWAP16(data->probeSize), ENDIAN_SWAP16(data->probePerHop), ENDIAN_SWAP32(data->probeInterval), data->dontFrag, ENDIAN_SWAP16(data->port), ENDIAN_SWAP16(data->maxTtl), ENDIAN_SWAP16(data->initTtl), ENDIAN_SWAP16(data->maxFail)))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unable to create new traceroute session");
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
    PT_LOG_ERR(LOG_CTX_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Querying traceroute session:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  sessionIdx = %u", ENDIAN_SWAP16(data->sessionIdx));

  /*
   I know that passing a ptin_msghandler struct to a file other than ptin_msg breaks PTIN Fastpath's architecture.
   However, doing so here allows me to hide the complexity of interacting with usmDb completly inside of ptin_routing.                                                                                                           .
   */
  if(L7_SUCCESS != ptin_routing_traceroutesession_query(data))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unable to query traceroute session");
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
    PT_LOG_ERR(LOG_CTX_MSG, "Abnormal context [inBuffer=%p outBuffer=%p readEntries=%p]", inBuffer, outBuffer, readEntries);
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Getting traceroute hops:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  sessionIdx = %u", ENDIAN_SWAP16(inBuffer->sessionIdx));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  lastIndex  = %u", ENDIAN_SWAP16(inBuffer->lastIndex));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  maxEntries = %u", maxEntries);

  /*
   I know that passing a ptin_msghandler struct to a file other than ptin_msg breaks PTIN Fastpath's architecture.
   However, doing so here allows me to hide the complexity of interacting with usmDb completly inside of ptin_routing.                                                                                                           .
  */
  if(L7_SUCCESS != ptin_routing_traceroutesession_gethops(ENDIAN_SWAP16(inBuffer->sessionIdx), ENDIAN_SWAP16(inBuffer->lastIndex),
                                                           maxEntries, readEntries, outBuffer))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Unable to get traceroute session hops");
    return L7_FAILURE;
  }
  PT_LOG_DEBUG(LOG_CTX_MSG, "Successfully read %u entries", *readEntries);

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
    PT_LOG_ERR(LOG_CTX_MSG, "Abnormal context [data=%p]", data);
    return L7_FAILURE;
  }

  /* Output data */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Freeing traceroute session:");
  PT_LOG_DEBUG(LOG_CTX_MSG, "  mask       = %02X", ENDIAN_SWAP8(data->mask));
  PT_LOG_DEBUG(LOG_CTX_MSG, "  sessionIdx = %u",   ENDIAN_SWAP16(data->sessionIdx));

  if(data->mask & CCMSG_ROUTING_TRACEROUTESESSION_MASK_SESSIONIDX)
  {
    if(L7_SUCCESS != ptin_routing_traceroutesession_free(ENDIAN_SWAP16(data->sessionIdx)))
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Unable to free traceroute session");
      return L7_FAILURE;
    }
  }
  else
  {
    if(L7_SUCCESS != ptin_routing_traceroutesession_freeall())
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Unable to free traceroute sessions");
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
    if(!ptin_fpga_mx_is_matrixactive_rt())//If I'm a Standby Matrix
    {         
      msg_SnoopSyncRequest_t   snoopSyncRequest = {0};
      L7_uint32                ipAddr; 
    
      /* IP address of Active Matrix*/
      ipAddr = IPC_MX_PAIR_IPADDR;

      PT_LOG_INFO(LOG_CTX_MSG, "Sending a Snoop Sync Request Message to ipAddr:%08X (%u)", ipAddr, MX_PAIR_SLOT_ID);

      /*Send the snoop sync request to the protection matrix */  
      if (send_ipc_message(IPC_HW_FASTPATH_PORT, ipAddr, CCMSG_MGMD_SNOOP_SYNC_REQUEST, (char *)(&snoopSyncRequest), NULL, sizeof(snoopSyncRequest), NULL) < 0)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Failed to send Snoop Sync Request Message");
//      return;
      }         
    }
    else
    {
      PT_LOG_NOTICE(LOG_CTX_MSG, "Not sending Snoop Sync Request Message. Since, I'm not a standby matrix");      
//    return
    }
  #endif
  }

  return;
}


/**
 * Clear RFC2819 buffer monitoring.
 * 
 * @param buffer_index: 
 * @param buffer_type: 0: 15min buffer, 1: 24Hours buffer
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 */
L7_RC_t ptin_msg_clear_rfc2819_monitoring_buffer(L7_uint32 buffer_index)
{ 
  PT_LOG_DEBUG(LOG_CTX_MSG, "Clear buffer_index: %u", buffer_index);

  //test bit31 to decide which buffer must be cleared (15min or 24hours)
  if (buffer_index & 0x80000000)
    buffer_index=RFC2819_BUFFER_24HOURS;
  else
    buffer_index=RFC2819_BUFFER_15MIN;

  return ptin_rfc2819_buffer_clear(buffer_index);
}




/**
 * get entrys from rfc2819 ring buffer
 * 
 * @param buffer_index: buffer index
 * @param buffer: points to the returned buffer
 * @param n_elements: number of buffers 
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
L7_RC_t ptin_msg_get_next_qualRFC2819_inv(L7_int buffer_index, msg_rfc2819_buffer_t *buffer, L7_int *n_elements)
{
  L7_int buffer_id,slot;
  L7_int first_reg=0;
  TBufferRegQualRFC2819 ring_buffer;

  buffer_id = buffer_index & 0xFFFF;

  slot      = (buffer_index>>16) & 0xFFF;
  PT_LOG_DEBUG(LOG_CTX_MSG, "slot %d", slot);

  if (buffer_index & 0x80000000)
    buffer_index=RFC2819_BUFFER_24HOURS;
  else
    buffer_index=RFC2819_BUFFER_15MIN;

  if (buffer_id==0xFFFF) {
    first_reg = -1; 
  } else {
    first_reg = buffer_id-1;
    if (first_reg<0)
      first_reg = MAX_QUAL_RFC2819_REG_NUM-1;
  }   

  *n_elements = 0;

  while (*n_elements<RFC2819_MAX_BUFFER_GET_NEXT) 
  { 
    L7_int32 port1;
    L7_int16 slot_ret,port_ret;
    port1 = (ring_buffer.path >> 14) & 0xFFF;

   
    ptin_intf_port2SlotPort(port1, &slot_ret, &port_ret, L7_NULLPTR);

    first_reg = ptin_rfc2819_buffer_get_inv(buffer_index, first_reg, &ring_buffer);

    PT_LOG_DEBUG(LOG_CTX_MSG, "slot_ret %d", slot_ret);
    PT_LOG_DEBUG(LOG_CTX_MSG, "port1 %d", port1);  

    #if(PTIN_BOARD == PTIN_BOARD_CXO640G) //|| (PTIN_BOARD == PTIN_BOARD_CXO160G)

    if(slot == slot_ret) //In TU40G and CXO160G check if the manager send slot match with the slot port 
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "port1 %d", port1);    
                                                                                                                 
      buffer[*n_elements].index                 = ENDIAN_SWAP32(ring_buffer.index);                              
      buffer[*n_elements].arg                   = ENDIAN_SWAP32(ring_buffer.arg);                                
      buffer[*n_elements].time                  = ENDIAN_SWAP32(ring_buffer.time);                               
      buffer[*n_elements].path                  = ENDIAN_SWAP32(ring_buffer.path);                               
      buffer[*n_elements].cTempo                = ENDIAN_SWAP32(ring_buffer.cTempo);                             
                                                                                                                 
      buffer[*n_elements].Octets                = ENDIAN_SWAP64(ring_buffer.Octets);                             
      buffer[*n_elements].Pkts                  = ENDIAN_SWAP64(ring_buffer.Pkts);                               
      buffer[*n_elements].Broadcast             = ENDIAN_SWAP64(ring_buffer.Broadcast);                          
      buffer[*n_elements].Multicast             = ENDIAN_SWAP64(ring_buffer.Multicast);                          
      buffer[*n_elements].CRCAlignErrors        = ENDIAN_SWAP64(ring_buffer.CRCAlignErrors);                     
      buffer[*n_elements].UndersizePkts         = ENDIAN_SWAP64(ring_buffer.UndersizePkts);                      
      buffer[*n_elements].OversizePkts          = ENDIAN_SWAP64(ring_buffer.OversizePkts);                       
      buffer[*n_elements].Fragments             = ENDIAN_SWAP64(ring_buffer.Fragments);                          
      buffer[*n_elements].Jabbers               = ENDIAN_SWAP64(ring_buffer.Jabbers);                            
      buffer[*n_elements].Collisions            = ENDIAN_SWAP64(ring_buffer.Collisions);                         
      buffer[*n_elements].Utilization           = ENDIAN_SWAP64(ring_buffer.Utilization);                        
      buffer[*n_elements].Pkts64Octets          = ENDIAN_SWAP64(ring_buffer.Pkts64Octets);                       
      buffer[*n_elements].Pkts65to127Octets     = ENDIAN_SWAP64(ring_buffer.Pkts65to127Octets);                  
      buffer[*n_elements].Pkts128to255Octets    = ENDIAN_SWAP64(ring_buffer.Pkts128to255Octets);                 
      buffer[*n_elements].Pkts256to511Octets    = ENDIAN_SWAP64(ring_buffer.Pkts256to511Octets);                 
      buffer[*n_elements].Pkts512to1023Octets   = ENDIAN_SWAP64(ring_buffer.Pkts512to1023Octets);                
      buffer[*n_elements].Pkts1024to1518Octets  = ENDIAN_SWAP64(ring_buffer.Pkts1024to1518Octets);               
    }
    else
    {
     if (first_reg<0) 
     break;

     continue;  

    }
    #else                                                                                                        
      buffer[*n_elements].index                 = ENDIAN_SWAP32(ring_buffer.index);                              
      buffer[*n_elements].arg                   = ENDIAN_SWAP32(ring_buffer.arg);                                
      buffer[*n_elements].time                  = ENDIAN_SWAP32(ring_buffer.time);                               
      buffer[*n_elements].path                  = ENDIAN_SWAP32(ring_buffer.path);                               
      buffer[*n_elements].cTempo                = ENDIAN_SWAP32(ring_buffer.cTempo);                             
                                                                                                                 
      buffer[*n_elements].Octets                = ENDIAN_SWAP64(ring_buffer.Octets);                             
      buffer[*n_elements].Pkts                  = ENDIAN_SWAP64(ring_buffer.Pkts);                               
      buffer[*n_elements].Broadcast             = ENDIAN_SWAP64(ring_buffer.Broadcast);                          
      buffer[*n_elements].Multicast             = ENDIAN_SWAP64(ring_buffer.Multicast);                          
      buffer[*n_elements].CRCAlignErrors        = ENDIAN_SWAP64(ring_buffer.CRCAlignErrors);                     
      buffer[*n_elements].UndersizePkts         = ENDIAN_SWAP64(ring_buffer.UndersizePkts);                      
      buffer[*n_elements].OversizePkts          = ENDIAN_SWAP64(ring_buffer.OversizePkts);                       
      buffer[*n_elements].Fragments             = ENDIAN_SWAP64(ring_buffer.Fragments);                          
      buffer[*n_elements].Jabbers               = ENDIAN_SWAP64(ring_buffer.Jabbers);                            
      buffer[*n_elements].Collisions            = ENDIAN_SWAP64(ring_buffer.Collisions);                         
      buffer[*n_elements].Utilization           = ENDIAN_SWAP64(ring_buffer.Utilization);                        
      buffer[*n_elements].Pkts64Octets          = ENDIAN_SWAP64(ring_buffer.Pkts64Octets);                       
      buffer[*n_elements].Pkts65to127Octets     = ENDIAN_SWAP64(ring_buffer.Pkts65to127Octets);                  
      buffer[*n_elements].Pkts128to255Octets    = ENDIAN_SWAP64(ring_buffer.Pkts128to255Octets);                 
      buffer[*n_elements].Pkts256to511Octets    = ENDIAN_SWAP64(ring_buffer.Pkts256to511Octets);                 
      buffer[*n_elements].Pkts512to1023Octets   = ENDIAN_SWAP64(ring_buffer.Pkts512to1023Octets);                
      buffer[*n_elements].Pkts1024to1518Octets  = ENDIAN_SWAP64(ring_buffer.Pkts1024to1518Octets);               
    #endif

    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].index %d", buffer[*n_elements].index);
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].arg  %d",  buffer[*n_elements].arg );   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].time  %d", buffer[*n_elements].time );   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].path %d",  buffer[*n_elements].path);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].cTempo %d", buffer[*n_elements].cTempo);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Octets  %d", buffer[*n_elements].Octets );   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Pkts %d", buffer[*n_elements].Pkts);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Broadcast %d", buffer[*n_elements].Broadcast);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Multicast %d", buffer[*n_elements].Multicast);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].CRCAlignErrors %d", buffer[*n_elements].CRCAlignErrors);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].UndersizePkts", buffer[*n_elements].UndersizePkts);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].OversizePkts %d", buffer[*n_elements].OversizePkts);

    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Fragments  %d", buffer[*n_elements].Fragments );   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Jabbers %d",  buffer[*n_elements].Jabbers);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Collisions %d", buffer[*n_elements].Collisions);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Utilization  %d", buffer[*n_elements].Utilization );   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Pkts64Octets %d", buffer[*n_elements].Pkts64Octets);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Pkts65to127Octets %d", buffer[*n_elements].Pkts65to127Octets);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Pkts128to255Octets %d", buffer[*n_elements].Pkts128to255Octets);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Pkts256to511Octets %d", buffer[*n_elements].Pkts256to511Octets);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Pkts512to1023Octets", buffer[*n_elements].Pkts512to1023Octets);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Pkts1024to1518Octets %d", buffer[*n_elements].Pkts1024to1518Octets);

    if (first_reg<0) 
      break;

    (*n_elements)++;        
  }  

  *n_elements = ENDIAN_SWAP16(*n_elements);

  return L7_SUCCESS;
}

/**
 * get entrys from rfc2819 ring buffer
 * 
 * @param buffer_index: buffer index
 * @param buffer: points to the returned buffer
 * @param n_elements: number of buffers 
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
L7_RC_t ptin_msg_get_next_qualRFC2819(L7_int buffer_index, msg_rfc2819_buffer_t *buffer)
{
  L7_int buffer_id;
  L7_int32 n_elements = 0;
  TBufferRegQualRFC2819 ring_buffer;

  buffer_id = buffer_index & 0xFFFF;

  if (buffer_index & 0x80000000)
    buffer_index=RFC2819_BUFFER_24HOURS;
  else
    buffer_index=RFC2819_BUFFER_15MIN;


    if(ptin_rfc2819_buffer_get(buffer_index, buffer_id, &ring_buffer) <0) 
    {
      memset(&buffer[n_elements], 0x00, sizeof(msg_rfc2819_buffer_t)); // When no performance monotoring is avaiable send zeros.
      return L7_SUCCESS;
    }
        
    buffer[n_elements].index                = ENDIAN_SWAP32(ring_buffer.index);
    buffer[n_elements].arg                  = ENDIAN_SWAP32(ring_buffer.arg);
    buffer[n_elements].time                 = ENDIAN_SWAP32(ring_buffer.time);
    buffer[n_elements].path                 = ENDIAN_SWAP32(ring_buffer.path);
    buffer[n_elements].cTempo               = ENDIAN_SWAP32(ring_buffer.cTempo);

    buffer[n_elements].Octets               = ENDIAN_SWAP64(ring_buffer.Octets);
    buffer[n_elements].Pkts                 = ENDIAN_SWAP64(ring_buffer.Pkts);                
    buffer[n_elements].Broadcast            = ENDIAN_SWAP64(ring_buffer.Broadcast);
    buffer[n_elements].Multicast            = ENDIAN_SWAP64(ring_buffer.Multicast);           
    buffer[n_elements].CRCAlignErrors       = ENDIAN_SWAP64(ring_buffer.CRCAlignErrors);      
    buffer[n_elements].UndersizePkts        = ENDIAN_SWAP64(ring_buffer.UndersizePkts);       
    buffer[n_elements].OversizePkts         = ENDIAN_SWAP64(ring_buffer.OversizePkts);        
    buffer[n_elements].Fragments            = ENDIAN_SWAP64(ring_buffer.Fragments);           
    buffer[n_elements].Jabbers              = ENDIAN_SWAP64(ring_buffer.Jabbers);             
    buffer[n_elements].Collisions           = ENDIAN_SWAP64(ring_buffer.Collisions);          
    buffer[n_elements].Utilization          = ENDIAN_SWAP64(ring_buffer.Utilization);         
    buffer[n_elements].Pkts64Octets         = ENDIAN_SWAP64(ring_buffer.Pkts64Octets);        
    buffer[n_elements].Pkts65to127Octets    = ENDIAN_SWAP64(ring_buffer.Pkts65to127Octets);   
    buffer[n_elements].Pkts128to255Octets   = ENDIAN_SWAP64(ring_buffer.Pkts128to255Octets);  
    buffer[n_elements].Pkts256to511Octets   = ENDIAN_SWAP64(ring_buffer.Pkts256to511Octets);  
    buffer[n_elements].Pkts512to1023Octets  = ENDIAN_SWAP64(ring_buffer.Pkts512to1023Octets); 
    buffer[n_elements].Pkts1024to1518Octets = ENDIAN_SWAP64(ring_buffer.Pkts1024to1518Octets);
   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].index %d", buffer[n_elements].index);
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].arg  %d",  buffer[n_elements].arg );   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].time  %d", buffer[n_elements].time );   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].path %d",  buffer[n_elements].path);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].cTempo %d", buffer[n_elements].cTempo);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Octets  %d", buffer[n_elements].Octets );   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Pkts %d", buffer[n_elements].Pkts);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Broadcast %d", buffer[n_elements].Broadcast);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Multicast %d", buffer[n_elements].Multicast);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].CRCAlignErrors %d", buffer[n_elements].CRCAlignErrors);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].UndersizePkts", buffer[n_elements].UndersizePkts);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].OversizePkts %d", buffer[n_elements].OversizePkts);

    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Fragments  %d", buffer[n_elements].Fragments );   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Jabbers %d",  buffer[n_elements].Jabbers);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Collisions %d", buffer[n_elements].Collisions);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Utilization  %d", buffer[n_elements].Utilization );   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Pkts64Octets %d", buffer[n_elements].Pkts64Octets);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Pkts65to127Octets %d", buffer[n_elements].Pkts65to127Octets);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Pkts128to255Octets %d", buffer[n_elements].Pkts128to255Octets);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Pkts256to511Octets %d", buffer[n_elements].Pkts256to511Octets);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Pkts512to1023Octets", buffer[n_elements].Pkts512to1023Octets);   
    PT_LOG_DEBUG(LOG_CTX_MSG, "buffer[n_elements].Pkts1024to1518Octets %d", buffer[n_elements].Pkts1024to1518Octets);
      
  return L7_SUCCESS;
}

/**
 * RFC2819 Probe Configuration
 * 
 * @param config: 
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
L7_RC_t ptin_msg_config_rfc2819_monitoring(msg_rfc2819_admin_t *config)
{ 
  L7_RC_t rc;

  PT_LOG_DEBUG(LOG_CTX_MSG, "Configure monitoring: SlotId: %d, Port: %d, admin: %d", config->SlotId, config->Port, config->Admin);

  #if (PTIN_BOARD == PTIN_BOARD_CXO160G) 

    PT_LOG_DEBUG(LOG_CTX_MSG, "%d", ptin_fpga_mx_is_matrix_in_workingslot()); 

    //Check if is a working CXO160G
    if((config->Port == 0 || config->Port == 1) && (ptin_fpga_mx_is_matrix_in_workingslot() == 1))
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Configure monitoring: SlotId: %d, Port: %d, admin: %d", config->SlotId, config->Port, config->Admin);
      return ptin_rfc2819_config_probe(config->Port, config->Admin);
    }
    //Check if is a protection CXO160G
    if((config->Port == 2 || config->Port == 3) && (ptin_fpga_mx_is_matrix_in_workingslot() == 0))
    {
      int aux_port;
      aux_port = config->Port - 2; //In CXO160G check if the manager send slot match with the slot port

      PT_LOG_DEBUG(LOG_CTX_MSG, "Configure monitoring: SlotId: %d, Port: %d, admin: %d", config->SlotId, config->Port, config->Admin);
      return ptin_rfc2819_config_probe(config->Port, config->Admin);
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Not possible to configure monitoring: SlotId: %d, Port: %d, admin: %d", config->SlotId, config->Port, config->Admin);
      return L7_SUCCESS;
    }
  #endif
  //Configure
  rc = ptin_rfc2819_config_probe(config->Port, config->Admin);

  PT_LOG_DEBUG(LOG_CTX_MSG, "Configure monitoring: SlotId: %d, Port: %d, admin: %d", config->SlotId, config->Port, config->Admin);
  if (L7_SUCCESS != rc)
    return L7_FAILURE;
  else
    return L7_SUCCESS;
}


/**
 * Get RFC2819 Probe Configuration
 * 
 * @param Port (input): 
 * @param Admin (output): 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_get_rfc2819_probe_config(L7_int Port, L7_uint8 *Admin)
{ 
  return ptin_rfc2819_get_config_probe(Port, Admin);
}


/**
 * Get RFC2819 Buffer status (For debug purposes)
 * 
 * @param buffer_type
 * @param status 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_msg_rfc2819_buffer_status(L7_int buffer_type, msg_rfc2819_buffer_status_t *status)
{  
  L7_RC_t rc;
  ENDIAN_SWAP16_MOD(buffer_type);

  rc = ptin_rfc2819_get_buffer_status(buffer_type, &status->max_entrys,  &status->wrptr, &status->bufferfull);

  ENDIAN_SWAP16_MOD(status->max_entrys);
  ENDIAN_SWAP16_MOD(status->wrptr);
  ENDIAN_SWAP16_MOD(status->bufferfull);

  return rc;
}

/*********************************************Multicast Package Feature**************************************************/

/**
 * Multicast Packages Add
 * 
 * @param msg : Pointer to  a Message 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE/L7_ALREADY_CONFIGURED
 */
L7_RC_t ptin_msg_igmp_packages_add(msg_igmp_package_t *msg)
{
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  L7_int32  packageIdIterator;
  L7_uint32 noOfPackagesFound = 0;
  L7_char8  packageBmpStr[PTIN_SYSTEM_IGMP_MAXPACKAGES/(sizeof(L7_uint8)*8)-1]={};
//L7_char8 *charPtr = packageBmpStr;
  L7_RC_t   rc = L7_SUCCESS;

  /* Input Argument validation */
  if ( msg  == L7_NULLPTR )
  {
    PT_LOG_ERR(LOG_CTX_IGMP, "Invalid arguments [msg:%p]",msg);    
    return L7_FAILURE;
  }
#if 0      
  for (packageIdIterator = (PTIN_SYSTEM_IGMP_MAXPACKAGES-1)/((sizeof(L7_uint8) * 8)); packageIdIterator>=0; --packageIdIterator)
  {
    osapiSnprintf(charPtr, sizeof(*charPtr),
                "%08X", msg->packageBmpList[packageIdIterator]);
    charPtr++;
  }
#endif
      
  /*Input Parameters*/
  PT_LOG_DEBUG(LOG_CTX_MSG, "Input Arguments [slotId:%u noOfPackages:%u packageBmpList:%s]",ENDIAN_SWAP8(msg->slotId), ENDIAN_SWAP16(msg->noOfPackages), packageBmpStr);

  for (packageIdIterator = 0; packageIdIterator < PTIN_SYSTEM_IGMP_MAXPACKAGES && ENDIAN_SWAP16(msg->noOfPackages) > 0; packageIdIterator++)
  {
    //Move forward 32 bits if this byte is 0 (no packages)
    if (IS_BITMAP_WORD_SET(ENDIAN_SWAP32(msg->packageBmpList), packageIdIterator, UINT32_BITSIZE) == L7_FALSE)
    {
     packageIdIterator += UINT32_BITSIZE -1; //Less one, because of the For cycle that increments also 1 unit.
     continue;
    }

    if (IS_BITMAP_BIT_SET( ENDIAN_SWAP32(msg->packageBmpList), packageIdIterator, UINT32_BITSIZE) == L7_FALSE)
    {
      continue;
    }
    /*Add This Package*/
    rc = ptin_igmp_multicast_package_add(packageIdIterator);      

    if (++noOfPackagesFound >= msg->noOfPackages)
    {
      /*Found All Packages*/
      break;
    }
 }
  return rc;  
#else
  PT_LOG_ERR(LOG_CTX_IGMP, "Featured not supported in this card!");  
  return L7_NOT_SUPPORTED;  
#endif 
}

/**
 * Multicast Packages Remove
 * 
 * @param msg : Pointer to  a Message 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t ptin_msg_igmp_packages_remove(msg_igmp_package_t *msg)
{
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  L7_int32  packageIdIterator;
  L7_uint32 noOfPackagesFound = 0;
  L7_char8  packageBmpStr[PTIN_SYSTEM_IGMP_MAXPACKAGES/(sizeof(L7_uint8)*8)-1]={};
//L7_char8 *charPtr = packageBmpStr;
  L7_BOOL   forceRemoval = L7_FALSE;
  L7_RC_t   rc = L7_SUCCESS;

  /* Input Argument validation */
  if ( msg  == L7_NULLPTR )
  {
    PT_LOG_ERR(LOG_CTX_IGMP, "Invalid arguments [msg:%p]",msg);    
    return L7_FAILURE;
  }
#if 0
  for (packageIdIterator =(PTIN_SYSTEM_IGMP_MAXPACKAGES-1)/((sizeof(L7_uint8) * 8)); packageIdIterator>=0; --packageIdIterator)
  {
    osapiSnprintf(charPtr, sizeof(*charPtr),
                "%08X", msg->packageBmpList[packageIdIterator]);
    charPtr++;
  }
#endif      
  /*Input Parameters*/
  PT_LOG_DEBUG(LOG_CTX_MSG, "Input Arguments [slotId:%u noOfPackages:%u packageBmpList:%s]",ENDIAN_SWAP8(msg->slotId), ENDIAN_SWAP16(msg->noOfPackages), packageBmpStr);

  for (packageIdIterator = 0; packageIdIterator < PTIN_SYSTEM_IGMP_MAXPACKAGES && ENDIAN_SWAP16(msg->noOfPackages) > 0; packageIdIterator++)
  {
    ENDIAN_SWAP32_MOD(msg[messageIterator].evcId);
    ENDIAN_SWAP16_MOD(msg[messageIterator].client.outer_vlan);
    ENDIAN_SWAP16_MOD(msg[messageIterator].client.outer_vlan);
    ENDIAN_SWAP16_MOD(msg[messageIterator].noOfPackages);
    ENDIAN_SWAP32_MOD(msg[messageIterator].packageBmpList);

    //Move forward 32 bits if this byte is 0 (no packages)
    if (IS_BITMAP_WORD_SET(ENDIAN_SWAP32(msg->packageBmpList), packageIdIterator, UINT32_BITSIZE) == L7_FALSE)
    {
     packageIdIterator += UINT32_BITSIZE -1; //Less one, because of the For cycle that increments also 1 unit.
     continue;
    }

    if (IS_BITMAP_BIT_SET( ENDIAN_SWAP32(msg->packageBmpList), packageIdIterator, UINT32_BITSIZE) == L7_FALSE)
    {
      continue;
    }
    /*Add This Package*/
    if ( L7_DEPENDENCY_NOT_MET == (rc = ptin_igmp_multicast_package_remove(packageIdIterator, forceRemoval)) )
    {
      /*Error Already Logged*/
      return rc;
    }

    if (++noOfPackagesFound >= ENDIAN_SWAP16(msg->noOfPackages))
    {
      /*Found All Packages*/
      break;
    }
 }
  return rc;   
#else
  PT_LOG_ERR(LOG_CTX_IGMP, "Feature not supported in this card!");  
  return L7_NOT_SUPPORTED;  
#endif 
}

/**
 * Multicast Package Channels Add
 * 
 * @param msg          : Pointer to  a Message 
 * @param noOfMessages : Number of Messages
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t ptin_msg_igmp_package_channels_add(msg_igmp_package_channels_t *msg, L7_uint32 noOfMessages)
{
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  char            groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char            sourceAddrStr[IPV6_DISP_ADDR_LEN]={};  
  L7_inet_addr_t  groupAddr;
  L7_inet_addr_t  sourceAddr;
  L7_uint32       messageIterator;
  L7_RC_t         rc = L7_SUCCESS;

  /* Input Argument validation */
  if ( msg  == L7_NULLPTR || noOfMessages == 0)
  {
    PT_LOG_ERR(LOG_CTX_IGMP, "Invalid arguments [msg:%p noOfMessages:%u]",msg, noOfMessages);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  PT_LOG_DEBUG(LOG_CTX_MSG,"Input Arguments [noOfMessages:%u]", noOfMessages);

  for (messageIterator = 0; messageIterator < noOfMessages; messageIterator++)
  {

    ENDIAN_SWAP32_MOD(msg[messageIterator].evcId);
    ENDIAN_SWAP16_MOD(msg[messageIterator].client.outer_vlan);
    ENDIAN_SWAP16_MOD(msg[messageIterator].client.outer_vlan);
    ENDIAN_SWAP16_MOD(msg[messageIterator].noOfPackages);
    ENDIAN_SWAP32_MOD(msg[messageIterator].packageBmpList);

    /*Convert Group Address to fp Notation*/
    rc = ptin_to_fp_ip_notation(&msg[messageIterator].groupAddr, &groupAddr);
    if ( rc != L7_SUCCESS)
    {
      return rc;
    }

    /*Convert Source Address to fp Notation*/
    rc = ptin_to_fp_ip_notation(&msg[messageIterator].sourceAddr, &sourceAddr);
    if ( rc != L7_SUCCESS)
    {
      return rc;
    }

    inetAddrPrint(&groupAddr, groupAddrStr);
    inetAddrPrint(&sourceAddr, sourceAddrStr);

    /*Validate Group Address & Group Mask */
    /*Validate Source Address & Source Mask*/
    if (inetIsAddressZero(&groupAddr) == L7_FALSE) 
    {
      if (inetIsInMulticast(&groupAddr)== L7_FALSE ||  msg[messageIterator].groupMask < PTIN_IGMP_GROUP_MASK_MIN ||  msg[messageIterator].groupMask > 32 || (!inetIsAddressZero(&sourceAddr) && inetIsValidHostAddress(&sourceAddr)== L7_FALSE) )            
      {      
        PT_LOG_ERR(LOG_CTX_MSG," Invalid Parameters: packageId:%u evcId:%u groupAddr=%s/%u sourceAddr:%s/%u ",  msg[messageIterator].packageId, msg[messageIterator].evcId, groupAddrStr,  msg[messageIterator].groupMask, sourceAddrStr,  msg[messageIterator].sourceMask);
        return L7_FAILURE;                           
      }

      if ( msg[messageIterator].sourceMask != 32 )
      {
//      PT_LOG_WARN(LOG_CTX_MSG," Invalid Parameters: packageId:%u evcId:%u groupAddr=%s/%u sourceAddr:%s/%u ",  msg[messageIterator].packageId, msg[messageIterator].evcId, groupAddrStr,  msg[messageIterator].groupMask, sourceAddrStr,  msg[messageIterator].sourceMask);
        msg[messageIterator].sourceMask = 32;          
      }
    }
    else
    { /*Default Multicast Entry*/
      if (!inetIsAddressZero(&sourceAddr))
      {
        PT_LOG_ERR(LOG_CTX_MSG," Invalid Parameters: packageId:%u evcId:%u groupAddr=%s/%u sourceAddr:%s/%u ",  msg[messageIterator].packageId, msg[messageIterator].evcId, groupAddrStr,  msg[messageIterator].groupMask, sourceAddrStr,  msg[messageIterator].sourceMask);
        return L7_FAILURE;                   
      }

      if ( msg[messageIterator].groupMask != 32)
      {
        PT_LOG_WARN(LOG_CTX_MSG," Invalid Parameters: packageId:%u evcId:%u groupAddr=%s/%u sourceAddr:%s/%u ",  msg[messageIterator].packageId, msg[messageIterator].evcId, groupAddrStr,  msg[messageIterator].groupMask, sourceAddrStr,  msg[messageIterator].sourceMask);
        msg[messageIterator].groupMask = 32;
      }

      if ( msg[messageIterator].sourceMask != 0 )
      {
        PT_LOG_WARN(LOG_CTX_MSG," Invalid Parameters: packageId:%u evcId:%u groupAddr=%s/%u sourceAddr:%s/%u ",  msg[messageIterator].packageId, msg[messageIterator].evcId, groupAddrStr,  msg[messageIterator].groupMask, sourceAddrStr,  msg[messageIterator].sourceMask);
        msg[messageIterator].sourceMask = 0;
      }       
    }

    /*Input Parameters*/
    PT_LOG_DEBUG(LOG_CTX_MSG, "Input Arguments [slotId:%u packageId:%u serviceId:%u groupAddr:%s groupMask:%u sourceAddr:%s sourceMask:%u]",
              msg[messageIterator].slotId, msg[messageIterator].packageId, msg[messageIterator].evcId, 
              inetAddrPrint(&groupAddr, groupAddrStr), msg[messageIterator].groupMask, inetAddrPrint(&sourceAddr, sourceAddrStr), msg[messageIterator].sourceMask );

    /*Error Any Error Occurs It is Already Logged*/
    if ( L7_SUCCESS != (rc = ptin_igmp_multicast_package_channels_add(msg[messageIterator].packageId, msg[messageIterator].evcId, 
                                                   &groupAddr, msg[messageIterator].groupMask, &sourceAddr, msg[messageIterator].sourceMask)) )
    {
      return rc;
    }
  }
  return rc;
#else
  PT_LOG_ERR(LOG_CTX_IGMP, "Featured not supported in this card!");  
  return L7_NOT_SUPPORTED;  
#endif 
}

/**
 * Multicast Package Channels Remove
 * 
 * @param msg          : Pointer to  a Message 
 * @param noOfMessages : Number of Messages
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t ptin_msg_igmp_package_channels_remove(msg_igmp_package_channels_t *msg, L7_uint32 noOfMessages)
{
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  char            groupAddrStr[IPV6_DISP_ADDR_LEN]={};
  char            sourceAddrStr[IPV6_DISP_ADDR_LEN]={};  
  L7_inet_addr_t  groupAddr;
  L7_inet_addr_t  sourceAddr;
  L7_uint32       messageIterator;
  L7_RC_t         rc = L7_SUCCESS;

  /* Input Argument validation */
  if ( msg  == L7_NULLPTR || noOfMessages == 0)
  {
    PT_LOG_ERR(LOG_CTX_IGMP, "Invalid arguments [msg:%p noOfMessages:%u]",msg, noOfMessages);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  PT_LOG_DEBUG(LOG_CTX_MSG,"Input Arguments [noOfMessages:%u]", noOfMessages);

  for (messageIterator = 0; messageIterator < noOfMessages; messageIterator++)
  {

    ENDIAN_SWAP32_MOD(msg[messageIterator].evcId);
    ENDIAN_SWAP16_MOD(msg[messageIterator].client.outer_vlan);
    ENDIAN_SWAP16_MOD(msg[messageIterator].client.outer_vlan);
    ENDIAN_SWAP16_MOD(msg[messageIterator].noOfPackages);
    ENDIAN_SWAP32_MOD(msg[messageIterator].packageBmpList);

    /*Convert Group Address to fp Notation*/
    rc = ptin_to_fp_ip_notation(&msg[messageIterator].groupAddr, &groupAddr);
    if ( rc != L7_SUCCESS)
    {
      return rc;
    }

    /*Convert Source Address to fp Notation*/
    rc = ptin_to_fp_ip_notation(&msg[messageIterator].sourceAddr, &sourceAddr);
    if ( rc != L7_SUCCESS)
    {
      return rc;
    }

    inetAddrPrint(&groupAddr, groupAddrStr);
    inetAddrPrint(&sourceAddr, sourceAddrStr);

    /*Validate Group Address & Group Mask */
    /*Validate Source Address & Source Mask*/
    if (inetIsAddressZero(&groupAddr) == L7_FALSE) 
    {
      if (inetIsInMulticast(&groupAddr)== L7_FALSE ||  msg[messageIterator].groupMask < PTIN_IGMP_GROUP_MASK_MIN ||  msg[messageIterator].groupMask > 32 || (!inetIsAddressZero(&sourceAddr) && inetIsValidHostAddress(&sourceAddr)== L7_FALSE) )            
      {      
        PT_LOG_ERR(LOG_CTX_MSG," Invalid Parameters: packageId:%u evcId:%u groupAddr=%s/%u sourceAddr:%s/%u ",  msg[messageIterator].packageId, msg[messageIterator].evcId, groupAddrStr,  msg[messageIterator].groupMask, sourceAddrStr,  msg[messageIterator].sourceMask);
        return L7_FAILURE;                           
      }

      if ( msg[messageIterator].sourceMask != 32 )
      {
//      PT_LOG_WARN(LOG_CTX_MSG," Invalid Parameters: packageId:%u evcId:%u groupAddr=%s/%u sourceAddr:%s/%u ",  msg[messageIterator].packageId, msg[messageIterator].evcId, groupAddrStr,  msg[messageIterator].groupMask, sourceAddrStr,  msg[messageIterator].sourceMask);
        msg[messageIterator].sourceMask = 32;          
      }
    }
    else
    { /*Default Multicast Entry*/
      if (!inetIsAddressZero(&sourceAddr))
      {
        PT_LOG_ERR(LOG_CTX_MSG," Invalid Parameters: packageId:%u evcId:%u groupAddr=%s/%u sourceAddr:%s/%u ",  msg[messageIterator].packageId, msg[messageIterator].evcId, groupAddrStr,  msg[messageIterator].groupMask, sourceAddrStr,  msg[messageIterator].sourceMask);
        return L7_FAILURE;                   
      }

      if ( msg[messageIterator].groupMask != 32 )
      {
        PT_LOG_WARN(LOG_CTX_MSG," Invalid Parameters: packageId:%u evcId:%u groupAddr=%s/%u sourceAddr:%s/%u ",  msg[messageIterator].packageId, msg[messageIterator].evcId, groupAddrStr,  msg[messageIterator].groupMask, sourceAddrStr,  msg[messageIterator].sourceMask);
        msg[messageIterator].groupMask = 32;
      }

      if ( msg[messageIterator].sourceMask != 0 )
      {
        PT_LOG_WARN(LOG_CTX_MSG," Invalid Parameters: packageId:%u evcId:%u groupAddr=%s/%u sourceAddr:%s/%u ",  msg[messageIterator].packageId, msg[messageIterator].evcId, groupAddrStr,  msg[messageIterator].groupMask, sourceAddrStr,  msg[messageIterator].sourceMask);
        msg[messageIterator].sourceMask = 0;
      }       
    }

    /*Input Parameters*/
    PT_LOG_DEBUG(LOG_CTX_MSG, "Input Arguments [slotId:%u packageId:%u serviceId:%u groupAddr:%s groupMask:%u sourceAddr:%s sourceMask:%u]",
              msg[messageIterator].slotId, msg[messageIterator].packageId, msg[messageIterator].evcId, 
              inetAddrPrint(&groupAddr, groupAddrStr), msg[messageIterator].groupMask, inetAddrPrint(&sourceAddr, sourceAddrStr), msg[messageIterator].sourceMask );

    /*If Any Error Occurs It is Already Logged*/
    if ( L7_SUCCESS != (rc = ptin_igmp_multicast_package_channels_remove(msg[messageIterator].packageId, msg[messageIterator].evcId, 
                                                   &groupAddr, msg[messageIterator].groupMask, &sourceAddr, msg[messageIterator].sourceMask)) )
    {
      return rc;
    }
  }
  return rc;
#else
  PT_LOG_ERR(LOG_CTX_IGMP, "Featured not supported in this card!");  
  return L7_NOT_SUPPORTED;  
#endif 
}

/**
 * Unicast Client Packages Add
 * 
 * @param msg          : Pointer to  a Message 
 * @param noOfMessages : Number of Messages
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t ptin_msg_igmp_unicast_client_packages_add(msg_igmp_unicast_client_packages_t *msg, L7_uint32 noOfMessages)
{
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  L7_uint32       messageIterator;
//L7_int32        packageIdIterator;
  L7_char8         packageBmpStr[PTIN_SYSTEM_IGMP_MAXPACKAGES/(sizeof(L7_uint8)*8)-1]={};
//L7_char8        *charPtr           = packageBmpStr;
  ptin_client_id_t client;
  L7_BOOL          addOrRemove = L7_FALSE;//Add Packages
  L7_uint32        intIfNum;
  L7_uint16        uni_ivid;
  L7_uint16        uni_ovid;
  L7_RC_t          rc          = L7_SUCCESS;
  
  /* Input Argument validation */
  if ( msg  == L7_NULLPTR || noOfMessages == 0)
  {
    PT_LOG_ERR(LOG_CTX_IGMP, "Invalid arguments [msg:%p noOfMessages:%u]",msg, noOfMessages);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  PT_LOG_DEBUG(LOG_CTX_MSG,"Input Arguments [noOfMessages:%u]", noOfMessages);

  for (messageIterator = 0; messageIterator < noOfMessages; messageIterator++)
  {
#if 0    
    for (packageIdIterator =PTIN_IGMP_PACKAGE_BITMAP_SIZE-1; packageIdIterator>=0; --packageIdIterator)
    {
      osapiSnprintf(charPtr, sizeof(*charPtr),
                "%08X", msg[messageIterator].packageBmpList[packageIdIterator]);
      charPtr++;
    }
#endif

    ENDIAN_SWAP32_MOD(msg[messageIterator].evcId);
    ENDIAN_SWAP16_MOD(msg[messageIterator].client.outer_vlan);
    ENDIAN_SWAP16_MOD(msg[messageIterator].client.outer_vlan);
    ENDIAN_SWAP16_MOD(msg[messageIterator].noOfPackages);
    ENDIAN_SWAP32_MOD(msg[messageIterator].packageBmpList);
    
    /* Output data */
    PT_LOG_DEBUG(LOG_CTX_MSG, "Going to add MC client");
    PT_LOG_DEBUG(LOG_CTX_MSG, "  MC evc_idx = %u", msg[messageIterator].evcId);
    PT_LOG_DEBUG(LOG_CTX_MSG, "   onuId               = %u", msg[messageIterator].onuId);
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.Mask         = 0x%02x", msg[messageIterator].client.mask);
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.OVlan        = %u", msg[messageIterator].client.outer_vlan);
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.IVlan        = %u", msg[messageIterator].client.inner_vlan);
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.Intf         = %u/%u", msg[messageIterator].client.intf.intf_type,msg[messageIterator].client.intf.intf_id);        
    PT_LOG_DEBUG(LOG_CTX_MSG, "   noOfPackages        = %u ", msg[messageIterator].noOfPackages);
    PT_LOG_DEBUG(LOG_CTX_MSG, "   PackageBmpList      = %s", packageBmpStr);

    #if PTIN_BOARD_IS_ACTIVETH   
    if (msg[messageIterator].onuId != 0)
    {
      PT_LOG_WARN(LOG_CTX_MSG, "   I'm an Active Ethernet Card. OnuId:%u is different from 0. Going to set it to zero", msg[messageIterator].onuId);
      msg[messageIterator].onuId = 0;
    }    
    #endif

    if ( msg[messageIterator].noOfPackages > 0 )
    {
      memset(&client,0x00,sizeof(ptin_client_id_t));
      if (msg[messageIterator].client.mask & MSG_CLIENT_OVLAN_MASK)
      {
        client.outerVlan = msg[messageIterator].client.outer_vlan;
        client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
      }
      if (msg[messageIterator].client.mask & MSG_CLIENT_IVLAN_MASK)
      {
        client.innerVlan = msg[messageIterator].client.inner_vlan;
        client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
      }
      if (msg[messageIterator].client.mask & MSG_CLIENT_INTF_MASK)
      {
        client.ptin_intf.intf_type  = msg[messageIterator].client.intf.intf_type;
        client.ptin_intf.intf_id    = msg[messageIterator].client.intf.intf_id;
        client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
      }

      {
        rc = ptin_igmp_clientId_convert(msg[messageIterator].evcId, &client);
        if ( rc != L7_SUCCESS )
        {
          PT_LOG_ERR(LOG_CTX_MSG, "Error converting clientId");
          continue;
        }

        /* Get interface as intIfNum format */      
        if (ptin_intf_ptintf2intIfNum(&client.ptin_intf, &intIfNum)==L7_SUCCESS)
        {
          if (ptin_evc_extVlans_get(intIfNum, msg[messageIterator].evcId,(L7_uint32)-1, client.innerVlan, &uni_ovid, &uni_ivid) == L7_SUCCESS)
          {
            PT_LOG_TRACE(LOG_CTX_IGMP,"Ext vlans for ptin_intf %u/%u, cvlan %u: uni_ovid=%u, uni_ivid=%u",
                      client.ptin_intf.intf_type,client.ptin_intf.intf_id, client.innerVlan, uni_ovid, uni_ivid);
          }
          else
          {
            uni_ovid = uni_ivid = 0;
            PT_LOG_ERR(LOG_CTX_IGMP,"Cannot get ext vlans for ptin_intf %u/%u, cvlan %u",
                    client.ptin_intf.intf_type,client.ptin_intf.intf_id, client.innerVlan);
          }
        }
        else
        {
          PT_LOG_ERR(LOG_CTX_IGMP,"Invalid ptin_intf %u/%u", client.ptin_intf.intf_type, client.ptin_intf.intf_id);
        }
      }

      /* Apply config */
      rc = ptin_igmp_api_client_add(&client, uni_ovid, uni_ivid, msg[messageIterator].onuId, 0x00, 0, 0, addOrRemove, msg[messageIterator].packageBmpList, msg[messageIterator].noOfPackages);

      if (rc!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error adding MC client");
        return rc;
      }
    }
  }
  return rc;
#else
  PT_LOG_ERR(LOG_CTX_IGMP, "Featured not supported in this card!");  
  return L7_NOT_SUPPORTED;  
#endif 
}

/**
 * Unicast Client Packages Remove
 * 
 * @param msg          : Pointer to  a Message 
 * @param noOfMessages : Number of Messages
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t ptin_msg_igmp_unicast_client_packages_remove(msg_igmp_unicast_client_packages_t *msg, L7_uint32 noOfMessages)
{
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  L7_uint32        messageIterator;
//L7_int32         packageIdIterator;
  L7_char8         packageBmpStr[PTIN_SYSTEM_IGMP_MAXPACKAGES/(sizeof(L7_uint8)*8)-1]={};
//L7_char8        *charPtr           = packageBmpStr;
  ptin_client_id_t client;
  L7_BOOL          addOrRemove = L7_TRUE;//Remove Packages
  L7_uint32        intIfNum;
  L7_uint16        uni_ivid;
  L7_uint16        uni_ovid;
  L7_RC_t          rc          = L7_SUCCESS;

  /* Input Argument validation */
  if ( msg  == L7_NULLPTR || noOfMessages == 0)
  {
    PT_LOG_ERR(LOG_CTX_IGMP, "Invalid arguments [msg:%p noOfMessages:%u]",msg, noOfMessages);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  PT_LOG_DEBUG(LOG_CTX_MSG,"Input Arguments [noOfMessages:%u]", noOfMessages);

  for (messageIterator = 0; messageIterator < noOfMessages; messageIterator++)
  {
#if 0    
    for (packageIdIterator =PTIN_IGMP_PACKAGE_BITMAP_SIZE -1; packageIdIterator>=0; --packageIdIterator)
    {
      osapiSnprintf(charPtr, sizeof(*charPtr),
                "%02X", msg[messageIterator].packageBmpList[packageIdIterator]);
      charPtr++;
    }
#endif

    #if PTIN_BOARD_IS_ACTIVETH   
    if (ENDIAN_SWAP8(msg[messageIterator].onuId) != 0)
    {
      PT_LOG_WARN(LOG_CTX_MSG, "   I'm an Active Ethernet Card. OnuId:%u is different from 0. Going to set it to zero", ENDIAN_SWAP8(msg[messageIterator].onuId));
      msg[messageIterator].onuId = 0;
    }    
    #endif
    
     /* Output data */
    PT_LOG_DEBUG(LOG_CTX_MSG, "Going to add MC client");
    PT_LOG_DEBUG(LOG_CTX_MSG, "  MC evc_idx = %u",               ENDIAN_SWAP32(msg[messageIterator].evcId));
    PT_LOG_DEBUG(LOG_CTX_MSG, "   onuId               = %u",     ENDIAN_SWAP8(msg[messageIterator].onuId));
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.Mask         = 0x%02x", ENDIAN_SWAP8(msg[messageIterator].client.mask));
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.OVlan        = %u",     ENDIAN_SWAP16(msg[messageIterator].client.outer_vlan));
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.IVlan        = %u",     ENDIAN_SWAP16(msg[messageIterator].client.inner_vlan));
    PT_LOG_DEBUG(LOG_CTX_MSG, "   Client.Intf         = %u/%u",  ENDIAN_SWAP8(msg[messageIterator].client.intf.intf_type),ENDIAN_SWAP8(msg[messageIterator].client.intf.intf_id));    
    PT_LOG_DEBUG(LOG_CTX_MSG, "   noOfPackages        = %u ",    ENDIAN_SWAP16(msg[messageIterator].noOfPackages));
    PT_LOG_DEBUG(LOG_CTX_MSG, "   PackageBmpList      = %s",     packageBmpStr);

    if ( ENDIAN_SWAP16(msg[messageIterator].noOfPackages) > 0 )
    {
      memset(&client,0x00,sizeof(ptin_client_id_t));
      if (ENDIAN_SWAP8(msg[messageIterator].client.mask) & MSG_CLIENT_OVLAN_MASK)
      {
        client.outerVlan = ENDIAN_SWAP16(msg[messageIterator].client.outer_vlan);
        client.mask |= PTIN_CLIENT_MASK_FIELD_OUTERVLAN;
      }
      if (ENDIAN_SWAP8(msg[messageIterator].client.mask) & MSG_CLIENT_IVLAN_MASK)
      {
        client.innerVlan = ENDIAN_SWAP16(msg[messageIterator].client.inner_vlan);
        client.mask |= PTIN_CLIENT_MASK_FIELD_INNERVLAN;
      }
      if (ENDIAN_SWAP8(msg[messageIterator].client.mask) & MSG_CLIENT_INTF_MASK)
      {
        client.ptin_intf.intf_type  = ENDIAN_SWAP8(msg[messageIterator].client.intf.intf_type);
        client.ptin_intf.intf_id    = ENDIAN_SWAP8(msg[messageIterator].client.intf.intf_id);
        client.mask |= PTIN_CLIENT_MASK_FIELD_INTF;
      }

      {
        rc = ptin_igmp_clientId_convert(ENDIAN_SWAP32(msg[messageIterator].evcId), &client);
        if ( rc != L7_SUCCESS )
        {
          PT_LOG_ERR(LOG_CTX_MSG, "Error converting clientId");
          /*The client may not exist!*/
          rc = L7_SUCCESS;
          continue;
        }

        /* Get interface as intIfNum format */      
        if (ptin_intf_ptintf2intIfNum(&client.ptin_intf, &intIfNum)==L7_SUCCESS)
        {
          if (ptin_evc_extVlans_get(intIfNum, ENDIAN_SWAP32(msg[messageIterator].evcId),(L7_uint32)-1, client.innerVlan, &uni_ovid, &uni_ivid) == L7_SUCCESS)
          {
            PT_LOG_TRACE(LOG_CTX_IGMP,"Ext vlans for ptin_intf %u/%u, cvlan %u: uni_ovid=%u, uni_ivid=%u",
                      client.ptin_intf.intf_type,client.ptin_intf.intf_id, client.innerVlan, uni_ovid, uni_ivid);
          }
          else
          {
            uni_ovid = uni_ivid = 0;
            PT_LOG_ERR(LOG_CTX_IGMP,"Cannot get ext vlans for ptin_intf %u/%u, cvlan %u",
                    client.ptin_intf.intf_type,client.ptin_intf.intf_id, client.innerVlan);
          }
        }
        else
        {
          PT_LOG_ERR(LOG_CTX_IGMP,"Invalid ptin_intf %u/%u", client.ptin_intf.intf_type, client.ptin_intf.intf_id);
        }
      }

      /* Apply config */
      rc = ptin_igmp_api_client_add(&client, uni_ovid, uni_ivid, ENDIAN_SWAP8(msg[messageIterator].onuId), 0x00, 0, 0, addOrRemove, ENDIAN_SWAP32(msg[messageIterator].packageBmpList), ENDIAN_SWAP16(msg[messageIterator].noOfPackages));

      if (rc!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error adding MC client rc:%u", rc);
        return rc;
      }
    }
  }
  return rc;
#else
  PT_LOG_ERR(LOG_CTX_IGMP, "Featured not supported in this card!");  
  return L7_NOT_SUPPORTED;  
#endif 
}

/**
 * Macbridge Client Packages Add
 * 
 * @param msg          : Pointer to  a Message 
 * @param noOfMessages : Number of Messages
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t ptin_msg_igmp_macbridge_client_packages_add(msg_igmp_macbridge_client_packages_t *msg, L7_uint32 noOfMessages)
{
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  L7_uint32        messageIterator;
//L7_int32         packageIdIterator;
  L7_char8         packageBmpStr[PTIN_SYSTEM_IGMP_MAXPACKAGES/(sizeof(L7_uint8)*8)-1]={};
//L7_char8        *charPtr           = packageBmpStr;
  ptin_evc_macbridge_client_packages_t ptinEvcFlow;  
  L7_RC_t          rc                = L7_SUCCESS;

  /* Input Argument validation */
  if ( msg  == L7_NULLPTR || noOfMessages == 0)
  {
    PT_LOG_ERR(LOG_CTX_IGMP, "Invalid arguments [msg:%p noOfMessages:%u]",msg, noOfMessages);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  PT_LOG_DEBUG(LOG_CTX_MSG,"Input Arguments [noOfMessages:%u]", noOfMessages);

  for (messageIterator = 0; messageIterator < noOfMessages; messageIterator++)
  {    
    #if PTIN_BOARD_IS_ACTIVETH   
    if (ENDIAN_SWAP8(msg[messageIterator].onuId) != 0)
    {
      PT_LOG_WARN(LOG_CTX_MSG, "   I'm an Active Ethernet Card. OnuId:%u is different from 0. Going to set it to zero", ENDIAN_SWAP8(msg[messageIterator].onuId));
      msg[messageIterator].onuId = 0;
    }    
    #endif

    /*Initialize Structure*/
    memset(&ptinEvcFlow, 0x00, sizeof(ptinEvcFlow));

    /* Copy data */                                                                                                  
    ptinEvcFlow.evc_idx             = ENDIAN_SWAP32(msg[messageIterator].evcId);                                     
    ptinEvcFlow.int_ivid            = ENDIAN_SWAP16(msg[messageIterator].nni_cvlan);                                 
    ptinEvcFlow.ptin_intf.intf_type = ENDIAN_SWAP8(msg[messageIterator].intf.intf_type);                             
    ptinEvcFlow.ptin_intf.intf_id   = ENDIAN_SWAP8(msg[messageIterator].intf.intf_id);                               
    ptinEvcFlow.uni_ovid            = ENDIAN_SWAP16(msg[messageIterator].intf.outer_vid); /* must be a leaf */       
    ptinEvcFlow.uni_ivid            = ENDIAN_SWAP16(msg[messageIterator].intf.inner_vid);                            
    ptinEvcFlow.onuId               = ENDIAN_SWAP8(msg[messageIterator].onuId);                                      
    ptinEvcFlow.noOfPackages        = ENDIAN_SWAP16(msg[messageIterator].noOfPackages);                              

    /*Copy Multicast Package Bitmap*/
    memcpy(ptinEvcFlow.packageBmpList, ENDIAN_SWAP32(msg[messageIterator].packageBmpList), sizeof(ptinEvcFlow.packageBmpList));
#if 0    
    for (packageIdIterator =PTIN_IGMP_PACKAGE_BITMAP_SIZE-1; packageIdIterator>=0; --packageIdIterator)
    {
      osapiSnprintf(charPtr, sizeof(*charPtr),
                  "%08X", ptinEvcFlow.packageBmpList[packageIdIterator]);
      charPtr++;
    }
#endif        
    
    PT_LOG_DEBUG(LOG_CTX_MSG, "EVC# %u Flow",     ptinEvcFlow.evc_idx);    
    PT_LOG_DEBUG(LOG_CTX_MSG, " %s# %u",          ptinEvcFlow.ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
                                                    ptinEvcFlow.ptin_intf.intf_id);
    PT_LOG_DEBUG(LOG_CTX_MSG, " Int.IVID    = %u", ptinEvcFlow.int_ivid);
    PT_LOG_DEBUG(LOG_CTX_MSG, " UNI-OVID    = %u", ptinEvcFlow.uni_ovid);
    PT_LOG_DEBUG(LOG_CTX_MSG, " UNI-IVID    = %u", ptinEvcFlow.uni_ivid);
    PT_LOG_DEBUG(LOG_CTX_MSG, " OnuId        = %u", ptinEvcFlow.onuId);
    PT_LOG_DEBUG(LOG_CTX_MSG, " noOfPackages       = %u", ptinEvcFlow.noOfPackages);      
    PT_LOG_DEBUG(LOG_CTX_MSG, " packageBmpList:%s", packageBmpStr);


    if (ptinEvcFlow.noOfPackages >= 0)
    {
      if ((rc=ptin_evc_macbridge_client_packages_add(&ptinEvcFlow)) != L7_SUCCESS)
      {        
        PT_LOG_ERR(LOG_CTX_MSG, "Error adding EVC# %u flow rc:%u", ptinEvcFlow.evc_idx, rc);
        return rc;
      }
    }
  }
  return rc;
#else
  PT_LOG_ERR(LOG_CTX_IGMP, "Featured not supported in this card!");  
  return L7_NOT_SUPPORTED;  
#endif
}

/**
 * Macbridge Client Packages Add
 * 
 * @param msg          : Pointer to  a Message 
 * @param noOfMessages : Number of Messages
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t ptin_msg_igmp_macbridge_client_packages_remove(msg_igmp_macbridge_client_packages_t *msg, L7_uint32 noOfMessages)
{
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  L7_uint32        messageIterator;
//L7_int32         packageIdIterator;
  L7_char8         packageBmpStr[PTIN_SYSTEM_IGMP_MAXPACKAGES/(sizeof(L7_uint8)*8)-1]={};
//L7_char8        *charPtr           = packageBmpStr;
  ptin_evc_macbridge_client_packages_t ptinEvcFlow;  
  L7_RC_t          rc                = L7_SUCCESS;

  /* Input Argument validation */
  if ( msg  == L7_NULLPTR || noOfMessages == 0)
  {
    PT_LOG_ERR(LOG_CTX_IGMP, "Invalid arguments [msg:%p noOfMessages:%u]",msg, noOfMessages);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  PT_LOG_DEBUG(LOG_CTX_MSG,"Input Arguments [noOfMessages:%u]", noOfMessages);

  for (messageIterator = 0; messageIterator < noOfMessages; messageIterator++)
  {    
    /*Initialize Structure*/
    memset(&ptinEvcFlow, 0x00, sizeof(ptinEvcFlow));

    #if PTIN_BOARD_IS_ACTIVETH   
    if (ENDIAN_SWAP8(msg[messageIterator].onuId) != 0)
    {
      PT_LOG_WARN(LOG_CTX_MSG, "   I'm an Active Ethernet Card. OnuId:%u is different from 0. Going to set it to zero", ENDIAN_SWAP8([messageIterator].onuId));
      ENDIAN_SWAP8(msg[messageIterator].onuId) = 0;
    }    
    #endif

    /* Copy data */                                                                                                  
    ptinEvcFlow.evc_idx             = ENDIAN_SWAP32(msg[messageIterator].evcId);                                     
    ptinEvcFlow.int_ivid            = ENDIAN_SWAP16(msg[messageIterator].nni_cvlan);                                 
    ptinEvcFlow.ptin_intf.intf_type = ENDIAN_SWAP8(msg[messageIterator].intf.intf_type);                             
    ptinEvcFlow.ptin_intf.intf_id   = ENDIAN_SWAP8(msg[messageIterator].intf.intf_id);                               
    ptinEvcFlow.uni_ovid            = ENDIAN_SWAP16(msg[messageIterator].intf.outer_vid); /* must be a leaf */       
    ptinEvcFlow.uni_ivid            = ENDIAN_SWAP16(msg[messageIterator].intf.inner_vid);                            
    ptinEvcFlow.onuId               = ENDIAN_SWAP8(msg[messageIterator].onuId);                                      
    ptinEvcFlow.noOfPackages        = ENDIAN_SWAP16(msg[messageIterator].noOfPackages);     

    /*Copy Multicast Package Bitmap*/
    memcpy(ptinEvcFlow.packageBmpList, ENDIAN_SWAP32(msg[messageIterator].packageBmpList), sizeof(ptinEvcFlow.packageBmpList));
#if 0    
    for (packageIdIterator =PTIN_IGMP_PACKAGE_BITMAP_SIZE-1; packageIdIterator>=0; --packageIdIterator)
    {
      osapiSnprintf(charPtr, sizeof(*charPtr),
                  "%08X", ptinEvcFlow.packageBmpList[packageIdIterator]);
      charPtr++;
    }
#endif        

//  osapiSnprintf(aclName, sizeof(aclName), "%u", aclnum);
//}
//else
//{
//  osapiStrncpySafe(aclName, ptr->aclName, sizeof(aclName));
    
    PT_LOG_DEBUG(LOG_CTX_MSG, "EVC# %u Flow",     ptinEvcFlow.evc_idx);    
    PT_LOG_DEBUG(LOG_CTX_MSG, " %s# %u",          ptinEvcFlow.ptin_intf.intf_type == PTIN_EVC_INTF_PHYSICAL ? "PHY":"LAG",
                                                    ptinEvcFlow.ptin_intf.intf_id);
    PT_LOG_DEBUG(LOG_CTX_MSG, " Int.IVID    = %u", ptinEvcFlow.int_ivid);
    PT_LOG_DEBUG(LOG_CTX_MSG, " UNI-OVID    = %u", ptinEvcFlow.uni_ovid);
    PT_LOG_DEBUG(LOG_CTX_MSG, " UNI-IVID    = %u", ptinEvcFlow.uni_ivid);
    PT_LOG_DEBUG(LOG_CTX_MSG, " OnuId        = %u", ptinEvcFlow.onuId);
    PT_LOG_DEBUG(LOG_CTX_MSG, " noOfPackages       = %u", ptinEvcFlow.noOfPackages);      
    PT_LOG_DEBUG(LOG_CTX_MSG, " packageBmpList:%s", packageBmpStr);


    if (ptinEvcFlow.noOfPackages > 0)
    {
      if ((rc=ptin_evc_macbridge_client_packages_remove(&ptinEvcFlow)) != L7_SUCCESS)
      {
        if (rc != L7_NOT_EXIST)
        {
          PT_LOG_ERR(LOG_CTX_MSG, "Error removing EVC# %u flow", ptinEvcFlow.evc_idx);
          return rc;
        }
        else
        {          
          //Warning already logged          
          rc = L7_SUCCESS;
        }
      }
    }
  }
  return rc;
#else
  PT_LOG_ERR(LOG_CTX_IGMP, "Featured not supported in this card!");  
  return L7_NOT_SUPPORTED;  
#endif
}

/**
 * Multicast Service Add
 * 
 * @param msg          : Pointer to  a Message 
 * @param noOfMessages : Number of Messages
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t ptin_msg_igmp_multicast_service_add(msg_multicast_service_t *msg, L7_uint32 noOfMessages)
{
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  L7_uint32         messageIterator; 
  ptin_intf_t       ptinIntf;
  L7_uint32         ptinPort;  
  L7_RC_t           rc                = L7_SUCCESS;

  /* Input Argument validation */
  if ( msg  == L7_NULLPTR || noOfMessages == 0)
  {
    PT_LOG_ERR(LOG_CTX_IGMP, "Invalid arguments [msg:%p noOfMessages:%u]",msg, noOfMessages);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  PT_LOG_DEBUG(LOG_CTX_MSG,"Input Arguments [msg:%p noOfMessages:%u]", msg, noOfMessages);

  for (messageIterator = 0; messageIterator < noOfMessages; messageIterator++)
  { 
    /*Input Parameters*/
    PT_LOG_DEBUG(LOG_CTX_MSG, "Input Arguments [slotId:%u evcId:%u intf.type:%u intf.id:%u onuId:%u]",
              ENDIAN_SWAP8(msg[messageIterator].slotId), ENDIAN_SWAP32(msg[messageIterator].evcId), ENDIAN_SWAP8(msg[messageIterator].intf.intf_type), ENDIAN_SWAP8(msg[messageIterator].intf.intf_id), ENDIAN_SWAP8(msg[messageIterator].onuId));

    /*Copy to ptin intf struct*/
    ptinIntf.intf_type = ENDIAN_SWAP8(msg[messageIterator].intf.intf_type);
    ptinIntf.intf_id   = ENDIAN_SWAP8(msg[messageIterator].intf.intf_id);

    /*Convert from ptin intf to otin port*/
    if ( L7_SUCCESS != (rc = ptin_intf_ptintf2port(&ptinIntf, &ptinPort) ) )
    {
      return rc;
    }

    /*If Any Error Occurs It is Already Logged*/
    if ( L7_SUCCESS != (rc = ptin_igmp_multicast_service_add(ptinPort, ENDIAN_SWAP8(msg[messageIterator].onuId), ENDIAN_SWAP32(msg[messageIterator].evcId))) )
    {
      return rc;
    }   
  }
  return rc;  
#else
  PT_LOG_ERR(LOG_CTX_IGMP, "Featured not supported on this card!");  
  return L7_NOT_SUPPORTED;  
#endif
}

/**
 * Multicast Service Remove
 * 
 * @param msg          : Pointer to  a Message 
 * @param noOfMessages : Number of Messages
 * 
 * @return L7_RC_t : 
 *         L7_SUCCESS/L7_FAILURE/L7_NOT_EXIST/L7_DEPENDENCY_NOT_MET
 */
L7_RC_t ptin_msg_igmp_multicast_service_remove(msg_multicast_service_t *msg, L7_uint32 noOfMessages)
{
#ifdef IGMPASSOC_MULTI_MC_SUPPORTED
  L7_uint32         messageIterator; 
  ptin_intf_t       ptinIntf;
  L7_uint32         ptinPort;
  L7_RC_t           rc                = L7_SUCCESS;

  /* Input Argument validation */
  if ( msg  == L7_NULLPTR || noOfMessages == 0)
  {
    PT_LOG_ERR(LOG_CTX_IGMP, "Invalid arguments [msg:%p noOfMessages:%u]",msg, noOfMessages);    
    return L7_FAILURE;
  }

  /*Input Parameters*/
  PT_LOG_DEBUG(LOG_CTX_MSG,"Input Arguments [msg:%p noOfMessages:%u]", msg, noOfMessages);

  for (messageIterator = 0; messageIterator < noOfMessages; messageIterator++)
  { 
    /*Input Parameters*/
    PT_LOG_DEBUG(LOG_CTX_MSG, "Input Arguments [slotId:%u evcId:%u intf.type:%u intf.id:%u onuId:%u]",
              ENDIAN_SWAP8(msg[messageIterator].slotId),ENDIAN_SWAP32(msg[messageIterator].evcId), ENDIAN_SWAP8(msg[messageIterator].intf.intf_type), ENDIAN_SWAP8(msg[messageIterator].intf.intf_id), ENDIAN_SWAP8(msg[messageIterator].onuId));

    /*Copy to ptin intf struct*/
    ptinIntf.intf_type = ENDIAN_SWAP8(msg[messageIterator].intf.intf_type);
    ptinIntf.intf_id   = ENDIAN_SWAP8(msg[messageIterator].intf.intf_id);

    /*Convert from ptin intf to otin port*/
    if ( L7_SUCCESS != (rc = ptin_intf_ptintf2port(&ptinIntf, &ptinPort) ) )
    {
      PT_LOG_ERR(LOG_CTX_IGMP, "Failed to convert to ptin port [slotId:%u evcId:%u intf.type:%u intf.id:%u onuId:%u]");  
      return rc;
    }

    /*If Any Error Occurs It is Already Logged*/
    if ( L7_SUCCESS != (rc = ptin_igmp_multicast_service_remove(ptinPort, ENDIAN_SWAP8(msg[messageIterator].onuId), ENDIAN_SWAP32(msg[messageIterator].evcId)) ) )
    {
      return rc;
    }
  }
  return rc;  
#else
  PT_LOG_ERR(LOG_CTX_IGMP, "Featured not supported on this card!");  
  return L7_NOT_SUPPORTED;  
#endif
}

/****************************************End Multicast Package Feature**************************************************/









#define DTL0_STRING     PTIN_ROUTING_DTL0_INTERFACE_NAME
//#define DTL0_STRING     "dtl0"
#define ETH1_STRING     "eth1"
extern L7_int dtlVlanIfAdd(L7_uint16 vlanId);//#include <os/linux/mgmt/dtl_net.h>

int ptin_msg_PTP_lnx_net_if_set(ipc_msg *inbuffer, ipc_msg *outbuffer) {
T_MSG_PTP_LNX_NET_IF_SET *ib;
L7_uint32 ip, msk,
          intIfNum;
#if (!PTIN_BOARD_IS_STANDALONE)
L7_uint16 internalVid;
#endif
unsigned long i;
char ifName[L7_NIM_IFNAME_SIZE], com[L7_NIM_IFNAME_SIZE+80];
BOOL enable;
#if (!PTIN_BOARD_IS_STANDALONE)
L7_RC_t rc;
#endif

        ib=(T_MSG_PTP_LNX_NET_IF_SET *)inbuffer->info;
        enable= ib->add1_del0?1:0;//CCMSG_PTP_LNX_NET_IF_SET==inbuffer->msgId?1:0;

        switch (ib->encap) {
        default:    return ERROR_CODE_INVALIDPARAM;
        case TS_ENCAP_ETH_IPv4_PTP:
            if (L7_SUCCESS!=ptin_intf_port2intIfNum(ib->board_port, &intIfNum)) {
                PT_LOG_ERR(LOG_CTX_MSG,"ptin_intf_port2intIfNum");
                return ERROR_CODE_INVALIDPARAM;
            }

#if (!PTIN_BOARD_IS_STANDALONE)
            if (L7_SUCCESS!=ptin_xlate_ingress_get(intIfNum, ib->vid, PTIN_XLATE_NOT_DEFINED, &internalVid, L7_NULLPTR)) {
                PT_LOG_ERR(LOG_CTX_MSG,"ptin_xlate_ingress_get");
                return ERROR_CODE_INVALIDPARAM;
            }
#endif

#if (PTIN_BOARD_IS_STANDALONE)
            sprintf(ifName, "%s.%d", ETH1_STRING, ib->dtl0vid);
#else
            sprintf(ifName, "%s.%d", DTL0_STRING, ib->dtl0vid);
#endif
            for (i=0, ip=0, msk=0; i<4; i++) {
                ip<<=8;
                ip|=ib->IP[i];
                msk<<=8;
                msk|=ib->IPmsk[i];
            }

            if (enable) {
#if (PTIN_BOARD_IS_STANDALONE)
                sprintf(com, "vconfig add %s %d\n", ETH1_STRING, ib->dtl0vid);
                PT_LOG_NOTICE(LOG_CTX_MSG, com);
                system(com);
                if (-1==system(com)) {
                    PT_LOG_ERR(LOG_CTX_MSG,"vconfig add %s", ETH1_STRING);
                    return ERROR_CODE_INVALIDPARAM;
                }
                if (L7_SUCCESS!=osapiIfEnable(ETH1_STRING)) {
#else
                if (L7_SUCCESS!=dtlVlanIfAdd(ib->dtl0vid)) {
                    PT_LOG_ERR(LOG_CTX_MSG,"dtlVlanIfAdd");
                    return ERROR_CODE_INVALIDPARAM;
                }
                if (L7_SUCCESS!=osapiIfEnable(DTL0_STRING)) {
#endif
                    PT_LOG_ERR(LOG_CTX_MSG,"osapiIfEnable(ifName=%s)", ifName);
                    return ERROR_CODE_INVALIDPARAM;
                }
                if (L7_SUCCESS!=osapiIfEnable(ifName)) {
                    PT_LOG_ERR(LOG_CTX_MSG,"osapiIfEnable(ifName=%s)", ifName);
                    return ERROR_CODE_INVALIDPARAM;
                }
                if (L7_SUCCESS!=osapiNetIfConfig(ifName, ip, msk)) {
                    PT_LOG_ERR(LOG_CTX_MSG,"osapiNetIfConfig(ifName=%s, ip=0x%lx, msk=0x%lx)", ifName, ip, msk);
                    return ERROR_CODE_INVALIDPARAM;
                }
                //sprintf(com, "ifconfig %s %d.%d.%d.%d netmask %d.%d.%d.%d up\n", ifName,
                //                ib->IP[0], ib->IP[1], ib->IP[2], ib->IP[3],
                //                ib->IPmsk[0], ib->IPmsk[1], ib->IPmsk[2], ib->IPmsk[3]);
                //PT_LOG_NOTICE(LOG_CTX_MSG, com);
                //system(com);
            }

#if (!PTIN_BOARD_IS_STANDALONE)
            rc = ptin_ipdtl0_control(ib->dtl0vid, ib->vid, internalVid, intIfNum, PTIN_IPDTL0_ETH_IPv4_UDP_PTP, enable);
            if (L7_SUCCESS!=rc) {
                PT_LOG_ERR(LOG_CTX_MSG,"ptin_ipdtl0_control(ib->dtl0vid=%u, ib->vid=%u, internalVid=%u, intIfNum=%lu, PTIN_IPDTL0_ETH_IPv4_UDP_PTP, enable=%u)=%d",
                                            ib->dtl0vid, ib->vid, internalVid, intIfNum, PTIN_IPDTL0_ETH_IPv4_UDP_PTP, enable, rc);
                return ERROR_CODE_INVALIDPARAM;
            }
#endif

            if (!enable) {
                sprintf(com, "vconfig rem %s\n", ifName);
                PT_LOG_NOTICE(LOG_CTX_MSG, com);
                system(com);
            }

#if ( PTIN_BOARD == PTIN_BOARD_TA48GE )
            //time_interface_enable();
            rc = dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_TIME_INTERFACE, DAPI_CMD_GET, sizeof(T_MSG_PTP_LNX_NET_IF_SET), ib);
            if (rc != L7_SUCCESS) {
              PT_LOG_ERR(LOG_CTX_MSG,"time_interface_enable()=%d", rc);
              return ERROR_CODE_INVALIDPARAM;
            }
#endif
            break;
        }//switch (ib->encap)

        return ERROR_CODE_OK;
}//ptin_msg_PTP_lnx_net_if_set

