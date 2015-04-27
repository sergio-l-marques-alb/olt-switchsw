/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  vlan_ipsubnet_api.c
*
* @purpose   IP Subnet Vlan  API file
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
#include "vlan_ipsubnet_api.h"

/* Begin Function Declarations: vlan_ipsubnet_api.h */

/*********************************************************************
 * @purpose  To assign a subnet to a VLAN.
 *          
 * @param    subnet      @b{(input)}    IP Subnet
 * @param    netmask     @b{(input)}    network mask 
 * @param    vlanId      @b{(input)}    vlan ID
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE  
 *
 * @end
 *********************************************************************/
L7_RC_t vlanIpSubnetSubnetAdd(L7_IP_ADDR_t subnet, L7_IP_ADDR_t netmask, L7_uint32 vlanId)
{
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 tempVlanId;
    
    /* Check for validity of VLAN identifier */
    if (vlanId < L7_DOT1Q_MIN_VLAN_ID  || vlanId > L7_DOT1Q_MAX_VLAN_ID)
        return(rc);

    /* IP address validation */
    subnet &= netmask;

    if (vlanIpSubnetSubnetValid(subnet, netmask) == L7_FALSE)
    {
		L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_VLAN_IPSUBNET_COMPONENT_ID, "ERROR vlanIpSubnetSubnetValid :"
								"Invalid subnet - subnet = %x netmask = %x. This occurs when an invalid"
								" pair of subnet and netmask has come from the CLI.\n",subnet,netmask);
        return(rc);
    }

    rc = vlanIpSubnetSubnetGet(subnet,netmask, &tempVlanId);
    if(rc == L7_SUCCESS)
    {
        if(tempVlanId == vlanId)
        { 
            return (rc);
        }
        else
        {
            vlanIpSubnetSubnetDelete(subnet, netmask, tempVlanId);
        }
    }

    /* Apply the configuration and update the configuration file accordingly */
    osapiWriteLockTake(vlanIpSubnetCfgRWLock, L7_WAIT_FOREVER);

    if (vlanIpSubnetDataCount(vlanIpSubnetTreeData) < L7_VLAN_IPSUBNET_MAX_VLANS)
    {
        /* Add the new vlan */
        vlanIpSubnetSubnetAddApply(subnet, netmask, vlanId);
        /* Save the configuration */
        vlanIpSubnetCfgEntryAdd(subnet, netmask, vlanId);
        /* Set the data-changed flag */
        vlanIpSubnetCfg->cfgHdr.dataChanged = L7_TRUE;
        rc = L7_SUCCESS;
    }

    osapiWriteLockGive(vlanIpSubnetCfgRWLock);
    return(rc);
}

/*********************************************************************
* @purpose  To delete a subnet assignment to a VLAN.
*          
* @param    subnet      @b{(input)}    IP Subnet
* @param    netmask     @b{(input)}    network mask 
* @param    vlanId      @b{(input)}    vlan ID
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t vlanIpSubnetSubnetDelete(L7_IP_ADDR_t subnet, L7_IP_ADDR_t netmask, L7_uint32 vlanId)
{
    vlanIpSubnetOperData_t *node;
    L7_RC_t rc = L7_FAILURE;

    /* Check for validity of VLAN identifier */
    if (vlanId < L7_DOT1Q_MIN_VLAN_ID  || vlanId > L7_DOT1Q_MAX_VLAN_ID)
        return (rc);

    subnet &= netmask;
    /* Check for validity of IP address and mask */
    if (vlanIpSubnetSubnetValid(subnet, netmask) == L7_FALSE)
        return(rc);


    /* Save the updated configuration */
    /* Set the data-changed flag */
    
    osapiWriteLockTake(vlanIpSubnetCfgRWLock, L7_WAIT_FOREVER);
    node = (vlanIpSubnetOperData_t *) vlanIpSubnetDataSearch(vlanIpSubnetTreeData, subnet, netmask);
    if (node != L7_NULLPTR)
    {
           vlanIpSubnetSubnetDeleteApply(subnet, netmask, vlanId);
           vlanIpSubnetCfgEntryDelete(subnet, netmask, vlanId);
           vlanIpSubnetCfg->cfgHdr.dataChanged = L7_TRUE;
           rc = L7_SUCCESS;
        }
    

    osapiWriteLockGive(vlanIpSubnetCfgRWLock);

    return(rc);
}


/*********************************************************************
* @purpose  To get the  subnet-vlan association in the list 
*          
* @param    subnet      @b{(input)}    IP Subnet
* @param    netmask     @b{(input)}    network mask
* @param    *vlanId      @b{(output)}  vlan ID associated with next subnet
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* 
* @comments Multiple subnets can be associated with the same VLAN
* @comments To get the first subnet, pass in a  "0" for the subnet value
*
* @end
*********************************************************************/
L7_RC_t vlanIpSubnetSubnetGetNext(L7_IP_ADDR_t subnet, L7_IP_ADDR_t netmask, 
                                 L7_IP_ADDR_t *nextSubnet, L7_IP_ADDR_t *nextMask, 
                                 L7_uint32 *vlanId)
{
    vlanIpSubnetOperData_t *node;
    L7_RC_t rc = L7_FAILURE;


    osapiReadLockTake(vlanIpSubnetCfgRWLock, L7_WAIT_FOREVER);
    if (vlanIpSubnetDataCount(vlanIpSubnetTreeData) <= 0)
    {
        osapiReadLockGive(vlanIpSubnetCfgRWLock);
        return(rc);
    }

    if ((subnet == 0) && (netmask == 0))
    
    {
        node = vlanIpSubnetFirstDataEntry(vlanIpSubnetTreeData);
        if (node == L7_NULL) 
        {
            osapiReadLockGive(vlanIpSubnetCfgRWLock);
            *nextSubnet = 0;
            *nextMask = 0;
            *vlanId = 0;
            return(rc);
        }
    }
    else
    {
        if (vlanIpSubnetSubnetValid(subnet, netmask) == L7_FALSE)
        {
            osapiReadLockGive(vlanIpSubnetCfgRWLock);
            return(rc);
        }

        node = vlanIpSubnetNextDataEntry(vlanIpSubnetTreeData, subnet, netmask);
        if (node == L7_NULL)
        {
            osapiReadLockGive(vlanIpSubnetCfgRWLock);
            *nextSubnet = 0;
            *nextMask = 0;
            *vlanId = 0;
            return(rc);
        }
        
    }
    osapiReadLockGive(vlanIpSubnetCfgRWLock);
    *nextSubnet = node->ipSubnet;
    *nextMask = node->netMask;
    *vlanId = node->vlanId;
    rc = L7_SUCCESS;
    return(rc);
}

/*********************************************************************
* @purpose  To get the  subnet-vlan association in the list 
*          
* @param    subnet      @b{(input)}    IP Subnet
* @param    netmask     @b{(input)}    network mask
* @param    vlanId      @b{(output)}  vlan ID associated with next subnet
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* 
* @end
*********************************************************************/
L7_RC_t vlanIpSubnetSubnetGet(L7_IP_ADDR_t subnet, L7_IP_ADDR_t netmask, L7_uint32 *vlanId)
{
    vlanIpSubnetOperData_t *node;
    L7_RC_t rc = L7_FAILURE;

    osapiReadLockTake(vlanIpSubnetCfgRWLock, L7_WAIT_FOREVER);

    if (vlanIpSubnetDataCount(vlanIpSubnetTreeData) <= 0)
    {
        osapiReadLockGive(vlanIpSubnetCfgRWLock);
        return(rc);
    }
    
    subnet &= netmask;
    node =  vlanIpSubnetDataEntry(vlanIpSubnetTreeData, subnet, netmask);
    
    if (node == L7_NULL)
    {
        rc = L7_FAILURE;
    }

    else
    {
        *vlanId = node->vlanId;
        rc = L7_SUCCESS;
    }
    
    osapiReadLockGive(vlanIpSubnetCfgRWLock);
    return rc;
}

/*********************************************************************
* @purpose  To Check for the subnet conflict
*
* @param    subnet      @b{(input)}    IP Subnet
* @param    netmask     @b{(input)}    network mask
* @param    vlanId      @b{(input)}    vlan ID
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL vlanIpSubnetCfgConflictCheck(L7_uint32 subnet, L7_uint32 netmask, L7_uint32 vlanId)
{
    int cfgIndex;
    L7_BOOL rc = L7_FALSE;

    osapiReadLockTake( vlanIpSubnetCfgRWLock, L7_WAIT_FOREVER);
    for (cfgIndex = 0; cfgIndex<L7_VLAN_IPSUBNET_MAX_VLANS; cfgIndex++)
        if (vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].ipSubnet &&
            vlanIpSubnetFindSubnetOverlap(subnet, netmask,    
                   vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].ipSubnet, 
                   vlanIpSubnetCfg->vlanIpSubnetCfgData[cfgIndex].netMask) 
                                                               == L7_TRUE)
        {
            rc = L7_TRUE;
            break;
        }

    osapiReadLockGive(vlanIpSubnetCfgRWLock);
    return (rc);
}


/* End Function Declarations */
