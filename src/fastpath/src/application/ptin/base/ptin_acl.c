/**
 * ptin_acl.c
 *  
 * Created on: 2013/19/30 
 * Author: Joao Mateiro
 * 
 */

#include <string.h>
#include "ptin_acl.h"
#include "ptin_evc.h"
#include "ptin_ipdtl0_packet.h"
#include "dot1q_api.h"
#include "dai_api.h"
#include "ptin_intf.h"

#define ACL_STANDARD  0
#define ACL_EXTENDED  1
#define ACL_NOTDEFIND 2

//#define __VLAN_XLATE__

/********************************************************************************** 
 *                            PTIN Local MAC/IP/IPv6 DB                           *
 **********************************************************************************/

struct {
  L7_uint32 aclId;
  L7_uint32 aclRuleNum[L7_MAX_NUM_RULES_PER_ACL+1];         /* [1..12] */
  L7_uint8  aclRuleCount;
  L7_BOOL   aclRuleCap;
  #ifdef __VLAN_XLATE__
  L7_uint32 aclRuleXlated[L7_MAX_NUM_RULES_PER_ACL+1];
  #endif
} ptin_aclMacDb[L7_MAX_ACL_LISTS];

struct {
  L7_uint32 aclId;
  L7_uint32 aclRuleNum[L7_MAX_NUM_RULES_PER_ACL+1];         /* [1..12] */
  L7_uint8  aclRuleCount;
  L7_BOOL   aclRuleCap;
} ptin_aclIpDb[2*L7_MAX_ACL_LISTS];                         /* Standard [1..99] and Extended [100..199]*/

struct {
  L7_uint32 aclId;
  L7_uint32 aclRuleNum[L7_MAX_NUM_RULES_PER_ACL+1];         /* [1..12] */
  L7_uint8  aclRuleCount;
  L7_BOOL   aclRuleCap;
} ptin_aclIpv6Db[L7_MAX_ACL_LISTS];                         /* IPv6 [1..99] */


L7_uint32 ptin_mirror_intfnum = 1; //FD_CNFGR_NIM_MIN_CPU_INTF_NUM;

void ptin_force_intfnum(L7_uint32 intfnum)
{
  ptin_mirror_intfnum = intfnum;
  printf("ptin_mirror_intfnum = %d\n", intfnum);
}

/********************************************************************************** 
 *                             MAC/IP/IPv6 ACL Clean                              *
 **********************************************************************************/


/**
 * 
 * 
 * @author alex (5/7/2015)
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_acl_init(void)
{
  sysnetNotifyEntry_t   snEntry;

  /* register with sysnet for sampled packets */
  bzero((char *)&snEntry, sizeof(sysnetNotifyEntry_t));
  strncpy(snEntry.funcName, "ptin_ipdtl0_mirrorPacketCapture", sizeof(snEntry.funcName));
  snEntry.notify_pdu_receive = ptin_ipdtl0_mirrorPacketCapture;
  snEntry.type = SYSNET_PKT_RX_REASON;
  snEntry.u.rxReason = L7_MBUF_RX_MIRROR;

  if (sysNetRegisterPduReceive(&snEntry) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Failure on sysNetRegisterPduReceive(ptinPacketCapture)");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/**
 * 
 * 
 * @author alex (5/7/2015)
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_acl_deinit(void)
{
  sysnetNotifyEntry_t   snEntry;

  /* register with sysnet for sampled packets */
  bzero((char *)&snEntry, sizeof(sysnetNotifyEntry_t));
  strncpy(snEntry.funcName, "ptin_ipdtl0_mirrorPacketCapture", sizeof(snEntry.funcName));
  snEntry.notify_pdu_receive = ptin_ipdtl0_mirrorPacketCapture;
  snEntry.type = SYSNET_PKT_RX_REASON;
  snEntry.u.rxReason = L7_MBUF_RX_MIRROR;

  if (sysNetDeregisterPduReceive(&snEntry) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Failure on sysNetDeregisterPduReceive(ptinPacketCapture)");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/**
 * Clean an IP ACL
 * 
 * @author joaom (11/4/2013)
 * 
 * @param isAclAdded 
 * @param mngAclId 
 * @param mngRuleId 
 */
void  ptin_aclIpClean(L7_BOOL isAclAdded, L7_uint32 mngAclId, L7_uint32 mngRuleId)
{
  L7_uint32 unit = 0;
  L7_RC_t   rc;

  if (ptin_aclIpDb[mngAclId].aclId == 0)
    return;
     
  if (isAclAdded == L7_FALSE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Removing Rule ID %d on ACL ID %d", ptin_aclIpDb[mngAclId].aclRuleNum[mngRuleId], ptin_aclIpDb[mngAclId].aclId);
    rc = usmDbQosAclRuleRemove(unit, ptin_aclIpDb[mngAclId].aclId, ptin_aclIpDb[mngAclId].aclRuleNum[mngRuleId]);
    ptin_aclIpDb[mngAclId].aclRuleCount--;

    /* If this is the last rule, force this ACL removal */
    if (ptin_aclIpDb[mngAclId].aclRuleCount == 0)
    {
      isAclAdded = L7_TRUE;
    }
  }

  if (isAclAdded == L7_TRUE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Removing ACL ID %d", ptin_aclIpDb[mngAclId].aclId);

    rc = usmDbQosAclDelete(unit, ptin_aclIpDb[mngAclId].aclId);

    ptin_aclIpDb[mngAclId].aclId = 0;
    memset(ptin_aclIpDb[mngAclId].aclRuleNum, 0, L7_MAX_NUM_RULES_PER_ACL);
    ptin_aclIpDb[mngAclId].aclRuleCount = 0;
    ptin_aclIpDb[mngAclId].aclRuleCap = L7_FALSE;
  }

  /* No need to check error case. This is garbage cleaner and doesn't
     throw any error message*/
}


/**
 * Clean All IP ACL
 * 
 * @author joaom (11/5/2013)
 * 
 */
void  ptin_aclIpCleanAll(void)
{
  int i;

  for (i=0; i<(2*L7_MAX_ACL_LISTS); i++)
  {
    ptin_aclIpClean(L7_TRUE, i, 0);
  }
}


/**
 * Clean an IPv6 ACL
 * 
 * @author joaom (11/4/2013)
 * 
 * @param isAclAdded 
 * @param mngAclId 
 * @param mngRuleId 
 */
void  ptin_aclIpv6Clean(L7_BOOL isAclAdded, L7_uint32 mngAclId, L7_uint32 mngRuleId)
{
  L7_uint32 unit = 0;
  L7_RC_t   rc;

  if (ptin_aclIpv6Db[mngAclId].aclId == 0)
    return;

  if( isAclAdded == L7_FALSE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Removing Rule ID %d on ACL ID %d", ptin_aclIpv6Db[mngAclId].aclRuleNum[mngRuleId], ptin_aclIpv6Db[mngAclId].aclId);
    rc = usmDbQosAclRuleRemove(unit, ptin_aclIpv6Db[mngAclId].aclId, ptin_aclIpv6Db[mngAclId].aclRuleNum[mngRuleId]);
    ptin_aclIpv6Db[mngAclId].aclRuleCount--;

    /* If this is the last rule, force this ACL removal */
    if (ptin_aclIpv6Db[mngAclId].aclRuleCount == 0)
    {
      isAclAdded = L7_TRUE;
    }
  }

  if( isAclAdded == L7_TRUE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Removing ACL ID %d", ptin_aclIpv6Db[mngAclId].aclId);
    rc = usmDbQosAclDelete(unit, ptin_aclIpv6Db[mngAclId].aclId);

    ptin_aclIpv6Db[mngAclId].aclId = 0;
    memset(ptin_aclIpv6Db[mngAclId].aclRuleNum, 0, L7_MAX_NUM_RULES_PER_ACL);
    ptin_aclIpv6Db[mngAclId].aclRuleCount = 0;
    ptin_aclIpv6Db[mngAclId].aclRuleCap = L7_FALSE;
  }  

  /* No need to check error case. This is garbage cleaner and doesn't
     throw any error message*/
}


/**
 * Clean All IPv6 ACL
 * 
 * @author joaom (11/5/2013)
 * 
 */
void  ptin_aclIpv6CleanAll(void)
{
  int i;

  for (i=0; i<L7_MAX_ACL_LISTS; i++)
  {
    ptin_aclIpv6Clean(L7_TRUE, i, 0);
  }
}


/**
 * Clean a MAC ACL
 * 
 * @author joaom (10/30/2013)
 * 
 * @param isAclAdded 
 * @param mngAclId 
 * @param mngRuleId 
 */
void ptin_aclMacClean(L7_BOOL isAclAdded, L7_uint32 mngAclId, L7_uint32 mngRuleId)
{
  L7_uint32 unit = 0;
  L7_RC_t   rc;

  if (ptin_aclMacDb[mngAclId].aclId == 0)
    return;

  if (isAclAdded == L7_FALSE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Removing Rule ID %d on ACL ID %d", ptin_aclMacDb[mngAclId].aclRuleNum[mngRuleId], ptin_aclMacDb[mngAclId].aclId);
    rc = usmDbQosAclMacRuleRemove(unit, ptin_aclMacDb[mngAclId].aclId, ptin_aclMacDb[mngAclId].aclRuleNum[mngRuleId]);
    ptin_aclMacDb[mngAclId].aclRuleCount--;

    /* If this is the last rule, force this ACL removal */
    if (ptin_aclMacDb[mngAclId].aclRuleCount == 0)
    {
      isAclAdded = L7_TRUE;
    }
  }

  if (isAclAdded == L7_TRUE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Removing ACL ID %d", ptin_aclMacDb[mngAclId].aclId);
    rc = usmDbQosAclMacDelete(unit, ptin_aclMacDb[mngAclId].aclId);

    ptin_aclMacDb[mngAclId].aclId = 0;
    memset(ptin_aclMacDb[mngAclId].aclRuleNum, 0, L7_MAX_NUM_RULES_PER_ACL);
    ptin_aclMacDb[mngAclId].aclRuleCount = 0;
    ptin_aclMacDb[mngAclId].aclRuleCap = L7_FALSE;
    #ifdef __VLAN_XLATE__
    memset(ptin_aclMacDb[mngAclId].aclRuleXlated, 0, L7_MAX_NUM_RULES_PER_ACL);
    #endif
  }  

  /* No need to check error case. This is garbage cleaner and doesn't
    throw any error message */
}


/**
 * Clean All MAC ACL
 * 
 * @author joaom (11/05/2013)
 *
 */
void ptin_aclMacCleanAll(void)
{
  int i;

  for (i=0; i<L7_MAX_ACL_LISTS; i++)
  {
    ptin_aclMacClean(L7_TRUE, i, 0);
  }  
}


/**
 * Clean All ACL
 * 
 * @author joaom (11/07/2013)
 *
 */
void ptin_aclCleanAll(void)
{
  ptin_aclIpCleanAll();
  ptin_aclIpv6CleanAll();
  ptin_aclMacCleanAll();
}



/********************************************************************************** 
 *                                   IP ACL                                       *
 **********************************************************************************/

/**
 * Create an IP ACL Rule
 * 
 * @author joaom (11/04/2013)
 * 
 * @param msgAcl 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclIpRuleConfig(msg_ip_acl_t *msgAcl, ACL_OPERATION_t operation)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 aclId = 0;
  L7_char8 accessListName[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 aclRuleNum;
  L7_uint32 aclruleNext;
  L7_uint32 unit = 0, maxAcls;
  L7_uint32 aclType;
  L7_uint32 actionType;
  L7_BOOL maskSupported = L7_FALSE;
  L7_uint32 protVal = 0;
  L7_uint32 srcIpAddr = 0;
  L7_uint32 srcIpMask = 0;
  L7_uint32 dstIp = 0;
  L7_uint32 dstIpMask = 0;
  L7_int32 srcPortValue = -1;
  L7_int32 srcStartPort = -1;
  L7_int32 srcEndPort = -1;
  L7_int32 dstPortValue = -1;
  L7_int32 dstStartPort = -1;
  L7_int32 dstEndPort = -1;
  L7_int32 precVal = -1;
  L7_int32 tosVal =-1;
  L7_int32 dscpVal = -1;
  L7_int32 mirrorVal = -1;
  L7_uint32 tosMask = 0;
  L7_BOOL matchEvery= L7_FALSE;
  L7_BOOL matchSrc = L7_FALSE;
  L7_BOOL matchDst = L7_FALSE;
  L7_BOOL isAclAdded = L7_FALSE;
  L7_BOOL matchOther = L7_FALSE;
  L7_BOOL configuringNamedIpAcl = L7_FALSE;


  /* *********************** ACL_OPERATION_REMOVE *********************** */
  if (operation == ACL_OPERATION_REMOVE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Removing MNG Rule ID %d on MNG ACL ID %d", msgAcl->aclRuleId, msgAcl->aclId);

    if ( (ptin_aclIpDb[msgAcl->aclId].aclId != 0) && (ptin_aclIpDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] != 0) )
    {
      ptin_aclIpClean(L7_FALSE, msgAcl->aclId, msgAcl->aclRuleId);
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Entry is empty");
    }

    return L7_SUCCESS;
  }


  /* *********************** ACL_OPERATION_CREATE *********************** */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Applying Rule...");

  if (msgAcl->action == ACL_ACTION_PERMIT)
  {
    actionType = L7_ACL_PERMIT;
  }
  else if (msgAcl->action == ACL_ACTION_DENY)
  {
    actionType = L7_ACL_DENY;
  }
  else if (msgAcl->action == ACL_ACTION_CAPTURE)
  {
    actionType = L7_ACL_PERMIT;
    mirrorVal = ptin_mirror_intfnum;

    ptin_aclIpDb[msgAcl->aclId].aclRuleCap = L7_TRUE;

    /* When an ACL with a CAPTURE rule is applied to a VLAN, on Line Cards we will assume the configuration as a permit
       to allow that traffic and block the other. The mirror/capture rule will be applied only on the Matrix */
    #if (PTIN_BOARD_IS_LINECARD)
    mirrorVal = -1;
    #endif
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid Action (%d)", msgAcl->action);
    return L7_FAILURE;
  }


  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE)
  {
    maskSupported = L7_TRUE;
  }


  /* determine if we are in named IPv4 access-list config mode */
  if (msgAcl->aclType == ACL_TYPE_IP_NAMED)
  {
    /* we are in ipv4 named access list mode, retrieve ACL name for list being configured */
    strcpy(accessListName, msgAcl->name);

    aclType = ACL_EXTENDED;

    configuringNamedIpAcl = L7_TRUE;
  }
  else if ( (msgAcl->aclType == ACL_TYPE_IP_STANDARD) || (msgAcl->aclType == ACL_TYPE_IP_EXTENDED))
  {
    /* we are configuring a numbered ACL */
    aclId = msgAcl->aclId;
    
    if (msgAcl->aclType == ACL_TYPE_IP_STANDARD)
    {
      aclType = ACL_STANDARD;
    }
    else
    {
      aclType = ACL_EXTENDED;
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid ACL Type (%d)", msgAcl->aclType);
    return L7_FAILURE;
  }


  if (msgAcl->aclRuleMask == ACL_IP_RULE_MASK_NONE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Every");
    matchEvery = L7_TRUE;
  }

  if (matchEvery == L7_FALSE)
  {
    if (!(msgAcl->aclRuleMask & ACL_IP_RULE_MASK_protocol))
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Assuming protocol is IP");
      msgAcl->protocol = L7_ACL_PROTOCOL_IP;
    }

    /* Check the protocol type*/
    if (aclType != ACL_STANDARD)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_PROTOCOL_FEATURE_ID) == L7_TRUE)
      {
        protVal = msgAcl->protocol;

        /* [0..255] */
        if ((protVal < L7_ACL_MIN_PROTOCOL_NUM) || (protVal > L7_ACL_MAX_PROTOCOL_NUM))
        {
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid Protocol Value [0..255] (%d)", protVal);
          return L7_FAILURE;
        }

        if (protVal != L7_ACL_PROTOCOL_IP)
        {
          matchOther = L7_TRUE;
        }
      }
    }


    /* Check Src IP */
    if ( (aclType != ACL_STANDARD) && !(msgAcl->aclRuleMask & ACL_IP_RULE_MASK_srcIpAddr) )
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_FALSE)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID");
        return L7_FAILURE;
      }
      matchSrc = L7_TRUE;
    }
    else if ( (aclType == ACL_STANDARD) && !(msgAcl->aclRuleMask & ACL_IP_RULE_MASK_srcIpAddr) )
    {
      /* In Standard ACL Type, Src IP must be specified */
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: In Standard ACL Type, Src IP must be specified");
      return L7_FAILURE;
    }

    if (matchSrc == L7_FALSE && usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) == L7_TRUE)
    {
      if ((aclType != ACL_STANDARD) && (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE))
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_FALSE)
        {
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID");
          return L7_FAILURE;
        }        
      }

      PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Src IP");
      memcpy(&srcIpAddr, &msgAcl->srcIpAddr, sizeof(srcIpAddr));
      memcpy(&srcIpMask, &msgAcl->srcIpMask, sizeof(srcIpMask));
    }
  }

  if (aclType == ACL_EXTENDED)
  {
    /* Check Src L4 Port */
    if (msgAcl->aclRuleMask & ACL_IP_RULE_MASK_srcStartPort)
    {
      if (msgAcl->srcStartPort == msgAcl->srcEndPort)
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Src L4 Port");

        srcPortValue = msgAcl->srcStartPort;

        if(srcPortValue < L7_ACL_MIN_L4PORT_NUM || srcPortValue > L7_ACL_MAX_L4PORT_NUM)
        {
          /* Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535> */
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535>");
          return L7_FAILURE;
        }
      }
      else
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Src L4 Port Range");

        srcStartPort = msgAcl->srcStartPort;
        srcEndPort = msgAcl->srcEndPort;

        if(srcStartPort < L7_ACL_MIN_L4PORT_NUM || srcEndPort > L7_ACL_MAX_L4PORT_NUM)
        {
          /* Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535> */
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535>");
          return L7_FAILURE;
        }

        if (srcEndPort < srcStartPort)
        {
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535>");
          return L7_FAILURE;
        }
      }

      matchOther = L7_TRUE;
    }


    /* Check Dst IP */
    if (!(msgAcl->aclRuleMask & ACL_IP_RULE_MASK_dstIpAddr))
    {
      if (matchEvery == L7_FALSE)
      {
        matchDst = L7_TRUE;
      }
    }

    if (matchEvery == L7_FALSE && matchDst == L7_FALSE && usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE)
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Dst IP");

      memcpy(&dstIp, &msgAcl->dstIpAddr, sizeof(dstIp));
      memcpy(&dstIpMask, &msgAcl->dstIpMask, sizeof(dstIpMask));
    }


    /* Check Dst L4 Port */
    if (msgAcl->aclRuleMask & ACL_IP_RULE_MASK_dstStartPort)
    {
      if (msgAcl->dstStartPort == msgAcl->dstEndPort)
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Dst L4 Port");

        dstPortValue = msgAcl->dstStartPort;

        if(dstPortValue < L7_ACL_MIN_L4PORT_NUM || dstPortValue > L7_ACL_MAX_L4PORT_NUM)
        {
          /* Invalid start DSTL4 port. <select DSTL4 port between 0 to 65535> */
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid start DSTL4 port. <select DSTL4 port between 0 to 65535>");
          return L7_FAILURE;
        }
      }
      else
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Dst L4 Port Range");

        dstStartPort = msgAcl->dstStartPort;
        dstEndPort = msgAcl->dstEndPort;

        if(dstStartPort < L7_ACL_MIN_L4PORT_NUM || dstEndPort > L7_ACL_MAX_L4PORT_NUM)
        {
          /* Invalid start DSTL4 port. <select DSTL4 port between 0 to 65535> */
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid start DSTL4 port. <select DSTL4 port between 0 to 65535>");
          return L7_FAILURE;
        }

        if (dstEndPort < dstStartPort)
        {
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid start DSTL4 port. <select DSTL4 port between 0 to 65535>");
          return L7_FAILURE;
        }
      }

      matchOther = L7_TRUE;
    }


    /* Check PREC */
    if (msgAcl->aclRuleMask & ACL_IP_RULE_MASK_precVal)
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Matching PREC");

      precVal = msgAcl->precVal;
      matchOther = L7_TRUE;
    }

    /* Check TOS */
    else if (msgAcl->aclRuleMask & ACL_IP_RULE_MASK_tosVal)
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Matching TOS");

      tosVal = msgAcl->tosVal;
      matchOther = L7_TRUE;

      if (maskSupported == TRUE)
      {
        tosMask = msgAcl->tosMask;
        matchOther = L7_TRUE;
      }
    }


    /* Check DSCP */
    else if (msgAcl->aclRuleMask & ACL_IP_RULE_MASK_dscpVal)
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Matching DSCP");

      dscpVal = msgAcl->dscpVal;

      /* verify if the specified value is in between 0 to 63 */
      if ((dscpVal < L7_ACL_MIN_DSCP) || (dscpVal > L7_ACL_MAX_DSCP))
      {
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: DSCP value must be between 0 to 63");
          return L7_FAILURE;
      }
      else
      {
        matchOther = L7_TRUE;
      }
    }
  }


  /* Apply the Rule */

  if (configuringNamedIpAcl == L7_FALSE)
  {
    if (ptin_aclIpDb[msgAcl->aclId].aclId == 0) /* ACL does not exists */
    {
      rc = usmDbQosAclCreate(unit, aclId); 
      switch (rc)
      {
        case L7_SUCCESS:
          break;

        case L7_FAILURE:
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclCreate return L7_FAILURE");
          return L7_FAILURE;

        case L7_TABLE_IS_FULL:
          usmDbQosAclMaxNumGet(unit, &maxAcls);           /* This function always returns L7_SUCCESS */
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclCreate return L7_TABLE_IS_FULL (maxAcls=%d)", maxAcls);
          return L7_FAILURE;

        case L7_ERROR:
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclCreate return L7_ERROR");
          return L7_FAILURE;

        case L7_REQUEST_DENIED:
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclCreate return L7_REQUEST_DENIED");
          return L7_FAILURE;

        default:
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclCreate return (rc=%d)", rc);
          return L7_FAILURE;
      }

      /* keep the association between MNG and usmDb ACL IDs */
      ptin_aclIpDb[msgAcl->aclId].aclId = aclId;
      PT_LOG_DEBUG(LOG_CTX_MSG, "Created New ACL: ID %d (MNG ID %d)", aclId, msgAcl->aclId);
      isAclAdded = L7_TRUE;
    }
    else  /* ACL already exists */
    {
      aclId = ptin_aclIpDb[msgAcl->aclId].aclId;
      PT_LOG_DEBUG(LOG_CTX_MSG, "Configuring existing ACL: ID %d (MNG ID %d)", aclId, msgAcl->aclId);
    }
  }
  else
  {
    /* Does not exist - create and add */
    if (usmDbQosAclNameToIndex(unit, accessListName, &aclId) != L7_SUCCESS)
    {
      rc = usmDbQosAclNamedIndexNextFree(unit, L7_ACL_TYPE_IP, &aclId);
      if (rc != L7_SUCCESS)
      {
        if (rc == L7_TABLE_IS_FULL)
        {
          (void)usmDbQosAclMaxNumGet(unit, &maxAcls);           /* This function always returns L7_SUCCESS */
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclNamedIndexNextFree return L7_TABLE_IS_FULL (maxAcls=%d)", maxAcls);
          return L7_FAILURE;
        }
        else
        {
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclNamedIndexNextFree return (rc=%d)", rc);
          return L7_FAILURE;
        }
      }

      rc = usmDbQosAclCreate(unit, aclId);
      switch (rc)
      {
        case L7_SUCCESS:
          break;

        case L7_TABLE_IS_FULL:
          usmDbQosAclMaxNumGet(unit, &maxAcls);           /* This function always returns L7_SUCCESS */
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclCreate return L7_TABLE_IS_FULL (maxAcls=%d)", maxAcls);
          return L7_FAILURE;

        case L7_REQUEST_DENIED:
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclCreate return L7_REQUEST_DENIED");
          return L7_FAILURE;

        default:
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclCreate return (rc=%d)", rc);
          return L7_FAILURE;
      }

      /* keep the association between MNG and usmDb ACL IDs */
      ptin_aclIpDb[msgAcl->aclId].aclId = aclId;
      PT_LOG_DEBUG(LOG_CTX_MSG, "Created New ACL: ID %d (MNG ID %d)", aclId, msgAcl->aclId);
      isAclAdded = L7_TRUE;

      if (usmDbQosAclNameAdd(unit, aclId, accessListName) != L7_SUCCESS)
      {
        ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclNameAdd return (rc=%d)", rc);
        return L7_FAILURE;
      }
    }
  }


  /* Verify if rule exists */
  if (ptin_aclIpDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] != 0)
  {
    aclRuleNum = ptin_aclIpDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId];
    PT_LOG_DEBUG(LOG_CTX_MSG, "Configuring existing Rule ID %d (MNG ID %d) on ACL ID %d (MNG ID %d)", aclRuleNum, msgAcl->aclRuleId, aclId, msgAcl->aclId);
  }
  /* Get the new rule number */
  else
  {
    if (usmDbQosAclRuleGetFirst(unit, aclId, &aclRuleNum)!= L7_SUCCESS)
    {
      aclRuleNum = L7_ACL_MIN_RULE_NUM;
    }
    else
    {
      while (usmDbQosAclRuleGetNext(unit, aclId, aclRuleNum, &aclruleNext) == L7_SUCCESS)
      {
        aclRuleNum = aclruleNext;
      }
      aclRuleNum = aclRuleNum+1;
    }
    if (aclRuleNum > L7_ACL_MAX_RULE_NUM)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: aclRuleNum > L7_ACL_MAX_RULE_NUM");
      return L7_FAILURE;
    }

    /* keep the association between MNG and usmDb Rule IDs */
    ptin_aclIpDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] = aclRuleNum;
    ptin_aclIpDb[msgAcl->aclId].aclRuleCount++;
    PT_LOG_DEBUG(LOG_CTX_MSG, "Created New Rule ID %d (MNG ID %d) on ACL ID %d (MNG ID %d)", aclRuleNum, msgAcl->aclRuleId, aclId, msgAcl->aclId);
  }

  /* Add the rule to acl*/
  rc = usmDbQosAclRuleActionAdd(unit, aclId, aclRuleNum, actionType);
  switch (rc)
  {
    case L7_SUCCESS:
      break;

    case L7_FAILURE:
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleActionAdd return L7_FAILURE");
      return L7_FAILURE;

    case L7_ERROR:
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleActionAdd return L7_ERROR");
      return L7_FAILURE;

    case L7_TABLE_IS_FULL:
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleActionAdd return L7_TABLE_IS_FULL");
      return L7_FAILURE;

    case L7_REQUEST_DENIED:
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleActionAdd return L7_REQUEST_DENIED");
      return L7_FAILURE;

    default:
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleActionAdd return (rc=%d)", rc);
      return L7_FAILURE;
  }


  /* Match Every */
  if ((matchEvery == L7_TRUE) ||
      (aclType == ACL_STANDARD && matchSrc == L7_TRUE) ||
      (aclType != ACL_STANDARD && matchSrc == L7_TRUE && matchDst == L7_TRUE && matchOther == L7_FALSE))
  {
    rc = usmDbQosAclRuleEveryAdd(unit, aclId, aclRuleNum, L7_TRUE);
    if (rc == L7_FAILURE)
    {
      ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleEveryAdd return L7_FAILURE");
      return L7_FAILURE;
    }

    if (rc == L7_ERROR)
    {
      ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleEveryAdd return L7_ERROR");
      return L7_FAILURE;
    }

    if (rc == L7_REQUEST_DENIED)
    {
      ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleEveryAdd return L7_REQUEST_DENIED");
      return L7_FAILURE;
    }

    /* Mirror */
    if (mirrorVal > -1)
    {
      rc = usmDbQosAclRuleMirrorIntfAdd(unit, aclId, aclRuleNum, mirrorVal);
      if (rc != L7_SUCCESS)
      {
        ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleMirrorIntfAdd return L7_REQUEST_DENIED");
        return L7_FAILURE;
      }
    }

  }
  else
  {
    if (matchSrc == L7_FALSE)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) == L7_TRUE)
      {
        rc = usmDbQosAclRuleSrcIpMaskAdd(unit, aclId, aclRuleNum, srcIpAddr, srcIpMask);
        if (rc == L7_FAILURE)
        {
          ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleSrcIpMaskAdd return L7_FAILURE");
          return L7_FAILURE;
        }
        else if (rc == L7_REQUEST_DENIED)
        {
          ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleSrcIpMaskAdd return L7_REQUEST_DENIED");
          return L7_FAILURE;
        }
      }
    }

    /* remaining match fields only meaningful for IP Extended ACLs */
    if (aclType != ACL_STANDARD)
    {
      /* Add the protocol value*/
      if (protVal != 0)
      {
        rc = usmDbQosAclRuleProtocolAdd(unit, aclId, aclRuleNum, protVal);
        if (rc != L7_SUCCESS)
        {
          ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleProtocolAdd return (rc=%d)", rc);
          return L7_FAILURE;
        }
      }
      if (srcPortValue > -1)
      {
        rc = usmDbQosAclRuleSrcL4PortAdd(unit, aclId, aclRuleNum, srcPortValue);
      }
      else if (srcStartPort > -1)
      {
        rc = usmDbQosAclRuleSrcL4PortRangeAdd(unit, aclId, aclRuleNum,srcStartPort, srcEndPort);
      }
      if (rc != L7_SUCCESS)
      {
        ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleSrcL4Port(Range)Add return (rc=%d)", rc);
        return L7_FAILURE;
      }

      if (matchDst == L7_FALSE)
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE)
        {
          rc = usmDbQosAclRuleDstIpMaskAdd(unit, aclId, aclRuleNum,dstIp,dstIpMask);
          if (rc == L7_FAILURE)
          {
            ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
            PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleDstIpMaskAdd return L7_FAILURE");
            return L7_FAILURE;
          }
          else if (rc == L7_REQUEST_DENIED)
          {
            ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
            PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleDstIpMaskAdd return L7_REQUEST_DENIED");
            return L7_FAILURE;
          }
        }
      }

      if (dstPortValue > -1)
      {
        rc = usmDbQosAclRuleDstL4PortAdd(unit, aclId, aclRuleNum, dstPortValue);
      }
      else if (dstStartPort > -1)
      {
        rc = usmDbQosAclRuleDstL4PortRangeAdd(unit, aclId, aclRuleNum,dstStartPort, dstEndPort);
      }
      if (rc != L7_SUCCESS)
      {
        ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleDstL4PortRangeAdd return (rc=%d)", rc);
        return L7_FAILURE;
      }

      if (precVal > -1)
      {
        rc = usmDbQosAclRuleIPPrecedenceAdd(unit, aclId, aclRuleNum, precVal);
        if (rc != L7_SUCCESS)
        {
          ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleIPPrecedenceAdd return (rc=%d)", rc);
          return L7_FAILURE;
        }
      }

      if (tosVal > -1)
      {
        rc = usmDbQosAclRuleIPTosAdd(unit, aclId, aclRuleNum, tosVal, ~tosMask & 0x000000FF);
        if (rc != L7_SUCCESS)
        {
          ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleIPTosAdd return (rc=%d)", rc);
          return L7_FAILURE;
        }
      }

      if (dscpVal > -1)
      {
        rc = usmDbQosAclRuleIPDscpAdd(unit, aclId, aclRuleNum, dscpVal);
        if (rc != L7_SUCCESS)
        {
          ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleIPDscpAdd return (rc=%d)", rc);
          return L7_FAILURE;
        }
      }
    }

    /* Mirror */
    if (mirrorVal > -1)
    {
      rc = usmDbQosAclRuleMirrorIntfAdd(unit, aclId, aclRuleNum, mirrorVal);
      if (rc != L7_SUCCESS)
      {
        ptin_aclIpClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleMirrorIntfAdd return (rc=%d)", rc);
        return L7_FAILURE;
      }
    }
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "Rule Applied!");

  return L7_SUCCESS;
}


/**
 * Apply an IP ACL to an interface or VLAN
 * 
 * @author joaom (11/04/2013)
 * 
 * @param aclApply 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclIpApply(ptin_acl_apply_t *aclApply, ACL_OPERATION_t operation)
{
  L7_BOOL     aclVlanIdGiven = L7_FALSE;
  L7_RC_t     rc;
  L7_uint32   aclId;
  ptin_intf_t ptin_intf;
  L7_uint32   intIfNum, vlanId = 0;
  L7_char8    aclName[L7_ACL_NAME_LEN_MAX+1];
  L7_uint32   direction, feature;
  L7_uint32   unit = 0;
  L7_uint32   sequence = L7_ACL_AUTO_INCR_INTF_SEQ_NUM;
  L7_BOOL     isNamedAcl = L7_FALSE;
  L7_RC_t     status = L7_SUCCESS;


  /* determine if ACL is being applied to a VLAN ID */
  if (aclApply->number_of_vlans > 0 &&
      aclApply->vlanId[0] != L7_ACL_INVALID_VLAN_ID)
  {
    aclVlanIdGiven = L7_TRUE;

    /* get the vlan id specified by msg */
    vlanId = aclApply->vlanId[0];

    PT_LOG_DEBUG(LOG_CTX_MSG, "Applying ACL to VLAN ID %d", vlanId);

    /* verify if the specified value is in valid range */
    if (usmDbQosAclIsValidVlan(unit, vlanId) == L7_FALSE)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: VLAN ID is invalid");
      return L7_FAILURE;
    }
  }
  else
  {
    if (ptin_intf_port2intIfNum(aclApply->interface, &intIfNum) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Cannot convert port %u to intIfNum", aclApply->interface);
      return L7_FAILURE;
    }
    if (ptin_intf_port2ptintf(aclApply->interface, &ptin_intf) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Cannot convert port %u to ptin_intf", aclApply->interface);
      return L7_FAILURE;
    }
    PT_LOG_DEBUG(LOG_CTX_MSG, "Applying ACL to Interface %d", aclApply->interface);
  }


  /* determine if we are in named IPv4 ACL */
  if (aclApply->aclType == ACL_TYPE_IP_NAMED)
  {
    /* we are in ipv4 named access list mode, retrieve ACL name for list being configured */
    strcpy(aclName, aclApply->name);

    if ((usmDbQosAclNameToIndex(unit, aclName, &aclId) != L7_SUCCESS) ||
        (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IP, aclId) != L7_SUCCESS))
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: ACL Name NOT found");
      if (operation == ACL_OPERATION_CREATE)
      {
        return L7_FAILURE;
      }
      else
      {
        return L7_SUCCESS;
      }
    }

    isNamedAcl = L7_TRUE;
  }
  else
  {
    aclId = aclApply->aclId;
  }


  if ((isNamedAcl == L7_FALSE) && ((aclId < L7_MIN_ACL_ID ) || (aclId > L7_MAX_ACL_ID)))
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: ACD ID out of range");
    return L7_FAILURE;
  }

  rc = usmDbQosAclNumCheckValid(unit, aclId);
  if (rc == L7_FAILURE)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclNumCheckValid return (%d)", rc);
    return L7_FAILURE;
  }


  if (aclApply->direction == ACL_DIRECTION_IN)          /* inbound */
  {
    feature = L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID;
    direction = L7_INBOUND_ACL;
  }
  else if (aclApply->direction == ACL_DIRECTION_OUT)    /* outbound */
  {
    feature = L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID;
    direction = L7_OUTBOUND_ACL;
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid direction (%d)", aclApply->direction);
    return L7_FAILURE;
  }


  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, feature) == L7_FALSE)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: L7_FLEX_QOS_ACL_COMPONENT_ID");
    return L7_FAILURE;
  }

  if (aclVlanIdGiven == L7_FALSE)
  {
    PT_LOG_TRACE(LOG_CTX_MSG, "operation=%d intIfNum=%d direction=%d aclId=%u sequence=%d", operation, intIfNum, direction, aclId, sequence);

    if (operation == ACL_OPERATION_CREATE)
    {
      /* When an ACL with a CAPTURE rule is applied to an interface that belongs to a Line Card an error is returned */
      #if (PTIN_BOARD_IS_LINECARD)
      if (ptin_aclIpDb[aclApply->aclId].aclRuleCap == L7_TRUE)
        return L7_FAILURE;
      #endif

      rc = usmDbQosAclInterfaceDirectionAdd(unit, intIfNum, direction, aclId, sequence);
    }
    else /* ACL_OPERATION_REMOVE */
    {
      rc = usmDbQosAclInterfaceDirectionRemove(unit, intIfNum, direction, aclId);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "No ACL to be Unapplied");
        return L7_SUCCESS;
      }
    }

    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclInterfaceDirectionRemove return (rc=%d)", rc);
      status = L7_FAILURE;
      return L7_FAILURE;
    }
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_MSG, "operation=%d vlanId=%d direction=%d aclId=%u sequence=%d", operation, vlanId, direction, aclId, sequence);

    #ifdef __VLAN_XLATE__
    L7_uint32 oldVlan = vlanId;
    L7_uint16 newVlan;

    /* ...and translate it to the internal VLAN ID */
    rc = ptin_evc_intVlan_get_fromOVlan(&ptin_intf, oldVlan, 0, &newVlan);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE (rc=%d)", rc);
      return L7_FAILURE;
    }

    PT_LOG_DEBUG(LOG_CTX_MSG, "Translating VLAN ID %d to %d (rc=%d)", oldVlan, (L7_uint32) newVlan, rc);
    vlanId = (L7_uint32) newVlan;
    #endif

    /* applying ACL to a VLAN */
    if (operation == ACL_OPERATION_CREATE)
    {
      if ((rc = usmDbQosAclVlanDirectionAdd(unit, vlanId, direction, aclId, sequence)) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclVlanDirectionAdd return (rc=%d)", rc);
        return L7_FAILURE;
      }
    }
    else  /* ACL_OPERATION_REMOVE */
    {
      if ((rc = usmDbQosAclVlanDirectionRemove(unit, vlanId, direction, aclId)) != L7_SUCCESS)
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "No ACL to be Unapplied");
        return L7_SUCCESS;
      }
    }
  }

  return L7_SUCCESS;
}



/********************************************************************************** 
 *                                  IPv6 ACL                                      *
 **********************************************************************************/


/**
 * Create an IPv6 ACL Rule
 * 
 * @author joaom (11/06/2013)
 * 
 * @param msgAcl 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclIpv6RuleConfig(msg_ipv6_acl_t *msgAcl, ACL_OPERATION_t operation)
{
  L7_RC_t rc;
  L7_uint32 aclId = 0;
  L7_uint32 aclRuleNum;
  L7_uint32 aclruleNext;
  L7_uint32 unit = 0, maxAcls;
  L7_char8 accessListName[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 actionType;
  L7_BOOL maskSupported = L7_FALSE;
  L7_uint32 protVal = 0;
  L7_in6_prefix_t srcIpv6Addr;
  L7_in6_prefix_t dstIpv6Addr;
  L7_int32 srcPortValue = -1;
  L7_int32 srcStartPort = -1;
  L7_int32 srcEndPort = -1;
  L7_int32 dstPortValue = -1;
  L7_int32 dstStartPort = -1;
  L7_int32 dstEndPort = -1;
  L7_int32 dscpVal = -1;
  L7_int32 mirrorVal = -1;
  L7_int32 flowLabelVal = -1;
  L7_BOOL matchEvery = L7_FALSE;
  L7_BOOL matchSrc = L7_FALSE;
  L7_BOOL matchDst = L7_FALSE;
  L7_BOOL isAclAdded = L7_FALSE;
  L7_BOOL matchOther = L7_FALSE;


  /* *********************** ACL_OPERATION_REMOVE *********************** */
  if (operation == ACL_OPERATION_REMOVE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Removing MNG Rule ID %d on MNG ACL ID %d", msgAcl->aclRuleId, msgAcl->aclId);

    if ((ptin_aclIpv6Db[msgAcl->aclId].aclId != 0) && (ptin_aclIpv6Db[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] != 0))
    {
      ptin_aclIpv6Clean(L7_FALSE, msgAcl->aclId, msgAcl->aclRuleId);
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Entry is empty");
    }

    return L7_SUCCESS;
  }


  /* *********************** ACL_OPERATION_CREATE *********************** */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Applying Rule...");

  if (msgAcl->action == ACL_ACTION_PERMIT)
  {
    actionType = L7_ACL_PERMIT;
  }
  else if (msgAcl->action == ACL_ACTION_DENY)
  {
    actionType = L7_ACL_DENY;
  }
  else if (msgAcl->action == ACL_ACTION_CAPTURE)
  {
    actionType = L7_ACL_PERMIT;
    mirrorVal = ptin_mirror_intfnum;    /* TODO: Currently the intfnum is being ignored on the hapi layer and the CPU port is forced */

    ptin_aclIpv6Db[msgAcl->aclId].aclRuleCap = L7_TRUE;
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid Action (%d)", msgAcl->action);
    return L7_FAILURE;
  }


  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_TRUE)
  {
    maskSupported = L7_TRUE;
  }


  /* determine if we are in IPv6 ACL */
  if (msgAcl->aclType == ACL_TYPE_IPv6_EXTENDED)
  {
    /* Retrieve ACL name for list being configured */
    strcpy(accessListName, msgAcl->name);
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid ACL Type (%d)", msgAcl->aclType);
    return L7_FAILURE;
  }

  if (msgAcl->aclRuleMask == ACL_IPv6_RULE_MASK_NONE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Every");
    matchEvery = L7_TRUE;
  }

  if (matchEvery == L7_FALSE)
  {
    if (!(msgAcl->aclRuleMask & ACL_IPv6_RULE_MASK_protocol))
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Assuming protocol is IPv6");
      msgAcl->protocol = L7_ACL_PROTOCOL_IP;
    }

    /* Check the protocol type*/
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_IPV6_PROTOCOL_FEATURE_ID) == L7_TRUE)
    {
      protVal = msgAcl->protocol;

      if ((protVal < L7_ACL_MIN_PROTOCOL_NUM) || (protVal > L7_ACL_MAX_PROTOCOL_NUM))
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid Protocol Value [0..255] (%d)", protVal);
        return L7_FAILURE;
      }

      if (protVal != L7_ACL_PROTOCOL_IP)
      {
        matchOther = L7_TRUE;
      }
    }


    /* Check Src IP */
    if (!(msgAcl->aclRuleMask & ACL_IPv6_RULE_MASK_src6Addr))
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) != L7_TRUE)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID");
        return L7_FAILURE;
      }
      matchSrc = L7_TRUE;
    }

    if (matchSrc == L7_FALSE && usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) == L7_TRUE)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID) != L7_TRUE)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID");
        return L7_FAILURE;
      }

      PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Src IP");
      memcpy(&srcIpv6Addr.in6Addr,      msgAcl->src6Addr, sizeof(msgAcl->src6Addr));
      memcpy(&srcIpv6Addr.in6PrefixLen, &msgAcl->src6PrefixLen, sizeof(msgAcl->src6PrefixLen));
    }
  }


  /* Check Src L4 Port */
  if (msgAcl->aclRuleMask & ACL_IPv6_RULE_MASK_srcStartPort)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Src L4 Port");

    if (msgAcl->srcStartPort == msgAcl->srcEndPort)
    {
      srcPortValue = msgAcl->srcStartPort;

      if (srcPortValue < L7_ACL_MIN_L4PORT_NUM || srcPortValue > L7_ACL_MAX_L4PORT_NUM)
      {
        /* Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535> */
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535>");
        return L7_FAILURE;
      }
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Src L4 Port Range");

      srcStartPort = msgAcl->srcStartPort;
      srcEndPort = msgAcl->srcEndPort;

      if (srcStartPort < L7_ACL_MIN_L4PORT_NUM || srcEndPort > L7_ACL_MAX_L4PORT_NUM)
      {
        /* Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535> */
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535>");
        return L7_FAILURE;
      }

      if (srcEndPort < srcStartPort)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535>");
        return L7_FAILURE;
      }
    }

    matchOther = L7_TRUE;
  }


  /* Check Dst IP */
  if (!(msgAcl->aclRuleMask & ACL_IPv6_RULE_MASK_dst6Addr))
  {
    if (matchEvery == L7_FALSE)
    {
      matchDst = L7_TRUE;
    }
  }


  if (matchEvery == L7_FALSE && matchDst == L7_FALSE &&
      usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID) == L7_TRUE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Dst IP");

    memcpy(&dstIpv6Addr.in6Addr, &msgAcl->dst6Addr, sizeof(msgAcl->dst6Addr));
    memcpy(&dstIpv6Addr.in6PrefixLen, &msgAcl->dst6PrefixLen, sizeof(msgAcl->dst6PrefixLen));
  }


  /* Check Dst L4 Port */
  if (msgAcl->aclRuleMask & ACL_IPv6_RULE_MASK_dstStartPort)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Dst L4 Port");

    if (msgAcl->dstStartPort == msgAcl->dstEndPort)
    {
      dstPortValue = msgAcl->dstStartPort;

      if (dstPortValue < L7_ACL_MIN_L4PORT_NUM || dstPortValue > L7_ACL_MAX_L4PORT_NUM)
      {
        /* Invalid start DSTL4 port. <select DSTL4 port between 0 to 65535> */
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid start DSTL4 port. <select DSTL4 port between 0 to 65535>");
        return L7_FAILURE;
      }
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Dst L4 Port Range");

      dstStartPort = msgAcl->dstStartPort;
      dstEndPort = msgAcl->dstEndPort;

      if (dstStartPort < L7_ACL_MIN_L4PORT_NUM || dstEndPort > L7_ACL_MAX_L4PORT_NUM)
      {
        /* Invalid start DSTL4 port. <select DSTL4 port between 0 to 65535> */
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid start DSTL4 port. <select DSTL4 port between 0 to 65535>");
        return L7_FAILURE;
      }

      if (dstEndPort < dstStartPort)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid start DSTL4 port. <select DSTL4 port between 0 to 65535>");
        return L7_FAILURE;
      }
    }

    matchOther = L7_TRUE;
  }


  /* dscp */
  if (msgAcl->aclRuleMask & ACL_IPv6_RULE_MASK_dscpVal)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Matching DSCP");

    dscpVal = msgAcl->dscpVal;

    /* verify if the specified value is in between 0 to 63 */
    if ((dscpVal < L7_ACL_MIN_DSCP) || (dscpVal > L7_ACL_MAX_DSCP))
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: DSCP value must be between 0 to 63");
      return L7_FAILURE;
    }
    else
    {
      matchOther = L7_TRUE;
    }
  }

  /* flow-label */
  else if (msgAcl->aclRuleMask & ACL_IPv6_RULE_MASK_flowLabelVal)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Flow-label");

    flowLabelVal = msgAcl->flowLabelVal;

    /* verify if the specified value is in between 0 to 1048575 */
    if ((flowLabelVal < L7_ACL_MIN_FLOWLBL) || (flowLabelVal > L7_ACL_MAX_FLOWLBL))
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: flow-label value must be between 0 to 1048575");
      return L7_FAILURE;
    }
    else
    {
      matchOther = L7_TRUE;
    }
  }


  /* Apply the Rule */

  /* Verify if the ACL exists - create and add */
  if (usmDbQosAclNameToIndex(unit, accessListName, &aclId) != L7_SUCCESS)
  {
    rc = usmDbQosAclNamedIndexNextFree(unit, L7_ACL_TYPE_IPV6, &aclId);
    if (rc != L7_SUCCESS)
    {
      if (rc == L7_TABLE_IS_FULL)
      {
        (void)usmDbQosAclMaxNumGet(unit, &maxAcls);           /* This function always returns L7_SUCCESS */
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclNamedIndexNextFree return L7_TABLE_IS_FULL (maxAcls=%d)", maxAcls);
        return L7_FAILURE;
      }
      else
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclNamedIndexNextFree return (rc=%d)", rc);
        return L7_FAILURE;
      }
    }

    rc = usmDbQosAclCreate(unit, aclId);
    switch (rc)
    {
      case L7_SUCCESS:
        break;

      case L7_TABLE_IS_FULL:
        usmDbQosAclMaxNumGet(unit, &maxAcls);           /* This function always returns L7_SUCCESS */
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclCreate return L7_TABLE_IS_FULL (maxAcls=%d)", maxAcls);
        return L7_FAILURE;

      case L7_REQUEST_DENIED:
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclCreate return L7_REQUEST_DENIED");
        return L7_FAILURE;

      default:
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclCreate return (rc=%d)", rc);
        return L7_FAILURE;
    }

    /* keep the association between MNG and usmDb ACL IDs */
    ptin_aclIpv6Db[msgAcl->aclId].aclId = aclId;
    PT_LOG_DEBUG(LOG_CTX_MSG, "Created New ACL: ID %d (MNG ID %d)", aclId, msgAcl->aclId);
    isAclAdded = L7_TRUE;

    if (usmDbQosAclNameAdd(unit, aclId, accessListName) != L7_SUCCESS)
    {
      ptin_aclIpv6Clean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclNameAdd return (rc=%d)", rc);
      return L7_FAILURE;
    }
  }


  /* Verify if rule exists */
  if (ptin_aclIpv6Db[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] != 0)
  {
    aclRuleNum = ptin_aclIpv6Db[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId]; 
    PT_LOG_DEBUG(LOG_CTX_MSG, "Configuring existing Rule ID %d (MNG ID %d) on ACL ID %d (MNG ID %d)", aclRuleNum, msgAcl->aclRuleId, aclId, msgAcl->aclId);
  }
  /* Get the new rule number */
  else {
    if (usmDbQosAclRuleGetFirst(unit, aclId, &aclRuleNum) != L7_SUCCESS)
    {
      aclRuleNum = L7_ACL_MIN_RULE_NUM;
    }
    else
    {
      while (usmDbQosAclRuleGetNext(unit, aclId, aclRuleNum, &aclruleNext) == L7_SUCCESS)
      {
        aclRuleNum = aclruleNext;
      }
      aclRuleNum = aclRuleNum + 1;
    }
    if (aclRuleNum > L7_ACL_MAX_RULE_NUM - 2)  /* account for 2 additional implied rules in IPv6 ACLs */
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: aclRuleNum > L7_ACL_MAX_RULE_NUM");
      return L7_FAILURE;
    }

    /* keep the association between MNG and usmDb Rule IDs */
    ptin_aclIpv6Db[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] = aclRuleNum;
    ptin_aclIpv6Db[msgAcl->aclId].aclRuleCount++;
    PT_LOG_DEBUG(LOG_CTX_MSG, "Created New Rule ID %d (MNG ID %d) on ACL ID %d (MNG ID %d)", aclRuleNum, msgAcl->aclRuleId, aclId, msgAcl->aclId);
  }

  /* Add the rule to acl*/

  rc = usmDbQosAclRuleActionAdd(unit, aclId, aclRuleNum, actionType);
  switch (rc)
  {
    case L7_SUCCESS:
      break;

    default:
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleActionAdd return (rc=%d)", rc);
      return L7_FAILURE;
  }


  /* Match Every */
  if ((matchEvery == L7_TRUE) ||
      (matchSrc == L7_TRUE && matchDst == L7_TRUE && matchOther == L7_FALSE))
  {
    rc = usmDbQosAclRuleEveryAdd(unit, aclId, aclRuleNum, L7_TRUE);
    if (rc != L7_SUCCESS)
    {
      ptin_aclIpv6Clean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleEveryAdd return (rc=%d)", rc);
      return L7_FAILURE;
    }
    
    /* Mirror */
    if (mirrorVal > -1)
    {
      rc = usmDbQosAclRuleMirrorIntfAdd(unit, aclId, aclRuleNum, mirrorVal);
      if (rc != L7_SUCCESS)
      {
        ptin_aclIpv6Clean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleMirrorIntfAdd return L7_REQUEST_DENIED");
        return L7_FAILURE;
      }
    }
  }
  else
  {
    if (matchSrc == L7_FALSE)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_IPV6_SRCIP_FEATURE_ID) == L7_TRUE)
      {
        rc = usmDbQosAclRuleSrcIpv6AddrAdd(unit, aclId, aclRuleNum, &srcIpv6Addr);
        if (rc != L7_SUCCESS)
        {
          ptin_aclIpv6Clean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleSrcIpv6AddrAdd return (rc=%d)", rc);
          return L7_FAILURE;
        }
      }
    }

    /* Add the protocol value*/
    if (protVal != 0)
    {
      rc = usmDbQosAclRuleProtocolAdd(unit, aclId, aclRuleNum, protVal);
      if (rc != L7_SUCCESS)
      {
        ptin_aclIpv6Clean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleProtocolAdd return (rc=%d)", rc);
        return L7_FAILURE;
      }
    }
    if (srcPortValue > -1)
    {
      rc = usmDbQosAclRuleSrcL4PortAdd(unit, aclId, aclRuleNum, srcPortValue);
    }
    else if (srcStartPort > -1)
    {
      rc = usmDbQosAclRuleSrcL4PortRangeAdd(unit, aclId, aclRuleNum, srcStartPort, srcEndPort);
    }
    if (rc != L7_SUCCESS)
    {
      ptin_aclIpv6Clean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleSrcL4Port(Range)Add return (rc=%d)", rc);
      return L7_FAILURE;
    }

    if (matchDst == L7_FALSE)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID) == L7_TRUE)
      {
        rc = usmDbQosAclRuleDstIpv6AddrAdd(unit, aclId, aclRuleNum, &dstIpv6Addr);
        if (rc != L7_SUCCESS)
        {
          ptin_aclIpv6Clean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
          PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleDstIpv6AddrAdd return (rc=%d)", rc);
          return L7_FAILURE;
        }
      }
    }

    if (dstPortValue > -1)
    {
      rc = usmDbQosAclRuleDstL4PortAdd(unit, aclId, aclRuleNum, dstPortValue);
    }
    else if (dstStartPort > -1)
    {
      rc = usmDbQosAclRuleDstL4PortRangeAdd(unit, aclId, aclRuleNum, dstStartPort, dstEndPort);
    }
    if (rc != L7_SUCCESS)
    {
      ptin_aclIpv6Clean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleDstL4Port(Range)Add return (rc=%d)", rc);
      return L7_FAILURE;
    }

    if (dscpVal > -1)
    {
      rc = usmDbQosAclRuleIPDscpAdd(unit, aclId, aclRuleNum, dscpVal);
      if (rc != L7_SUCCESS)
      {
        ptin_aclIpv6Clean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleIPDscpAdd return (rc=%d)", rc);
        return L7_FAILURE;
      }
    }

    if (flowLabelVal > -1)
    {
      rc = usmDbQosAclRuleIpv6FlowLabelAdd(unit, aclId, aclRuleNum, flowLabelVal);
      if (rc != L7_SUCCESS)
      {
        ptin_aclIpv6Clean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleIpv6FlowLabelAdd return (rc=%d)", rc);
        return L7_FAILURE;
      }
    }

    /* Mirror */
    if (mirrorVal > -1)
    {
      rc = usmDbQosAclRuleMirrorIntfAdd(unit, aclId, aclRuleNum, mirrorVal);
      if (rc != L7_SUCCESS)
      {
        ptin_aclIpv6Clean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleMirrorIntfAdd return (rc=%d)", rc);
        return L7_FAILURE;
      }
    }
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "Rule Applied!");

  return L7_SUCCESS;
}


/**
 * Apply an IPv6 ACL to an interface or VLAN
 * 
 * @author joaom (11/06/2013)
 * 
 * @param aclApply 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclIpv6Apply(ptin_acl_apply_t *aclApply, ACL_OPERATION_t operation)
{
  L7_BOOL     aclVlanIdGiven = L7_FALSE;
  L7_RC_t     rc;
  L7_uint32   aclId;
  ptin_intf_t ptin_intf;
  L7_uint32   intIfNum, vlanId = 0;
  L7_char8    aclName[L7_ACL_NAME_LEN_MAX+1];
  L7_uint32   direction, feature;
  L7_uint32   unit = 0;
  L7_uint32   sequence = L7_ACL_AUTO_INCR_INTF_SEQ_NUM;
  L7_RC_t     status = L7_SUCCESS;


  /* determine if ACL is being applied to a VLAN ID */
  if (aclApply->number_of_vlans > 0 &&
      aclApply->vlanId[0] != L7_ACL_INVALID_VLAN_ID)
  {
    aclVlanIdGiven = L7_TRUE;

    /* get the vlan id specified by msg */
    vlanId = aclApply->vlanId[0];

    PT_LOG_DEBUG(LOG_CTX_MSG, "Applying ACL to VLAN ID %d", vlanId);

    /* verify if the specified value is in valid range */
    if (usmDbQosAclIsValidVlan(unit, vlanId) == L7_FALSE)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: VLAN ID is invalid");
      return L7_FAILURE;
    }
  }
  else
  {
    if (ptin_intf_port2intIfNum(aclApply->interface, &intIfNum) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Cannot convert port %u to intIfNum", aclApply->interface);
      return L7_FAILURE;
    }
    if (ptin_intf_port2ptintf(aclApply->interface, &ptin_intf) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Cannot convert port %u to ptin_intf", aclApply->interface);
      return L7_FAILURE;
    }
    PT_LOG_DEBUG(LOG_CTX_MSG, "Applying ACL to Interface %d", aclApply->interface);
  }


  /* determine if we are in IPv6 ACL */
  if (aclApply->aclType == ACL_TYPE_IPv6_EXTENDED)
  {
    /* Retrieve ACL name for list being configured */
    strcpy(aclName, aclApply->name);

    if ((usmDbQosAclNameToIndex(unit, aclName, &aclId) != L7_SUCCESS) ||
        (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IPV6, aclId) != L7_SUCCESS))
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: ACL Name NOT found");
      if (operation == ACL_OPERATION_CREATE)
      {
        return L7_FAILURE;
      }
      else
      {
        return L7_SUCCESS;
      }
    }
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid ACL Type");
    return L7_FAILURE;
  }


  rc = usmDbQosAclNumCheckValid(unit, aclId);
  if (rc == L7_FAILURE)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclNumCheckValid return (%d)", rc);
    return L7_FAILURE;
  }


  if (aclApply->direction == ACL_DIRECTION_IN)          /* inbound */
  {
    feature = L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID;
    direction = L7_INBOUND_ACL;
  }
  else if (aclApply->direction == ACL_DIRECTION_OUT)    /* outbound */
  {
    feature = L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID;
    direction = L7_OUTBOUND_ACL;
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Ivalid direction (%d)", aclApply->direction);
    return L7_FAILURE;
  }


  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, feature) == L7_FALSE)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: L7_FLEX_QOS_ACL_COMPONENT_ID");
    return L7_FAILURE;
  }

  if (aclVlanIdGiven == L7_FALSE)
  {
    PT_LOG_TRACE(LOG_CTX_MSG, "operation=%d intIfNum=%d direction=%d aclId=%u sequence=%d", operation, intIfNum, direction, aclId, sequence);

    if (operation == ACL_OPERATION_CREATE)
    {
      #if (PTIN_BOARD_IS_LINECARD)
      if (ptin_aclIpv6Db[aclApply->aclId].aclRuleCap == L7_TRUE)
        return L7_FAILURE;
      #endif

      rc = usmDbQosAclInterfaceDirectionAdd(unit, intIfNum, direction, aclId, sequence);
    }
    else /* ACL_OPERATION_REMOVE */
    {
      rc = usmDbQosAclInterfaceDirectionRemove(unit, intIfNum, direction, aclId);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "No ACL to be Unapplied");
        return L7_SUCCESS;
      }
    }

    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclInterfaceDirectionAdd return (rc=%d)", rc);
      status = L7_FAILURE;
      return L7_FAILURE;
    }
  }
  else
  {

    #ifdef __VLAN_XLATE__
    L7_uint32 oldVlan = vlanId;
    L7_uint16 newVlan;

    /* ...and translate it to the internal VLAN ID */
    rc = ptin_evc_intVlan_get_fromOVlan(&ptin_intf, oldVlan, 0, &newVlan);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE (rc=%d)", rc);
      return L7_FAILURE;
    }

    PT_LOG_DEBUG(LOG_CTX_MSG, "Translating VLAN ID %d to %d (rc=%d)", oldVlan, (L7_uint32) newVlan, rc);
    vlanId = (L7_uint32) newVlan;
    #endif

    /* applying ACL to a VLAN */
    if (operation == ACL_OPERATION_CREATE)
    {
      if ((rc = usmDbQosAclVlanDirectionAdd(unit, vlanId, direction, aclId, sequence)) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclVlanDirectionAdd return (rc=%d)", rc);
        return L7_FAILURE;
      }
    }
    else  /* ACL_OPERATION_REMOVE */
    {
      if ((rc = usmDbQosAclVlanDirectionRemove(unit, vlanId, direction, aclId)) != L7_SUCCESS)
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "No ACL to be Unapplied");
        return L7_SUCCESS;
      }
    }
  }

  return L7_SUCCESS;
}

/********************************************************************************** 
 *                                   MAC ACL                                      *
 **********************************************************************************/

/**
 * Create a MAC ACL Rule
 * 
 * @author joaom (10/30/2013)
 * 
 * @param msgAcl 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclMacRuleConfig(msg_mac_acl_t *msgAcl, ACL_OPERATION_t operation)
{
  L7_RC_t rc = L7_FAILURE;
  
  L7_uint32 unit = 0, aclId, aclRuleNum, aclruleNext;
  L7_uint32 actionType;
  L7_uchar8 srcMacAddr[L7_MAC_ADDR_LEN];
  L7_uchar8 srcMacMask[L7_MAC_ADDR_LEN];
  L7_uchar8 dstMacAddr[L7_MAC_ADDR_LEN];
  L7_uchar8 dstMacMask[L7_MAC_ADDR_LEN];
  
  L7_int32 eTypeKeyVal = -1;
  L7_int32 eTypeCustVal = -1;
  L7_int32 vlanVal = -1;
  L7_int32 startVlanVal = -1;
  L7_int32 endVlanVal = -1;
  
  L7_int32 cosVal = -1;
  L7_int32 mirrorVal = -1;
  
  L7_char8 macAccessListName[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL matchSrc = L7_FALSE;
  L7_BOOL matchDst = L7_FALSE;
  L7_BOOL matchOther = L7_FALSE;
  L7_BOOL isAclAdded = L7_FALSE;
  

  /* *********************** ACL_OPERATION_REMOVE *********************** */
  if (operation == ACL_OPERATION_REMOVE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Removing MNG Rule ID %d on MNG ACL ID %d", msgAcl->aclRuleId, msgAcl->aclId);

    if ((ptin_aclMacDb[msgAcl->aclId].aclId != 0) && (ptin_aclMacDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] != 0))
    {
      ptin_aclMacClean(L7_FALSE, msgAcl->aclId, msgAcl->aclRuleId);
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Entry is empty");
    }

    return L7_SUCCESS;
  }


  /* *********************** ACL_OPERATION_CREATE *********************** */
  PT_LOG_DEBUG(LOG_CTX_MSG, "Applying Rule...");

  if (msgAcl->action == ACL_ACTION_PERMIT)
  {
    actionType = L7_ACL_PERMIT;
  }
  else if (msgAcl->action == ACL_ACTION_DENY)
  {
    actionType = L7_ACL_DENY;
  }
  else if (msgAcl->action == ACL_ACTION_CAPTURE)
  {
    actionType = L7_ACL_PERMIT;
    mirrorVal = ptin_mirror_intfnum;

    ptin_aclMacDb[msgAcl->aclId].aclRuleCap = L7_TRUE;
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid Action (%d)", msgAcl->action);
    return L7_FAILURE;
  }


  /* Any Src MAC */
  if (!(msgAcl->aclRuleMask & ACL_MAC_RULE_MASK_srcMacAddr))
  {
    matchSrc = L7_TRUE;
  }

  if (matchSrc == L7_FALSE && usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_SRCMAC_FEATURE_ID) == L7_TRUE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Src MAC");
    memcpy(srcMacAddr, msgAcl->srcMacAddr, sizeof(srcMacAddr));
    memcpy(srcMacMask, msgAcl->srcMacMask, sizeof(srcMacMask));
  }


  /* Any Dst MAC */
  if (!(msgAcl->aclRuleMask & ACL_MAC_RULE_MASK_dstMacAddr))
  {
    matchDst = L7_TRUE;
  }

  if (matchDst == L7_FALSE && usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_DSTMAC_FEATURE_ID) == L7_TRUE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Dst MAC");
    memcpy(dstMacAddr, msgAcl->dstMacAddr, sizeof(dstMacAddr));
    memcpy(dstMacMask, msgAcl->dstMacMask, sizeof(dstMacMask));
  }


  /* Ethertype */
  if (msgAcl->aclRuleMask & ACL_MAC_RULE_MASK_eType)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Ethertype");
    eTypeKeyVal = L7_QOS_ETYPE_KEYID_CUSTOM;
    eTypeCustVal = msgAcl->eType;
    matchOther = L7_TRUE;
  }


  /* VLAN */
  if (msgAcl->aclRuleMask & ACL_MAC_RULE_MASK_startVlan)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Matching VLAN");
    if (msgAcl->startVlan == msgAcl->endVlan)
    {
      vlanVal = msgAcl->startVlan;

      /* verify if the specified value is in between 1 to 4094 */
      if ((vlanVal < L7_ACL_MIN_VLAN_ID) || (vlanVal > L7_ACL_MAX_VLAN_ID))
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid VLAN ID [1..4094] (%d)", vlanVal);
        return L7_FAILURE;
      }
    }
    else
    {
      PT_LOG_DEBUG(LOG_CTX_MSG, "Matching VLAN Range");

      startVlanVal = msgAcl->startVlan;
      endVlanVal = msgAcl->endVlan;

      /* verify if the specified value is in between 1 to 4094 */
      if ((startVlanVal < L7_ACL_MIN_VLAN_ID) || (endVlanVal > L7_ACL_MAX_VLAN_ID))
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid VLAN ID [1..4094] (%d:%d)", startVlanVal, endVlanVal);
        return L7_FAILURE;
      }

      if (endVlanVal < startVlanVal)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid VLAN ID [1..4094] (%d:%d)", startVlanVal, endVlanVal);
        return L7_FAILURE;
      }
    }
    matchOther = L7_TRUE;
  }


  /* COS */
  if (msgAcl->aclRuleMask & ACL_MAC_RULE_MASK_cosVal)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Matching COS");

    cosVal = msgAcl->cosVal;

    if ((cosVal < L7_ACL_MIN_COS) ||
        (cosVal > L7_ACL_MAX_COS))
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Invalid COS Value [0..7] (%d)", cosVal);
      return L7_FAILURE;
    }
    matchOther = L7_TRUE;
  }


  /* Apply the rule */

  strcpy(macAccessListName, msgAcl->name);

  /* Check to see if MAC ACL exists     */
  /* If does not exist - create and add */
  if (usmDbQosAclMacNameToIndex(unit, macAccessListName, &aclId) != L7_SUCCESS)
  {
    rc = usmDbQosAclMacIndexNext(unit, &aclId);
    if (rc != L7_SUCCESS)
    {
      if (rc == L7_TABLE_IS_FULL)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacIndexNext return L7_TABLE_IS_FULL");
        return L7_FAILURE;
      }
      else
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacIndexNext return (rc=%d)", rc);
        return L7_FAILURE;
      }
    }

    rc = usmDbQosAclMacCreate(unit, aclId);
    switch (rc)
    {
      case L7_SUCCESS:
        break;

      case L7_TABLE_IS_FULL:
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacCreate return L7_TABLE_IS_FULL");
        return L7_FAILURE;

      case L7_REQUEST_DENIED:
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacCreate return L7_REQUEST_DENIED");
        return L7_FAILURE;

      default:
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacCreate return (rc=%d)", rc);
        return L7_FAILURE;
    }

    /* keep the association between MNG and usmDb ACL IDs */
    ptin_aclMacDb[msgAcl->aclId].aclId = aclId;
    PT_LOG_DEBUG(LOG_CTX_MSG, "Created New ACL: ID %d (MNG ID %d)", aclId, msgAcl->aclId);
    isAclAdded = L7_TRUE;

    if (usmDbQosAclMacNameAdd(unit, aclId, macAccessListName) != L7_SUCCESS)
    {
      ptin_aclMacClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacNameAdd return (rc=%d)", rc);
      return L7_FAILURE;
    }
  }


  /* Verify if rule exists */
  if ( ptin_aclMacDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] != 0)
  {
    aclRuleNum = ptin_aclMacDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId]; 
    PT_LOG_DEBUG(LOG_CTX_MSG, "Configuring existing Rule ID %d (MNG ID %d) on ACL ID %d (MNG ID %d)", aclRuleNum, msgAcl->aclRuleId, aclId, msgAcl->aclId);
  }
  /* Get the new rule number */
  else {
    if (usmDbQosAclMacRuleGetFirst(unit, aclId, &aclRuleNum) != L7_SUCCESS)
    {
      aclRuleNum = L7_ACL_MIN_RULE_NUM;
    }
    else
    {
      while (usmDbQosAclMacRuleGetNext(unit, aclId, aclRuleNum, &aclruleNext) == L7_SUCCESS)
      {
        aclRuleNum = aclruleNext;
      }
      aclRuleNum++;
    }
    if (aclRuleNum > L7_ACL_MAX_RULE_NUM)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: aclRuleNum > L7_ACL_MAX_RULE_NUM");
      return L7_FAILURE;
    }

    /* keep the association between MNG and usmDb Rule IDs */
    ptin_aclMacDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] = aclRuleNum;
    ptin_aclMacDb[msgAcl->aclId].aclRuleCount++;
    PT_LOG_DEBUG(LOG_CTX_MSG, "Created New Rule ID %d (MNG ID %d) on ACL ID %d (MNG ID %d)", aclRuleNum, msgAcl->aclRuleId, aclId, msgAcl->aclId);
  }
  

  rc = usmDbQosAclMacRuleActionAdd(unit, aclId, aclRuleNum, actionType);
  switch (rc)
  {
  case L7_SUCCESS:
    break;

  default:
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacRuleActionAdd return (rc=%d)", rc);
    return L7_FAILURE;
  }


  /* Match Every */
  if (matchOther == L7_FALSE && matchSrc == L7_TRUE && matchDst == L7_TRUE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Matching Every!");
    rc = usmDbQosAclMacRuleEveryAdd(unit, aclId, aclRuleNum, matchSrc);
    if (rc == L7_FAILURE)
    {
      ptin_aclMacClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacRuleEveryAdd return L7_FAILURE");
      return L7_FAILURE;
    }

    if (rc == L7_ERROR)
    {
      ptin_aclMacClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacRuleEveryAdd return L7_ERROR");
      return L7_FAILURE;
    }

    if (rc == L7_REQUEST_DENIED)
    {
      ptin_aclMacClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacRuleEveryAdd return L7_REQUEST_DENIED");
      return L7_FAILURE;
    }

    /* Mirror */
    if (mirrorVal > -1)
    {
      rc = usmDbQosAclMacRuleMirrorIntfAdd(unit, aclId, aclRuleNum, mirrorVal);
      if (rc != L7_SUCCESS)
      {
        ptin_aclMacClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleMirrorIntfAdd return (rc=%d)", rc);
        return L7_FAILURE;
      }
    }
  }
  else
  {
    if (matchSrc == L7_FALSE)
    {
      rc = usmDbQosAclMacRuleSrcMacAdd(unit, aclId, aclRuleNum, srcMacAddr, srcMacMask);
      if (rc == L7_FAILURE)
      {
        ptin_aclMacClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacRuleSrcMacAdd return L7_FAILURE");
        return L7_FAILURE;
      }
      else if (rc == L7_REQUEST_DENIED)
      {
        ptin_aclMacClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacRuleSrcMacAdd return L7_REQUEST_DENIED");
        return L7_FAILURE;
      }
    }

    if (matchDst == L7_FALSE)
    {
      rc = usmDbQosAclMacRuleDstMacAdd(unit, aclId, aclRuleNum, dstMacAddr, dstMacMask);
      if (rc == L7_FAILURE)
      {
        ptin_aclMacClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacRuleDstMacAdd return L7_FAILURE");
        return L7_FAILURE;
      }
      else if (rc == L7_REQUEST_DENIED)
      {
        ptin_aclMacClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacRuleDstMacAdd return L7_REQUEST_DENIED");
        return L7_FAILURE;
      }
    }

    /* EtherType */
    if (eTypeKeyVal > -1 && eTypeKeyVal != 0)
    {
      rc = usmDbQosAclMacRuleEtypeKeyAdd(unit, aclId, aclRuleNum, eTypeKeyVal, eTypeCustVal);
      if (rc != L7_SUCCESS)
      {
        ptin_aclMacClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacRuleEtypeKeyAdd return (rc=%d)", rc);
        return L7_FAILURE;
      }
    }

    /* VLAN Value */
    if (vlanVal > -1)
    {
      rc = usmDbQosAclMacRuleVlanIdAdd(unit, aclId, aclRuleNum, vlanVal);
    }

    /* VLAN Range*/
    else if (startVlanVal > -1)
    {
      rc = usmDbQosAclMacRuleVlanIdRangeAdd(unit, aclId, aclRuleNum, startVlanVal, endVlanVal);
    }
    if (rc != L7_SUCCESS)
    {
      ptin_aclMacClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacRuleVlanIdRangeAdd return (rc=%d)", rc);
      return L7_FAILURE;
    }

    /* COS */
    if (cosVal > -1)
    {
      rc = usmDbQosAclMacRuleCosAdd(unit, aclId, aclRuleNum, cosVal);
      if (rc != L7_SUCCESS)
      {
        ptin_aclMacClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacRuleCosAdd return (rc=%d)", rc);
        return L7_FAILURE;
      }
    }

    /* Mirror */
    if (mirrorVal > -1)
    {
      rc = usmDbQosAclMacRuleMirrorIntfAdd(unit, aclId, aclRuleNum, mirrorVal);
      if (rc != L7_SUCCESS)
      {
        ptin_aclMacClean(isAclAdded, msgAcl->aclId, msgAcl->aclRuleId);
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclRuleMirrorIntfAdd return (rc=%d)", rc);
        return L7_FAILURE;
      }
    }
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "Rule Applied!");

  return L7_SUCCESS;
}


/**
 * Apply a MAC ACL to an interface or VLAN
 * 
 * @author joaom (10/30/2013)
 * 
 * @param aclApply 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclMacApply(ptin_acl_apply_t *aclApply, ACL_OPERATION_t operation)
{
  L7_BOOL     aclVlanIdGiven = L7_FALSE;
  L7_RC_t     rc = L7_FAILURE;
  ptin_intf_t ptin_intf;
  L7_uint32   aclId = 0, direction, feature;
  L7_uint32   intIfNum, vlanId = 0;
  L7_char8    macAclName[L7_ACL_NAME_LEN_MAX + 1];
  L7_uint32   sequence = L7_ACL_AUTO_INCR_INTF_SEQ_NUM;
  L7_uint32   unit = 0;
  L7_BOOL     noMacAclName = L7_FALSE;
  L7_RC_t     status = L7_SUCCESS;

  /* determine if ACL is being applied to a VLAN ID */
  if (aclApply->number_of_vlans > 0 &&
      aclApply->vlanId[0] != L7_ACL_INVALID_VLAN_ID)
  {
    aclVlanIdGiven = L7_TRUE;

    /* get the vlan id specified by msg */
    vlanId = aclApply->vlanId[0];

    PT_LOG_DEBUG(LOG_CTX_MSG, "Applying ACL to VLAN ID %d", vlanId);

    /* verify if the specified value is in valid range */
    if (usmDbQosAclIsValidVlan(unit, vlanId) == L7_FALSE)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: VLAN ID is invalid");
      return L7_FAILURE;
    }
  }
  else
  {
    if (ptin_intf_port2intIfNum(aclApply->interface, &intIfNum) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Cannot convert port %u to intIfNum", aclApply->interface);
      return L7_FAILURE;
    }
    if (ptin_intf_port2ptintf(aclApply->interface, &ptin_intf) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Cannot convert port %u to ptin_intf", aclApply->interface);
      return L7_FAILURE;
    }
    PT_LOG_DEBUG(LOG_CTX_MSG, "Applying ACL to Interface %d", aclApply->interface);
  }


  strcpy(macAclName, aclApply->name);

  if (usmDbQosAclMacNameToIndex(0, macAclName, &aclId) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: ACL Name NOT found");
    if (operation == ACL_OPERATION_CREATE)
    {
      return L7_FAILURE;
    }
    else
    {
      return L7_SUCCESS;
    }
  }

  if (usmDbQosAclMacIndexCheckValid(0, aclId) == L7_ERROR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: ACD ID Invalid");
    return L7_FAILURE;
  }


  if (aclApply->direction == ACL_DIRECTION_IN)          /* inbound */
  {
    feature = L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID;
    direction = L7_INBOUND_ACL;
  }
  else if (aclApply->direction == ACL_DIRECTION_OUT)    /* outbound */
  {
    feature = L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID;
    direction = L7_OUTBOUND_ACL;
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: Ivalid direction (%d)", aclApply->direction);
    return L7_FAILURE;
  }


  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, feature) == L7_FALSE)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: L7_FLEX_QOS_ACL_COMPONENT_ID");
    return L7_FAILURE;
  }


  if (aclVlanIdGiven == L7_FALSE)
  {
    PT_LOG_TRACE(LOG_CTX_MSG, "operation=%d intIfNum=%d direction=%d aclId=%u sequence=%d", operation, intIfNum, direction, aclId, sequence);

    if (operation == ACL_OPERATION_CREATE)
    {
      #ifdef __VLAN_XLATE__
      L7_uint32 i;
      L7_uint32 oldVlan;
      L7_uint16 newVlan;

      /* On L2 ACLs, if any rule has a VLAN ID specified, this VLAN ID must be translated to the internal VLAN ID */
      for (i=0; i<L7_MAX_NUM_RULES_PER_ACL; i++)
      {
        /* Verify if rule is in used */
        if ( (ptin_aclMacDb[aclApply->aclId].aclRuleNum[i] != 0) && !(ptin_aclMacDb[aclApply->aclId].aclRuleXlated[i]))
        {
          /* Get the VLAN ID originally associated to the rule...  */
          rc = aclMacRuleVlanIdGet(aclId, i, &oldVlan);
          if (rc != L7_SUCCESS)
          {
            PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE (rc=%d)", rc);
          }
          else if (oldVlan != 0)
          {
            /* ...and translate it to the internal VLAN ID */
            rc = ptin_evc_intVlan_get_fromOVlan(&ptin_intf, oldVlan, 0, &newVlan);
            if (rc == L7_SUCCESS)
            {
              PT_LOG_DEBUG(LOG_CTX_MSG, "Rule %d: Translating VLAN ID %d to %d (rc=%d)", i, oldVlan, (L7_uint32) newVlan, rc);

              aclMacRuleVlanIdAdd(aclId, i, (L7_uint32) newVlan);

              ptin_aclMacDb[aclApply->aclId].aclRuleXlated[i] = 1;
            }
          }
        }
      }
      #endif

      #if (PTIN_BOARD_IS_LINECARD)
      if (ptin_aclMacDb[aclApply->aclId].aclRuleCap == L7_TRUE)
        return L7_FAILURE;
      #endif

      rc = usmDbQosAclMacInterfaceDirectionAdd(unit, intIfNum, direction, aclId, sequence);
    }
    else /* ACL_OPERATION_REMOVE */
    {
      rc = usmDbQosAclMacInterfaceDirectionRemove(unit, intIfNum, direction, aclId);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "No ACL to be Unapplied");
        return L7_SUCCESS;
      }
    }

    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacInterfaceDirectionAdd return (rc=%d)", rc);
      status = L7_FAILURE;
      /* Able to remove or add an interface, so set flag to FALSE */
      noMacAclName = L7_TRUE;
    }
  }
  else
  {
    /* applying ACL to a VLAN */
    #ifdef __VLAN_XLATE__
    L7_uint32 oldVlan = vlanId;
    L7_uint16 newVlan;

    /* ...and translate it to the internal VLAN ID */
    rc = ptin_evc_intVlan_get_fromOVlan(&ptin_intf, oldVlan, 0, &newVlan);
    if (rc != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE (rc=%d)", rc);
      return L7_FAILURE;
    }

    PT_LOG_DEBUG(LOG_CTX_MSG, "Translating VLAN ID %d to %d (rc=%d)", oldVlan, (L7_uint32) newVlan, rc);
    vlanId = (L7_uint32) newVlan;
    #endif

    if (operation == ACL_OPERATION_CREATE)
    {
      if ((rc = usmDbQosAclMacVlanDirectionAdd(unit, vlanId, direction, aclId, sequence)) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL FAILURE: usmDbQosAclMacVlanDirectionAdd return (rc=%d)", rc);
        return L7_FAILURE;
      }
    }
    else  /* ACL_OPERATION_REMOVE */
    {
      // Just unapply the ACL. The same ACL can be in used by several entities (VLANs and Interfaces).
      if ((rc = usmDbQosAclMacVlanDirectionRemove(unit, vlanId, direction, aclId)) != L7_SUCCESS)
      {
        PT_LOG_DEBUG(LOG_CTX_MSG, "No ACL to be Unapplied");
        return L7_SUCCESS;
      }
    }
  }

  return L7_SUCCESS;
}

/********************************************************************************** 
 *                                   ARP ACL                                      *
 **********************************************************************************/

/**
 * Create an ARP ACL Rule
 * 
 * @author mruas (02/16/2015)
 * 
 * @param msgAcl 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclArpRuleConfig(msg_arp_acl_t *msgAcl, ACL_OPERATION_t operation)
{
  L7_enetMacAddr_t srcMacAddr;
  L7_uint32 ipAddr;
  
  /* Validate arguments */
  if (msgAcl == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Null pointer!");
    return L7_FAILURE;
  }

  /* Validate ACL type */
  if (msgAcl->aclType != ACL_TYPE_ARP)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid ACL_TYPE (%u)!", msgAcl->aclType);
    return L7_FAILURE;
  }

  /* Validate IP family */
  if (msgAcl->srcIpAddr.family != PTIN_AF_INET)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Only IPv4 is supported!");
    return L7_FAILURE;
  }

  /* Validate ACL name */
  if (msgAcl->name[0] == '\0')
  {
    PT_LOG_ERR(LOG_CTX_MSG, "ACL name is empty!");
    return L7_FAILURE;
  }

  /* MAC address */
  memcpy(srcMacAddr.addr, msgAcl->srcMacAddr, sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);

  /* *********************** ACL_OPERATION_REMOVE *********************** */
  if (operation == ACL_OPERATION_REMOVE)
  {
    /* Validate ACL name */
    if (arpAclGet(msgAcl->name) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "ACL group \"%s\" does not exist", msgAcl->name);
      return L7_NOT_EXIST;
    }
    /* Check if entry does not exist */
    if (arpAclRuleGet(msgAcl->name, msgAcl->srcIpAddr.addr.ipv4, &srcMacAddr) != L7_SUCCESS)
    {
      PT_LOG_WARN(LOG_CTX_MSG, "ACL group \"%s\" does not exist", msgAcl->name);
      return L7_NOT_EXIST;
    }
    /* Remove entry */
    if (arpAclRuleDelete(msgAcl->name, msgAcl->srcIpAddr.addr.ipv4, &srcMacAddr) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error removing ACL rule %02x:%02x:%02x:%02x:%02x:%02x / %03u.%03u.%03u.%03u from \"%s\" group",
              msgAcl->srcMacAddr[0],msgAcl->srcMacAddr[1],msgAcl->srcMacAddr[2],msgAcl->srcMacAddr[3],msgAcl->srcMacAddr[4],msgAcl->srcMacAddr[5],
              (msgAcl->srcIpAddr.addr.ipv4>>24) & 0xff, (msgAcl->srcIpAddr.addr.ipv4>>16) & 0xff, (msgAcl->srcIpAddr.addr.ipv4>>8) & 0xff, msgAcl->srcIpAddr.addr.ipv4 & 0xff,
              msgAcl->name);
      return L7_FAILURE;
    }

    /* Check if ACL group is empty. If so, delete it */
    ipAddr = 0;
    memset(srcMacAddr.addr, 0x00, sizeof(srcMacAddr.addr));
    if (arpAclRuleInAclNextGet(msgAcl->name, ipAddr, &srcMacAddr, &ipAddr, &srcMacAddr) != L7_SUCCESS)
    {
      if (arpAclDelete(msgAcl->name) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error deleting ACL group \"%s\"", msgAcl->name);
        return L7_FAILURE;
      }
    }
    return L7_SUCCESS;
  }
  /* *********************** ACL_OPERATION_CREATE *********************** */
  else if (operation == ACL_OPERATION_CREATE)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Applying Rule...");

    if (msgAcl->action != ACL_ACTION_PERMIT)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Only PERMIT action is allowed");
      return L7_FAILURE;
    }

    /* If ACL name does not exist, create it */
    if (arpAclGet(msgAcl->name) != L7_SUCCESS)
    {
      if (arpAclCreate(msgAcl->name) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error creating ACL group \"%s\"", msgAcl->name);
        return L7_FAILURE;
      }
    }

    /* Add ACL entry */
    if (arpAclRuleAdd(msgAcl->name, msgAcl->srcIpAddr.addr.ipv4, &srcMacAddr) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Error adding ACL rule %02x:%02x:%02x:%02x:%02x:%02x / %03u.%03u.%03u.%03u to \"%s\" group",
              msgAcl->srcMacAddr[0],msgAcl->srcMacAddr[1],msgAcl->srcMacAddr[2],msgAcl->srcMacAddr[3],msgAcl->srcMacAddr[4],msgAcl->srcMacAddr[5],
              (msgAcl->srcIpAddr.addr.ipv4>>24) & 0xff, (msgAcl->srcIpAddr.addr.ipv4>>16) & 0xff, (msgAcl->srcIpAddr.addr.ipv4>>8) & 0xff, msgAcl->srcIpAddr.addr.ipv4 & 0xff,
              msgAcl->name);
      return L7_FAILURE;
    }

    PT_LOG_DEBUG(LOG_CTX_MSG, "Rule Applied!");
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid operation %u", operation);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/**
 * Apply an ARP ACL to a VLAN
 * 
 * @author mruas (02/16/2015)
 * 
 * @param aclApply 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclArpApply(ptin_acl_apply_t *aclApply, ACL_OPERATION_t operation)
{
  L7_uint16 i, vlanId;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if (aclApply == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Null pointer!");
    return L7_FAILURE;
  }

  /* Validate ACL type */
  if (aclApply->aclType != ACL_TYPE_ARP)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Invalid ACL_TYPE (%u)!", aclApply->aclType);
    return L7_FAILURE;
  }

  /* Validate direction */
  if (aclApply->direction != ACL_DIRECTION_IN)
  {
    PT_LOG_ERR(LOG_CTX_MSG, "Only ACL_DIRECTION_IN is allowed (%u)!", aclApply->direction);
    return L7_FAILURE;
  }

  /* Run all VLANs */
  for (i = 0; i < aclApply->number_of_vlans; i++)
  {
    vlanId = aclApply->vlanId[i];

    /* Validate VLAN */
    if (vlanId < PTIN_VLAN_MIN || vlanId > PTIN_VLAN_MAX)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Invalid VLAN %u at index %u", vlanId, i);
      rc = L7_FAILURE;
      continue;
    }

    if (dot1qVlanCheckValid(vlanId) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MSG, "VLAN %u is not valid!", vlanId);
      rc = L7_NOT_EXIST;
      continue;
    }
    
    /* Validate ACL name */
    if (aclApply->name[0] == '\0')
    {
      PT_LOG_TRACE(LOG_CTX_MSG, "ACL name is empty... changing operation to REMOVE");
      operation = ACL_OPERATION_REMOVE;
    }
    
    /* Assign ACL name to VLAN */
    if (operation == ACL_OPERATION_CREATE)
    {
      if (arpAclGet(aclApply->name) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "ACL group \"%s\" does not exist", aclApply->name);
        rc = L7_NOT_EXIST;
        continue;
      }

      if (daiVlanArpAclSet(vlanId, aclApply->name) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error assigning ACL name to VLAN %u", vlanId);
        rc = L7_FAILURE;
        continue;
      }
    }
    /* Remove VLAN related to ACL name */
    else if (operation == ACL_OPERATION_REMOVE)
    {
      if (daiVlanArpAclClear(vlanId) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_MSG, "Error removing ACL name from VLAN %u", vlanId);
        rc = L7_FAILURE;
        continue;
      }
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_MSG, "Invalid operation %u", operation);
      return L7_FAILURE;
    }
  }

  return rc;
}


/********************************************************************************** 
 *                                  DEBUG MNG ACL DB                              *
 **********************************************************************************/

/**
 * Dump IP ACL MNG DB
 * 
 * @author joaom (11/04/2013)
 * 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclIpDbDump(L7_uint32 mngAclId)
{
  int i;

  if (mngAclId >= (2*L7_MAX_ACL_LISTS))
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Invalid ACL ID %d", mngAclId);
    return L7_FAILURE;
  }

  printf("------------------------------------------- \n\r");
  printf(" IP ACL Info: MNG ID %d -> ACL ID %d \n\r",        mngAclId, ptin_aclIpDb[mngAclId].aclId);
  printf("------------------------------------------- \n\r");
  printf(" Number of Rules: %d \n\r",                      ptin_aclIpDb[mngAclId].aclRuleCount);
  printf("------------------------------------------- \n\r");

  for (i=1; i<=L7_MAX_NUM_RULES_PER_ACL; i++)
  {
    printf(" Rule Info: MNG ID %2d -> Rule ID %2d \n\r",   i, ptin_aclIpDb[mngAclId].aclRuleNum[i]);
  }

  PT_LOG_DEBUG(LOG_CTX_MSG, "------------------------------------------- \n\r");

  return L7_SUCCESS; 
}


/**
 * Dump IPv6 ACL MNG DB
 * 
 * @author joaom (11/06/2013)
 * 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclIpv6DbDump(L7_uint32 mngAclId)
{
  int i;

  if (mngAclId >= L7_MAX_ACL_LISTS)
  {
    printf("Invalid ACL ID %d", mngAclId);
    return L7_FAILURE;
  }

  printf("-------------------------------------------\n\r");
  printf("IPv6 ACL Info: MNG ID %d -> ACL ID %d\n\r",      mngAclId, ptin_aclIpv6Db[mngAclId].aclId);
  printf("-------------------------------------------\n\r");
  printf("Number of Rules: %d",                      ptin_aclIpv6Db[mngAclId].aclRuleCount);
  printf("-------------------------------------------\n\r");

  for (i=1; i<=L7_MAX_NUM_RULES_PER_ACL; i++)
  {
    printf("Rule Info: MNG ID %2d -> Rule ID %2d \n\r",   i, ptin_aclIpv6Db[mngAclId].aclRuleNum[i]);
  }

  printf("------------------------------------------- \n\r");

  return L7_SUCCESS; 
}


/**
 * Dump MAC ACL MNG DB
 * 
 * @author joaom (11/04/2013)
 * 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclMacDbDump(L7_uint32 mngAclId)
{
  int i;

  if (mngAclId >= L7_MAX_ACL_LISTS)
  {
    PT_LOG_DEBUG(LOG_CTX_MSG, "Invalid ACL ID %d", mngAclId);
    return L7_FAILURE;
  }

  printf("------------------------------------------- \n\r");
  printf(" MAC ACL Info: MNG ID %d -> ACL ID %d \n\r",       mngAclId, ptin_aclMacDb[mngAclId].aclId);
  printf("------------------------------------------- \n\r");
  printf(" Number of Rules: %d \n\r",                      ptin_aclMacDb[mngAclId].aclRuleCount);
  printf("------------------------------------------- \n\r");

  for (i=1; i<=L7_MAX_NUM_RULES_PER_ACL; i++)
  {
    printf("  Rule Info: MNG ID %2d -> Rule ID %2d \n\r",   i, ptin_aclMacDb[mngAclId].aclRuleNum[i]);
  }

  printf("------------------------------------------- \n\r");

  return L7_SUCCESS;
}


/**
 * Dump ACL MNG DB
 * 
 * @author joaom (12/02/2013)
 * 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclDbDump(L7_uint8 aclType, L7_uint32 mngAclId)
{
  int i;

  if (aclType == ACL_TYPE_MAC)
  {
    if (mngAclId == -1)
    {
      for (i=0; i<L7_MAX_ACL_LISTS; i++)
      {
        if (ptin_aclMacDb[i].aclId==0)  continue;

        ptin_aclMacDbDump(i); 
      }
    }
    else
    {
      ptin_aclMacDbDump(mngAclId);
    }
  }
  else if ( (aclType == ACL_TYPE_IP_STANDARD) || (aclType == ACL_TYPE_IP_EXTENDED) || (aclType == ACL_TYPE_IP_NAMED) )
  {
    if (mngAclId == -1)
    {
      for (i=0; i<(2*L7_MAX_ACL_LISTS); i++)
      {
        if (ptin_aclIpDb[i].aclId==0)  continue;

        ptin_aclIpDbDump(i);
      }
    }
    else
    {
      ptin_aclIpDbDump(mngAclId);
    }
  }
  else if (aclType == ACL_TYPE_IPv6_EXTENDED)
  {
    if (mngAclId == -1)
    {
      for (i=0; i<L7_MAX_ACL_LISTS; i++)
      {
        if (ptin_aclIpv6Db[i].aclId==0)  continue;

        ptin_aclIpv6DbDump(i);
      }
    }
    else
    {
      ptin_aclIpv6DbDump(mngAclId);
    }
  } 

  return L7_SUCCESS; 
}

/**
 * Dump all ACL MNG DB
 * 
 * @author ruif (13/01/2016)
 * 
 * 
 * @return L7_RC_t 
 */

void ptin_aclDbDump_all()
{
  int i =0;

  for (i=0; i<L7_MAX_ACL_LISTS; i++)
  {
    if (ptin_aclMacDb[i].aclId==0)  continue;

    ptin_aclMacDbDump(i); 
  }

  for (i=0; i<(2*L7_MAX_ACL_LISTS); i++)
  {
    if (ptin_aclIpDb[i].aclId==0)  continue;

    ptin_aclIpDbDump(i);
  }

  for (i=0; i<L7_MAX_ACL_LISTS; i++)
  {
    if (ptin_aclIpv6Db[i].aclId==0)  continue;

    ptin_aclIpv6DbDump(i);
  }
}
