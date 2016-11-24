
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename ospf_migrate.c
*
* @purpose OSPF Configuration Migration
*
* @component OSPF
*
* @comments  none
*
* @create  08/24/2004
*
* @author  Rama Sasthri, Kristipati 
* @end
*
**********************************************************************/

#include "ospf_migrate.h"
#include "ip_config.h"
#include "ipmap_migrate.h"

static L7_RC_t ospfMigrateConfigV4V5Convert (L7_ospfMapCfgV4_t * pCfgV4,
                                             L7_ospfMapCfgV5_t * pCfgV5);
static L7_RC_t ospfMigrateConfigV5V6Convert (L7_ospfMapCfgV5_t * pCfgV5,
                                             L7_ospfMapCfgV6_t * pCfgV6);
static L7_RC_t ospfMigrateConfigV6V7Convert (L7_ospfMapCfgV6_t * pCfgV6,
                                             L7_ospfMapCfgV7_t * pCfgV7);
static L7_RC_t ospfMigrateConfigV7V8Convert (L7_ospfMapCfgV7_t * pCfgV7,
                                             L7_ospfMapCfgV8_t * pCfgV8);
static L7_RC_t ospfMigrateConfigV8V9Convert (L7_ospfMapCfgV8_t * pCfgV8,
                                             L7_ospfMapCfgV9_t * pCfgV9);
static void ospfBuildDefaultConfigDataV5 (L7_ospfMapCfgV5_t * pCfgV5);
static void ospfBuildDefaultConfigDataV6 (L7_ospfMapCfgV6_t * pCfgV6);
static void ospfBuildDefaultConfigDataV7 (L7_ospfMapCfgV7_t * pCfgV7);
static void ospfBuildDefaultConfigDataV8 (L7_ospfMapCfgV8_t * pCfgV8);
static void ospfBuildDefaultConfigDataV9 (L7_ospfMapCfgV9_t * pCfgV9);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static L7_ospfMapCfg_t *pCfgCur = L7_NULLPTR;

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
ospfMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  L7_ospfMapCfgV4_t *pCfgV4 = L7_NULLPTR;
  L7_ospfMapCfgV5_t *pCfgV5 = L7_NULLPTR;
  L7_ospfMapCfgV6_t *pCfgV6 = L7_NULLPTR;
  L7_ospfMapCfgV7_t *pCfgV7 = L7_NULLPTR;
  L7_ospfMapCfgV8_t *pCfgV8 = L7_NULLPTR;
  L7_ospfMapCfgV9_t *pCfgV9 = L7_NULLPTR;

  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;


  pCfgCur = (L7_ospfMapCfg_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  if (pCfgHdr->version == L7_OSPF_CFG_VER_4 && pCfgHdr->length == sizeof (L7_ospfMapCfgV5_t))
  {
    oldVer = L7_OSPF_CFG_VER_5;
  }
  else if (pCfgHdr->version == L7_OSPF_CFG_VER_4 && pCfgHdr->length == sizeof (L7_ospfMapCfgV6_t))
  {
    oldVer = L7_OSPF_CFG_VER_6;
  }

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case L7_OSPF_CFG_VER_4:
    pCfgV4 = (L7_ospfMapCfgV4_t *) osapiMalloc (L7_OSPF_MAP_COMPONENT_ID,
                                                (L7_uint32) sizeof (L7_ospfMapCfgV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (L7_ospfMapCfgV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = L7_OSPF_CFG_VER_4;
    break;

  case L7_OSPF_CFG_VER_5:
    pCfgV5 = (L7_ospfMapCfgV5_t *) osapiMalloc (L7_OSPF_MAP_COMPONENT_ID,
                                                (L7_uint32) sizeof (L7_ospfMapCfgV5_t));
    if (L7_NULLPTR == pCfgV5)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV5;
    oldCfgSize = (L7_uint32) sizeof (L7_ospfMapCfgV5_t);
    pOldCfgCksum = &pCfgV5->checkSum;
    oldCfgVer = L7_OSPF_CFG_VER_4;
    break;

  case L7_OSPF_CFG_VER_6:
    pCfgV6 = (L7_ospfMapCfgV6_t *) osapiMalloc (L7_OSPF_MAP_COMPONENT_ID,
                                                (L7_uint32) sizeof (L7_ospfMapCfgV6_t));
    if (L7_NULLPTR == pCfgV6)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV6;
    oldCfgSize = (L7_uint32) sizeof (L7_ospfMapCfgV6_t);
    pOldCfgCksum = &pCfgV6->checkSum;
    oldCfgVer = L7_OSPF_CFG_VER_4;
    break;

  case L7_OSPF_CFG_VER_7:
    pCfgV7 = (L7_ospfMapCfgV7_t *) osapiMalloc (L7_OSPF_MAP_COMPONENT_ID,
                                                (L7_uint32) sizeof (L7_ospfMapCfgV7_t));

    if (L7_NULLPTR == pCfgV7)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV7;
    oldCfgSize = (L7_uint32) sizeof (L7_ospfMapCfgV7_t);
    pOldCfgCksum = &pCfgV7->checkSum;
    oldCfgVer = L7_OSPF_CFG_VER_7;
    break;

  case L7_OSPF_CFG_VER_8:
    pCfgV8 = (L7_ospfMapCfgV8_t *) osapiMalloc (L7_OSPF_MAP_COMPONENT_ID,
                                                (L7_uint32) sizeof (L7_ospfMapCfgV8_t));
    if (L7_NULLPTR == pCfgV8)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV8;
    oldCfgSize = (L7_uint32) sizeof (L7_ospfMapCfgV8_t);
    pOldCfgCksum = &pCfgV8->checkSum;
    oldCfgVer = L7_OSPF_CFG_VER_8;
    break;
  case L7_OSPF_CFG_VER_9:
    pCfgV9 = (L7_ospfMapCfgV9_t *) osapiMalloc (L7_OSPF_MAP_COMPONENT_ID,
                                               (L7_uint32) sizeof (L7_ospfMapCfgV9_t));
    if (L7_NULLPTR == pCfgV9)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV9;
    oldCfgSize = (L7_uint32) sizeof (L7_ospfMapCfgV9_t);
    pOldCfgCksum = &pCfgV9->checkSum;
    oldCfgVer = L7_OSPF_CFG_VER_9;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_OSPF_MAP_COMPONENT_ID,
                                         L7_OSPF_CFG_FILENAME, pOldCfgBuf,
                                         oldCfgSize, pOldCfgCksum, oldCfgVer,
                                         L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_OSPF_MAP_COMPONENT_ID,
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
    case L7_OSPF_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      pCfgV5 = (L7_ospfMapCfgV5_t *) osapiMalloc (L7_OSPF_MAP_COMPONENT_ID,
                                                  (L7_uint32) sizeof (L7_ospfMapCfgV5_t));
      if ((L7_NULLPTR == pCfgV5) || (L7_NULLPTR == pCfgV4) || (L7_SUCCESS != ospfMigrateConfigV4V5Convert (pCfgV4, pCfgV5)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_OSPF_MAP_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;
      /*passthru */

    case L7_OSPF_CFG_VER_5:
      /* pCfgV5 has already been allocated and filled in */
      pCfgV6 = (L7_ospfMapCfgV6_t *) osapiMalloc (L7_OSPF_MAP_COMPONENT_ID,
                                                  (L7_uint32) sizeof (L7_ospfMapCfgV6_t));
      if ((L7_NULLPTR == pCfgV6) ||(L7_NULLPTR == pCfgV5) || (L7_SUCCESS != ospfMigrateConfigV5V6Convert (pCfgV5, pCfgV6)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_OSPF_MAP_COMPONENT_ID, pCfgV5);
      pCfgV5 = L7_NULLPTR;
      /*passthru */

    case L7_OSPF_CFG_VER_6:
      /* pCfgV6 has already been allocated and filled in */
      pCfgV7 = (L7_ospfMapCfgV7_t *) osapiMalloc (L7_OSPF_MAP_COMPONENT_ID,
                                                  (L7_uint32) sizeof (L7_ospfMapCfgV7_t));
      if ((L7_NULLPTR == pCfgV7) || (L7_NULLPTR == pCfgV6) || (L7_SUCCESS != ospfMigrateConfigV6V7Convert (pCfgV6, pCfgV7)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_OSPF_MAP_COMPONENT_ID, pCfgV6);
      pCfgV6 = L7_NULLPTR;
      /*passthru */

    case L7_OSPF_CFG_VER_7:
      /* pCfgV7 has already been allocated and filled in */
      pCfgV8 = (L7_ospfMapCfgV8_t *) osapiMalloc (L7_OSPF_MAP_COMPONENT_ID,
                                                  (L7_uint32) sizeof (L7_ospfMapCfgV8_t));
      if ((L7_NULLPTR == pCfgV8) || (L7_NULLPTR == pCfgV7) || (L7_SUCCESS != ospfMigrateConfigV7V8Convert (pCfgV7, pCfgV8)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_OSPF_MAP_COMPONENT_ID, pCfgV7);
      pCfgV7 = L7_NULLPTR;
      /*passthru */

    case L7_OSPF_CFG_VER_8:
      /* pCfgV8 has already been allocated and filled in */
      pCfgV9 = (L7_ospfMapCfgV9_t *) osapiMalloc (L7_OSPF_MAP_COMPONENT_ID,
                                                 (L7_uint32) sizeof (L7_ospfMapCfgV9_t));
      if ((L7_NULLPTR == pCfgV9) || (L7_SUCCESS != ospfMigrateConfigV8V9Convert (pCfgV8, pCfgV9)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_OSPF_MAP_COMPONENT_ID, pCfgV8);
      pCfgV8 = L7_NULLPTR;
      /*passthru */
    case L7_OSPF_CFG_VER_9:
      /* pCfgV9 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV9, sizeof (*pCfgCur));
      osapiFree (L7_OSPF_MAP_COMPONENT_ID, pCfgV9);
      pCfgV9 = L7_NULLPTR;

      /* done with migration - Flag unsaved configuration */
      pCfgCur->cfgHdr.dataChanged = L7_TRUE;
      break;

    default:
      buildDefault = L7_TRUE;
      break;

    }                           /* endswitch */
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_OSPF_MAP_COMPONENT_ID, pCfgV4);
  if (pCfgV5 != L7_NULLPTR)
    osapiFree (L7_OSPF_MAP_COMPONENT_ID, pCfgV5);
  if (pCfgV6 != L7_NULLPTR)
    osapiFree (L7_OSPF_MAP_COMPONENT_ID, pCfgV6);
  if (pCfgV7 != L7_NULLPTR)
    osapiFree (L7_OSPF_MAP_COMPONENT_ID, pCfgV7);
  if (pCfgV8 != L7_NULLPTR)
    osapiFree (L7_OSPF_MAP_COMPONENT_ID, pCfgV8);
  if (pCfgV9 != L7_NULLPTR)
    osapiFree (L7_OSPF_MAP_COMPONENT_ID, pCfgV9);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    ospfBuildDefaultConfigData (L7_OSPF_CFG_VER_CURRENT);
    pCfgCur->cfgHdr.dataChanged = L7_TRUE;
  }

  return;
}

/*********************************************************************
* @purpose  Converts the interface config data structure from version V4 to V5
*
* @param    pCfgV4    @b{(input)} ptr to version 4 config data structure
* @param    pCfgV5    @b{(input)} ptr to version 5 config data structure
* @param    intfType  @b{(input)} interface types being migrated
* @param    offset    @b{(input)} current offset to version 2 config data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*          
*
* @end
*********************************************************************/
static void
ospfMigrateCktConfigV4V5Convert (L7_ospfMapCfgV4_t * pCfgV4, L7_ospfMapCfgV5_t * pCfgV5,
                                 L7_uint32 intfType, L7_uint32 * offset)
{
  L7_RC_t rc;
  L7_uint32 i, j;
  L7_uint32 intfCfgIndex;
  nimConfigID_t configId;
  L7_BOOL truncated;

           
  /* Note: The methodology used by the policyMigrate* routines in migrating different 
           types  of interfaces is more mature than the multiple invocations
           of the same routine implemented for this routine as fewer loop checks
           are required. However, this works, so we go with it. */ 

  /* Note: The truncated flag merely indicates if the migration was truncated within
           this invocation of this routine. */ 

  truncated = L7_FALSE;


  intfCfgIndex = *offset;
  for (i = 0; i < L7_MAX_SLOTS_PER_BOX_REL_4_0; i++)
  {
    for (j = 1; j <= L7_MAX_PORTS_PER_SLOT_REL_4_0; j++)
    {
      rc = sysapiCfgFileRel4_0SlotPortToIntfInfoGet (i, j, &configId, L7_NULL, L7_NULL);
      if (L7_SUCCESS != rc)
      {
        if (L7_ERROR == rc)
          MIGRATE_INVALID_SLOT_PORT (i, j);
        continue;
      }

      /* Determine if the interface is valid for participation in this feature */
      if (configId.type != intfType)
        continue;

      if (intfCfgIndex >= L7_OSPF_INTF_MAX_COUNT_REL_4_1)
      {
        MIGRATE_LOG_INTERFACE_TRUNCATE (L7_OSPF_CFG_FILENAME, intfCfgIndex);
        truncated = L7_TRUE;
        break;
      }

      (void) nimConfigIdCopy (&configId, &pCfgV5->ckt[intfCfgIndex].configId);
      pCfgV5->ckt[intfCfgIndex].adminMode = pCfgV4->ckt[i][j].adminMode;
      pCfgV5->ckt[intfCfgIndex].area = pCfgV4->ckt[i][j].area;
      pCfgV5->ckt[intfCfgIndex].priority = pCfgV4->ckt[i][j].priority;
      pCfgV5->ckt[intfCfgIndex].helloInterval = pCfgV4->ckt[i][j].helloInterval;
      pCfgV5->ckt[intfCfgIndex].deadInterval = pCfgV4->ckt[i][j].deadInterval;
      pCfgV5->ckt[intfCfgIndex].rxmtInterval = pCfgV4->ckt[i][j].rxmtInterval;
      pCfgV5->ckt[intfCfgIndex].nbmaPollInterval = pCfgV4->ckt[i][j].nbmaPollInterval;
      pCfgV5->ckt[intfCfgIndex].ifTransitDelay = pCfgV4->ckt[i][j].ifTransitDelay;
      pCfgV5->ckt[intfCfgIndex].lsaAckInterval = pCfgV4->ckt[i][j].lsaAckInterval;
      pCfgV5->ckt[intfCfgIndex].authType = pCfgV4->ckt[i][j].authType;
      MIGRATE_COPY_STRUCT (pCfgV5->ckt[intfCfgIndex].authKey, pCfgV4->ckt[i][j].authKey);
      pCfgV5->ckt[intfCfgIndex].authKeyLen = pCfgV4->ckt[i][j].authKeyLen;
      pCfgV5->ckt[intfCfgIndex].authKeyId = pCfgV4->ckt[i][j].authKeyId;
      pCfgV5->ckt[intfCfgIndex].ifDemandMode = pCfgV4->ckt[i][j].ifDemandMode;
      pCfgV5->ckt[intfCfgIndex].virtTransitAreaID = pCfgV4->ckt[i][j].virtTransitAreaID;
      pCfgV5->ckt[intfCfgIndex].virtIntfNeighbor = pCfgV4->ckt[i][j].virtIntfNeighbor;
      pCfgV5->ckt[intfCfgIndex].ifType = pCfgV4->ckt[i][j].ifType;
      pCfgV5->ckt[intfCfgIndex].metric = pCfgV4->ckt[i][j].metric;

      intfCfgIndex++;
    }

    if (truncated == L7_TRUE)
        break;
  }
  *offset = intfCfgIndex;
}

/*********************************************************************
* @purpose  When migrating from release G, get the OSPF router ID from
*           the IP MAP config file.
*
* @param    pCfgV5    @b{(input)} ptr to version 5 config data structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t
ospfMigrateRelgRouterId(L7_ospfMapCfgV5_t *pCfgV5)
{
  L7_uchar8 *ipMapCfgBuf = L7_NULLPTR;
  L7_ipMapCfgV4_t *ipMapCfg;
  L7_uint32 ipMapCfgSize = (L7_uint32) sizeof (L7_ipMapCfgV4_t);
  L7_uint32 *ipMapCfgCksum = L7_NULLPTR;

  ipMapCfgBuf = (L7_uchar8 *) osapiMalloc (L7_IP_MAP_COMPONENT_ID,
                                                (L7_uint32) sizeof (L7_ipMapCfgV4_t));
  if (ipMapCfgBuf == L7_NULLPTR)
  {
    LOG_MSG("ospfMigrateRelgRouterId() failed to allocate config buffer.");
    return L7_FAILURE;
  }
  ipMapCfg = (L7_ipMapCfgV4_t*)ipMapCfgBuf;
  ipMapCfgCksum = &ipMapCfg->checkSum;

  if (sysapiCfgFileGet(L7_IP_MAP_COMPONENT_ID, L7_IP_CFG_FILENAME, ipMapCfgBuf,
                       ipMapCfgSize, ipMapCfgCksum, L7_IP_CFG_VER_4, L7_NULL, 
                       L7_NULL) != L7_SUCCESS)    
  {
    LOG_MSG("ospfMigrateRelgRouterId() failed to get IP MAP configuration file.");
    return L7_FAILURE;
  }

  pCfgV5->rtr.routerID = ipMapCfg->rtr.routerID;

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
ospfMigrateConfigV4V5Convert (L7_ospfMapCfgV4_t * pCfgV4, L7_ospfMapCfgV5_t * pCfgV5)
{
  L7_int32 intfCfgIndex;

  /* verify correct version of old config file */
  if (pCfgV4->cfgHdr.version != L7_OSPF_CFG_VER_4)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV4->cfgHdr.version, L7_OSPF_CFG_VER_4);
    return L7_FAILURE;
  }

  ospfBuildDefaultConfigDataV5 (pCfgV5);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV5->area, pCfgV4->area);
  MIGRATE_COPY_STRUCT (pCfgV5->areaRange, pCfgV4->areaRange);
  MIGRATE_COPY_STRUCT (pCfgV5->virtNbrs, pCfgV4->virtNbrs);
  MIGRATE_COPY_STRUCT (pCfgV5->redistCfg, pCfgV4->redistCfg);
  MIGRATE_COPY_STRUCT (pCfgV5->defRouteCfg, pCfgV4->defRouteCfg);

  pCfgV5->rtr.routerID = 0;
  pCfgV5->rtr.ospfAdminMode = pCfgV4->rtr.ospfAdminMode;
  pCfgV5->rtr.rfc1583Compatibility = pCfgV4->rtr.rfc1583Compatibility;
  pCfgV5->rtr.extLSALimit = pCfgV4->rtr.extLSALimit;
  pCfgV5->rtr.exitOverflowInterval = pCfgV4->rtr.exitOverflowInterval;
  pCfgV5->rtr.opaqueLSAMode = pCfgV4->rtr.opaqueLSAMode;
  pCfgV5->rtr.traceMode = pCfgV4->rtr.traceMode;
  pCfgV5->rtr.defaultMetric = pCfgV4->rtr.defaultMetric;
  pCfgV5->rtr.defMetConfigured = pCfgV4->rtr.defMetConfigured;

  intfCfgIndex = 1;
  ospfMigrateCktConfigV4V5Convert (pCfgV4, pCfgV5, L7_PHYSICAL_INTF, &intfCfgIndex);

  if (intfCfgIndex < L7_OSPF_INTF_MAX_COUNT_REL_4_1)
      ospfMigrateCktConfigV4V5Convert (pCfgV4, pCfgV5, L7_LOGICAL_VLAN_INTF, &intfCfgIndex);
  if (intfCfgIndex < L7_OSPF_INTF_MAX_COUNT_REL_4_1)
      ospfMigrateCktConfigV4V5Convert (pCfgV4, pCfgV5, L7_LAG_INTF, &intfCfgIndex);

  /* In release G, the OSPF router ID was stored on the IP MAP configuration.
   * Get the router ID from there. */
  if (ospfMigrateRelgRouterId(pCfgV5) != L7_SUCCESS)
  {
      LOG_MSG("Failed to migrate OSPF router ID.");
  }

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
ospfMigrateConfigV5V6Convert (L7_ospfMapCfgV5_t * pCfgV5, L7_ospfMapCfgV6_t * pCfgV6)
{
  L7_uint32 i;

  /* verify correct version of old config file */
  if (pCfgV5->cfgHdr.version != L7_OSPF_CFG_VER_5 && pCfgV5->cfgHdr.version != L7_OSPF_CFG_VER_4)       /* b'cos: REL_H version was not updated */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV5->cfgHdr.version, L7_OSPF_CFG_VER_5);
    return L7_FAILURE;
  }

  ospfBuildDefaultConfigDataV6 (pCfgV6);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  MIGRATE_COPY_STRUCT (pCfgV6->areaRange, pCfgV5->areaRange);
  MIGRATE_COPY_STRUCT (pCfgV6->virtNbrs, pCfgV5->virtNbrs);
  MIGRATE_COPY_STRUCT (pCfgV6->redistCfg, pCfgV5->redistCfg);
  MIGRATE_COPY_STRUCT (pCfgV6->defRouteCfg, pCfgV5->defRouteCfg);

  pCfgV6->rtr.routerID = pCfgV5->rtr.routerID;
  pCfgV6->rtr.ospfAdminMode = pCfgV5->rtr.ospfAdminMode;
  pCfgV6->rtr.rfc1583Compatibility = pCfgV5->rtr.rfc1583Compatibility;
  pCfgV6->rtr.extLSALimit = pCfgV5->rtr.extLSALimit;
  pCfgV6->rtr.exitOverflowInterval = pCfgV5->rtr.exitOverflowInterval;
  pCfgV6->rtr.opaqueLSAMode = pCfgV5->rtr.opaqueLSAMode;
  pCfgV6->rtr.traceMode = pCfgV5->rtr.traceMode;
  pCfgV6->rtr.defaultMetric = pCfgV5->rtr.defaultMetric;
  pCfgV6->rtr.defMetConfigured = pCfgV5->rtr.defMetConfigured;

  for (i = 0; i < L7_OSPF_INTF_MAX_COUNT_REL_4_1; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV6->ckt[i].configId, pCfgV5->ckt[i].configId);
    pCfgV6->ckt[i].adminMode = pCfgV5->ckt[i].adminMode;
    pCfgV6->ckt[i].area = pCfgV5->ckt[i].area;
    pCfgV6->ckt[i].priority = pCfgV5->ckt[i].priority;
    pCfgV6->ckt[i].helloInterval = pCfgV5->ckt[i].helloInterval;
    pCfgV6->ckt[i].deadInterval = pCfgV5->ckt[i].deadInterval;
    pCfgV6->ckt[i].rxmtInterval = pCfgV5->ckt[i].rxmtInterval;
    pCfgV6->ckt[i].nbmaPollInterval = pCfgV5->ckt[i].nbmaPollInterval;
    pCfgV6->ckt[i].ifTransitDelay = pCfgV5->ckt[i].ifTransitDelay;
    pCfgV6->ckt[i].lsaAckInterval = pCfgV5->ckt[i].lsaAckInterval;
    pCfgV6->ckt[i].authType = pCfgV5->ckt[i].authType;
    MIGRATE_COPY_STRUCT (pCfgV6->ckt[i].authKey, pCfgV5->ckt[i].authKey);
    pCfgV6->ckt[i].authKeyLen = pCfgV5->ckt[i].authKeyLen;
    pCfgV6->ckt[i].authKeyId = pCfgV5->ckt[i].authKeyId;
    pCfgV6->ckt[i].ifDemandMode = pCfgV5->ckt[i].ifDemandMode;
    pCfgV6->ckt[i].virtTransitAreaID = pCfgV5->ckt[i].virtTransitAreaID;
    pCfgV6->ckt[i].virtIntfNeighbor = pCfgV5->ckt[i].virtIntfNeighbor;
    pCfgV6->ckt[i].ifType = pCfgV5->ckt[i].ifType;
    pCfgV6->ckt[i].metric = pCfgV5->ckt[i].metric;
  }

  for (i = 0; i < L7_OSPF_MAX_AREAS; i++)
  {
    pCfgV6->area[i].inUse = pCfgV5->area[i].inUse;
    pCfgV6->area[i].area = pCfgV5->area[i].area;
    pCfgV6->area[i].extRoutingCapability = pCfgV5->area[i].extRoutingCapability;
    pCfgV6->area[i].summary = pCfgV5->area[i].summary;
    pCfgV6->area[i].agingInterval = pCfgV5->area[i].agingInterval;
    MIGRATE_COPY_STRUCT (pCfgV6->area[i].stubCfg, pCfgV5->area[i].stubCfg);
    MIGRATE_COPY_STRUCT (pCfgV6->area[i].nssaCfg, pCfgV5->area[i].nssaCfg);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V6 to V7.
*
* @param    pCfgV6    @b{(input)} ptr to version 6 config data structure
* @param    pCfgV7    @b{(input)} ptr to version 7 config data structure
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
ospfMigrateConfigV6V7Convert (L7_ospfMapCfgV6_t * pCfgV6, L7_ospfMapCfgV7_t * pCfgV7)
{
  L7_uint32 i, count;

  /* verify correct version of old config file */
  if (pCfgV6->cfgHdr.version != L7_OSPF_CFG_VER_6 && pCfgV6->cfgHdr.version != L7_OSPF_CFG_VER_4)       /* b'cos: REL_H version was not updated */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV6->cfgHdr.version, L7_OSPF_CFG_VER_6);
    return L7_FAILURE;
  }

  ospfBuildDefaultConfigDataV7 (pCfgV7);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  count = min (L7_OSPF_INTF_MAX_COUNT_REL_4_1, L7_OSPF_INTF_MAX_COUNT_REL_4_3);
  for (i = 0; i < count; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV7->ckt[i], pCfgV6->ckt[i]);
  }
  MIGRATE_COPY_STRUCT (pCfgV7->rtr, pCfgV6->rtr);
  MIGRATE_COPY_STRUCT (pCfgV7->areaRange, pCfgV6->areaRange);
  MIGRATE_COPY_STRUCT (pCfgV7->virtNbrs, pCfgV6->virtNbrs);
  MIGRATE_COPY_STRUCT (pCfgV7->redistCfg, pCfgV6->redistCfg);
  MIGRATE_COPY_STRUCT (pCfgV7->defRouteCfg, pCfgV6->defRouteCfg);

  for (i = 0; i < L7_OSPF_MAX_AREAS; i++)
  {
    pCfgV7->area[i].inUse = pCfgV6->area[i].inUse;
    pCfgV7->area[i].area = pCfgV6->area[i].area;
    pCfgV7->area[i].extRoutingCapability = pCfgV6->area[i].extRoutingCapability;
    pCfgV7->area[i].summary = pCfgV6->area[i].summary;
    pCfgV7->area[i].agingInterval = pCfgV6->area[i].agingInterval;
    MIGRATE_COPY_STRUCT (pCfgV7->area[i].stubCfg, pCfgV6->area[i].stubCfg);
    MIGRATE_COPY_STRUCT (pCfgV7->area[i].nssaCfg, pCfgV6->area[i].nssaCfg);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V7 to V8.
*
* @param    pCfgV7    @b{(input)} ptr to version 7 config data structure
* @param    pCfgV8    @b{(input)} ptr to version 8 config data structure
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
ospfMigrateConfigV7V8Convert (L7_ospfMapCfgV7_t * pCfgV7, L7_ospfMapCfgV8_t * pCfgV8)
{
  L7_uint32 i, intfCount;

  /* verify correct version of old config file */
  /* b'cos: REL_H version was not updated */
  if (pCfgV7->cfgHdr.version != L7_OSPF_CFG_VER_7 && pCfgV7->cfgHdr.version != L7_OSPF_CFG_VER_4)       
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV7->cfgHdr.version, L7_OSPF_CFG_VER_7);
    return L7_FAILURE;
  }

  ospfBuildDefaultConfigDataV8 (pCfgV8);

  pCfgV8->rtr.routerID = pCfgV7->rtr.routerID;
  pCfgV8->rtr.ospfAdminMode = pCfgV7->rtr.ospfAdminMode;
  pCfgV8->rtr.rfc1583Compatibility = pCfgV7->rtr.rfc1583Compatibility;
  pCfgV8->rtr.extLSALimit = pCfgV7->rtr.extLSALimit;
  pCfgV8->rtr.exitOverflowInterval = pCfgV7->rtr.exitOverflowInterval;
  pCfgV8->rtr.opaqueLSAMode = pCfgV7->rtr.opaqueLSAMode;
  pCfgV8->rtr.traceMode = pCfgV7->rtr.traceMode;
  pCfgV8->rtr.extLSALimit = pCfgV7->rtr.extLSALimit;
  pCfgV8->rtr.defaultMetric = pCfgV7->rtr.defaultMetric;
  pCfgV8->rtr.defMetConfigured = pCfgV7->rtr.defMetConfigured;
  pCfgV8->rtr.maxPaths = pCfgV7->rtr.maxPaths;

  intfCount = min (L7_OSPF_INTF_MAX_COUNT_REL_4_3,
                   L7_OSPF_INTF_MAX_COUNT_REL_4_4);
  for (i = 0; i < intfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV8->ckt[i], pCfgV7->ckt[i]);
  }

  MIGRATE_COPY_STRUCT (pCfgV8->area, pCfgV7->area);
  MIGRATE_COPY_STRUCT (pCfgV8->areaRange, pCfgV7->areaRange);
  MIGRATE_COPY_STRUCT (pCfgV8->virtNbrs, pCfgV7->virtNbrs);
  MIGRATE_COPY_STRUCT (pCfgV8->defRouteCfg, pCfgV7->defRouteCfg);

  /* REDIST_RT_OSPFV3 added, so can't do structure copy here. */
  pCfgV8->redistCfg[REDIST_RT_LOCAL] = pCfgV7->redistCfg[REDIST_RT_LOCAL];
  pCfgV8->redistCfg[REDIST_RT_STATIC] = pCfgV7->redistCfg[REDIST_RT_STATIC];
  pCfgV8->redistCfg[REDIST_RT_RIP] = pCfgV7->redistCfg[REDIST_RT_RIP];
  pCfgV8->redistCfg[REDIST_RT_OSPF] = pCfgV7->redistCfg[REDIST_RT_OSPF];
  pCfgV8->redistCfg[REDIST_RT_BGP] = pCfgV7->redistCfg[REDIST_RT_BGP];

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts the config data structure from version V8 to V9.
*
* @param    pCfgV8    @b{(input)} ptr to version 7 config data structure
* @param    pCfgV9    @b{(input)} ptr to version 8 config data structure
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
ospfMigrateConfigV8V9Convert (L7_ospfMapCfgV8_t * pCfgV8, L7_ospfMapCfgV9_t * pCfgV9)
{
  L7_uint32 i, intfCount;

  /* verify correct version of old config file */
  /* b'cos: REL_H version was not updated */
  if (pCfgV8->cfgHdr.version != L7_OSPF_CFG_VER_8 && pCfgV8->cfgHdr.version != L7_OSPF_CFG_VER_4)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV8->cfgHdr.version, L7_OSPF_CFG_VER_8);
    return L7_FAILURE;
  }
  
  ospfBuildDefaultConfigDataV9 (pCfgV9);

  pCfgV9->rtr.routerID = pCfgV8->rtr.routerID;
  pCfgV9->rtr.ospfAdminMode = pCfgV8->rtr.ospfAdminMode;
  pCfgV9->rtr.rfc1583Compatibility = pCfgV8->rtr.rfc1583Compatibility;
  pCfgV9->rtr.extLSALimit = pCfgV8->rtr.extLSALimit;
  pCfgV9->rtr.exitOverflowInterval = pCfgV8->rtr.exitOverflowInterval;
  pCfgV9->rtr.opaqueLSAMode = pCfgV8->rtr.opaqueLSAMode;
  pCfgV9->rtr.traceMode = pCfgV8->rtr.traceMode;
  pCfgV9->rtr.extLSALimit = pCfgV8->rtr.extLSALimit;
  pCfgV9->rtr.defaultMetric = pCfgV8->rtr.defaultMetric;
  pCfgV9->rtr.defMetConfigured = pCfgV8->rtr.defMetConfigured;
  pCfgV9->rtr.maxPaths = pCfgV8->rtr.maxPaths;
  pCfgV9->rtr.spfDelay = pCfgV8->rtr.spfDelay;
  pCfgV9->rtr.spfHoldtime = pCfgV8->rtr.spfHoldtime;

  intfCount = min (L7_OSPF_INTF_MAX_COUNT_REL_4_3,
      L7_OSPF_INTF_MAX_COUNT_REL_4_4);
  for (i = 0; i < intfCount; i++)
  {
    nimConfigIdCopy (&pCfgV8->ckt[i].configId, &pCfgV9->ckt[i].configId);

    pCfgV9->ckt[i].adminMode          = pCfgV8->ckt[i].adminMode;
    pCfgV9->ckt[i].area               = pCfgV8->ckt[i].area;
    pCfgV9->ckt[i].priority           = pCfgV8->ckt[i].priority;
    pCfgV9->ckt[i].maxIpMtu           = pCfgV8->ckt[i].maxIpMtu;
    pCfgV9->ckt[i].mtuIgnore          = pCfgV8->ckt[i].mtuIgnore;
    pCfgV9->ckt[i].helloInterval      = pCfgV8->ckt[i].helloInterval;
    pCfgV9->ckt[i].deadInterval       = pCfgV8->ckt[i].deadInterval;
    pCfgV9->ckt[i].rxmtInterval       = pCfgV8->ckt[i].rxmtInterval;
    pCfgV9->ckt[i].nbmaPollInterval   = pCfgV8->ckt[i].nbmaPollInterval;
    pCfgV9->ckt[i].ifTransitDelay     = pCfgV8->ckt[i].ifTransitDelay;
    pCfgV9->ckt[i].lsaAckInterval     = pCfgV8->ckt[i].lsaAckInterval;
    pCfgV9->ckt[i].authType           = pCfgV8->ckt[i].authType;
    memcpy((L7_uchar8 *)pCfgV9->ckt[i].authKey,
           (L7_uchar8 *)pCfgV8->ckt[i].authKey, L7_AUTH_MAX_KEY_OSPF);
    pCfgV9->ckt[i].authKeyLen         = pCfgV8->ckt[i].authKeyLen;
    pCfgV9->ckt[i].authKeyId          = pCfgV8->ckt[i].authKeyId;
    pCfgV9->ckt[i].ifDemandMode       = pCfgV8->ckt[i].ifDemandMode;
    pCfgV9->ckt[i].virtTransitAreaID  = pCfgV8->ckt[i].virtTransitAreaID;
    pCfgV9->ckt[i].virtIntfNeighbor   = pCfgV8->ckt[i].virtIntfNeighbor;
    pCfgV9->ckt[i].ifType             = pCfgV8->ckt[i].ifType;
    pCfgV9->ckt[i].metric             = pCfgV8->ckt[i].metric;
  }

  MIGRATE_COPY_STRUCT (pCfgV9->area, pCfgV8->area);
  MIGRATE_COPY_STRUCT (pCfgV9->areaRange, pCfgV8->areaRange);
  MIGRATE_COPY_STRUCT (pCfgV9->virtNbrs, pCfgV8->virtNbrs);
  MIGRATE_COPY_STRUCT (pCfgV9->defRouteCfg, pCfgV8->defRouteCfg);

  /* REDIST_RT_OSPFV3 added, so can't do structure copy here. */
  pCfgV9->redistCfg[REDIST_RT_LOCAL] = pCfgV8->redistCfg[REDIST_RT_LOCAL];
  pCfgV9->redistCfg[REDIST_RT_STATIC] = pCfgV8->redistCfg[REDIST_RT_STATIC];
  pCfgV9->redistCfg[REDIST_RT_RIP] = pCfgV8->redistCfg[REDIST_RT_RIP];
  pCfgV9->redistCfg[REDIST_RT_OSPF] = pCfgV8->redistCfg[REDIST_RT_OSPF];
  pCfgV9->redistCfg[REDIST_RT_BGP] = pCfgV8->redistCfg[REDIST_RT_BGP];

  return L7_SUCCESS;
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
ospfBuildDefaultConfigDataV5 (L7_ospfMapCfgV5_t * pCfgV5)
{
  L7_uint32 cfgIndex;           /* cfgIndex               */
  L7_uint32 area;               /* area index             */
  L7_uint32 tosIndex;           /* tos  index             */
  L7_uint32 vNbr;               /* virtual neighbor index */
  L7_uint32 i;

  memset (pCfgV5, 0, sizeof (*pCfgV5));

  /* Generic OSPF Stack Cfg */
  pCfgV5->rtr.routerID = FD_OSPF_DEFAULT_ROUTER_ID;
  pCfgV5->rtr.ospfAdminMode = FD_OSPF_DEFAULT_ADMIN_MODE;
  pCfgV5->rtr.rfc1583Compatibility = FD_OSPF_DEFAULT_RFC_1583_COMPATIBILITY;
  pCfgV5->rtr.exitOverflowInterval = FD_OSPF_DEFAULT_EXIT_OVERFLOW_INTERVAL;
  pCfgV5->rtr.extLSALimit = FD_OSPF_DEFAULT_EXT_LSDB_LIMIT;
  pCfgV5->rtr.opaqueLSAMode = FD_OSPF_DEFAULT_OPAQUE_LSA_MODE;
  pCfgV5->rtr.traceMode = FD_OSPF_DEFAULT_TRACE_MODE;

  /* There really isn't a default value for defaultMetric because its value is
   * irrelevant until defMetConfigured is L7_TRUE, but this value
   * is as good as any.
   */
  pCfgV5->rtr.defaultMetric = FD_OSPF_DEFAULT_METRIC;
  pCfgV5->rtr.defMetConfigured = L7_FALSE;

  /* OSPF Router Interface Configuration parameters */
  for (cfgIndex = 1; cfgIndex < L7_OSPF_INTF_MAX_COUNT_REL_4_1; cfgIndex++)
  {
    ospfCfgCktV5_t *pCfg = &pCfgV5->ckt[cfgIndex];

    pCfg->adminMode = FD_OSPF_INTF_DEFAULT_ADMIN_MODE;
    pCfg->area = FD_OSPF_INTF_DEFAULT_AREA;
    pCfg->priority = FD_OSPF_INTF_DEFAULT_PRIORITY;
    pCfg->helloInterval = FD_OSPF_INTF_DEFAULT_HELLO_INTERVAL;
    pCfg->deadInterval = FD_OSPF_INTF_DEFAULT_DEAD_INTERVAL;
    pCfg->rxmtInterval = FD_OSPF_INTF_DEFAULT_RXMT_INTERVAL;
    pCfg->nbmaPollInterval = FD_OSPF_INTF_DEFAULT_NBMA_POLL_INTERVAL;
    pCfg->ifTransitDelay = FD_OSPF_INTF_DEFAULT_IF_TRANSIT_DELAY;
    pCfg->lsaAckInterval = FD_OSPF_INTF_DEFAULT_LSA_ACK_INTERVAL;
    pCfg->authType = FD_OSPF_INTF_DEFAULT_AUTH_TYPE;
    memset (pCfg->authKey, FD_OSPF_INTF_DEFAULT_AUTH_KEY_CHAR, (size_t) L7_AUTH_MAX_KEY_OSPF);
    pCfg->authKeyLen = FD_OSPF_INTF_DEFAULT_AUTH_KEY_LEN;
    pCfg->authKeyId = FD_OSPF_INTF_DEFAULT_AUTH_KEY_ID;
    pCfg->ifDemandMode = FD_OSPF_INTF_DEFAULT_IF_DEMAND_MODE;
    pCfg->virtTransitAreaID = FD_OSPF_INTF_DEFAULT_VIRT_TRANSIT_AREA_ID;
    pCfg->virtIntfNeighbor = FD_OSPF_INTF_DEFAULT_VIRT_INTF_NEIGHBOR;
    pCfg->ifType = FD_OSPF_INTF_DEFAULT_TYPE;
    /* by default, the metric will be computed based on port speed */
    pCfg->metric = L7_OSPF_INTF_METRIC_NOT_CONFIGURED;
  }

  /* OSPF Area Configuration parameters */

  /* Only the backbone area needs to be configured initially */
  area = FD_OSPF_AREA_DEFAULT_AREA;

  pCfgV5->area[area].inUse = L7_TRUE;
  pCfgV5->area[area].area = FD_OSPF_AREA_DEFAULT_AREA;
  pCfgV5->area[area].agingInterval = FD_OSPF_AREA_DEFAULT_AGING_INTERVAL;
  pCfgV5->area[area].summary = FD_OSPF_AREA_DEFAULT_SUMMARY;
  pCfgV5->area[area].extRoutingCapability = FD_OSPF_AREA_DEFAULT_EXT_ROUTING_CAPABILITY;
  /* pCfgV5->area[area].authType = FD_OSPF_AREA_DEFAULT_AUTH_TYPE; */

  /* Configure stub metrics */
  for (tosIndex = 0; tosIndex < L7_RTR_MAX_TOS_INDICES; tosIndex++)
  {
    pCfgV5->area[area].stubCfg[tosIndex].stubMetricType = FD_OSPF_AREA_DEFAULT_STUB_METRIC_TYPE;
    pCfgV5->area[area].stubCfg[tosIndex].stubMetric = FD_OSPF_AREA_DEFAULT_STUB_METRIC;
    pCfgV5->area[area].stubCfg[tosIndex].inUse = L7_TRUE;
  }

  /* OSPF Area Range Configuration parameters:
     Nothing to be done.  Zeroeing out the configuration is enough */

  /* OSPF Virtual Neighbor Configuration Parameters */
  for (vNbr = 0; vNbr < L7_OSPF_MAX_VIRT_NBRS_REL_4_1; vNbr++)
  {
    /* These settings are the default values for virtual neighbor entries when created */
    pCfgV5->virtNbrs[vNbr].virtTransitAreaID = FD_OSPF_INTF_DEFAULT_VIRT_TRANSIT_AREA_ID;
    pCfgV5->virtNbrs[vNbr].virtIntfNeighbor = FD_OSPF_INTF_DEFAULT_VIRT_INTF_NEIGHBOR;
    pCfgV5->virtNbrs[vNbr].intIfNum = FD_OSPF_INTF_DEFAULT_VIRT_INTIFNUM;
    pCfgV5->virtNbrs[vNbr].helloInterval = FD_OSPF_INTF_DEFAULT_HELLO_INTERVAL;
    pCfgV5->virtNbrs[vNbr].deadInterval = FD_OSPF_INTF_DEFAULT_DEAD_INTERVAL;
    pCfgV5->virtNbrs[vNbr].rxmtInterval = FD_OSPF_INTF_DEFAULT_RXMT_INTERVAL;
    pCfgV5->virtNbrs[vNbr].ifTransitDelay = FD_OSPF_INTF_DEFAULT_IF_TRANSIT_DELAY;
    pCfgV5->virtNbrs[vNbr].authType = FD_OSPF_INTF_DEFAULT_AUTH_TYPE;
    memset ((void *) pCfgV5->virtNbrs[vNbr].authKey, FD_OSPF_INTF_DEFAULT_AUTH_KEY_CHAR,
            (size_t) L7_AUTH_MAX_KEY_OSPF);
    pCfgV5->virtNbrs[vNbr].authKeyLen = FD_OSPF_INTF_DEFAULT_AUTH_KEY_LEN;
    pCfgV5->virtNbrs[vNbr].authKeyId = FD_OSPF_INTF_DEFAULT_AUTH_KEY_ID;
  }

  /* Defaults for route redistribution */
  for (i = 0; i < L7_OSPF_REDIST_RT_LAST_REL_4_1; i++)
  {
    ospfRedistCfgInit (&pCfgV5->redistCfg[i]);
  }

  /* Defaults for default route origination */
  ospfDefaultRouteCfgInit (&pCfgV5->defRouteCfg);

  strcpy (pCfgV5->cfgHdr.filename, L7_OSPF_CFG_FILENAME);
  pCfgV5->cfgHdr.version = L7_OSPF_CFG_VER_5;
  pCfgV5->cfgHdr.componentID = L7_OSPF_MAP_COMPONENT_ID;
  pCfgV5->cfgHdr.type = L7_CFG_DATA;
  pCfgV5->cfgHdr.length = sizeof (*pCfgV5);
  pCfgV5->cfgHdr.dataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  Build Version 6 defaults
*
* @param    pCfgV6    @b{(input)} ptr to version 6 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
ospfBuildDefaultConfigDataV6 (L7_ospfMapCfgV6_t * pCfgV6)
{
  L7_uint32 cfgIndex;           /* cfgIndex               */
  L7_uint32 area;               /* area index             */
  L7_uint32 tosIndex;           /* tos  index             */
  L7_uint32 vNbr;               /* virtual neighbor index */
  L7_uint32 i;

  memset (pCfgV6, 0, sizeof (*pCfgV6));

    /*---------------------------*/
  /* Build Default Config Data */

    /*---------------------------*/

  /* Generic OSPF Stack Cfg */
  pCfgV6->rtr.routerID = FD_OSPF_DEFAULT_ROUTER_ID;
  pCfgV6->rtr.ospfAdminMode = FD_OSPF_DEFAULT_ADMIN_MODE;
  pCfgV6->rtr.rfc1583Compatibility = FD_OSPF_DEFAULT_RFC_1583_COMPATIBILITY;
  pCfgV6->rtr.exitOverflowInterval = FD_OSPF_DEFAULT_EXIT_OVERFLOW_INTERVAL;
  pCfgV6->rtr.extLSALimit = FD_OSPF_DEFAULT_EXT_LSDB_LIMIT;
  pCfgV6->rtr.opaqueLSAMode = FD_OSPF_DEFAULT_OPAQUE_LSA_MODE;
  pCfgV6->rtr.traceMode = FD_OSPF_DEFAULT_TRACE_MODE;
  /* There really isn't a default value for defaultMetric because its value is
   * irrelevant until defMetConfigured is L7_TRUE, but this value
   * is as good as any.
   */
  pCfgV6->rtr.defaultMetric = FD_OSPF_DEFAULT_METRIC;
  pCfgV6->rtr.defMetConfigured = L7_FALSE;
  pCfgV6->rtr.maxPaths = FD_OSPF_MAXIMUM_PATHS;

  /* OSPF Router Interface Configuration parameters */
  for (cfgIndex = 1; cfgIndex < L7_OSPF_INTF_MAX_COUNT_REL_4_1; cfgIndex++)
  {
    ospfCfgCktV6_t *pCfg = &pCfgV6->ckt[cfgIndex];

    pCfg->adminMode = FD_OSPF_INTF_DEFAULT_ADMIN_MODE;
    pCfg->area = FD_OSPF_INTF_DEFAULT_AREA;
    pCfg->priority = FD_OSPF_INTF_DEFAULT_PRIORITY;
    pCfg->helloInterval = FD_OSPF_INTF_DEFAULT_HELLO_INTERVAL;
    pCfg->deadInterval = FD_OSPF_INTF_DEFAULT_DEAD_INTERVAL;
    pCfg->rxmtInterval = FD_OSPF_INTF_DEFAULT_RXMT_INTERVAL;
    pCfg->nbmaPollInterval = FD_OSPF_INTF_DEFAULT_NBMA_POLL_INTERVAL;
    pCfg->ifTransitDelay = FD_OSPF_INTF_DEFAULT_IF_TRANSIT_DELAY;
    pCfg->lsaAckInterval = FD_OSPF_INTF_DEFAULT_LSA_ACK_INTERVAL;
    pCfg->authType = FD_OSPF_INTF_DEFAULT_AUTH_TYPE;
    memset (pCfg->authKey, FD_OSPF_INTF_DEFAULT_AUTH_KEY_CHAR, (size_t) L7_AUTH_MAX_KEY_OSPF);
    pCfg->authKeyLen = FD_OSPF_INTF_DEFAULT_AUTH_KEY_LEN;
    pCfg->authKeyId = FD_OSPF_INTF_DEFAULT_AUTH_KEY_ID;
    pCfg->ifDemandMode = FD_OSPF_INTF_DEFAULT_IF_DEMAND_MODE;
    pCfg->virtTransitAreaID = FD_OSPF_INTF_DEFAULT_VIRT_TRANSIT_AREA_ID;
    pCfg->virtIntfNeighbor = FD_OSPF_INTF_DEFAULT_VIRT_INTF_NEIGHBOR;
    pCfg->ifType = FD_OSPF_INTF_DEFAULT_TYPE;
    /* by default, the metric will be computed based on port speed */
    pCfg->metric = L7_OSPF_INTF_METRIC_NOT_CONFIGURED;
    pCfg->mtuIgnore = FD_OSPF_DEFAULT_MTU_IGNORE;
    pCfg->maxIpMtu = FD_IP_DEFAULT_IP_MTU;
  }

  /* OSPF Area Configuration parameters */

  /* Only the backbone area needs to be configured initially */
  area = FD_OSPF_AREA_DEFAULT_AREA;

  pCfgV6->area[area].inUse = L7_TRUE;
  pCfgV6->area[area].area = FD_OSPF_AREA_DEFAULT_AREA;
  pCfgV6->area[area].agingInterval = FD_OSPF_AREA_DEFAULT_AGING_INTERVAL;
  pCfgV6->area[area].summary = FD_OSPF_AREA_DEFAULT_SUMMARY;
  pCfgV6->area[area].extRoutingCapability = FD_OSPF_AREA_DEFAULT_EXT_ROUTING_CAPABILITY;

  /* Configure stub metrics */
  for (tosIndex = 0; tosIndex < L7_RTR_MAX_TOS_INDICES; tosIndex++)
  {
    pCfgV6->area[area].stubCfg[tosIndex].stubMetricType = FD_OSPF_AREA_DEFAULT_STUB_METRIC_TYPE;
    pCfgV6->area[area].stubCfg[tosIndex].stubMetric = FD_OSPF_AREA_DEFAULT_STUB_METRIC;
    pCfgV6->area[area].stubCfg[tosIndex].inUse = L7_TRUE;
  }

  /* OSPF Area Range Configuration parameters:
     Nothing to be done.  Zeroeing out the configuration is enough */

  /* OSPF Virtual Neighbor Configuration Parameters */
  for (vNbr = 0; vNbr < L7_OSPF_MAX_VIRT_NBRS; vNbr++)
  {
    /* These settings are the default values for virtual neighbor entries when created */
    pCfgV6->virtNbrs[vNbr].virtTransitAreaID = FD_OSPF_INTF_DEFAULT_VIRT_TRANSIT_AREA_ID;
    pCfgV6->virtNbrs[vNbr].virtIntfNeighbor = FD_OSPF_INTF_DEFAULT_VIRT_INTF_NEIGHBOR;
    pCfgV6->virtNbrs[vNbr].intIfNum = FD_OSPF_INTF_DEFAULT_VIRT_INTIFNUM;
    pCfgV6->virtNbrs[vNbr].helloInterval = FD_OSPF_INTF_DEFAULT_HELLO_INTERVAL;
    pCfgV6->virtNbrs[vNbr].deadInterval = FD_OSPF_INTF_DEFAULT_DEAD_INTERVAL;
    pCfgV6->virtNbrs[vNbr].rxmtInterval = FD_OSPF_INTF_DEFAULT_RXMT_INTERVAL;
    pCfgV6->virtNbrs[vNbr].ifTransitDelay = FD_OSPF_INTF_DEFAULT_IF_TRANSIT_DELAY;
    pCfgV6->virtNbrs[vNbr].authType = FD_OSPF_INTF_DEFAULT_AUTH_TYPE;
    memset ((void *) pCfgV6->virtNbrs[vNbr].authKey, FD_OSPF_INTF_DEFAULT_AUTH_KEY_CHAR,
            (size_t) L7_AUTH_MAX_KEY_OSPF);
    pCfgV6->virtNbrs[vNbr].authKeyLen = FD_OSPF_INTF_DEFAULT_AUTH_KEY_LEN;
    pCfgV6->virtNbrs[vNbr].authKeyId = FD_OSPF_INTF_DEFAULT_AUTH_KEY_ID;

  }                             /* vNbr < L7_OSPF_MAX_VIRT_NBRS */

  /* Defaults for route redistribution */
  for (i = 0; i < REDIST_RT_LAST; i++)
  {
    ospfRedistCfgInit (&pCfgV6->redistCfg[i]);
  }

  /* Defaults for default route origination */
  ospfDefaultRouteCfgInit (&pCfgV6->defRouteCfg);

  strcpy (pCfgV6->cfgHdr.filename, L7_OSPF_CFG_FILENAME);
  pCfgV6->cfgHdr.version = L7_OSPF_CFG_VER_6;
  pCfgV6->cfgHdr.componentID = L7_OSPF_MAP_COMPONENT_ID;
  pCfgV6->cfgHdr.type = L7_CFG_DATA;
  pCfgV6->cfgHdr.length = sizeof (*pCfgV6);
  pCfgV6->cfgHdr.dataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  Build Version 7 defaults
*
* @param    pCfgV7    @b{(input)} ptr to version 7 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
ospfBuildDefaultConfigDataV7 (L7_ospfMapCfgV7_t * pCfgV7)
{
  L7_uint32 cfgIndex; /* cfgIndex               */
  L7_uint32 area;     /* area index             */
  L7_uint32 tosIndex; /* tos  index             */
  L7_uint32 vNbr;     /* virtual neighbor index */
  L7_uint32 i;        

  memset(( void * )pCfgV7, 0, sizeof( L7_ospfMapCfgV7_t));

  pCfgV7->rtr.routerID             = FD_OSPF_DEFAULT_ROUTER_ID;
  pCfgV7->rtr.ospfAdminMode        = FD_OSPF_DEFAULT_ADMIN_MODE;
  pCfgV7->rtr.rfc1583Compatibility = FD_OSPF_DEFAULT_RFC_1583_COMPATIBILITY;
  pCfgV7->rtr.extLSALimit          = FD_OSPF_DEFAULT_EXT_LSDB_LIMIT;
  pCfgV7->rtr.exitOverflowInterval = FD_OSPF_DEFAULT_EXIT_OVERFLOW_INTERVAL;
  pCfgV7->rtr.opaqueLSAMode        = FD_OSPF_DEFAULT_OPAQUE_LSA_MODE;
  pCfgV7->rtr.traceMode            = FD_OSPF_DEFAULT_TRACE_MODE;
  /* There really isn't a default value for defaultMetric because its value is
   * irrelevant until defMetConfigured is L7_TRUE, but this value
   * is as good as any. 
   */
  pCfgV7->rtr.defaultMetric        = FD_OSPF_DEFAULT_METRIC;
  pCfgV7->rtr.defMetConfigured     = L7_FALSE;
  pCfgV7->rtr.maxPaths             = FD_OSPF_MAXIMUM_PATHS;


  /* OSPF Router Interface Configuration parameters */
  for (cfgIndex = 1; cfgIndex < L7_OSPF_INTF_MAX_COUNT_REL_4_3; cfgIndex++)
  {
    ospfCfgCktV7_t *pCfg = &pCfgV7->ckt[cfgIndex];
    pCfg->adminMode      = FD_OSPF_INTF_DEFAULT_ADMIN_MODE;
    pCfg->area           = FD_OSPF_INTF_DEFAULT_AREA;
    pCfg->priority       = FD_OSPF_INTF_DEFAULT_PRIORITY;              
    pCfg->helloInterval  = FD_OSPF_INTF_DEFAULT_HELLO_INTERVAL;
    pCfg->deadInterval   = FD_OSPF_INTF_DEFAULT_DEAD_INTERVAL;         
    pCfg->rxmtInterval   = FD_OSPF_INTF_DEFAULT_RXMT_INTERVAL;         
    pCfg->nbmaPollInterval   = FD_OSPF_INTF_DEFAULT_NBMA_POLL_INTERVAL;    
    pCfg->ifTransitDelay     = FD_OSPF_INTF_DEFAULT_IF_TRANSIT_DELAY;      
    pCfg->lsaAckInterval     = FD_OSPF_INTF_DEFAULT_LSA_ACK_INTERVAL;      
    pCfg->authType           = FD_OSPF_INTF_DEFAULT_AUTH_TYPE;             
    memset(pCfg->authKey, FD_OSPF_INTF_DEFAULT_AUTH_KEY_CHAR, (size_t)L7_AUTH_MAX_KEY_OSPF);  
    pCfg->authKeyLen         = FD_OSPF_INTF_DEFAULT_AUTH_KEY_LEN;
    pCfg->authKeyId          = FD_OSPF_INTF_DEFAULT_AUTH_KEY_ID;
    pCfg->ifDemandMode       = FD_OSPF_INTF_DEFAULT_IF_DEMAND_MODE;
    pCfg->virtTransitAreaID  = FD_OSPF_INTF_DEFAULT_VIRT_TRANSIT_AREA_ID;
    pCfg->virtIntfNeighbor   = FD_OSPF_INTF_DEFAULT_VIRT_INTF_NEIGHBOR;  
    pCfg->ifType             = FD_OSPF_INTF_DEFAULT_TYPE;
    /* by default, the metric will be computed based on port speed */
    pCfg->metric             = L7_OSPF_INTF_METRIC_NOT_CONFIGURED; 
    pCfg->mtuIgnore          = FD_OSPF_DEFAULT_MTU_IGNORE; 
    pCfg->maxIpMtu           = FD_IP_DEFAULT_IP_MTU;
  }

  /* OSPF Area Configuration parameters */

  /* Only the backbone area needs to be configured initially */
  area =  FD_OSPF_AREA_DEFAULT_AREA;

  pCfgV7->area[area].inUse         = L7_TRUE;
  pCfgV7->area[area].area          = FD_OSPF_AREA_DEFAULT_AREA;
  pCfgV7->area[area].agingInterval = FD_OSPF_AREA_DEFAULT_AGING_INTERVAL;
  pCfgV7->area[area].summary       = FD_OSPF_AREA_DEFAULT_SUMMARY;
  pCfgV7->area[area].extRoutingCapability  = FD_OSPF_AREA_DEFAULT_EXT_ROUTING_CAPABILITY;

  /* Configure stub metrics */
  for (tosIndex = 0; tosIndex < L7_RTR_MAX_TOS_INDICES; tosIndex++)
  {
      pCfgV7->area[area].stubCfg[tosIndex].stubMetricType 
      = FD_OSPF_AREA_DEFAULT_STUB_METRIC_TYPE;

      pCfgV7->area[area].stubCfg[tosIndex].stubMetric = FD_OSPF_AREA_DEFAULT_STUB_METRIC;

      pCfgV7->area[area].stubCfg[tosIndex].inUse   = L7_TRUE;
  }  /*   tos < L7_MAX_TOS_INDICES */


  /* OSPF Area Range Configuration parameters:
     Nothing to be done.  Zeroeing out the configuration is enough */

  /* OSPF Virtual Neighbor Configuration Parameters */
  for (vNbr = 0; vNbr < L7_OSPF_MAX_VIRT_NBRS; vNbr++)
  {
      /* These settings are the default values for virtual neighbor entries when created */
      pCfgV7->virtNbrs[vNbr].virtTransitAreaID = FD_OSPF_INTF_DEFAULT_VIRT_TRANSIT_AREA_ID;
      pCfgV7->virtNbrs[vNbr].virtIntfNeighbor = FD_OSPF_INTF_DEFAULT_VIRT_INTF_NEIGHBOR;
      pCfgV7->virtNbrs[vNbr].intIfNum = FD_OSPF_INTF_DEFAULT_VIRT_INTIFNUM;
      pCfgV7->virtNbrs[vNbr].helloInterval = FD_OSPF_INTF_DEFAULT_HELLO_INTERVAL;
      pCfgV7->virtNbrs[vNbr].deadInterval = FD_OSPF_INTF_DEFAULT_DEAD_INTERVAL;
      pCfgV7->virtNbrs[vNbr].rxmtInterval = FD_OSPF_INTF_DEFAULT_RXMT_INTERVAL;
      pCfgV7->virtNbrs[vNbr].ifTransitDelay = FD_OSPF_INTF_DEFAULT_IF_TRANSIT_DELAY;
      pCfgV7->virtNbrs[vNbr].authType = FD_OSPF_INTF_DEFAULT_AUTH_TYPE;
      memset((void *)pCfgV7->virtNbrs[vNbr].authKey, FD_OSPF_INTF_DEFAULT_AUTH_KEY_CHAR, (size_t)L7_AUTH_MAX_KEY_OSPF);  
      pCfgV7->virtNbrs[vNbr].authKeyLen = FD_OSPF_INTF_DEFAULT_AUTH_KEY_LEN;
      pCfgV7->virtNbrs[vNbr].authKeyId = FD_OSPF_INTF_DEFAULT_AUTH_KEY_ID;

  } /* vNbr < L7_OSPF_MAX_VIRT_NBRS */

  /* Defaults for route redistribution */
  for (i = 0; i < REDIST_RT_LAST_V7; i++) {
      ospfRedistCfgInit(&pCfgV7->redistCfg[i]);
  }

  /* Defaults for default route origination */
  ospfDefaultRouteCfgInit(&pCfgV7->defRouteCfg);
}

/*********************************************************************
* @purpose  Build Version 8 defaults
*
* @param    pCfgV8    @b{(input)} ptr to version 8 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
  static void
ospfBuildDefaultConfigDataV8 (L7_ospfMapCfgV8_t * pCfgV8)
{
  L7_uint32 area;     /* area index             */
  L7_uint32 tosIndex; /* tos  index             */
  L7_uint32 vNbr;     /* virtual neighbor index */
  L7_uint32 i;

  memset(( void * )pCfgV8, 0, sizeof( L7_ospfMapCfgV8_t));

  /* Build Config File Header */
  strcpy(pCfgV8->cfgHdr.filename, L7_OSPF_CFG_FILENAME);
  pCfgV8->cfgHdr.version       =L7_OSPF_CFG_VER_8 ;
  pCfgV8->cfgHdr.componentID   = L7_OSPF_MAP_COMPONENT_ID;
  pCfgV8->cfgHdr.type          = L7_CFG_DATA;
  pCfgV8->cfgHdr.length        = sizeof(pCfgV8);
  pCfgV8->cfgHdr.dataChanged   = L7_FALSE;


  /*---------------------------*/
  /* Build Default Config Data */
  /*---------------------------*/

  /* Generic OSPF Stack Cfg */
  pCfgV8->rtr.routerID             = FD_OSPF_DEFAULT_ROUTER_ID;
  pCfgV8->rtr.ospfAdminMode        = FD_OSPF_DEFAULT_ADMIN_MODE;
  pCfgV8->rtr.rfc1583Compatibility = FD_OSPF_DEFAULT_RFC_1583_COMPATIBILITY;
  pCfgV8->rtr.exitOverflowInterval = FD_OSPF_DEFAULT_EXIT_OVERFLOW_INTERVAL;
  pCfgV8->rtr.spfDelay             = FD_OSPF_DEFAULT_SPF_DELAY;
  pCfgV8->rtr.spfHoldtime          = FD_OSPF_DEFAULT_SPF_HOLDTIME;
  pCfgV8->rtr.extLSALimit          = FD_OSPF_DEFAULT_EXT_LSDB_LIMIT;
  pCfgV8->rtr.opaqueLSAMode        = FD_OSPF_DEFAULT_OPAQUE_LSA_MODE;
  pCfgV8->rtr.traceMode            = FD_OSPF_DEFAULT_TRACE_MODE;
  /* There really isn't a default value for defaultMetric because its value is
   * irrelevant until defMetConfigured is L7_TRUE, but this value
   * is as good as any.
   */
  pCfgV8->rtr.defaultMetric        = FD_OSPF_DEFAULT_METRIC;
  pCfgV8->rtr.defMetConfigured     = L7_FALSE;
  pCfgV8->rtr.maxPaths             = FD_OSPF_MAXIMUM_PATHS;

   /* OSPF Router Interface Configuration parameters */
  /* Interface config defaults removed to fix coverity defect. Binary config 
   * migration for this application is no longer supported and will be 
   * completely removed in 6.2. */

  /* OSPF Area Configuration parameters */

  /* Only the backbone area needs to be configured initially */
  area =  FD_OSPF_AREA_DEFAULT_AREA;

  pCfgV8->area[area].inUse         = L7_TRUE;
  pCfgV8->area[area].area          = FD_OSPF_AREA_DEFAULT_AREA;
  pCfgV8->area[area].summary       = FD_OSPF_AREA_DEFAULT_SUMMARY;
  pCfgV8->area[area].extRoutingCapability  =  FD_OSPF_AREA_DEFAULT_EXT_ROUTING_CAPABILITY;

  /* Configure stub metrics */
  for (tosIndex = 0; tosIndex < L7_RTR_MAX_TOS_INDICES; tosIndex++)
  {
     pCfgV8->area[area].stubCfg[tosIndex].stubMetricType
      = FD_OSPF_AREA_DEFAULT_STUB_METRIC_TYPE;

     pCfgV8->area[area].stubCfg[tosIndex].stubMetric = FD_OSPF_AREA_DEFAULT_STUB_METRIC;

     pCfgV8->area[area].stubCfg[tosIndex].inUse   = L7_TRUE;
  }  /*   tos < L7_MAX_TOS_INDICES */


  /* OSPF Area Range Configuration parameters:
     Nothing to be done.  Zeroeing out the configuration is enough */

  /* OSPF Virtual Neighbor Configuration Parameters */
  for (vNbr = 0; vNbr < L7_OSPF_MAX_VIRT_NBRS; vNbr++)
  {
    /* These settings are the default values for virtual neighbor entries when created */
     pCfgV8->virtNbrs[vNbr].virtTransitAreaID = FD_OSPF_INTF_DEFAULT_VIRT_TRANSIT_AREA_ID;
     pCfgV8->virtNbrs[vNbr].virtIntfNeighbor = FD_OSPF_INTF_DEFAULT_VIRT_INTF_NEIGHBOR;
     pCfgV8->virtNbrs[vNbr].intIfNum = FD_OSPF_INTF_DEFAULT_VIRT_INTIFNUM;
     pCfgV8->virtNbrs[vNbr].helloInterval = FD_OSPF_INTF_DEFAULT_HELLO_INTERVAL;
     pCfgV8->virtNbrs[vNbr].deadInterval = FD_OSPF_INTF_DEFAULT_DEAD_INTERVAL;
     pCfgV8->virtNbrs[vNbr].rxmtInterval = FD_OSPF_INTF_DEFAULT_RXMT_INTERVAL;
     pCfgV8->virtNbrs[vNbr].ifTransitDelay = FD_OSPF_INTF_DEFAULT_IF_TRANSIT_DELAY;
     pCfgV8->virtNbrs[vNbr].authType = FD_OSPF_INTF_DEFAULT_AUTH_TYPE;
     memset((void *)pCfgV8->virtNbrs[vNbr].authKey, FD_OSPF_INTF_DEFAULT_AUTH_KEY_CHAR, (size_t)L7_AUTH_MAX_KEY_OSPF);
     pCfgV8->virtNbrs[vNbr].authKeyLen = FD_OSPF_INTF_DEFAULT_AUTH_KEY_LEN;
     pCfgV8->virtNbrs[vNbr].authKeyId = FD_OSPF_INTF_DEFAULT_AUTH_KEY_ID;

  } /* vNbr < L7_OSPF_MAX_VIRT_NBRS */

  /* Defaults for route redistribution */
  for (i = 0; i < REDIST_RT_LAST; i++) {
    ospfRedistCfgInit(&pCfgV8->redistCfg[i]);
  }

  /* Defaults for default route origination */
  ospfDefaultRouteCfgInit(&pCfgV8->defRouteCfg);

  /* End of OSPF Component's Factory Defaults */

}

/*********************************************************************
  * @purpose  Build Version 9 defaults
  *
  * @param    pCfgV9    @b{(input)} ptr to version 9 config data structure
  *
  * @returns  none
  *
  * @end
  *********************************************************************/
  static void
ospfBuildDefaultConfigDataV9 (L7_ospfMapCfgV9_t * pCfgV9)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (L7_ospfMapCfgV9_t));
  ospfBuildDefaultConfigData (L7_OSPF_CFG_VER_9);
  memcpy ((L7_uchar8 *) pCfgV9, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}

