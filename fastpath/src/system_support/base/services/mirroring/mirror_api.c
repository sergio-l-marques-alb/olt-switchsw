
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    mirror_api.c
* @purpose     Port mirroring APIs
* @component   mirroring
* @comments    none
* @create      03/05/2004
* @author      harvinder 
* @end
*             
**********************************************************************/

#include <string.h>
#include "l7_common.h"
#include "nimapi.h"
#include "l7_product.h"
#include "defaultconfig.h"
#include "mirror_cfg.h"
#include "mirror.h"
#include "mirror_api.h"
#include "usmdb_mirror_api.h"
#include "usmdb_util_api.h"
#include "dtlapi.h"
#include "cnfgr.h"

extern L7_BOOL firstSourcePort[];
mirrorCfgData_t      *mirrorCfgData = L7_NULLPTR;  
mirrorSessionInfo_t   mirrorInfo[L7_MIRRORING_MAX_SESSIONS];
/*semaphores*/
void *intfListSema;  /* semaphore to access interface list array*/
/* Interface list arrray*/
L7_uint32 intfList[L7_MAX_INTERFACE_COUNT + 1];


/*******************************************************************
* @purpose  Determine if the interface is attached
*
* @param  intIfNum - internal interface number
*
* @returns  L7_TRUE     interface is attached/attaching
* @returns  L7_FALSE    interface is not attached/attaching
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL mirrorIntfAttached(L7_uint32 intIfNum)
{
  L7_NIM_QUERY_DATA_t       nimQueryData;

  memset((void *)&nimQueryData,0,sizeof(nimQueryData));
  nimQueryData.request  = L7_NIM_QRY_RQST_STATE;
  
  nimQueryData.intIfNum = intIfNum;
  if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
  {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

      /* should never get here */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MIRROR_COMPONENT_ID,
              " nimIntfQuery failed for intf %s\n", ifName);
      return L7_FALSE;
  }
  if (nimQueryData.data.state == L7_INTF_ATTACHED || 
      nimQueryData.data.state == L7_INTF_ATTACHING ||
      nimQueryData.data.state == L7_INTF_DETACHING)
  {
    return L7_TRUE;
  }

  return L7_FALSE;
}


/*********************************************************************
* @purpose  Gets the list of source ports configured for the specified 
*           session
*
* @param  sessionNum   @b{(input))  Session number
*                              Valid range: 1 - L7_MIRRORING_MAX_SESSIONS
* @param  *intIfMask   @b{(output)} Internal mask for source interfaces
*
* @returns  L7_SUCCESS     result is available in *intIfMask
* @returns  L7_NOT_EXIST   session "sessionNum" does not exists
*
* @notes    It is valid for a port not to be configured in the 
*            source port list 
*
* @end
*********************************************************************/
L7_RC_t mirrorSourcePortsGet(L7_uint32 sessionNum,L7_INTF_MASK_t *intIfMask)
{
  L7_RC_t rc = L7_SUCCESS;
  
  if(mirrorIsValidSession(sessionNum) == L7_TRUE)
  {
    memcpy(intIfMask, &(mirrorCfgData->sessionData[sessionNum-1].srcPorts)
           , L7_INTF_INDICES);
  }
  else
    rc = L7_NOT_EXIST;
  return(rc);
}

/*********************************************************************
* @purpose  Get the direction to be mirrored of a source port
*
* @param  intIfNum  (input) internal interface number
* @param  probeType (output) direction of traffic to be mirrored
*
* @returns  L7_SUCCESS     
* @returns  L7_FAILURE    
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mirrorSourcePortDirectionGet(L7_uint32 sessionNum,L7_uint32 intIfNum, L7_MIRROR_DIRECTION_t *probeType)
{

  /* check if session number is valid*/
  if(mirrorIsValidSession(sessionNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  if(nimCheckIfNumber(intIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Check if the interface number is a valid source port */
  if (mirrorIsSrcConfigured(intIfNum, &sessionNum) == L7_FALSE)
  {
    /* since source port is unconfigured , return direction as unconfigured*/
    *probeType = L7_MIRROR_UNCONFIGURED; 
    return L7_SUCCESS;
  }
 
  /*default*/
  *probeType = L7_MIRROR_BIDIRECTIONAL;

  if((L7_INTF_ISMASKBITSET(mirrorCfgData->sessionData[sessionNum-1].srcIngressMirrorType,intIfNum)!=0)
     &&(L7_INTF_ISMASKBITSET(mirrorCfgData->sessionData[sessionNum-1].srcEgressMirrorType,intIfNum)!=0))
  {
    *probeType = L7_MIRROR_BIDIRECTIONAL;
  }
  else if ((L7_INTF_ISMASKBITSET(mirrorCfgData->sessionData[sessionNum-1].srcIngressMirrorType,intIfNum))!=0)
  {
     *probeType = L7_MIRROR_INGRESS; 
  }
  else if ((L7_INTF_ISMASKBITSET(mirrorCfgData->sessionData[sessionNum-1].srcEgressMirrorType,intIfNum))!=0)
  {
     *probeType = L7_MIRROR_EGRESS;
  }
  else
  {
    /* should never reach here. If no direction is specified default of bidirectional should have been recorded*/
    return L7_ERROR;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Add interface to the mirrored ports of session "sessionNum"
*
* @param  sessionNum @b{(input)) Session number 
*                          Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param  intIfNum   @b{(input)} Internal interface number to be added
*
* @param  mode        @b{(input)} Add or remove the port as a mirrored port
*
* @returns  L7_SUCCESS        intIfNum added to the source interface list 
* @returns  L7_NOT_EXIST      session "sessionNum" does not exists
* @returns  L7_REQUEST_DENIED maximum limit of source ports reached
* @returns  L7_FAILURE        intIfNum is not valid 
*
* @notes    This function is not an external api function 
*
* @end
*********************************************************************/
L7_RC_t mirrorSourcePortSet(L7_uint32 sessionNum, L7_uint32 intIfNum,
                            L7_uint32 mode, L7_MIRROR_DIRECTION_t probeType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 dstIntfNum;  
  DTL_MIRROR_t dtlMirrorCmd;

  /* Check if minimum requirement for a funtional port mirroring has been configured*/
  /* before sending a dtl command to push configuration on to hardware */
  /* Minimum conditions: destination port configured, and session enabled */
  /* and atleast one source port configured*/
  
  if (mirrorCfgData->sessionData[sessionNum-1].mode == L7_DISABLE 
      || mirrorDestPortGet(sessionNum, &dstIntfNum) != L7_SUCCESS
      || mirrorIntfAttached(dstIntfNum) == L7_FALSE  
      || mirrorIntfAttached(intIfNum) == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  if (mode == L7_ENABLE)
  {
    dtlMirrorCmd.cmdType.modify.add = L7_TRUE;
  }
  else
  {
    dtlMirrorCmd.cmdType.modify.add = L7_FALSE;
  }

  MIRROR_TRACE("\n\nmirrorSourcePortSet::Sending Dtl Port Modify command.Session :%d,Source Interface: %d,ProbeType: %d\n",
               sessionNum,intIfNum,probeType);

  dtlMirrorCmd.cmdType.modify.sessionNum = sessionNum;
  dtlMirrorCmd.cmdType.modify.srcPort = intIfNum;
  dtlMirrorCmd.cmdType.modify.probeType = probeType;
  
  rc = dtlMirrorPortModify(&dtlMirrorCmd);

  if (rc != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    if (mode == L7_ENABLE)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MIRROR_COMPONENT_ID,
              " mirrorSourcePortSet failed to enable source interface %s\
              in direction %d in session %d.", ifName, probeType,sessionNum);
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_PORT_MIRROR_COMPONENT_ID,
              " mirrorSourcePortSet failed to disable source interface %s\
              in direction %d in session %d.", ifName, probeType,sessionNum);
    }
    
  }
  return(rc);
}

/*********************************************************************
* @purpose  Add interface to the mirrored ports of session "sessionNum"
*
* @param  sessionNum @b{(input)) Session number 
*                          Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param  intIfNum   @b{(input)} Internal interface number to be added
*
* @returns  L7_SUCCESS        intIfNum added to the source interface list 
* @returns  L7_NOT_EXIST      session "sessionNum" does not exists
* @returns  L7_REQUEST_DENIED maximum limit of source ports reached
* @returns  L7_FAILURE        intIfNum is not valid or probeType is not valid 
* @returns  L7_ERROR          previous source port configuration could not be
*                             removed to add new one 
*
* @notes
*
* @end
*********************************************************************/
                            
L7_RC_t mirrorSourcePortAdd(L7_uint32 sessionNum, L7_uint32 intIfNum,
                             L7_MIRROR_DIRECTION_t probeType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 maskOffset, numPorts; 
  L7_BOOL status, firstSrcPort = L7_FALSE; 
  L7_uint32 dstIntIfNum;
  L7_BOOL srcPortPresent=L7_FALSE;
  L7_MIRROR_DIRECTION_t oldProbeType;

  MIRROR_TRACE("\n\nEntering mirrorSourcePortAdd::Session :%d,Source Interface: %d,ProbeType: %d\n",
               sessionNum,intIfNum,probeType);
  

  if(mirrorIsValidSession(sessionNum) == L7_FALSE)
  {
    return L7_NOT_EXIST;
  }
  
  if ((nimConfigIdMaskOffsetGet(intIfNum,&maskOffset) != L7_SUCCESS)
      || (mirrorIsValidSrcIntf(intIfNum, &status) != L7_SUCCESS)
      || (status != L7_TRUE)
      || (mirrorIsDestConfigured(intIfNum, L7_NULLPTR) != L7_FALSE)
      )
  {
    return L7_FAILURE;
  }

  /* check if valid probeType value */
  if((probeType != L7_MIRROR_BIDIRECTIONAL) && 
      (probeType != L7_MIRROR_INGRESS) &&
      (probeType != L7_MIRROR_EGRESS))
  {
    return L7_FAILURE;
  }

 
  if (L7_INTF_ISMASKBITSET(mirrorCfgData->sessionData[sessionNum-1].srcPorts,
                               maskOffset))
  {
    srcPortPresent = L7_TRUE;
  }            

  /* take semaphore */
  osapiSemaTake(intfListSema,L7_NO_WAIT);

  if (nimMaskToList((NIM_INTF_MASK_t *)&(mirrorCfgData->sessionData[sessionNum-1].srcPorts), 
                    intfList, &numPorts) != L7_SUCCESS)
    return L7_FAILURE;

  /* release semaphore */
  osapiSemaGive(intfListSema);

  if (numPorts == 0)
  {
    /* This is the first Source Port */
    firstSrcPort = L7_TRUE;
  }

  if((numPorts >= L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION) &&
     (srcPortPresent != L7_TRUE)) 
    return L7_REQUEST_DENIED;

  if(srcPortPresent == L7_TRUE)
  {
    if ((mirrorSourcePortDirectionGet(sessionNum,intIfNum,(L7_MIRROR_DIRECTION_t *)&oldProbeType)) != L7_SUCCESS)
    {
         return L7_FAILURE;
    }

    if(oldProbeType == probeType)
    {
      return L7_SUCCESS;
    }
    else
    {
      /* remove the source port configuration and add it again*/
       L7_INTF_CLRMASKBIT(mirrorCfgData->sessionData[sessionNum-1].srcPorts, intIfNum);
       L7_INTF_CLRMASKBIT(mirrorCfgData->sessionData[sessionNum-1].srcIngressMirrorType,intIfNum);
       L7_INTF_CLRMASKBIT(mirrorCfgData->sessionData[sessionNum-1].srcEgressMirrorType,intIfNum);
       mirrorCfgData->cfgHdr.dataChanged = L7_TRUE;

       rc = mirrorSourcePortSet(sessionNum, intIfNum, L7_DISABLE, oldProbeType);
       if (rc != L7_SUCCESS)
       {
          return L7_ERROR;
       }
    }
  }
  
  L7_INTF_SETMASKBIT(mirrorCfgData->sessionData[sessionNum-1].srcPorts, maskOffset);

  /*--------------------------------------------------------------------------*/
  /* Set Probe Direction  - Begin                                             */
  /*--------------------------------------------------------------------------*/
  if (probeType == L7_MIRROR_BIDIRECTIONAL)
  {
     L7_INTF_SETMASKBIT(mirrorCfgData->sessionData[sessionNum-1].srcIngressMirrorType,intIfNum);
     L7_INTF_SETMASKBIT(mirrorCfgData->sessionData[sessionNum-1].srcEgressMirrorType,intIfNum);

  }
  else if (probeType == L7_MIRROR_INGRESS)
  {
    L7_INTF_SETMASKBIT(mirrorCfgData->sessionData[sessionNum-1].srcIngressMirrorType,intIfNum);

  }
  else if (probeType == L7_MIRROR_EGRESS)
  {
    L7_INTF_SETMASKBIT(mirrorCfgData->sessionData[sessionNum-1].srcEgressMirrorType,intIfNum);
  }
  else
  {
    L7_INTF_CLRMASKBIT(mirrorCfgData->sessionData[sessionNum-1].srcPorts, intIfNum);
    return L7_FAILURE;
  }
  
  /*--------------------------------------------------------------------------*/
  /* Set Probe Direction  - End                                               */
  /*--------------------------------------------------------------------------*/
  mirrorCfgData->cfgHdr.dataChanged = L7_TRUE;
   
  rc = mirrorSourcePortSet(sessionNum, intIfNum, L7_ENABLE, probeType);

  /* remove success check before NIM event notify , to be on the safer side*/
  
  /* If first src port, dst port configured and session enabled,
   * then we need to advertize probe setup for the dest port
   */
   if (firstSrcPort == L7_TRUE &&
       mirrorCfgData->sessionData[sessionNum-1].mode == L7_ENABLE &&
	   mirrorDestPortGet(sessionNum, &dstIntIfNum) == L7_SUCCESS)
   {
     MIRROR_TRACE("\n\nmirrorSourcePortAdd::Calling mirrorNimEventIntfNotify with L7_PROBE_SETUP for Interface: %d.\n",dstIntIfNum);

     mirrorNimEventIntfNotify(dstIntIfNum, L7_PROBE_SETUP);
     firstSourcePort[sessionNum-1] = L7_FALSE;
   }


  return rc;

}

/*********************************************************************
* @purpose  Remove interface from mirrored ports of session "sessionNum"
*
* @param  sessionNum @b{(input)) Session number 
*                         Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param  intIfNum   @b{(input)} Internal interface number to remove
*
* @returns  L7_SUCCESS    intIfNum is removed from source list
* @returns  L7_NOT_EXIST  session does not exists
* @returns  L7_FAILURE    intIfNum is not in source list of sessionNum
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mirrorSourcePortRemove(L7_uint32 sessionNum,L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;

  MIRROR_TRACE("\n\nEntering mirrorSourcePortRemove::Session :%d,Source Interface: %d\n",
               sessionNum,intIfNum);

  if(mirrorIsValidSession(sessionNum) == L7_FALSE)
  {
    return L7_NOT_EXIST;
  }
  
  if(nimCheckIfNumber(intIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
 
  if (L7_INTF_ISMASKBITSET(mirrorCfgData->sessionData[sessionNum-1].srcPorts,
                               intIfNum) == 0)
  {
    /* if given port is not a source port in this session, then no configuration 
       needs to be changed hence it is a success*/ 
    return L7_SUCCESS;
  }

  L7_INTF_CLRMASKBIT(mirrorCfgData->sessionData[sessionNum-1].srcPorts, intIfNum);
  L7_INTF_CLRMASKBIT(mirrorCfgData->sessionData[sessionNum-1].srcIngressMirrorType,intIfNum);
  L7_INTF_CLRMASKBIT(mirrorCfgData->sessionData[sessionNum-1].srcEgressMirrorType,intIfNum);
  mirrorCfgData->cfgHdr.dataChanged = L7_TRUE;
    
   /* if this is the last source port to be removed then the session is inactive
   * hence the probe port,if configured, has to be released.*/

  /* The last argument, probeType, is taken from the config 
  as the UI does not send it */
  rc = mirrorSourcePortConfigUnApply(sessionNum, intIfNum);
  
  return(rc);
}
/*********************************************************************
* @purpose  Remove interface from mirrored ports of session "sessionNum"
*           from hw
*
* @param  sessionNum @b{(input)) Session number
*                         Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param  intIfNum   @b{(input)} Internal interface number to remove
*
* @returns  L7_SUCCESS    intIfNum is removed from source list
* @returns  L7_NOT_EXIST  session does not exists
* @returns  L7_FAILURE    intIfNum is not in source list of sessionNum
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mirrorSourcePortConfigUnApply(L7_uint32 sessionNum, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_MIRROR_DIRECTION_t probeType;
  L7_uint32 i, dstIntIfNum;
  DTL_MIRROR_t dtlMirrorCmd;
  L7_BOOL srcConfigured = L7_FALSE;

  MIRROR_TRACE("\n\nEntering mirrorSourcePortConfigUnApply::Session :%d,Source Interface: %d\n",
               sessionNum,intIfNum);

  if((mirrorSourcePortDirectionGet(sessionNum,intIfNum,&probeType))!=L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  rc = mirrorSourcePortSet(sessionNum, intIfNum, L7_DISABLE, probeType);

  if (rc == L7_SUCCESS && mirrorDestPortGet(sessionNum, &dstIntIfNum) == L7_SUCCESS &&
      mirrorIntfAttached(dstIntIfNum) == L7_TRUE)
  {
    for (i=1;i<=L7_MAX_INTERFACE_COUNT;i++)
    {
      if (L7_INTF_ISMASKBITSET(mirrorCfgData->sessionData[sessionNum-1].srcPorts,i) && (i != intIfNum))
      {
        srcConfigured = L7_TRUE;
        break;
      }
    }

    /* Remove the session from hw if this is last source port */ 
    if ((srcConfigured == L7_FALSE) && (mirrorCfgData->sessionData[sessionNum-1].mode == L7_ENABLE))
    {
      bzero((char *)&dtlMirrorCmd, (L7_int32)sizeof(dtlMirrorCmd));

      if (mirrorCfgData->sessionData[sessionNum-1].mode != L7_ENABLE)
      {
        dtlMirrorCmd.cmdType.config.enable = L7_FALSE;
      }
      else
      {
        dtlMirrorCmd.cmdType.config.enable = L7_TRUE;
      }

      dtlMirrorCmd.cmdType.config.destPort = dstIntIfNum;
      dtlMirrorCmd.cmdType.config.probeSpecified = L7_TRUE;

      MIRROR_TRACE("\n\nmirrorSourcePortConfigUnApply::Sending Dtl Session Config command.Session : %d,Destination Interface: %d, Source Interface: %d",sessionNum, dstIntIfNum, intIfNum);
      rc = dtlMirrorSessionConfig(&dtlMirrorCmd);
      if (rc == L7_SUCCESS)
      {
        MIRROR_TRACE("\n\nmirrorSourcePortConfigUnApply::Calling mirrorNimEventIntfNotify with L7_PROBE_TEARDOWN for Interface: %d.\n",dstIntIfNum);
        mirrorNimEventIntfNotify(dstIntIfNum, L7_PROBE_TEARDOWN);
        firstSourcePort[sessionNum-1] = L7_TRUE;
      }
    }
  }
  return rc;
}
/*********************************************************************
* @purpose  Determine if an interface is an active probe port
*
* @param    intIfNum  @b((input)) Interface number 
*
* @returns  L7_TRUE, intIfNum is configured as active probe port  
* @returns  L7_FALSE, intIfNum is either not configured as probe port 
*                       or it is not active
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL mirrorIsActiveProbePort(L7_uint32 intIfNum)
{
  L7_BOOL rc = L7_FALSE;
  L7_uint32 sessionNum,i;
  L7_BOOL srcConfiguredAttached = L7_FALSE;
  L7_uint32 numSrcIntf = 0;

  if(mirrorIsDestConfigured(intIfNum, &sessionNum) == L7_TRUE && mirrorIntfAttached(intIfNum) == L7_TRUE)
  {
    /* take semaphore */
    osapiSemaTake(intfListSema,L7_NO_WAIT);

    nimMaskToList((NIM_INTF_MASK_t *)&(mirrorCfgData->sessionData[sessionNum-1].srcPorts)
				  , intfList, &numSrcIntf);
    if (numSrcIntf > L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION)
    {
      /* should never get here */
      MIRROR_TRACE("\n\n mirrorIsActiveProbePort:: Error!!Number of source interface : %d is greater than Maximum number of source interfaces allowed : %d.\n",
                   numSrcIntf,L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION) ;
	  /* release semaphore */
      osapiSemaGive(intfListSema);
      return L7_FALSE;
      
    }
	for (i = 0; i < numSrcIntf; i++)
	{
	  if (mirrorIntfAttached(intfList[i]) == L7_TRUE)
	  {
		srcConfiguredAttached = L7_TRUE;
		break;
	  }
	}
    /* release semaphore */
    osapiSemaGive(intfListSema);

	if(mirrorCfgData->sessionData[sessionNum-1].mode == L7_ENABLE && 
	   srcConfiguredAttached == L7_TRUE)
      rc = L7_TRUE;
  }
  return rc;
}

/*********************************************************************
* @purpose  Returns Destination Port for session "sessionNum"
*
* @param  sessionNum @b{(input)) Session number 
*                                Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param  *intIfNum  @b((output)) Interface number of destination port
*
* @returns  L7_SUCCESS, destination interface is available in *intIfNum 
* @returns  L7_FAILURE, destination interface not configured for session 
* @returns  L7_NOT_EXIST, session "sessionNum" does not exist
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t mirrorDestPortGet(L7_uint32 sessionNum,L7_uint32 *intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;
  nimConfigID_t nullCfgID;

  if(mirrorIsValidSession(sessionNum) == L7_FALSE)
  {
    return L7_NOT_EXIST;
  }
  memset((void *)&nullCfgID,0,sizeof(nimConfigID_t));

  if (NIM_CONFIG_ID_IS_EQUAL(&nullCfgID,
        &(mirrorCfgData->sessionData[sessionNum-1].destID)) == L7_TRUE )
  {
    rc = L7_FAILURE;
    *intIfNum = 0;
  }
  else if ((rc = nimIntIfFromConfigIDGet(&(mirrorCfgData->sessionData[sessionNum-1].destID),
                                           intIfNum)) != L7_SUCCESS)
  {
    rc = L7_FAILURE;
    *intIfNum = 0;
  }
  return(rc);
}


/*********************************************************************
* @purpose  Sets Destination Port for session "sessionNum"
*
* @param  sessionNum   @b{(input)) Session number 
*                            Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param  intIfNum     @b{(input)} The internal interface number to set
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXIST, session "sessionNum" is not valid
* @returns  L7_FAILURE, intIfNum is not valid
* @returns  L7_ALREADY_CONFIGURED, destination port must be removed before 
*                          re-configuring it
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mirrorDestPortSet(L7_uint32 sessionNum, L7_uint32 intIfNum)
{
  L7_RC_t rc=L7_SUCCESS;
  L7_BOOL status;
  L7_uint32 tempSession;
 
  MIRROR_TRACE("\n\nEntering mirrorDestPortSet::Session :%d,Destination Interface: %d\n",
               sessionNum,intIfNum);

  if(mirrorIsValidSession(sessionNum) == L7_FALSE)
    return L7_NOT_EXIST;

  if (mirrorDestEmpty(sessionNum) == L7_FALSE)
  {
    /* 
     * destination port should be removed before reconfiguring it 
    */
    return L7_ALREADY_CONFIGURED;
  }
   
  if (mirrorIsValidDestIntf(intIfNum, &status) != L7_SUCCESS 
      || status != L7_TRUE 
      || mirrorIsSrcConfigured(intIfNum, &tempSession) == L7_TRUE 
      || mirrorIsDestConfigured(intIfNum,L7_NULLPTR) == L7_TRUE)
  {
    return L7_FAILURE;
  }

  if (nimConfigIdGet(intIfNum, &(mirrorCfgData->sessionData[sessionNum-1].destID))
       != L7_SUCCESS)
  {
    memset((void *)&(mirrorCfgData->sessionData[sessionNum-1].destID),
           0, sizeof(nimConfigID_t));
    return L7_FAILURE;
  }

  mirrorCfgData->cfgHdr.dataChanged = L7_TRUE;

  /* Check if minimum requirement for a funtional port mirroring has been configured*/
  /* before sending a dtl command to push configuration on to hardware */
  /* Minimum conditions: destination port configured, and session enabled */
  
  if (mirrorCfgData->sessionData[sessionNum-1].mode != L7_ENABLE ||
      mirrorIntfAttached(intIfNum) == L7_FALSE)
  {
    return L7_SUCCESS;
  }
  
  rc = mirrorSessionConfig(sessionNum);
  
  return(rc);
}

/*********************************************************************
* @purpose  Remove Destination Port for session "sessionNum"
*
* @param  sessionNum @b{(input))  Session number 
*                             Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
*
* @returns  L7_SUCCESS, destination port is removed
* @returns  L7_NOT_EXIST, session "sessionNum" is not valid
* @returns  L7_FAILURE, destination is not set for this session
*
* @notes  
*
* @end
*********************************************************************/
L7_RC_t mirrorDestPortRemove(L7_uint32 sessionNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 intIfNum;

  MIRROR_TRACE("\n\nEntering mirrorDestPortRemove::Session :%d\n",sessionNum);

  if(mirrorIsValidSession(sessionNum) == L7_FALSE) 
  {
    return L7_NOT_EXIST;
  }

  if (mirrorDestPortGet(sessionNum, &intIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if(nimCheckIfNumber(intIfNum) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  memset((void *)&(mirrorCfgData->sessionData[sessionNum-1].destID),
           0, sizeof(nimConfigID_t));
  mirrorCfgData->cfgHdr.dataChanged = L7_TRUE;

  if (mirrorIntfAttached(intIfNum) == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  /* Check if minimum requirement for a funtional port mirroring has been set up*/
  /* before sending a dtl command to push configuration on to hardware */
  /* Minimum conditions: destination port configured, and session enabled */
  /* check if session enabled before sending dtl command */
  if (mirrorCfgData->sessionData[sessionNum-1].mode == L7_DISABLE)
  {
    return L7_SUCCESS;
  }

  rc = mirrorDestPortConfigUnApply(sessionNum, intIfNum);

  return rc;
}

/*********************************************************************
* @purpose  Remove Destination Port From H/W for session "sessionNum"
*
* @param  sessionNum @b{(input))  Session number
*                             Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
*
* @returns  L7_SUCCESS, destination port is removed
* @returns  L7_NOT_EXIST, session "sessionNum" is not valid
* @returns  L7_FAILURE, destination is not set for this session
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t mirrorDestPortConfigUnApply(L7_uint32 sessionNum, L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_FAILURE;
  L7_ushort16 i;
  dtlMirrorSrcPortInfo_t intfInfoList[L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION];
  DTL_MIRROR_t dtlMirrorCmd;
  L7_INTF_MASK_t presentMask, tempIfMask_g;
  L7_uint32 numSrcIntf = 0;
  L7_MIRROR_DIRECTION_t tempProbeType=L7_MIRROR_BIDIRECTIONAL;

  MIRROR_TRACE("\n\nEntering mirrorDestPortConfigUnApply::Session :%d\n",sessionNum);

  if(mirrorIsValidSession(sessionNum) == L7_FALSE)
  {
    return L7_NOT_EXIST;
  }

  if (mirrorIntfAttached(intIfNum) == L7_FALSE)
  {
    return L7_SUCCESS;
  }

  /* Check if minimum requirement for a funtional port mirroring has been set up*/
  /* before sending a dtl command to push configuration on to hardware */
  /* Minimum conditions: destination port configured, and session enabled */
  /* check if session enabled before sending dtl command */
  if (mirrorCfgData->sessionData[sessionNum-1].mode == L7_DISABLE)
  {
    return L7_SUCCESS;
  }

  bzero((char *)&dtlMirrorCmd, (L7_int32)sizeof(dtlMirrorCmd));

  dtlMirrorCmd.cmdType.config.sessionNum = sessionNum;
  dtlMirrorCmd.cmdType.config.enable = L7_FALSE;
  dtlMirrorCmd.cmdType.config.destPort = intIfNum;
  dtlMirrorCmd.cmdType.config.probeType = FD_MIRRORING_DIRECTION;
  dtlMirrorCmd.cmdType.config.probeSpecified = L7_TRUE;

  nimIntfMaskOfPresentGet((NIM_INTF_MASK_t *)&presentMask);         
  memcpy(&tempIfMask_g, &(mirrorCfgData->sessionData[sessionNum-1].srcPorts), L7_INTF_INDICES);
  /* mask the detached interfaces */
  L7_INTF_MASKANDEQ(tempIfMask_g,presentMask);

  /* take semaphore */
  osapiSemaTake(intfListSema,L7_NO_WAIT);

  nimMaskToList((NIM_INTF_MASK_t *)&tempIfMask_g, intfList, 
                &(dtlMirrorCmd.cmdType.config.numOfSrcPortEntries));

  numSrcIntf = dtlMirrorCmd.cmdType.config.numOfSrcPortEntries;

  if (numSrcIntf > L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION)
  {
    /* should never get here */
    MIRROR_TRACE("\n\n mirrorDestPortConfigUnApply:: Error!!Number of source interface : %d is greater than Maximum number of source interfaces allowed : %d.\n", numSrcIntf,L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION) ;
	  /* release the interface list semaphore */
    osapiSemaGive(intfListSema);
    return L7_FAILURE;
  }

  /* Create the data to send to DTL */
  for (i = 0; i < dtlMirrorCmd.cmdType.config.numOfSrcPortEntries; i++ )
  {
    intfInfoList[i].intIfNum = intfList[i];

    if(mirrorSourcePortDirectionGet(sessionNum,intfList[i],&tempProbeType)!= L7_SUCCESS)
    {
      /* should never get here */
      /* release the interface list semaphore */
      osapiSemaGive(intfListSema);
      return L7_FAILURE;
    }
    intfInfoList[i].probeType = tempProbeType;
  }
  dtlMirrorCmd.cmdType.config.srcPortInfo = intfInfoList;

  /* relase the interface list semaphore */
  osapiSemaGive(intfListSema);

  MIRROR_TRACE("\n\nmirrorDestPortConfigUnApply::Sending Dtl Session Config command.Session :%d,Destination Interface: %d,:No.of Source Interfaces: %d", sessionNum,intIfNum,numSrcIntf);
  MIRROR_TRACE("\nSourceInterface  Direction");
  for(i=0;i<numSrcIntf;i++)
  {
      MIRROR_TRACE("\n%14d  %5d",intfInfoList[i].intIfNum,intfInfoList[i].probeType); 
  }

  rc = dtlMirrorSessionConfig(&dtlMirrorCmd);
  /* Send a probe teardown only if the mirror was enabled and had atleat one src port */
  if (rc == L7_SUCCESS &&
      numSrcIntf > 0 &&
      mirrorCfgData->sessionData[sessionNum-1].mode == L7_ENABLE)
  {
    MIRROR_TRACE("\n\nmirrorDestPortConfigUnApply::Calling mirrorNimEventIntfNotify with L7_PROBE_TEARDOWN for Interface: %d.\n",intIfNum);
    mirrorNimEventIntfNotify(intIfNum, L7_PROBE_TEARDOWN);
  }
  return rc;
}

/*********************************************************************
* @purpose  Returns the Port Monitoring Mode for session "sessionNum" 
*
* @param    sessionNum   @b{(input))  Session number 
*                      Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param    *mode       @b{(output)} mode of the session 
*
* @returns  L7_SUCCESS, enable/disable status is available in *mode
* @returns  L7_NOT_EXIST, sessionNum does not exists 
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t mirrorModeGet(L7_uint32 sessionNum,L7_BOOL *mode)
{
  L7_RC_t rc = L7_SUCCESS;

  if(mirrorIsValidSession(sessionNum) == L7_FALSE)
  {
    rc = L7_NOT_EXIST;
  }
  else
  {
    *mode = mirrorCfgData->sessionData[sessionNum-1].mode; 
    rc = L7_SUCCESS;
  }
  return rc;
}

/*********************************************************************
* @purpose  Sets the Port Monitoring Mode for session "sessionNum" 
*
* @param    sessionNum  @b{(input)) Session number
*                           Valid range is 1 - L7_MIRRORING_MAX_SESSIONS
* @param    mode        @b{(input)} Monitoring Mode (enable/disable)  
*
* @returns L7_SUCCESS, mirroring mode for session is set successfully
* @returns L7_NOT_EXIST, sessionNum does not exists
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t mirrorModeSet(L7_uint32 sessionNum, L7_uint32 mode)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;
  
  MIRROR_TRACE("\n\nEntering mirrorModeSet::Session :%d , Mode :%d\n",sessionNum,mode);
  
  if(mirrorIsValidSession(sessionNum) == L7_FALSE)
  {
    return L7_NOT_EXIST;
  }

  if (mode == mirrorCfgData->sessionData[sessionNum-1].mode)
      return L7_SUCCESS;

  mirrorCfgData->sessionData[sessionNum-1].mode = mode;
  mirrorCfgData->cfgHdr.dataChanged     = L7_TRUE;

  /* Check if minimum requirement for a funtional port mirroring has been configured*/
  /* before sending a dtl command to push configuration on to hardware */
  /* Minimum conditions: destination port configured, and session enabled */

  if ((mirrorDestPortGet(sessionNum, &intIfNum) != L7_SUCCESS) ||
     ( mirrorIntfAttached(intIfNum) == L7_FALSE)) 
  {
    return L7_SUCCESS;
  }

  rc = mirrorSessionConfig(sessionNum);
    
  return rc;
}

/*********************************************************************
* @purpose  Returns count of Port Monitoring sessions
*
* @param   none
*
* @returns  Count of sessions
*
* @notes  It returns maximum number of sessions that can be configured.
*
* @end
*********************************************************************/
L7_uint32 mirrorSessionCountGet()
{
  return L7_MIRRORING_MAX_SESSIONS;
}

/*********************************************************************
* @purpose  Removes port monitoring configuration for a session
*
* @param    void
*
* @returns  L7_SUCCESS, mirroring config data is removed successfully
*
* @notes    disables any current port monitoring setup, removes current 
*           values for src and dest ports, replaces them with zeros, 
*           the default configuration. 
*            
* @end
*********************************************************************/
L7_RC_t mirrorConfigRemove(L7_uint32 sessionNum)
{
  L7_uint32 i, numIntf = 0;
  L7_RC_t rc;
  L7_uint32 intIfNum = 0;

  MIRROR_TRACE("\n\nEntering mirrorConfigRemove::Session :%d\n",sessionNum);

  /* do not send a DTL command unless required */
  
  if (mirrorDestPortGet(sessionNum, &intIfNum) == L7_SUCCESS)
  {
    rc = mirrorDestPortRemove(sessionNum); 
    
    if (rc != L7_SUCCESS)
    {
      return rc;
    }
  }
  
  /* take semaphore */
  osapiSemaTake(intfListSema,L7_NO_WAIT);

  rc = nimMaskToList((NIM_INTF_MASK_t *)&(mirrorCfgData->sessionData[sessionNum-1].srcPorts), 
                     intfList, &numIntf);
  if (rc != L7_SUCCESS)
  {
     osapiSemaGive(intfListSema);
	 return rc;
  }

  if (numIntf > L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION)
  {
      /* should never get here */
      MIRROR_TRACE("\n\n mirrorConfigRemove:: Error!!Number of source interface : %d is greater than Maximum number of source interfaces allowed : %d.\n",
                   numIntf,L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION) ;

      osapiSemaGive(intfListSema);
      return L7_FAILURE;
  }
  for (i=0; i<numIntf; i++)
  {
    if (mirrorSourcePortRemove(sessionNum, intfList[i]) != L7_SUCCESS)
    {
      osapiSemaGive(intfListSema);
      return L7_FAILURE;
    }
  }

  /* release interface list sema*/
  osapiSemaGive(intfListSema);

  /* set the mode last so that appropriate checks can be made in destination remove and source remove*/

  mirrorModeSet(sessionNum, L7_DISABLE); /* return value check removed because of defect 22939*/

  
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Removes port monitoring configuration for all sessions
*
* @param    void
*
* @returns  L7_SUCCESS, mirroring config data is removed successfully
*
* @notes    disables any current port monitoring setup, removes current 
*           values for src and dest ports, replaces them with zeros, 
*           the default configuration. 
*            
* @end
*********************************************************************/
L7_RC_t mirrorConfigRemoveAll()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 sessionIndex, sessionCount;

  MIRROR_TRACE("\n\nEntering mirrorConfigRemoveAll.\n");

  sessionCount = mirrorSessionCountGet(); 
  for(sessionIndex=1; sessionIndex <= sessionCount; sessionIndex++)
  {
    mirrorConfigRemove(sessionIndex);
  }
  return(rc);
}

/*********************************************************************
* @purpose  Removes port monitoring configuration for sessionNum
*
* @param    sessionNum   session number
*
* @param    sessionNum   mode - enable or disable the session
*
* @returns  L7_SUCCESS
* @returns  L7_NOT_EXIST, session is not valid
*
* @notes    disables any current port monitoring setup, removes current values
*           for src and dest ports, replaces them with zeros,
*           the default configuration.
*
* @end
*********************************************************************/
L7_RC_t mirrorSessionConfig(L7_uint32 sessionNum)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 intIfNum;
  L7_ushort16 i;
  dtlMirrorSrcPortInfo_t intfInfoList[L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION];

  DTL_MIRROR_t dtlMirrorCmd;
  L7_INTF_MASK_t presentMask, tempIfMask_g;
  L7_BOOL probeSpecified = L7_FALSE;
  L7_uint32 numSrcIntf = 0;
  L7_MIRROR_DIRECTION_t tempProbeType = L7_MIRROR_BIDIRECTIONAL;

  MIRROR_TRACE("\n\nEntering mirrorSessionConfig::SessionNum: %d.\n",sessionNum);

  if(mirrorIsValidSession(sessionNum) == L7_FALSE)
  {
    return L7_NOT_EXIST;
  }

  bzero((char *)&dtlMirrorCmd, (L7_int32)sizeof(dtlMirrorCmd));

  dtlMirrorCmd.cmdType.config.enable = L7_TRUE;

  if (mirrorDestPortGet(sessionNum, &intIfNum) == L7_SUCCESS &&
      mirrorIntfAttached(intIfNum) == L7_TRUE)
  {
    dtlMirrorCmd.cmdType.config.destPort = intIfNum;
    dtlMirrorCmd.cmdType.config.probeSpecified = L7_TRUE;
	probeSpecified = L7_TRUE;
  }
  else
  {
    dtlMirrorCmd.cmdType.config.destPort = L7_NULL;
    dtlMirrorCmd.cmdType.config.probeSpecified = L7_FALSE;
  }
  
  if (mirrorCfgData->sessionData[sessionNum-1].mode != L7_ENABLE)
  {
      dtlMirrorCmd.cmdType.config.enable = L7_FALSE;
  }
  
  nimIntfMaskOfPresentGet((NIM_INTF_MASK_t *)&presentMask);         
  memcpy(&tempIfMask_g, &(mirrorCfgData->sessionData[sessionNum-1].srcPorts), L7_INTF_INDICES);
  /* mask the detached interfaces */
  L7_INTF_MASKANDEQ(tempIfMask_g,presentMask);

  /* take semaphore */
  osapiSemaTake(intfListSema,L7_NO_WAIT);

  nimMaskToList((NIM_INTF_MASK_t *)&tempIfMask_g, intfList, 
                &(dtlMirrorCmd.cmdType.config.numOfSrcPortEntries));

   numSrcIntf = dtlMirrorCmd.cmdType.config.numOfSrcPortEntries;

   if (numSrcIntf > L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION)
   {
     /* should never get here */
     MIRROR_TRACE("\n\n mirrorSessionConfig:: Error!!Number of source interface : %d is greater than Maximum number of source interfaces allowed : %d.\n",
                     numSrcIntf,L7_MIRRORING_MAX_SRC_PORTS_PER_SESSION) ;
     osapiSemaGive(intfListSema);
     return L7_FAILURE;

   }

  /* Create the data to send to DTL */
  for (i = 0; i < dtlMirrorCmd.cmdType.config.numOfSrcPortEntries; i++ )

  {
    intfInfoList[i].intIfNum = intfList[i];
    mirrorSourcePortDirectionGet(sessionNum,intfList[i],&tempProbeType);
    intfInfoList[i].probeType = tempProbeType;

  }
  /* release semaphore */
  osapiSemaGive(intfListSema);

  dtlMirrorCmd.cmdType.config.srcPortInfo = intfInfoList;


  MIRROR_TRACE("\n\nmirrorSessionConfig::Sending Dtl Session Config command.Session :%d,Destination Interface: %d,:No.of Source Interfaces: %d",
               sessionNum,intIfNum,numSrcIntf);
  MIRROR_TRACE("\nSource Interface  Direction");
  for(i=0;i<numSrcIntf;i++)
  {
      MIRROR_TRACE("\n%14d  %5d",intfInfoList[i].intIfNum,intfInfoList[i].probeType); 
  }

  rc = dtlMirrorSessionConfig(&dtlMirrorCmd);

  if (rc == L7_SUCCESS)
  {
	/* Notify rest of the system of the status of the probe interface 
	 * mirror enabled  probe specified     atleast one src port -> L7_PROBE_SETUP
	 * mirror enabled  probe not specified atleast one src port -> L7_PROBE_TEARDOWN
	 * mirror disabled probe specified     atleast one src port -> L7_PROBE_TEARDOWN
	 * mirror disabled probe not specified atleast one src port -> No Notification
	 * mirror enabled  probe specified     no src ports         -> No Notification 
	 * mirror enabled  probe not specified no src ports         -> No Notification
	 * mirror disabled probe not specified no src ports         -> No Notification
	 * mirror disabled probe specified     no src ports         -> No Notification
	 *
	 * NOTE: the three parms cannot change simultaneously. i.e. each parm changes one 
	 *       at a time. We cannot have the case of the mirror being disabled as well 
	 *       as the probe being specified. Also src port going to zero does not come
	 *       via this routine hence there is no notification sent for 
	 *       "mirror enabled  probe specified     no src ports"
	 */ 
	if (mirrorCfgData->sessionData[sessionNum-1].mode == L7_ENABLE && 
		probeSpecified == L7_TRUE && 
		numSrcIntf > 0)
	{
      MIRROR_TRACE("\n\nmirrorSessionConfig::Calling mirrorNimEventIntfNotify with L7_PROBE_SETUP for Interface: %d.\n"
                   ,intIfNum);

      mirrorNimEventIntfNotify(intIfNum, L7_PROBE_SETUP);
	}
	else if ((mirrorCfgData->sessionData[sessionNum-1].mode == L7_ENABLE && 
		      probeSpecified == L7_FALSE &&
			  numSrcIntf > 0) ||
			 (mirrorCfgData->sessionData[sessionNum-1].mode == L7_DISABLE &&
			  probeSpecified == L7_TRUE &&
			  numSrcIntf > 0))
	{
      MIRROR_TRACE("\n\nmirrorSessionConfig::Calling mirrorNimEventIntfNotify with L7_PROBE_TEARDOWN for Interface: %d.\n"
                   ,intIfNum);

      mirrorNimEventIntfNotify(intIfNum, L7_PROBE_TEARDOWN);
	}

  }

  return rc;
}

/*********************************************************************
* @purpose  Check if intIfNum is configured as probe port in any session
*
* @param  intIfNum  @b{(input)}   The internal interface number being validated
* @param  *sessionNum @b{(output)} Session number in which this interface is
*                                    configured as probe port
*
* @returns  L7_TRUE   intIfNum is configured as probe port in some of the sessions
* @returns  L7_FALSE  intIfNum is not configured as probe port in any of the sessions
*
* @notes  sessionNum is valid only if return value is L7_TRUE
*
* @end
*********************************************************************/
L7_BOOL mirrorIsDestConfigured(L7_uint32 intIfNum, L7_uint32 *sessionNum)
{
  L7_BOOL rc = L7_FALSE;
  L7_uint32 sessionIndex,sessionCount;
  nimConfigID_t tempCfgID;

  if(nimConfigIdGet(intIfNum,&tempCfgID) != L7_SUCCESS)
    return L7_FALSE;

  sessionCount = mirrorSessionCountGet();
  for( sessionIndex=0;sessionIndex<sessionCount; sessionIndex++ )
  {
    if( NIM_CONFIG_ID_IS_EQUAL(&tempCfgID,
        &(mirrorCfgData->sessionData[sessionIndex].destID)) == L7_TRUE )
    {
      if(sessionNum != L7_NULLPTR)
        *sessionNum = sessionIndex + 1;
      rc=L7_TRUE;
      break;
    }
  }
  return(rc);
}

/*********************************************************************
* @purpose  whether the interface is configured as source in any of the sessions
*
* @param    intIfNum     @b{(input)} The internal interface number being validated
* @param    *sessionNum  @b{(input)} Pointer to where session is stored
*
* @returns  L7_TRUE   intIfNum is configured as sourc port in some of the sessions
* @returns  L7_FALSE  intIfNum is not configured as source port in any of the sessions
*
* @notes   None
*
* @end
*********************************************************************/
L7_BOOL mirrorIsSrcConfigured(L7_uint32 intIfNum, L7_uint32 *sessionNum)
{
  L7_BOOL rc = L7_FALSE;
  L7_uint32 sessionIndex,sessionCount;

  *sessionNum = 0;
  sessionCount = mirrorSessionCountGet();
  for( sessionIndex=0;sessionIndex<sessionCount; sessionIndex++ )
  {
    if( L7_INTF_ISMASKBITSET(mirrorCfgData->sessionData[sessionIndex].srcPorts, intIfNum) != 0)
    {
      *sessionNum = sessionIndex + 1;
      rc = L7_TRUE;
      break;
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  check validity of interface for a sourceport 
*
* @param    intIfNum     @b{(input)} The internal interface number being validated
* 
* @returns  L7_TRUE   intIfNum can be configured as source port 
* @returns  L7_FALSE  intIfNum cannot be configured as source port 
*
* @notes   checks if the interface is a physical port or 
*          CPU port when feature is present  
*
* @end
*********************************************************************/
L7_BOOL mirrorValidSrcIntfCheck(L7_uint32 intIfNum)
{
  L7_uint32 maskOffset=0;
  L7_BOOL status= L7_FALSE;

  /* Source port valid check*/
  if ((nimConfigIdMaskOffsetGet(intIfNum,&maskOffset) != L7_SUCCESS)
      || (mirrorIsValidSrcIntf(intIfNum, &status) != L7_SUCCESS)
      || (status != L7_TRUE)
     )
    return L7_FALSE;
  else
     return L7_TRUE;
}

/*********************************************************************
* @purpose  check validity of interface for a destination port 
*
* @param    intIfNum     @b{(input)} The internal interface number being validated
* 
* @returns  L7_TRUE   intIfNum can be configured as source port in some of the sessions
* @returns  L7_FALSE  intIfNum cannot be configured as source port in any of the sessions
*
* @notes   checks if the interface is a physical port 
*            
*
* @end
*********************************************************************/
L7_BOOL mirrorValidDestIntfCheck(L7_uint32 intIfNum)
{
  L7_BOOL status= L7_FALSE;

  /* Destination Port vaild Check */
  if (mirrorIsValidDestIntf(intIfNum, &status) != L7_SUCCESS 
      || status != L7_TRUE 
      )
    return L7_FALSE;
  else
    return L7_TRUE;
}

/*********************************************************************
* @purpose  get first valid interface that can be configured as source
*
* @param    intIfNum     @b{(output)} The internal interface number 
*                                     of first valid source interface
* 
* @returns  L7_SUCCESS  first valid source interface is found
* @returns  L7_FAILURE  no valid source interfaces can be found
*
* @notes    
* 
*
* @end
*********************************************************************/
L7_BOOL mirrorValidSrcIntfFirstGet(L7_uint32 *intIfNum)
{
  L7_uint32 intIfCurr=0, intIfPrev=0;
  L7_RC_t rc= L7_FAILURE;
  L7_BOOL bIntfFound = L7_FALSE; 
 
  rc=nimFirstValidIntfNumber(&intIfCurr);
  
  while((bIntfFound==L7_FALSE) && (rc == L7_SUCCESS))
  {
    /* Check If valid source port */
    if(mirrorValidSrcIntfCheck(intIfCurr) == L7_TRUE)
    {
      *intIfNum = intIfCurr;
      bIntfFound = L7_TRUE;
      break;
    }

    intIfPrev=intIfCurr;
    rc= nimNextValidIntfNumber(intIfPrev,&intIfCurr);
  }
  if (bIntfFound==L7_FALSE)
  {
    *intIfNum =0;
    return L7_FAILURE;
  }
  else
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  get next valid interface that can be configured as source 
*
* @param     intIfNum     @b{(input)}  Internal Interface Number of 
*                                      a valid source port
* @param    nextIfNum @b{(output)} Internal Interface Number of 
*                                     next valid source port,
* 
* @returns  L7_SUCCESS  next valid source interface is found
* @returns  L7_FAILURE  no valid source interfaces can be found
*
* @notes  nextIfNum value is 0 when no more source interfaces can 
*        be found   
* 
*
* @end
*********************************************************************/
L7_BOOL mirrorValidSrcIntfNextGet(L7_uint32 intIfNum, L7_uint32 *nextIfNum)
{
  L7_uint32 intIfCurr=0, intIfPrev=0;
  L7_RC_t rc= L7_FAILURE;
  L7_BOOL bIntfFound = L7_FALSE; 
  
  /* preset value in case of error*/
  *nextIfNum=0;

  rc=nimNextValidIntfNumber(intIfNum,&intIfCurr);
  
  while((bIntfFound==L7_FALSE) && (rc==L7_SUCCESS))
  {
    /* Check If valid source port */
    if(mirrorValidSrcIntfCheck(intIfCurr) == L7_TRUE)
    {
      *nextIfNum = intIfCurr;
      bIntfFound = L7_TRUE;
      break;
    }

    intIfPrev=intIfCurr;
    rc= nimNextValidIntfNumber(intIfPrev,&intIfCurr);
   
  }
  if (bIntfFound==L7_FALSE)
  {
    *nextIfNum =0;
    return L7_FAILURE;
  }
  else
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  get first valid interface that can be configured as destination
*
* @param    intIfNum     @b{(output)} The internal interface number 
*                                     of first valid destination interface
* 
* @returns  L7_SUCCESS  first valid source interface is found
* @returns  L7_FAILURE  no valid source interfaces can be found
*
* @notes    
* 
*
* @end
*********************************************************************/
L7_BOOL mirrorValidDestIntfFirstGet(L7_uint32 *intIfNum)
{
  L7_uint32 intIfCurr=0, intIfPrev=0;
  L7_RC_t rc= L7_FAILURE;
  L7_BOOL bIntfFound = L7_FALSE; 
  
  rc=nimFirstValidIntfNumber(&intIfCurr);
  
  while((bIntfFound==L7_FALSE) && (rc == L7_SUCCESS))
  {
   /* Destination Port vaild Check */
    if (mirrorValidDestIntfCheck(intIfCurr)==L7_TRUE)
    {
      *intIfNum = intIfCurr;
      bIntfFound = L7_TRUE;
      break;
    }

    intIfPrev=intIfCurr;
    rc= nimNextValidIntfNumber(intIfPrev,&intIfCurr);
  }

  if (bIntfFound==L7_FALSE)
  {
    *intIfNum =0;
    return L7_FAILURE;
  }
  else
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  get next valid interface that can be configured as destination
*
*@param     intIfNum     @b{(input)}  Internal Interface Number of 
*                                      a valid destination port
* @param    nextIfNum @b{(output)} Internal Interface Number of 
*                                     next valid destination port,
* 
* @returns  L7_SUCCESS  next valid destination interface is found
* @returns  L7_FAILURE  no valid destination interfaces can be found or
*                       any other error 
*
* @notes  nextIfNum value is 0 when no more destination interfaces can 
*        be found  
*
* @end
*********************************************************************/
L7_BOOL mirrorValidDestIntfNextGet(L7_uint32 intIfNum, L7_uint32 *nextIfNum)
{
  L7_uint32 intIfCurr=0, intIfPrev=0;
  L7_RC_t rc= L7_FAILURE;
  L7_BOOL bIntfFound = L7_FALSE; 
   
  /* preset value in case of error*/
  *nextIfNum=0;

  rc=nimNextValidIntfNumber(intIfNum,&intIfCurr);
  
  while((bIntfFound==L7_FALSE) && (rc==L7_SUCCESS))
  {
     
    /* Destination Port vaild Check */
    if (mirrorValidDestIntfCheck(intIfCurr)==L7_TRUE)
    {
      *nextIfNum = intIfCurr;
      bIntfFound = L7_TRUE;
      break;
    }

    intIfPrev=intIfCurr;
    rc= nimNextValidIntfNumber(intIfPrev,&intIfCurr);
  }
  if (bIntfFound==L7_FALSE)
  {
    *nextIfNum =0;
    return L7_FAILURE;
  }
  else
    return L7_SUCCESS;
}


/*********************************************************************
* @purpose ptin_mirror_reset - Reset mirror procedure 
* 
*
* @end
*********************************************************************/
void ptin_mirror_reset()
{
   usmDbSwPortMonitorSessionRemove(1,1);
}

/*********************************************************************
* @purpose ptin_mirror_dump - Reset mirror procedure 
* 
*
* @end
*********************************************************************/

void ptin_mirror_dump()
{
  L7_BOOL        mode;
  L7_RC_t        rc;
  L7_uint32      intIfNum_dst, numPorts, probeType;
  L7_INTF_MASK_t intIfMask; 
  static L7_uint32      listSrcPorts[L7_FILTER_MAX_INTF];
  int i;

  rc = mirrorModeGet(1,&mode);

  if( rc != L7_SUCCESS)
  {
    printf(" No mirror session configured \n\r");
    return;
  }

  rc = mirrorSourcePortsGet(1,&intIfMask);

  if( rc != L7_SUCCESS)
  {
    printf("No source ports configured \n\r");
  }
  else
  {
    usmDbSwPortMonitorSourcePortsGet(1, 1, &intIfMask);
    usmDbConvertMaskToList(&intIfMask, listSrcPorts, &numPorts);

    for (i=1; i<=numPorts;i++)
    {
      printf("Src Port %d = %d \n\r",i, listSrcPorts[i]);

      //Get direction
      mirrorSourcePortDirectionGet(1, listSrcPorts[i], &probeType);
      switch (probeType) 
      {
      case 0:
              printf("Direction None \n\r");
      break;
      case 1:
              printf("Direction TX/RX \n\r");
      break;
      case 2:
              printf("Direction RX \n\r");
      break;
      case 3:
              printf("Direction Tx \n\r");
      default:
              printf("Invalid Direction \n\r");
      break;
      }
    }
  }

  rc = mirrorDestPortGet(1,&intIfNum_dst);

  if( rc != L7_SUCCESS)
  {
    printf("No destination ports configured \n\r");
  }
  else
  {
    printf("Dst Port: %d \n\r", intIfNum_dst);
  }

}
