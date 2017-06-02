/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    mirror_cfg.c
* @purpose     Port mirroring initialization and configuration
* @component   mirroring
* @comments    none
* @create      11/21/2001
* @author      skalyanam
* @end
*
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "mirror_api.h"
#include "mirror_cnfgr.h"
#include "nvstoreapi.h"
#include "mirror.h"
#include "mirror_cfg.h"
#include "defaultconfig.h"
#include "nimapi.h"
#include "cnfgr.h"
#include "dot3ad_api.h"

extern mirrorCfgData_t   *mirrorCfgData;
extern mirrorSessionInfo_t mirrorInfo[];
extern mirrorPhases_t mirrorState;
L7_BOOL firstSourcePort[L7_MIRRORING_MAX_SESSIONS];
PORTEVENT_MASK_t mirrorPortEventMask_g;

/*********************************************************************
* @purpose  Saves mirror user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mirrorSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if (mirrorCfgData->cfgHdr.dataChanged == L7_TRUE)
  {
    mirrorCfgData->cfgHdr.dataChanged = L7_FALSE;

    mirrorCfgData->checkSum = nvStoreCrc32((L7_uchar8 *)mirrorCfgData,
                              (L7_uint32)(sizeof (mirrorCfgData_t) -
                                        sizeof (mirrorCfgData->checkSum)));

    /* call save NVStore routine */
    rc = sysapiCfgFileWrite(L7_PORT_MIRROR_COMPONENT_ID, MIRROR_CFG_FILENAME,
                     (L7_char8 *)mirrorCfgData, (L7_int32)sizeof(mirrorCfgData_t));

    if (rc != L7_SUCCESS)
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MIRROR_COMPONENT_ID,
              "Error on call to sysapiCfgFileWrite routine on config file %s\n",
               MIRROR_CFG_FILENAME);

  }
  return(rc);
}

/*********************************************************************
* @purpose  Checks if mirror user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL mirrorHasDataChanged(void)
{
  return(mirrorCfgData->cfgHdr.dataChanged);
}
void mirrorResetDataChanged(void)
{
  mirrorCfgData->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Apply Mirror Configuration Data
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mirrorApplyConfigData(void)
{
  L7_uint32 sessionIndex;


  for(sessionIndex = 1; sessionIndex <= L7_MIRRORING_MAX_SESSIONS; sessionIndex++)
  {
    /*
     * mirrorSessionApply will log error message if there is any error
     * while session data is being applied
    */
    if ((mirrorCfgData->sessionData[sessionIndex - 1].mode == L7_ENABLE) &&
        (mirrorDestEmpty(sessionIndex) == L7_FALSE))
    {
      if (mirrorSessionConfig(sessionIndex) != L7_SUCCESS)
          return L7_FAILURE;
    }
  }
  /*
   * As of this writing, the error returns are logged in the called routines.
   * To allow for future change, and to be consistent with similar routines
   * in other application, return L7_RC_t
  */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To process L7_CREATE for each individual interface
*
* @param    intIfNum    @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t mirrorIntfCreate(L7_uint32 intIfNum)
{
  MIRROR_TRACE("\n Creating Interface : %d",intIfNum);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  To apply config data for each individual interface
*
* @param    intIfNum    @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
*
* @notes   This is meeded to apply the the config data for individual
*          interfaces when these are ATTACHed
*
* @end
*********************************************************************/

L7_RC_t mirrorIntfApplyConfigData(L7_uint32 intIfNum)
{
  L7_uint32 sessionNum;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 dstIntfNum = 0;
  L7_MIRROR_DIRECTION_t probeType = 0;


  if(mirrorIsDestConfigured(intIfNum, &sessionNum) == L7_TRUE)
  {
    if(mirrorCfgData->sessionData[sessionNum-1].mode == L7_ENABLE)
      rc = mirrorSessionConfig(sessionNum);

  }
  else if(mirrorIsSrcConfigured(intIfNum, &sessionNum) == L7_TRUE)
  {
    if (mirrorSourcePortDirectionGet(sessionNum,intIfNum, &probeType) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MIRROR_COMPONENT_ID,
              "mirrorIntfApplyConfigData(): Failed to get direction for intf %s\n", ifName);
      return L7_FAILURE;
    }

    /* apply source port configuration to hardware */
    rc=mirrorSourcePortSet(sessionNum,intIfNum,L7_ENABLE,probeType);
    if ((firstSourcePort[sessionNum-1] == L7_TRUE) && (L7_SUCCESS == rc))
    {
      if (mirrorDestPortGet(sessionNum, &dstIntfNum) == L7_SUCCESS &&
          mirrorIntfAttached(dstIntfNum) == L7_TRUE &&
          mirrorCfgData->sessionData[sessionNum-1].mode == L7_ENABLE)
      {
        MIRROR_TRACE("\n\nmirrorIntfApplyConfigData::Calling mirrorNimEventIntfNotify\
                     with L7_PROBE_SETUP for Interface: %d.\n",dstIntfNum);

        mirrorNimEventIntfNotify(dstIntfNum,  L7_PROBE_SETUP);
      }
      firstSourcePort[sessionNum-1] = L7_FALSE;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  To remove config data for each individual interface
*
* @param    intIfNum    @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
*
* @notes   This is meeded to remove the config data for individual
*          interfaces when these are DETACHed
*
* @end
*********************************************************************/

L7_RC_t mirrorIntfDetach(L7_uint32 intIfNum)
{
  L7_uint32 sessionIndex;
  L7_RC_t rc = L7_SUCCESS;

  if(mirrorIsDestConfigured(intIfNum, &sessionIndex) == L7_TRUE)
  {
    rc = mirrorDestPortConfigUnApply(sessionIndex, intIfNum);
  }
  else if (mirrorIsSrcConfigured(intIfNum, &sessionIndex) == L7_TRUE)
  {
    rc = mirrorSourcePortConfigUnApply(sessionIndex, intIfNum);
  }

  return rc;
}

/*********************************************************************
* @purpose  To delete individual interface from mirror config data
*
* @param    intIfnum    @b{(input)} internal interface number
*
* @returns
*
* @notes   This is meeded to delete interfaces from  mirror config data
*          when DELETE event is generated for this interface
*
* @end
*********************************************************************/

L7_RC_t mirrorIntfDelete(L7_uint32 intIfNum)
{
  L7_uint32 sessionIndex;
  L7_RC_t rc = L7_SUCCESS;

  if(mirrorIsDestConfigured(intIfNum, &sessionIndex) == L7_TRUE)
  {
    rc = mirrorDestPortRemove(sessionIndex);
  }
  else if (mirrorIsSrcConfigured(intIfNum, &sessionIndex) == L7_TRUE)
  {
    rc = mirrorSourcePortRemove(sessionIndex, intIfNum);
  }

  return rc;
}

/*********************************************************************
* @purpose  Determine if mirror is interested in the given interface
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL mirrorIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  if (sysIntfType == L7_PHYSICAL_INTF || sysIntfType == L7_CPU_INTF ||
        (sysIntfType == L7_LAG_INTF && (cnfgrIsFeaturePresent(L7_PORT_MIRROR_COMPONENT_ID,
         L7_MIRRORING_LAG_INTF_SOURCE_SUPPORTED_FEATURE_ID)) == L7_TRUE))
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Callback function to process interface state changes.
*
* @param    intIfNum    @b{(input)} internal interface whose state has changed
* @param    event       @b{(input)} new state (see L7_PORT_EVENTS_t for list)
*
* @returns  L7_SUCCESS
*
* @notes    This function currently only cares about interface
*           creation.  We may have tried to apply port monitoring
*           data in mirrorApplyConfigData before the interfaces were
*           created. This function will add/remove the interface from DTL
*           configuration data and application data
*
* @end
*********************************************************************/
L7_RC_t mirrorIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event,NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;
  L7_uint32 sessionIndex;

  status.intIfNum     = intIfNum;
  status.component    = L7_PORT_MIRROR_COMPONENT_ID;
  status.event        = event;
  status.correlator   = correlator;
 /* initialize staus.response.reason as unused */
  status.response.reason = NIM_ERR_RC_UNUSED;

  if (mirrorIsValidIntf(intIfNum) == L7_FALSE)
  {
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  if ( (mirrorIsDestConfigured(intIfNum, L7_NULLPTR) == L7_FALSE) &&
       (mirrorIsSrcConfigured(intIfNum, &sessionIndex) == L7_FALSE)
     )
  {
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }
  /* only process the event if mirroring needs to */

  if (MIRRORING_IS_READY)
  {
    switch (event)
    {
      case L7_CREATE:    /* Nothing to do until an ATTACH */
        rc = mirrorIntfCreate(intIfNum);
        break;

      case L7_ATTACH:   /* Apply data for this interface to DTL */
        rc = mirrorIntfApplyConfigData(intIfNum);
        break;

      case L7_DETACH:  /* remove this interface from DTL configuration */
        rc = mirrorIntfDetach(intIfNum);
        break;

      case L7_DELETE:   /* remove the interface from the config */
        rc = mirrorIntfDelete(intIfNum);
        break;

      default:
        rc = L7_SUCCESS;
        break;
    } /* switch(event)  */
  } /* if mirroring is ready */
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MIRROR_COMPONENT_ID,
            "mirrorIntfChangeCallback: Received an interface event callback while not in EXECUTE state.\n");
  }


  status.response.rc  = rc;

  nimEventStatusCallback(status);

  return(rc);
}

/*********************************************************************
* @purpose  Propogate Startup notifications to mirroring component
*
* @param    NIM_STARTUP_PHASE_t  startupPhase
*
* @notes
*
* @end
*********************************************************************/
void mirrorStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;

  rc = nimFirstValidIntfNumber(&intIfNum);

  while (rc == L7_SUCCESS)
  {
    switch (startupPhase)
    {
      case NIM_INTERFACE_CREATE_STARTUP:
        rc = mirrorIntfCreate(intIfNum);
        if (rc != L7_SUCCESS)
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

          L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_PORT_MIRROR_COMPONENT_ID, "Intf %s create, rc = %d", ifName, rc);
        }
        break;

    case NIM_INTERFACE_ACTIVATE_STARTUP:
        if (mirrorIntfAttached(intIfNum))
        {
          rc = mirrorIntfApplyConfigData(intIfNum);
          if (rc != L7_SUCCESS)
          {
            L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
            nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

            L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_PORT_MIRROR_COMPONENT_ID, "Intf %s activate, rc = %d", ifName, rc);
          }
        }
        break;

      default:
        break;
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  switch (startupPhase)
  {
    case NIM_INTERFACE_CREATE_STARTUP:
      /* Register with NIM to receive port CREATE/DELETE events */
      PORTEVENT_SETMASKBIT(mirrorPortEventMask_g, L7_CREATE);
      PORTEVENT_SETMASKBIT(mirrorPortEventMask_g, L7_DELETE);
      nimRegisterIntfEvents(L7_PORT_MIRROR_COMPONENT_ID, mirrorPortEventMask_g);
      break;

    case NIM_INTERFACE_ACTIVATE_STARTUP:
      /* Add ATTACH/DETACH events to our NIM registration */
      PORTEVENT_SETMASKBIT(mirrorPortEventMask_g, L7_ATTACH);
      PORTEVENT_SETMASKBIT(mirrorPortEventMask_g, L7_DETACH);
      nimRegisterIntfEvents(L7_PORT_MIRROR_COMPONENT_ID, mirrorPortEventMask_g);
      break;

    default:
      break;
  }

  nimStartupEventDone(L7_PORT_MIRROR_COMPONENT_ID);

  /* Inform cnfgr that mirroring has completed it HW updates */
  if (startupPhase == NIM_INTERFACE_ACTIVATE_STARTUP)
  {
    cnfgrApiComponentHwUpdateDone(L7_PORT_MIRROR_COMPONENT_ID,
                                  L7_CNFGR_HW_APPLY_CONFIG);
  }
}

/*********************************************************************
* @purpose  Build default mirror config data
*
* @param    ver   @b{(input)} Software version of Config Data
*
* @returns  none
*
* @notes    none
*
* @end
*********************************************************************/
void mirrorBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 sessionIndex;

  memset((void *)mirrorCfgData, 0, sizeof(mirrorCfgData_t));
  strcpy((char *)mirrorCfgData->cfgHdr.filename, MIRROR_CFG_FILENAME);
  mirrorCfgData->cfgHdr.version = ver;
  mirrorCfgData->cfgHdr.componentID = L7_PORT_MIRROR_COMPONENT_ID;
  mirrorCfgData->cfgHdr.type = L7_CFG_DATA;
  mirrorCfgData->cfgHdr.length = (L7_uint32)sizeof(mirrorCfgData_t);
  mirrorCfgData->cfgHdr.dataChanged = L7_FALSE;

  for(sessionIndex = 1; sessionIndex <= L7_MIRRORING_MAX_SESSIONS; sessionIndex++)
  {
    mirrorCfgData->sessionData[sessionIndex - 1].mode = FD_MIRRORING_MODE;
  }

  mirrorCfgData->tracemode = FD_MIRRORING_TRACE_MODE;
}

/*********************************************************************
* @purpose  Notify nim about the status change of probe port
*
* @param   intIfDest   @((input)) Probe/destination interface
* @param   eventType   @((input)) L7_PROBE_TEARDOWN or L7_PROBE_SETUP
*
* @returns   none
*
* @notes
*
* @end
*********************************************************************/

void mirrorNimEventIntfNotify(L7_uint32 intIfDest,L7_PORT_EVENTS_t eventType)
{
  NIM_EVENT_NOTIFY_INFO_t eventInfo;
  NIM_HANDLE_t   handle;
  L7_uint32 maskOffset;

  if(eventType == L7_PROBE_SETUP)
  {
    MIRROR_DEBUG("L7_PROBE_SETUP");
  }
  else if(eventType == L7_PROBE_TEARDOWN)
  {
    MIRROR_DEBUG("L7_PROBE_TEARDOWN");
  }

  /* notification can only be sent for attached interfaces */
  if(nimConfigIdMaskOffsetGet(intIfDest, &maskOffset) != L7_SUCCESS)
    return;

  eventInfo.component     = L7_PORT_MIRROR_COMPONENT_ID;
  eventInfo.pCbFunc       = L7_NULLPTR;

  /*
  * TEARDOWN event does not restore vlan membership
  * this port will have to be included in vlans all over again as the
  * vlan config data was lost during probe setup
  */
  eventInfo.event         = eventType;
  eventInfo.intIfNum      = intIfDest;

  if ((nimEventIntfNotify(eventInfo,&handle)) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MIRROR_COMPONENT_ID,
            "Mirroring: Failed to notify components of probe status change\n");
  }
}


/*********************************************************************
* @purpose  Check whether intIfNum is capable of operating as mirrored port
*
* @param    intIfNum  @b{(input)} The internal interface number being validated
* @param    *valid    @b((output)) validity of source port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   This function will be needed to check any constraint for source port.
*
* @end
*********************************************************************/
L7_RC_t mirrorIsValidSrcIntf(L7_uint32 intIfNum,L7_BOOL *valid)
{
  L7_RC_t rc = L7_FAILURE;
  L7_INTF_TYPES_t  sysIntfType;

  if((nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
     ((sysIntfType == L7_PHYSICAL_INTF) || (sysIntfType == L7_LAG_INTF &&
        (cnfgrIsFeaturePresent(L7_PORT_MIRROR_COMPONENT_ID,
            L7_MIRRORING_LAG_INTF_SOURCE_SUPPORTED_FEATURE_ID) == L7_TRUE)) ||
       (sysIntfType == L7_CPU_INTF &&
         (cnfgrIsFeaturePresent(L7_PORT_MIRROR_COMPONENT_ID,
                       L7_MIRRORING_CPU_INTF_SUPPORTED_FEATURE_ID) == L7_TRUE)
        )
      )&& ( dot3adIsLagMember(intIfNum)== L7_FALSE))
  {
    if( valid != L7_NULLPTR)
      *valid = L7_TRUE;
    rc = L7_SUCCESS;
  }

  return(rc);
}

/*********************************************************************
* @purpose  Check whether intIfNum is capable of operating as probe port
*
* @param    intIfNum  @b{(input)} The internal interface number being validated
* @param    *valid     @b((output)) validity of destinaiton(probe) port
*
* @returns  L7_SUCCESS
* @returns  L7_FILURE
*
* @notes   This function will be need to check any constraint for probe port.
*
* @end
*********************************************************************/
L7_RC_t mirrorIsValidDestIntf(L7_uint32 intIfNum, L7_BOOL *valid)
{
  L7_RC_t rc = L7_FAILURE;
  L7_INTF_TYPES_t  sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    if(sysIntfType == L7_PHYSICAL_INTF)
    {
      if (dot3adIsLagMember(intIfNum) == L7_FALSE)
      {
        if( valid != L7_NULLPTR)
          *valid = L7_TRUE;
        rc = L7_SUCCESS;
      }
    }
  }
  return(rc);
}

/*********************************************************************
* @purpose  Determines whether the sessionNum is valid or not
*
* @param    sessionNum  @b{(input)} The session number being validated
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes  This will return TRUE even if session is disabled or have no
*         source/destinaiton ports set.
*
* @end
*********************************************************************/
L7_BOOL mirrorIsValidSession(L7_uint32 sessionNum)
{
  L7_BOOL rc = L7_TRUE;

  if( sessionNum > L7_MIRRORING_MAX_SESSIONS || sessionNum == 0)
    rc = L7_FALSE;

  return(rc);
}

/*********************************************************************
* @purpose  Determines whether destination interface is set or not
*
* @param    sessionNum  @b{(input)} The session number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL mirrorDestEmpty(L7_uint32 sessionNum)
{
  nimConfigID_t nullCfgID;

 if(mirrorIsValidSession(sessionNum) == L7_TRUE)
 {
   memset((void *)&nullCfgID,0,sizeof(nimConfigID_t));
   if (NIM_CONFIG_ID_IS_EQUAL(&nullCfgID,
        &(mirrorCfgData->sessionData[sessionNum-1].destID)) == L7_TRUE )
     return L7_TRUE;
  }
  return L7_FALSE;
}


