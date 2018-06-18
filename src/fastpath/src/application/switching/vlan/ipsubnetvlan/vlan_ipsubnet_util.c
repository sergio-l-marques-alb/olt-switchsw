/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_ipsubnet_util.c
*
* @purpose   IP Subnet Vlan utility file
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

#include "cli_txt_cfg_api.h"
#include "vlan_ipsubnet_include.h"

/* Begin Function Declarations: vlan_ipsubnet_util.h */

/*********************************************************************
* @purpose  To assign a subnet to a VLAN.
*
* @param    subnet      @b{(input)}    IP Subnet
* @param    netmask     @b{(input)}    network mask
* @param    vlanId      @b{(input)}    vlan ID
*
* @returns void
*
* @end
*********************************************************************/
void vlanIpSubnetSubnetAddApply(L7_uint32 subnet, L7_uint32 netmask, L7_uint32 vlanId)
{
    vlanIpSubnetOperData_t node ;

    memset(&node, 0, sizeof(vlanIpSubnetOperData_t));
    node.ipSubnet = subnet;
    node.netMask = netmask;
    node.vlanId = vlanId;
    vlanIpSubnetEntryAdd(vlanIpSubnetTreeData, &node);

  if (dtlVlanIpSubnetCreate(subnet, netmask, vlanId) != L7_SUCCESS)
   {
       L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_VLAN_IPSUBNET_COMPONENT_ID, "vlanIpSubnetDtlVlanCreate: Failed. This "
                                                "appears when a dtl call fails to add an entry into the table.");
   }

}

/*********************************************************************
* @purpose  To delete a subnet assignment to a VLAN.
*
* @param    subnet      @b{(input)}    IP Subnet address
* @param    netmask     @b{(input)}    network mask
* @param    vlanId      @b{(input)}    vlan ID
*
* @returns void
*
* @end
*********************************************************************/
void vlanIpSubnetSubnetDeleteApply(L7_uint32 subnet, L7_uint32 netmask, L7_uint32 vlanId)
{
    vlanIpSubnetOperData_t *node = L7_NULLPTR;

    node = (vlanIpSubnetOperData_t *) vlanIpSubnetDataSearch(vlanIpSubnetTreeData, subnet, netmask);
    if (node != L7_NULLPTR)
    {
        vlanIpSubnetEntryDelete(vlanIpSubnetTreeData, subnet, netmask);
        /* Synchronous processing by a call to DTL */
        if (dtlVlanIpSubnetDelete(subnet, netmask, vlanId) != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_VLAN_IPSUBNET_COMPONENT_ID, "vlanIpSubnetSubnetDeleteApply: Failed. "
                                                 "This appears when a dtl fails to delete an entry from the table.");
        }
    }

}

/*********************************************************************
* @purpose  To add an entry to the configuration file
*
* @param    subnet      @b{(input)}    IP Subnet
* @param    netmask     @b{(input)}    network mask
* @param    vlanId      @b{(input)}    vlan ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
void vlanIpSubnetCfgEntryAdd(L7_uint32 subnet, L7_uint32 netmask, L7_uint32 vlanId)
{
    L7_uint32 cfgIndex;

    for (cfgIndex = 0; cfgIndex < L7_VLAN_IPSUBNET_MAX_VLANS; cfgIndex++)
        if (vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].ipSubnet == 0)
        {
            vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].ipSubnet = subnet;
            vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].netMask = netmask;
            vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].vlanId = vlanId;
            break;
        }
}

/*********************************************************************
* @purpose  To delete an entry from the configuration file
*
* @param    subnet      @b{(input)}    IP Subnet
* @param    netmask     @b{(input)}    network mask
* @param    vlanId      @b{(input)}    vlan ID
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void vlanIpSubnetCfgEntryDelete(L7_uint32 subnet, L7_uint32 netmask, L7_uint32 vlanId)
{
    L7_uint32  cfgIndex;

    for (cfgIndex = 0; cfgIndex < L7_VLAN_IPSUBNET_MAX_VLANS; cfgIndex++)
        if ( (vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].ipSubnet == subnet) &&
           (vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].netMask == netmask) )
        {
            memset( &vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex], 0,
                    sizeof(vlanIpSubnetCfgData_t));
            break;
        }
}


/*********************************************************************
* @purpose  Callback function to process VLAN changes.
*
* @param    vlanId     @b{(input)}   VLAN ID
* @param    intIfnum   @b{(input)}   internal interface whose state has changed
* @param    event      @b{(input)}   VLAN event (see vlanNotifyEvent_t for list)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t vlanIpSubnetVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event)
{
    return(L7_SUCCESS);
}

/*********************************************************************
* @purpose Function registered with Text Based configuration.  It is
*          invoked when all text-based configuration has been pushed
*          to components.
*
* @param   L7_uint32   event  indicates status of text config completion
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 vlanIpTxtCfgApplyCompletionCallback(L7_uint32 event)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Ignore events types we don't care about.  */
  if ( (event != TXT_CFG_APPLY_FAILURE) &&
       (event != TXT_CFG_APPLY_SUCCESS) )
  {
    return L7_SUCCESS;
  }

  /* signal configurator that this component is ready for driver sync. */
  cnfgrApiComponentHwUpdateDone(L7_VLAN_IPSUBNET_COMPONENT_ID, L7_CNFGR_HW_APPLY_CONFIG);
  return(rc);
}

/*********************************************************************
 * @purpose  Verify a mask is contiguous one's flush left
 *
 * @param    netmask     @b{(input)}    network mask
 * @param    numOfOnes   @b{(output)}   Num of Ones in the mask
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @end
 *********************************************************************/
L7_BOOL vlanIpSubnetIsNetMaskContig(L7_uint32 netmask, L7_uint32 *numOfOnes)
{
    L7_uint32 i = 0 , rval;

    for (i = 0; VLAN_IPSUBNET_ISBITOFF(netmask,i) && i<32;)
        i++;

    rval = 0;
    for (; VLAN_IPSUBNET_ISBITON(netmask,i) && i<32; i++)
        rval++;

    if (i < 32)
    {
        rval = 0;
        *numOfOnes = rval;
        return(L7_FALSE);
    }
    *numOfOnes = rval;
    return(L7_TRUE);
}

/*********************************************************************
 * @purpose  Given a subnet IP address, find it is valid unicast IP's
 *
 * @param    subnetip    @b{(input)}    subnet address
 * @param    netmask     @b{(input)}    network mask
 *
 * @returns  L7_TRUE     Valid
 * @returns  L7_FALSE    Invalid
 *
 * @end
 *********************************************************************/
L7_BOOL vlanIpSubnetSubnetValid(L7_uint32 subnetip, L7_uint32 netmask)
{
    L7_uint32 noofbits, subnet;

    if (subnetip == 0 || netmask == 0)
        return(L7_FALSE);

    subnet = osapiNtohl(subnetip);

    if ((vlanIpSubnetIsNetMaskContig(netmask, &noofbits)) == L7_TRUE)
    {
      /* allow any subnet as long as the netmask is contiguous */
      return L7_TRUE;
/*
        if (noofbits)
        {
            if ((subnet >= VLAN_IPSUBNET_IPADDR_CLASSA_BEG) &&
                                  (subnet <= VLAN_IPSUBNET_IPADDR_CLASSA_END))
            {
                if (noofbits >= 8)
                    return L7_TRUE;
            }
            if ((subnet >= VLAN_IPSUBNET_IPADDR_CLASSB_BEG) &&
                                  (subnet <= VLAN_IPSUBNET_IPADDR_CLASSB_END))
            {
                if (noofbits >= 16)
                    return L7_TRUE;
            }
            if ((subnet >= VLAN_IPSUBNET_IPADDR_CLASSC_BEG) &&
                                  (subnet <= VLAN_IPSUBNET_IPADDR_CLASSC_END))
            {
                if (noofbits >= 24)
                    return L7_TRUE;
            }
        }
        else
            return L7_FALSE;
*/
    }

    return(L7_FALSE);
}

/*********************************************************************
 * @purpose  Given an IP address and netmask, find subnet
 *
 * @param    subnetip    @b{(input)}    IP address
 * @param    netmask     @b{(input)}    network mask
 *
 * @returns  Subnet address
 *
 * @end
 *********************************************************************/
L7_uint32 vlanIpSubnetFindSubnet(L7_uint32 ipaddr, L7_uint32 netmask)
{
    L7_uint32 subnetAddr = L7_NULL;

    if (vlanIpSubnetSubnetValid(ipaddr, netmask) == L7_TRUE)
        /*
        * As it is a processor-sensitive it is the right way
        * to convert bytes to host order first and back to network
        * order for binary or arithmetic operatons.
        */
        subnetAddr = (osapiHtonl(osapiNtohl(ipaddr)&osapiNtohl(netmask)));
    return(subnetAddr);
}

/*********************************************************************
 * @purpose   Given two subnets, find they overlap anywhere
 *
 * @param    subnet1     @b{(input)}    IP address
 * @param    netmask1    @b{(input)}    network mask
 * @param    subnet2     @b{(input)}    IP address
 * @param    netmask2    @b{(input)}    network mask
 *
 * @returns  L7_TRUE
 * @returns  L7_FALSE
 *
 * @end
 *********************************************************************/
L7_BOOL
vlanIpSubnetFindSubnetOverlap(L7_uint32 subnet1, L7_uint32 netmask1,
                              L7_uint32 subnet2, L7_uint32 netmask2)
{
    if (vlanIpSubnetSubnetValid(subnet1, netmask1) == L7_TRUE &&
        vlanIpSubnetSubnetValid(subnet2, netmask2) == L7_TRUE &&
        (subnet1&netmask1&netmask2) == (subnet2&netmask2&netmask1))
    {
        return(L7_TRUE);
    }

    return(L7_FALSE);
}

/* End Function Declarations */
