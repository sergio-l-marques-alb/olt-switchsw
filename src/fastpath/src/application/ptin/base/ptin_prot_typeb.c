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
  if(intfNum>=PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Invalid intfNum[%u]", data->intfNum);
    return L7_FAILURE;
  }

  /* Set interface status based on its role */
  if(data->intfRole == PROT_TYPEB_ROLE_WORKING)
  {
    data->status = L7_ENABLE;
  }
  else if(data->intfRole == PROT_TYPEB_ROLE_PROTECTION)
  {
    data->status = L7_DISABLE;
  }

  /* Return the configurations for the desired interface */
  LOG_DEBUG(LOG_CTX_PTIN_PROTB, "Setting intfNum[%u] type-b protection configurations", intfNum);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "Configurations:");
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    intfNum    : %u", data->intfNum);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    intfRole   : %u", data->intfRole);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    status     : %u", data->status);     
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    pairSlotId : %u", data->pairSlotId);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    pairIntfNum: %u", data->pairIntfNum);
  memcpy(&prottypeb_interfaces[intfNum], data, sizeof(ptin_prottypeb_intf_config_t));

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
  if(intfNum>=PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Invalid intfNum[%u]", intfNum);
    return L7_FAILURE;
  }

  /* Return the configurations for the desired interface */
  LOG_DEBUG(LOG_CTX_PTIN_PROTB, "Getting intfNum[%u] type-b protection configurations", intfNum);
  memcpy(data, &prottypeb_interfaces[intfNum], sizeof(ptin_prottypeb_intf_config_t));
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
  /* Ensure the requested interface is valid */
  if(intfNum>=PTIN_SYSTEM_N_INTERF)
  {
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Invalid intfNum[%u]", intfNum);
    return L7_FAILURE;
  }

  prottypeb_interfaces[intfNum].status = status;

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

