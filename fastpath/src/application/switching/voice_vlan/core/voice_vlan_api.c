/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename voice_vlan_api.c
 *
 * @purpose The API file for the Voice Vlan
 *
 * @component Voice Vlan
 *
 * @comments none
 *
 * @create  Jan 11, 2007
 *
 * @author  PKB
 * @end
 *
 **********************************************************************/
#include "commdefs.h"
#include "voice_vlan_api.h"
#include "voice_vlan.h"
#include "voice_vlan_debug.h"
#include "dot1q_api.h"

#include "dot1x_api.h"
#include "voicevlan_exports.h"
#include "dot1x_exports.h"

#ifdef L7_NSF_PACKAGE
#include "voice_vlan_ckpt.h"
#endif /* L7_NSF_PACKAGE */



extern L7_VLAN_MASK_t voiceVlanMask;
extern L7_ushort16 * voiceVlanIntfCount;

/*********************************************************************
* @purpose  Set voice vlan admin mode
*
* @param    mode @b{(input)} voice vlan mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanAdminModeSet (L7_uint32 adminMode)
{
  voiceVlanCfg_t    *vlanCfg;

  vlanCfg = voiceVlanGetCfg();

  if (vlanCfg == L7_NULLPTR || (adminMode != L7_ENABLE && adminMode != L7_DISABLE))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Input \n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  if (vlanCfg->voiceVlanAdminMode == adminMode)
  {
    return L7_SUCCESS;
  }

  if (vlanCfg->voiceVlanAdminMode != adminMode)
  {
    vlanCfg->voiceVlanAdminMode = adminMode;
    vlanCfg->cfgHdr.dataChanged = L7_TRUE;
  }

  voiceVlanAuthAdminModeUpdate(adminMode);

  /* Call the enable function */
  voiceVlanApplyConfigData();

#ifdef L7_NSF_PACKAGE
  if (adminMode == L7_DISABLE)
  {
    voiceVlanCheckpointFlushAllCallback();
  }
#endif /* L7_NSF_PACKAGE */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get administrative mode setting for voice vlan
*
* @param    *adminMode @b{(output)} voice vlan admin mode
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanAdminModeGet(L7_uint32 *adminMode)
{
  voiceVlanCfg_t    *vlanCfg= voiceVlanGetCfg();

  if (vlanCfg == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  *adminMode = vlanCfg->voiceVlanAdminMode;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get administrative mode setting for voice vlan
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    *adminMode @b{(output)} voice vlan admin mode
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortAdminModeGet(L7_uint32 intIfNum,VOICE_VLAN_CONFIG_TYPE_t *mode)
{
  voiceVlanPortCfg_t *pCfg;

  if (voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_ISDP_COMPONENT_ID,"Invalid Params");
    return L7_FAILURE;
  }
  if (pCfg->adminMode == L7_ENABLE)
  {
    if (pCfg->voiceVlanId != FD_VOICE_VLAN_VLAN_ID)
      *mode = VOICE_VLAN_CONFIG_VLAN_ID;
    else if (pCfg->portPriority != FD_VOICE_VLAN_PRIORITY)
      *mode =  VOICE_VLAN_CONFIG_DOT1P;
    else if (pCfg->noneMode != FD_VOICE_VLAN_NONE_MODE)
      *mode =  VOICE_VLAN_CONFIG_NONE;
    else if (pCfg->untaggingEnabled != FD_VOICE_VLAN_TAGGING)
      *mode = VOICE_VLAN_CONFIG_UNTAGGED;
  } else
  {
    *mode = VOICE_VLAN_CONFIG_DISABLE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Reset the Voice Vlan adminmode for the port.
*
* @param    intIfNum     @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t voiceVlanPortAdminModeReset(L7_uint32 intIfNum)
{
  voiceVlanPortCfg_t *pCfg;
  voiceVlanCfg_t    *vlanCfg= voiceVlanGetCfg();


  if (vlanCfg == L7_NULLPTR ||
      voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  if (pCfg->adminMode != L7_ENABLE)
  {
    return L7_SUCCESS;
  }
  pCfg->adminMode = FD_VOICE_VLAN_ADMIN_MODE;
  if (pCfg->voiceVlanId != FD_VOICE_VLAN_VLAN_ID)
  {
    voiceVlanIntfCount[pCfg->voiceVlanId]--;
    if (voiceVlanIntfCount[pCfg->voiceVlanId] == 0)
    {
      L7_VLAN_CLRMASKBIT(voiceVlanMask,pCfg->voiceVlanId);
    }
  }

  pCfg->voiceVlanId = FD_VOICE_VLAN_VLAN_ID;
  pCfg->portPriority = FD_VOICE_VLAN_PRIORITY;
  pCfg->untaggingEnabled = FD_VOICE_VLAN_TAGGING;
  pCfg->noneMode = FD_VOICE_VLAN_NONE_MODE;
  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s: Calling voiceVlanPortStatusSet for interface[%d] , status[%d]",
                   __FUNCTION__,intIfNum,L7_DISABLE);
  voiceVlanPortStatusSet(intIfNum,L7_DISABLE);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Voice Vlan Id for the port.
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    voiceVlanId  @b{(input)} voice vlan Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVlanIdSet(L7_uint32 intIfNum,L7_uint32 voiceVlanId)
{
  voiceVlanPortCfg_t *pCfg;
  voiceVlanCfg_t    *vlanCfg= voiceVlanGetCfg();
  L7_uint32 currentVoiceVlanId;

  if (vlanCfg == L7_NULLPTR ||
      voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  if (pCfg->voiceVlanId == voiceVlanId)
  {
    return L7_SUCCESS;
  }

  if (voiceVlanId > FD_VOICE_VLAN_VLAN_ID && voiceVlanId <= L7_PLATFORM_MAX_VLAN_ID)
  {
    voiceVlanPortAdminModeReset(intIfNum);
    pCfg->adminMode = L7_ENABLE;
     VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s: Set configured admin mode for interface[%d] to [%d]",
                   __FUNCTION__,intIfNum,L7_ENABLE);
  } else if (voiceVlanId == FD_VOICE_VLAN_VLAN_ID)
  {
    voiceVlanPortAdminModeReset(intIfNum);
  } else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  currentVoiceVlanId = pCfg->voiceVlanId;

  if (currentVoiceVlanId == FD_VOICE_VLAN_VLAN_ID && voiceVlanId != FD_VOICE_VLAN_VLAN_ID)
  {
    /* Case of enabling a voice vlan to an interfce */
    L7_VLAN_SETMASKBIT(voiceVlanMask, voiceVlanId);
    voiceVlanIntfCount[voiceVlanId]++;
  }
  else if (currentVoiceVlanId != FD_VOICE_VLAN_VLAN_ID && voiceVlanId == FD_VOICE_VLAN_VLAN_ID)
  {
    /* Case of disabling voice vlan on an interface */
    voiceVlanIntfCount[currentVoiceVlanId]--;
    if (voiceVlanIntfCount[currentVoiceVlanId] == 0)
    {
      L7_VLAN_CLRMASKBIT(voiceVlanMask,currentVoiceVlanId);
    }
  }
  else if (currentVoiceVlanId != FD_VOICE_VLAN_VLAN_ID && voiceVlanId != FD_VOICE_VLAN_VLAN_ID)
  {
    /* Case of changing a voice vlan association for this interface */
    voiceVlanIntfCount[currentVoiceVlanId]--;
    if (voiceVlanIntfCount[currentVoiceVlanId] == 0)
    {
      L7_VLAN_CLRMASKBIT(voiceVlanMask,currentVoiceVlanId);
    }

    L7_VLAN_SETMASKBIT(voiceVlanMask, voiceVlanId);
    voiceVlanIntfCount[voiceVlanId]++;
  }

  pCfg->voiceVlanId = voiceVlanId;
  vlanCfg->cfgHdr.dataChanged = L7_TRUE;

  if (L7_DISABLE == pCfg->authState)
  {
    voiceVlanPortAuthSet(intIfNum, pCfg->authState);
  }

  /* Apply the config */
   VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s: Calling voiceVlanPortStatusSet for interface[%d] , status[%d]",
                   __FUNCTION__,intIfNum,pCfg->adminMode);
  voiceVlanPortStatusSet(intIfNum,pCfg->adminMode);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Get the Voice Vlan Id for the port.
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    *voiceVlanId  @b{(intput)} voice vlan Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVlanIdGet(L7_uint32 intIfNum,L7_uint32 *voiceVlanId)
{
  voiceVlanPortCfg_t *pCfg;

  if (voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  *voiceVlanId = pCfg->voiceVlanId;
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose Set the Voice Vlan dot1p priority for the port.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    priority  @b{(intput)} priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVlanPrioritySet(L7_uint32 intIfNum,L7_uint32 priority)
{
  voiceVlanPortCfg_t *pCfg;
  voiceVlanCfg_t    *vlanCfg= voiceVlanGetCfg();

  if (vlanCfg == L7_NULLPTR ||
      voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  if (pCfg->portPriority == priority)
  {
    return L7_SUCCESS;
  } else if (priority < VOICE_VLAN_MIN_DOT1P_PRIORITY ||
             priority > VOICE_VLAN_MAX_DOT1P_PRIORITY)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  } else
  {
    voiceVlanPortAdminModeReset(intIfNum);
    pCfg->adminMode = L7_ENABLE;
  }

  pCfg->portPriority = priority;
  vlanCfg->cfgHdr.dataChanged = L7_TRUE;

  /* Apply the config */
  voiceVlanPortStatusSet(intIfNum,pCfg->adminMode);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Get the Voice Vlan dot1p priority for the port.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    priority  @b{(input)} priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVlanPriorityGet(L7_uint32 intIfNum,L7_uint32 *priority)
{
  voiceVlanPortCfg_t *pCfg;

  if (voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  *priority = pCfg->portPriority;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Set the Voice Vlan configuration as none.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    mode      @b{(input)} voice vlan none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortNoneSet(L7_uint32 intIfNum,L7_BOOL mode)
{
  voiceVlanPortCfg_t *pCfg;
  voiceVlanCfg_t    *vlanCfg= voiceVlanGetCfg();

  if (vlanCfg == L7_NULLPTR ||
      voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  if (mode == pCfg->noneMode)
  {
    return L7_SUCCESS;
  }
  voiceVlanPortAdminModeReset(intIfNum);

  pCfg->adminMode = (mode==L7_TRUE)?L7_ENABLE:L7_DISABLE;
  pCfg->noneMode = mode;
  pCfg->untaggingEnabled = L7_TRUE;
  vlanCfg->cfgHdr.dataChanged = L7_TRUE;

  /* apply the config */
  voiceVlanPortStatusSet(intIfNum,pCfg->adminMode);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Get the Voice Vlan configuration as none.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mode @b{(output)} voice vlan none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortNoneGet(L7_uint32 intIfNum,L7_BOOL *mode)
{
  voiceVlanPortCfg_t *pCfg;

  if (voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_ISDP_COMPONENT_ID,"Invalid Params");
    return L7_FAILURE;
  }
  *mode = pCfg->noneMode;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Set voice traffic to be untagged on the port.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    mode      @b{(input)} voice vlan untag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortUntagSet(L7_uint32 intIfNum,L7_BOOL mode)
{
  voiceVlanPortCfg_t *pCfg;
  voiceVlanCfg_t    *vlanCfg= voiceVlanGetCfg();

  if (vlanCfg == L7_NULLPTR ||
      voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  if (mode == pCfg->untaggingEnabled)
  {
    return L7_SUCCESS;
  }

  voiceVlanPortAdminModeReset(intIfNum);

  pCfg->untaggingEnabled = mode;
  pCfg->adminMode = (mode==L7_TRUE)?L7_ENABLE:L7_DISABLE;
  vlanCfg->cfgHdr.dataChanged = L7_TRUE;

  /* apply the config */
  voiceVlanPortStatusSet(intIfNum,pCfg->adminMode);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Get the Voice traffic status.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mode @b{(output)} voice vlan untag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortUntagGet(L7_uint32 intIfNum,L7_BOOL *mode)
{
  voiceVlanPortCfg_t *pCfg;

  if (voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  *mode = pCfg->untaggingEnabled;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Set the default cos override for the data traffic.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    mode      @b{(input)} voice vlan cos override
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortCosOverrideSet(L7_uint32 intIfNum,L7_BOOL mode)
{
  voiceVlanPortCfg_t *pCfg;
  voiceVlanCfg_t     *vlanCfg= voiceVlanGetCfg();
  L7_uint32          status=L7_DISABLE;
  L7_uint32          refCount=0;

  if (vlanCfg == L7_NULLPTR ||
      voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  if (pCfg->defaultCosOverride == mode)
  {
    return L7_SUCCESS;
  }

  /* check the max count */
  if (voiceVlanPortOverrideCount(intIfNum,mode)!=L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  pCfg->defaultCosOverride = mode;
  vlanCfg->cfgHdr.dataChanged = L7_TRUE;


  voiceVlanPortStatusGet(intIfNum,&status);
  voiceVlanPortReferenceCountGet(intIfNum,&refCount);

  if ((status == L7_ENABLE)&&(refCount >0))
  {
    /* memset(voipMac,0,sizeof(voipMac));
     voiceVlanPortVoipMacGet(intIfNum,voipMac);*/

    /*  call the DTL call here */
    dtlVoiceVlanCosOverrideSet(intIfNum,mode);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Get the default cos override for the data traffic.
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mode     @b{(input)} voice vlan cos override
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortCosOverrideGet(L7_uint32 intIfNum,L7_BOOL *mode)
{
  voiceVlanPortCfg_t *pCfg;

  if (voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG,L7_ISDP_COMPONENT_ID,"Invalid Params");
    return L7_FAILURE;
  }

  /*printf("\r\n voiceVlanPortCosOverrideGet() ");*/
  *mode = pCfg->defaultCosOverride;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Get the Port status
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    *mode     @b{(input)} voice vlan status
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanApiPortStatusGet(L7_uint32 intIfNum,L7_uint32 *mode)
{
  voiceVlanPortCfg_t *pCfg;

  if (voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  voiceVlanPortStatusGet(intIfNum,mode);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set the Voice Vlan AdminMode for the port
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    type         @b{(input)} type of parameter
* @param    val          @b{(input)} value for the parameter
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortAdminModeSet(L7_uint32 intIfNum,
                                  VOICE_VLAN_CONFIG_TYPE_t  type,
                                  L7_uint32 val)
{
  L7_RC_t   rc=L7_FAILURE;
  voiceVlanPortCfg_t *pCfg;
  voiceVlanCfg_t     *vlanCfg= voiceVlanGetCfg();

  if (voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return rc;
  }

  switch (type)
  {
  case VOICE_VLAN_CONFIG_DISABLE:
    voiceVlanPortAdminModeReset(intIfNum);
    /* Apply the config */
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s():pCfgAuthState[%d],Enabling AuthState IFace[%d] on VOICE_VLAN_CONFIG_DISABLE\r\n",
                     __FUNCTION__, pCfg->authState, intIfNum);
    voiceVlanPortAuthSet(intIfNum, FD_VOICE_VLAN_AUTH_STATE);
    voiceVlanPortStatusSet(intIfNum, pCfg->adminMode);
    rc = L7_SUCCESS;
    vlanCfg->cfgHdr.dataChanged = L7_TRUE;
    break;

  case VOICE_VLAN_CONFIG_VLAN_ID:
    rc =voiceVlanPortVlanIdSet(intIfNum,val);
    break;

  case VOICE_VLAN_CONFIG_DOT1P:
    rc = voiceVlanPortVlanPrioritySet(intIfNum,val);
    break;

  case VOICE_VLAN_CONFIG_NONE:
    rc =voiceVlanPortNoneSet(intIfNum,L7_TRUE);
    break;

  case VOICE_VLAN_CONFIG_UNTAGGED:
    rc = voiceVlanPortUntagSet(intIfNum,L7_TRUE);
    break;
  default:
    rc = L7_FAILURE;
    break;
  }
  return rc;
}

/*********************************************************************
* @purpose Set the Port DSCP val
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    val       @b{(input)} dscp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE - if interface number is invalid
*           L7_NO_VLAUE - if Dscp value is out of range
*
* @comments Right now there is no implementation for DSCP
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDscpSet(L7_uint32 intIfNum,L7_uint32 val)
{
  L7_RC_t   rc=L7_FAILURE;
  voiceVlanPortCfg_t *pCfg;
  voiceVlanCfg_t     *vlanCfg= voiceVlanGetCfg();

  if (voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return rc;
  }
  /* add check for dscp range */
  if ((val < L7_VOICE_VLAN_DSCP_MIN)||(val > L7_VOICE_VLAN_DSCP_MAX))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Range for dscp val (%d). Should be between %d - %d \n",
            __FUNCTION__,__LINE__,val,L7_VOICE_VLAN_DSCP_MIN,L7_VOICE_VLAN_DSCP_MAX);
    return L7_NO_VALUE;
  }

  pCfg->dscp = val;
  vlanCfg->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Get the Port DSCP val
*
* @param    intIfNum  @b{(input)} internal interface number
* @param    val       @b{(input)} dscp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Right now there is no implementation for DSCP
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDscpGet(L7_uint32 intIfNum,L7_uint32 *val)
{
  L7_RC_t   rc=L7_FAILURE;
  voiceVlanPortCfg_t *pCfg;

  if (voiceVlanIsValidIntf(intIfNum) != L7_TRUE ||
      voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \n",__FUNCTION__,__LINE__);
    return rc;
  }

  *val = pCfg->dscp ; /* for expedited forwarding */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the Voice Vlan Auth State for the port
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    state        @b{(input)} auth value for the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortAuthStateSet(L7_uint32 intIfNum,
                                  L7_BOOL state)
{
  voiceVlanPortCfg_t      *pCfg;
  voiceVlanCfg_t          *vlanCfg = voiceVlanGetCfg();
  L7_DOT1X_PORT_CONTROL_t  portControl = 1;
  L7_uint32                adminMode = 0;
  L7_RC_t                  rc = L7_FAILURE;

  if ((vlanCfg == L7_NULLPTR) ||
      (voiceVlanIsValidIntf(intIfNum) != L7_TRUE) ||
      (voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \r\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  if ((state != L7_ENABLE) &&
      (state != L7_DISABLE))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \r\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

   if (state == pCfg->authState)
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Auth State already %s\r\n", __FUNCTION__, ((state == L7_ENABLE) ? "ENABLED" : "DISABLED"));
    return L7_SUCCESS;
  }

  pCfg->authState = state;
  vlanCfg->cfgHdr.dataChanged = L7_TRUE;
  rc = L7_SUCCESS;

  if (state == L7_DISABLE)
  {
    /* Can be disabled only if Dot1x Port-control Mode is MAC-Based. */
    dot1xAdminModeGet(&adminMode);

    dot1xPortControlModeGet(intIfNum, &portControl);

    if ((adminMode == L7_ENABLE) && (portControl == L7_DOT1X_PORT_AUTO_MAC_BASED))
    {
      rc = voiceVlanPortAuthSet(intIfNum, state);
      voiceVlanPortStatusSet(intIfNum,pCfg->adminMode);
    }

  }
  else
  {
    rc = voiceVlanPortAuthSet(intIfNum, state);
  }
  return rc;
}

/*********************************************************************
* @purpose  Get the Voice Vlan Auth State for the port
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    state        @b{(input)} reference to auth value for the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortAuthStateGet(L7_uint32 intIfNum,
                                  L7_BOOL *state)
{
  voiceVlanPortCfg_t *pCfg;

  if ((voiceVlanIsValidIntf(intIfNum) != L7_TRUE) ||
      (voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE))
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Invalid Params \r\n", __FUNCTION__);
    return L7_FAILURE;
  }

  *state = pCfg->authState;
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set the Voice Vlan Auth State for a client on a port
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    mac          @b{(input)} mac address of the client
* @param    state        @b{(input)} auth value for the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function clears the mac address
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortClientAuthSet(L7_uint32 intIfNum,
                                   L7_uchar8 *mac,
                                   L7_BOOL   state)
{
  L7_RC_t   rc = L7_FAILURE;
  voiceVlanDeviceInfo_t *node;

  if (voiceVlanIsValidIntf(intIfNum) != L7_TRUE)
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Invalid Params intfNum[%d]\r\n", __FUNCTION__, intIfNum);
    return rc;
  }

  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,
                   "%s(): Params intfNum[%d], mac %02X:%02X:%02X:%02X:%02X:%02X, state = %d\r\n",
                   __FUNCTION__, intIfNum,
                   mac[0],
                   mac[1],
                   mac[2],
                   mac[3],
                   mac[4],
                   mac[5],
                   state);

  if (mac != L7_NULLPTR)
  {
    if ((node = voiceVlanDeviceInfoGet(mac))!= L7_NULLPTR)
    {
      /* No processing for enable*/
      if (state == L7_FALSE)
      {
        rc = voiceVlanPortVoipDeviceRemove(intIfNum,mac);
        if (rc != L7_SUCCESS)
        {
          VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH,"%s: Could not remove voice device info for Iface[%d] and mac %02X:%02X:%02X:%02X:%02X:%02X, state = %d\r\n",
                           __FUNCTION__,intIfNum,
                           mac[0],
                           mac[1],
                           mac[2],
                           mac[3],
                           mac[4],
                           mac[5],
                           state);
        }
        VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s: Removed voice device info for Iface[%d] and mac %02X:%02X:%02X:%02X:%02X:%02X, state = %d\r\n",
                         __FUNCTION__,intIfNum,
                         mac[0],
                         mac[1],
                         mac[2],
                         mac[3],
                         mac[4],
                         mac[5],
                         state);
      }
    }
  }


  return rc;
}

/*********************************************************************
* @purpose  So dot1x can inform voice VLAN if the port control mode
*           has changed.
*
* @param    intIfNum     @b{(input)} internal interface number
* @param    mode         @b{(input)} the port control mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sets the operational state of unauthorized
*           voice VLAN.
*
* @notes    This function will only be called if dot1x is enabled.
*
* @end
*********************************************************************/
L7_RC_t voiceVlanDot1xPortControlCallback(L7_uint32 intIfNum,
                                          L7_DOT1X_PORT_CONTROL_t mode)
{
  L7_uint32 state;
  L7_RC_t   rc;

  rc = voiceVlanPortAuthStateGet(intIfNum, &state);
  if (L7_SUCCESS != rc)
  {
    return rc;
  }

  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s(): mode = %d, state = %d\r\n", __FUNCTION__, mode, state);

  if (L7_DOT1X_PORT_AUTO_MAC_BASED == mode)
  {
    if (L7_DISABLE == state)
    {
      rc = voiceVlanPortAuthSet(intIfNum, state);
    }

    if (L7_SUCCESS == rc)
    {
      rc = voiceVlanPortStatusSet(intIfNum, L7_ENABLE);
    }
  } else                          /* Authorization required for all other modes */
  {
    rc = voiceVlanPortAuthSet(intIfNum, L7_ENABLE);
  }

  return rc;
}

/*********************************************************************
* @purpose  So dot1x can inform voice VLAN if the admin mode has changed.
*
* @param    mode         @b{(input)} the port control mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This function sets the administrative state of unauthorized
*           voice VLAN.
*
* @end
*********************************************************************/
L7_RC_t voiceVlanDot1xAdminModeCallback(L7_uint32 mode)
{
  L7_RC_t rc;
  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"\r\n%s: Calling voiceVlanAuthAdminModeUpdate for mode %d",__FUNCTION__,mode);
  rc = voiceVlanAuthAdminModeUpdate(mode);

  if (rc==L7_SUCCESS)
  {
     rc = voiceVlanApplyConfigData();
  }

  return rc;
}

/*********************************************************************
* @purpose  Get the first voice vlan device on the given interface.
*
* @param    intIfNum      @b{(input)} the interface
* @param    mac_addr      @b{(output)} the mac address of the first
*                                      VOIP device detected on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDeviceInfoFirstGet(L7_uint32 intIfNum,L7_uchar8 *mac_addr)
{
  voiceVlanDeviceInfo_t * node;
  voiceVlanPortCfg_t      *pCfg;
  voiceVlanCfg_t          *vlanCfg = voiceVlanGetCfg();
  L7_RC_t                 rc = L7_FAILURE;
  L7_uint32               mode;
  L7_uint32               refCount;

  /*check input*/
  if ((vlanCfg == L7_NULLPTR) ||
      (voiceVlanIsValidIntf(intIfNum) != L7_TRUE) ||
      (voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE))
  {
     VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s:%d - Invalid Params \r\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  rc = voiceVlanPortStatusGet(intIfNum,&mode);
  if ((pCfg->adminMode != L7_ENABLE) ||
      (rc != L7_SUCCESS) ||
      (mode == L7_DISABLE))
  {
     VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s:%d - Voice vlan admin mode not enabled on Iface[%d]",__FUNCTION__,__LINE__,intIfNum);
    return L7_FAILURE;
  }

  rc = voiceVlanPortReferenceCountGet(intIfNum,&refCount);
  if ((rc != L7_SUCCESS) ||
       (refCount == 0))
  {
     VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s:%d - No voice devices identified on the Iface[%d]",__FUNCTION__,__LINE__,intIfNum);
    return L7_FAILURE;
  }
  if (mac_addr == L7_NULL)
  {
     VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s:%d - Invalid Params \r\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  /*
  get the first voice vlan device
  */

  node = voiceVlanDeviceInfoFirstGet(intIfNum);
  if (node == L7_NULLPTR)
  {
     memset(mac_addr,0,L7_MAC_ADDR_LEN);
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s:%d - Could not find the first voice vlan device identified on the Iface[%d]",__FUNCTION__,__LINE__,intIfNum);
    return L7_FAILURE;
  }
  else
  {
     VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG," 1 %s:%d - Could not find the first voice vlan device identified on the Iface[%d]",__FUNCTION__,__LINE__,intIfNum);
    memset(mac_addr,0,L7_MAC_ADDR_LEN);
    memcpy(mac_addr,node->voipMac,L7_MAC_ADDR_LEN);
    rc = L7_SUCCESS;
  }
  return rc;
}

/*********************************************************************
* @purpose  Get the next voice vlan device on the given interface.
*
* @param    intIfNum      @b{(input)} the interface
* @param    mac_addr      @b{(input/output)} the mac address of the first
*                                      VOIP device detected on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDeviceInfoNextGet(L7_uint32 intIfNum,L7_uchar8 *mac_addr)
{
  voiceVlanDeviceInfo_t   *node=L7_NULLPTR;
  voiceVlanPortCfg_t      *pCfg;
  voiceVlanCfg_t          *vlanCfg = voiceVlanGetCfg();
  L7_RC_t                 rc = L7_FAILURE;
  L7_uint32               mode;
  L7_uint32               refCount;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  /*check input*/
  if ((vlanCfg == L7_NULLPTR) ||
      (voiceVlanIsValidIntf(intIfNum) != L7_TRUE) ||
      (voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \r\n", __FUNCTION__, __LINE__);
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s:%d - Invalid Params \r\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  rc = voiceVlanPortStatusGet(intIfNum,&mode);
  if ((pCfg->adminMode != L7_ENABLE) ||
      (rc != L7_SUCCESS) ||
      (mode == L7_DISABLE))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Voice vlan admin mode not enabled on intf[%d, %s]",__FUNCTION__,__LINE__,intIfNum, ifName);
     VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s:%d - Voice vlan admin mode not enabled on intf[%d, %s]",__FUNCTION__,__LINE__,intIfNum, ifName);
    return L7_FAILURE;
  }

  rc = voiceVlanPortReferenceCountGet(intIfNum,&refCount);
  if ((rc != L7_SUCCESS) ||
       (refCount <= 1))
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - No more than one voice devices identified on the intf[%d, %s]",__FUNCTION__,__LINE__,intIfNum, ifName);
     VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s:%d - No more than one voice devices identified on the intf[%d, %s]",
                      __FUNCTION__,__LINE__,intIfNum, ifName);
    return L7_FAILURE;
  }
  if (mac_addr == L7_NULL)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
            "%s:%d - Invalid Params \r\n", __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  /*
  get the first voice vlan device
  */

  node = voiceVlanDeviceInfoGetNextNode(intIfNum,mac_addr);
  if (node == L7_NULLPTR)
  {
     memset(mac_addr,0,L7_MAC_ADDR_LEN);
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_VOICE_VLAN_COMPONENT_ID,
             "%s:%d - Could not find anymore voice vlan devices on the intf[%d, %s]",__FUNCTION__,__LINE__,intIfNum, ifName);
     VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s:%d - Could not find anymore voice vlan devices on the intf[%d, %s]",
                      __FUNCTION__,__LINE__,intIfNum, ifName);
    return L7_FAILURE;
  }
  else
  {
    memset(mac_addr,0,L7_MAC_ADDR_LEN);
    memcpy(mac_addr,node->voipMac,L7_MAC_ADDR_LEN);
    rc = L7_SUCCESS;
  }
  return rc;
}

/*********************************************************************
* @purpose  Get the voice vlan for a device on the given interface.
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    mac_addr      @b{(input)} the mac address of the VoIP
*                                     device detected on the interface
* @param    voiceVlan     @b{(output)} the voice VLAN for the device,
*                                      if the RC == L7_SUCCESS
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDeviceVlanGet(L7_uint32 intIfNum, L7_uchar8 *mac_addr, L7_short16 *voiceVlan)
{
  voiceVlanDeviceInfo_t *node = voiceVlanDeviceInfoGet(mac_addr);

  if ((L7_NULLPTR != node) &&
      (0 != node->vlanId))
  {
    *voiceVlan = node->vlanId;
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s - Returned VLAN ID %d \r\n", __FUNCTION__, *voiceVlan);
    return L7_SUCCESS;
  }

  *voiceVlan = 0;
  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s - No VLAN ID\r\n", __FUNCTION__);
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Add a voice device
*
* @param    intIfNum      @b{(input)} internal interface number
* @param    compId        @b{(input)} the component ID of the requestor
* @param    mac_addr      @b{(input)} the mac address of the VoIP
*                                     device detected on the interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDeviceVlanAdd(L7_uint32 intIfNum, L7_COMPONENT_IDS_t compId, L7_uchar8 *mac_addr)
{
  voiceVlanPortCfg_t *pCfg;
  L7_RC_t             rc = L7_FAILURE;

  if (L7_TRUE != voiceVlanIsValidIntf(intIfNum))
  {
    return rc;
  }

  if (L7_TRUE != voiceVlanIntfIsConfigurable(intIfNum, &pCfg))
  {
    return rc;
  }

  switch (compId)
  {
    case L7_ISDP_COMPONENT_ID:
      rc = voiceVlanPortVoipDeviceAdd(intIfNum, mac_addr, L7_TRUE, VOICE_VLAN_SOURCE_CDP);
      break;

    default:
      /* pass through with default failure rc */
      break;
  }

  return rc;
}
