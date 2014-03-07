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

  /* Return the configurations for the desired interface */
  LOG_DEBUG(LOG_CTX_PTIN_PROTB, "Setting intfNum[%u] type-b protection configurations", intfNum);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "Configurations:");
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    intfNum    : %u", data->intfNum);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    intfRole   : %u", data->intfRole);
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
    LOG_ERR(LOG_CTX_PTIN_PROTB, "Invalid intfNum[%u]", data->intfNum);
    return L7_FAILURE;
  }

  /* Return the configurations for the desired interface */
  LOG_DEBUG(LOG_CTX_PTIN_PROTB, "Getting intfNum[%u] type-b protection configurations", intfNum);
  memcpy(data, &prottypeb_interfaces[intfNum], sizeof(ptin_prottypeb_intf_config_t));
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "Configurations:");
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    intfNum    : %u", data->intfNum);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    intfRole   : %u", data->intfRole);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    pairSlotId : %u", data->pairSlotId);
  LOG_TRACE(LOG_CTX_PTIN_PROTB, "    pairIntfNum: %u", data->pairIntfNum);

  return L7_SUCCESS;
}

