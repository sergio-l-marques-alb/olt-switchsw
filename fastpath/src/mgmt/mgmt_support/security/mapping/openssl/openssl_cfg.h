/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     openssl_cfg.h
*
* @purpose      SSH configuration header
*
* @component    openssl
*
* @comments     none
*
* @create       09/15/2003
*
* @author       dcbii
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_OPENSSL_CFG_H
#define INCLUDE_OPENSSL_CFG_H

#include "openssl_include.h"

/* LVL7 Configuration file */


typedef enum
{
    OPENSSL_PHASE_INIT_0 = 0,
    OPENSSL_PHASE_INIT_1,
    OPENSSL_PHASE_INIT_2,
    OPENSSL_PHASE_WMU,
    OPENSSL_PHASE_INIT_3,
    OPENSSL_PHASE_EXECUTE,
    OPENSSL_PHASE_UNCONFIG_1,
    OPENSSL_PHASE_UNCONFIG_2,

} opensslCnfgrState_t;

typedef enum
{
  OPENSSL_DEBUG_LVL0 = L7_NULL, /* off */
  OPENSSL_DEBUG_LVL1,           /* function Entry */
  OPENSSL_DEBUG_LVL2,           /* Misc. */
  OPENSSL_DEBUG_LVL3,           /* General error messages */
  OPENSSL_DEBUG_LVL4            /* SSL Semaphores */

} OPENSSL_DEBUG_LVL_t;
/*
Internal function prototypes
*/

L7_RC_t opensslSave(void);
L7_BOOL opensslHasDataChanged(void);
void opensslResetDataChanged(void);

#endif /* INCLUDE_OPENSSL_CFG_H */

