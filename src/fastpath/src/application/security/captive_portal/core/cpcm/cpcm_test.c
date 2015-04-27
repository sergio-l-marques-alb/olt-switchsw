/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     cpcm_test.c
*
* @purpose      Captive Portal Credential Manager (CPCM) test functions.
*
* @component    CP
*
* @comments     This file contains all necessary test functions to
*               verify CPCM functionality without other captive
*               portal components.  These functions are only included
*               when L7_CPCM_TEST is defined. The definition can be
*               added to the captive portal make.pkg file as follows;
*               
*               # Additional compiler flags.
*               #
*               CFLAGSEXTRA += \
*               	-DL7_CPCM_TEST
*
* @create       11/13/2007
*
* @author       darsen
*
* @end
*
**********************************************************************/
#ifdef L7_CPCM_TEST

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "l7utils_api.h"
#include "osapi.h"
#include "osapi_support.h"
#include "sysapi.h"
#include "usmdb_util_api.h"
#include "captive_portal_commdefs.h"
#include "cpdm_api.h"
#include "cpcm_api.h"


/*********************************************************************
*
* @purpose  No-op function to include all cpcmTest* functions for devshell.
*
* @param    none
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void cpcmTestNoop()
{
  return;
}

#endif  /* L7_CPCM_TEST */
