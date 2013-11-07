/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename emweb_main.c
*
* @purpose
*
* @component XLIB
*
* @create 06/21/2008
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include "flex.h"

#ifndef L7_OLDCLIWEB_PACKAGE

#include "ews_api.h"
#include "ewnet.h"

extern void xLibInit (void);
void *xLibWaCreate(void);
void xLibWaDelete(void *wap);

extern int ewmain(void );
char *emwebStrdup (char *srcString);

extern struct EwsArchive_s *ew_archive;
extern EwsArchiveData ew_archive_data;
extern void osapiDevshellTaskInit(void );

char *local_host_name;
char *local_host_just_name;

static char realm_qualifier[64];

/* Use a cast to eliminate the 'const' if we are using C++. */
uint8 *ew_archive_data_p = (uint8 *) ew_archive_data;

int emweb_main (void)
{
  local_host_name = emwebStrdup ("0.0.0.0");
  local_host_just_name = emwebStrdup (local_host_name);

  /* Assemble realm_qualifier based on the host name. */
  strcpy (realm_qualifier, "@");
  strncat (realm_qualifier, local_host_name, sizeof (realm_qualifier) - 1);

  /* Initialise XLIB */
  xLibInit ();
  #ifdef L7_PRODUCT_SMARTPATH
  {
   #ifdef L7_BCM_SHELL_SUPPORT
    extern L7_BOOL bcm_shell_support;
    if (bcm_shell_support == 1)
    {
      sysapiPrintf ("\n\t--------------------------------------------------------------\n\n");
      sysapiPrintf ("\t  Important Note::\n");
      sysapiPrintf ("\t  This SmartPATH image is not QA Qualified and intended for \n");
      sysapiPrintf ("\t  internal debugging purposes only.\n\n");
      sysapiPrintf ("\t--------------------------------------------------------------\n");
    }
  #endif
  }
  osapiDevshellTaskInit();
#endif


  return ewmain ();
}

/*
 * ewaInstallArchives
 *   Called at init time - it should make any calls
 *   to ewsDocumentInstallArchive that are needed.
 */
EwaStatus ewaInstallArchives (void)
{
  EwsStatus status;

  /*
   * Install archive in EmWeb/Server database.
   */
  status = ewsDocumentInstallArchive (ew_archive, ew_archive_data_p);
  if (status != EWS_STATUS_OK)
  {
    EMWEB_ERROR (("ewsDocumentInstallArchive failed %d\n", status));
    return status;
  }

  return status;
}

/*
 * ewaTick is called once per tick by the server main loop (ewmain.c)
 */
void ewaTick (void)
{
}

static int xLibWaDelayedCreate = 1;
int xLibWaDelayedCreateToggle(void)
{
  xLibWaDelayedCreate = 1 - xLibWaDelayedCreate;
  return xLibWaDelayedCreate;
}

/*
 * ewaConnectionInit
 *   Called when a new connection is allocated by ewaSsdpNewConnection()
 *   and ewaClientConnectRequest().
 *   Should initialize any application-specific field in the net handle.
 */
boolean ewaConnectionInit( EwaNetHandle connection )
{
  if(xLibWaDelayedCreate == 0)
  {
    connection->workarea = xLibWaCreate();
    if(connection->workarea == NULL)
    {
      return FALSE;
    }
  }
  return TRUE;
}

void ewaConnectionFinish(EwaNetHandle connection)
{
  xLibWaDelete(connection->workarea);
  connection->workarea = NULL;
}

/*
 * ewaAuthRealmQualifier
 * Return qualifier to append to realms
 */
const char *ewaAuthRealmQualifier (void)
{
  return (realm_qualifier);
}

/*
 * ewaNetLocalHostName
 * Return configured name to EmWeb/Server
 */
const char *ewaNetLocalHostName (EwsContext context)
{
  return( ewsContextNetHandle( context )->localName );
}

/*
 * ewaArchiveDate
 *   Return date on which the archive was compiled
 */
const char *ewaArchiveDate (void)
{
  return ewsDocumentArchiveDate (ew_archive_data_p);
}

#endif

