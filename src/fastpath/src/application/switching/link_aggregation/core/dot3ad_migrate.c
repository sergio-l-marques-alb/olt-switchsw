
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename dot3ad_migrate.c
*
* @purpose dot3ad Configuration Migration
*
* @component dot3ad
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include "dot3ad_migrate.h"
#include "dot3ad_exports.h"
#include "simapi.h"


/* Converts an IntIfNum from release 4_2 to 4_3. Defined in nim_migrate.c */
extern L7_uint32 sysapiCfgFileMigrateIntIfNumRel4_2ToRel4_3(L7_uint32 intIfNum_r4_2);
extern L7_uint32 sysapiCfgFileMigrateIntIfNumRel4_3ToRel4_4(L7_uint32 intIfNum_r4_2);
extern L7_uint32 sysapiCfgFileMigrateIntIfNumRel4_4ToRel5_0(L7_uint32 intIfNum_r4_4);

static L7_RC_t dot3adMigrateConfigV1V3Convert (dot3adCfgV1_t * pCfgV1, dot3adCfgV3_t * pCfgV3);
static L7_RC_t dot3adMigrateConfigV2V3Convert (dot3adCfgV2_t * pCfgV2, dot3adCfgV3_t * pCfgV3);
static L7_RC_t dot3adMigrateConfigV3V4Convert (dot3adCfgV3_t * pCfgV3, dot3adCfgV4_t * pCfgV4);
static L7_RC_t dot3adMigrateConfigV4V5Convert (dot3adCfgV4_t * pCfgV4, dot3adCfgV5_t * pCfgV5);
void dot3adBuildConfigDataV2 (dot3adCfgV2_t * pCfgV2);
static void dot3adBuildConfigDataV3 (dot3adCfgV3_t * pCfgV3);
static void dot3adBuildConfigDataV4 (dot3adCfgV4_t * pCfgV4);
static void dot3adBuildConfigDataV5 (dot3adCfgV5_t * pCfgV5);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static dot3adCfg_t *pCfgCur = L7_NULLPTR;

extern L7_BOOL defaultsBuilt;
void dot3adConfigDataTestShow(void);


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
dot3adMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  dot3adCfgV1_t *pCfgV1 = L7_NULLPTR;
  dot3adCfgV2_t *pCfgV2 = L7_NULLPTR;
  dot3adCfgV3_t *pCfgV3 = L7_NULLPTR;
  dot3adCfgV4_t *pCfgV4 = L7_NULLPTR;
  dot3adCfgV5_t *pCfgV5 = L7_NULLPTR;

  pCfgCur = (dot3adCfg_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */
  switch (oldVer)
  {
  case DOT3AD_CFG_VER_1:
    pCfgV1 = (dot3adCfgV1_t *) osapiMalloc (L7_DOT3AD_COMPONENT_ID,
                                            (L7_uint32) sizeof (dot3adCfgV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (dot3adCfgV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = DOT3AD_CFG_VER_1;
    break;

  case DOT3AD_CFG_VER_2:
    pCfgV2 = (dot3adCfgV2_t *) osapiMalloc (L7_DOT3AD_COMPONENT_ID,
                                            (L7_uint32) sizeof (dot3adCfgV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (dot3adCfgV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = DOT3AD_CFG_VER_2;
    break;

  case DOT3AD_CFG_VER_3:
    pCfgV3 = (dot3adCfgV3_t *) osapiMalloc (L7_DOT3AD_COMPONENT_ID,
                                            (L7_uint32) sizeof (dot3adCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (dot3adCfgV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = DOT3AD_CFG_VER_3;
    break;

  case DOT3AD_CFG_VER_4:
    pCfgV4 = (dot3adCfgV4_t *) osapiMalloc (L7_DOT3AD_COMPONENT_ID,
                                            (L7_uint32) sizeof (dot3adCfgV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (dot3adCfgV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = DOT3AD_CFG_VER_4;
    break;

  case DOT3AD_CFG_VER_5:
    pCfgV5 = (dot3adCfgV5_t *) osapiMalloc (L7_DOT3AD_COMPONENT_ID,
                                            (L7_uint32) sizeof (dot3adCfgV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (dot3adCfgV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = DOT3AD_CFG_VER_5;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_DOT3AD_COMPONENT_ID,
                                         DOT3AD_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT3AD_COMPONENT_ID,
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

     !! except VER_1 which is converted directly to VER_3 !!
   */

  if (buildDefault != L7_TRUE)
  {
    switch (oldVer)
    {
    case DOT3AD_CFG_VER_1:
      /* convert directly to ver 2 */

      /* pCfgV1 has already been allocated and filled in */
      pCfgV3 = (dot3adCfgV3_t *) osapiMalloc (L7_DOT3AD_COMPONENT_ID,
                                              (L7_uint32) sizeof (dot3adCfgV3_t));
      if ((L7_NULLPTR == pCfgV1) || (L7_NULLPTR == pCfgV3) ||
          (L7_SUCCESS != dot3adMigrateConfigV1V3Convert (pCfgV1, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT3AD_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case DOT3AD_CFG_VER_2:

      /* DOT3AD_CFG_VER_1 is an exception! it converts directly to DOT3AD_CFG_VER_3 */
      if(oldVer != DOT3AD_CFG_VER_1)
      {
        /* pCfgV2 has already been allocated and filled in */
        pCfgV3 = (dot3adCfgV3_t *) osapiMalloc (L7_DOT3AD_COMPONENT_ID,
                                                (L7_uint32) sizeof (dot3adCfgV3_t));
        if ((L7_NULLPTR == pCfgV2) || (L7_NULLPTR == pCfgV3) ||
            (L7_SUCCESS != dot3adMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
        {
          buildDefault = L7_TRUE;
          break;
        }
        osapiFree (L7_DOT3AD_COMPONENT_ID, pCfgV2);
        pCfgV2 = L7_NULLPTR;
      }
      /*passthru */

    case DOT3AD_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (dot3adCfgV4_t *) osapiMalloc (L7_DOT3AD_COMPONENT_ID,
                                              (L7_uint32) sizeof (dot3adCfgV4_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_NULLPTR == pCfgV4) ||
          (L7_SUCCESS != dot3adMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT3AD_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case DOT3AD_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (dot3adCfgV5_t *) osapiMalloc (L7_DOT3AD_COMPONENT_ID,
                                              (L7_uint32) sizeof (dot3adCfgV5_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_NULLPTR == pCfgV5) ||
          (L7_SUCCESS != dot3adMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT3AD_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case DOT3AD_CFG_VER_5:
      /* pCfgV5 has already been allocated and filled in */
      if (L7_NULLPTR == pCfgV5 )
      {
        buildDefault = L7_TRUE;
        break;
      }
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV5, sizeof (*pCfgCur));
      osapiFree (L7_DOT3AD_COMPONENT_ID, pCfgV5);
      pCfgV5 = L7_NULLPTR;

      /* if static capability has been set in the previous version , reset staticcapability value*/
      if(pCfgCur->cfg.dot3adSystem.unused != 0)
         pCfgCur->cfg.dot3adSystem.unused = 0;

      /* done with migration - Flag unsaved configuration */
      pCfgCur->hdr.dataChanged = L7_TRUE;
      break;

    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV1 != L7_NULLPTR)
    osapiFree (L7_DOT3AD_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_DOT3AD_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_DOT3AD_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_DOT3AD_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_DOT3AD_COMPONENT_ID, pCfgV5);

  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT3AD_COMPONENT_ID,
            "Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    dot3adBuildConfigData (DOT3AD_CFG_VER_CURRENT);
    pCfgCur->hdr.dataChanged = L7_TRUE;
  }

  return;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V1 to V3.
*
* @param    pCfgV1    @b{(input)} ptr to version 1 config data structure
* @param    pCfgV3        @b{(input)} ptr to version 2 config data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Assumes the caller validated the version number of the old
*           config data prior to calling this conversion function.
*
*           This is an exception to the usual iterative migration in that
*           it converts directly to V3 skipping over V2!
*
* @end
*********************************************************************/
static L7_RC_t
dot3adMigrateConfigV1V3Convert (dot3adCfgV1_t * pCfgV1, dot3adCfgV3_t * pCfgV3)
{
  L7_uint32 i, j, count, idx;
  nimConfigID_t configId_v3;
  L7_INTF_TYPES_t type;
  L7_uint32 offset;

  /* verify correct version of old config file */
  if (pCfgV1->hdr.version != DOT3AD_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->hdr.version, DOT3AD_CFG_VER_1);
    return L7_FAILURE;
  }

  dot3adBuildConfigDataV3 (pCfgV3);

  count = min (L7_MAX_PORT_COUNT_REL_4_0 - 1, L7_MAX_PORT_COUNT_REL_4_3);
  for (i = 0; i <= count; i++)
  {
    if (pCfgV1->cfg.dot3adPort[i].actorPortNum != 0)
    {
      MIGRATE_COPY_STRUCT (pCfgV3->cfg.dot3adPort[i + 1], pCfgV1->cfg.dot3adPort[i]);

      idx = pCfgV3->cfg.dot3adPort[i + 1].actorPortNum;
      pCfgV3->cfg.dot3adPortIdx[idx] = idx;
    }
  }
  /* setup the LAGs */
  count = min (L7_MAX_NUM_LAG_INTF_REL_4_0, L7_MAX_NUM_LAG_INTF_REL_4_3);
  for (i = 0; i < count; i++)
  {
    if (pCfgV1->cfg.dot3adAgg[i].aggId != 0)
    {
      MIGRATE_COPY_STRUCT (pCfgV3->cfg.dot3adAgg[i], pCfgV1->cfg.dot3adAgg[i]);

      pCfgV3->cfg.dot3adAggIdx[pCfgV3->cfg.dot3adAgg[i].aggId] = i;
      /* Reset static mode of lag of pCfgV3 to L7_FALSE (dynamic) irrespective of the value in pCfgV1*/
      pCfgV3->cfg.dot3adAgg[i].isStatic = FD_DOT3AD_STATIC_MODE;

      /* migrate USP */
      for(j = 0; j < L7_MAX_MEMBERS_PER_LAG; j++)
      {
        memset(&configId_v3, 0, sizeof(configId_v3));

        if(sysapiCfgFileRel4_0SlotPortToIntfInfoGet(pCfgV1->cfg.dot3adAgg[i].aggPortListUsp[j].slot,
                                                 pCfgV1->cfg.dot3adAgg[i].aggPortListUsp[j].port,
                                                 &configId_v3, &offset, &type) != L7_SUCCESS)
        {
          MIGRATE_INVALID_SLOT_PORT(pCfgV1->cfg.dot3adAgg[i].aggPortListUsp[j].slot,
                                    pCfgV1->cfg.dot3adAgg[i].aggPortListUsp[j].port);
          pCfgV3->cfg.dot3adAgg[i].aggPortListUsp[j].unit = 0;
          pCfgV3->cfg.dot3adAgg[i].aggPortListUsp[j].slot = 0;
          pCfgV3->cfg.dot3adAgg[i].aggPortListUsp[j].port = 0;
        }
        else
        {
          pCfgV3->cfg.dot3adAgg[i].aggPortListUsp[j].unit = configId_v3.configSpecifier.usp.unit;
          pCfgV3->cfg.dot3adAgg[i].aggPortListUsp[j].slot = configId_v3.configSpecifier.usp.slot;
          pCfgV3->cfg.dot3adAgg[i].aggPortListUsp[j].port = configId_v3.configSpecifier.usp.port;
        }
      }
    }
  }

  MIGRATE_COPY_STRUCT (pCfgV3->cfg.dot3adSystem, pCfgV1->cfg.dot3adSystem);
  /* Reset system wide static capability flag of pCfgV3 to 0 irrespective of the value in pCfgV1*/
  /* since the system wide static capability flag is unused */

  pCfgV3->cfg.dot3adSystem.unused = 0;

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
dot3adMigrateConfigV2V3Convert (dot3adCfgV2_t * pCfgV2, dot3adCfgV3_t * pCfgV3)
{
  L7_uint32 i, j, count;
  L7_uint32 intIfNum_v3;

  /* verify correct version of old config file */
  if ((pCfgV2->hdr.version != DOT3AD_CFG_VER_2) && (pCfgV2->hdr.version != DOT3AD_CFG_VER_1))
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->hdr.version, DOT3AD_CFG_VER_2);
    return L7_FAILURE;
  }

  dot3adBuildConfigDataV3 (pCfgV3);

  /* dot3ad indexes into the port array using the infIfNum in addition to
   * using the explicit idx array. So we figure out the v3 intIfNum value
   * of the key (actorPortNum) and copy to that element
   */
  count = min (L7_MAX_PORT_COUNT_REL_4_1, L7_MAX_PORT_COUNT_REL_4_3);
  for (i = 0; i <= count; i++)
  {
    if (pCfgV2->cfg.dot3adPort[i].actorPortNum != 0)
    {
      /* clean out fields that are not needed */
      pCfgV2->cfg.dot3adPort[i].actorPortAggId          = 0;
      pCfgV2->cfg.dot3adPort[i].actorPortSelectedAggId  = 0;
      pCfgV2->cfg.dot3adPort[i].currentWhileTimer       = 0;
      pCfgV2->cfg.dot3adPort[i].periodicTimer           = 0;
      pCfgV2->cfg.dot3adPort[i].waitWhileTimer          = 0;
      pCfgV2->cfg.dot3adPort[i].nttTimer                = 0;

      /* convert actorPortNum to v3 intIfNum */
      intIfNum_v3 = sysapiCfgFileMigrateIntIfNumRel4_2ToRel4_3(pCfgV2->cfg.dot3adPort[i].actorPortNum);
      MIGRATE_ASSERT(intIfNum_v3 < (L7_MAX_PORT_COUNT + 1)); /* must be within bounds */

      /* copy data to new location! */
      MIGRATE_COPY_STRUCT (pCfgV3->cfg.dot3adPort[intIfNum_v3], pCfgV2->cfg.dot3adPort[i]);

      /* convert V2 intIfNum member(s) to V3 */
      pCfgV3->cfg.dot3adPort[intIfNum_v3].actorPortNum              = intIfNum_v3;
      pCfgV3->cfg.dot3adPort[intIfNum_v3].actorAdminPortKey         = sysapiCfgFileMigrateIntIfNumRel4_2ToRel4_3(pCfgV2->cfg.dot3adPort[i].actorAdminPortKey);
      pCfgV3->cfg.dot3adPort[intIfNum_v3].actorOperPortKey          = sysapiCfgFileMigrateIntIfNumRel4_2ToRel4_3(pCfgV2->cfg.dot3adPort[i].actorOperPortKey);
      pCfgV3->cfg.dot3adPort[intIfNum_v3].actorPortWaitSelectedAggId= sysapiCfgFileMigrateIntIfNumRel4_2ToRel4_3(pCfgV2->cfg.dot3adPort[i].actorPortWaitSelectedAggId);

      /* set v3 port idx */
      pCfgV3->cfg.dot3adPortIdx[intIfNum_v3] = intIfNum_v3;
    }
  }

  /* setup the LAGs */
  count = min (L7_MAX_NUM_LAG_INTF_REL_4_1, L7_MAX_NUM_LAG_INTF_REL_4_3);
  for (i = 0; i < count; i++)
  {
    if (pCfgV2->cfg.dot3adAgg[i].aggId != 0)
    {
      /* zero out data that can be re-created */
      memset(pCfgV2->cfg.dot3adAgg[i].aggActivePortList, 0, (L7_MAX_MEMBERS_PER_LAG * sizeof(L7_uint32)));
      memset(pCfgV2->cfg.dot3adAgg[i].aggPortList, 0, (L7_MAX_MEMBERS_PER_LAG * sizeof(L7_uint32)));
      memset(pCfgV2->cfg.dot3adAgg[i].aggSelectedPortList, 0, (L7_MAX_MEMBERS_PER_LAG * sizeof(L7_uint32)));

      pCfgV2->cfg.dot3adAgg[i].activeNumMembers       = 0;
      pCfgV2->cfg.dot3adAgg[i].currNumMembers         = 0;
      pCfgV2->cfg.dot3adAgg[i].currNumSelectedMembers = 0;

      /* copy data across */
      MIGRATE_COPY_STRUCT (pCfgV3->cfg.dot3adAgg[i], pCfgV2->cfg.dot3adAgg[i]);
      /* Reset static mode of lag of pCfgV3 to L7_FALSE (dynamic) irrespective of the value in pCfgV2*/
      pCfgV3->cfg.dot3adAgg[i].isStatic = FD_DOT3AD_STATIC_MODE;


      /* migrate key (aggId) and AggIdx */
      intIfNum_v3 = sysapiCfgFileMigrateIntIfNumRel4_2ToRel4_3(pCfgV2->cfg.dot3adAgg[i].aggId);
      MIGRATE_ASSERT(intIfNum_v3 < L7_MAX_INTERFACE_COUNT); /* v3 intIfNum must be within bounds */
      if (intIfNum_v3 >= L7_MAX_INTERFACE_COUNT_REL_4_3)
          continue;

      pCfgV3->cfg.dot3adAgg[i].aggId        = intIfNum_v3;
      pCfgV3->cfg.dot3adAggIdx[intIfNum_v3] = i;

      /* migrate intIfNum values */
      pCfgV3->cfg.dot3adAgg[i].actorAdminAggKey = sysapiCfgFileMigrateIntIfNumRel4_2ToRel4_3(pCfgV2->cfg.dot3adAgg[i].actorAdminAggKey);
      pCfgV3->cfg.dot3adAgg[i].actorOperAggKey  = sysapiCfgFileMigrateIntIfNumRel4_2ToRel4_3(pCfgV2->cfg.dot3adAgg[i].actorOperAggKey);

      for(j = 0; j < L7_MAX_MEMBERS_PER_LAG; j++)
        pCfgV3->cfg.dot3adAgg[i].aggWaitSelectedPortList[j] = sysapiCfgFileMigrateIntIfNumRel4_2ToRel4_3(pCfgV2->cfg.dot3adAgg[i].aggWaitSelectedPortList[j]);

    }
  }

  MIGRATE_COPY_STRUCT (pCfgV3->cfg.dot3adSystem, pCfgV2->cfg.dot3adSystem);
   /* Reset system wide static capability flag of pCfgV3 to 0 irrespective of the value in pCfgV2*/
  /* since the system wide static capability flag is unused */
  pCfgV3->cfg.dot3adSystem.unused = 0;

  defaultsBuilt = L7_FALSE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V3 to V4.
*
* @param    pCfgV3    @b{(input)} ptr to version 2 config data structure
* @param    pCfgV4    @b{(input)} ptr to version 3 config data structure
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
dot3adMigrateConfigV3V4Convert (dot3adCfgV3_t * pCfgV3, dot3adCfgV4_t * pCfgV4)
{
  L7_uint32 i, j, count;
  L7_uint32 intIfNum_V4;

  /* verify correct version of old config file */
  if ((pCfgV3->hdr.version != DOT3AD_CFG_VER_3) &&
      (pCfgV3->hdr.version != DOT3AD_CFG_VER_2) &&
      (pCfgV3->hdr.version != DOT3AD_CFG_VER_1))
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->hdr.version, DOT3AD_CFG_VER_3);
    return L7_FAILURE;
  }

  dot3adBuildConfigDataV4 (pCfgV4);

  /* dot3ad indexes into the port array using the infIfNum in addition to
   * using the explicit idx array. So we figure out the v3 intIfNum value
   * of the key (actorPortNum) and copy to that element
   */
  count = min (L7_MAX_PORT_COUNT_REL_4_3, L7_MAX_PORT_COUNT_REL_4_4);
  for (i = 0; i <= count; i++)
  {
    if (pCfgV3->cfg.dot3adPort[i].actorPortNum != 0)
    {
      /* clean out fields that are not needed */
      pCfgV3->cfg.dot3adPort[i].actorPortAggId          = 0;
      pCfgV3->cfg.dot3adPort[i].actorPortSelectedAggId  = 0;
      pCfgV3->cfg.dot3adPort[i].currentWhileTimer       = 0;
      pCfgV3->cfg.dot3adPort[i].periodicTimer           = 0;
      pCfgV3->cfg.dot3adPort[i].waitWhileTimer          = 0;
      pCfgV3->cfg.dot3adPort[i].nttTimer                = 0;

      /* convert actorPortNum to v3 intIfNum */
      intIfNum_V4 = sysapiCfgFileMigrateIntIfNumRel4_3ToRel4_4(pCfgV3->cfg.dot3adPort[i].actorPortNum);
      MIGRATE_ASSERT(intIfNum_V4 < (L7_MAX_PORT_COUNT + 1)); /* must be within bounds */
      if(intIfNum_V4 >= (L7_MAX_PORT_COUNT_REL_4_4 + 1))
          continue;

      /* copy data to new location! */
      MIGRATE_COPY_STRUCT (pCfgV4->cfg.dot3adPort[intIfNum_V4], pCfgV3->cfg.dot3adPort[i]);

      /* convert V3 intIfNum member(s) to V4 */
      pCfgV4->cfg.dot3adPort[intIfNum_V4].actorPortNum              = intIfNum_V4;
      pCfgV4->cfg.dot3adPort[intIfNum_V4].actorAdminPortKey         = sysapiCfgFileMigrateIntIfNumRel4_3ToRel4_4(pCfgV3->cfg.dot3adPort[i].actorAdminPortKey);
      pCfgV4->cfg.dot3adPort[intIfNum_V4].actorOperPortKey          = sysapiCfgFileMigrateIntIfNumRel4_3ToRel4_4(pCfgV3->cfg.dot3adPort[i].actorOperPortKey);
      pCfgV4->cfg.dot3adPort[intIfNum_V4].actorPortWaitSelectedAggId= sysapiCfgFileMigrateIntIfNumRel4_3ToRel4_4(pCfgV3->cfg.dot3adPort[i].actorPortWaitSelectedAggId);

      /* set v3 port idx */
      pCfgV4->cfg.dot3adPortIdx[intIfNum_V4] = intIfNum_V4;
    }
  }

  /* setup the LAGs */
  count = min (L7_MAX_NUM_LAG_INTF_REL_4_3, L7_MAX_NUM_LAG_INTF_REL_4_4);
  for (i = 0; i < count; i++)
  {
    if (pCfgV3->cfg.dot3adAgg[i].aggId != 0)
    {
      /* zero out data that can be re-created */
      memset(pCfgV3->cfg.dot3adAgg[i].aggActivePortList, 0, (L7_MAX_MEMBERS_PER_LAG * sizeof(L7_uint32)));
      memset(pCfgV3->cfg.dot3adAgg[i].aggPortList, 0, (L7_MAX_MEMBERS_PER_LAG * sizeof(L7_uint32)));
      memset(pCfgV3->cfg.dot3adAgg[i].aggSelectedPortList, 0, (L7_MAX_MEMBERS_PER_LAG * sizeof(L7_uint32)));

      pCfgV3->cfg.dot3adAgg[i].activeNumMembers       = 0;
      pCfgV3->cfg.dot3adAgg[i].currNumMembers         = 0;
      pCfgV3->cfg.dot3adAgg[i].currNumSelectedMembers = 0;

      /* copy data across */
      MIGRATE_COPY_STRUCT (pCfgV4->cfg.dot3adAgg[i], pCfgV3->cfg.dot3adAgg[i]);

      /* migrate key (aggId) and AggIdx */
      intIfNum_V4 = sysapiCfgFileMigrateIntIfNumRel4_3ToRel4_4(pCfgV3->cfg.dot3adAgg[i].aggId);
      MIGRATE_ASSERT(intIfNum_V4 < L7_MAX_INTERFACE_COUNT); /* v3 intIfNum must be within bounds */
      if (intIfNum_V4 >= L7_MAX_INTERFACE_COUNT_REL_4_4)
          continue;


      pCfgV4->cfg.dot3adAgg[i].aggId        = intIfNum_V4;
      pCfgV4->cfg.dot3adAggIdx[intIfNum_V4] = i;

      /* migrate intIfNum values */
      pCfgV4->cfg.dot3adAgg[i].actorAdminAggKey = sysapiCfgFileMigrateIntIfNumRel4_3ToRel4_4(pCfgV3->cfg.dot3adAgg[i].actorAdminAggKey);
      pCfgV4->cfg.dot3adAgg[i].actorOperAggKey  = sysapiCfgFileMigrateIntIfNumRel4_3ToRel4_4(pCfgV3->cfg.dot3adAgg[i].actorOperAggKey);

      for(j = 0; j < L7_MAX_MEMBERS_PER_LAG; j++)
        pCfgV4->cfg.dot3adAgg[i].aggWaitSelectedPortList[j] = sysapiCfgFileMigrateIntIfNumRel4_3ToRel4_4(pCfgV3->cfg.dot3adAgg[i].aggWaitSelectedPortList[j]);

    }
  }

  MIGRATE_COPY_STRUCT (pCfgV4->cfg.dot3adSystem, pCfgV3->cfg.dot3adSystem);

  defaultsBuilt = L7_FALSE;

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
dot3adMigrateConfigV4V5Convert (dot3adCfgV4_t * pCfgV4, dot3adCfgV5_t * pCfgV5)
{
  L7_uint32 i, j, count;
  L7_uint32 intIfNum_V5;

  /* verify correct version of old config file */
  if ((pCfgV4->hdr.version != DOT3AD_CFG_VER_4) &&
      (pCfgV4->hdr.version != DOT3AD_CFG_VER_3) &&
      (pCfgV4->hdr.version != DOT3AD_CFG_VER_2) &&
      (pCfgV4->hdr.version != DOT3AD_CFG_VER_1))
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->hdr.version, DOT3AD_CFG_VER_4);
    return L7_FAILURE;
  }

  dot3adBuildConfigDataV5 (pCfgV5);

  /* dot3ad indexes into the port array using the infIfNum in addition to
   * using the explicit idx array. So we figure out the v3 intIfNum value
   * of the key (actorPortNum) and copy to that element
   */
  count = min (L7_MAX_PORT_COUNT_REL_4_4, L7_MAX_PORT_COUNT_REL_5_0);
  for (i = 0; i <= count; i++)
  {
    if (pCfgV4->cfg.dot3adPort[i].actorPortNum != 0)
    {
      /* clean out fields that are not needed */
      pCfgV4->cfg.dot3adPort[i].actorPortAggId          = 0;
      pCfgV4->cfg.dot3adPort[i].actorPortSelectedAggId  = 0;
      pCfgV4->cfg.dot3adPort[i].currentWhileTimer       = 0;
      pCfgV4->cfg.dot3adPort[i].periodicTimer           = 0;
      pCfgV4->cfg.dot3adPort[i].waitWhileTimer          = 0;
      pCfgV4->cfg.dot3adPort[i].nttTimer                = 0;

      /* convert actorPortNum to v3 intIfNum */
      intIfNum_V5 = sysapiCfgFileMigrateIntIfNumRel4_4ToRel5_0(pCfgV4->cfg.dot3adPort[i].actorPortNum);
      MIGRATE_ASSERT(intIfNum_V5 < (L7_MAX_PORT_COUNT + 1)); /* must be within bounds */

      if( intIfNum_V5 >= (L7_MAX_PORT_COUNT + 1)) {
          /*avoid illegal indexing of the array*/
          continue;
      }

      /* copy data to new location! */
      MIGRATE_COPY_STRUCT (pCfgV5->cfg.dot3adPort[intIfNum_V5], pCfgV4->cfg.dot3adPort[i]);

      /* convert V4 intIfNum member(s) to V5 */
      pCfgV5->cfg.dot3adPort[intIfNum_V5].actorPortNum              = intIfNum_V5;
      pCfgV5->cfg.dot3adPort[intIfNum_V5].actorAdminPortKey         = sysapiCfgFileMigrateIntIfNumRel4_4ToRel5_0(pCfgV4->cfg.dot3adPort[i].actorAdminPortKey);
      pCfgV5->cfg.dot3adPort[intIfNum_V5].actorOperPortKey          = sysapiCfgFileMigrateIntIfNumRel4_4ToRel5_0(pCfgV4->cfg.dot3adPort[i].actorOperPortKey);
      pCfgV5->cfg.dot3adPort[intIfNum_V5].actorPortWaitSelectedAggId= sysapiCfgFileMigrateIntIfNumRel4_4ToRel5_0(pCfgV4->cfg.dot3adPort[i].actorPortWaitSelectedAggId);

      /* set v5 port idx */
      pCfgV5->cfg.dot3adPortIdx[intIfNum_V5] = intIfNum_V5;
    }
  }

  /* setup the LAGs */
  count = min (L7_MAX_NUM_LAG_INTF_REL_4_4, L7_MAX_NUM_LAG_INTF_REL_5_0);
  for (i = 0; i < count; i++)
  {
    if (pCfgV4->cfg.dot3adAgg[i].aggId != 0)
    {
      /* zero out data that can be re-created */
      memset(pCfgV4->cfg.dot3adAgg[i].aggActivePortList, 0, (L7_MAX_MEMBERS_PER_LAG * sizeof(L7_uint32)));
      memset(pCfgV4->cfg.dot3adAgg[i].aggPortList, 0, (L7_MAX_MEMBERS_PER_LAG * sizeof(L7_uint32)));
      memset(pCfgV4->cfg.dot3adAgg[i].aggSelectedPortList, 0, (L7_MAX_MEMBERS_PER_LAG * sizeof(L7_uint32)));

      pCfgV4->cfg.dot3adAgg[i].activeNumMembers       = 0;
      pCfgV4->cfg.dot3adAgg[i].currNumMembers         = 0;
      pCfgV4->cfg.dot3adAgg[i].currNumSelectedMembers = 0;

      /* copy data across */
      MIGRATE_COPY_STRUCT (pCfgV5->cfg.dot3adAgg[i].aggMacAddr, pCfgV4->cfg.dot3adAgg[i].aggMacAddr);
      MIGRATE_COPY_STRUCT (pCfgV5->cfg.dot3adAgg[i].partnerSys, pCfgV4->cfg.dot3adAgg[i].partnerSys);
      strcpy(pCfgV5->cfg.dot3adAgg[i].name, pCfgV4->cfg.dot3adAgg[i].name);
      for (j = 0;j < L7_MAX_MEMBERS_PER_LAG; j++)
        MIGRATE_COPY_STRUCT (pCfgV5->cfg.dot3adAgg[i].aggPortListUsp[j], pCfgV4->cfg.dot3adAgg[i].aggPortListUsp[j]);

      pCfgV5->cfg.dot3adAgg[i].individualAgg                = pCfgV4->cfg.dot3adAgg[i].individualAgg;
      pCfgV5->cfg.dot3adAgg[i].partnerSysPri                = pCfgV4->cfg.dot3adAgg[i].partnerSysPri;
      pCfgV5->cfg.dot3adAgg[i].partnerOperAggKey            = pCfgV4->cfg.dot3adAgg[i].partnerOperAggKey;
      pCfgV5->cfg.dot3adAgg[i].collectorMaxDelay            = pCfgV4->cfg.dot3adAgg[i].collectorMaxDelay;
      pCfgV5->cfg.dot3adAgg[i].rxState                      = pCfgV4->cfg.dot3adAgg[i].rxState;
      pCfgV5->cfg.dot3adAgg[i].txState                      = pCfgV4->cfg.dot3adAgg[i].txState;
      pCfgV5->cfg.dot3adAgg[i].currNumWaitSelectedMembers   = pCfgV4->cfg.dot3adAgg[i].currNumWaitSelectedMembers;
      pCfgV5->cfg.dot3adAgg[i].ready                        = pCfgV4->cfg.dot3adAgg[i].ready;
      pCfgV5->cfg.dot3adAgg[i].inuse                        = pCfgV4->cfg.dot3adAgg[i].inuse;
      pCfgV5->cfg.dot3adAgg[i].adminMode                    = pCfgV4->cfg.dot3adAgg[i].adminMode;
      pCfgV5->cfg.dot3adAgg[i].unused                       = pCfgV4->cfg.dot3adAgg[i].unused;
      pCfgV5->cfg.dot3adAgg[i].stpMode                      = pCfgV4->cfg.dot3adAgg[i].stpMode;
      pCfgV5->cfg.dot3adAgg[i].isStatic                     = pCfgV4->cfg.dot3adAgg[i].isStatic;
      pCfgV5->cfg.dot3adAgg[i].hashMode                     = FD_DOT3AD_HASH_MODE;

      /* migrate key (aggId) and AggIdx */
      intIfNum_V5 = sysapiCfgFileMigrateIntIfNumRel4_4ToRel5_0(pCfgV4->cfg.dot3adAgg[i].aggId);
      MIGRATE_ASSERT(intIfNum_V5 < L7_MAX_INTERFACE_COUNT); /* v5 intIfNum must be within bounds */

      if (intIfNum_V5 >= L7_MAX_INTERFACE_COUNT ) {
          /*avoid illegal indexing of the array*/
          continue;
      }

      pCfgV5->cfg.dot3adAgg[i].aggId        = intIfNum_V5;
      pCfgV5->cfg.dot3adAggIdx[intIfNum_V5] = i;

      /* migrate intIfNum values */
      pCfgV5->cfg.dot3adAgg[i].actorAdminAggKey = sysapiCfgFileMigrateIntIfNumRel4_4ToRel5_0(pCfgV4->cfg.dot3adAgg[i].actorAdminAggKey);
      pCfgV5->cfg.dot3adAgg[i].actorOperAggKey  = sysapiCfgFileMigrateIntIfNumRel4_4ToRel5_0(pCfgV4->cfg.dot3adAgg[i].actorOperAggKey);

      for(j = 0; j < L7_MAX_MEMBERS_PER_LAG; j++)
        pCfgV5->cfg.dot3adAgg[i].aggWaitSelectedPortList[j] = sysapiCfgFileMigrateIntIfNumRel4_4ToRel5_0(pCfgV4->cfg.dot3adAgg[i].aggWaitSelectedPortList[j]);
    }
  }

  MIGRATE_COPY_STRUCT (pCfgV5->cfg.dot3adSystem, pCfgV4->cfg.dot3adSystem);

  defaultsBuilt = L7_FALSE;

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
void
dot3adBuildConfigDataV2 (dot3adCfgV2_t * pCfgV2)
{
  L7_uint32 i, j;
  L7_uint32 maxIntf, maxLagIntf;

  memset (pCfgV2, 0, sizeof (*pCfgV2));

  maxIntf = L7_MAX_PORT_COUNT_REL_4_1 + 1;
  maxLagIntf = L7_MAX_NUM_LAG_INTF_REL_4_1;

  /* setup the LAGs */
  for (j = 0; j < maxLagIntf; j++)
  {
    (void) dot3adLagIntfDefaultBuildV4 (j, j, DOT3AD_CFG_VER_2, &pCfgV2->cfg.dot3adAgg[j],
                                      &pCfgV2->cfg.dot3adAggIdx[j]);
  }

  /* setup the Physical interfaces */
  for (i = 0; i < maxIntf; i++)
  {
    (void) dot3adPhysIntfDefaultBuild (i, DOT3AD_CFG_VER_2, &pCfgV2->cfg.dot3adPort[i],
                                       &pCfgV2->cfg.dot3adPortIdx[i]);
  }

  simMacAddrGet(pCfgV2->cfg.dot3adSystem.actorSys.addr);

  pCfgV2->cfg.dot3adSystem.actorSysPriority = FD_DOT3AD_DEFAULT_PRIORITY;

  /* setup file header */
  pCfgV2->hdr.version = DOT3AD_CFG_VER_2;
  pCfgV2->hdr.componentID = L7_DOT3AD_COMPONENT_ID;
  pCfgV2->hdr.type = L7_CFG_DATA;
  pCfgV2->hdr.length = (L7_uint32) sizeof (*pCfgV2);
  strcpy ((L7_char8 *) pCfgV2->hdr.filename, DOT3AD_CFG_FILENAME);
  pCfgV2->hdr.dataChanged = L7_FALSE;

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
dot3adBuildConfigDataV3 (dot3adCfgV3_t * pCfgV3)
{
  L7_uint32 i,j;
  L7_uint32 maxIntf,maxLagIntf;

  /* setup file header */
  pCfgV3->hdr.version = DOT3AD_CFG_VER_3;
  pCfgV3->hdr.componentID = L7_DOT3AD_COMPONENT_ID;
  pCfgV3->hdr.type = L7_CFG_DATA;
  pCfgV3->hdr.length = (L7_uint32)sizeof(*pCfgV3);
  strcpy((L7_char8 *)pCfgV3->hdr.filename, DOT3AD_CFG_FILENAME);
  pCfgV3->hdr.dataChanged = L7_FALSE;

  /* zero overlays */
  memset((void*)&pCfgV3->cfg.dot3adAgg, 0, sizeof(pCfgV3->cfg.dot3adAgg));
  memset((void*)&pCfgV3->cfg.dot3adPort, 0, sizeof(pCfgV3->cfg.dot3adPort));
  memset((void*)&pCfgV3->cfg.dot3adAggIdx, 0, sizeof(pCfgV3->cfg.dot3adAggIdx));
  memset((void*)&pCfgV3->cfg.dot3adPortIdx, 0 ,sizeof(pCfgV3->cfg.dot3adPortIdx));

  maxIntf = L7_MAX_PORT_COUNT_REL_4_3 + 1;
  maxLagIntf = L7_MAX_NUM_LAG_INTF_REL_4_3;

  /* setup the LAGs */
  for (j=0 ;j < maxLagIntf; j++)
  {
    (void)dot3adLagIntfDefaultBuildV4(j, j, DOT3AD_CFG_VER_3,
                                    &pCfgV3->cfg.dot3adAgg[j],
                                    &pCfgV3->cfg.dot3adAggIdx[j]);
  }

  /* setup the Physical interfaces */
  for (i=0; i < maxIntf; i++)
  {
    (void)dot3adPhysIntfDefaultBuild(i, DOT3AD_CFG_VER_3,
                                     &pCfgV3->cfg.dot3adPort[i],
                                     &pCfgV3->cfg.dot3adPortIdx[i]);
  }

  simMacAddrGet(pCfgV3->cfg.dot3adSystem.actorSys.addr);

  pCfgV3->cfg.dot3adSystem.actorSysPriority = FD_DOT3AD_DEFAULT_PRIORITY;

  defaultsBuilt = L7_TRUE;
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
dot3adBuildConfigDataV4 (dot3adCfgV4_t * pCfgV4)
{
  L7_uint32 i,j;
  L7_uint32 maxIntf,maxLagIntf;

  /* setup file header */
  pCfgV4->hdr.version = DOT3AD_CFG_VER_4;
  pCfgV4->hdr.componentID = L7_DOT3AD_COMPONENT_ID;
  pCfgV4->hdr.type = L7_CFG_DATA;
  pCfgV4->hdr.length = (L7_uint32)sizeof(*pCfgV4);
  strcpy((L7_char8 *)pCfgV4->hdr.filename, DOT3AD_CFG_FILENAME);
  pCfgV4->hdr.dataChanged = L7_FALSE;

  /* zero overlays */
  memset((void*)&pCfgV4->cfg.dot3adAgg, 0, sizeof(pCfgV4->cfg.dot3adAgg));
  memset((void*)&pCfgV4->cfg.dot3adPort, 0, sizeof(pCfgV4->cfg.dot3adPort));
  memset((void*)&pCfgV4->cfg.dot3adAggIdx, 0, sizeof(pCfgV4->cfg.dot3adAggIdx));
  memset((void*)&pCfgV4->cfg.dot3adPortIdx, 0 ,sizeof(pCfgV4->cfg.dot3adPortIdx));

  maxIntf = L7_MAX_PORT_COUNT_REL_4_4 + 1;
  maxLagIntf = L7_MAX_NUM_LAG_INTF_REL_4_4;

  /* setup the LAGs */
  for (j=0 ;j < maxLagIntf; j++)
  {
    (void)dot3adLagIntfDefaultBuildV4(j, j, DOT3AD_CFG_VER_4,
                                    &pCfgV4->cfg.dot3adAgg[j],
                                    &pCfgV4->cfg.dot3adAggIdx[j]);
  }

  /* setup the Physical interfaces */
  for (i=0; i < maxIntf; i++)
  {
    (void)dot3adPhysIntfDefaultBuild(i, DOT3AD_CFG_VER_4,
                                     &pCfgV4->cfg.dot3adPort[i],
                                     &pCfgV4->cfg.dot3adPortIdx[i]);
  }

  simMacAddrGet(pCfgV4->cfg.dot3adSystem.actorSys.addr);

  pCfgV4->cfg.dot3adSystem.actorSysPriority = FD_DOT3AD_DEFAULT_PRIORITY;

  defaultsBuilt = L7_TRUE;
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
dot3adBuildConfigDataV5 (dot3adCfgV5_t * pCfgV5)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (dot3adCfgV5_t));
  dot3adBuildConfigData (DOT3AD_CFG_VER_5);
  memcpy ((L7_uchar8 *) pCfgV5, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}

/*********************************************************************
* @purpose  Build default dot3ad data for a lag interface
*
* @param    intIfNum  @b{(input)}   internal intf of the lag
* @param    lagId     @b{(input)}   lagId of the lag
* @param    ver       @b{(input)}   Software version of Config Data
* @param    agg       @b{(output)}  The agg port pointer to be modified
* @param    idx       @b{(output)}  The agg idx pointer to be modified
*
* @returns  L7_SUCCESS
*
* @notes    Since the runtime and config structures are the same, this routine
*           can be used to set either structure to the defaults
*
* @end
*********************************************************************/
L7_RC_t dot3adLagIntfDefaultBuildV4(L7_uint32 intIfNum, L7_uint32 lagId, L7_uint32 ver,
                                  dot3ad_aggV4_t *agg, L7_uint32 *idx)
{
  agg->inuse = L7_FALSE;
  agg->ready = L7_FALSE;

  agg->aggId = intIfNum;

  agg->actorAdminAggKey = intIfNum;
  agg->actorOperAggKey = intIfNum;

  agg->individualAgg = L7_TRUE;
  agg->collectorMaxDelay = 0;
  agg->adminMode = FD_DOT3AD_ADMIN_MODE;

  /* add default value for static mode of Lag */
  agg->isStatic =  FD_DOT3AD_STATIC_MODE;

  /* setup internal interface <-> index mapping */
  *idx = lagId;

  return L7_SUCCESS;
}
