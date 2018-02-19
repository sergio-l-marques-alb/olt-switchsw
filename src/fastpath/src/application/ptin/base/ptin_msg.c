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
#include "ptin_cfg.h"
#include "ptin_control.h"
#include "tty_ptin.h"
#include "ipc.h"
#include "ptin_msghandler.h"
#include "ptin_cnfgr.h"
#include "nimapi.h"
#include "fdb_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_dot3ad_api.h"
#include "mirror_api.h"
#include "usmdb_dvlantag_api.h"
#include "usmdb_mirror_api.h"
#include "ptin_status.h"

#ifndef SNOOPING_API_H
#include "snooping_api.h" //To interact with SNOOP
#endif

#ifndef DHCP_SNOOPING_API_H
#include "dhcp_snooping_api.h"//To interact with IPSG
#endif

#include "ptin_fpga_api.h"//To interact with CPLD register

#include <dtl_ptin.h>

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
static L7_RC_t ptin_shell_command_run(L7_char8 *tty, L7_char8 *type, L7_char8 *cmd);

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

#if 0
  /*This Should be the First Module*/
  /* Reset IGMP Module */
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on IGMP...");
  ptin_igmp_default_reset();

   /* Reset DAI Module */
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on DAI...");
  daiRestore();

#ifdef __Y1731_802_1ag_OAM_ETH__
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on OAM...");
  eth_srv_oam_msg_defaults_reset();
#endif

  /* Reset Routing Module*/
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on Routing...");
  ptin_routing_intf_remove_all();

  /* ERPS */
#ifdef PTIN_ENABLE_ERPS
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on ERPS...");
  ptin_erps_clear();
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on HAL...");
  ptin_hal_erps_clear();
#endif

  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on ACL...");
  ptin_aclCleanAll();

  /* Reset EVC Module */
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on EVC...");
  ptin_evc_destroy_all();

  /* Reset Mirror */
  PT_LOG_INFO(LOG_CTX_MSG, "Performing Mirror Reset...");
  ptin_mirror_reset();

  /* Reset NGPON2 groups and ports */
#ifdef NGPON2_SUPPORTED
  PT_LOG_INFO(LOG_CTX_MSG, "Performing NGPON2 reset...");
  ptin_intf_NGPON2_clear();
  ptin_msg_NGPON2_clear();
#endif

  /* Remove prot uplink configuration  */
  ptin_prot_uplink_clear_all();

  if (mode == DEFAULT_RESET_MODE_FULL)
  {
    ptin_NtwConnectivity_t ptinNtwConn;

    /* Unconfig Connectivity */
    memset(&ptinNtwConn, 0x00, sizeof(ptin_NtwConnectivity_t));
    ptinNtwConn.mask = PTIN_NTWCONN_MASK_IPADDR;
    PT_LOG_INFO(LOG_CTX_MSG, "(Re)Configure Inband...");
    ptin_cfg_ntw_connectivity_set(&ptinNtwConn);

    /*This Should be the Last Module*/
    PT_LOG_INFO(LOG_CTX_MSG, "Performing Reset on LAG...");
    ptin_intf_Lag_delete_all();
  }
#endif
  PT_LOG_INFO(LOG_CTX_MSG, "Complete!");
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


/******************************************************************************
 * STATIC FUNCTIONS IMPLEMENTATION
 ******************************************************************************/
#if 0
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
#endif

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
 * Read all temperature sensors
 * 
 * @param msg 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_msg_switch_temperature_get(msg_ptin_temperature_monitor_t *msg)
{
  L7_uint8  i;
  L7_RC_t   rc;
  ptin_dtl_temperature_monitor_t temp_info;

  memset(&temp_info, 0x00, sizeof(ptin_dtl_temperature_monitor_t));
  temp_info.index = 0;
  temp_info.number_of_sensors = -1; /* All sensors to be read */

  rc = ptin_status_temperature_monitor(&temp_info);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG,"Error reading temperature sensors! (rc=%d)", rc);
    return rc;
  }
 
  PT_LOG_TRACE(LOG_CTX_MSG, "Index=%d", temp_info.index);
  PT_LOG_TRACE(LOG_CTX_MSG, "Number of sensors=%d", temp_info.number_of_sensors);

  /* Copy results to message */
  if (temp_info.number_of_sensors > 0)
  {
    for (i = 0; i < temp_info.number_of_sensors; i++) 
    {
      PT_LOG_TRACE(LOG_CTX_MSG, "Sensor %u: %d/%d", i, temp_info.sensors_data[i].curr_value, temp_info.sensors_data[i].peak_value);

      msg->sensors_data[i].curr_value = ENDIAN_SWAP16(temp_info.sensors_data[i].curr_value);
      msg->sensors_data[i].peak_value = ENDIAN_SWAP16(temp_info.sensors_data[i].peak_value);
    }
    msg->number_of_sensors = ENDIAN_SWAP16(temp_info.number_of_sensors);
  }
  msg->index  = ENDIAN_SWAP16(temp_info.index);
  msg->SlotId = ENDIAN_SWAP8(ptin_fpga_board_slot_get());

  return L7_SUCCESS; 
}



/**************** test functions ****************/

