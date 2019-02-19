/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2007
 *
 **********************************************************************
 *
 * @filename  captive_portal_cnfgr.c
 *
 * @purpose   Contains definitions to support the configurator API
 *
 * @component captive portal
 *
 * @comments
 *
 * @create    6/27/2007
 *
 * @author    dcaugherty, rjindal
 *
 * @end
 *
 **********************************************************************/

#include <string.h>
#include <dirent.h>

#include "commdefs.h"
#include "datatypes.h"
#include "sysapi.h"
#include "default_cnfgr.h"
#include "l7_cnfgr_api.h"
#include "nvstoreapi.h"
#include "osapi.h"
#include "platform_config.h"
#include "avl_api.h"
#include "wnv_api.h"
#include "usmdb_util_api.h"
#include "log.h"
#include "intf_cb_api.h"
#include "captive_portal_include.h"
#include "captive_portal_config.h"
#include "captive_portal_cnfgr.h"
#include "captive_portal_debug.h"
#include "cpdm_api.h"
#include "cpdm_user_api.h"
#include "cpdm_connstatus_api.h"
#include "cpdm_web_api.h"
#include "radius_api.h"
#include "clustering_api.h"
#include "cp_cluster.h"
#include "cp_cluster_msg.h"
#include "sslt_exports.h"

/* Syntactic sugar */
#define SEMA_TAKE(access)   \
  if ( CPDM_##access##_LOCK_TAKE(cpdmSema, L7_WAIT_FOREVER) != L7_SUCCESS)  LOG_ERROR(0);  
#define SEMA_GIVE(access)   \
  if ( CPDM_##access##_LOCK_GIVE(cpdmSema) != L7_SUCCESS)  LOG_ERROR(0);  

cpdmCfgData_t            *cpdmCfgData        = (cpdmCfgData_t *) L7_NULLPTR;
cpdmCfgData_t            *cpdmCfgDataToStore = (cpdmCfgData_t *) L7_NULLPTR;
cpdmOprData_t            *cpdmOprData        = (cpdmOprData_t *) L7_NULLPTR;
cpdmRAMCfgDataToStore_t  *cpdmRAMCfgDataToStore= (cpdmRAMCfgDataToStore_t *) L7_NULLPTR;

L7_enetMacAddr_t         *resendPeer = L7_NULLPTR;

cpdmGlobal_t             *ptrCpGlobalData = L7_NULLPTR;
cpdmConfigData_t         *ptrCpConfigData = L7_NULLPTR;
L7_uint32 numCP          = 0;
cpLocaleKey_t            *ptrCpWebID = L7_NULLPTR;
cpLocaleCodeLink_t       *ptrCpCodeLink = L7_NULLPTR;
ifNumCPPair_t            *ptrCpInterfaceAssocData = L7_NULLPTR;
cpdmUserGroupData_t      *ptrCpUserGroupData = L7_NULLPTR;
L7_uint32 numGroup       = 0;
cpdmUser_t               *ptrCpUsers = L7_NULLPTR;
L7_uint32 numUser        = 0;
cpdmUserGroupAssocData_t *ptrCpUserGroupAssocData = L7_NULLPTR;


void *cpdmSema = (void *) L7_NULLPTR;

static L7_CNFGR_CMD_DATA_t savedCnfgrCommand;
static L7_BOOL             configCommandPending = L7_FALSE;

typedef L7_RC_t (*configPhaseFunc)(L7_CNFGR_CMD_DATA_t *, /* cmd (input)       */
                                    L7_CNFGR_RESPONSE_t *, /* response (output) */
                                   L7_CNFGR_ERR_RC_t   *  /* reason (output)   */  );
typedef void (*configPhaseFinalizer)(void);

static captivePortalCnfgrState_t captivePortalCnfgrState   = CAPTIVE_PORTAL_PHASE_INIT_0;


L7_BOOL captivePortalCnfgrComplete(void);

#if 0
static void cpConfigShow(cpdmCfgData_t * pC); /* Remove me from production builds! */
#endif 


void  
captivePortalApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData);

static L7_RC_t
captivePortalNOP(captivePortalCnfgrState_t, L7_CNFGR_RESPONSE_t *, L7_CNFGR_ERR_RC_t *);

static L7_RC_t
captivePortalInitPhase1Func(L7_CNFGR_CMD_DATA_t *, L7_CNFGR_RESPONSE_t *, L7_CNFGR_ERR_RC_t *);

static L7_RC_t
captivePortalInitPhase2Func(L7_CNFGR_CMD_DATA_t *, L7_CNFGR_RESPONSE_t *, L7_CNFGR_ERR_RC_t *);

static L7_RC_t
captivePortalInitPhase3Func(L7_CNFGR_CMD_DATA_t *, L7_CNFGR_RESPONSE_t *, L7_CNFGR_ERR_RC_t *);

static L7_RC_t
captivePortalInitWaitMUFunc(L7_CNFGR_CMD_DATA_t *, L7_CNFGR_RESPONSE_t *, L7_CNFGR_ERR_RC_t *);

static L7_RC_t
captivePortalExecStartFunc(L7_CNFGR_CMD_DATA_t *, L7_CNFGR_RESPONSE_t *, L7_CNFGR_ERR_RC_t *);

static L7_RC_t
captivePortalUncfgPhase1Func(L7_CNFGR_CMD_DATA_t *, L7_CNFGR_RESPONSE_t *, L7_CNFGR_ERR_RC_t *);

static L7_RC_t
captivePortalUncfgPhase2Func(L7_CNFGR_CMD_DATA_t *, L7_CNFGR_RESPONSE_t *, L7_CNFGR_ERR_RC_t *);

static void
captivePortalFiniPhase1Func(void);

static void
captivePortalFiniPhase2Func(void);

/* I know, I know, we hate extern declarations.  But do we really want anyone else knowing
 * about this function?
 */

extern L7_RC_t cpimIntfChangeCallback(L7_uint32 intIfNum, 
                                      L7_uint32 intfEvent, 
                                      NIM_CORRELATOR_t correlator);

extern L7_RC_t cpcmRadiusResponseCallback(L7_uint32 status,
                                          L7_uint32 correlator,
                                          L7_uchar8 *attributes,
                                          L7_uint32 attributesLen);

/* TODO: put this prototype in a ..util.h file when we re-structure code */
extern L7_RC_t cpdmCPConfigModeDisableForce(void);

extern L7_RC_t  cpdmIntfStatusNextGet(L7_uint32, L7_uint32 *);
extern L7_RC_t  cpimIntfDisable(L7_uint32);

extern void     cpdmActStatusFinalize(void);
extern void     cpdmCPConfigFinalize(void);
extern void     cpIntfAssocFinalize(void);
extern void     cpdmIntfStatusFinalize(void);
extern void     cpdmCACStatusFinalize(void);
extern void     cpdmUserGroupEntryFinalize(void);
extern void     cpdmUserGroupAssocEntryFinalize(void);

void i2b(L7_char8 *b, L7_uint32 i)
{
  *b   = (L7_char8)(i>>24);
  *++b = (L7_char8)(i>>16);
  *++b = (L7_char8)(i>>8);
  *++b = (L7_char8)i;
}

/*********************************************************************
*
* @purpose  To signify when the Configurator has finished its work
*           for Captive Portal
* 
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*
* @end
*********************************************************************/

L7_BOOL  captivePortalCnfgrComplete(void)
{
  L7_BOOL rc = L7_FALSE;
  switch( captivePortalCnfgrState )
  {
    /* case CAPTIVE_PORTAL_PHASE_INIT_3: */
    case CAPTIVE_PORTAL_PHASE_EXECUTE:
      /* case CAPTIVE_PORTAL_PHASE_UNCONFIG_1: */
      rc = L7_TRUE;
      break;
    default:
      break;
  }
  return rc;
}


/*********************************************************************
*
* @purpose  Copies configuration data to our private buffer
*
* @param    void
*
* @returns  void
*
* @notes    Also marks config info as unchanged!
*
* @end
*********************************************************************/

void cpCopyCfgDataToStore(void)
{
  SEMA_TAKE(WRITE);
  cpdmCfgData->hdr.dataChanged = L7_FALSE;
  memcpy(cpdmCfgDataToStore, cpdmCfgData, sizeof(cpdmCfgData_t));
  SEMA_GIVE(WRITE);
}

/*********************************************************************
*
* @purpose  Copies our private buffer to the real configuration data 
*
* @param    void
*
* @returns  void
*
* @notes    Also marks config info as unchanged!
*
* @end
*********************************************************************/

void cpCopyStoreToCfgData(void)
{
  SEMA_TAKE(WRITE);
  memcpy(cpdmCfgData, cpdmCfgDataToStore, sizeof(cpdmCfgData_t));
  SEMA_GIVE(WRITE);
  cpdmCfgData->hdr.dataChanged = L7_FALSE;
}

/*********************************************************************
*
* @purpose  Builds default config data in our private buffer
*
* @param    L7_uint32 version number of config to use
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/

void cpBuildDefaultConfigData(L7_uint32 version)
{
  cpdmCfgData_t * pC = cpdmCfgDataToStore;

  /* TODO: Add support for multiple versions once we have multiple
   * versions to support.
   */

  version = version; /* keep compiler from whining */

  if (L7_NULLPTR == pC)
  {
    return;
  }
  memset(pC, 0, sizeof(cpdmCfgData_t));
  osapiStrncpySafe(pC->hdr.filename, CPDM_CFG_FILENAME, L7_MAX_FILENAME);
  pC->hdr.version = version;
  pC->hdr.componentID = L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID;
  pC->hdr.type = L7_CFG_DATA;
  pC->hdr.length = sizeof(cpdmCfgData_t);
  pC->hdr.dataChanged = L7_FALSE;

  /* Now build default data for CP */
  {
    cpdmGlobal_t * pG = &(pC->cpGlobalData);
    pG->cpMode = FD_CP_DEFAULT_CP_MODE;
    pG->peerSwitchStatsReportInterval = FD_CP_PS_STATS_REPORT_INTERVAL;
    pG->sessionTimeout = FD_CP_AUTH_SESSION_TIMEOUT;
    pG->httpPort = FD_CP_HTTP_PORT;
    pG->httpsPort1 = L7_SSLT_SECURE_PORT;
    pG->httpsPort2 = FD_CP_HTTP_AUX_SECURE_PORT2;
  }
  pC->hdr.dataChanged = L7_TRUE;
  pC->checksum = nvStoreCrc32((L7_uint8 *) pC,
			      (L7_uint32) (sizeof(cpdmCfgData_t) - 
					   sizeof(pC->checksum)));
}

/*********************************************************************
*
* @purpose  Restore CP images NVRAM format to default.
*
* @param    L7_uchar8 *images @b{(input)} pointer to NVRAM cfg
*
* @returns  void
*
* @notes  This function is called to build default data, this would
*           be done on a "clear config". As such, the images
*           structure is cleared included the RAM disk.
*
* @end
*********************************************************************/
void cpImageCfgDefault(L7_uchar8 *images)
{
  L7_char8 fileName[CP_FILE_NAME_MAX+1]; 
  L7_uint32 FQF_MAX = 256;
  L7_char8 fqf[FQF_MAX];
  struct dirent *entry;
  DIR *dir;  

  if (L7_NULLPTR != images)
  {
    memset(images, 0, L7_CAPTIVE_PORTAL_IMAGES_NVRAM_SIZE);
    cpdmRAMCfgDataToStore->hdr.dataChanged = L7_TRUE;
  }

  /* Delete all files residing in the RAM disk */
  if ((dir = opendir(RAM_CP_PATH RAM_CP_NAME)) == NULL)
  {
    return;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    memset(&fileName,0x00,sizeof(fileName));
    strncpy(fileName,entry->d_name,CP_FILE_NAME_MAX);
    sprintf(fqf,"%s%s/%s",RAM_CP_PATH,RAM_CP_NAME,fileName);
    remove(fqf);
  }
  closedir(dir);
}

/*********************************************************************
*
* @purpose  CNFGR System Initialization for ACL component
*
* @param    L7_CNFGR_CMD_DATA_t  *pCmdData    Data structure for this
*                                             CNFGR request
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ERROR,   if failure
*
* @notes    This API is provided to allow the Configurator to issue a
*           request to the CAPTIVE_PORTAL comp.  This function is re-entrant.
*
* @end
*********************************************************************/
void  captivePortalApiCnfgrCommand(L7_CNFGR_CMD_DATA_t *pCmdData)
{

  char * cmd = "???";
  char * req = "???";
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request;

  command  = pCmdData->command;
  request = pCmdData->u.rqstData.request;

  switch (command) 
  {
    case L7_CNFGR_CMD_INITIALIZE:
      cmd = "L7_CNFGR_CMD_INITIALIZE";
      switch (request) 
      {
        case L7_CNFGR_RQST_I_PHASE1_START:
          req = "L7_CNFGR_RQST_I_PHASE1_START";

          /* Start the Captive Portal Task
           */
          (void) captivePortalDebugReset();
          captivePortalTaskStart();
          break;
        case L7_CNFGR_RQST_I_PHASE2_START:
          req = "L7_CNFGR_RQST_I_PHASE2_START";
          break;
        case L7_CNFGR_RQST_I_PHASE3_START:
          req = "L7_CNFGR_RQST_I_PHASE3_START";
          break;
        case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
          req = "L7_CNFGR_RQST_I_WAIT_MGMT_UNIT";
          break;
        default:
          break;
      }
      break;

    case L7_CNFGR_CMD_EXECUTE:
      cmd = "L7_CNFGR_CMD_EXECUTE";
      switch (request) 
      {
        case L7_CNFGR_RQST_E_START:
          req = "L7_CNFGR_RQST_E_START";
          break;
        default:
          break;
      }
      break;

    case L7_CNFGR_CMD_UNCONFIGURE:
      cmd = "L7_CNFGR_CMD_UNCONFIGURE";
      switch (request) 
      {
        case L7_CNFGR_RQST_U_PHASE1_START:
          req = "L7_CNFGR_RQST_U_PHASE1_START";
          break;
        case L7_CNFGR_RQST_U_PHASE2_START:
          req = "L7_CNFGR_RQST_U_PHASE2_START";
          break;
        default:
          break;
      }
      break;

    default:
      break; 
  }

#if 0
  LOG_MSG("%s: cmd = %s, req = %s\n", __FUNCTION__, cmd, req);
  printf("%s: cmd = %s, req = %s\n", __FUNCTION__, cmd, req);
#endif

  memcpy (&savedCnfgrCommand, pCmdData, sizeof (savedCnfgrCommand));
  configCommandPending = L7_TRUE;

  /* Notify the Captive Portal task that it has work to do.
  */
  captivePortalTaskWake ();
}



/*********************************************************************
*
* @purpose  To parse the configurator commands send to the Captive
*           Portal component
*
* @param    none
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/

void captivePortalCnfgrParse(void)
{
  /* set up variables and structures */
  L7_CNFGR_CMD_t        command;
  L7_CNFGR_RQST_t       request = L7_CNFGR_RQST_FIRST;
  L7_CNFGR_CB_DATA_t    cbData;
  L7_CNFGR_RESPONSE_t   response = L7_CNFGR_INVALID_RESPONSE;
  L7_CNFGR_CORRELATOR_t correlator;
  L7_RC_t               rc;
  L7_CNFGR_ERR_RC_t     reason;
  configPhaseFunc       configFunc;
  configPhaseFinalizer  finalizer;
  L7_CNFGR_CMD_DATA_t   *pCmdData;

  if (configCommandPending != L7_TRUE)
  {
    /* Nothing to do. */
    return;
  }

  configCommandPending = L7_FALSE;
  correlator = L7_NULL;
  rc = L7_ERROR;
  reason = 0;
  configFunc = L7_NULLPTR;
  finalizer   = L7_NULLPTR;
  pCmdData = &savedCnfgrCommand;

  do 
  {
    if (!pCmdData) 
    {
      reason = L7_CNFGR_ERR_RC_FATAL;
      break;
    }
    if (pCmdData->type != L7_CNFGR_RQST) 
    {
      reason = L7_CNFGR_ERR_RC_INVALID_CMD_TYPE;
      break;
    }

    request = pCmdData->u.rqstData.request;
    if ( request <= L7_CNFGR_RQST_FIRST || request >= L7_CNFGR_RQST_LAST )
    {
      reason = L7_CNFGR_ERR_RC_INVALID_RSQT;
      break;
    }

    command  = pCmdData->command;       /* Now validate command */
    switch (command) 
    {
      case L7_CNFGR_CMD_INITIALIZE:
      case L7_CNFGR_CMD_EXECUTE:
      case L7_CNFGR_CMD_UNCONFIGURE:
      case L7_CNFGR_CMD_TERMINATE:
      case L7_CNFGR_CMD_SUSPEND:
        break; /* okay */
      default:
        reason = L7_CNFGR_ERR_RC_INVALID_CMD;
        break; /* leaves inner switch statement ONLY */
    }

    if (0 != reason) 
    {
      break; /* break out of DO loop */
    } 

    /* If we get here, we should finish successfully.  We simply give a thumbs-up 
     * (and nothing else) if we receive a meaningless command/event pair.
     */
    correlator = pCmdData->correlator;
    switch (command) 
    {
      case L7_CNFGR_CMD_INITIALIZE:
        switch (request) 
        {
          case L7_CNFGR_RQST_I_PHASE1_START:
            configFunc = captivePortalInitPhase1Func;
            finalizer  = captivePortalFiniPhase1Func;
            break;
          case L7_CNFGR_RQST_I_PHASE2_START:
            configFunc = captivePortalInitPhase2Func;
            finalizer  = captivePortalFiniPhase2Func;
            break;
          case L7_CNFGR_RQST_I_PHASE3_START:
            configFunc = captivePortalInitPhase3Func;
            break;
          case L7_CNFGR_RQST_I_WAIT_MGMT_UNIT:
            configFunc = captivePortalInitWaitMUFunc;
            break;
          default:
            break;
        }
        break;

      case L7_CNFGR_CMD_EXECUTE:
        switch (request) 
        {
          case L7_CNFGR_RQST_E_START:
            configFunc = captivePortalExecStartFunc;
            break;
          default:
            break;
        }
        break;

      case L7_CNFGR_CMD_UNCONFIGURE:
        switch (request) 
        {
          case L7_CNFGR_RQST_U_PHASE1_START:
            configFunc = captivePortalUncfgPhase1Func;
            break;
          case L7_CNFGR_RQST_U_PHASE2_START:
            configFunc = captivePortalUncfgPhase2Func;
            break;
          default:
            break;
        }
        break;

      default:
        break; /* leaves inner switch statement ONLY */
    }
    
    if (L7_NULLPTR == configFunc) 
    {
      rc = L7_SUCCESS;
    }
    else
    {
      rc = configFunc(pCmdData, &response, &reason);
      if ((L7_SUCCESS != rc) && (L7_NULLPTR != finalizer))
      {
        finalizer();
      }
    }
  } while( 0 );

  cbData.correlator       = correlator;
  cbData.asyncResponse.rc = rc;

  if (L7_SUCCESS == rc) 
  {
    /* printf("req %d ok\n", request); */
    cbData.asyncResponse.u.response = response;
  }
  else
  {
    /* printf("req %d BAD\n", request); */
    cbData.asyncResponse.u.reason   = reason;
  }
  cnfgrApiCallback(&cbData);
}


/*********************************************************************
*
* @purpose  Place-holder for various config stages.  When nothing
*           needs to be done during any stage, this "virtual no-op"
*           should be used.
* 
* @param    captivePortalCnfgrState_t new state for CP configuration 
* @param    L7_CNFGR_RESPONSE_t *     response (set to "complete") 
* @param    L7_CNFGR_ERR_RC_t *       reason (always set to 0)
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*
*********************************************************************/

L7_RC_t
captivePortalNOP(captivePortalCnfgrState_t newState, 
		 L7_CNFGR_RESPONSE_t * pResp, 
		 L7_CNFGR_ERR_RC_t * pReason)
{
  captivePortalCnfgrState = newState;
  *pResp = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  return  L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Handles events for phase 1 of CP configuration
* 
* @param    L7_CNFGR_CMD_DATA_t * pCmd  - not used here
* @param    L7_CNFGR_RESPONSE_t * pResp - set to "complete" 
* @param    L7_CNFGR_ERR_RC_t * pReason - set to 0
*
* @returns  L7_SUCCESS
*
* @notes    Allocates private buffer for config storage. 
*           Switch will reboot if allocation fails (we have
*           BIG problems if this happens).
*
* @end
*
*********************************************************************/

L7_RC_t
captivePortalInitPhase1Func(L7_CNFGR_CMD_DATA_t * pCmd, 
			    L7_CNFGR_RESPONSE_t * pResp, 
			    L7_CNFGR_ERR_RC_t * pReason)
{
  if (L7_SUCCESS != osapiFsRamCreate(RAM_CP_PATH,RAM_CP_NAME,L7_CAPTIVE_PORTAL_IMAGES_NVRAM_SIZE))
  {
    LOG_MSG("%s: Failed to create RAM for images.", __FUNCTION__); 
  }

  /* Initialize our buffer area for CP images. */
  cpdmRAMCfgDataToStore = osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
                                      sizeof(cpdmRAMCfgDataToStore_t));
  if (L7_NULLPTR == cpdmRAMCfgDataToStore) 
  {
    LOG_ERROR(0); /* no point in continuing */
  }
  memset((void *) cpdmRAMCfgDataToStore, 0, sizeof(cpdmRAMCfgDataToStore_t));

  /* Initialize our buffer area for NVRam config data.
  ** Let the CP task do the rest.
  */
  if (L7_NULLPTR == cpdmCfgDataToStore)  
  {
    cpdmCfgDataToStore = osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                                     sizeof(cpdmCfgData_t));
    if (L7_NULLPTR == cpdmCfgDataToStore)
    {
      /* fatal error, thump the switch */
      LOG_ERROR(0);  
    }
  }
  memset(cpdmCfgDataToStore, 0, sizeof(cpdmCfgData_t));

  ptrCpGlobalData = (cpdmGlobal_t *)osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, sizeof(cpdmGlobal_t));
  if (ptrCpGlobalData == L7_NULLPTR)
  {
    LOG_MSG("captivePortalInitPhase1Func: Failed to allocate memory for CP global config.\n");
    return L7_FAILURE;
  }

  ptrCpConfigData = (cpdmConfigData_t *)osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                                                    sizeof(cpdmConfigData_t)*FD_CP_CONFIG_MAX);
  if (ptrCpConfigData == L7_NULLPTR)
  {
    LOG_MSG("captivePortalInitPhase1Func: Failed to allocate memory for CP config.\n");
    return L7_FAILURE;
  }
  numCP = 0;

  ptrCpWebID = (cpLocaleKey_t *)osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                                            sizeof(cpLocaleKey_t)*FD_CP_CONFIG_MAX*FD_CP_CUSTOM_LOCALE_MAX);
  if (ptrCpWebID == L7_NULLPTR)
  {
    LOG_MSG("captivePortalInitPhase1Func: Failed to allocate memory for CP WebID config.\n");
    return L7_FAILURE;
  }

  ptrCpCodeLink = (cpLocaleCodeLink_t *)osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                                                    sizeof(cpLocaleCodeLink_t)*FD_CP_CONFIG_MAX*FD_CP_CUSTOM_LOCALE_MAX);
  if (ptrCpCodeLink == L7_NULLPTR)
  {
    LOG_MSG("captivePortalInitPhase1Func: Failed to allocate memory for CP locale code/link config.\n");
    return L7_FAILURE;
  }

  ptrCpInterfaceAssocData = (ifNumCPPair_t *)osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                                                         sizeof(ifNumCPPair_t)*FD_CP_CONFIG_MAX*CP_INTERFACE_MAX);
  if (ptrCpInterfaceAssocData == L7_NULLPTR)
  {
    LOG_MSG("captivePortalInitPhase1Func: Failed to allocate memory for CP interface config.\n");
    return L7_FAILURE;
  }

  ptrCpUserGroupData = (cpdmUserGroupData_t *)osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                                                          sizeof(cpdmUserGroupData_t)*FD_CP_USER_GROUP_MAX);
  if (ptrCpUserGroupData == L7_NULLPTR)
  {
    LOG_MSG("captivePortalInitPhase1Func: Failed to allocate memory for CP user group config.\n");
    return L7_FAILURE;
  }
  numGroup = 0;

  ptrCpUsers = (cpdmUser_t *)osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                                         sizeof(cpdmUser_t)*FD_CP_LOCAL_USERS_MAX);
  if (ptrCpUsers == L7_NULLPTR)
  {
    LOG_MSG("captivePortalInitPhase1Func: Failed to allocate memory for CP user config.\n");
    return L7_FAILURE;
  }
  numUser = 0;

  ptrCpUserGroupAssocData = (cpdmUserGroupAssocData_t *)osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                                                                    sizeof(cpdmUserGroupAssocData_t)*FD_CP_USER_GROUP_ASSOC_MAX);
  if (ptrCpUserGroupAssocData == L7_NULLPTR)
  {
    LOG_MSG("captivePortalInitPhase1Func: Failed to allocate memory for CP user group association config.\n");
    return L7_FAILURE;
  }

  resendPeer = (L7_enetMacAddr_t *)osapiMalloc(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                                               sizeof(L7_enetMacAddr_t)*CP_CLUSTER_MEMBERS);
  if (resendPeer == L7_NULLPTR)
  {
    LOG_MSG("captivePortalInitPhase1Func: Failed to allocate memory for CP peers.\n");
    return L7_FAILURE;
  }
  memset(resendPeer, 0, sizeof(L7_enetMacAddr_t)*CP_CLUSTER_MEMBERS);


  captivePortalCnfgrState = CAPTIVE_PORTAL_PHASE_INIT_1;
  *pResp = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  return  L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Handles events for phase 2 of CP configuration
* 
* @param    L7_CNFGR_CMD_DATA_t * pCmd  - not used here
* @param    L7_CNFGR_RESPONSE_t * pResp - response code
* @param    L7_CNFGR_ERR_RC_t * pReason - reason code
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Registers config save/restore callbacks, as well
*           as NIM interface event change callbacks.  
*
* @end
*
*********************************************************************/
L7_RC_t
captivePortalInitPhase2Func(L7_CNFGR_CMD_DATA_t * pCmd, 
			    L7_CNFGR_RESPONSE_t * pResp, 
			    L7_CNFGR_ERR_RC_t * pReason)
{
  L7_uint32 clusterMaxMsgSize_datagram;
  L7_uint32 clusterMaxMsgSize_reliable;
  nvStoreFunctionList_t cpFuncList;
  captivePortalDebugRegister();

  /* Register for nvStore here */
  memset((void *) &cpFuncList, 0, sizeof(cpFuncList));
  cpFuncList.registrar_ID     = L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID;
  cpFuncList.notifySave       = cpConfigSave;
  cpFuncList.hasDataChanged   = cpConfigHasDataChanged;
  cpFuncList.notifyConfigDump = L7_NULLPTR;
  cpFuncList.notifyDebugDump  = L7_NULLPTR;
  cpFuncList.resetDataChanged = cpResetDataChanged;

  if (L7_SUCCESS != nvStoreRegister(cpFuncList))
  {
    *pResp   = 0;
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    return L7_ERROR;
  }

  /* Register for NIM callback here */
  if (nimRegisterIntfChange(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, 
			    cpimIntfChangeCallback) != L7_SUCCESS)
  {
    LOG_MSG("Unable to register Captive Portal for NIM callback\n");
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    *pResp = 0;
    return L7_ERROR;
  }

  /* Register for RADIUS callback here */
  if (radiusResponseRegister(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
			     cpcmRadiusResponseCallback) != L7_SUCCESS)
  {
    LOG_MSG("Unable to register Captive Portal for RADIUS callback\n");
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    *pResp = 0;
    return L7_ERROR;
  }

  if (clusterSupportGet() == L7_SUCCESS)
  {
    /* Register for Cluster event callback */
    if (clusterMemberNotifyRegister(cpClusterMemberEventCallback) != L7_SUCCESS)
    {
      LOG_MSG("Unable to register Captive Portal callback for Cluster events.\n");
      *pReason = L7_CNFGR_ERR_RC_FATAL;
      *pResp = 0;
      return L7_ERROR;
    }

    /* retrieve maximum message sizes for sending/receiving cluster data */
    clusterMaxMsgSize_datagram = clusterMsgMaxSizeGet(CLUSTER_MSG_DELIVERY_DATAGRAM);
    clusterMaxMsgSize_reliable = clusterMsgMaxSizeGet(CLUSTER_MSG_DELIVERY_RELIABLE);

    /* Register for Cluster message callbacks */
    if (clusterMsgRegister(CP_CLUSTER_CP_CLIENT_NOTIFICATION_MSG, CLUSTER_MSG_DELIVERY_RELIABLE,
                           cpClusterMsgCallback) != L7_SUCCESS)
    {
      LOG_MSG("Unable to register Captive Portal callback for Cluster client notifications.\n");
      *pReason = L7_CNFGR_ERR_RC_FATAL;
      *pResp = 0;
      return L7_ERROR;
    }

    if (clusterMsgRegister(CP_CLUSTER_CP_CLIENT_AUTH_REQUEST_MSG, CLUSTER_MSG_DELIVERY_RELIABLE, 
                           cpClusterMsgCallback /*cpClusterClientAuthRequestMsgCallback*/) != L7_SUCCESS)
    {
      LOG_MSG("Unable to register Captive Portal callback for Cluster client authentication requests.\n");
      *pReason = L7_CNFGR_ERR_RC_FATAL;
      *pResp = 0;
      return L7_ERROR;
    }

    if (clusterMsgRegister(CP_CLUSTER_CP_CLIENT_AUTH_REPLY_MSG, CLUSTER_MSG_DELIVERY_RELIABLE,
                           cpClusterMsgCallback /*cpClusterClientAuthReplyMsgCallback*/) != L7_SUCCESS)
    { 
      LOG_MSG("Unable to register Captive Portal callback for Cluster client authentication reply.\n");
      *pReason = L7_CNFGR_ERR_RC_FATAL;
      *pResp = 0;
      return L7_ERROR;
    }

    if (clusterMsgRegister(CP_CLUSTER_CP_CONTROLLER_COMMAND_MSG, CLUSTER_MSG_DELIVERY_RELIABLE,
                           cpClusterMsgCallback) != L7_SUCCESS)
    {
      LOG_MSG("Unable to register Captive Portal callback for Cluster controller commands.\n");
      *pReason = L7_CNFGR_ERR_RC_FATAL;
      *pResp = 0;
      return L7_ERROR;
    }

    if (clusterMsgRegister(CP_CLUSTER_CP_CONNECTED_CLIENTS_UPDATE_MSG, CLUSTER_MSG_DELIVERY_RELIABLE,
                           cpClusterMsgCallback) != L7_SUCCESS)
    {
      LOG_MSG("Unable to register Captive Portal callback for Cluster CP connected clients update.\n");
      *pReason = L7_CNFGR_ERR_RC_FATAL;
      *pResp = 0;
      return L7_ERROR;
    }

    if (clusterMsgRegister(CP_CLUSTER_CP_INSTANCE_INTERFACE_UPDATE_MSG, CLUSTER_MSG_DELIVERY_RELIABLE,
                           cpClusterMsgCallback) != L7_SUCCESS)
    {
        LOG_MSG("Unable to register Captive Portal callback for Cluster CP instance interface updates.\n");
        *pReason = L7_CNFGR_ERR_RC_FATAL;
        *pResp = 0;
        return L7_ERROR;
    }

    if (clusterMsgRegister(CP_CLUSTER_CP_STATISTICS_UPDATE_MSG, CLUSTER_MSG_DELIVERY_DATAGRAM,
                           cpClusterMsgCallback) != L7_SUCCESS)
    {
      LOG_MSG("Unable to register Captive Portal callback for Cluster statistics updates.\n");
      *pReason = L7_CNFGR_ERR_RC_FATAL;
      *pResp = 0;
      return L7_ERROR;
    }

    if (clusterMsgRegister(CP_CLUSTER_CP_CONFIG_SYNC_MSG, CLUSTER_MSG_DELIVERY_RELIABLE,
                           cpClusterMsgCallback) != L7_SUCCESS)
    {
      LOG_MSG("Unable to register Captive Portal callback for Cluster config sync.\n");
      *pReason = L7_CNFGR_ERR_RC_FATAL;
      *pResp = 0;
      return L7_ERROR;
    }

    if (clusterMsgRegister(CP_CLUSTER_CP_RESEND_MSG, CLUSTER_MSG_DELIVERY_RELIABLE,
                           cpClusterMsgCallback) != L7_SUCCESS)
    {
      LOG_MSG("Unable to register Captive Portal callback for Cluster resend messages.\n");
      *pReason = L7_CNFGR_ERR_RC_FATAL;
      *pResp = 0;
      return L7_ERROR;
    }

    /* Register for Cluster send/receive CP config callback */
    if (clusterConfigRegister(CLUSTER_CFG_ID(CAPTIVE_PORTAL),
                              cpClusterConfigSendCallback, cpClusterConfigRxCallback) != L7_SUCCESS)
    {
      LOG_MSG("Unable to register Captive Portal callback for Cluster send/receive CP config.\n");
      *pReason = L7_CNFGR_ERR_RC_FATAL;
      *pResp = 0;
      return L7_ERROR;
    }
  }

  captivePortalCnfgrState = CAPTIVE_PORTAL_PHASE_INIT_2;
  *pResp = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  return  L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Handles events for phase 3 of CP configuration
* 
* @param    L7_CNFGR_CMD_DATA_t * pCmd  - not used here
* @param    L7_CNFGR_RESPONSE_t * pResp - response code
* @param    L7_CNFGR_ERR_RC_t * pReason - reason code
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Reads and applies configuration info for CP 
*           from NVRAM.
* @end
*
*********************************************************************/

L7_RC_t
captivePortalInitPhase3Func(L7_CNFGR_CMD_DATA_t * pCmd, 
			    L7_CNFGR_RESPONSE_t * pResp, 
			    L7_CNFGR_ERR_RC_t * pReason)
{
  L7_RC_t rc;

  if (L7_SUCCESS == cpConfigRead())
  {
    cpConfigApply();
    *pResp = L7_CNFGR_CMD_COMPLETE;
    *pReason = 0;
    rc = L7_SUCCESS;
    captivePortalCnfgrState = CAPTIVE_PORTAL_PHASE_INIT_3;
  }
  else
  {
    *pResp = 0;
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    rc = L7_ERROR;
  }
  return rc;
}


/*********************************************************************
*
* @purpose  Handles events for phase "wait MU" of CP configuration
* 
* @param    L7_CNFGR_CMD_DATA_t * pCmd  - not used here
* @param    L7_CNFGR_RESPONSE_t * pResp  
* @param    L7_CNFGR_ERR_RC_t * pReason  
*
* @returns  L7_SUCCESS
*
* @notes    We try to register for NIM interface changes here if
*           needed.  This is required for "clear config" support.
* @end
*
*********************************************************************/
L7_RC_t
captivePortalInitWaitMUFunc(L7_CNFGR_CMD_DATA_t * pCmd, 
			    L7_CNFGR_RESPONSE_t * pResp, 
			    L7_CNFGR_ERR_RC_t * pReason)
{
  captivePortalCnfgrState = CAPTIVE_PORTAL_PHASE_WMU;
  *pResp = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  return  L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Handles events for phase "wait MU" of CP configuration
* 
* @param    L7_CNFGR_CMD_DATA_t * pCmd  - not used here
* @param    L7_CNFGR_RESPONSE_t * pResp  
* @param    L7_CNFGR_ERR_RC_t * pReason  
*
* @returns  L7_SUCCESS
*
* @notes    Nothing to do here, so we do.. nothing.
*           
* @end
*
*********************************************************************/

L7_RC_t
captivePortalExecStartFunc(L7_CNFGR_CMD_DATA_t * pCmd, 
			   L7_CNFGR_RESPONSE_t * pResp, 
			   L7_CNFGR_ERR_RC_t * pReason)
{
  return captivePortalNOP(CAPTIVE_PORTAL_PHASE_EXECUTE, pResp, pReason);
} 


/*********************************************************************
*
* @purpose  Handles events for phase "unconfig 1" of CP configuration
* 
* @param    L7_CNFGR_CMD_DATA_t * pCmd  - not used here
* @param    L7_CNFGR_RESPONSE_t * pResp  
* @param    L7_CNFGR_ERR_RC_t * pReason  
*
* @returns  L7_SUCCESS
*
* @notes    Sends signal to CP task to kill its main loop.  This will
*           in turn cause memory to be deallocated in the CP task 
*           before the task itself goes away.
*
* @end
*
*********************************************************************/

L7_RC_t
captivePortalUncfgPhase1Func(L7_CNFGR_CMD_DATA_t * pCmd, 
			     L7_CNFGR_RESPONSE_t * pResp, 
			     L7_CNFGR_ERR_RC_t * pReason)
{
  L7_RC_t rc;

  captivePortalCnfgrState = CAPTIVE_PORTAL_PHASE_UNCONFIG_1;
  rc =  cpdmCPConfigModeDisableForce();
  if (L7_SUCCESS != rc)
  {
    *pResp = 0;
    *pReason = L7_CNFGR_ERR_RC_FATAL;
    rc = L7_ERROR;
  }
  else
  {
    *pResp = L7_CNFGR_CMD_COMPLETE;
    *pReason = 0;
  }
  return rc;
}


/*********************************************************************
*
* @purpose  Handles events for phase "unconfig 2" of CP configuration
* 
* @param    L7_CNFGR_CMD_DATA_t * pCmd  - not used here
* @param    L7_CNFGR_RESPONSE_t * pResp  
* @param    L7_CNFGR_ERR_RC_t * pReason  
*
* @returns  L7_SUCCESS
*
* @notes    Deregisters with NIM.
*
* @end
*
*********************************************************************/

L7_RC_t
captivePortalUncfgPhase2Func(L7_CNFGR_CMD_DATA_t * pCmd, 
			     L7_CNFGR_RESPONSE_t * pResp, 
			     L7_CNFGR_ERR_RC_t * pReason)
{
  L7_uint32 intIfNum = 0;

  captivePortalCnfgrState = CAPTIVE_PORTAL_PHASE_UNCONFIG_2;  

  /* Disable our interfaces here - don't de-register with NIM */

  while(L7_SUCCESS == cpdmIntfStatusNextGet(intIfNum, &intIfNum))
  {
    (void) cpimIntfDisable(intIfNum);
  }  

  cpImageCfgDefault(cpdmRAMCfgDataToStore->images);

  /* Now wipe out all operational/status-related data */
  cpdmCPConfigFinalize();
  cpdmCACStatusFinalize();
  cpIntfAssocFinalize();
  cpdmIntfStatusFinalize();
  cpdmUserGroupEntryFinalize();
  cpdmUserGroupAssocEntryFinalize();

  captivePortalCnfgrState = CAPTIVE_PORTAL_PHASE_WMU;
  *pResp = L7_CNFGR_CMD_COMPLETE;
  *pReason = 0;
  return  L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Handles events for finalizing phase 1 of CP configuration
* 
* @param    L7_CNFGR_CMD_DATA_t * pCmd  - not used here
* @param    L7_CNFGR_RESPONSE_t * pResp  
* @param    L7_CNFGR_ERR_RC_t * pReason  
*
* @returns  L7_SUCCESS
*
* @notes    Right now, this does nothing.  We may want to signal the
*           CP task to kill itself, however, should we get called here.
* @end
*
*********************************************************************/
void
captivePortalFiniPhase1Func(void)
{
  osapiFree(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID, cpdmCfgDataToStore);
  cpdmCfgDataToStore = L7_NULLPTR;
  /* Send signal to our CP task ("Die, monster, die!") */
}


/*********************************************************************
*
* @purpose  Handles events for finalizing phase 2 of CP configuration
* 
* @param    L7_CNFGR_CMD_DATA_t * pCmd  - not used here
* @param    L7_CNFGR_RESPONSE_t * pResp  
* @param    L7_CNFGR_ERR_RC_t * pReason  
*
* @returns  L7_SUCCESS
*
* @notes    Deregisters our routines for NVRAM save/restore.
*           
* @end
*
*********************************************************************/
void
captivePortalFiniPhase2Func(void)
{
  nvStoreDeregister(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID);
  radiusResponseDeregister(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID);
  captivePortalCnfgrState = CAPTIVE_PORTAL_PHASE_INIT_1;
}



/**************************************************************************
 *
 *                 CONFIGURATION SAVE AND RESTORE ROUTINES
 *
 **************************************************************************/

/*********************************************************************
* @purpose  Checks if Captive Portal config data has changed
*
* @param    void
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Call should probably be protected by semaphore. 
*
* @end
*********************************************************************/
L7_BOOL cpConfigHasDataChanged(void)
{
  L7_BOOL rc = L7_FALSE;
  cpdmCfgData_t * pC = L7_NULLPTR;
  cpdmRAMCfgDataToStore_t * pIC = L7_NULLPTR;

  SEMA_TAKE(READ);
  pC = cpdmCfgData; 
  pIC = cpdmRAMCfgDataToStore;
  rc = ((L7_NULLPTR != pC) && ((pC->hdr.dataChanged) || (pIC->hdr.dataChanged)));
  SEMA_GIVE(READ);

  return rc;
}

/*********************************************************************
*
* @purpose  Copy CP image files from OS RAM into the CP data structure
*
* @param    void
* 
* @returns  void
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t cpCopyImageFsToData(L7_uchar8 *images)
{
  L7_char8 fileName[CP_FILE_NAME_MAX+1];
  L7_char8 tempName[CP_FILE_NAME_MAX+1];
  L7_char8 *filePath = RAM_CP_PATH RAM_CP_NAME;
  L7_uint32 FQF_MAX = 256;
  L7_char8 fqf[FQF_MAX];
  L7_uint32 fileNameLen = 0;
  L7_uint32 fileSize = 0;
  L7_uint32 cnt = 0;
  L7_uint32 idx = 0;
  L7_uint32 eoi = 0;
  L7_char8 *ext;
  struct dirent *entry;
  DIR *dir;

  if (images == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  SEMA_TAKE(WRITE);
  memset(images,0,L7_CAPTIVE_PORTAL_IMAGES_NVRAM_SIZE);

  if ((dir = opendir(RAM_CP_PATH RAM_CP_NAME)) == NULL)
  {
    LOG_MSG("%s: Failed to open directory:%s.",__FUNCTION__,filePath);
    SEMA_GIVE(WRITE);
    return L7_FAILURE;
  }

  fileSize = 4; /* Init for first fileNameLen index */
  while ((entry = readdir(dir)) != NULL)
  {
    strncpy(fileName, entry->d_name, CP_FILE_NAME_MAX);
    fileName[CP_FILE_NAME_MAX] = '\0';
    strncpy(tempName,fileName,CP_FILE_NAME_MAX);
    ext = strrchr(tempName,'.');

    if ((NULL!=ext) && (4==strlen(ext)))
    {
      osapiConvertToLowerCase(ext);
      if ((0==strcmp(ext,".gif")) || (0==strcmp(ext,".jpg")))
      {
	strncpy(fqf,filePath,FQF_MAX);
	strncat(fqf,"/",FQF_MAX);
	strncat(fqf,fileName,FQF_MAX);
	fileNameLen = strlen(fileName);

	if (L7_CAPTIVE_PORTAL_IMAGES_NVRAM_SIZE >
	    (idx+sizeof(L7_uint32)+fileNameLen+sizeof(L7_uint32)+fileSize))
	{
	  i2b(&images[0],++cnt); /* update total # of files */
	  i2b(&images[idx+=fileSize],fileNameLen);
	  strncpy(&images[idx+=4],fileName,fileNameLen);
	  if (osapiFsFileSizeGet(fqf,&fileSize) != L7_SUCCESS)
	  {
            closedir(dir);
            SEMA_GIVE(WRITE);
            LOG_MSG("%s: Unable to read file size for:%s.",__FUNCTION__,fileName);
            return L7_FAILURE;
	  }
	  i2b(&images[idx+=fileNameLen],fileSize);
	  if (L7_SUCCESS != osapiFsRead(fqf,&images[idx+=4],fileSize))
	  {
            closedir(dir);
            SEMA_GIVE(WRITE);
            LOG_MSG("%s: Unable to read file:%s.",__FUNCTION__,fileName);
            return L7_FAILURE;
	  }
          eoi += 4 + strlen(fileName) + 4 + fileSize;
	}
      }
    }
  }

  if (0<eoi)
  {
    eoi += 4; /* Add file count */
  }

  cpdmRAMCfgDataToStore->eoi = eoi;
  cpdmRAMCfgDataToStore->eot = 0;
  cpdmRAMCfgDataToStore->hdr.dataChanged = L7_FALSE;
  closedir(dir);
  SEMA_GIVE(WRITE);
  return L7_SUCCESS;
}


void cpResetDataChanged(void)
{
  SEMA_TAKE(WRITE);
  cpdmCfgData->hdr.dataChanged = L7_FALSE;
  SEMA_GIVE(WRITE);
  return;
}

/*********************************************************************
* @purpose  cpConfigSave
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Call should probably be protected by semaphore. 
*
* @end
*********************************************************************/
L7_RC_t cpConfigSave(void)
{
  L7_RC_t rc = L7_SUCCESS;
  cpdmCfgData_t * pC = L7_NULLPTR;   
  cpdmRAMCfgDataToStore_t * pIC = L7_NULLPTR; 

  cpCopyCfgDataToStore();
  pC = cpdmCfgDataToStore;

  if (L7_TRUE == pC->hdr.dataChanged)
  {
    if (usmDbFeaturePresentCheck(1, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) != L7_TRUE)
    {
      do
      {
        osapiStrncpySafe(pC->hdr.filename, CPDM_CFG_FILENAME, L7_MAX_FILENAME);
        pC->hdr.version = CP_CFG_CURRENT_VERSION;
        pC->hdr.componentID = L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID;
        pC->hdr.type = L7_CFG_DATA;
        pC->hdr.length = sizeof(cpdmCfgData_t);
        pC->hdr.dataChanged = L7_FALSE;
    
        pC->checksum = nvStoreCrc32((L7_uchar8 *) pC, 
                                    (L7_uint32) (sizeof(cpdmCfgData_t) - sizeof(pC->checksum)));
        if (L7_SUCCESS !=
            (rc = sysapiCfgFileWrite(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
				     CPDM_CFG_FILENAME, (L7_uchar8 *) pC,
				     (L7_uint32) sizeof(cpdmCfgData_t))))
        {
          break;
        }
        rc = L7_SUCCESS;
      } while(0);
    }
  }

  pIC = cpdmRAMCfgDataToStore;
  if (L7_TRUE == pIC->hdr.dataChanged)
  {
    cpCopyImageFsToData(cpdmRAMCfgDataToStore->images);
    if (usmDbFeaturePresentCheck(1, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) != L7_TRUE)
    {
      do
      {
        osapiStrncpySafe(pIC->hdr.filename, CPDM_CFG_FILENAME2, L7_MAX_FILENAME);
        pIC->hdr.version = CP_CFG_CURRENT_VERSION;
        pIC->hdr.componentID = L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID;
        pIC->hdr.type = L7_CFG_DATA;
        pIC->hdr.length = sizeof(cpdmRAMCfgDataToStore_t);
        pIC->hdr.dataChanged = L7_FALSE;
        pIC->checksum =
	  nvStoreCrc32((L7_uchar8 *) pIC, 
		       (L7_uint32) (sizeof(cpdmRAMCfgDataToStore_t) - sizeof(pIC->checksum)));
        if (L7_SUCCESS !=
            (rc = sysapiCfgFileWrite(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
				     CPDM_CFG_FILENAME2, (L7_uchar8 *) pIC,
				     (L7_uint32) sizeof(cpdmRAMCfgDataToStore_t))))
        {
          break;
        }
        rc = L7_SUCCESS;
      } while(0);
    }
  }

  pC->hdr.dataChanged = L7_FALSE;
  pIC->hdr.dataChanged = L7_FALSE;

  return rc;
}

/*********************************************************************
*
* @purpose  Applies configuration retrieved from NVRAM to our
*           run-time config info
*
* @param    void
* 
* @returns  void
*
* @notes    Right now, all we do is one copy. 
*
* @end
*
*********************************************************************/

void cpConfigApply(void)
{
  cpCopyStoreToCfgData();
  cpdmCPConfigCountUpdate();
  cpdmActStatusUpdate();
  cpdmCPIntfCountUpdate();
  cpdmUserEntryCountUpdate();
  cpdmWebIdCountUpdate();
  cpdmUserGroupEntryCountUpdate();
  cpdmUserGroupAssocEntryCountUpdate();
  cpdmCPConfigAddDefaults();
  cpdmUserGroupEntryAddDefaults();
  if (usmDbFeaturePresentCheck(1, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) != L7_TRUE)
  {
    cpdmCopyImageDataToFs(cpdmRAMCfgDataToStore->images);
  }
}


/*********************************************************************
* @purpose  cpConfigRead
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t cpConfigRead(void)
{
  L7_RC_t rc = L7_FAILURE;
  cpdmCfgData_t * pC = L7_NULLPTR;   
  cpdmRAMCfgDataToStore_t * pIC = L7_NULLPTR;

  if (usmDbFeaturePresentCheck(1, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) == L7_TRUE)
  {
    memset(cpdmCfgData,0,sizeof(cpdmCfgData_t));
    cpBuildDefaultConfigData(CP_CFG_CURRENT_VERSION);
    return L7_SUCCESS;
  }

  do 
  {
    L7_RC_t get_rc;
    pC = cpdmCfgDataToStore;

    if (L7_NULLPTR == pC) 
    {
      break;
    }
    memset(pC, 0, sizeof(cpdmCfgData_t));
    get_rc = sysapiCfgFileGet(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                              CPDM_CFG_FILENAME,
                              (L7_uchar8 *) pC,
                              (L7_uint32) sizeof(cpdmCfgData_t),
                              &(pC->checksum),
                              CP_CFG_CURRENT_VERSION,
                              cpBuildDefaultConfigData,
                              L7_NULL);
    if (get_rc != L7_SUCCESS)
    {
      break;
    }

    pIC = cpdmRAMCfgDataToStore;
    if (L7_NULLPTR == pIC) 
    {
      break;
    }
    memset(pIC, 0, sizeof(cpdmRAMCfgDataToStore_t) );
    get_rc = sysapiCfgFileGet(L7_FLEX_CAPTIVE_PORTAL_COMPONENT_ID,
                              CPDM_CFG_FILENAME2,
                              (L7_uchar8 *) pIC,
                              (L7_uint32) sizeof(cpdmRAMCfgDataToStore_t),
                              &(pIC->checksum),
                              CP_CFG_CURRENT_VERSION,
                              L7_NULL,
                              L7_NULL);

    /* Force update so we can create the file if it doesn't exist */
    pIC->hdr.dataChanged = (L7_FAILURE==get_rc)?L7_TRUE:L7_FALSE;
    rc = L7_SUCCESS;
  } while(0);

  return rc;
}

#if 0
/*********************************************************************
*
* @purpose  Force our config data to spill its guts to the console.
* 
* @param    cdpmCfgData_t *  config data with spillable guts
*
* @returns  void 
*
* @notes    
*
* @end
*
*********************************************************************/

void cpConfigShow(cpdmCfgData_t * pC)
{
  L7_int32 idx; 

  if (L7_NULLPTR == pC)
  {
    sysapiPrintf("%s, got null pointer\n", __FUNCTION__);
    return;
  }

  for(idx = 0; idx < FD_CP_LOCAL_USERS_MAX; idx++)
  {
    L7_char8 * pName = pC->cpUsersData.cpUsers[idx].loginName;
    if (0 == strlen(pName))
      break;
    sysapiPrintf("USER: %s\n", pName);
  }
    
  for(idx = 0; idx < FD_CP_CONFIG_MAX; idx++)
  {
    if (0 ==  pC->cpConfigData[idx].cpId)
      break;
    sysapiPrintf("CP ID: %u\n", pC->cpConfigData[idx].cpId);
  }

  for(idx = 0; idx < FD_CP_CONFIG_MAX * FD_CP_CUSTOM_LOCALE_MAX; idx++)
  {
    if (0 ==  pC->cpLocaleData[idx].key.cpId)
      break;
    sysapiPrintf("WEB ID: %u/%u\n", 
		 pC->cpLocaleData[idx].key.cpId,  
		 pC->cpLocaleData[idx].key.webId);
  }

  for(idx = 0; idx < CP_INTERFACE_MAX; idx++)
  {
    if (0 ==  pC->cpInterfaceAssocData[idx].intIfNum)
      break;
    sysapiPrintf("INTF: %u\n", pC->cpInterfaceAssocData[idx].intIfNum);
  }

}
#endif
