/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename acl_migrate.c
*
* @purpose Access Control Lists' Configuration Migration
*
* @component Access Control List
*
* @comments none
*
* @create 08/24/2004
*
* @author Rama Sasthri, Kristipati
*
* @end
*
**********************************************************************/

#include <string.h>
#include "acl_migrate.h"
#include "acl_api.h"

/* The following configuration versions are supported for migration in this file:
 *   V1: Rel 4.0.0.2 (rel_g)
 *   V2: Rel 4.1.2.0 (rel_h)
 *   V3: Rel 4.2.1.0 (rel_h2)
 *   V4: Rel 4.3.x   (rel_i)
 *   V5: Rel 4.4.x   (rel_k)
 *   V6: Rel 5.0.x   (rel_l)
 */

static L7_RC_t aclMigrateConfigV1V2Convert(aclCfgFileDataV1_t *pCfgV1,
                                           aclCfgFileDataV2_t *pCfgV2);
static L7_RC_t aclMigrateConfigV2V3Convert(aclCfgFileDataV2_t *pCfgV2,
                                           aclCfgFileDataV3_t *pCfgV3);
static L7_RC_t aclMigrateConfigV3V4Convert(aclCfgFileDataV3_t *pCfgV3,
                                           aclCfgFileDataV4_t *pCfgV4);
static L7_RC_t aclMigrateConfigV4V5Convert(aclCfgFileDataV4_t *pCfgV4, 
                                           aclCfgFileDataV5_t *pCfgV5);
static L7_RC_t aclMigrateConfigV5V6Convert(aclCfgFileDataV5_t *pCfgV5, 
                                           aclCfgFileDataV6_t *pCfgV6);
static void aclBuildDefaultConfigDataVer(void *pCfg, L7_uint32 cfgSize, L7_uint32 ver);
static void aclMigrateConfigAclIdDataV5V6Convert(aclIdV5_t *paclIdV5, aclIdV6_t *paclIdV6);
static void aclMigrateConfigMaskV3V4Convert(L7_uint32 configMaskV3, L7_uint32 *pCfgMaskV4);
static void aclMigrateConfigMaskIpV4V5Convert(L7_uint32 configMaskV4, L7_uint32 *pCfgMaskV5);
static void aclMigrateConfigMaskMacV4V5Convert(L7_uint32 configMaskV4, L7_uint32 *pCfgMaskV5);
static void aclMigrateConfigMaskIpV5V6Convert(L7_uint32 configMaskV5, L7_uint32 *pCfgMaskV6);
static void aclMigrateConfigMaskMacV5V6Convert(L7_uint32 configMaskV5, L7_uint32 *pCfgMaskV6);

#ifndef PROD_USE_BINARY_CONFIGURATION  
static L7_RC_t aclApplyV6RuleConfigData(aclCfgFileDataV6_t *pCfgV6);
static L7_RC_t aclApplyV6ConfigDataIp(aclCfgDataV6_t *aclData);
static L7_RC_t aclApplyV6ConfigDataMac(aclCfgDataV6_t *aclData);
#endif

static aclCfgFileData_t *pCfgCur = L7_NULLPTR;

extern L7_uint32 aclCompId_g;

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
aclMigrateConfigData(L7_uint32 oldVer, L7_uint32 ver, L7_char8 *pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize, *pOldCfgCksum, oldCfgVer;
#ifndef PROD_USE_BINARY_CONFIGURATION  
  L7_RC_t rc;
#endif

  /* declare a pointer to each supported config structure version */
  aclCfgFileDataV1_t *pCfgV1 = L7_NULLPTR;
  aclCfgFileDataV2_t *pCfgV2 = L7_NULLPTR;
  aclCfgFileDataV3_t *pCfgV3 = L7_NULLPTR;
  aclCfgFileDataV4_t *pCfgV4 = L7_NULLPTR;
  aclCfgFileDataV5_t *pCfgV5 = L7_NULLPTR;
  aclCfgFileDataV6_t *pCfgV6 = L7_NULLPTR;

  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;

  pCfgCur = (aclCfgFileData_t *)pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *)pCfgBuffer;
  MIGRATE_FUNCTION(oldVer, ver, sizeof(*pCfgCur));

  if ((pCfgHdr->version == ACL_CFG_VER_2) && (pCfgHdr->length == sizeof(aclCfgFileDataV3_t)))
  {
    oldVer = ACL_CFG_VER_3;
  }

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case ACL_CFG_VER_1:
    pCfgV1 = (aclCfgFileDataV1_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                               (L7_uint32)sizeof(aclCfgFileDataV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *)pCfgV1;
    oldCfgSize = (L7_uint32)sizeof(aclCfgFileDataV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = ACL_CFG_VER_1;
    break;

  case ACL_CFG_VER_2:
    pCfgV2 = (aclCfgFileDataV2_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                               (L7_uint32)sizeof(aclCfgFileDataV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *)pCfgV2;
    oldCfgSize = (L7_uint32)sizeof(aclCfgFileDataV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = ACL_CFG_VER_2;
    break;

  case ACL_CFG_VER_3:
    pCfgV3 = (aclCfgFileDataV3_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                               (L7_uint32)sizeof(aclCfgFileDataV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *)pCfgV3;
    oldCfgSize = (L7_uint32)sizeof(aclCfgFileDataV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = ACL_CFG_VER_2;  /* b'cos: REL_H version was not updated */
    break;

  case ACL_CFG_VER_4:
    pCfgV4 = (aclCfgFileDataV4_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                               (L7_uint32)sizeof(aclCfgFileDataV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *)pCfgV4;
    oldCfgSize = (L7_uint32)sizeof(aclCfgFileDataV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = ACL_CFG_VER_4;
    break;

  case ACL_CFG_VER_5:
    pCfgV5 = (aclCfgFileDataV5_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                               (L7_uint32) sizeof(aclCfgFileDataV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *)pCfgV5;
    oldCfgSize = (L7_uint32)sizeof(aclCfgFileDataV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = ACL_CFG_VER_5;
    break;

  case ACL_CFG_VER_6:
    pCfgV6 = (aclCfgFileDataV6_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                               (L7_uint32) sizeof(aclCfgFileDataV6_t));
    if (L7_NULLPTR == pCfgV6)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *)pCfgV6;
    oldCfgSize = (L7_uint32)sizeof(aclCfgFileDataV6_t);
    pOldCfgCksum = &pCfgV6->checkSum;
    oldCfgVer = ACL_CFG_VER_6;
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
        (L7_SUCCESS != sysapiCfgFileGet(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                        ACL_CFG_FILENAME,
                                        pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                        oldCfgVer, L7_NULL /* do not build defaults */ ,
                                        L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_QOS_ACL_COMPONENT_ID,
                     "sysapiCfgFileGet failed size = %d version = %d."
                     " Configuration did not exist or could not be read"
                     " for the specified feature.  This message is usually"
                     " followed by a message indicating that default configuration"
                     " values will be used.", 
                    (L7_int32)oldCfgSize, (L7_int32) oldCfgVer);
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
    case ACL_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (aclCfgFileDataV2_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                 (L7_uint32)sizeof(aclCfgFileDataV2_t));
      if ((L7_NULLPTR == pCfgV1) || 
          (L7_NULLPTR == pCfgV2) || 
          (L7_SUCCESS != aclMigrateConfigV1V2Convert(pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case ACL_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (aclCfgFileDataV3_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                 (L7_uint32)sizeof(aclCfgFileDataV3_t));
      if ((L7_NULLPTR == pCfgV2) || 
          (L7_NULLPTR == pCfgV3) || 
          (L7_SUCCESS != aclMigrateConfigV2V3Convert(pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case ACL_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (aclCfgFileDataV4_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                 (L7_uint32)sizeof(aclCfgFileDataV4_t));
      if ((L7_NULLPTR == pCfgV3) || 
          (L7_NULLPTR == pCfgV4) || 
          (L7_SUCCESS != aclMigrateConfigV3V4Convert(pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case ACL_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (aclCfgFileDataV5_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                 (L7_uint32)sizeof(aclCfgFileDataV5_t));
      if ((L7_NULLPTR == pCfgV4) || 
          (L7_NULLPTR == pCfgV5) || 
          (L7_SUCCESS != aclMigrateConfigV4V5Convert(pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case ACL_CFG_VER_5:
      /* pCfgV5 has already been allocated and filled in */
      pCfgV6 = (aclCfgFileDataV6_t *)osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID,
                                                 (L7_uint32)sizeof(aclCfgFileDataV6_t));
      if ((L7_NULLPTR == pCfgV5) || 
          (L7_NULLPTR == pCfgV6) || 
          (L7_SUCCESS != aclMigrateConfigV5V6Convert(pCfgV5, pCfgV6)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pCfgV5);
      pCfgV5 = L7_NULLPTR;
      /*passthru */

    case ACL_CFG_VER_6:
      /* pCfgV6 has already been allocated and filled in */
      if (L7_NULLPTR == pCfgV6)
      {
        buildDefault = L7_TRUE;
        break;
      }
      /* this check assures the internal definition of the V6 configuration structure matches that of the 
       * current release.  An error here might indicate a problem with how the two structures are defined with respect
       * to scaling parameters.
       */
      if ((sizeof(pCfgCur->cfgParms.aclIntfCfgData) != sizeof(pCfgV6->cfgParms.aclIntfCfgData)) ||
          (sizeof(pCfgCur->cfgParms.aclVlanCfgData) != sizeof(pCfgV6->cfgParms.aclVlanCfgData)) ||
#ifdef PROD_USE_BINARY_CONFIGURATION  
          (sizeof(pCfgCur->cfgParms.aclData) != sizeof(pCfgV6->cfgParms.aclData)) ||
#endif
          (sizeof(pCfgCur->cfgParms.aclMsgLvl) != sizeof(pCfgV6->cfgParms.aclMsgLvl)))
      {
        MIGRATE_DEBUG("Configuration structure size mismatch.  Building default configuration.\n " \
                      "sizeof(pCfgCur->cfgParms.aclIntfCfgData) = %u, sizeof(pCfgV6->cfgParms.aclIntfCfgData) = %u\n" \
                      "sizeof(pCfgCur->cfgParms.aclVlanCfgData) = %u, sizeof(pCfgV6->cfgParms.aclVlanCfgData) = %u\n",
                      sizeof(pCfgCur->cfgParms.aclIntfCfgData), sizeof(pCfgV6->cfgParms.aclIntfCfgData),
                      sizeof(pCfgCur->cfgParms.aclVlanCfgData), sizeof(pCfgV6->cfgParms.aclVlanCfgData));
#ifdef PROD_USE_BINARY_CONFIGURATION  
        MIGRATE_DEBUG("sizeof(pCfgCur->cfgParms.aclData) = %u, sizeof(pCfgV6->cfgParms.aclData) = %u\n",
                      sizeof(pCfgCur->cfgParms.aclData), sizeof(pCfgV6->cfgParms.aclData));
#endif
        buildDefault = L7_TRUE;
      }
      else
      {
#ifdef PROD_USE_BINARY_CONFIGURATION  
        /* if product uses binary configuration, copy the rule configuration into the current version configuration structure
         * so it can be applied by the configurator code
         */
        memcpy((L7_char8 *)&pCfgCur->cfgParms.aclData, (L7_char8 *)&pCfgV6->cfgParms.aclData, sizeof(pCfgCur->cfgParms.aclData));
#else
        /* In release 5.0.x and beyond, text-based-config and the way the ACL application stores operational and configuration data
         * allowed for the rule data to no longer be stored in the current configuration structure.  This save significant memory on platforms
         * where ACL scaling factors are increasing.  Due to this, we must apply any rule configuration directly from the V6 migrated structure since
         * it cannot be copied into the current configuration structure. 
         */
        if ((rc = aclApplyV6RuleConfigData(pCfgV6)) != L7_SUCCESS)
        {
          MIGRATE_DEBUG("Error in applying migrated binary rule configuration.  Building default configuration. rc = %d\n", rc);
          buildDefault = L7_TRUE;
        }
#endif
        /* copy the migrated config to the current config struct */
        memcpy((L7_char8 *)&pCfgCur->cfgParms.aclIntfCfgData, (L7_char8 *)&pCfgV6->cfgParms.aclIntfCfgData, sizeof(pCfgCur->cfgParms.aclIntfCfgData));
        memcpy((L7_char8 *)&pCfgCur->cfgParms.aclVlanCfgData, (L7_char8 *)&pCfgV6->cfgParms.aclVlanCfgData, sizeof(pCfgCur->cfgParms.aclVlanCfgData));
        memcpy((L7_char8 *)&pCfgCur->cfgParms.aclMsgLvl, (L7_char8 *)&pCfgV6->cfgParms.aclMsgLvl, sizeof(pCfgCur->cfgParms.aclMsgLvl));

        /* done with migration - Flag unsaved configuration */
        pCfgCur->cfgHdr.dataChanged = L7_TRUE;
      }
      osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pCfgV6);
      pCfgV6 = L7_NULLPTR;

      break;

    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV1 != L7_NULLPTR)
    osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pCfgV5);
  if (pCfgV6 != L7_NULLPTR)
    osapiFree(L7_FLEX_QOS_ACL_COMPONENT_ID, pCfgV6);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG("Building Defaults\n");
    memset((L7_char8 *)pCfgCur, 0, sizeof(*pCfgCur));
    aclBuildDefaultConfigData(ACL_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }
  else
  {
    /* display the test config data that was migrated, since it
     * is not likely to be applied successfully (this value for
     * the message level was stored into the test config data)
     */
    if (pCfgCur->cfgParms.aclMsgLvl == (L7_uint32)ACL_MSGLVL_OFF)
    {
      MIGRATE_DEBUG("Displaying migrated test config data\n");
      aclConfigDataTestShow();
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
aclMigrateConfigV1V2Convert(aclCfgFileDataV1_t *pCfgV1, aclCfgFileDataV2_t *pCfgV2)
{
  L7_RC_t rc;
  L7_int32 i, j;
  L7_uint32 count;
  nimConfigID_t configId;
  L7_INTF_TYPES_t intfType;
  L7_int32 intfCfgIndex;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != ACL_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION(pCfgV1->cfgHdr.version, ACL_CFG_VER_1);
    return L7_FAILURE;
  }

  aclBuildDefaultConfigDataVer(pCfgV2, sizeof(*pCfgV2), ACL_CFG_VER_2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  count = min(L7_ACL_MAX_RULE_NUM_REL_4_0, L7_ACL_MAX_RULE_NUM_REL_4_1);
  for (i = 0; i <= L7_ACL_MAX_LISTS; i++)
  {
    if (pCfgV1->cfgParms.aclData[i].aclNum == 0)
      continue;                 /* default */

    pCfgV2->cfgParms.aclData[i].aclNum = pCfgV1->cfgParms.aclData[i].aclNum;

    for (j = 0; j <= count; j++)
    {
      MIGRATE_COPY_STRUCT(pCfgV2->cfgParms.aclData[i].ruleData[j],
                          pCfgV1->cfgParms.aclData[i].ruleData[j]);
    }
  }

  intfCfgIndex = 1;
  intfType = 0;
  for (i = 1; i < L7_MAX_INTERFACE_COUNT_REL_4_0; i++)
  {
    rc = sysapiCfgFileRel4_0IndexToIntfInfoGet(i, &configId, L7_NULL, &intfType);
    if (L7_SUCCESS != rc)
    {
      if (L7_ERROR == rc)
        MIGRATE_INVALID_INDEX(i);
      continue;
    }

    /* Determine if the interface is valid for participation in this feature */
    if (aclIsValidIntfType(intfType) != L7_TRUE)
      continue;

    if (intfCfgIndex >= L7_ACL_INTF_MAX_COUNT_REL_4_1)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE("acl.cfg", intfCfgIndex);
      break;
    }

    
    /* copy interface ACL assignments */
    pCfgV2->cfgParms.aclIntfCfgData[intfCfgIndex].intf_dir_Tb[L7_INBOUND_ACL_REL_4_1] =
      pCfgV1->cfgParms.intf_dir_Tb[i][L7_INBOUND_ACL_REL_4_0];

    pCfgV2->cfgParms.aclIntfCfgData[intfCfgIndex].intf_dir_Tb[L7_OUTBOUND_ACL_REL_4_1] =
      pCfgV1->cfgParms.intf_dir_Tb[i][L7_OUTBOUND_ACL_REL_4_0];


    (void)nimConfigIdCopy(&configId, &pCfgV2->cfgParms.aclIntfCfgData[intfCfgIndex].configId);
    intfCfgIndex++;
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
aclMigrateConfigV2V3Convert(aclCfgFileDataV2_t *pCfgV2, aclCfgFileDataV3_t *pCfgV3)
{
  L7_uint32 i, ifCount;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != ACL_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION(pCfgV2->cfgHdr.version, ACL_CFG_VER_2);
    return L7_FAILURE;
  }

  aclBuildDefaultConfigDataVer(pCfgV3, sizeof(*pCfgV3), ACL_CFG_VER_3);

  MIGRATE_COPY_STRUCT(pCfgV3->cfgParms.aclData, pCfgV2->cfgParms.aclData);

  ifCount = min(L7_ACL_INTF_MAX_COUNT_REL_4_1, L7_ACL_INTF_MAX_COUNT_REL_4_1);
  for (i = 0; i < ifCount; i++)
  {
    MIGRATE_COPY_STRUCT(pCfgV3->cfgParms.aclIntfCfgData[i].configId,
                        pCfgV2->cfgParms.aclIntfCfgData[i].configId);

    /* copy interface ACL assignments */
    pCfgV3->cfgParms.aclIntfCfgData[i].intf_dir_Tb[L7_INBOUND_ACL_REL_4_2] = 
      pCfgV2->cfgParms.aclIntfCfgData[i].intf_dir_Tb[L7_INBOUND_ACL_REL_4_1];

    pCfgV3->cfgParms.aclIntfCfgData[i].intf_dir_Tb[L7_OUTBOUND_ACL_REL_4_2] = 
      pCfgV2->cfgParms.aclIntfCfgData[i].intf_dir_Tb[L7_OUTBOUND_ACL_REL_4_1];
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
* @end
*********************************************************************/
static L7_RC_t
aclMigrateConfigV3V4Convert(aclCfgFileDataV3_t *pCfgV3, aclCfgFileDataV4_t *pCfgV4)
{
  L7_int32 j, k, ifCount, aclId;
  aclRuleParmsV1_t *pRuleParamV3;
  aclRuleParmsV4_t *pRuleParamV4;
  aclIntfParmsV4_t *pAssignList;
  nimConfigID_t configIdNull;

  /* verify correct version of old config file */
  if ((pCfgV3->cfgHdr.version != ACL_CFG_VER_3) && (pCfgV3->cfgHdr.version != ACL_CFG_VER_2))   /* b'cos: REL_H version was not updated */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION(pCfgV3->cfgHdr.version, ACL_CFG_VER_3);
    return L7_FAILURE;
  }

  aclBuildDefaultConfigDataVer(pCfgV4, sizeof(*pCfgV4), ACL_CFG_VER_4);

  for (j = 1; j <= L7_ACL_MAX_LISTS; j++)
  {
    if (pCfgV3->cfgParms.aclData[j].aclNum == 0)
      continue;                 /* default */

    pCfgV4->cfgParms.aclData[j].id.aclId = pCfgV3->cfgParms.aclData[j].aclNum;
    pCfgV4->cfgParms.aclData[j].id.aclType = L7_ACL_TYPE_IP;

    for (k = 0; k <= L7_ACL_MAX_RULE_NUM_REL_4_2; k++)
    {
      pRuleParamV3 = &pCfgV3->cfgParms.aclData[j].ruleData[k];

      if (pRuleParamV3->configMask == 0)
        continue;

      /* starting with Rel 4.3, the ACL max rule number definition is
       * specified per-platform instead of within the application,
       * so check for possible rule truncation here
       */
      if (k > L7_ACL_MAX_RULE_NUM_REL_4_3)
      {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_QOS_ACL_COMPONENT_ID, 
                "IP ACL %u:  Forced truncation of one or more rules during config migration."
                " While processing the saved configuration, the system encountered an ACL with "
                "more rules than is supported by the current version. This may happen when code is"
                " updated to a version supporting fewer rules per ACL than the previous version.",
                pCfgV3->cfgParms.aclData[j].aclNum);
        break;
      }

      pRuleParamV4 = &pCfgV4->cfgParms.aclData[j].ruleData[k].ip;

      aclMigrateConfigMaskV3V4Convert(pRuleParamV3->configMask, &pRuleParamV4->configMask);
      pRuleParamV4->ruleNum      = pRuleParamV3->ruleNum;
      pRuleParamV4->action       = pRuleParamV3->action;
      pRuleParamV4->every        = pRuleParamV3->every;
      pRuleParamV4->protocol     = pRuleParamV3->protocol;
      pRuleParamV4->protmask     = pRuleParamV3->protmask;
      pRuleParamV4->srcIp        = pRuleParamV3->srcIp;
      pRuleParamV4->srcMask      = pRuleParamV3->srcMask;
      pRuleParamV4->srcPort      = pRuleParamV3->srcPort;
      pRuleParamV4->srcStartPort = pRuleParamV3->srcStartPort;
      pRuleParamV4->srcEndPort   = pRuleParamV3->srcEndPort;
      pRuleParamV4->dstIp        = pRuleParamV3->dstIp;
      pRuleParamV4->dstMask      = pRuleParamV3->dstMask;
      pRuleParamV4->dstPort      = pRuleParamV3->dstPort;
      pRuleParamV4->dstStartPort = pRuleParamV3->dstStartPort;
      pRuleParamV4->dstEndPort   = pRuleParamV3->dstEndPort;
      pRuleParamV4->tosbyte      = pRuleParamV3->tosbyte;
      pRuleParamV4->tosmask      = pRuleParamV3->tosmask;
      pRuleParamV4->last         = pRuleParamV3->last;
      pRuleParamV4->next         = L7_NULLPTR;
    }
  }

  memset((L7_char8 *)&configIdNull, 0, sizeof(nimConfigID_t));
  ifCount = min(L7_ACL_INTF_MAX_COUNT_REL_4_1, L7_ACL_INTF_MAX_COUNT_REL_4_3);
  for (j = 0; j < ifCount; j++)
  {
    MIGRATE_COPY_STRUCT(pCfgV4->cfgParms.aclIntfCfgData[j].configId,
                        pCfgV3->cfgParms.aclIntfCfgData[j].configId);

    if (L7_TRUE == NIM_CONFIG_ID_IS_EQUAL(&pCfgV4->cfgParms.aclIntfCfgData[j].configId,
                                          &configIdNull))
    {
      continue;
    }

    for (k = 0; k < ACL_INTF_DIR_MAX; k++)
    {
      pAssignList = &pCfgV4->cfgParms.aclIntfCfgData[j].intf_dir_Tb[k].assignList[0];
      if (k == L7_INBOUND_ACL_REL_4_3)
        aclId = pCfgV3->cfgParms.aclIntfCfgData[j].intf_dir_Tb[L7_INBOUND_ACL_REL_4_2];
      else if (k == L7_OUTBOUND_ACL_REL_4_3)
        aclId = pCfgV3->cfgParms.aclIntfCfgData[j].intf_dir_Tb[L7_OUTBOUND_ACL_REL_4_2];
      else
        aclId = 0;
      pAssignList->id.aclId = aclId;
      if (pAssignList->id.aclId == 0)
        continue;               /* default */
      pAssignList->inUse = L7_TRUE;
      pAssignList->seqNum = 1;
      pAssignList->id.aclType = L7_ACL_TYPE_IP;
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
aclMigrateConfigV4V5Convert(aclCfgFileDataV4_t *pCfgV4, aclCfgFileDataV5_t *pCfgV5)
{
  L7_uint32             i, j, k;
  L7_uint32             listCount, ruleCount, ifCount, assignCount;
  aclRuleParmsV4_t      *pIpRuleV4;
  aclRuleParmsV5_t      *pIpRuleV5;
  aclMacRuleParmsV4_t   *pMacRuleV4;
  aclMacRuleParmsV5_t   *pMacRuleV5;
  nimConfigID_t         configIdNull;
  aclIntfParmsV4_t      *pAssignListV4;
  aclIntfParmsV5_t      *pAssignListV5;

  /* verify correct version of old config file */
  if (pCfgV4->cfgHdr.version != ACL_CFG_VER_4)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION(pCfgV4->cfgHdr.version, ACL_CFG_VER_4);
    return L7_FAILURE;
  }

  aclBuildDefaultConfigDataVer(pCfgV5, sizeof(*pCfgV5), ACL_CFG_VER_5);

  /* migrate the ACL rule config */
  listCount = min(L7_ACL_MAX_LISTS_REL_4_3, L7_ACL_MAX_LISTS_REL_4_4);
  ruleCount = min(L7_ACL_MAX_RULE_NUM_REL_4_3, L7_ACL_MAX_RULE_NUM_REL_4_4);
  for (i = 1; i <= listCount; i++)
  {
    /* check for supported ACL types */
    if ((pCfgV4->cfgParms.aclData[i].id.aclType != L7_ACL_TYPE_IP) &&
        (pCfgV4->cfgParms.aclData[i].id.aclType != L7_ACL_TYPE_MAC))
      continue;                 /* default */

    pCfgV5->cfgParms.aclData[i].id.aclType = pCfgV4->cfgParms.aclData[i].id.aclType;
    pCfgV5->cfgParms.aclData[i].id.aclId   = pCfgV4->cfgParms.aclData[i].id.aclId;

    memcpy(pCfgV5->cfgParms.aclData[i].aclName,
           pCfgV4->cfgParms.aclData[i].aclName,
           sizeof(pCfgV5->cfgParms.aclData[i].aclName));

    for (j = 0; j <= ruleCount; j++)
    {
      if (pCfgV4->cfgParms.aclData[i].id.aclType == L7_ACL_TYPE_IP)
      {
        /* IP ACL rule */
        pIpRuleV4 = &pCfgV4->cfgParms.aclData[i].ruleData[j].ip;
        pIpRuleV5 = &pCfgV5->cfgParms.aclData[i].ruleData[j].ip;

        if (pIpRuleV4->configMask == 0)
          continue;

        aclMigrateConfigMaskIpV4V5Convert(pIpRuleV4->configMask, &pIpRuleV5->configMask);

        pIpRuleV5->ruleNum          = pIpRuleV4->ruleNum;
        pIpRuleV5->action           = pIpRuleV4->action;

        pIpRuleV5->every            = pIpRuleV4->every;
        pIpRuleV5->assignQueueId    = pIpRuleV4->assignQueueId;
        pIpRuleV5->redirectConfigId = pIpRuleV4->redirectConfigId;

        pIpRuleV5->protocol         = pIpRuleV4->protocol;
        pIpRuleV5->protmask         = pIpRuleV4->protmask;

        pIpRuleV5->srcIp            = pIpRuleV4->srcIp;
        pIpRuleV5->srcMask          = pIpRuleV4->srcMask;
        pIpRuleV5->srcPort          = pIpRuleV4->srcPort;
        pIpRuleV5->srcStartPort     = pIpRuleV4->srcStartPort;
        pIpRuleV5->srcEndPort       = pIpRuleV4->srcEndPort;

        pIpRuleV5->dstIp            = pIpRuleV4->dstIp;
        pIpRuleV5->dstMask          = pIpRuleV4->dstMask;
        pIpRuleV5->dstPort          = pIpRuleV4->dstPort;
        pIpRuleV5->dstStartPort     = pIpRuleV4->dstStartPort;
        pIpRuleV5->dstEndPort       = pIpRuleV4->dstEndPort;

        pIpRuleV5->tosbyte          = pIpRuleV4->tosbyte;
        pIpRuleV5->tosmask          = pIpRuleV4->tosmask;

        pIpRuleV5->last             = pIpRuleV4->last;
        pIpRuleV5->next             = L7_NULLPTR;

      } /* endif IP rule */

      else
      {
        /* MAC ACL rule */
        pMacRuleV4 = &pCfgV4->cfgParms.aclData[i].ruleData[j].mac;
        pMacRuleV5 = &pCfgV5->cfgParms.aclData[i].ruleData[j].mac;

        if (pMacRuleV4->configMask == 0)
          continue;

        aclMigrateConfigMaskMacV4V5Convert(pMacRuleV4->configMask, &pMacRuleV5->configMask);

        pMacRuleV5->ruleNum          = pMacRuleV4->ruleNum;
        pMacRuleV5->action           = pMacRuleV4->action;

        pMacRuleV5->every            = pMacRuleV4->every;
        pMacRuleV5->assignQueueId    = pMacRuleV4->assignQueueId;
        pMacRuleV5->redirectConfigId = pMacRuleV4->redirectConfigId;

        pMacRuleV5->cos              = pMacRuleV4->cos;
        pMacRuleV5->cos2             = pMacRuleV4->cos2;

        memcpy(pMacRuleV5->dstMac, pMacRuleV4->dstMac, (size_t)L7_MAC_ADDR_LEN);
        memcpy(pMacRuleV5->dstMacMask, pMacRuleV4->dstMacMask, (size_t)L7_MAC_ADDR_LEN);

        pMacRuleV5->etypeKeyId       = pMacRuleV4->etypeKeyId;
        pMacRuleV5->etypeValue       = pMacRuleV4->etypeValue;

        memcpy(pMacRuleV5->srcMac, pMacRuleV4->srcMac, (size_t)L7_MAC_ADDR_LEN);
        memcpy(pMacRuleV5->srcMacMask, pMacRuleV4->srcMacMask, (size_t)L7_MAC_ADDR_LEN);

        pMacRuleV5->vlanId           = pMacRuleV4->vlanId;
        pMacRuleV5->vlanIdStart      = pMacRuleV4->vlanIdStart;
        pMacRuleV5->vlanIdEnd        = pMacRuleV4->vlanIdEnd;
        pMacRuleV5->vlanId2          = pMacRuleV4->vlanId2;
        pMacRuleV5->vlanId2Start     = pMacRuleV4->vlanId2Start;
        pMacRuleV5->vlanId2End       = pMacRuleV4->vlanId2End;

        pMacRuleV5->last             = pMacRuleV4->last;
        pMacRuleV5->next             = L7_NULLPTR;

      } /* endelse MAC rule */

    } /* endfor j */
  } /* endfor i */

  /* migrate the ACL interface config */
  memset (&configIdNull, 0, sizeof (nimConfigID_t));

  ifCount = min(L7_ACL_INTF_MAX_COUNT_REL_4_3, L7_ACL_INTF_MAX_COUNT_REL_4_4);
  assignCount = min(L7_ACL_MAX_LISTS_PER_INTF_DIR_REL_4_3, L7_ACL_MAX_LISTS_PER_INTF_DIR_REL_4_4);
  for (i = 0; i < ifCount; i++)
  {
    MIGRATE_COPY_STRUCT(pCfgV5->cfgParms.aclIntfCfgData[i].configId,
                        pCfgV4->cfgParms.aclIntfCfgData[i].configId);

    if (L7_TRUE == NIM_CONFIG_ID_IS_EQUAL(&pCfgV5->cfgParms.aclIntfCfgData[i].configId,
                                          &configIdNull))
    {
      continue;
    }

    for (j = 0; j < ACL_INTF_DIR_MAX; j++)
    {
      for (k = 0; k < assignCount; k++)
      {
        pAssignListV4 = &pCfgV4->cfgParms.aclIntfCfgData[i].intf_dir_Tb[j].assignList[k];
        pAssignListV5 = &pCfgV5->cfgParms.aclIntfCfgData[i].intf_dir_Tb[j].assignList[k];

        if (pAssignListV4->inUse == L7_TRUE)
        {
          /* only allow supported ACL types to be assigned to an interface */
          if ((pAssignListV4->id.aclType == L7_ACL_TYPE_IP) ||
              (pAssignListV4->id.aclType == L7_ACL_TYPE_MAC))
          {
            pAssignListV5->inUse      = pAssignListV4->inUse;
            pAssignListV5->seqNum     = pAssignListV4->seqNum;
            pAssignListV5->id.aclType = pAssignListV4->id.aclType;
            pAssignListV5->id.aclId   = pAssignListV4->id.aclId;
          }
        }
      } /* endfor k */
    } /* endfor j */
  } /* endfor i */

  /* migrate ACL debug message level */
  pCfgV5->cfgParms.aclMsgLvl = pCfgV4->cfgParms.aclMsgLvl;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V5 to V6.
*
* @param    pCfgV5    @b{(input)} ptr to version 5 config data structure
* @param    pCfgV6    @b{(input)} ptr to version 6 config data structure
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
aclMigrateConfigV5V6Convert(aclCfgFileDataV5_t *pCfgV5, aclCfgFileDataV6_t *pCfgV6)
{
  L7_uint32             i, j, k;
  L7_uint32             listCount, ruleCount, ifCount, assignCount;
  aclRuleParmsV5_t      *pIpRuleV5;
  aclRuleParmsV6_t      *pIpRuleV6;
  aclMacRuleParmsV5_t   *pMacRuleV5;
  aclMacRuleParmsV6_t   *pMacRuleV6;
  nimConfigID_t         configIdNull;
  aclIntfParmsV5_t      *pAssignListV5;
  aclIntfParmsV6_t      *pAssignListV6;

  /* verify correct version of old config file */
  if (pCfgV5->cfgHdr.version != ACL_CFG_VER_5)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION(pCfgV5->cfgHdr.version, ACL_CFG_VER_5);
    return L7_FAILURE;
  }

  aclBuildDefaultConfigDataVer(pCfgV6, sizeof(*pCfgV6), ACL_CFG_VER_6);

  /* migrate the ACL rule config */
  listCount = min(L7_ACL_MAX_LISTS_REL_4_4, L7_ACL_MAX_LISTS_REL_5_0);
  ruleCount = min(L7_ACL_MAX_RULE_NUM_REL_4_4, L7_ACL_MAX_RULE_NUM_REL_5_0);
  for (i = 1; i <= listCount; i++)
  {
    /* check for supported ACL types */
    if ((pCfgV5->cfgParms.aclData[i].id.aclType != L7_ACL_TYPE_IP) &&
        (pCfgV5->cfgParms.aclData[i].id.aclType != L7_ACL_TYPE_MAC))
      continue;                 /* default */

    aclMigrateConfigAclIdDataV5V6Convert(&pCfgV5->cfgParms.aclData[i].id, &pCfgV6->cfgParms.aclData[i].id);

    memcpy(pCfgV6->cfgParms.aclData[i].aclName,
           pCfgV5->cfgParms.aclData[i].aclName,
           sizeof(pCfgV6->cfgParms.aclData[i].aclName));

    for (j = 0; j <= ruleCount; j++)
    {
      if (pCfgV5->cfgParms.aclData[i].id.aclType == L7_ACL_TYPE_IP)
      {
        /* IP ACL rule */
        pIpRuleV5 = &pCfgV5->cfgParms.aclData[i].ruleData[j].ip;
        pIpRuleV6 = &pCfgV6->cfgParms.aclData[i].ruleData[j].ip;

        if (pIpRuleV5->configMask == 0)
          continue;

        aclMigrateConfigMaskIpV5V6Convert(pIpRuleV5->configMask, &pIpRuleV6->configMask);

        pIpRuleV6->ruleNum          = pIpRuleV5->ruleNum;
        pIpRuleV6->action           = pIpRuleV5->action;

        pIpRuleV6->assignQueueId    = pIpRuleV5->assignQueueId;
        pIpRuleV6->redirectConfigId = pIpRuleV5->redirectConfigId;
        pIpRuleV6->mirrorConfigId   = pIpRuleV5->mirrorConfigId;
        pIpRuleV6->logging          = pIpRuleV5->logging;

        pIpRuleV6->every            = pIpRuleV5->every;

        pIpRuleV6->protocol         = pIpRuleV5->protocol;
        pIpRuleV6->protmask         = pIpRuleV5->protmask;

        pIpRuleV6->srcIp.v4.addr    = pIpRuleV5->srcIp;
        pIpRuleV6->srcIp.v4.mask    = pIpRuleV5->srcMask;
        pIpRuleV6->srcPort          = pIpRuleV5->srcPort;
        pIpRuleV6->srcStartPort     = pIpRuleV5->srcStartPort;
        pIpRuleV6->srcEndPort       = pIpRuleV5->srcEndPort;

        pIpRuleV6->dstIp.v4.addr    = pIpRuleV5->dstIp;
        pIpRuleV6->dstIp.v4.mask    = pIpRuleV5->dstMask;
        pIpRuleV6->dstPort          = pIpRuleV5->dstPort;
        pIpRuleV6->dstStartPort     = pIpRuleV5->dstStartPort;
        pIpRuleV6->dstEndPort       = pIpRuleV5->dstEndPort;

        pIpRuleV6->tosbyte          = pIpRuleV5->tosbyte;
        pIpRuleV6->tosmask          = pIpRuleV5->tosmask;

        pIpRuleV6->last             = pIpRuleV5->last;
        pIpRuleV6->next             = L7_NULLPTR;

      } /* endif IP rule */

      else
      {
        /* MAC ACL rule */
        pMacRuleV5 = &pCfgV5->cfgParms.aclData[i].ruleData[j].mac;
        pMacRuleV6 = &pCfgV6->cfgParms.aclData[i].ruleData[j].mac;

        if (pMacRuleV5->configMask == 0)
          continue;

        aclMigrateConfigMaskMacV5V6Convert(pMacRuleV5->configMask, &pMacRuleV6->configMask);

        pMacRuleV6->ruleNum          = pMacRuleV5->ruleNum;
        pMacRuleV6->action           = pMacRuleV5->action;

        pMacRuleV6->assignQueueId    = pMacRuleV5->assignQueueId;
        pMacRuleV6->redirectConfigId = pMacRuleV5->redirectConfigId;
        pMacRuleV6->mirrorConfigId   = pMacRuleV5->mirrorConfigId;
        pMacRuleV6->logging          = pMacRuleV5->logging;

        pMacRuleV6->every            = pMacRuleV5->every;

        pMacRuleV6->cos              = pMacRuleV5->cos;
        pMacRuleV6->cos2             = pMacRuleV5->cos2;

        memcpy(pMacRuleV6->dstMac, pMacRuleV5->dstMac, (size_t)L7_MAC_ADDR_LEN);
        memcpy(pMacRuleV6->dstMacMask, pMacRuleV5->dstMacMask, (size_t)L7_MAC_ADDR_LEN);

        pMacRuleV6->etypeKeyId       = pMacRuleV5->etypeKeyId;
        pMacRuleV6->etypeValue       = pMacRuleV5->etypeValue;

        memcpy(pMacRuleV6->srcMac, pMacRuleV5->srcMac, (size_t)L7_MAC_ADDR_LEN);
        memcpy(pMacRuleV6->srcMacMask, pMacRuleV5->srcMacMask, (size_t)L7_MAC_ADDR_LEN);

        pMacRuleV6->vlanId           = pMacRuleV5->vlanId;
        pMacRuleV6->vlanIdStart      = pMacRuleV5->vlanIdStart;
        pMacRuleV6->vlanIdEnd        = pMacRuleV5->vlanIdEnd;
        pMacRuleV6->vlanId2          = pMacRuleV5->vlanId2;
        pMacRuleV6->vlanId2Start     = pMacRuleV5->vlanId2Start;
        pMacRuleV6->vlanId2End       = pMacRuleV5->vlanId2End;

        pMacRuleV6->last             = pMacRuleV5->last;
        pMacRuleV6->next             = L7_NULLPTR;

      } /* endelse MAC rule */

    } /* endfor j */
  } /* endfor i */

  /* migrate the ACL interface config */
  memset (&configIdNull, 0, sizeof (nimConfigID_t));

  ifCount = min(L7_ACL_INTF_MAX_COUNT_REL_4_4, L7_ACL_INTF_MAX_COUNT_REL_5_0);
  assignCount = min(L7_ACL_MAX_LISTS_PER_INTF_DIR_REL_4_4, L7_ACL_MAX_LISTS_PER_INTF_DIR_REL_5_0);
  for (i = 0; i < ifCount; i++)
  {
    MIGRATE_COPY_STRUCT(pCfgV6->cfgParms.aclIntfCfgData[i].configId,
                        pCfgV5->cfgParms.aclIntfCfgData[i].configId);

    if (L7_TRUE == NIM_CONFIG_ID_IS_EQUAL(&pCfgV6->cfgParms.aclIntfCfgData[i].configId,
                                          &configIdNull))
    {
      continue;
    }

    for (j = 0; j < ACL_INTF_DIR_MAX; j++)
    {
      for (k = 0; k < assignCount; k++)
      {
        pAssignListV5 = &pCfgV5->cfgParms.aclIntfCfgData[i].intf_dir_Tb[j].assignList[k];
        pAssignListV6 = &pCfgV6->cfgParms.aclIntfCfgData[i].intf_dir_Tb[j].assignList[k];

        if (pAssignListV5->inUse == L7_TRUE)
        {
          /* only allow supported ACL types to be assigned to an interface */
          if ((pAssignListV5->id.aclType == L7_ACL_TYPE_IP) ||
              (pAssignListV5->id.aclType == L7_ACL_TYPE_MAC))
          {
            pAssignListV6->inUse      = pAssignListV5->inUse;
            pAssignListV6->seqNum     = pAssignListV5->seqNum;
            aclMigrateConfigAclIdDataV5V6Convert(&pAssignListV5->id, &pAssignListV6->id);
          }
        }
      } /* endfor k */
    } /* endfor j */
  } /* endfor i */

  /* migrate ACL debug message level */
  pCfgV6->cfgParms.aclMsgLvl = pCfgV5->cfgParms.aclMsgLvl;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Build default acl config data for a particular file version
*
* @param    pCfg      @b{(input)} ptr to version-specific config structure
* @param    cfgSize   @b{(input)} size of config data structure
* @param    ver       @b{(input)} config version number 
*
* @returns  void
*
* @comments Assumes the config header starts at offset 0 of any
*           version-specific config structure.
*
* @end
*
*********************************************************************/
static void
aclBuildDefaultConfigDataVer(void *pCfg, L7_uint32 cfgSize, L7_uint32 ver)
{
  L7_fileHdr_t  *pCfgHdr = (L7_fileHdr_t *)pCfg;

  memset(pCfg, 0, (size_t)cfgSize);
  osapiStrncpySafe(pCfgHdr->filename, ACL_CFG_FILENAME, sizeof(pCfgHdr->filename));
  pCfgHdr->version = ver;
  pCfgHdr->componentID = L7_FLEX_QOS_ACL_COMPONENT_ID;
  pCfgHdr->type = L7_CFG_DATA;
  pCfgHdr->length = cfgSize;
  pCfgHdr->dataChanged = L7_FALSE;
}

/*********************************************************************
*
* @purpose  Convert V5 ACL aclId_t structure into a V6
*
* @param    paclIdV5    @b{(input)} version 5 aclId_t data
* @param    paclIdV6    @b{(input)} version 6 aclId_t data
*
* @returns  void
*
* @comments None.
*
* @end
*
*********************************************************************/
static void aclMigrateConfigAclIdDataV5V6Convert(aclIdV5_t *paclIdV5, aclIdV6_t *paclIdV6)
{
  paclIdV6->aclType = paclIdV5->aclType;

  switch(paclIdV6->aclType)
  {
  case L7_ACL_TYPE_NONE:
  case L7_ACL_TYPE_IPV6:
    break;

  case L7_ACL_TYPE_IP:
    paclIdV6->aclId = paclIdV5->aclId;
    break;

  case L7_ACL_TYPE_MAC:
    if ((paclIdV5->aclId+(L7_ACL_MAC_MIN_INDEX-1)) <= L7_ACL_MAC_MAX_INDEX)
    {
      paclIdV6->aclId = paclIdV5->aclId+(L7_ACL_MAC_MIN_INDEX-1);
    }
    else
    {
      paclIdV6->aclType = L7_ACL_TYPE_NONE;
    }
    break;

  default:
    break;
  }
}

/*********************************************************************
*
* @purpose  Convert V3 Config Mask to V4
*
* @param    pCfgMaskV3    @b{(input)} version 3 config mask
* @param    pCfgMaskV4    @b{(output)} ptr to version 4 config mask
*
* @returns  void
*
* @comments None.
*
* @end
*
*********************************************************************/
static void
aclMigrateConfigMaskV3V4Convert(L7_uint32 configMaskV3, L7_uint32 *pCfgMaskV4)
{
  L7_uint32 configMask = 0;

  if (0 != ((1 << ACL_RULENUM_V3) & configMaskV3))
    configMask |= (1 << ACL_RULENUM_V4);

  if (0 != ((1 << ACL_ACTION_V3) & configMaskV3))
    configMask |= (1 << ACL_ACTION_V4);

  if (0 != ((1 << ACL_EVERY_V3) & configMaskV3))
    configMask |= (1 << ACL_EVERY_V4);

  if (0 != ((1 << ACL_PROTOCOL_V3) & configMaskV3))
    configMask |= (1 << ACL_PROTOCOL_V4);

  if (0 != ((1 << ACL_PROTOCOL_MASK_V3) & configMaskV3))
    configMask |= (1 << ACL_PROTOCOL_MASK_V4);

  if (0 != ((1 << ACL_SRCIP_V3) & configMaskV3))
    configMask |= (1 << ACL_SRCIP_V4);

  if (0 != ((1 << ACL_SRCIP_MASK_V3) & configMaskV3))
    configMask |= (1 << ACL_SRCIP_MASK_V4);

  if (0 != ((1 << ACL_SRCPORT_V3) & configMaskV3))
    configMask |= (1 << ACL_SRCPORT_V4);

  if (0 != ((1 << ACL_SRCSTARTPORT_V3) & configMaskV3))
    configMask |= (1 << ACL_SRCSTARTPORT_V4);

  if (0 != ((1 << ACL_SRCENDPORT_V3) & configMaskV3))
    configMask |= (1 << ACL_SRCENDPORT_V4);

  if (0 != ((1 << ACL_DSTIP_V3) & configMaskV3))
    configMask |= (1 << ACL_DSTIP_V4);

  if (0 != ((1 << ACL_DSTIP_MASK_V3) & configMaskV3))
    configMask |= (1 << ACL_DSTIP_MASK_V4);

  if (0 != ((1 << ACL_DSTPORT_V3) & configMaskV3))
    configMask |= (1 << ACL_DSTPORT_V4);

  if (0 != ((1 << ACL_DSTSTARTPORT_V3) & configMaskV3))
    configMask |= (1 << ACL_DSTSTARTPORT_V4);

  if (0 != ((1 << ACL_DSTENDPORT_V3) & configMaskV3))
    configMask |= (1 << ACL_DSTENDPORT_V4);

  if (0 != ((1 << ACL_TOSBYTE_V3) & configMaskV3))
  {
    /* this field was eliminated starting with Rel 4.3, so there is
     * no corresponding bit to set in the V4 configMask
     */
  }

  if (0 != ((1 << ACL_TOSMASK_V3) & configMaskV3))
    configMask |= (1 << ACL_TOSMASK_V4);

  if (0 != ((1 << ACL_IPPREC_V3) & configMaskV3))
    configMask |= (1 << ACL_IPPREC_V4);

  if (0 != ((1 << ACL_IPTOS_V3) & configMaskV3))
    configMask |= (1 << ACL_IPTOS_V4);

  if (0 != ((1 << ACL_IPDSCP_V3) & configMaskV3))
    configMask |= (1 << ACL_IPDSCP_V4);

  *pCfgMaskV4 = configMask;
}

/*********************************************************************
*
* @purpose  Convert V4 IP ACL Config Mask to V5
*
* @param    pCfgMaskV4    @b{(input)} version 4 config mask
* @param    pCfgMaskV5    @b{(output)} ptr to version 5 config mask
*
* @returns  void
*
* @comments None.
*
* @end
*
*********************************************************************/
static void
aclMigrateConfigMaskIpV4V5Convert(L7_uint32 configMaskV4, L7_uint32 *pCfgMaskV5)
{
  L7_uint32 configMask = 0;

  if (0 != ((1 << ACL_RULENUM_V4) & configMaskV4))
    configMask |= (1 << ACL_RULENUM_V5);

  if (0 != ((1 << ACL_ACTION_V4) & configMaskV4))
    configMask |= (1 << ACL_ACTION_V5);

  if (0 != ((1 << ACL_ASSIGN_QUEUEID_V4) & configMaskV4))
    configMask |= (1 << ACL_ASSIGN_QUEUEID_V5);

  if (0 != ((1 << ACL_REDIRECT_INTF_V4) & configMaskV4))
    configMask |= (1 << ACL_REDIRECT_INTF_V5);

  if (0 != ((1 << ACL_EVERY_V4) & configMaskV4))
    configMask |= (1 << ACL_EVERY_V5);

  if (0 != ((1 << ACL_PROTOCOL_V4) & configMaskV4))
    configMask |= (1 << ACL_PROTOCOL_V5);

  if (0 != ((1 << ACL_PROTOCOL_MASK_V4) & configMaskV4))
    configMask |= (1 << ACL_PROTOCOL_MASK_V5);

  if (0 != ((1 << ACL_SRCIP_V4) & configMaskV4))
    configMask |= (1 << ACL_SRCIP_V5);

  if (0 != ((1 << ACL_SRCIP_MASK_V4) & configMaskV4))
    configMask |= (1 << ACL_SRCIP_MASK_V5);

  if (0 != ((1 << ACL_SRCPORT_V4) & configMaskV4))
    configMask |= (1 << ACL_SRCPORT_V5);

  if (0 != ((1 << ACL_SRCSTARTPORT_V4) & configMaskV4))
    configMask |= (1 << ACL_SRCSTARTPORT_V5);

  if (0 != ((1 << ACL_SRCENDPORT_V4) & configMaskV4))
    configMask |= (1 << ACL_SRCENDPORT_V5);

  if (0 != ((1 << ACL_DSTIP_V4) & configMaskV4))
    configMask |= (1 << ACL_DSTIP_V5);

  if (0 != ((1 << ACL_DSTIP_MASK_V4) & configMaskV4))
    configMask |= (1 << ACL_DSTIP_MASK_V5);

  if (0 != ((1 << ACL_DSTPORT_V4) & configMaskV4))
    configMask |= (1 << ACL_DSTPORT_V5);

  if (0 != ((1 << ACL_DSTSTARTPORT_V4) & configMaskV4))
    configMask |= (1 << ACL_DSTSTARTPORT_V5);

  if (0 != ((1 << ACL_DSTENDPORT_V4) & configMaskV4))
    configMask |= (1 << ACL_DSTENDPORT_V5);

  if (0 != ((1 << ACL_TOSMASK_V4) & configMaskV4))
    configMask |= (1 << ACL_TOSMASK_V5);

  if (0 != ((1 << ACL_IPPREC_V4) & configMaskV4))
    configMask |= (1 << ACL_IPPREC_V5);

  if (0 != ((1 << ACL_IPTOS_V4) & configMaskV4))
    configMask |= (1 << ACL_IPTOS_V5);

  if (0 != ((1 << ACL_IPDSCP_V4) & configMaskV4))
    configMask |= (1 << ACL_IPDSCP_V5);

  *pCfgMaskV5 = configMask;
}

/*********************************************************************
*
* @purpose  Convert V4 MAC ACL Config Mask to V5
*
* @param    pCfgMaskV4    @b{(input)} version 4 config mask
* @param    pCfgMaskV5    @b{(output)} ptr to version 5 config mask
*
* @returns  void
*
* @comments None.
*
* @end
*
*********************************************************************/
static void
aclMigrateConfigMaskMacV4V5Convert(L7_uint32 configMaskV4, L7_uint32 *pCfgMaskV5)
{
  L7_uint32 configMask = 0;

  if (0 != ((1 << ACL_MAC_RULENUM_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_RULENUM_V5);

  if (0 != ((1 << ACL_MAC_ACTION_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_ACTION_V5);

  if (0 != ((1 << ACL_MAC_EVERY_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_EVERY_V5);

  if (0 != ((1 << ACL_MAC_ASSIGN_QUEUEID_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_ASSIGN_QUEUEID_V5);

  if (0 != ((1 << ACL_MAC_REDIRECT_INTF_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_REDIRECT_INTF_V5);

  if (0 != ((1 << ACL_MAC_COS_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_COS_V5);

  if (0 != ((1 << ACL_MAC_COS2_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_COS2_V5);

  if (0 != ((1 << ACL_MAC_DSTMAC_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_DSTMAC_V5);

  if (0 != ((1 << ACL_MAC_DSTMAC_MASK_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_DSTMAC_MASK_V5);

  if (0 != ((1 << ACL_MAC_ETYPE_KEYID_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_ETYPE_KEYID_V5);

  if (0 != ((1 << ACL_MAC_ETYPE_VALUE_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_ETYPE_VALUE_V5);

  if (0 != ((1 << ACL_MAC_SRCMAC_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_SRCMAC_V5);

  if (0 != ((1 << ACL_MAC_SRCMAC_MASK_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_SRCMAC_MASK_V5);

  if (0 != ((1 << ACL_MAC_VLANID_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_VLANID_V5);

  if (0 != ((1 << ACL_MAC_VLANID_START_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_VLANID_START_V5);

  if (0 != ((1 << ACL_MAC_VLANID_END_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_VLANID_END_V5);

  if (0 != ((1 << ACL_MAC_VLANID2_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_VLANID2_V5);

  if (0 != ((1 << ACL_MAC_VLANID2_START_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_VLANID2_START_V5);

  if (0 != ((1 << ACL_MAC_VLANID2_END_V4) & configMaskV4))
    configMask |= (1 << ACL_MAC_VLANID2_END_V5);

  *pCfgMaskV5 = configMask;
}

/*********************************************************************
*
* @purpose  Convert V5 IP ACL Config Mask to V6
*
* @param    pCfgMaskV5    @b{(input)} version 5 config mask
* @param    pCfgMaskV6    @b{(output)} ptr to version 6 config mask
*
* @returns  void
*
* @comments None.
*
* @end
*
*********************************************************************/
static void
aclMigrateConfigMaskIpV5V6Convert(L7_uint32 configMaskV5, L7_uint32 *pCfgMaskV6)
{
  L7_uint32 configMask = 0;

  if (0 != ((1 << ACL_RULENUM_V5) & configMaskV5))
    configMask |= (1 << ACL_RULENUM_V6);

  if (0 != ((1 << ACL_ACTION_V5) & configMaskV5))
    configMask |= (1 << ACL_ACTION_V6);

  if (0 != ((1 << ACL_ASSIGN_QUEUEID_V5) & configMaskV5))
    configMask |= (1 << ACL_ASSIGN_QUEUEID_V6);

  if (0 != ((1 << ACL_REDIRECT_INTF_V5) & configMaskV5))
    configMask |= (1 << ACL_REDIRECT_INTF_V6);

  if (0 != ((1 << ACL_MIRROR_INTF_V5) & configMaskV5))
    configMask |= (1 << ACL_MIRROR_INTF_V6);

  if (0 != ((1 << ACL_LOGGING_V5) & configMaskV5))
    configMask |= (1 << ACL_LOGGING_V6);

  if (0 != ((1 << ACL_EVERY_V5) & configMaskV5))
    configMask |= (1 << ACL_EVERY_V6);

  if (0 != ((1 << ACL_PROTOCOL_V5) & configMaskV5))
    configMask |= (1 << ACL_PROTOCOL_V6);

  if (0 != ((1 << ACL_PROTOCOL_MASK_V5) & configMaskV5))
    configMask |= (1 << ACL_PROTOCOL_MASK_V6);

  if (0 != ((1 << ACL_SRCIP_V5) & configMaskV5))
    configMask |= (1 << ACL_SRCIP_V6);

  if (0 != ((1 << ACL_SRCIP_MASK_V5) & configMaskV5))
    configMask |= (1 << ACL_SRCIP_MASK_V6);

  if (0 != ((1 << ACL_SRCPORT_V5) & configMaskV5))
    configMask |= (1 << ACL_SRCPORT_V6);

  if (0 != ((1 << ACL_SRCSTARTPORT_V5) & configMaskV5))
    configMask |= (1 << ACL_SRCSTARTPORT_V6);

  if (0 != ((1 << ACL_SRCENDPORT_V5) & configMaskV5))
    configMask |= (1 << ACL_SRCENDPORT_V6);

  if (0 != ((1 << ACL_DSTIP_V5) & configMaskV5))
    configMask |= (1 << ACL_DSTIP_V6);

  if (0 != ((1 << ACL_DSTIP_MASK_V5) & configMaskV5))
    configMask |= (1 << ACL_DSTIP_MASK_V6);

  if (0 != ((1 << ACL_DSTPORT_V5) & configMaskV5))
    configMask |= (1 << ACL_DSTPORT_V6);

  if (0 != ((1 << ACL_DSTSTARTPORT_V5) & configMaskV5))
    configMask |= (1 << ACL_DSTSTARTPORT_V6);

  if (0 != ((1 << ACL_DSTENDPORT_V5) & configMaskV5))
    configMask |= (1 << ACL_DSTENDPORT_V6);

  if (0 != ((1 << ACL_TOSMASK_V5) & configMaskV5))
    configMask |= (1 << ACL_TOSMASK_V6);

  if (0 != ((1 << ACL_IPPREC_V5) & configMaskV5))
    configMask |= (1 << ACL_IPPREC_V6);

  if (0 != ((1 << ACL_IPTOS_V5) & configMaskV5))
    configMask |= (1 << ACL_IPTOS_V6);

  if (0 != ((1 << ACL_IPDSCP_V5) & configMaskV5))
    configMask |= (1 << ACL_IPDSCP_V6);

  *pCfgMaskV6 = configMask;
}

/*********************************************************************
*
* @purpose  Convert V5 MAC ACL Config Mask to V6
*
* @param    pCfgMaskV5    @b{(input)} version 5 config mask
* @param    pCfgMaskV6    @b{(output)} ptr to version 6 config mask
*
* @returns  void
*
* @comments None.
*
* @end
*
*********************************************************************/
static void
aclMigrateConfigMaskMacV5V6Convert(L7_uint32 configMaskV5, L7_uint32 *pCfgMaskV6)
{
  L7_uint32 configMask = 0;

  if (0 != ((1 << ACL_MAC_RULENUM_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_RULENUM_V6);

  if (0 != ((1 << ACL_MAC_ACTION_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_ACTION_V6);

  if (0 != ((1 << ACL_MAC_ASSIGN_QUEUEID_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_ASSIGN_QUEUEID_V6);

  if (0 != ((1 << ACL_MAC_REDIRECT_INTF_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_REDIRECT_INTF_V6);

  if (0 != ((1 << ACL_MAC_MIRROR_INTF_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_MIRROR_INTF_V6);

  if (0 != ((1 << ACL_MAC_LOGGING_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_LOGGING_V6);

  if (0 != ((1 << ACL_MAC_EVERY_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_EVERY_V6);

  if (0 != ((1 << ACL_MAC_COS_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_COS_V6);

  if (0 != ((1 << ACL_MAC_COS2_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_COS2_V6);

  if (0 != ((1 << ACL_MAC_DSTMAC_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_DSTMAC_V6);

  if (0 != ((1 << ACL_MAC_DSTMAC_MASK_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_DSTMAC_MASK_V6);

  if (0 != ((1 << ACL_MAC_ETYPE_KEYID_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_ETYPE_KEYID_V6);

  if (0 != ((1 << ACL_MAC_ETYPE_VALUE_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_ETYPE_VALUE_V6);

  if (0 != ((1 << ACL_MAC_SRCMAC_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_SRCMAC_V6);

  if (0 != ((1 << ACL_MAC_SRCMAC_MASK_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_SRCMAC_MASK_V6);

  if (0 != ((1 << ACL_MAC_VLANID_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_VLANID_V6);

  if (0 != ((1 << ACL_MAC_VLANID_START_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_VLANID_START_V6);

  if (0 != ((1 << ACL_MAC_VLANID_END_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_VLANID_END_V6);

  if (0 != ((1 << ACL_MAC_VLANID2_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_VLANID2_V6);

  if (0 != ((1 << ACL_MAC_VLANID2_START_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_VLANID2_START_V6);

  if (0 != ((1 << ACL_MAC_VLANID2_END_V5) & configMaskV5))
    configMask |= (1 << ACL_MAC_VLANID2_END_V6);

  *pCfgMaskV6 = configMask;
}


#ifndef PROD_USE_BINARY_CONFIGURATION  
/*********************************************************************
*
* @purpose  Apply config data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @comments None.
*
* @end
*
*********************************************************************/
static L7_RC_t 
aclApplyV6RuleConfigData(aclCfgFileDataV6_t *pCfgV6)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 idx;
  aclCfgDataV6_t *aclData;  

  /* element 0 not used */
  for (idx = 1; idx <= L7_ACL_MAX_LISTS_REL_5_0; idx++)
  {
    aclData = &pCfgV6->cfgParms.aclData[idx];

    switch (aclData->id.aclType)
    {
    case L7_ACL_TYPE_NONE:
      break;

    case L7_ACL_TYPE_IP:
      if (cnfgrIsFeaturePresent(aclCompId_g, L7_ACL_FEATURE_SUPPORTED) == L7_TRUE)
        rc = aclApplyV6ConfigDataIp(aclData);
      break;

    case L7_ACL_TYPE_IPV6:
      if (cnfgrIsFeaturePresent(aclCompId_g, L7_ACL_TYPE_IPV6_FEATURE_ID) == L7_TRUE)
        rc = aclApplyV6ConfigDataIp(aclData);
      break;

    case L7_ACL_TYPE_MAC:
      if (cnfgrIsFeaturePresent(aclCompId_g, L7_ACL_TYPE_MAC_FEATURE_ID) == L7_TRUE)
        rc = aclApplyV6ConfigDataMac(aclData);
      break;

    default:
      /* be lenient here and simply ignore unrecognized ACL types */
      break;
    } /* endswitch */

  } /* endfor */

  return rc;
}

/*********************************************************************
*
* @purpose  Apply config data for IP access lists (of any type)
*
* @param    aclCfgData_t *aclData  access list data pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
static L7_RC_t 
aclApplyV6ConfigDataIp(aclCfgDataV6_t *aclData)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL isIpv6 = L7_FALSE;
  L7_uint32 rule, aclnum;
  L7_uint32 featId;
  aclRuleParmsV6_t *r;

  if ((aclData->id.aclType != L7_ACL_TYPE_IP) &&
      (aclData->id.aclType != L7_ACL_TYPE_IPV6))
    return L7_FAILURE;

  if (aclData->id.aclType == L7_ACL_TYPE_IPV6)
    isIpv6 = L7_TRUE;

  aclnum = aclData->id.aclId;
  if (aclnum != 0)
  {
    /* active acl */
    rc=aclCreate(aclnum);
    if (rc == L7_SUCCESS)
    {
      if (isIpv6 == L7_TRUE)
      {
        /* must set the IPV6 ACL name field before creating any rules */
        rc=aclNameAdd(aclnum, aclData->aclName);
      }

      for (rule = L7_ACL_MIN_RULE_NUM; rule <= L7_ACL_MAX_RULE_NUM_REL_5_0; rule++)
      {
        if (aclData->ruleData[rule].ip.ruleNum != 0)
        {
          r = &(aclData->ruleData[rule].ip);

          rc=aclRuleActionAdd(aclnum, rule, r->action);

          if ( ((r->configMask) & (1 << ACL_ASSIGN_QUEUEID)) != 0 )
          {
            featId = L7_ACL_ASSIGN_QUEUE_FEATURE_ID;
            if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              rc=aclRuleAssignQueueIdAdd(aclnum, rule, r->assignQueueId);
          }

          if ( ((r->configMask) & (1 << ACL_REDIRECT_INTF)) != 0 )
          {
            featId = L7_ACL_REDIRECT_FEATURE_ID;
            if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              rc=aclRuleRedirectConfigIdAdd(aclnum, rule, &r->redirectConfigId);
          }

          if ( ((r->configMask) & (1 << ACL_MIRROR_INTF)) != 0 )
          {
            featId = L7_ACL_MIRROR_FEATURE_ID;
            if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              rc=aclRuleMirrorConfigIdAdd(aclnum, rule, &r->mirrorConfigId);
          }

          if ( ((r->configMask) & (1 << ACL_LOGGING)) != 0 )
          {
            if (aclImpLoggingIsAllowed(r->action) == L7_TRUE)  /* action-specific feature check */
              rc=aclRuleLoggingAdd(aclnum, rule, r->logging);
          }

          if ( ((r->configMask) & (1 << ACL_EVERY)) != 0)
          {
            if (r->every == L7_TRUE)
            {
              featId = L7_ACL_RULE_MATCH_EVERY_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclRuleEveryAdd(aclnum, rule, r->every);
            }
          }

          else
          {
            if ( ((r->configMask) & (1 << ACL_DSTIP)) != 0)
            {
              if (isIpv6 == L7_FALSE)
              {
                featId = L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID;
                if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                {
                  featId = L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID;
                  if ((cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE) ||
                      (r->dstIp.v4.mask == (L7_uint32)L7_IP_SUBMASK_EXACT_MATCH_MASK))
                    rc=aclRuleDstIpMaskAdd(aclnum, rule, r->dstIp.v4.addr, r->dstIp.v4.mask);
                }
              }
            }

            if ( ((r->configMask) & (1 << ACL_DSTIPV6)) != 0)
            {
              if (isIpv6 == L7_TRUE)
              {
                featId = L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID;
                if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                  rc=aclRuleDstIpv6AddrAdd(aclnum, rule, &r->dstIp.v6);
              }
            }

            if ( ((r->configMask) & (1 << ACL_DSTPORT)) != 0)
            {
              if (isIpv6 == L7_TRUE)
                featId = L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_FEATURE_ID;
              else
                featId = L7_ACL_RULE_MATCH_DSTL4PORT_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclRuleDstL4PortAdd(aclnum, rule, r->dstPort);
            }

            if ( ((r->configMask) & (1 << ACL_DSTSTARTPORT)) != 0)
            {
              if (isIpv6 == L7_TRUE)
                featId = L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_RANGE_FEATURE_ID;
              else
                featId = L7_ACL_RULE_MATCH_DSTL4PORT_RANGE_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclRuleDstL4PortRangeAdd(aclnum, rule, r->dstStartPort, r->dstEndPort);
            }

            if ( ((r->configMask) & (1 << ACL_FLOWLBLV6)) != 0)
            {
              if (isIpv6 == L7_TRUE)
              {
                featId = L7_ACL_RULE_MATCH_IPV6_FLOWLBL_FEATURE_ID;
                if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                  rc=aclRuleIpv6FlowLabelAdd(aclnum, rule, r->flowlbl);
              }
            }

            if ( ((r->configMask) & (1 << ACL_IPDSCP)) != 0)
            {
              if (isIpv6 == L7_TRUE)
                featId = L7_ACL_RULE_MATCH_IPV6_IPDSCP_FEATURE_ID;
              else
                featId = L7_ACL_RULE_MATCH_IPDSCP_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclRuleIPDscpAdd(aclnum, rule, (L7_uint32)(r->tosbyte >> L7_QOS_ACL_TLV_MATCH_IPDSCP_SHIFT));
            }

            if ( ((r->configMask) & (1 << ACL_IPPREC)) != 0)
            {
              if (isIpv6 == L7_FALSE)
              {
                featId = L7_ACL_RULE_MATCH_IPPRECEDENCE_FEATURE_ID;
                if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                  rc=aclRuleIPPrecedenceAdd(aclnum, rule, (L7_uint32)(r->tosbyte >> L7_QOS_ACL_TLV_MATCH_IPPRECEDENCE_SHIFT));
              }
            }

            if ( ((r->configMask) & (1 << ACL_IPTOS)) != 0)
            {
              if (isIpv6 == L7_FALSE)
              {
                featId = L7_ACL_RULE_MATCH_IPTOS_FEATURE_ID;
                if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                  rc=aclRuleIPTosAdd(aclnum, rule, (L7_uint32)r->tosbyte, (L7_uint32)r->tosmask);
              }
            }

            if ( ((r->configMask) & (1 << ACL_PROTOCOL)) != 0 )
            {
              if (isIpv6 == L7_TRUE)
                featId = L7_ACL_RULE_MATCH_IPV6_PROTOCOL_FEATURE_ID;
              else
                featId = L7_ACL_RULE_MATCH_PROTOCOL_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclRuleProtocolAdd(aclnum, rule, (L7_uint32)r->protocol);
            }

            if ( ((r->configMask) & (1 << ACL_SRCIP)) != 0)
            {
              if (isIpv6 == L7_FALSE)
              {
                featId = L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID;
                if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                {
                  featId = L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID;
                  if ((cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE) ||
                      (r->srcIp.v4.mask == (L7_uint32)L7_IP_SUBMASK_EXACT_MATCH_MASK))
                    rc=aclRuleSrcIpMaskAdd(aclnum, rule, r->srcIp.v4.addr, r->srcIp.v4.mask);
                }
              }
            }

            if ( ((r->configMask) & (1 << ACL_SRCIPV6)) != 0)
            {
              if (isIpv6 == L7_TRUE)
              {
                featId = L7_ACL_RULE_MATCH_IPV6_SRCIP_FEATURE_ID;
                if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                  rc=aclRuleSrcIpv6AddrAdd(aclnum, rule, &r->srcIp.v6);
              }
            }

            if ( ((r->configMask) & (1 << ACL_SRCPORT)) != 0)
            {
              if (isIpv6 == L7_TRUE)
                featId = L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_FEATURE_ID;
              else
                featId = L7_ACL_RULE_MATCH_SRCL4PORT_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclRuleSrcL4PortAdd(aclnum, rule, r->srcPort);
            }

            if ( ((r->configMask) & (1 << ACL_SRCSTARTPORT)) != 0 )
            {
              if (isIpv6 == L7_TRUE)
                featId = L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_RANGE_FEATURE_ID;
              else
                featId = L7_ACL_RULE_MATCH_SRCL4PORT_RANGE_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclRuleSrcL4PortRangeAdd(aclnum, rule, r->srcStartPort, r->srcEndPort);
            }
          }
        }
      } /* endfor rule */
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Apply config data for MAC access lists
*
* @param    aclCfgData_t *aclData  access list data pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
static L7_RC_t 
aclApplyV6ConfigDataMac(aclCfgDataV6_t *aclData)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 hostMacMask[L7_MAC_ADDR_LEN] = {0xff,0xff,0xff,0xff,0xff,0xff};
  L7_uint32 rule, aclIndex;
  L7_uint32 featId;
  aclMacRuleParmsV6_t *r;

  if (aclData->id.aclType != L7_ACL_TYPE_MAC)
    return L7_FAILURE;

  aclIndex = aclData->id.aclId;
  if (aclIndex != 0)
  {
    /* active acl */
    rc=aclMacCreate(aclIndex);
    if (rc == L7_SUCCESS)
    {
      /* must set the MAC ACL name field before creating any rules */
      rc=aclMacNameAdd(aclIndex, aclData->aclName);

      for (rule=L7_ACL_MIN_RULE_NUM; rule<=L7_ACL_MAX_RULE_NUM_REL_5_0; rule++)
      {
        if (aclData->ruleData[rule].mac.ruleNum != 0)
        {
          r = &(aclData->ruleData[rule].mac);

          rc=aclMacRuleActionAdd(aclIndex, rule, r->action);

          if ( ((r->configMask) & (1 << ACL_MAC_ASSIGN_QUEUEID)) != 0 )
          {
            featId = L7_ACL_ASSIGN_QUEUE_FEATURE_ID;
            if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              rc=aclMacRuleAssignQueueIdAdd(aclIndex, rule, r->assignQueueId);
          }

          if ( ((r->configMask) & (1 << ACL_MAC_REDIRECT_INTF)) != 0 )
          {
            featId = L7_ACL_REDIRECT_FEATURE_ID;
            if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              rc=aclMacRuleRedirectConfigIdAdd(aclIndex, rule, &r->redirectConfigId);
          }

          if ( ((r->configMask) & (1 << ACL_MAC_MIRROR_INTF)) != 0 )
          {
            featId = L7_ACL_MIRROR_FEATURE_ID;
            if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              rc=aclMacRuleMirrorConfigIdAdd(aclIndex, rule, &r->mirrorConfigId);
          }

          if ( ((r->configMask) & (1 << ACL_MAC_LOGGING)) != 0 )
          {
            if (aclImpLoggingIsAllowed(r->action) == L7_TRUE)  /* action-specific feature check */
              rc=aclMacRuleLoggingAdd(aclIndex, rule, r->logging);
          }

          if ( ((r->configMask) & (1 << ACL_MAC_EVERY)) != 0)
          {
            if (r->every == L7_TRUE)
            {
              featId = L7_ACL_RULE_MATCH_EVERY_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleEveryAdd(aclIndex, rule, r->every);
            }
          }

          else
          {
            if ( ((r->configMask) & (1 << ACL_MAC_COS)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_COS_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleCosAdd(aclIndex, rule, (L7_uint32)r->cos);
            }

            if ( ((r->configMask) & (1 << ACL_MAC_COS2)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_COS2_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleCos2Add(aclIndex, rule, (L7_uint32)r->cos2);
            }

            if ( ((r->configMask) & (1 << ACL_MAC_DSTMAC)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_DSTMAC_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              {
                featId = L7_ACL_RULE_MATCH_DSTMAC_MASK_FEATURE_ID;
                if ((cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE) ||
                    (memcmp(r->dstMacMask, hostMacMask, sizeof(hostMacMask)) == 0))
                  rc=aclMacRuleDstMacAdd(aclIndex, rule, r->dstMac, r->dstMacMask);
              }
            }

            if ( ((r->configMask) & (1 << ACL_MAC_ETYPE_KEYID)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_ETYPE_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleEtypeKeyAdd(aclIndex, rule, r->etypeKeyId, r->etypeValue);
            }

            if ( ((r->configMask) & (1 << ACL_MAC_SRCMAC)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_SRCMAC_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              {
                featId = L7_ACL_RULE_MATCH_SRCMAC_MASK_FEATURE_ID;
                if ((cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE) ||
                    (memcmp(r->srcMacMask, hostMacMask, sizeof(hostMacMask)) == 0))
                  rc=aclMacRuleSrcMacAdd(aclIndex, rule, r->srcMac, r->srcMacMask);
              }
            }

            if ( ((r->configMask) & (1 << ACL_MAC_VLANID)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_VLANID_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleVlanIdAdd(aclIndex, rule, r->vlanId);
            }

            if ( ((r->configMask) & (1 << ACL_MAC_VLANID_START)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_VLANID_RANGE_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleVlanIdRangeAdd(aclIndex, rule, r->vlanIdStart, r->vlanIdEnd);
            }

            if ( ((r->configMask) & (1 << ACL_MAC_VLANID2)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_VLANID2_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleVlanId2Add(aclIndex, rule, r->vlanId2);
            }

            if ( ((r->configMask) & (1 << ACL_MAC_VLANID2_START)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_VLANID2_RANGE_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleVlanId2RangeAdd(aclIndex, rule, r->vlanId2Start, r->vlanId2End);
            }
          }
        }
      } /* endfor rule */
    }
  }

  return rc;
}
#endif


/* QSCAN: DELETE -- Unit test function */
#include "osapi.h"
L7_RC_t acl_test_defaults()
{
  L7_uint32 i, bufSize, cfgSize, ver;
  L7_uchar8 *buf;
  L7_fileHdr_t *p;

  bufSize = sizeof(aclCfgFileData_t) * 2;
  buf = osapiMalloc(L7_FLEX_QOS_ACL_COMPONENT_ID, bufSize);
  if (buf == L7_NULLPTR)
  {
    printf("\nOut of Memory!\n\n");
    return L7_FAILURE;
  }

  for (i = ACL_CFG_VER_1; i <= ACL_CFG_VER_CURRENT+1; i++)
  {
    memset(buf, 0, bufSize);

    switch (i)
    {
    case 1:  ver = ACL_CFG_VER_1; cfgSize = sizeof(aclCfgFileDataV1_t); break;
    case 2:  ver = ACL_CFG_VER_2; cfgSize = sizeof(aclCfgFileDataV2_t); break;
    case 3:  ver = ACL_CFG_VER_3; cfgSize = sizeof(aclCfgFileDataV3_t); break;
    case 4:  ver = ACL_CFG_VER_4; cfgSize = sizeof(aclCfgFileDataV4_t); break;
    case 5:  ver = ACL_CFG_VER_5; cfgSize = sizeof(aclCfgFileDataV5_t); break;
    case 6:  ver = ACL_CFG_VER_6; cfgSize = sizeof(aclCfgFileDataV6_t); break;

    case 7:  ver = i; cfgSize = sizeof(aclCfgFileData_t); break;

    default:
      return L7_FAILURE;
      /*passthru*/
    }

    aclBuildDefaultConfigDataVer(buf, cfgSize, ver);

    p = (L7_fileHdr_t *)buf;

    printf("\nVersion %u Cfg Header:\n", ver);
    printf("\tfilename      : %s\n", p->filename);
    printf("\tversion       : %u\n", p->version);
    printf("\tcomponentID   : %u\n", p->componentID);
    printf("\ttype          : %u\n", p->type);
    printf("\tlength        : %u\n", p->length);
    printf("\tdataChanged   : %u\n", p->dataChanged);
    printf("\tsavePointId   : %u\n", p->savePointId);
    printf("\ttargetDevice  : %u\n", p->targetDevice);
    printf("\tfileHdrVersion: %u\n", p->fileHdrVersion);
    printf("\n\n");
  }

  printf("Max intf count:  4_4 = %u  Cur = %u\n\n", 
         L7_ACL_INTF_MAX_COUNT_REL_4_4, L7_ACL_INTF_MAX_COUNT);

  return L7_SUCCESS;
}
