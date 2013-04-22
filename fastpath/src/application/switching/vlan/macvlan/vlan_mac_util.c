/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  vlan_mac_util.c
*
* @purpose   MAC Vlan Utilities file
*
* @component vlanMac
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
#include "vlan_mac_include.h"

extern macvlanTree_t *macVlanTreeData;
extern macvlanTreeTables_t *macVlanTreeHeap;

/* Begin Function Declarations: vlan_mac_util.h */

/*********************************************************************
* @purpose  To assign a mac to a VLAN.
*
* @param    mac         @b{(input)}    Mac Address
* @param    vlanId      @b{(input)}    vlan ID
*
* @returns L7_RC_t
*
* @end
*********************************************************************/
L7_RC_t vlanMacAddApply(L7_enetMacAddr_t mac, L7_uint32 vlanId)
{
    L7_RC_t rc;
    vlanMacOperData_t node;
#if 0
    vlanMacCfgData_t *nodep, node;
    nodep = vlanMacDataSearch(macVlanTreeData, mac);
    if (nodep)
    {
        if (nodep->vlanId == vlanId)
            return(L7_SUCCESS);
        else
            vlanMacDeleteApply(mac, vlanId);
    }
#endif
    memset(&node, 0, sizeof(vlanMacOperData_t));
    node.vlanId = vlanId;
    node.mac = mac;
    node.inUse = (L7_uchar8)L7_TRUE;
    vlanMacEntryAdd(macVlanTreeData, &node);

    rc = dtlVlanMacEntryCreate(node.mac, node.vlanId);

    if(rc != L7_SUCCESS)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_VLAN_MAC_COMPONENT_ID, "vlanMacAddApply: Failed to add an entry. "
                                       "This appears when a dtl call fails to add an entry into the table.\n");
        vlanMacEntryDelete(macVlanTreeData, mac);
    }
    return rc;
}


/*********************************************************************
* @purpose  To delete a mac assignment to a VLAN.
*
* @param    mac         @b{(input)}    Mac Address
* @param    vlanId      @b{(input)}    vlan ID
*
* @returns void
*
* @end
*********************************************************************/
void vlanMacDeleteApply(L7_enetMacAddr_t mac, L7_uint32 vlanId)
{
    vlanMacOperData_t *nodep;

    nodep = vlanMacDataSearch(macVlanTreeData, mac);
    if (nodep && nodep->vlanId == vlanId)
    {
        nodep->inUse = (L7_uchar8)L7_FALSE;
        vlanMacEntryDelete(macVlanTreeData, mac);

        if(dtlVlanMacEntryDelete(mac, vlanId) != L7_SUCCESS)
        {
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_VLAN_MAC_COMPONENT_ID, "vlanMacDeleteApply: Unable to delete an "
                                     "Entry. This appears when a dtl fails to delete an entry from the table\n");
        }
    }
}

/*********************************************************************
* @purpose  To delete a mac assignment to a VLAN.
*
* @param    mac         @b{(input)}    Mac Address
* @param    vlanId      @b{(input)}    vlan ID
*
* @returns void
*
* @end
*********************************************************************/
void vlanMacCfgEntryAdd(L7_enetMacAddr_t mac, L7_uint32 vlanId)
{
    L7_uint32 cfgIndex;
    L7_enetMacAddr_t tmpmac;

    memset(tmpmac.addr, 0x00, sizeof tmpmac);
    for (cfgIndex = 0; cfgIndex < L7_VLAN_MAC_MAX_VLANS; cfgIndex++)
    {
        if (memcmp(vlanMacCfg->macVlanInfo[cfgIndex].mac.addr, tmpmac.addr,
                   sizeof mac) == 0)
        {
            vlanMacCfg->macVlanInfo[cfgIndex].mac = mac;
            vlanMacCfg->macVlanInfo[cfgIndex].vlanId = vlanId;
            break;
        }
    }
}

/*********************************************************************
* @purpose  To delete a mac assignment to a VLAN.
*
* @param    mac         @b{(input)}    Mac Adddress
* @param    vlanId      @b{(input)}    vlan ID
*
* @returns void
*
*
* @end
*********************************************************************/
void vlanMacCfgEntryDelete(L7_enetMacAddr_t mac, L7_uint32 vlanId)
{
    L7_uint32 cfgIndex;

    for (cfgIndex = 0; cfgIndex < L7_VLAN_MAC_MAX_VLANS; cfgIndex++)
    {
        if (memcmp(vlanMacCfg->macVlanInfo[cfgIndex].mac.addr, mac.addr,
                   sizeof mac) == 0)
        {
            memset(vlanMacCfg->macVlanInfo[cfgIndex].mac.addr, 0x00, sizeof mac);
            vlanMacCfg->macVlanInfo[cfgIndex].vlanId = 0;
            break;
        }
    }
}

/*********************************************************************
* @purpose  Callback function to process VLAN changes.
*
* @param    vlanId    @b{(input)}  VLAN ID
* @param    intIfnum  @b{(input)}  internal interface whose state has changed
* @param    event     @b{(input)}  VLAN event (see vlanNotifyEvent_t for list)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t vlanMacVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum,
                                  L7_uint32 event)
{
    vlanMacOperData_t *nodep;
    vlanMacOperData_t *delnode = L7_NULLPTR;
    L7_uint32      i = 0, vlanId = 0, numVlans = 0;
    /* Take write locks as the pointer that is passed back from getFirst and next
     * are directly from the avl tree strcture
     */


   for (i = 1; i<=L7_VLAN_MAX_MASK_BIT; i++)
    {
      if (vlanData->numVlans == 1)
      {
          vlanId = vlanData->data.vlanId;
          /* For any continue, we will break out */
          i = L7_VLAN_MAX_MASK_BIT + 1;
      }
      else
      {
          if (L7_VLAN_ISMASKBITSET(vlanData->data.vlanMask,i))
          {
              vlanId = i;
          }
          else
          {
              if (numVlans == vlanData->numVlans)
              {
                  /* Already taken care of all the bits in the mask so break out of for loop */
                  break;
              }
              else
              {
                  /* Vlan is not set check for the next bit since there are more bits that are set*/
                  continue;
              }
          }
      }
        switch (event)
        {
        case VLAN_ADD_NOTIFY:

          osapiWriteLockTake(macVlanCfgRWLock, L7_WAIT_FOREVER);

            nodep = vlanMacFirstDataEntry(macVlanTreeData);

            while (nodep)
            {
              if(nodep->inUse == (L7_uchar8)L7_FALSE)
              {
                if (nodep->vlanId == vlanId)
                {
                  if(dtlVlanMacEntryCreate(nodep->mac, nodep->vlanId) != L7_SUCCESS)
                  {
                    delnode = nodep;
                       L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_VLAN_MAC_COMPONENT_ID, "vlanMacVlanChangeCallback: Failed to"
                  " add an entry. This appears when a dtl fails to add an entry for a vlan add notify event.\n");
                  }
                  nodep->inUse = (L7_uchar8)L7_TRUE;
                }
                if (delnode != L7_NULLPTR)
                {
                  delnode->inUse = (L7_uchar8)L7_FALSE;
                  vlanMacEntryDelete(macVlanTreeData, delnode->mac);
                  delnode = L7_NULLPTR;
                }
              }
              nodep = vlanMacNextDataEntry(macVlanTreeData, nodep->mac);
            }
          osapiWriteLockGive(macVlanCfgRWLock);
            break;

        case VLAN_DELETE_NOTIFY:
          osapiWriteLockTake(macVlanCfgRWLock, L7_WAIT_FOREVER);
            nodep = vlanMacFirstDataEntry(macVlanTreeData);
            while (nodep)
            {
                if(nodep->inUse == (L7_uchar8)L7_TRUE)
                {
                  if (nodep->vlanId == vlanId)
                  {
                    if(dtlVlanMacEntryDelete(nodep->mac, nodep->vlanId) != L7_SUCCESS)
                             L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_VLAN_MAC_COMPONENT_ID, "vlanMacVlanChangeCallback: "
                        "Failed to delete an entry. This appears when a dtl fails to delete an entry for "
                        "an vlan delete notify event.\n");

                     nodep->inUse = (L7_uchar8) L7_FALSE;
                  }
               }
               nodep = vlanMacNextDataEntry(macVlanTreeData, nodep->mac);
            }
          osapiWriteLockGive(macVlanCfgRWLock);
          break;

        default:
            break;
        }
    numVlans++;
   }

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
L7_uint32 vlanMacTxtCfgApplyCompletionCallback(L7_uint32 event)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Ignore events types we don't care about.  */
  if ( (event != TXT_CFG_APPLY_FAILURE) &&
       (event != TXT_CFG_APPLY_SUCCESS) )
  {
    return L7_SUCCESS;
  }

  /* signal configurator that this component is ready for driver sync. */
  cnfgrApiComponentHwUpdateDone(L7_VLAN_MAC_COMPONENT_ID, L7_CNFGR_HW_APPLY_CONFIG);
  return(rc);
}

/* End Function Declaration */
