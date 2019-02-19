/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename dot1q_intf.h
*
* @purpose Interface management for vlans
*
* @component dot1q
*
* @comments none
*
* @create 05/22/2002
*
* @author skanchi
*
* @end
*             
**********************************************************************/
#ifndef DOT1Q_INTF_H__
#define DOT1Q_INTF_H__

#define DOT1Q_MAX_VLAN_INTF 		L7_MAX_NUM_VLAN_INTF

typedef enum {
  DOT1Q_VLAN_INTF_INVALID = 0,
  DOT1Q_VLAN_INTF_VALID,
  DOT1Q_VLAN_INTF_CREATE_PENDING,
  DOT1Q_VLAN_INTF_DELETE_PENDING,
  DOT1Q_VLAN_INTF_LAST_STATE
} DOT1Q_VLAN_INTF_STATE_t;

typedef enum
{
  DOT1Q_INVALID_INTF = 0,
  DOT1Q_VLAN_INTF,
  DOT1Q_PORT_INTF
} DOT1Q_INTF_TYPE_t;


typedef struct dot1qVlanIntf_s
{
  /* For VLAN routing interfaces, the VLAN ID. For port based routing interfaces,
   * a VLAN ID taken for internal use. */
  L7_uint32 vlanId;

  /* For VLAN routing interfaces, the internal interface number of the logical
   * interface as assigned by NIM. For port based routing interfaces, the 
   * internal interface number of the physical port. */
  L7_uint32 intIfNum;

  /* Whether the VLAN ID is used for a VLAN routing interface or 
   * a port based routing interface. */
  DOT1Q_INTF_TYPE_t intfType;

  /* Always DOT1Q_VLAN_INTF_VALID for port based routing interfaces */
  DOT1Q_VLAN_INTF_STATE_t intfState;

  L7_BOOL   inUse;

  /* Not used? */
  COMPONENT_MASK_t alwaysUpMask;

}dot1qVlanIntf_t;



/********************************************************************/
/*             FUNCTION PROTOTYPES                                  */
/********************************************************************/
void dot1qVlanIntfCreateInterfaces(void);
void dot1qVlanIntfDeleteInterfaces(void);
L7_RC_t dot1qVlanIntfCreate(L7_uint32 vlanId, L7_uint32 intfId, L7_uint32 *intIfNum);
L7_RC_t dot1qVlanIntfDelete(L7_uint32 vlanId);
L7_RC_t dot1qVlanIntfVlanChange(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, 
								L7_uint32 event);
L7_RC_t dot1qVlanIntfIntfChange(L7_uint32 intIfNum, L7_uint32 event);
L7_RC_t dot1qVlanIntfDeleteInNim(L7_uint32 intIfNum);
L7_RC_t dot1qVlanIntfCreateInNim(L7_uint32 vlanId, L7_uint32 *pIntIfNum);
void dot1qNimEventCompletionCallback(NIM_NOTIFY_CB_INFO_t retVal);
L7_RC_t dot1qVlanIntfIndexGet(L7_uint32 vlanId, L7_uint32 *pIndex);
L7_RC_t dot1qVlanIntfNextAvailableIndexGet(L7_uint32 *pIndex);
L7_RC_t dot1qVlanIntfIntIfNumIsValid(L7_uint32 intIfNum, L7_uint32 *pIndex);
L7_RC_t dot1qVlanIntfLinkStatus(L7_uint32 vlanId, L7_uint32 *linkStatus);

#endif

