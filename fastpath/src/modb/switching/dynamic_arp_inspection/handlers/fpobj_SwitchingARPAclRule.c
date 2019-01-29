
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingARPAclRule.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  20 May 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingARPAclRule_obj.h"
#include "usmdb_dai_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingARPAclRule_ARPACL
*
* @purpose Get 'ARPACL'
 *@description  [ARPACL] ARP ACL entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingARPAclRule_ARPACL (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objARPACLValue;
  xLibStr256_t nextObjARPACLValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ARPACL */
  memset(nextObjARPACLValue, 0x0, sizeof(nextObjARPACLValue));
  memset(objARPACLValue, 0x0, sizeof(objARPACLValue));
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingARPAclRule_ARPACL, (xLibU8_t *) objARPACLValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    memset(objARPACLValue, 0x0, sizeof(objARPACLValue));
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbArpAclNextGet(objARPACLValue, nextObjARPACLValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objARPACLValue, owa.len);
    owa.l7rc = usmDbArpAclNextGet(objARPACLValue, nextObjARPACLValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjARPACLValue, owa.len);

  /* return the object value: ARPACL */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjARPACLValue, strlen (nextObjARPACLValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingARPAclRule_RuleIP
*
* @purpose Get 'RuleIP'
 *@description  [RuleIP] Add IP of the Rule to ARP ACL   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingARPAclRule_RuleIP (void *wap, void *bufp)
{

  xLibStr256_t objARPACLValue, nextObjARPACLValue;
  xLibIpV4_t objRuleIPValue;
  xLibIpV4_t nextObjRuleIPValue;
  L7_enetMacAddr_t macAddr;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t ));
  FPOBJ_TRACE_ENTER (bufp);

  memset (objARPACLValue, 0, sizeof (objARPACLValue));
  memset (&objRuleIPValue, 0, sizeof (objRuleIPValue));

  memset (nextObjARPACLValue, 0, sizeof (nextObjARPACLValue));
  memset (&nextObjRuleIPValue, 0, sizeof (nextObjRuleIPValue));

  /* retrieve key: ARPACL */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingARPAclRule_ARPACL,
                          (xLibU8_t *) objARPACLValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objARPACLValue, owa.len);

  owa.len = sizeof(objRuleIPValue);
  /* retrieve key: RuleIP */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingARPAclRule_RuleIP,
                          (xLibU8_t *) & objRuleIPValue, &owa.len);

  memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbArpAclRuleNextGet(objARPACLValue,
                                     objRuleIPValue,
                                     &macAddr, nextObjARPACLValue, &nextObjRuleIPValue,
                                     &macAddr);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRuleIPValue, owa.len);
    do
    {
      owa.l7rc = usmDbArpAclRuleNextGet(objARPACLValue,
                                      objRuleIPValue,
                                      &macAddr, nextObjARPACLValue, &nextObjRuleIPValue,
                                      &macAddr);
    }
    while ((strcmp(objARPACLValue, nextObjARPACLValue) == 0)  && (objRuleIPValue == nextObjRuleIPValue)
           && (owa.l7rc == L7_SUCCESS));
  }

  if ((strcmp(objARPACLValue, nextObjARPACLValue) != 0)  || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjRuleIPValue, owa.len);

  /* return the object value: RuleIP */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjRuleIPValue, sizeof (nextObjRuleIPValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingARPAclRule_RuleMAC
*
* @purpose Get 'RuleMAC'
 *@description  [RuleMAC] Add MAC of the Rule to ARP ACL   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingARPAclRule_RuleMAC (void *wap, void *bufp)
{

  xLibStr256_t objARPACLValue;
  xLibStr256_t nextObjARPACLValue;
  xLibIpV4_t objRuleIPValue, nextObjRuleIPValue;
  xLibStr256_t objRuleMACValue;
  xLibStr256_t nextObjRuleMACValue;
  L7_enetMacAddr_t macAddr;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  FPOBJ_TRACE_ENTER (bufp);

  memset (objARPACLValue, 0, sizeof (objARPACLValue));
  memset (&objRuleIPValue, 0, sizeof (objRuleIPValue));
  memset (objRuleMACValue, 0, sizeof (objRuleMACValue));

  memset (nextObjARPACLValue, 0, sizeof (nextObjARPACLValue));
  memset (&nextObjRuleIPValue, 0, sizeof (nextObjRuleIPValue));
  memset (nextObjRuleMACValue, 0, sizeof (nextObjRuleMACValue));

  /* retrieve key: ARPACL */


  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingARPAclRule_ARPACL,
                          (xLibU8_t *) objARPACLValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, objARPACLValue, owa.len);

  owa.len = sizeof(objRuleIPValue);

  /* retrieve key: RuleIP */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingARPAclRule_RuleIP,
                          (xLibU8_t *) & objRuleIPValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objRuleIPValue, owa.len);

  owa.len = sizeof(objRuleMACValue);

  /* retrieve key: RuleMAC */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingARPAclRule_RuleMAC,
                          (xLibU8_t *) objRuleMACValue, &owa.len);

  memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (objRuleMACValue, 0, sizeof (objRuleMACValue));
    FPOBJ_CPY_IPV4(nextObjRuleIPValue,objRuleIPValue);
    FPOBJ_CPY_STR256(nextObjARPACLValue,objARPACLValue);

    owa.l7rc = usmDbArpAclRuleGet (objARPACLValue, objRuleIPValue, &macAddr);
    if(owa.l7rc != L7_SUCCESS)
    {
      owa.l7rc = usmDbArpAclRuleNextGet(objARPACLValue,
                                        objRuleIPValue,
                                        &macAddr, nextObjARPACLValue, &nextObjRuleIPValue,
                                        &macAddr);
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objRuleMACValue, owa.len);

    memcpy(macAddr.addr, objRuleMACValue, L7_ENET_MAC_ADDR_LEN);
    owa.l7rc = usmDbArpAclRuleNextGet(objARPACLValue,
                                      objRuleIPValue,
                                      &macAddr, nextObjARPACLValue, &nextObjRuleIPValue,
                                      &macAddr);

  }

  if ((strcmp(objARPACLValue, nextObjARPACLValue) != 0) || (objRuleIPValue != nextObjRuleIPValue) 
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RuleMAC */
  memcpy(nextObjRuleMACValue, macAddr.addr, L7_ENET_MAC_ADDR_LEN);

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjRuleMACValue, sizeof(nextObjRuleMACValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingARPAclRule_ROWSTATUS
*
* @purpose Get 'ROWSTATUS'
 *@description  [ROWSTATUS] Create and Delete an ARP ACL   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingARPAclRule_ROWSTATUS (void *wap, void *bufp)
{

  fpObjWa_t kwaARPACL = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyARPACLValue;
  fpObjWa_t kwaRuleIP = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t keyRuleIPValue;
  fpObjWa_t kwaRuleMAC = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyRuleMACValue;
  L7_enetMacAddr_t macAddr;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objROWSTATUSValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ARPACL */
  kwaARPACL.rc = xLibFilterGet (wap, XOBJ_SwitchingARPAclRule_ARPACL,
                                (xLibU8_t *) keyARPACLValue, &kwaARPACL.len);
  if (kwaARPACL.rc != XLIBRC_SUCCESS)
  {
    kwaARPACL.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaARPACL);
    return kwaARPACL.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyARPACLValue, kwaARPACL.len);

  /* retrieve key: RuleIP */
  kwaRuleIP.rc = xLibFilterGet (wap, XOBJ_SwitchingARPAclRule_RuleIP,
                                (xLibU8_t *) & keyRuleIPValue, &kwaRuleIP.len);
  if (kwaRuleIP.rc != XLIBRC_SUCCESS)
  {
    kwaRuleIP.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRuleIP);
    return kwaRuleIP.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRuleIPValue, kwaRuleIP.len);

  /* retrieve key: RuleMAC */
  kwaRuleMAC.rc = xLibFilterGet (wap, XOBJ_SwitchingARPAclRule_RuleMAC,
                                 (xLibU8_t *) keyRuleMACValue, &kwaRuleMAC.len);
  if (kwaRuleMAC.rc != XLIBRC_SUCCESS)
  {
    kwaRuleMAC.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaRuleMAC);
    return kwaRuleMAC.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyRuleMACValue, kwaRuleMAC.len);

  memcpy(macAddr.addr, keyRuleMACValue, L7_ENET_MAC_ADDR_LEN);

  /* get the value from application */
  owa.l7rc = usmDbArpAclRuleGet(keyARPACLValue,
                              keyRuleIPValue, &macAddr);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objROWSTATUSValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: ROWSTATUS */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objROWSTATUSValue, sizeof (objROWSTATUSValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingARPAclRule_ROWSTATUS
*
* @purpose Set 'ROWSTATUS'
 *@description  [ROWSTATUS] Create and Delete an ARP ACL   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingARPAclRule_ROWSTATUS (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objROWSTATUSValue;

  fpObjWa_t kwaARPACL = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyARPACLValue;
  fpObjWa_t kwaRuleIP = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t keyRuleIPValue;
  fpObjWa_t kwaRuleMAC = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyRuleMACValue;
  L7_enetMacAddr_t macAddr;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ROWSTATUS */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objROWSTATUSValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objROWSTATUSValue, owa.len);

  /* retrieve key: ARPACL */
  kwaARPACL.rc = xLibFilterGet (wap, XOBJ_SwitchingARPAclRule_ARPACL,
                                (xLibU8_t *) keyARPACLValue, &kwaARPACL.len);
  if (kwaARPACL.rc != XLIBRC_SUCCESS)
  {
    kwaARPACL.rc = XLIBRC_ARP_ACL_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaARPACL);
    return kwaARPACL.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyARPACLValue, kwaARPACL.len);

  /* retrieve key: RuleIP */
  kwaRuleIP.rc = xLibFilterGet (wap, XOBJ_SwitchingARPAclRule_RuleIP,
                                (xLibU8_t *) & keyRuleIPValue, &kwaRuleIP.len);
  if (kwaRuleIP.rc != XLIBRC_SUCCESS)
  {
    kwaRuleIP.rc = XLIBRC_IP_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaRuleIP);
    return kwaRuleIP.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRuleIPValue, kwaRuleIP.len);

  /* retrieve key: RuleMAC */
  kwaRuleMAC.rc = xLibFilterGet (wap, XOBJ_SwitchingARPAclRule_RuleMAC,
                                 (xLibU8_t *) keyRuleMACValue, &kwaRuleMAC.len);
  if (kwaRuleMAC.rc != XLIBRC_SUCCESS)
  {
    kwaRuleMAC.rc = XLIBRC_MAC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaRuleMAC);
    return kwaRuleMAC.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, keyRuleMACValue, kwaRuleMAC.len);

  memcpy(macAddr.addr, keyRuleMACValue, L7_ENET_MAC_ADDR_LEN);
  owa.l7rc = L7_SUCCESS;
  if (objROWSTATUSValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    owa.l7rc = usmDbArpAclRuleAdd(keyARPACLValue,
                                keyRuleIPValue, &macAddr);
      if (owa.l7rc != L7_SUCCESS)
      {
         owa.rc = XLIBRC_ACL_CREATE_FAILURE;    /* TODO: Change if required */
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
      }

  }
  else if (objROWSTATUSValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbArpAclRuleDelete(keyARPACLValue,
                                   keyRuleIPValue, &macAddr);
      if (owa.l7rc != L7_SUCCESS)
      {
         owa.rc = XLIBRC_ACL_DEL_FAILURE;    /* TODO: Change if required */
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
      }

  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
