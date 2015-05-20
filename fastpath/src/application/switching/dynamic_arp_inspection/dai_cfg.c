
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  dai_cfg.c
*
* @purpose   Dynamic ARP Inspection Configuration routines
*
* @component Dynamic ARP Inspection
*
* @comments none
*
* @create 09/06/2007
*
* @author Kiran Kumar Kella
*
* @end
*             
**********************************************************************/

#include "l7_common.h"
#include "osapi.h"
#include "log.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "comm_mask.h"

#include "dai_cfg.h"
#include "dai_util.h"

extern osapiRWLock_t   daiCfgRWLock;
extern daiCfgData_t    *daiCfgData;
extern daiInfo_t       *daiInfo;
extern daiIntfInfo_t   *daiIntfInfo;

/*********************************************************************
* @purpose  Set the enable bit for a given VLAN in the config
*
* @param    vlanId   VLAN ID
*
* @returns  none
*
* @end
*********************************************************************/
void _daiVlanEnable(L7_uint32 vlanId)
{
  L7_VLAN_SETMASKBIT(daiCfgData->daiVlans, vlanId);
}

/*********************************************************************
* @purpose  Clear the enable bit for a given VLAN in the config
*
* @param    vlanId   VLAN ID
*
* @returns  none
*
* @end
*********************************************************************/
void _daiVlanDisable(L7_uint32 vlanId)
{
  L7_VLAN_CLRMASKBIT(daiCfgData->daiVlans, vlanId);
}

/*********************************************************************
* @purpose  Determine if DAI is enabled on a given VLAN.
*
* @param    vlanId   VLAN ID
*
* @returns  none
*
* @end
*********************************************************************/
L7_BOOL _daiVlanEnableGet(L7_uint32 vlanId)
{
  if (L7_VLAN_ISMASKBITSET(daiCfgData->daiVlans, vlanId))
    return L7_TRUE;
  else
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Set the trust bit for a given interface in the config
*
* @param    intIfNum   internal interface number
*
* @returns  none
*
* @end
*********************************************************************/
void _daiIntfTrust(L7_uint32 intIfNum)
{
  L7_INTF_SETMASKBIT(daiCfgData->daiTrust, intIfNum);
}

/*********************************************************************
* @purpose  Clear the trust bit for a given interface in the config
*
* @param    intIfNum   internal interface number
*
* @returns  none
*
* @end
*********************************************************************/
void _daiIntfUntrust(L7_uint32 intIfNum)
{
  L7_INTF_CLRMASKBIT(daiCfgData->daiTrust, intIfNum);
}

/*********************************************************************
* @purpose  Determine whether a given interface is trusted for DAI
*
* @param    intIfNum   internal interface number
*
* @returns  L7_TRUE if interface is trusted.
*
* @end
*********************************************************************/
L7_BOOL _daiIntfTrustGet(L7_uint32 intIfNum)
{
  if (L7_INTF_ISMASKBITSET(daiCfgData->daiTrust, intIfNum))
    return L7_TRUE;
  else
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Configure DAI to log invalid packets received on
*           a specific VLAN 
*
* @param    vlanId   VLAN Id
*
* @returns  none
*
* @end
*********************************************************************/
void _daiVlanLogInvalid(L7_uint32 vlanId)
{
  L7_VLAN_SETMASKBIT(daiCfgData->daiLogInvalid, vlanId);
}

/*********************************************************************
* @purpose  Configure an interface not to log invalid packets received
*           on a specific VLAN
*
* @param    vlanId   VLAN Id
*
* @returns  none
*
* @end
*********************************************************************/
void _daiVlanDontLogInvalid(L7_uint32 vlanId)
{
  L7_VLAN_CLRMASKBIT(daiCfgData->daiLogInvalid, vlanId);
}

/*********************************************************************
* @purpose  Determine whether a given vlan is configured to 
*           log invalid packets.
*
* @param    vlanId   VLAN Id
*
* @returns  L7_TRUE  if enabled
*           L7_FALSE if disabled
*
* @end
*********************************************************************/
L7_BOOL _daiVlanLogInvalidGet(L7_uint32 vlanId)
{
  if (L7_VLAN_ISMASKBITSET(daiCfgData->daiLogInvalid, vlanId))
    return L7_TRUE;
  else
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Set Static ARP ACL flag on a specific VLAN 
*
* @param    vlanId   VLAN Id
*
* @returns  none
*
* @end
*********************************************************************/
void _daiVlanStaticFlagSet(L7_uint32 vlanId)
{
  L7_VLAN_SETMASKBIT(daiCfgData->daiStaticFlag, vlanId);
}

/*********************************************************************
* @purpose  Clear Static ARP ACL flag on a specific VLAN
*
* @param    vlanId   VLAN Id
*
* @returns  none
*
* @end
*********************************************************************/
void _daiVlanStaticFlagClear(L7_uint32 vlanId)
{
  L7_VLAN_CLRMASKBIT(daiCfgData->daiStaticFlag, vlanId);
}

/*********************************************************************
* @purpose  Determine whether an ARP ACL on a specific VLAN
*           has static flag set
*
* @param    vlanId   VLAN Id
*
* @returns  L7_TRUE  if enabled
*           L7_FALSE if disabled
*
* @end
*********************************************************************/
L7_BOOL _daiVlanStaticFlagGet(L7_uint32 vlanId)
{
  if (L7_VLAN_ISMASKBITSET(daiCfgData->daiStaticFlag, vlanId))
    return L7_TRUE;
  else
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Enable an ARP ACL on a specific VLAN 
*
* @param    vlanId   VLAN Id
* @param    aclName  ACL name
*
* @returns  none
*
* @end
*********************************************************************/
void _daiVlanArpAclSet(L7_uint32 vlanId, L7_uchar8 *aclName)
{
  osapiStrncpy(daiCfgData->aclName[vlanId], aclName, L7_ARP_ACL_NAME_LEN_MAX+1);
}

/*********************************************************************
* @purpose  Disable an ARP ACL flag on a specific VLAN
*
* @param    vlanId   VLAN Id
*
* @returns  none
*
* @end
*********************************************************************/
void _daiVlanArpAclClear(L7_uint32 vlanId)
{
  osapiStrncpy(daiCfgData->aclName[vlanId], ARP_ACL_NULL,
               L7_ARP_ACL_NAME_LEN_MAX+1);
}

/*********************************************************************
* @purpose  Get the ARP ACL name configured on a specific VLAN
*
* @param    vlanId   VLAN Id
*
* @returns  L7_TRUE  if enabled
*           L7_FALSE if disabled
*
* @end
*********************************************************************/
L7_BOOL _daiVlanArpAclGet(L7_uint32 vlanId, L7_uchar8 *aclName)
{
  if(osapiStrncmp(daiCfgData->aclName[vlanId], ARP_ACL_NULL,
                  L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
  {
    osapiStrncpy(aclName, ARP_ACL_NULL, L7_ARP_ACL_NAME_LEN_MAX+1);
    return L7_FALSE;
  }
  osapiStrncpy(aclName, daiCfgData->aclName[vlanId], L7_ARP_ACL_NAME_LEN_MAX+1);
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Build default DAI config data
*
* @param    ver   Software version of Config Data
*
* @returns  none
*
* @end
*********************************************************************/
void daiBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 i;

  memset((void *)daiCfgData, 0, sizeof(daiCfgData_t));

  daiCfgData->daiVerifySMac = FD_DAI_VERIFY_SOURCE_MAC;
  daiCfgData->daiVerifyDMac = FD_DAI_VERIFY_DEST_MAC;
  daiCfgData->daiVerifyIP   = FD_DAI_VERIFY_IP;

  for (i = 1; i <= DAI_MAX_VLAN_COUNT; i++)
  {
    if (FD_DAI_VLAN == L7_ENABLE)
    {
      _daiVlanEnable(i);
    }
    if (FD_DAI_LOG_INVALID == L7_ENABLE)
    {
      _daiVlanLogInvalid(i);
    }
  }

  for (i = 1; i < DAI_MAX_INTF_COUNT; i++)
  {
    if (FD_DAI_TRUST == L7_ENABLE)
    {
      _daiIntfTrust(i);
    }
    daiCfgData->intfCfg[i].rate_limit     = FD_DAI_RATE_LIMIT;
    daiCfgData->intfCfg[i].burst_interval = FD_DAI_BURST_INTERVAL;
  }
  daiCfgData->daiTraceFlags = 0;
}

/*********************************************************************
* @purpose  No-op
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t daiSave(void)
{
  daiCfgDataNotChanged();
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Restores DAI user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t daiRestore(void)
{
  L7_RC_t rc = L7_FAILURE;

  osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER);
  rc = daiRestoreProcess();
  osapiWriteLockGive(daiCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  Restores DAI user config file to factory defaults
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    assuming semaphore already taken
*
* @end
*********************************************************************/
L7_RC_t daiRestoreProcess(void)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 i;

  daiBuildDefaultConfigData(0);    /* was daiCfgData->cfgHdr.version */

  /* Disable DAI */
  rc = daiApplyConfigData();

  daiConfigDataChange();

  /* Clear status data */
  for (i = 1; i < DAI_MAX_INTF_COUNT; i++)
  {
    daiIntfInfo[i].daiNumVlansEnabled = 0;
  }

  return rc;
}

/*********************************************************************
* @purpose  Checks if DAI user config data is out of sync
*           with persistent storage.
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL daiHasDataChanged(void)
{
  return daiInfo->cfgDataChanged;
}
void daiResetDataChanged(void)
{
  daiInfo->cfgDataChanged = L7_FALSE;
  return;
}
/*********************************************************************
* @purpose  Apply DAI configuration
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t daiApplyConfigData(void)
{
  L7_uint32 i;

  for (i = 1; i <= DAI_MAX_VLAN_COUNT; i++)
  {
    daiVlanEnableApply(i, _daiVlanEnableGet(i));
  }

  for (i = 1; i < DAI_MAX_INTF_COUNT; i++)
  {
    daiIntfRateLimitApply(i);
  }

  return L7_SUCCESS;
}

/*******************************************************************/
/*                     ARP ACL Configuration APIs                  */
/*******************************************************************/

/*********************************************************************
* @purpose  Create ARP ACL
*
* @param    aclName @b((input))  ACL Name
*
* @returns  L7_SUCCESS
*           L7_FAILURE if the max number of ARP ACLs reached already
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t _arpAclCreate(L7_uchar8 *aclName)
{
  L7_int32 i, j;

  for(i=0; i<L7_ARP_ACL_CMDS_MAX; i++)
  {
    if((osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, ARP_ACL_NULL,
                     L7_ARP_ACL_NAME_LEN_MAX+1) == 0) ||
       (osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, aclName,
                     L7_ARP_ACL_NAME_LEN_MAX+1) == 0))
    {
      osapiStrncpy(daiCfgData->arpAclCfg[i].aclName, aclName, L7_ARP_ACL_NAME_LEN_MAX+1);
      return L7_SUCCESS;
    }
    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, aclName,
                    L7_ARP_ACL_NAME_LEN_MAX+1) > 0)
    {
      if(osapiStrncmp(daiCfgData->arpAclCfg[L7_ARP_ACL_CMDS_MAX-1].aclName,
                      ARP_ACL_NULL, L7_ARP_ACL_NAME_LEN_MAX+1) != 0)
      {
        /* Maximum commands already reached. Can't insert this
         * new ACL in the list, return failure */
        return L7_TABLE_IS_FULL;
      }
      for(j=L7_ARP_ACL_CMDS_MAX-2;j>=i;j--)
      {
        memcpy(&(daiCfgData->arpAclCfg[j+1]), &(daiCfgData->arpAclCfg[j]), sizeof(arpAclCfg_t));
      }
      memset(&(daiCfgData->arpAclCfg[i]), 0, sizeof(arpAclCfg_t));
      osapiStrncpy(daiCfgData->arpAclCfg[i].aclName, aclName, L7_ARP_ACL_NAME_LEN_MAX+1);
      return L7_SUCCESS;
    }
  }
  /* List full, return failure */
  return L7_TABLE_IS_FULL;
}

/*********************************************************************
* @purpose  Delete ARP ACL
*
* @param    aclName @b((input))  ACL Name
*
* @returns  L7_SUCCESS
*           L7_FAILURE if no ARP ACL exists with that name
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t _arpAclDelete(L7_uchar8 *aclName)
{
  L7_uint32 i, j;

  for(i=0; i<L7_ARP_ACL_CMDS_MAX; i++)
  {
    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, ARP_ACL_NULL,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    {
      return L7_FAILURE;
    }
    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, aclName,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    {
      for(j=i+1; j<=L7_ARP_ACL_CMDS_MAX-1; j++)
      {
        memcpy(&(daiCfgData->arpAclCfg[j-1]), &(daiCfgData->arpAclCfg[j]), sizeof(arpAclCfg_t));
      }
      memset(&(daiCfgData->arpAclCfg[L7_ARP_ACL_CMDS_MAX-1]), 0, sizeof(arpAclCfg_t));
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Check if the ARP ACL exists
*
* @param    aclName @b((input))  ACL Name
*
* @returns  L7_SUCCESS if found
*           L7_FAILURE if not found
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t _arpAclGet(L7_uchar8 *aclName)
{
  L7_uint32 i;

  for(i=0; i<L7_ARP_ACL_CMDS_MAX; i++)
  {
    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, ARP_ACL_NULL,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    {
      return L7_FAILURE;
    }
    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, aclName,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    {
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the ARP ACL entry if it exists
*
* @param    aclName @b((input))  ACL Name
*
* @returns  pointer to ACL entry if it exists
*           L7_NULL if not
*
* @notes   
*
* @end
*********************************************************************/
arpAclCfg_t* _arpAclEntryGet(L7_uchar8 *aclName)
{
  L7_uint32    i;

  for(i=0; i<L7_ARP_ACL_CMDS_MAX; i++)
  {
    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, ARP_ACL_NULL,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    {
      return L7_NULL;
    }
    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, aclName,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    {
      return &(daiCfgData->arpAclCfg[i]);
    }
  }
  return L7_NULL;
}

/*********************************************************************
* @purpose  API to get the next ARP ACL entry
*
* @param    aclName     @b((input))  ACL Name
* @param    nextAclName @b((output)) Next ACL Name
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes
*           memory for nextAclName should have been allocated by
*           the caller of this function
*
* @end
*********************************************************************/
L7_RC_t _arpAclNextGet(L7_uchar8 *aclName, L7_uchar8 *nextAclName)
{
  L7_uint32 i;

  if(osapiStrncmp(aclName, ARP_ACL_NULL, L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
  {
    /* Return the first ARP ACL name */
    osapiStrncpy(nextAclName, daiCfgData->arpAclCfg[0].aclName,
                 L7_ARP_ACL_NAME_LEN_MAX+1);
    if(osapiStrncmp(nextAclName, ARP_ACL_NULL,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    {
      return L7_FAILURE;
    }
    return L7_SUCCESS;
  }

  for(i=0; i<L7_ARP_ACL_CMDS_MAX; i++)
  {
    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, aclName,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    {
      if((i != L7_ARP_ACL_CMDS_MAX-1) &&
         (osapiStrncmp(daiCfgData->arpAclCfg[i+1].aclName, ARP_ACL_NULL,
                       L7_ARP_ACL_NAME_LEN_MAX+1) != 0))
      {
        osapiStrncpy(nextAclName, daiCfgData->arpAclCfg[i+1].aclName,
                     L7_ARP_ACL_NAME_LEN_MAX+1);
        return L7_SUCCESS;
      }
      return L7_FAILURE;
    }
    else if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, aclName,
                         L7_ARP_ACL_NAME_LEN_MAX+1) > 0)
    {
        osapiStrncpy(nextAclName, daiCfgData->arpAclCfg[i].aclName,
                     L7_ARP_ACL_NAME_LEN_MAX+1);
        return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  API to add rule to ARP ACL
*
* @param    aclName @b((input))  ACL Name
* @param    ipAddress  @b((input))  Sender's IP address
* @param    macAddress @b((input))  Sender's MAC address
*
* @returns  L7_SUCCESS
*           L7_FAILURE if the max number of rules in ARP ACL have
*                      already reached
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t _arpAclRuleAdd(L7_uchar8 *aclName, L7_uint32 ipAddress,
                       L7_uchar8 *macAddress)
{
  L7_int32 i, j, k;
  arpAclCfg_t  *acl;
  arpAclRule_t *rule;

  for(i=0; i<L7_ARP_ACL_CMDS_MAX; i++)
  {
    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, ARP_ACL_NULL,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
      return L7_FAILURE;

    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, aclName,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    {
      acl = &(daiCfgData->arpAclCfg[i]);
      for(j=0; j<L7_ARP_ACL_RULES_MAX; j++)
      {
        rule = &(acl->rules[j]);
        if(rule->senderHostIp == 0)
        {
          /* Add the first rule in this ACL */
          rule->senderHostIp = ipAddress;
          memcpy(rule->senderHostMac, macAddress, L7_ENET_MAC_ADDR_LEN);
          /* PTin added: DAI */
          acl->number_of_rules++;
          return L7_SUCCESS;
        }
        if(rule->senderHostIp == ipAddress)
        {
          if(memcmp(rule->senderHostMac, macAddress,
                    L7_ENET_MAC_ADDR_LEN) == 0)
            return L7_SUCCESS;

          if(memcmp(rule->senderHostMac, macAddress, L7_ENET_MAC_ADDR_LEN) > 0)
          {
            if(acl->rules[L7_ARP_ACL_RULES_MAX-1].senderHostIp != 0)
            {
              /* Rules reached max */
              return L7_TABLE_IS_FULL;
            }
            for(k= L7_ARP_ACL_RULES_MAX-2; k >= j; k--)
            {
              memcpy(&(acl->rules[k+1]), &(acl->rules[k]), sizeof(arpAclRule_t));
            }
            rule->senderHostIp = ipAddress;
            memcpy(rule->senderHostMac, macAddress, L7_ENET_MAC_ADDR_LEN);
            /* PTin added: DAI */
            acl->number_of_rules++;
            return L7_SUCCESS;
          }
        }
        else if(rule->senderHostIp > ipAddress)
        {
            if(acl->rules[L7_ARP_ACL_RULES_MAX-1].senderHostIp != 0)
            {
              /* Rules reached max */
              return L7_TABLE_IS_FULL;
            }
            for(k= L7_ARP_ACL_RULES_MAX-2; k >= j; k--)
            {
              memcpy(&(acl->rules[k+1]), &(acl->rules[k]), sizeof(arpAclRule_t));
            }
            rule->senderHostIp = ipAddress;
            memcpy(rule->senderHostMac, macAddress, L7_ENET_MAC_ADDR_LEN);
            /* PTin added: DAI */
            acl->number_of_rules++;
            return L7_SUCCESS;
        }
      }
      return L7_TABLE_IS_FULL;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  API to remove rule from ARP ACL
*
* @param    aclName @b((input))  ACL Name
* @param    ipAddress  @b((input))  Sender's IP address
* @param    macAddress @b((input))  Sender's MAC address
*
* @returns  L7_SUCCESS
*           L7_FAILURE if no ARP ACL exists or if the rule doesn't exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t _arpAclRuleDelete(L7_uchar8 *aclName, L7_uint32 ipAddress,
                          L7_uchar8 *macAddress)
{
  L7_uint32 i, j, k;
  arpAclCfg_t  *acl;
  arpAclRule_t *rule;

  for(i=0; i<L7_ARP_ACL_CMDS_MAX; i++)
  {
    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, ARP_ACL_NULL,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
      return L7_FAILURE;

    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, aclName,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    {
      acl = &(daiCfgData->arpAclCfg[i]);
      for(j=0; j<L7_ARP_ACL_RULES_MAX; j++)
      {
        rule = &(acl->rules[j]);
        if((rule->senderHostIp == ipAddress) &&
           (memcmp(rule->senderHostMac, macAddress, L7_ENET_MAC_ADDR_LEN) == 0))
        {
          for(k=j+1; k<=L7_ARP_ACL_RULES_MAX-1; k++)
          {
            memcpy(&(acl->rules[k-1]), &(acl->rules[k]), sizeof(arpAclRule_t));
          }
          memset(&(acl->rules[L7_ARP_ACL_RULES_MAX-1]), 0, sizeof(arpAclRule_t));
          /* PTin added: DAI */
          if (acl->number_of_rules > 0)
            acl->number_of_rules--;
          return L7_SUCCESS;
        }
        else if(rule->senderHostIp > ipAddress)
        {
          return L7_FAILURE;
        }
      }
      return L7_FAILURE;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  API to Check if an ARP ACL Rule exists
*
* @param    aclName    @b((input))  ACL Name
* @param    ipAddress  @b((input))  Sender's IP address
* @param    macAddress @b((input))  Sender's MAC address
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t _arpAclRuleGet(L7_uchar8 *aclName, L7_uint32 ipAddress, L7_uchar8 *macAddress)
{
  L7_uint32 i, j;
  arpAclCfg_t  *acl;
  arpAclRule_t *rule;

  for(i=0; i<L7_ARP_ACL_CMDS_MAX; i++)
  {
    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, ARP_ACL_NULL,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
      return L7_FAILURE;

    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, aclName,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    {
      acl = &(daiCfgData->arpAclCfg[i]);
      for(j=0; j<L7_ARP_ACL_RULES_MAX; j++)
      {
        rule = &(acl->rules[j]);
        if((rule->senderHostIp == ipAddress) &&
           (memcmp(rule->senderHostMac, macAddress, L7_ENET_MAC_ADDR_LEN) == 0))
        {
          return L7_SUCCESS;
        }
        else if(rule->senderHostIp > ipAddress)
        {
          return L7_FAILURE;
        }
      }
      return L7_FAILURE;
    }
  }
  return L7_FAILURE;
}

/* PTin added: DAI */
#if 1
/*********************************************************************
* @purpose  API to Check if an ARP ACL Rule exists
*
* @param    aclName    @b((input))  ACL Name
* @param    ipAddress  @b((input))  Sender's IP address
* @param    macAddress @b((input))  Sender's MAC address
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t _ptin_arpAclRuleGet(L7_uchar8 *aclName, L7_uint32 ipAddress, L7_uchar8 *macAddress)
{
  L7_uint32 i;
  arpAclCfg_t  *acl;

  for(i=0; i<L7_ARP_ACL_CMDS_MAX; i++)
  {
    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, ARP_ACL_NULL,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
      return L7_FAILURE;

    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, aclName,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    {
      acl = &(daiCfgData->arpAclCfg[i]);

      if (_ptin_arpAclRuleFindIndex(acl, ipAddress, macAddress) >= 0)
      {
        return L7_SUCCESS;
      }
    }
  }
  return L7_FAILURE;
}
#endif

/*********************************************************************
* @purpose  API to get the next ARP ACL Rule from all the ACL list
*
* @param    aclNameIn @b((input))  ACL Name
* @param    ipAddrIn  @b((input))  Sender's IP address
* @param    macAddrIn @b((input))  Sender's MAC address
* @param    aclNameOut @b((output))  ACL Name
* @param    ipAddrOut  @b((output))  Sender's IP address
* @param    macAddrOut @b((output))  Sender's MAC address
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t _arpAclRuleNextGet(L7_uchar8 *aclNameIn, L7_uint32 ipAddrIn, L7_uchar8 *macAddrIn,
                           L7_uchar8 *aclNameOut, L7_uint32 *ipAddrOut, L7_uchar8 *macAddrOut)
{
  L7_uint32 j;
  L7_int32 i;
  arpAclCfg_t  *acl;
  arpAclRule_t *rule;

  if(osapiStrncmp(aclNameIn, ARP_ACL_NULL, L7_ARP_ACL_NAME_LEN_MAX+1) != 0)
  {
    for(i=0; i<L7_ARP_ACL_CMDS_MAX; i++)
    {
      if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, ARP_ACL_NULL,
                      L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
        return L7_FAILURE;
  
      /* Get the next rule in the matching ACL */
      if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, aclNameIn,
                      L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
      {
        acl = &(daiCfgData->arpAclCfg[i]);
        for(j=0; j<L7_ARP_ACL_RULES_MAX; j++)
        {
          rule = &(acl->rules[j]);
          if(rule->senderHostIp == ipAddrIn)
          {
            if(memcmp(rule->senderHostMac, macAddrIn, L7_ENET_MAC_ADDR_LEN) == 0)
            {
              if((j != L7_ARP_ACL_RULES_MAX-1) &&
                 (acl->rules[j+1].senderHostIp != 0))
              {
                memcpy(aclNameOut, aclNameIn, L7_ARP_ACL_NAME_LEN_MAX+1);
                *ipAddrOut = acl->rules[j+1].senderHostIp;
                memcpy(macAddrOut, acl->rules[j+1].senderHostMac, L7_ENET_MAC_ADDR_LEN);
                return L7_SUCCESS;
              }
            }
            else if(memcmp(rule->senderHostMac, macAddrIn, L7_ENET_MAC_ADDR_LEN) > 0)
            {
                memcpy(aclNameOut, aclNameIn, L7_ARP_ACL_NAME_LEN_MAX+1);
                *ipAddrOut = acl->rules[j].senderHostIp;
                memcpy(macAddrOut, acl->rules[j].senderHostMac, L7_ENET_MAC_ADDR_LEN);
                return L7_SUCCESS;
            }
          }
          else if(rule->senderHostIp > ipAddrIn)
          {
              memcpy(aclNameOut, aclNameIn, L7_ARP_ACL_NAME_LEN_MAX+1);
              *ipAddrOut = acl->rules[j].senderHostIp;
              memcpy(macAddrOut, acl->rules[j].senderHostMac, L7_ENET_MAC_ADDR_LEN);
              return L7_SUCCESS;
          }
        }
        break;
      }
    }
  }
  else
  {
    /* Return the first ARP ACL rule in the list */ 
    i = -1;
  }

  /* If it came here, return the first rule in the next non-empty ACL */
  for(i=i+1; i<L7_ARP_ACL_CMDS_MAX; i++)
  {
    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, ARP_ACL_NULL,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    {
      return L7_FAILURE;
    }
    if(daiCfgData->arpAclCfg[i].rules[0].senderHostIp != 0)
    {
      memcpy(aclNameOut, daiCfgData->arpAclCfg[i].aclName, L7_ARP_ACL_NAME_LEN_MAX+1);
      *ipAddrOut = daiCfgData->arpAclCfg[i].rules[0].senderHostIp;
      memcpy(macAddrOut, daiCfgData->arpAclCfg[i].rules[0].senderHostMac, L7_ENET_MAC_ADDR_LEN);
      return L7_SUCCESS;
    }
  }

  /* Either matching current ACL is not found, or we reached end of the ACLs */
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  API to get the next ARP ACL Rule in this ACL
*
* @param    aclName @b((input))  ACL Name
* @param    ipAddrIn  @b((input))  Sender's IP address
* @param    macAddrIn @b((input))  Sender's MAC address
* @param    ipAddrOut  @b((output))  Sender's IP address
* @param    macAddrOut @b((output))  Sender's MAC address
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t _arpAclRuleInAclNextGet(L7_uchar8 *aclName, L7_uint32 ipAddrIn, L7_uchar8 *macAddrIn,
                                L7_uint32 *ipAddrOut, L7_uchar8 *macAddrOut)
{
  L7_uint32 j;
  L7_int32 i;
  arpAclCfg_t  *acl;
  arpAclRule_t *rule;

  for(i=0; i<L7_ARP_ACL_CMDS_MAX; i++)
  {
    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, ARP_ACL_NULL,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
      return L7_FAILURE;
  
    /* Get the next rule in the matching ACL */
    if(osapiStrncmp(daiCfgData->arpAclCfg[i].aclName, aclName,
                    L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    {
      acl = &(daiCfgData->arpAclCfg[i]);
      if(ipAddrIn == 0)
      {
        if(acl->rules[0].senderHostIp == 0)
          return L7_FAILURE;

        *ipAddrOut = acl->rules[0].senderHostIp;
        memcpy(macAddrOut, acl->rules[0].senderHostMac, L7_ENET_MAC_ADDR_LEN);
        return L7_SUCCESS;
      }
      for(j=0; j<L7_ARP_ACL_RULES_MAX; j++)
      {
        if((j == L7_ARP_ACL_RULES_MAX-1) ||
           (acl->rules[0].senderHostIp == 0))
        {
          /* reached end of the rules in this list */
          return L7_FAILURE;
        }
        rule = &(acl->rules[j]);
        if((rule->senderHostIp == ipAddrIn) &&
           (memcmp(rule->senderHostMac, macAddrIn, L7_ENET_MAC_ADDR_LEN) == 0) &&
           (acl->rules[j+1].senderHostIp != 0))
        {
          *ipAddrOut = acl->rules[j+1].senderHostIp;
          memcpy(macAddrOut, acl->rules[j+1].senderHostMac, L7_ENET_MAC_ADDR_LEN);
          return L7_SUCCESS;
        }
        else if(rule->senderHostIp > ipAddrIn)
        {
          *ipAddrOut = rule->senderHostIp;
          memcpy(macAddrOut, rule->senderHostMac, L7_ENET_MAC_ADDR_LEN);
          return L7_SUCCESS;
        }
      }
      return L7_FAILURE;
    }
  }

  /* Either matching current ACL is not found, or we reached end of the ACLs */
  return L7_FAILURE;
}

/* PTin added: DAI */
#if 1
/**
 * Recursive algorithm divide-and-conquer to search for a target
 * 
 * @author mruas (2/24/2015)
 * 
 * @param array 
 * @param start_index 
 * @param end_index 
 * @param target 
 * 
 * @return L7_int32 
 */
static L7_int32 _ptin_arpAclRuleBinarySearch(const arpAclRule_t *array, L7_int32 start_index, L7_int32 end_index, const arpAclRule_t *target)
{
  L7_int32 middle_index;
  const arpAclRule_t *compare_value;

  /* Not found */
  if (start_index > end_index)
  {
    return -1;
  }

  middle_index = (start_index + end_index) / 2;
  compare_value = &array[middle_index];

  /* Comapare IP address */
  if (compare_value->senderHostIp > target->senderHostIp)
  {
    return _ptin_arpAclRuleBinarySearch(array, start_index, middle_index-1, target);
  }
  if (compare_value->senderHostIp < target->senderHostIp)
  {
    return _ptin_arpAclRuleBinarySearch(array, middle_index+1, end_index, target);
  }
  /* Compare MAC address */
  if (memcmp(compare_value->senderHostMac, target->senderHostMac, L7_ENET_MAC_ADDR_LEN) > 0)
  {
    return _ptin_arpAclRuleBinarySearch(array, start_index, middle_index-1, target);
  }
  if (memcmp(compare_value->senderHostMac, target->senderHostMac, L7_ENET_MAC_ADDR_LEN) < 0)
  {
    return _ptin_arpAclRuleBinarySearch(array, middle_index+1, end_index, target);
  }

  /* Found index */
  return middle_index;
}

/**
 * Search for an entry at ACL table, using divide-and-conquer 
 * algorithm 
 * 
 * @author mruas (2/24/2015)
 * 
 * @param arpAcl_table 
 * @param find_senderHostIp 
 * @param find_senderHostMac 
 * 
 * @return L7_int32 : index (-1: not found)
 */
L7_int32 _ptin_arpAclRuleFindIndex(const arpAclCfg_t *arpAcl_table, L7_IP_ADDR_t find_senderHostIp, L7_uchar8 *find_senderHostMac)
{
  arpAclRule_t rule;

  /* Check if table is empty */
  if (arpAcl_table->number_of_rules == 0)
  {
    return -1;
  }

  /* Target to search for */
  rule.senderHostIp = find_senderHostIp;
  memcpy(rule.senderHostMac, find_senderHostMac, sizeof(L7_uchar8)*L7_ENET_MAC_ADDR_LEN);

  return _ptin_arpAclRuleBinarySearch(arpAcl_table->rules, 0, arpAcl_table->number_of_rules, &rule);
}
#endif
