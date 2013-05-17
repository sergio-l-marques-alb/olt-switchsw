
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
*
* @filename    mirror_migrate.c
* @purpose     Port mirroring Configuration Migration
* @component   mirroring
* @comments    none
* @create      08/24/2004
* @author      Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#include <string.h>
#include "mirror_migrate.h"

extern  L7_BOOL nimConfigIdTreeIsPopulated(void);

static L7_RC_t mirrorMigrateConfigV1V2Convert (mirrorCfgDataV1_t * pCfgV1,
                                               mirrorCfgDataV2_t * pCfgV2);
static L7_RC_t mirrorMigrateConfigV2V3Convert (mirrorCfgDataV2_t * pCfgV2,
                                               mirrorCfgDataV3_t * pCfgV3);
static L7_RC_t mirrorMigrateConfigV3V4Convert (mirrorCfgDataV3_t * pCfgV3,
                                               mirrorCfgDataV4_t * pCfgV4);
static void mirrorBuildDefaultConfigDataV2 (mirrorCfgDataV2_t * pCfgV2);
static void mirrorBuildDefaultConfigDataV3 (mirrorCfgDataV3_t * pCfgV3);
static void mirrorBuildDefaultConfigDataV4 (mirrorCfgDataV4_t * pCfgV4);


/* We need to hold buffer passed get access defaults when BuildDefault called in convert */
static mirrorCfgData_t *pCfgCur = L7_NULLPTR;

extern void *intfListSema;  /* semaphore to access interface list array*/
extern L7_uint32 intfList[];


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
mirrorMigrateConfigData (L7_uint32 oldVer, L7_uint32 ver, L7_char8 * pCfgBuffer)
{
  L7_fileHdr_t *pCfgHdr;
  L7_BOOL buildDefault = L7_FALSE;
  L7_uchar8 *pOldCfgBuf = L7_NULLPTR;
  L7_uint32 oldCfgSize = 0;
  L7_uint32 *pOldCfgCksum = L7_NULLPTR;
  L7_uint32 oldCfgVer = 0;

  /* declare a pointer to each supported config structure version */
  mirrorCfgDataV1_t *pCfgV1 = L7_NULLPTR;
  mirrorCfgDataV2_t *pCfgV2 = L7_NULLPTR;
  mirrorCfgDataV3_t *pCfgV3 = L7_NULLPTR;
  mirrorCfgDataV4_t *pCfgV4 = L7_NULLPTR;

  pCfgCur = (mirrorCfgData_t *) pCfgBuffer;
  pCfgHdr = (L7_fileHdr_t *) pCfgBuffer;
  MIGRATE_FUNCTION (oldVer, ver, sizeof (*pCfgCur));
  /* testing config migration */
  MIRROR_TRACE("\n\nmirrorMigrateConfigData::oldver; %d, newver;%d\n",oldVer,ver);


  if (pCfgHdr->version == MIRROR_CFG_VER_1 && pCfgHdr->length == sizeof (mirrorCfgDataV2_t))
    oldVer = MIRROR_CFG_VER_2;


  /* Attempt to read the old version config file and migrate it to the current
   * version.  If not successful, build a default config of the latest version.
   */

  switch (oldVer)
  {
  case MIRROR_CFG_VER_1:
    pCfgV1 = (mirrorCfgDataV1_t *) osapiMalloc (L7_PORT_MIRROR_COMPONENT_ID,
                                                (L7_uint32) sizeof (mirrorCfgDataV1_t));
    if (L7_NULLPTR == pCfgV1)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV1;
    oldCfgSize = (L7_uint32) sizeof (mirrorCfgDataV1_t);
    pOldCfgCksum = &pCfgV1->checkSum;
    oldCfgVer = MIRROR_CFG_VER_1;
    break;

  case MIRROR_CFG_VER_2:
    pCfgV2 = (mirrorCfgDataV2_t *) osapiMalloc (L7_PORT_MIRROR_COMPONENT_ID,
                                                (L7_uint32) sizeof (mirrorCfgDataV2_t));
    if (L7_NULLPTR == pCfgV2)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV2;
    oldCfgSize = (L7_uint32) sizeof (mirrorCfgDataV2_t);
    pOldCfgCksum = &pCfgV2->checkSum;
    oldCfgVer = MIRROR_CFG_VER_1;       /* b'cos: REL_H version was not updated */
    break;

  case MIRROR_CFG_VER_3:
    pCfgV3 = (mirrorCfgDataV3_t *) osapiMalloc (L7_PORT_MIRROR_COMPONENT_ID,
                                                (L7_uint32) sizeof (mirrorCfgDataV3_t));
    if (L7_NULLPTR == pCfgV3)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV3;
    oldCfgSize = (L7_uint32) sizeof (mirrorCfgDataV3_t);
    pOldCfgCksum = &pCfgV3->checkSum;
    oldCfgVer = MIRROR_CFG_VER_3;
    break;

  case MIRROR_CFG_VER_4:
    pCfgV4 = (mirrorCfgDataV4_t *) osapiMalloc (L7_PORT_MIRROR_COMPONENT_ID,
                                                (L7_uint32) sizeof (mirrorCfgDataV4_t));
    if (L7_NULLPTR == pCfgV4)
    {
      buildDefault = L7_TRUE;
      break;
    }

    pOldCfgBuf = (L7_uchar8 *) pCfgV4;
    oldCfgSize = (L7_uint32) sizeof (mirrorCfgDataV4_t);
    pOldCfgCksum = &pCfgV4->checkSum;
    oldCfgVer = MIRROR_CFG_VER_4;
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
        (L7_SUCCESS != sysapiCfgFileGet (L7_PORT_MIRROR_COMPONENT_ID,
                                         MIRROR_CFG_FILENAME,
                                         pOldCfgBuf, oldCfgSize, pOldCfgCksum,
                                         oldCfgVer, L7_NULL /* do not build defaults */ ,
                                         L7_NULL        /* do not call migrate again */
         )))
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_PORT_MIRROR_COMPONENT_ID,
                     "sysapiCfgFileGet failed size = %d version = %d."
                     " Configuration did not exist or could not be read"
                     " for the specified feature.  This message is usually"
                     " followed by a message indicating that default configuration"
                     " values will be used."
                     , (L7_int32) oldCfgSize,
                     (L7_int32) oldCfgVer);
      /* testing config migration */
      MIRROR_TRACE("\n\nsysapiCfgFileGet failed size = %d version = %d\n", (L7_int32) oldCfgSize,
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
    case MIRROR_CFG_VER_1:
      /* pCfgV1 has already been allocated and filled in */
      pCfgV2 = (mirrorCfgDataV2_t *) osapiMalloc (L7_PORT_MIRROR_COMPONENT_ID,
                                                  (L7_uint32) sizeof (mirrorCfgDataV2_t));
      /* add check for pCfgV1 != Null --klockworks error*/
      if ((L7_NULLPTR == pCfgV1) || (L7_NULLPTR == pCfgV2) ||
          (L7_SUCCESS != mirrorMigrateConfigV1V2Convert (pCfgV1, pCfgV2)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_PORT_MIRROR_COMPONENT_ID, pCfgV1);
      pCfgV1 = L7_NULLPTR;
      /*passthru */

    case MIRROR_CFG_VER_2:
      /* pCfgV2 has already been allocated and filled in */
      pCfgV3 = (mirrorCfgDataV3_t *) osapiMalloc (L7_PORT_MIRROR_COMPONENT_ID,
                                        (L7_uint32) sizeof (mirrorCfgDataV3_t));

      /* add check for pCfgV2 != Null --klockworks error*/
      if ((L7_NULLPTR == pCfgV2) || (L7_NULLPTR == pCfgV3) ||
          (L7_SUCCESS != mirrorMigrateConfigV2V3Convert (pCfgV2, pCfgV3)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_PORT_MIRROR_COMPONENT_ID, pCfgV2);
      pCfgV2 = L7_NULLPTR;
      /*passthru */

    case MIRROR_CFG_VER_3:
      /* pCfgV3 has already been allocated and filled in */
      pCfgV4 = (mirrorCfgDataV4_t *) osapiMalloc (L7_PORT_MIRROR_COMPONENT_ID,
                                                  (L7_uint32) sizeof (mirrorCfgDataV4_t));

      /* add check for pCfgV3 != Null --klockworks error*/
      if ((L7_NULLPTR == pCfgV3) || (L7_NULLPTR == pCfgV4) ||
          (L7_SUCCESS != mirrorMigrateConfigV3V4Convert (pCfgV3, pCfgV4)))
      {
        buildDefault = L7_TRUE;
        break;
      }
      osapiFree (L7_PORT_MIRROR_COMPONENT_ID, pCfgV3);
      pCfgV3 = L7_NULLPTR;
      /*passthru */

    case MIRROR_CFG_VER_4:
      /* pCfgV4 has already been allocated and filled in */
      /* add check for pCfgV4 != Null --klockworks error*/
      if (L7_NULLPTR == pCfgV4)
      {
        buildDefault = L7_TRUE;
        break;
      }
      memcpy ((L7_uchar8 *) pCfgCur, (L7_uchar8 *) pCfgV4, sizeof (*pCfgCur));
      osapiFree (L7_PORT_MIRROR_COMPONENT_ID, pCfgV4);
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
    osapiFree (L7_PORT_MIRROR_COMPONENT_ID, pCfgV1);
  if (pCfgV2 != L7_NULLPTR)
    osapiFree (L7_PORT_MIRROR_COMPONENT_ID, pCfgV2);
  if (pCfgV3 != L7_NULLPTR)
    osapiFree (L7_PORT_MIRROR_COMPONENT_ID, pCfgV3);
  if (pCfgV4 != L7_NULLPTR)
    osapiFree (L7_PORT_MIRROR_COMPONENT_ID, pCfgV4);

  if (buildDefault == L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MIRROR_COMPONENT_ID,
            "Building Defaults\n");
    /* testing config migration */
    MIRROR_TRACE("\n\nmirrorMigrateConfigData::oldver; %d, newver;%d. Building defaults\n",oldVer,ver);

    memset ((void *) pCfgCur, 0, sizeof (*pCfgCur));
    mirrorBuildDefaultConfigData (MIRROR_CFG_VER_CURRENT);
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
mirrorMigrateConfigV1V2Convert (mirrorCfgDataV1_t * pCfgV1, mirrorCfgDataV2_t * pCfgV2)
{
  L7_RC_t rc;
  L7_uint32 slot;
  L7_uint32 port;
  nimConfigID_t configId;
  L7_uint32  configIdOffset;
  L7_INTF_TYPES_t sysIntfType;



  /* verify correct version of old config file */
  if (pCfgV1->cfgHdr.version != MIRROR_CFG_VER_1)
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV1->cfgHdr.version, MIRROR_CFG_VER_1);
    return L7_FAILURE;
  }

  mirrorBuildDefaultConfigDataV2 (pCfgV2);

  /* Map the fields in the older structure to the appropriate fields in the newer one */

  /*------------------------*/
  /* Migrate Src  interface */
  /*------------------------*/
  slot = pCfgV1->mirrorSourceUSP.slot;
  port = pCfgV1->mirrorSourceUSP.port;

  rc = sysapiCfgFileRel4_0SlotPortToIntfInfoGet (slot, port, &configId,
                                                 &configIdOffset, &sysIntfType);
  if (L7_SUCCESS != rc)
  {
    if (L7_ERROR == rc)
      MIGRATE_INVALID_SLOT_PORT (slot, port);
  }

  /* Hack:  Need mirrorIsValidIntfType routines for both source and dest */
  if (sysIntfType != L7_PHYSICAL_INTF)
  {

      /* Do not log a message for default configuration of zero */
      if (sysIntfType != 0)
      {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MIRROR_COMPONENT_ID,
                  "Cannot migrate source interface %d/%d: invalid type (%d)\n",
                  slot, port, sysIntfType);
      }
  }
  else
      (void) nimConfigIdCopy (&configId, &pCfgV2->mirrorSourceID);


  /*------------------------*/
  /* Migrate Dest interface */
  /*------------------------*/
  slot = pCfgV1->mirrorDestUSP.slot;
  port = pCfgV1->mirrorDestUSP.port;

  rc = sysapiCfgFileRel4_0SlotPortToIntfInfoGet (slot, port, &configId,
                                                 &configIdOffset, &sysIntfType);
  if (L7_SUCCESS != rc)
  {
    if (L7_ERROR == rc)
      MIGRATE_INVALID_SLOT_PORT (slot, port);
  }

  /* Hack:  Need mirrorIsValidIntfType routines for both source and dest */
  if (sysIntfType != L7_PHYSICAL_INTF)
  {

      /* Do not log a message for default configuration of zero */
      if (sysIntfType != 0)
      {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MIRROR_COMPONENT_ID,
                  "Cannot migrate destination slot/port %d/%d: invalid type (%d)\n",
                  slot, port, sysIntfType);
      }
  }
  else
  (void) nimConfigIdCopy (&configId, &pCfgV2->mirrorDestID);

  /*------------------------*/
  /* Migrate Mirror Mode    */
  /*------------------------*/

  pCfgV2->mirrorMode = pCfgV1->mirrorMode;

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
mirrorMigrateConfigV2V3Convert (mirrorCfgDataV2_t * pCfgV2, mirrorCfgDataV3_t * pCfgV3)
{

    L7_uint32  configIdOffset;
    L7_INTF_TYPES_t sysIntfType;


  /* verify correct version of old config file */
  if (pCfgV2->cfgHdr.version != MIRROR_CFG_VER_2 && pCfgV2->cfgHdr.version != MIRROR_CFG_VER_1) /* b'cos: REL_H version was not updated */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV2->cfgHdr.version, MIRROR_CFG_VER_2);
    return L7_FAILURE;
  }


  mirrorBuildDefaultConfigDataV3 (pCfgV3);

  /*------------------------*/
  /* Migrate Src interface  */
  /*------------------------*/

  /* Hack:  Need mirrorIsValidIntfType routines for both source and dest */
  sysIntfType = pCfgV2->mirrorSourceID.type;
  if (sysIntfType != L7_PHYSICAL_INTF)
  {

      /* Do not log a message for default configuration of zero */
      if (sysIntfType != 0)
      {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MIRROR_COMPONENT_ID,
                  "Cannot migrate source interface : invalid type (%d)\n", sysIntfType);
      }
  }
  else
  {
      /* Hack:  Need to move reading of NIM configuration before cnfgr phased init
                in order to facilitate migrations */

      while (nimConfigIdTreeIsPopulated() == L7_FALSE)
      {
          osapiSleepMSec(10);
      }


      if (nimIntIfFromConfigIDGet(&pCfgV2->mirrorSourceID, &configIdOffset) == L7_SUCCESS)
      {
          L7_INTF_SETMASKBIT (pCfgV3->sessionData[0].srcPorts,  configIdOffset);

      }

   }


  /*------------------------*/
  /* Migrate Dest interface */
  /*------------------------*/


  sysIntfType = pCfgV2->mirrorDestID.type;
  if (sysIntfType != L7_PHYSICAL_INTF)
  {
      /* Do not log a message for default configuration of zero */
      if (sysIntfType != 0)
      {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MIRROR_COMPONENT_ID,
                  "Cannot migrate dest interface : invalid type (%d)\n", sysIntfType);
      }
  }
  else
  {
      (void) nimConfigIdCopy (&pCfgV2->mirrorDestID, &pCfgV3->sessionData[0].destID);
  }


  /*------------------------*/
  /* Migrate Mirror Mode    */
  /*------------------------*/



  MIGRATE_COPY_STRUCT (pCfgV3->sessionData[0].destID, pCfgV2->mirrorDestID);
  pCfgV3->sessionData[0].mode = pCfgV2->mirrorMode;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Converts the config data structure from version V3 to V4.
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
mirrorMigrateConfigV3V4Convert (mirrorCfgDataV3_t * pCfgV3, mirrorCfgDataV4_t * pCfgV4)
{

    L7_uint32 maskOffset;
    L7_uint32 sessionIndex;
    L7_uint32 numSrcIntf=0, i;
    L7_INTF_MASK_t temp_src_lst;

  /* verify correct version of old config file */
  if (pCfgV3->cfgHdr.version != MIRROR_CFG_VER_3)
      /*&& pCfgV3->cfgHdr.version != MIRROR_CFG_VER_1)*/ /* b'cos: REL_H version was not updated */
  {
    MIGRATE_LOG_UNEXPECTED_VERSION (pCfgV3->cfgHdr.version, MIRROR_CFG_VER_3);
    return L7_FAILURE;
  }


  mirrorBuildDefaultConfigDataV4 (pCfgV4);

  /*------------------------*/
  /* Migrate Src interfaces */
  /*------------------------*/

  /* Hack:  Need to move reading of NIM configuration before cnfgr phased init
           in order to facilitate migrations
    */

  while (nimConfigIdTreeIsPopulated() == L7_FALSE)
  {
     osapiSleepMSec(10);
  }

  /* take interfae list semaphore*/
  osapiSemaTake(intfListSema,L7_NO_WAIT);


  for(sessionIndex=1;sessionIndex<= L7_MIRRORING_MAX_SESSIONS; sessionIndex++)
  {
     /* migrate Interface List mask from Rel 3 to 4 */
    memset(&temp_src_lst.value ,0,sizeof(L7_INTF_MASK_t));
    MIGRATE_COPY_NIM_INTF_MASK(temp_src_lst,pCfgV3->sessionData[sessionIndex-1].srcPorts);
    /* add direction per source interface */
   nimMaskToList((NIM_INTF_MASK_t *)&temp_src_lst,intfList,&(numSrcIntf));

    /* check if number of source interfaces configured greater
       than maximum source interfaces allowed per session */
    if (numSrcIntf > L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MIRROR_COMPONENT_ID,
              "\nMirror Config Migrate V3 to V4: The Number of source ports set %d\
              is greater than maximum allowed source ports per session %d.\
              Considering only the first %d source ports.\n",numSrcIntf,L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION,
              L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION);
      numSrcIntf =  L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION;
    }

    for (i = 0; i < numSrcIntf; i++ )
    {
      /* set each source port in version 4*/
      if ((nimConfigIdMaskOffsetGet(intfList[i],&maskOffset) )!= L7_SUCCESS)
      {
        L7_INTF_SETMASKBIT (pCfgV4->sessionData[sessionIndex-1].srcPorts,intfList[i]);
        /* set direction for each source port in ver 4 as bidirectional*/
        L7_INTF_SETMASKBIT(pCfgV4->sessionData[sessionIndex-1].srcIngressMirrorType,intfList[i]);
        L7_INTF_SETMASKBIT(pCfgV4->sessionData[sessionIndex-1].srcEgressMirrorType,  intfList[i]);
      }
      else
      {
        L7_INTF_SETMASKBIT (pCfgV4->sessionData[sessionIndex-1].srcPorts,maskOffset);
        /* set direction for each source port in ver 4 as bidirectional*/
        L7_INTF_SETMASKBIT(pCfgV4->sessionData[sessionIndex-1].srcIngressMirrorType,  maskOffset);
        L7_INTF_SETMASKBIT(pCfgV4->sessionData[sessionIndex-1].srcEgressMirrorType,  maskOffset);
      }

    }/* for--numSrcIntf*/

    /*------------------------*/
    /* Migrate Dest interface */
    /*------------------------*/
     (void) nimConfigIdCopy (&pCfgV3->sessionData[sessionIndex-1].destID,
                             &pCfgV4->sessionData[sessionIndex-1].destID);

     /*------------------------*/
     /* Migrate Mirror Mode    */
     /*------------------------*/

     pCfgV4->sessionData[sessionIndex-1].mode = pCfgV3->sessionData[sessionIndex-1].mode;


  } /*session Index*/

  /* release interface list semphore*/
  osapiSemaGive(intfListSema);
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
mirrorBuildDefaultConfigDataV2 (mirrorCfgDataV2_t * pCfgV2)
{
  memset (pCfgV2, 0, sizeof (*pCfgV2));
  strcpy ((char *) pCfgV2->cfgHdr.filename, MIRROR_CFG_FILENAME);
  pCfgV2->cfgHdr.version = MIRROR_CFG_VER_2;
  pCfgV2->cfgHdr.componentID = L7_PORT_MIRROR_COMPONENT_ID;
  pCfgV2->cfgHdr.type = L7_CFG_DATA;
  pCfgV2->cfgHdr.length = (L7_uint32) sizeof (*pCfgV2);
  pCfgV2->cfgHdr.dataChanged = L7_FALSE;
  pCfgV2->mirrorMode = L7_DISABLE;
}

/*********************************************************************
* @purpose  Build Version 3 defaults
*
* @param    ver   @b{(input)} Software version of Config Data
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
static void mirrorBuildDefaultConfigDataV3(mirrorCfgDataV3_t * pCfgV3)
{
  L7_uint32 sessionIndex;

  memset((void *)pCfgV3, 0, sizeof(mirrorCfgDataV3_t));
  strcpy((char *)pCfgV3->cfgHdr.filename, MIRROR_CFG_FILENAME);
  pCfgV3->cfgHdr.version = MIRROR_CFG_VER_3;
  pCfgV3->cfgHdr.componentID = L7_PORT_MIRROR_COMPONENT_ID;
  pCfgV3->cfgHdr.type = L7_CFG_DATA;
  pCfgV3->cfgHdr.length = (L7_uint32)sizeof(*pCfgV3);
  pCfgV3->cfgHdr.dataChanged = L7_FALSE;

  for(sessionIndex = 1; sessionIndex <= L7_MIRRORING_MAX_SESSIONS; sessionIndex++)
  {
    pCfgV3->sessionData[sessionIndex - 1].probeType = FD_MIRRORING_DIRECTION;
    pCfgV3->sessionData[sessionIndex - 1].mode = FD_MIRRORING_MODE;
  }
}

/*********************************************************************
* @purpose  Build Version 4 defaults
*
* @param    pCfgV3    @b{(input)} ptr to version 3 config data structure
*
* @returns  none
*
* @end
*********************************************************************/
static void
mirrorBuildDefaultConfigDataV4 (mirrorCfgDataV4_t * pCfgV4)
{
  memset ((L7_uchar8 *) pCfgCur, 0, sizeof (mirrorCfgDataV4_t));
  mirrorBuildDefaultConfigData (MIRROR_CFG_VER_4);
  memcpy ((L7_uchar8 *) pCfgV4, (L7_uchar8 *) pCfgCur, sizeof (*pCfgCur));
}
