
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename dot1q_migrate.c
*
* @purpose dot1q Configuration Migration
*
* @component dot1q
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include "commdefs.h"
#define DOT1D_PRIORITY_TABLE FACTORY_DEFAULT_REFER

#include "dot1q_common.h"
#include "dot1q_cfg.h"
#include "dot1q_migrate.h"

static L7_RC_t dot1qMigrateConfigV2V3Convert (dot1qCfgDataV2_t * pCfgV2, dot1qCfgDataV3_t * pCfgV3);
static L7_RC_t dot1qMigrateConfigV3V4Convert (dot1qCfgDataV3_t * pCfgV3, dot1qCfgDataV4_t * pCfgV4);
static L7_RC_t dot1qMigrateConfigV4V5Convert (dot1qCfgDataV4_t * pCfgV4, dot1qCfgDataV5_t * pCfgV5);
static L7_RC_t dot1qMigrateConfigV5V6Convert (dot1qCfgDataV5_t * pCfgV5, dot1qCfgDataV6_t * pCfgV6);

static void dot1qBuildDefaultConfigDataV3 (dot1qCfgDataV3_t * pCfgV3);
static void dot1qBuildDefaultConfigDataV4 (dot1qCfgDataV4_t * pCfgV4);
static void dot1qBuildDefaultConfigDataV5 (dot1qCfgDataV5_t * pCfgV5);
static void dot1qBuildDefaultConfigDataV6 (dot1qCfgDataV6_t * pCfgV6);

static DOT1Q_VLAN_MASK_REL_4_X_t dot1qRel4_0VlanIntfMask;

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static dot1qCfgData_t *pCfgCur = L7_NULLPTR;


extern L7_RC_t sysapiCfgFileMigrateMaskRel4_2To4_3 (NIM_INTF_MASK_REL_4_2_t *srcMask,
                                                    NIM_INTF_MASK_REL_4_3_t *destMask);
extern L7_RC_t sysapiCfgFileRel4_0VlanIntfGet(L7_uint32 mappingIndex, L7_uint32 *vlanId);

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
dot1qMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  dot1qCfgDataV2_t *pCfgV2 = L7_NULLPTR;
  dot1qCfgDataV3_t *pCfgV3 = L7_NULLPTR;
  dot1qCfgDataV4_t *pCfgV4 = L7_NULLPTR;
  dot1qCfgDataV5_t *pCfgV5 = L7_NULLPTR;
  dot1qCfgDataV6_t *pCfgV6 = L7_NULLPTR;


  pCfgCur = (dot1qCfgData_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION(oldVer, ver, sizeof (*pCfgCur));

  if (pCfgHdr->version == DOT1Q_CFG_VER_2 && pCfgHdr->length == sizeof (dot1qCfgDataV3_t))
    oldVer = DOT1Q_CFG_VER_3;

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case DOT1Q_CFG_VER_2:
    pCfgV2 = (dot1qCfgDataV2_t *) osapiMalloc (L7_DOT1Q_COMPONENT_ID,
                                               (L7_uint32) sizeof (dot1qCfgDataV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (dot1qCfgDataV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = DOT1Q_CFG_VER_2;
    break;

  case DOT1Q_CFG_VER_3:
    pCfgV3 = (dot1qCfgDataV3_t *) osapiMalloc (L7_DOT1Q_COMPONENT_ID,
                                               (L7_uint32) sizeof (dot1qCfgDataV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (dot1qCfgDataV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = DOT1Q_CFG_VER_2;        /* b'cos: REL_H version was not updated */
    break;

  case DOT1Q_CFG_VER_4:
    pCfgV4 = (dot1qCfgDataV4_t *) osapiMalloc (L7_DOT1Q_COMPONENT_ID,
                                               (L7_uint32) sizeof (dot1qCfgDataV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (dot1qCfgDataV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = DOT1Q_CFG_VER_4;
    break;

  case DOT1Q_CFG_VER_5:
    pCfgV5 = (dot1qCfgDataV5_t *) osapiMalloc (L7_DOT1Q_COMPONENT_ID,
                                               (L7_uint32) sizeof (dot1qCfgDataV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (dot1qCfgDataV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = DOT1Q_CFG_VER_5;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_DOT1Q_COMPONENT_ID,
                                         DOT1Q_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DOT1Q_COMPONENT_ID,
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
    case DOT1Q_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (dot1qCfgDataV3_t *) osapiMalloc (L7_DOT1Q_COMPONENT_ID, sizeof (dot1qCfgDataV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_NULLPTR == pCfgV2) || (L7_SUCCESS != dot1qMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT1Q_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case DOT1Q_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (dot1qCfgDataV4_t *) osapiMalloc (L7_DOT1Q_COMPONENT_ID, sizeof (dot1qCfgDataV4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_NULLPTR == pCfgV3) ||  (L7_SUCCESS != dot1qMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT1Q_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case DOT1Q_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (dot1qCfgDataV5_t *) osapiMalloc (L7_DOT1Q_COMPONENT_ID, sizeof (dot1qCfgDataV5_t));
      if ((L7_NULLPTR == pCfgV5) || (L7_NULLPTR == pCfgV4) || (L7_SUCCESS != dot1qMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_DOT1Q_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case DOT1Q_CFG_VER_5:
      /* pCfgV5 has already been allocated and filled in */
        pCfgV6 = (dot1qCfgDataV6_t *) osapiMalloc (L7_DOT1Q_COMPONENT_ID, sizeof (dot1qCfgDataV6_t));
        if ((L7_NULLPTR == pCfgV6) || (L7_NULLPTR == pCfgV5) || (L7_SUCCESS != dot1qMigrateConfigV5V6Convert (pCfgV5, pCfgV6)))
        {
          buildDefault = L7_TRUE;
          break;
        }
        osapiFree (L7_DOT1Q_COMPONENT_ID, pCfgV5);
      pCfgV5 = L7_NULLPTR;
        /*passthru */


    case DOT1Q_CFG_VER_6:
      /* pCfgV6 has already been allocated and filled in */
      if (pCfgV6 != L7_NULLPTR)
      {
        memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV6, sizeof (*pCfgCur));
        osapiFree (L7_DOT1Q_COMPONENT_ID, pCfgV6);
      }
      pCfgV6 = L7_NULLPTR;

      /* done with migration - Flag unsaved configuration */
      pCfgCur->hdr.dataChanged = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_DOT1Q_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_DOT1Q_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_DOT1Q_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_DOT1Q_COMPONENT_ID, pCfgV5);
  if (pCfgV6 != L7_NULLPTR)
    osapiFree (L7_DOT1Q_COMPONENT_ID, pCfgV6);



  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1Q_COMPONENT_ID,
            "Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    dot1qBuildDefaultConfigData (DOT1Q_CFG_VER_CURRENT);
    pCfgCur->hdr.dataChanged = L7_TRUE;
  }

  return;
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
dot1qMigrateConfigV2V3Convert (dot1qCfgDataV2_t * pCfgV2, dot1qCfgDataV3_t * pCfgV3)
{
  L7_RC_t rc;
  L7_uint32 i, count, ifCfgIndex;
  nimConfigID_t configId;

  /* verify correct version of old config file */
  if (pCfgV2->hdr.version != DOT1Q_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->hdr.version, DOT1Q_CFG_VER_2);
    return L7_FAILURE;
  }

  dot1qBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  count = min (L7_MAX_VLANS_REL_4_1, L7_MAX_VLANS_REL_4_0);
  for (i = 0; i < count; i++)
  {
    if (L7_FALSE == pCfgV2->cfg.Qvlan[i].inUse)
      continue;
    pCfgV3->Qvlan[i].inUse = pCfgV2->cfg.Qvlan[i].inUse;
    pCfgV3->Qvlan[i].vlanID = pCfgV2->cfg.Qvlan[i].vlanID;
    strcpy (pCfgV3->Qvlan[i].name, pCfgV2->cfg.Qvlan[i].name);
    pCfgV3->Qvlan[i].fdbID = pCfgV2->cfg.Qvlan[i].fdbID;
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV3->Qvlan[i].staticEgressPorts,
                                pCfgV2->cfg.Qvlan[i].staticEgressPorts);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV3->Qvlan[i].forbiddenEgressPorts,
                                pCfgV2->cfg.Qvlan[i].forbiddenEgressPorts);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV3->Qvlan[i].taggedSet, pCfgV2->cfg.Qvlan[i].taggedSet);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV3->Qvlan[i].groupFilter.forwardAllMask,
                                pCfgV2->cfg.Qvlan[i].groupFilter.forwardAllMask);
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV3->Qvlan[i].groupFilter.forwardUnregMask,
                                pCfgV2->cfg.Qvlan[i].groupFilter.forwardUnregMask);
    pCfgV3->Qvlan[i].flags = pCfgV2->cfg.Qvlan[i].flags;

  }

  /*  Copy the Vlan interface mask from the data migrated from IP MAP */
  MIGRATE_COPY_STRUCT(pCfgV3->QvlanIntf,dot1qRel4_0VlanIntfMask);

  count = min (L7_MAX_INTERFACE_COUNT_REL_4_1, L7_MAX_INTERFACE_COUNT_REL_4_0);
  ifCfgIndex = 1;
  for (i = 1; i <= count - 1; i++)
  {
    rc = sysapiCfgFileRel4_0IndexToIntfInfoGet (i, &configId, L7_NULL, L7_NULL);
    if (L7_SUCCESS != rc)
    {
      if (L7_ERROR == rc)
        MIGRATE_INVALID_INDEX (i);
      continue;
    }

    if (dot1qIntfIsValidType (configId.type) == L7_FALSE)
      continue;

    (void) nimConfigIdCopy (&configId, &pCfgV3->Qports[ifCfgIndex].configId);

    MIGRATE_COPY_STRUCT (pCfgV3->Qports[ifCfgIndex].VID, pCfgV2->cfg.Qports[i].VID);
    pCfgV3->Qports[ifCfgIndex].acceptFrameType = pCfgV2->cfg.Qports[i].acceptFrameType;
    pCfgV3->Qports[ifCfgIndex].enableIngressFiltering =
      pCfgV2->cfg.Qports[i].enableIngressFiltering;
    MIGRATE_COPY_STRUCT (pCfgV3->Qports[ifCfgIndex].priority, pCfgV2->cfg.Qports[i].priority);
    pCfgV3->Qports[ifCfgIndex].gvrp_enabled = pCfgV2->cfg.Qports[i].gvrp_enabled;
    pCfgV3->Qports[ifCfgIndex].gmrp_enabled = pCfgV2->cfg.Qports[i].gmrp_enabled;
    pCfgV3->Qports[ifCfgIndex].portCapabilities = pCfgV2->cfg.Qports[i].portCapabilities;
    ifCfgIndex++;
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
void dot1qMigrateRel4_0VlanIntfMask(L7_uint32 min_vlan_intIfNum)
{

    L7_uint32 i;
    L7_uint32 vlanId;
    L7_uint32 mappingIndex;

    memset((L7_uchar8 *)&dot1qRel4_0VlanIntfMask, 0, sizeof(dot1qRel4_0VlanIntfMask));

    for (i = 0; i <= L7_MAX_NUM_VLAN_INTF_REL_4_0; i++)
    {
        /* Pass the release 4_0 internal interface and get the VLAN ID associated
           with that interface.  That VLAN ID is then used to populate the VLAN interface
           mask used in subsequent releases. */
      mappingIndex = min_vlan_intIfNum + i - 1;

      if (sysapiCfgFileRel4_0VlanIntfGet(mappingIndex, &vlanId) == L7_SUCCESS)
      {
           L7_VLAN_SETMASKBIT(dot1qRel4_0VlanIntfMask, vlanId);
      }

    }

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
dot1qMigrateConfigV3V4Convert (dot1qCfgDataV3_t * pCfgV3, dot1qCfgDataV4_t * pCfgV4)
{
  L7_uint32 i, j, count;

  /* verify correct version of old config file */
  if (pCfgV3->hdr.version != DOT1Q_CFG_VER_3 && pCfgV3->hdr.version != DOT1Q_CFG_VER_2) /* b'cos: REL_H version was not updated */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->hdr.version, DOT1Q_CFG_VER_3);
    return L7_FAILURE;
  }

  dot1qBuildDefaultConfigDataV4 (pCfgV4);

  count = min (L7_DOT1Q_MAX_INTF_REL_4_1, L7_DOT1Q_MAX_INTF_REL_4_3);
  for (i = 0; i < count; i++)
  {

      (void) nimConfigIdCopy (&pCfgV3->Qports[i].configId, &pCfgV4->Qports[i].configId);
      for (j = 0; j < DOT1Q_MAX_PVID; j++)
      {
          pCfgV4->Qports[i].VID[j] = pCfgV3->Qports[i].VID[j];
      }
      pCfgV4->Qports[i].acceptFrameType = pCfgV3->Qports[i].acceptFrameType;
      pCfgV4->Qports[i].enableIngressFiltering = pCfgV3->Qports[i].enableIngressFiltering;
      pCfgV4->Qports[i].priority.NumTrafficClasses = pCfgV3->Qports[i].priority.NumTrafficClasses;
      pCfgV4->Qports[i].priority.DefaultUserPriority = pCfgV3->Qports[i].priority.DefaultUserPriority;
      memcpy(pCfgV4->Qports[i].priority.Mapping, pCfgV3->Qports[i].priority.Mapping,
             sizeof(pCfgV3->Qports[i].priority.Mapping));
      pCfgV4->Qports[i].gvrp_enabled = pCfgV3->Qports[i].gvrp_enabled;
      pCfgV4->Qports[i].gmrp_enabled = pCfgV3->Qports[i].gmrp_enabled;
      pCfgV4->Qports[i].portCapabilities = pCfgV3->Qports[i].portCapabilities;

  }


  count = min (L7_MAX_VLANS_REL_4_1, L7_MAX_VLANS_REL_4_3);
  for (i = 0; i < count; i++)
  {
    if (L7_FALSE == pCfgV3->Qvlan[i].inUse)
      continue;
    pCfgV4->Qvlan[i].inUse = pCfgV3->Qvlan[i].inUse;
    pCfgV4->Qvlan[i].vlanID = pCfgV3->Qvlan[i].vlanID;
    pCfgV4->Qvlan[i].fdbID = pCfgV3->Qvlan[i].fdbID;
    pCfgV4->Qvlan[i].flags = pCfgV3->Qvlan[i].flags;
    strcpy (pCfgV4->Qvlan[i].name, pCfgV3->Qvlan[i].name);

    /* Copying the mask */

     sysapiCfgFileMigrateMaskRel4_2To4_3 ((NIM_INTF_MASK_REL_4_2_t *)&pCfgV3->Qvlan[i].staticEgressPorts,
                                          (NIM_INTF_MASK_REL_4_3_t *)&pCfgV4->Qvlan[i].staticEgressPorts);


     sysapiCfgFileMigrateMaskRel4_2To4_3 ( (NIM_INTF_MASK_REL_4_2_t *)&pCfgV3->Qvlan[i].forbiddenEgressPorts,
                                           (NIM_INTF_MASK_REL_4_3_t *)&pCfgV4->Qvlan[i].forbiddenEgressPorts);


     sysapiCfgFileMigrateMaskRel4_2To4_3((NIM_INTF_MASK_REL_4_2_t *)&pCfgV3->Qvlan[i].taggedSet,
                                         (NIM_INTF_MASK_REL_4_3_t *)&pCfgV4->Qvlan[i].taggedSet);


     sysapiCfgFileMigrateMaskRel4_2To4_3 ( (NIM_INTF_MASK_REL_4_2_t *)&pCfgV3->Qvlan[i].groupFilter.forwardAllMask,
                                           (NIM_INTF_MASK_REL_4_3_t *)&pCfgV4->Qvlan[i].groupFilter.forwardAllMask);


     sysapiCfgFileMigrateMaskRel4_2To4_3((NIM_INTF_MASK_REL_4_2_t *)&pCfgV3->Qvlan[i].groupFilter.forwardUnregMask,
                                         (NIM_INTF_MASK_REL_4_3_t *)&pCfgV4->Qvlan[i].groupFilter.forwardUnregMask);

  }

  MIGRATE_COPY_STRUCT (pCfgV4->QvlanIntf, pCfgV3->QvlanIntf);

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
dot1qMigrateConfigV4V5Convert (dot1qCfgDataV4_t * pCfgV4, dot1qCfgDataV5_t * pCfgV5)
{
  L7_uint32 i, j, count;

  /* verify correct version of old config file */
  if (pCfgV4->hdr.version != DOT1Q_CFG_VER_4 &&
      pCfgV4->hdr.version != DOT1Q_CFG_VER_3 &&
      pCfgV4->hdr.version != DOT1Q_CFG_VER_2) /* b'cos: REL_H version was not updated */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->hdr.version, DOT1Q_CFG_VER_4);
    return L7_FAILURE;
  }

  dot1qBuildDefaultConfigDataV5 (pCfgV5);

  /*---------------------------*/
  /* Migrate Qglobal Structure */
  /*---------------------------*/

  MIGRATE_COPY_STRUCT (pCfgV5->Qglobal, pCfgV4->Qglobal);


  /*-------------------------*/
  /* Migrate Qports Structure*/
  /*-------------------------*/

  count = min (L7_DOT1Q_MAX_INTF_REL_4_3, L7_DOT1Q_MAX_INTF_REL_4_4);

  for (i = 0; i < count; i++)
  {
      /* In this release, configuration was restriced to only participating interfaces */
      if (dot1qIntfIsValidType(pCfgV4->Qports[i].configId.type)  != L7_TRUE)
          continue;

      MIGRATE_COPY_STRUCT (pCfgV5->Qports[i].configId, pCfgV4->Qports[i].configId);

      /* Note if DOT1Q_MAX_PVID changes from 4 to another number in portNProtocolVid_t
       * we have to accomodate for it with pertinent release defines
       * for example DOT1Q_MAX_PVID_REL_4_5 etc to differnetiate the new one
       */
      for (j = 0; j < DOT1Q_MAX_PVID; j++)
          pCfgV5->Qports[i].portCfg.VID[j] = pCfgV4->Qports[i].VID[j];

      pCfgV5->Qports[i].portCfg.acceptFrameType = pCfgV4->Qports[i].acceptFrameType;
      pCfgV5->Qports[i].portCfg.enableIngressFiltering = pCfgV4->Qports[i].enableIngressFiltering;

      MIGRATE_COPY_STRUCT (pCfgV5->Qports[i].portCfg.priority,
                           pCfgV4->Qports[i].priority);

      /* Note:  In release 4.4, the following parameters were moved to garp config:
            - gvrp_enabled;
            -  gmrp_enabled;

        The following parameter was removed from configuration completely:
             portCapabilities;
      */
  }


  /*-------------------------*/
  /* Migrate Qvlan  Structure*/
  /*-------------------------*/

  count = min (L7_MAX_VLANS_REL_4_3, L7_MAX_VLANS_REL_4_4);
  for (i = 0; i < count; i++)
  {
    if (L7_FALSE == pCfgV4->Qvlan[i].inUse)
      continue;

    /*
    The following parameter was removed from configuration completely
    due to not being used:
         inUse;
         fdbID;
         groupFilter
         flags

    */
    pCfgV5->Qvlan[i].vlanId = pCfgV4->Qvlan[i].vlanID;
    strcpy (pCfgV5->Qvlan[i].vlanCfg.name, pCfgV4->Qvlan[i].name);

    MIGRATE_COPY_NIM_INTF_MASK(pCfgV5->Qvlan[i].vlanCfg.staticEgressPorts,
                               pCfgV4->Qvlan[i].staticEgressPorts);
    MIGRATE_COPY_NIM_INTF_MASK(pCfgV5->Qvlan[i].vlanCfg.forbiddenEgressPorts,
                               pCfgV4->Qvlan[i].forbiddenEgressPorts);
    MIGRATE_COPY_NIM_INTF_MASK(pCfgV5->Qvlan[i].vlanCfg.taggedSet,
                               pCfgV4->Qvlan[i].taggedSet);


#if defined(DOT1Q_FUTURE_FUNC_GROUP_FILTER) && DOT1Q_FUTURE_FUNC_GROUP_FILTER
    MIGRATE_COPY_NIM_INTF_MASK(pCfgV5->Qvlan[i].groupFilter.forwardAllMask,
                               pCfgV4->Qvlan[i].groupFilter.forwardAllMask);
    MIGRATE_COPY_NIM_INTF_MASK(pCfgV5->Qvlan[i].groupFilter.forwardUnregMask,
                               pCfgV4->Qvlan[i].groupFilter.forwardUnregMask);
#endif
  }


  /*------------------------------*/
  /* Migrate QvlanIntf  Structure */
  /*------------------------------*/
  MIGRATE_COPY_STRUCT (pCfgV5->QvlanIntf, pCfgV4->QvlanIntf);

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
dot1qMigrateConfigV5V6Convert (dot1qCfgDataV5_t * pCfgV5, dot1qCfgDataV6_t * pCfgV6)
{
  L7_uint32 i, count;
  L7_uint32 size;

  /* verify correct version of old config file */
  if (pCfgV5->hdr.version != DOT1Q_CFG_VER_5 &&
      pCfgV5->hdr.version != DOT1Q_CFG_VER_4 &&
      pCfgV5->hdr.version != DOT1Q_CFG_VER_3 &&
      pCfgV5->hdr.version != DOT1Q_CFG_VER_2) /* b'cos: REL_H version was not updated */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV5->hdr.version, DOT1Q_CFG_VER_5);
    return L7_FAILURE;
  }

  dot1qBuildDefaultConfigDataV6 (pCfgV6);

  /*---------------------------*/
  /* Migrate Qglobal Structure */
  /*---------------------------*/

  MIGRATE_COPY_STRUCT (pCfgV6->Qglobal, pCfgV5->Qglobal);


  /*-------------------------*/
  /* Migrate Qports Structure*/
  /*-------------------------*/
  MIGRATE_COPY_STRUCT(pCfgV6->Qports,  pCfgV5->Qports);


  /*-------------------------*/
  /* Migrate Qvlan  Structure*/
  /*-------------------------*/
  count = min(L7_MAX_VLANS,  L7_MAX_VLANS_REL_4_4);
  /* The arrays are 1 more than that max num of vlans */
  count++;
  size = min(sizeof(NIM_INTF_MASK_REL_4_4_t), sizeof(NIM_INTF_MASK_t));
  for (i = 0; i < count; i++)
  {
    pCfgV6->Qvlan[i].vlanId = pCfgV5->Qvlan[i].vlanId;
    memcpy(pCfgV6->Qvlan[i].vlanCfg.name , pCfgV5->Qvlan[i].vlanCfg.name, L7_MAX_VLAN_NAME);
    memcpy(&pCfgV6->Qvlan[i].vlanCfg.staticEgressPorts.value[0],
           &pCfgV5->Qvlan[i].vlanCfg.staticEgressPorts.value[0],
           size);
    memcpy(&pCfgV6->Qvlan[i].vlanCfg.taggedSet.value[0],
           &pCfgV5->Qvlan[i].vlanCfg.taggedSet.value[0],
           size);
    memcpy(&pCfgV6->Qvlan[i].vlanCfg.forbiddenEgressPorts.value[0],
           &pCfgV5->Qvlan[i].vlanCfg.forbiddenEgressPorts.value[0],
           size);
    #if defined(DOT1Q_FUTURE_FUNC_GROUP_FILTER) && DOT1Q_FUTURE_FUNC_GROUP_FILTER
    memcpy(&pCfgV6->Qvlan[i].vlanCfg.groupFilter.forwardAllMask.value[0],
           &pCfgV5->Qvlan[i].vlanCfg.groupFilter.forwardAllMask.value[0],
           size);
    memcpy(&pCfgV6->Qvlan[i].vlanCfg.groupFilter.forwardUnregMask.value[0],
           &pCfgV5->Qvlan[i].vlanCfg.groupFilter.forwardUnregMask.value[0],
           size);
    #endif
  }


  /*------------------------------*/
  /* Migrate QvlanIntf  Structure */
  /*------------------------------*/
  /* This is the only thing that changed between version 5 and version 6 */
  count = min(DOT1Q_VLAN_INDICES_REL_4_X, L7_VLAN_INDICES);

  for (i = 0; i < count; i++)
  {
    pCfgV6->QvlanIntf.value[i] = pCfgV5->QvlanIntf.value[i];
  }

  /*------------------------------*/
  /* Migrate traceFlags           */
  /*------------------------------*/
  pCfgV6->traceFlags = pCfgV5->traceFlags;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build Version 3 defaults
*
* @param    pCfgV3    @b{(input)} ptr to version 3 config data structure
*
* @returns  none
*
* @notes    This needs to re-written when new version added
*
* @end
*********************************************************************/
static void
dot1qBuildDefaultConfigDataV3 (dot1qCfgDataV3_t * pCfgV3)
{
  L7_uint32 i;
  dot1qIntfCfgDataV3_t *pCfg;
  dot1qVlanCfgDataV3_t *vCfg;

  memset ((L7_uchar8 *) pCfgV3, 0, sizeof (dot1qCfgDataV3_t));

  vCfg = &pCfgV3->Qvlan[0];
  vCfg->inUse = L7_TRUE;
  vCfg->vlanID = L7_DOT1Q_DEFAULT_VLAN;
  vCfg->fdbID = L7_DOT1Q_DEFAULT_VLAN;
  strcpy ((char *) &vCfg->name, "Default");

  for (i = 1; i < L7_DOT1Q_MAX_INTF_REL_4_1; i++)
  {
    pCfg = &pCfgV3->Qports[i];
    pCfg->VID[DOT1Q_DEFAULT_PVID] = L7_DOT1Q_DEFAULT_VLAN;
    pCfg->acceptFrameType = L7_DOT1Q_ADMIT_ALL;
    pCfg->enableIngressFiltering = L7_DISABLE;
    pCfg->gmrp_enabled = L7_DISABLE;
    pCfg->gvrp_enabled = L7_DISABLE;
    pCfg->priority.DefaultUserPriority = L7_DOT1P_DEFAULT_USER_PRIORITY;
    pCfg->priority.NumTrafficClasses = L7_DOT1P_NUM_TRAFFIC_CLASSES;
    memcpy(pCfg->priority.Mapping,
           dot1dUserPriorityRegenTable[L7_DOT1P_NUM_TRAFFIC_CLASSES-1],
           sizeof(dot1dUserPriorityRegenTable[L7_DOT1P_NUM_TRAFFIC_CLASSES-1]));

  }

  strcpy ((char *) pCfgV3->hdr.filename, DOT1Q_CFG_FILENAME);
  pCfgV3->hdr.version = DOT1Q_CFG_VER_3;
  pCfgV3->hdr.componentID = L7_DOT1Q_COMPONENT_ID;
  pCfgV3->hdr.type = L7_CFG_DATA;
  pCfgV3->hdr.length = (L7_uint32) sizeof (dot1qCfgDataV3_t);
  pCfgV3->hdr.dataChanged = L7_FALSE;

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
dot1qBuildDefaultConfigDataV4 (dot1qCfgDataV4_t * pCfgV4)
{
  dot1qVlanCfgDataV4_t *vCfg;
  dot1qIntfCfgDataV4_t *pCfg;
  L7_uint32 i;

  memset((void*)pCfgV4, 0, sizeof(dot1qCfgDataV4_t));

  /* build header */
  strcpy((char*)pCfgV4->hdr.filename, DOT1Q_CFG_FILENAME);
  pCfgV4->hdr.version = DOT1Q_CFG_VER_4;
  pCfgV4->hdr.componentID = L7_DOT1Q_COMPONENT_ID;
  pCfgV4->hdr.type = L7_CFG_DATA;
  pCfgV4->hdr.length = (L7_uint32)sizeof(*pCfgV4);
  pCfgV4->hdr.dataChanged = L7_FALSE;

  vCfg = &pCfgV4->Qvlan[0];
  vCfg->inUse = L7_TRUE;
  vCfg->vlanID = L7_DOT1Q_DEFAULT_VLAN;
  vCfg->fdbID = L7_DOT1Q_DEFAULT_VLAN;
  strcpy( (char*)&vCfg->name, "Default");

  /* build dot1p global priority defaults */
  pCfgV4->Qglobal.priority.NumTrafficClasses = L7_DOT1P_NUM_TRAFFIC_CLASSES;
  pCfgV4->Qglobal.priority.DefaultUserPriority = L7_DOT1P_DEFAULT_USER_PRIORITY;
  memcpy(pCfgV4->Qglobal.priority.Mapping,
         dot1dUserPriorityRegenTable[L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT-1],
         sizeof(dot1dUserPriorityRegenTable[L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT-1]));

  for (i = 1; i < L7_DOT1Q_MAX_INTF_REL_4_3; i++)
  {
    pCfg = &pCfgV4->Qports[i];
    pCfg->VID[DOT1Q_DEFAULT_PVID] = L7_DOT1Q_DEFAULT_VLAN;
    pCfg->acceptFrameType = L7_DOT1Q_ADMIT_ALL;
    pCfg->enableIngressFiltering = L7_DISABLE;
    pCfg->gmrp_enabled = L7_DISABLE;
    pCfg->gvrp_enabled = L7_DISABLE;
    pCfg->priority.DefaultUserPriority = L7_DOT1P_DEFAULT_USER_PRIORITY;
    pCfg->priority.NumTrafficClasses = L7_DOT1P_NUM_TRAFFIC_CLASSES;
    memcpy(pCfg->priority.Mapping,
           dot1dUserPriorityRegenTable[L7_DOT1P_NUM_TRAFFIC_CLASSES-1],
           sizeof(dot1dUserPriorityRegenTable[L7_DOT1P_NUM_TRAFFIC_CLASSES-1]));

  }
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
dot1qBuildDefaultConfigDataV5 (dot1qCfgDataV5_t * pCfgV5)
{
  dot1qVlanCfgDataV5_t *vCfg;
  dot1qIntfCfgData_t *pCfg;
  L7_uint32 i;

  memset((void*)pCfgV5, 0, sizeof(dot1qCfgDataV5_t));

  /* build header */
  strcpy((char*)pCfgV5->hdr.filename, DOT1Q_CFG_FILENAME);
  pCfgV5->hdr.version = DOT1Q_CFG_VER_5;
  pCfgV5->hdr.componentID = L7_DOT1Q_COMPONENT_ID;
  pCfgV5->hdr.type = L7_CFG_DATA;
  pCfgV5->hdr.length = (L7_uint32)sizeof(dot1qCfgDataV5_t);
  pCfgV5->hdr.dataChanged = L7_FALSE;

  /* Initialize the default VLAN */

  vCfg = &pCfgV5->Qvlan[0];

  vCfg->vlanId = FD_DOT1Q_DEFAULT_VLAN;
  strcpy( (char*)&vCfg->vlanCfg.name, FD_DOT1Q_DEFAULT_VLAN_NAME);

  /* build dot1p global priority defaults */
  pCfgV5->Qglobal.priority.NumTrafficClasses   = FD_DOT1P_NUM_TRAFFIC_CLASSES;
  pCfgV5->Qglobal.priority.DefaultUserPriority = FD_DOT1P_DEFAULT_USER_PRIORITY;
  memcpy(pCfgV5->Qglobal.priority.Mapping,
         dot1dUserPriorityRegenTable[L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT-1],
         sizeof(dot1dUserPriorityRegenTable[L7_DOT1P_NUM_TRAFFIC_CLASSES_INIT-1]));


  for (i = 1; i < L7_DOT1Q_MAX_INTF_REL_4_4; i++)
  {
    pCfg = &pCfgV5->Qports[i];

    pCfg->portCfg.VID[DOT1Q_DEFAULT_PVID] = FD_DOT1Q_DEFAULT_PVID;
    pCfg->portCfg.acceptFrameType         = L7_DOT1Q_ADMIT_ALL;
    pCfg->portCfg.enableIngressFiltering  = FD_DOT1Q_DEFAULT_INGRESS_FILTER_PORT;
    /* copy from dot1p global values to establish interface defaults */
    memcpy(&pCfg->portCfg.priority, &pCfgV5->Qglobal.priority, sizeof(dot1p_prio_t));

    /* Set this port to pariticipate in default vlan */
    NIM_INTF_SETMASKBIT(vCfg->vlanCfg.staticEgressPorts, i);
    NIM_INTF_CLRMASKBIT(vCfg->vlanCfg.taggedSet, i);
  }
  /* By default no vlan interfaces are configured */

  return;



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
dot1qBuildDefaultConfigDataV6 (dot1qCfgDataV6_t * pCfgV6)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (dot1qCfgDataV6_t));
  dot1qBuildDefaultConfigData (DOT1Q_CFG_VER_6);
  memcpy ((L7_uchar8 *) pCfgV6, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}

void dot1qDebugSizes()
{
  printf("sizeof dot1qCfgV5_t %lu \n", (L7_ulong32)sizeof(dot1qCfgV5_t));
  printf("sizeof dot1qGlobCfgData_t %lu \n", (L7_ulong32)sizeof(dot1qGlobCfgData_t));
  printf("sizeof dot1qIntfCfgData_t %lu \n", (L7_ulong32)sizeof(dot1qIntfCfgData_t));
  printf("sizeof dot1qVlanCfgDataV5_t %lu \n", (L7_ulong32)sizeof (dot1qVlanCfgDataV5_t));
  printf("sizeof dot1q_vlan_cfgV5_t %lu \n",  (L7_ulong32)sizeof(dot1q_vlan_cfgV5_t));
  printf("sizeof DOT1Q_VLAN_MASK_REL_4_X_t %lu \n", (L7_ulong32)sizeof(DOT1Q_VLAN_MASK_REL_4_X_t));
  printf("L7_DOT1Q_MAX_INTF_REL_4_4 %d\n", L7_DOT1Q_MAX_INTF_REL_4_4);
  printf("L7_MAX_VLANS_REL_4_4 %d\n", L7_MAX_VLANS_REL_4_4);
  printf("sizeof NIM_INTF_MASK_REL_4_4_t %lu\n", (L7_ulong32)sizeof(NIM_INTF_MASK_REL_4_4_t));
  printf("version 6 numbers \n");
  printf("sizeof dot1qCfgData_t %lu \n", (L7_ulong32)sizeof(dot1qCfgData_t));
  printf("sizeof dot1qVlanCfgData_t %lu\n", (L7_ulong32)sizeof(dot1qVlanCfgData_t));
  printf("sizeof dot1q_vlan_cfg_t %lu\n",  (L7_ulong32)sizeof(dot1q_vlan_cfg_t));
  printf("sizeof L7_VLAN_MASK_t %lu\n",  (L7_ulong32)sizeof(L7_VLAN_MASK_t));
  printf("DOT1Q_INTF_MAX_COUNT %d\n", DOT1Q_INTF_MAX_COUNT);
  printf("L7_MAX_VLANS %d\n", L7_MAX_VLANS);
  printf("sizeof NIM_INTF_MASK_t %lu\n", (L7_ulong32)sizeof(NIM_INTF_MASK_t));

  return;
}

