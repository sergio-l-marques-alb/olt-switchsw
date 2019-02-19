/*
 * pintHapi_debug.c
 *
 *  Created on: 2010/04/08
 *      Author: Andre Brizido
 *
 *      Debug Routines
 */


#include "l7_common.h"
#include "osapi.h"
#include "support_api.h"
#include "log.h"
#include "ptinHapi_include.h"


static void ptinHapiDebugRegister(void);
/*********************************************************************
* @purpose  Do any initilization required for the debug utilities.
*
* @param    @b{(input)}
* @param    @b{(output)}
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t ptinHapiDebugInit()
{
  /* register for debug */
  ptinHapiDebugRegister();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Register to general debug infrastructure
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void ptinHapiDebugRegister(void)
{
    supportDebugDescr_t supportDebugDescr;

    memset(&supportDebugDescr, 0x00, sizeof(supportDebugDescr));

    supportDebugDescr.componentId = L7_PTINHAPI_COMPONENT_ID;

    /* Register User Control Parms */

    supportDebugDescr.userControl.notifySave = ptinHapiDebugSave;
    supportDebugDescr.userControl.hasDataChanged = ptinHapiDebugHasDataChanged;
    supportDebugDescr.userControl.clearFunc = ptinHapiDebugRestore;

    /* Register Internal Parms */
    supportDebugDescr.internal.debugHelp = ptinHapiDebugHelp;
    strncpy(supportDebugDescr.internal.debugHelpRoutineName,
            "ptinHapiDebugHelp",SUPPORT_DEBUG_HELP_NAME_SIZE);
    strncpy(supportDebugDescr.internal.debugHelpDescr,
            "Help Menu for ptinHapi",SUPPORT_DEBUG_HELP_DESCR_SIZE);

    (void) supportDebugRegister(supportDebugDescr);
}


/*********************************************************************
* @purpose  List the Debug commands availiable
*
* @param    @b{(input)}
* @param    @b{(output)}
*
* @returns  L7_SUCCESS
*
* @comments devshell command
*
* @end
*********************************************************************/
void ptinHapiDebugHelp()
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Available ptinHapi Debug commands:\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "-------------------------------\r\n");
}

