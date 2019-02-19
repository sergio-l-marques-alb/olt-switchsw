
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename vrrp_migrate.c
*
* @purpose VRRP Configuration Migration
*
* @component VRRP
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#include "vrrp_migrate.h"

static L7_RC_t vrrpMigrateConfigV1V2Convert (L7_vrrpMapCfgV1_t * pCfgV1,
                                             L7_vrrpMapCfgV2_t * pCfgV2);
static L7_RC_t vrrpMigrateConfigV2V3Convert (L7_vrrpMapCfgV2_t * pCfgV2,
                                             L7_vrrpMapCfgV3_t * pCfgV3);
static void vrrpBuildDefaultConfigDataV2 (L7_vrrpMapCfgV2_t * pCfgV2);
static void vrrpBuildDefaultConfigDataV3 (L7_vrrpMapCfgV3_t * pCfgV3);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static L7_vrrpMapCfg_t *pCfgCur = L7_NULLPTR;

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
vrrpMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  L7_vrrpMapCfgV1_t *pCfgV1 = L7_NULLPTR;
  L7_vrrpMapCfgV2_t *pCfgV2 = L7_NULLPTR;
  L7_vrrpMapCfgV3_t *pCfgV3 = L7_NULLPTR;


  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;


  pCfgCur = (L7_vrrpMapCfg_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case L7_VRRP_CFG_VER_1:
    pCfgV1 = (L7_vrrpMapCfgV1_t *) osapiMalloc (L7_VRRP_MAP_COMPONENT_ID,
                                                (L7_uint32) sizeof (L7_vrrpMapCfgV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (L7_vrrpMapCfgV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = L7_VRRP_CFG_VER_1;
    break;

  case L7_VRRP_CFG_VER_2:
    pCfgV2 = (L7_vrrpMapCfgV2_t *) osapiMalloc (L7_VRRP_MAP_COMPONENT_ID,
                                                (L7_uint32) sizeof (L7_vrrpMapCfgV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (L7_vrrpMapCfgV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = L7_VRRP_CFG_VER_2;
    break;

  case L7_VRRP_CFG_VER_3:
    pCfgV3 = (L7_vrrpMapCfgV3_t *) osapiMalloc (L7_VRRP_MAP_COMPONENT_ID,
                                                (L7_uint32) sizeof (L7_vrrpMapCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (L7_vrrpMapCfgV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = L7_VRRP_CFG_VER_3;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_VRRP_MAP_COMPONENT_ID,
                                         L7_VRRP_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_VRRP_MAP_COMPONENT_ID,
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
    case L7_VRRP_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (L7_vrrpMapCfgV2_t *) osapiMalloc (L7_VRRP_MAP_COMPONENT_ID,
                                                  (L7_uint32) sizeof (L7_vrrpMapCfgV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_SUCCESS != vrrpMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_VRRP_MAP_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case L7_VRRP_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (L7_vrrpMapCfgV3_t *) osapiMalloc (L7_VRRP_MAP_COMPONENT_ID,
                                                  (L7_uint32) sizeof (L7_vrrpMapCfgV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_SUCCESS != vrrpMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_VRRP_MAP_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case L7_VRRP_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV3, sizeof (*pCfgCur));
      osapiFree (L7_VRRP_MAP_COMPONENT_ID, pCfgV3);
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
    osapiFree (L7_VRRP_MAP_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_VRRP_MAP_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_VRRP_MAP_COMPONENT_ID, pCfgV3);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    vrrpBuildDefaultConfigData (L7_VRRP_CFG_VER_CURRENT);
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
vrrpMigrateConfigV1V2Convert (L7_vrrpMapCfgV1_t * pCfgV1, L7_vrrpMapCfgV2_t * pCfgV2)
{
  L7_uint32 i;
  L7_RC_t rc;
  L7_uint32 slot;
  L7_uint32 port;
  L7_uint32 intIfIndex;
  nimConfigID_t configId;
  vrrpCfgCktV1_t *pCktCfgV1;
  vrrpVirtRouterCfgV2_t *pRtrCfgV2;
  L7_uint32 ifConfigIndex;

  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != L7_VRRP_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, L7_VRRP_CFG_VER_1);
    return L7_FAILURE;
  }

  vrrpBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV2->rtr, pCfgV1->rtr);

  /* L7_VRRP_MAX_VIRT_ROUTERS is same in REL_G and DEV_I */
  intIfIndex = 0; 
  ifConfigIndex = 0;
  for (i = 0; i < L7_VRRP_MAX_VIRT_ROUTERS_REL_4_0; i++)
  {
    pCktCfgV1 = &pCfgV1->ckt[i];
    slot = pCktCfgV1->usp.slot;
    port = pCktCfgV1->usp.port;
    rc = sysapiCfgFileRel4_0SlotPortToIntfInfoGet (slot, port, &configId, &intIfIndex, L7_NULL);
    if (L7_SUCCESS != rc)
    {
      if (L7_ERROR == rc)
        MIGRATE_INVALID_SLOT_PORT (slot, port);
      continue;
    }

    if (ifConfigIndex >= L7_VRRP_MAX_VIRT_ROUTERS_REL_4_1)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (L7_VRRP_CFG_FILENAME, ifConfigIndex);
      break;
    }

    pRtrCfgV2 = &pCfgV2->virtRouter[ifConfigIndex];
    (void) nimConfigIdCopy (&configId, &pRtrCfgV2->configId);
    pRtrCfgV2->vrid = pCktCfgV1->vrid;
    pRtrCfgV2->priority = pCktCfgV1->priority;
    pRtrCfgV2->preempt_mode = pCktCfgV1->preempt_mode;
    pRtrCfgV2->adver_int = pCktCfgV1->adver_int;
    pRtrCfgV2->auth_types = pCktCfgV1->auth_types;
    MIGRATE_COPY_STRUCT (pRtrCfgV2->auth_data, pCktCfgV1->auth_data);
    pRtrCfgV2->ipaddress = pCktCfgV1->ipaddress;
    pRtrCfgV2->vr_start_state = pCktCfgV1->vr_start_state;

    ifConfigIndex++;
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
vrrpMigrateConfigV2V3Convert (L7_vrrpMapCfgV2_t * pCfgV2, L7_vrrpMapCfgV3_t * pCfgV3)
{
  L7_uint32 i;
  vrrpVirtRouterCfgV2_t *pRtrCfgV2;
  vrrpVirtRouterCfgV3_t *pRtrCfgV3;
  L7_uint32 ifConfigIndex;

  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != L7_VRRP_CFG_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, L7_VRRP_CFG_VER_2);
    return L7_FAILURE;
  }

  vrrpBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV3->rtr, pCfgV2->rtr);


  ifConfigIndex = 1;
  /* L7_VRRP_MAX_VIRT_ROUTERS is same in REL_G and DEV_I */
  for (i = 0; i < L7_VRRP_MAX_VIRT_ROUTERS_REL_4_2; i++)
  {

    if (ifConfigIndex > L7_VRRP_MAX_VIRT_ROUTERS_REL_4_3)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (L7_VRRP_CFG_FILENAME, ifConfigIndex);
      break;
    }

    pRtrCfgV2 = &pCfgV2->virtRouter[i];
    pRtrCfgV3 = &pCfgV3->virtRouter[i];
    MIGRATE_COPY_STRUCT (pRtrCfgV3->configId, pRtrCfgV2->configId);
    pRtrCfgV3->vrid = pRtrCfgV2->vrid;
    pRtrCfgV3->priority = pRtrCfgV2->priority;
    pRtrCfgV3->preempt_mode = pRtrCfgV2->preempt_mode;
    pRtrCfgV3->adver_int = pRtrCfgV2->adver_int;
    pRtrCfgV3->auth_types = pRtrCfgV2->auth_types;
    MIGRATE_COPY_STRUCT (pRtrCfgV3->auth_data, pRtrCfgV2->auth_data);
    pRtrCfgV3->ipaddress[0] = pRtrCfgV2->ipaddress;
    pRtrCfgV3->vr_start_state = pRtrCfgV2->vr_start_state;


    ifConfigIndex++;
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
vrrpBuildDefaultConfigDataV2 (L7_vrrpMapCfgV2_t * pCfgV2)
{
  L7_uint32 k;
  L7_uint32 max;                /* max configuration indices */

  memset (pCfgV2, 0, sizeof (*pCfgV2));

  strcpy (pCfgV2->cfgHdr.filename, L7_VRRP_CFG_FILENAME);
  pCfgV2->cfgHdr.version = L7_VRRP_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_VRRP_MAP_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = sizeof (*pCfgV2);
  pCfgV2->cfgHdr.dataChanged = L7_FALSE;

  /* generic VRRP cfg */
  pCfgV2->rtr.vrrpAdminMode = FD_VRRP_DEFAULT_ADMIN_MODE;

  /* VRRP router interface configuration parameters */
  for (max = 0; max < L7_VRRP_MAX_VIRT_ROUTERS_REL_4_3; max++)
  {
    pCfgV2->virtRouter[max].vrid = FD_VRRP_DEFAULT_VRID;
    pCfgV2->virtRouter[max].priority = FD_VRRP_DEFAULT_PRIORITY;
    pCfgV2->virtRouter[max].preempt_mode = FD_VRRP_DEFAULT_PREEMPT;
    pCfgV2->virtRouter[max].adver_int = FD_VRRP_DEFAULT_ADVERTISEMENT_INTERVAL;
    pCfgV2->virtRouter[max].auth_types = FD_VRRP_DEFAULT_AUTH_TYPE;
    pCfgV2->virtRouter[max].vr_start_state = FD_VRRP_DEFAULT_START_STATE;

    pCfgV2->virtRouter[max].ipaddress = FD_VRRP_DEFAULT_IP_ADDRESS;
    for (k = 0; k < L7_VRRP_MAX_AUTH_DATA; k++)
      pCfgV2->virtRouter[max].auth_data[k] = FD_VRRP_DEFAULT_AUTH_DATA_KEY_CHAR;
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
vrrpBuildDefaultConfigDataV3 (L7_vrrpMapCfgV3_t * pCfgV3)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (L7_vrrpMapCfgV3_t));
  vrrpBuildDefaultConfigData (L7_VRRP_CFG_VER_3);
  memcpy ((L7_uchar8 *) pCfgV3, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}
