/**
 * ptin_prot_typeb.h
 *  
 * Implements the Type-B Protection interface module
 *
 * Created on: 2014/02/26
 * Author: Daniel Figueira
 *  
 * Notes:
 */

#ifndef _PTIN_PROT_TYPEB_H
#define _PTIN_PROT_TYPEB_H

#include "ptin_include.h"

/*********************************************************** 
 * Defines
 ***********************************************************/

#define PROT_TYPEB_ROLE_NONE        0
#define PROT_TYPEB_ROLE_WORKING     1
#define PROT_TYPEB_ROLE_PROTECTION  2

/***********************************************************
 * Typedefs
 ***********************************************************/

typedef struct {
  L7_uint32 intfNum;
  L7_uint8  intfRole; /* [PROT_TYPEB_ROLE_NONE, PROT_TYPEB_ROLE_WORKING, PROT_TYPEB_ROLE_PROTECTION] */
  L7_uint8  status;   /* [1-active; 0-inactive] */

  L7_uint8  pairSlotId;
  L7_uint32 pairIntfNum;
} ptin_prottypeb_intf_config_t;

/*********************************************************** 
 * External function prototypes
 ***********************************************************/

/**
 * Initialization
 *  
 * @return none
 */
extern L7_RC_t ptin_prottypeb_init(void);

/**
 * Set type-b protection interface configurations
 * 
 * @param data : Struct where interface configuration will be written
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE 
 *  
 * @note: These configurations will be applied to the interface indexed by data.intfNum
 */
extern L7_RC_t ptin_prottypeb_intf_config_set(ptin_prottypeb_intf_config_t* data);

/**
 * Get type-b protection interface configurations
 * 
 * @param intfNum : ID of the desired interface
 * @param data    : Struct where interface configuration will be written
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_prottypeb_intf_config_get(L7_uint32 intfNum, ptin_prottypeb_intf_config_t* data);

/**
 * Get type-b protection interface configurations
 * 
 * @param intfNum : ID of the desired interface
 * @param status  : Status (1-active; 0-inactive)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_prottypeb_intf_switch_notify(L7_uint32 intfNum, L7_uint8 status);

/**
 * Dump type-b protection interface configurations
 */
extern void ptin_prottypeb_intf_config_dump(void);

#endif /* _PTIN_PROT_TYPEB_H */
