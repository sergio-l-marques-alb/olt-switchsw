
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename    garp_migrate.c
* @purpose     GARP Configuration Migration
* @component   GARP
* @comments    none
* @create      08/24/2004
* @author      Rama Sasthri, Kristipati
* @author      
* @end
*             
**********************************************************************/

#include <string.h>
#include "garp_migrate.h"
#include "../vlan/dot1q/include/dot1q.h"        
#include "../vlan/dot1q/include/dot1q_migrate.h"

/* Note: In release 4.4, the garp component was decoupled from the dot1q component.
         This results in a need to access private data structures in dot1q
         solely for the purpose of config migration.
         
*/
                                                 
static L7_RC_t garpMigrateConfigV1V2Convert (garpCfgV1_t * pCfgV1, garpCfgV2_t * pCfgV2);
static L7_RC_t garpMigrateConfigV2V3Convert (garpCfgV2_t * pCfgV2, garpCfgV3_t * pCfgV3);
static L7_RC_t garpMigrateConfigV3V4Convert (garpCfgV3_t * pCfgV3, garpCfgV4_t * pCfgV4);

static void garpBuildDefaultConfigDataV2 (garpCfgV2_t * pCfgV2);
static void garpBuildDefaultConfigDataV3 (garpCfgV3_t * pCfgV3);
static void garpBuildDefaultConfigDataV4 (garpCfgV4_t * pCfgV4);


static L7_BOOL garpMigrateRel4_3ParmsFromDot1q(dot1qCfgV4_t **pDot1qCfg);

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static garpCfg_t *pCfgCur = L7_NULLPTR;

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
garpMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  garpCfgV1_t *pCfgV1 = L7_NULLPTR;
  garpCfgV2_t *pCfgV2 = L7_NULLPTR;
  garpCfgV3_t *pCfgV3 = L7_NULLPTR;
  garpCfgV4_t *pCfgV4 = L7_NULLPTR;

  pOldCfgCksum  = L7_NULLPTR;
  oldCfgSize    = 0;
  oldCfgVer     = 0;

  pCfgCur = (garpCfg_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));

  if (pCfgHdr->version == GARP_CFG_VER_0 && pCfgHdr->length == sizeof (garpCfgV1_t))
  {
    oldVer = GARP_CFG_VER_1;
  }
  else if (pCfgHdr->version == GARP_CFG_VER_1 && pCfgHdr->length == sizeof (garpCfgV2_t))
  {
    oldVer = GARP_CFG_VER_2;
  }

  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case GARP_CFG_VER_1:
    pCfgV1 = (garpCfgV1_t *) osapiMalloc (L7_GARP_COMPONENT_ID, (L7_uint32) sizeof (garpCfgV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (garpCfgV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = pCfgHdr->version;
    break;

  case GARP_CFG_VER_2:
    pCfgV2 = (garpCfgV2_t *) osapiMalloc (L7_GARP_COMPONENT_ID, (L7_uint32) sizeof (garpCfgV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (garpCfgV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = pCfgHdr->version;
    break;

  case GARP_CFG_VER_3:
    pCfgV3 = (garpCfgV3_t *) osapiMalloc (L7_GARP_COMPONENT_ID, (L7_uint32) sizeof (garpCfgV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (garpCfgV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = pCfgHdr->version;
    break;

  case GARP_CFG_VER_4:
    pCfgV4 = (garpCfgV4_t *) osapiMalloc (L7_GARP_COMPONENT_ID, (L7_uint32) sizeof (garpCfgV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (garpCfgV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = pCfgHdr->version;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_GARP_COMPONENT_ID,
                                         GARP_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
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
    case GARP_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (garpCfgV2_t *) osapiMalloc (L7_GARP_COMPONENT_ID, (L7_uint32) sizeof (garpCfgV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_SUCCESS != garpMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_GARP_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case GARP_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (garpCfgV3_t *) osapiMalloc (L7_GARP_COMPONENT_ID, (L7_uint32) sizeof (garpCfgV3_t));
      if ((L7_NULLPTR == pCfgV3) || (L7_SUCCESS != garpMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_GARP_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case GARP_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (garpCfgV4_t *) osapiMalloc (L7_GARP_COMPONENT_ID, (L7_uint32) sizeof (garpCfgV4_t));
      if ((L7_NULLPTR == pCfgV4) || (L7_SUCCESS != garpMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_GARP_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case GARP_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV4, sizeof (*pCfgCur));
      osapiFree (L7_GARP_COMPONENT_ID, pCfgV4);
      pCfgV4 = L7_NULLPTR;

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
    osapiFree (L7_GARP_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_GARP_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_GARP_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_GARP_COMPONENT_ID, pCfgV4);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    garpBuildDefaultConfigData (GARP_CFG_VER_CURRENT);
    pCfgCur->hdr.dataChanged = L7_TRUE;
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
garpMigrateConfigV1V2Convert (garpCfgV1_t * pCfgV1, garpCfgV2_t * pCfgV2)
{
  L7_RC_t rc;
  L7_uint32 i;
  L7_int32 intfCfgIndex;
  nimConfigID_t configId;
  L7_INTF_TYPES_t intfType;

  /* verify correct version of old config file */
  if (pCfgV1->hdr.version != GARP_CFG_VER_1 && pCfgV1->hdr.version != GARP_CFG_VER_0)   /* b'cos REL_G gives wrong version */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->hdr.version, GARP_CFG_VER_1);
    return L7_FAILURE;
  }

  garpBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  intfCfgIndex = 1;
  intfType = 0;
  for (i = 1; i < L7_MAX_INTERFACE_COUNT_REL_4_0; i++)
  {
    rc = sysapiCfgFileRel4_0IndexToIntfInfoGet (i, &configId, L7_NULL, &intfType);
    if (L7_SUCCESS != rc)
    {
      if (L7_ERROR == rc)
        MIGRATE_INVALID_INDEX (i);
      continue;
    }

    /* Determine if the interface is valid for participation in this feature */
    if (garpIsValidIntfType (intfType) != L7_TRUE)
      continue;

    if (intfCfgIndex >= L7_GARP_MAX_INTF_REL_4_1)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (GARP_CFG_FILENAME, intfCfgIndex);
      break;
    }

    (void) nimConfigIdCopy (&configId, &pCfgV2->intf[intfCfgIndex].configId);

    pCfgV2->intf[intfCfgIndex].join_time = pCfgV1->cfg.join_time[i];
    pCfgV2->intf[intfCfgIndex].leave_time = pCfgV1->cfg.leave_time[i];
    pCfgV2->intf[intfCfgIndex].leaveall_time = pCfgV1->cfg.leaveall_time[i];
    intfCfgIndex++;
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
garpMigrateConfigV2V3Convert (garpCfgV2_t * pCfgV2, garpCfgV3_t * pCfgV3)
{
  L7_int32 i, intIfCount;
  L7_uint32 ifConfigIndex;

  /* verify correct version of old config file */
  if ((pCfgV2->hdr.version != GARP_CFG_VER_2) && (pCfgV2->hdr.version != GARP_CFG_VER_1) &&     /* b'cos REL_H gives wrong version */
      (pCfgV2->hdr.version != GARP_CFG_VER_0))  /* b'cos REL_G gives wrong version */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->hdr.version, GARP_CFG_VER_2);
    return L7_FAILURE;
  }

  garpBuildDefaultConfigDataV3 (pCfgV3);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV3->gvrp_enabled = pCfgV2->gvrp_enabled;
  pCfgV3->gmrp_enabled = pCfgV2->gmrp_enabled;


  ifConfigIndex = 1;
  intIfCount = min (L7_GARP_MAX_INTF_REL_4_1, L7_GARP_MAX_INTF_REL_4_3);
  for (i = 0; i < intIfCount; i++)
  {
    MIGRATE_COPY_STRUCT (pCfgV3->intf[i], pCfgV2->intf[i]); 
    if (ifConfigIndex >= L7_GARP_MAX_INTF_REL_4_3)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (GARP_CFG_FILENAME, ifConfigIndex);
      break;
    }
    ifConfigIndex++;
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
garpMigrateConfigV3V4Convert (garpCfgV3_t * pCfgV3, garpCfgV4_t * pCfgV4)
{
  L7_int32 i, j, intIfCount;
  L7_uint32 ifConfigIndex;
  dot1qCfgV4_t *pDot1qCfg=L7_NULLPTR;
  L7_BOOL   dot1qCfgFetched;

  /* verify correct version of old config file */
  if ((pCfgV3->hdr.version != GARP_CFG_VER_3) &&
      (pCfgV3->hdr.version != GARP_CFG_VER_2) &&
      (pCfgV3->hdr.version != GARP_CFG_VER_1) &&     /* b'cos REL_H gives wrong version */
      (pCfgV3->hdr.version != GARP_CFG_VER_0))  /* b'cos REL_G gives wrong version */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->hdr.version, GARP_CFG_VER_3);
    return L7_FAILURE;
  }

  /*-------------------------------------------------------------------*/
  /* Between release 4.3 and 4.4, parms were moved from dot1qCfg
     to garpCfg.  The release 4.3 dot1qCfg file must be read in
     order to map the config parms. 
   */
  /*-------------------------------------------------------------------*/
  dot1qCfgFetched = garpMigrateRel4_3ParmsFromDot1q(&pDot1qCfg);


  /* Map fields of the GARP config file */
  garpBuildDefaultConfigDataV4 (pCfgV4);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV4->gvrp_enabled = pCfgV3->gvrp_enabled;
  pCfgV4->gmrp_enabled = pCfgV3->gmrp_enabled;


  ifConfigIndex = 1;
  intIfCount = min (L7_GARP_MAX_INTF_REL_4_3, L7_GARP_MAX_INTF_REL_4_4);
  for (i = 0; i < intIfCount; i++)
  {

    MIGRATE_COPY_STRUCT (pCfgV4->intf[i].configId, pCfgV3->intf[i].configId); 
    pCfgV4->intf[i].join_time = pCfgV3->intf[i].join_time;
    pCfgV4->intf[i].leave_time = pCfgV3->intf[i].leave_time;
    pCfgV4->intf[i].leaveall_time = pCfgV3->intf[i].leaveall_time;

    /* The following parameters were moved from dot1qCfg to garpCfg 
       in release 4.4:
       
       gmrp_enabled
       gvrp_enabled
       
    */
    if (dot1qCfgFetched == L7_TRUE) 
    {
        for (j = 0; j < L7_DOT1Q_MAX_INTF_REL_4_3; j++) 
        {

            if (NIM_CONFIG_ID_IS_EQUAL(&pCfgV4->intf[i].configId, 
                                       &pDot1qCfg->Qports[j].configId))
            {
                pCfgV4->intf[i].gmrp_enabled = pDot1qCfg->Qports[j].gmrp_enabled;
                pCfgV4->intf[i].gvrp_enabled = pDot1qCfg->Qports[j].gvrp_enabled;
                break;
            }

        }
              
    }

    /* Check for truncation */
    if (ifConfigIndex >= L7_GARP_MAX_INTF_REL_4_4)
    {
      MIGRATE_LOG_INTERFACE_TRUNCATE (GARP_CFG_FILENAME, ifConfigIndex);
      break;
    }
    ifConfigIndex++;
  }  /*  i < intIfCount */

  if (pDot1qCfg!=L7_NULLPTR)
  {
    osapiFree(L7_GARP_COMPONENT_ID,pDot1qCfg);
    pDot1qCfg=L7_NULLPTR;
  }
  
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Access GARP parms which were previously stored in dot1q
*
* @param    pDot1qCfg    @b{(output)} ptr to dot1q cfg data for release 4.3
*
* @returns  none
*
* @end
*********************************************************************/
static L7_BOOL garpMigrateRel4_3ParmsFromDot1q(dot1qCfgV4_t **pDot1qCfg)
{
  L7_uchar8 *dot1qCfgBuf;
  L7_uchar8 *dot1qCfgBuf_4_2;
  dot1qCfgV4_t *dot1qCfg;
  dot1qCfgV3_t *dot1qCfg_4_2;  /* Version 3 was in release 4.2 */
  L7_uint32 dot1qCfgSize;
  L7_uint32 dot1qCfgCksum;
  L7_BOOL   cfgMapped;
  L7_int32  j, intIfCount;

  /* Initialization */
  dot1qCfgBuf       = L7_NULLPTR;
  dot1qCfgBuf_4_2   = L7_NULLPTR;
  dot1qCfgSize      = (L7_uint32) sizeof (dot1qCfgV4_t);
  dot1qCfgCksum     = 0;
  cfgMapped         = L7_FALSE;

  dot1qCfgBuf = (L7_uchar8 *) osapiMalloc (L7_GARP_COMPONENT_ID,
                                                (L7_uint32) sizeof (dot1qCfgV4_t));
  if (dot1qCfgBuf == L7_NULLPTR)
  {
    LOG_MSG("garpMigrateRel4_3Cfg () failed to allocate config buffer.");
    return L7_FALSE;
  }


  dot1qCfg = (dot1qCfgV4_t *)dot1qCfgBuf;

  /* Attempt to fetch release 4.3 (Version 4) of the dot1q configuration */
  if (sysapiCfgFileGet(L7_DOT1Q_COMPONENT_ID, DOT1Q_CFG_FILENAME, dot1qCfgBuf,
                       dot1qCfgSize, &dot1qCfgCksum, DOT1Q_CFG_VER_4, L7_NULL, 
                       L7_NULL) == L7_SUCCESS)    
  {
      LOG_MSG("Mapping version 4 dot1qCfg parms to garp.");
      cfgMapped = L7_TRUE;
  }
  else 
  {

      /* Release 4.3 (Version 4) of the dot1q configuration could not be fetched.
         Attempt to fetch release 4.2 (version 2) of the dot1q configuration.  
         (Note that there was no version 3) */

      dot1qCfgBuf_4_2 = (L7_uchar8 *) osapiMalloc (L7_GARP_COMPONENT_ID,
                                                    (L7_uint32) sizeof (dot1qCfgV3_t));
      if (sysapiCfgFileGet(L7_DOT1Q_COMPONENT_ID, DOT1Q_CFG_FILENAME, dot1qCfgBuf_4_2,
                                 (L7_uint32)sizeof(dot1qCfgV3_t), &dot1qCfgCksum, DOT1Q_CFG_VER_2, L7_NULL, 
                                 L7_NULL) == L7_SUCCESS)
      {
          LOG_MSG("Mapping version 2 dot1qCfg parms to garp.");
          /* Copy relevant 4.2 config parms into 4.3 config structure */

          dot1qCfg_4_2 = (dot1qCfgV3_t *)dot1qCfgBuf_4_2;
          intIfCount = min(L7_DOT1Q_MAX_INTF_REL_4_3,L7_DOT1Q_MAX_INTF_REL_4_1);
          for (j = 0; j < intIfCount; j++) 
          {

              NIM_CONFIG_ID_COPY(&dot1qCfg->Qports[j].configId,
                                 &dot1qCfg_4_2->Qports[j].configId);
              
              dot1qCfg->Qports[j].gmrp_enabled = dot1qCfg_4_2->Qports[j].gmrp_enabled;
              dot1qCfg->Qports[j].gvrp_enabled = dot1qCfg_4_2->Qports[j].gvrp_enabled;
          }

          cfgMapped = L7_TRUE;

      }
      else
      {
          LOG_MSG("Failed to map any version of dot1qCfg parms to garp.");
      }


  } /* sysapiCfgFileGet(...DOT1Q_CFG_VER_4...) */


  /* Free memory and setup for return */
  if (dot1qCfgBuf_4_2 != L7_NULLPTR)
      osapiFree(L7_GARP_COMPONENT_ID,dot1qCfgBuf_4_2);

  /* Free memory */
  if (cfgMapped == L7_TRUE) 
  {
      *pDot1qCfg = (dot1qCfgV4_t *)dot1qCfgBuf;

  }

  return cfgMapped;
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
garpBuildDefaultConfigDataV2 (garpCfgV2_t * pCfgV2)
{
  memset (pCfgV2, 0, sizeof (*pCfgV2));

  pCfgV2->gvrp_enabled = L7_FALSE;
  pCfgV2->gmrp_enabled = L7_FALSE;

  strcpy (pCfgV2->hdr.filename, GARP_CFG_FILENAME);
  pCfgV2->hdr.version = GARP_CFG_VER_2;
  pCfgV2->hdr.componentID = L7_GARP_COMPONENT_ID;
  pCfgV2->hdr.type = L7_CFG_DATA;
  pCfgV2->hdr.length = sizeof (*pCfgV2);
  pCfgV2->hdr.dataChanged = L7_FALSE;
  pCfgV2->hdr.savePointId = 0;
  pCfgV2->hdr.targetDevice = 0;
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
garpBuildDefaultConfigDataV3 (garpCfgV3_t * pCfgV3)
{
  memset (pCfgV3, 0, sizeof (*pCfgV3));

  pCfgV3->gvrp_enabled = L7_FALSE;
  pCfgV3->gmrp_enabled = L7_FALSE;

  strcpy (pCfgV3->hdr.filename, GARP_CFG_FILENAME);
  pCfgV3->hdr.version = GARP_CFG_VER_3;
  pCfgV3->hdr.componentID = L7_GARP_COMPONENT_ID;
  pCfgV3->hdr.type = L7_CFG_DATA;
  pCfgV3->hdr.length = sizeof (*pCfgV3);
  pCfgV3->hdr.dataChanged = L7_FALSE;
  pCfgV3->hdr.savePointId = 0;
  pCfgV3->hdr.targetDevice = 0;
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
garpBuildDefaultConfigDataV4 (garpCfgV4_t * pCfgV4)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (garpCfgV4_t));
  garpBuildDefaultConfigData (GARP_CFG_VER_4);
  memcpy ((L7_uchar8 *) pCfgV4, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}

#ifdef SYSAPI_MIGRATE_TEST_DATA
#include "garp_test.c"
#endif



