/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename    gmrp_api.c
* @purpose     GMRP API
* @component   GMRP-GARP
* @comments    none
* @create      02/21/2002
* @author      Shekhar Kalyanam
* @end
*             
**********************************************************************/

#include "string.h"
#include "l7_common.h"
#include "usmdb_garp.h"
#include "usmdb_mib_vlan_api.h"
#include "defaultconfig.h"
#include "comm_mask.h"
#include "osapi.h"
#include "avl_api.h"
#include "garp.h"
#include "garpapi.h"
#include "garpcfg.h"
#include "gidapi.h"
#include "gipapi.h"
#include "gmrapi.h"
#include "gmrp_api.h"
#include "gmd.h"
#include "avl_api.h"
#include "nvstoreapi.h"
#include "dtlapi.h"
#include "dot1q_api.h"
#include "mfdb_api.h"
#include "dot1dgarp.h"
#include "dot1s_api.h"
#include "usmdb_util_api.h"

#include "dot3ad_api.h"
#include "sysapi.h"
#include "garp_dot1q_api.h"
#include "garp_dot1q_util.h"
#include "garp_debug.h"

gmrpCfg_t     *gmrpCfg;     /* config file overlay, config id and offset based */
gmrpOperCfg_t *gmrpOperCfg; /* operational config, used by apis and protocol   */

gmrpInstance_t *gmrpInstance;

void *globalGmd;
void *gmrpCfgSema;

extern L7_uint32 *gmrpMapTbl;
extern garpCnfgrState_t garpCnfgrState;
extern GarpInfo_t garpInfo;

static const L7_uchar8 allFoxes[L7_MAC_ADDR_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
static const L7_uchar8 firstFive[5] = {0x01,0x80,0xC2,0x00,0x00};

#define GMRP_MAX_RESTRICTED_ADDR  32
static const L7_uchar8 gmrpRestrictedMacAddr[GMRP_MAX_RESTRICTED_ADDR][L7_MAC_ADDR_LEN] =
{
  {0x01,0x80,0xC2,0x00,0x00,0x00},
  {0x01,0x80,0xC2,0x00,0x00,0x01},
  {0x01,0x80,0xC2,0x00,0x00,0x02},
  {0x01,0x80,0xC2,0x00,0x00,0x03},
  {0x01,0x80,0xC2,0x00,0x00,0x04},
  {0x01,0x80,0xC2,0x00,0x00,0x05},
  {0x01,0x80,0xC2,0x00,0x00,0x06},
  {0x01,0x80,0xC2,0x00,0x00,0x07},
  {0x01,0x80,0xC2,0x00,0x00,0x08},
  {0x01,0x80,0xC2,0x00,0x00,0x09},
  {0x01,0x80,0xC2,0x00,0x00,0x0A},
  {0x01,0x80,0xC2,0x00,0x00,0x0B},
  {0x01,0x80,0xC2,0x00,0x00,0x0C},
  {0x01,0x80,0xC2,0x00,0x00,0x0D},
  {0x01,0x80,0xC2,0x00,0x00,0x0E},
  {0x01,0x80,0xC2,0x00,0x00,0x0F},
  {0x01,0x80,0xC2,0x00,0x00,0x20},
  {0x01,0x80,0xC2,0x00,0x00,0x21},
  {0x01,0x80,0xC2,0x00,0x00,0x22},
  {0x01,0x80,0xC2,0x00,0x00,0x23},
  {0x01,0x80,0xC2,0x00,0x00,0x24},
  {0x01,0x80,0xC2,0x00,0x00,0x25},
  {0x01,0x80,0xC2,0x00,0x00,0x26},
  {0x01,0x80,0xC2,0x00,0x00,0x27},
  {0x01,0x80,0xC2,0x00,0x00,0x28},
  {0x01,0x80,0xC2,0x00,0x00,0x29},
  {0x01,0x80,0xC2,0x00,0x00,0x2A},
  {0x01,0x80,0xC2,0x00,0x00,0x2B},
  {0x01,0x80,0xC2,0x00,0x00,0x2C},
  {0x01,0x80,0xC2,0x00,0x00,0x2D},
  {0x01,0x80,0xC2,0x00,0x00,0x2E},
  {0x01,0x80,0xC2,0x00,0x00,0x2F}
};

/*********************************************************************
* @purpose  create a new gmr and add it to the table of gmr pointers    
*
* @param    L7_uint32 vlanId 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpCreateGmr(L7_uint32 vlanId)
{
  L7_uint32 emptySlotIndex;

  /*find an empty slot in the gmrp Instance table*/
  if (gmrpInstanceEmptyIndexGet(vlanId, &emptySlotIndex) != L7_SUCCESS)
  {
      LOG_MSG("Failed to find empty gmrpInstance for new vlan %d\n", vlanId);
      return L7_FAILURE;
  }
  /*create the gmr*/
  if (gmr_create_gmr(L7_NULL, (L7_ushort16)vlanId, 
                     (void *)&gmrpInstance[emptySlotIndex].gmr)!= L7_TRUE)
  {
      /* Failure reasons were logged at point of failure */
      return L7_FAILURE;
  }
  /*now set the gmrpInstance in_use field to L7_TRUE*/
  /*as the gmr was successfully created */
  gmrpInstance[emptySlotIndex].inuse = L7_TRUE;

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  destroys a gmr and deletes it from the table of gmr pointers    
*
* @param    L7_uint32 vlanId 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpDestroyGmr(L7_uint32 vlanId)
{
  L7_uint32 index;

  if (gmrpInstanceIndexFind(vlanId,&index)==L7_FAILURE)
    return L7_FAILURE;

  gmr_destroy_gmr(gmrpInstance[index].gmr);

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Gets an empty gmrp index to save this instance of gmrp    
*
* @param    L7_uint32 *emptyIndex 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpInstanceEmptyIndexGet(L7_uint32 vlanId, L7_uint32 *emptyIndex)
{
  if (gmrpInstance[vlanId].inuse == L7_FALSE)
  {
	*emptyIndex = vlanId;
	return L7_SUCCESS;
  }
  else
	return L7_FAILURE;

}
/*********************************************************************
* @purpose  Finds a GMRP instance based on the vlanId    
*
* @param    vlanId 	 Vlan ID
* @param	*index   found at this index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpInstanceIndexFind(L7_uint32 vlanId, L7_uint32 *index)
{

  if (gmrpInstance[vlanId].inuse == L7_TRUE &&
	  gmrpInstance[vlanId].gmr->vlan_id == (L7_ushort16)vlanId)
  {
	*index = vlanId;
	return L7_SUCCESS;
  }
  else
	return L7_FAILURE;

}
/*********************************************************************
* @purpose  Finds a GMRP instance based on the vlanId    
*
* @param    vlanId 	 Vlan ID
* @param	*index   found at this index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This routine returns the pointer to the GMR
*       
* @end
*********************************************************************/
Gmr *gmrpInstanceGet(L7_uint32 vlanId)
{
  if ((gmrpInstance[vlanId].inuse == L7_TRUE) && 
	  (gmrpInstance[vlanId].gmr->vlan_id == (L7_ushort16)vlanId)) 
	return gmrpInstance[vlanId].gmr;
  else
	return L7_NULLPTR;


}

/*********************************************************************
* @purpose  Saves gmrp configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    dot3adCfg is the overlay
*       
* @end
*********************************************************************/
L7_RC_t gmrpSave(void)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 entry, vlan, cfgIdx, intIfNum, maskOffset;
  nimConfigID_t configId;

  if ( gmrpCfg->hdr.dataChanged == L7_TRUE)
  {
  /* per reg entry only */
  for (entry=0;entry<L7_MAX_GROUP_REGISTRATION_ENTRIES;entry++)
  {
    gmrpCfg->cfg.staticEntry[entry].inUse = gmrpOperCfg->staticEntry[entry].inUse;
    
    memcpy(gmrpOperCfg->staticEntry[entry].vlanIdmacAddress,
           gmrpCfg->cfg.staticEntry[entry].vlanIdmacAddress,
           sizeof(L7_uchar8)*8);
  }
  
  /* per vlan only */
  for (vlan=0;vlan<L7_MAX_VLANS;vlan++)
  {
    gmrpCfg->cfg.staticForwardAll[vlan].vlanId = gmrpOperCfg->staticForwardAll[vlan].vlanId;
    gmrpCfg->cfg.staticForwardUnreg[vlan].vlanId = gmrpOperCfg->staticForwardUnreg[vlan].vlanId;
  }
  
  /* per interface items */
  rc = nimFirstValidIntfNumber(&intIfNum);
  while (rc == L7_SUCCESS && nimConfigIdGet(intIfNum, &configId)==L7_SUCCESS)
  {
    nimConfigIdMaskOffsetGet(intIfNum, &maskOffset);

    cfgIdx = gmrpMapTbl[intIfNum];
    
    /* per interface only */
    NIM_CONFIG_ID_COPY(&gmrpCfg->intf[cfgIdx].configId, &configId);

    gmrpCfg->cfg.gmrpEnabled[cfgIdx] = gmrpOperCfg->gmrpEnabled[intIfNum];

    /* per reg entry per interface */
    for (entry=0;entry<L7_MAX_GROUP_REGISTRATION_ENTRIES;entry++)
    {
      
      if (NIM_INTF_ISMASKBITSET(gmrpOperCfg->staticEntry[entry].fixedReg, intIfNum))
        NIM_INTF_SETMASKBIT(gmrpCfg->cfg.staticEntry[entry].fixedReg, maskOffset);
      
      if (NIM_INTF_ISMASKBITSET(gmrpOperCfg->staticEntry[entry].forbiddenReg, intIfNum))
        NIM_INTF_SETMASKBIT(gmrpCfg->cfg.staticEntry[entry].forbiddenReg, maskOffset);
    }
    
    /* per vlan per interface */
    for (vlan=0;vlan<L7_MAX_VLANS;vlan++)
    {
      if (NIM_INTF_ISMASKBITSET(gmrpOperCfg->staticForwardAll[vlan].fixedReg, intIfNum))
        NIM_INTF_SETMASKBIT(gmrpCfg->cfg.staticForwardAll[vlan].fixedReg, maskOffset);
      
      if (NIM_INTF_ISMASKBITSET(gmrpOperCfg->staticForwardAll[vlan].forbiddenReg, intIfNum))
        NIM_INTF_SETMASKBIT(gmrpCfg->cfg.staticForwardAll[vlan].forbiddenReg, maskOffset);
    
      if (NIM_INTF_ISMASKBITSET(gmrpOperCfg->staticForwardUnreg[vlan].fixedReg, intIfNum))
        NIM_INTF_SETMASKBIT(gmrpCfg->cfg.staticForwardUnreg[vlan].fixedReg, maskOffset);
    
      if (NIM_INTF_ISMASKBITSET(gmrpOperCfg->staticForwardUnreg[vlan].forbiddenReg, intIfNum))
        NIM_INTF_SETMASKBIT(gmrpCfg->cfg.staticForwardUnreg[vlan].forbiddenReg, maskOffset);
    
    }
    
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  gmrpCfg->hdr.dataChanged = L7_FALSE;
  gmrpCfg->checkSum = nvStoreCrc32((L7_uchar8 *)gmrpCfg,
                                   (L7_uint32)(sizeof(gmrpCfg_t) - sizeof(gmrpCfg->checkSum)));                      

  if ( (rc = sysapiCfgFileWrite(L7_GMRP_COMPONENT_ID, GMRP_CFG_FILENAME, (L7_char8*)gmrpCfg, (L7_int32)sizeof(gmrpCfg_t))) != L7_SUCCESS)
  {
    LOG_MSG("Error on call to sysapiCfgFileWrite routine on config file %s\n",GMRP_CFG_FILENAME);
  }
  }
  
  return(rc);
}

/*********************************************************************
* @purpose  Checks if gmrp user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_BOOL gmrpHasDataChanged(void)
{
  return gmrpCfg->hdr.dataChanged;
}
void gmrpResetDataChanged(void)
{
  gmrpCfg->hdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Build default gmrp config data  
*
* @param    ver   Software version of Config Data
*
* @returns  void
*
* @notes    
*
* @end
*********************************************************************/
void gmrpBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 intIfNum,cfgIdx,index;
  L7_RC_t rc=L7_FAILURE;

  nimConfigID_t configId[L7_GMRP_MAX_CFG_IDX];
  
  /* save config ids since the interfaces will not be recreated after a restore */
  for (cfgIdx=0; cfgIdx<L7_GMRP_MAX_CFG_IDX; cfgIdx++)
  {
    NIM_CONFIG_ID_COPY(&configId[cfgIdx], &gmrpCfg->intf[cfgIdx].configId);
  }
  
  memset( (void*)gmrpCfg, 0, sizeof(gmrpCfg_t) );

  for (cfgIdx=0;cfgIdx<L7_GMRP_MAX_CFG_IDX;cfgIdx++)
  {
    NIM_CONFIG_ID_COPY(&gmrpCfg->intf[cfgIdx].configId, &configId[cfgIdx]);
  }
  
  /* setup file header */
  gmrpCfg->hdr.version = ver;
  gmrpCfg->hdr.componentID = L7_GMRP_COMPONENT_ID;
  gmrpCfg->hdr.type = L7_CFG_DATA;
  gmrpCfg->hdr.length = (L7_uint32)sizeof(gmrpCfg_t);
  strcpy((char *)gmrpCfg->hdr.filename, GMRP_CFG_FILENAME);
  gmrpCfg->hdr.dataChanged = L7_FALSE;
  
  if (garpCnfgrState == GARP_PHASE_EXECUTE)
  {
    rc = nimFirstValidIntfNumber(&intIfNum);
  }
  
  while (rc == L7_SUCCESS)
  {
    if(nimConfigIdGet(intIfNum,configId) == L7_SUCCESS) {
		index = gmrpMapTbl[intIfNum];
		gmrpCfg->cfg.gmrpEnabled[index] = L7_FALSE;
	}
    
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

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
L7_BOOL gmrpMapIntfConfigEntryGet(L7_uint32 intIfNum, gmrpIntfCfgData_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configId;
  nimConfigID_t configIdNull;
  L7_RC_t rc;

  memset(&configIdNull, 0, sizeof(nimConfigID_t));

  if (garpCnfgrState != GARP_PHASE_EXECUTE)
    return L7_FALSE;

  if ((rc = nimConfigIdGet(intIfNum, &configId)) == L7_SUCCESS)
  {
    for (i = 1; i < L7_GARP_MAX_INTF; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&gmrpCfg->intf[i].configId, &configIdNull))
      {
        gmrpMapTbl[intIfNum] = i;
        *pCfg = &gmrpCfg->intf[i];
        return L7_TRUE;
      }
    }
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  See if an interface type is of the right type for GARP
*
* @param    sysIntfType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, if not created, not configured or wrong type
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL gmrpIsValidIntfType(L7_INTF_TYPES_t sysIntfType)
{
  switch (sysIntfType)
  {
    case L7_PHYSICAL_INTF:
    case L7_LAG_INTF:
      return L7_TRUE;
      break;

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
L7_BOOL gmrpIsValidIntf(L7_uint32 intIfNum)
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
        if (dot3adIsLagConfigured(intIfNum) == L7_FALSE)
          return L7_FALSE;
    
      default:
        return L7_FALSE;
        break;
    }
  }
  return L7_FALSE;
}


/*********************************************************************
*
* @purpose  Process the Callback for L7_CREATE
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
L7_RC_t gmrpIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t configId;
  gmrpIntfCfgData_t *pCfg;
  L7_uint32 i, maskOffset, cfgIdx, entry, vid;
  
  if (gmrpIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = 1; i < L7_GARP_MAX_CFG_IDX; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&gmrpCfg->intf[i].configId, &configId))
    {
      gmrpMapTbl[intIfNum] = i;
      break;
    }
  }
  
  pCfg = L7_NULL;

  if (gmrpMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    /* config does not exist, build defaults */
    if(gmrpMapIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
      return L7_FAILURE;

    nimConfigIdMaskOffsetGet(intIfNum, &maskOffset);
    cfgIdx = gmrpMapTbl[intIfNum];
    
    gmrpCfg->cfg.gmrpEnabled[cfgIdx] = L7_FALSE;
    
    for (entry=0;entry<L7_MAX_GROUP_REGISTRATION_ENTRIES;entry++)
    {
      NIM_INTF_CLRMASKBIT(gmrpCfg->cfg.staticEntry[entry].fixedReg, maskOffset);
      NIM_INTF_CLRMASKBIT(gmrpCfg->cfg.staticEntry[entry].forbiddenReg, maskOffset);
    }
    
    for (vid=0;vid<L7_MAX_VLANS;vid++)
    {
      NIM_INTF_CLRMASKBIT(gmrpCfg->cfg.staticForwardAll[vid].fixedReg, maskOffset);
      NIM_INTF_CLRMASKBIT(gmrpCfg->cfg.staticForwardAll[vid].forbiddenReg, maskOffset);

      NIM_INTF_CLRMASKBIT(gmrpCfg->cfg.staticForwardUnreg[vid].fixedReg, maskOffset);
      NIM_INTF_CLRMASKBIT(gmrpCfg->cfg.staticForwardUnreg[vid].forbiddenReg, maskOffset);
    }
  }
  
  if (pCfg != L7_NULL)
  {
    NIM_CONFIG_ID_COPY(&pCfg->configId, &configId);
  }

  return L7_SUCCESS;
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
L7_RC_t gmrpIntfDelete(L7_uint32 intIfNum)
{
  gmrpIntfCfgData_t *pCfg;

  if (gmrpMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    memset((void *)&pCfg->configId, 0, sizeof(nimConfigID_t));
    gmrpMapTbl[intIfNum] = 0;
    return L7_SUCCESS;
  }

  /* ALT - If interface is not configurable then there is no
  **	   configuration to delete, so return success.
  **	   (Confirm with Dan)
  */
  return L7_SUCCESS;
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
L7_BOOL gmrpMapIntfIsConfigurable(L7_uint32 intIfNum, gmrpIntfCfgData_t **pCfg)
{
  L7_uint32 index;
  nimConfigID_t configId;

  if ((garpCnfgrState != GARP_PHASE_EXECUTE) &&
	  (garpCnfgrState != GARP_PHASE_UNCONFIG_1))
    return L7_FALSE;

  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  index = gmrpMapTbl[intIfNum];

  if (index == 0)
    return L7_FALSE;
  
  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   * the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(gmrpCfg->intf[index].configId)) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between snoopCfgData and snoopMapTbl or
       * there is synchronization issue between NIM and components w.r.t. interface creation/deletion
       */
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_GMRP_COMPONENT_ID,
          "Error accessing GMRP config data for interface %d in gmrpMapIntfIsConfigurable."
          " A default configuration does not exist for this interface. Typically a case when"
          " a new interface is created and has no pre-configuration.", intIfNum);
      return L7_FALSE;
    }
  }
  
  *pCfg = &gmrpCfg->intf[index];

  return L7_TRUE;
}

/*********************************************************************
* @purpose      
*
* @param    L7_uint32 vlanId 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpPortStatusSet(L7_uint32 intIfNum, L7_uint32 status)
{
  #ifdef GMRP_STATIC_ENABLED /*This is a FUTURE FUNCTIONALITY*/
  L7_uchar8 key[L7_MAC_ADDR_LEN];
  L7_ushort16 vlanId;

  
  if (status == L7_ENABLE)
  {
	/*read from the config file for fixed forbidden or normal registration*/
	/*of group addresses on this port and issue GarpCommands for the same*/
	for(i=0;i<L7_MAX_GROUP_REGISTRATION_ENTRIES;i++)
	{
	  if (gmrpCfg.cfg.staticEntry[i].inUse == L7_TRUE)
	  {
		memcpy((void *)&vlanId,(void *)gmrpCfg.cfg.staticEntry[i].vlanIdmacAddress,2);
		memcpy((void *)key,(void *)&gmrpCfg.cfg.staticEntry[i].vlanIdmacAddress[2],L7_MAC_ADDR_LEN);

		if (NIM_INTF_ISMASKBITSET(gmrpCfg.cfg.staticEntry[i].fixedReg,intIfNum))
		{
		  GarpIssueCmd(GARP_GMRP_APP,GARP_REGISTRATION_FIXED,intIfNum,GARP_GMRP_MULTICAST_ATTRIBUTE,
					   key, (L7_uint32)vlanId);
		}
		else if (NIM_INTF_ISMASKBITSET(gmrpCfg.cfg.staticEntry[i].forbiddenReg,intIfNum))
        {
          GarpIssueCmd(GARP_GMRP_APP,GARP_REGISTRATION_FORBIDDEN,intIfNum,GARP_GMRP_MULTICAST_ATTRIBUTE,
					   key, (L7_uint32)vlanId);
		}
		else
		{
          GarpIssueCmd(GARP_GMRP_APP,GARP_REGISTRATION_NORMAL,intIfNum,GARP_GMRP_MULTICAST_ATTRIBUTE,
					   key, (L7_uint32)vlanId);
		}


	  }
	}

  }
  #endif
  gmrpOperCfg->gmrpEnabled[intIfNum] = status;
  gmrpCfg->hdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose      
*
* @param    L7_uint32 vlanId 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpGroupEntryRegFixed(L7_uint32 vlanId, L7_uchar8 *macAddr, L7_uint32 intIfNum)
{
  /*remove this from the forbidden list if it is there and send msg to gmrp*/
  /*for fixed registration. Also independantly set this valn Id Mac address in the MTM table*/
  
  #ifdef GMRP_STATIC_ENABLED  /*This is a FUTURE FUNCTIONALITY*/
  L7_uint32 i;
  L7_uint32 index;
  
  osapiSemaTake(gmrpCfgSema, L7_WAIT_FOREVER);

  if(L7_FAILURE==gmrpStaticEntryFind(vlanId,macAddr,&index))
  {
	/*need to create a static entry for cfg file and then proceed*/
    /*find unsed index and populate it with information*/
	if(L7_FAILURE==gmrpStaticEntryFindEmpty(&index))
	  return L7_FAILURE;/*database is full */
	else
	{
	  memcpy((void *)gmrpCfg.cfg.staticEntry[index].vlanIdmacAddress,(void *)&vlanId,2);
	  memcpy((void *)&gmrpCfg.cfg.staticEntry[index].vlanIdmacAddress[2],(void *)macAddr,L7_MAC_ADDR_LEN);
	  gmrpCfg.cfg.staticEntry[index].inUse = L7_TRUE;
    }
  }
  /*assume the we find entry*/
  if (NIM_INTF_ISMASKBITSET(gmrpCfg.cfg.staticEntry[index].forbiddenReg,intIfNum))
	NIM_INTF_CLRMASKBIT(gmrpCfg.cfg.staticEntry[index].forbiddenReg,intIfNum);

  /*set this bit in the fixed list*/
  NIM_INTF_SETMASKBIT(gmrpCfg.cfg.staticEntry[index].fixedReg, intIfNum);

  /*issue call to garp*/
  GarpIssueCmd(GARP_GMRP_APP,GARP_REGISTRATION_FIXED,intIfNum,GARP_GMRP_MULTICAST_ATTRIBUTE,
					   macAddr, (L7_uint32)vlanId);

  /*issue MTM call to add this as a part of GMRP*/

  osapiSemaGive(gmrpCfgSema);

  gmrpCfg.hdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
  #endif 
  return L7_FAILURE;
}

/*********************************************************************
* @purpose      
*
* @param    L7_uint32 vlanId 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpGroupEntryRegForbid(L7_uint32 vlanId, L7_uchar8 *macAddr, L7_uint32 intIfNum)
{ 
  /*remove this from the fixed list if it is there and send msg to gmrp*/
  /*for forbidden registration. Also independantly set this valn Id Mac address in the MTM table*/
  
  #ifdef GMRP_STATIC_ENABLED /*This is a FUTURE FUNCTIONALITY*/
  L7_uint32 i;
  L7_uint32 index;

  osapiSemaTake(gmrpCfgSema, L7_WAIT_FOREVER);

  if(L7_FAILURE==gmrpStaticEntryFind(vlanId,macAddr,&index))
  {
	/*need to create a static entry for cfg file and then proceed*/
    /*find unsed index and populate it with information*/
	if(L7_FAILURE==gmrpStaticEntryFindEmpty(&index))
	  return L7_FAILURE;/*database is full */
	else
	{
	  memcpy((void *)gmrpCfg.cfg.staticEntry[index].vlanIdmacAddress,(void *)&vlanId,2);
	  memcpy((void *)&gmrpCfg.cfg.staticEntry[index].vlanIdmacAddress[2],(void *)macAddr,L7_MAC_ADDR_LEN);
	  gmrpCfg.cfg.staticEntry[index].inUse = L7_TRUE;
    }
  }
  /*assume the we find entry*/
  if (NIM_INTF_ISMASKBITSET(gmrpCfg.cfg.staticEntry[index].fixedReg,intIfNum))
	NIM_INTF_CLRMASKBIT(gmrpCfg.cfg.staticEntry[index].fixedReg,intIfNum);

  /*set this bit in the forbidden list*/
  NIM_INTF_SETMASKBIT(gmrpCfg.cfg.staticEntry[index].forbiddenReg, intIfNum);

  /*issue call to garp*/
  GarpIssueCmd(GARP_GMRP_APP,GARP_REGISTRATION_FORBIDDEN,intIfNum,GARP_GMRP_MULTICAST_ATTRIBUTE,
					   macAddr, (L7_uint32)vlanId);

  /*issue MTM call to add this as a part of GMRP*/
  osapiSemaGive(gmrpCfgSema);

  gmrpCfg.hdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
  #endif
  return L7_FAILURE;
}

/*********************************************************************
* @purpose      
*
* @param    L7_uint32 vlanId 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpGroupEntryRegNormal(L7_uint32 vlanId, L7_uchar8 *macAddr, L7_uint32 intIfNum)
{
  /*Remove this interface from the fixed or forbidden list and send a message to*/
  /*gmrp to treat this valnid mac addr as a normally registered attribute. Also send msg to MTM*/
  /*if it was previously fixed or forbidden to delete this interface from their fixed or forbidden list*/
  /*as the case may be*/
  #ifdef GMRP_STATIC_ENABLED  /*This is a FUTURE FUNCTIONALITY*/
  L7_uint32 i;
  L7_uint32 index;
  L7_BOOL stillInUse = L7_FALSE;
  
  osapiSemaTake(gmrpCfgSema, L7_WAIT_FOREVER);

  if(L7_FAILURE==gmrpStaticEntryFind(vlanId,macAddr,&index))
  {
	/*if entry does not exist does not matter just proceed to set the interface */
	/*to normal registration*/
  }
  else/*assume the we find entry*/
  {
	/*clear this bit in the fixed list*/
	NIM_INTF_CLRMASKBIT(gmrpCfg.cfg.staticEntry[index].fixedReg,intIfNum);

	/*clear this bit in the forbidden list*/
	NIM_INTF_CLRMASKBIT(gmrpCfg.cfg.staticEntry[index].forbiddenReg, intIfNum);
	/*check to see if there are no interfaces that are fixed or forbidden - if so*/
	/*proceed to delete this entry from the static table*/
	for (i=0;i<L7_MAX_INTERFACE_COUNT;i++)
	{
	 if(NIM_INTF_ISMASKBITSET(gmrpCfg.cfg.staticEntry[index].fixedReg, intIfNum))
	   {
		stillInUse=L7_TRUE;
		break;
	   }
	 if(NIM_INTF_ISMASKBITSET(gmrpCfg.cfg.staticEntry[index].forbiddenReg, intIfNum))
	   {
		stillInUse=L7_TRUE;
		break;
	   }
	}
	/*if stillInUse remains false them we can procede to delete this entry from our cfg*/
	if(!stillInUse)
		gmrpCfg.cfg.staticEntry[index].inUse = L7_FALSE;

  }
  /*issue call to garp*/
  GarpIssueCmd(GARP_GMRP_APP,GARP_REGISTRATION_NORMAL,intIfNum,GARP_GMRP_MULTICAST_ATTRIBUTE,
					   macAddr, (L7_uint32)vlanId);

  /*issue MTM call to add this as a part of GMRP*/

  osapiSemaGive(gmrpCfgSema);

  gmrpCfg.hdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
  #endif
  return L7_FAILURE;
}
/*********************************************************************
* @purpose      
*
* @param    L7_uint32 vlanId 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpStaticFwdAllRegFixed(L7_uint32 vlanId, L7_uint32 intIfNum)
{
L7_RC_t rc = L7_FAILURE;

return rc;
}
/*********************************************************************
* @purpose      
*
* @param    L7_uint32 vlanId 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpStaticFwdAllRegForbid(L7_uint32 vlanId, L7_uint32 intIfNum)
{
L7_RC_t rc = L7_FAILURE;

return rc;
}
/*********************************************************************
* @purpose      
*
* @param    L7_uint32 vlanId 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpStaticFwdAllRegNormal(L7_uint32 vlanId, L7_uint32 intIfNum)
{
L7_RC_t rc = L7_FAILURE;

return rc;
}
/*********************************************************************
* @purpose      
*
* @param    L7_uint32 vlanId 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpStaticFwdUnRegFixed(L7_uint32 vlanId, L7_uint32 intIfNum)
{
L7_RC_t rc = L7_FAILURE;

return rc;
}
/*********************************************************************
* @purpose      
*
* @param    L7_uint32 vlanId 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpStaticFwdUnRegForbid(L7_uint32 vlanId, L7_uint32 intIfNum)
{
L7_RC_t rc = L7_FAILURE;

return rc;
}
/*********************************************************************
* @purpose      
*
* @param    L7_uint32 vlanId 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpStaticFwdUnRegNormal(L7_uint32 vlanId, L7_uint32 intIfNum)
{
L7_RC_t rc = L7_FAILURE;

return rc;
}
/*********************************************************************
* @purpose      
*
* @param    L7_uint32 vlanId 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpStaticEntryFind(L7_uint32 vlanId, L7_uchar8* MacAddr, L7_uint32 *foundIndex)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i;
  L7_ushort16 vlan_id;
  L7_uchar8 key[L7_MAC_ADDR_LEN];

  for(i=0;i<L7_MAX_GROUP_REGISTRATION_ENTRIES;i++)
  {
    if (gmrpOperCfg->staticEntry[i].inUse == L7_TRUE)
	{
      memcpy((void *)&vlan_id,(void *)gmrpOperCfg->staticEntry[i].vlanIdmacAddress,2);
      memcpy((void *)key,(void *)&gmrpOperCfg->staticEntry[i].vlanIdmacAddress[2],L7_MAC_ADDR_LEN);
      if ((vlan_id == (L7_ushort16) vlanId) && memcmp((void*)key,(void*)MacAddr,L7_MAC_ADDR_LEN)==0)
      {
        *foundIndex = i;
        rc = L7_SUCCESS;
		break;
      }
    }
  }
  
  return rc;
}

/*********************************************************************
* @purpose      
*
* @param    L7_uint32 vlanId 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpStaticEntryFindEmpty(L7_uint32 *foundIndex)
{
  L7_uint32 i;
  L7_RC_t rc = L7_FAILURE;

  for(i=0;i<L7_MAX_GROUP_REGISTRATION_ENTRIES;i++)
  {
    if (gmrpOperCfg->staticEntry[i].inUse == L7_FALSE)
    {
      *foundIndex = i;
      rc = L7_SUCCESS;
      break;
    }
  }
  return rc;
}

/*********************************************************************
* @purpose  Adds a port to the forwarding list for the vlan id group mac address entry
*
* @param    L7_uint32 	vlanId 	 Vlan Id associated
* @param	L7_uchar8 	*macAddr Group MacAddr
* @param	L7_uint32 	intIfNum The interface to be forwarded on
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This routine check if the entry exists if not it creates an entry with the
*           mFdb and then proceeds to add this port in the forwarding mask
*       
* @end
*********************************************************************/
L7_RC_t gmrpFwdPortAdd(L7_uint32 vlanId, L7_uchar8 *macAddr, L7_uint32 intIfNum)
{
   mfdbMemberInfo_t memInfo;
   mfdbUserInfo_t   userInfo;
   L7_RC_t rc = L7_FAILURE;
   L7_uchar8   vidMac[L7_MFDB_KEY_SIZE];
   
   /* check if this entry exists and this port is already there in the 
    * forwarding port list
    */
   (void)usmDbEntryVidMacCombine(vlanId, macAddr, vidMac);
   /*bzero the info structure*/
   bzero((char*)&userInfo,(L7_int32)sizeof(mfdbUserInfo_t));

   rc = mfdbEntryProtocolInfoGet(vidMac, L7_MFDB_PROTOCOL_GMRP, &userInfo);
   if (rc == L7_FAILURE)
   {
	 LOG_MSG("GMRP Entry not found in MFDB\n");
	 /*return L7_FAILURE;*/
   }

   if (L7_INTF_ISMASKBITSET(userInfo.fwdPorts, intIfNum))
   {
	 /* port is already there do not try to add it again*/
	 return L7_SUCCESS;
   }
   /*bzero the info structure*/
   bzero((char*)&memInfo,(L7_int32)sizeof(mfdbMemberInfo_t));
   /*update the structure*/
   memcpy((void *)memInfo.macAddr,(void *)macAddr,L7_MAC_ADDR_LEN);
   memInfo.vlanId = vlanId;
   memInfo.user.componentId = L7_MFDB_PROTOCOL_GMRP;
   memcpy((void *)memInfo.user.description,(void *)L7_MFDB_NETWORK_CONFIGURED,L7_MFDB_COMPONENT_DESCR_STRING_LEN);
   memInfo.user.type = L7_MFDB_TYPE_DYNAMIC;

   /*now set the mask bit in the fwd mask for this interface */
   L7_INTF_SETMASKBIT(memInfo.user.fwdPorts, intIfNum);
   
   /*check if this entry exists*/
   /*
   if(mfdbEntryExist(macAddr,vlanId)==L7_FAILURE)
	 rc = mfdbEntryAdd(&memInfo);
   else*/
	 rc = mfdbEntryPortsAdd(&memInfo);

   return rc;
}

/*********************************************************************
* @purpose  deletes a port from the vlan id group mac address entry   
*
* @param    L7_uint32 	vlanId 	 Vlan Id associated
* @param	L7_uchar8 	*macAddr Group MacAddr
* @param	L7_uint32 	intIfNum The interface to be forwarded on
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This routine proceeds to delete this interface from the forwarding mask
*       
* @end
*********************************************************************/
L7_RC_t gmrpFwdPortDelete(L7_uint32 vlanId, L7_uchar8 *macAddr, L7_uint32 intIfNum)
{
  mfdbMemberInfo_t memInfo;
   L7_RC_t rc = L7_FAILURE;

   /*bzero the info structure*/
   bzero((char*)&memInfo,(L7_int32)sizeof(mfdbMemberInfo_t));
   /*update the structure*/
   memcpy((void *)memInfo.macAddr,(void *)macAddr,L7_MAC_ADDR_LEN);
   memInfo.vlanId = vlanId;
   memInfo.user.componentId = L7_MFDB_PROTOCOL_GMRP;
   memcpy((void *)memInfo.user.description,(void *)L7_MFDB_NETWORK_CONFIGURED,L7_MFDB_COMPONENT_DESCR_STRING_LEN);
   memInfo.user.type = L7_MFDB_TYPE_DYNAMIC;

   /*now set the mask bit in the fwd mask for this interface */
   L7_INTF_SETMASKBIT(memInfo.user.fwdPorts, intIfNum);
   
   rc = mfdbEntryPortsDelete(&memInfo);

   return rc;

}
/*********************************************************************
* @purpose  This routine delete a mFdb entry     
*
* @param    L7_uint32 	vlanId 	 Vlan Id associated
* @param	L7_uchar8 	*macAddr Group MacAddr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpEntryDelete(L7_uint32 vlanId, L7_uchar8 *macAddr)
{
  mfdbMemberInfo_t memInfo;
  L7_RC_t rc = L7_FAILURE;
  L7_char8  buf[32];
  L7_uint32 i;

  /*bzero the info structure*/
  bzero((char*)&memInfo,(L7_int32)sizeof(mfdbMemberInfo_t));
  /*update the structure*/
  memcpy((void *)memInfo.macAddr,(void *)macAddr,L7_MAC_ADDR_LEN);
  memInfo.vlanId = vlanId;
  memInfo.user.componentId = L7_MFDB_PROTOCOL_GMRP;
  memcpy((void *)memInfo.user.description,(void *)L7_MFDB_NETWORK_CONFIGURED,L7_MFDB_COMPONENT_DESCR_STRING_LEN);
  memInfo.user.type = L7_MFDB_TYPE_DYNAMIC;

  /* Set up debug buffer */
  memset(buf, 0,sizeof(buf));
  for (i = 0;i < L7_MAC_ADDR_LEN ;i++)
  {
      sprintf(buf,"%02x:",memInfo.macAddr[i]);
  }
  GARP_TRACE_INTERNAL_EVENTS("Deleting GMRP entry for vlan %d and address %s\n",
                             vlanId, buf);

  rc= mfdbEntryDelete(&memInfo);
  if (rc!= L7_SUCCESS) 
  {
      LOG_MSG("GMRP failed to delete MFDB entry: vlan %d and address %s\n",
              vlanId, buf);
  }
  garpInfo.infoCounters.gmrpRegistationCount--;

  return rc;

}
/*********************************************************************
* @purpose  This checks if a given mac address is a group address or not     
*
* @param	L7_uchar8 	*macAddr MacAddr
*
* @returns  L7_SUCCESS - if it is a group mac address
* @returns  L7_FAILURE - if it is a unicast mac adddress
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpGroupAddrCheck(L7_uchar8 *macAddr)
{
  L7_uchar8 mask[1];

  mask[0] = (L7_uchar8)0x01;

  if((macAddr[0]&mask[0])&&(gmrpMacAddrCheck(macAddr)==L7_SUCCESS))
	return L7_SUCCESS;
  else
	return L7_FAILURE;

}
/*********************************************************************
* @purpose  This checks if a given mac address is a restricted address or not     
*
* @param	L7_uchar8 	*macAddr MacAddr
*
* @returns  L7_SUCCESS - if it is NOT a restricted mac address
* @returns  L7_FAILURE - if it is a restricted mac adddress
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t gmrpMacAddrCheck(L7_uchar8 *macAddr)
{
  L7_uint32 r;
    
  /*first check for all foxes*/
  if(memcmp((void *)macAddr,(void *)allFoxes,L7_MAC_ADDR_LEN) == 0)
	return L7_FAILURE;
  /*next check for all 00*/
  /*this check is taken care by this address not being a group address*/
  /*next check for the first 5 bytes of restricted mac addressess*/
  if (memcmp((void *)macAddr,(void *)firstFive,5) == 0)
  {
    /*only if there is a hit on the previous condition do we execute the below code */
	for (r=0;r<GMRP_MAX_RESTRICTED_ADDR;r++)
	{
	  if (memcmp((void *)macAddr,(void *)&gmrpRestrictedMacAddr[r][0],L7_MAC_ADDR_LEN)==0)
		return L7_FAILURE;
	}
	/*if it reaches here no match found, so return L7_SUCCESS*/
	return L7_SUCCESS;
  }
  else
	return L7_SUCCESS;



}


/***************************************************************************/
/********************************DEBUG ROUTINES*****************************/
/***************************************************************************/
#include "dll_api.h"
void gmrpEntriesGet(L7_uint32 vlanId)
{
  Gmr *my_gmr;
  L7_dll_t *local_gmd;
  L7_uint32 size;
  L7_RC_t rc;
    
  my_gmr = gmrpInstanceGet(vlanId);
  if (my_gmr == L7_NULLPTR)
  {
	LOG_MSG("Could not find a gmrp instance for this vlan id %u\n",vlanId);
	return;
  }
  local_gmd = (L7_dll_t *)my_gmr->gmd;

  rc = DLLSizeGet(local_gmd,&size);

  return;


}




void gmrpSet()
{
  L7_RC_t rc;

  rc = usmDbGarpGmrpEnableSet(1,L7_ENABLE);
  rc = usmDbGarpGmrpPortEnableSet(1,1,L7_ENABLE);
  rc = usmDbGarpGmrpPortEnableSet(1,2,L7_ENABLE);
  /*put in code for timers to test -- for the time being use default ones*/
  /*dtlDot1qGvrpGmrpConfig(1,L7_TRUE);*/
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
void gmrpBuildTestConfigData(void)
{
    L7_uint32 i, j, k;
    L7_uint32 min, max;

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/
  
    sysapiPrintf("Not building gmrpIntfCfgData_t because it is extraneous\n");


    /* Get the highest interface number of types valid for gmrp */
    (void)nimIntIfNumRangeGet(L7_LAG_INTF, &min, &max);


    /*-------------------------------------------*/
    /*   Set test staticEntry configuration      */
    /*-------------------------------------------*/

    for (i = 0; i < L7_MAX_GROUP_REGISTRATION_ENTRIES; i++ ) 
    {

        /*-------------------------------------------*/
        /*   Set unique vlanId/Mac combination       */
        /*-------------------------------------------*/

        for (j = 0; j <= 7; j++)
        {
            gmrpCfg->cfg.staticEntry[i].vlanIdmacAddress[j] = 0x10;
        }
        /* Adjust for uniqueness */
        gmrpCfg->cfg.staticEntry[i].vlanIdmacAddress[3] = j+i;
        gmrpCfg->cfg.staticEntry[i].vlanIdmacAddress[6] = j+i+1;
        gmrpCfg->cfg.staticEntry[i].vlanIdmacAddress[7] = j+i+2;


        /*-------------------------------------------*/
        /*  Set unique interface mask configuration */
        /*-------------------------------------------*/

        /* For test purposes, only turn on a subset of physical ports */
        k = 2 + i%L7_MAX_PORT_COUNT;  /* arbitrary seed */

        for (j = 1; j <= max; j++)
        {

          /*  Force each entry to have a different port list range */
          if (j <  (2*k) ) 
          {
              continue;
          }


          /*  Force each group to have a different port list range */
          if (j > (4*k) ) 
          {
              continue;
          }
          NIM_INTF_SETMASKBIT(gmrpCfg->cfg.staticEntry[i].fixedReg, ( j + i/42) ); 

          /* Do not bother to set forbidden registration since the fixed is enough
             to test config migration. */

          k++;
        }  /* j <= max */


        /*-------------------------------------------*/
        /*   Set in use flag                         */
        /*-------------------------------------------*/

        gmrpCfg->cfg.staticEntry[i].inUse = L7_TRUE;

    }  /*   L7_MAX_GROUP_REGISTRATION_ENTRIES */


    /*-------------------------------------------*/
    /*   Set test staticForwardAll configuration */
    /*-------------------------------------------*/

    for (i = 0; i < L7_MAX_VLANS; i++ ) 
    {


        gmrpCfg->cfg.staticForwardAll[i].vlanId = i;
        /*-------------------------------------------*/
        /*  Set unique interface mask configuration */
        /*-------------------------------------------*/

        /* For test purposes, only turn on a subset of physical ports */
        k = 2 + i%L7_MAX_PORT_COUNT;  /* arbitrary seed */

        for (j = 1; j <= max; j++)
        {

          /*  Force each entry to have a different port list range */
          if (j <  (2*k) ) 
          {
              continue;
          }


          /*  Force each group to have a different port list range */
          if (j > (4*k) ) 
          {
              continue;
          }
          NIM_INTF_SETMASKBIT(gmrpCfg->cfg.staticForwardAll[i].fixedReg,  j ); 

          /* Do not bother to set forbidden registration since the fixed is enough
             to test config migration. */

          k++;
        }  /* j <= max */

    }  /*   L7_MAX_VLANS */


    /*---------------------------------------------*/
    /*   Set test staticForwardUnreg configuration */
    /*---------------------------------------------*/

    for (i = 0; i < L7_MAX_VLANS; i++ ) 
    {
      
        gmrpCfg->cfg.staticForwardUnreg[i].vlanId = i;

        /*-------------------------------------------*/
        /*  Set unique interface mask configuration */
        /*-------------------------------------------*/

        /* For test purposes, only turn on a subset of physical ports */
        k = 2 + i%L7_MAX_PORT_COUNT;  /* arbitrary seed */

        for (j = 1; j <= max; j++)
        {

          /*  Force each entry to have a different port list range */
          if (j <  (2*k) ) 
          {
              continue;
          }


          /*  Force each group to have a different port list range */
          if (j > (4*k) ) 
          {
              continue;
          }
          NIM_INTF_SETMASKBIT(gmrpCfg->cfg.staticForwardUnreg[i].fixedReg,  j ); 

          /* Do not bother to set forbidden registration since the fixed is enough
             to test config migration. */

          k++;
        }  /* j <= max */
    }  /*   L7_MAX_VLANS */



    /*--------------------------------------*/
    /*   Set test gmrpEnabled configuration */
    /*--------------------------------------*/

    for (i = 0; i < L7_MAX_INTERFACE_COUNT; i++ ) 
    {  
        gmrpCfg->cfg.gmrpEnabled[i] = L7_TRUE;

    }  /*   L7_MAX_VLANS */



 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   gmrpCfg->hdr.dataChanged = L7_TRUE;  
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
void gmrpConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;
    L7_uint32 i, j;
    nimUSP_t usp;
    L7_uint32 count;

    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(gmrpCfg->hdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/
  
   sysapiPrintf( "\n");

   for (i = 0; i < L7_MAX_GROUP_REGISTRATION_ENTRIES; i++ ) 
   {

       sysapiPrintf( "staticEntry  %d\n", i);
       sysapiPrintf( "---------------\n");
       sysapiPrintf( "---------------\n");
       
       sysapiPrintf("vlanIdmacAddress = ");
       for (j = 0;j < 7 ;j++)
       {
         sysapiPrintf("%0.2x:", gmrpCfg->cfg.staticEntry[i].vlanIdmacAddress[j]);
       }

       sysapiPrintf("%0.2x:", gmrpCfg->cfg.staticEntry[i].vlanIdmacAddress[7]);


       /*-------------------------------------*/
       /*  Print fixed mask configuration */
       /*-------------------------------------*/

       sysapiPrintf( "staticEntry  %d\n", i);
       sysapiPrintf( "\n");
       sysapiPrintf("Fixed Ports - ");
       
       count = 0;
       for (j=0; j < L7_MAX_INTERFACE_COUNT; j++) 
       {
           if (NIM_INTF_ISMASKBITSET(gmrpCfg->cfg.staticEntry[i].fixedReg, j))
           {

               memset((void *)&usp, 0, sizeof(nimUSP_t));
               if (nimGetUnitSlotPort(j, &usp) == L7_SUCCESS)
               {
                   sysapiPrintf("   %d.%d.%d, ", (L7_uint32)usp.unit, (L7_int32)usp.slot, (L7_int32)usp.port);
               }

               count++;
           }  /*  L7_INTF_ISMASKBITSET */

           /* Print no more than 10 ports on a line */
           if (count%10 == 0) 
           {

               sysapiPrintf( "\n");
           }

       }/* j < L7_MAX_INTERFACE_COUNT */

       sysapiPrintf( "\n");


       /*-------------------------------------*/
       /*  Print forbidden mask configuration */
       /*-------------------------------------*/
       
       sysapiPrintf("Forbidden Ports - ");
       
       count = 0;
       for (j=0; j < L7_MAX_INTERFACE_COUNT; j++) 
       {
           if (NIM_INTF_ISMASKBITSET(gmrpCfg->cfg.staticEntry[i].forbiddenReg, j))
           {

               memset((void *)&usp, 0, sizeof(nimUSP_t));
               if (nimGetUnitSlotPort(j, &usp) == L7_SUCCESS)
               {
                   sysapiPrintf("   %d.%d.%d, ", (L7_uint32)usp.unit, (L7_int32)usp.slot, (L7_int32)usp.port);
               }

               count++;
           }  /*  L7_INTF_ISMASKBITSET */

           /* Print no more than 10 ports on a line */
           if (count%10 == 0) 
           {

               sysapiPrintf( "\n");
           }

       }/* j < L7_MAX_INTERFACE_COUNT */

       sysapiPrintf( "\n");

       /*-------------------------------------------*/
       /*   Print in use flag                       */
       /*-------------------------------------------*/

       if (gmrpCfg->cfg.staticEntry[i].inUse == L7_TRUE)
           sysapiPrintf( "staticEntry == L7_TRUE\n");
       else
           sysapiPrintf( "staticEntry == L7_FALSE\n");
   


   }  /*   L7_MAX_GROUP_REGISTRATION_ENTRIES */


    /*-------------------------------------------*/
    /*   Print staticForwardAll configuration    */
    /*-------------------------------------------*/

    for (i = 0; i < L7_MAX_VLANS; i++ ) 
    {    
         sysapiPrintf( "staticForwardAll entry  %d\n", i);
         sysapiPrintf( "---------------\n");
         sysapiPrintf( "---------------\n");

         sysapiPrintf( "vlanId =  %d\n", gmrpCfg->cfg.staticForwardAll[i].vlanId);


         /*-------------------------------------*/
         /*  Print fixed mask configuration */
         /*-------------------------------------*/

               
         sysapiPrintf("Fixed Ports - ");

         sysapiPrintf( "\n");
         count = 0;
         for (j=0; j < L7_MAX_INTERFACE_COUNT; j++) 
         {
             if (NIM_INTF_ISMASKBITSET(gmrpCfg->cfg.staticForwardAll[i].fixedReg, j))
             {

                 memset((void *)&usp, 0, sizeof(nimUSP_t));
                 if (nimGetUnitSlotPort(j, &usp) == L7_SUCCESS)
                 {
                     sysapiPrintf("   %d.%d.%d, ", (L7_uint32)usp.unit, (L7_int32)usp.slot, (L7_int32)usp.port);
                 }

                 count++;
             }  /*  L7_INTF_ISMASKBITSET */

             /* Print no more than 10 ports on a line */
             if (count%10 == 0) 
             {

                 sysapiPrintf( "\n");
             }

         }/* j < L7_MAX_INTERFACE_COUNT */

         sysapiPrintf( "\n");

         /*-------------------------------------*/
         /*  Print forbidden mask configuration */
         /*-------------------------------------*/


         sysapiPrintf("Forbidden Ports - ");
         sysapiPrintf( "\n");
         count = 0;
         for (j=0; j < L7_MAX_INTERFACE_COUNT; j++) 
         {
             if (NIM_INTF_ISMASKBITSET(gmrpCfg->cfg.staticForwardAll[i].forbiddenReg, j))
             {

                 memset((void *)&usp, 0, sizeof(nimUSP_t));
                 if (nimGetUnitSlotPort(j, &usp) == L7_SUCCESS)
                 {
                     sysapiPrintf("   %d.%d.%d, ", (L7_uint32)usp.unit, (L7_int32)usp.slot, (L7_int32)usp.port);
                 }

                 count++;
             }  /*  L7_INTF_ISMASKBITSET */

             /* Print no more than 10 ports on a line */
             if (count%10 == 0) 
             {

                 sysapiPrintf( "\n");
             }

         }/* j < L7_MAX_INTERFACE_COUNT */

         sysapiPrintf( "\n");


    }  /*   L7_MAX_VLANS */




    /*-------------------------------------------*/
    /*   Print staticForwardUnreg configuration  */
    /*-------------------------------------------*/

    for (i = 0; i < L7_MAX_VLANS; i++ ) 
    {    
         sysapiPrintf( "staticForwardUnreg entry  %d\n", i);
         sysapiPrintf( "---------------\n");
         sysapiPrintf( "---------------\n");

         sysapiPrintf( "vlanId =  %d\n", gmrpCfg->cfg.staticForwardUnreg[i].vlanId);


         /*-------------------------------------*/
         /*  Print fixed mask configuration */
         /*-------------------------------------*/

               
         sysapiPrintf("Fixed Ports - ");
         sysapiPrintf( "\n");
         count = 0;
         for (j=0; j < L7_MAX_INTERFACE_COUNT; j++) 
         {
             if (NIM_INTF_ISMASKBITSET(gmrpCfg->cfg.staticForwardUnreg[i].fixedReg, j))
             {

                 memset((void *)&usp, 0, sizeof(nimUSP_t));
                 if (nimGetUnitSlotPort(j, &usp) == L7_SUCCESS)
                 {
                     sysapiPrintf("   %d.%d.%d, ", (L7_uint32)usp.unit, (L7_int32)usp.slot, (L7_int32)usp.port);
                 }

                 count++;
             }  /*  L7_INTF_ISMASKBITSET */

             /* Print no more than 10 ports on a line */
             if (count%10 == 0) 
             {

                 sysapiPrintf( "\n");
             }

         }/* j < L7_MAX_INTERFACE_COUNT */

         sysapiPrintf( "\n");


         /*-------------------------------------*/
         /*  Print forbidden mask configuration */
         /*-------------------------------------*/


         sysapiPrintf("Forbidden Ports - ");
         sysapiPrintf( "\n");
         count = 0;
         for (j=0; j < L7_MAX_INTERFACE_COUNT; j++) 
         {
             if (NIM_INTF_ISMASKBITSET(gmrpCfg->cfg.staticForwardUnreg[i].forbiddenReg, j))
             {

                 memset((void *)&usp, 0, sizeof(nimUSP_t));
                 if (nimGetUnitSlotPort(j, &usp) == L7_SUCCESS)
                 {
                     sysapiPrintf("   %d.%d.%d, ", (L7_uint32)usp.unit, (L7_int32)usp.slot, (L7_int32)usp.port);
                 }

                 count++;
             }  /*  L7_INTF_ISMASKBITSET */

             /* Print no more than 10 ports on a line */
             if (count%10 == 0) 
             {

                 sysapiPrintf( "\n");
             }

         }/* j < L7_MAX_INTERFACE_COUNT */

         sysapiPrintf( "\n");


    }  /*   L7_MAX_VLANS */


    /*-------------------------------------------*/
    /*   Print gmrpEnabled                       */
    /*-------------------------------------------*/

    sysapiPrintf( "List of gmrpEnabled Ports: %d\n");
    sysapiPrintf( "---------------\n");
    sysapiPrintf( "---------------\n");


    for (i = 0; i < L7_MAX_INTERFACE_COUNT; i++ ) 
    {    
          if (gmrpCfg->cfg.gmrpEnabled[i] == L7_TRUE) 
         {

             memset((void *)&usp, 0, sizeof(nimUSP_t));
             (void) nimGetUnitSlotPort(i, &usp);
             sysapiPrintf("   %d.%d.%d, ", (L7_uint32)usp.unit, (L7_int32)usp.slot, (L7_int32)usp.port);
         }
    }


    sysapiPrintf( "\n");

    
   /*-------------------------------*/
   /* Scaling Constants             */
   /*-------------------------------*/

    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "L7_MAX_GROUP_REGISTRATION_ENTRIES - %d\n", L7_MAX_GROUP_REGISTRATION_ENTRIES);
    sysapiPrintf( "L7_MAX_VLANS - %d\n", L7_MAX_VLANS);
    sysapiPrintf( "L7_MAX_INTERFACE_COUNT - %d\n", L7_MAX_INTERFACE_COUNT);
    sysapiPrintf( "sizeof(NIM_INTF_MASK_t) - %d\n", sizeof(NIM_INTF_MASK_t));



    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("gmrpCfg->checkSum : %u\n", gmrpCfg->checkSum);


}


/*********************************************************************
* @purpose  Saves gmrp configuration
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
void gmrpSaveTestConfigData(void)
{
  
  gmrpCfg->hdr.dataChanged = L7_FALSE;
  gmrpCfg->checkSum = nvStoreCrc32((L7_uchar8 *)gmrpCfg,
                                   (L7_uint32)(sizeof(gmrpCfg_t) - sizeof(gmrpCfg->checkSum)));                      

  if ( (sysapiCfgFileWrite(L7_GMRP_COMPONENT_ID, GMRP_CFG_FILENAME, (L7_char8*)gmrpCfg, (L7_int32)sizeof(gmrpCfg_t))) != L7_SUCCESS)
  {
    LOG_MSG("Error on call to sysapiCfgFileWrite routine on config file %s\n",GMRP_CFG_FILENAME);
  }

}



/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/



