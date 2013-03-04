/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\application\layer2\pbvlan\pbVlan.h
*
* @purpose Protocol-Based Vlan Utilities File
*
* @component Protocol-Based VLAN
*
* @comments tba
*
* @create 01/07/2002
*
* @author rjindal
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_PBVLAN_H
#define INCLUDE_PBVLAN_H

#include "l7_common.h"
#include "comm_mask.h"
#include "pbvlan_api.h"
#include "dot1q_api.h"

#define PBVLAN_CFG_FILENAME  "pbVlan.cfg"
#define PBVLAN_CFG_VER_1      0x1
#define PBVLAN_CFG_VER_2      0x2
#define PBVLAN_CFG_VER_3      0x3
#define PBVLAN_CFG_VER_4      0x4
#define PBVLAN_CFG_VER_5      0x5
#define PBVLAN_CFG_VER_CURRENT  PBVLAN_CFG_VER_5

typedef struct
{
  L7_uint32     msg_id;
  union
  {
  }msg;
}pbVlanMgmtMsg_t;

/*--------------------------------------*/
/*  PBVLAN TASKS AND QUEUES             */
/*--------------------------------------*/
#define PBVLAN_QUEUE        "pbVlan_Queue"
#define PBVLAN_MSG_COUNT    L7_PBVLAN_MAX_NUM_GROUPS
#define PBVLAN_MSG_SIZE     sizeof(pbVlanMgmtMsg_t)/sizeof(L7_uint32)      

/*********************************************************************
*
* @enum     pbVlanProtocolIndex_t
*
* @purpose  List of all the configurable protocols in the 
*           Fastpath reference implementation.
*
* @comments None.         
*
* @end
*
*********************************************************************/
typedef enum
{
  PBVLAN_PROTOCOL_INDEX_IP = 0,              /* for IP traffic  */
  PBVLAN_PROTOCOL_INDEX_ARP,                 /* for ARP traffic */
  PBVLAN_PROTOCOL_INDEX_IPX                  /* for IPX traffic */
} pbVlanProtocolIndex_t;

typedef struct
{
  L7_char8        name[L7_PBVLAN_MAX_GROUP_NAME + 1];
  L7_BOOL         inUse;
  L7_uint32       groupID;
  L7_uint32       vlanid;
  L7_uint32       protocol[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32       type[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS]; /*indicates which type has been 
                                                              entered by the user 0:string 1:hex */
  NIM_INTF_MASK_t ports;
} pbVlanGroupInfo_t;

typedef struct
{
  L7_fileHdr_t        cfgHdr;
  L7_uint32           etherProtocol[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  L7_uint32           prototype[L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
  pbVlanGroupInfo_t group[L7_PBVLAN_MAX_NUM_GROUPS + 1];
  L7_uint32           checkSum;
} pbVlanCfgData_t;

/*********************************************************************
*
* @purpose  Build default pbVlan config data.
*
* @param    L7_uint32   ver   Software version of Config Data
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
void pbVlanBuildDefaultConfigData(L7_uint32 ver);

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
void pbvlanMigrateConfigData(L7_uint32 oldVer , L7_uint32 ver, L7_char8 *pCfgBuffer);

/*********************************************************************
*
* @purpose  Save pbVlan user config file to NVStore.
*
* @param    void
*
* @returns  L7_SUCCESS or
* @returns  L7_FAILURE
*
* @comments none 
*       
* @end
*
*********************************************************************/
L7_uint32 pbVlanSave(void);

/*********************************************************************
*
* @purpose  Check if pbVlan user config data has changed.
*
* @param    void
*
* @returns  L7_TRUE or
* @returns  L7_FALSE
*
* @comments none 
*       
* @end
*
*********************************************************************/
L7_BOOL pbVlanHasDataChanged(void);
void pbVlanResetDataChanged(void);
/*********************************************************************
*
* @purpose  Prints the Configuration Data for debugging
*
* @param    void
*
* @returns  L7_TRUE or
* @returns  L7_FALSE
*
* @comments none 
*       
* @end
*
*********************************************************************/
L7_RC_t pbVlanConfigDump(void);

/*********************************************************************
*
* @purpose  Create the pbVlan task
*          
* @param        void
*       
* @returns  L7_SUCCESS, if task created successfully
* @returns  L7_ERROR, if unsuccessful task creation
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanStartTask(void);

/*********************************************************************
*
* @purpose  PBVLAN Task    
*
* @param    void
*
* @returns  void
*
* @comments none
*       
* @end
*
*********************************************************************/
void pbVlanTask(void);

/*********************************************************************
*
* @purpose  To create all pbVlan groups that exist in the current
*           configuration.
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @comments none
*       
* @end
*
*********************************************************************/
L7_RC_t pbVlanApplyConfigData(void);

/*********************************************************************
*
* @purpose  To process link state changes.
*
* @param    L7_uint32  intIfNum  internal interface number
* @param    L7_uint32  event     event, defined by L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanIntfChangeCallback (L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator);

/*********************************************************************
* @purpose  Propogate Startup notifications
*
* @param    NIM_STARTUP_PHASE_t  startupPhase
*
* @notes
*
* @end
*********************************************************************/
void pbVlanStartupNotifyCallback(NIM_STARTUP_PHASE_t startupPhase);

/*********************************************************************
*
* @purpose  To process vlan changes.
*
* @param    L7_uint32  vlanid    id of the vlan
* @param    L7_uint32  intIfNum  internal interface number
* @param    L7_uint32  event     event, defined by L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*                                                   
* @end
*
*********************************************************************/
L7_RC_t pbVlanVLANChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event);

/*********************************************************************
*
* @purpose  To get the array index in the protocol array.
*
* @param    L7_uint32  prtl   @b((input)) specified protocol
* @param    L7_uint32  index  @b((output)) array index
*
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t pbVlanGroupPrtlIndexGet(L7_uint32 prtl, L7_uint32 *index);

#endif  /* PBVLAN_H */
