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
#include "dhcp_snooping_api.h"

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
  PT_LOG_INFO(LOG_CTX_INTF, "Initializing interface type-b protection configurations");
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
 * @note: These configurations will be applied to the interface 
 *        indexed by data.ptin_port
 */
L7_RC_t ptin_prottypeb_intf_config_set(ptin_prottypeb_intf_config_t* data)
{
  L7_uint32 ptin_port;
 
  /* Check input */
  if(data==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid context [data:%p]", data);
    return L7_FAILURE;
  }

  /* Ensure the requested interface is valid */
  ptin_port = data->ptin_port;
  if(ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid ptin_port %u", data->ptin_port);
    return L7_FAILURE;
  }

  /* Ensure that we do not modify the 'status' variable. Only the CCMSG_TYPEB_PROT_SWITCH_NOTIFY message is allowed to do that */
  data->status = prottypeb_interfaces[ptin_port].status;

  ENDIAN_SWAP32_MOD(data->slotId);
  ENDIAN_SWAP32_MOD(data->ptin_port);
  ENDIAN_SWAP32_MOD(data->pairPtinPort);

  /* Return the configurations for the desired interface */
  PT_LOG_DEBUG(LOG_CTX_INTF, "Setting ptin_port[%u] type-b protection configurations", ptin_port);
  PT_LOG_TRACE(LOG_CTX_INTF, "Configurations:");
  PT_LOG_TRACE(LOG_CTX_INTF, "    slotId      : %u", data->slotId);
  PT_LOG_TRACE(LOG_CTX_INTF, "    ptin_port   : %u", data->ptin_port);
  PT_LOG_TRACE(LOG_CTX_INTF, "    intfRole    : %u", data->intfRole);
  PT_LOG_TRACE(LOG_CTX_INTF, "    status      : %u", data->status);
  PT_LOG_TRACE(LOG_CTX_INTF, "    pairSlotId  : %u", data->pairSlotId);
  PT_LOG_TRACE(LOG_CTX_INTF, "    pairPtinPort: %u", data->pairPtinPort);
  memcpy(&prottypeb_interfaces[ptin_port], data, sizeof(ptin_prottypeb_intf_config_t));

#if !PTIN_BOARD_IS_MATRIX
  {
    msg_SnoopSyncRequest_t   snoopSyncRequest={0};
    L7_uint32 ipAddr;

    if(prottypeb_interfaces[ptin_port].status==L7_ENABLE)
    {
      PT_LOG_NOTICE(LOG_CTX_MSG, "Not sending a Snoop Sync Request Message to Sync the  Snoop Entries. I'm a Working slotId/ptin_port:%u/%u",
                    prottypeb_interfaces[ptin_port].slotId, prottypeb_interfaces[ptin_port].ptin_port);
      return L7_SUCCESS;
    }

    /* FIXME TC16SXG: mgmd portId */
    snoopSyncRequest.portId = prottypeb_interfaces[ptin_port].pairPtinPort;

    #if PTIN_BOARD_IS_STANDALONE
      ipAddr = simGetIpcIpAddr();
    #else
      /* Determine the IP address of the working port/slot */
      if (L7_SUCCESS != ptin_fpga_slot_ip_addr_get(prottypeb_interfaces[ptin_port].pairSlotId, &ipAddr))
      {
        PT_LOG_ERR(LOG_CTX_INTF, "Failed to obtain ipAddress of slotId:%u", prottypeb_interfaces[ptin_port].pairSlotId);
        return L7_FAILURE;
      }
    #endif      

      PT_LOG_DEBUG(LOG_CTX_MSG, "Sending a Snoop Sync Request Message to ipAddr:%08X to Sync the Snoop Entries of remote slotId/ptin_port:%u/%u",
                   ipAddr, prottypeb_interfaces[ptin_port].pairSlotId, prottypeb_interfaces[ptin_port].pairPtinPort);
    /*Send the snoop sync request to the protection matrix */  
    if (send_ipc_message(IPC_HW_FASTPATH_PORT, ipAddr, CCMSG_MGMD_SNOOP_SYNC_REQUEST,
                         (char *)(&snoopSyncRequest), NULL,
                         sizeof(snoopSyncRequest), NULL) != 0)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Failed to send Snoop Sync Request Message");
      return L7_FAILURE;
    }  
  }
#endif
  return L7_SUCCESS;
}

/**
 * Get type-b protection interface configurations
 * 
 * @param ptin_port : ID of the desired interface
 * @param data      : Struct where interface configuration will 
 *                    be written
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_prottypeb_intf_config_get(L7_uint32 ptin_port, ptin_prottypeb_intf_config_t* data)
{
  /* Check input */
  if(data==L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid context [data:%p]", data);
    return L7_FAILURE;
  }

  /* Ensure the requested interface is valid */
  if(ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }

  /* Return the configurations for the desired interface */
  PT_LOG_DEBUG(LOG_CTX_INTF, "Getting ptin_port[%u] type-b protection configurations", ptin_port);
  memcpy(data, &prottypeb_interfaces[ptin_port], sizeof(ptin_prottypeb_intf_config_t));
  PT_LOG_TRACE(LOG_CTX_INTF, "Configurations:");
  PT_LOG_TRACE(LOG_CTX_INTF, "    ptin_port   : %u", data->ptin_port);
  PT_LOG_TRACE(LOG_CTX_INTF, "    intfRole    : %u", data->intfRole);
  PT_LOG_TRACE(LOG_CTX_INTF, "    status      : %u", data->status);     
  PT_LOG_TRACE(LOG_CTX_INTF, "    pairSlotId  : %u", data->pairSlotId);
  PT_LOG_TRACE(LOG_CTX_INTF, "    pairPtinPort: %u", data->pairPtinPort);

  return L7_SUCCESS;
}

/**
 * Get type-b protection interface configurations
 * 
 * @param ptin_port: ID of the desired interface
 * @param status  : Status (1-active; 0-inactive)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_prottypeb_intf_switch_notify(L7_uint32 ptin_port, L7_uint8 status)
{
  L7_uint8 previousStatus;


  /* Ensure the requested interface is valid */
  if(ptin_port >= PTIN_SYSTEM_N_INTERF || (status | L7_ENABLE) != L7_ENABLE)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid ptin_port[%u] status[%u]", ptin_port, status);
    return L7_FAILURE;
  }


  /*Save Previous Status to Further Use*/
  previousStatus = prottypeb_interfaces[ptin_port].status;


  if (previousStatus == status)
  {

    return L7_SUCCESS;
  }

  /*Assign New Status*/
  prottypeb_interfaces[ptin_port].status = status;


  if (prottypeb_interfaces[ptin_port].intfRole != PROT_TYPEB_ROLE_NONE)
  {

    if (status == L7_ENABLE)
    {
      /* Reset MGMD General Querier state */    
      if (ptin_igmp_generalquerier_reset((L7_uint32) -1, (L7_uint32) -1)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Unable to reset MGMD General Queriers");
      }
    }
    else if (status == L7_DISABLE)
    {
      /* Remove Port from the the MGMD Control Plane*/
      /* FIXME TC16SXG: intIfNum->ptin_port */
      if ( ptin_igmp_mgmd_port_remove((L7_uint32) -1, ptin_port)!=L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Unable to remove protection port from MGMD Control Plane");
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_EVC, "Removed Mgmd Port [ptin_port=%u]", ptin_port);
      }
      /* FIXME TC16SXG: intIfNum->ptin_port */
      dsBindingClear(ptin_port /*intIfNum*/);
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
      printf("ptin_port: %u\n", prottypeb_interfaces[i].ptin_port);
      printf("\tintfRole    : %u\n", prottypeb_interfaces[i].intfRole);
      printf("\tstatus      : %u\n", prottypeb_interfaces[i].status);     
      printf("\tpairSlotId  : %u\n", prottypeb_interfaces[i].pairSlotId);
      printf("\tpairPtinPort: %u\n", prottypeb_interfaces[i].pairPtinPort);
    }
  }
}

