/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: broad_l2_dot1x.c
*
* Purpose: Implement Layer-2 Dot1x HAPI code
*
* Component: hapi
*
* Comments:
*
* Created by: colinw 4/16/07
*
*********************************************************************/

#include <string.h>

#include "commdefs.h"
#include "datatypes.h"
#include "osapi.h"
#include "sysapi.h"
#include "zlib.h"

#include "broad_common.h"
#include "broad_l2_std.h"
#include "broad_l2_vlan.h"
#include "broad_policy.h"
#include "sysbrds.h"
#include "bcmx/port.h"


#ifdef L7_QOS_PACKAGE
#include "broad_qos_common.h"
#include "broad_diffserv.h"
#endif
#include "bcm/field.h"

static void *hapiBroadVoiceVlanSemaphore = L7_NULL;


static L7_BOOL hapiVoiceVlanDebug1=0;

void hapiBroadVoiceVlanDebugSet(L7_BOOL val)
{
  hapiVoiceVlanDebug1=val;
}

HAPI_VOICE_VLAN_DEVICE_t voiceVlanDeviceDB[L7_VOICE_VLAN_MAX_DEVICE_COUNT];
static L7_uint32         voiceVlanDeviceCount=0;

/*********************************************************************
*
* @purpose Take semaphore to protect dot1x resources
*
* @param   
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVoiceVlanSemTake()
{
    L7_RC_t rc;

    rc = osapiSemaTake(hapiBroadVoiceVlanSemaphore, L7_WAIT_FOREVER);

    return rc;
}

/*********************************************************************
*
* @purpose Give semaphore to protect dot1x resources
*
* @param   
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVoiceVlanSemGive()
{
    L7_RC_t rc;

    rc = osapiSemaGive(hapiBroadVoiceVlanSemaphore);

    return rc;
}

/*********************************************************************
 *
 * @purpose Initializes voice vlan database
 *
 * @param *dapi_g          system information
 *
 * @returns L7_RC_t result
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadVoiceVlanDBInit()
{
  L7_uint32 i;
  hapiBroadVoiceVlanSemTake();
  for (i=0;i<L7_VOICE_VLAN_MAX_DEVICE_COUNT;i++)
  {
    voiceVlanDeviceDB[i].inUse = L7_FALSE;
    memset(voiceVlanDeviceDB[i].macAddr.addr,0,L7_MAC_ADDR_LEN);
    voiceVlanDeviceDB[i].vid =0;
    voiceVlanDeviceDB[i].usp.unit =0;
    voiceVlanDeviceDB[i].usp.slot =0;
    voiceVlanDeviceDB[i].usp.port =0;
  }
  hapiBroadVoiceVlanSemGive();
  return L7_SUCCESS;
}
/*********************************************************************
 *
 * @purpose Initializes voice vlan code
 *
 * @param *dapi_g          system information
 *
 * @returns L7_RC_t result
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadVoiceVlanInit(DAPI_t *dapi_g)
{
  hapiBroadVoiceVlanSemaphore = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
  if (hapiBroadVoiceVlanSemaphore == L7_NULL)
  {
    LOG_ERROR(0);
  }
  voiceVlanDeviceCount = 0;
  hapiBroadVoiceVlanDBInit();
  return L7_SUCCESS;
}



/*********************************************************************
*
* @purpose  Voice Vlan : Maintain information about voip device
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_VOICE_VLAN_DEVICE_ADD
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.voiceVlanDeviceAdd
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVoiceVlanDeviceAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  DAPI_INTF_MGMT_CMD_t     *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  BROAD_PORT_t             *hapiPortPtr;
  DAPI_PORT_t              *dapiPortPtr;
  L7_uint32                 i;
  L7_int32                  availableIndex = -1;

  if (dapiCmd->cmdData.dot1xClientAdd.getOrSet != DAPI_CMD_SET)
  {
    
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return L7_FAILURE;
  }

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ))
  {
    return( L7_FAILURE );
  }

  /* find a empty device slot*/
  hapiBroadVoiceVlanSemTake();
  for(i=0;i<L7_VOICE_VLAN_MAX_DEVICE_COUNT;i++)
  {
     if (memcmp(voiceVlanDeviceDB[i].macAddr.addr,dapiCmd->cmdData.voiceVlanDeviceAdd.macAddr.addr,sizeof(L7_enetMacAddr_t))==0)
    {
       hapiBroadVoiceVlanSemGive();
       if(hapiVoiceVlanDebug1)
         sysapiPrintf("DeviceAdd: Entry Exists!\n\r");
       return L7_FAILURE;
     }
     
  }

  for(i=0;i<L7_VOICE_VLAN_MAX_DEVICE_COUNT;i++)
  {
     if (voiceVlanDeviceDB[i].inUse == L7_FALSE)
      {
        availableIndex =i;
        break;
      }
      
  }

  if (availableIndex == -1)
  {
    hapiBroadVoiceVlanSemGive();
    return L7_FAILURE;
  }
  voiceVlanDeviceDB[availableIndex].inUse = L7_TRUE;
  memcpy(voiceVlanDeviceDB[availableIndex].macAddr.addr,dapiCmd->cmdData.voiceVlanDeviceAdd.macAddr.addr,sizeof(L7_enetMacAddr_t));
  voiceVlanDeviceDB[availableIndex].vid = dapiCmd->cmdData.voiceVlanDeviceAdd.vlanId;
  voiceVlanDeviceDB[availableIndex].usp.unit = usp->unit;
  voiceVlanDeviceDB[availableIndex].usp.slot = usp->slot;
  voiceVlanDeviceDB[availableIndex].usp.port = usp->port;
  hapiPortPtr->voiceVlanPort.deviceCount++;
  voiceVlanDeviceCount++;
  hapiBroadVoiceVlanSemGive();
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  VoiceVlan : Remove information about voip devices 
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_VOICE_VLAN_DEVICE_REMOVE
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.voiceVlanDeviceRemove
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVoiceVlanDeviceRemove(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                   result       = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t     *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  BROAD_PORT_t             *hapiPortPtr;
  DAPI_PORT_t              *dapiPortPtr;
  L7_uint32                 i;
  L7_int32                  foundIndex =-1;

  if (dapiCmd->cmdData.dot1xClientAdd.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ))
  {
    return( L7_FAILURE );
  }
  /* find a the device */
  hapiBroadVoiceVlanSemTake();
  for(i=0;i<L7_VOICE_VLAN_MAX_DEVICE_COUNT;i++)
  {
      if (voiceVlanDeviceDB[i].inUse == L7_TRUE &&
          memcmp(voiceVlanDeviceDB[i].macAddr.addr,dapiCmd->cmdData.voiceVlanDeviceRemove.macAddr.addr,sizeof(L7_enetMacAddr_t))==0)
      {
        foundIndex =i;
        break;
      }
      
  }

  if (foundIndex == -1)
  {
    hapiBroadVoiceVlanSemGive();
    if(hapiVoiceVlanDebug1)
            sysapiPrintf("DeviceRemove: Could not find entry!\n\r");
    return L7_FAILURE;
  }
  voiceVlanDeviceDB[foundIndex].inUse = L7_FALSE;
  memset(voiceVlanDeviceDB[foundIndex].macAddr.addr,0,sizeof(L7_enetMacAddr_t));
  voiceVlanDeviceDB[foundIndex].vid = 0;
  voiceVlanDeviceDB[foundIndex].usp.unit = 0;
  voiceVlanDeviceDB[foundIndex].usp.slot = 0;
  voiceVlanDeviceDB[foundIndex].usp.port = 0;
  hapiPortPtr->voiceVlanPort.deviceCount--;
  voiceVlanDeviceCount--;
  hapiBroadVoiceVlanSemGive();
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Initializes voice vlan code
 *
 *
 * @returns L7_RC_t result
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadVoiceVlanCosOverridePolicyCreate(BROAD_PORT_t *hapiPortPtr,DAPI_USP_t *usp)
{
  L7_RC_t                 result       = L7_SUCCESS;
  BROAD_POLICY_t cosqId;
  BROAD_POLICY_RULE_t     ruleId;
  L7_uchar8               exact_match[] = { FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                            FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  L7_RC_t rc= L7_FAILURE;
  L7_uint32 i;


 /* Create policy to map packets to default traffic class. */
  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PORT, &cosqId);
  /* find all devices on this port*/
  hapiBroadVoiceVlanSemTake();
  for(i=0;i<voiceVlanDeviceCount;i++)
  {
    if (voiceVlanDeviceDB[i].inUse == L7_TRUE)
    {
      if(voiceVlanDeviceDB[i].usp.unit == usp->unit &&
          voiceVlanDeviceDB[i].usp.slot == usp->slot &&
          voiceVlanDeviceDB[i].usp.port == usp->port)
      {
        hapiBroadPolicyRuleAdd(cosqId, &ruleId);
        hapiBroadPolicyRuleQualifierAdd(cosqId, ruleId, BROAD_FIELD_MACSA, voiceVlanDeviceDB[i].macAddr.addr, exact_match);
        hapiBroadPolicyRuleActionAdd(cosqId, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);
      }
    }
  }
  hapiBroadVoiceVlanSemGive();
  hapiBroadPolicyRuleAdd(cosqId, &ruleId);
  hapiBroadPolicyRuleActionAdd(cosqId, ruleId, BROAD_ACTION_SET_COSQ, 0, 0, 0);
  
  result = hapiBroadPolicyCommit(cosqId);
  if (L7_SUCCESS == result)
  {
   
     hapiBroadPolicyApplyToIface(cosqId, hapiPortPtr->bcmx_lport);
     hapiPortPtr->voiceVlanPort.voiceVlanPolicy = cosqId;
     rc =  L7_SUCCESS;
   }
    
    return rc;
}

/*********************************************************************
*
* @purpose VoiceVlan:To override the default Cos on a voice vlan port.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_VOICE_VLAN_COS_OVERRIDE
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.voiceVlanCoSOverride
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   We do not support specification of the destination ports
*          on Broadcom.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadVoiceVlanCosOverride(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  DAPI_INTF_MGMT_CMD_t    *dapiCmd      = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;
 
                                           
 
  
  if (dapiCmd->cmdData.voiceVlanCosOverride.getOrSet != DAPI_CMD_SET)
    return L7_FAILURE;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (L7_TRUE == dapiCmd->cmdData.voiceVlanCosOverride.enable)   /* create new policy */
  {
    if (hapiPortPtr->voiceVlanPort.voiceVlanPolicy != BROAD_POLICY_INVALID)
    {
      /* remove currently applied policy and reconstruct new policy*/
      hapiBroadPolicyRemoveFromIface(hapiPortPtr->voiceVlanPort.voiceVlanPolicy, hapiPortPtr->bcmx_lport);
      hapiBroadPolicyDelete(hapiPortPtr->voiceVlanPort.voiceVlanPolicy);
      hapiPortPtr->voiceVlanPort.voiceVlanPolicy = BROAD_POLICY_INVALID;
          
    }

    /*construct new policy*/
    if (hapiBroadVoiceVlanCosOverridePolicyCreate(hapiPortPtr,usp) != L7_SUCCESS)
    {
      /* failure in applying new policy to interface */
      hapiBroadPolicyDelete(hapiPortPtr->voiceVlanPort.voiceVlanPolicy);
      hapiPortPtr->voiceVlanPort.voiceVlanPolicy = BROAD_POLICY_INVALID;
    }
   
  }
  else
  {
    if(hapiPortPtr->voiceVlanPort.voiceVlanPolicy != BROAD_POLICY_INVALID)
    {
     
        hapiBroadPolicyRemoveFromIface(hapiPortPtr->voiceVlanPort.voiceVlanPolicy, hapiPortPtr->bcmx_lport);
        hapiBroadPolicyDelete(hapiPortPtr->voiceVlanPort.voiceVlanPolicy);
        hapiPortPtr->voiceVlanPort.voiceVlanPolicy = BROAD_POLICY_INVALID;
        if (hapiPortPtr->voiceVlanPort.deviceCount > 0)
        {
          /*reconstruct new policy and apply to the interface */
          if (hapiBroadVoiceVlanCosOverridePolicyCreate(hapiPortPtr,usp) != L7_SUCCESS)
          {
            /* failure in applying new policy to interface */
            hapiBroadPolicyDelete(hapiPortPtr->voiceVlanPort.voiceVlanPolicy);
            hapiPortPtr->voiceVlanPort.voiceVlanPolicy = BROAD_POLICY_INVALID;
          }
        }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose To Enable/Disable Auth state on a voice vlan port.
 *
 * @param   DAPI_USP_t *usp    - needs to be a valid usp
 * @param   DAPI_CMD_t  cmd    - DAPI_CMD_VOICE_VLAN_AUTH
 * @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
 * @param   DAPI_t     *dapi_g - the driver object
 *
 * @returns L7_RC_t result
 *
 * @notes   We do not support specification of the destination ports
 *          on Broadcom.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadVoiceVlanAuth(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 rc = L7_FAILURE;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd      = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;
  BROAD_SYSTEM_t          *hapiSystem;
  /*TBR*/
 /* soc_chip_family_t       board_family;*/

  /*hapiBroadGetSystemBoardFamily(&board_family);*/

  if (dapiCmd->cmdData.voiceVlanAuth.getOrSet != DAPI_CMD_SET)
  {
    return rc;
  }

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  if (L7_TRUE == dapiCmd->cmdData.voiceVlanAuth.enable)   /* add policies to port */
  {
    if (BROAD_POLICY_INVALID != hapiSystem->voiceCdpSysId)
    {
      rc = hapiBroadPolicyApplyToIface(hapiSystem->voiceCdpSysId, hapiPortPtr->bcmx_lport);
      if (L7_SUCCESS != rc)
      {
        if (hapiVoiceVlanDebug1)
        {
          sysapiPrintf("%s: CDP Policy Apply for voice VLAN policy failed!", __FUNCTION__);
        }
      }
       if (hapiVoiceVlanDebug1)
        {
          sysapiPrintf("%s: CDP Policy Apply for voice VLAN policy succeded!", __FUNCTION__);
        }
    }

    if (L7_SUCCESS != rc)
    {
      return rc;
    }

    if (BROAD_POLICY_INVALID != hapiSystem->voiceDhcpSysId)
    {
      rc = hapiBroadPolicyApplyToIface(hapiSystem->voiceDhcpSysId, hapiPortPtr->bcmx_lport);
      if (L7_SUCCESS != rc)
      {
        (void)hapiBroadPolicyRemoveFromIface(hapiSystem->voiceCdpSysId, hapiPortPtr->bcmx_lport);

        if (hapiVoiceVlanDebug1)
        {
          sysapiPrintf("%s: DHCP Policy Apply for voice VLAN policy failed!", __FUNCTION__);
        }
      }
      if (hapiVoiceVlanDebug1)
        {
          sysapiPrintf("%s: DHCP Policy Apply for voice VLAN policy succeded!", __FUNCTION__);
        }
    }

    if (rc == L7_SUCCESS)
    {
      hapiPortPtr->voiceVlanPort.voiceVlanUnauthMode = L7_ENABLE;
    }
  }
  else
  {
    if (BROAD_POLICY_INVALID != hapiSystem->voiceCdpSysId)
    {
      rc = hapiBroadPolicyRemoveFromIface(hapiSystem->voiceCdpSysId, hapiPortPtr->bcmx_lport);
      if (L7_SUCCESS != rc)
      {
        if (hapiVoiceVlanDebug1)
        {
          sysapiPrintf("%s: CDP Policy Remove for voice VLAN failed!", __FUNCTION__);
        }
      }
      if (hapiVoiceVlanDebug1)
        {
          sysapiPrintf("%s: CDP Policy Remove for voice VLAN succeded!", __FUNCTION__);
        }
    }

    if (BROAD_POLICY_INVALID != hapiSystem->voiceDhcpSysId)
    {
      rc = hapiBroadPolicyRemoveFromIface(hapiSystem->voiceDhcpSysId, hapiPortPtr->bcmx_lport);
      if (L7_SUCCESS != rc)
      {
        if (hapiVoiceVlanDebug1)
        {
          sysapiPrintf("%s: DHCP Policy Remove for voice VLAN failed!", __FUNCTION__);
        }
      }
       if (hapiVoiceVlanDebug1)
        {
          sysapiPrintf("%s: DHCP Policy Remove for voice VLAN succeded!", __FUNCTION__);
        }
    }

    hapiPortPtr->voiceVlanPort.voiceVlanUnauthMode = L7_DISABLE;
  }

  return rc;
}

