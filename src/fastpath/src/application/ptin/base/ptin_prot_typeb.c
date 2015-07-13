/**
 * ptin_prot_typeb.c
 *  
 * Implements the Type-B Protection interface module
 *
 * Created on: 2014/02/26
 * Author: Daniel Figueira
 *  
 * Notes:
 */

#include "ptin_prot_typeb.h"
#include "ptin_msghandler.h"
#include "ptin_igmp.h"
#include "ptin_fpga_api.h"

/*********************************************************** 
 * Defines
 ***********************************************************/

/*********************************************************** 
 * Typedefs
 ***********************************************************/

/*********************************************************** 
 * Data structs
 ***********************************************************/

/* Interfaces configured with Type-b Protection */
ptin_prottypeb_intf_config_t prottypeb_interfaces[PTIN_SYSTEM_N_INTERF];

/*********************************************************** 
 * Static prototypes
 ***********************************************************/

/*********************************************************** 
 * INLINE FUNCTIONS
 ***********************************************************/

/*********************************************************** 
 * Global functions
 ***********************************************************/

/**
 * Initialization
 *  
 * @return none
 */
L7_RC_t ptin_prottypeb_init(void)
{
  /* Initialize the interface configurations */
  LOG_INFO(LOG_CTX_PTIN_PROTB, "Initializing interface type-b protection configurations");
  memset(prottypeb_interfaces, 0x00, sizeof(prottypeb_interfaces));

  return L7_SUCCESS;
}

/**
 * Set type-b protection interface configurations
 * 
 * @param data : Struct where interface configuration will be written
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note: These configurations will be applied to the interface indexed by data.intfNum
 */
L7_RC_t ptin_prottypeb_intf_config_set(ptin_prottypeb_intf_config_t* data)
{
  L7_uint32 intfNum;
 
  /* Check input */
  if(data==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Invalid context [data:%p]", data);
    return L7_FAILURE;
  }

  /* Ensure the requested interface is valid */
  intfNum = data->intfNum;
  if(intfNum==0 || intfNum>=PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Invalid intfNum[%u]", data->intfNum);
    return L7_FAILURE;
  }

  /* Ensure that we do not modify the 'status' variable. Only the CCMSG_TYPEB_PROT_SWITCH_NOTIFY message is allowed to do that */
  data->status = prottypeb_interfaces[intfNum-1].status;

  /* Return the configurations for the desired interface */
  LOG_DEBUG(LOG_CTX_PTIN_PROTB, "Setting intfNum[%u] type-b protection configurations", intfNum);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "Configurations:");
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    slotId     : %u", data->slotId);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    intfNum    : %u", data->intfNum);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    intfRole   : %u", data->intfRole);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    status     : %u", data->status);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    pairSlotId : %u", data->pairSlotId);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    pairIntfNum: %u", data->pairIntfNum);
  memcpy(&prottypeb_interfaces[intfNum-1], data, sizeof(ptin_prottypeb_intf_config_t));

#if !PTIN_BOARD_IS_MATRIX
  if(prottypeb_interfaces[intfNum-1].status==L7_ENABLE)
  {
    LOG_NOTICE(LOG_CTX_PTIN_MSG, "Not sending a Snoop Sync Request Message to Sync the  Snoop Entries. I'm a Working slotId/intfNum:%u/%u",prottypeb_interfaces[intfNum-1].slotId, prottypeb_interfaces[intfNum-1].intfNum);
    return L7_SUCCESS;
  }

  msg_SnoopSyncRequest_t   snoopSyncRequest={0};

  snoopSyncRequest.portId=prottypeb_interfaces[intfNum-1].pairIntfNum;

  L7_uint32 ipAddr;

  #if PTIN_BOARD_IS_STANDALONE
    ipAddr = simGetIpcIpAddr();
  #else
    /* Determine the IP address of the working port/slot */
    if (L7_SUCCESS != ptin_fpga_slot_ip_addr_get(prottypeb_interfaces[intfNum-1].pairSlotId, &ipAddr))
    {
      LOG_ERR(LOG_CTX_PTIN_PROTB, "Failed to obtain ipAddress of slotId:%u", prottypeb_interfaces[intfNum-1].pairSlotId);
      return L7_FAILURE;
    }
  #endif      

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Sending a Snoop Sync Request Message to ipAddr:%08X to Sync the Snoop Entries of remote slotId/intfNum:%u/%u", ipAddr,prottypeb_interfaces[intfNum-1].pairSlotId, prottypeb_interfaces[intfNum-1].pairIntfNum);
  /*Send the snoop sync request to the protection matrix */  
  if (send_ipc_message(IPC_HW_FASTPATH_PORT, ipAddr, CCMSG_MGMD_SNOOP_SYNC_REQUEST, (char *)(&snoopSyncRequest), NULL, sizeof(snoopSyncRequest), NULL) < 0)
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Failed to send Snoop Sync Request Message");
    return L7_FAILURE;
  }  
#endif
  return L7_SUCCESS;
}

/**
 * Get type-b protection interface configurations
 * 
 * @param intfNum : ID of the desired interface
 * @param data    : Struct where interface configuration will be written
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_prottypeb_intf_config_get(L7_uint32 intfNum, ptin_prottypeb_intf_config_t* data)
{
  /* Check input */
  if(data==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Invalid context [data:%p]", data);
    return L7_FAILURE;
  }

  /* Ensure the requested interface is valid */
  if(intfNum==0 || intfNum>=PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Invalid intfNum[%u]", intfNum);
    return L7_FAILURE;
  }

  /* Return the configurations for the desired interface */
  LOG_DEBUG(LOG_CTX_PTIN_PROTB, "Getting intfNum[%u] type-b protection configurations", intfNum);
  memcpy(data, &prottypeb_interfaces[intfNum-1], sizeof(ptin_prottypeb_intf_config_t));
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "Configurations:");
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    intfNum    : %u", data->intfNum);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    intfRole   : %u", data->intfRole);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    status     : %u", data->status);     
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    pairSlotId : %u", data->pairSlotId);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    pairIntfNum: %u", data->pairIntfNum);

  return L7_SUCCESS;
}

/**
 * Get type-b protection interface configurations
 * 
 * @param intfNum : ID of the desired interface
 * @param status  : Status (1-active; 0-inactive)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_prottypeb_intf_switch_notify(L7_uint32 intfNum, L7_uint8 status)
{
  L7_uint8 previousStatus;

  /* Ensure the requested interface is valid */
  if(intfNum==0 || intfNum>=PTIN_SYSTEM_N_INTERF || (status | L7_ENABLE) != L7_ENABLE)
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Invalid intfNum[%u] status[%u]", intfNum, status);
    return L7_FAILURE;
  }

  /*Save Previous Status to Further Use*/
  previousStatus = prottypeb_interfaces[intfNum-1].status;

  if (previousStatus == status)
  {
    return L7_SUCCESS;
  }

  /*Assign New Status*/
  prottypeb_interfaces[intfNum-1].status = status;

  if (prottypeb_interfaces[intfNum-1].intfRole != PROT_TYPEB_ROLE_NONE)
  {
    if (status == L7_ENABLE)
    {
      /* Reset MGMD General Querier state */    
      if (ptin_igmp_generalquerier_reset()!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to reset MGMD General Queriers");
      }
    }
    else if (status == L7_DISABLE)
    {
      /* Remove Port from the the MGMD Control Plane*/    
      if ( ptin_igmp_mgmd_port_remove((L7_uint32) -1, intfNum)!=L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "Unable to remove protection port from MGMD Control Plane");
      }     
    }
  }
  
  return L7_SUCCESS;
}

/**
 * Dump type-b protection interface configurations
 */
void ptin_prottypeb_intf_config_dump(void)
{
  L7_uint i;

  printf("Type-B Protection interface configurations dump:\n");
  for(i=0; i<PTIN_SYSTEM_N_INTERF; ++i)
  {
    if(prottypeb_interfaces[i].intfRole != PROT_TYPEB_ROLE_NONE)
    {
      printf("intfNum: %u\n", prottypeb_interfaces[i].intfNum);
      printf("\tintfRole   : %u\n", prottypeb_interfaces[i].intfRole);
      printf("\tstatus     : %u\n", prottypeb_interfaces[i].status);     
      printf("\tpairSlotId : %u\n", prottypeb_interfaces[i].pairSlotId);
      printf("\tpairIntfNum: %u\n", prottypeb_interfaces[i].pairIntfNum);
    }
  }
}

