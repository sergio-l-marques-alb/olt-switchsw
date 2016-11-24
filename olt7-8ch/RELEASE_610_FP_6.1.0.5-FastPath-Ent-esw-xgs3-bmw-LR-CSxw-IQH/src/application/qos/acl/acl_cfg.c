/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename acl_cfg.c
*
* @purpose ACL config file and init fns
*
* @component ACL
*
* @comments 
*
* @create 06/03/2002
*
* @author djohnson
*
* @end
*
**********************************************************************/


#include <string.h>
#include <stdio.h>
#include "l7_common.h"
#include "osapi.h"
#include "sysapi.h"
#include "nimapi.h"
#include "nvstoreapi.h"
#include "log.h"
#include "acl_api.h"
#include "tlv_api.h"
#include "acl.h"
#include "l7_cnfgr_api.h"
#include "cnfgr.h"
#include "platform_config.h"
#include "l7_packet.h"
/* check if this is for SMARTPATH */
#ifdef L7_PRODUCT_SMARTPATH
#include "easyacl_api.h"
#endif

L7_BOOL           aclCompInitialized= L7_FALSE;
aclCfgFileData_t  *aclCfgFileData   = L7_NULLPTR;
L7_tlvHandle_t    aclTlvHandle      = L7_NULL;
L7_uint32         *aclMapTbl        = L7_NULLPTR;
aclIntfInfo_t     *pAclIntfInfo     = L7_NULLPTR;
aclDeregister_t   aclDeregister;
aclVlanInfo_t     *pAclVlanInfo     = L7_NULLPTR;
L7_uint32         *aclVlanMapTbl    = L7_NULLPTR;

char *acl_direction_str[] = { "inbound", "outbound" };
char *acl_type_str[] = { "none", "IP  ", "MAC ", "IPV6" };

/* ACL feature list display strings */
char *aclFeatureString[L7_ACL_FEATURE_ID_TOTAL] = 
{
  "",                                           /* not used */
  "MAC ACL Support........................... %s\n\n",
  "IPV6 ACL Support.......................... %s\n\n",

  "Match Every............................... %s\n",

  "Match Dst IP.............................. %s\n",
  "Match Dst L4 Port......................... %s\n",
  "Match Dst L4 Port Range................... %s\n",
  "Match IP DSCP............................. %s\n",
  "Match IP Precedence....................... %s\n",
  "Match IP TOS.............................. %s\n",
  "Match Protocol............................ %s\n",
  "Match Src IP.............................. %s\n",
  "Match Src L4 Port......................... %s\n",
  "Match Src L4 Port Range................... %s\n",
  "General IP Masking Support................ %s\n\n",

  "Match IPv6 Dst IP......................... %s\n",
  "Match IPv6 Dst L4 Port.................... %s\n",
  "Match IPv6 Dst L4 Port Range.............. %s\n",
  "Match IPv6 Flow Label..................... %s\n",
  "Match IPv6 IP DSCP........................ %s\n",
  "Match IPv6 Protocol....................... %s\n",
  "Match IPv6 Src IP......................... %s\n",
  "Match IPv6 Src L4 Port.................... %s\n",
  "Match IPv6 Src L4 Port Range.............. %s\n\n",

  "Match COS................................. %s\n",
  "Match COS2................................ %s\n",
  "Match Dst MAC............................. %s\n",
  "Match Dst MAC Mask........................ %s\n",
  "Match Ethertype........................... %s\n",
  "Match Src MAC............................. %s\n",
  "Match Src MAC Mask........................ %s\n",
  "Match VLAN ID............................. %s\n",
  "Match VLAN ID Range....................... %s\n",
  "Match VLAN ID2............................ %s\n",
  "Match VLAN ID2 Range...................... %s\n\n",

  "Assign Queue.............................. %s\n",
  "Redirect.................................. %s\n",
  "Mirror.................................... %s\n",
  "Log Deny.................................. %s\n",
  "Log Permit................................ %s\n\n",

  "Intf Inbound Direction.................... %s\n",
  "Intf Outbound Direction................... %s\n",
  "Intf DiffServ Coexist..................... %s\n",
  "Intf LAG Support.......................... %s\n",
  "Intf Multiple ACLs........................ %s\n",
  "Intf Mixed Inbound Type ACLs (IPv4/MAC)... %s\n",
  "Intf Mixed Inbound IPv6/MAC............... %s\n",
  "Intf Mixed Inbound IPv6/IPv4.............. %s\n",
  "Intf Mixed Outbound Type ACLs (IPv4/MAC).. %s\n",
  "Intf Mixed Outbound IPv6/MAC.............. %s\n",
  "Intf Mixed Outbound IPv6/IPv4............. %s\n\n",

  "Bind ACLs to VLAN ID support.............. %s\n",
  "VLAN Inbound Direction.................... %s\n",
  "VLAN Outbound Direction................... %s\n",
  "VLAN Multiple ACLs........................ %s\n",
  "VLAN Mixed Inbound Type ACLs (IPv4/MAC)... %s\n",
  "VLAN Mixed Inbound IPv6/MAC............... %s\n",
  "VLAN Mixed Inbound IPv6/IPv4.............. %s\n",
  "VLAN Mixed Outbound Type ACLs (IPv4/MAC).. %s\n",
  "VLAN Mixed Outbound IPv6/MAC.............. %s\n",
  "VLAN Mixed Outbound IPv6/IPv4............. %s\n\n",

  "Use ingress FP for egress ACLs............ %s\n\n",
};

L7_uint32 aclCompId_g = L7_FLEX_QOS_ACL_COMPONENT_ID;

extern aclCnfgrState_t   aclCnfgrState;
extern avlTree_t         *pAclTree;
extern osapiRWLock_t     aclRwLock;



/*********************************************************************
*
* @purpose  Indicates whether the ACL component has been initialized
*
* @param    
*
* @returns  L7_TRUE if the ACL component has been initialized.
*           L7_FALSE otherwise.
*
* @comments 
*
* @end
*
*********************************************************************/
L7_BOOL aclComponentInitialized(void)
{
  return aclCompInitialized;
}

/*********************************************************************
*
* @purpose  Build default acl intf config data.
*
* @param    L7_uint32  ver  Software version of Config Data
*
* @returns  void
*
* @comments None.
*
* @end
*
*********************************************************************/
void aclBuildDefaultIntfConfigData(nimConfigID_t *configId, aclIntfCfgData_t *pCfg)
{
  memset((L7_char8 *)pCfg, 0, sizeof(aclIntfCfgData_t));
  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);
}
/*********************************************************************
*
* @purpose  Build default acl VLAN config data.
*
* @param    L7_uint32        vlanNum  internal VLAN number
* @param    aclVlanCfgData_t pCfg     VLAN config data
*
* @returns  void
*
* @comments None.
*
* @end
*
*********************************************************************/
void aclBuildDefaultVlanConfigData( L7_uint32 vlanNum, aclVlanCfgData_t *pCfg )
{
  memset((L7_char8 *)pCfg, 0, sizeof(aclVlanCfgData_t));
  pCfg->inUse  = L7_TRUE;
  pCfg->vlanId = vlanNum;
}

/*********************************************************************
*
* @purpose  Build default acl config data.
*
* @param    L7_uint32  ver  Software version of Config Data
*
* @returns  void
*
* @comments None.
*
* @end
*
*********************************************************************/
void aclBuildDefaultConfigData(L7_uint32 ver)
{
  L7_uint32 cfgIndex;
  nimConfigID_t     configId_temp[L7_ACL_INTF_MAX_COUNT];

  memset((L7_char8 *)&configId_temp[0], 0, sizeof(nimConfigID_t) * L7_ACL_INTF_MAX_COUNT);

  /* save all the config Ids */
  for (cfgIndex = 1; cfgIndex < L7_ACL_INTF_MAX_COUNT; cfgIndex++)
    NIM_CONFIG_ID_COPY(&configId_temp[cfgIndex], &aclCfgFileData->cfgParms.aclIntfCfgData[cfgIndex].configId);

  /* convert intf direction table */
  memset((L7_char8 *)aclCfgFileData, 0, sizeof(aclCfgFileData_t));

  for (cfgIndex = 1; cfgIndex < L7_ACL_INTF_MAX_COUNT; cfgIndex++)
    aclBuildDefaultIntfConfigData(&configId_temp[cfgIndex], &aclCfgFileData->cfgParms.aclIntfCfgData[cfgIndex]);

  
  /* Build header */
  strcpy((char*)aclCfgFileData->cfgHdr.filename, ACL_CFG_FILENAME);
  aclCfgFileData->cfgHdr.version     = ver;
  aclCfgFileData->cfgHdr.componentID = aclCompId_g;
  aclCfgFileData->cfgHdr.type        = L7_CFG_DATA;
  aclCfgFileData->cfgHdr.length      = (L7_uint32)sizeof(aclCfgFileData_t);
  aclCfgFileData->cfgHdr.dataChanged = L7_FALSE;

}


/*********************************************************************
*
* @purpose  Apply config data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclApplyConfigData(void)
{
  L7_RC_t rc = L7_SUCCESS;
#ifdef PROD_USE_BINARY_CONFIGURATION  
  L7_uint32 idx;
#endif
  L7_uint32 cfgIndex, intIfNum;
  aclIntfCfgData_t  *pCfg;
  nimConfigID_t configIdNull;
#ifdef PROD_USE_BINARY_CONFIGURATION  
  aclCfgData_t *aclData;
#endif

  /* Carefully apply the configured msgLvl to the operational msgLvl.
   * In order to facilitate debugging across a 'clear config', only
   * overwrite the operational msgLvl if it is currently set to zero.
   * This allows a current msgLvl setting to remain in effect during
   * a clear config.
   *
   * (This also means the msgLvl must be manually turned off if debugging
   * output is not desired.)
   */
  if (aclMsgLvlGet() == ACL_MSGLVL_ON)
  {
    aclMsgLvlSet(aclCfgFileData->cfgParms.aclMsgLvl);
  }

  /* assume not packed (so it can process pre-Rel. G saved config) */

#ifdef PROD_USE_BINARY_CONFIGURATION  
  /* element 0 not used */
  for (idx = 1; idx <= L7_ACL_MAX_LISTS; idx++)
  {
    aclData = &aclCfgFileData->cfgParms.aclData[idx];

    switch (aclData->id.aclType)
    {
    case L7_ACL_TYPE_NONE:
      break;

    case L7_ACL_TYPE_IP:
      if (cnfgrIsFeaturePresent(aclCompId_g, L7_ACL_FEATURE_SUPPORTED) == L7_TRUE)
        rc = aclApplyConfigDataIp(aclData);
      break;

    case L7_ACL_TYPE_IPV6:
      if (cnfgrIsFeaturePresent(aclCompId_g, L7_ACL_TYPE_IPV6_FEATURE_ID) == L7_TRUE)
        rc = aclApplyConfigDataIp(aclData);
      break;

    case L7_ACL_TYPE_MAC:
      if (cnfgrIsFeaturePresent(aclCompId_g, L7_ACL_TYPE_MAC_FEATURE_ID) == L7_TRUE)
        rc = aclApplyConfigDataMac(aclData);
      break;

    default:
      /* be lenient here and simply ignore unrecognized ACL types */
      break;
    } /* endswitch */

  } /* endfor */
#endif

  /* NOTE:  The intf_dir_Tb[] serves as both the config and operational
   *        data for the ACL interface configuration, so no copying is
   *        needed.
   */

  memset((L7_char8 *)&configIdNull, 0, sizeof(nimConfigID_t));

  if (ACL_IS_READY)
  {
    for (cfgIndex = 1; cfgIndex < L7_ACL_INTF_MAX_COUNT; cfgIndex++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&aclCfgFileData->cfgParms.aclIntfCfgData[cfgIndex].configId, &configIdNull))
        continue;
      if (nimIntIfFromConfigIDGet(&aclCfgFileData->cfgParms.aclIntfCfgData[cfgIndex].configId, &intIfNum) != L7_SUCCESS)
        continue;
      if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
        continue;

      (void)aclApplyIntfConfigData(intIfNum); 
    }
  }

  for (cfgIndex = 1; cfgIndex <= L7_ACL_VLAN_MAX_COUNT; cfgIndex++)
  {
    if (aclCfgFileData->cfgParms.aclVlanCfgData[cfgIndex].inUse != L7_TRUE)
      continue;

    if (aclApplyVlanConfigData(aclCfgFileData->cfgParms.aclVlanCfgData[cfgIndex].vlanId) == L7_SUCCESS)
    {
      aclVlanMapTbl[aclCfgFileData->cfgParms.aclVlanCfgData[cfgIndex].vlanId] = cfgIndex;
    }
  }

  return rc;
}

#ifdef PROD_USE_BINARY_CONFIGURATION  
/*********************************************************************
*
* @purpose  Apply config data for IP access lists (of any type)
*
* @param    aclCfgData_t *aclData  access list data pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclApplyConfigDataIp(aclCfgData_t *aclData)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL isIpv6 = L7_FALSE;
  L7_BOOL isNamedIpv4 = L7_FALSE;
  L7_uint32 rule, aclnum;
  L7_uint32 featId;
  aclRuleParms_t *r;

  if ((aclData->id.aclType != L7_ACL_TYPE_IP) &&
      (aclData->id.aclType != L7_ACL_TYPE_IPV6))
    return L7_FAILURE;

  if (aclData->id.aclType == L7_ACL_TYPE_IPV6)
    isIpv6 = L7_TRUE;

  if (aclData->id.aclId >= L7_ACL_NAMED_IPV4_MIN_INDEX)
  {
    isNamedIpv4 = L7_TRUE;
  }

  aclnum = aclData->id.aclId;
  if (aclnum != 0)
  {
    /* active acl */
    rc=aclCreate(aclnum);
    if (rc == L7_SUCCESS)
    {
      if ((isIpv6 == L7_TRUE) || (isNamedIpv4 == L7_TRUE))
      {
        /* must set the IP ACL name field before creating any rules */
        rc=aclNameAdd(aclnum, aclData->aclName);
      }

      for (rule = L7_ACL_MIN_RULE_NUM; rule <= L7_ACL_MAX_RULE_NUM; rule++)
      {
        if (aclData->ruleData[rule].ip.ruleNum != 0)
        {
          r = &(aclData->ruleData[rule].ip);

          rc=aclRuleActionAdd(aclnum, rule, r->action);

          if ( ((r->configMask) & (1 << ACL_ASSIGN_QUEUEID)) != 0 )
          {
            featId = L7_ACL_ASSIGN_QUEUE_FEATURE_ID;
            if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              rc=aclRuleAssignQueueIdAdd(aclnum, rule, r->assignQueueId);
          }

          if ( ((r->configMask) & (1 << ACL_REDIRECT_INTF)) != 0 )
          {
            featId = L7_ACL_REDIRECT_FEATURE_ID;
            if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              rc=aclRuleRedirectConfigIdAdd(aclnum, rule, &r->redirectConfigId);
          }

          if ( ((r->configMask) & (1 << ACL_MIRROR_INTF)) != 0 )
          {
            featId = L7_ACL_MIRROR_FEATURE_ID;
            if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              rc=aclRuleMirrorConfigIdAdd(aclnum, rule, &r->mirrorConfigId);
          }

          if ( ((r->configMask) & (1 << ACL_LOGGING)) != 0 )
          {
            if (aclImpLoggingIsAllowed(r->action) == L7_TRUE)  /* action-specific feature check */
              rc=aclRuleLoggingAdd(aclnum, rule, r->logging);
          }

          if ( ((r->configMask) & (1 << ACL_EVERY)) != 0)
          {
            if (r->every == L7_TRUE)
            {
              featId = L7_ACL_RULE_MATCH_EVERY_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclRuleEveryAdd(aclnum, rule, r->every);
            }
          }

          else
          {
            if ( ((r->configMask) & (1 << ACL_DSTIP)) != 0)
            {
              if (isIpv6 == L7_FALSE)
              {
                featId = L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID;
                if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                {
                  featId = L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID;
                  if ((cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE) ||
                      (r->dstIp.v4.mask == (L7_uint32)L7_IP_SUBMASK_EXACT_MATCH_MASK))
                    rc=aclRuleDstIpMaskAdd(aclnum, rule, r->dstIp.v4.addr, r->dstIp.v4.mask);
                }
              }
            }

            if ( ((r->configMask) & (1 << ACL_DSTIPV6)) != 0)
            {
              if (isIpv6 == L7_TRUE)
              {
                featId = L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID;
                if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                  rc=aclRuleDstIpv6AddrAdd(aclnum, rule, &r->dstIp.v6);
              }
            }

            if ( ((r->configMask) & (1 << ACL_DSTPORT)) != 0)
            {
              if (isIpv6 == L7_TRUE)
                featId = L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_FEATURE_ID;
              else
                featId = L7_ACL_RULE_MATCH_DSTL4PORT_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclRuleDstL4PortAdd(aclnum, rule, r->dstPort);
            }

            if ( ((r->configMask) & (1 << ACL_DSTSTARTPORT)) != 0)
            {
              if (isIpv6 == L7_TRUE)
                featId = L7_ACL_RULE_MATCH_IPV6_DSTL4PORT_RANGE_FEATURE_ID;
              else
                featId = L7_ACL_RULE_MATCH_DSTL4PORT_RANGE_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclRuleDstL4PortRangeAdd(aclnum, rule, r->dstStartPort, r->dstEndPort);
            }

            if ( ((r->configMask) & (1 << ACL_FLOWLBLV6)) != 0)
            {
              if (isIpv6 == L7_TRUE)
              {
                featId = L7_ACL_RULE_MATCH_IPV6_FLOWLBL_FEATURE_ID;
                if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                  rc=aclRuleIpv6FlowLabelAdd(aclnum, rule, r->flowlbl);
              }
            }

            if ( ((r->configMask) & (1 << ACL_IPDSCP)) != 0)
            {
              if (isIpv6 == L7_TRUE)
                featId = L7_ACL_RULE_MATCH_IPV6_IPDSCP_FEATURE_ID;
              else
                featId = L7_ACL_RULE_MATCH_IPDSCP_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclRuleIPDscpAdd(aclnum, rule, (L7_uint32)(r->tosbyte >> L7_QOS_ACL_TLV_MATCH_IPDSCP_SHIFT));
            }

            if ( ((r->configMask) & (1 << ACL_IPPREC)) != 0)
            {
              if (isIpv6 == L7_FALSE)
              {
                featId = L7_ACL_RULE_MATCH_IPPRECEDENCE_FEATURE_ID;
                if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                  rc=aclRuleIPPrecedenceAdd(aclnum, rule, (L7_uint32)(r->tosbyte >> L7_QOS_ACL_TLV_MATCH_IPPRECEDENCE_SHIFT));
              }
            }

            if ( ((r->configMask) & (1 << ACL_IPTOS)) != 0)
            {
              if (isIpv6 == L7_FALSE)
              {
                featId = L7_ACL_RULE_MATCH_IPTOS_FEATURE_ID;
                if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                  rc=aclRuleIPTosAdd(aclnum, rule, (L7_uint32)r->tosbyte, (L7_uint32)r->tosmask);
              }
            }

            if ( ((r->configMask) & (1 << ACL_PROTOCOL)) != 0 )
            {
              if (isIpv6 == L7_TRUE)
                featId = L7_ACL_RULE_MATCH_IPV6_PROTOCOL_FEATURE_ID;
              else
                featId = L7_ACL_RULE_MATCH_PROTOCOL_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclRuleProtocolAdd(aclnum, rule, (L7_uint32)r->protocol);
            }

            if ( ((r->configMask) & (1 << ACL_SRCIP)) != 0)
            {
              if (isIpv6 == L7_FALSE)
              {
                featId = L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID;
                if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                {
                  featId = L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID;
                  if ((cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE) ||
                      (r->srcIp.v4.mask == (L7_uint32)L7_IP_SUBMASK_EXACT_MATCH_MASK))
                    rc=aclRuleSrcIpMaskAdd(aclnum, rule, r->srcIp.v4.addr, r->srcIp.v4.mask);
                }
              }
            }

            if ( ((r->configMask) & (1 << ACL_SRCIPV6)) != 0)
            {
              if (isIpv6 == L7_TRUE)
              {
                featId = L7_ACL_RULE_MATCH_IPV6_SRCIP_FEATURE_ID;
                if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                  rc=aclRuleSrcIpv6AddrAdd(aclnum, rule, &r->srcIp.v6);
              }
            }

            if ( ((r->configMask) & (1 << ACL_SRCPORT)) != 0)
            {
              if (isIpv6 == L7_TRUE)
                featId = L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_FEATURE_ID;
              else
                featId = L7_ACL_RULE_MATCH_SRCL4PORT_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclRuleSrcL4PortAdd(aclnum, rule, r->srcPort);
            }

            if ( ((r->configMask) & (1 << ACL_SRCSTARTPORT)) != 0 )
            {
              if (isIpv6 == L7_TRUE)
                featId = L7_ACL_RULE_MATCH_IPV6_SRCL4PORT_RANGE_FEATURE_ID;
              else
                featId = L7_ACL_RULE_MATCH_SRCL4PORT_RANGE_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclRuleSrcL4PortRangeAdd(aclnum, rule, r->srcStartPort, r->srcEndPort);
            }
          }
        }
      } /* endfor rule */
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Apply config data for MAC access lists
*
* @param    aclCfgData_t *aclData  access list data pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclApplyConfigDataMac(aclCfgData_t *aclData)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uchar8 hostMacMask[L7_MAC_ADDR_LEN] = {0xff,0xff,0xff,0xff,0xff,0xff};
  L7_uint32 rule, aclIndex;
  L7_uint32 featId;
  aclMacRuleParms_t *r;

  if (aclData->id.aclType != L7_ACL_TYPE_MAC)
    return L7_FAILURE;

  aclIndex = aclData->id.aclId;
  if (aclIndex != 0)
  {
    /* active acl */
    rc=aclMacCreate(aclIndex);
    if (rc == L7_SUCCESS)
    {
      /* must set the MAC ACL name field before creating any rules */
      rc=aclMacNameAdd(aclIndex, aclData->aclName);

      for (rule=L7_ACL_MIN_RULE_NUM; rule<=L7_ACL_MAX_RULE_NUM; rule++)
      {
        if (aclData->ruleData[rule].mac.ruleNum != 0)
        {
          r = &(aclData->ruleData[rule].mac);

          rc=aclMacRuleActionAdd(aclIndex, rule, r->action);

          if ( ((r->configMask) & (1 << ACL_MAC_ASSIGN_QUEUEID)) != 0 )
          {
            featId = L7_ACL_ASSIGN_QUEUE_FEATURE_ID;
            if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              rc=aclMacRuleAssignQueueIdAdd(aclIndex, rule, r->assignQueueId);
          }

          if ( ((r->configMask) & (1 << ACL_MAC_REDIRECT_INTF)) != 0 )
          {
            featId = L7_ACL_REDIRECT_FEATURE_ID;
            if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              rc=aclMacRuleRedirectConfigIdAdd(aclIndex, rule, &r->redirectConfigId);
          }

          if ( ((r->configMask) & (1 << ACL_MAC_MIRROR_INTF)) != 0 )
          {
            featId = L7_ACL_MIRROR_FEATURE_ID;
            if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              rc=aclMacRuleMirrorConfigIdAdd(aclIndex, rule, &r->mirrorConfigId);
          }

          if ( ((r->configMask) & (1 << ACL_MAC_LOGGING)) != 0 )
          {
            if (aclImpLoggingIsAllowed(r->action) == L7_TRUE)  /* action-specific feature check */
              rc=aclMacRuleLoggingAdd(aclIndex, rule, r->logging);
          }

          if ( ((r->configMask) & (1 << ACL_MAC_EVERY)) != 0)
          {
            if (r->every == L7_TRUE)
            {
              featId = L7_ACL_RULE_MATCH_EVERY_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleEveryAdd(aclIndex, rule, r->every);
            }
          }

          else
          {
            if ( ((r->configMask) & (1 << ACL_MAC_COS)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_COS_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleCosAdd(aclIndex, rule, (L7_uint32)r->cos);
            }

            if ( ((r->configMask) & (1 << ACL_MAC_COS2)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_COS2_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleCos2Add(aclIndex, rule, (L7_uint32)r->cos2);
            }

            if ( ((r->configMask) & (1 << ACL_MAC_DSTMAC)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_DSTMAC_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              {
                featId = L7_ACL_RULE_MATCH_DSTMAC_MASK_FEATURE_ID;
                if ((cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE) ||
                    (memcmp(r->dstMacMask, hostMacMask, sizeof(hostMacMask)) == 0))
                  rc=aclMacRuleDstMacAdd(aclIndex, rule, r->dstMac, r->dstMacMask);
              }
            }

            if ( ((r->configMask) & (1 << ACL_MAC_ETYPE_KEYID)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_ETYPE_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleEtypeKeyAdd(aclIndex, rule, r->etypeKeyId, r->etypeValue);
            }

            if ( ((r->configMask) & (1 << ACL_MAC_SRCMAC)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_SRCMAC_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
              {
                featId = L7_ACL_RULE_MATCH_SRCMAC_MASK_FEATURE_ID;
                if ((cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE) ||
                    (memcmp(r->srcMacMask, hostMacMask, sizeof(hostMacMask)) == 0))
                  rc=aclMacRuleSrcMacAdd(aclIndex, rule, r->srcMac, r->srcMacMask);
              }
            }

            if ( ((r->configMask) & (1 << ACL_MAC_VLANID)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_VLANID_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleVlanIdAdd(aclIndex, rule, r->vlanId);
            }

            if ( ((r->configMask) & (1 << ACL_MAC_VLANID_START)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_VLANID_RANGE_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleVlanIdRangeAdd(aclIndex, rule, r->vlanIdStart, r->vlanIdEnd);
            }

            if ( ((r->configMask) & (1 << ACL_MAC_VLANID2)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_VLANID2_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleVlanId2Add(aclIndex, rule, r->vlanId2);
            }

            if ( ((r->configMask) & (1 << ACL_MAC_VLANID2_START)) != 0 )
            {
              featId = L7_ACL_RULE_MATCH_VLANID2_RANGE_FEATURE_ID;
              if (cnfgrIsFeaturePresent(aclCompId_g, featId) == L7_TRUE)
                rc=aclMacRuleVlanId2RangeAdd(aclIndex, rule, r->vlanId2Start, r->vlanId2End);
            }
          }
        }
      } /* endfor rule */
    }
  }

  return rc;
}
#endif

/*********************************************************************
*
* @purpose  To process the Callback for L7_ATTACH 
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t aclApplyIntfConfigData(L7_uint32 intIfNum) 
{
  L7_uint32 d;
  L7_uint32 featId;
  aclIntfCfgData_t  *pCfg;

  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
    return L7_SUCCESS;

  for (d=0; d<ACL_INTF_DIR_MAX; d++)
  {
    if (d == ACL_INTF_DIR_IN)
      featId = L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID;
    else
      featId = L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID;
    if (cnfgrIsFeaturePresent(aclCompId_g, featId) != L7_TRUE)
      continue;

    if (aclImpIsInterfaceInUse(intIfNum, d) == L7_TRUE)
    {
      if (aclIntfUpdate(intIfNum, d, ACL_INTF_ADD) != L7_SUCCESS)
      {
        /* remove the configuration from this intf, dir and build defaults */
        if (aclIntfDirListPurge(intIfNum, d) == L7_SUCCESS)
        {
          LOG_MSG("aclApplyIntfConfigData: Error activating saved config for "
                  "intIfNum %u %s -- using factory defaults instead\n", 
                  intIfNum, acl_direction_str[d]);
        }
        else
        {
          LOG_MSG("aclApplyIntfConfigData: Error activating saved config for "
                  "intIfNum %u %s -- attempt to use factory defaults also failed\n", 
                  intIfNum, acl_direction_str[d]);
          return L7_FAILURE;
        }
      }
    }
  } /* endfor */

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To remove the ACL config data from the interface
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This differs from L7_DELETE processing in that the interface
*           config is removed from the device, but is not removed from the
*           ACL operational config data.
*
* @end
*
*********************************************************************/
L7_RC_t aclUnApplyIntfConfigData(L7_uint32 intIfNum) 
{
  L7_uint32 d;
  aclIntfCfgData_t  *pCfg;

  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
    return L7_SUCCESS;

  for (d=0; d<ACL_INTF_DIR_MAX; d++)
  {
    if (aclImpIsInterfaceInUse(intIfNum, d) == L7_TRUE)
    {
      if (aclIntfUpdate(intIfNum, d, ACL_INTF_REMOVE) != L7_SUCCESS)
      {
        LOG_MSG("aclUnApplyIntfConfigData: unable to remove ACL config from intf %u %s\n",
                intIfNum, acl_direction_str[d]);
        return L7_FAILURE;
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Apply collateral config related to this interface 
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments This is typically used to update the device when config
*           such as for redirect and mirror interfaces come and go.
*
* @comments The intIfNum parm represents the potential target interface
*           for redirect or mirroring, not the current interface(s) to 
*           which an ACL containing those attributes is applied.
*
* @end
*
*********************************************************************/
L7_RC_t aclCollateralConfigApply(L7_uint32 intIfNum)
{
  aclIntfCfgData_t  *pCfg;

  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
    return L7_SUCCESS;

  /* find all ACLs containing at least one rule with a redirect/mirror
   * attribute directed to the specified interface
   */
  (void)aclImpCollateralConfigApply(intIfNum);      /* ignoring rc here */
  (void)aclMacImpCollateralConfigApply(intIfNum);   /* ignoring rc here */

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Apply configuration data for given VLAN ID
*
* @param    L7_uint32  vlanNum  VLAN ID number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t aclApplyVlanConfigData(L7_uint32 vlanNum) 
{
  L7_uint32 d, direction;
  L7_uint32 featId;
  aclVlanCfgData_t  *pCfg;

  if (aclMapVlanIsConfigurable(vlanNum, &pCfg) != L7_TRUE)
    return L7_FAILURE;

  /* found an ACL configuration for this vlanNum; apply it */
  if ((L7_TRUE != pCfg->inUse) ||
      (vlanNum != pCfg->vlanId))
  {
    LOG_MSG("Data inconsistency found in config table for vlanNum %u\n", vlanNum);
    return L7_FAILURE;
  }
  else
  {
    for (d=0; d<ACL_INTF_DIR_MAX; d++)
    {
      if (d == ACL_INTF_DIR_IN)
        featId = L7_ACL_VLAN_DIRECTION_INBOUND_FEATURE_ID;
      else
        featId = L7_ACL_VLAN_DIRECTION_OUTBOUND_FEATURE_ID;
      if (cnfgrIsFeaturePresent(aclCompId_g, featId) != L7_TRUE)
        continue;

      if (aclDirIntToExt(d, &direction) != L7_SUCCESS)
        continue;

      if (aclIsVlanInUse(vlanNum, direction) == L7_TRUE)
      {
        if (aclVlanUpdate(vlanNum, d, ACL_VLAN_ADD) != L7_SUCCESS)
        {
          /* remove the configuration from this vlan, dir and build defaults */
          if (aclVlanDirListPurge(vlanNum, d) == L7_SUCCESS)
          {
            LOG_MSG("Error activating saved config for vlanNum %u %s -- using factory defaults instead\n", 
                    vlanNum, acl_direction_str[d]);
          }
          else
          {
            LOG_MSG("Error activating saved config for "
                    "vlanNum %u %s -- attempt to use factory defaults also failed\n", 
                    vlanNum, acl_direction_str[d]);
            return L7_FAILURE;
          }
        }
      }
    } /* endfor */
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Save acl user config file to NVStore.
*
* @param    void
*
* @returns  L7_SUCCESS or
* @returns  L7_FAILURE
*
* @comments None. 
*       
* @end
*
*********************************************************************/
L7_RC_t aclSave(void)
{
#ifdef PROD_USE_BINARY_CONFIGURATION  
  L7_uint32 index, aclCount;
#endif

  if (aclDeregister.aclSave == L7_TRUE)
    return L7_SUCCESS;

  if (aclCfgFileData->cfgHdr.dataChanged == L7_FALSE)
  {
    /* no need to save */
    return L7_SUCCESS;
  }

#ifdef PROD_USE_BINARY_CONFIGURATION  
  memset((L7_char8 *)aclCfgFileData->cfgParms.aclData, 0, sizeof(aclCfgFileData->cfgParms.aclData));

  /* save IP ACL config */
  index = 1;                                    /* index 0 not used */
  aclCount = 0;
  (void)aclSaveIp(index, &aclCount);            /* ignoring rc here */

  /* save IPv6 ACL config */
  index += aclCount;                            /* calc next unused index */
  aclCount = 0;
  (void)aclSaveIpv6(index, &aclCount);          /* ignoring rc here */

  /* save MAC ACL config */
  index += aclCount;                            /* calc next unused index */
  aclCount = 0;
  (void)aclSaveMac(index, &aclCount);           /* ignoring rc here */
#endif

  /* NOTE:  Use the intf_dir_Tb[] array as is for saving config */

  /* save the current ACL debug message level */
  aclCfgFileData->cfgParms.aclMsgLvl = aclMsgLvlGet();
#ifdef L7_PRODUCT_SMARTPATH
  easyACLCopy(aclCfgFileData->easyRules_cfg);
#endif
  aclCfgFileData->cfgHdr.dataChanged = L7_FALSE;
  aclCfgFileData->checkSum = nvStoreCrc32((L7_uchar8*)aclCfgFileData, 
                                          (L7_uint32)(sizeof(aclCfgFileData_t) - sizeof(aclCfgFileData->checkSum)) );

  if (sysapiCfgFileWrite(aclCompId_g, ACL_CFG_FILENAME,
                        (L7_char8 *) aclCfgFileData, (L7_uint32)sizeof(aclCfgFileData_t)) != L7_SUCCESS)
  {
    LOG_MSG("ACL: Error writing config file %s\n", ACL_CFG_FILENAME);
  }

  return L7_SUCCESS;
}

#ifdef PROD_USE_BINARY_CONFIGURATION  
/*********************************************************************
*
* @purpose  Save IP ACL user config.
*
* @param    L7_uint32   indexStart    starting index in saved cfg array
* @param    L7_uint32   *savedCount   number of ACL entries saved
*
* @returns  L7_SUCCESS or
* @returns  L7_FAILURE
*
* @comments None. 
*       
* @end
*
*********************************************************************/
L7_uint32 aclSaveIp(L7_uint32 indexStart, L7_uint32 *savedCount)
{
  L7_uint32 rc;
  aclRuleParms_t *acl_rp, *r;
  L7_uint32 aclnum, rule, idx, aclCount;
  L7_BOOL   saved_last;
  aclRuleParms_t *saved_next;
  L7_ACL_TYPE_t aclType;

  *savedCount = aclCount = 0;

  /* push current into cfg file */
  rc = aclNumGetFirst(&aclnum);

  for (idx=indexStart; (idx <= L7_ACL_MAX_LISTS) && (rc == L7_SUCCESS); idx++)
  {
    aclType = aclImpTypeFromIndexGet(aclnum);

    aclCfgFileData->cfgParms.aclData[idx].id.aclType = aclType;
    aclCfgFileData->cfgParms.aclData[idx].id.aclId = aclnum;

    /* save a named ACL's name into the config struct */
    if ((aclType == L7_ACL_TYPE_IPV6) ||
        ((aclType == L7_ACL_TYPE_IP) && (aclnum >= L7_ACL_NAMED_IPV4_MIN_INDEX)))
    {
      (void)aclNameGet(aclnum, aclCfgFileData->cfgParms.aclData[idx].aclName);
    }

    aclCount++;

    rc = aclRuleGetFirst(aclnum, &rule);

    while (rc == L7_SUCCESS)
    {
      if (aclRuleGet(aclnum, rule, &acl_rp) == L7_SUCCESS)
      {
        r = &aclCfgFileData->cfgParms.aclData[idx].ruleData[rule].ip;

        /* preserve fields we do not want to overlay */
        saved_last = r->last;
        saved_next = r->next;

        /* copy current rule configuration into ACL config data structure */
        memcpy((L7_char8 *)r, (L7_char8 *)acl_rp, sizeof(*r));

        /* restore preserved fields */
        r->last = saved_last;
        r->next = saved_next;
      }

      rc = aclRuleGetNext(aclnum, rule, &rule);
    }

    rc = aclNumGetNext(aclnum, &aclnum);

  } /* endfor idx */

  *savedCount = aclCount;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Save IPv6 ACL user config.
*
* @param    L7_uint32   indexStart    starting index in saved cfg array
* @param    L7_uint32   *savedCount   number of ACL entries saved
*
* @returns  L7_SUCCESS or
* @returns  L7_FAILURE
*
* @comments None. 
*       
* @end
*
*********************************************************************/
L7_uint32 aclSaveIpv6(L7_uint32 indexStart, L7_uint32 *savedCount)
{
  L7_uint32 rc;
  aclRuleParms_t *acl_rp, *r;
  L7_uint32 aclnum, rule, idx, aclCount;
  L7_BOOL   saved_last;
  aclRuleParms_t *saved_next;

  *savedCount = aclCount = 0;

  /* push current into cfg file */
  rc = aclNamedIndexGetFirst(L7_ACL_TYPE_IPV6, &aclnum);

  for (idx=indexStart; (idx <= L7_ACL_MAX_LISTS) && (rc == L7_SUCCESS); idx++)
  {
    aclCfgFileData->cfgParms.aclData[idx].id.aclType = L7_ACL_TYPE_IPV6;
    aclCfgFileData->cfgParms.aclData[idx].id.aclId = aclnum;

    (void)aclNameGet(aclnum, aclCfgFileData->cfgParms.aclData[idx].aclName);

    aclCount++;

    rc = aclRuleGetFirst(aclnum, &rule);

    while (rc == L7_SUCCESS)
    {
      if (aclRuleGet(aclnum, rule, &acl_rp) == L7_SUCCESS)
      {
        r = &aclCfgFileData->cfgParms.aclData[idx].ruleData[rule].ip;

        /* preserve fields we do not want to overlay */
        saved_last = r->last;
        saved_next = r->next;

        /* copy current rule configuration into ACL config data structure */
        memcpy((L7_char8 *)r, (L7_char8 *)acl_rp, sizeof(*r));

        /* restore preserved fields */
        r->last = saved_last;
        r->next = saved_next;
      }
      rc = aclRuleGetNext(aclnum, rule, &rule);
    }

    rc = aclNamedIndexGetNext(L7_ACL_TYPE_IPV6, aclnum, &aclnum);

  } /* endfor idx */
  *savedCount = aclCount;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Save MAC ACL user config.
*
* @param    L7_uint32   indexStart    starting index in saved cfg array
* @param    L7_uint32   *savedCount   number of ACL entries saved
*
* @returns  L7_SUCCESS or
* @returns  L7_FAILURE
*
* @comments None. 
*       
* @end
*
*********************************************************************/
L7_uint32 aclSaveMac(L7_uint32 indexStart, L7_uint32 *savedCount)
{
  L7_uint32 rc;
  aclMacRuleParms_t *acl_rp, *r;
  L7_uint32 aclIndex, rule, idx, aclCount;
  L7_BOOL   saved_last;
  aclMacRuleParms_t *saved_next;

  *savedCount = aclCount = 0;

  /* push current into cfg file */
  rc = aclMacIndexGetFirst(&aclIndex);

  for (idx=indexStart; (idx <= L7_ACL_MAX_LISTS) && (rc == L7_SUCCESS); idx++)
  {
    aclCfgFileData->cfgParms.aclData[idx].id.aclType = L7_ACL_TYPE_MAC;
    aclCfgFileData->cfgParms.aclData[idx].id.aclId = aclIndex;

    /* save the MAC ACL name into the config struct */
    (void)aclMacNameGet(aclIndex, aclCfgFileData->cfgParms.aclData[idx].aclName);

    aclCount++;

    rc = aclMacRuleGetFirst(aclIndex, &rule);

    while (rc == L7_SUCCESS)
    {
      if (aclMacRuleGet(aclIndex, rule, &acl_rp) == L7_SUCCESS)
      {
        r = &aclCfgFileData->cfgParms.aclData[idx].ruleData[rule].mac;

        /* preserve fields we do not want to overlay */
        saved_last = r->last;
        saved_next = r->next;

        /* copy current rule configuration into ACL config data structure */
        memcpy((L7_char8 *)r, (L7_char8 *)acl_rp, sizeof(*r));

        /* restore preserved fields */
        r->last = saved_last;
        r->next = saved_next;
      }

      rc = aclMacRuleGetNext(aclIndex, rule, &rule);
    }

    rc = aclMacIndexGetNext(aclIndex, &aclIndex);

  } /* endfor idx */
  *savedCount = aclCount;
  return L7_SUCCESS;
}
#endif

/*********************************************************************
*
* @purpose  Remove all the configured ACL VLAN bindings.
*
* @param    void
*
* @returns  Nothing
*
* @comments Removes all ACLs configured on VLAN ids. 
*       
* @end
*
*********************************************************************/
void aclVlanConfigRestore(void)
{
  L7_uint32 i, vlanId = 0;
  L7_ACL_DIRECTION_t dir;
  L7_ACL_VLAN_DIR_LIST_t listInfo;
  L7_RC_t rc = L7_SUCCESS;

  while (aclVlanIdGetNext(vlanId, &vlanId) == L7_SUCCESS)
  {
    for (dir = L7_INBOUND_ACL; dir < L7_ACL_DIRECTION_TOTAL; dir++)
    {
      if (aclVlanDirAclListGet(vlanId, dir, &listInfo) == L7_SUCCESS)
      {
        for (i=0; i<listInfo.count; i++)
        {
          switch (listInfo.listEntry[i].aclType)
          {
          case L7_ACL_TYPE_IP:
          case L7_ACL_TYPE_IPV6: 
            rc = aclVlanDirectionRemove(vlanId, dir, listInfo.listEntry[i].aclId);
            break;
          case L7_ACL_TYPE_MAC:
            rc = aclMacVlanDirectionRemove(vlanId, dir, listInfo.listEntry[i].aclId);
            break;
          default:
            LOG_MSG("Unknown aclType encountered in table (aclType = %d, vlanId=%d, dir=%d, aclId=%d).\n", 
                    listInfo.listEntry[i].aclType, vlanId, dir, listInfo.listEntry[i].aclId);
          }
          if (rc  != L7_SUCCESS)
          {
            LOG_MSG("Error returned from acl(Mac)VlanDirectionRemove(rc=%d, vlanId=%d, dir=%d, aclId=%d, aclType=%d).\n", 
                    rc, vlanId, dir, listInfo.listEntry[i].aclId,listInfo.listEntry[i].aclType);

          }
        }
      }
    }
  }
}

/*********************************************************************
*
* @purpose  Restore ACL user config to defaults
*
* @param    void
*
* @returns  L7_SUCCESS or
* @returns  L7_FAILURE
*
* @comments Removes all existing ACLs and re-creates based on
*           the specs in the factory-default configuration. 
*       
* @end
*
*********************************************************************/
L7_uint32 aclRestore(void)
{
  L7_RC_t rc;
  L7_uint32 anum;

  if (aclDeregister.aclRestore == L7_TRUE)
    return L7_SUCCESS;

  /* clear out IP ACLs */
  rc = aclNumGetFirst(&anum);
  while (rc == L7_SUCCESS)
  {
    (void)aclDelete(anum, L7_TRUE);             /* ignoring rc here */
    rc = aclNumGetNext(anum, &anum);
  }

  /* clear out named IPv4 ACLs */
  rc = aclNamedIndexGetFirst(L7_ACL_TYPE_IP, &anum);
  while (rc == L7_SUCCESS)
  {
    (void)aclDelete(anum, L7_TRUE);             /* ignoring rc here */
    rc = aclNamedIndexGetNext(L7_ACL_TYPE_IP, anum, &anum);
  }

  /* clear out IPv6 ACLs */
  rc = aclNamedIndexGetFirst(L7_ACL_TYPE_IPV6, &anum);
  while (rc == L7_SUCCESS)
  {
    (void)aclDelete(anum, L7_TRUE);             /* ignoring rc here */
    rc = aclNamedIndexGetNext(L7_ACL_TYPE_IPV6, anum, &anum);
  }

  /* clear out MAC ACLs */
  rc = aclMacIndexGetFirst(&anum);
  while (rc == L7_SUCCESS)
  {
    (void)aclMacDelete(anum, L7_TRUE);          /* ignoring rc here */
    rc = aclMacIndexGetNext(anum, &anum);
  }
  /* If product is smartpath, clear EASYACL Configuration too */
#ifdef L7_PRODUCT_SMARTPATH
  easyACLRulesClear();
#endif
  aclBuildDefaultConfigData(aclCfgFileData->cfgHdr.version);
  aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Check if acl user config data has changed.
*
* @param    void
*
* @returns  L7_TRUE or
* @returns  L7_FALSE
*
* @comments None. 
*       
* @end
*
*********************************************************************/
L7_BOOL aclHasDataChanged(void)
{
  if (aclDeregister.aclHasDataChanged == L7_TRUE)
    return L7_TRUE;

  return aclCfgFileData->cfgHdr.dataChanged;
}
void aclResetDataChanged(void)
{
  aclCfgFileData->cfgHdr.dataChanged = L7_FALSE;
  return;
}

/*********************************************************************
* @purpose  Obtain a pointer to the specified interface configuration data
*           for this interface
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to acl interface config structure
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
* @notes    The caller can set the pCfg parm to L7_NULLPTR if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL aclMapIntfIsConfigurable(L7_uint32 intIfNum, aclIntfCfgData_t **pCfg)
{
  L7_uint32 index;
  nimConfigID_t configId;

  if (ACL_IS_READY == L7_FALSE)
    return L7_FALSE;

  /* Check boundary conditions */
  if (intIfNum <= 0 || intIfNum >= platIntfMaxCountGet())
    return L7_FALSE;

  index = aclMapTbl[intIfNum];

  if (index == 0)
    return L7_FALSE;

  /* verify that the configId in the config data table entry matches the configId that NIM maps to
   ** the intIfNum we are considering
   */
  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&configId, &(aclCfgFileData->cfgParms.aclIntfCfgData[index].configId)) == L7_FALSE)
    {
      /* if we get here, either we have a table management error between aclCfgFileData and aclMapTbl or
      ** there is synchronization issue between NIM and components w.r.t. interface creation/deletion
      */
      LOG_MSG("Error accessing ACL config data for interface %u in aclMapIntfIsConfigurable.\n", intIfNum);
      return L7_FALSE;
    }
  }

  if (pCfg != L7_NULLPTR)
    *pCfg = &aclCfgFileData->cfgParms.aclIntfCfgData[index];

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Obtain a pointer to the first interface config structure
*
* @param    intIfNum @b{(input)} Internal Interface Number
* @param    **pCfg   @b{(output)}  Ptr  to acl interface config structure
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
L7_BOOL aclMapIntfConfigEntryGet(L7_uint32 intIfNum, aclIntfCfgData_t **pCfg)
{
  L7_uint32 i;
  nimConfigID_t configId;
  nimConfigID_t configIdNull;

  memset((L7_char8 *)&configIdNull, 0, sizeof(nimConfigID_t));

  if (!(ACL_IS_READY))
    return L7_FALSE;

  if (nimConfigIdGet(intIfNum, &configId) == L7_SUCCESS)
  {
    for (i = 1; i < L7_ACL_INTF_MAX_COUNT; i++)
    {
      if (NIM_CONFIG_ID_IS_EQUAL(&aclCfgFileData->cfgParms.aclIntfCfgData[i].configId, &configIdNull))
      {
        aclMapTbl[intIfNum] = i;
        *pCfg = &aclCfgFileData->cfgParms.aclIntfCfgData[i];
        return L7_TRUE;
      }
    }
  }
  return L7_FALSE;
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
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfCreate(L7_uint32 intIfNum)
{
  nimConfigID_t configId;
  aclIntfCfgData_t  *pCfg;
  L7_uint32 i;

  if (aclIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  if (nimConfigIdGet(intIfNum, &configId) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = 1; i < L7_ACL_INTF_MAX_COUNT; i++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&aclCfgFileData->cfgParms.aclIntfCfgData[i].configId, &configId))
    {
      aclMapTbl[intIfNum] = i;
      break;
    }
  }

  pCfg = L7_NULL;

  /* If an interface configuration entry is not already assigned to the interface,
     assign one */
  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) != L7_TRUE)
  {
    if (aclMapIntfConfigEntryGet(intIfNum, &pCfg) != L7_TRUE)
      return L7_FAILURE;

    /* Update the configuration structure with the config id and build the default config */
    if (pCfg != L7_NULL)
      aclBuildDefaultIntfConfigData(&configId, pCfg);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To process the Callback for L7_DETACH 
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfDetach(L7_uint32 intIfNum)
{
  L7_RC_t   rc = L7_SUCCESS;
  L7_uint32 d;
  aclIntfCfgData_t *pCfg;

  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    if (nimCheckIfNumber(intIfNum) != L7_SUCCESS)
      return L7_SUCCESS;

    /* remove the ACL interface config from the device */
    if (aclUnApplyIntfConfigData(intIfNum) != L7_SUCCESS)
      return L7_FAILURE;

    /* set default ACL config for the interface (purge all ACL intf config) */
    for (d=0; d<ACL_INTF_DIR_MAX; d++)
    {
      if (aclIntfDirListPurge(intIfNum, d) != L7_SUCCESS)
        rc = L7_FAILURE;

    } /* endfor d */
  }

  return rc;
}

/*********************************************************************
*
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
*
*********************************************************************/
L7_RC_t aclIntfDelete(L7_uint32 intIfNum)
{
  aclIntfCfgData_t *pCfg;

  if (aclIntfDetach(intIfNum) != L7_SUCCESS)
    return L7_FAILURE;

  if (aclMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    memset((L7_char8 *)&pCfg->configId, 0, sizeof(nimConfigID_t));
    memset((L7_char8 *)&aclMapTbl[intIfNum], 0, sizeof(L7_uint32));
    aclCfgFileData->cfgHdr.dataChanged = L7_TRUE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Callback function to process interface state changes.
*
* @param    L7_uint32  intIfNum  internal interface number
* @param    L7_uint32  event     event, defined by L7_PORT_EVENTS_t
* @param    L7_uint32  correlator event, defined by L7_PORT_EVENTS_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t aclIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;    
  L7_BOOL   prevAcquired;
  L7_uint32 compId, d;
  aclIntfCfgData_t *pCfg;

  status.intIfNum        = intIfNum;
  status.component       = aclCompId_g;
  status.event           = event;
  status.correlator      = correlator;
  status.response.rc     = rc;
  status.response.reason = NIM_ERR_RC_UNUSED;

  if (aclDeregister.aclIntfChangeCallback == L7_TRUE)
  {
    rc = L7_SUCCESS;
  }
  else if (aclIsValidIntf(intIfNum) != L7_TRUE)
  {
    rc = L7_SUCCESS;
  }
  else if (ACL_IS_READY)
  {
    switch (event)
    {
      case L7_CREATE:
        rc = aclIntfCreate(intIfNum);
        break;

      case L7_ATTACH:
        ACL_PRT(ACL_MSGLVL_LO_2, "ACL: Attach %3u\n", intIfNum);
        if (osapiWriteLockTake(aclRwLock, L7_WAIT_FOREVER) == L7_SUCCESS)
        {
          rc = aclApplyIntfConfigData(intIfNum);
          if (rc == L7_SUCCESS)
          {
            rc = aclCollateralConfigApply(intIfNum);
          }
          (void)osapiWriteLockGive(aclRwLock);
        }
        break;

      case L7_DETACH:
        ACL_PRT(ACL_MSGLVL_LO_2, "ACL: Detach %3u\n", intIfNum);
        if (osapiWriteLockTake(aclRwLock, L7_WAIT_FOREVER) == L7_SUCCESS)
        {
          rc = aclUnApplyIntfConfigData(intIfNum); 
          if (rc == L7_SUCCESS)
          {
            rc = aclCollateralConfigApply(intIfNum);
          }
          (void)osapiWriteLockGive(aclRwLock);
        }
        break;

      case L7_DELETE:
        if (osapiWriteLockTake(aclRwLock, L7_WAIT_FOREVER) == L7_SUCCESS)
        {
          rc = aclIntfDelete(intIfNum);
          (void)osapiWriteLockGive(aclRwLock);
        }
        break;

      case L7_LAG_ACQUIRE:
      case L7_PROBE_SETUP:
        compId = (event == L7_LAG_ACQUIRE) ? L7_DOT3AD_COMPONENT_ID : L7_PORT_MIRROR_COMPONENT_ID;
        prevAcquired = aclIsIntfAcquired(intIfNum);

        /* if the interface was not previously acquired, withdraw the
         * ACL attached to this interface (check both directions)
         *
         * NOTE:  Must do this before setting acquired list bit mask so that
         *        aclBuildTLVDelete() will actually issue the TLV
         */
        if (prevAcquired == L7_FALSE)
        {
          /* only process interfaces known to ACL */
          if (aclMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
          {
            if (osapiWriteLockTake(aclRwLock, L7_WAIT_FOREVER) == L7_SUCCESS)
            {
              for (d=0; d<ACL_INTF_DIR_MAX; d++)
              {
                if (aclImpIsInterfaceInUse(intIfNum, d) != L7_TRUE)
                  continue;

                if (aclIntfUpdate(intIfNum, d, ACL_INTF_REMOVE) != L7_SUCCESS)
                  rc = L7_FAILURE;
              }
              (void)osapiWriteLockGive(aclRwLock);
            }
            else
              rc = L7_FAILURE;
          }
        }

        /* update acquired list */
        COMPONENT_ACQ_SETMASKBIT(pAclIntfInfo[intIfNum].acquiredList, compId);
        break;

      case L7_LAG_RELEASE:
      case L7_PROBE_TEARDOWN:
        compId = (event == L7_LAG_RELEASE) ? L7_DOT3AD_COMPONENT_ID : L7_PORT_MIRROR_COMPONENT_ID;
        prevAcquired = aclIsIntfAcquired(intIfNum);

        /* update acquired list */
        COMPONENT_ACQ_CLRMASKBIT(pAclIntfInfo[intIfNum].acquiredList, compId);

        /* if the interface is no longer acquired, activate the
         * ACL attached to this interface (check both directions)
         */
        if ((aclIsIntfAcquired(intIfNum) == L7_FALSE) && (prevAcquired == L7_TRUE))
        {
          if (aclMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
          {
            if (osapiWriteLockTake(aclRwLock, L7_WAIT_FOREVER) == L7_SUCCESS)
            {
              for (d=0; d<ACL_INTF_DIR_MAX; d++)
              {
                if (aclImpIsInterfaceInUse(intIfNum, d) != L7_TRUE)
                  continue;

                if (aclIntfUpdate(intIfNum, d, ACL_INTF_ADD) != L7_SUCCESS)
                  rc = L7_FAILURE;
              }
              (void)osapiWriteLockGive(aclRwLock);
            }
            else
              rc = L7_FAILURE;
          }
        }
        break;

      default:
        rc = L7_SUCCESS;
        break;
    }

    if (rc != L7_SUCCESS)
    {
      status.response.reason = NIM_ERR_RC_INTERNAL;
    }
  }
  else
  {
    LOG_MSG("aclIntfChangeCallback called outside permitted phase\n");
    rc = L7_FAILURE;
    status.response.reason = NIM_ERR_RC_OUT_OF_SEQUENCE;
  }

  status.response.rc  = rc;
  nimEventStatusCallback(status);

  return rc;
}

/*********************************************************************
*
* @purpose  Determines if an interface has been acquired by another 
*           component and is therefore unavailable to ACL.
*
* @param    L7_uint32  intIfNum  internal interface number
*
* @returns  L7_TRUE    interface has been acquired
* @returns  L7_FALSE
*
* @comments The interface is only considered to be not available for 
*           ACL purposes if it is acquired by any of these components:
*              - DOT3AD (LAGs)
*              - port mirroring
*       
* @end
*********************************************************************/
L7_BOOL aclIsIntfAcquired(L7_uint32 intIfNum)
{
  if (COMPONENT_ACQ_ISMASKBITSET(pAclIntfInfo[intIfNum].acquiredList, 
                                 L7_DOT3AD_COMPONENT_ID))
      return L7_TRUE;

  if (COMPONENT_ACQ_ISMASKBITSET(pAclIntfInfo[intIfNum].acquiredList, 
                                 L7_PORT_MIRROR_COMPONENT_ID))
    return L7_TRUE;

  return L7_FALSE;
}


/*********************************************************************
* @purpose  Obtain a pointer to the specified VLAN configuration data
*           for this interface
*
* @param    vlanNum  @b{(input)}   Internal VLAN Number
* @param    **pCfg   @b{(output)}  Ptr  to acl VLAN config structure
*                                  or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks if the
*           VLAN exists and whether the component is in a state to
*           be configured (regardless of whether the component is enabled
*           or not).
*
* @notes    The caller can set the pCfg parm to L7_NULLPTR if it does not
*           want the value output from this function.
*
* @end
*********************************************************************/
L7_BOOL aclMapVlanIsConfigurable(L7_uint32 vlanNum, aclVlanCfgData_t **pCfg)
{
  /* Check boundary conditions */
  if (aclIsValidVlan(vlanNum) != L7_TRUE)
  {
    return(L7_FALSE);
  }

  if (0 == aclVlanMapTbl[vlanNum])
  {
    return(L7_FALSE);
  }

  if (pCfg != L7_NULLPTR)
  {
    *pCfg = &aclCfgFileData->cfgParms.aclVlanCfgData[aclVlanMapTbl[vlanNum]];
  }

  return(L7_TRUE);
}

/*********************************************************************
* @purpose  Obtain a pointer to the first available VLAN config structure
*
* @param    vlanNum  @b{(input)}   Internal VLAN Number
* @param    **pCfg   @b{(output)}  Ptr  to acl VLAN config structure
*                                  or L7_NULL if not needed
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @notes    Facilitates pre-configuration, as it checks whether the 
*           component is in a state to be configured (regardless of 
*           whether the component is enabled or not).
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL aclMapVlanConfigEntryGet( L7_uint32 vlanNum, aclVlanCfgData_t **pCfg )
{
  L7_uint32 i;

  if ( L7_NULLPTR == pCfg )
  {
    return( L7_FALSE );
  }

  if ( L7_TRUE == aclMapVlanIsConfigurable( vlanNum, pCfg ) )
  {
    return( L7_TRUE );
  }

  if ( L7_TRUE == aclIsValidVlan( vlanNum ) )
  {
    for ( i = 1; i <= L7_ACL_VLAN_MAX_COUNT; i++ )
    {
      if ( L7_FALSE == aclCfgFileData->cfgParms.aclVlanCfgData[i].inUse )
      {
        *pCfg = &aclCfgFileData->cfgParms.aclVlanCfgData[i];
        aclBuildDefaultVlanConfigData(vlanNum, *pCfg);
        aclVlanMapTbl[vlanNum] = i;

        return( L7_TRUE );
      }
    }
  }

  return( L7_FALSE );
}

/*********************************************************************
* @purpose  Free and clear contents of a VLAN config structure
*
* @param    vlanNum  @b{(input)}   Internal VLAN Number
*
*
* @notes    Removes data from a config entry and marks it not in use.
*
* @notes
*
* @end
*********************************************************************/
void aclMapVlanConfigEntryRelease(L7_uint32 vlanNum)
{
  aclVlanCfgData_t *pCfg;

  if (L7_TRUE != aclMapVlanIsConfigurable(vlanNum, &pCfg))
  {
    LOG_MSG("aclMapVlanConfigEntryRelease called to release config entry for unconfigurable vlanNum (==%u)\n", vlanNum);
    return;
  }

  memset((void *)pCfg,  0,  sizeof(aclVlanCfgData_t));
  pCfg->inUse = L7_FALSE;
  aclVlanMapTbl[vlanNum] = 0;
}

/*********************************************************************
*
* @purpose  Determines if an interface is in an attached state. 
*
* @param    L7_uint32         intIfNum     internal interface number
* @param    L7_INTF_STATES_t  *pIntfState  ptr to output location, or L7_NULLPTR
*
* @returns  L7_TRUE    interface is attached
* @returns  L7_FALSE   interface not attached
*
* @comments An 'attached' state by this definition is one of:
*           L7_INTF_ATTACHING, L7_INTF_ATTACHED, or L7_INTF_DETACHING.
*
* @comments Pass a non-null pointer for the pIntfState parameter
*           to retrieve the NIM interface state value used here.
*       
* @end
*********************************************************************/
L7_BOOL aclIsIntfAttached(L7_uint32 intIfNum, L7_INTF_STATES_t *pIntfState)
{
  L7_RC_t             rc = L7_FALSE;
  L7_NIM_QUERY_DATA_t nimQueryData;

  /* Obtain the state of the interface */
  nimQueryData.intIfNum = intIfNum;
  nimQueryData.request = L7_NIM_QRY_RQST_STATE;

  if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
    nimQueryData.data.state = L7_INTF_UNINITIALIZED;

  if (pIntfState != L7_NULLPTR)
    *pIntfState = nimQueryData.data.state;

  if ((nimQueryData.data.state == L7_INTF_ATTACHING) ||
      (nimQueryData.data.state == L7_INTF_ATTACHED) ||
      (nimQueryData.data.state == L7_INTF_DETACHING))
  {
    rc = L7_TRUE;
  }

  return rc;
}
#ifdef L7_PRODUCT_SMARTPATH
L7_RC_t aclConfigSyncWithEasyACL()
{
  /* Gather all ACL Config Data and Sync it with EasyACLs */
  easyACLSync(aclCfgFileData->easyRules_cfg);
  return L7_SUCCESS;
}
#endif
/*********************************************************************
* @purpose  Display a list of debug commands available for ACL
*
* @param    void
*
* @returns  void
*
* @end
*********************************************************************/
void aclDebugHelp(void)
{
  L7_uint32     msgLvlReqd = ACL_MSGLVL_ON;     /* always display output */

  /* display header banner */
  ACL_PRT(msgLvlReqd, "\nThe following debug \'show\' commands are available:\n\n");
  ACL_PRT(msgLvlReqd, "  - aclConfigDataShow\n");
  ACL_PRT(msgLvlReqd, "  - aclFeatureShow\n");
  ACL_PRT(msgLvlReqd, "  - aclIpShow(0=list only | 1=show rule details)\n");
  ACL_PRT(msgLvlReqd, "  - aclIpRulesShow(IPv4 ACL number)\n");
  ACL_PRT(msgLvlReqd, "  - aclIpv6RulesShow(IPv6 ACL index)\n");
  ACL_PRT(msgLvlReqd, "  - aclMacShow(0=list only | 1=show rule details)\n");
  ACL_PRT(msgLvlReqd, "  - aclMacRulesShow(MAC ACL index)\n");
  ACL_PRT(msgLvlReqd, "  - aclCorrTableShow(0=show all | log correlator from TLV)\n");
  ACL_PRT(msgLvlReqd, "\n\n");

  ACL_PRT(msgLvlReqd, "The L7_ALL_INTERFACES value is: %u\n\n", L7_ALL_INTERFACES);
}

static char *acl_dir_str[] = { "in ", "out" };
static char *acl_tlv_issued_str[] = { "N", "Y" };

/*********************************************************************
*
* @purpose  Dump the contents of the acl config data.
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
L7_RC_t aclConfigDataShow(void)
{
  L7_uint32         msgLvlReqd;

#ifdef PROD_USE_BINARY_CONFIGURATION  
  aclCfgData_t      *pAclCfg;
#endif
  aclIntfCfgData_t  *pAclIntfCfg;
#ifdef PROD_USE_BINARY_CONFIGURATION  
  L7_ACL_TYPE_t     aclType;
#endif
  aclVlanCfgData_t  *pAclVlanCfg;
  L7_uint32         i, j, d, intIfNum, aclRuleCount;
  aclIntfParms_t    *pList;
#ifdef PROD_USE_BINARY_CONFIGURATION  
  aclRuleParms_t    *pRuleIp;
  aclMacRuleParms_t *pRuleMac;
#endif

  /* specify required message level */
  msgLvlReqd = ACL_MSGLVL_ON;

#ifdef PROD_USE_BINARY_CONFIGURATION  
  /* display all ACL IDs */
  ACL_PRT(msgLvlReqd, "\nACL IDs:\n");
  for (i = 1; i <= L7_ACL_MAX_LISTS; i++)
  {
    pAclCfg = &aclCfgFileData->cfgParms.aclData[i];
    aclType = pAclCfg->id.aclType;
    if (aclType != L7_ACL_TYPE_NONE)
    {
      ACL_PRT(msgLvlReqd, 
              "  list[%3u]:  aclType=%4s aclId=%3u aclName=\'%s\'\n", 
              i, acl_type_str[aclType], pAclCfg->id.aclId,
              (char *)pAclCfg->aclName);
    }
  }
  ACL_PRT(msgLvlReqd, "\n\n");

  /* display all ACL rules */
  for (i = 1; i <= L7_ACL_MAX_LISTS; i++)
  {
    pAclCfg = &aclCfgFileData->cfgParms.aclData[i];
    aclType = pAclCfg->id.aclType;

    if ((aclType == L7_ACL_TYPE_IP) ||
        (aclType == L7_ACL_TYPE_IPV6))
    {
      ACL_PRT(msgLvlReqd, "%s ACL \"%s\" (Index %u)\n", 
              acl_type_str[aclType], (char *)pAclCfg->aclName, pAclCfg->id.aclId);

      /* valid rule numbers range from 1 to MAX, but may be sparsely populated 
       * (the implicit deny all rule is not stored in config struct)
       */
      for (j = 1; j <= L7_ACL_MAX_RULE_NUM; j++)
      {
        pRuleIp = &pAclCfg->ruleData[j].ip;

        if (pRuleIp->ruleNum == 0)
          continue;

        aclIpRuleDisplay(aclType, pRuleIp, msgLvlReqd);

      } /* endfor j */
      ACL_PRT(msgLvlReqd, "\n");
    }

    else if (aclType == L7_ACL_TYPE_MAC)
    {
      ACL_PRT(msgLvlReqd, "%s ACL \"%s\" (Index %u)\n", 
              acl_type_str[aclType], (char *)pAclCfg->aclName, pAclCfg->id.aclId);

      /* valid rule numbers range from 1 to MAX, but may be sparsely populated 
       * (the implicit deny all rule is not stored in config struct)
       */
      for (j = 1; j <= L7_ACL_MAX_RULE_NUM; j++)
      {
        pRuleMac = &pAclCfg->ruleData[j].mac;

        if (pRuleMac->ruleNum == 0)
          continue;

        aclMacRuleDisplay(pRuleMac, msgLvlReqd);

      } /* endfor j */
      ACL_PRT(msgLvlReqd, "\n");
    }

    else
    {
      /* ignore unrecognized ACL types */
    }
  } /* endfor i */
  ACL_PRT(msgLvlReqd, "\n");
#endif

  /* display all interface config */
  ACL_PRT(msgLvlReqd, "\nACL Interface Configuration:\n");
  for (i = 0; i < L7_ACL_INTF_MAX_COUNT; i++)
  {
    pAclIntfCfg = &aclCfgFileData->cfgParms.aclIntfCfgData[i];
    if (nimIntIfFromConfigIDGet(&pAclIntfCfg->configId, &intIfNum) != L7_SUCCESS)
      continue;
    if (aclMapIntfIsConfigurable(intIfNum, &pAclIntfCfg) == L7_TRUE)
    {
      /* only display interfaces with meaningful content */
      if ((pAclIntfInfo[intIfNum].dir[ACL_INTF_DIR_IN].tlvInDevice != L7_TRUE) &&
          (pAclIntfInfo[intIfNum].dir[ACL_INTF_DIR_OUT].tlvInDevice != L7_TRUE) &&
          (pAclIntfCfg->intf_dir_Tb[ACL_INTF_DIR_IN].assignList[0].inUse != L7_TRUE) &&
          (pAclIntfCfg->intf_dir_Tb[ACL_INTF_DIR_OUT].assignList[0].inUse != L7_TRUE))
      {
        continue;
      }

      ACL_PRT(msgLvlReqd, "  intf[%3u]:  type=%d port=%2hu\n", 
              i, pAclIntfCfg->configId.type, 
              pAclIntfCfg->configId.configSpecifier.usp.port);

      for (d=0; d<ACL_INTF_DIR_MAX; d++)
      {
        /* only display directions with meaningful content */
        if ((pAclIntfInfo[intIfNum].dir[d].tlvInDevice != L7_TRUE) &&
            (pAclIntfCfg->intf_dir_Tb[d].assignList[0].inUse != L7_TRUE))
        {
          continue;
        }

        pList = pAclIntfCfg->intf_dir_Tb[d].assignList;
        ACL_PRT(msgLvlReqd, "    %s (inDev=%s instKey=0x%8.8x)\n", 
                acl_dir_str[d], 
                acl_tlv_issued_str[pAclIntfInfo[intIfNum].dir[d].tlvInDevice],
                pAclIntfInfo[intIfNum].dir[d].tlvInstanceKey);
        for (j = 0; j < L7_ACL_MAX_LISTS_PER_INTF_DIR; j++, pList++)
        {
          /* NOTE:  Purposely checks entire array (to see any errant entries) */
          if (pList->inUse == L7_TRUE)
          {
            /* get the number of rules in this ACL and add it to the total for intf,dir */
            if (aclRuleCountGet(pList->id.aclType, pList->id.aclId, &aclRuleCount) != L7_SUCCESS)
              aclRuleCount = 0;

            ACL_PRT(msgLvlReqd, 
                    "      %3u: type=%s aclId=%3u ruleCt=%2u seq=%u\n", 
                    j, acl_type_str[pList->id.aclType],
                    pList->id.aclId, aclRuleCount, pList->seqNum);
          }
        } /* endfor j */
      } /* endfor d */
    }
  } /* endfor i */

  /* display all VLAN config */
  ACL_PRT( msgLvlReqd, "\nACL VLAN Configuration:\n" );
  for ( i = 0; i <= L7_ACL_MAX_VLAN_ID; i++ )
  {
    aclVlanInfo_t  *pVlanInfo;
    aclIntfParms_t *pVlanList;

    if ( 0 == aclVlanMapTbl[i] )
    {
      continue;
    }

    pAclVlanCfg = &aclCfgFileData->cfgParms.aclVlanCfgData[aclVlanMapTbl[i]];
    pVlanInfo   = &pAclVlanInfo[aclVlanMapTbl[i]];

    if ( L7_FALSE == aclIsValidVlan( pAclVlanCfg->vlanId ) )
    {
      continue;
    }

    /* only display VLANs with meaningful content */
    if ( ( pVlanInfo->dir[ACL_INTF_DIR_IN].tlvInDevice  != L7_TRUE ) &&
         ( pVlanInfo->dir[ACL_INTF_DIR_OUT].tlvInDevice != L7_TRUE ) &&
         ( pAclVlanCfg->vlan_dir_Tb[ACL_INTF_DIR_IN].assignList[0].inUse != L7_TRUE ) &&
         ( pAclVlanCfg->vlan_dir_Tb[ACL_INTF_DIR_OUT].assignList[0].inUse != L7_TRUE ) )
    {
      continue;
    }

    ACL_PRT( msgLvlReqd,
             "  vlan[%3u]\n",
             pAclVlanCfg->vlanId);

    for ( d = 0; d < ACL_INTF_DIR_MAX; d++ )
    {
      /* only display directions with meaningful content */
      if ( ( pVlanInfo->dir[d].tlvInDevice != L7_TRUE ) &&
           ( pAclVlanCfg->vlan_dir_Tb[d].assignList[0].inUse != L7_TRUE ) )
      {
        continue;
      }

      pVlanList = pAclVlanCfg->vlan_dir_Tb[d].assignList;
      ACL_PRT( msgLvlReqd,
               "    %s (inDev=%s instKey=0x%8.8x)\n",
               acl_dir_str[d],
               acl_tlv_issued_str[pVlanInfo->dir[d].tlvInDevice],
               pVlanInfo->dir[d].tlvInstanceKey );
      for ( j = 0; j < L7_ACL_MAX_LISTS_PER_VLAN_DIR; j++, pVlanList++ )
      {
        /* NOTE:  Purposely checks entire array (to see any errant entries) */
        if ( pVlanList->inUse == L7_TRUE )
        {
          /* get the number of rules in this ACL and add it to the total for vlan,dir */
          if ( aclRuleCountGet( pVlanList->id.aclType,
                                pVlanList->id.aclId,
                                &aclRuleCount ) != L7_SUCCESS )
          {
            aclRuleCount = 0;
          }

          ACL_PRT( msgLvlReqd,
                   "      %3u: type=%s aclId=%3u ruleCt=%2u seq=%u\n",
                   j,
                   acl_type_str[pVlanList->id.aclType],
                   pVlanList->id.aclId,
                   aclRuleCount,
                   pVlanList->seqNum );
        }
      } /* endfor j */
    } /* endfor d */
  } /* endfor i */

  /* display other config data */
  ACL_PRT(msgLvlReqd, "\nACL Msg Lvl:  %u\n", 
          aclCfgFileData->cfgParms.aclMsgLvl);

  ACL_PRT(msgLvlReqd, "\n");
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Show the ACL feature support list for this platform
*
* @param    none
*
* @returns  none
*
* @comments
*       
* @end
*********************************************************************/
void aclFeatureShow(void)
{
#define ACL_PLATFORM_NAME_LEN      128
  L7_uint32     msgLvlReqd;
  char          platName[ACL_PLATFORM_NAME_LEN+1];
  L7_uint32     i;
  char          *pYes = "Yes", *pNo = "No";

  /* specify required message level */
  msgLvlReqd = ACL_MSGLVL_ON;

  if (sysapiRegistryGet(NPD_TYPE_STRING, STR_ENTRY, platName) != L7_SUCCESS)
    strcpy(platName, "Unknown");

  if (cnfgrIsFeaturePresent(aclCompId_g, L7_ACL_FEATURE_SUPPORTED) 
      == L7_FALSE)
  {
    ACL_PRT(msgLvlReqd, "\nACL not supported on platform: %s\n\n", platName);
    return;
  }

  ACL_PRT(msgLvlReqd, "\nACL FEATURE SUPPORT LIST (platform: %s)\n\n", 
          platName);

  for (i = 1; i < L7_ACL_FEATURE_ID_TOTAL; i++)
  {
    ACL_PRT(msgLvlReqd, aclFeatureString[i],
            (cnfgrIsFeaturePresent(aclCompId_g, i) == L7_TRUE) ? pYes : pNo);
  }

  ACL_PRT(msgLvlReqd, 
          "\nPLATFORM LIMITS\n\n");
  ACL_PRT(msgLvlReqd,
          "Max Lists........................ %u\n", (L7_uint32)L7_ACL_MAX_LISTS);
  ACL_PRT(msgLvlReqd,
          "Max Rules........................ %u\n", (L7_uint32)L7_ACL_MAX_RULES);
  ACL_PRT(msgLvlReqd,
          "Max Rules per List............... %u\n", (L7_uint32)L7_ACL_MAX_RULES_PER_LIST);
  ACL_PRT(msgLvlReqd,
          "Max Rules per Multi-List......... %u\n", (L7_uint32)L7_ACL_MAX_RULES_PER_MULTILIST);
  ACL_PRT(msgLvlReqd,
          "Max Lists per Intf,Dir........... %u\n", (L7_uint32)L7_ACL_MAX_LISTS_PER_INTF_DIR);
  ACL_PRT(msgLvlReqd,
          "Max Lists per Vlan,Dir........... %u\n", (L7_uint32)L7_ACL_MAX_LISTS_PER_VLAN_DIR);
  ACL_PRT(msgLvlReqd,
          "Log Rule Limit................... %u\n", (L7_uint32)L7_ACL_LOG_RULE_LIMIT);
  ACL_PRT(msgLvlReqd,
          "Log Rule per Intf.Dir Limit...... %u\n", (L7_uint32)L7_ACL_LOG_RULE_PER_INTF_DIR_LIMIT);

  ACL_PRT(msgLvlReqd, "\n");
}

void aclParmsShow(void)
{
  L7_uint32     msgLvlReqd;

  msgLvlReqd = ACL_MSGLVL_ON;

  ACL_PRT(msgLvlReqd,
          "platIntfMaxCountGet()............... %u\n", platIntfMaxCountGet());
  ACL_PRT(msgLvlReqd,                         
          "L7_ACL_VLAN_MAX_COUNT............... %u\n", (L7_uint32)L7_ACL_VLAN_MAX_COUNT);
  ACL_PRT(msgLvlReqd,                         
          "L7_ACL_MAX_VLAN_ID.................. %u\n", (L7_uint32)L7_ACL_MAX_VLAN_ID);
  ACL_PRT(msgLvlReqd,                         
          "L7_ACL_MAX_LISTS.................... %u\n", (L7_uint32)L7_ACL_MAX_LISTS);
  ACL_PRT(msgLvlReqd,                         
          "L7_ACL_MAX_RULES.................... %u\n", (L7_uint32)L7_ACL_MAX_RULES);
  ACL_PRT(msgLvlReqd,                         
          "L7_ACL_INTF_MAX_COUNT............... %u\n", (L7_uint32)L7_ACL_INTF_MAX_COUNT);
  ACL_PRT(msgLvlReqd,                         
          "L7_ACL_NAME_LEN_MAX................. %u\n", (L7_uint32)L7_ACL_NAME_LEN_MAX);
  ACL_PRT(msgLvlReqd,                         
          "L7_ACL_MAX_RULE_NUM................. %u\n", (L7_uint32)L7_ACL_MAX_RULE_NUM);
  ACL_PRT(msgLvlReqd,                         
          "ACL_INTF_DIR_MAX.................... %u\n", (L7_uint32)ACL_INTF_DIR_MAX);
  ACL_PRT(msgLvlReqd,                         
          "L7_ACL_MAX_LISTS_PER_INTF_DIR....... %u\n", (L7_uint32)L7_ACL_MAX_LISTS_PER_INTF_DIR);
  ACL_PRT(msgLvlReqd,                         
          "L7_ACL_MAX_LISTS_PER_VLAN_DIR....... %u\n", (L7_uint32)L7_ACL_MAX_LISTS_PER_VLAN_DIR);
  ACL_PRT(msgLvlReqd,                         
          "L7_ACL_MAX_RULES_PER_MULTILIST...... %u\n", (L7_uint32)L7_ACL_MAX_RULES_PER_MULTILIST);
  ACL_PRT(msgLvlReqd,                         
          "L7_ACL_LOG_RULE_LIMIT............... %u\n", (L7_uint32)L7_ACL_LOG_RULE_LIMIT);
  ACL_PRT(msgLvlReqd,
          "L7_ACL_LOG_RULE_PER_INTF_DIR_LIMIT.. %u\n", (L7_uint32)L7_ACL_LOG_RULE_PER_INTF_DIR_LIMIT);
  ACL_PRT(msgLvlReqd,
          "Sizeof(AcquiredMask)................ %u\n", (L7_uint32)sizeof(AcquiredMask));

  ACL_PRT(msgLvlReqd, "\n");

  ACL_PRT(msgLvlReqd,
          "Sizeof(avlTreeTables_t)......... %u\n", (L7_uint32)sizeof(avlTreeTables_t));
  ACL_PRT(msgLvlReqd,
          "Sizeof(aclStructure_t).......... %u\n", (L7_uint32)sizeof(aclStructure_t));
  ACL_PRT(msgLvlReqd,
          "Sizeof(aclRuleParms_t).......... %u\n", (L7_uint32)sizeof(aclRuleParms_t));
  ACL_PRT(msgLvlReqd,
          "Sizeof(aclMacRuleParms_t)....... %u\n", (L7_uint32)sizeof(aclMacRuleParms_t));
  
}


void aclMapTblDataShow(void)
{
  L7_uint32 i, zero_count = 0;
  L7_uint32     msgLvlReqd;
  L7_uint32 loop_limit;

  /* specify required message level */
  msgLvlReqd = ACL_MSGLVL_ON;

  loop_limit = platIntfMaxCountGet();
  for (i = 0; i < loop_limit; i++)
  {
    if (aclMapTbl[i] == 0)
    {
      zero_count++;
    }
    else
    {
      if (zero_count != 0)
      {
        ACL_PRT(msgLvlReqd,  "\naclMapTbl[i] == 0 %d times", zero_count);
        zero_count = 0;
      }
      ACL_PRT(msgLvlReqd,  "\naclMapTbl[%d] = %d", i, aclMapTbl[i]);
    }
  }
  if (zero_count != 0)
  {
    ACL_PRT(msgLvlReqd,  "\naclMapTbl[i] == 0 %d times", zero_count);
  }

  ACL_PRT(msgLvlReqd,  "\n- - - - -");

  zero_count = 0;
  for (i = 0; i <= L7_ACL_MAX_VLAN_ID; i++)
  {
    if (aclVlanMapTbl[i] == 0)
    {
      zero_count++;
    }
    else
    {
      if (zero_count != 0)
      {
        ACL_PRT(msgLvlReqd,  "\naclVlanMapTbl[i] == 0 %d times", zero_count);
        zero_count = 0;
      }
      ACL_PRT(msgLvlReqd,  "\naclVlanMapTbl[%d] = %d", i, aclVlanMapTbl[i]);
    }
  }
  if (zero_count != 0)
  {
    ACL_PRT(msgLvlReqd,  "\naclVlanMapTbl[i] == 0 %d times", zero_count);
  }
}

/*============================================================================*/
/*================= START OF CONFIG MIGRATION DEBUG CHANGES ==================*/
/*============================================================================*/

/*********************************************************************
*
* @purpose  Build test ACL intf config data
*
* @param    *configId   Interface config ID
* @param    *pCfg       Configuration data ptr for this interface
*
* @returns  void
*
* @comments The test config data consists of arbitrary non-default values.
*
* @end
*
*********************************************************************/
void aclBuildTestIntfConfigData(nimConfigID_t *configId, aclIntfCfgData_t *pCfg)
{
  L7_uint32       intIfNum, seqNum = 0;
  L7_uint32       d, i;
  aclIntfParms_t  *p;

  memset((L7_char8 *)pCfg, 0, sizeof(aclIntfCfgData_t));
  NIM_CONFIG_ID_COPY(&pCfg->configId, configId);

  if (nimIntIfFromConfigIDGet(configId, &intIfNum) == L7_SUCCESS)
    seqNum = 100 * intIfNum;

  for (d=0; d<ACL_INTF_DIR_MAX; d++)
  {
    for (i=0; i<L7_ACL_MAX_LISTS_PER_INTF_DIR; i++)
    {
      p = &pCfg->intf_dir_Tb[d].assignList[i];

      p->inUse = L7_TRUE;
      p->seqNum = seqNum++;
      p->id.aclType = (d == ACL_INTF_DIR_IN) ? L7_ACL_TYPE_IP : L7_ACL_TYPE_MAC;
      p->id.aclId = i+1;
    } /* endfor i */
  } /* endfor d */
}

/*********************************************************************
*
* @purpose  Build test ACL config data
*
* @param    void
*
* @returns  void
*
* @comments The test config data consists of arbitrary non-default values.
*
* @end
*
*********************************************************************/
void aclBuildTestConfigData(void)
{
#ifdef PROD_USE_BINARY_CONFIGURATION  
  L7_uint32         i, j, k;
  L7_uint32         macIndex;
#endif
  L7_uint32         cfgIndex;
#ifdef PROD_USE_BINARY_CONFIGURATION  
  L7_BOOL           buildIpv6;
#endif
  nimConfigID_t     configIdTemp[L7_ACL_INTF_MAX_COUNT];
  nimConfigID_t     configIdRule;
#ifdef PROD_USE_BINARY_CONFIGURATION  
  aclCfgData_t      *pAclCfg;
  aclRuleParms_t    *pRuleIp;
  aclMacRuleParms_t *pRuleMac;
#endif

  memset((L7_char8 *)&configIdTemp[0], 0, sizeof(nimConfigID_t) * L7_ACL_INTF_MAX_COUNT);
  memset((L7_char8 *)&configIdRule, 0, sizeof(nimConfigID_t));

  /* save all the config Ids */
  for (cfgIndex = 1; cfgIndex < L7_ACL_INTF_MAX_COUNT; cfgIndex++)
    NIM_CONFIG_ID_COPY(&configIdTemp[cfgIndex], &aclCfgFileData->cfgParms.aclIntfCfgData[cfgIndex].configId);

  /* covers intf direction table */
  memset((L7_char8 *)aclCfgFileData, 0, sizeof(aclCfgFileData_t));

#ifdef PROD_USE_BINARY_CONFIGURATION  
  /* build IP ACL definitions (first half of total)
   * (let first one be an IPv6 ACL)
   */
  for (i = 1; i <= (L7_ACL_MAX_LISTS/2); i++)
  {
    buildIpv6 = (i <= 1) ? L7_TRUE : L7_FALSE;

    pAclCfg = &aclCfgFileData->cfgParms.aclData[i];

    if (buildIpv6 == L7_TRUE)
    {
      pAclCfg->id.aclType = L7_ACL_TYPE_IPV6;
      pAclCfg->id.aclId = i + L7_ACL_IPV6_MIN_INDEX;
      osapiSnprintf(pAclCfg->aclName, sizeof(pAclCfg->aclName), 
                    "ipv6_access_list_%u", i);
    }
    else
    {
      pAclCfg->id.aclType = L7_ACL_TYPE_IP;
      pAclCfg->id.aclId = i;
      osapiSnprintf(pAclCfg->aclName, sizeof(pAclCfg->aclName), 
                    "noNameForIpAcl_%u", i);
    }

    configIdRule.type = L7_PHYSICAL_INTF;
    configIdRule.configSpecifier.usp.unit = 1;
    configIdRule.configSpecifier.usp.slot = 0;
    configIdRule.configSpecifier.usp.port = (L7_ushort16)i;

    for (j=1; j<=L7_ACL_MAX_RULE_NUM; j++)
    {
      pRuleIp = &pAclCfg->ruleData[j].ip;

      if (buildIpv6 == L7_TRUE)
        pRuleIp->configMask = (L7_uint32)ACL_IPV6_RULE_CFG_MASK;
      else
        pRuleIp->configMask = (L7_uint32)ACL_RULE_CFG_MASK;
      pRuleIp->configMask |= (L7_uint32)((1 << ACL_RULENUM) | 
                                         (1 << ACL_ACTION) |
                                         (1 << ACL_ASSIGN_QUEUEID) | 
                                         (1 << ACL_REDIRECT_INTF) |
                                         (1 << ACL_MIRROR_INTF) |
                                         (1 << ACL_LOGGING));

      pRuleIp->ruleNum = j;
      pRuleIp->action = L7_ACL_PERMIT;
      pRuleIp->every = L7_TRUE;
      pRuleIp->assignQueueId = 2;
      NIM_CONFIG_ID_COPY(&pRuleIp->redirectConfigId, &configIdRule);
      NIM_CONFIG_ID_COPY(&pRuleIp->mirrorConfigId, &configIdRule);
      pRuleIp->logging = L7_TRUE;

      pRuleIp->protocol = 0x25;
      pRuleIp->protmask = 0xFF;

      pRuleIp->srcPort = 100;
      pRuleIp->srcStartPort = 110;
      pRuleIp->srcEndPort = 111;

      pRuleIp->dstPort = 200;
      pRuleIp->dstStartPort = 220;
      pRuleIp->dstEndPort = 222;

      pRuleIp->tosbyte = 0x58;                        /* AF23 = 22d = 0x58 */
      pRuleIp->tosmask = 0xFC;

      if (buildIpv6 == L7_TRUE)
      {
        for (k = 0; k < L7_IP6_ADDR_LEN; k++)
        {
          pRuleIp->srcIp.v6.in6Addr.in6.addr8[k] = (L7_uchar8)(k + 0x50); /* 5051.5253.5455. etc */
          pRuleIp->dstIp.v6.in6Addr.in6.addr8[k] = (L7_uchar8)(k + 0x60); /* 6061.6263.6465. etc */
        }

        pRuleIp->srcIp.v6.in6PrefixLen = 128;
        pRuleIp->dstIp.v6.in6PrefixLen = 96;

        pRuleIp->flowlbl = 1048575;                   /* equals 0xfffff */
      }
      else
      {
        pRuleIp->srcIp.v4.addr = j + (L7_uint32)0x0a010200;              /* 10.1.2.j */
        pRuleIp->srcIp.v4.mask = 0xfffffff8;                             /* 255.255.255.248 */

        pRuleIp->dstIp.v4.addr = j + (L7_uint32)0x14030400;              /* 20.3.4.j */
        pRuleIp->dstIp.v4.mask = 0xfffffffe;                             /* 255.255.255.254 */
      }

      pRuleIp->last = L7_FALSE;                       /* ignored by config processing */
    } /* endfor j */
  } /* endfor i */

  /* build MAC ACL definitions (second half of total) */
  macIndex = 1;
  for (; i <= L7_ACL_MAX_LISTS; i++, macIndex++)
  {
    pAclCfg = &aclCfgFileData->cfgParms.aclData[i];

    pAclCfg->id.aclType = L7_ACL_TYPE_MAC;
    pAclCfg->id.aclId = macIndex;
    osapiSnprintf(pAclCfg->aclName, sizeof(pAclCfg->aclName), 
                  "testMacAccessListExtended_%u", macIndex);

    configIdRule.type = L7_PHYSICAL_INTF;
    configIdRule.configSpecifier.usp.unit = 1;
    configIdRule.configSpecifier.usp.slot = 0;
    configIdRule.configSpecifier.usp.port = (L7_ushort16)i;

    for (j=1; j<=L7_ACL_MAX_RULE_NUM; j++)
    {
      pRuleMac = &pAclCfg->ruleData[j].mac;

      pRuleMac->configMask = (L7_uint32)ACL_MAC_RULE_CFG_MASK;
      pRuleMac->configMask |= (L7_uint32)((1 << ACL_MAC_RULENUM) | 
                                          (1 << ACL_MAC_ACTION) |
                                          (1 << ACL_MAC_ASSIGN_QUEUEID) | 
                                          (1 << ACL_MAC_REDIRECT_INTF) |
                                          (1 << ACL_MAC_MIRROR_INTF) |
                                          (1 << ACL_MAC_LOGGING));

      pRuleMac->ruleNum = j;
      pRuleMac->action = L7_ACL_PERMIT;
      pRuleMac->every = L7_TRUE;
      pRuleMac->assignQueueId = 3;
      NIM_CONFIG_ID_COPY(&pRuleMac->redirectConfigId, &configIdRule);
      NIM_CONFIG_ID_COPY(&pRuleMac->mirrorConfigId, &configIdRule);
      pRuleMac->logging = L7_TRUE;

      pRuleMac->cos = 1;
      pRuleMac->cos2 = 2;
      for (k=0; k<L7_MAC_ADDR_LEN; k++)
      {
        pRuleMac->dstMac[k] = (L7_uchar8)(k + 0x30);                /* 30:31:32:33:34:35 */
        pRuleMac->dstMacMask[k] = (L7_uchar8)0xFF - (L7_uchar8)k;   /* FF:FE:FD:FC:FB:FA */

        pRuleMac->srcMac[k] = (L7_uchar8)(k + 0x40);                /* 40:41:42:43:44:45 */
        pRuleMac->srcMacMask[k] = (L7_uchar8)0xEF - (L7_uchar8)k;   /* EF:EE:ED:EC:EB:EA */
      }

      pRuleMac->etypeKeyId = (L7_ushort16)L7_QOS_ETYPE_KEYID_CUSTOM;
      pRuleMac->etypeValue = (L7_ushort16)(k + L7_QOS_ETYPE_CUSTOM_VAL_MIN);  /* 0x0600, etc. */

      pRuleMac->vlanId = 1000;
      pRuleMac->vlanIdStart = 1100;
      pRuleMac->vlanIdEnd = 1110;

      pRuleMac->vlanId2 = 2000;
      pRuleMac->vlanId2Start = 2200;
      pRuleMac->vlanId2End = 2220;

      pRuleMac->last = L7_FALSE;        /* ignored by config processing */
    } /* endfor j */
  } /* endfor i */
#endif

  /* build interface assignments */
  for (cfgIndex = 1; cfgIndex < L7_ACL_INTF_MAX_COUNT; cfgIndex++)
    aclBuildTestIntfConfigData(&configIdTemp[cfgIndex], &aclCfgFileData->cfgParms.aclIntfCfgData[cfgIndex]);

  aclCfgFileData->cfgParms.aclMsgLvl = ACL_MSGLVL_OFF;

  /* Build header */
  strcpy((char*)aclCfgFileData->cfgHdr.filename, ACL_CFG_FILENAME);
  aclCfgFileData->cfgHdr.version     = (L7_uint32)ACL_CFG_VER_CURRENT;
  aclCfgFileData->cfgHdr.componentID = aclCompId_g;
  aclCfgFileData->cfgHdr.type        = L7_CFG_DATA;
  aclCfgFileData->cfgHdr.length      = (L7_uint32)sizeof(aclCfgFileData_t);
  aclCfgFileData->cfgHdr.dataChanged = L7_FALSE;

  sysapiPrintf("\nBuilt ACL test config data\n\n");
}

/*********************************************************************
*
* @purpose  Save test ACL config data to NVStore
*
* @param    void
*
* @returns  void
*
* @comments Cannot use normal config save, since the config data contents 
*           get overwritten by the operational config.
*
* @end
*
*********************************************************************/
void aclSaveTestConfigData(void)
{
  aclCfgFileData->cfgHdr.dataChanged = L7_FALSE;
  aclCfgFileData->checkSum = nvStoreCrc32((L7_uchar8*)aclCfgFileData, 
                                          (L7_uint32)(sizeof(aclCfgFileData_t) - sizeof(aclCfgFileData->checkSum)) );

  if (sysapiCfgFileWrite(aclCompId_g, ACL_CFG_FILENAME,
                        (L7_char8 *) aclCfgFileData, (L7_uint32)sizeof(aclCfgFileData_t)) != L7_SUCCESS)
  {
    sysapiPrintf("\nAttempt to save ACL test config data failed!");
    return;
  }

  sysapiPrintf("\nSaved ACL test config data to NVStore\n\n");
}

/*********************************************************************
*
* @purpose  Dump test ACL config data contents
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
void aclConfigDataTestShow(void)
{
  /* config file header contents */
  sysapiCfgFileHeaderDump(&aclCfgFileData->cfgHdr);
  sysapiPrintf("\n");

  /* config file parameters */
  aclConfigDataShow();

  /* checksum */
  sysapiPrintf("aclCfgFileData->checkSum : 0x%8.8x\n", aclCfgFileData->checkSum);

  /* pertinent scaling constants */
  sysapiPrintf("\nScaling Constants\n");
  sysapiPrintf(  "-----------------\n");

  sysapiPrintf("L7_ACL_MAX_LISTS........................... %u\n", 
               (L7_uint32)L7_ACL_MAX_LISTS);

  sysapiPrintf("L7_ACL_MAX_RULE_NUM........................ %u\n", 
               (L7_uint32)L7_ACL_MAX_RULE_NUM);

  sysapiPrintf("L7_ACL_MAX_RULES_PER_MULTILIST............. %u\n", 
               (L7_uint32)L7_ACL_MAX_RULES_PER_MULTILIST);

  sysapiPrintf("L7_ACL_MAX_LISTS_PER_INTF_DIR.............. %u\n", 
               (L7_uint32)L7_ACL_MAX_LISTS_PER_INTF_DIR);

  sysapiPrintf("L7_ACL_INTF_MAX_COUNT...................... %u\n", 
               (L7_uint32)L7_ACL_INTF_MAX_COUNT);

  sysapiPrintf("ACL_INTF_DIR_MAX........................... %u\n", 
               (L7_uint32)ACL_INTF_DIR_MAX);

  sysapiPrintf("L7_ACL_NAME_LEN_MAX........................ %u\n", 
               (L7_uint32)L7_ACL_NAME_LEN_MAX);

  sysapiPrintf("\n");
}

/*============================================================================*/
/*================== END OF CONFIG MIGRATION DEBUG CHANGES ===================*/
/*============================================================================*/

