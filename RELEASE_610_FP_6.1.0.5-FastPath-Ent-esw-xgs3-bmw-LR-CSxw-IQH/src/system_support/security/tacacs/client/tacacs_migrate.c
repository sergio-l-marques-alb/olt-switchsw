/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename tacacs_migrate.c
*
* @purpose Tacacs Configuration Migration
*
* @component Tacacs
*
* @comments  none
*
* @create  02/14/2007
*
* @author  I. Kiran
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "tacacs_cfg.h"
#include "tacacs_migrate.h"

/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static tacacsCfg_t *pCfgCur = L7_NULLPTR;


static L7_RC_t tacacsMigrateConfigV1V2Convert (tacacsCfgV1_t * pCfgV1, tacacsCfgV2_t * pCfgV2);
static void tacacsBuildDefaultConfigDataV2 (tacacsCfgV2_t *pCfgV2);


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
tacacsMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  tacacsCfgV1_t *pCfgV1 = L7_NULLPTR;
  tacacsCfgV2_t *pCfgV2 = L7_NULLPTR;

  pCfgCur = (tacacsCfg_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));
  
  switch (oldVer)
  {
  case L7_TACACS_VER_1:
    pCfgV1 = (tacacsCfgV1_t *) osapiMalloc (L7_TACACS_COMPONENT_ID,
                                           (L7_uint32) sizeof (tacacsCfgV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (tacacsCfgV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = oldVer;
    break;

  case L7_TACACS_VER_2:
    pCfgV2 = (tacacsCfgV2_t *) osapiMalloc (L7_TACACS_COMPONENT_ID,
                                           (L7_uint32) sizeof (tacacsCfgV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (tacacsCfgV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = L7_TACACS_VER_2;
    break;

  default:
    buildDefault = L7_TRUE;
    oldCfgSize = 0;
    pOldCfgCksum = 0;
    oldCfgVer = 0;
    break;
  }

  if (buildDefault != L7_TRUE)
  {
    /* read the old config file found on this system */
    if ((L7_NULLPTR == pOldCfgBuf) ||
        (L7_SUCCESS != sysapiCfgFileGet (L7_TACACS_COMPONENT_ID,
                                         L7_TACACS_CFG_FILE_NAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_TACACS_COMPONENT_ID,
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
    case L7_TACACS_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (tacacsCfgV2_t *) osapiMalloc (L7_TACACS_COMPONENT_ID,
                                             (L7_uint32) sizeof (tacacsCfgV2_t));
      if ((L7_NULLPTR == pCfgV2) || (L7_SUCCESS != tacacsMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_TACACS_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case L7_TACACS_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV2, sizeof (*pCfgCur));
      osapiFree (L7_TACACS_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;

      /* done with migration - Flag unsaved configuration */
      pCfgCur->header.dataChanged = L7_TRUE;
      break;

    default:
      buildDefault = L7_TRUE;
      break;
    }
  }

  /* clean up any left over memory allocations from above */
  if (pCfgV1 != L7_NULLPTR)
    osapiFree (L7_TACACS_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_TACACS_COMPONENT_ID, pCfgV2);

  if (buildDefault == L7_TRUE)
  {
    MIGRATE_DEBUG ("Building Defaults\n");
    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    tacacsBuildDefaultConfigData (L7_TACACS_CFG_VER_CURRENT);
    pCfgCur->header.dataChanged = L7_TRUE;
  }
  return;
}

/*********************************************************************
* @purpose  Converts the config data structure from version v1 to v2
*
* @param    pCfgV1    @b{(input)} ptr to version 1 config data structure
* @param    pCfgV2   @b{(input)} ptr to version 2 config data structure
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
tacacsMigrateConfigV1V2Convert (tacacsCfgV1_t * pCfgV1, tacacsCfgV2_t * pCfgV2)
{
  L7_uint32 i;

  /* verify correct version of old config file */
  if (pCfgV1->header.version != L7_TACACS_VER_1 &&
      pCfgV1->header.version != L7_TACACS_VER_2)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->header.version, L7_TACACS_VER_2);
    return L7_FAILURE;
  }

  tacacsBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */
  pCfgV2->timeout = pCfgV1->timeout;
  pCfgV2->sourceIP = pCfgV1->sourceIP;
  strcpy(pCfgV2->key, pCfgV1->key);
  
  for (i = 0; i < L7_TACACS_MAX_SERVERS; i++)
  {
    pCfgV2->servers[i].ip = pCfgV1->servers[i].ip;
    memset(&pCfgV2->servers[i].hostName, 0, sizeof(dnsHost_t));
    if(pCfgV2->servers[i].ip != 0)
    {
      pCfgV2->servers[i].hostName.hostAddrType = L7_IP_ADDRESS_TYPE_IPV4;
    }
    pCfgV2->servers[i].port = pCfgV1->servers[i].port;
    pCfgV2->servers[i].priority = pCfgV1->servers[i].priority;
    pCfgV2->servers[i].singleConnection = pCfgV1->servers[i].singleConnection;
    pCfgV2->servers[i].timeout = pCfgV1->servers[i].timeout;
    pCfgV2->servers[i].sourceIP = pCfgV1->servers[i].sourceIP;
    strcpy(pCfgV2->servers[i].key, pCfgV1->servers[i].key);
    pCfgV2->servers[i].useGblTimeout = pCfgV1->servers[i].useGblTimeout;
    pCfgV2->servers[i].useGblSourceIP = pCfgV1->servers[i].useGblSourceIP;
    pCfgV2->servers[i].useGblKey = pCfgV1->servers[i].useGblKey;
  }
  return L7_SUCCESS; 
}


/*********************************************************************
* @purpose  Build Version 2 defaults
*
* @param    pCfgV1    @b{(input)} ptr to version 3 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
tacacsBuildDefaultConfigDataV2 (tacacsCfgV2_t * pCfgV2)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (tacacsCfgV2_t));
  tacacsBuildDefaultConfigData (L7_TACACS_VER_2);
  memcpy ((L7_uchar8 *) pCfgV2, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}
