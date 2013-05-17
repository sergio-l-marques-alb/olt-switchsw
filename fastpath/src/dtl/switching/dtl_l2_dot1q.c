/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_l2_dot1q.c
*
* Purpose: This file contains the functions to transform the vlan
*          components's driver requests.
*
* Component: Device Transformation Layer (DTL)
*
* Commnets:
*
* Created by: Shekhar Kalyanam 3/14/2001
*
*********************************************************************/
/*************************************************************

*************************************************************/





/*
**********************************************************************
*                           HEADER FILES
**********************************************************************
*/

#define DTLCTRL_L2_DOT1Q_GLOBALS              /* Enable global space   */
#include "dtlinclude.h"
#include "garpapi.h"
#include "garp_dot1q_api.h"

#if DTLCTRL_COMPONENT_L2_DOT1Q

static DAPI_USP_t opMS[L7_MAX_INTERFACE_COUNT+1];
static DAPI_USP_t opTS[L7_MAX_INTERFACE_COUNT+1];

static DAPI_USP_t opnMS[L7_MAX_INTERFACE_COUNT+1];
static DAPI_USP_t opnTS[L7_MAX_INTERFACE_COUNT+1];

/* Database of protocol based VLANs used by each interface.
*/
#ifdef L7_PBVLAN_PACKAGE
static L7_uint32 pbvlanTable[L7_MAX_INTERFACE_COUNT+1][L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS];
extern L7_RC_t pbVlanGroupPrtlIndexGet(L7_uint32 prtl, L7_uint32 *index);
#endif
/*
**********************************************************************
*                           PRIVATE FUNCTIONS PROTOTYPES
**********************************************************************
*/


/*
**********************************************************************
*                           API FUNCTIONS
**********************************************************************
*/
/*********************************************************************
* @purpose  Creates a VLAN with no members
*
* @param    fdbID       @b{(input)} Filtering DB ID
* @param    vlanID      @b{(input)} VLAN ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1qCreate(L7_ulong32 fdbID,
                       L7_ushort16 vlanID)
{

  DAPI_QVLAN_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  L7_RC_t rc;

  memset(&dapiCmd, 0, sizeof(dapiCmd));
  /* ddUsp is not used in this command */
  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.vlanCreate.vlanID = vlanID;

  dapiCmd.cmdData.vlanCreate.getOrSet = DAPI_CMD_SET;

  rc = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_VLAN_CREATE, &dapiCmd);
  if (rc == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}
/*********************************************************************
* @purpose  Creates VLANs with no members
*
* @param    vlanMask      @b{(input)} VLAN ID Mask
* @param    numVlans      @b{(input)} Num of vlans set in the mask
* @param    *vlanMaskFailure @b{(output)} Vlan mask of vlans that were not created
* @param    *vlanFailureCount @b{(output)} Num of vlan that were not created
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1qCreateMask(L7_VLAN_MASK_t *vlanMask, L7_uint32 numVlans, L7_VLAN_MASK_t* vlanMaskFailure, L7_uint32 *vlanFailureCount)
{

  DAPI_QVLAN_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  L7_RC_t rc;

  /* ddUsp is not used in this command */
  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  memcpy(&dapiCmd.cmdData.vlanCreateList.vlanMask, vlanMask, sizeof(L7_VLAN_MASK_t));
  dapiCmd.cmdData.vlanCreateList.numVlans = (L7_ushort16)numVlans;
  dapiCmd.cmdData.vlanCreateList.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.vlanCreateList.vlanFailureCount = vlanFailureCount;
  dapiCmd.cmdData.vlanCreateList.vlanMaskFailure = vlanMaskFailure;

  rc = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_VLAN_LIST_CREATE, &dapiCmd);
  if (rc == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}
/*********************************************************************
* @purpose  Deletes a VLAN and its members
*
* @param    fdbID       @b{(input)} Filtering DB ID
* @param    vlanID      @b{(input)} VLAN ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1qPurge(L7_ulong32 fdbID,
                      L7_ushort16 vlanID)
{

  DAPI_QVLAN_MGMT_CMD_t   dapiCmd;
  DAPI_USP_t              ddUsp;
  L7_RC_t           rc;

  memset(&dapiCmd, 0, sizeof(dapiCmd));
  /* ddUsp is not used in this command */
  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.vlanPurge.vlanID = vlanID;

  dapiCmd.cmdData.vlanPurge.getOrSet = DAPI_CMD_SET;

  rc = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_VLAN_PURGE, &dapiCmd);

  if (rc == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}
/*********************************************************************
* @purpose  Deletes VLANs and its members
*
* @param    vlanMask      @b{(input)} VLAN ID Mask
* @param    numVlans      @b{(input)} Num of vlans set in the mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1qPurgeMask(L7_VLAN_MASK_t *vlanMask, L7_uint32 numVlans)
{

  DAPI_QVLAN_MGMT_CMD_t   dapiCmd;
  DAPI_USP_t              ddUsp;
  L7_RC_t           rc;

  /* ddUsp is not used in this command */
  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  memcpy(&dapiCmd.cmdData.vlanPurgeList.vlanMask, vlanMask, sizeof(L7_VLAN_MASK_t));
  dapiCmd.cmdData.vlanPurgeList.numVlans = (L7_ushort16) numVlans;
  dapiCmd.cmdData.vlanPurgeList.getOrSet = DAPI_CMD_SET;

  rc = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_VLAN_LIST_PURGE, &dapiCmd);

  if (rc == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}
/*********************************************************************
* @purpose  Configures vlan membership and tagging for a port
*
* @param    fdbID       @b{(input)} Filtering DB ID
* @param    vlanID      @b{(input)} VLAN ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1qConfigInterface(L7_uint32 intIfNum,
                                L7_uchar8 *memberSet,
                                L7_uchar8 *taggedSet)
{

  DAPI_QVLAN_MGMT_CMD_t   dapiCmd;
  DAPI_USP_t              ddUsp;
  nimUSP_t usp;
  L7_RC_t           rc;

  memset(&dapiCmd, 0, sizeof(dapiCmd));
  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  else
  {

      ddUsp.unit = (L7_ushort16)usp.unit;
      ddUsp.slot = (L7_ushort16)usp.slot;
      ddUsp.port = (L7_ushort16)usp.port-1;

      dapiCmd.cmdData.portVlanListSet.vlanAddCmd = L7_TRUE;

      memset(&dapiCmd.cmdData.portVlanListSet.operationalMemberSet, 0x0,
                 sizeof(dapiCmd.cmdData.portVlanListSet.operationalMemberSet));
      memset(&dapiCmd.cmdData.portVlanListSet.operationalTagSet, 0x0,
                 sizeof(dapiCmd.cmdData.portVlanListSet.operationalTagSet));

      if (memberSet == L7_NULLPTR)
      {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

          /* If there is no new member then there is nothing for the driver to do */
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DTL_COMPONENT_ID,
                  "%s: Intf %s Member set is NULL \n",__FUNCTION__, ifName);
          return L7_SUCCESS;

      }

      memcpy(&dapiCmd.cmdData.portVlanListSet.operationalMemberSet, memberSet,
                 sizeof(dapiCmd.cmdData.portVlanListSet.operationalMemberSet));


      if (taggedSet != L7_NULLPTR)
      {
          memcpy(&dapiCmd.cmdData.portVlanListSet.operationalTagSet, taggedSet,
             sizeof(dapiCmd.cmdData.portVlanListSet.operationalTagSet));
      }

      dapiCmd.cmdData.portVlanListSet.getOrSet = DAPI_CMD_SET;
      rc = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_PORT_VLAN_LIST_SET, &dapiCmd);

  }

  if (rc == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Deletes a VLAN and its members
*
* @param    fdbID       @b{(input)} Filtering DB ID
* @param    vlanID      @b{(input)} VLAN ID
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1qUnConfigInterface(L7_uint32 intIfNum,
                                L7_uchar8 *memberSet)
{

  DAPI_QVLAN_MGMT_CMD_t   dapiCmd;
  DAPI_USP_t              ddUsp;
  nimUSP_t usp;
  L7_RC_t           rc;

  memset(&dapiCmd, 0, sizeof(dapiCmd));
  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
      return L7_FAILURE;
  }
  else
  {

      ddUsp.unit = (L7_ushort16)usp.unit;
      ddUsp.slot = (L7_ushort16)usp.slot;
      ddUsp.port = (L7_ushort16)usp.port-1;

      dapiCmd.cmdData.portVlanListSet.vlanAddCmd = L7_FALSE;

      if (memberSet == L7_NULLPTR )
      {
          return L7_FAILURE;
      }
      memcpy(&dapiCmd.cmdData.portVlanListSet.operationalMemberSet, memberSet,
             sizeof(dapiCmd.cmdData.portVlanListSet.operationalMemberSet));

      dapiCmd.cmdData.portVlanListSet.getOrSet = DAPI_CMD_SET;

      rc = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_PORT_VLAN_LIST_SET, &dapiCmd);
  }

  if (rc == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Adds port(s) to an existing VLAN
*
* @param     fdbID       @b{(input)} Filtering DB ID
* @param     vlanID      @b{(input)} VLAN ID
* @param     *MS_intf    @b{(input)} Memberset, list of interfaces to add
* @param     msNum       @b{(input)} Number of members in memberset
* @param     *TS_intf    @b{(input)} list of interfaces to be tagged
* @param     tsNum       @b{(input)} Number of members in taggedset
* @param     *GF_intf    @b{(input)} Groupfilter
* @param     gfNum       @b{(input)} Number of members in groupfilter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    The interfaces can be physical or logical
*
* @end
*********************************************************************/
L7_RC_t dtlDot1qAddPort(L7_ulong32 fdbID,
                        L7_ushort16 vlanID,
                        L7_ushort16 *MS_intf,
                        L7_uint32 msNum,
                        L7_ushort16 *TS_intf,
                        L7_uint32 tsNum,
                        L7_ushort16 *GF_intf,
                        L7_uint32 gfNum)

{

  DAPI_QVLAN_MGMT_CMD_t   dapiCmd;
  L7_uint32 i;
  DAPI_USP_t ddUsp;
  L7_RC_t rc = L7_SUCCESS;
  L7_RC_t lrc;
  nimUSP_t usp;

  memset(&dapiCmd, 0, sizeof(dapiCmd));
  /* ddUsp is not used in this command */
  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  /* for each interface passed down in the memberset */
  for (i=0;i<msNum;i++)
  {
    lrc = nimGetUnitSlotPort((L7_uint32 )MS_intf[i], &usp);
    opMS[i].unit = (L7_ushort16)usp.unit;
    opMS[i].slot = (L7_ushort16)usp.slot;
    opMS[i].port = (L7_ushort16)usp.port-1;
  }

  for (i=0;i<tsNum;i++)
  {
    lrc = nimGetUnitSlotPort((L7_uint32 )TS_intf[i], &usp);
#if 0
    if (usp.slot == L7_LIF_SLOT_NUM)
    {
      dapiCmdLif.cmdData.intfParmSet.lifUsp.unit = usp.unit;
      dapiCmdLif.cmdData.intfParmSet.lifUsp.slot = usp.slot;
      dapiCmdLif.cmdData.intfParmSet.lifUsp.port = usp.port-1;

      /* Set untagged Bit to FALSE */
      dapiCmdLif.cmdData.intfParmSet.enable = L7_FALSE;

      rc = dapiCtl(&ddUsp, DAPI_CMD_LIF_BM_INTF_PARM_SET, &dapiCmdLif);
      lifCount++;
    }
    else
    {
      opTS[i].unit = usp.unit;
      opTS[i].slot = usp.slot;
      opTS[i].port = usp.port-1;
    }
#else
  /* colinw: The above code has been disabled since the driver doesn't support
             the operation */
    opTS[i].unit = (L7_ushort16)usp.unit;
    opTS[i].slot = (L7_ushort16)usp.slot;
    opTS[i].port = (L7_ushort16)usp.port-1;
#endif
  }

  /* TBD: handle group filter */

  if (msNum != 0 || (tsNum) != 0)
  {
    dapiCmd.cmdData.vlanPortAdd.vlanID = (L7_ushort16)vlanID;

    dapiCmd.cmdData.vlanPortAdd.numOfMemberSetEntries = (L7_ushort16)msNum;
    dapiCmd.cmdData.vlanPortAdd.operationalMemberSet = opMS;

    dapiCmd.cmdData.vlanPortAdd.numOfTagSetEntries = (L7_ushort16)tsNum;
    dapiCmd.cmdData.vlanPortAdd.operationalTagSet = opTS;

    dapiCmd.cmdData.vlanPortAdd.getOrSet = DAPI_CMD_SET;

    rc = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_VLAN_PORT_ADD, &dapiCmd);
  }

  if (rc != L7_FAILURE)  /* could be a delayed success */
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Deletes port(s) from an existing VLAN
*
* @param    fdbID       @b{(input)} Filtering DB ID
* @param    vlanID      @b{(input)} VLAN ID
* @param    *nMS        @b{(input)} Memberset, list of interfaces to delete
* @param    nmsNum      @b{(input)} Number of members to delete
* @param    *nTS        @b{(input)} Taggedset, list of interfaces to be untagged
* @param    ntsNum      @b{(input)} Number of members to be flagged as untagged
* @param    *nGF        @b{(input)} Slot/ports not in groupfilter
* @param    ngfNum      @b{(input)} Number of slot/ports not in groupfilter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1qDeletePort(L7_ulong32 fdbID,
                           L7_ushort16 vlanID,
                           L7_ushort16 *nMS_intf,
                           L7_uint32 nmsNum,
                           L7_ushort16 *nTS_intf,
                           L7_uint32 ntsNum,
                           L7_ushort16 *nGF_intf,
                           L7_uint32 ngfNum)
{
  DAPI_QVLAN_MGMT_CMD_t dapiCmd;
  L7_uint32 i;
  DAPI_USP_t ddUsp;
  L7_RC_t rc = L7_SUCCESS;
  nimUSP_t usp;
  L7_RC_t lrc;

  memset(&dapiCmd, 0, sizeof(dapiCmd));
  /* ddUsp is not used in this command */
  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  for (i=0;i<nmsNum;i++)
  {
    lrc = nimGetUnitSlotPort((L7_uint32 )nMS_intf[i], &usp);
    opnMS[i].unit = (L7_ushort16)usp.unit;
    opnMS[i].slot = (L7_ushort16)usp.slot;
    opnMS[i].port = (L7_ushort16)usp.port-1;
  }

  for (i=0;i<ntsNum;i++)
  {
    lrc = nimGetUnitSlotPort((L7_uint32 )nTS_intf[i], &usp);
#if 0
    if (usp.slot == L7_LIF_SLOT_NUM)
    {
      dapiCmdLif.cmdData.intfParmSet.lifUsp.unit = usp.unit;
      dapiCmdLif.cmdData.intfParmSet.lifUsp.slot = usp.slot;
      dapiCmdLif.cmdData.intfParmSet.lifUsp.port = usp.port-1;

      /* Set untagged Bit to TRUE */
      dapiCmdLif.cmdData.intfParmSet.enable = L7_TRUE;

      rc = dapiCtl(&ddUsp, DAPI_CMD_LIF_BM_INTF_PARM_SET, &dapiCmdLif);
      lifCount++;
    }
    else
    {
      opnTS[i].unit = usp.unit;
      opnTS[i].slot = usp.slot;
      opnTS[i].port = usp.port-1;
    }

#else
  /* colinw: The above code has been disabled since the driver doesn't support
             the operation */
    opnTS[i].unit = (L7_ushort16)usp.unit;
    opnTS[i].slot = (L7_ushort16)usp.slot;
    opnTS[i].port = (L7_ushort16)usp.port-1;
#endif
  }

  /* TBD: handle group filter */
  if (nmsNum != 0 || (ntsNum) != 0)
  {
    dapiCmd.cmdData.vlanPortDelete.vlanID = (L7_ushort16)vlanID;

    dapiCmd.cmdData.vlanPortDelete.numOfMemberSetEntries = (L7_ushort16)nmsNum;
    dapiCmd.cmdData.vlanPortDelete.operationalMemberSet = opnMS;

    dapiCmd.cmdData.vlanPortDelete.numOfTagSetEntries = (L7_ushort16)ntsNum;
    dapiCmd.cmdData.vlanPortDelete.operationalTagSet = opnTS;

    dapiCmd.cmdData.vlanPortDelete.getOrSet = DAPI_CMD_SET;

    rc = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_VLAN_PORT_DELETE, &dapiCmd);
  }

  if (rc != L7_FAILURE)  /* could be a delayed success */
    return L7_SUCCESS;
  else
    return L7_SUCCESS;


}


/*********************************************************************
* @purpose  Enables GVRP for the interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1qGVRPEnable(L7_uint32 intIfNum)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_GARP_MGMT_CMD_t dapiCmd;
  L7_RC_t dr;


  memset(&dapiCmd, 0, sizeof(dapiCmd));
  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {

    return L7_FAILURE;
  }

  else
  {

    ddUsp.unit = (L7_ushort16)usp.unit;
    ddUsp.slot = (L7_ushort16)usp.slot;
    ddUsp.port = (L7_ushort16)usp.port-1;

    dapiCmd.cmdData.portGvrp.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.portGvrp.enable = L7_TRUE;


    dr = dapiCtl(&ddUsp,DAPI_CMD_GARP_GVRP,&dapiCmd);
    if (dr == L7_SUCCESS)
      return L7_SUCCESS;
    else
      return L7_FAILURE;
  }


}
/*********************************************************************
* @purpose  Disables GVRP for the interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1qGVRPDisable(L7_uint32 intIfNum)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  DAPI_GARP_MGMT_CMD_t dapiCmd;
  L7_RC_t dr;


  memset(&dapiCmd, 0, sizeof(dapiCmd));
  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {

    return L7_FAILURE;
  }

  else
  {

    ddUsp.unit = (L7_ushort16)usp.unit;
    ddUsp.slot = (L7_ushort16)usp.slot;
    ddUsp.port = (L7_ushort16)usp.port-1;

    dapiCmd.cmdData.portGvrp.getOrSet = DAPI_CMD_SET;
    dapiCmd.cmdData.portGvrp.enable = L7_FALSE;


    dr = dapiCtl(&ddUsp,DAPI_CMD_GARP_GVRP,&dapiCmd);
    if (dr == L7_SUCCESS)
      return L7_SUCCESS;
    else
      return L7_FAILURE;

  }


}

/*********************************************************************
* @purpose  Configures GVRP / GMRP globally
*
* @param    appl        @b{(input)} Application to configure
*                       @b{{GARP_GVRP_APP
*                           GARP_GMRP_APP}}
*
* @param    mode        @b{(input)} Enable/Disable
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1qGvrpGmrpConfig(L7_uint32 appl, L7_BOOL mode)
{
  DAPI_USP_t            ddUsp;
  DAPI_GARP_MGMT_CMD_t  dapiCmd;
  L7_RC_t         dr;

  memset(&dapiCmd, 0, sizeof(dapiCmd));
  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dapiCmd.cmdData.gvrpGmrpConfig.getOrSet = DAPI_CMD_SET;
  dapiCmd.cmdData.gvrpGmrpConfig.gmrp     = L7_FALSE;
  dapiCmd.cmdData.gvrpGmrpConfig.gvrp     = L7_FALSE;

  if (appl == GARP_GVRP_APP)
  {
    dapiCmd.cmdData.gvrpGmrpConfig.gvrp  = L7_TRUE;
  }
  else if (appl == GARP_GMRP_APP)
  {
    dapiCmd.cmdData.gvrpGmrpConfig.gmrp  = L7_TRUE;
  }
  else
  {
    return L7_FAILURE;
  }

  if (mode == L7_TRUE)
  {
    dapiCmd.cmdData.gvrpGmrpConfig.enable = L7_TRUE;
  }
  else
  {
    dapiCmd.cmdData.gvrpGmrpConfig.enable = L7_FALSE;
  }

  dr = dapiCtl(&ddUsp,DAPI_CMD_GVRP_GMRP_CONFIG,&dapiCmd);

  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}

/*********************************************************************
* @purpose  Configures PVID, acceptable frame type and ingress filtering
*
* @param    intIfNum       @b{(input)} Internal Interface Number to config
* @param    parameterMask  @b{(input)} Which is being modified
*                                      bit 0 - VID
*                                      bit 1 - ingress filter mode
*                                      bit 2 - acceptable frame types
*                                      bit 3 - dot1q mode
* @param    pvid           @b{(input)} Port VLAN ID
* @param    type           @b{(input)} Acceptable frame type
*                          L7_DOT1Q_ADMIT_ALL, L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED
* @param    filter         @b{(input)} Ingress filtering
*                          L7_ENABLE, L7_DISABLE
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1qPortConfig(L7_uint32 intIfNum,
                           DTL_L2_DOT1Q_PORT_CONFIG_PARAMETER_MASK_t parameterMask,
                           L7_uint32 pvid,
                           L7_uint32 type,
                           L7_uint32 filter)
{
  DAPI_QVLAN_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  L7_RC_t rc;
  nimUSP_t usp;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;

  ddUsp.unit = (L7_ushort16)usp.unit;
  ddUsp.slot = (L7_ushort16)usp.slot;
  ddUsp.port = (L7_ushort16)usp.port-1;

  bzero((L7_uchar8*)&dapiCmd,sizeof(DAPI_QVLAN_MGMT_CMD_t));

  if (parameterMask & DTL_L2_DOT1Q_PORT_CONFIG_ACCEPT_FRAME_TYPE)
  {
    dapiCmd.cmdData.portConfig.acceptFrameType = type;
    dapiCmd.cmdData.portConfig.acceptFrameTypeModify = L7_TRUE;
  }

  if (parameterMask & DTL_L2_DOT1Q_PORT_CONFIG_VID_SET)
  {
    dapiCmd.cmdData.portConfig.vlanID = (L7_ushort16)pvid;
    dapiCmd.cmdData.portConfig.vlanIDModify = L7_TRUE;
  }

  if (parameterMask & DTL_L2_DOT1Q_PORT_CONFIG_INGRESS_FILTER)
  {
    if (filter == L7_ENABLE)
      dapiCmd.cmdData.portConfig.ingressFilteringEnabled = L7_TRUE;
    else
      dapiCmd.cmdData.portConfig.ingressFilteringEnabled = L7_FALSE;
    dapiCmd.cmdData.portConfig.ingressFilteringEnabledModify = L7_TRUE;
  }

  dapiCmd.cmdData.portConfig.getOrSet = DAPI_CMD_SET;

  rc = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_PORT_CONFIG, &dapiCmd);

  if (rc != L7_FAILURE)  /* could be a delayed success */
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}


#ifdef L7_PBVLAN_PACKAGE
/*********************************************************************
* @purpose  Configures PVID, based on the protocol
*
* @param    intIfNum    @b{(input)} Internal Interface Number to config
* @param    *protocol   @b{(input)} Protocol to add or delete.
* @param    vlanid      @b{(input)} VLAN ID
* @param    pbvlan_cmd  @b{(input)} L7_TRUE - Add protocol
*                 L7_FALSE - Delete protocol
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPbVlanPortConfig(L7_uint32 intIfNum,
                            L7_uint32 prtl,
                            L7_uint32 vlanid,
                            L7_BOOL   pbvlan_cmd)
{
  DAPI_QVLAN_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  L7_RC_t rc;
  nimUSP_t usp;
  L7_uint32 i, j;
  L7_RC_t prc;
  L7_uint32 index;

  static L7_BOOL first_time = L7_TRUE;


  memset(&dapiCmd, 0, sizeof(dapiCmd));
  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Clear out all command and data fields in the DAPI command.
  */
  bzero((L7_uchar8*)&dapiCmd,sizeof(DAPI_QVLAN_MGMT_CMD_t));

  /* If executing this code first time then invalidate the pbvlanTable.
  */
  if (first_time == L7_TRUE)
  {
    first_time = L7_FALSE;
    for (i = 0; i <= (L7_uint32)L7_MAX_INTERFACE_COUNT; i++)
    {
      for (j = 0; j < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; j++)
      {
        pbvlanTable[i][j] = L7_PBVLAN_NULL_VLANID;
      }
    }
  }

  ddUsp.unit = (L7_ushort16)usp.unit;
  ddUsp.slot = (L7_ushort16)usp.slot;
  ddUsp.port = (L7_ushort16)usp.port - 1;

  dapiCmd.cmdData.portConfig.getOrSet = DAPI_CMD_SET;

  prc = pbVlanGroupPrtlIndexGet(prtl, &index);

  if (prc != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* Merge the protocols configured for this group with protocols
  ** that are already configured for other groups.
  */
  if (pbvlan_cmd == L7_TRUE)
  {
    /* We are adding a new protocol or replacing a VLAN ID for existing protocol.
    */
    pbvlanTable [intIfNum][index] = vlanid;

  } else
  {
    /* We are deleting an existing protocol.
    */
    pbvlanTable [intIfNum][index] = L7_PBVLAN_NULL_VLANID;
  }

  for (i = 0; i < L7_PBVLAN_MAX_CONFIGURABLE_PROTOCOLS; i++)
  {
  dapiCmd.cmdData.portConfig.protocol[i] = (L7_ushort16)pbvlanTable[intIfNum][i];
  }
  dapiCmd.cmdData.portConfig.protocolModify = L7_TRUE;

  rc = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_PORT_CONFIG, &dapiCmd);

  if (rc != L7_FAILURE)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Configures PVID, based on the protocol
*
* @param    protoIndex  @b{(input)} Protocol index in which protocol is added
* @param    protoId     @b{(input)} Protocol to add or delete.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlPbVlanProtocolConfig(L7_uint32 protoId,
                            L7_uint32 protoIndex)
{

  DAPI_SYSTEM_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  L7_RC_t rc=L7_FAILURE;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  bzero((L7_uchar8*)&dapiCmd,sizeof(DAPI_SYSTEM_CMD_t));

  dapiCmd.cmdData.pbVlanConfig.getOrSet = DAPI_CMD_SET;

  dapiCmd.cmdData.pbVlanConfig.protoNum   = protoId;
  dapiCmd.cmdData.pbVlanConfig.protoIndex = protoIndex;

   rc = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_PBVLAN_CONFIG, &dapiCmd);

  if (rc != L7_FAILURE)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

#endif
/*********************************************************************
* @purpose  Sets the traffic class for the specified priority
*
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    priority      @b{(input)} traffic class priority
* @param    trafficClass  @b{(input)} traffic class
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1dTrafficClassSet(L7_uint32 intIfNum,
                                L7_uint32 priority,
                                L7_uint32 trafficClass)
{
  DAPI_QVLAN_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  L7_RC_t rc;
  nimUSP_t usp;

  memset(&dapiCmd, 0, sizeof(dapiCmd));
  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;

  ddUsp.unit = (L7_ushort16)usp.unit;
  ddUsp.slot = (L7_ushort16)usp.slot;
  ddUsp.port = (L7_ushort16)usp.port-1;

  dapiCmd.cmdData.portPriorityToTcMap.priority = priority;
  dapiCmd.cmdData.portPriorityToTcMap.traffic_class = trafficClass;
  dapiCmd.cmdData.portPriority.getOrSet = DAPI_CMD_SET;

  rc = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_PORT_PRIORITY_TO_TC_MAP, &dapiCmd);

  if (rc != L7_FAILURE)  /* could be a delayed success */
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Sets the default port priority.
*
* @param    intIfNum      @b{(input)} Internal Interface Number
* @param    priority      @b{(input)} traffic class priority
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlDot1dDefaultPortPrioritySet(L7_uint32 intIfNum,
                     L7_uint32 priority)
{
  DAPI_QVLAN_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  L7_RC_t rc;
  nimUSP_t usp;

  memset(&dapiCmd, 0, sizeof(dapiCmd));
  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;

  if (priority > L7_DTL_MAX_PRIORITY )
  {
    return L7_FAILURE;
  }

  ddUsp.unit = (L7_ushort16)usp.unit;
  ddUsp.slot = (L7_ushort16)usp.slot;
  ddUsp.port = (L7_ushort16)usp.port-1;

  dapiCmd.cmdData.portPriority.priority = priority;
  dapiCmd.cmdData.portPriority.getOrSet = DAPI_CMD_SET;

  rc = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_PORT_PRIORITY, &dapiCmd);

  if (rc != L7_FAILURE)  /* could be a delayed success */
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*
**********************************************************************
*                           PRIVATE FUNCTIONS
**********************************************************************
*/



#endif /*DTLCTRL_COMPONENT_L2_DOT1Q*/


