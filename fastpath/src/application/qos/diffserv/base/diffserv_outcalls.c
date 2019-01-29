/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_outcalls.c
*
* @purpose    DiffServ component outcall functions.
*
* @component  DiffServ
*
* @comments   none
*
* @create     08/28/2002
*
* @author     rjindal
* @author     gpaussa
* @end
*
**********************************************************************/
#include "l7_common.h"
#include "usmdb_mib_diffserv_common.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "l7_diffserv_include.h"

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  #include "usmdb_qos_acl_api.h"
  #include "acl_api.h"
#endif


/*********************************************************************
* @purpose  Check if the ACL component is present
*
* @param    Void
*
* @returns  L7_TRUE, if ACL component is present
* @returns  L7_FALSE, if ACL component is absent
*
* @comments None
*
* @end
*********************************************************************/
L7_BOOL diffServIsAclPresent(void)
{
  return cnfgrIsComponentPresent(L7_FLEX_QOS_ACL_COMPONENT_ID);
}

/*********************************************************************
* @purpose  Check the validity of the input ACL number per the ACL type
*
* @param    aclType     @b{(input)} ACL type
* @param    aclNum      @b{(input)} ACL number
*
* @returns  L7_SUCCESS, if valid ACL number
* @returns  L7_ERROR, if invalid ACL number
* @returns  L7_FAILURE, if ACL component is absent
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t diffServAclNumCheckValid(dsmibClassAclType_t aclType, L7_uint32 aclNum)
{
#ifdef L7_QOS_FLEX_PACKAGE_ACL

  L7_RC_t rc = L7_ERROR;

  if (aclType == L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_IP)
    rc = aclNumCheckValid(aclNum);
  else if (aclType == L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_MAC)
    rc = aclMacIndexCheckValid(aclNum);

  if (rc != L7_SUCCESS)
    rc = L7_ERROR;

  return rc;

#else
  return L7_FAILURE;

#endif
}

/*********************************************************************
* @purpose  Get the ACL number of the first created Access List
*
* @param    val  @b{(output)} pointer to the ACL number
*
* @returns  L7_SUCCESS, if an ACL exists
* @returns  L7_ERROR, if no ACLs have been created
* @returns  L7_FAILURE, if ACL component is absent
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t diffServAclNumFirstGet(L7_uint32 *val)
{
#ifdef L7_QOS_FLEX_PACKAGE_ACL

  L7_RC_t rc;

  *val = 0;
  rc = aclNumGetFirst(val);
  if (rc != L7_SUCCESS)
    rc = L7_ERROR;

  return rc;

#else
  *val = 0;
  return L7_FAILURE;

#endif
}

/*********************************************************************
* @purpose  Process each valid rule in an Access List definition
*
* @param    pRow            @b{(input)} Class Table row pointer
* @param    aclType         @b{(input)} Access List type
* @param    aclNum          @b{(input)} Access List number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The ACL component does not expose the implicit 'deny all'
*           last rule via its getFirst/getNext APIs.
*
* @end
*********************************************************************/
L7_RC_t diffServAclExtract(dsmibClassEntryCtrl_t *pRow,
                           dsmibClassAclType_t aclType, L7_uint32 aclNum)
{
#ifdef L7_QOS_FLEX_PACKAGE_ACL

  L7_RC_t       rc;

  if (aclType == L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_IP)
    rc = diffServAclIpExtract(pRow, aclNum);
  else if (aclType == L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_MAC)
    rc = diffServAclMacExtract(pRow, aclNum);
  else
    rc = L7_FAILURE;

  /* an L7_ERROR is returned when something goes wrong during ACL list processing,
   * so anything that was created up to this point must now be deleted
   */
  if (rc == L7_ERROR)
  {
    if (dsmibClassRuleAclDelete(pRow->key.classIndex) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
              "diffServAclExtract: Unable to delete rules for class index %u\n",
              pRow->key.classIndex);
    }
    rc = L7_FAILURE;
  }

  return rc;

#else
  return L7_FAILURE;

#endif /* L7_QOS_FLEX_PACKAGE_ACL */
}

/*********************************************************************
* @purpose  Process each valid rule in an IP Access List definition
*
* @param    pRow            @b{(input)} Class Table row pointer
* @param    aclNum          @b{(input)} IP Access List number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        ACL list processing error
* @returns  L7_FAILURE
*
* @notes    The ACL component does not expose the implicit 'deny all'
*           last rule via its getFirst/getNext APIs.
*
* @end
*********************************************************************/
L7_RC_t diffServAclIpExtract(dsmibClassEntryCtrl_t *pRow, L7_uint32 aclNum)
{
#ifdef L7_QOS_FLEX_PACKAGE_ACL

  L7_RC_t       rc;
  L7_uint32     ruleNum;

  /* Use the ACL component API functions to walk through each ACL rule and
   * create a corresponding set of rules in the Class Rule Table for
   * this class.
   *
   * NOTE: We can ignore the implicit 'deny all' rule that is defined at
   *       the end of every ACL, since for DiffServ this implies any
   *       packet not belonging to the class is to receive best-effort
   *       treatment, which is the normal action for a DiffServ policy
   *       instance.
   */
  if (aclNumCheckValid(aclNum) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclRuleGetFirst(aclNum, &ruleNum);

  while (rc == L7_SUCCESS)
  {
    if (aclRuleCheckValid(aclNum, ruleNum) != L7_SUCCESS)
    {
      return L7_ERROR;
    }

    if (diffServAclIpClassRuleSetup(pRow, aclNum, ruleNum) != L7_SUCCESS)
    {
      return L7_ERROR;
    }

    rc = aclRuleGetNext(aclNum, ruleNum, &ruleNum);
  }

  return L7_SUCCESS;

#else
  return L7_FAILURE;

#endif /* L7_QOS_FLEX_PACKAGE_ACL */
}

/*********************************************************************
* @purpose  Process each valid rule in a MAC Access List definition
*
* @param    pRow            @b{(input)} Class Table row pointer
* @param    aclNum          @b{(input)} MAC Access List number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        ACL list processing error
* @returns  L7_FAILURE
*
* @notes    The ACL component does not expose the implicit 'deny all'
*           last rule via its getFirst/getNext APIs.
*
* @end
*********************************************************************/
L7_RC_t diffServAclMacExtract(dsmibClassEntryCtrl_t *pRow, L7_uint32 aclNum)
{
#ifdef L7_QOS_FLEX_PACKAGE_ACL

  L7_RC_t       rc;
  L7_uint32     ruleNum;

  /* Use the ACL component API functions to walk through each ACL rule and
   * create a corresponding set of rules in the Class Rule Table for
   * this class.
   *
   * NOTE: We can ignore the implicit 'deny all' rule that is defined at
   *       the end of every ACL, since for DiffServ this implies any
   *       packet not belonging to the class is to receive best-effort
   *       treatment, which is the normal action for a DiffServ policy
   *       instance.
   */
  if (aclMacIndexCheckValid(aclNum) != L7_SUCCESS)
    return L7_FAILURE;

  rc = aclMacRuleGetFirst(aclNum, &ruleNum);

  while (rc == L7_SUCCESS)
  {
    if (aclMacRuleCheckValid(aclNum, ruleNum) != L7_SUCCESS)
    {
      return L7_ERROR;
    }

    if (diffServAclMacClassRuleSetup(pRow, aclNum, ruleNum) != L7_SUCCESS)
    {
      return L7_ERROR;
    }

    rc = aclMacRuleGetNext(aclNum, ruleNum, &ruleNum);
  }

  return L7_SUCCESS;

#else
  return L7_FAILURE;

#endif /* L7_QOS_FLEX_PACKAGE_ACL */
}

/*********************************************************************
* @purpose  Gather information from the IP Access List Rule for use in
*           setting up a DiffServ class rule
*
* @param    pClass          @b{(input)} Class Table row pointer
* @param    aclNum          @b{(input)} IP Access List number
* @param    ruleNum         @b{(input)} ACL Rule number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Does not follow the normal row creation process, but fills in the
*           class rule row objects directly.  This assumes that the ACL rule
*           is properly configured with valid information.
*
* @end
*********************************************************************/
L7_RC_t diffServAclIpClassRuleSetup(dsmibClassEntryCtrl_t *pClass,
                                    L7_uint32 aclNum,
                                    L7_uint32 ruleNum)
{
#ifdef L7_QOS_FLEX_PACKAGE_ACL

  dsmibClassAclType_t       aclType = L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_IP;
  L7_uint32                 action;
  dsmibTruthValue_t         excludeFlag, matchEvery;
  L7_BOOL                   aclEveryFlag;
  L7_uint32                 val, val2;
  L7_uchar8                 cval, cval2;
  L7_in6_prefix_t           ipv6addr;

  if (aclRuleActionGet(aclNum, ruleNum, &action) != L7_SUCCESS)
    return L7_FAILURE;

  /* translate ACL Rule action into DiffServ rule exclude flag value */
  if (action == L7_ACL_PERMIT)
    excludeFlag = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE;
  else
    excludeFlag = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE;

  /* manually set up the fields in this class rule row using the valid fields
   * from the ACL Rule as indicated by the flags read earlier
   *
   * NOTE: Each group of DiffServ class rules belonging to the SAME
   *       ACL Rule is treated internally like a class type 'all'.  The
   *       collection of class rule groups, one per each ACL Rule, is
   *       handled internally like a class type 'any'.  Thus, a DiffServ
   *       class of type 'acl' is somewhat of a hybrid within the component.
   */

  /* check first for 'match every' ACL classifier */
  if (aclIsFieldConfigured(aclNum, ruleNum, ACL_EVERY) == L7_TRUE)
  {
    if (aclRuleEveryGet(aclNum, ruleNum, &aclEveryFlag) != L7_SUCCESS)
      return L7_FAILURE;
    /* translate ACL match every flag into DiffServ match every flag value */
    if (aclEveryFlag == L7_TRUE)
      matchEvery = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE;
    else
      matchEvery = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE;
    val2 = 0;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY,
                                  excludeFlag, &matchEvery, &val2) != L7_SUCCESS)
      return L7_FAILURE;

    /* no additional DiffServ rules to set up for 'match every' case */
    return L7_SUCCESS;
  }

  /* check for match on ACL destination IP address and mask */
  if ((aclIsFieldConfigured(aclNum, ruleNum, ACL_DSTIP) == L7_TRUE) &&
      (aclIsFieldConfigured(aclNum, ruleNum, ACL_DSTIP_MASK) == L7_TRUE))
  {
    if (aclRuleDstIpMaskGet(aclNum, ruleNum, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on ACL destination IPv6 address (and prefix length) */
  if (aclIsFieldConfigured(aclNum, ruleNum, ACL_DSTIPV6) == L7_TRUE)
  {
    if (aclRuleDstIpv6AddrGet(aclNum, ruleNum, &ipv6addr) != L7_SUCCESS)
      return L7_FAILURE;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6,
                                  excludeFlag, &ipv6addr.in6Addr, &ipv6addr.in6PrefixLen) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on ACL destination IPv6 address (and prefix length) */
  if (aclIsFieldConfigured(aclNum, ruleNum, ACL_FLOWLBLV6) == L7_TRUE)
  {
    if (aclRuleIpv6FlowLabelGet(aclNum, ruleNum, &val) != L7_SUCCESS)
      return L7_FAILURE;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on ACL layer 4 destination port number */
  if (aclIsFieldConfigured(aclNum, ruleNum, ACL_DSTPORT) == L7_TRUE)
  {
    /* NOTE: same value used for both start and end L4 port number */
    if (aclRuleDstL4PortGet(aclNum, ruleNum, &val) != L7_SUCCESS)
      return L7_FAILURE;
    val2 = val;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on ACL layer 4 destination port range */
  if ((aclIsFieldConfigured(aclNum, ruleNum, ACL_DSTSTARTPORT) == L7_TRUE) &&
      (aclIsFieldConfigured(aclNum, ruleNum, ACL_DSTENDPORT) == L7_TRUE))
  {
    if (aclRuleDstL4PortRangeGet(aclNum, ruleNum, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on ACL IP DSCP */
  if (aclIsFieldConfigured(aclNum, ruleNum, ACL_IPDSCP) == L7_TRUE)
  {
    if (aclRuleIPDscpGet(aclNum, ruleNum, &val) != L7_SUCCESS)
      return L7_FAILURE;
    val2 = 0;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on ACL IP Precedence */
  if (aclIsFieldConfigured(aclNum, ruleNum, ACL_IPPREC) == L7_TRUE)
  {
    if (aclRuleIPPrecedenceGet(aclNum, ruleNum, &val) != L7_SUCCESS)
      return L7_FAILURE;
    val2 = 0;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on ACL IP TOS (free form) */
  if (aclIsFieldConfigured(aclNum, ruleNum, ACL_IPTOS) == L7_TRUE)
  {
    if (aclRuleIPTosGet(aclNum, ruleNum, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;

    cval = (L7_uchar8)val;
    cval2 = (L7_uchar8)val2;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS,
                                  excludeFlag, &cval, &cval2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on ACL layer 4 protocol field */
  if (aclIsFieldConfigured(aclNum, ruleNum, ACL_PROTOCOL) == L7_TRUE)
  {
    if (aclRuleProtocolGet(aclNum, ruleNum, &val) != L7_SUCCESS)
      return L7_FAILURE;
    val2 = 0;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on ACL source IP address and mask */
  if ((aclIsFieldConfigured(aclNum, ruleNum, ACL_SRCIP) == L7_TRUE) &&
      (aclIsFieldConfigured(aclNum, ruleNum, ACL_SRCIP_MASK) == L7_TRUE))
  {
    if (aclRuleSrcIpMaskGet(aclNum, ruleNum, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on ACL source IPv6 address (and prefix length) */
  if (aclIsFieldConfigured(aclNum, ruleNum, ACL_SRCIPV6) == L7_TRUE)
  {
    if (aclRuleSrcIpv6AddrGet(aclNum, ruleNum, &ipv6addr) != L7_SUCCESS)
      return L7_FAILURE;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6,
                                  excludeFlag, &ipv6addr.in6Addr, &ipv6addr.in6PrefixLen) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on ACL layer 4 source port number */
  if (aclIsFieldConfigured(aclNum, ruleNum, ACL_SRCPORT) == L7_TRUE)
  {
    /* NOTE: same value used for both start and end L4 port number */
    if (aclRuleSrcL4PortGet(aclNum, ruleNum, &val) != L7_SUCCESS)
      return L7_FAILURE;
    val2 = val;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on ACL layer 4 source port range */
  if ((aclIsFieldConfigured(aclNum, ruleNum, ACL_SRCSTARTPORT) == L7_TRUE) &&
      (aclIsFieldConfigured(aclNum, ruleNum, ACL_SRCENDPORT) == L7_TRUE))
  {
    if (aclRuleSrcL4PortRangeGet(aclNum, ruleNum, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  return L7_SUCCESS;

#else
  return L7_FAILURE;

#endif /* L7_QOS_FLEX_PACKAGE_ACL */
}

/*********************************************************************
* @purpose  Gather information from the MAC Access List Rule for use in
*           setting up a DiffServ class rule
*
* @param    pClass          @b{(input)} Class Table row pointer
* @param    aclNum          @b{(input)} MAC Access List number
* @param    ruleNum         @b{(input)} ACL Rule number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Does not follow the normal row creation process, but fills in the
*           class rule row objects directly.  This assumes that the ACL rule
*           is properly configured with valid information.
*
* @end
*********************************************************************/
L7_RC_t diffServAclMacClassRuleSetup(dsmibClassEntryCtrl_t *pClass,
                                     L7_uint32 aclNum,
                                     L7_uint32 ruleNum)
{
#ifdef L7_QOS_FLEX_PACKAGE_ACL

  dsmibClassAclType_t       aclType = L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_MAC;
  L7_uint32                 action;
  dsmibTruthValue_t         excludeFlag, matchEvery;
  L7_BOOL                   aclEveryFlag;
  L7_uint32                 val, val2;
  L7_uchar8                 macAddr[L7_MAC_ADDR_LEN];
  L7_uchar8                 macMask[L7_MAC_ADDR_LEN];
  L7_QOS_ETYPE_KEYID_t      etypeKeyid;

  if (aclMacRuleActionGet(aclNum, ruleNum, &action) != L7_SUCCESS)
    return L7_FAILURE;

  /* translate ACL Rule action into DiffServ rule exclude flag value */
  if (action == L7_ACL_PERMIT)
    excludeFlag = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE;
  else
    excludeFlag = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE;

  /* manually set up the fields in this class rule row using the valid fields
   * from the ACL Rule as indicated by the flags read earlier
   *
   * NOTE: Each group of DiffServ class rules belonging to the SAME
   *       ACL Rule is treated internally like a class type 'all'.  The
   *       collection of class rule groups, one per each ACL Rule, is
   *       handled internally like a class type 'any'.  Thus, a DiffServ
   *       class of type 'acl' is somewhat of a hybrid within the component.
   */

  /* check first for 'match every' ACL classifier */
  if (aclMacIsFieldConfigured(aclNum, ruleNum, ACL_MAC_EVERY) == L7_TRUE)
  {
    if (aclMacRuleEveryGet(aclNum, ruleNum, &aclEveryFlag) != L7_SUCCESS)
      return L7_FAILURE;
    /* translate ACL match every flag into DiffServ match every flag value */
    if (aclEveryFlag == L7_TRUE)
      matchEvery = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE;
    else
      matchEvery = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE;
    val2 = 0;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY,
                                  excludeFlag, &matchEvery, &val2) != L7_SUCCESS)
      return L7_FAILURE;

    /* no additional DiffServ rules to set up for 'match every' case */
    return L7_SUCCESS;
  }

  /* check for match on MAC ACL class-of-service */
  if (aclMacIsFieldConfigured(aclNum, ruleNum, ACL_MAC_COS) == L7_TRUE)
  {
    if (aclMacRuleCosGet(aclNum, ruleNum, &val) != L7_SUCCESS)
      return L7_FAILURE;
    val2 = 0;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on MAC ACL secondary class-of-service */
  if (aclMacIsFieldConfigured(aclNum, ruleNum, ACL_MAC_COS2) == L7_TRUE)
  {
    if (aclMacRuleCos2Get(aclNum, ruleNum, &val) != L7_SUCCESS)
      return L7_FAILURE;
    val2 = 0;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on MAC ACL destination MAC address and mask */
  if ((aclMacIsFieldConfigured(aclNum, ruleNum, ACL_MAC_DSTMAC) == L7_TRUE) &&
      (aclMacIsFieldConfigured(aclNum, ruleNum, ACL_MAC_DSTMAC_MASK) == L7_TRUE))
  {
    if (aclMacRuleDstMacGet(aclNum, ruleNum, macAddr, macMask) != L7_SUCCESS)
      return L7_FAILURE;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC,
                                  excludeFlag, macAddr, macMask) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on MAC ACL Ethertype keyid and possible custom value */
  if (aclMacIsFieldConfigured(aclNum, ruleNum, ACL_MAC_ETYPE_KEYID) == L7_TRUE)
  {
    if (aclMacRuleEtypeKeyGet(aclNum, ruleNum, &etypeKeyid, &val2) != L7_SUCCESS)
      return L7_FAILURE;

    if ((aclMacIsFieldConfigured(aclNum, ruleNum, ACL_MAC_ETYPE_VALUE) != L7_TRUE) ||
        (etypeKeyid != L7_QOS_ETYPE_KEYID_CUSTOM))
      val2 = 0;                                 /* only used for 'custom' keyid */

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE,
                                  excludeFlag, &etypeKeyid, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on MAC ACL source MAC address and mask */
  if ((aclMacIsFieldConfigured(aclNum, ruleNum, ACL_MAC_SRCMAC) == L7_TRUE) &&
      (aclMacIsFieldConfigured(aclNum, ruleNum, ACL_MAC_SRCMAC_MASK) == L7_TRUE))
  {
    if (aclMacRuleSrcMacGet(aclNum, ruleNum, macAddr, macMask) != L7_SUCCESS)
      return L7_FAILURE;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC,
                                  excludeFlag, macAddr, macMask) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on MAC ACL VLAN id value */
  if (aclMacIsFieldConfigured(aclNum, ruleNum, ACL_MAC_VLANID) == L7_TRUE)
  {
    /* NOTE: same value used for both start and end VLAN ID value */
    if (aclMacRuleVlanIdGet(aclNum, ruleNum, &val) != L7_SUCCESS)
      return L7_FAILURE;
    val2 = val;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on MAC ACL VLAN ID range */
  if ((aclMacIsFieldConfigured(aclNum, ruleNum, ACL_MAC_VLANID_START) == L7_TRUE) &&
      (aclMacIsFieldConfigured(aclNum, ruleNum, ACL_MAC_VLANID_END) == L7_TRUE))
  {
    if (aclMacRuleVlanIdRangeGet(aclNum, ruleNum, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on MAC ACL Secondary VLAN id value */
  if (aclMacIsFieldConfigured(aclNum, ruleNum, ACL_MAC_VLANID2) == L7_TRUE)
  {
    /* NOTE: same value used for both start and end VLAN ID2 value */
    if (aclMacRuleVlanId2Get(aclNum, ruleNum, &val) != L7_SUCCESS)
      return L7_FAILURE;
    val2 = val;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* check for match on MAC ACL Secondary VLAN ID range */
  if ((aclMacIsFieldConfigured(aclNum, ruleNum, ACL_MAC_VLANID2_START) == L7_TRUE) &&
      (aclMacIsFieldConfigured(aclNum, ruleNum, ACL_MAC_VLANID2_END) == L7_TRUE))
  {
    if (aclMacRuleVlanId2RangeGet(aclNum, ruleNum, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;

    if (diffServAclClassRuleBuild(pClass, aclType, aclNum, ruleNum,
                                  L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2,
                                  excludeFlag, &val, &val2) != L7_SUCCESS)
      return L7_FAILURE;
  }

  return L7_SUCCESS;

#else
  return L7_FAILURE;

#endif /* L7_QOS_FLEX_PACKAGE_ACL */
}

/*********************************************************************
* @purpose  Build a complete DiffServ class rule row for an ACL Rule
*
* @param    pClass          @b{(input)} Class Table row pointer
* @param    aclType         @b{(input)} Access List type
* @param    aclNum          @b{(input)} Access List number
* @param    ruleNum         @b{(input)} ACL Rule number
* @param    entryType       @b{(input)} Class rule entry type identifier
* @param    excludeFlag     @b{(input)} Class rule exclude flag value
* @param    *pVal           @b{(input)} row object value ptr
* @param    *pVal2          @b{(input)} row object value 2 ptr (if needed)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The val and val2 parameter usage is implied by the entryType.
*
* @end
*********************************************************************/
L7_RC_t diffServAclClassRuleBuild(dsmibClassEntryCtrl_t *pClass,
                                  dsmibClassAclType_t aclType,
                                  L7_uint32 aclNum,
                                  L7_uint32 ruleNum,
                                  dsmibClassRuleType_t entryType,
                                  dsmibTruthValue_t excludeFlag,
                                  void *pVal,
                                  void *pVal2)
{
#ifdef L7_QOS_FLEX_PACKAGE_ACL

  L7_uint32                 classIndex;
  dsmibClassRuleEntryCtrl_t *pRow;

  classIndex = pClass->key.classIndex;

  /* validate the ACL type and number */
  if (diffServAclNumCheckValid(aclType, aclNum) != L7_SUCCESS)
    return L7_FAILURE;

  /* create a class rule with normal default fields */
  if (dsmibClassRuleAclCreate(classIndex, &pRow) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "Number of fields in ACL rule exceeds DiffServ rules per class limit (%u)\n",
            (L7_uint32)L7_DIFFSERV_RULE_PER_CLASS_LIM);
    return L7_FAILURE;
  }

  /* set the DiffServ class rule ARID to this ACL rule number */
  pRow->arid = ruleNum;

  /* set the class rule row entry type and exclude flag */
  pRow->mib.entryType = entryType;
  pRow->mib.excludeFlag = excludeFlag;

  /* manually set up the unique fields in this class rule row according to the
   * class rule entry type
   *
   * NOTE: Not using the normal 'objectSet' function here so that it can
   *       prevent any attempt to set an ACL class rule object from the
   *       user interface.  Also want to skip the policy evaluation process
   *       until after the entire 'acl' class has been established and just
   *       do it once.
   */
  switch (entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS:
      pRow->mib.match.cos = *(L7_uint32 *)pVal;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2:
      pRow->mib.match.cos2 = *(L7_uint32 *)pVal;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
      pRow->mib.match.dstIp.addr = *(L7_uint32 *)pVal;
      pRow->mib.match.dstIp.mask = *(L7_uint32 *)pVal2;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
      memcpy(&pRow->mib.match.dstIpv6.ip6prefix.in6Addr, (L7_in6_addr_t *)pVal, sizeof(L7_in6_addr_t));
      pRow->mib.match.dstIpv6.ip6prefix.in6PrefixLen = *(L7_uint32 *)pVal2;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
      pRow->mib.match.dstL4Port.start = *(L7_uint32 *)pVal;
      pRow->mib.match.dstL4Port.end = *(L7_uint32 *)pVal2;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC:
      memcpy(pRow->mib.match.dstMac.addr, (L7_uchar8 *)pVal, (size_t)L7_MAC_ADDR_LEN);
      memcpy(pRow->mib.match.dstMac.mask, (L7_uchar8 *)pVal2, (size_t)L7_MAC_ADDR_LEN);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE:
      pRow->mib.match.etype.keyid = *(dsmibEtypeKeyid_t *)pVal;
      pRow->mib.match.etype.value = *(L7_uint32 *)pVal2;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY:
      pRow->mib.match.matchEvery = *(dsmibTruthValue_t *)pVal;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
      pRow->mib.match.ipv6FlowLabel = *(L7_uint32 *)pVal;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
      pRow->mib.match.ipDscp = *(L7_uint32 *)pVal;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
      pRow->mib.match.ipPrecedence = *(L7_uint32 *)pVal;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
      pRow->mib.match.ipTos.bits = *(L7_uchar8 *)pVal;
      pRow->mib.match.ipTos.mask = *(L7_uchar8 *)pVal2;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
      pRow->mib.match.protocolNum = *(L7_uint32 *)pVal;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
      pRow->mib.match.srcIp.addr = *(L7_uint32 *)pVal;
      pRow->mib.match.srcIp.mask = *(L7_uint32 *)pVal2;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
      memcpy(&pRow->mib.match.srcIpv6.ip6prefix.in6Addr, (L7_in6_addr_t *)pVal, sizeof(L7_in6_addr_t));
      pRow->mib.match.srcIpv6.ip6prefix.in6PrefixLen = *(L7_uint32 *)pVal2;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
      pRow->mib.match.srcL4Port.start = *(L7_uint32 *)pVal;
      pRow->mib.match.srcL4Port.end = *(L7_uint32 *)pVal2;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC:
      memcpy(pRow->mib.match.srcMac.addr, (L7_uchar8 *)pVal, (size_t)L7_MAC_ADDR_LEN);
      memcpy(pRow->mib.match.srcMac.mask, (L7_uchar8 *)pVal2, (size_t)L7_MAC_ADDR_LEN);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID:
      pRow->mib.match.vlanId.start = *(L7_uint32 *)pVal;
      pRow->mib.match.vlanId.end = *(L7_uint32 *)pVal2;
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2:
      pRow->mib.match.vlanId2.start = *(L7_uint32 *)pVal;
      pRow->mib.match.vlanId2.end = *(L7_uint32 *)pVal2;
      break;

    default:
      return L7_FAILURE;
      /*PASSTHRU*/

  } /* endswitch */

  /* update necessary fields in class rule row to make it complete */
  pRow->rowInvalidFlags = 0;
  pRow->mib.rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
  pRow->rowPending = L7_FALSE;

  /* successful row completion -- update next free index value
   * NOTE: Must do this under index semaphore control.
   */
  DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER); /* start critical sect */
  dsmibClassRuleIndexNextUpdate(classIndex);
  DIFFSERV_SEMA_GIVE(dsmibIndexSemId);                  /* end critical sect   */

  return L7_SUCCESS;

#else
  return L7_FAILURE;

#endif /* L7_QOS_FLEX_PACKAGE_ACL */
}

/*********************************************************************
* @purpose  Check if the specified Internal Interface Number is in
*           use by the ACL component
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ifDirection @b{(input)} Interface Direction
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL diffServIsAclIntfInUse(L7_uint32 intIfNum,
                               L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection)
{
#ifdef L7_QOS_FLEX_PACKAGE_ACL
  L7_uint32   dir;

  if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
    dir = L7_INBOUND_ACL;
  else if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT)
    dir = L7_OUTBOUND_ACL;
  else
    return L7_FALSE;

  return aclIsInterfaceInUse(intIfNum, dir);

#else
  return L7_FALSE;

#endif
}

/*********************************************************************
* @purpose  Determine if the interface is valid in diffServ
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL diffServIsValidIntfType(L7_uint32 sysIntfType)
{
  switch (sysIntfType)
  {
    case L7_PHYSICAL_INTF:
      return L7_TRUE;
      /*PASSTHRU*/

    case L7_LAG_INTF:
      /* check the feature support to ensure LAG interfaces are allowed */
      if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                L7_DIFFSERV_SERVICE_SUPPORTS_LAG_INTF_FEATURE_ID) == L7_TRUE)
      {
        return L7_TRUE;
      }
      break;

    default:
      break;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine if the interface is valid in diffServ
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL diffServIsValidIntf(L7_uint32 intIfNum)
{
  L7_uint32 sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    return diffServIsValidIntfType(sysIntfType);
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Determine if interface is in an attached state
*
* @param    intIfNum    @b{(input)}  internal interface number
* @param    *pIntfState @b{(output)} ptr to output location, or L7_NULLPTR
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments An 'attached' state by this definition is one of:
*           L7_INTF_ATTACHING, L7_INTF_ATTACHED, or L7_INTF_DETACHING.
*
* @comments Pass a non-null pointer for the pIntfState parameter
*           to retrieve the NIM interface state value used here.
*
* @end
*********************************************************************/
L7_BOOL diffServIsIntfAttached(L7_uint32 intIfNum, L7_INTF_STATES_t *pIntfState)
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

/*********************************************************************
* @purpose  Get the MAC Access List name for the specified ACL number
*
* @param    aclNum          @b{(input)} MAC Access List number
* @param    pName           @b{(output)} MAC ACL name pointer
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        ACL does not exist
* @returns  L7_FAILURE
*
* @notes    Caller must provide a name buffer of at least
*           (L7_USMDB_MIB_DIFFSERV_CLASS_ACL_NAME_MAX+1) characters.
*
* @end
*********************************************************************/
L7_RC_t diffServAclMacNameGet(L7_uint32 aclNum, L7_uchar8 *pName)
{
#ifdef L7_QOS_FLEX_PACKAGE_ACL

  #if (L7_USMDB_MIB_DIFFSERV_CLASS_ACL_NAME_MAX < L7_ACL_NAME_LEN_MAX)
  #error The L7_USMDB_MIB_DIFFSERV_CLASS_ACL_NAME_MAX definition is too low!
  #endif

  return aclMacNameGet(aclNum, pName);

#else
  return L7_FAILURE;

#endif /* L7_QOS_FLEX_PACKAGE_ACL */
}

/*********************************************************************
* @purpose  Query if the specified ACL number matches a configured
*           named IPv4 Access List
*
* @param    aclNum          @b{(input)} IP Access List number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        ACL does not exist
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t diffServIsAclNamedIpAcl(L7_uint32 aclNum)
{
#ifdef L7_QOS_FLEX_PACKAGE_ACL

  return aclNamedIndexCheckValid(L7_ACL_TYPE_IP, aclNum);

#else
  return L7_FAILURE;

#endif /* L7_QOS_FLEX_PACKAGE_ACL */
}
/*********************************************************************
* @purpose  Get the IP Access List name for the specified ACL number
*
* @param    aclNum          @b{(input)} IP Access List number
* @param    pName           @b{(output)} IP ACL name pointer
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        ACL does not exist
* @returns  L7_FAILURE
*
* @notes    Caller must provide a name buffer of at least
*           (L7_USMDB_MIB_DIFFSERV_CLASS_ACL_NAME_MAX+1) characters.
*
* @end
*********************************************************************/
L7_RC_t diffServAclIpNameGet(L7_uint32 aclNum, L7_uchar8 *pName)
{
#ifdef L7_QOS_FLEX_PACKAGE_ACL

  #if (L7_USMDB_MIB_DIFFSERV_CLASS_ACL_NAME_MAX < L7_ACL_NAME_LEN_MAX)
  #error The L7_USMDB_MIB_DIFFSERV_CLASS_ACL_NAME_MAX definition is too low!
  #endif

  return aclNameGet(aclNum, pName);

#else
  return L7_FAILURE;

#endif /* L7_QOS_FLEX_PACKAGE_ACL */
}

/*********************************************************************
*
* @purpose  Get the index number of a MAC access list, given its name.
*
* @param    pName      @b{(input)} access list name pointer
* @param    *aclIndex  @b{(output)} access list index pointer
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        ACL does not exist
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t diffServAclMacNameToIndex(L7_uchar8 *pName, L7_uint32 *pAclNum)
{
#ifdef L7_QOS_FLEX_PACKAGE_ACL

  return aclMacNameToIndex(pName, pAclNum);

#else
  return L7_FAILURE;

#endif /* L7_QOS_FLEX_PACKAGE_ACL */
}

/*********************************************************************
*
* @purpose  Get the index number of an access list, given its ACL type and name.
*
* @param    aclType     @b{(input)} access list type
* @param    *pName      @b{(input)} access list name pointer
* @param    *aclNum     @b{(output)} access list identifying number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    ACL does not exist
* @returns  L7_FAILURE
*
* @notes    Uses the ACL type and name to output the list index
*           number.  For IPV4 numbered access lists, the ACL name
*           is its number represented in string notation, e.g. "101".
*
* @end
*
*********************************************************************/
L7_RC_t diffServAclCommonNameToIndex(L7_ACL_TYPE_t aclType, L7_uchar8 *pName,
                                     L7_uint32 *pAclNum)
{
#ifdef L7_QOS_FLEX_PACKAGE_ACL

  return aclCommonNameToIndex(aclType, pName, pAclNum);

#else
  return L7_FAILURE;

#endif /* L7_QOS_FLEX_PACKAGE_ACL */
}

/*********************************************************************
*
* @purpose  Get a name string for any type of access list, given its index
*
* @param    aclnum      @b{(input)}  access list identifying number
* @param    *aclType    @b{(output)} access list type
* @param    *name       @b{(output)} access list name
*
* @returns  L7_SUCCESS  name is retrieved
* @returns  L7_FAILURE  invalid parms, or other failure
* @returns  L7_ERROR    access list does not exist
*
* @notes    Assumes caller provides a name buffer of at least
*           (L7_ACL_NAME_LEN_MAX+1) characters.
*
* @notes    Determines the ACL type from its index number and outputs
*           the list name.  For IPV4 numbered access lists, the
*           ACL number is output in string notation, e.g. "101".
*
* @end
*
*********************************************************************/
L7_RC_t diffServAclCommonNameStringGet(L7_uint32 aclnum, L7_ACL_TYPE_t *aclType,
                                       L7_uchar8 *name)
{
#ifdef L7_QOS_FLEX_PACKAGE_ACL

  return aclCommonNameStringGet(aclnum, aclType, name);

#else
  return L7_FAILURE;

#endif /* L7_QOS_FLEX_PACKAGE_ACL */
}
