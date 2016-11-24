
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename   diffserv_migrate.c
*
* @purpose    DiffServ Configuration Migration
*
* @component  DiffServ
*
* @comments   none
*
* @create     08/24/2004
*
* @author     Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include "diffserv_migrate.h"
#include "usmdb_mib_diffserv_private_api.h"

static L7_RC_t diffServMigrateConfigV1V2Convert (L7_diffServCfgV1_t * pCfgV1,
                                                 L7_diffServCfgV2_t * pCfgV2);
static L7_RC_t diffServMigrateConfigV2V3Convert (L7_diffServCfgV2_t * pCfgV2,
                                                 L7_diffServCfgV3_t * pCfgV3);
static L7_RC_t diffServMigrateConfigV3V4Convert (L7_diffServCfgV3_t * pCfgV3, 
                                                 L7_diffServCfgV4_t * pCfgV4);
static L7_RC_t diffServMigrateConfigV4V5Convert (L7_diffServCfgV4_t * pCfgV4, 
                                                 L7_diffServCfgV5_t * pCfgV5);
static void diffServBuildDefaultConfigDataV2 (L7_diffServCfgV2_t * pCfgV2);
static void diffServBuildDefaultConfigDataV3 (L7_diffServCfgV3_t * pCfgV3);
static void diffServBuildDefaultConfigDataV4 (L7_diffServCfgV4_t * pCfgV4);
static void diffServBuildDefaultConfigDataV5 (L7_diffServCfgV5_t * pCfgV5);
static void diffServMigrateConfigV1V2ClassRulesMibConvert (L7_diffServClassRuleV1_t * pMibV1,
                                                           L7_diffServClassRuleV3_t * pMibV3);
static void diffServMigrateConfigV3V5ClassRulesMibConvert (L7_diffServClassRuleV3_t * pMibV3,
                                                           L7_diffServClassRule_t * pMibV5);
static void diffServMigrateConfigV1V3PolicyAttrConvert (L7_diffServPolicyAttrV1_t * pMibV1,
                                                        L7_diffServPolicyAttrV3_t * pMibV3);
static void diffServMigrateConfigV1V3PoliceActConvert (dsmibPoliceActV1_t * pActV1,
                                                       dsmibPoliceActV3_t * pActV3);
static void diffServMigrateConfigV3V4PolicyAttrConvert (L7_diffServPolicyAttrV3_t * pMibV3,
                                                        L7_diffServPolicyAttr_t * pMibV4);


static L7_diffServCfg_t *pCfgCur = L7_NULLPTR;

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
diffServMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize, *pOldCfgCksum, oldCfgVer;

  /* declare a pointer to each supported config structure version */
  L7_diffServCfgV1_t *pCfgV1 = L7_NULLPTR;
  L7_diffServCfgV2_t *pCfgV2 = L7_NULLPTR;
  L7_diffServCfgV3_t *pCfgV3 = L7_NULLPTR;
  L7_diffServCfgV4_t *pCfgV4 = L7_NULLPTR;
  L7_diffServCfgV5_t *pCfgV5 = L7_NULLPTR;


  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;


  ver = ver;
  pCfgCur = (L7_diffServCfg_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case L7_DIFFSERV_CFG_VER_1:
    pCfgV1 = (L7_diffServCfgV1_t *) osapiMalloc (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                                 (L7_uint32) sizeof (L7_diffServCfgV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (L7_diffServCfgV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = L7_DIFFSERV_CFG_VER_1;
    break;

  case L7_DIFFSERV_CFG_VER_2:
    pCfgV2 = (L7_diffServCfgV2_t *) osapiMalloc (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                                 (L7_uint32) sizeof (L7_diffServCfgV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (L7_diffServCfgV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = L7_DIFFSERV_CFG_VER_2;
    break;

  case L7_DIFFSERV_CFG_VER_3:
    pCfgV3 = (L7_diffServCfgV3_t *) osapiMalloc (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                                 (L7_uint32) sizeof (L7_diffServCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (L7_diffServCfgV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = L7_DIFFSERV_CFG_VER_3;
    break;

  case L7_DIFFSERV_CFG_VER_4:
    pCfgV4 = (L7_diffServCfgV4_t *) osapiMalloc (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                                 (L7_uint32) sizeof (L7_diffServCfgV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (L7_diffServCfgV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = L7_DIFFSERV_CFG_VER_4;
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
     *
     * The routine sysapiCfgFileGet which invokes the migrate routine will pass
     * in the a pointer to the location of the configuration data read from
     * nonvolatile storage.
     *
     * In cases where the older version of the configuration file may be larger
     * than the current version, sysapiCfgFileGet truncates the data to the size
     * of the storage location pointed to by the buffer provided for the component.
     * Thus, it is important to read the configuration file again so that the entirety
     * of the configuration data can be accessed.
     *
     */

    if ((L7_NULLPTR == pOldCfgBuf) ||
        (L7_SUCCESS != sysapiCfgFileGet (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                         L7_DIFFSERV_CFG_FILENAME, 
                                         (L7_char8 *)pOldCfgBuf,
                                         oldCfgSize, pOldCfgCksum, oldCfgVer,
                                         L7_NULLPTR,  /* do not build defaults */
                                         L7_NULLPTR   /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
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
   * Each version of the configuation is iteratively migrated to the
   * next version until brought up to the current level.
   */

  if (buildDefault != L7_TRUE)
  {
    switch (oldVer)
    {
    case L7_DIFFSERV_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (L7_diffServCfgV2_t *) osapiMalloc (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                                   (L7_uint32) sizeof (L7_diffServCfgV2_t));
      if ((L7_NULLPTR == pCfgV1) || 
          (L7_NULLPTR == pCfgV2) || 
          (L7_SUCCESS != diffServMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case L7_DIFFSERV_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (L7_diffServCfgV3_t *) osapiMalloc (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                                   (L7_uint32) sizeof (L7_diffServCfgV3_t));
      if ((L7_NULLPTR == pCfgV2) ||
          (L7_NULLPTR == pCfgV3) ||
          (L7_SUCCESS != diffServMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case L7_DIFFSERV_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (L7_diffServCfgV4_t *) osapiMalloc (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                                   (L7_uint32) sizeof (L7_diffServCfgV4_t));
      if ((L7_NULLPTR == pCfgV3) ||
          (L7_NULLPTR == pCfgV4) ||
          (L7_SUCCESS != diffServMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case L7_DIFFSERV_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (L7_diffServCfgV5_t *) osapiMalloc (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                                   (L7_uint32) sizeof (L7_diffServCfgV5_t));
      if ((L7_NULLPTR == pCfgV4) ||
          (L7_NULLPTR == pCfgV5) ||
          (L7_SUCCESS != diffServMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case L7_DIFFSERV_CFG_VER_5:
      /* pCfgV5 has already been allocated and filled in */
      if (L7_NULLPTR == pCfgV5)
      {
        buildDefault = L7_TRUE;
        break;
      }
      memcpy ((L7_char8 *) pCfgCur, (L7_char8 *) pCfgV5, sizeof (*pCfgCur));
      osapiFree (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pCfgV5);
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
  if (pCfgV1 != L7_NULLPTR)
    osapiFree (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pCfgV4);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    diffServBuildDefaultConfigData (L7_DIFFSERV_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }
  else
  {
    /* display the test config data that was migrated, since it
     * is not likely to be applied successfully (this value for
     * the message level was stored into the test config data)
     */
    if (pCfgCur->msgLvl == (L7_uint32)DIFFSERV_MSGLVL_OFF)
    {
      MIGRATE_DEBUG("Displaying migrated test config data\n");
      diffServConfigDataTestShow();
    }
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
diffServMigrateConfigV1V2Convert (L7_diffServCfgV1_t * pCfgV1, L7_diffServCfgV2_t * pCfgV2)
{
  L7_RC_t rc;
  L7_uint32 i, j, count;
  L7_INTF_TYPES_t intfType;
  L7_uint32 intfCfgIndex;
  nimConfigID_t configId;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != L7_DIFFSERV_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, L7_DIFFSERV_CFG_VER_1);
    return L7_FAILURE;
  }

  diffServBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV2->adminMode = pCfgV1->adminMode;
  pCfgV2->traceMode = pCfgV1->traceMode;
  pCfgV2->msgLvl = pCfgV1->msgLvl;

  count = (L7_uint32) min (L7_DIFFSERV_CLASS_HDR_MAX_REL_4_1, L7_DIFFSERV_CLASS_HDR_MAX_REL_4_0);
  for (i = 0; i < count; i++)
  {

    MIGRATE_COPY_STRUCT (pCfgV2->classHdr[i], pCfgV1->classHdr[i]);  

  }

  count = (L7_uint32) min (L7_DIFFSERV_CLASS_RULE_MAX_REL_4_1, L7_DIFFSERV_CLASS_RULE_MAX_REL_4_0);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV2->classRule[i], pCfgV1->classRule[i]);
  }

  count = (L7_uint32) min (L7_DIFFSERV_POLICY_HDR_MAX_REL_4_1, L7_DIFFSERV_POLICY_HDR_MAX_REL_4_0);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV2->policyHdr[i], pCfgV1->policyHdr[i]);
  }

  count = (L7_uint32) min (L7_DIFFSERV_POLICY_INST_MAX_REL_4_1, L7_DIFFSERV_POLICY_INST_MAX_REL_4_0);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV2->policyInst[i], pCfgV1->policyInst[i]);
  }

  count = (L7_uint32) min (L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_1, L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_0);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV2->policyAttr[i], pCfgV1->policyAttr[i]);
  }

  intfCfgIndex = 1;
  intfType = 0;
  for (i = 0; i < L7_MAX_SLOTS_PER_BOX_REL_4_0; i++)
  {
    for (j = 1; j <= L7_MAX_PORTS_PER_SLOT_REL_4_0; j++)
    {
      rc = sysapiCfgFileRel4_0SlotPortToIntfInfoGet (i, j, &configId, L7_NULL, &intfType);
      if (L7_SUCCESS != rc)
      {
        if (L7_ERROR == rc)
          MIGRATE_INVALID_SLOT_PORT (i, j);
        continue;
      }

      /* Determine if the interface is valid for participation in this feature */
      if (diffServIsValidIntfType (intfType) != L7_TRUE)
        continue;

      if (intfCfgIndex >= L7_DIFFSERV_INTF_MAX_COUNT_REL_4_1)
      {
        MIGRATE_LOG_INTERFACE_TRUNCATE ("diffserv.cfg", intfCfgIndex);
        break;
      }

      (void) nimConfigIdCopy (&configId, &pCfgV2->diffServIntfCfg[intfCfgIndex].configId);
      MIGRATE_COPY_STRUCT (pCfgV2->diffServIntfCfg[intfCfgIndex].serviceIn,
                           pCfgV1->serviceIn[i][j]);
      MIGRATE_COPY_STRUCT (pCfgV2->diffServIntfCfg[intfCfgIndex].serviceOut,
                           pCfgV1->serviceOut[i][j]);
      intfCfgIndex++;
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
* @notes    This migration function converts all the information that
*           it finds, but lets the config 'apply' processing worry about
*           ignoring older config data that is no longer supported (e.g.
*           outbound policies).
*
* @end
*********************************************************************/
static L7_RC_t
diffServMigrateConfigV2V3Convert (L7_diffServCfgV2_t * pCfgV2, L7_diffServCfgV3_t * pCfgV3)
{
  L7_uint32 i, count;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != L7_DIFFSERV_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, L7_DIFFSERV_CFG_VER_2);
    return L7_FAILURE;
  }

  diffServBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV3->adminMode = pCfgV2->adminMode;
  pCfgV3->traceMode = pCfgV2->traceMode;
  pCfgV3->msgLvl = pCfgV2->msgLvl;

  for (i = 0; i < L7_DIFFSERV_CLASS_HDR_MAX_REL_4_3; i++)
  {
    pCfgV3->classHdr[i].inUse = pCfgV2->classHdr[i].inUse;
    pCfgV3->classHdr[i].chainIndex = pCfgV2->classHdr[i].chainIndex;
    pCfgV3->classHdr[i].mib.index = pCfgV2->classHdr[i].mib.index;
    memcpy ((char *)pCfgV3->classHdr[i].mib.name, 
            (char *)pCfgV2->classHdr[i].mib.name,
            (size_t)(L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX +1));
    pCfgV3->classHdr[i].mib.type = pCfgV2->classHdr[i].mib.type;
    pCfgV3->classHdr[i].mib.aclNum = pCfgV2->classHdr[i].mib.aclNum;
    if (pCfgV3->classHdr[i].mib.aclNum != 0)
      pCfgV3->classHdr[i].mib.aclType = L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_IP;
    pCfgV3->classHdr[i].mib.ruleIndexNextFree = pCfgV2->classHdr[i].mib.ruleIndexNextFree;
    pCfgV3->classHdr[i].mib.storageType = pCfgV2->classHdr[i].mib.storageType;
    pCfgV3->classHdr[i].mib.rowStatus = pCfgV2->classHdr[i].mib.rowStatus;


    /* --------------------------- */
    /* Handle flags                */
    /* --------------------------- */

    if (DSMIB_ROW_INVALID_FLAG_IS_ON(&pCfgV2->classHdr[i], DSMIB_CLASS_FLAGS_NAME_V1))
    {
      DSMIB_ROW_INVALID_FLAG_SET(&pCfgV3->classHdr[i], DSMIB_CLASS_FLAGS_NAME_V2);
    }
    if (DSMIB_ROW_INVALID_FLAG_IS_ON(&pCfgV2->classHdr[i], DSMIB_CLASS_FLAGS_TYPE_V1))
    {
      DSMIB_ROW_INVALID_FLAG_SET(&pCfgV3->classHdr[i], DSMIB_CLASS_FLAGS_TYPE_V2);
    }
    if (DSMIB_ROW_INVALID_FLAG_IS_ON(&pCfgV2->classHdr[i], DSMIB_CLASS_FLAGS_ACLNUM_V1))
    {
      DSMIB_ROW_INVALID_FLAG_SET(&pCfgV3->classHdr[i], DSMIB_CLASS_FLAGS_ACLTYPE_V2);
      DSMIB_ROW_INVALID_FLAG_SET(&pCfgV3->classHdr[i], DSMIB_CLASS_FLAGS_ACLNUM_V2);
    }
    if (DSMIB_ROW_INVALID_FLAG_IS_ON(&pCfgV2->classHdr[i], ~DSMIB_CLASS_FLAGS_COMMON_V1))
    {
      LOG_MSG ("Unrecognized rowInvalidFlags 0x%8.8x in class id %u\n", 
               (L7_uint32)(pCfgV2->classHdr[i].rowInvalidFlags & (~DSMIB_CLASS_FLAGS_COMMON_V1)), 
               i);
    }
  }

  count = (L7_uint32) min (L7_DIFFSERV_CLASS_RULE_MAX_REL_4_3, L7_DIFFSERV_CLASS_RULE_MAX_REL_4_1);
  for (i = 0; i < count; i++)
  {
    diffServMigrateConfigV1V2ClassRulesMibConvert (&pCfgV2->classRule[i],
                                                   &pCfgV3->classRule[i]);
  }

  MIGRATE_COPY_STRUCT (pCfgV3->policyHdr, pCfgV2->policyHdr);
  MIGRATE_COPY_STRUCT (pCfgV3->policyInst, pCfgV2->policyInst);

  count = (L7_uint32) min (L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_3, L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_1);
  for (i = 0; i < count; i++)
  {
    diffServMigrateConfigV1V3PolicyAttrConvert (&pCfgV2->policyAttr[i],
                                                &pCfgV3->policyAttr[i]);
  }

  count = (L7_uint32) min (L7_DIFFSERV_INTF_MAX_COUNT_REL_4_3, L7_DIFFSERV_INTF_MAX_COUNT_REL_4_1);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV3->diffServIntfCfg[i], pCfgV2->diffServIntfCfg[i]);
  }

  return L7_SUCCESS;
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
* @notes    This migration function converts all the information that
*           it finds, but lets the config 'apply' processing worry about
*           ignoring older config data that is no longer supported (e.g.
*           outbound policies).
*
* @end
*********************************************************************/
static L7_RC_t
diffServMigrateConfigV3V4Convert (L7_diffServCfgV3_t * pCfgV3, L7_diffServCfgV4_t * pCfgV4)
{
  L7_uint32 i, count;

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != L7_DIFFSERV_CFG_VER_3)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, L7_DIFFSERV_CFG_VER_3);
    return L7_FAILURE;
  }

  diffServBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV4->adminMode = pCfgV3->adminMode;
  pCfgV4->traceMode = pCfgV3->traceMode;
  pCfgV4->msgLvl    = pCfgV3->msgLvl;

  count = (L7_uint32) min (L7_DIFFSERV_CLASS_HDR_MAX_REL_4_4, L7_DIFFSERV_CLASS_HDR_MAX_REL_4_3);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->classHdr[i], pCfgV3->classHdr[i]);
  }

  count = (L7_uint32) min (L7_DIFFSERV_CLASS_RULE_MAX_REL_4_4, L7_DIFFSERV_CLASS_RULE_MAX_REL_4_3);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->classRule[i], pCfgV3->classRule[i]);
  }

  count = (L7_uint32) min (L7_DIFFSERV_POLICY_HDR_MAX_REL_4_4, L7_DIFFSERV_POLICY_HDR_MAX_REL_4_3);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->policyHdr[i], pCfgV3->policyHdr[i]);
  }

  count = (L7_uint32) min (L7_DIFFSERV_POLICY_INST_MAX_REL_4_4, L7_DIFFSERV_POLICY_INST_MAX_REL_4_3);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->policyInst[i], pCfgV3->policyInst[i]);
  }

  count = (L7_uint32) min (L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_4, L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_3);
  for (i = 0; i < count; i++)
  {
    diffServMigrateConfigV3V4PolicyAttrConvert (&pCfgV3->policyAttr[i],
                                                &pCfgV4->policyAttr[i]);
  }

  count = (L7_uint32) min (L7_DIFFSERV_INTF_MAX_COUNT_REL_4_4, L7_DIFFSERV_INTF_MAX_COUNT_REL_4_3);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV4->diffServIntfCfg[i], pCfgV3->diffServIntfCfg[i]);
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
* @notes    This migration function converts all the information that
*           it finds, but lets the config 'apply' processing worry about
*           ignoring older config data that is no longer supported (e.g.
*           outbound policies).
*
* @end
*********************************************************************/
static L7_RC_t
diffServMigrateConfigV4V5Convert (L7_diffServCfgV4_t * pCfgV4, L7_diffServCfgV5_t * pCfgV5)
{
  L7_uint32 i, count;

  /* verify correct version of old config file */
  if (pCfgV4->cfgHdr.version != L7_DIFFSERV_CFG_VER_4)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, L7_DIFFSERV_CFG_VER_4);
    return L7_FAILURE;
  }

  diffServBuildDefaultConfigDataV5 (pCfgV5);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV5->adminMode = pCfgV4->adminMode;
  pCfgV5->traceMode = pCfgV4->traceMode;
  pCfgV5->msgLvl    = pCfgV4->msgLvl;

  count = (L7_uint32) min (L7_DIFFSERV_CLASS_HDR_MAX_REL_4_5, L7_DIFFSERV_CLASS_HDR_MAX_REL_4_4);
  for (i = 0; i < count; i++)
  {
    pCfgV5->classHdr[i].inUse = pCfgV4->classHdr[i].inUse;
    pCfgV5->classHdr[i].chainIndex = pCfgV4->classHdr[i].chainIndex;
    pCfgV5->classHdr[i].mib.index = pCfgV4->classHdr[i].mib.index;
    memcpy ((char *)pCfgV5->classHdr[i].mib.name, 
            (char *)pCfgV4->classHdr[i].mib.name,
            (size_t)(L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX +1));
    pCfgV5->classHdr[i].mib.type = pCfgV4->classHdr[i].mib.type;
    pCfgV5->classHdr[i].mib.l3Protocol = L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4;
    pCfgV5->classHdr[i].mib.aclNum = pCfgV4->classHdr[i].mib.aclNum;
    pCfgV5->classHdr[i].mib.aclType = pCfgV4->classHdr[i].mib.aclType;
    pCfgV5->classHdr[i].mib.ruleIndexNextFree = pCfgV4->classHdr[i].mib.ruleIndexNextFree;
    pCfgV5->classHdr[i].mib.storageType = pCfgV4->classHdr[i].mib.storageType;
    pCfgV5->classHdr[i].mib.rowStatus = pCfgV4->classHdr[i].mib.rowStatus;


    /* --------------------------- */
    /* Handle flags                */
    /* --------------------------- */

    if (DSMIB_ROW_INVALID_FLAG_IS_ON(&pCfgV4->classHdr[i], DSMIB_CLASS_FLAGS_NAME_V3))
    {
      DSMIB_ROW_INVALID_FLAG_SET(&pCfgV5->classHdr[i], DSMIB_CLASS_FLAGS_NAME);
    }
    if (DSMIB_ROW_INVALID_FLAG_IS_ON(&pCfgV4->classHdr[i], DSMIB_CLASS_FLAGS_TYPE_V3))
    {
      DSMIB_ROW_INVALID_FLAG_SET(&pCfgV5->classHdr[i], DSMIB_CLASS_FLAGS_TYPE);
    }
    if (DSMIB_ROW_INVALID_FLAG_IS_ON(&pCfgV4->classHdr[i], DSMIB_CLASS_FLAGS_ACLNUM_V3))
    {
      DSMIB_ROW_INVALID_FLAG_SET(&pCfgV5->classHdr[i], DSMIB_CLASS_FLAGS_ACLNUM);
    }
    if (DSMIB_ROW_INVALID_FLAG_IS_ON(&pCfgV4->classHdr[i], DSMIB_CLASS_FLAGS_ACLTYPE_V3))
    {
      DSMIB_ROW_INVALID_FLAG_SET(&pCfgV5->classHdr[i], DSMIB_CLASS_FLAGS_ACLTYPE);
    }

    /* the mib.l3Protocol member was set above to a default, so clear the invalid flag for the new item */
    DSMIB_ROW_INVALID_FLAG_CLR(&pCfgV5->classHdr[i], DSMIB_CLASS_FLAGS_L3PROTO);

    if (DSMIB_ROW_INVALID_FLAG_IS_ON(&pCfgV4->classHdr[i], ~DSMIB_CLASS_FLAGS_COMMON_V3))
    {
      LOG_MSG ("Unrecognized rowInvalidFlags 0x%8.8x in class id %u\n", 
               (L7_uint32)(pCfgV4->classHdr[i].rowInvalidFlags & (~DSMIB_CLASS_FLAGS_COMMON_V3)), 
               i);
    }
  }

  count = (L7_uint32) min (L7_DIFFSERV_CLASS_RULE_MAX_REL_4_5, L7_DIFFSERV_CLASS_RULE_MAX_REL_4_4);
  for (i = 0; i < count; i++)
  {
    diffServMigrateConfigV3V5ClassRulesMibConvert (&pCfgV4->classRule[i],
                                                   &pCfgV5->classRule[i]);
  }

  count = (L7_uint32) min (L7_DIFFSERV_POLICY_HDR_MAX_REL_4_5, L7_DIFFSERV_POLICY_HDR_MAX_REL_4_4);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV5->policyHdr[i], pCfgV4->policyHdr[i]);
  }

  count = (L7_uint32) min (L7_DIFFSERV_POLICY_INST_MAX_REL_4_5, L7_DIFFSERV_POLICY_INST_MAX_REL_4_4);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV5->policyInst[i], pCfgV4->policyInst[i]);
  }

  count = (L7_uint32) min (L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_5, L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_4);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV5->policyAttr[i], pCfgV4->policyAttr[i]);
  }

  count = (L7_uint32) min (L7_DIFFSERV_INTF_MAX_COUNT_REL_4_5, L7_DIFFSERV_INTF_MAX_COUNT_REL_4_4);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV5->diffServIntfCfg[i], pCfgV4->diffServIntfCfg[i]);
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
diffServBuildDefaultConfigDataV2 (L7_diffServCfgV2_t * pCfgV2)
{
  /* build config file header */
  strcpy ((char *) pCfgV2->cfgHdr.filename, L7_DIFFSERV_CFG_FILENAME);
  pCfgV2->cfgHdr.version = L7_DIFFSERV_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = (L7_uint32) sizeof (*pCfgV2);
  pCfgV2->cfgHdr.dataChanged = L7_FALSE;

  /* build default config data */
  pCfgV2->adminMode = FD_QOS_DIFFSERV_ADMIN_MODE;
  pCfgV2->traceMode = FD_QOS_DIFFSERV_TRACE_MODE;
  pCfgV2->msgLvl = FD_QOS_DIFFSERV_MSG_LVL;
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
diffServBuildDefaultConfigDataV3 (L7_diffServCfgV3_t * pCfgV3)
{
  /* build config file header */
  strcpy ((char *) pCfgV3->cfgHdr.filename, L7_DIFFSERV_CFG_FILENAME);
  pCfgV3->cfgHdr.version = L7_DIFFSERV_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
  pCfgV3->cfgHdr.type = L7_CFG_DATA;
  pCfgV3->cfgHdr.length = (L7_uint32) sizeof (*pCfgV3);
  pCfgV3->cfgHdr.dataChanged = L7_FALSE;

  /* build default config data */
  pCfgV3->adminMode = FD_QOS_DIFFSERV_ADMIN_MODE;
  pCfgV3->traceMode = FD_QOS_DIFFSERV_TRACE_MODE;
  pCfgV3->msgLvl = FD_QOS_DIFFSERV_MSG_LVL;
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
diffServBuildDefaultConfigDataV4 (L7_diffServCfgV4_t * pCfgV4)
{
  memset ((L7_char8 *) pCfgCur, 0, sizeof (*pCfgCur));
  diffServBuildDefaultConfigData (L7_DIFFSERV_CFG_VER_4);
  memcpy ((L7_char8 *) pCfgV4, (L7_char8 *) pCfgCur, sizeof (*pCfgV4));
}

/*********************************************************************
* @purpose  Build Version 5 defaults
*
* @param    pCfgV4    @b{(input)} ptr to version 4 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
diffServBuildDefaultConfigDataV5 (L7_diffServCfgV5_t * pCfgV5)
{
  memset ((L7_char8 *) pCfgCur, 0, sizeof (*pCfgCur));
  diffServBuildDefaultConfigData (L7_DIFFSERV_CFG_VER_5);
  memcpy ((L7_char8 *) pCfgV5, (L7_char8 *) pCfgCur, sizeof (*pCfgV5));
}

/*********************************************************************
* @purpose  Converts the mib data structure from the original version V1 
*           version introduced in release 4.3.
*
* @param    pCfgV1    @b{(input)} ptr to version 1 mib data structure
* @param    pCfgV2    @b{(input)} ptr to version 2 mib data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Version V3 of the L7_diffServClassRule_t was introduced in
*           release 4.3.
*
*           Maintenance note:  If L7_diffServClassRule_t changes on a
*           subsequent release, create L7_diffServClassRuleV@ and modify
*           this function's prototype accordingly. (This has been done
*           for release 4.5.)
*
* @end
*********************************************************************/
static void
diffServMigrateConfigV1V2ClassRulesMibConvert (L7_diffServClassRuleV1_t * pMibV1,   
                                               L7_diffServClassRuleV3_t * pMibV2)
{
  pMibV2->hdrIndexRef = pMibV1->hdrIndexRef;
  pMibV2->chainIndex  = pMibV1->chainIndex;
  pMibV2->arid        = pMibV1->arid;

  
  pMibV2->mib.excludeFlag   = pMibV1->mib.excludeFlag;    
  pMibV2->mib.storageType   = pMibV1->mib.storageType;    
  pMibV2->mib.index         = pMibV1->mib.index;       
  pMibV2->mib.rowStatus     = pMibV1->mib.rowStatus;    

  /* --------------------------- */
  /* Handle entry types          */
  /* --------------------------- */

  switch (pMibV1->mib.entryType)
  {
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE_V1:
    pMibV2->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE_V3;
    /* no MIB data to copy for this case */
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS_V1:
    pMibV2->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS_V3;
    pMibV2->mib.match.cos = pMibV1->mib.match.cos;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP_V1:
    pMibV2->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP_V3;
    MIGRATE_COPY_STRUCT (pMibV2->mib.match.dstIp, pMibV1->mib.match.dstIp);
    break;
  
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT_V1:
    pMibV2->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT_V3;
    MIGRATE_COPY_STRUCT (pMibV2->mib.match.dstL4Port, pMibV1->mib.match.dstL4Port);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC_V1:
    pMibV2->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC_V3;
    MIGRATE_COPY_STRUCT (pMibV2->mib.match.dstMac, pMibV1->mib.match.dstMac);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY_V1:
    pMibV2->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY_V3;
    MIGRATE_COPY_STRUCT (pMibV2->mib.match.matchEvery, pMibV1->mib.match.matchEvery);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP_V1:
    pMibV2->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP_V3;
    MIGRATE_COPY_STRUCT (pMibV2->mib.match.ipDscp, pMibV1->mib.match.ipDscp);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE_V1:
    pMibV2->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE_V3;
    MIGRATE_COPY_STRUCT (pMibV2->mib.match.ipPrecedence, pMibV1->mib.match.ipPrecedence);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS_V1:
    pMibV2->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS_V3;
    MIGRATE_COPY_STRUCT (pMibV2->mib.match.ipTos, pMibV1->mib.match.ipTos);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL_V1:
    pMibV2->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL_V3;
    MIGRATE_COPY_STRUCT (pMibV2->mib.match.protocolNum, pMibV1->mib.match.protocolNum);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS_V1:
    pMibV2->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS_V3;
    MIGRATE_COPY_STRUCT (pMibV2->mib.match.refClassIndex, pMibV1->mib.match.refClassIndex);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP_V1:
    pMibV2->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP_V3;
    MIGRATE_COPY_STRUCT (pMibV2->mib.match.srcIp, pMibV1->mib.match.srcIp);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT_V1:
    pMibV2->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT_V3;
    MIGRATE_COPY_STRUCT (pMibV2->mib.match.srcL4Port, pMibV1->mib.match.srcL4Port);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC_V1:
    pMibV2->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC_V3;
    MIGRATE_COPY_STRUCT (pMibV2->mib.match.srcMac, pMibV1->mib.match.srcMac);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLAN_V1:
    pMibV2->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID_V3;
    pMibV2->mib.match.vlanId.start = pMibV1->mib.match.vlanId;
    pMibV2->mib.match.vlanId.end = pMibV1->mib.match.vlanId;
    break; 

  default:
    /* unrecognized entry type */
    LOG_MSG ("Unrecognized class rule entry type %d\n", (L7_int32) pMibV1->mib.entryType);
    break;                     
  }


  /* --------------------------- */
  /* Handle flags                */
  /* --------------------------- */

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_COS_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_COS_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_PORTSTART_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_PORTEND_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_IPDSCP_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_IPDSCP_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_IPPRECEDENCE_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_IPPRECEDENCE_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSBITS_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSBITS_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSMASK_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSMASK_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_PROTOCOLNUM_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_PROTOCOLNUM_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_REFCLASSINDEX_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_REFCLASSINDEX_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_VLANID_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART_V3);
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV2, DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, ~DSMIB_CLASS_RULE_FLAGS_ALL_V1))
  {
    LOG_MSG ("Unrecognized rowInvalidFlags 0x%8.8x in class rule id (%u,%u)\n", 
             (L7_uint32)(pMibV1->rowInvalidFlags & (~DSMIB_CLASS_RULE_FLAGS_ALL_V1)), 
             pMibV1->hdrIndexRef, pMibV1->mib.index);
  }

}

/*********************************************************************
* @purpose  Converts the mib data structure from version V3
*           version introduced in release 4.5.
*
* @param    pCfgV3    @b{(input)} ptr to version 3 diffServ config structure
* @param    pCfgV5    @b{(input)} ptr to version 5 diffServ config structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Version V5 of the L7_diffServClassRule_t was introduced in
*           release 4.5.
*
*           Maintenance note:  If L7_diffServClassRule_t changes on a
*           subsequent release, create L7_diffServClassRuleV@ and modify
*           this function's prototype accordingly. 
*
* @end
*********************************************************************/
static void
diffServMigrateConfigV3V5ClassRulesMibConvert (L7_diffServClassRuleV3_t * pMibV3,   
                                               L7_diffServClassRule_t   * pMibV5)
{
  pMibV5->hdrIndexRef = pMibV3->hdrIndexRef;
  pMibV5->chainIndex  = pMibV3->chainIndex;
  pMibV5->arid        = pMibV3->arid;

  
  pMibV5->mib.excludeFlag   = pMibV3->mib.excludeFlag;    
  pMibV5->mib.storageType   = pMibV3->mib.storageType;    
  pMibV5->mib.index         = pMibV3->mib.index;       
  pMibV5->mib.rowStatus     = pMibV3->mib.rowStatus;    

  /* --------------------------- */
  /* Handle entry types          */
  /* --------------------------- */

  switch (pMibV3->mib.entryType)
  {
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
    /* no MIB data to copy for this case */
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS;
    pMibV5->mib.match.cos = pMibV3->mib.match.cos;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2;
    pMibV5->mib.match.cos2 = pMibV3->mib.match.cos2;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP;
    MIGRATE_COPY_STRUCT (pMibV5->mib.match.dstIp, pMibV3->mib.match.dstIp);
    break;
  
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT;
    MIGRATE_COPY_STRUCT (pMibV5->mib.match.dstL4Port, pMibV3->mib.match.dstL4Port);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC;
    MIGRATE_COPY_STRUCT (pMibV5->mib.match.dstMac, pMibV3->mib.match.dstMac);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE;
    MIGRATE_COPY_STRUCT (pMibV5->mib.match.etype, pMibV3->mib.match.etype);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY;
    MIGRATE_COPY_STRUCT (pMibV5->mib.match.matchEvery, pMibV3->mib.match.matchEvery);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP;
    MIGRATE_COPY_STRUCT (pMibV5->mib.match.ipDscp, pMibV3->mib.match.ipDscp);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE;
    MIGRATE_COPY_STRUCT (pMibV5->mib.match.ipPrecedence, pMibV3->mib.match.ipPrecedence);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS;
    MIGRATE_COPY_STRUCT (pMibV5->mib.match.ipTos, pMibV3->mib.match.ipTos);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL;
    MIGRATE_COPY_STRUCT (pMibV5->mib.match.protocolNum, pMibV3->mib.match.protocolNum);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS;
    MIGRATE_COPY_STRUCT (pMibV5->mib.match.refClassIndex, pMibV3->mib.match.refClassIndex);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP;
    MIGRATE_COPY_STRUCT (pMibV5->mib.match.srcIp, pMibV3->mib.match.srcIp);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT;
    MIGRATE_COPY_STRUCT (pMibV5->mib.match.srcL4Port, pMibV3->mib.match.srcL4Port);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC;
    MIGRATE_COPY_STRUCT (pMibV5->mib.match.srcMac, pMibV3->mib.match.srcMac);
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID;
    MIGRATE_COPY_STRUCT (pMibV5->mib.match.vlanId, pMibV3->mib.match.vlanId);
    break; 

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2_V3:
    pMibV5->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2;
    MIGRATE_COPY_STRUCT (pMibV5->mib.match.vlanId2, pMibV3->mib.match.vlanId2);
    break; 

  default:
    /* unrecognized entry type */
    LOG_MSG ("Unrecognized class rule entry type %d\n", (L7_int32) pMibV3->mib.entryType);
    break;                     
  }


  /* --------------------------- */
  /* Handle flags                */
  /* --------------------------- */

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_COS_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_COS);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_KEYID_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_KEYID);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_VALUE_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_VALUE);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_IPDSCP_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_IPDSCP);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_IPPRECEDENCE_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_IPPRECEDENCE);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSBITS_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSBITS);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSMASK_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSMASK);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_PROTOCOLNUM_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_PROTOCOLNUM);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_REFCLASSINDEX_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_REFCLASSINDEX);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV5, DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, ~DSMIB_CLASS_RULE_FLAGS_ALL_V3))
  {
    LOG_MSG ("Unrecognized rowInvalidFlags 0x%8.8x in class rule id (%u,%u)\n", 
             (L7_uint32)(pMibV3->rowInvalidFlags & (~DSMIB_CLASS_RULE_FLAGS_ALL_V3)), 
             pMibV3->hdrIndexRef, pMibV3->mib.index);
  }

}

/*********************************************************************
* @purpose  Converts policy attr mib data structure from version V1 to V3.
*
* @param    pCfgV1    @b{(input)} ptr to version 1 mib data structure
* @param    pCfgV3    @b{(input)} ptr to version 3 mib data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
static void
diffServMigrateConfigV1V3PolicyAttrConvert (L7_diffServPolicyAttrV1_t * pMibV1,
                                            L7_diffServPolicyAttrV3_t * pMibV3)
{

  pMibV3->instIndexRef = pMibV1->instIndexRef;
  pMibV3->chainIndex = pMibV1->chainIndex;


  pMibV3->mib.index = pMibV1->mib.index;
  pMibV3->mib.storageType = pMibV1->mib.storageType;
  pMibV3->mib.rowStatus = pMibV1->mib.rowStatus;


  /* --------------------------- */
  /* Handle entry types          */
  /* --------------------------- */


  switch (pMibV1->mib.entryType)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE_V1:
    pMibV3->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE_V3;
    /* no MIB data to copy for this case */
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_BANDWIDTH_V1:
    /* this outbound policy attribute is obsolete in V2 config */
    pMibV3->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_BANDWIDTH_V3;
    MIGRATE_COPY_STRUCT (pMibV3->mib.stmt.bandwidth, pMibV1->mib.stmt.bandwidth);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_EXPEDITE_V1:
    /* this outbound policy attribute is obsolete in V2 config */
    pMibV3->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_EXPEDITE_V3;
    MIGRATE_COPY_STRUCT (pMibV3->mib.stmt.expedite, pMibV1->mib.stmt.expedite);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL_V1:
    /* this outbound policy attribute is obsolete in V2 config */
    pMibV3->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL_V3;
    pMibV3->mib.stmt.markCos.val = (L7_uchar8)pMibV1->mib.stmt.markCos.val;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL_V1:
    pMibV3->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL_V3;
    MIGRATE_COPY_STRUCT (pMibV3->mib.stmt.markIpDscp, pMibV1->mib.stmt.markIpDscp);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL_V1:
    pMibV3->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL_V3;
    MIGRATE_COPY_STRUCT (pMibV3->mib.stmt.markIpPrecedence, pMibV1->mib.stmt.markIpPrecedence);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE_V1:
    /* color mode fields were introduced in V2 config */
    pMibV3->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE_V3;
    pMibV3->mib.stmt.policeSimple.crate = pMibV1->mib.stmt.policeSimple.crate;
    pMibV3->mib.stmt.policeSimple.cburst = pMibV1->mib.stmt.policeSimple.cburst;
    diffServMigrateConfigV1V3PoliceActConvert(&pMibV1->mib.stmt.policeSimple.conformAct,
                                              &pMibV3->mib.stmt.policeSimple.conformAct);
    diffServMigrateConfigV1V3PoliceActConvert(&pMibV1->mib.stmt.policeSimple.nonconformAct,
                                              &pMibV3->mib.stmt.policeSimple.nonconformAct);
    pMibV3->mib.stmt.policeSimple.conformVal = pMibV1->mib.stmt.policeSimple.conformVal;
    pMibV3->mib.stmt.policeSimple.nonconformVal = pMibV1->mib.stmt.policeSimple.nonconformVal;
    pMibV3->mib.stmt.policeSimple.colorConformIndex = 0;
    pMibV3->mib.stmt.policeSimple.colorConformMode = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND_V3;
    pMibV3->mib.stmt.policeSimple.colorConformVal = 0;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE_V1:
    /* color mode fields were introduced in V2 config */
    pMibV3->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE_V3;
    pMibV3->mib.stmt.policeSingleRate.crate = pMibV1->mib.stmt.policeSingleRate.crate;
    pMibV3->mib.stmt.policeSingleRate.cburst = pMibV1->mib.stmt.policeSingleRate.cburst;
    pMibV3->mib.stmt.policeSingleRate.eburst = pMibV1->mib.stmt.policeSingleRate.eburst;
    diffServMigrateConfigV1V3PoliceActConvert(&pMibV1->mib.stmt.policeSingleRate.conformAct,
                                              &pMibV3->mib.stmt.policeSingleRate.conformAct);
    diffServMigrateConfigV1V3PoliceActConvert(&pMibV1->mib.stmt.policeSingleRate.exceedAct,
                                              &pMibV3->mib.stmt.policeSingleRate.exceedAct);
    diffServMigrateConfigV1V3PoliceActConvert(&pMibV1->mib.stmt.policeSingleRate.nonconformAct,
                                              &pMibV3->mib.stmt.policeSingleRate.nonconformAct);
    pMibV3->mib.stmt.policeSingleRate.conformVal = pMibV1->mib.stmt.policeSingleRate.conformVal;
    pMibV3->mib.stmt.policeSingleRate.exceedVal = pMibV1->mib.stmt.policeSingleRate.exceedVal;
    pMibV3->mib.stmt.policeSingleRate.nonconformVal = pMibV1->mib.stmt.policeSingleRate.nonconformVal;
    pMibV3->mib.stmt.policeSingleRate.colorConformIndex = 0;
    pMibV3->mib.stmt.policeSingleRate.colorConformMode = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND_V3;
    pMibV3->mib.stmt.policeSingleRate.colorConformVal = 0;
    pMibV3->mib.stmt.policeSingleRate.colorExceedIndex = 0;
    pMibV3->mib.stmt.policeSingleRate.colorExceedMode = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND_V3;
    pMibV3->mib.stmt.policeSingleRate.colorExceedVal = 0;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE_V1:
    /* color mode fields were introduced in V2 config */
    pMibV3->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE_V3;
    pMibV3->mib.stmt.policeTwoRate.crate = pMibV1->mib.stmt.policeTwoRate.crate;
    pMibV3->mib.stmt.policeTwoRate.cburst = pMibV1->mib.stmt.policeTwoRate.cburst;
    pMibV3->mib.stmt.policeTwoRate.prate = pMibV1->mib.stmt.policeTwoRate.prate;
    pMibV3->mib.stmt.policeTwoRate.pburst = pMibV1->mib.stmt.policeTwoRate.pburst;
    diffServMigrateConfigV1V3PoliceActConvert(&pMibV1->mib.stmt.policeTwoRate.conformAct,
                                              &pMibV3->mib.stmt.policeTwoRate.conformAct);
    diffServMigrateConfigV1V3PoliceActConvert(&pMibV1->mib.stmt.policeTwoRate.exceedAct,
                                              &pMibV3->mib.stmt.policeTwoRate.exceedAct);
    diffServMigrateConfigV1V3PoliceActConvert(&pMibV1->mib.stmt.policeTwoRate.nonconformAct,
                                              &pMibV3->mib.stmt.policeTwoRate.nonconformAct);
    pMibV3->mib.stmt.policeTwoRate.conformVal = pMibV1->mib.stmt.policeTwoRate.conformVal;
    pMibV3->mib.stmt.policeTwoRate.exceedVal = pMibV1->mib.stmt.policeTwoRate.exceedVal;
    pMibV3->mib.stmt.policeTwoRate.nonconformVal = pMibV1->mib.stmt.policeTwoRate.nonconformVal;
    pMibV3->mib.stmt.policeTwoRate.colorConformIndex = 0;
    pMibV3->mib.stmt.policeTwoRate.colorConformMode = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND_V3;
    pMibV3->mib.stmt.policeTwoRate.colorConformVal = 0;
    pMibV3->mib.stmt.policeTwoRate.colorExceedIndex = 0;
    pMibV3->mib.stmt.policeTwoRate.colorExceedMode = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND_V3;
    pMibV3->mib.stmt.policeTwoRate.colorExceedVal = 0;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_RANDOMDROP_V1:
    /* this outbound policy attribute is obsolete in V2 config */
    pMibV3->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_RANDOMDROP_V3;
    MIGRATE_COPY_STRUCT (pMibV3->mib.stmt.randomDrop, pMibV1->mib.stmt.randomDrop);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_SHAPE_AVERAGE_V1:
    /* this outbound policy attribute is obsolete in V2 config */
    pMibV3->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_SHAPE_AVERAGE_V3;
    MIGRATE_COPY_STRUCT (pMibV3->mib.stmt.shapeAverage, pMibV1->mib.stmt.shapeAverage);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_SHAPE_PEAK_V1:
    /* this outbound policy attribute is obsolete in V2 config */
    pMibV3->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_SHAPE_PEAK_V3;
    MIGRATE_COPY_STRUCT (pMibV3->mib.stmt.shapePeak, pMibV1->mib.stmt.shapePeak);
    break;

  default:
    /* unrecognized entry type */
    LOG_MSG ("Unrecognized policy attribute entry type %u\n", 
             (L7_uint32)pMibV1->mib.entryType);
    break;                     
  }

  /* --------------------------- */
  /* Handle flags                */
  /* --------------------------- */

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_ENTRYTYPE_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_ENTRYTYPE_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATEUNITS_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATEUNITS_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_EBURST_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_EBURST_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_PRATE_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_PRATE_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_PBURST_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_PBURST_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_COSVAL_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_COSVAL_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_IPDSCPVAL_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_IPDSCPVAL_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_IPPRECEDENCEVAL_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_IPPRECEDENCEVAL_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_MINTHRESH_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_MINTHRESH_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_MAXTHRESH_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_MAXTHRESH_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_MAXDROPPROB_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_MAXDROPPROB_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_SAMPLINGRATE_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_SAMPLINGRATE_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, DSMIB_POLICY_ATTR_FLAGS_STMT_DECAYEXPONENT_V1))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_DECAYEXPONENT_V3);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV1, ~DSMIB_POLICY_ATTR_FLAGS_ALL_V1))
  {
    LOG_MSG ("Unrecognized rowInvalidFlags 0x%8.8x in policy attr id (%u,%u)\n", 
             (L7_uint32)(pMibV1->rowInvalidFlags & (~DSMIB_POLICY_ATTR_FLAGS_ALL_V1)), 
             pMibV1->instIndexRef, pMibV1->mib.index);
  }

}

/*********************************************************************
* @purpose  Converts DiffServ policy attr police action from version V1 to V3.
*
* @param    pActV1    @b{(input)} ptr to version 1 police action
* @param    pActV3    @b{(input)} ptr to version 3 police action
*
* @returns  void
*
* @end
*********************************************************************/
static void
diffServMigrateConfigV1V3PoliceActConvert (dsmibPoliceActV1_t * pActV1,
                                           dsmibPoliceActV3_t * pActV3)
{
  switch (*pActV1)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE_V1:
    *pActV3 = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE_V3;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP_V1:
    *pActV3 = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP_V3;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP_V1:
    *pActV3 = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP_V3;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC_V1:
    *pActV3 = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC_V3;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND_V1:
    *pActV3 = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND_V3;
    break;

  default:
    /* unrecognized police action type */
    *pActV3 = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE_V3;
    LOG_MSG ("Unrecognized police action type %u\n", (L7_uint32)*pActV1); 
    break;                   
  }
}

/*********************************************************************
* @purpose  Converts policy attr mib data structure from version V3 to V4.
*
* @param    pCfgV3    @b{(input)} ptr to version 3 mib data structure
* @param    pCfgV4    @b{(input)} ptr to version 4 mib data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
static void
diffServMigrateConfigV3V4PolicyAttrConvert (L7_diffServPolicyAttrV3_t * pMibV3,
                                            L7_diffServPolicyAttr_t * pMibV4)
{

  pMibV4->instIndexRef = pMibV3->instIndexRef;
  pMibV4->chainIndex = pMibV3->chainIndex;


  pMibV4->mib.index = pMibV3->mib.index;
  pMibV4->mib.storageType = pMibV3->mib.storageType;
  pMibV4->mib.rowStatus = pMibV3->mib.rowStatus;


  /* --------------------------- */
  /* Handle entry types          */
  /* --------------------------- */


  switch (pMibV3->mib.entryType)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE_V3:
    pMibV4->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
    /* no MIB data to copy for this case */
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE_V3:
    /* this policy attribute was introduced in attr V3 config */
    pMibV4->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE;
    MIGRATE_COPY_STRUCT (pMibV4->mib.stmt.assignQueue, pMibV3->mib.stmt.assignQueue);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_BANDWIDTH_V3:
    /* this outbound policy attribute is obsolete starting with attr V3 config */
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP_V3:
    /* this policy attribute was introduced in attr V3 config */
    pMibV4->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP;
    MIGRATE_COPY_STRUCT (pMibV4->mib.stmt.drop, pMibV3->mib.stmt.drop);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_EXPEDITE_V3:
    /* this outbound policy attribute is obsolete starting with attr V3 config */
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL_V3:
    /* this policy attribute moved from outbound to inbound starting with attr V3 config */
    pMibV4->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL;
    MIGRATE_COPY_STRUCT (pMibV4->mib.stmt.markCos, pMibV3->mib.stmt.markCos);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL_V3:
    /* this policy attribute was introduced in attr V3 config */
    pMibV4->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL;
    MIGRATE_COPY_STRUCT (pMibV4->mib.stmt.markCos2, pMibV3->mib.stmt.markCos2);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL_V3:
    pMibV4->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL;
    MIGRATE_COPY_STRUCT (pMibV4->mib.stmt.markIpDscp, pMibV3->mib.stmt.markIpDscp);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL_V3:
    pMibV4->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL;
    MIGRATE_COPY_STRUCT (pMibV4->mib.stmt.markIpPrecedence, pMibV3->mib.stmt.markIpPrecedence);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE_V3:
    /* color mode fields were introduced in attr V3 config */
    pMibV4->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE;
    MIGRATE_COPY_STRUCT (pMibV4->mib.stmt.policeSimple, pMibV3->mib.stmt.policeSimple);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE_V3:
    /* color mode fields were introduced in attr V3 config */
    pMibV4->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    MIGRATE_COPY_STRUCT (pMibV4->mib.stmt.policeSingleRate, pMibV3->mib.stmt.policeSingleRate);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE_V3:
    /* color mode fields were introduced in attr V3 config */
    pMibV4->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    MIGRATE_COPY_STRUCT (pMibV4->mib.stmt.policeTwoRate, pMibV3->mib.stmt.policeTwoRate);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_RANDOMDROP_V3:
    /* this outbound policy attribute is obsolete starting with attr V3 config */
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT_V3:
    /* this policy attribute was introduced in attr V3 config */
    pMibV4->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT;
    MIGRATE_COPY_STRUCT (pMibV4->mib.stmt.redirect, pMibV3->mib.stmt.redirect);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_SHAPE_AVERAGE_V3:
    /* this outbound policy attribute is obsolete starting with attr V3 config */
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_SHAPE_PEAK_V3:
    /* this outbound policy attribute is obsolete starting with attr V3 config */
    break;

  default:
    /* unrecognized entry type */
    LOG_MSG ("Unrecognized policy attribute entry type %u\n", 
             (L7_uint32)pMibV3->mib.entryType);
    break;                     
  }

  /* --------------------------- */
  /* Handle flags                */
  /* --------------------------- */

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_ENTRYTYPE_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_ENTRYTYPE);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_QUEUEID_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_QUEUEID);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATEUNITS_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATEUNITS);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_EBURST_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_EBURST);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_PRATE_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_PRATE);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_PBURST_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_PBURST);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_COSVAL_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_COSVAL);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_COS2VAL_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_COS2VAL);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_IPDSCPVAL_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_IPDSCPVAL);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_IPPRECEDENCEVAL_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_IPPRECEDENCEVAL);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMINDEX_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMINDEX);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDINDEX_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDINDEX);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDMODE_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDMODE);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDVAL_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDVAL);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_MINTHRESH_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_MINTHRESH);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_MAXTHRESH_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_MAXTHRESH);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_MAXDROPPROB_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_MAXDROPPROB);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_SAMPLINGRATE_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_SAMPLINGRATE);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_DECAYEXPONENT_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_DECAYEXPONENT);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, DSMIB_POLICY_ATTR_FLAGS_STMT_REDIRECTINTF_V3))
  {
    DSMIB_ROW_INVALID_FLAG_SET(pMibV4, DSMIB_POLICY_ATTR_FLAGS_STMT_REDIRECTINTF);
  }

  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pMibV3, ~DSMIB_POLICY_ATTR_FLAGS_ALL_V3))
  {
    LOG_MSG ("Unrecognized rowInvalidFlags 0x%8.8x in policy attr id (%u,%u)\n", 
             (L7_uint32)(pMibV3->rowInvalidFlags & (~DSMIB_POLICY_ATTR_FLAGS_ALL_V3)), 
             pMibV3->instIndexRef, pMibV3->mib.index);
  }

}

