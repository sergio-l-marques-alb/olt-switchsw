
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename cardmgr_migrate.c
*
* @purpose cardmgr Configuration Migration
*
* @component cardmgr
*
* @comments cardmgr
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include "cardmgr_migrate.h"

static L7_RC_t cardMgrMigrateConfigV1V2Convert (L7_cardMgrCfgFileDataV1_t * pCfgV1,
                                                L7_cardMgrCfgFileDataV2_t * pCfgV2);
static L7_RC_t cardMgrMigrateConfigV2V3Convert (L7_cardMgrCfgFileDataV2_t * pCfgV2,
                                                L7_cardMgrCfgFileDataV3_t * pCfgV3);
static void cmgrBuildDefaultConfigDataV2 (L7_cardMgrCfgFileDataV2_t * pCfgV2);
static void cmgrBuildDefaultConfigDataV3 (L7_cardMgrCfgFileDataV3_t * pCfgV3);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static L7_cardMgrCfgFileData_t *pCfgCur = L7_NULLPTR;

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
cardMgrMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  L7_cardMgrCfgFileDataV1_t *pCfgV1 = L7_NULLPTR;
  L7_cardMgrCfgFileDataV2_t *pCfgV2 = L7_NULLPTR;
  L7_cardMgrCfgFileDataV3_t *pCfgV3 = L7_NULLPTR;

  SYSAPI_MIGRATE_UNUSED (ver);
  pCfgCur = (L7_cardMgrCfgFileData_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  if (pCfgHdr->version == CMGR_CFG_VER_1 && pCfgHdr->length == sizeof (L7_cardMgrCfgFileDataV2_t))
  {
    oldVer = CMGR_CFG_VER_2;
  }

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case CMGR_CFG_VER_1:
    pCfgV1 = (L7_cardMgrCfgFileDataV1_t *) osapiMalloc (L7_CARDMGR_COMPONENT_ID,
                                                        (L7_uint32)
                                                        sizeof (L7_cardMgrCfgFileDataV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (L7_cardMgrCfgFileDataV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = CMGR_CFG_VER_1;
    break;

  case CMGR_CFG_VER_2:
    pCfgV2 = (L7_cardMgrCfgFileDataV2_t *) osapiMalloc (L7_CARDMGR_COMPONENT_ID,
                                                        (L7_uint32)
                                                        sizeof (L7_cardMgrCfgFileDataV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (L7_cardMgrCfgFileDataV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = pCfgHdr->version;       /* b'cos REL_H2 version not updated */
    break;

  case CMGR_CFG_VER_3:
    pCfgV3 = (L7_cardMgrCfgFileDataV3_t *) osapiMalloc (L7_CARDMGR_COMPONENT_ID,
                                                        (L7_uint32)
                                                        sizeof (L7_cardMgrCfgFileDataV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (L7_cardMgrCfgFileDataV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = CMGR_CFG_VER_3;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_CARDMGR_COMPONENT_ID,
                                         CMGR_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_CARDMGR_COMPONENT_ID,
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
    case CMGR_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (L7_cardMgrCfgFileDataV2_t *) osapiMalloc (L7_CARDMGR_COMPONENT_ID,
                                                          (L7_uint32)
                                                          sizeof (L7_cardMgrCfgFileDataV2_t));
      if ((L7_NULLPTR == pCfgV2) ||
          (L7_SUCCESS != cardMgrMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_CARDMGR_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case CMGR_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (L7_cardMgrCfgFileDataV3_t *) osapiMalloc (L7_CARDMGR_COMPONENT_ID,
                                                          (L7_uint32)
                                                          sizeof (L7_cardMgrCfgFileDataV3_t));
      if ((L7_NULLPTR == pCfgV3) ||
          (L7_SUCCESS != cardMgrMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_CARDMGR_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case CMGR_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV3, sizeof (*pCfgCur));
      osapiFree (L7_CARDMGR_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;

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
    osapiFree (L7_CARDMGR_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_CARDMGR_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_CARDMGR_COMPONENT_ID, pCfgV3);

  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_CARDMGR_COMPONENT_ID,
            "Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    cmgrBuildDefaultConfigData (CMGR_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V1 to V2.
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
cardMgrMigrateConfigV1V2Convert (L7_cardMgrCfgFileDataV1_t * pCfgV1,
                                 L7_cardMgrCfgFileDataV2_t * pCfgV2)
{
  L7_uint32 i, j, unitMax, slotMax;
  L7_uint32 new_slot_index;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != CMGR_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, CMGR_CFG_VER_1);
    return L7_FAILURE;
  }

  cmgrBuildDefaultConfigDataV2 (pCfgV2);

  unitMax = min (L7_MAX_UNITS_PER_STACK_REL_4_1, L7_MAX_UNITS_PER_STACK_REL_4_3);
  slotMax = min (L7_MAX_SLOTS_PER_UNIT_REL_4_1, L7_MAX_SLOTS_PER_UNIT_REL_4_3);
  for (i = 0; i <= unitMax; i++)
  {
    new_slot_index = 0;
    for (j = 0; j < slotMax; j++)
    {
      if (pCfgV1->cfgParms.configCard[i][j].card_id ==  0x56650101)
      {
          /* The 4-port Gig card is not supported in I. All ports
          ** are located in one card on the reference platform.
          */
          continue;
      }
      MIGRATE_COPY_STRUCT (pCfgV2->cfgParms.configCard[i][new_slot_index],
              pCfgV1->cfgParms.configCard[i][j]);
      new_slot_index++;

    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V2 to V3.
*
* @param    pCfgV2    @b{(input)} ptr to version 2 config data structure
* @param    pCfgV3    @b{(input)} ptr to version 3 config data structure
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
cardMgrMigrateConfigV2V3Convert (L7_cardMgrCfgFileDataV2_t * pCfgV2,
                                 L7_cardMgrCfgFileDataV3_t * pCfgV3)
{
  L7_uint32 i, j, unitMax, slotMax;
  L7_uint32 new_slot_index;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != CMGR_CFG_VER_1 &&
      pCfgV2->cfgHdr.version != CMGR_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, CMGR_CFG_VER_2);
    return L7_FAILURE;
  }

  cmgrBuildDefaultConfigDataV3 (pCfgV3);

  unitMax = min (L7_MAX_UNITS_PER_STACK_REL_4_3, L7_MAX_UNITS_PER_STACK_REL_4_4);
  slotMax = min (L7_MAX_SLOTS_PER_UNIT_REL_4_3, L7_MAX_SLOTS_PER_UNIT_REL_4_4);
  for (i = 0; i <= unitMax; i++)
  {
    new_slot_index = 0;
    for (j = 0; j < slotMax; j++)
    {
      MIGRATE_COPY_STRUCT (pCfgV3->cfgParms.configCard[i][new_slot_index],
              pCfgV2->cfgParms.configCard[i][j]);
      new_slot_index++;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build Version 2 defaults
*
* @param    pCfgV2    @b{(input)} ptr to version 2 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
cmgrBuildDefaultConfigDataV2 (L7_cardMgrCfgFileDataV2_t * pCfgV2)
{
  /*
   * Copied from the V2 version of cmgrBuildDefaultConfigData()
   */
  L7_uint32 unit, slot;
  L7_uint32 maxUnits, maxSlots;

  maxUnits = L7_MAX_UNITS_PER_STACK_REL_4_3;
  maxSlots = L7_MAX_SLOTS_PER_UNIT_REL_4_3;

  memset((void *)pCfgV2, 0, sizeof(*pCfgV2));
  strcpy(pCfgV2->cfgHdr.filename, CMGR_CFG_FILENAME);
  pCfgV2->cfgHdr.version = CMGR_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_CARDMGR_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = sizeof(*pCfgV2);
  pCfgV2->cfgHdr.dataChanged = L7_FALSE;

  for (unit = 0; unit <= maxUnits; unit++)
  {
    for (slot = 0; slot <= maxSlots; slot++)
    {
      pCfgV2->cfgParms.configCard[unit][slot].card_id = L7_CMGR_CARD_ID_NONE;
      pCfgV2->cfgParms.configCard[unit][slot].adminMode = L7_ENABLE;
      pCfgV2->cfgParms.configCard[unit][slot].powerMode = L7_ENABLE;
    }
  }
}

/*********************************************************************
* @purpose  Build Version 3 defaults
*
* @param    pCfgV3    @b{(input)} ptr to version 3 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
cmgrBuildDefaultConfigDataV3 (L7_cardMgrCfgFileDataV3_t * pCfgV3)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (L7_cardMgrCfgFileDataV3_t));
  cmgrBuildDefaultConfigData (CMGR_CFG_VER_3);
  memcpy ((L7_uchar8 *) pCfgV3, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}
