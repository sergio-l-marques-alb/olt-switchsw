/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
*
* @filename     captive_portal_debug.c
*
* @purpose      Captive Portal Debug functions
*
* @component    Captive Portal
*
* @comments     none
*
* @create       06/27/2007
*
* @author       dcaugherty
*
* @end
*
**********************************************************************/

#include "string.h"

#include "l7_common.h"
#include "osapi.h"
#include "sysapi.h"
#include "log.h"
#include "captive_portal_commdefs.h"

#include "wdm_api.h"
/* DMX #include "support_api.h" */

#include "captive_portal_debug.h"


static L7_uchar8 captivePortalDebugLevel[CPD_LEVEL_LAST];


/*********************************************************************
* @purpose  This function clears all captive portal debug flags.
* @param    none
* @returns  none
* @notes    none
* @end
*********************************************************************/

L7_RC_t
captivePortalDebugReset(void)
{
    memset(captivePortalDebugLevel, 0, sizeof (captivePortalDebugLevel));
    return L7_SUCCESS;
} 


/*********************************************************************
* @purpose  This function sets specified debug flag.
* @param    level
* @returns  none
* @notes    none
* @end
*********************************************************************/

void
captivePortalDebugLevelSet(L7_uint32 level)
{
    if (level >= CPD_LEVEL_LAST)
    {
      printf("Invalid Debug level!");      
      return;
    }
    captivePortalDebugLevel[level] = 1;
}


/*********************************************************************
* @purpose  This function clears specified debug flag.
* @param    level
* @returns  none
* @notes    none
* @end
*********************************************************************/

void
captivePortalDebugLevelClear(L7_uint32 level)
{
    if (level >= CPD_LEVEL_LAST)
    {
      printf("Invalid Debug level!");      
      return;
    }
    captivePortalDebugLevel[level] = 0;
}


/*********************************************************************
* @purpose  This function checks whether specified debug flag is set.
*
* @param    level
*
* @returns  L7_TRUE - Flag is set.
*           L7_FALSE - Flag is not set.
*
* @notes    none
*
* @end
*********************************************************************/

L7_BOOL
captivePortalDebugLevelCheck(L7_uint32 level)
{
    if (level >= CPD_LEVEL_LAST)
    {
      printf("Invalid Debug level!");      
      return L7_TRUE;
    }

    if (captivePortalDebugLevel[level] == 1)
    {
        return L7_TRUE;
    }
    return L7_FALSE;
}


/*********************************************************************
* @purpose  Register to general debug infrastructure
* @param    void
* @returns  void
* @notes 
* @end
*********************************************************************/

void captivePortalDebugRegister(void)
{ 
/* DMX
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0, sizeof(supportDebugDescr));
    supportDebugDescr.componentId = L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID;
    // Register User Control Parms
    supportDebugDescr.userControl.clearFunc = captivePortalDebugReset;

    // Register Internal Parms
    // Submit registration
    (void) supportDebugRegister(supportDebugDescr);
*/
}

