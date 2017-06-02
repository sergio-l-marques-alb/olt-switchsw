/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     openssl_cfg.c
*
* @purpose      OPENSSL configuration code
*
* @component    openssl
*
* @comments     This component does not have any configuration items.
*               However we have created this file for consistency.
*
* @create       11/16/2006
*
* @author       jshaw
*
* @end
*
**********************************************************************/

#include "openssl_include.h"

/*********************************************************************
*
* @purpose Save the OPENSSL configuration
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t opensslSave(void)
{
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Check if the OPENSSL user configuration data has changed.
*
* @returns L7_TRUE
* @returns L7_FALSE
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL opensslHasDataChanged(void)
{
  return L7_FALSE;
}
void opensslResetDataChanged(void)
{
  /* No flag to reset */
  return;
}
