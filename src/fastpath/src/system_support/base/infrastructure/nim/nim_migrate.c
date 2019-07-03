
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename nim_migrate.c
*
* @purpose NIM Configuration Migration
*
* @component NIM
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include <string.h>
#include "nim_migrate.h"
#include "nim_util.h"
#include "defaultconfig.h"

extern L7_RC_t maskShiftRightBits(L7_uchar8 *j, L7_int32 k, L7_int32 m);
extern L7_RC_t maskShiftLeftBits(L7_uchar8 *j, L7_int32 k, L7_int32 m);
extern L7_RC_t maskOrBits(L7_uchar8 *j, L7_uchar8 *k, L7_int32 m);
extern L7_RC_t maskAndBits(L7_uchar8 *j, L7_uchar8 *k, L7_int32 m);
extern L7_RC_t maskSetBit (L7_uchar8 * j, L7_int32 k, L7_int32 m);
extern L7_RC_t maskClearBit(L7_uchar8 *j, L7_int32 k, L7_int32 m);
extern void maskDebugBitMaskAllocate(L7_int32 m);
extern L7_RC_t sysapiCfgFileMigrateMaskRel4_2To4_3 (NIM_INTF_MASK_REL_4_2_t *srcMask,
                                                    NIM_INTF_MASK_REL_4_3_t *destMask);

/* type for the format of the configuration file */
typedef struct
{
  L7_fileHdr_t cfgHdr;
  L7_uint32 numOfInterfaces;    /* Number of created interfaces */
  NIM_INTF_MASK_REL_4_3_t configMaskBitmap;     /* a bitmap to determine the used maskoffsets */
  nimCfgPort_t cfgPort[L7_MAX_INTERFACE_COUNT_REL_4_3 + 1];     /* an array of the ports configuration */
  L7_uint32 *checkSum;          /* check sum of config file NOTE: must be last entry */
} nimConfigDataDebug_t;

typedef struct
{
  L7_char8 *pCfgBuffer;
  L7_uint32 cfgLen;
  L7_uint32 numPorts;
}
rel4_0NimSlotConfig_t;



extern void sysapiCfgMigrateDataDump (void);
extern void nimDebugCfgPort (nimCfgPort_t * configPort);

void nimMigrateConfigDebug (void);

static rel4_0NimSlotConfig_t rel4_0NimSlotConfig[L7_MAX_SLOTS_PER_BOX_REL_4_0];
static nimConfigDataDebug_t *pNimConfigDebugData = L7_NULLPTR;


static L7_RC_t nimRel4_0FileMigrate (nimConfigData_t * pCfgData);
static L7_RC_t nimMigrateConfigV3V4Convert (nimConfigDataV3_t * pCfgV1, nimConfigDataV4_t * pCfgV2);
static L7_RC_t nimMigrateConfigV4V5Convert (nimConfigDataV4_t * pCfgV1, nimConfigDataV5_t * pCfgV2);
static void nimConfigFileDefaultCreateV4 (nimConfigDataV4_t * pCfgV4);
static void nimConfigFileDefaultCreateV5 (nimConfigDataV5_t * pCfgV5);
static L7_RC_t sysapiCfgFileMigrateMaskRel4_3To4_4 (NIM_INTF_MASK_REL_4_3_t *srcMask, NIM_INTF_MASK_REL_4_4_t *destMask);
static nimConfigDataCurrent_t *pCfgCur = L7_NULLPTR;

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
* @end
*********************************************************************/
void
nimMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  nimConfigDataV3_t *pCfgV3 = L7_NULLPTR;
  nimConfigDataV4_t *pCfgV4 = L7_NULLPTR;
  nimConfigDataV5_t *pCfgV5 = L7_NULLPTR;

  ver = ver;
  pCfgCur = (nimConfigDataCurrent_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, (L7_uint32) sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case 0:
    nimConfigFileDefaultCreate (NIM_CFG_VER_CURRENT);
    (void) nimRel4_0FileMigrate ((nimConfigData_t *) pCfgBuffer);
    return;

  case NIM_CFG_VER_3:
    pCfgV3 = (nimConfigDataV3_t *) osapiMalloc (L7_NIM_COMPONENT_ID,
                                                (L7_uint32) sizeof (nimConfigDataV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (nimConfigDataV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = NIM_CFG_VER_3;
    break;

  case NIM_CFG_VER_4:
    pCfgV4 = (nimConfigDataV4_t *) osapiMalloc (L7_NIM_COMPONENT_ID,
                                                (L7_uint32) sizeof (nimConfigDataV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (nimConfigDataV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = NIM_CFG_VER_4;
    break;

  case NIM_CFG_VER_5:
    pCfgV5 = (nimConfigDataV5_t *) osapiMalloc (L7_NIM_COMPONENT_ID,
                                                (L7_uint32) sizeof (nimConfigDataV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (nimConfigDataV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = NIM_CFG_VER_5;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_NIM_COMPONENT_ID,
                                         NIM_CFG_FILE_NAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_NIM_COMPONENT_ID,
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
    case NIM_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (nimConfigDataV4_t *) osapiMalloc (L7_NIM_COMPONENT_ID,
                                                  (L7_uint32) sizeof (nimConfigDataV4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_SUCCESS != nimMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_NIM_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case NIM_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (nimConfigDataV5_t *) osapiMalloc (L7_NIM_COMPONENT_ID,
                                                  (L7_uint32) sizeof (nimConfigDataV5_t));
      if ((L7_NULLPTR == pCfgV5) || (L7_SUCCESS != nimMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_NIM_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case NIM_CFG_VER_5:
      /* pCfgV5 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV5, sizeof (*pCfgCur));
      osapiFree (L7_NIM_COMPONENT_ID, pCfgV5);
      pCfgV5 = L7_NULLPTR;

      /* done with migration - Flag unsaved configuration */
      pCfgCur->cfgHdr.dataChanged = L7_TRUE;
      break;

    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_NIM_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_NIM_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_NIM_COMPONENT_ID, pCfgV5);

  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
            "Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    nimConfigFileDefaultCreate (NIM_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V3 to V4.
*
* @param    pCfgV3    @b{(input)} ptr to version 3 config data structure
* @param    pCfgV4    @b{(input)} ptr to version 4 config data structure
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
nimMigrateConfigV3V4Convert (nimConfigDataV3_t * pCfgV3, nimConfigDataV4_t * pCfgV4)
{
  L7_uint32 i, count;
  L7_uint32 cfgsCopied;
  L7_int32  maskOffsetDelta;

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != NIM_CFG_VER_3)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, NIM_CFG_VER_3);
    return L7_FAILURE;
  }

  nimConfigFileDefaultCreateV4 (pCfgV4);
  pCfgV4->checkSum = pCfgV3->checkSum;
  pCfgV4->checkSum2 = pCfgV3->checkSum2;

  /* Check that port migration can be supported */

  if ( L7_MAX_INTERFACE_COUNT_REL_4_2 < L7_MAX_INTERFACE_COUNT_REL_4_3)
  {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
              "Migration from V3 to V4 cannot be supported\n");
  }

  /*============================================================================*/
  /*
        Copy port configuration structues
  */
  /*============================================================================*/

  /* Due to a correction in port counts, the non-stacking version of release 4.3
     (release I) has fewer physical ports than the non-stacking version of releases
     4.1 and 4.2. Thus, adjust accordingly when determining the minimum number of
     port configuration structures to copy. */


  /* count = min (L7_MAX_INTERFACE_COUNT_REL_4_1, L7_MAX_INTERFACE_COUNT); */

  count = min (L7_MAX_INTERFACE_COUNT_REL_4_1, L7_MAX_INTERFACE_COUNT_REL_4_3);
  count = min (count, pCfgV3->numOfInterfaces);

  cfgsCopied = 1;
  for (i = 1; i <= L7_MAX_INTERFACE_COUNT_REL_4_1; i++)
  {
    /* Do not copy configuration if it is not applicable. */
      if ( pCfgV3->cfgPort[i].configIdMaskOffset == L7_NULL)
          continue;

    if (cfgsCopied <= count)
    {
        MIGRATE_COPY_STRUCT (pCfgV4->cfgPort[cfgsCopied], pCfgV3->cfgPort[i]);

        /* Adjust the configIdMaskOffset if it does not map to V4.
           This is needed for the non-stacking builds  */

        /* Ugly: This is just for this release */
        maskOffsetDelta = L7_MAX_INTERFACE_COUNT_REL_4_2 - L7_MAX_INTERFACE_COUNT_REL_4_3;
        if (maskOffsetDelta > 0)
        {
            if (pCfgV4->cfgPort[cfgsCopied].configIdMaskOffset > L7_MAX_PORT_COUNT_REL_4_2 )
            {
                /*  Adjust */
                pCfgV4->cfgPort[cfgsCopied].configIdMaskOffset -= maskOffsetDelta;
            }
        }
        cfgsCopied++;
    }

  }



  /* Signal truncation if more nim entries existed than can be copied to the current
     number of entries.  Also, adjust the numOfInterfaces accordingly */
  if (pCfgV3->numOfInterfaces >= L7_MAX_INTERFACE_COUNT_REL_4_3)
  {
      MIGRATE_LOG_INTERFACE_TRUNCATE (NIM_CFG_FILE_NAME, pCfgV3->numOfInterfaces);
      pCfgV4->numOfInterfaces = L7_MAX_INTERFACE_COUNT_REL_4_3;
  }
  else
      pCfgV4->numOfInterfaces = pCfgV3->numOfInterfaces;


  /*============================================================================*/
  /*
        Adjust interface masks appropriately   (not pretty)
  */
  /*============================================================================*/

   /* Only adjust the sizes needed to refect current build */
   if (sizeof(NIM_INTF_MASK_REL_4_3_t) < sizeof(NIM_INTF_MASK_REL_4_2_t) )

   {
       /* Release 4.2 is same size as 4.1, hence the cast */
       if ( sysapiCfgFileMigrateMaskRel4_2To4_3((NIM_INTF_MASK_REL_4_2_t *) &pCfgV3->configMaskBitmap,
                                           &pCfgV4->configMaskBitmap) != L7_SUCCESS)
       {
           L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
                   " Failed to migrate interface config mask from version 3 to version 4\n");
           return L7_FAILURE;
       }

   }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V4 to V5.
*
* @param    pCfgV4    @b{(input)} ptr to version 4 config data structure
* @param    pCfgV5    @b{(input)} ptr to version 5 config data structure
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
nimMigrateConfigV4V5Convert (nimConfigDataV4_t * pCfgV4,
                             nimConfigDataV5_t * pCfgV5)
{
  L7_uint32 i, count, diff;
  L7_RC_t rc;

  /* verify correct version of old config file */
  if (pCfgV4->cfgHdr.version != NIM_CFG_VER_3 &&
      pCfgV4->cfgHdr.version != NIM_CFG_VER_4)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, NIM_CFG_VER_4);
    return L7_FAILURE;
  }
  nimConfigFileDefaultCreateV5 (pCfgV5);
  pCfgV5->checkSum = pCfgV4->checkSum;
  pCfgV5->checkSum2 = pCfgV4->checkSum2;

  /*=========================================================================*/
  /*
        Copy port configuration structues
  */
  /*=========================================================================*/


  /* Number of lag interfaces changed between 4_3 and 4_4 releases.
   * We have to accomodate for it. We cannot migrate between stacking and non stacking
   * (both ways) so it is safe to assume that uptil the
   * L7_MAX_PORT_COUNT_REL_4_3 + L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_3 + L7_MAX_NUM_LAG_INTF_REL_4_3
   * the port config strutures can be directly copied.
   */
  count = L7_MAX_PORT_COUNT_REL_4_3 + L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_3 + L7_MAX_NUM_LAG_INTF_REL_4_3;

  for (i = 1; i <= count; i++)
  {
      MIGRATE_COPY_STRUCT (pCfgV5->cfgPort[i], pCfgV4->cfgPort[i]);
  }

  /* Now we have to skip L7_MAX_NUM_LAG_INTF_REL_4_4 - L7_MAX_NUM_LAG_INTF_REL_4_3 entries
   * as these will be blank in 4_4, and continue to copy the remaining port configs
   * in their appropriate places.
   */
  diff = L7_MAX_NUM_LAG_INTF_REL_4_4 - L7_MAX_NUM_LAG_INTF_REL_4_3;
  i = count + 1;
  count = min (L7_MAX_INTERFACE_COUNT_REL_4_3, L7_MAX_INTERFACE_COUNT_REL_4_4);
  for ( ; i <= count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV5->cfgPort[i+diff], pCfgV4->cfgPort[i]);
    if (pCfgV5->cfgPort[i+diff].configIdMaskOffset != 0)
    {
      /* Adjust configIdMaskOffset as that contains intIfNum */
      pCfgV5->cfgPort[i+diff].configIdMaskOffset = pCfgV5->cfgPort[i+diff].configIdMaskOffset + diff;
    }
  }



  /* Signal truncation if more nim entries existed than can be copied to
   * the current number of entries.  Note that numOfInterfaces will already
   * be correct in the new config due to initialization to the default above.
   */
  if (pCfgV4->numOfInterfaces > L7_MAX_INTERFACE_COUNT_REL_4_4)
  {
      MIGRATE_LOG_INTERFACE_TRUNCATE (NIM_CFG_FILE_NAME,
                                      pCfgV4->numOfInterfaces);
  }

  rc = sysapiCfgFileMigrateMaskRel4_3To4_4(&pCfgV4->configMaskBitmap, &pCfgV5->configMaskBitmap);

  return rc;
}

/*********************************************************************
* @purpose  Build Version 4 defaults
*
* @param    pCfgV4    @b{(input)} ptr to version 4 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
nimConfigFileDefaultCreateV4 (nimConfigDataV4_t * pCfgV4)
{
  L7_fileHdr_t  *myHdr;

  /*
   * Based on the V4 versions of nimConfigFileDefaultCreate(),
   * nimConfigFileOffsetsSet(), and nimConfigFileHdrCreate().
   */
  memset((void*)pCfgV4, 0, sizeof(*pCfgV4));
  myHdr = &pCfgV4->cfgHdr;
  myHdr->version = NIM_CFG_VER_4;
  strncpy(myHdr->filename, NIM_CFG_FILE_NAME, L7_MAX_FILENAME-1);
  myHdr->componentID = L7_NIM_COMPONENT_ID;

  /* sizeof the config structure + the malloc'd data + CRC - File Hdr*/
  myHdr->length = sizeof(nimConfigDataV4_t);
  pCfgV4->numOfInterfaces = L7_MAX_INTERFACE_COUNT_REL_4_3;
}

/*********************************************************************
* @purpose  Build Version 5 defaults
*
* @param    pCfgV5    @b{(input)} ptr to version 5 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
nimConfigFileDefaultCreateV5 (nimConfigDataV5_t * pCfgV5)
{
  memset (pCfgV5, 0, sizeof (*pCfgV5));
  nimConfigFileDefaultCreate (NIM_CFG_VER_5);
  memcpy (pCfgV5, pCfgCur, sizeof (*pCfgV5));
}

/*********************************************************************
* @purpose  Store the NIM slot info, which will be used later while migrating
*
* @param    slot        @b{(input)} slot number
* @param    pSlotInfo   @b{(input)} config data
* @param    cfgLen      @b{(input)} config data length
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void
nimRel4_0SlotInfoRecord (L7_uint32 slot, L7_uchar8 * pSlotInfo, L7_uint32 cfgLen)
{
  L7_uint32 portCfgLen;

  if (slot >= L7_MAX_SLOTS_PER_BOX_REL_4_0)
  {
    if (pSlotInfo)
      osapiFree (L7_NIM_COMPONENT_ID, pSlotInfo);
    return;
  }

  if (pSlotInfo == L7_NULL)
    return;

  portCfgLen = cfgLen;
  portCfgLen -= sizeof (L7_fileHdrV1_t);        /* cfgHdr */
  portCfgLen -= sizeof (L7_uint32);     /* cardIdRev */
  portCfgLen -= sizeof (L7_uint32);     /* checkSum */

  rel4_0NimSlotConfig[slot].pCfgBuffer = pSlotInfo;
  rel4_0NimSlotConfig[slot].cfgLen = cfgLen;
  rel4_0NimSlotConfig[slot].numPorts = portCfgLen / sizeof (nimCfgPortV1_t) - 1;

  if (0 != (portCfgLen % sizeof (nimCfgPortV1_t)))
  {
    NIM_LOG_MSG ("Invalid Port Config Length %d\n", (L7_int32) cfgLen);
  }
}

/******************** Release G format of nimSlotx.cfg file ***************
    ------------------------------
   |                               |
   |                               |
   |       L7_fileHdr_t            |
   |                               |
   |                               |
    ------------------------------
   |                               |
   |         cardIdRev             |
   |   from rel_g nimConfigData_t  |
    ------------------------------
   |     Port 1 Config Data        |
   |                               |
   |  from rel_g nimCfgPort_t      |
    ------------------------------
   |     Port 2 Config Data        |
   |                               |
   |  from rel_g nimCfgPort_t      |
    ------------------------------
   |     Port 3 Config Data        |
   |                               |
   |  from rel_g nimCfgPort_t      |
    ------------------------------
   |                               |
   |        ....                   |
   |                               |
    ------------------------------
   |     Port N Config Data        |
   |                               |    where N = numOfPortsInSlot as calculated below
   |  from rel_g nimCfgPort_t      |              for release G
    ------------------------------
   |                               |
   |         crc                   |
   |                               |
    ------------------------------


******************** Release I format of nimStack.cfg file ***************

    ------------------------------
   |                               |
   |                               |
   |       nimConfigData_t         |
   |                               |
   |                               |
    ------------------------------
   |     Port 1 Config Data        |
   |                               |
   |  from rel_i nimCfgPort_t      |
    ------------------------------
   |     Port 2 Config Data        |
   |                               |
   |  from rel_i nimCfgPort_t      |
    ------------------------------
   |     Port 3 Config Data        |
   |                               |
   |  from rel_i nimCfgPort_t      |
    ------------------------------
   |                               |
   |        ....                   |
   |                               |
    ------------------------------
   |     Port N Config Data        |
   |                               |    where N = L7_MAX_INTERFACE_COUNT +1
   |  from rel_i nimCfgPort_t      |
    ------------------------------
   |                               |
   |         crc                   |
   |                               |
    ------------------------------

*****************************************************************************/


/*********************************************************************
* @purpose  Store the NIM slot info, which will be used later while migrating
*
* @param    *pCfgData  @b{(input)} pointer to nimConfigData_t structure
* @param    intfType   @b{(input)} one of L7_INTF_TYPES_t
* @param    pIntIfNum  @b{(output)} next intIfNum
*
* @returns  *nimCfgPort_t
*
* @notes
*
* @end
*********************************************************************/
static nimCfgPort_t *
nimMigratePortConfigGetNext (nimConfigData_t * pCfgData, L7_INTF_TYPES_t intfType,
                             L7_uint32 * pIntIfNum)
{
  static L7_uint32 vlan_intf = 0;
  static L7_uint32 lag_intf = 0;
  static L7_uint32 phy_intf = 0;
  static L7_uint32 cpu_intf = 0;
  L7_uint32 temp;

  if (phy_intf == 0)
  {
    nimIntIfNumRangePopulate ();
    nimIntIfNumRangeGet (L7_LOGICAL_VLAN_INTF, &vlan_intf, &temp);
    nimIntIfNumRangeGet (L7_LAG_INTF, &lag_intf, &temp);
    nimIntIfNumRangeGet (L7_PHYSICAL_INTF, &phy_intf, &temp);
    nimIntIfNumRangeGet (L7_CPU_INTF, &cpu_intf, &temp);
  }

  switch (intfType)
  {
  case L7_PHYSICAL_INTF:
    temp = phy_intf++;
    break;
  case L7_LAG_INTF:
    temp = lag_intf++;
    break;
  case L7_LOGICAL_VLAN_INTF:
    temp = vlan_intf++;
    break;
  case L7_CPU_INTF:
    temp = cpu_intf++;
    break;
  default:
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
            "unknown intfType %d\n", (L7_int32) intfType);
    return NULL;
    break;
  }

  *pIntIfNum = temp;
  return (pCfgData->cfgPort + temp);
}

/*********************************************************************
* @purpose  Convert release G nimSlotX.cfg files to subsequent release format
*
* @param    fileName   Pointer to file name
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error from osapiFsWrite
*
* @notes    none
*
* @end
*********************************************************************/
static L7_RC_t
nimRel4_0FileMigrate (nimConfigData_t * pCfgData)
{
  L7_RC_t rc;
  L7_uint32 slot;
  L7_uint32 port = 0;
  L7_uchar8 *pCfgBuf;
  L7_uint32 intIfNum = 0;
  L7_INTF_TYPES_t intfType = 0;
  L7_BOOL calcCheckSum = 0;
  nimConfigID_t configId;
  nimCfgPortV1_t *pCfgPortV1;
  nimCfgPort_t *pCfgPort;
  nimIntfConfig_t *pCfgIntf;

  pCfgPort = pCfgData->cfgPort;
  if (pCfgPort == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
            "Failed to get port config");
    return L7_ERROR;
  }

  /* Read and convert data in each file */
  for (slot = 0; slot < L7_MAX_SLOTS_PER_BOX_REL_4_0; slot++)
  {
    pCfgBuf = rel4_0NimSlotConfig[slot].pCfgBuffer;
    if (L7_NULL == pCfgBuf)
    {
      /*
      NIM_LOG_MSG ("nimSlot%d.cfg was not found\n", (L7_int32) slot);
      */
      continue;
    }

    pCfgPortV1 = (nimCfgPortV1_t *) (pCfgBuf + sizeof (L7_fileHdrV1_t) + sizeof (L7_uint32));
    pCfgPortV1++;               /* skip first */

    if (rel4_0NimSlotConfig[slot].numPorts > L7_MAX_PORTS_PER_SLOT_REL_4_0)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
              "rel4_0NimSlotConfig[slot].numPorts %d out of bounds",
               rel4_0NimSlotConfig[slot].numPorts);
      continue;
    }

    intIfNum = 0;
    intfType = 0;
    for (port = 1; port <= rel4_0NimSlotConfig[slot].numPorts; port++, pCfgPortV1++)
    {
      rc = sysapiCfgFileRel4_0SlotPortToIntfInfoGet (slot, port, &configId, &intIfNum, &intfType);
      if (L7_SUCCESS != rc)
      {
        if (L7_ERROR == rc)
          MIGRATE_INVALID_SLOT_PORT (slot, port);
        continue;
      }
      if (intIfNum >= platIntfTotalMaxCountGet ())
      {
        MIGRATE_INDEX_OUTOF_BOUND (intIfNum);
        continue;
      }

      pCfgPort = nimMigratePortConfigGetNext (pCfgData, intfType, &intIfNum);
      if (pCfgPort != L7_NULL)
      {
        L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
        nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
                "Migrating NIM: slot %d port %d intf %d, %s intfType %d addrType = %d",
                 slot, port, intIfNum, ifName, intfType, pCfgPortV1->addrType);
        (void) nimConfigIdCopy (&configId, &pCfgPort->configInterfaceId);
        pCfgPort->configIdMaskOffset = intIfNum;
        NIM_INTF_SETMASKBIT (pCfgData->configMaskBitmap, intIfNum);
        pCfgIntf = &pCfgPort->cfgInfo;
        MIGRATE_COPY_STRUCT (pCfgIntf->LAAMacAddr.addr, pCfgPortV1->LAAMacAddr);
        pCfgIntf->addrType = pCfgPortV1->addrType;
        strncpy (pCfgIntf->ifAlias, pCfgPortV1->ifAlias, L7_NIM_IF_ALIAS_SIZE);
        if (pCfgPortV1->nameType == 0)
          pCfgIntf->nameType = FD_NIM_NAME_TYPE;
        else
          pCfgIntf->nameType = pCfgPortV1->nameType;
        pCfgIntf->ifSpeed = pCfgPortV1->ifSpeed;
        if (pCfgPortV1->autoNegAdminStatus == L7_ENABLE)
        {
          pCfgIntf->negoCapabilities = L7_PORT_NEGO_CAPABILITY_ALL;
        }
        else
        {
          pCfgIntf->negoCapabilities = 0;
        }
        pCfgIntf->adminState = pCfgPortV1->adminState;
        pCfgIntf->trapState = pCfgPortV1->trapState;
        pCfgIntf->ipMtu = pCfgPortV1->ipMtu;
        pCfgIntf->encapsType = pCfgPortV1->encapsType;
        pCfgIntf->cfgMaxFrameSize = pCfgPortV1->cfgMaxFrameSize;
      }
      calcCheckSum = L7_TRUE;
    }

    osapiFree (L7_NIM_COMPONENT_ID, pCfgBuf);
  }

  if (L7_TRUE == calcCheckSum)
  {
    pCfgData->cfgHdr.dataChanged = L7_TRUE;
    *pCfgData->checkSum = nvStoreCrc32 ((L7_char8 *) pCfgData,
                                        pCfgData->cfgHdr.length - sizeof (L7_uint32));
  }

  /* This path will always be taken when there is no previous configuration in the switch.
     Do not uncomment except for debugging.

  if (port == 0)
  {
    nimMigrateConfigDebug ();
  }
  */

  return L7_SUCCESS;
}


/*************************** DEBUG ROUTINES  ************************************/
/*************************** DEBUG ROUTINES  ************************************/
/*************************** DEBUG ROUTINES  ************************************/
/*************************** DEBUG ROUTINES  ************************************/
/*************************** DEBUG ROUTINES  ************************************/

/*********************************************************************
* @purpose  Debug dump routine for config migrationn
*
* @param    void    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void nimMigrateConfigDebug (void)
{
  int i, numOfPorts;

  printf ("copying NIM info\n");

  if (pNimConfigDebugData == L7_NULLPTR)
  {
    pNimConfigDebugData = (nimConfigDataDebug_t *) osapiMalloc (L7_NIM_COMPONENT_ID,
                                                                sizeof (nimConfigDataDebug_t));
    if (pNimConfigDebugData == L7_NULL)
    {
      NIM_L7_LOG_ERROR("NIM:Couldn't allocate memory for the nimConfigData\n");
      return;
    }
    memset ((void *) pNimConfigDebugData, 0, sizeof (nimConfigDataDebug_t));
  }

  MIGRATE_COPY_STRUCT (pNimConfigDebugData->cfgHdr, nimCtlBlk_g->nimConfigData->cfgHdr);
  pNimConfigDebugData->numOfInterfaces = nimCtlBlk_g->nimConfigData->numOfInterfaces;
  MIGRATE_COPY_STRUCT (pNimConfigDebugData->configMaskBitmap,
                       nimCtlBlk_g->nimConfigData->configMaskBitmap);
  pNimConfigDebugData->checkSum = nimCtlBlk_g->nimConfigData->checkSum;

  numOfPorts = platIntfTotalMaxCountGet ();
  for (i = 0; i <= numOfPorts; i++)
  {
    memcpy (&pNimConfigDebugData->cfgPort[i], &nimCtlBlk_g->nimPorts[i].configPort,
            sizeof (nimCfgPort_t));
  }

  sysapiCfgMigrateDataDump ();
}





/*************************** MASK MIGRATION  ************************************/
/*************************** MASK MIGRATION  ************************************/
/*************************** MASK MIGRATION  ************************************/
/*************************** MASK MIGRATION  ************************************/
/*************************** MASK MIGRATION  ************************************/



/*********************************************************************
* @purpose  Converts the config data structure from version V3 to V4.
*
* @param    *srcMask    pointer to mask of size NIM_INTF_MASK_REL_4_2_t
* @param    *destMask   pointer to mask of size NIM_INTF_MASK_REL_4_3_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
L7_RC_t sysapiCfgFileMigrateMaskRel4_2To4_3 (NIM_INTF_MASK_REL_4_2_t *srcMask,
                                             NIM_INTF_MASK_REL_4_3_t *destMask)
{
    NIM_INTF_MASK_REL_4_2_t notLogicalPortMask;
    NIM_INTF_MASK_REL_4_2_t logicalPortMask;
    L7_int32   firstNonPhysPort;
    L7_uint32  i;
    L7_uint32  bits_in_mask;



    if ( (srcMask == L7_NULLPTR)  || (destMask == L7_NULLPTR) )
    {
        return L7_FAILURE;
    }


    if ( (sizeof (NIM_INTF_MASK_REL_4_2_t)) < (sizeof(NIM_INTF_MASK_REL_4_3_t) ) )
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
                "Migration of previous mask size %d is not supported in this release\n",
                sizeof( NIM_INTF_MASK_REL_4_2_t) );
        return L7_FAILURE;
    }

    /* Adjust the bit mask offsets in the prev release to reflect the bit masks in
       the current release.


       From release 4.2 to 4.3,in NON-STACKING builds, the bitmask is smaller
       since the  add'l phys ports used for stacking have been removed.

       Thus, to bring this up to release 4.3, the valid bits for the logical
       interfaces in the previous release have to be shifted down to
       just above the phys ports.

         --------------------------------------------------------------------
        | Physical interfaces | add'l phys ports    | logical interfaces    |
        |     for 1 box       | for stacked boxes   | for all boxes         |
         -------------------------------------------------------------------
                              A                    B                       C


     */

    /* Step 1.  */

    /* Initialize and get working copies */

    memset( (void *)&notLogicalPortMask, 0, sizeof(notLogicalPortMask) );
    memset( (void *)&logicalPortMask, 0, sizeof(logicalPortMask) );

    memcpy ((L7_uchar8 *) &notLogicalPortMask, srcMask, sizeof(notLogicalPortMask) );

    memcpy ((L7_uchar8 *) &logicalPortMask, srcMask, sizeof(logicalPortMask) );

    bits_in_mask = sizeof(NIM_INTF_MASK_REL_4_2_t)*8;

    /* Step 2.  */

    /* Shift the logical port mask left to the        port


       The port number assignment is as is below:

         -----------------------------------------------
        | Physical  |  CPU  |   LAGs      | VLAN intf  |
        |           |       |            |
         ----------------------------------------------


       */

        /* Move logical ports all the way to the left so that the
           first logical port for the 4_2 version is in the zeroeth bit.
           Then shift to the right so that the logical port is repositioned
           as appropriate for release 4_3. */
    firstNonPhysPort = L7_MAX_PORT_COUNT_REL_4_2;
    maskShiftLeftBits( (L7_uchar8 *)&logicalPortMask, firstNonPhysPort, bits_in_mask);


    firstNonPhysPort = L7_MAX_PORT_COUNT_REL_4_3;
    maskShiftRightBits( (L7_uchar8 *)&logicalPortMask, firstNonPhysPort, bits_in_mask);


    /* Step 3.  */

    /* Obtain a mask with just the non-logical ports:

             Clear all bits above the physical interfaces
    */


    firstNonPhysPort = L7_MAX_PORT_COUNT_REL_4_3 + 1;

    for (i = (firstNonPhysPort); i <  bits_in_mask; i++)
    {
        maskClearBit((L7_uchar8 *)&notLogicalPortMask, i, bits_in_mask);
    }

    /* Step 4.  */

    /* Get the combined mask */

    /* Now combine the valid bits in the two masks */
    maskOrBits( (L7_uchar8 *) &logicalPortMask,
                (L7_uchar8 *) &notLogicalPortMask,bits_in_mask);


    /* Step 5.  */

    /* Copy  the adjusted mask to the current configuration

       CAUTION:  Ensure that the copy is for the same mask size!!!
    */

     memcpy ((L7_uchar8 *) destMask, (L7_uchar8 *) &logicalPortMask,
             sizeof(NIM_INTF_MASK_REL_4_3_t) );


     /* Step 6  */


     return L7_SUCCESS;

}
/*********************************************************************
* @purpose  Converts the config data structure from version V4 to V5.
*
* @param    *srcMask    pointer to mask of size NIM_INTF_MASK_REL_4_3_t
* @param    *destMask   pointer to mask of size NIM_INTF_MASK_REL_4_4_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
* @end
*********************************************************************/
L7_RC_t sysapiCfgFileMigrateMaskRel4_3To4_4 (NIM_INTF_MASK_REL_4_3_t *srcMask,
                                             NIM_INTF_MASK_REL_4_4_t *destMask)
{
    NIM_INTF_MASK_REL_4_4_t leftMask;
    NIM_INTF_MASK_REL_4_4_t rightMask;
    L7_int32   shiftBits;
    L7_uint32  bits_in_mask;

    if ( (srcMask == L7_NULLPTR)  || (destMask == L7_NULLPTR) )
    {
        return L7_FAILURE;
    }


    if ( (sizeof (NIM_INTF_MASK_REL_4_3_t)) > (sizeof(NIM_INTF_MASK_REL_4_4_t) ) )
    {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
                "Migration of previous mask size %d is not supported in this release\n",
                sizeof( NIM_INTF_MASK_REL_4_3_t) );
        return L7_FAILURE;
    }

    /*  The idea is to introduce  L7_MAX_NUM_LAG_INTF_REL_4_4 - L7_MAX_NUM_LAG_INTF_REL_4_3 zeros
        in the 4_4 bit mask to represent the additional lags that were not present in 4_3

        phy cpu lag (1-6) vlanIntf stackIntf

        We need to migrate to
        phy cpu lag (1-6) (L7_MAX_NUM_LAG_INTF_REL_4_4 - L7_MAX_NUM_LAG_INTF_REL_4_4 = 58 bits of zeros) vlanIntf stackIntf

     */

    /* Step 1.  */

    /* Initialize and get working copies */

    memset( (void *)&leftMask, 0, sizeof(leftMask) );
    memset( (void *)&rightMask, 0, sizeof(rightMask) );

    memcpy ((L7_uchar8 *) &leftMask, srcMask, sizeof(NIM_INTF_MASK_REL_4_3_t) );
    memcpy ((L7_uchar8 *) &rightMask, srcMask, sizeof(NIM_INTF_MASK_REL_4_3_t) );

    bits_in_mask = sizeof(NIM_INTF_MASK_REL_4_4_t)*8;

    /* Step 2.  */

    shiftBits = bits_in_mask - (L7_MAX_PORT_COUNT_REL_4_3 + L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_3 + L7_MAX_NUM_LAG_INTF_REL_4_3);
    (void)maskShiftRightBits((L7_uchar8 *)&leftMask, shiftBits, bits_in_mask);
    (void)maskShiftLeftBits((L7_uchar8 *)&leftMask, shiftBits, bits_in_mask);

    /* Now left shift to eliminate all the phy cpu and the first 6 lag interfaces */
    shiftBits = L7_MAX_PORT_COUNT_REL_4_3 + L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_3 + L7_MAX_NUM_LAG_INTF_REL_4_3;
    (void)maskShiftLeftBits( (L7_uchar8 *)&rightMask, shiftBits, bits_in_mask);

    /* Now right shift taking additional bits for the new lag interfaces */
    shiftBits = L7_MAX_PORT_COUNT_REL_4_3 + L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_3 + L7_MAX_NUM_LAG_INTF_REL_4_4;
    (void)maskShiftRightBits((L7_uchar8 *)&rightMask, shiftBits, bits_in_mask);

    /* Step 3.  */

    /* Get the combined mask */

    /* Now combine the valid bits in the two masks */
    (void)maskOrBits( (L7_uchar8 *) &leftMask,
                (L7_uchar8 *) &rightMask,bits_in_mask);


    /* Step 4.  */

    /* Copy  the adjusted mask to the current configuration

       CAUTION:  Ensure that the copy is for the same mask size!!!
    */

     memcpy ((L7_uchar8 *) destMask, (L7_uchar8 *) &leftMask,
             sizeof(NIM_INTF_MASK_REL_4_4_t) );

     /* Step 5  */


     return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Converts an IntIfNum from version 4_4 to 5_0.
*
* @param    intIfNum_r4_4    @b{(input)} version rel4_4 intIfNum
*
* @returns  version rel5_0 intIfNum
*
* @notes
*
*
* @end
*********************************************************************/
L7_uint32 sysapiCfgFileMigrateIntIfNumRel4_4ToRel5_0(L7_uint32 intIfNum_r4_4)
{
  /* nothing to do... */
  return intIfNum_r4_4;
}

/*********************************************************************
* @purpose  Converts an IntIfNum from version 4_3 to 4_4.
*
* @param    intIfNum_r4_3    @b{(input)} version rel4_3 intIfNum
*
* @returns  version rel4_4 intIfNum
*
* @notes
*
*
* @end
*********************************************************************/
L7_uint32 sysapiCfgFileMigrateIntIfNumRel4_3ToRel4_4(L7_uint32 intIfNum_r4_3)
{
  /* nothing to do... */
  return intIfNum_r4_3;
}

/*********************************************************************
* @purpose  Converts an IntIfNum from version 4_2 to 4_3.
*
* @param    intIfNum_r4_2  @b{(input)} version rel4_2 intIfNum
*
* @returns  version rel4_3 intIfNum
*
* @notes
*
*
* @end
*********************************************************************/
L7_uint32 sysapiCfgFileMigrateIntIfNumRel4_2ToRel4_3(L7_uint32 intIfNum_r4_2)
{
  L7_uint32 i;
  L7_uint32 retval = 0;
  NIM_INTF_MASK_REL_4_2_t v2mask;
  NIM_INTF_MASK_REL_4_3_t v3mask;

  /* dont bother with 0 */
  if(intIfNum_r4_2 == 0)
    return 0;

  memset(&v2mask, 0, sizeof(v2mask));
  memset(&v3mask, 0, sizeof(v3mask));

  maskSetBit(v2mask.value, intIfNum_r4_2, (sizeof(NIM_INTF_MASK_REL_4_2_t)*8));
  sysapiCfgFileMigrateMaskRel4_2To4_3(&v2mask, &v3mask);

  /* calculate intIfNum from the mask */
  for(i = 0; i < NIM_INTF_INDICES_REL_4_3; i++)
  {
    if(v3mask.value[i] == 0)
    {
      retval += 8;
    }
    else
    {
      /* found the bit! */
      while(v3mask.value[i] != 0)
      {
        retval++;
        v3mask.value[i] >>= 1;
      }

      break;
    }
  }

  return retval;
}
