/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_mac_cfg.c
*
* @purpose   Mac Vlan  Configuration file
*
* @component vlanMac
*
* @comments
*
* @create    05/20/2005
*
* @author    
*
* @end
*
**********************************************************************/

#include "vlan_mac_include.h"

vlanMacCfg_t *vlanMacCfg = L7_NULLPTR;

/* Begin Function Declarations: vlan_mac_cfg.h */

/*********************************************************************
* @purpose  Builds the cfg file with default parameter values
*          
* @param    ver    @b{(input)}  Software version of Config Data
*
* @returns  void  
*
* @comments updates the port structure too.
*
* @end
*********************************************************************/
void vlanMacBuildDefaultConfigData(L7_uint32 ver)
{
    memset((void *) vlanMacCfg, 0x00, sizeof(vlanMacCfg_t));
    strcpy((L7_char8 *) vlanMacCfg->cfgHdr.filename, VLAN_MAC_CFG_FILENAME);
    vlanMacCfg->cfgHdr.version = ver;
    vlanMacCfg->cfgHdr.componentID = L7_VLAN_MAC_COMPONENT_ID;
    vlanMacCfg->cfgHdr.type = L7_CFG_DATA;
    vlanMacCfg->cfgHdr.length = (L7_uint32) sizeof(vlanMacCfg_t);
    vlanMacCfg->cfgHdr.dataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  Has the data in the cfg file changed
*          
* @param    none 
*
* @returns  L7_TRUE  
* @returns  L7_FALSE  
*
* @comments 
*
* @end
*********************************************************************/
L7_BOOL vlanMacHasDataChanged()
{     
    return vlanMacCfg->cfgHdr.dataChanged;
}
void vlanMacResetDataChanged(void)
{
  vlanMacCfg->cfgHdr.dataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Restores the default configuration
*          
* @param    none 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t vlanMacRestore()
{
    /* Remove */
    vlanMacCfgRemove();

    /* Build defaults */
    vlanMacBuildDefaultConfigData(VLAN_MAC_CFG_VER_CURRENT);

    /* Apply defaults */
    vlanMacApplyConfigData();
    return(L7_SUCCESS);
}

/*********************************************************************
* @purpose  Saves the current configuration
*          
* @param    none 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t vlanMacSave()
{
    L7_RC_t rc = L7_SUCCESS;

    if (vlanMacCfg->cfgHdr.dataChanged == L7_TRUE)
    {
        vlanMacCfg->cfgHdr.dataChanged = L7_FALSE;
        vlanMacCfg->checkSum = nvStoreCrc32((L7_char8 *)vlanMacCfg, 
                                            ((L7_uint32)sizeof(vlanMacCfg_t) - (L7_uint32)sizeof(vlanMacCfg->checkSum)));

        rc = sysapiCfgFileWrite(L7_VLAN_MAC_COMPONENT_ID, VLAN_MAC_CFG_FILENAME, 
                                (L7_char8 *)vlanMacCfg, (L7_int32)sizeof(vlanMacCfg_t));
        if (rc != L7_SUCCESS)
        {
            rc = L7_ERROR;
			L7_LOG(L7_LOG_SEVERITY_NOTICE, L7_VLAN_MAC_COMPONENT_ID, "MAC VLANs: Failed to save configuration. "
										     "This message appears when save configuration of Mac vlans failed.");
        }
    }
    return(rc);
}

/*********************************************************************
* @purpose  Applies the configured saved configuration
*          
* @param    none 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t vlanMacApplyConfigData()
{
/* To eliminate dual configuration being applied and to remove duplicate entries 
   in the hardware table, ( one is in Component configuration and the other is when 
   VLAN based notifications) a bool flag is added in the vlanMacOperData_t to tell whether the 
  entry is operationally enabled. During the component configuration time, 
  it is set to TRUE and it is set to TRUE/FALSE during vlan notifications
*/

    L7_enetMacAddr_t tmpmac;
    L7_uint32 cfgIndex;

    memset(tmpmac.addr, 0x00, sizeof tmpmac);
	osapiWriteLockTake(macVlanCfgRWLock, L7_WAIT_FOREVER);
    for (cfgIndex = 0; cfgIndex < L7_VLAN_MAC_MAX_VLANS; cfgIndex++)
    {
        if (memcmp(vlanMacCfg->macVlanInfo[cfgIndex].mac.addr, tmpmac.addr, sizeof tmpmac))
        {
            vlanMacAddApply(vlanMacCfg->macVlanInfo[cfgIndex].mac, 
                            vlanMacCfg->macVlanInfo[cfgIndex].vlanId);
        }
    }
	osapiWriteLockGive(macVlanCfgRWLock);
    return (L7_SUCCESS);
}

/*********************************************************************
* @purpose  Remove any configured vlan interface
*          
* @param    none 
*
* @returns  void
*
* @comments 
*
* @end
*********************************************************************/
void vlanMacCfgRemove()
{
    L7_enetMacAddr_t tmpmac;
    L7_uint32 cfgIndex;

    memset(tmpmac.addr, 0x00, sizeof tmpmac);
	osapiWriteLockTake(macVlanCfgRWLock, L7_WAIT_FOREVER);
    for (cfgIndex = 0; cfgIndex < L7_VLAN_MAC_MAX_VLANS; cfgIndex++)
    {
        if (memcmp(vlanMacCfg->macVlanInfo[cfgIndex].mac.addr, tmpmac.addr, sizeof tmpmac))
        {
            vlanMacDeleteApply(vlanMacCfg->macVlanInfo[cfgIndex].mac, 
                               vlanMacCfg->macVlanInfo[cfgIndex].vlanId);
        }
    }
	osapiWriteLockGive(macVlanCfgRWLock);
}

/* End Function Declarations */
