/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_mac_api.c
*
* @purpose   MAC Vlan  API file
*
* @component vlanMac
*
* @comments
*
* @create    5/20/2005
*
* @author    tsrikanth
*
* @end
*
**********************************************************************/

#include "vlan_mac_include.h"
#include "vlan_mac_api.h"

extern macvlanTree_t *macVlanTreeData;
extern macvlanTreeTables_t *macVlanTreeHeap;

/* Begin Function Declarations: vlan_mac_api.h */

/*********************************************************************
 * @purpose  To assign a MAC to a VLAN.
 *          
 * @param    mac         @b{(input)}    Mac Address
 * @param    vlanId      @b{(input)}    vlan ID
 *
 * @returns  L7_SUCCESS  
 * @returns  L7_FAILURE  
 *
 * @end
 *********************************************************************/
L7_RC_t vlanMacAdd(L7_enetMacAddr_t mac, L7_uint32 vlanId)
{
/*    vlanMacCfgData_t *nodep;*/
    L7_RC_t rc = L7_FAILURE;
    L7_uint32 tempVlanId;

    /* Check for validity of VLAN identifier */
    if ((vlanId < L7_DOT1Q_MIN_VLAN_ID) || (vlanId > L7_DOT1Q_MAX_VLAN_ID))
        return(rc);


    rc = vlanMacGet(mac,&tempVlanId);

    if(rc == L7_SUCCESS)
    {
        if(tempVlanId == vlanId)
        {  
            return (rc);
        }
        else
            vlanMacDelete(mac,tempVlanId);
    }
    
    osapiWriteLockTake(macVlanCfgRWLock, L7_WAIT_FOREVER);

    if (vlanMacDataCount(macVlanTreeData) < L7_VLAN_MAC_MAX_VLANS)
    {
        rc = vlanMacAddApply(mac, vlanId);
        vlanMacCfgEntryAdd(mac, vlanId);
        vlanMacCfg->cfgHdr.dataChanged = L7_TRUE;
    }

    osapiWriteLockGive(macVlanCfgRWLock);
    return(rc);
}

/*********************************************************************
* @purpose  To delete a mac assignment to a VLAN.
*          
* @param    mac         @b{(input)}    Mac Address
* @param    vlanId      @b{(input)}    vlan ID
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @end
*********************************************************************/
L7_RC_t vlanMacDelete(L7_enetMacAddr_t mac, L7_uint32 vlanId)
{
    vlanMacOperData_t *nodep;
    L7_RC_t rc = L7_FAILURE;

    /* Check for validity of VLAN identifier */
    if (vlanId < L7_DOT1Q_MIN_VLAN_ID || vlanId > L7_DOT1Q_MAX_VLAN_ID)
        return(rc);

    osapiWriteLockTake(macVlanCfgRWLock, L7_WAIT_FOREVER);

    nodep = (vlanMacOperData_t *) vlanMacDataSearch(macVlanTreeData, mac);
    if ((nodep != L7_NULLPTR) && (nodep->vlanId == vlanId))
    {
        vlanMacDeleteApply(mac, vlanId);
        vlanMacCfgEntryDelete(mac, vlanId);
        vlanMacCfg->cfgHdr.dataChanged = L7_TRUE;
        rc = L7_SUCCESS;
    }

    osapiWriteLockGive(macVlanCfgRWLock);
    return(rc);
}

/*********************************************************************
* @purpose  To get the  details of the entry 
*          
* @param    mac      @b{(input)}    Mac Address
* @param    vlanId   @b{(input)}   vlan ID 
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* 
* @comments 
* @comments 
*
* @end
*********************************************************************/
L7_RC_t vlanMacGet(L7_enetMacAddr_t mac, L7_uint32 *vlanId)
{
    vlanMacOperData_t *node;
    L7_RC_t status = L7_FAILURE;

    osapiReadLockTake(macVlanCfgRWLock, L7_WAIT_FOREVER);

    if (vlanMacDataCount(macVlanTreeData) <= 0)
    {
        osapiReadLockGive(macVlanCfgRWLock);
        return(status);
    }
    node =  vlanMacDataEntry(macVlanTreeData, mac);
    if (node == NULL)
    {
        status = L7_FAILURE;
    }
    else if( (memcmp((void *)&node->mac, (void *)&mac, sizeof(L7_enetMacAddr_t)) == 0)) 
                           
    {
        *vlanId = node->vlanId;
        status = L7_SUCCESS;
    }
    else
    {
        *vlanId = 0;
        status = L7_FAILURE;
    }
    osapiReadLockGive(macVlanCfgRWLock);
    return status;

}
/*********************************************************************
* @purpose  To get the next mac association with a vlan 
*          
* @param    mac       @b{(input)}    Mac Address
* @param    *nextMac  @b{(output)}   Mac Address
* @param    *vlanId   @b{(output)}   vlan ID associated with next mac
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* 
* @comments Multiple macs can be associated with the same VLAN
* @comments To get the first mac, pass in a  "0" for the mac value
*
* @end
*********************************************************************/
L7_RC_t vlanMacGetNext(L7_enetMacAddr_t mac, L7_enetMacAddr_t *nextMac, L7_uint32 *vlanId)
{
    L7_enetMacAddr_t tmpmac ;
    vlanMacOperData_t *nodep;
    L7_RC_t rc = L7_FAILURE;

    osapiReadLockTake(macVlanCfgRWLock, L7_WAIT_FOREVER);

    if (vlanMacDataCount(macVlanTreeData) < 0)
    {
        osapiReadLockGive(macVlanCfgRWLock);
        return(rc);
    }

    memset((void *)&tmpmac, 0x00, sizeof tmpmac);
    if (memcmp((void *)&mac, (void *)&tmpmac, sizeof(L7_enetMacAddr_t)) == 0)
    {
        nodep = vlanMacFirstDataEntry(macVlanTreeData);
        if (nodep == L7_NULL ||
            memcmp((void *)&nodep->mac, (void *)&mac, sizeof(L7_enetMacAddr_t)) == 0)
        {
            osapiReadLockGive(macVlanCfgRWLock);
            *vlanId = 0;
            memset((void *)&nextMac, 0x00, sizeof nextMac);
            return(rc);
        }
    }
    else
    {
       nodep = vlanMacNextDataEntry(macVlanTreeData, mac);
       if (nodep == L7_NULL ||
          memcmp((void *)&nodep->mac, (void *)&mac, sizeof(L7_enetMacAddr_t)) == 0)
       {
           *vlanId = 0;
           memset((void *)&nextMac, 0x00, sizeof nextMac);
           osapiReadLockGive(macVlanCfgRWLock);
           return(rc);
       }
    }
    osapiReadLockGive(macVlanCfgRWLock);
    *nextMac = nodep->mac; 
    *vlanId = nodep->vlanId;
    rc = L7_SUCCESS;
    return(rc);
}

/*********************************************************************
* @purpose  Checks whether a MAC address is unicast or multicast
*
* @param    L7_enetMacAddr_t  mac  @b((input)) MAC Address
*
* @returns  L7_TRUE, if MAC address is of type multicast
* @returns  L7_FALSE, if MAC address is of type unicast
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t vlanMacIsMacAddrTypeMulticast(L7_enetMacAddr_t mac)
{
  if ((mac.addr[0] & 0x01) != 0x01)
    return L7_FALSE;

  return L7_TRUE;
}

/* End Function Declarations */
