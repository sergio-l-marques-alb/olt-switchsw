
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename dos_migrate.c
*
* @purpose Denial of Service Configuration Migration
*
* @component DOSCONTROL
*
* @comments  none
*
* @create  08/15/2005
*
* @author  esmiley
* @end
*
**********************************************************************/

#include "dos_migrate.h"

static L7_RC_t doSMigrateConfigV1V2Convert (doSCfgDataV1_t * pCfgV1,
                                            doSCfgDataV2_t * pCfgV2);

static void doSBuildDefaultConfigDataV2 (doSCfgDataV2_t * pCfgV2);

static doSCfgData_t *pCfgCur = L7_NULLPTR;

/*********************************************************************
* @purpose  Migrate old config to new
*
* @param    oldVer      @b{(input)} version of old config definition
* @param    ver         @b{(input)} version of current config definition
* @param    pCfgBuffer  @b{(input)} ptr to location of read configuration
*
* @returns  void
*
* @notes    This is the callback function provided to the sysapiCfgFileGet
*           routine to handle cases where the config file is of an older
*           version.
*
* @notes
*
* @end
*********************************************************************/
void
doSMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
   L7_BOOL buildDefault = L7_FALSE;
   L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
   L7_uint32 oldCfgSize = 0;
   L7_uint32 *pOldCfgCksum = L7_NULLPTR;
   L7_uint32 oldCfgVer = 0;

   /* declare a pointer to each supported config structure version */
   doSCfgDataV1_t *pCfgV1 = L7_NULLPTR;
   doSCfgDataV2_t *pCfgV2 = L7_NULLPTR;

   pOldCfgCksum  = L7_NULLPTR;
   oldCfgSize    = 0;
   oldCfgVer     = 0;


   pCfgCur = (doSCfgData_t *) pCfgBuffer;
   MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

   /* Attempt to read the old version config file and migrate it to the current
    * version.  If not successful, build a default config of the latest version.
    */

   switch (oldVer)
   {
   case DOSCONTROL_CFG_VER_1:
     pCfgV1 = (doSCfgDataV1_t *) osapiMalloc (L7_DOSCONTROL_COMPONENT_ID,
                                             (L7_uint32) sizeof (doSCfgDataV1_t));
     if (L7_NULLPTR == pCfgV1)
     {
       buildDefault = L7_TRUE;
       break;
     }

     pOldCfgBuf = (L7_uchar8 *) pCfgV1;
     oldCfgSize = (L7_uint32) sizeof (doSCfgDataV1_t);
     pOldCfgCksum = &pCfgV1->checkSum;
     oldCfgVer = DOSCONTROL_CFG_VER_1;
     break;

   case DOSCONTROL_CFG_VER_2:
     pCfgV2 = (doSCfgDataV2_t *) osapiMalloc (L7_DOSCONTROL_COMPONENT_ID,
                                             (L7_uint32) sizeof (doSCfgDataV2_t));
     if (L7_NULLPTR == pCfgV2)
     {
       buildDefault = L7_TRUE;
       break;
     }

     pOldCfgBuf = (L7_uchar8 *) pCfgV2;
     oldCfgSize = (L7_uint32) sizeof (doSCfgDataV2_t);
     pOldCfgCksum = &pCfgV2->checkSum;
     oldCfgVer = DOSCONTROL_CFG_VER_2;
     break;

   default:
     buildDefault = L7_TRUE;
     oldCfgSize = 0;
     pOldCfgCksum = 0;
     oldCfgVer = 0;
     break;

   }                             /* endswitch */

   if (buildDefault != L7_TRUE)
   {
     /* read the old config file found on this system */

     /* Note:

        The routine sysapiCfgFileGet which invokes the migrate routine will pass
        in the a pointer to the location of the configuration data read from
        nonvolatile storage.

        In cases where the older version of the configuration file may be larger
        than the current version, sysapiCfgFileGet truncates the data to the size
        of the storage location pointed to by the buffer provided for the component.
        Thus, it is important to read the configuration file again so that the entirety
        of the configuration data can be accessed.

      */

     if ((L7_NULLPTR == pOldCfgBuf) ||
         (L7_SUCCESS != sysapiCfgFileGet (L7_DOSCONTROL_COMPONENT_ID,
                                          DOSCONTROL_CFG_FILENAME,
                                          pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                          oldCfgVer, L7_NULL /* do not build defaults */ ,
                                          L7_NULL        /* do not call migrate again */
          )))
     {
       L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOSCONTROL_COMPONENT_ID,
                     "sysapiCfgFileGet failed size = %d version = %d."
                     " Configuration did not exist or could not be read"
                     " for the specified feature.  This message is usually"
                     " followed by a message indicating that default configuration"
                     " values will be used."
                      , (L7_int32) oldCfgSize,
                      (L7_int32) oldCfgVer);
       buildDefault = L7_TRUE;
     }
   }

   /* Convert the old config file to the latest version.
      Each version of the configuation is iteratively migrated to the
      next version until brought up to the current level.
    */

   if (buildDefault != L7_TRUE)
   {
     switch (oldVer)
     {
     case DOSCONTROL_CFG_VER_1:
       /* pCfgV1 has already been allocated and filled in */
       pCfgV2 = (doSCfgDataV2_t *) osapiMalloc (L7_DOSCONTROL_COMPONENT_ID,
                                               (L7_uint32) sizeof (doSCfgDataV2_t));
       if ((L7_NULLPTR == pCfgV2) || (L7_SUCCESS != doSMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
       {
         buildDefault = L7_TRUE;
         break;
       }
       osapiFree (L7_DOSCONTROL_COMPONENT_ID, pCfgV1);
       pCfgV1 = L7_NULLPTR;
       /*passthru */

     case DOSCONTROL_CFG_VER_2:
       /* pCfgV2 has already been allocated and filled in */
       memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV2, sizeof (*pCfgCur));
       osapiFree (L7_DOSCONTROL_COMPONENT_ID, pCfgV2);
       pCfgV2 = L7_NULLPTR;

       /* done with migration - Flag unsaved configuration */
       pCfgCur->cfgHdr.dataChanged = L7_TRUE;
       break;

     default:
       buildDefault = L7_TRUE;
       break;

     }                           /* endswitch */
   }

   /* clean up any left over memory allocations from above */
   if (pCfgV1 != L7_NULLPTR)
     osapiFree (L7_DOSCONTROL_COMPONENT_ID, pCfgV1);
   if (pCfgV2 != L7_NULLPTR)
     osapiFree (L7_DOSCONTROL_COMPONENT_ID, pCfgV2);

   if (buildDefault == L7_TRUE)
   {
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOSCONTROL_COMPONENT_ID,
             "Building Defaults\n");
     memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
     doSBuildDefaultConfigData (DOSCONTROL_CFG_VER_CURRENT);
     pCfgCur->cfgHdr.dataChanged = L7_TRUE;
   }

   return;
}

/*********************************************************************
* @purpose  Converts the config data structure from version v1 to v2
*
* @param    pCfgV1    @b{(input)} ptr to version 1 config data structure
* @param    pCfgV2    @b{(input)} ptr to version 2 config data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
static L7_RC_t
doSMigrateConfigV1V2Convert (doSCfgDataV1_t * pCfgV1, doSCfgDataV2_t * pCfgV2)
{
  /* verify correct version of old config file */
  if (DOSCONTROL_CFG_VER_1 != pCfgV1->cfgHdr.version)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, DOSCONTROL_CFG_VER_1);
    return L7_FAILURE;
  }

  doSBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV2->systemFirstFragMode = pCfgV1->systemFirstFragMode;
  pCfgV2->systemMinTCPHdrLength = pCfgV1->systemMinTCPHdrLength;
  pCfgV2->systemICMPMode = pCfgV1->systemICMPMode;
  pCfgV2->systemMaxICMPSize = pCfgV1->systemMaxICMPSize;
  pCfgV2->systemL4PortMode = pCfgV1->systemL4PortMode;
  pCfgV2->systemSIPDIPMode = pCfgV1->systemSIPDIPMode;
  pCfgV2->systemTCPFlagMode = pCfgV1->systemTCPFlagMode;
  pCfgV2->systemTCPFragMode = pCfgV1->systemTCPFragMode;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build Version 2 defaults
*
* @param    pCfgV1    @b{(input)} ptr to version 2 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
doSBuildDefaultConfigDataV2 (doSCfgDataV2_t * pCfgV2)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (doSCfgDataV2_t));
  doSBuildDefaultConfigData (DOSCONTROL_CFG_VER_2);
  memcpy ((L7_uchar8 *) pCfgV2, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}
