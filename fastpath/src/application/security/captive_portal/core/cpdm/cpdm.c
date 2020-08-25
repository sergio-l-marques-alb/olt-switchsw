/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     cpdm.c
*
* @purpose      Captive Portal Data Manager (CPDM) control functions
*
* @component    CPDM
*
* @comments     none
*
* @create       06/28/2007
*
* @author       darsenault
*
* @end
*
**********************************************************************/
#include <string.h>
#include "l7utils_api.h"
#include "sysapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "intf_cb_api.h"
#include "captive_portal_commdefs.h"
#include "cpdm.h"

static unsigned char cpdm_write_lock_file[128];
static L7_uint32     cpdm_write_lock_line;
static L7_uint32     cpdm_lock_taken;

extern cpdmCfgData_t            *cpdmCfgData;
extern cpdmOprData_t            *cpdmOprData;
extern void                     *cpdmSema;

/*********************************************************************
*
* @purpose  Debug function to show last function that took the
*           write lock.
*
* @param   none
*
* returns   none
*
* @comments
*
* @end
*
*********************************************************************/
int cpdmDebugWriteLock(void)
{
  sysapiPrintf("Lock Taken: %d\n", cpdm_lock_taken);
  sysapiPrintf("CPDM Write Lock Last Take: %s (%d)\n",
            cpdm_write_lock_file,
            cpdm_write_lock_line);
  osapiSleepMSec (500);
  return 0;
}

/*********************************************************************
*
* @purpose  Take the write lock.
*
* @param   rwLock - cpdm Read/Write Lock.
* @param   timeout - Timeout option
* @param   file - File that got the lock.
* @param   line_num - Line number that got the lock.
*
* returns   return code from osapiWriteLockTake
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t cpdmSemaTake (void *sem, L7_int32 timeout,
                L7_uchar8 *file, L7_uint32 line_num)
{
  L7_RC_t rc;

  rc = osapiSemaTake(sem, L7_WAIT_FOREVER);

  cpdm_lock_taken = 1;
  l7utilsFilenameStrip((L7_char8 **)&file);
  osapiStrncpy (cpdm_write_lock_file,
          file,
          sizeof (cpdm_write_lock_file));
  cpdm_write_lock_line = line_num;
  /* printf("> %s %d\n", cpdm_write_lock_file, cpdm_write_lock_line); */
  return (rc);
}
/*********************************************************************
*
* @purpose  Give the write lock.
*
* @param   rwLock - cpdm Read/Write Lock.
*
* returns   return code from osapiWriteLockGive
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t cpdmSemaGive (void *sem)
{
  L7_RC_t rc;

  cpdm_lock_taken = 0;
  /* printf("<\n"); */
  rc = osapiSemaGive(sem);
  return (rc);
}


#ifdef L7_WIRELESS_CPDM_TEST

extern void cpdmTestNoop(void);

/*********************************************************************
* @purpose  A dummy function used to link cpdmTest* functions for devshell.
*
* @param    void
*
* returns   void
*
* @notes    none.
*
* @end
*********************************************************************/
void cpdmTestDummyFunc()
{
  cpdmTestNoop();

} /* cpdmTestDummyFunc */

#endif

