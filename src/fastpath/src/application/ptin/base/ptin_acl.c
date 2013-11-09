/**
 * ptin_acl.c
 *  
 * Created on: 2013/19/30 
 * Author: Joao Mateiro
 * 
 */

#include "ptin_acl.h"
#include "ptin_evc.h"

#define ACL_STANDARD  0
#define ACL_EXTENDED  1
#define ACL_NOTDEFIND 2

//#define __VLAN_XLATE__

/********************************************************************************** 
 *                            PTIN Local MAC/IP/IPv6 DB                           *
 **********************************************************************************/

struct {
  L7_uint32 aclId;
  L7_uint32 aclRuleNum[L7_MAX_NUM_RULES_PER_ACL];
  #ifdef __VLAN_XLATE__
  L7_uint32 aclRuleXlated[L7_MAX_NUM_RULES_PER_ACL];
  #endif
} ptin_aclMacDb[L7_MAX_ACL_LISTS];

struct {
  L7_uint32 aclId;
  L7_uint32 aclRuleNum[L7_MAX_NUM_RULES_PER_ACL];
} ptin_aclIpDb[2*L7_MAX_ACL_LISTS]; /* Standard [1..99] and Extended [100..199]*/

struct {
  L7_uint32 aclId;
  L7_uint32 aclRuleNum[L7_MAX_NUM_RULES_PER_ACL];
} ptin_aclIpv6Db[L7_MAX_ACL_LISTS];



/********************************************************************************** 
 *                             MAC/IP/IPv6 ACL Clean                              *
 **********************************************************************************/

/**
 * Clean an IP ACL
 * 
 * @author joaom (11/4/2013)
 * 
 * @param isAclAdded 
 * @param aclId 
 * @param ruleNum 
 */
void  ptin_aclIpClean(L7_BOOL isAclAdded, L7_uint32 aclId, L7_uint32 ruleNum)
{
  L7_uint32 unit = 0;
  L7_RC_t   rc;
  int       i;

  if( isAclAdded == L7_TRUE)
  {
    rc = usmDbQosAclDelete(unit, aclId);
    for (i=0; i<L7_MAX_ACL_LISTS; i++)
    {
      if (ptin_aclIpDb[i].aclId == aclId)
      {
        ptin_aclIpDb[i].aclId = 0;
        memset(ptin_aclIpDb[i].aclRuleNum, 0, L7_MAX_NUM_RULES_PER_ACL);
      }
    }
  }
  else
  {
    rc = usmDbQosAclRuleRemove(unit, aclId, ruleNum);
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

  for (i=0; i<L7_MAX_ACL_LISTS; i++)
  {
    if (ptin_aclIpDb[i].aclId != 0)
    {
      ptin_aclIpClean(L7_TRUE, ptin_aclIpDb[i].aclId, 0);

      ptin_aclIpDb[i].aclId = 0;
      memset(ptin_aclIpDb[i].aclRuleNum, 0, L7_MAX_NUM_RULES_PER_ACL);
    }
  }

}


/**
 * Clean an IPv6 ACL
 * 
 * @author joaom (11/4/2013)
 * 
 * @param isAclAdded 
 * @param aclId 
 * @param ruleNum 
 */
void  ptin_aclIpv6Clean(L7_BOOL isAclAdded, L7_uint32 aclId, L7_uint32 ruleNum)
{
  L7_uint32 unit = 0;
  L7_RC_t   rc;
  int       i;

  if( isAclAdded == L7_TRUE)
  {
    rc = usmDbQosAclDelete(unit, aclId);
    for (i=0; i<L7_MAX_ACL_LISTS; i++)
    {
      if (ptin_aclIpv6Db[i].aclId == aclId)
      {
        ptin_aclIpv6Db[i].aclId = 0;
        memset(ptin_aclIpv6Db[i].aclRuleNum, 0, L7_MAX_NUM_RULES_PER_ACL);
      }
    }
  }
  else
  {
    rc = usmDbQosAclRuleRemove(unit, aclId, ruleNum);
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
    if (ptin_aclIpv6Db[i].aclId != 0)
    {
      ptin_aclIpv6Clean(L7_TRUE, ptin_aclIpv6Db[i].aclId, 0);

      ptin_aclIpv6Db[i].aclId = 0;
      memset(ptin_aclIpv6Db[i].aclRuleNum, 0, L7_MAX_NUM_RULES_PER_ACL);
    }
  }

}


/**
 * Clean a MAC ACL
 * 
 * @author joaom (10/30/2013)
 * 
 * @param isAclAdded 
 * @param aclId 
 * @param ruleNum 
 */
void ptin_aclMacClean(L7_BOOL isAclAdded, L7_uint32 aclId, L7_uint32 ruleNum)
{
  L7_uint32 unit = 0;
  L7_RC_t   rc;
  int       i;

  if (isAclAdded == L7_TRUE)
  {
    rc = usmDbQosAclMacDelete(unit, aclId);
    for (i=0; i<L7_MAX_ACL_LISTS; i++)
    {
      if (ptin_aclMacDb[i].aclId == aclId)
      {
        ptin_aclMacDb[i].aclId = 0;
        memset(ptin_aclMacDb[i].aclRuleNum, 0, L7_MAX_NUM_RULES_PER_ACL);
        #ifdef __VLAN_XLATE__
        memset(ptin_aclMacDb[i].aclRuleXlated, 0, L7_MAX_NUM_RULES_PER_ACL);
        #endif
      }
    }
  }
  else
  {
    rc = usmDbQosAclMacRuleRemove(unit, aclId, ruleNum);
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
    if (ptin_aclMacDb[i].aclId != 0)
    {
      ptin_aclMacClean(L7_TRUE, ptin_aclMacDb[i].aclId, 0);

      ptin_aclMacDb[i].aclId = 0;
      memset(ptin_aclMacDb[i].aclRuleNum, 0, L7_MAX_NUM_RULES_PER_ACL);
      #ifdef __VLAN_XLATE__
      memset(ptin_aclMacDb[i].aclRuleXlated, 0, L7_MAX_NUM_RULES_PER_ACL);
      #endif
    }
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
  L7_uint32 tosMask = 0;
  L7_BOOL matchEvery= L7_FALSE;
  L7_BOOL matchSrc = L7_FALSE;
  L7_BOOL matchDst = L7_FALSE;
  L7_BOOL isAclAdded = L7_TRUE;
  L7_BOOL matchOther = L7_FALSE;
  L7_BOOL configuringNamedIpAcl = L7_FALSE;



  /* *********************** ACL_OPERATION_REMOVE *********************** */
  if (operation == ACL_OPERATION_REMOVE)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Removing Rule ID %d (MNG ID %d) on ACL: ID %d (MNG ID %d)", ptin_aclIpDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId], msgAcl->aclRuleId, ptin_aclIpDb[msgAcl->aclId].aclId, msgAcl->aclId);

    if ( (ptin_aclIpDb[msgAcl->aclId].aclId != 0) && (ptin_aclIpDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] != 0) )
    {
      usmDbQosAclRuleRemove(unit, ptin_aclIpDb[msgAcl->aclId].aclId, ptin_aclIpDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId]);

      /* Clear the association between MNG and usmDb Rule IDs */
      ptin_aclIpDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] = 0;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Entry is empty");
    }

    return L7_SUCCESS;
  }


  /* *********************** ACL_OPERATION_CREATE *********************** */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Applying Rule...");

  if (msgAcl->action == ACL_ACTION_PERMIT)
  {
    actionType = L7_ACL_PERMIT;
  }
  else if (msgAcl->action == ACL_ACTION_DENY)
  {
    actionType = L7_ACL_DENY;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
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
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;
  }


  if (msgAcl->aclRuleMask == ACL_IP_RULE_MASK_NONE)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Matching Every");
    matchEvery = L7_TRUE;
  }

  if (matchEvery == L7_FALSE)
  {
    if (!(msgAcl->aclRuleMask & ACL_IP_RULE_MASK_protocol))
    {
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Assuming protocol is IP");
      msgAcl->protocol = L7_ACL_PROTOCOL_IP;
    }

    /* Check the protocol type*/
    if (aclType != ACL_STANDARD)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_PROTOCOL_FEATURE_ID) == L7_TRUE)
      {

        protVal = msgAcl->protocol;

        if ((protVal < L7_ACL_MIN_PROTOCOL_NUM) || (protVal > L7_ACL_MAX_PROTOCOL_NUM))
        {
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
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
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;
      }
      matchSrc = L7_TRUE;
    }
    else if ( (aclType == ACL_STANDARD) && !(msgAcl->aclRuleMask & ACL_IP_RULE_MASK_srcIpAddr) )
    {
      /* In Standard ACL Type, Src IP must be specified */
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;
    }

    if (matchSrc == L7_FALSE && usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) == L7_TRUE)
    {
      if ((aclType != ACL_STANDARD) && (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE))
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_SUPPORTS_MASKING_FEATURE_ID) == L7_FALSE)
        {
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;
        }        
      }

      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Matching Src IP");
      memcpy(&srcIpAddr, &msgAcl->srcIpAddr, sizeof(srcIpAddr));
      memcpy(&srcIpMask, &msgAcl->srcIpMask, sizeof(srcIpMask));
    }
  }

  if (aclType == ACL_EXTENDED)
  {

    /* Check Src L4 Port */
    if (msgAcl->aclRuleMask & ACL_IP_RULE_MASK_srcStartPort)
    {
    
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Matching Src L4 Port");
      if (msgAcl->srcStartPort == msgAcl->srcEndPort)
      {
        srcPortValue = msgAcl->srcStartPort;

        if(srcPortValue < L7_ACL_MIN_L4PORT_NUM || srcPortValue > L7_ACL_MAX_L4PORT_NUM)
        {
          /* Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535> */
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;
        }
      }
      else
      {
        srcStartPort = msgAcl->srcStartPort;
        srcEndPort = msgAcl->srcEndPort;

        if(srcStartPort < L7_ACL_MIN_L4PORT_NUM || srcEndPort > L7_ACL_MAX_L4PORT_NUM)
        {
          /* Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535> */
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;
        }

        if (srcEndPort < srcStartPort)
        {
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;
        }
      }
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
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Matching Dst IP");
      memcpy(&dstIp, &msgAcl->dstIpAddr, sizeof(dstIp));
      memcpy(&dstIpMask, &msgAcl->dstIpMask, sizeof(dstIpMask));
    }


    /* Check Dst L4 Port */
    if (msgAcl->aclRuleMask & ACL_IP_RULE_MASK_dstStartPort)
    {   
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Matching Dst L4 Port");
      if (msgAcl->dstStartPort == msgAcl->dstEndPort)
      {
        dstPortValue = msgAcl->dstStartPort;

        if(dstPortValue < L7_ACL_MIN_L4PORT_NUM || dstPortValue > L7_ACL_MAX_L4PORT_NUM)
        {
          /* Invalid start DSTL4 port. <select DSTL4 port between 0 to 65535> */
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;
        }
      }
      else
      {
        dstStartPort = msgAcl->dstStartPort;
        dstEndPort = msgAcl->dstEndPort;

        if(dstStartPort < L7_ACL_MIN_L4PORT_NUM || dstEndPort > L7_ACL_MAX_L4PORT_NUM)
        {
          /* Invalid start DSTL4 port. <select DSTL4 port between 0 to 65535> */
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;
        }

        if (dstEndPort < dstStartPort)
        {
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;
        }
      }
    }


    /* Check PREC */
    if (msgAcl->aclRuleMask & ACL_IP_RULE_MASK_precVal)
    {
      precVal = msgAcl->precVal;
      matchOther = L7_TRUE;
    }

    /* Check TOS */
    else if (msgAcl->aclRuleMask & ACL_IP_RULE_MASK_tosVal)
    {
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
      dscpVal = msgAcl->dscpVal;

      /* verify if the specified value is in between 0 to 63 */
      if ((dscpVal < L7_ACL_MIN_DSCP) || (dscpVal > L7_ACL_MAX_DSCP))
      {
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
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
    if (ptin_aclIpDb[msgAcl->aclId].aclId != 0) /* ACL already exists */
    {
      rc = usmDbQosAclCreate(unit, aclId); 
      switch (rc)
      {
        case L7_SUCCESS:
          break;

        case L7_FAILURE:
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;

        case L7_TABLE_IS_FULL:
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;

        case L7_ERROR:
          isAclAdded = L7_FALSE;
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;

        case L7_REQUEST_DENIED:
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;

        default:
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
          return L7_FAILURE;
      }

      /* keep the association between MNG and usmDb ACL IDs */
      ptin_aclIpDb[msgAcl->aclId].aclId = aclId;
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Created New ACL: ID %d (MNG ID %d)", aclId, msgAcl->aclId);
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
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (maxAcls=%d)", maxAcls);
          return L7_FAILURE;
        }
        else
        {
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
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
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (maxAcls=%d)", maxAcls);
          return L7_FAILURE;

        case L7_REQUEST_DENIED:
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;

        default:
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
          return L7_FAILURE;
      }

      /* keep the association between MNG and usmDb ACL IDs */
      ptin_aclIpDb[msgAcl->aclId].aclId = aclId;
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Created New ACL: ID %d (MNG ID %d)", aclId, msgAcl->aclId);

      if (usmDbQosAclNameAdd(unit, aclId, accessListName) != L7_SUCCESS)
      {
        ptin_aclIpClean(isAclAdded, aclId, aclRuleNum);
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
        return L7_FAILURE;
      }
    }
  }


  /* Verify if rule exists */
  if ( ptin_aclIpDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] != 0)
  {
    aclRuleNum = ptin_aclIpDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId];
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Configuring existing Rule ID %d (MNG ID %d) on ACL: ID %d (MNG ID %d)", aclRuleNum, msgAcl->aclRuleId, aclId, msgAcl->aclId);
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
      while (usmDbQosAclRuleGetNext(unit, aclId, aclRuleNum, &aclruleNext)== L7_SUCCESS)
      {
        aclRuleNum = aclruleNext;
      }
      aclRuleNum = aclRuleNum+1;
    }
    if (aclRuleNum >L7_ACL_MAX_RULE_NUM)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;
    }
  }

  /* Add the rule to acl*/
  rc = usmDbQosAclRuleActionAdd(unit, aclId, aclRuleNum, actionType);
  switch (rc)
  {
    case L7_SUCCESS:
      break;

    case L7_FAILURE:
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;

    case L7_ERROR:
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;

    case L7_TABLE_IS_FULL:
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;

    case L7_REQUEST_DENIED:
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;

    default:
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
      return L7_FAILURE;
  }

  /* keep the association between MNG and usmDb Rule IDs */
  ptin_aclIpDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] = aclRuleNum;
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Created New Rule ID %d (MNG ID %d) on ACL: ID %d (MNG ID %d)", aclRuleNum, msgAcl->aclRuleId, aclId, msgAcl->aclId);


  /* Match Every */
  if ((matchEvery == L7_TRUE) ||
      (aclType == ACL_STANDARD && matchSrc == L7_TRUE) ||
      (aclType != ACL_STANDARD && matchSrc == L7_TRUE && matchDst == L7_TRUE && matchOther == L7_FALSE))
  {
    rc = usmDbQosAclRuleEveryAdd(unit, aclId, aclRuleNum, L7_TRUE);
    if (rc == L7_FAILURE)
    {
      ptin_aclIpClean(isAclAdded, aclId, aclRuleNum);
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;
    }

    if (rc == L7_ERROR)
    {
      ptin_aclIpClean(isAclAdded, aclId, aclRuleNum);
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;
    }

    if (rc == L7_REQUEST_DENIED)
    {
      ptin_aclIpClean(isAclAdded, aclId, aclRuleNum);
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;
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
          ptin_aclIpClean(isAclAdded, aclId, aclRuleNum);
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;
        }
        else if (rc == L7_REQUEST_DENIED)
        {
          ptin_aclIpClean(isAclAdded, aclId, aclRuleNum);
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
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
          ptin_aclIpClean(isAclAdded, aclId, aclRuleNum);
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
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
        ptin_aclIpClean(isAclAdded, aclId, aclRuleNum);
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;
      }

      if (matchDst == L7_FALSE)
      {
        if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE)
        {
          rc = usmDbQosAclRuleDstIpMaskAdd(unit, aclId, aclRuleNum,dstIp,dstIpMask);

          if (rc == L7_FAILURE)
          {
            ptin_aclIpClean(isAclAdded, aclId, aclRuleNum);
            LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
            return L7_FAILURE;
          }
          else if (rc == L7_REQUEST_DENIED)
          {
            ptin_aclIpClean(isAclAdded, aclId, aclRuleNum);
            LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
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
        ptin_aclIpClean(isAclAdded, aclId, aclRuleNum);
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;
      }

      if (precVal > -1)
      {
        rc = usmDbQosAclRuleIPPrecedenceAdd(unit, aclId, aclRuleNum, precVal);
        if (rc != L7_SUCCESS)
        {
          ptin_aclIpClean(isAclAdded, aclId, aclRuleNum);
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;
        }
      }

      if (tosVal > -1)
      {
        rc = usmDbQosAclRuleIPTosAdd(unit, aclId, aclRuleNum, tosVal, ~tosMask & 0x000000FF);
        if (rc != L7_SUCCESS)
        {
          ptin_aclIpClean(isAclAdded, aclId, aclRuleNum);
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;
        }
      }

      if (dscpVal > -1)
      {
        rc = usmDbQosAclRuleIPDscpAdd(unit, aclId, aclRuleNum, dscpVal);
        if (rc != L7_SUCCESS)
        {
          ptin_aclIpClean(isAclAdded, aclId, aclRuleNum);
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
          return L7_FAILURE;
        }
      }
    }
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Rule Applied!");

  return L7_SUCCESS;
}


/**
 * Apply an IP ACL to an interface or VLAN
 * 
 * @author joaom (11/04/2013)
 * 
 * @param msgAcl 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclIpApply(msg_apply_acl_t *msgAcl, ACL_OPERATION_t operation)
{
  L7_BOOL aclVlanIdGiven = L7_FALSE;
  L7_RC_t rc;
  L7_uint32 aclId, interface = 0;
  L7_uint32 vlanId = 0;
  L7_char8 aclName[L7_ACL_NAME_LEN_MAX+1];
  L7_uint32 direction, feature;
  L7_uint32 unit = 0;
  L7_uint32 sequence = L7_ACL_AUTO_INCR_INTF_SEQ_NUM;
  L7_BOOL   isNamedAcl = L7_FALSE;
  L7_RC_t status = L7_SUCCESS;


  /* determine if ACL is being applied to a VLAN ID */
  if (msgAcl->vlanId != L7_ACL_INVALID_VLAN_ID)
  {
    aclVlanIdGiven = L7_TRUE;

    /* get the vlan id specified by msg */
    vlanId = msgAcl->vlanId;

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Applying ACL to VLAN ID %d", vlanId);

    /* verify if the specified value is in valid range */
    if (usmDbQosAclIsValidVlan(unit, vlanId) == L7_FALSE)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;
    }
  }
  else
  {
    interface = msgAcl->interface;
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Applying ACL to Interface %d", interface);
  }


  /* determine if we are in named IPv4 ACL */
  if (msgAcl->aclType == ACL_TYPE_IP_NAMED)
  {
    /* we are in ipv4 named access list mode, retrieve ACL name for list being configured */
    strcpy(aclName, msgAcl->name);

    if ((usmDbQosAclNameToIndex(unit, aclName, &aclId) != L7_SUCCESS) ||
        (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IP, aclId) != L7_SUCCESS))
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;
    }

    isNamedAcl = L7_TRUE;
  }


  if ((isNamedAcl == L7_FALSE) && ((aclId < L7_MIN_ACL_ID ) || (aclId > L7_MAX_ACL_ID)))
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;
  }

  rc = usmDbQosAclNumCheckValid(unit, aclId);
  if (rc == L7_FAILURE)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;
  }


  if (msgAcl->direction == ACL_DIRECTION_IN)          /* inbound */
  {
    feature = L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID;
    direction = L7_INBOUND_ACL;
  }
  else if (msgAcl->direction == ACL_DIRECTION_OUT)    /* outbound */
  {
    feature = L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID;
    direction = L7_OUTBOUND_ACL;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;
  }


  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, feature) == L7_FALSE)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;
  }

  if (aclVlanIdGiven == L7_FALSE)
  {
    if (operation == ACL_OPERATION_CREATE)
    {
      rc = usmDbQosAclInterfaceDirectionAdd(unit, interface, direction, aclId, sequence);
    }
    else /* ACL_OPERATION_REMOVE */
    {
      rc = usmDbQosAclInterfaceDirectionRemove(unit, interface, direction, aclId);
      if (rc != L7_SUCCESS)
      {
        LOG_DEBUG(LOG_CTX_PTIN_MSG, "No ACL to be Unapplied");
        return L7_SUCCESS;
      }
    }

    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
      status = L7_FAILURE;
    }

    if (rc == L7_FAILURE)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
    }
    else if (rc == L7_ERROR)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
    }
    else if (rc == L7_ALREADY_CONFIGURED)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
    }
    else if (rc == L7_REQUEST_DENIED)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
    }
    else if (rc == L7_TABLE_IS_FULL)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
    }
    else if (rc == L7_NOT_SUPPORTED)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
    }
    else if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
    }

    if (rc != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    
    #ifdef __VLAN_XLATE__
    L7_uint32 oldVlan = vlanId;
    L7_uint16 newVlan;
    ptin_intf_t intf;
    intf.intf_type = 0;
    intf.intf_id = msgAcl->interface; /* A valid interface MUST be specified by the Manager in order to translate the VLAN ID */

    /* ...and translate it to the internal VLAN ID */
    rc = ptin_evc_intVlan_get_fromOVlan(&intf, oldVlan, 0, &newVlan);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
      return L7_FAILURE;
    }

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Translating VLAN ID %d to %d (rc=%d)", oldVlan, (L7_uint32) newVlan, rc);
    vlanId = (L7_uint32) newVlan;
    #endif

    /* applying ACL to a VLAN */
    if (operation == ACL_OPERATION_CREATE)
    {
      if ((rc = usmDbQosAclVlanDirectionAdd(unit, vlanId, direction, aclId, sequence)) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
        return L7_FAILURE;
      }
    }
    else  /* ACL_OPERATION_REMOVE */
    {
      if ((rc = usmDbQosAclVlanDirectionRemove(unit, vlanId, direction, aclId)) != L7_SUCCESS)
      {
        LOG_DEBUG(LOG_CTX_PTIN_MSG, "No ACL to be Unapplied");
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
  L7_int32 flowLabelVal = -1;
  L7_BOOL matchEvery = L7_FALSE;
  L7_BOOL matchSrc = L7_FALSE;
  L7_BOOL matchDst = L7_FALSE;
  L7_BOOL isAclAdded = L7_FALSE;  /* for IPv6 ACLs the list creation is done before entering this mode */
  L7_BOOL matchOther = L7_FALSE;


  /* *********************** ACL_OPERATION_REMOVE *********************** */
  if (operation == ACL_OPERATION_REMOVE)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Removing Rule ID %d (MNG ID %d) on ACL: ID %d (MNG ID %d)", ptin_aclIpv6Db[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId], msgAcl->aclRuleId, ptin_aclIpv6Db[msgAcl->aclId].aclId, msgAcl->aclId);

    if ((ptin_aclIpv6Db[msgAcl->aclId].aclId != 0) && (ptin_aclIpv6Db[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] != 0))
    {
      usmDbQosAclRuleRemove(unit, ptin_aclIpv6Db[msgAcl->aclId].aclId, ptin_aclIpv6Db[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId]);

      /* Clear the association between MNG and usmDb Rule IDs */
      ptin_aclIpv6Db[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] = 0;
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Entry is empty");
    }

    return L7_SUCCESS;
  }


  /* *********************** ACL_OPERATION_CREATE *********************** */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Applying Rule...");

  if (msgAcl->action == ACL_ACTION_PERMIT)
  {
    actionType = L7_ACL_PERMIT;
  }
  else if (msgAcl->action == ACL_ACTION_DENY)
  {
    actionType = L7_ACL_DENY;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
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
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;
  }

  if (msgAcl->aclRuleMask == ACL_IPv6_RULE_MASK_NONE)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Matching Every");
    matchEvery = L7_TRUE;
  }

  if (matchEvery == L7_FALSE)
  {
    if (!(msgAcl->aclRuleMask & ACL_IPv6_RULE_MASK_protocol))
    {
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Assuming protocol is IPv6");
      msgAcl->protocol = L7_ACL_PROTOCOL_IP;
    }

    /* Check the protocol type*/
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_IPV6_PROTOCOL_FEATURE_ID) == L7_TRUE)
    {
      protVal = msgAcl->protocol;

      if ((protVal < L7_ACL_MIN_PROTOCOL_NUM) || (protVal > L7_ACL_MAX_PROTOCOL_NUM))
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
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
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_DSTIP_FEATURE_ID) == L7_TRUE)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;
      }
      matchSrc = L7_TRUE;
    }

    if (matchSrc == L7_FALSE && usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_SRCIP_FEATURE_ID) == L7_TRUE)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID) != L7_TRUE)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;
      }

      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Matching Src IP");
      memcpy(&srcIpv6Addr.in6Addr,      msgAcl->src6Addr, sizeof(msgAcl->src6Addr));
      memcpy(&srcIpv6Addr.in6PrefixLen, &msgAcl->src6PrefixLen, sizeof(msgAcl->src6PrefixLen));
    }
  }


  /* Check Src L4 Port */
  if (msgAcl->aclRuleMask & ACL_IPv6_RULE_MASK_srcStartPort)
  {

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Matching Src L4 Port");
    if (msgAcl->srcStartPort == msgAcl->srcEndPort)
    {
      srcPortValue = msgAcl->srcStartPort;

      if (srcPortValue < L7_ACL_MIN_L4PORT_NUM || srcPortValue > L7_ACL_MAX_L4PORT_NUM)
      {
        /* Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535> */
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;
      }
    }
    else
    {
      srcStartPort = msgAcl->srcStartPort;
      srcEndPort = msgAcl->srcEndPort;

      if (srcStartPort < L7_ACL_MIN_L4PORT_NUM || srcEndPort > L7_ACL_MAX_L4PORT_NUM)
      {
        /* Invalid start SRCL4 port. <select SRCL4 port between 0 to 65535> */
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;
      }

      if (srcEndPort < srcStartPort)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;
      }
    }
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
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Matching Dst IP");
    memcpy(&dstIpv6Addr.in6Addr, &msgAcl->dst6Addr, sizeof(msgAcl->dst6Addr));
    memcpy(&dstIpv6Addr.in6PrefixLen, &msgAcl->dst6PrefixLen, sizeof(msgAcl->dst6PrefixLen));
  }


  /* Check Dst L4 Port */
  if (msgAcl->aclRuleMask & ACL_IPv6_RULE_MASK_dstStartPort)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Matching Dst L4 Port");
    if (msgAcl->dstStartPort == msgAcl->dstEndPort)
    {
      dstPortValue = msgAcl->dstStartPort;

      if (dstPortValue < L7_ACL_MIN_L4PORT_NUM || dstPortValue > L7_ACL_MAX_L4PORT_NUM)
      {
        /* Invalid start DSTL4 port. <select DSTL4 port between 0 to 65535> */
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;
      }
    }
    else
    {
      dstStartPort = msgAcl->dstStartPort;
      dstEndPort = msgAcl->dstEndPort;

      if (dstStartPort < L7_ACL_MIN_L4PORT_NUM || dstEndPort > L7_ACL_MAX_L4PORT_NUM)
      {
        /* Invalid start DSTL4 port. <select DSTL4 port between 0 to 65535> */
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;
      }

      if (dstEndPort < dstStartPort)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;
      }
    }
  }


  /* dscp */
  if (msgAcl->aclRuleMask & ACL_IPv6_RULE_MASK_dscpVal)
  {
    dscpVal = msgAcl->dscpVal;

    /* verify if the specified value is in between 0 to 63 */
    if ((dscpVal < L7_ACL_MIN_DSCP) || (dscpVal > L7_ACL_MAX_DSCP))
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
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
    flowLabelVal = msgAcl->flowLabelVal;

    /* verify if the specified value is in between 0 to 1048575 */
    if ((flowLabelVal < L7_ACL_MIN_FLOWLBL) || (flowLabelVal > L7_ACL_MAX_FLOWLBL))
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
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
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (maxAcls=%d)", maxAcls);
        return L7_FAILURE;
      }
      else
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
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
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (maxAcls=%d)", maxAcls);
        return L7_FAILURE;

      case L7_REQUEST_DENIED:
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;

      default:
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
        return L7_FAILURE;
    }

    /* keep the association between MNG and usmDb ACL IDs */
    ptin_aclIpv6Db[msgAcl->aclId].aclId = aclId;
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Created New ACL: ID %d (MNG ID %d)", aclId, msgAcl->aclId);

    if (usmDbQosAclNameAdd(unit, aclId, accessListName) != L7_SUCCESS)
    {
      ptin_aclIpv6Clean(isAclAdded, aclId, aclRuleNum);
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
      return L7_FAILURE;
    }
  }


  /* Verify if rule exists */
  if ( ptin_aclIpv6Db[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] != 0)
  {
    aclRuleNum = ptin_aclIpv6Db[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId]; 
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Configuring existing Rule ID %d (MNG ID %d) on ACL: ID %d (MNG ID %d)", aclRuleNum, msgAcl->aclRuleId, aclId, msgAcl->aclId);
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
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;
    }
  }

  /* Add the rule to acl*/

  rc = usmDbQosAclRuleActionAdd(unit, aclId, aclRuleNum, actionType);
  switch (rc)
  {
    case L7_SUCCESS:
      break;

    case L7_ERROR:
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;

    case L7_TABLE_IS_FULL:
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;

    case L7_REQUEST_DENIED:
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;

    default:
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
      return L7_FAILURE;
  }

  /* keep the association between MNG and usmDb Rule IDs */
  ptin_aclIpv6Db[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] = aclRuleNum;
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Created New Rule ID %d (MNG ID %d) on ACL: ID %d (MNG ID %d)", aclRuleNum, msgAcl->aclRuleId, aclId, msgAcl->aclId);


  /* Match Every */
  if ((matchEvery == L7_TRUE) ||
      (matchSrc == L7_TRUE && matchDst == L7_TRUE && matchOther == L7_FALSE))
  {
    rc = usmDbQosAclRuleEveryAdd(unit, aclId, aclRuleNum, L7_TRUE);
    if (rc != L7_SUCCESS)
    {
      ptin_aclIpv6Clean(isAclAdded, aclId, aclRuleNum);
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
      return L7_FAILURE;
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
          ptin_aclIpv6Clean(isAclAdded, aclId, aclRuleNum);
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
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
        ptin_aclIpv6Clean(isAclAdded, aclId, aclRuleNum);
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
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
      ptin_aclIpv6Clean(isAclAdded, aclId, aclRuleNum);
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
      return L7_FAILURE;
    }

    if (matchDst == L7_FALSE)
    {
      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_IPV6_DSTIP_FEATURE_ID) == L7_TRUE)
      {
        rc = usmDbQosAclRuleDstIpv6AddrAdd(unit, aclId, aclRuleNum, &dstIpv6Addr);

        if (rc != L7_SUCCESS)
        {
          ptin_aclIpv6Clean(isAclAdded, aclId, aclRuleNum);
          LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
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
      ptin_aclIpv6Clean(isAclAdded, aclId, aclRuleNum);
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
      return L7_FAILURE;
    }

    if (dscpVal > -1)
    {
      rc = usmDbQosAclRuleIPDscpAdd(unit, aclId, aclRuleNum, dscpVal);
      if (rc != L7_SUCCESS)
      {
        ptin_aclIpv6Clean(isAclAdded, aclId, aclRuleNum);
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
        return L7_FAILURE;
      }
    }

    if (flowLabelVal > -1)
    {
      rc = usmDbQosAclRuleIpv6FlowLabelAdd(unit, aclId, aclRuleNum, flowLabelVal);
      if (rc != L7_SUCCESS)
      {
        ptin_aclIpv6Clean(isAclAdded, aclId, aclRuleNum);
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
        return L7_FAILURE;
      }
    }
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Rule Applied!");

  return L7_SUCCESS;
}


/**
 * Apply an IPv6 ACL to an interface or VLAN
 * 
 * @author joaom (11/06/2013)
 * 
 * @param msgAcl 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclIpv6Apply(msg_apply_acl_t *msgAcl, ACL_OPERATION_t operation)
{
  L7_BOOL aclVlanIdGiven = L7_FALSE;
  L7_RC_t rc;
  L7_uint32 aclId, interface = 0;
  L7_uint32 vlanId = 0;
  L7_char8 aclName[L7_ACL_NAME_LEN_MAX+1];
  L7_uint32 direction, feature;
  L7_uint32 unit = 0;
  L7_uint32 sequence = L7_ACL_AUTO_INCR_INTF_SEQ_NUM;
  L7_RC_t status = L7_SUCCESS;


  /* determine if ACL is being applied to a VLAN ID */
  if (msgAcl->vlanId != L7_ACL_INVALID_VLAN_ID)
  {
    aclVlanIdGiven = L7_TRUE;

    /* get the vlan id specified by msg */
    vlanId = msgAcl->vlanId;

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Applying ACL to VLAN ID %d", vlanId);

    /* verify if the specified value is in valid range */
    if (usmDbQosAclIsValidVlan(unit, vlanId) == L7_FALSE)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;
    }
  }
  else
  {
    interface = msgAcl->interface;
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Applying ACL to Interface %d", interface);
  }


  /* determine if we are in IPv6 ACL */
  if (msgAcl->aclType == ACL_TYPE_IPv6_EXTENDED)
  {
    /* Retrieve ACL name for list being configured */
    strcpy(aclName, msgAcl->name);

    if ((usmDbQosAclNameToIndex(unit, aclName, &aclId) != L7_SUCCESS) ||
        (usmDbQosAclNamedIndexRangeCheck(unit, L7_ACL_TYPE_IPV6, aclId) != L7_SUCCESS))
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;
    }
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;
  }


  rc = usmDbQosAclNumCheckValid(unit, aclId);
  if (rc == L7_FAILURE)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;
  }


  if (msgAcl->direction == ACL_DIRECTION_IN)          /* inbound */
  {
    feature = L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID;
    direction = L7_INBOUND_ACL;
  }
  else if (msgAcl->direction == ACL_DIRECTION_OUT)    /* outbound */
  {
    feature = L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID;
    direction = L7_OUTBOUND_ACL;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;
  }


  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, feature) == L7_FALSE)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;
  }

  if (aclVlanIdGiven == L7_FALSE)
  {
    if (operation == ACL_OPERATION_CREATE)
    {
      rc = usmDbQosAclInterfaceDirectionAdd(unit, interface, direction, aclId, sequence);
    }
    else /* ACL_OPERATION_REMOVE */
    {
      rc = usmDbQosAclInterfaceDirectionRemove(unit, interface, direction, aclId);
      if (rc != L7_SUCCESS)
      {
        LOG_DEBUG(LOG_CTX_PTIN_MSG, "No ACL to be Unapplied");
        return L7_SUCCESS;
      }
    }

    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
      status = L7_FAILURE;
    }

    if (rc == L7_FAILURE)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
    }
    else if (rc == L7_ERROR)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
    }
    else if (rc == L7_ALREADY_CONFIGURED)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
    }
    else if (rc == L7_REQUEST_DENIED)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
    }
    else if (rc == L7_TABLE_IS_FULL)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
    }
    else if (rc == L7_NOT_SUPPORTED)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
    }
    else if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
    }

    if (rc != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }
  else
  {

    #ifdef __VLAN_XLATE__
    L7_uint32 oldVlan = vlanId;
    L7_uint16 newVlan;
    ptin_intf_t intf;
    intf.intf_type = 0;
    intf.intf_id = msgAcl->interface; /* A valid interface MUST be specified by the Manager in order to translate the VLAN ID */

    /* ...and translate it to the internal VLAN ID */
    rc = ptin_evc_intVlan_get_fromOVlan(&intf, oldVlan, 0, &newVlan);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
      return L7_FAILURE;
    }

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Translating VLAN ID %d to %d (rc=%d)", oldVlan, (L7_uint32) newVlan, rc);
    vlanId = (L7_uint32) newVlan;
    #endif

    /* applying ACL to a VLAN */
    if (operation == ACL_OPERATION_CREATE)
    {
      if ((rc = usmDbQosAclVlanDirectionAdd(unit, vlanId, direction, aclId, sequence)) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
        return L7_FAILURE;
      }
    }
    else  /* ACL_OPERATION_REMOVE */
    {
      if ((rc = usmDbQosAclVlanDirectionRemove(unit, vlanId, direction, aclId)) != L7_SUCCESS)
      {
        LOG_DEBUG(LOG_CTX_PTIN_MSG, "No ACL to be Unapplied");
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
  
  L7_char8 macAccessListName[L7_CLI_MAX_STRING_LENGTH];
  L7_BOOL matchSrc = L7_FALSE;
  L7_BOOL matchDst = L7_FALSE;
  L7_BOOL matchOther = L7_FALSE;
  L7_BOOL isAclAdded = L7_FALSE;  /* for MAC ACLs the list creation is done before entering this mode */
  

  /* *********************** ACL_OPERATION_REMOVE *********************** */
  if (operation == ACL_OPERATION_REMOVE)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Removing Rule ID %d (MNG ID %d) on ACL: ID %d (MNG ID %d)", ptin_aclMacDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId], msgAcl->aclRuleId, ptin_aclMacDb[msgAcl->aclId].aclId, msgAcl->aclId);

    if ( (ptin_aclMacDb[msgAcl->aclId].aclId != 0) && (ptin_aclMacDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] != 0) )
    {
      usmDbQosAclMacRuleRemove(unit, ptin_aclMacDb[msgAcl->aclId].aclId, ptin_aclMacDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId]);

      /* Clear the association between MNG and usmDb Rule IDs */
      ptin_aclMacDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] = 0;
      #ifdef __VLAN_XLATE__
      ptin_aclMacDb[msgAcl->aclId].aclRuleXlated[msgAcl->aclRuleId] = 0;
      #endif
    }
    else
    {
      LOG_DEBUG(LOG_CTX_PTIN_MSG, "Entry is empty");
    }

    return L7_SUCCESS;
  }


  /* *********************** ACL_OPERATION_CREATE *********************** */
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Applying Rule...");

  if (msgAcl->action == ACL_ACTION_PERMIT)
  {
    actionType = L7_ACL_PERMIT;
  }
  else if (msgAcl->action == ACL_ACTION_DENY)
  {
    actionType = L7_ACL_DENY;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;
  }


  /* Any Src MAC */
  if (!(msgAcl->aclRuleMask & ACL_MAC_RULE_MASK_srcMacAddr))
  {
    matchSrc = L7_TRUE;
  }

  if (matchSrc == L7_FALSE && usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, L7_ACL_RULE_MATCH_SRCMAC_FEATURE_ID) == L7_TRUE)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Matching Src MAC");
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
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Matching Dst MAC");
    memcpy(dstMacAddr, msgAcl->dstMacAddr, sizeof(dstMacAddr));
    memcpy(dstMacMask, msgAcl->dstMacMask, sizeof(dstMacMask));
  }


  /* Ethertype */
  if (msgAcl->aclRuleMask & ACL_MAC_RULE_MASK_eType)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Matching Ethertype");
    eTypeKeyVal = L7_QOS_ETYPE_KEYID_CUSTOM;
    eTypeCustVal = msgAcl->eType;
    matchOther = L7_TRUE;
  }


  /* VLAN */
  if (msgAcl->aclRuleMask & ACL_MAC_RULE_MASK_startVlan)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Matching VLAN");
    if (msgAcl->startVlan == msgAcl->endVlan)
    {
      vlanVal = msgAcl->startVlan;

      /* verify if the specified value is in between 1 to 4094 */
      if ((vlanVal < L7_ACL_MIN_VLAN_ID) || (vlanVal > L7_ACL_MAX_VLAN_ID))
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;
      }
    }
    else
    {
      startVlanVal = msgAcl->startVlan;
      endVlanVal = msgAcl->endVlan;

      /* verify if the specified value is in between 1 to 4094 */
      if ((startVlanVal < L7_ACL_MIN_VLAN_ID) || (endVlanVal > L7_ACL_MAX_VLAN_ID))
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;
      }

      if (endVlanVal < startVlanVal)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;
      }
    }
    matchOther = L7_TRUE;
  }


  /* COS */
  if (msgAcl->aclRuleMask & ACL_MAC_RULE_MASK_cosVal)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Matching COS");

    cosVal = msgAcl->cosVal;

    if ((cosVal < L7_ACL_MIN_COS) ||
        (cosVal > L7_ACL_MAX_COS))
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
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
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;
      }
      else
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
        return L7_FAILURE;
      }
    }

    rc = usmDbQosAclMacCreate(unit, aclId);
    switch (rc)
    {
      case L7_SUCCESS:
        break;

      case L7_TABLE_IS_FULL:
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;

      case L7_REQUEST_DENIED:
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
        return L7_FAILURE;

      default:
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
        return L7_FAILURE;
    }

    /* keep the association between MNG and usmDb ACL IDs */
    ptin_aclMacDb[msgAcl->aclId].aclId = aclId;
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Created New ACL: ID %d (MNG ID %d)", aclId, msgAcl->aclId);

    if (usmDbQosAclMacNameAdd(unit, aclId, macAccessListName) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
      return L7_FAILURE;
    }
  }


  /* Verify if rule exists */
  if ( ptin_aclMacDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] != 0)
  {
    aclRuleNum = ptin_aclMacDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId]; 
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Configuring existing Rule ID %d (MNG ID %d) on ACL: ID %d (MNG ID %d)", aclRuleNum, msgAcl->aclRuleId, aclId, msgAcl->aclId);
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
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;
    }
  }
  

  rc = usmDbQosAclMacRuleActionAdd(unit, aclId, aclRuleNum, actionType);
  switch (rc)
  {
  case L7_SUCCESS:
    break;

  case L7_ERROR:
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;

  case L7_TABLE_IS_FULL:
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;

  case L7_REQUEST_DENIED:
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;

  default:
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
    return L7_FAILURE;
  }

  /* keep the association between MNG and usmDb Rule IDs */
  ptin_aclMacDb[msgAcl->aclId].aclRuleNum[msgAcl->aclRuleId] = aclRuleNum;
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Created New Rule ID %d (MNG ID %d) on ACL: ID %d (MNG ID %d)", aclRuleNum, msgAcl->aclRuleId, aclId, msgAcl->aclId);


  /* Match Every */
  if (matchOther == L7_FALSE && matchSrc == L7_TRUE && matchDst == L7_TRUE)
  {
    rc = usmDbQosAclMacRuleEveryAdd(unit, aclId, aclRuleNum, matchSrc);
    if (rc == L7_FAILURE)
    {
      ptin_aclMacClean(isAclAdded, aclId, aclRuleNum);
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;
    }

    if (rc == L7_ERROR)
    {
      ptin_aclMacClean(isAclAdded, aclId, aclRuleNum);
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;
    }

    if (rc == L7_REQUEST_DENIED)
    {
      ptin_aclMacClean(isAclAdded, aclId, aclRuleNum);
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;
    }
  }
  else
  {
    if (matchSrc == L7_FALSE)
    {
      rc = usmDbQosAclMacRuleSrcMacAdd(unit, aclId, aclRuleNum, srcMacAddr, srcMacMask);

      if (rc == L7_FAILURE)
      {
        ptin_aclMacClean(isAclAdded, aclId, aclRuleNum);
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
        return L7_FAILURE;
      }
      else if (rc == L7_REQUEST_DENIED)
      {
        ptin_aclMacClean(isAclAdded, aclId, aclRuleNum);
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
        return L7_FAILURE;
      }
    }

    if (matchDst == L7_FALSE)
    {
      rc = usmDbQosAclMacRuleDstMacAdd(unit, aclId, aclRuleNum, dstMacAddr, dstMacMask);

      if (rc == L7_FAILURE)
      {
        ptin_aclMacClean(isAclAdded, aclId, aclRuleNum);
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
        return L7_FAILURE;
      }
      else if (rc == L7_REQUEST_DENIED)
      {
        ptin_aclMacClean(isAclAdded, aclId, aclRuleNum);
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
        return L7_FAILURE;
      }
    }

    /* EtherType */
    if (eTypeKeyVal > -1 && eTypeKeyVal != 0)
    {
      rc = usmDbQosAclMacRuleEtypeKeyAdd(unit, aclId, aclRuleNum, eTypeKeyVal, eTypeCustVal);
      if (rc != L7_SUCCESS)
      {
        ptin_aclMacClean(isAclAdded, aclId, aclRuleNum);
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
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
      ptin_aclMacClean(isAclAdded, aclId, aclRuleNum);
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
      return L7_FAILURE;
    }

    /* COS */
    if (cosVal > -1)
    {
      rc = usmDbQosAclMacRuleCosAdd(unit, aclId, aclRuleNum, cosVal);
      if (rc != L7_SUCCESS)
      {
        ptin_aclMacClean(isAclAdded, aclId, aclRuleNum);
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
        return L7_FAILURE;
      }
    }

  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "Rule Applied!");

  return L7_SUCCESS;
}


/**
 * Apply a MAC ACL to an interface or VLAN
 * 
 * @author joaom (10/30/2013)
 * 
 * @param msgAcl 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_aclMacApply(msg_apply_acl_t *msgAcl, ACL_OPERATION_t operation)
{
  L7_BOOL aclVlanIdGiven = L7_FALSE;
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 interface = 0, aclId = 0, direction, feature;
  L7_uint32 vlanId = 0;
  L7_char8 macAclName[L7_ACL_NAME_LEN_MAX + 1];
  L7_uint32 sequence = L7_ACL_AUTO_INCR_INTF_SEQ_NUM;
  L7_uint32 unit = 0;
  L7_BOOL noMacAclName = L7_FALSE;
  L7_RC_t status = L7_SUCCESS;


  /* determine if ACL is being applied to a VLAN ID */
  if (msgAcl->vlanId != L7_ACL_INVALID_VLAN_ID)
  {
    aclVlanIdGiven = L7_TRUE;

    /* get the vlan id specified by msg */
    vlanId = msgAcl->vlanId;

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Applying ACL to VLAN ID %d", vlanId);

    /* verify if the specified value is in valid range */
    if (usmDbQosAclIsValidVlan(unit, vlanId) == L7_FALSE)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
      return L7_FAILURE;
    }
  }
  else
  {
    interface = msgAcl->interface;
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Applying ACL to Interface %d", interface);
  }


  strcpy(macAclName, msgAcl->name);

  if (usmDbQosAclMacNameToIndex(0, macAclName, &aclId) != L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;
  }

  if (usmDbQosAclMacIndexCheckValid(0, aclId) == L7_ERROR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;
  }



  if (msgAcl->direction == ACL_DIRECTION_IN)          /* inbound */
  {
    feature = L7_ACL_INTF_DIRECTION_INBOUND_FEATURE_ID;
    direction = L7_INBOUND_ACL;
  }
  else if (msgAcl->direction == ACL_DIRECTION_OUT)    /* outbound */
  {
    feature = L7_ACL_INTF_DIRECTION_OUTBOUND_FEATURE_ID;
    direction = L7_OUTBOUND_ACL;
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;
  }



  if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_ACL_COMPONENT_ID, feature) == L7_FALSE)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    return L7_FAILURE;
  }



  if (aclVlanIdGiven == L7_FALSE)
  {
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
        if ( (ptin_aclMacDb[msgAcl->aclId].aclRuleNum[i] != 0) && !(ptin_aclMacDb[msgAcl->aclId].aclRuleXlated[i]))
        {
          /* Get the VLAN ID originally associated to the rule...  */
          rc = aclMacRuleVlanIdGet(aclId, i, &oldVlan);
          if (rc != L7_SUCCESS)
          {
            LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
          }
          else if (oldVlan != 0)
          {
            ptin_intf_t intf;
            intf.intf_type = 0;
            intf.intf_id = interface;
            /* ...and translate it to the internal VLAN ID */
            rc = ptin_evc_intVlan_get_fromOVlan(&intf, oldVlan, 0, &newVlan);
            if (rc == L7_SUCCESS)
            {
              LOG_DEBUG(LOG_CTX_PTIN_MSG, "Rule %d: Translating VLAN ID %d to %d (rc=%d)", i, oldVlan, (L7_uint32) newVlan, rc);

              aclMacRuleVlanIdAdd(aclId, i, (L7_uint32) newVlan);

              ptin_aclMacDb[msgAcl->aclId].aclRuleXlated[i] = 1;
            }
          }
        }
      }
      #endif

      rc = usmDbQosAclMacInterfaceDirectionAdd(unit, interface, direction, aclId, sequence);
    }
    else /* ACL_OPERATION_REMOVE */
    {
      rc = usmDbQosAclMacInterfaceDirectionRemove(unit, interface, direction, aclId);
      if (rc != L7_SUCCESS)
      {
        LOG_DEBUG(LOG_CTX_PTIN_MSG, "No ACL to be Unapplied");
        return L7_SUCCESS;
      }
    }

    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
      status = L7_FAILURE;
    }

    if (rc == L7_TABLE_IS_FULL)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    }
    else if (rc == L7_ALREADY_CONFIGURED)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    }
    else if (rc == L7_REQUEST_DENIED)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    }
    else if (rc == L7_NOT_SUPPORTED)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE");
    }
    else if (rc != L7_SUCCESS)
    {
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
    ptin_intf_t intf;
    intf.intf_type = 0;
    intf.intf_id = msgAcl->interface; /* A valid interface MUST be specified by the Manager in order to translate the VLAN ID */

    /* ...and translate it to the internal VLAN ID */
    rc = ptin_evc_intVlan_get_fromOVlan(&intf, oldVlan, 0, &newVlan);
    if (rc != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
      return L7_FAILURE;
    }

    LOG_DEBUG(LOG_CTX_PTIN_MSG, "Translating VLAN ID %d to %d (rc=%d)", oldVlan, (L7_uint32) newVlan, rc);
    vlanId = (L7_uint32) newVlan;
    #endif

    if (operation == ACL_OPERATION_CREATE)
    {
      if ((rc = usmDbQosAclMacVlanDirectionAdd(unit, vlanId, direction, aclId, sequence)) != L7_SUCCESS)
      {
        LOG_ERR(LOG_CTX_PTIN_MSG, "ACL FAILURE (rc=%d)", rc);
        return L7_FAILURE;
      }
    }
    else  /* ACL_OPERATION_REMOVE */
    {
      // Just unapply the ACL. The same ACL can be in used by several entities (VLANs and Interfaces).
      if ((rc = usmDbQosAclMacVlanDirectionRemove(unit, vlanId, direction, aclId)) != L7_SUCCESS)
      {
        LOG_DEBUG(LOG_CTX_PTIN_MSG, "No ACL to be Unapplied");
        return L7_SUCCESS;
      }
    }
  }

  return L7_SUCCESS;
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

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "--------------------------------------");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "IP ACL Info: MNG ID %d -> ACL ID %d", mngAclId, ptin_aclIpDb[mngAclId].aclId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "--------------------------------------");

  for (i=1; i<=L7_MAX_NUM_RULES_PER_ACL; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Rule Info: MNG ID %2d -> Rule ID %2d", i, ptin_aclIpDb[mngAclId].aclRuleNum[i]);
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "--------------------------------------");

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

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "--------------------------------------");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "IPv6 ACL Info: MNG ID %d -> ACL ID %d", mngAclId, ptin_aclIpv6Db[mngAclId].aclId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "--------------------------------------");

  for (i=1; i<=L7_MAX_NUM_RULES_PER_ACL; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Rule Info: MNG ID %2d -> Rule ID %2d", i, ptin_aclIpv6Db[mngAclId].aclRuleNum[i]);
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "--------------------------------------");

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

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "--------------------------------------");
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "MAC ACL Info: MNG ID %d -> ACL ID %d", mngAclId, ptin_aclMacDb[mngAclId].aclId);
  LOG_DEBUG(LOG_CTX_PTIN_MSG, "--------------------------------------");

  for (i=1; i<=L7_MAX_NUM_RULES_PER_ACL; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG, "  Rule Info: MNG ID %2d -> Rule ID %2d", i, ptin_aclMacDb[mngAclId].aclRuleNum[i]);
  }

  LOG_DEBUG(LOG_CTX_PTIN_MSG, "--------------------------------------");

  return L7_SUCCESS; 
}

