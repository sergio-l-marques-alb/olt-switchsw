/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_ipsubnet_cfg.c
*
* @purpose   IP Subnet Vlan  configuration file
*
* @component vlanIpSubnet
*
* @comments
*
* @create    5/5/2005
*
* @author    tsrikanth
*
* @end
*
**********************************************************************/

#include "vlan_ipsubnet_include.h"

vlanIpSubnetCfg_t *vlanIpSubnetCfg = L7_NULLPTR;
osapiRWLock_t vlanIpSubnetCfgRWLock;

/* Begin Function Declarations: vlan_ipsubnet_cfg.h */

/*********************************************************************
* @purpose  Builds the cfg file with default parameter values
*
* @param    ver     @@b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void vlanIpSubnetBuildDefaultConfigData(L7_uint32 ver)
{
    memset((void *)vlanIpSubnetCfg, 0, sizeof(vlanIpSubnetCfg_t));
    strcpy((L7_char8 *)vlanIpSubnetCfg->cfgHdr.filename, VLAN_IPSUBNET_CFG_FILENAME);
    vlanIpSubnetCfg->cfgHdr.version = ver;
    vlanIpSubnetCfg->cfgHdr.componentID = L7_VLAN_IPSUBNET_COMPONENT_ID;
    vlanIpSubnetCfg->cfgHdr.type = L7_CFG_DATA;
    vlanIpSubnetCfg->cfgHdr.length = (L7_uint32) sizeof(vlanIpSubnetCfg_t);
    vlanIpSubnetCfg->cfgHdr.dataChanged = L7_FALSE;
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
L7_BOOL vlanIpSubnetHasDataChanged()
{
    return vlanIpSubnetCfg->cfgHdr.dataChanged;
}
void vlanIpSubnetResetDataChanged(void)
{
  vlanIpSubnetCfg->cfgHdr.dataChanged = L7_FALSE;
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
L7_RC_t vlanIpSubnetRestore()
{
    /* Remove */
    vlanIpSubnetCfgRemove();
    /* Build defaults */
    vlanIpSubnetBuildDefaultConfigData(VLAN_IPSUBNET_CFG_VER_CURRENT);
    /* Apply defaults */
    vlanIpSubnetApplyConfigData();

    return L7_SUCCESS;
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
L7_RC_t vlanIpSubnetSave()
{
    L7_RC_t rc = L7_SUCCESS;

    if (vlanIpSubnetCfg->cfgHdr.dataChanged == L7_TRUE)
    {
        vlanIpSubnetCfg->cfgHdr.dataChanged = L7_FALSE;
        vlanIpSubnetCfg->checkSum = nvStoreCrc32((L7_char8 *)vlanIpSubnetCfg,
                                                 ((L7_uint32)sizeof(vlanIpSubnetCfg_t) -
                                                  (L7_uint32)sizeof(vlanIpSubnetCfg->checkSum)));

        if ( (rc = sysapiCfgFileWrite(L7_VLAN_IPSUBNET_COMPONENT_ID, VLAN_IPSUBNET_CFG_FILENAME,
                                      (L7_char8 *) vlanIpSubnetCfg, (L7_int32)sizeof(vlanIpSubnetCfg_t))) != L7_SUCCESS )
        {
            rc = L7_ERROR;
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_VLAN_IPSUBNET_COMPONENT_ID, "IP Subnet Vlans: failed to save "
                        "configuration. This message appears when save configuration of subnet vlans failed.");
        }
    }
    return rc;
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
L7_RC_t vlanIpSubnetApplyConfigData()
{
    L7_uint32 cfgIndex = 0;

    for (cfgIndex = 0; cfgIndex < L7_VLAN_IPSUBNET_MAX_VLANS; cfgIndex++)
    {
        if (vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].ipSubnet != 0)
        {
            vlanIpSubnetSubnetAddApply(vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].ipSubnet,
                                       vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].netMask,
                                       vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].vlanId);
        }

    }
    return L7_SUCCESS;
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
void vlanIpSubnetCfgRemove()
{
    L7_uint32 cfgIndex;

    for (cfgIndex = 0; cfgIndex < L7_VLAN_IPSUBNET_MAX_VLANS; cfgIndex++)
    {
        if (vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].ipSubnet != 0)
        {
            vlanIpSubnetSubnetDeleteApply(vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].ipSubnet                                        ,vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].netMask,
                                          vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].vlanId);
        }
    }
}

/* End Function Declarations */
