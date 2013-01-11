/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  osapi_reg.c
*
* @purpose   Register SW version & OS details for linux platforms
*
* @component osapi
*
* @create    12/28/2006
*
* @author    Brady Rutherford
*
* @end
*
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "l7_common.h"
#include "sysapi.h"
#include "registry.h"
#include "bspapi.h"

/**************************************************************************
*
* @purpose  Populate registry with the software version
*
* @param    none
*
* @returns  none
*
* @comments Sets version to "Unknown" if able to determine version
*
* @end
*
*************************************************************************/
void osapiSwVersionRegister(void)
{
  FILE *vpd_fd;
  char rel[128], ver[128], maint[128], bld[128]; 
  char swFileName[SW_FILENAME_MAX_LEN];
  L7_char8 tempstring[20];
  char swTimeStamp[128];

  osapiSnprintf(tempstring,sizeof(tempstring), "Unknown");
  osapiSnprintf(swFileName,sizeof(swFileName),"Unknown");
  osapiSnprintf(swTimeStamp,sizeof(swTimeStamp),"Unknown");


#ifdef L7_VER_STRING /* linuxhost specific */
  osapiSnprintf(tempstring,sizeof(tempstring), L7_VER_STRING);
#endif

  /*
   *add the software version string
   */
  vpd_fd = fopen(EXEC_PATH VPD_FILE,"r");
  if(vpd_fd)
  {
      int comma_len;
      if(0 > fscanf(vpd_fd, "Vital Product Data:-\n")){}
      if(0 > fscanf(vpd_fd, "Operational Code Image File Name - %s\n", swFileName)){}
      if(0 > fscanf(vpd_fd,"Rel %s Ver %s Maint Lev %s Bld No %s",
             rel,ver,maint,bld)){}
      if(0 > fscanf(vpd_fd, "\nTimestamp - %28c\n", swTimeStamp)){}

      comma_len = strlen(rel);
      if (comma_len) comma_len--;
      rel[comma_len] = 0;
      comma_len = strlen(ver);
      if (comma_len) comma_len--;
      ver[comma_len] = 0;
      comma_len = strlen(maint);
      if (comma_len) comma_len--;
      maint[comma_len] = 0;

      osapiSnprintf(tempstring,sizeof(tempstring),"%s.%s.%s.%s",rel,ver,maint,bld);
      fclose(vpd_fd);
  }
  sysapiRegistryPut(SW_VERSION, STR_ENTRY, (void *)tempstring);
  sysapiRegistryPut(SW_FILENAME, STR_ENTRY, (void *)swFileName);
  sysapiRegistryPut(SW_FILE_TIMESTAMP, STR_ENTRY, (void *)swTimeStamp);

}

/**************************************************************************
*
* @purpose  Populate registry with OS version for Linux platforms
*
* @param    none
*
* @returns  none
*
* @comments none
*
* @end
*
*************************************************************************/
void osapiOSVersionRegister(void)
{
  L7_char8 tempstring[60];

  FILE *fver;
  L7_char8 linux_string[20], version_string[20], kern_string[20];

  memset(linux_string, 0x00, sizeof(linux_string));
  memset(version_string, 0x00, sizeof(version_string));
  memset(kern_string, 0x00, sizeof(kern_string));

  /* add the os version string */
  
  fver = fopen("/proc/version","r");
  if (fver != NULL)
  {
    if(0 > fscanf(fver,"%s %s %s", linux_string, version_string, kern_string)){}
    fclose(fver);
  }
  
  osapiSnprintf(tempstring,sizeof(tempstring),"%s %s", linux_string, kern_string);

  sysapiRegistryPut(OS,STR_ENTRY, (void *)tempstring);

}

