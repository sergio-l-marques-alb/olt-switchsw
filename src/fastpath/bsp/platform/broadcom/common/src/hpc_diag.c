/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  hpc_diag.c
*
* @purpose   Support diagnostic tests on this platform
*
* @component hpc
*
* @create    08/02/2006
*
* @author    Brady Rutherford 
* @end
*
*********************************************************************/

#include <stdio.h>

#include "l7_common.h"
#include "hpc_db.h"
#include "broad_debug.h"

#include "sal/appl/sal.h"
#include "sal/core/boot.h"
#include "sal/core/thread.h"

#define DIAGS_PASSWORD "lvl7dbg"
#define DIAGS_PASSWORD_TRIES 3
#define DIAGS_PASSWORD_MAX_ENTRY 63

/**************************************************************************
*
* @purpose  Initialize the platform for diagnostic testing.
*
* @param    none.
*
* @returns  L7_SUCCESS  Diagnostics started successfully
* @returns  L7_ERROR    problem encountered starting diagnostics
*
* @notes 
*
* @end
*
*************************************************************************/
L7_RC_t hpcDiagStart(void)
{
#ifdef L7_ROBO_SUPPORT
  return L7_SUCCESS;
#else
  L7_RC_t rc = L7_SUCCESS;
  char entered_password[DIAGS_PASSWORD_MAX_ENTRY+1];
  char password_format_string[16];
  int numTries = 0;

  do
  {
    sprintf(password_format_string, "%%%ds", DIAGS_PASSWORD_MAX_ENTRY);
    entered_password[0] = '\0';
    printf("Enter diag password: ");
    scanf(password_format_string, entered_password);
    if (strcmp(DIAGS_PASSWORD, entered_password) != 0)
    {
      printf("Incorrect!\n");
      numTries++;
    } else
    {
      break;
    }
  } while (numTries < DIAGS_PASSWORD_TRIES);
  if (numTries == DIAGS_PASSWORD_TRIES)
  {
    exit(1);
  }
  sal_core_init();
  sal_appl_init();

  sal_thread_create("bcmCLI", 128*1024,100, (void *)hapiBroadDebugDiagShell, 0);

  return(rc);
#endif
}

