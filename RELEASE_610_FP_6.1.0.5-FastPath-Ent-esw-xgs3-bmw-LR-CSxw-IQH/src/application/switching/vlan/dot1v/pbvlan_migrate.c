/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename pbvlan_migrate.c
*
* @purpose pbvlan Configuration Migration
*
* @component pbvlan
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#include "pbvlan_migrate.h"

static L7_RC_t pbvlanMigrateConfigV1V2Convert (pbVlanCfgDataV1_t * pCfgV1,
                                               pbVlanCfgDataV2_t * pCfgV2);

static L7_RC_t pbvlanMigrateConfigV2V3Convert (pbVlanCfgDataV2_t * pCfgV2,
                                               pbVlanCfgDataV3_t * pCfgV3);

static L7_RC_t pbvlanMigrateConfigV3V4Convert (pbVlanCfgDataV3_t * pCfgV3,
                                               pbVlanCfgDataV4_t * pCfgV4);

static void pbVlanBuildDefaultConfigDataV2 (pbVlanCfgDataV2_t * pCfgV2);
static void pbVlanBuildDefaultConfigDataV3 (pbVlanCfgDataV3_t * pCfgV3);
static void pbVlanBuildDefaultConfigDataV4 (pbVlanCfgDataV4_t * pCfgV4);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static pbVlanCfgData_t *pCfgCur = L7_NULLPTR;


extern L7_RC_t sysapiCfgFileMigrateMaskRel4_2To4_3 (NIM_INTF_MASK_REL_4_2_t *srcMask, 
                                                    NIM_INTF_MASK_REL_4_3_t *destMask);

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
pbvlanMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  pbVlanCfgDataV1_t *pCfgV1 = L7_NULLPTR;
  pbVlanCfgDataV2_t *pCfgV2 = L7_NULLPTR;
  pbVlanCfgDataV3_t *pCfgV3 = L7_NULLPTR;
  pbVlanCfgDataV4_t *pCfgV4 = L7_NULLPTR;

  pCfgCur = (pbVlanCfgData_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case PBVLAN_CFG_VER_1:
    pCfgV1 = (pbVlanCfgDataV1_t *) osapiMalloc (L7_PBVLAN_COMPONENT_ID,
                                                (L7_uint32) sizeof (pbVlanCfgDataV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (pbVlanCfgDataV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = PBVLAN_CFG_VER_1;
    break;

  case PBVLAN_CFG_VER_2:
    pCfgV2 = (pbVlanCfgDataV2_t *) osapiMalloc (L7_PBVLAN_COMPONENT_ID,
                                                (L7_uint32) sizeof (pbVlanCfgDataV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (pbVlanCfgDataV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = PBVLAN_CFG_VER_2;
    break;

  case PBVLAN_CFG_VER_3:
    pCfgV3 = (pbVlanCfgDataV3_t *) osapiMalloc (L7_PBVLAN_COMPONENT_ID,
                                                (L7_uint32) sizeof (pbVlanCfgDataV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (pbVlanCfgDataV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = PBVLAN_CFG_VER_3;
    break;

  case PBVLAN_CFG_VER_4:
    pCfgV4 = (pbVlanCfgDataV4_t *) osapiMalloc (L7_PBVLAN_COMPONENT_ID,
                                                (L7_uint32) sizeof (pbVlanCfgDataV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (pbVlanCfgDataV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = PBVLAN_CFG_VER_4;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_PBVLAN_COMPONENT_ID,
                                         PBVLAN_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PBVLAN_COMPONENT_ID,
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
    case PBVLAN_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (pbVlanCfgDataV2_t *) osapiMalloc (L7_PBVLAN_COMPONENT_ID,
                                                  sizeof (pbVlanCfgDataV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_NULLPTR == pCfgV1) || (L7_SUCCESS != pbvlanMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_PBVLAN_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case PBVLAN_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (pbVlanCfgDataV3_t *) osapiMalloc (L7_PBVLAN_COMPONENT_ID,
                                                  sizeof (pbVlanCfgDataV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_NULLPTR == pCfgV2) || (L7_SUCCESS != pbvlanMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_PBVLAN_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case PBVLAN_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (pbVlanCfgDataV4_t *) osapiMalloc (L7_PBVLAN_COMPONENT_ID,
                                                  sizeof (pbVlanCfgDataV4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_NULLPTR == pCfgV3) || (L7_SUCCESS != pbvlanMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_PBVLAN_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case PBVLAN_CFG_VER_4:
	  if (pCfgV4 != L7_NULLPTR)
	  {
		/* pCfgV4 has already been allocated and filled in */
		memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV4, sizeof (*pCfgCur));
	    osapiFree (L7_PBVLAN_COMPONENT_ID, pCfgV4);
	  }
	  
      pCfgV4 = L7_NULLPTR;

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
    osapiFree (L7_PBVLAN_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_PBVLAN_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_PBVLAN_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_PBVLAN_COMPONENT_ID, pCfgV4);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    pbVlanBuildDefaultConfigData (PBVLAN_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V1 to V2.
*
* @param    pCfgV1    @b{(input)} ptr to version 1 config data structure
* @param    pCfgV2        @b{(input)} ptr to version 2 config data structure
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
pbvlanMigrateConfigV1V2Convert (pbVlanCfgDataV1_t * pCfgV1, pbVlanCfgDataV2_t * pCfgV2)
{
  L7_uint32 i, count;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != PBVLAN_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, PBVLAN_CFG_VER_1);
    return L7_FAILURE;
  }

  pbVlanBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  count = min (L7_PBVLAN_MAX_NUM_GROUPS_REL_4_0, L7_PBVLAN_MAX_NUM_GROUPS_REL_4_1);
  for (i = 0; i <= count; i++)
  {
    strcpy (pCfgV2->group[i].name, pCfgV1->group[i].name);
    pCfgV2->group[i].inUse = pCfgV1->group[i].inUse;
    pCfgV2->group[i].groupID = pCfgV1->group[i].groupID;
    pCfgV2->group[i].vlanid = pCfgV1->group[i].vlanid;
    MIGRATE_COPY_STRUCT (pCfgV2->group[i].protocol, pCfgV1->group[i].protocol);

    MIGRATE_COPY_NIM_INTF_MASK (pCfgV2->group[i].ports, pCfgV1->group[i].ports);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V2 to V3.
*
* @param    pCfgV2    @b{(input)} ptr to version 2 config data structure
* @param    pCfgV3        @b{(input)} ptr to version 3 config data structure
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
pbvlanMigrateConfigV2V3Convert (pbVlanCfgDataV2_t * pCfgV2, pbVlanCfgDataV3_t * pCfgV3)
{
  L7_uint32 i, count;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != PBVLAN_CFG_VER_2 && pCfgV2->cfgHdr.version != PBVLAN_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, PBVLAN_CFG_VER_2);
    return L7_FAILURE;
  }

  pbVlanBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  count = min (L7_PBVLAN_MAX_NUM_GROUPS_REL_4_3, L7_PBVLAN_MAX_NUM_GROUPS_REL_4_1);
  for (i = 0; i <= count; i++)
  {
    strcpy (pCfgV3->group[i].name, pCfgV2->group[i].name);
    pCfgV3->group[i].inUse = pCfgV2->group[i].inUse;
    pCfgV3->group[i].groupID = pCfgV2->group[i].groupID;
    pCfgV3->group[i].vlanid = pCfgV2->group[i].vlanid;
    MIGRATE_COPY_STRUCT (pCfgV3->group[i].protocol, pCfgV2->group[i].protocol);


    sysapiCfgFileMigrateMaskRel4_2To4_3((NIM_INTF_MASK_REL_4_2_t *)&pCfgV2->group[i].ports,
                                        (NIM_INTF_MASK_REL_4_3_t *)&pCfgV3->group[i].ports);
#if 0
    MIGRATE_COPY_NIM_INTF_MASK (pCfgV3->group[i].ports, pCfgV2->group[i].ports);
#endif
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
pbvlanMigrateConfigV3V4Convert (pbVlanCfgDataV3_t * pCfgV3, pbVlanCfgDataV4_t * pCfgV4)
{
  L7_uint32 i, count;

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != PBVLAN_CFG_VER_3 &&
      pCfgV3->cfgHdr.version != PBVLAN_CFG_VER_2 &&
      pCfgV3->cfgHdr.version != PBVLAN_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, PBVLAN_CFG_VER_3);
    return L7_FAILURE;
  }

  pbVlanBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  count = min (L7_PBVLAN_MAX_NUM_GROUPS_REL_4_4, L7_PBVLAN_MAX_NUM_GROUPS_REL_4_3);
  for (i = 0; i <= count; i++)
  {
    strcpy (pCfgV4->group[i].name, pCfgV3->group[i].name);
    pCfgV4->group[i].inUse = pCfgV3->group[i].inUse;
    pCfgV4->group[i].groupID = pCfgV3->group[i].groupID;
    pCfgV4->group[i].vlanid = pCfgV3->group[i].vlanid;
    MIGRATE_COPY_STRUCT (pCfgV4->group[i].protocol, pCfgV3->group[i].protocol);
    MIGRATE_COPY_NIM_INTF_MASK(pCfgV4->group[i].ports, pCfgV3->group[i].ports);
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
pbVlanBuildDefaultConfigDataV2 (pbVlanCfgDataV2_t * pCfgV2)
{
  memset (pCfgV2, 0, sizeof (*pCfgV2));

  /* Build header */
  strcpy ((char *) &pCfgV2->cfgHdr.filename, PBVLAN_CFG_FILENAME);
  pCfgV2->cfgHdr.version = PBVLAN_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_PBVLAN_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = (L7_uint32) sizeof (*pCfgV2);
  pCfgV2->cfgHdr.dataChanged = L7_FALSE;

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
pbVlanBuildDefaultConfigDataV3 (pbVlanCfgDataV3_t * pCfgV3)
{
  bzero((char*)pCfgV3, sizeof(*pCfgV3));

  /* Build header */
  strcpy((char *)&pCfgV3->cfgHdr.filename, PBVLAN_CFG_FILENAME);
  pCfgV3->cfgHdr.version = PBVLAN_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_PBVLAN_COMPONENT_ID;
  pCfgV3->cfgHdr.type = L7_CFG_DATA;
  pCfgV3->cfgHdr.length = (L7_uint32)sizeof(*pCfgV3);
  pCfgV3->cfgHdr.dataChanged = L7_FALSE;
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
pbVlanBuildDefaultConfigDataV4 (pbVlanCfgDataV4_t * pCfgV4)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (pbVlanCfgDataV4_t));
  pbVlanBuildDefaultConfigData (PBVLAN_CFG_VER_4);
  memcpy ((L7_uchar8 *) pCfgV4, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}
