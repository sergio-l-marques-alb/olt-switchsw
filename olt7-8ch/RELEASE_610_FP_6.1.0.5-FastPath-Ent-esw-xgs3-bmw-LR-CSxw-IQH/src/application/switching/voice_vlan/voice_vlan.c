/**********************************************************************
*
* @filename  voice_vlan.c
*
* @purpose   voice vlan
*
* @component voice vlan
*
* @comments  none
*
* @create
*
* @author
*
* @end
**********************************************************************/

/*to accesss LLDP MAC address in voiceVlanAuthLLDPduRcvCallback*/
#define L7_MAC_LLDP_PDU

#include "dot1q_api.h"
#include "voice_vlan.h"
#include "voice_vlan_cnfgr.h"
#include "voice_vlan_api.h"
#include "dtlapi.h"

#include "nimapi.h"
#include "osapi_support.h"
#include "dot1x_api.h"
#include "sysnet_api_ipv4.h"
#include "l7_common.h"
#include "l7utils_api.h"
#include "fdb_api.h"

extern voiceVlanPortCnfgrState_t voiceVlanPortCnfgrState;
extern voiceVlanCfg_t   *voiceVlanCfg;

/*FDB functions*/
extern void fdbInsert(char *mac, L7_uint32 intIfNum, L7_uint32 vlanId, L7_ushort16 entryType);
extern void fdbDelete(char *mac, L7_uint32 vlanId);

L7_uint32  *voiceVlanMapTbl = L7_NULLPTR;
voiceVlanInfo_t  *voiceVlanInfo = L7_NULLPTR;
voiceVlanTraceLevel_t voiceVlanTrace  = VOICE_VLAN_TRACE_DISABLED;
L7_VLAN_MASK_t voiceVlanMask;
L7_ushort16 *voiceVlanIntfCount;

static const L7_uchar8 zeroMac[L7_MAC_ADDR_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static L7_RC_t voiceVlanPacketDump(L7_uchar8 *data, L7_uint32 len);
static L7_RC_t voiceVlanDhcpPacketHandle(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo);

static L7_enetMacAddr_t voiceVlanCdpDstMacAddr = {{0x01, 0x00, 0x0C, 0xCC, 0xCC, 0xCC}};

/*********************************************************************
* @purpose  To return the index for the voice vlan operational portdata
*
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns  index  A non-zero positive number if success else zero
*
* @comments
*
* @end
*********************************************************************/
static
L7_uint32 voiceVlanGetMapTblIndex(L7_uint32 intIfNum)
{
  /* Check boundary conditions */
  if(intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return 0;

  return voiceVlanMapTbl[intIfNum];
}

/*********************************************************************
* @purpose  To set the Acquire list for Voice Vlan
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    intfEvent  @b{(input)} NIM event
* @param    flag       @b{(input)} Set or reset
*
* @returns  None
*
* @comments
*
* @end
*********************************************************************/
void voiceVlanSetAcquireMask(L7_uint32 intIfNum,L7_uint32 intfEvent,L7_BOOL flag)
{
  L7_uint32 operIndex;

  if((operIndex = voiceVlanGetMapTblIndex(intIfNum))== 0)
  {
    return;
  }

  if(intfEvent == L7_LAG_ACQUIRE || intfEvent == L7_LAG_RELEASE)
  {
    if(flag == L7_TRUE)
      COMPONENT_ACQ_SETMASKBIT(voiceVlanInfo->portData[operIndex].acquiredList,L7_DOT3AD_COMPONENT_ID);
    else
      COMPONENT_ACQ_CLRMASKBIT(voiceVlanInfo->portData[operIndex].acquiredList,L7_DOT3AD_COMPONENT_ID);
  }
  else
  {
    if(flag == L7_TRUE)
      COMPONENT_ACQ_SETMASKBIT(voiceVlanInfo->portData[operIndex].acquiredList,L7_PORT_MIRROR_COMPONENT_ID);
    else
      COMPONENT_ACQ_CLRMASKBIT(voiceVlanInfo->portData[operIndex].acquiredList,L7_PORT_MIRROR_COMPONENT_ID);
  }
  return;
}

/*********************************************************************
* @purpose  To Add the Vlan Participation for the given port
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    vlanId     @b{(input)} vlan id
* @param    flag       @b{(input)} TRUE/FALSE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t voiceVlanSetVlanPariticipation(L7_uint32 intIfNum,
                                       L7_uint32 vlanId,
                                       L7_BOOL   flag)
{
  L7_uint32 mode;
  L7_uint32 operIndex;

  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "--> %s(): ENTER, intIfNum:[%d] vlanId:[%d] flag:[%s]\r\n",
                   __FUNCTION__, intIfNum, vlanId, ((flag == L7_TRUE) ? "TRUE" : "FALSE"));

  if(vlanId == 0)
    return L7_SUCCESS;

  if((operIndex = voiceVlanGetMapTblIndex(intIfNum))== 0)
    return L7_FAILURE;

  /* check the validity of the vlan */
  if (dot1qStaticVlanCheckValid(vlanId) == L7_SUCCESS)
  {
    if (dot1qOperVlanMemberGet(vlanId,intIfNum, &mode) == L7_SUCCESS)
    {
      if (flag == L7_TRUE)
      {
        /* add the vlan participation */
        if (mode != L7_DOT1Q_FIXED && voiceVlanInfo->portData[operIndex].portStatus == L7_ENABLE &&
            voiceVlanInfo->portData[operIndex].referenceCount > 0)
        {
          if (dot1qVlanMemberSet(vlanId,intIfNum,L7_DOT1Q_FIXED,DOT1Q_VOICE_VLAN,DOT1Q_SWPORT_MODE_NONE) != L7_SUCCESS)
          {
            VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "VoiceVlan:Failed to add the vlan %d to the intf -%d \r\n", vlanId, intIfNum);
            LOG_MSG("Failed to add the vlan %d to the intf -%d \n",vlanId,intIfNum);
            return L7_FAILURE;
          }
          VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "VoiceVlan: Added the vlan %d to the intf -%d \r\n", vlanId, intIfNum);
          voiceVlanInfo->portData[operIndex].portParticipation = L7_TRUE;
        }
        VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "--> %s(): EXIT\r\n", __FUNCTION__);
        return L7_SUCCESS;
      }
      else
      {
        if (mode == L7_DOT1Q_FIXED && voiceVlanInfo->portData[operIndex].portParticipation == L7_TRUE)
        {
          if (dot1qVlanMemberGet(vlanId,intIfNum, &mode) == L7_SUCCESS && mode != L7_DOT1Q_FIXED)
          {
            if (dot1qVlanMemberRevert(vlanId,intIfNum)!= L7_SUCCESS)
            {
              VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "VoiceVlan:Failed to delete the vlan %d to the intf -%d \r\n", vlanId, intIfNum);
              LOG_MSG("Failed to delete the vlan %d to the intf -%d \n",vlanId,intIfNum);
              return L7_FAILURE;
            }
            VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "VoiceVlan: Deleted the vlan %d to the intf -%d \r\n", vlanId, intIfNum);
            voiceVlanInfo->portData[operIndex].portParticipation = L7_FALSE;
          }
        }
        VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "--> %s(): EXIT\r\n", __FUNCTION__);
        return L7_SUCCESS;
      }
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Processes  Vlan Delete Port Notify event.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    VlanId       @b{(input)} VlanId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
static
L7_RC_t voiceVlanDot1qDeletePort(L7_uint32 intIfNum,L7_uint32 vlanId)
{
  L7_uint32       intf = 0;
  L7_RC_t nimRc;
  voiceVlanPortCfg_t *pCfg;
  L7_uint32 operIndex;

  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "--> %s(): ENTER, intIfNum:[%d] vlanId:[%d]\r\n",
                   __FUNCTION__, intIfNum, vlanId);

  nimRc = voiceVlanFirstValidIntfNumber(&intf);
  while(nimRc == L7_SUCCESS)
  {
    if(voiceVlanIntfIsConfigurable(intf, &pCfg) != L7_TRUE)
    {
      nimRc = voiceVlanNextValidIntf(intf, &intf);
      continue;
    }

    if((operIndex = voiceVlanGetMapTblIndex(intIfNum))== 0)
      continue;

    if((dot1qStaticVlanCheckValid(pCfg->voiceVlanId) == L7_SUCCESS)&&
       (pCfg->voiceVlanId == vlanId))
    {
      if (osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) == L7_SUCCESS)
      {
        if (voiceVlanInfo->portData[operIndex].portParticipation == L7_TRUE)
        {
          voiceVlanSetVlanPariticipation(intIfNum,vlanId,L7_TRUE);
        }
      }
      osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    }
    nimRc = voiceVlanNextValidIntf(intf, &intf);

  }
  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "--> %s(): EXIT\r\n", __FUNCTION__);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine if the interface is valid to participate in voice
*           vlan
*
* @param    intIfNum     @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL voiceVlanIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if((nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS) ||
     (sysIntfType != L7_PHYSICAL_INTF))
  {
    return L7_FALSE;
  }
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to voiceVlan port config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @comments The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL voiceVlanIntfIsConfigurable(L7_uint32 intIfNum,voiceVlanPortCfg_t **pCfg)
{
  L7_uint32 index;
  nimConfigID_t configId;

  if(!(VOICE_VLAN_READY))
    return L7_FALSE;

  index = voiceVlanGetMapTblIndex(intIfNum);

  if(index == 0)
    return L7_FALSE;

  /* verify that the configId in the config data table entry matches the configId that NIM maps to
  ** the intIfNum we are considering
  */
  if(nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if(NIM_CONFIG_ID_IS_EQUAL(&configId, &(voiceVlanCfg->portCfg[index].configId)) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between voiceVlanCfg and voiceVlanMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      LOG_MSG("Error accessing voiceVlan config data for interface %d in voiceVlanIntfIsConfigurable.\n", intIfNum);
      return L7_FALSE;
    }
  }

  *pCfg = &voiceVlanCfg->portCfg[index];

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Processes Voice Vlan-related event initiated by LLDP
*
* @param (in)    intIfNum  Interface Number
*
* @param (in)    data
*
* @returns  L7_SUCCESS  or L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t voiceVlanLLDPNotificationCallback(L7_uint32 intIfNum,  lldpXMedNotify_t *data)
{
  voiceVlanPortCfg_t *pCfg;
  L7_uint32          operindex;
  L7_uchar8          macStr[20];

  if(data->event < LLDP_MED_ADD_EVENT || data->event > LLDP_MED_REMOVE_EVENT)
  {
    /* This event is not interesting to me */
    return L7_SUCCESS;
  }

  if (voiceVlanIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  if((operindex = voiceVlanGetMapTblIndex(intIfNum))== 0)
    return L7_FAILURE;

  if (voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  switch (data->event)
  {
    case LLDP_MED_ADD_EVENT:
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s: Recieved LLDP_MED_ADD_EVENT Notification for %d \n",__FUNCTION__,intIfNum);
      if (voiceVlanPortVoipDeviceAdd(intIfNum,data->evData.med.mac,L7_FALSE)!=L7_SUCCESS)
      {
        l7utilsMacAddrHexToString(data->evData.med.mac, 20, macStr);
        VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH,"%s: Could not add device information for Iface [%d] Mac[%s] \n",
                         __FUNCTION__,intIfNum,macStr);
      }

      break;

    case LLDP_MED_REMOVE_EVENT:
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s: Recieved LLDP_MED_REMOVE_EVENT Notification for %d \n",__FUNCTION__,intIfNum);
      if (voiceVlanPortVoipDeviceRemove(intIfNum,data->evData.med.mac)!=L7_SUCCESS)
      {
        l7utilsMacAddrHexToString(data->evData.med.mac, 20, macStr);
        VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH,"%s: Could not remove device information for Iface [%d] Mac[%s] \n",
                         __FUNCTION__,intIfNum,macStr);
      }
      break;

    default:
      break;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Processes Voice Vlan-related event initiated by Dot1Q
*
* @param (in)    vlanId    Virtual LAN Id
* @param (in)    intIfNum  Interface Number
* @param (in)    event
*
* @returns  L7_SUCCESS  or L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t voiceVlanDot1QNotificationCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event)
{
  L7_uint32      i = 0, vlanId = 0, numVlans = 0;

    for (i = 1; i<=L7_VLAN_MAX_MASK_BIT; i++)
    {
      if (vlanData->numVlans == 1)
      {
          vlanId = vlanData->data.vlanId;
          /* For any continue, we will break out */
          i = L7_VLAN_MAX_MASK_BIT + 1;
      }
      else
      {
          if (L7_VLAN_ISMASKBITSET(vlanData->data.vlanMask,i))
          {
              vlanId = i;
          }
          else
          {
              if (numVlans == vlanData->numVlans)
              {
                  /* Already taken care of all the bits in the mask so break out of for loop */
                  break;
              }
              else
              {
                  /* Vlan is not set check for the next bit since there are more bits that are set*/
                  continue;
              }
          }
      }
      /* Currently we respond to VLAN_DELETE_PENDING_NOTIFY, VLAN_ADD_NOTIFY, and VLAN_DELETE_PORT_NOTIFY
       * Each of the case routines called check for the fact if the port's config, pCfg->voiceVlanId == vlanId
       * if not then they skip to the next interface without performing any action. We take advantage of the fact
       * that we maintain a mask of all the vlans which are configured as voice vlan and process only those
       * vlans. This saves a lot of loops in the code. Potentially 4k time max num ports in system.
       */
      if (L7_VLAN_ISMASKBITSET(voiceVlanMask,vlanId))
      {
        /* There is one or more interfaces who have this vlan id configured as voice vlans */
          switch(event)
          {
            case VLAN_DELETE_PENDING_NOTIFY:
                VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s: Recieved Vlan Delete Notification for %d for vlan - %d \n",__FUNCTION__,intIfNum,vlanId);
                voiceVlanDot1qDelete(intIfNum,vlanId);
                break;

            case VLAN_ADD_NOTIFY:
                VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s: Recieved Vlan Add Notification for %d for vlan - %d \n",__FUNCTION__,intIfNum,vlanId);
                voiceVlanDot1qAdd(intIfNum,vlanId);
                break;

            case VLAN_DELETE_PORT_NOTIFY:
                VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s: Recieved Vlan Delete Port Notification for %d for vlan - %d \n",__FUNCTION__,intIfNum,vlanId);
                voiceVlanDot1qDeletePort(intIfNum ,vlanId);
                break;

              default:
                break;
          }
      }
      numVlans++;
    }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_ATTACH
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t voiceVlanIntfAttach(L7_uint32 intIfNum)
{
  voiceVlanPortCfg_t *pCfg=L7_NULL;

  if(voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    return L7_SUCCESS;
  }
  return voiceVlanApplyPortConfigData(intIfNum);
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DETACH
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
static L7_RC_t voiceVlanIntfDetach(L7_uint32 intIfNum)
{
  voiceVlanPortCfg_t *pCfg=L7_NULL;

  if(voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    return L7_SUCCESS;
  }
  voiceVlanPortStatusSet(intIfNum,FD_VOICE_VLAN_ADMIN_MODE);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Process Link state changes
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    intIfEvent @b{(input)) interface event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t voiceVlanIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 intfEvent, NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;

  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "--> %s(): ENTER, ifNum:[%d] intfEvent:[%d]\r\n", __FUNCTION__, intIfNum, intfEvent);

  status.intIfNum     = intIfNum;
  status.component    = L7_VOICE_VLAN_COMPONENT_ID;
  status.event        = intfEvent;
  status.correlator   = correlator;
  status.response.reason =  0;

  if (!(VOICE_VLAN_READY))
  {
    LOG_MSG("%s: Received an interface change in invalid state \n",__FUNCTION__);
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s: intIfNum:[%d] Event[%d] Invalid state[%d]\r\n",
                     __FUNCTION__, intIfNum, intfEvent, voiceVlanPortCnfgrState);
    rc = L7_FAILURE;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  if ((intfEvent != L7_CREATE) && (intfEvent != L7_DELETE) && (intfEvent != L7_ATTACH))
  {
    /* Ensure Voice Vlan is globally enabled */
    if (voiceVlanCfg->voiceVlanAdminMode != L7_ENABLE)
    {
      rc = L7_SUCCESS;
      status.response.rc = rc;
      nimEventStatusCallback(status);
      return rc;
    }
  }

  if (voiceVlanIsValidIntf(intIfNum) != L7_TRUE)
  {
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  switch (intfEvent)
  {
    case L7_CREATE:
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s: Received Intf Create Notification for %d \r\n", __FUNCTION__, intIfNum);
      voiceVlanIntfCreate(intIfNum);
      break;

    case L7_DELETE:
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s: Received Intf Delete Notification for %d \r\n", __FUNCTION__, intIfNum);
      voiceVlanPortStatusSet(intIfNum,L7_DISABLE);
      voiceVlanIntfDelete(intIfNum);
      break;

    case L7_LAG_RELEASE:
    case L7_PROBE_TEARDOWN:
      voiceVlanSetAcquireMask(intIfNum,intfEvent,L7_FALSE);

    case L7_ACTIVE:
    case L7_UP:
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM,"%s: Recieved Intf Up/Lag Notification for %d - %d \n",__FUNCTION__,intIfNum,intfEvent);
      voiceVlanApplyPortConfigData(intIfNum);
      break;

    case L7_LAG_ACQUIRE:
    case L7_PROBE_SETUP:
      voiceVlanSetAcquireMask(intIfNum,intfEvent,L7_TRUE);

    /*case L7_INACTIVE:*/
    case L7_DOWN:
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM,"%s: Recieved Intf Down/Lag Notification for %d -%d \n",__FUNCTION__,intIfNum,intfEvent);
      voiceVlanPortAuthSet(intIfNum,L7_ENABLE);
      voiceVlanPortStatusSet(intIfNum,L7_DISABLE);
      break;

    case L7_ATTACH:
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s: Received Intf Attach Notification for %d \r\n", __FUNCTION__, intIfNum);
      voiceVlanIntfAttach(intIfNum);
      break;

    case L7_DETACH:
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s: Received Intf Detach Notification for %d \r\n", __FUNCTION__, intIfNum);
      voiceVlanIntfDetach(intIfNum);
      break;
  }

  status.response.rc = rc;
  nimEventStatusCallback(status);

  return rc;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_CREATE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t voiceVlanIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t configId;
  voiceVlanPortCfg_t *pCfg=L7_NULL;
  L7_uint32 i;

  if(voiceVlanIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  if(nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  for(i = 1; i < L7_VOICE_VLAN_INTF_MAX_COUNT; i++)
  {
    if(NIM_CONFIG_ID_IS_EQUAL(&voiceVlanCfg->portCfg[i].configId, &configId))
    {
      voiceVlanMapTbl[intIfNum] = i;
      break;
    }
  }

  /* If an interface configuration entry is not already assigned to the interface, assign one */
  if(voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    if(voiceVlanIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
      return L7_FAILURE;

    /* Update the configuration structure with the config id */
    if(pCfg != L7_NULL)
      voiceVlanBuildDefaultIntfConfigData(&configId, pCfg);
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to voiceVlan port config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @comments Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @end
*********************************************************************/
L7_BOOL voiceVlanIntfConfigEntryGet(L7_uint32 intIfNum, voiceVlanPortCfg_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configId;
  nimConfigID_t configIdNull;
  L7_RC_t rc;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if(!(VOICE_VLAN_READY))
    return L7_FALSE;

  if((rc = nimConfigIdGet(intIfNum, &configId)) == L7_SUCCESS)
  {
    for(i = 1; i < L7_VOICE_VLAN_INTF_MAX_COUNT; i++)
    {
      if(NIM_CONFIG_ID_IS_EQUAL(&voiceVlanCfg->portCfg[i].configId, &configIdNull))
      {
        voiceVlanMapTbl[intIfNum] = i;
        *pCfg = &voiceVlanCfg->portCfg[i];
        return L7_TRUE;
      }
    }
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DELETE
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t voiceVlanIntfDelete(L7_uint32 intIfNum)
{
  voiceVlanPortCfg_t *pCfg;

  if(voiceVlanIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
    voiceVlanMapTbl[intIfNum] = 0;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable/Disable  voice vlan on a specified interface
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    status @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  voiceVlanPortStatusSet(L7_uint32 intfNum, L7_uint32 status)
{
  voiceVlanPortCfg_t *pCfg;
  L7_uchar8 zeroMac[L7_MAC_ADDR_LEN];
  lldpXMedPolicyInfoParms_t policyInfo;
  L7_uint32 activeState=L7_INACTIVE,operIndex=0;
  L7_RC_t rc = L7_FAILURE;

  if((operIndex = voiceVlanGetMapTblIndex(intfNum))== 0)
    return L7_FAILURE;

  memset(&policyInfo,0,sizeof(lldpXMedPolicyInfoParms_t));
  memset(zeroMac,0,L7_MAC_ADDR_LEN);

  if (voiceVlanIntfIsConfigurable(intfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s: Interface [%d] Status [%d] Auth State[%d]\r\n",
                       __FUNCTION__, intfNum,status,voiceVlanInfo->portData[operIndex].authState);
  /* check the interface state */
  rc = nimGetIntfActiveState(intfNum, &activeState);
  if (L7_FAILURE==rc || (((activeState != L7_ACTIVE)&&(activeState != L7_UP)) && status == L7_ENABLE))
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s:Error Interface [%d] Status [%d] Intf state [%d] Auth State[%d]\r\n",
                       __FUNCTION__, intfNum,status,activeState,voiceVlanInfo->portData[operIndex].authState);
    return L7_FAILURE;
  }
  if (osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s: Interface [%d] Status [%d] Operational Status [%d]\r\n",
                   __FUNCTION__, intfNum,status,voiceVlanInfo->portData[operIndex].portStatus);
  if (status == L7_ENABLE && status != voiceVlanInfo->portData[operIndex].portStatus)
  {
    /* check if administratively enabled */
    if ((voiceVlanCfg->voiceVlanAdminMode == L7_ENABLE) && (pCfg->adminMode == L7_ENABLE))
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s: Voice Vlan admin mode [%d] Configured Interface mode [%d] \r\n",
                       __FUNCTION__,voiceVlanCfg->voiceVlanAdminMode,pCfg->adminMode );
      L7_uint32 currAcquired;

      /* check none of components have acquired the port */
      COMPONENT_ACQ_NONZEROMASK(voiceVlanInfo->portData[operIndex].acquiredList,currAcquired);

      if (currAcquired == L7_FALSE)
      {

        /*Set port status to enable */
        voiceVlanInfo->portData[operIndex].portStatus  =  L7_ENABLE;
        VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s: Auth Mode [%d] currently accquired [%d],operational port status [%d] \r\n",
                         __FUNCTION__,voiceVlanInfo->portData[operIndex].authState,currAcquired,voiceVlanInfo->portData[operIndex].portStatus );

        if (voiceVlanInfo->portData[operIndex].referenceCount > 0)
        {
          osapiSemaGive(voiceVlanInfo->voiceVlanLock);
          if ((rc=voiceVlanPortVoipDeviceAddAll(intfNum))!= L7_SUCCESS )
          {
            VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Applying all voip device information identified on iFace[%d] to hardware failed !\r\n",
                             __FUNCTION__, intfNum);
          }
          if (osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) != L7_SUCCESS)
            return L7_FAILURE;
        }
      }
      else
      {
        VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s: Currently accquired [%d],operational port status [%d] \r\n",
                         __FUNCTION__,currAcquired,voiceVlanInfo->portData[operIndex].portStatus );
        osapiSemaGive(voiceVlanInfo->voiceVlanLock);
        return L7_FAILURE;
      }
    }
    else
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s: VoiceVlan AdminMode[%d] PortAdminMode[%d]\r\n",
                       __FUNCTION__, voiceVlanCfg->voiceVlanAdminMode, pCfg->adminMode);
      osapiSemaGive(voiceVlanInfo->voiceVlanLock);
      return L7_FAILURE;
    }
  }
  else if (status == L7_DISABLE && status != voiceVlanInfo->portData[operIndex].portStatus)
  {
    if (voiceVlanInfo->portData[operIndex].referenceCount > 0)
    {
      osapiSemaGive(voiceVlanInfo->voiceVlanLock);
      if ((rc=voiceVlanPortVoipDeviceRemoveAll(intfNum))!= L7_SUCCESS )
      {
        VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Removing all voip device information on iFace[%d] failed !\r\n",
                         __FUNCTION__, intfNum);
      }
      if (osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) != L7_SUCCESS)
        return L7_FAILURE;
    }
    voiceVlanInfo->portData[operIndex].portStatus = L7_DISABLE;
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH,"%s: Voice Vlan Status Inactive for  %d \n",__FUNCTION__,intfNum);
  }
  osapiSemaGive(voiceVlanInfo->voiceVlanLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable  voice vlan to a specified interface
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    status @b{(input)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t  voiceVlanPortStatusGet(L7_uint32 intfNum, L7_uint32 *status)
{
  L7_uint32 operIndex =0;

  if((operIndex = voiceVlanGetMapTblIndex(intfNum))== 0)
    return L7_FAILURE;

  if(osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  *status = voiceVlanInfo->portData[operIndex].portStatus;
  osapiSemaGive(voiceVlanInfo->voiceVlanLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Processes  Vlan Add Notify event.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    VlanId       @b{(input)} VlanId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanDot1qAdd(L7_uint32 intIfNum, L7_uint32 vlanId)
{
  voiceVlanPortCfg_t *pCfg;

  if (voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  if ((dot1qStaticVlanCheckValid(pCfg->voiceVlanId) == L7_SUCCESS) &&
      (pCfg->voiceVlanId == vlanId))
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s: VLAN[%d] Add Notific'n for intf[%d]\r\n",
                     __FUNCTION__, vlanId, intIfNum);
    (void)voiceVlanPortStatusSet(intIfNum, L7_ENABLE);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Return Internal Interface Number of the first valid interface for
*           voice vlan.
*
* @param    pFirstIntIfNum @b{(output)}  pointer to first internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t voiceVlanFirstValidIntfNumber(L7_uint32 *pFirstIntIfNum)
{
  L7_RC_t rc;

  rc = nimFirstValidIntfNumberByType(L7_PHYSICAL_INTF, pFirstIntIfNum);

  return(rc);
}

/*********************************************************************
* @purpose  Return Internal Interface Number of next valid interface for
*           voice vlan.
*
* @param    intIfNum  @b{(input)}   Internal Interface Number
* @param    pNextintIfNum @b{(output)}  pointer to Next Internal Interface Number,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t voiceVlanNextValidIntf(L7_uint32 intIfNum, L7_uint32 *pNextIntIfNum)
{
  L7_RC_t rc;

  rc = nimNextValidIntfNumberByType(L7_PHYSICAL_INTF, intIfNum, pNextIntIfNum);

  return(rc);
}

/*********************************************************************
* @purpose  Processes  Vlan Delete Notify event.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    VlanId       @b{(input)} VlanId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanDot1qDelete(L7_uint32 intIfNum,L7_uint32 vlanId)
{
  voiceVlanPortCfg_t *pCfg;

  if (voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  if ((dot1qStaticVlanCheckValid(pCfg->voiceVlanId) == L7_SUCCESS) &&
      (pCfg->voiceVlanId == vlanId))
  {
    (void)voiceVlanPortStatusSet(intIfNum, L7_DISABLE);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a Voip Device Mac address to the voice vlan database.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    mac       @b{(input)} mac
* @param    vlanId    @b{(input)} voice vlan Id associated with this device
* @param    clientAuthState @b{(input)} clientAuthState of the device
*
* @returns -1,0-,1
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDeviceAddDB(L7_uint32 intIfNum,L7_uchar8 *mac, L7_uint32 vlanId,L7_BOOL clientAuthState)
{
  voiceVlanDeviceInfo_t *node;
  L7_enetMacAddr_t      macAddr;
  L7_RC_t               rc = L7_FAILURE;
  L7_uchar8             macStr[20];

  if ((node = voiceVlanDeviceInfoGet(mac))==L7_NULLPTR)
  {
    /* Add to voice vlan database*/
    rc = voiceVlanDeviceInfoAlloc(mac,vlanId,intIfNum,clientAuthState);
    if (rc!= L7_SUCCESS)
    {
      l7utilsMacAddrHexToString(mac, 20, macStr);
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Voice Vlan Device Info add for iFace[%d], mac [%s] Failed!\r\n",
                       __FUNCTION__, intIfNum,macStr );
      return L7_FAILURE;
    }

    /* Add device information to hardware*/
    memset(macAddr.addr,0,sizeof(L7_enetMacAddr_t));
    memcpy(macAddr.addr,mac,L7_MAC_ADDR_LEN);
    if ((rc=dtlVoiceVlanDeviceAdd(intIfNum,macAddr,vlanId))!=L7_SUCCESS)
    {
      /*Error msg*/
      l7utilsMacAddrHexToString(mac, 20, macStr);
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Add Voice Vlan Device Info to hardware failed  for iFace[%d], mac [%s] !\r\n",
                       __FUNCTION__, intIfNum, macStr);
      /* remove device infor from voice vlan database*/
      voiceVlanDeviceInfoDeAlloc(node);
      return L7_FAILURE;
    }
  }
  else
  {
    /*TBD*/
    /*Check what is to be done in case of roaming*/
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Remove a Voip Device Mac address from a voice vlan database.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    mac       @b{(input)} mac
* @param    vlanId    @b{(input)} voice vlan Id associated with this device
* @param    clientAuthState @b{(input)} clientAuthState of the device
*
* @returns -1,0-,1
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortDeviceRemoveDB(L7_uint32 intIfNum,L7_uchar8 *mac)
{
  voiceVlanDeviceInfo_t *node;
  L7_enetMacAddr_t      macAddr;
  L7_RC_t               rc = L7_FAILURE;
  L7_uchar8             macStr[20];

  if (mac == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  if ((node = voiceVlanDeviceInfoGet(mac))!=L7_NULLPTR)
  {
    /*remove device information from hardware */
    memset(macAddr.addr,0,sizeof(L7_enetMacAddr_t));
    memcpy(macAddr.addr,mac,L7_MAC_ADDR_LEN);
    if ((rc=dtlVoiceVlanDeviceRemove(intIfNum,macAddr,node->vlanId))==L7_SUCCESS )
    {
      if ((rc=voiceVlanDeviceInfoDeAlloc(node))!= L7_SUCCESS)
      {
        l7utilsMacAddrHexToString(mac, 20, macStr);
        VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Voice Vlan Device Info remove for iFace[%d], mac %s Failed!\r\n",
                         __FUNCTION__, intIfNum, macStr);
        return L7_FAILURE;
      }
    }
    else
    {
      /*Error msg*/
      l7utilsMacAddrHexToString(mac, 20, macStr);
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Remove Voice Vlan Device Info from hardware failed for iFace[%d], mac %s !\r\n",
                       __FUNCTION__, intIfNum, macStr);
      return L7_FAILURE;
    }
  }
  else
  {
    l7utilsMacAddrHexToString(mac, 20, macStr);
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Voice Vlan Device Info remove for iFace[%d], mac %s Failed!Could not find device!\r\n",
                     __FUNCTION__, intIfNum,macStr );
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a Voip Device Mac address.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    mac       @b{(input)} mac
* @param   clientAuthestate @b{(input)} checks if voip device is unauthenticated voip device
*
* @returns -1,0-,1
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVoipDeviceAdd(L7_uint32 intIfNum,L7_uchar8 *mac_addr, L7_BOOL clientAuthState)
{
  voiceVlanPortCfg_t *pCfg;
  L7_uint32 mode = L7_DISABLE;
  L7_uchar8 zeroMac[L7_MAC_ADDR_LEN];
  L7_uint32 operIndex=0;
  voiceVlanDeviceInfo_t *node=L7_NULLPTR;
  lldpXMedPolicyInfoParms_t policyInfo;
  L7_uint32 linkState = L7_DOWN;
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8   macStr[20];

  if (osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /*check input*/
  if((operIndex = voiceVlanGetMapTblIndex(intIfNum))== 0)
  {
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return L7_FAILURE;
  }

  memset(zeroMac,0,L7_MAC_ADDR_LEN);
  if ((mac_addr == L7_NULL) ||
      (memcmp(mac_addr,zeroMac,L7_MAC_ADDR_LEN)==0))
  {
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return  L7_FAILURE;
  }
  /* convert given macAddr to string*/
  l7utilsMacAddrHexToString(mac_addr, 20, macStr);

  if (voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return L7_FAILURE;
  }

  /* check the interface state */
  if ((L7_SUCCESS != nimGetIntfLinkState(intIfNum, &linkState)) ||
      ((L7_UP != linkState)))
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Iface[%d] is not active\r\n", __FUNCTION__, intIfNum);
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return L7_FAILURE;
  }

  /* check if administratively enabled */
  if ((voiceVlanCfg->voiceVlanAdminMode == L7_ENABLE) && (pCfg->adminMode == L7_ENABLE))
  {
    L7_uint32 currAcquired;
    L7_uint32  mode;

    /* check none of components have acquired the port */
    COMPONENT_ACQ_NONZEROMASK(voiceVlanInfo->portData[operIndex].acquiredList, currAcquired);

    if (currAcquired == L7_FALSE)
    {
      if (voiceVlanInfo->portData[operIndex].authState == L7_ENABLE)
      {
        /* check LLDP mode */
        lldpXMedPortAdminModeGet(intIfNum, &mode);
        if (mode != L7_ENABLE)
        {
          VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): LLDP has not been enabled on Iface [%d] \r\n", __FUNCTION__, intIfNum);
          osapiSemaGive(voiceVlanInfo->voiceVlanLock);
          return L7_FAILURE;
        }
      }
    }
    else
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Iface [%d] has been accquired by another component\r\n", __FUNCTION__, intIfNum);
      osapiSemaGive(voiceVlanInfo->voiceVlanLock);
      return L7_FAILURE;
    }
  }
  else
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Voice Vlan component has not been enabled\r\n", __FUNCTION__);
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return L7_FAILURE;
  }

  /* check if information is already in the database*/
  node=voiceVlanDeviceInfoGet(mac_addr);
  if (node==L7_NULLPTR)
  {
    /*1. Increment reference count*/
    voiceVlanInfo->portData[operIndex].referenceCount++;

    /* add client information to the voice vlan database */
    /* Add to voice vlan database*/
    if ((rc=voiceVlanPortDeviceAddDB(intIfNum,mac_addr,pCfg->voiceVlanId,clientAuthState))!= L7_SUCCESS)
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Voice Vlan Device Info Add for iFace[%d], mac %s Failed!\r\n",
                       __FUNCTION__, intIfNum, macStr);
      voiceVlanInfo->portData[operIndex].referenceCount--;
      osapiSemaGive(voiceVlanInfo->voiceVlanLock);
      return L7_FAILURE;
    }
  }

  /* 2.if unauthenticated voice vlan support has been enabled for the port , attempt to authenticate the client through dot1x*/
  if (pCfg->voiceVlanId != FD_VOICE_VLAN_VLAN_ID)
  {
    if (voiceVlanInfo->portData[operIndex].portStatus == L7_ENABLE)
    {
      if (clientAuthState == L7_TRUE)
      {
        L7_enetMacAddr_t mac;

        memcpy(mac.addr, mac_addr, sizeof(mac.addr));
        /* osapiSemaGive(voiceVlanInfo->voiceVlanLock);*/
        if (L7_SUCCESS != dot1xPortVoiceVlanAuthMode(intIfNum, mac,
                                                     pCfg->voiceVlanId, L7_TRUE))
        {
          VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): dot1x auth. for iFace[%d], mac %s Failed!\r\n",
                           __FUNCTION__, intIfNum, macStr);
          voiceVlanInfo->portData[operIndex].referenceCount--;
          osapiSemaGive(voiceVlanInfo->voiceVlanLock);
          return L7_FAILURE;
        }
        else
        {
          /* if (osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) != L7_SUCCESS)
             {
             return L7_FAILURE;
             } */
          if (voiceVlanInfo->portData[operIndex].referenceCount ==1)
          {
            voiceVlanInfo->portData[operIndex].vlanId = pCfg->voiceVlanId;
          }

          /* add a entry in FDB*/
          fdbInsert(mac.addr,intIfNum,pCfg->voiceVlanId, L7_FDB_ADDR_FLAG_LEARNED);
          VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s(): dot1x auth. for iFace[%d], mac %s succeeded!\r\n",
                           __FUNCTION__, intIfNum, macStr);
        }
      }/*client Auth State = L7_TRUE*/
      else
      {
        /* add port to voice vlan if required.*/
        if (voiceVlanInfo->portData[operIndex].referenceCount==1 &&
            voiceVlanSetVlanPariticipation(intIfNum, pCfg->voiceVlanId, L7_TRUE) != L7_SUCCESS)
        {
          /* error condition */
          VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): VLAN[%d] Participation for iFace[%d] Failed!\r\n",
                           __FUNCTION__, pCfg->voiceVlanId, intIfNum);
          if ((rc=voiceVlanPortDeviceRemoveDB(intIfNum,mac_addr)) != L7_SUCCESS)
          {
            VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Removing device information from voice vlan database failed for iFace[%d] mac[%s]!\r\n",
                             __FUNCTION__, intIfNum,macStr);
          }
          else
          {
            voiceVlanInfo->portData[operIndex].referenceCount--;
          }
          osapiSemaGive(voiceVlanInfo->voiceVlanLock);
          return L7_FAILURE;
        }
        voiceVlanInfo->portData[operIndex].vlanId = pCfg->voiceVlanId;
      }/*clientAuth != L7_TRUE*/
    }/* port status is enabled */
  }/* Vlan Id configured*/

  /* if Cos override has been configured on the port , reset CoS override policy on that port in hardware*/
  if ((pCfg->defaultCosOverride == L7_TRUE) && (voiceVlanInfo->portData[operIndex].portStatus == L7_ENABLE))
  {
    dtlVoiceVlanCosOverrideSet(intIfNum,L7_TRUE);
  }

  /*notify LLDP-MED*/
  if ((voiceVlanInfo->portData[operIndex].referenceCount ==1) && (voiceVlanInfo->portData[operIndex].portStatus == L7_ENABLE))
  {
    if (L7_ENABLE == voiceVlanInfo->portData[operIndex].lldpPduAuthFlag)
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Auth LLDP Pdu is Enabled!\r\n", __FUNCTION__);
    }
    else
    {
      /* check LLDP mode */
      lldpXMedPortAdminModeGet(intIfNum, &mode);
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s():Iface[%d], LLDP AdminMode[%d] \r\n",
                       __FUNCTION__,intIfNum, mode);

      if (mode == L7_ENABLE)
      {
        policyInfo.vlanId   = pCfg->voiceVlanId;
        policyInfo.intIfNum = intIfNum;
        policyInfo.type     = voice_policyapptype;
        policyInfo.priority = pCfg->portPriority;
        policyInfo.U        = 1;
        policyInfo.T        = (pCfg->untaggingEnabled == L7_TRUE) ? 0 : 1;
        policyInfo.dscp     = pCfg->dscp;
        VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s: Voice Vlan Status Active for %d\r\n", __FUNCTION__, intIfNum);

        if (pCfg->noneMode != L7_TRUE)
        {
          if (lldpXMedPolicyEventCallback(intIfNum, &policyInfo) == L7_SUCCESS)
          {
            VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s: Sent notification to LLDP for intf[%d]\r\n",
                             __FUNCTION__, intIfNum);
          }
          else
          {
            VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s: Could not send notification to LLDP for intf[%d]\r\n",
                             __FUNCTION__, intIfNum);
          }
        }
      }
    }
  }

  osapiSemaGive(voiceVlanInfo->voiceVlanLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Remove a Voip Device Mac address.
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    mac       @b{(input)} mac
*
* @returns -1,0-,1
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVoipDeviceRemove(L7_uint32 intIfNum,L7_uchar8 *mac_addr)
{
  voiceVlanPortCfg_t *pCfg;
  voiceVlanDeviceInfo_t *node= L7_NULLPTR;
  L7_uchar8 zeroMac[L7_MAC_ADDR_LEN];
  L7_uint32 operIndex=0;
  L7_RC_t rc= L7_FAILURE;
  L7_uint32 mode = L7_DISABLE;
  lldpXMedPolicyInfoParms_t policyInfo;
  L7_enetMacAddr_t macAddr;
  L7_uint32 currAcquired;
  L7_uchar8 macStr[20];

  if (osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if((operIndex = voiceVlanGetMapTblIndex(intIfNum))== 0)
  {
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return L7_FAILURE;
  }

  memset(zeroMac,0,L7_MAC_ADDR_LEN);
  if ((mac_addr == L7_NULL) ||
      (memcmp(mac_addr,zeroMac,L7_MAC_ADDR_LEN)==0))
  {
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return  L7_FAILURE;
  }
  /* Convert given mac address to string*/
  l7utilsMacAddrHexToString(mac_addr, 20, macStr);

  if (voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return L7_FAILURE;
  }

  /* check none of components have acquired the port */
  COMPONENT_ACQ_NONZEROMASK(voiceVlanInfo->portData[operIndex].acquiredList, currAcquired);

  if (currAcquired != L7_FALSE)
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Iface [%d] has been accquired by another component\r\n", __FUNCTION__, intIfNum);
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return L7_FAILURE;
  }

  node=voiceVlanDeviceInfoGet(mac_addr);
  if (node==L7_NULLPTR)
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Voice Vlan Device Info remove for iFace[%d], mac %s Failed!Could not find Device!\r\n",
                     __FUNCTION__, intIfNum, macStr);
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return L7_FAILURE;
  }

  /*decrement refernce count*/
  voiceVlanInfo->portData[operIndex].referenceCount--;
  /*1. Inform dot1x */
  if (node->clientAuthState == L7_TRUE)
  {
    dot1dTpFdbData_t pData;

    memset(macAddr.addr,0,sizeof(L7_enetMacAddr_t));
    memcpy(macAddr.addr,mac_addr,L7_MAC_ADDR_LEN);
    /* osapiSemaGive(voiceVlanInfo->voiceVlanLock);*/
    if (L7_SUCCESS != dot1xPortVoiceVlanAuthMode(intIfNum,macAddr ,
                                                 pCfg->voiceVlanId, L7_FALSE))
    {
      /*
       * OK for failure since dot1x may have already deleted the MAC address.
       */
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s(): dot1x unauth. for iFace[%d], mac %s Failed!\r\n",
                       __FUNCTION__, intIfNum, macStr);
    }
    else if (L7_SUCCESS == fdbFind(macAddr.addr, L7_MATCH_EXACT, &pData))
    {
      /* remove FDB entry*/
      fdbDelete(macAddr.addr, pCfg->voiceVlanId);
    }
  }
  else
  {
    if (voiceVlanInfo->portData[operIndex].referenceCount == 0 &&
        voiceVlanSetVlanPariticipation(intIfNum,voiceVlanInfo->portData[operIndex].vlanId, L7_FALSE) != L7_SUCCESS)
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Voice Vlan Device Info remove for iFace[%d], mac %s Failed!Could not reset vlan participation!\r\n",
                       __FUNCTION__, intIfNum,macStr);
      voiceVlanInfo->portData[operIndex].referenceCount++;
      osapiSemaGive(voiceVlanInfo->voiceVlanLock);
      return L7_FAILURE;
    }
    else
    {
      voiceVlanInfo->portData[operIndex].vlanId = pCfg->voiceVlanId;
    }
  }

  /*2. notify lldp */
  if (voiceVlanInfo->portData[intIfNum].referenceCount==0)
  {
    /* check LLDP mode */
    lldpXMedPortAdminModeGet(intIfNum, &mode);
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s(): LLDP AdminMode[%d]\r\n", __FUNCTION__, mode);

    if (mode == L7_ENABLE)
    {
      policyInfo.intIfNum = intIfNum;
      policyInfo.type     = voice_policyapptype;

      if (pCfg->noneMode != L7_TRUE)
      {
        if (lldpXMedPolicyEventCallback(intIfNum, &policyInfo) == L7_SUCCESS)
        {
          VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s: Sent notification to LLDP for intf[%d]\r\n",
                           __FUNCTION__, intIfNum);
        }
        else
        {
          VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s: Could not send notification to LLDP for intf[%d]\r\n",
                           __FUNCTION__, intIfNum);
        }
      }
    }
  }

  /* 3. Check if cos override has been set for the port */
  if (pCfg->defaultCosOverride == L7_TRUE)
  {
    /* remove devie from voice vlan DB*/
    if ((rc=voiceVlanPortDeviceRemoveDB(intIfNum,mac_addr))== L7_SUCCESS)
    {
      /*set dtl Cos Override for the port*/
      dtlVoiceVlanCosOverrideSet(intIfNum,L7_FALSE);
    }
    else
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Voice Vlan Device Info remove for iFace[%d], mac %s Failed!\
                                               Could not remove device form voice vlan database!\r\n",
                       __FUNCTION__, intIfNum,macStr);
      voiceVlanInfo->portData[operIndex].referenceCount++;
      osapiSemaGive(voiceVlanInfo->voiceVlanLock);
      return L7_FAILURE;
    }
  }
  else
  {
    /*remove device from voice vlan DB*/
    if ((rc=voiceVlanPortDeviceRemoveDB(intIfNum,mac_addr))!= L7_SUCCESS)
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Voice Vlan Device Info remove for iFace[%d], mac %s Failed!\
                                               Could not remove device form voice vlan database!\r\n",
                       __FUNCTION__, intIfNum, macStr);
      voiceVlanInfo->portData[operIndex].referenceCount++;
      osapiSemaGive(voiceVlanInfo->voiceVlanLock);
      return L7_FAILURE;
    }
  }

  osapiSemaGive(voiceVlanInfo->voiceVlanLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Remove all the Voip Devices on a interface.
*
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns -1,0-,1
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVoipDeviceRemoveAll(L7_uint32 intIfNum)
{
  L7_enetMacAddr_t mac_addr;
  voiceVlanDeviceInfo_t *node = L7_NULLPTR;
  L7_RC_t rc=L7_FAILURE;
  L7_BOOL removalFailed;
  L7_uchar8 macStr[20];

  memset(mac_addr.addr,0,sizeof(L7_enetMacAddr_t));
  removalFailed = L7_FALSE;
  while((node=voiceVlanDeviceInfoGetNextNode(intIfNum,mac_addr.addr))!=L7_NULLPTR)
  {
    memset(mac_addr.addr,0,L7_MAC_ADDR_LEN);
    memcpy(mac_addr.addr,node->voipMac,L7_MAC_ADDR_LEN);
    if ((rc = voiceVlanPortVoipDeviceRemove(intIfNum,node->voipMac))!=L7_SUCCESS)
    {
      l7utilsMacAddrHexToString(mac_addr.addr, 20, macStr);
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s():Could not remove device from voice vlan database for iFace[%d], mac %s!\r\n",
                       __FUNCTION__, intIfNum, macStr);
      removalFailed = L7_TRUE;
    }
  }

  if (removalFailed == L7_TRUE)
  {
    return  L7_FAILURE;
  }
  else
  {
    return L7_SUCCESS;
  }
}

/*********************************************************************
* @purpose  Add all known Voip Devices on a interface.
*
* @param    intIfNum   @b{(input)} internal interface number
*
* @returns -1,0-,1
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortVoipDeviceAddAll(L7_uint32 intIfNum)
{
  L7_enetMacAddr_t mac_addr;
  voiceVlanDeviceInfo_t *node = L7_NULLPTR;
  L7_RC_t rc=L7_FAILURE;
  L7_BOOL additionFailed;
  L7_uchar8 macStr[20];

  memset(mac_addr.addr,0,sizeof(L7_enetMacAddr_t));
  additionFailed = L7_FALSE;
  while((node=voiceVlanDeviceInfoGetNextNode(intIfNum,mac_addr.addr))!=L7_NULLPTR)
  {
    if ((rc = voiceVlanPortVoipDeviceAdd(intIfNum,node->voipMac,node->clientAuthState))!=L7_SUCCESS)
    {
      l7utilsMacAddrHexToString(node->voipMac, 20, macStr);
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s():Could not remove device from voice vlan database for iFace[%d], mac %s!\r\n",
                       __FUNCTION__, intIfNum, macStr);
      additionFailed = L7_TRUE;
    }
    memset(mac_addr.addr,0,L7_MAC_ADDR_LEN);
    memcpy(mac_addr.addr,node->voipMac,L7_MAC_ADDR_LEN);
  }

  if (additionFailed == L7_TRUE)
  {
    return  L7_FAILURE;
  }
  else
  {
    return L7_SUCCESS;
  }
}

/*********************************************************************
* @purpose  Set the Count for the number of Override Ports
*
* @param    intIfNum   @b{(input)} internal interface number
* @param    flag       @b{(input)} True/false
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortOverrideCount(L7_uint32 intIfNum,L7_BOOL flag)
{
  L7_RC_t rc=L7_FAILURE;

  /* take the lock */
  if(osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return rc;

  if(flag == L7_TRUE)
  {
    if(voiceVlanInfo->overridePorts < VOICE_VLAN_MAX_OVERIDE_PORTS)
    {
      voiceVlanInfo->overridePorts++;
      rc = L7_SUCCESS;
    }
  }
  else
  {
    if(voiceVlanInfo->overridePorts > 0)
    {
      voiceVlanInfo->overridePorts--;
      rc = L7_SUCCESS;
    }
  }

  osapiSemaGive(voiceVlanInfo->voiceVlanLock);
  return rc;
}

/*********************************************************************
* @purpose  Get the number of voice vlan devices identified on
*           a specified interface
*
* @param    intIfNum   @b{(input)) internal interface number
* @param    count      @b{(output)}
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortReferenceCountGet(L7_uint32 intfNum, L7_uint32 *refcount)
{
  L7_uint32 operIndex =0;

  if((operIndex = voiceVlanGetMapTblIndex(intfNum))== 0)
    return L7_FAILURE;

  if(osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  *refcount = voiceVlanInfo->portData[operIndex].referenceCount;
  osapiSemaGive(voiceVlanInfo->voiceVlanLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose Configure Voice VLAN Auth Admin State
*
* @param   state   - L7_ENABLE/L7_DISABLE
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes   L7_ENABLE will allow the unauthorized vlan function.
*          This is not a saved state as such, it is called whenever
*          one of the states this feature depends upon changes.
*          I.e., Voice VLAN Admin Mode
*                Dot1x Admin Mode
*
* @end
*
*********************************************************************/
L7_RC_t voiceVlanAuthAdminModeUpdate(L7_uint32 state)
{
  voiceVlanPortCfg_t   *pCfg;
  sysnetPduIntercept_t  sysnetPduIntercept;
  sysnetNotifyEntry_t   snEntry;
  L7_uint32             dot1xMode;
  L7_uint32             intIfNum;
  L7_RC_t               rc = L7_SUCCESS;
  static L7_uint32      voiceVlanUnauthEnabled = L7_FALSE;
  static L7_uint32      voiceVlanSysnetRegistered = L7_FALSE;

  memset(&snEntry, 0, sizeof(snEntry));
  osapiStrncpySafe(snEntry.funcName, "voiceVlanCdpPduReceiveCallback", sizeof(snEntry.funcName));
  snEntry.notify_pdu_receive = voiceVlanCdpPduReceiveCallback;
  snEntry.type               = SYSNET_MAC_ENTRY;
  memcpy(snEntry.u.macAddr, voiceVlanCdpDstMacAddr.addr, sizeof(snEntry.u.macAddr));

  memset(&sysnetPduIntercept, 0, sizeof(sysnetPduIntercept));
  sysnetPduIntercept.addressFamily  = L7_AF_INET;
  sysnetPduIntercept.hookId         = SYSNET_INET_RECV_IN;
  sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_VOICE_VLAN_PRECEDENCE;
  sysnetPduIntercept.interceptFunc  = voiceVlanDhcpPduReceiveCallback;
  osapiStrncpySafe(sysnetPduIntercept.interceptFuncName, "voiceVlanDhcpPduReceiveCallback", sizeof(sysnetPduIntercept.interceptFuncName));

  if (L7_ENABLE == state)
  {
    if ((L7_FALSE   == voiceVlanSysnetRegistered) &&
        (L7_SUCCESS == dot1xAdminModeGet(&dot1xMode)) &&
        (L7_ENABLE  == dot1xMode) &&
        (L7_ENABLE  == voiceVlanGetCfg()->voiceVlanAdminMode))
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s: Registering CDP and DHCP callback functions. state [%d]",__FUNCTION__,state);
      /* register with sysnet for CDP packets */
      rc = sysNetRegisterPduReceive(&snEntry);
      if (L7_SUCCESS != rc)
      {
        LOG_MSG("%s(): Unable to Register PDU Receive for CDP Packets!  rc = %d\r\n", __FUNCTION__, rc);
      }
      else
      {
        /* register with sysnet for DHCP packets */
        rc = sysNetPduInterceptRegister(&sysnetPduIntercept);
        if (L7_SUCCESS != rc)
        {
          sysNetDeregisterPduReceive(&snEntry);
          LOG_MSG("%s(): Could not register with sysnet for intercept on DHCP PDU receive.  rc = %d\r\n",
                  __FUNCTION__, rc);
        }
        else
        {
          voiceVlanSysnetRegistered = L7_TRUE;
        }
      }
    }

    if (L7_FALSE == voiceVlanUnauthEnabled)
    {
      voiceVlanUnauthEnabled = L7_TRUE;

      for (intIfNum = 1; intIfNum < L7_VOICE_VLAN_INTF_MAX_COUNT; intIfNum++)
      {
        if ((L7_TRUE == voiceVlanIntfIsConfigurable(intIfNum, &pCfg)) &&
            (pCfg->adminMode == L7_ENABLE))
        {
          voiceVlanPortAuthSet(intIfNum, pCfg->authState);
        }
      }
    }
  }
  else if (L7_DISABLE == state)
  {
    if (L7_TRUE == voiceVlanSysnetRegistered)
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s: Deregistering CDP and DHCP callback functions. state [%d]",__FUNCTION__,state);
      /* Deregister with sysnet for CDP packets */
      rc = sysNetDeregisterPduReceive(&snEntry);
      if (L7_SUCCESS != rc)
      {
        LOG_MSG("%s(): Unable to Deregister PDU Receive for CDP Packets!  rc = %d\r\n", __FUNCTION__, rc);
      }

      /* Deregister with sysnet for DHCP packets */
      rc = sysNetPduInterceptDeregister(&sysnetPduIntercept);
      if (L7_SUCCESS != rc)
      {
        LOG_MSG("%s(): Could not deregister with sysnet for intercept on DHCP PDU receive.  rc = %d\r\n",
                __FUNCTION__, rc);
      }

      voiceVlanSysnetRegistered = L7_FALSE;
    }

    if (L7_TRUE == voiceVlanUnauthEnabled)
    {
      voiceVlanUnauthEnabled = L7_FALSE;
      for (intIfNum = 1; intIfNum < L7_VOICE_VLAN_INTF_MAX_COUNT; intIfNum++)
      {
        if ((L7_TRUE == voiceVlanIntfIsConfigurable(intIfNum, &pCfg)) &&
            (pCfg->adminMode == L7_ENABLE))
        {
          voiceVlanPortAuthSet(intIfNum, L7_ENABLE);
        }
      }
    }
  }

  return rc;
}

/*********************************************************************
* @purpose Configure Voice VLAN Auth State
*
* @param   intfNum - InterfaceID
*          state   - L7_ENABLE/L7_DISABLE
*
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t voiceVlanPortAuthSet(L7_uint32 intfNum, L7_uint32 state)
{
  voiceVlanPortCfg_t *pCfg;
  L7_uint32 operIndex = 0;
  L7_uint32 prevAuthStatus;
  L7_uint32 dot1xMode;

  operIndex = voiceVlanGetMapTblIndex(intfNum);
  if (operIndex == 0)
  {
    return L7_FAILURE;
  }

  if (voiceVlanIntfIsConfigurable(intfNum, &pCfg) != L7_TRUE)
  {
    return L7_FAILURE;
  }

  if (osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  prevAuthStatus = voiceVlanInfo->portData[operIndex].authState;
  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "AUTH SET: InfoAuthState:[%d] state:[%d]\r\n", prevAuthStatus, state);

  if (state != prevAuthStatus)
  {
    if (state == L7_ENABLE)
    {
      voiceVlanInfo->portData[operIndex].authState = L7_ENABLE;

      dtlVoiceVlanAuthSet(intfNum, L7_FALSE);

      /* Deregister LLDP PDU Rcv */
      voiceVlanInfo->portData[operIndex].lldpPduAuthFlag = L7_DISABLE;

      if (voiceVlanInfo->portData[operIndex].referenceCount > 0)
      {
        osapiSemaGive(voiceVlanInfo->voiceVlanLock);
        voiceVlanPortStatusSet(intfNum, L7_DISABLE);
        return L7_SUCCESS;
      }

      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "VoiceVLAN Auth Status Enabled for iface %d\r\n", intfNum);
    }
    else                        /* state == L7_DISABLE */
    {
      if ((L7_SUCCESS != dot1xAdminModeGet(&dot1xMode)) ||
          (L7_ENABLE  != dot1xMode) ||
          (L7_ENABLE  != voiceVlanCfg->voiceVlanAdminMode) ||
          (L7_ENABLE  != pCfg->adminMode))
      {
        VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH,
                         "%s():VoiceVLAN AdminMode Disabled, AuthMode Set for Intf[%d] Failed!\r\n",
                         __FUNCTION__, intfNum);
        osapiSemaGive(voiceVlanInfo->voiceVlanLock);
        return L7_SUCCESS;
      }

      if (pCfg->voiceVlanId == 0)
      {
        LOG_MSG("%s()-> VOICE VLANID not Configured!\r\n", __FUNCTION__);
        osapiSemaGive(voiceVlanInfo->voiceVlanLock);
        return L7_SUCCESS;
      }

      if (dtlVoiceVlanAuthSet(intfNum, L7_TRUE) == L7_FAILURE)
      {
        VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "DTL Voice VLAN AUTH Set Failed for iface[%d]!\r\n", intfNum);
        LOG_MSG("%s(): DTL Voice VLAN AUTH Set Failed for iface[%d]!\r\n", __FUNCTION__, intfNum);
        osapiSemaGive(voiceVlanInfo->voiceVlanLock);
        return L7_FAILURE;
      }

      voiceVlanInfo->portData[operIndex].authState = L7_DISABLE;

      /* Register LLDP PDU Rcv */
      voiceVlanInfo->portData[operIndex].lldpPduAuthFlag = L7_ENABLE;



      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "VoiceVLAN Auth Status Disabled for iface %d. Auth state[%d] LLDP PDU Auth state[%d]\r\n",
                       intfNum,voiceVlanInfo->portData[operIndex].authState,voiceVlanInfo->portData[operIndex].lldpPduAuthFlag);
    }
  }

  osapiSemaGive(voiceVlanInfo->voiceVlanLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get voice vlan Auth Status for a specified interface
*
* @param    intfNum   @b{(input)) internal interface number
* @param    state      @b{(input)} reference to a pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t voiceVlanPortAuthGet(L7_uint32 intfNum, L7_uint32 *state)
{
  L7_uint32 operIndex = 0;

  if (voiceVlanIsValidIntf(intfNum) != L7_TRUE)
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Invalid Params intfNum[%d]\r\n", __FUNCTION__, intfNum);
    return L7_FAILURE;
  }

  operIndex = voiceVlanGetMapTblIndex(intfNum);
  if (operIndex == 0)
  {
    return L7_FAILURE;
  }

  if (osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  *state = voiceVlanInfo->portData[operIndex].authState;

  osapiSemaGive(voiceVlanInfo->voiceVlanLock);
  return L7_SUCCESS;
}

/*******************************************************************************
* @purpose  Send a message to the voiceVlanTask to process incoming CDP PDU
*
* @param    bufHandle          buffer  @b((input)) handle to the lldpdu received
* @param    sysnet_pdu_info_t *pduInfo @b((input)) pointer to pdu info structure
*
* @returns  L7_SUCCESS  if the message was put on the queue
* @returns  L7_FAILURE  if the message was not put on the queue
*
* @notes    none
*
* @end
********************************************************************************/
L7_RC_t voiceVlanCdpPduReceiveCallback(L7_netBufHandle bufHandle,
                                       sysnet_pdu_info_t *pduInfo)
{
  L7_uchar8          *data = L7_NULLPTR;
  L7_enet_encaps_t   *encap = L7_NULLPTR;
  L7_uint32          offset = 0;
  L7_uchar8          *pEtype;
  L7_ushort16        temp16;
  L7_8021QTag_t      *vlanTagPtr;
  voiceVlanPortCfg_t *pCfg;
  L7_uint32          operIndex = 0;

  L7_uint32          vlanId = 0;
  L7_uchar8          srcMac[L7_MAC_ADDR_LEN];
  L7_ushort16        protocol_type;
  L7_uint32          ethHeaderSize;
  L7_uint32          len = 0;
  L7_RC_t            rc = L7_FAILURE;
  L7_uint32          intIfNum = pduInfo->intIfNum;
  voiceVlanDeviceInfo_t *node = L7_NULLPTR;
  L7_uchar8          macStr[20];

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Invalid Interface[%d]\r\n", __FUNCTION__, intIfNum);
    return rc;
  }

  if (voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    return rc;
  }

  operIndex = voiceVlanGetMapTblIndex(intIfNum);
  if (operIndex == 0)
  {
    return rc;
  }

  if (osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return rc;
  }

  if ((voiceVlanCfg->voiceVlanAdminMode != L7_ENABLE) ||
      (pCfg->adminMode != L7_ENABLE) ||
      (voiceVlanInfo->portData[operIndex].authState != L7_DISABLE))
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s():Intf[%d] adminMode[%d] pCfg->adminMode[%d] authState[%d]\r\n",
                     __FUNCTION__,intIfNum, voiceVlanCfg->voiceVlanAdminMode, pCfg->adminMode, voiceVlanInfo->portData[operIndex].authState);
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Voice VLAN Feature Disabled!\r\n", __FUNCTION__);
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return rc;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, len);

  /*if (L7_FALSE != voiceVlanAuthDebug)
    {*/
  /*sysapiPrintf("CDP Packet Dump:[%s]\r\n", (voiceVlanPacketDump(data, len) == L7_SUCCESS) ? "SUCCESS" : "EMPTY");*/
  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"CDP Packet Dump:[%s]\r\n", (voiceVlanPacketDump(data, len) == L7_SUCCESS) ? "SUCCESS" : "EMPTY");
  /*}*/

  /* Parse frame for VlanId */
  pEtype = data + L7_ENET_HDR_SIZE;
  memcpy((L7_uchar8 *)&temp16, pEtype, sizeof(L7_ushort16));
  protocol_type = osapiNtohs(temp16);

  if (protocol_type == L7_ETYPE_8021Q)
  {
    vlanTagPtr = (L7_8021QTag_t *)(data + L7_ENET_HDR_SIZE);
    vlanId = vlanTagPtr->tci & L7_VLAN_TAG_VLAN_ID_MASK;
    offset = offset + L7_8021Q_ENCAPS_HDR_SIZE;
  }
  else
  {
    dot1qQportsVIDGet(intIfNum, L7_DOT1Q_DEFAULT_VID_INDEX, &vlanId);
  }

  if ((vlanId < L7_DOT1Q_MIN_VLAN_ID) ||
      (vlanId > L7_MAX_VLAN_ID))
  {
    LOG_MSG("%s(): Invalid VLANID %d\r\n", __FUNCTION__, vlanId);
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Invalid VLANID %d\r\n", __FUNCTION__, vlanId);
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return rc;
  }
  ethHeaderSize = sysNetDataOffsetGet(data);

  /* Check if the frame is LLC SNAP Packet */
  if (ethHeaderSize != (L7_ENET_HDR_SIZE + offset + L7_802_ENCAPS_HDR_SIZE))
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Not a LLC SNAP Packet\r\n", __FUNCTION__);
    LOG_MSG("%s(): Not a LLC SNAP Packet\r\n", __FUNCTION__);
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return rc;
  }

  encap = (L7_enet_encaps_t *)(data + (L7_ENET_HDR_SIZE + offset + L7_802_ENCAPS_HDR_SIZE - L7_ENET_HDR_TYPE_LEN_SIZE));

  protocol_type = osapiNtohs(encap->type);
  if ((memcmp(data, voiceVlanCdpDstMacAddr.addr, L7_MAC_ADDR_LEN) != 0) ||
      (protocol_type != L7_ETYPE_CDP))
  {
    LOG_MSG("%s(): EtherType Not a CDP Type\r\n");
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): EtherType Not a CDP Type\r\n", __FUNCTION__);
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return rc;
  }

  memcpy(srcMac, (data + L7_MAC_ADDR_LEN), sizeof(srcMac));

  osapiSemaGive(voiceVlanInfo->voiceVlanLock);

  if ((node = voiceVlanDeviceInfoGet(srcMac))!=L7_NULLPTR)
  {
    /*device already exists*/
    SYSAPI_NET_MBUF_FREE(bufHandle);
    rc = L7_SUCCESS;
  }
  else
  {
    /* if the port status is set to disable , set it to enable*/
    if (voiceVlanInfo->portData[operIndex].portStatus != L7_ENABLE)
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s(): Setting port status from %d to %d for Intf[%d] \r\n",
                       __FUNCTION__,voiceVlanInfo->portData[operIndex].portStatus,L7_ENABLE, intIfNum);
      voiceVlanPortStatusSet(intIfNum,L7_ENABLE);
    }
    /* add device info to database */
    if ((rc =voiceVlanPortVoipDeviceAdd(intIfNum,srcMac, L7_TRUE))!= L7_SUCCESS)
    {
      l7utilsMacAddrHexToString(srcMac, 20, macStr);
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Could not add device details to the voice vlan database for Iface[%d] and Mac [%s]\r\n",
                       __FUNCTION__, intIfNum,macStr);
      rc = L7_FAILURE;
    }
    else
    {
      rc = L7_SUCCESS;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Process DHCP Packet and Discard a unwanted packet not useful
*           for Voice VLAN Auth
*
* @param    hookId        The hook location
* @param    bufHandle     Handle to the frame to be processed
* @param    *pduInfo      Pointer to info about this frame
* @param    continueFunc  Optional pointer to a continue function
*
* @returns  SYSNET_PDU_RC_COPIED     if frame has been copied.
* @returns  SYSNET_PDU_RC_IGNORED    if frame has been ignored.
*
* @notes    Verify the DHCP packet is destined with the option 176
*
* @end
*********************************************************************/
SYSNET_PDU_RC_t voiceVlanDhcpPduReceiveCallback(L7_uint32 hookId,
                                                L7_netBufHandle bufHandle,
                                                sysnet_pdu_info_t *pduInfo,
                                                L7_FUNCPTR_t continueFunc)
{
  L7_uchar8          *data;
  L7_ipHeader_t      *ipHeader;
  L7_udp_header_t    *udpHeader;
  L7_dhcpsPacket_t   *pDhcpPacket;

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);

  ipHeader = (L7_ipHeader_t *)(data + sysNetDataOffsetGet(data));

  if ((ipHeader->iph_versLen == ((L7_IP_VERSION<<4) | L7_IP_HDR_VER_LEN)) &&
      (ipHeader->iph_prot == IP_PROT_UDP))
  {
    udpHeader = (L7_udp_header_t *)((L7_char8 *)ipHeader + sizeof(L7_ipHeader_t));
    if (osapiNtohs(udpHeader->destPort) == L7_DHCPS_SERVER_PORT)
    {
      /* Parse for DHCP BOOT Request packet */
      pDhcpPacket = (L7_dhcpsPacket_t *)((L7_char8 *)udpHeader + sizeof(L7_udp_header_t));
      if ((pDhcpPacket->header.op) == L7_DHCP_BOOTP_REQUEST)
      {
        if (voiceVlanDhcpPacketHandle(bufHandle, pduInfo) == L7_SUCCESS)
        {
          return SYSNET_PDU_RC_COPIED;
        }
      }
    }
  }

  return SYSNET_PDU_RC_IGNORED;
}

/***********************************************************************
* @purpose Function to handle incoming dhcp packets
*
* @param   netBufHandle  @b{(input)}  Buffer handle to the dhcp
*                                     frame received
* @param   pduInfo       @b{(input)}  Pointer to PDU info
*
* @returns L7_SUCCESS  Packet copied into local buffer
* @returns L7_FAILURE  Packet ignored
*
* @notes none
*
* @end
*
***********************************************************************/
static L7_RC_t voiceVlanDhcpPacketHandle(L7_netBufHandle bufHandle, sysnet_pdu_info_t *pduInfo)
{
  L7_ipHeader_t      *ipHeader;
  L7_udp_header_t    *udpHeader;
  L7_uchar8          *data;
  L7_uint32          ethHeaderSize, len, ipLen;
  L7_uint32          operIndex = 0;
  L7_dhcpsPacket_t   *pDhcpPacket;
  L7_uchar8          *pEtype;
  L7_ushort16        protocol_type;
  L7_uint32          dhcpLen;
  L7_ushort16        temp16;
  voiceVlanPortCfg_t *pCfg;
  L7_8021QTag_t      *vlanTagPtr;
  L7_uint32          offset = 0;
  L7_uint32          msgOptionsLength;
  L7_BOOL            isDhcpIpPhoneType = L7_FALSE;
  L7_uchar8          *pOptionBuf;
  L7_dhcp_option_t   *pOption;
  L7_dhcp_option_t   *pParameterList = L7_NULLPTR;
  L7_uchar8          srcMac[L7_MAC_ADDR_LEN];
  L7_uint32          optionLen = 0;
  L7_uint32          vlanId;
  L7_uint32          i;
  L7_RC_t            rc = L7_FAILURE;
  voiceVlanDeviceInfo_t *node = L7_NULLPTR;
  L7_uint32           intIfNum=0;
  L7_uchar8           macStr[20];

  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "--> %s(): ENTER\r\n", __FUNCTION__);

  if (nimCheckIfNumber(pduInfo->intIfNum) != L7_SUCCESS)
  {
    /* Interface does not exists. Discard the packet */
    return rc;
  }

  if (voiceVlanIntfIsConfigurable(pduInfo->intIfNum, &pCfg) != L7_TRUE)
  {
    return rc;
  }

  operIndex = voiceVlanGetMapTblIndex(pduInfo->intIfNum);
  if (operIndex == 0)
  {
    return rc;
  }

  intIfNum = pduInfo->intIfNum;
  if (osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return rc;
  }

  if ((voiceVlanCfg->voiceVlanAdminMode != L7_ENABLE) ||
      (pCfg->adminMode != L7_ENABLE) ||
      (voiceVlanInfo->portData[operIndex].authState != L7_DISABLE))
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s(): adminMode[%d] pCfg->adminMode[%d] authState[%d]\r\n",
                     __FUNCTION__, voiceVlanCfg->voiceVlanAdminMode, pCfg->adminMode, voiceVlanInfo->portData[operIndex].authState);
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Intf %d Voice VLAN Feature Disabled!\r\n",
                     __FUNCTION__, pduInfo->intIfNum);
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return rc;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, len);

  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "DHCP Packet Dump:[%s]\r\n",
                   (voiceVlanPacketDump(data, len) == L7_SUCCESS) ? "SUCCESS":  "EMPTY");

  pEtype = data + L7_ENET_HDR_SIZE;
  memcpy((L7_uchar8 *)&temp16, pEtype, sizeof(L7_ushort16));
  protocol_type = osapiNtohs(temp16);

  if (protocol_type == L7_ETYPE_8021Q)
  {
    vlanTagPtr = (L7_8021QTag_t *)(data + L7_ENET_HDR_SIZE);
    vlanId = vlanTagPtr->tci & L7_VLAN_TAG_VLAN_ID_MASK;
    offset = offset + L7_8021Q_ENCAPS_HDR_SIZE;
  }
  else
  {
    dot1qQportsVIDGet(pduInfo->intIfNum, L7_DOT1Q_DEFAULT_VID_INDEX, &vlanId);
  }

  if ((vlanId < L7_DOT1Q_MIN_VLAN_ID) ||
      (vlanId > L7_MAX_VLAN_ID))
  {
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return rc;
  }

  ethHeaderSize = sysNetDataOffsetGet(data);

  ipHeader = (L7_ipHeader_t *)(data + ethHeaderSize);
  ipLen = len - ethHeaderSize;

  if ((ipHeader->iph_versLen == ((L7_IP_VERSION<<4) | L7_IP_HDR_VER_LEN)) &&
      (ipHeader->iph_prot == IP_PROT_UDP))
  {
    if (((osapiNtohl(ipHeader->iph_src) & L7_CLASS_D_ADDR_NETWORK) == L7_CLASS_D_ADDR_NETWORK) ||
        ((osapiNtohl(ipHeader->iph_src) & L7_CLASS_E_ADDR_NETWORK) == L7_CLASS_E_ADDR_NETWORK))
    {
      /* Illegal src IP in BOOTP/DHCP frame... tell the caller we've processed it */
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Illegal Src IP\r\n", __FUNCTION__);
      osapiSemaGive(voiceVlanInfo->voiceVlanLock);
      return rc;
    }

    udpHeader = (L7_udp_header_t *)((L7_char8 *)ipHeader + sizeof(L7_ipHeader_t));
    if (osapiNtohs(udpHeader->sourcePort) != L7_DHCPS_CLIENT_PORT)
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): UDP SrcPort:[%d] Unknown\r\n",
                       __FUNCTION__, osapiNtohs(udpHeader->sourcePort));
      osapiSemaGive(voiceVlanInfo->voiceVlanLock);
      return rc;
    }

    if (osapiNtohs(udpHeader->destPort) != L7_DHCPS_SERVER_PORT)
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): UDP DestPort:[%d] Unknown\r\n",
                       __FUNCTION__, osapiNtohs(udpHeader->destPort));
      osapiSemaGive(voiceVlanInfo->voiceVlanLock);
      return rc;
    }

    /* Parse DHCP packet for Option 176 */
    pDhcpPacket = (L7_dhcpsPacket_t *)((L7_char8 *)udpHeader + sizeof(L7_udp_header_t));
    dhcpLen = ipLen - (sizeof(L7_ipHeader_t) + sizeof(L7_udp_header_t));
    if (dhcpLen > L7_DHCP_PACKET_LEN)
    {
      dhcpLen = L7_DHCP_PACKET_LEN;
    }

    if ((pDhcpPacket->header.op) != L7_DHCP_BOOTP_REQUEST)
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): processDhcpsPacket discarded Bootp reply\r\n", __FUNCTION__);
      osapiSemaGive(voiceVlanInfo->voiceVlanLock);
      return rc;
    }

    msgOptionsLength = dhcpLen - sizeof(L7_dhcp_packet_t) - L7_DHCP_MAGIC_COOKIE_LEN;

    pOptionBuf = pDhcpPacket->options;
    pOption    = (L7_dhcp_option_t *)pOptionBuf;
    optionLen  = pOption->length + 2; /* 2 for Type and length bytes */

    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s: DHCP Packet Options Types of length[%d]:\r\n", __FUNCTION__, msgOptionsLength);

    while (msgOptionsLength > 0)
    {

      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"- %d\r\n", pOption->type);

      switch (pOption->type)
      {
        case L7_DHCP_OPT_PAD:

          VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s: Option Type PAD\r\n", __FUNCTION__);
          optionLen = 1;
          break;

        case L7_DHCP_OPT_DHCP_MSG_TYPE:
          if ((pOption->value[0] < L7_DHCPS_MESSAGE_TYPE_START) || (pOption->value[0] > L7_DHCPS_MESSAGE_TYPE_END))
          {
            VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s: DHCP packet found malformed msg with Erroneous MessageType:%d\r\n",
                             __FUNCTION__, pOption->value[0]);
            osapiSemaGive(voiceVlanInfo->voiceVlanLock);
            return L7_FAILURE;
          }

          VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s: DHCP recd msg with MessageType:%d\r\n", pOption->value[0]);
          break;

        case L7_DHCP_OPT_END:

          VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s: END option, remaining extra bytes %d\r\n", __FUNCTION__, msgOptionsLength);
          optionLen        = 1;
          msgOptionsLength = 0;
          break;

        case L7_DHCP_OPT_PARAMETER_REQLIST:
          VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s: Option Parameter List\r\n", __FUNCTION__);
          pParameterList = (L7_dhcp_option_t *)&(pOption->type);
          break;

        case L7_DHCP_OPT_IP_PHONE:

          VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s: DHCP Packet is of option 176\r\n", __FUNCTION__);
          isDhcpIpPhoneType = L7_TRUE;
          break;

        default:
          break;
      }

      if (msgOptionsLength == 0)
      {
        break;
      }

      msgOptionsLength -= optionLen;
      pOptionBuf       += optionLen;
      pOption           = (L7_dhcp_option_t *)pOptionBuf;
      optionLen         = pOption->length + 2; /* 2 for Type and length bytes */
    }
  }
  else
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s: Not an IP Packet!\r\n", __FUNCTION__);
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return rc;
  }

  if (pParameterList != L7_NULLPTR)
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%s: DHCP Packet Parameter List of length[%d]:\r\n", __FUNCTION__, pParameterList->length);

    for (i = 0; i < pParameterList->length; i++)
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"%d. %d\r\n", i, pParameterList->value[i]);

      if (pParameterList->value[i] == L7_DHCP_OPT_IP_PHONE)
      {
        isDhcpIpPhoneType = L7_TRUE;
      }
    }
  }

  if (isDhcpIpPhoneType == L7_FALSE)
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s: DHCP Packet does NOT have option 176\r\n", __FUNCTION__);
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return rc;
  }

  memcpy(srcMac, data + L7_MAC_ADDR_LEN, L7_MAC_ADDR_LEN);
  osapiSemaGive(voiceVlanInfo->voiceVlanLock);

  if ((node = voiceVlanDeviceInfoGet(srcMac))!= L7_NULLPTR)
  {
    /*device already exists in the database*/
    rc = L7_SUCCESS;
  }
  else
  {
    /* if the port status is set to disable , set it to enable*/
    if (voiceVlanInfo->portData[operIndex].portStatus != L7_ENABLE)
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s(): Setting port status from %d to %d for Intf[%d] \r\n",
                       __FUNCTION__,voiceVlanInfo->portData[operIndex].portStatus,L7_ENABLE, intIfNum);
      voiceVlanPortStatusSet(intIfNum,L7_ENABLE);
    }
    /* add device info to database */
    if ((rc =voiceVlanPortVoipDeviceAdd(intIfNum,srcMac, L7_TRUE))!= L7_SUCCESS)
    {
      l7utilsMacAddrHexToString(srcMac, 20, macStr);
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Could not add device details to the voice vlan database for Iface[%d] and Mac [%s]\r\n",
                       __FUNCTION__, intIfNum,macStr);
      rc = L7_FAILURE;
    }
    else
    {
      rc = L7_SUCCESS;
    }
  }

  return rc;
}

/*******************************************************************************
* @purpose  Process incoming LLDP PDU
*
* @param    bufHandle          buffer  @b((input)) handle to the lldpdu received
* @param    sysnet_pdu_info_t *pduInfo @b((input)) pointer to pdu info structure
*
* @returns  L7_SUCCESS  if the message was put on the queue
* @returns  L7_FAILURE  if the message was not put on the queue
*
* @notes    none
*
* @end
********************************************************************************/
L7_RC_t voiceVlanAuthLLDPduRcvCallback(L7_netBufHandle bufHandle,
                                       sysnet_pdu_info_t *pduInfo)
{
  L7_uchar8          *data = L7_NULLPTR;
  L7_enet_encaps_t   *encap = L7_NULLPTR;
  voiceVlanPortCfg_t *pCfg;
  L7_uint32          operIndex = 0;
  L7_uchar8          srcMac[L7_MAC_ADDR_LEN];
  L7_ushort16        etherType = 0;
  L7_uint32          ethHeaderSize;
  L7_uint32          len = 0;
  L7_RC_t            rc = L7_FAILURE;
  L7_uint32          intIfNum = pduInfo->intIfNum;

  L7_uint32          portControl = 0;
  L7_uint32          adminMode = 0;
  voiceVlanDeviceInfo_t *node = L7_NULLPTR;
  L7_uchar8           macStr[20];

  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s(): LLDP Pkt Received on Intf[%d]\n\r", __FUNCTION__, intIfNum);

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Invalid Interface[%d]\r\n", __FUNCTION__, intIfNum);
    return rc;
  }

  if (!(VOICE_VLAN_READY))
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Component is not Ready .. Invalid state[%d]\r\n",
                     __FUNCTION__, voiceVlanPortCnfgrState);
    return rc;
  }

  if (voiceVlanIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    return rc;
  }

  operIndex = voiceVlanGetMapTblIndex(intIfNum);
  if (operIndex == 0)
  {
    return rc;
  }

  if ((voiceVlanCfg->voiceVlanAdminMode != L7_ENABLE) ||
      (pCfg->adminMode != L7_ENABLE) ||
      (voiceVlanInfo->portData[operIndex].authState != L7_DISABLE))
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s(): adminMode[%d] pCfg->adminMode[%d] authState[%d]\r\n",
                     __FUNCTION__, voiceVlanCfg->voiceVlanAdminMode, pCfg->adminMode, voiceVlanInfo->portData[operIndex].authState);
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Voice VLAN Feature Disabled on Intf[%d]\r\n", __FUNCTION__, intIfNum);
    return rc;
  }

  /* Don't Allow, if the port is Authorized */
  dot1xAdminModeGet(&adminMode);
  if (adminMode == L7_DISABLE)
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Returns, Dot1x AdminMode not Enabled!\n\r", __FUNCTION__);
    return rc;
  }

  dot1xPortControlModeGet(intIfNum, &portControl);
  if (portControl != L7_DOT1X_PORT_AUTO_MAC_BASED)
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Returns, Dot1x Port[%d] Control status is not MAC-BASED or Auto!\n\r",
                     __FUNCTION__, intIfNum);
    return rc;
  }

  if (osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return rc;
  }

  if (L7_DISABLE == voiceVlanInfo->portData[operIndex].lldpPduAuthFlag)
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): VoiceVlanAuthLLDP PDU Disabled on Intf[%d]\r\n", __FUNCTION__, intIfNum);
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return rc;
  }

  SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, data);
  SYSAPI_NET_MBUF_GET_DATALENGTH(bufHandle, len);

  VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG,"LLDP Packet Dump:[%s]\r\n", (voiceVlanPacketDump(data, len) == L7_SUCCESS) ? "SUCCESS" : "EMPTY");

  ethHeaderSize = sysNetDataOffsetGet(data);
  encap = (L7_enet_encaps_t *)(data + L7_ENET_HDR_SIZE);

  etherType = osapiNtohs(encap->type);
  if ((memcmp(data, L7_ENET_LLDP_MULTICAST_MAC_ADDR.addr, L7_MAC_ADDR_LEN) != 0) ||
      (etherType != L7_ETYPE_LLDP))
  {
    l7utilsMacAddrHexToString(data, 20, macStr);
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): Not a LLDP Packet!\r\n", __FUNCTION__);
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "  DstMac[%s] EtherType[0x%x] Not a LLDP Type\r\n",
                     macStr, etherType);
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
    return L7_FAILURE;
  }

  memcpy(srcMac, (data + L7_MAC_ADDR_LEN), sizeof(srcMac));

  if ((node = voiceVlanDeviceInfoGet(srcMac))!= L7_NULLPTR)
  {
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "voiceVLAN device MAC on Intf[%d] Already Exists\r\n", intIfNum);
    rc = L7_SUCCESS;
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);
  }
  else
  {
    osapiSemaGive(voiceVlanInfo->voiceVlanLock);

    /* if the port status is set to disable , set it to enable*/
    if (voiceVlanInfo->portData[operIndex].portStatus != L7_ENABLE)
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s(): Setting port status from %d to %d for Intf[%d] \r\n",
                       __FUNCTION__,voiceVlanInfo->portData[operIndex].portStatus,L7_ENABLE, intIfNum);
      voiceVlanPortStatusSet(intIfNum,L7_ENABLE);
    }

    l7utilsMacAddrHexToString(srcMac, 20, macStr);
    VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_DEBUG, "%s(): Calling voiceVlanPortVoipDeviceAdd for Intf[%d] Mac[%s]\r\n",
                     __FUNCTION__, intIfNum,macStr);
    rc = voiceVlanPortVoipDeviceAdd(intIfNum,srcMac,L7_TRUE);
    if (L7_SUCCESS == rc)
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_MEDIUM, "%s(): voiceVlanPortVoipDeviceAdd Success on Intf[%d] for  Mac [%s]\n\r",
                       __FUNCTION__, intIfNum,macStr);
      rc = L7_SUCCESS;
    }
    else
    {
      VOICE_VLAN_TRACE(VOICE_VLAN_TRACE_HIGH, "%s(): voiceVlanPortVoipDeviceAdd failed on Intf[%d] for Mac [%s]\r\n",
                       __FUNCTION__, intIfNum,macStr);

      rc = L7_FAILURE;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Dump the Voice Vlan Info
*
* @param    None
*
*
* @returns  None
*
* @comments none
*
* @end
*********************************************************************/
void voiceVlanInfoDump()
{
  L7_uint32 intf,flag;
  VOICE_VLAN_CONFIG_TYPE_t mode;
  L7_enetMacAddr_t mac_addr;
  voiceVlanDeviceInfo_t *node = L7_NULLPTR;
  L7_uchar8 macStr[20];

  /* take the lock */
  if(osapiSemaTake(voiceVlanInfo->voiceVlanLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return;

  for(intf = 0;intf != L7_VOICE_VLAN_INTF_MAX_COUNT;intf++)
  {
    if(voiceVlanPortAdminModeGet(intf,&mode) == L7_SUCCESS && mode != VOICE_VLAN_CONFIG_DISABLE)
    {
      sysapiPrintf("Internal Interface - %d \n",intf);
      sysapiPrintf("Voice Vlan Port status %d \n",voiceVlanInfo->portData[intf].portStatus);
      COMPONENT_NONZEROMASK(voiceVlanInfo->portData[intf].acquiredList,flag);
      sysapiPrintf("Voice Vlan Port Acquired list %d \n",flag);
      sysapiPrintf("Voice Mac reference count %d \n",voiceVlanInfo->portData[intf].referenceCount);

      sysapiPrintf("Voice Vlan Device Mac Address \n");
      sysapiPrintf("------------------------------\n");

      memset(mac_addr.addr,0,sizeof(L7_enetMacAddr_t));
      if ((node = voiceVlanDeviceInfoFirstGet(intf))!= L7_NULLPTR)
      {
        l7utilsMacAddrHexToString(node->voipMac, 20, macStr);
        sysapiPrintf("%s\n",macStr);
        memcpy(mac_addr.addr,node->voipMac,L7_MAC_ADDR_LEN);
      }
      while((node=voiceVlanDeviceInfoGetNextNode(intf,mac_addr.addr))!=L7_NULLPTR)
      {
        l7utilsMacAddrHexToString(node->voipMac, 20, macStr);
        sysapiPrintf("%s\n",macStr);
        memcpy(mac_addr.addr,node->voipMac,L7_MAC_ADDR_LEN);
        node = L7_NULLPTR;
      }
    }
  }

  osapiSemaGive(voiceVlanInfo->voiceVlanLock);
  return;
}

/*********************************************************************
* @purpose Trace voice vlan events
*
* @param   1 - enable tracing
*          0 - disable tracing
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void voiceVlanTraceEvent(L7_uint32 debug)
{
  voiceVlanTrace = debug;
}

/* debug functions */
void voiceVlanAddSample(int i)
{
  unsigned char temp[] ={0x00,0x02,0x02,0x05,0x06,0x04};
  if (voiceVlanPortVoipDeviceAdd(i,temp,L7_TRUE) != L7_SUCCESS)
  {
    sysapiPrintf("\r\n %s: Unable to add  trial voip device with mac_addr: %02X:%02X:%02X:%02X:%02X:%02X ",
                 __FUNCTION__,
                 temp[0],
                 temp[1],
                 temp[2],
                 temp[3],
                 temp[4],
                 temp[5]);
  }
}

/********************************************************
*
* @purpose     Display Voice VLAN Local FDB Table
*
* @param       none
*
* @returns     none
*
* @notes
*
* @end
*
********************************************************/
static L7_RC_t voiceVlanPacketDump(L7_uchar8 *data, L7_uint32 len)
{
  L7_uint32 i;
  L7_uint32 ct = 0;

  sysapiPrintf("\r\n----------------------------------------------------------------\r\n");
  sysapiPrintf("                        PACKET DUMP\r\n");
  sysapiPrintf("----------------------------------------------------------------\r\n");

  if (data == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  for (i = 0; i < len; i++)
  {
    if (ct == 22)
    {
      ct = 0;
      sysapiPrintf("\r\n");
    }
    sysapiPrintf("%02x ", data[i]);
    ct++;
  }
  sysapiPrintf("\r\n");
  sysapiPrintf("----------------------------------------------------------------\r\n");
  return(L7_SUCCESS);
}
