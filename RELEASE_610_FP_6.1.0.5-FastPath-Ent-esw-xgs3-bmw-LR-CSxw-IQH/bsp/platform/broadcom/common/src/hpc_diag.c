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

#include "l7_common.h"
#include "hpc_db.h"
#include "broad_debug.h"

#include "sal/appl/sal.h"
#include "sal/core/boot.h"
#include "sal/core/thread.h"

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
  sal_core_init();
  sal_appl_init();

  sal_thread_create("bcmCLI", 128*1024,100, (void *)hapiBroadDebugDiagShell, 0);

  return(rc);
#endif
}

