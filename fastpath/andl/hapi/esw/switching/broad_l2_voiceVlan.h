/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: broad_l2_dot1x.h
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
#include <commdefs.h>

#ifndef L7_VOICE_VLAN_MAX_DEVICE_COUNT
#define L7_VOICE_VLAN_MAX_DEVICE_COUNT 64
#endif

typedef struct
{
  L7_BOOL                     inUse;
  L7_enetMacAddr_t            macAddr;
  L7_ushort16                 vid;     /* for VLAN assignment */
  DAPI_USP_t                  usp;    /* pointer to the interface on which teh device was identified*/
} HAPI_VOICE_VLAN_DEVICE_t;

typedef struct
{
  BROAD_POLICY_t              voiceVlanPolicy;
  L7_BOOL                     voiceVlanUnauthMode;
  L7_uint32                   deviceCount;
} HAPI_VOICE_VLAN_PORT_t;



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
L7_RC_t hapiBroadVoiceVlanDeviceAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

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
L7_RC_t hapiBroadVoiceVlanDeviceRemove(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);


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
L7_RC_t hapiBroadVoiceVlanCosOverride(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
 *
 * @purpose To Enable/Disable Auth state on a voice vlan port.
 *
 * @param   DAPI_USP_t *usp    - needs to be a valid usp
 * @param   DAPI_CMD_t  cmd    - DAPI_CMD_VOICE_VLAN_AUTH
 * @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.voiceVlanUnauthSet
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
L7_RC_t hapiBroadVoiceVlanAuth(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

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
L7_RC_t hapiBroadVoiceVlanInit(DAPI_t *dapi_g);
