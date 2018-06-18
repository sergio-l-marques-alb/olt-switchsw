/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    garpcfg.c
* @purpose     GARP Cfg functions
* @component   GARP
* @comments    none
* @create      03/12/2001
* @author      Hassan
* @author
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "string.h"
#include "osapi.h"
#include "log.h"
#include "nvstoreapi.h"
#include "garpapi.h"
#include "garpctlblk.h"
#include "garpcfg.h"
#include "gidapi.h"
#include "gvrapi.h"
#include "dot3ad_api.h"
#include "dtlapi.h"
#include "nimapi.h"
#include "dot1dgarp.h"
#include "dot1q_api.h"
#include "garp_dot1q_api.h"
#include "garp_dot1q_util.h"
#include "garp_leaveall_timer.h"

extern garpCfg_t *garpCfg;
extern gvrpMac_t *gvrpMac;
extern gvrpMac_t *gmrpMac;
extern L7_uint32 *garpMapTbl;
extern garpCnfgrState_t garpCnfgrState;
extern GARPCBptr GARPCB;
extern GarpInfo_t garpInfo;
extern void garpExpireGVRPLeaveAllTimer(L7_uint32 port_no);

extern L7_uint32 GenerateLeaveAllTimeoutValue(L7_uint32 leaveall_timeout);

L7_uint32 garp_counters[] =
{
  L7_PLATFORM_CTR_GVRP_PDU_RCV_COUNT,
  L7_PLATFORM_CTR_GVRP_PDU_SND_COUNT,
  L7_PLATFORM_CTR_GVRP_FAILED_REGISTRATIONS,
  L7_PLATFORM_CTR_GMRP_PDU_RCV_COUNT,
  L7_PLATFORM_CTR_GMRP_PDU_SND_COUNT,
  L7_PLATFORM_CTR_GMRP_FAILED_REGISTRATIONS
};/*the above list contains both GVRP and GMRP per port counter list*/


L7_uchar8 *garpCnfgrStateNames[GARP_PHASE_UNCONFIG_2 + 1] =
{
  "P0", "P1", "P2", "WMU", "P3", "EXE", "U1", "U2"
};


/*********************************************************************
* @purpose  Checks if garp user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL garpHasDataChanged(void)
{
  return garpCfg->hdr.dataChanged;
}
void garpResetDataChanged(void)
{
  garpCfg->hdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Saves GARP user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t garpSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if (garpCfg->hdr.dataChanged == L7_TRUE)
  {
    garpCfg->hdr.dataChanged = L7_FALSE;
    garpCfg->checkSum = nvStoreCrc32((L7_uchar8*)garpCfg,
                                     (L7_uint32)(sizeof(garpCfg_t) - sizeof(garpCfg->checkSum)));

    if ( (rc = sysapiCfgFileWrite(L7_GARP_COMPONENT_ID, GARP_CFG_FILENAME, (L7_char8*)garpCfg, (L7_int32)sizeof(garpCfg_t))) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
              "Error on call to sysapiCfgFileWrite routine on config file %s\n", GARP_CFG_FILENAME);
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Unapply existing configuration
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Resets active configuration without changing config parms
*
* @end
*********************************************************************/
L7_RC_t garpUnapplyConfigData()
{
  (void) garpGarpModeProcess(GARP_GVRP_APP,GARP_DISABLE);
  (void) garpGarpModeProcess(GARP_GMRP_APP,GARP_DISABLE);

  return L7_SUCCESS;
}



/*********************************************************************
* @purpose  Reset the garp active configuration to default values
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    Sets active configuration without changing config parms
*
* @end
*********************************************************************/
L7_RC_t garpRestore()
{
    garpUnapplyConfigData();

    garpBuildDefaultConfigData(garpCfg->hdr.version);
    garpApplyConfigData();


  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Gets the GVRP admin mode
*
* @param    void
*
* @returns  L7_TRUE if enabled , L7_FALSE if disabled
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL garpGetGvrpEnabled()
{
  return garpCfg->gvrp_enabled;
}

/*********************************************************************
* @purpose  Gets the GMRP admin mode
*
* @param    void
*
* @returns  L7_TRUE if enabled , L7_FALSE if disabled
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL garpGetGmrpEnabled()
{
  return garpCfg->gmrp_enabled;
}

/*********************************************************************
* @purpose  Builds a GARP config file with defaults
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    Builds Garp, VLAN and port default data which will be
*           applied in garpApplyConfigData
*
* @end
*********************************************************************/
void garpBuildDefaultConfigData(L7_uint32 ver)
{
    L7_RC_t rc =  L7_FAILURE;
    L7_BOOL brc =  L7_FALSE;
    L7_uint32  idx,intIfNum;
    garpIntfCfgData_t *pCfg;

    GARPCB->GarpCB_joinTime      = (L7_uint32)DEFAULT_GARPCONFIGJOINTIME;
    GARPCB->GarpCB_leaveTime     = (L7_uint32)DEFAULT_GARPCONFIGLEAVETIME;
    GARPCB->GarpCB_leaveAllTime  = (L7_uint32)DEAFULT_GARPCONFIGLEAVEALLTIME;

    memset( (void*)garpCfg, 0, sizeof(garpCfg_t) );

    strcpy(garpCfg->hdr.filename, GARP_CFG_FILENAME);
    garpCfg->hdr.version = GARP_CFG_VER_CURRENT;
    garpCfg->hdr.componentID = L7_GARP_COMPONENT_ID;
    garpCfg->hdr.type = L7_CFG_DATA;
    garpCfg->hdr.length = sizeof(garpCfg_t);
    garpCfg->hdr.dataChanged = L7_FALSE;
    garpCfg->hdr.savePointId = 0;
    garpCfg->hdr.targetDevice = 0;

    if (garpCnfgrState == GARP_PHASE_EXECUTE)
    {
      rc = nimFirstValidIntfNumber(&intIfNum);
    }

    while (rc == L7_SUCCESS)
    {
      if ((brc = garpMapIntfIsConfigurable(intIfNum, &pCfg)) == L7_TRUE)
      {
        idx = garpMapTbl[intIfNum];

        pCfg->join_time = DEFAULT_GARPCONFIGJOINTIME;
        pCfg->leave_time = DEFAULT_GARPCONFIGLEAVETIME;
        pCfg->leaveall_time = DEAFULT_GARPCONFIGLEAVEALLTIME;
        pCfg->vlanCreationForbid = DEFAULT_GVRP_VLAN_CREATION_FORBID;
        pCfg->registrationForbid = DEFAULT_GVRP_REGISTRATION_FORBID;
      }

      rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
    }

    GARP_GVRP_IS_ENABLED = L7_FALSE;
    GARP_GMRP_IS_ENABLED = L7_FALSE;

    garpCfg->gvrp_enabled = L7_FALSE;
    garpCfg->gmrp_enabled = L7_FALSE;

    /*Reset AcquiredList Structure*/
    memset(&(garpInfo.acquiredList), 0, sizeof(AcquiredMask) * L7_MAX_INTERFACE_COUNT);

    garpCfg->hdr.dataChanged = L7_TRUE;

    return;
}

/*********************************************************************
* @purpose  Apply GARP config data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t garpApplyConfigData(void)
{
  L7_uint32 cfgIndex, intIfNum;
  nimConfigID_t configIdNull;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  /* apply global data */
  if (garpCfg->gvrp_enabled == L7_TRUE)
  {
      (void)garpGarpModeProcess(GARP_GVRP_APP, GARP_ENABLE);
  }
  if (garpCfg->gmrp_enabled == L7_TRUE)
  {
      (void)garpGarpModeProcess(GARP_GMRP_APP, GARP_ENABLE);
  }

  if (garpCnfgrState == GARP_PHASE_EXECUTE)
  {
    /* apply configuration for all configured interfaces, used for clear,restore only */
    for (cfgIndex = 0; cfgIndex < L7_GARP_MAX_INTF; cfgIndex++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&garpCfg->intf[cfgIndex].configId, &configIdNull))
        continue;
      if (nimIntIfFromConfigIDGet(&(garpCfg->intf[cfgIndex].configId), &intIfNum) != L7_SUCCESS)
        continue;
      garpApplyIntfConfigData(intIfNum, GARP_APPLY_CONFIG_NORMAL);
    }
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Apply interface configuration
*
* @param    L7_uint32  intIfNum   internal interface number
* @param    L7_uint32  applyParam how to apply config data
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments Invoked for an L7_ATTACH or during an apply config
*
* @end
*********************************************************************/
L7_RC_t garpApplyIntfConfigData(L7_uint32 intIfNum, L7_uint32 applyParam)
{
    L7_BOOL rc;
    garpIntfCfgData_t *pCfg = NULL;
    L7_uint32 val;

    rc = garpMapIntfIsConfigurable(intIfNum, &pCfg);

    if (rc == L7_TRUE)
    {
      val = pCfg->join_time/10;
      garpJoinTimeProcess(intIfNum, val);

      val = pCfg->leave_time/10;
      garpLeaveTimeProcess(intIfNum, val);

      val = pCfg->leaveall_time/10;
      garpLeaveAllTimeProcess(intIfNum, val);

      if (applyParam != GARP_APPLY_CONFIG_NSF)
      {
        /*
        In the NSF case, the following functions should only be done
        if the proper interface state exists, even though the rest of
        the config should be applied
        */

        (void)garpApplyIntfGvrpMode(intIfNum);
        (void)garpApplyIntfGmrpMode(intIfNum);
      }

      GarpIssueCmd(GARP_GVRP_APP, GVRP_VLAN_CREATION_FORBID, intIfNum, GARP_GVRP_VLAN_ATTRIBUTE, (GARPAttrValue)(&pCfg->vlanCreationForbid), L7_NULL);
      rc = GarpIssueCmd(GARP_GVRP_APP, GVRP_VLAN_REGISTRATION_FORBID, intIfNum, GARP_GVRP_VLAN_ATTRIBUTE, (GARPAttrValue)(&pCfg->registrationForbid), L7_NULL);
    }

    return rc;

}

/*********************************************************************
* @purpose  Apply GVRP interface mode
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Invoked for L7_DETACH
*
* @end
*********************************************************************/
L7_RC_t garpApplyIntfGvrpMode(L7_uint32 intIfNum)
{
    GARPCommand  cmd;
    L7_uint32    gvrpIntfMode;


    /* Apply GVRP interface cfg */
    if (L7_SUCCESS == garpQportsGVRPCfgGet(intIfNum, &gvrpIntfMode))
    {
        if (gvrpIntfMode == L7_ENABLE)
        {

            if ( GarpBuildCommand(&cmd,
                                  GARP_GVRP_APP,
                                  GARP_ENABLE_PORT,
                                  GARP_GVRP_VLAN_ATTRIBUTE,
                                  L7_NULL,
                                  intIfNum,
                                  L7_NULL) == GARP_SUCCESS)
            {
                garpIntfGVRPModeEnableProcess(cmd.port, &cmd);
            }
        }
        else
        {

            if ( GarpBuildCommand(&cmd,
                                  GARP_GVRP_APP,
                                  GARP_DISABLE_PORT,
                                  GARP_GVRP_VLAN_ATTRIBUTE,
                                  L7_NULL,
                                  intIfNum,
                                  L7_NULL) == GARP_SUCCESS)
            {
                garpIntfGVRPModeDisableProcess(cmd.port, &cmd);
            }
        }
    }

    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Apply GMRP interface mode
*
* @param    intIfNum @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    Invoked for L7_DETACH
*
* @end
*********************************************************************/
L7_RC_t garpApplyIntfGmrpMode(L7_uint32 intIfNum)
{
    GARPCommand  cmd;
    L7_uint32    gvrpIntfMode;


    /* Apply GVRP interface cfg */
    if (L7_SUCCESS == garpQportsGMRPCfgGet(intIfNum, &gvrpIntfMode))
    {
        if (gvrpIntfMode == L7_ENABLE)
        {

            if ( GarpBuildCommand(&cmd,
                                  GARP_GMRP_APP,
                                  GARP_ENABLE_PORT,
                                  GARP_GMRP_ATTRIBUTE,
                                  L7_NULL,
                                  intIfNum,
                                  L7_NULL) == GARP_SUCCESS)
            {
                garpIntfGMRPModeProcess(cmd.port, L7_ENABLE, &cmd);
            }
        }
        else
        {

            if ( GarpBuildCommand(&cmd,
                                  GARP_GMRP_APP,
                                  GARP_DISABLE_PORT,
                                  GARP_GMRP_ATTRIBUTE,
                                  L7_NULL,
                                  intIfNum,
                                  L7_NULL) == GARP_SUCCESS)
            {
                garpIntfGMRPModeProcess(cmd.port, L7_DISABLE, &cmd);
            }
        }
    }

    return L7_SUCCESS;
}




/*********************************************************************
* @purpose  Check whether dot1q is ready.
*
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL garpCnfgrStateCheck (void)
{
  if ((garpCnfgrState == GARP_PHASE_EXECUTE) ||
      (garpCnfgrState == GARP_PHASE_INIT_3)  ||
        (garpCnfgrState == GARP_PHASE_UNCONFIG_1))
  {
      return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to dot1q interface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULL if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL garpMapIntfIsConfigurable(L7_uint32 intIfNum, garpIntfCfgData_t **pCfg)
{
  L7_uint32 index;
  nimConfigID_t configId;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  if ((garpCfg_t *)L7_NULL == garpCfg)
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_GARP_COMPONENT_ID,
            "garpCfg NULL");
    return L7_FALSE;
  }

  if (garpCnfgrStateCheck() == L7_FALSE)
  {
    return L7_FALSE;
  }

  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
  {
    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_GARP_COMPONENT_ID,
            "intf %d out of bounds", intIfNum);
    return L7_FALSE;
  }

  index = garpMapTbl[intIfNum];

  if (index == 0)
  {
    return L7_FALSE;
  }

  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   * the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(garpCfg->intf[index].configId)) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between snoopCfgData and snoopMapTbl or
       * there is synchronization issue between NIM and components w.r.t. interface creation/deletion
       */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_GARP_COMPONENT_ID,
          "Error accessing GARP config data for interface %s in garpMapIntfIsConfigurable."
          " A default configuration does not exist for this interface. Typically a case when"
          " a new interface is created and has no pre-configuration.", ifName);
      return L7_FALSE;
    }
  }

  *pCfg = &garpCfg->intf[index];

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first free interface config struct
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to snoop nterface config structure
*                           or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the NIM
*           interface exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL garpMapIntfConfigEntryGet(L7_uint32 intIfNum, garpIntfCfgData_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configId;
  nimConfigID_t configIdNull;
  L7_RC_t rc;
  static L7_uint32 nextIndex = 1;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if (garpCnfgrStateCheck() == L7_FALSE)
    return L7_FALSE;

  if ((rc = nimConfigIdGet(intIfNum, &configId)) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  /* Avoid N^2 processing when interfaces created at startup */
  if (nextIndex < L7_GARP_MAX_INTF)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&garpCfg->intf[nextIndex].configId, &configIdNull))
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_GARP_COMPONENT_ID,
              "garpMapIntfConfigEntryGet: Found empty config ID 1 %d for interface %d, %s\n",
              nextIndex, intIfNum, ifName);
      garpMapTbl[intIfNum] = nextIndex;
      *pCfg = &garpCfg->intf[nextIndex];
      nextIndex++;
      return L7_TRUE;
    }
  }

  for (i = 1; i < L7_GARP_MAX_INTF; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&garpCfg->intf[i].configId, &configIdNull))
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_GARP_COMPONENT_ID,
              "garpMapIntfConfigEntryGet: Found empty config ID 1 %d for interface %d, %s\n",
              nextIndex, intIfNum, ifName);
      garpMapTbl[intIfNum] = i;
      *pCfg = &garpCfg->intf[i];
      nextIndex = i + 1;
      return L7_TRUE;
    }
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  To check if the GARP interface has already been created
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL garpIntfExists(L7_uint32 intIfNum)
{
    if (garpMapTbl[intIfNum] != 0)
    {
        return L7_TRUE;
    }
    else
    {
        return L7_FALSE;
    }
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
* @comments Allocates intf config storage if a config does not exist
*
* @end
*
*********************************************************************/
L7_RC_t garpIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t configId;
  garpIntfCfgData_t *pCfg;
  /*
  L7_uint32 i;
  */


  if (garpIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_FAILURE;
  }
  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /*
  for (i = 1; i < L7_GARP_MAX_INTF; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&garpCfg->intf[i].configId, &configId))
    {
      garpMapTbl[intIfNum] = i;
      break;
    }
  }
  */

  pCfg = L7_NULL;

  if (garpMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    /* config does not exist, build defaults */
    if (garpMapIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
    {
        garpMapTbl[intIfNum] = 0;
        return L7_FAILURE;
    }

    /* port defaults */
    pCfg->join_time = DEFAULT_GARPCONFIGJOINTIME;
    pCfg->leave_time = DEFAULT_GARPCONFIGLEAVETIME;
    pCfg->leaveall_time = DEAFULT_GARPCONFIGLEAVEALLTIME;
    pCfg->vlanCreationForbid =  DEFAULT_GVRP_VLAN_CREATION_FORBID;
    pCfg->registrationForbid =  DEFAULT_GVRP_REGISTRATION_FORBID;
  }

  if (pCfg != L7_NULL)
  {
    NIM_CONFIG_ID_COPY(&pCfg->configId, &configId);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Register garp interface counters with the stats manager
*
* @param    None
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @end
*********************************************************************/
L7_RC_t garpStatsIntfCreate(L7_uint32 intIfNum)
{
  L7_uint32 listSize = sizeof(garp_counters) / sizeof(L7_uint32);
  statsParm_entry_t garpCtr;
  L7_uint32   i;
  L7_RC_t rc = L7_SUCCESS;

  garpCtr.cSize = C32_BITS;
  garpCtr.cType = ExternalCtr;
  garpCtr.isResettable = L7_FALSE;
  garpCtr.pMutlingsParmList = L7_NULL;
  garpCtr.pSource = (L7_VOIDFUNCPTR_t) garpStatGet;

  for (i=0; (i< listSize && rc == L7_SUCCESS); i++)
  {
    garpCtr.cId = garp_counters[i];
    /* have the intf number as the first 12 bits of CID */
    garpCtr.cKey = intIfNum;
    rc = statsCreate(1, (pStatsParm_list_t)(&garpCtr));
  }

  return rc;
}

/*********************************************************************
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
*********************************************************************/
L7_RC_t garpIntfDelete(L7_uint32 intIfNum)
{
  garpIntfCfgData_t *pCfg;

  if (garpMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
    garpMapTbl[intIfNum] = 0;
    return L7_SUCCESS;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the join time for a port
*
* @param    first_port   start of gid ring
* @param    port         port to set
* @param    join_time    join time to set
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void setJoinTime(void *first_port, L7_uint32 port, L7_uint32 join_time)
{
  Gid *my_port;
  if (gid_find_port((Gid*)first_port, port, (void*)&my_port))
  {
    my_port->join_timeout = join_time*10;
  }
}

/*********************************************************************
* @purpose  Set the leave time for a port
*
* @param    first_port   start of gid ring
* @param    port         port to set
* @param    join_time    leave time to set
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void setLeaveTime(void *first_port, L7_uint32 port, L7_uint32 leave_time)
{
  Gid *my_port;

  if (gid_find_port((Gid*)first_port, port, (void*)&my_port))
  {
    my_port->leave_timeout = leave_time*10;
  }
}

/*********************************************************************
* @purpose  Set the leaveall time for a port
*
* @param    first_port   start of gid ring
* @param    port         port to set
* @param    join_time    leaveall time to set
*
* @returns  void
*
* @comments none
*
* @end
*********************************************************************/
void setLeaveAllTime(void *first_port, L7_uint32 port, L7_uint32 leaveall_time)
{
  Gid *my_port;

  if (gid_find_port((Gid*)first_port, port, (void*)&my_port))
  {
    my_port->leaveall_timeout = leaveall_time*10;

    switch (my_port->application->app)
    {
        case GARP_GVRP_APP:
            /* update the GVRP Leave All Timer for the current interface */
            garpUpdateLeaveAllTimer(UPDATE_LEAVEALL_TIMER,
                        my_port->port_no, GARP_GVRP_APP, 0, 0,
                        my_port->leaveall_timeout);
            break;

        case GARP_GMRP_APP:
            /* GMRP Leave All timer is running in the Gid structure, so
             * update it there */
            my_port->gmrp_leaveall_left =
                GenerateLeaveAllTimeoutValue(my_port->leaveall_timeout);

            break;

        default:
            break;
    }

  }
  return;
}

/*********************************************************************
* @purpose  enable or disable gvrp application
*
* @param    enable    application mode
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void setGvrpApp(L7_BOOL enable)
{
  (void)dtlDot1qGvrpGmrpConfig(GARP_GVRP_APP, enable);

  return;
}

/*********************************************************************
* @purpose  enable or disable gmrp application
*
* @param    enable    application mode
*
* @returns  none
*
* @notes
*
* @end
*********************************************************************/
void setGmrpApp(L7_BOOL enable)
{
  (void)dtlDot1qGvrpGmrpConfig(GARP_GMRP_APP, enable);

  return;
}

/*********************************************************************
* @purpose  returns the MAC address of last GVRP PDU received on a port
*
* @param    intIfNum    internal interface number
* @param    *macAddr    pointer to a structure where mac address is stored
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t garpPortGvrpLastPduOriginGet(L7_uint32 BasePort, L7_uchar8 *macAddr)
{
  memcpy(macAddr, gvrpMac[BasePort].mac_addr, L7_MAC_ADDR_LEN);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  returns the MAC address of last GVRP PDU received on a port
*
* @param    intIfNum    internal interface number
* @param    *macAddr    pointer to a structure where mac address is stored
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t garpPortGmrpLastPduOriginGet(L7_uint32 BasePort, L7_uchar8 *macAddr)
{
  memcpy(macAddr, gmrpMac[BasePort].mac_addr, L7_MAC_ADDR_LEN);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  See if an interface type is of the right type for GARP
*
* @param    sysIntfType              @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL garpIsValidIntfType(L7_uint32 sysIntfType)
{
  switch (sysIntfType)
  {
    case L7_PHYSICAL_INTF:
    case L7_LAG_INTF:
      return L7_TRUE;

      default:
        return L7_FALSE;
        break;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  See if an interface is of the right type for GARP
*
* @param    intifNum
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if not created, not configured or wrong type
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL garpIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
    return L7_FALSE;

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    switch (sysIntfType)
    {
      case L7_PHYSICAL_INTF:
        return L7_TRUE;
        break;

      case L7_LAG_INTF:
        if (dot3adIsLagConfigured(intIfNum) == L7_TRUE)
          return L7_TRUE;
        break;

      default:
        return L7_FALSE;
        break;
    }
  }
  return L7_FALSE;
}


/*********************************************************************
* @purpose  Set the value of vlan-creation-forbid flag for an interface
*
* @param    interface   (input) interface number
* @param    forbid      (input) vlan-creation-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t  setVlanCreationForbid(void *first_port, L7_uint32 interface, L7_BOOL forbid)
{
  Gid *my_port;
  garpIntfCfgData_t *pCfg;
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(interface, L7_SYSNAME, ifName);

  if (garpMapIntfIsConfigurable(interface, &pCfg) == L7_TRUE)
  {
    pCfg->vlanCreationForbid = forbid;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
            "setVlanCreationForbid(): Failed to set the vlan-creation-forbid config flag for interface %s.\n", ifName);
    return L7_FAILURE;
  }

  if (gid_find_port((Gid*)first_port, interface, (void*)&my_port))
  {
    my_port->vlanCreationForbid = forbid;
  }
  return rc;
}


/*********************************************************************
* @purpose  Set the value of VLAN registration-forbid flag for an interface
*
* @param    first_port  (input) GID instance for the first port in ring
* @param    interface   (input) interface number
* @param    forbid      (input) VLAN registration-forbid flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t  setVlanRegistrationForbid(void *first_port, L7_uint32 interface, L7_BOOL forbid)
{
  Gid *my_port;
  garpIntfCfgData_t *pCfg;
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(interface, L7_SYSNAME, ifName);

  if (garpMapIntfIsConfigurable(interface, &pCfg) == L7_TRUE)
  {
    pCfg->registrationForbid = forbid;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_GARP_COMPONENT_ID,
            "setVlanRegistrationForbid(): Failed to set the registration-forbid config flag for interface %s.\n", ifName);
    return L7_FAILURE;
  }

  if (gid_find_port((Gid*)first_port, interface, (void*)&my_port))
  {
    my_port->registrationForbid = forbid;
    if (forbid)
    {
      /* remove the dynamic VLANs from this port */
      garpExpireGVRPLeaveAllTimer(interface);
    }
  }
  return rc;
}
/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void garpBuildTestConfigData(void)
{
  L7_RC_t rc =  L7_FAILURE;
  L7_BOOL brc =  L7_FALSE;
  L7_uint32  intIfNum;
  garpIntfCfgData_t *pCfg;


  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/

  if (garpCfg != L7_NULL)
  {
      if (garpCnfgrState == GARP_PHASE_EXECUTE)
      {
        rc = nimFirstValidIntfNumber(&intIfNum);
      }

      while (rc == L7_SUCCESS)
      {
        if ((brc = garpMapIntfIsConfigurable(intIfNum, &pCfg)) == L7_TRUE)
        {
          /* Ensure at least one entry in each array has a unique value to validate
             accurate migration */
          pCfg->join_time = 550 + intIfNum;
          pCfg->leave_time = 650;
          pCfg->leaveall_time = 750;
        }

        rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
      }

      garpCfg->gmrp_enabled = L7_TRUE;
      garpCfg->gvrp_enabled = L7_TRUE;
  }

 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   garpCfg->hdr.dataChanged = L7_TRUE;
   sysapiPrintf("Built test config data\n");


}


/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void garpConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(garpCfg->hdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

    GarpConfigDump();


    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "L7_GARP_MAX_INTF - %d\n", L7_GARP_MAX_INTF);



    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("garpCfg->checkSum : %u\n", garpCfg->checkSum);


}



/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/


