/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/qos/diffserv/policy_attributes.c
 *
 * @purpose Code in support of the policy_attributes pages
 *
 * @component unitmgr
 *
 * @comments tba
 *
 * @create 07/30/2002
 *
 * @author jlshaw
 * @end
 *
 **********************************************************************/

#include "strlib_common_common.h"
#include "strlib_common_web.h"
#include "strlib_qos_common.h"
#include "strlib_qos_web.h"
#include "l7_common.h"
#include "diffserv_exports.h"
#include "usmdb_mib_diffserv_common.h"
#include "usmdb_mib_diffserv_private_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"
#include <stdio.h>
#include <string.h>
#include "ew_proto.h"
#include "ewnet.h"
#include "log.h"
#include "web.h"
#include "web_diffserv.h"
#include "usmdb_mib_diffserv_api.h"
#include "cli_web_util.h"
#include "web_oem.h"
#include "util_pstring.h"

static L7_char8 usmWebBigBuf[1024];

/*********************************************************************
*
* @purpose Get the Policy Attribute Color Name from the Color
*          Value configured.
*
* @param actionVal     L7_uint32
* @param *actionName   L7_char8
* @param stringMax     L7_char8
*
* @returns L7_SUCCESS
*          L7_ERROR   if no match
*
* @end
*
*********************************************************************/
L7_RC_t usmWebDiffServPolicyAttrColorNameFromColorVal(L7_uint32 colorVal, L7_char8 * colorName, L7_int32 stringMax)
{
  switch (colorVal)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND:
    (void)osapiStrncpySafe(colorName, pStrInfo_qos_ColorBlind, stringMax);
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
    (void)osapiStrncpySafe(colorName, pStrInfo_qos_ColorAwareCos, stringMax);
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
    (void)osapiStrncpySafe(colorName, pStrInfo_qos_ColorAwareSecCos, stringMax);
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
    (void)osapiStrncpySafe(colorName, pStrInfo_qos_ColorAwareIpDscp, stringMax);
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
    (void)osapiStrncpySafe(colorName, pStrInfo_qos_ColorAwareIpPrec, stringMax);
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED:
    (void)osapiStrncpySafe(colorName, pStrErr_qos_ColorModeAwareNotSpecified, stringMax);
    break;
  default:
    return L7_ERROR;
    break;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Policy Attribute Action Name from the Action
*          Value configured.
*
* @param actionVal     L7_uint32
* @param *actionName   L7_char8
* @param stringMax     L7_char8
*
* @returns L7_SUCCESS
*          L7_ERROR   if no match
*
* @end
*
*********************************************************************/
L7_RC_t usmWebDiffServPolicyAttrActionNameFromActionVal(L7_uint32 actionVal, L7_char8 * actionName, L7_int32 stringMax)
{
  switch (actionVal)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
    (void)osapiStrncpySafe(actionName, pStrInfo_qos_Drop, stringMax);
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
    (void)osapiStrncpySafe(actionName, pStrInfo_qos_MarkCos, stringMax);
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
    (void)osapiStrncpySafeAddBlanks (0, 0, 0, 1, L7_NULLPTR, actionName, pStrInfo_qos_MarkSecondaryCos, stringMax);
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
    (void)osapiStrncpySafe(actionName, pStrInfo_qos_MarkCosAsCos2, stringMax);  
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
    (void)osapiStrncpySafe(actionName, pStrInfo_qos_MarkIpDscp, stringMax);
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
    (void)osapiStrncpySafe(actionName, pStrInfo_qos_MarkIpPrecedence, stringMax);
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
    (void)osapiStrncpySafeAddBlanks (0, 0, 0, 1, L7_NULLPTR, actionName, pStrInfo_common_Send, stringMax);
    break;
  default:
    return L7_ERROR;
    break;
  }
  return L7_SUCCESS;
}
#if 0
/*********************************************************************
*
* @purpose Get the Policy Attribute Action select value from the Action
*          Value configured.
*
* @param actionVal     L7_uint32
* @param *selectedVal  L7_uint32
*
* @returns L7_SUCCESS
*          L7_ERROR   if no match
*
* @end
*
*********************************************************************/
L7_RC_t usmWebDiffServPolicyAttrActionSelectFromActionVal(L7_uint32 actionVal, L7_uint32 * selectedVal)
{
  switch (actionVal)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
    *selectedVal = drop;
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
    *selectedVal = mark_cos;
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
    *selectedVal = mark_cos2;
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
    *selectedVal = mark_ip_dscp;
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
    *selectedVal = mark_ip_prec;
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
    *selectedVal = sendd;
    break;
  default:
    return L7_ERROR;
    break;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Policy Attribute Action Value from the Action
*          selected in the HTML form.
*
* @param selectedVal  L7_uint32
* @param *actionVal   L7_uint32
*
* @returns L7_SUCCESS
*          L7_ERROR   if no match
*
* @end
*
*********************************************************************/
L7_RC_t usmWebDiffServPolicyAttrActionValFromActionSelect(L7_uint32 selectedVal, L7_uint32 * actionVal)
{
  switch (selectedVal)
  {
  case drop:
    *actionVal = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP;
    break;
  case mark_cos:
    *actionVal = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS;
    break;
  case mark_cos2:
    *actionVal = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2;
    break;
  case mark_ip_dscp:
    *actionVal = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP;
    break;
  case mark_ip_prec:
    *actionVal = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC;
    break;
  case sendd:
    *actionVal = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND;
    break;
  default:
    return L7_ERROR;
    break;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Policy Attribute selected DSCP value given the actual
*          DSCP Value configured.
*
* @param dscpVal        L7_uint32
* @param *selectedVal   L7_uint32
*
* @returns L7_SUCCESS
*          L7_ERROR   if no match
*
* @end
*
*********************************************************************/
L7_RC_t usmWebDiffServPolicyAttrSelectFromDscpVal(L7_uint32 dscpVal, L7_uint32 * selectedVal)
{
  switch (dscpVal)
  {
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF11: *selectedVal = af11;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF12: *selectedVal = af12;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF13: *selectedVal = af13;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF21: *selectedVal = af21;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF22: *selectedVal = af22;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF23: *selectedVal = af23;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF31: *selectedVal = af31;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF32: *selectedVal = af32;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF33: *selectedVal = af33;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF41: *selectedVal = af41;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF42: *selectedVal = af42;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF43: *selectedVal = af43;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_BE:   *selectedVal = be;
    break;
    /* case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS0: *selectedVal = cs0;
       break; */
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS1:  *selectedVal = cs1;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS2:  *selectedVal = cs2;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS3:  *selectedVal = cs3;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS4:  *selectedVal = cs4;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS5:  *selectedVal = cs5;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS6:  *selectedVal = cs6;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS7:  *selectedVal = cs7;
    break;
  case L7_USMDB_MIB_DIFFSERV_IP_DSCP_EF:   *selectedVal = ef;
    break;
  default: *selectedVal = 0;
    return L7_ERROR;
    break;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Get the Policy Attribute DSCP Value given the value selected
*          in the form.
*
* @param selectedVal    L7_uint32
* @param *dscpVal       L7_uint32
*
* @returns L7_SUCCESS
*          L7_ERROR   if "other" was selected, or no match
*
* @end
*
*********************************************************************/
L7_RC_t usmWebDiffServPolicyAttrDscpValFromSelect(L7_uint32 selectedVal, L7_uint32 * dscpVal)
{
  switch (selectedVal)
  {
  case af11: *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF11;
    break;
  case af12: *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF12;
    break;
  case af13: *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF13;
    break;
  case af21: *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF21;
    break;
  case af22: *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF22;
    break;
  case af23: *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF23;
    break;
  case af31: *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF31;
    break;
  case af32: *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF32;
    break;
  case af33: *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF33;
    break;
  case af41: *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF41;
    break;
  case af42: *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF42;
    break;
  case af43: *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF43;
    break;
  case be:   *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_BE;
    break;
  case cs0:  *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS0;
    break;
  case cs1:  *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS1;
    break;
  case cs2:  *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS2;
    break;
  case cs3:  *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS3;
    break;
  case cs4:  *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS4;
    break;
  case cs5:  *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS5;
    break;
  case cs6:  *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS6;
    break;
  case cs7:  *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS7;
    break;
  case ef:   *dscpVal = L7_USMDB_MIB_DIFFSERV_IP_DSCP_EF;
    break;
  default:   *dscpVal = 0;
    return L7_ERROR;
    break;
  }
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose Determine if any valid color classes exist for use with this
*          policy instance.
*
* @param unit        L7_uint32
* @param policyIndex L7_uint32
* @param classIndex  L7_uint32
*
* @returns L7_SUCCESS color classes exist
*          L7_ERROR   no color classes exist
*
* @end
*
*********************************************************************/
L7_RC_t usmWebDiffServPolicyAttrValidColorClassesExist(L7_uint32 unit, L7_uint32 policyIndex, L7_uint32 classIndex)
{
  L7_uint32 policyInstIndex, colorClassIndex;

  if (usmDbDiffServPolicyInstIndexFromClass(unit, policyIndex, classIndex, &policyInstIndex) == L7_SUCCESS)
  {
    /* check if at least one valid color class exists for this policy instance */
    colorClassIndex = 0;
    while (usmDbDiffServClassGetNext(unit, colorClassIndex, &colorClassIndex) == L7_SUCCESS)
    {
      if (usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(unit, policyIndex, policyInstIndex, colorClassIndex) == L7_SUCCESS)
      {
        return L7_SUCCESS;
      }
    }
  }

  return L7_ERROR;
}

/*********************************************************************
*
* @purpose Get the Policy Attribute Index corresponding to the
*          specified Entry Type.
*
* @param unit              L7_uint32
* @param policyIndex       L7_uint32
* @param policyInstIndex   L7_uint32
* @param entryType         L7_uint32
* @param *pPolicyAttrIndex L7_uint32
*
* @returns L7_SUCCESS
*          L7_ERROR   if no match
*
* @end
*
*********************************************************************/
L7_RC_t usmWebDiffServPolicyAttrIndexFromEntryTypeGet(L7_uint32 unit, L7_uint32 policyIndex, L7_uint32 policyInstIndex,
                                                      L7_uint32 entryType, L7_uint32 * pPolicyAttrIndex)
{
  L7_RC_t rc = L7_ERROR;
  L7_uint32 nextPolicyIndex, nextPolicyInstIndex, policyAttrIndex;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t attrEntryType;

  policyAttrIndex = 0;

  while (usmDbDiffServPolicyAttrGetNext(unit, policyIndex, policyInstIndex, policyAttrIndex,
                                        &nextPolicyIndex, &nextPolicyInstIndex, &policyAttrIndex) == L7_SUCCESS)
  {
    if ((nextPolicyIndex != policyIndex) || (nextPolicyInstIndex != policyInstIndex))
    {
      break;
    }

    if ((usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, policyIndex, policyInstIndex, policyAttrIndex,
                                                 &attrEntryType) == L7_SUCCESS) &&
        (attrEntryType == (L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t)entryType))
    {
      if (pPolicyAttrIndex != L7_NULLPTR)
      {
        *pPolicyAttrIndex = policyAttrIndex;
      }
      rc = L7_SUCCESS;
      break;
    }
  }

  return rc;
}
#endif
/*********************************************************************
*
* @purpose Get the Policy Attribute Fields for a policy attribute entry
*          Policy Name, Policy Type, Class Name, Attribute Type,
*          Attribute Details
*
* @param unit            L7_uint32  Unit Number
* @param policyAttr_itr  usmWeb_diffServPolicyAttrEntry_t
*
* @returns usmWebBigBuf with the attributes entry formatted as an HTML row
*
* @end
*
*********************************************************************/
L7_char8 *usmWebDiffServPolicyAttrInfoGet(L7_uint32 unit, usmWeb_diffServPolicyAttrEntry_t policyAttr_itr)
{
  L7_char8 buf[32];
  L7_char8 smallerBuf[256];
  L7_char8 strType[32];
  L7_char8 strClassName[32];
  L7_char8 strPolicyName[32];
  L7_char8 dscpString[16];
  L7_char8 stat[32];
  L7_uint32 classIndex;
  L7_uint32 policyId = policyAttr_itr.policyId;
  L7_uint32 policyInstId = policyAttr_itr.policyInstId;
  L7_uint32 policyAttrId = policyAttr_itr.policyAttrId;
  L7_uint32 entryType = policyAttr_itr.type;
  L7_uint32 val, idx, len;
  L7_uint32 u, s, p;
  L7_RC_t rc;
  L7_uint32 semiColonCounter=0;

  memset(usmWebBigBuf, 0, sizeof(usmWebBigBuf));
  memset(strClassName, 0, sizeof(strClassName));
  memset(strPolicyName, 0, sizeof(strPolicyName));
  len = (L7_uint32)sizeof(strPolicyName);
  usmDbDiffServPolicyNameGet(unit, policyId, strPolicyName, &len);

  if (usmDbDiffServPolicyTypeGet(unit, policyId, &val) == L7_SUCCESS)
  {
    switch (val)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
      (void)osapiSnprintf(strType, sizeof(strType), pStrInfo_common_In);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
      (void)osapiSnprintf(strType, sizeof(strType), pStrInfo_qos_Out);
      break;
    default:
      (void)osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, strType, sizeof(strType), pStrInfo_common_EmptyString);
      break;
    }
  }

  rc = usmDbDiffServPolicyInstClassIndexGet(unit, policyId, policyInstId, &classIndex);

  len = (L7_uint32)sizeof(strClassName);
  if (rc == L7_SUCCESS)
  {
    usmDbDiffServClassNameGet(unit, classIndex, strClassName, &len);

    (void)osapiSnprintf(usmWebBigBuf, sizeof(usmWebBigBuf),  "<tr> <td CLASS=\"tabledata\">%s</td> <td CLASS=\"tabledata\">%s</td> <td CLASS=\"tabledata\">%s</td>",
                        strPolicyName, strType, strClassName);
  }
   semiColonCounter=0; 
  switch (entryType)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
    (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "<td CLASS=\"tabledata\">%s</td><td CLASS=\"tabledata\">", pStrInfo_qos_PoliceTwoRate);
     OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);

    if(usmDbDiffServPolicyAttrStmtPoliceColorConformModeGet(unit, policyId, policyInstId, policyAttrId, &val) == L7_SUCCESS)
    {
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrColorNameFromColorVal(val, stat, sizeof(stat));
      if (val != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND)
      {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_qos_StringString, pStrInfo_qos_ColorConformMode, stat);
        semiColonCounter++;
      }
      else
      {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_qos_StringString, pStrInfo_qos_ColorConformMode, stat);
        semiColonCounter++;
      }
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
    }

    if(val != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND)
    {
      if (usmDbDiffServPolicyAttrStmtPoliceColorConformValGet(unit, policyId, policyInstId, policyAttrId, &val) == L7_SUCCESS)
      {
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %d", val);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);        
      }
      
      len = (L7_uint32)sizeof(strClassName);
      if ((usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet(unit, policyId, policyInstId, policyAttrId, &idx) == L7_SUCCESS) &&
          (usmDbDiffServClassNameGet(unit, idx, strClassName, &len) == L7_SUCCESS))
      {
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  " %s", strClassName);
      }
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if(usmDbDiffServPolicyAttrStmtPoliceColorExceedModeGet(unit, policyId, policyInstId, policyAttrId, &val) == L7_SUCCESS)
    {
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrColorNameFromColorVal(val, stat, sizeof(stat));
      if ((val != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND) && (val != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED))
      {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_qos_StringString, pStrInfo_qos_ColorExceedMode, stat);
      }
      else
      {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_qos_StringString, pStrInfo_qos_ColorExceedMode, stat);
      }
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if ((val != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND) && (val != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED))
    {
      if (usmDbDiffServPolicyAttrStmtPoliceColorExceedValGet(unit, policyId, policyInstId, policyAttrId,
                                                             &val) == L7_SUCCESS)
      {
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %d", val);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }

      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_common_Br_4);
      len = (L7_uint32)sizeof(strClassName);
      if ((usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexGet(unit, policyId, policyInstId, policyAttrId,
                                                                &idx) == L7_SUCCESS) &&
          (usmDbDiffServClassNameGet(unit, idx, strClassName, &len) == L7_SUCCESS))
      {
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %s", strClassName);
      }
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (usmDbDiffServPolicyAttrStmtPoliceTworateCrateGet(unit, policyId, policyInstId, policyAttrId,
                                                         &val) == L7_SUCCESS)
    {
      if(semiColonCounter > 0)
      {
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
       }

      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringDecimal, pStrInfo_qos_CommittedRateKbps, val); 
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (usmDbDiffServPolicyAttrStmtPoliceTworateCburstGet(unit, policyId, policyInstId, policyAttrId,
                                                          &val) == L7_SUCCESS)
    {
      if(semiColonCounter > 0)
      {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      }
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringDecimal, pStrInfo_qos_CommittedBurstSizeKb, val);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (usmDbDiffServPolicyAttrStmtPoliceTworatePrateGet(unit, policyId, policyInstId, policyAttrId,
                                                         &val) == L7_SUCCESS)
    {
      if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_qos_StringDecimal, pStrInfo_qos_PeakRateKbps, val);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (usmDbDiffServPolicyAttrStmtPoliceTworatePburstGet(unit, policyId, policyInstId, policyAttrId,
                                                          &val) == L7_SUCCESS)
    {if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_qos_StringDecimal, pStrInfo_qos_PeakBurstSizeKb, val);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (usmDbDiffServPolicyAttrStmtPoliceConformActGet(unit, policyId, policyInstId, policyAttrId,
                                                       &val) == L7_SUCCESS)
    {
      if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrActionNameFromActionVal(val, stat, sizeof(stat));
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_qos_StringString, pStrInfo_qos_ConformAction, stat);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if ((val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP)||(val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC))
    {
      if (usmDbDiffServPolicyAttrStmtPoliceConformValGet(unit, policyId, policyInstId, policyAttrId,
                                                         &val) == L7_SUCCESS)
      {
        memset(dscpString, 0, sizeof(dscpString));
        cliWebConvertDSCPValToString(val, dscpString);
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  " %s", dscpString);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }

    if (usmDbDiffServPolicyAttrStmtPoliceExceedActGet(unit, policyId, policyInstId, policyAttrId,
                                                      &val) == L7_SUCCESS)
    {
      if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrActionNameFromActionVal(val, stat, sizeof(stat));
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_qos_StringString, pStrInfo_qos_ExceedAction, stat);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if ((val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP)||(val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC))
    {
      
      if (usmDbDiffServPolicyAttrStmtPoliceExceedValGet(unit, policyId, policyInstId, policyAttrId, &val) == L7_SUCCESS)
      {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  " %s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
        memset(dscpString, 0, sizeof(dscpString));
        cliWebConvertDSCPValToString(val, dscpString);
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  " %s", dscpString);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }

    if ( usmDbDiffServPolicyAttrStmtPoliceNonconformActGet(unit, policyId, policyInstId, policyAttrId,
                                                           &val) == L7_SUCCESS)
    {
      if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrActionNameFromActionVal(val, stat, sizeof(stat));
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_qos_StringString, pStrInfo_qos_ViolateAction, stat);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if ((val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP)||(val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC))
    {
      if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(unit, policyId, policyInstId, policyAttrId,
                                                            &val) == L7_SUCCESS)
      {
        memset(dscpString, 0, sizeof(dscpString));
        cliWebConvertDSCPValToString(val, dscpString);
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %s", dscpString);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
    (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "<td CLASS=\"tabledata\">%s</td><td CLASS=\"tabledata\">", pStrInfo_qos_PoliceSingleRate);
    OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
    if (usmDbDiffServPolicyAttrStmtPoliceColorConformModeGet(unit, policyId, policyInstId, policyAttrId,
                                                             &val) == L7_SUCCESS)
    {
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrColorNameFromColorVal(val, stat, sizeof(stat));
      if (val != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND)
      {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_qos_StringString, pStrInfo_qos_ColorConformMode, stat);
      }
      else
      {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_qos_StringString, pStrInfo_qos_ColorConformMode, stat);
      }
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (val != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND)
    {
      if (usmDbDiffServPolicyAttrStmtPoliceColorConformValGet(unit, policyId, policyInstId, policyAttrId, &val) == L7_SUCCESS)
      {
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %d", val);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_common_Br_4);
      len = (L7_uint32)sizeof(strClassName);
      if ((usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet(unit, policyId, policyInstId, policyAttrId,
                                                                 &idx) == L7_SUCCESS) &&
          (usmDbDiffServClassNameGet(unit, idx, strClassName, &len) == L7_SUCCESS))
      {
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %s", strClassName);
      }
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (usmDbDiffServPolicyAttrStmtPoliceColorExceedModeGet(unit, policyId, policyInstId, policyAttrId,
                                                            &val) == L7_SUCCESS)
    {
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrColorNameFromColorVal(val, stat, sizeof(stat));
      if ((val != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND) && (val != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED))
      {

        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringString, pStrInfo_qos_ColorExceedMode, stat);
      }
      else
      {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringString, pStrInfo_qos_ColorExceedMode, stat);
      }
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if ((val != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND) && (val != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED))
    {
      if (usmDbDiffServPolicyAttrStmtPoliceColorExceedValGet(unit, policyId, policyInstId, policyAttrId,
                                                             &val) == L7_SUCCESS)
      {
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %d", val);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      }

      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_common_Br_4);
      len = (L7_uint32)sizeof(strClassName);
      if ((usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexGet(unit, policyId, policyInstId, policyAttrId,
                                                                &idx) == L7_SUCCESS) &&
          (usmDbDiffServClassNameGet(unit, idx, strClassName, &len) == L7_SUCCESS))
      {
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %s", strClassName);
      }
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCrateGet(unit, policyId, policyInstId, policyAttrId,
                                                            &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringDecimal, pStrInfo_qos_CommittedRateKbps, val);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCburstGet(unit, policyId, policyInstId, policyAttrId,
                                                             &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_qos_StringDecimal, pStrInfo_qos_CommittedBurstSizeKb, val);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (usmDbDiffServPolicyAttrStmtPoliceSinglerateEburstGet(unit, policyId, policyInstId, policyAttrId,
                                                             &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_qos_StringDecimal, pStrInfo_qos_ExcessBurstSizeKb, val);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (usmDbDiffServPolicyAttrStmtPoliceConformActGet(unit, policyId, policyInstId, policyAttrId,
                                                       &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrActionNameFromActionVal(val, stat, sizeof(stat));
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_qos_StringString, pStrInfo_qos_ConformAction, stat);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if ((val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP)||(val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC))
    {
      if (usmDbDiffServPolicyAttrStmtPoliceConformValGet(unit, policyId, policyInstId, policyAttrId,
                                                         &val) == L7_SUCCESS)
      {
        memset(dscpString, 0, sizeof(dscpString));
        cliWebConvertDSCPValToString(val, dscpString);
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %s", dscpString);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }

    if (usmDbDiffServPolicyAttrStmtPoliceExceedActGet(unit, policyId, policyInstId, policyAttrId,
                                                      &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrActionNameFromActionVal(val, stat, sizeof(stat));
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringString, pStrInfo_qos_ExceedAction, stat);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if ((val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP)||(val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC))
    {
      if (usmDbDiffServPolicyAttrStmtPoliceExceedValGet(unit, policyId, policyInstId, policyAttrId,
                                                        &val) == L7_SUCCESS)
      {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  " %s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
        memset(dscpString, 0, sizeof(dscpString));
        cliWebConvertDSCPValToString(val, dscpString);
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), "%s", dscpString);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }

    if ( usmDbDiffServPolicyAttrStmtPoliceNonconformActGet(unit, policyId, policyInstId, policyAttrId,
                                                           &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrActionNameFromActionVal(val, stat, sizeof(stat));
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_qos_StringString, pStrInfo_qos_ViolateAction, stat);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if ((val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP)||(val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC))
    {
      if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(unit, policyId, policyInstId, policyAttrId,
                                                            &val) == L7_SUCCESS)
      {
        memset(dscpString, 0, sizeof(dscpString));
        cliWebConvertDSCPValToString(val, dscpString);
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  " %s", dscpString);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }

    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
    (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "<td CLASS=\"tabledata\">%s</td><td CLASS=\"tabledata\">", pStrInfo_qos_PoliceSimple);
    OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);

    if (usmDbDiffServPolicyAttrStmtPoliceColorConformModeGet(unit, policyId, policyInstId, policyAttrId,
                                                             &val) == L7_SUCCESS)
    {
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrColorNameFromColorVal(val, stat, sizeof(stat));
      if (val != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND)
      {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringString, pStrInfo_qos_ColorConformMode, stat);
      }
      else
      {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringString, pStrInfo_qos_ColorConformMode, stat);
      }
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (val != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND)
    {
      if (usmDbDiffServPolicyAttrStmtPoliceColorConformValGet(unit, policyId, policyInstId, policyAttrId,
                                                              &val) == L7_SUCCESS)
      {
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %d", val);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }

      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  pStrInfo_common_Br_4);
      len = (L7_uint32)sizeof(strClassName);
      if ((usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet(unit, policyId, policyInstId, policyAttrId,
                                                                 &idx) == L7_SUCCESS) &&
          (usmDbDiffServClassNameGet(unit, idx, strClassName, &len) == L7_SUCCESS))
      {
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %s", strClassName);
      }
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (usmDbDiffServPolicyAttrStmtPoliceSimpleCrateGet(unit, policyId, policyInstId, policyAttrId,
                                                        &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringDecimal, pStrInfo_qos_CommittedRateKbps, val);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (usmDbDiffServPolicyAttrStmtPoliceSimpleCburstGet(unit, policyId, policyInstId, policyAttrId,
                                                         &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringDecimal, pStrInfo_qos_CommittedBurstSizeKb, val);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (usmDbDiffServPolicyAttrStmtPoliceConformActGet(unit, policyId, policyInstId, policyAttrId,
                                                       &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrActionNameFromActionVal(val, stat, sizeof(stat));
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringString, pStrInfo_qos_ConformAction, stat);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if ((val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP)||(val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC)|| (val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS)|| (val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2))
    {
      if (usmDbDiffServPolicyAttrStmtPoliceConformValGet(unit, policyId, policyInstId, policyAttrId,
                                                         &val) == L7_SUCCESS)
      {
        memset(dscpString, 0, sizeof(dscpString));
        cliWebConvertDSCPValToString(val, dscpString);
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %s", dscpString);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }

    if ( usmDbDiffServPolicyAttrStmtPoliceNonconformActGet(unit, policyId, policyInstId, policyAttrId,
                                                           &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrActionNameFromActionVal(val, stat, sizeof(stat));
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringString, pStrInfo_qos_ViolateAction, stat);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if ((val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP)||(val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC)|| (val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS)|| (val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2))
    {
      if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(unit, policyId, policyInstId, policyAttrId,
                                                            &val) == L7_SUCCESS)
      {
        memset(dscpString, 0, sizeof(dscpString));
        cliWebConvertDSCPValToString(val, dscpString);
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %s", dscpString);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }

    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
    (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), "<td CLASS=\"tabledata\">%s</td><td CLASS=\"tabledata\">", pStrInfo_qos_MarkIpDscp);
    OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
    if (usmDbDiffServPolicyAttrStmtMarkIpDscpValGet(unit,policyId, policyInstId, policyAttrId,
                                                    &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      memset(dscpString, 0, sizeof(dscpString));
      cliWebConvertDSCPValToString(val, dscpString);
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringString, pStrInfo_qos_DscpVal, dscpString);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
    (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), "<td CLASS=\"tabledata\">%s</td><td CLASS=\"tabledata\">", pStrInfo_qos_MarkIpPrecedence);
    OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
    if (usmDbDiffServPolicyAttrStmtMarkIpPrecedenceValGet(unit,policyId, policyInstId,policyAttrId,
                                                          &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringDecimal, pStrInfo_qos_IpPrecedenceVal, val);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
    (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), "<td CLASS=\"tabledata\">%s</td><td CLASS=\"tabledata\">", pStrInfo_qos_MarkCos);
    OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
    if (usmDbDiffServPolicyAttrStmtMarkCosValGet(unit,policyId, policyInstId, policyAttrId,
                                                 &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringDecimal, pStrInfo_qos_CosVal_1, val);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (usmDbDiffServPolicyAttrStmtPoliceConformActGet(unit, policyId, policyInstId, policyAttrId,
                                                       &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrActionNameFromActionVal(val, stat, sizeof(stat));
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringString, pStrInfo_qos_ConformAction, stat);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if ((val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS))
    {
      if(usmDbDiffServPolicyAttrStmtPoliceConformValGet(unit, policyId, policyInstId, policyAttrId,
                                                        &val) == L7_SUCCESS)
      {
        memset(dscpString, 0, sizeof(dscpString));
        cliWebConvertDSCPValToString(val, dscpString);
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %s", dscpString);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }

    if (usmDbDiffServPolicyAttrStmtPoliceExceedActGet(unit, policyId, policyInstId, policyAttrId,
                                                      &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrActionNameFromActionVal(val, stat, sizeof(stat));
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringString, pStrInfo_qos_ExceedAction, stat);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if ((val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS))
    {
      if (usmDbDiffServPolicyAttrStmtPoliceExceedValGet(unit, policyId, policyInstId, policyAttrId,
                                                        &val) == L7_SUCCESS)
      {
        memset(dscpString, 0, sizeof(dscpString));
        cliWebConvertDSCPValToString(val, dscpString);
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %s", dscpString);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }

    if(usmDbDiffServPolicyAttrStmtPoliceNonconformActGet(unit, policyId, policyInstId, policyAttrId,
                                                         &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrActionNameFromActionVal(val, stat, sizeof(stat));
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringString, pStrInfo_qos_ViolateAction, stat);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if ((val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP)||(val==L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC))
    {
      if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(unit, policyId, policyInstId, policyAttrId,
                                                            &val) == L7_SUCCESS)
      {
        memset(dscpString, 0, sizeof(dscpString));
        cliWebConvertDSCPValToString(val, dscpString);
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %s", dscpString);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
    (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), "<td CLASS=\"tabledata\">%s</td><td CLASS=\"tabledata\">",
                        pStrInfo_qos_MarkSecondaryCos);
    OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
    if (usmDbDiffServPolicyAttrStmtMarkCos2ValGet(unit,policyId, policyInstId, policyAttrId, &val) == L7_SUCCESS)
    {
        if(semiColonCounter > 0)
        {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        }
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringDecimal, pStrInfo_qos_SecondaryCosVal_1, val);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (usmDbDiffServPolicyAttrStmtPoliceConformActGet(unit, policyId, policyInstId, policyAttrId, &val) == L7_SUCCESS)
    {   
      if(semiColonCounter > 0)
      {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      }     
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrActionNameFromActionVal(val, stat, sizeof(stat));
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringString, pStrInfo_qos_ConformAction, stat);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (val == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2)
    {
      if (usmDbDiffServPolicyAttrStmtPoliceConformValGet(unit, policyId, policyInstId, policyAttrId,
                                                         &val) == L7_SUCCESS)
      {  
        memset(dscpString, 0, sizeof(dscpString));
        cliWebConvertDSCPValToString(val, dscpString);
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %s", dscpString);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }

    if (usmDbDiffServPolicyAttrStmtPoliceExceedActGet(unit, policyId, policyInstId, policyAttrId, &val) == L7_SUCCESS)
    {
      if(semiColonCounter > 0)
      {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      }
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrActionNameFromActionVal(val, stat, sizeof(stat));
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringString, pStrInfo_qos_ExceedAction, stat);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (val == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2)
    {
      if (usmDbDiffServPolicyAttrStmtPoliceExceedValGet(unit, policyId, policyInstId, policyAttrId, &val) == L7_SUCCESS)
      {  
        memset(dscpString, 0, sizeof(dscpString));
        cliWebConvertDSCPValToString(val, dscpString);
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %s", dscpString);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }

    if ( usmDbDiffServPolicyAttrStmtPoliceNonconformActGet(unit,
                                                           policyId, policyInstId, policyAttrId,
                                                           &val) == L7_SUCCESS)
    {  
      if(semiColonCounter > 0)
      {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      }
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrActionNameFromActionVal(val, stat, sizeof(stat));
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringString, pStrInfo_qos_ViolateAction, stat);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }

    if (val == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2)
    {
      if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(unit, policyId, policyInstId, policyAttrId,
                                                            &val) == L7_SUCCESS)
      {  
        memset(dscpString, 0, sizeof(dscpString));
        cliWebConvertDSCPValToString(val, dscpString);
        (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), " %s", dscpString);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }

    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
    (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), "<td CLASS=\"tabledata\">%s</td><td CLASS=\"tabledata\">", pStrInfo_qos_Drop);
    OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
    (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_Br_1, pStrInfo_common_PktsDiscarded);
    OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
    (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), "<td CLASS=\"tabledata\">%s</td><td CLASS=\"tabledata\">", pStrInfo_qos_MarkCosAsCos2);
    OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
    (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_Br_1, pStrInfo_qos_MarkCosAsCos2);
    OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
    (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf)-1, "<td CLASS=\"tabledata\">%s</td><td CLASS=\"tabledata\">",
                        pStrInfo_qos_MirrorIntf);
    OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
    if(usmDbDiffServPolicyAttrStmtMirrorIntfGet(unit,policyId, policyInstId, policyAttrId, &val) == L7_SUCCESS)
    {
      if(usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
      {  
      if(semiColonCounter > 0)
      {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      }
        osapiSnprintf(buf, sizeof(buf)-1, "%s", strUtilUspGet(unit, u, s, p, L7_NULLPTR, 0));
        osapiSnprintf(smallerBuf, sizeof(smallerBuf)-1, "%s : %s", pStrInfo_qos_MirrorIntf, buf);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
    (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), "<td CLASS=\"tabledata\">%s</td><td CLASS=\"tabledata\">",
                        pStrInfo_qos_RedirectIntf);
    OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
    if(usmDbDiffServPolicyAttrStmtRedirectIntfGet(unit,policyId, policyInstId, policyAttrId, &val) == L7_SUCCESS)
    {
      if(usmDbUnitSlotPortGet(val, &u, &s, &p) == L7_SUCCESS)
      {  
      if(semiColonCounter > 0)
      {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      }
        osapiSnprintf(buf, sizeof(buf)-1, "%s", strUtilUspGet(unit, u, s, p, L7_NULLPTR, 0));
        (void)osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringString, pStrInfo_qos_RedirectIntf, buf);
        OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
        semiColonCounter++;
      }
    }
    break;
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
    (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), "<td CLASS=\"tabledata\">%s</td><td CLASS=\"tabledata\">",
                        pStrInfo_qos_AsSignQueue);
    OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
    if (usmDbDiffServPolicyAttrStmtAssignQueueIdGet(unit,policyId, policyInstId, policyAttrId,
                                                    &val) == L7_SUCCESS)
    {  
      if(semiColonCounter > 0)
      {
          (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf),  "%s", "; ");
          OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      }
      osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_StringDecimal, pStrInfo_qos_AsSignQueue, val);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      semiColonCounter++;
    }
    break;
  default:
    if ((policyId != 0) && (policyAttrId == 0))
    {
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), "<td CLASS=\"tabledata\">%s</td><td CLASS=\"tabledata\">", pStrInfo_common_None_1);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
      (void)osapiSnprintf(smallerBuf, sizeof(smallerBuf), pStrInfo_qos_BestEffortWillBeUsed);
      OSAPI_STRNCAT(usmWebBigBuf, smallerBuf);
    }
    break;
  }
 
  OSAPI_STRNCAT(usmWebBigBuf, "</td></tr>");
  return usmWebBigBuf;
}
#if 0
/*********************************************************************
*
* @purpose Get all the valid color Classes in an options list drop down box
*
* @param context  EmWeb/Server request context handle
*
* @param optionp   Dynamic Select Support
*
* @param iterator pointer to the current iteration
*
* @param isExceed  Boolean (L7_FALSE=conform, L7_TRUE=exceed)
*
* @returns option
*
* @comments It is expected that a complete iteration for one list type
*           (conform or exceed) will occur before the other list type
*           so that the static variables are valid during their separate
*           list iterations.
*
* @end
*
*********************************************************************/
void *usmWebDiffServPolicyAttrColorClassListGenerate(EwsContext context, EwsFormSelectOptionP optionp, void * iterator, L7_BOOL isExceed )
{
  static L7_BOOL stringflag = L7_FALSE;
  static L7_BOOL displayNone = L7_FALSE;
  static L7_uint32 selected_ID;
  static L7_uint32 classIndex, policyIndex, policyInstIndex, policyAttrIndex;
  static L7_uint32 colorClassIndex;
  static L7_uint32 itrValid;
  static L7_BOOL lookForNextAdd;
  static L7_BOOL selectFirstClass = L7_FALSE;
  static L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t entryType;

  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 buf[APP_BUFFER_SIZE];
  L7_char8 * bufChoice;
  L7_uint32 last_add;
  L7_uint32 len;
  L7_uint32 unit;
  L7_BOOL haveSelectedValue = L7_FALSE;
  L7_RC_t rc;

  unit = usmDbThisUnitGet();
  itrValid = L7_FALSE;

  if (iterator == NULL)
  {
    selected_ID = L7_NULL;
    stringflag = L7_FALSE;
    colorClassIndex = 0;

    /* set flag to display <blank> option in class list */
    if (isExceed != L7_FALSE)
    {
      displayNone = L7_TRUE;
    }

    net = ewsContextNetHandle(context);
    if (net->app_pointer != NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      policyIndex = appInfo.data[0];
      classIndex = appInfo.data[2];
      entryType = (L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t)appInfo.data[3];

      if (usmDbDiffServPolicyInstIndexFromClass(unit, policyIndex, classIndex, &policyInstIndex) != L7_SUCCESS)
      {
        return NULL;
      }
      if (usmWebDiffServPolicyAttrIndexFromEntryTypeGet(unit, policyIndex, policyInstIndex, entryType,
                                                        &policyAttrIndex) != L7_SUCCESS)
      {
        policyAttrIndex = 0;
      }

      if (appInfo.data[7] == L7_TRUE)
      {
        if (isExceed == L7_FALSE)
        {
          selected_ID = appInfo.data[16];
        }
        else
        {
          selected_ID = appInfo.data[17];
        }
        haveSelectedValue = L7_TRUE;
      }
    }
    else
    {
      /* establish policyIndex, classIndex, and entryType from first available policy attribute */
      if (usmDbDiffServPolicyAttrGetNext(unit, 0, 0, 0, &policyIndex, &policyInstIndex, &policyAttrIndex) != L7_SUCCESS)
      {
        return NULL;
      }
      if (usmDbDiffServPolicyInstClassIndexGet(unit, policyIndex, policyInstIndex, &classIndex) != L7_SUCCESS)
      {
        return NULL;
      }
      if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, policyIndex, policyInstIndex,
                                                  policyAttrIndex, &entryType) != L7_SUCCESS)
      {
        return NULL;
      }
    }

    if (haveSelectedValue == L7_FALSE)
    {
      if (policyAttrIndex == 0)
      {
        rc = L7_ERROR;
      }
      else if (isExceed == L7_FALSE)
      {
        rc = usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet(unit, policyIndex, policyInstIndex, policyAttrIndex, &selected_ID);
      }
      else
      {
        rc = usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexGet(unit, policyIndex, policyInstIndex, policyAttrIndex, &selected_ID);
      }

      if (rc != L7_SUCCESS)
      {
        selected_ID = L7_NULL;
      }

      /* force color conform to select first class that it finds */
      if (selected_ID == L7_NULL)
      {
        if (isExceed == L7_FALSE)
        {
          selectFirstClass = L7_TRUE;
        }
      }
    }

    lookForNextAdd = L7_TRUE;
  }

  if (displayNone == L7_TRUE)
  {
    /*
     * Always give the blank entry option
     */
    displayNone = L7_FALSE;

    (void)osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, buf, sizeof(buf), pStrInfo_common_EmptyString);        /* (blank entry) */

    bufChoice = ewsContextNetHandle(context)->buffer;
    osapiStrncpySafe(bufChoice, buf, APP_BUFFER_SIZE);

    optionp->choice = bufChoice;
    optionp->valuep = (void *) &none_color_class_value;

    if (selected_ID == none_color_class_value)
    {
      optionp->selected = TRUE;
    }
    else
    {
      optionp->selected = FALSE;
    }
    return (void *) optionp;
  }

  while (lookForNextAdd == L7_TRUE)
  {                                       /* Get next, if it exists */
    if (usmDbDiffServClassGetNext(unit, colorClassIndex, &colorClassIndex) == L7_SUCCESS)
    {
      if (usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(unit, policyIndex, policyInstIndex, colorClassIndex) == L7_SUCCESS)
      {
        memset(buf, 0, sizeof(buf));
        len = sizeof(buf);
        if (usmDbDiffServClassNameGet(unit, colorClassIndex, buf, &len) == L7_SUCCESS)
        {
          bufChoice = ewsContextNetHandle(context)->buffer;
          osapiStrncpySafe(bufChoice, buf, len);

          optionp->choice = bufChoice;
          optionp->valuep = (void *) &colorClassIndex;
          if (selectFirstClass == L7_TRUE)
          {
            /* use the first class entry as the selected one */
            selectFirstClass = L7_FALSE;
            selected_ID = colorClassIndex;
          }
          if (colorClassIndex == selected_ID)
          {
            optionp->selected = TRUE;
          }
          else
          {
            optionp->selected = FALSE;
          }
          itrValid = L7_TRUE;
          last_add = colorClassIndex;
          break;
        }
      }
    }
    else
    {
      lookForNextAdd = L7_FALSE;
    }
  }

  if (itrValid == L7_TRUE)
  {
    return (void *) optionp;
  }
  else
  {
    return NULL;
  }
}
/*********************************************************************
*
* @purpose Get possible Attributes in an options list drop down box
*
* @param context  EmWeb/Server request context handle
*
* @param optionp   Dynamic Select Support
*
* @param iterator pointer to the current iteration
*
* @returns option
*
* @end
*
*********************************************************************/
void *ewaFormSelect_attribute_sel(EwsContext context, EwsFormSelectOptionP optionp, void * iterator )
{
  static L7_uint32 selected_ID;
  static L7_uint32 prevPolicyAttrId, staticVal;
  static L7_uint32 policyId, policyInstId, classId, nextPolicyId, nextPolicyInstId, nextPolicyAttrId;
  static L7_uint32 itrValid, entryListMax, tempPolicyIndex;
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  static L7_uint32 lookForNextAdd;
  L7_char8 buf[APP_BUFFER_SIZE];
  L7_char8 * bufChoice;
  L7_uint32 last_add, val, entryType;
  static L7_uint32 entryList[L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_TOTAL+1];
  static L7_uint32 i;
  static L7_BOOL stringflag = L7_FALSE;
  L7_BOOL attribCfg;
  L7_BOOL isFeature;
  L7_RC_t rc;
  L7_uint32 unit;

  itrValid = L7_FALSE;

  unit = usmDbThisUnitGet();
  if (iterator == NULL)
  {
    policyInstId = 0;
    classId = 0;
    selected_ID = 0;
    i=1;
    stringflag = L7_FALSE;
    net = ewsContextNetHandle(context);
    if (net->app_pointer != NULL)
    {
      memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
      policyId = appInfo.data[0];
      classId = appInfo.data[2];
    }
    else
    {
      if (usmDbDiffServPolicyGetNext(unit, 0, &policyId) != L7_SUCCESS)
      {
        return NULL;
      }
    }

    if (classId == 0)
    {
      tempPolicyIndex = policyId;
      if (usmDbDiffServPolicyInstGetNext(unit, policyId, policyInstId, &tempPolicyIndex, &policyInstId) == L7_SUCCESS)
      {
        if (policyId == tempPolicyIndex)
        {
          if (usmDbDiffServPolicyInstClassIndexGet(unit, policyId, policyInstId, &classId) != L7_SUCCESS)
          {
            classId = 0;
          }
        }
      }
    }
    else
    {
      rc = usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                                 &policyInstId);
    }

    if (usmDbDiffServPolicyTypeGet(unit, policyId, &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
        entryList[1] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE;
        entryList[2] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP;
        entryList[3] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL;
        entryList[4] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL;
        entryList[5] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL;
        entryList[6] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL;
        entryList[7] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR;
        entryList[8] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE;
        entryList[9] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
        entryList[10] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
        entryList[11] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT;
        entryList[12] = 0;
        entryListMax = 11;
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
        entryList[1] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE;
        entryList[2] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP;
        entryList[3] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL;
        entryList[4] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL;
        entryList[5] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL;
        entryList[6] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL;
        entryList[7] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR;
        entryList[8] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE;
        entryList[9] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
        entryList[10] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
        entryList[11] = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT;
        entryList[12] = 0;
        entryListMax = 11;
        break;
      default:
        break;
      }
    }
    else
    {
      return NULL;
    }

    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      return NULL;
    }

    lookForNextAdd = L7_TRUE;

    /* this is used to check if the blank option in the selector is to be
     *  chosen value this is only when there are no policy attributes configured for
     *  the particular policy*/
    attribCfg = L7_FALSE;
    if (usmDbDiffServPolicyAttrGetNext(unit, policyId, policyInstId,
                                       prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                       &nextPolicyAttrId) == L7_SUCCESS &&
        nextPolicyId == policyId &&
        nextPolicyInstId == policyInstId)
    {
      attribCfg = L7_TRUE;
    }

    bufChoice = ewsContextNetHandle(context)->buffer;
    memset(bufChoice, 0, APP_BUFFER_SIZE);
    staticVal = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
    optionp->choice = bufChoice;
    optionp->valuep = &staticVal;

    if (attribCfg == L7_FALSE)
    {
      l7flag = L7_FALSE;
      selected_ID = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
      optionp->selected = TRUE;
    }
    else
    {
      optionp->selected = FALSE;
    }

    return (void *) optionp;

  }
  while (lookForNextAdd == L7_TRUE)
  {
    prevPolicyAttrId = 0;

    /* Loop through all configured attributes for the selected Policy/Class combination
       (Policy Instance) looking for incompatibilities with the current entryList attribute.
       If there is an attribute that is already configured and is incompatible with the entryList attribute,
       skip to the next entryList attribute (not showing it in the select box and thus not allowing it to be
       configured by the user).
       And begin searching the Attribute Table beginning with the first entry of for the selected
       Policy Instance for incompatibilities with the new entryList attribute.

     */

    while (usmDbDiffServPolicyAttrGetNext(unit, policyId, policyInstId,
                                          prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                          &nextPolicyAttrId) == L7_SUCCESS &&
           nextPolicyId == policyId &&
           nextPolicyInstId == policyInstId)
    {
      usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId, nextPolicyInstId,
                                              nextPolicyAttrId, &entryType);
      prevPolicyAttrId = nextPolicyAttrId;

      attribCfg = L7_TRUE;
      selected_ID = entryList[i];

      switch (entryList[i])              /* check for incompatibilities */
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
        switch (entryType)
        {
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
          i++;
          prevPolicyAttrId = 0;
          break;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
        switch (entryType)
        {
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
          i++;
          prevPolicyAttrId = 0;
          break;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:       /* these don't have incompatibilities */
        switch (entryType)
        {
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
          i++;
          prevPolicyAttrId = 0;
          break;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
        switch (entryType)
        {
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
          i++;
          prevPolicyAttrId = 0;
          break;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
        switch (entryType)
        {
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
          i++;
          prevPolicyAttrId = 0;
          break;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
        switch (entryType)
        {
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
          i++;
          prevPolicyAttrId = 0;
          break;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
        switch (entryType)
        {
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
          i++;
          prevPolicyAttrId = 0;
          break;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
        switch (entryType)
        {
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
          i++;
          prevPolicyAttrId = 0;
          break;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
        switch (entryType)
        {
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
          i++;
          prevPolicyAttrId = 0;
          break;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
        switch (entryType)
        {
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
          i++;
          prevPolicyAttrId = 0;
          break;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
        switch (entryType)
        {
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
          i++;
          prevPolicyAttrId = 0;
          break;
        }
        break;

      default:
        return NULL;
        break;
      }
    }

    memset(buf, 0, sizeof(buf));
    isFeature = L7_FALSE;
    switch (entryList[i])
    {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
      isFeature = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                           L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID);
      (void)osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_MarkIpDscp);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
      isFeature = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                           L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID);
      (void)osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_MarkIpPrecedence);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      isFeature = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                           L7_DIFFSERV_POLICY_ATTR_POLICE_SIMPLE_FEATURE_ID);
      (void)osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_PoliceSimple);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      isFeature = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                           L7_DIFFSERV_POLICY_ATTR_POLICE_SINGLERATE_FEATURE_ID);
      (void)osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_PoliceSingleRate);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      isFeature = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                           L7_DIFFSERV_POLICY_ATTR_POLICE_TWORATE_FEATURE_ID);
      (void)osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_PoliceTwoRate);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
      isFeature = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                           L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID);
      (void)osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_MarkCos);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
      isFeature = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                           L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID);
      (void)osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_MarkSecondaryCos);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
      isFeature = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                           L7_DIFFSERV_POLICY_ATTR_DROP_FEATURE_ID);
      (void)osapiSnprintf(buf, sizeof(buf), pStrInfo_qos_Drop);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
      isFeature = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                           L7_DIFFSERV_POLICY_ATTR_MIRROR_FEATURE_ID);
      (void)osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_MirrorIntf);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
      isFeature = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                           L7_DIFFSERV_POLICY_ATTR_REDIRECT_FEATURE_ID);
      (void)osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_RedirectIntf);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
      isFeature = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                           L7_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_FEATURE_ID);
      (void)osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_qos_AsSignQueue);
      break;
    default:
      (void)osapiSnprintf(buf, sizeof(buf), pStrInfo_common_Error_5);
      break;
    }

    if (isFeature == L7_FALSE)
    {
      if (i == entryListMax)
      {
        lookForNextAdd = L7_FALSE;
      }
      last_add =  entryList[i++];
      continue;
    }

    bufChoice = ewsContextNetHandle(context)->buffer;
    osapiStrncpySafe(bufChoice, buf, APP_BUFFER_SIZE);

    optionp->choice = bufChoice;
    optionp->valuep = (void *) &entryList[i];

    if (entryList[i] == selected_ID)
    {
      l7flag = L7_FALSE;
      optionp->selected = TRUE;
    }
    else
    {
      optionp->selected = FALSE;
    }
    itrValid = L7_TRUE;
    if (i == entryListMax)
    {
      lookForNextAdd = L7_FALSE;
    }
    last_add =  entryList[i++];
    break;

  }
  if (itrValid == L7_TRUE)
  {
    return (void *) optionp;
  }
  else
  {
    return NULL;
  }
}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the diffserv_policy_def form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_diffserv_policy_def(EwsContext context, EwaForm_diffserv_policy_defP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 val;
  L7_char8 stat[80];
  L7_uint32 policyIndex, classIndex;
  L7_RC_t rc;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  net = ewsContextNetHandle(context);

  form->value.policy_sel2 |= (EW_FORM_INITIALIZED);
  form->status.err_flag |= (EW_FORM_INITIALIZED);

  form->value.policy_changed = 0;
  form->status.policy_changed |= (EW_FORM_INITIALIZED);

  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    if (form->value.err_flag != 0)
    {
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
    else
    {
      form->value.err_msg = NULL;
      form->status.err_msg |= (EW_FORM_INITIALIZED);
    }
    policyIndex = appInfo.data[0];
    classIndex = appInfo.data[2];
  }
  else         /* == null */
  {

    rc = usmDbDiffServPolicyGetNext(unit, 0, &policyIndex);

    form->value.err_flag = L7_FALSE;
    form->value.err_msg = NULL;
    form->status.err_msg |= (EW_FORM_INITIALIZED);
  }

  memset(stat, 0, sizeof(stat));
  if (usmDbDiffServPolicyTypeGet(unit, policyIndex, &val) == L7_SUCCESS)
  {
    switch (val)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
      (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_common_In);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
      (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_qos_Out);
      break;
    default:
      (void)osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_EmptyString);
      break;
    }
    form->value.policy_type = osapiStrDup(stat);
    form->status.policy_type |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  }

  form->status.class_member_sel |= (EW_FORM_INITIALIZED);
  form->status.attribute_sel |= (EW_FORM_INITIALIZED);

  form->value.Submit = (char *) osapiStrDup( pStrInfo_qos_CfgureSelectedAttr);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the diffserv_policy_def form
*
* @returns diffserv_policy_def.html
*
* @end
*
*********************************************************************/
L7_char8 *ewaFormSubmit_diffserv_policy_def( EwsContext context, EwaForm_diffserv_policy_defP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 pageToLoad[USMWEB_FILENAME_SIZE];
  L7_uint32 val;
  L7_uint32 policyID, classID, policyInstId;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  memset(&pageToLoad, 0, sizeof(pageToLoad));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  appInfo.data[0] = form->value.policy_sel2;
  appInfo.data[2] = 0;
  /* If a new policy was selected, get the first class for that policy */
  /* If a new policy was selected, get the first class for that policy */
  if (form->value.policy_changed == 1)
  {
    policyID = form->value.policy_sel2;
    policyInstId = 0;
    if (usmDbDiffServPolicyInstGetNext(unit, policyID, policyInstId, &policyID, &policyInstId) == L7_SUCCESS)
    {
      if (policyID == form->value.policy_sel2)
      {
        if (usmDbDiffServPolicyInstClassIndexGet(unit, policyID, policyInstId, &classID) == L7_SUCCESS)
        {
          appInfo.data[2] = classID;
        }
      }
    }
  }
  else
  {
    appInfo.data[2] = form->value.class_member_sel;
  }

  if (form->status.Submit & EW_FORM_RETURNED)
  {
    appInfo.data[3] = form->value.attribute_sel;
    switch (form->value.attribute_sel)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE:
      appInfo.err.err_flag = L7_TRUE;
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, appInfo.err.msg, pStrErr_qos_PolicyAttrSelector);
      OSAPI_STRNCPY_SAFE(pageToLoad, pStrInfo_qos_HtmlFileDiffservPolicyDef);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      OSAPI_STRNCPY_SAFE(pageToLoad, pStrInfo_qos_HtmlFilePolicingAttrs);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
      appInfo.data[5] = L7_FALSE;
      OSAPI_STRNCPY_SAFE(pageToLoad, pStrInfo_qos_HtmlFileMarkIpDscp);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
      OSAPI_STRNCPY_SAFE(pageToLoad, pStrInfo_qos_HtmlFileMarkIpPrec);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
      OSAPI_STRNCPY_SAFE(pageToLoad, pStrInfo_qos_HtmlFileMarkCos);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
      OSAPI_STRNCPY_SAFE(pageToLoad, pStrInfo_qos_HtmlFileMarkCos2);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
      OSAPI_STRNCPY_SAFE(pageToLoad, pStrInfo_qos_HtmlFileDrop);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
      OSAPI_STRNCPY_SAFE(pageToLoad, pStrInfo_qos_HtmlFileAssignQueue);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
      OSAPI_STRNCPY_SAFE(pageToLoad, pStrInfo_qos_HtmlFileMirrorIntf);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
      OSAPI_STRNCPY_SAFE(pageToLoad, pStrInfo_qos_HtmlFileRedirectIntf);
      break;
    default:
      OSAPI_STRNCPY_SAFE(pageToLoad, pStrInfo_qos_HtmlFileDiffservPolicyDef);
      break;
    }

    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pageToLoad);

  }

  if (usmDbDiffServPolicyTypeGet(unit, form->value.policy_sel2, &val) == L7_SUCCESS)
  {
    switch (val)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
      appInfo.data[1] = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
      appInfo.data[1] = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT;
      break;
    default:
      break;
    }
  }

  return usmWebEwsContextSendReply (context, L7_TRUE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyDef);
}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the policing_attributes form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_policing_attributes(EwsContext context, EwaForm_policing_attributesP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 val, selectVal, entryType;
  L7_RC_t rc;
  L7_char8 stat[80];
  L7_uint32 len = 32;
  L7_uint32 policyId, classId, policyInstId,
            prevPolicyId, prevPolicyInstId, prevPolicyAttrId,
            nextPolicyId, nextPolicyInstId, nextPolicyAttrId;
  L7_uint32 unit;
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_t colorMode;

  unit = usmDbThisUnitGet();

  net = ewsContextNetHandle(context);

  form->status.err_flag |= (EW_FORM_INITIALIZED);

  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    if (form->value.err_flag != 0)
    {
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
    else
    {
      form->value.err_msg = NULL;
      form->status.err_msg |= (EW_FORM_INITIALIZED);
    }

    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyNameGet(unit, appInfo.data[0], stat, &len) == L7_SUCCESS)
    {
      form->value.policy_name = osapiStrDup(stat);
      form->status.policy_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyTypeGet(unit, appInfo.data[0], &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_common_In);
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_qos_Out);
        break;
      default:
        (void)osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_EmptyString);
        break;
      }
      form->value.policy_type = osapiStrDup(stat);
      form->status.policy_type |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    len = 32;
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServClassNameGet(unit, appInfo.data[2], stat, &len) == L7_SUCCESS)
    {
      form->value.class_name = osapiStrDup(stat);
      form->status.class_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    memset(stat, 0, sizeof(stat));
    switch (appInfo.data[3])
    {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_qos_PoliceSimple);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_qos_PoliceSingleRate);
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_qos_PoliceTwoRate);
      break;
    default:
      break;
    }

    form->value.style_uint = appInfo.data[3];
    form->status.style_uint |= (EW_FORM_INITIALIZED);
    form->value.policing_style = osapiStrDup(stat);
    form->status.policing_style |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

    policyId = appInfo.data[0];
    classId = appInfo.data[2];

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                        prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                        &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       needs to be created or modified */
    while (rc != L7_ERROR)
    {
      if (rc == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId, nextPolicyAttrId,
                                                      &entryType) == L7_SUCCESS)
          {
            /* check if attribute is already specified */
            if ( entryType == appInfo.data[3])
            {
              form->value.attribute_exists = L7_TRUE;
              form->status.attribute_exists |= (EW_FORM_INITIALIZED);
              form->value.attribute_id = nextPolicyAttrId;
              form->status.attribute_id |= (EW_FORM_INITIALIZED);

              /* get the current value(s) */
              if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                           L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID) == L7_TRUE)
              {
                if (usmDbDiffServPolicyAttrStmtPoliceColorConformModeGet(unit, nextPolicyId, nextPolicyInstId,
                                                                         nextPolicyAttrId, &colorMode) == L7_SUCCESS)
                {
                  if (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND)
                  {
                    form->value.color_mode = clr_blind;
                  }
                  else
                  {
                    form->value.color_mode = clr_aware;
                  }
                  form->status.color_mode |= (EW_FORM_INITIALIZED);
                }
              }
              if (usmDbDiffServPolicyAttrStmtPoliceExceedActGet(unit, nextPolicyId, nextPolicyInstId,
                                                                nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                if (usmWebDiffServPolicyAttrActionSelectFromActionVal(val, &selectVal) == L7_SUCCESS)
                {
                  form->value.exceed_action_sel = selectVal;
                  form->status.exceed_action_sel |= (EW_FORM_INITIALIZED);
                }
              }
              if (usmDbDiffServPolicyAttrStmtPoliceConformActGet(unit, nextPolicyId, nextPolicyInstId,
                                                                 nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                if (usmWebDiffServPolicyAttrActionSelectFromActionVal(val, &selectVal) == L7_SUCCESS)
                {
                  form->value.conform_action_sel = selectVal;
                  form->status.conform_action_sel |= (EW_FORM_INITIALIZED);
                }
              }
              if (usmDbDiffServPolicyAttrStmtPoliceNonconformActGet(unit, nextPolicyId, nextPolicyInstId,
                                                                    nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                if (usmWebDiffServPolicyAttrActionSelectFromActionVal(val, &selectVal) == L7_SUCCESS)
                {
                  form->value.nonconform_action_sel = selectVal;
                  form->status.nonconform_action_sel |= (EW_FORM_INITIALIZED);
                }
              }
            }
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */
          prevPolicyAttrId = nextPolicyAttrId;

        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                            prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                            &nextPolicyAttrId);
      }
    }

  }
  else
  {
    form->value.err_flag = L7_FALSE;
    form->value.err_msg = NULL;
    form->status.err_msg |= (EW_FORM_INITIALIZED);
  }

  form->value.Submit = (char *) osapiStrDup( pStrInfo_qos_Confirm);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the policing_attributes form
*
* @returns policing_attributes.html
*
* @end
*
*********************************************************************/
L7_char8 *ewaFormSubmit_policing_attributes( EwsContext context, EwaForm_policing_attributesP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 pageToLoad[USMWEB_FILENAME_SIZE];
  L7_uint32 val;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  memset(&pageToLoad, 0, sizeof(pageToLoad));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  usmDbDiffServPolicyNameToIndex(unit, form->value.policy_name, &val);
  appInfo.data[0] = val;

  usmDbDiffServClassNameToIndex(unit, form->value.class_name, &val);
  appInfo.data[2] = val;

  if (form->status.Submit & EW_FORM_RETURNED)
  {
    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID) == L7_TRUE)
    {
      if (form->value.color_mode == clr_aware)
      {
        if (usmWebDiffServPolicyAttrValidColorClassesExist(unit, appInfo.data[0], appInfo.data[2]) != L7_SUCCESS)
        {
          appInfo.err.err_flag = L7_TRUE;
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, appInfo.err.msg, pStrInfo_qos_ColorAwareModeRequiresExistenceOfOneOrMoreColorClassesThatAreValidForUseWithPolicyInstPressHelpForMoreDetails);
          appInfo.data[3] = form->value.style_uint;
          return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFilePolicingAttrs);
        }
        appInfo.data[18] = 2;    /* color aware */
      }
      else
      {
        appInfo.data[18] = 1;    /* color blind */
      }
    }

    if (usmWebDiffServPolicyAttrActionValFromActionSelect(form->value.conform_action_sel, &val) != L7_SUCCESS)
    {
      appInfo.data[4] = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP;
    }
    else
    {
      precflag = L7_FALSE;
      appInfo.data[4] = val;
      if((appInfo.data[4] ==  L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC && tempAppInfo == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP)
         || (appInfo.data[4] == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP && tempAppInfo == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC))
      {
        precflag = L7_TRUE;
      }

    }
    tempAppInfo = appInfo.data[4];

    if (form->value.style_uint == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE ||
        form->value.style_uint == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
    {
      if (usmWebDiffServPolicyAttrActionValFromActionSelect(form->value.exceed_action_sel, &val) != L7_SUCCESS)
      {
        appInfo.data[5] = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP;
      }
      else
      {
        appInfo.data[5] = val;
      }
    }

    if (usmWebDiffServPolicyAttrActionValFromActionSelect(form->value.nonconform_action_sel, &val) != L7_SUCCESS)
    {
      appInfo.data[6] = L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP;
    }
    else
    {
      appInfo.data[6] = val;
    }

    appInfo.data[3] = form->value.style_uint;
    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFilePolicingCfg);

  }
  else if (form->status.Cancel & EW_FORM_RETURNED)
  {
    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyDef);
  }

  return usmWebEwsContextSendReply (context, L7_TRUE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyDef);
}

/*********************************************************************
*
* @purpose Get all the valid color conform Classes in an options list drop down box
*
* @param context  EmWeb/Server request context handle
*
* @param optionp   Dynamic Select Support
*
* @param iterator pointer to the current iteration
*
* @returns option
*
* @end
*
*********************************************************************/
void *ewaFormSelect_color_conform_class(EwsContext context, EwsFormSelectOptionP optionp, void * iterator )
{
  return usmWebDiffServPolicyAttrColorClassListGenerate(context, optionp, iterator, L7_FALSE);
}

/*********************************************************************
*
* @purpose Get all the valid color exceed Classes in an options list drop down box
*
* @param context  EmWeb/Server request context handle
*
* @param optionp   Dynamic Select Support
*
* @param iterator pointer to the current iteration
*
* @returns option
*
* @end
*
*********************************************************************/
void *ewaFormSelect_color_exceed_class(EwsContext context, EwsFormSelectOptionP optionp, void * iterator )
{
  return usmWebDiffServPolicyAttrColorClassListGenerate(context, optionp, iterator, L7_TRUE);
}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the policing_cfg form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_policing_cfg(EwsContext context, EwaForm_policing_cfgP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 val, selectVal, entryType;
  L7_RC_t rc;
  L7_char8 buf[128];
  L7_char8 stat[80];
  L7_uint32 len;
  L7_uint32 policyId, classId, policyInstId,
            prevPolicyId, prevPolicyInstId, prevPolicyAttrId,
            nextPolicyId, nextPolicyInstId, nextPolicyAttrId;
  L7_uint32 unit;
  L7_uint32 colorConformClassId = 0, colorExceedClassId = 0;
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_t colorConformMode, colorExceedMode;
  L7_uint32 colorConformVal = 0, colorExceedVal = 0;
  L7_uint32 tempId;
  L7_BOOL colorAwareFeaturePresent = L7_FALSE;
  L7_BOOL singleOrTwoRatePolicing = L7_FALSE;
  unit = usmDbThisUnitGet();
  net = ewsContextNetHandle(context);

  form->value.attribute_exists = L7_FALSE;
  form->status.attribute_exists |= (EW_FORM_INITIALIZED);
  form->status.err_flag |= (EW_FORM_INITIALIZED);

  /* insert field ranges next to the each input field */
  memset(stat, 0, sizeof(stat));
  (void)osapiSnprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_Kbytes, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MIN,
                                pStrInfo_common_To_5, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MAX);
  form->value.committed_burst_size_limits = osapiStrDup(stat);
  form->status.committed_burst_size_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  memset(stat, 0, sizeof(stat));


  (void)osapiSnprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_Kbps, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CRATE_KBPS_MIN,
                                pStrInfo_common_To_5, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CRATE_KBPS_MAX);

  form->value.committed_rate_limits = osapiStrDup(stat);
  form->status.committed_rate_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  memset(stat, 0, sizeof(stat));
  (void)osapiSnprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_Kbytes, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_EBURST_MIN,
                                pStrInfo_common_To_5, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_EBURST_MAX);
  form->value.excess_burst_size_limits = osapiStrDup(stat);
  form->status.excess_burst_size_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  memset(stat, 0, sizeof(stat));
  (void)osapiSnprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_Kbps, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PRATE_KBPS_MIN,
                                pStrInfo_common_To_5, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PRATE_KBPS_MAX);


  form->value.peak_rate_limits = osapiStrDup(stat);
  form->status.peak_rate_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  memset(stat, 0, sizeof(stat));
  (void)osapiSnprintfAddBlanks (0, 0, 1, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_qos_Kbytes, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PBURST_MIN,
                                pStrInfo_common_To_5, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PBURST_MAX);
  form->value.peak_burst_size_limits = osapiStrDup(stat);
  form->status.peak_burst_size_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  memset(stat, 0, sizeof(stat));
  (void)osapiSnprintf(stat, sizeof(stat), " (%d %s %u)", L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN,
                      pStrInfo_common_To_5, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX);
  form->value.conform_dscp_val_limits = osapiStrDup(stat);
  form->status.conform_dscp_val_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->value.exceed_dscp_val_limits = osapiStrDup(stat);
  form->status.exceed_dscp_val_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->value.nonconform_dscp_val_limits = osapiStrDup(stat);
  form->status.nonconform_dscp_val_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  memset(stat, 0, sizeof(stat));
  (void)osapiSnprintf(stat, sizeof(stat), " (%d %s %u)", L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN,
                      pStrInfo_common_To_5, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX);
  form->value.conform_ip_prec_val_limits = osapiStrDup(stat);
  form->status.conform_ip_prec_val_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->value.exceed_ip_prec_val_limits = osapiStrDup(stat);
  form->status.exceed_ip_prec_val_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->value.nonconform_ip_prec_val_limits = osapiStrDup(stat);
  form->status.nonconform_ip_prec_val_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  memset(stat, 0, sizeof(stat));
  (void)osapiSnprintf(stat, sizeof(stat), " (%d %s %u)", L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN,
                      pStrInfo_common_To_5, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX);

  form->value.conform_cos_val_limits = osapiStrDup(stat);
  form->status.conform_cos_val_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->value.nonconform_cos_val_limits = osapiStrDup(stat);
  form->status.nonconform_cos_val_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->value.exceed_cos_val_limits = osapiStrDup(stat);
  form->status.exceed_cos_val_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  memset(stat, 0, sizeof(stat));
  (void)osapiSnprintf(stat, sizeof(stat), " (%d %s %u)", L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN,
                      pStrInfo_common_To_5, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX);

  form->value.conform_cos2_val_limits = osapiStrDup(stat);
  form->status.conform_cos2_val_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->value.nonconform_cos2_val_limits = osapiStrDup(stat);
  form->status.nonconform_cos2_val_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->value.exceed_cos2_val_limits = osapiStrDup(stat);
  form->status.exceed_cos2_val_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  /* done inserting field ranges */

  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    if (form->value.err_flag != 0)
    {
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
    else
    {
      form->value.err_msg = NULL;
      form->status.err_msg |= (EW_FORM_INITIALIZED);
    }

    memset(stat, 0, sizeof(stat));
    len = (L7_uint32)sizeof(stat);
    if (usmDbDiffServPolicyNameGet(unit, appInfo.data[0], stat, &len) == L7_SUCCESS)
    {
      form->value.policy_name = osapiStrDup(stat);
      form->status.policy_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyTypeGet(unit, appInfo.data[0], &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_common_In);
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_qos_Out);
        break;
      default:
        (void)osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_EmptyString);
        break;
      }
      form->value.policy_type = osapiStrDup(stat);
      form->status.policy_type |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    memset(stat, 0, sizeof(stat));
    len = (L7_uint32)sizeof(stat);
    if (usmDbDiffServClassNameGet(unit, appInfo.data[2], stat, &len) == L7_SUCCESS)
    {
      form->value.class_name = osapiStrDup(stat);
      form->status.class_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID) == L7_TRUE)
    {
      form->value.color_mode_uint = appInfo.data[18];
      form->status.color_mode_uint |= (EW_FORM_INITIALIZED);
    }

    memset(stat, 0, sizeof(stat));
    usmWebDiffServPolicyAttrActionNameFromActionVal(appInfo.data[4], stat, sizeof(stat));

    form->value.conform_action_uint = appInfo.data[4];
    form->status.conform_action_uint |= (EW_FORM_INITIALIZED);
    form->value.conform_action = osapiStrDup(stat);
    form->status.conform_action |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

    memset(stat, 0, sizeof(stat));
    usmWebDiffServPolicyAttrActionNameFromActionVal(appInfo.data[5], stat, sizeof(stat));

    form->value.exceed_action_uint = appInfo.data[5];
    form->status.exceed_action_uint |= (EW_FORM_INITIALIZED);
    form->value.exceed_action = osapiStrDup(stat);
    form->status.exceed_action |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

    memset(stat, 0, sizeof(stat));
    usmWebDiffServPolicyAttrActionNameFromActionVal(appInfo.data[6], stat, sizeof(stat));

    form->value.nonconform_action_uint = appInfo.data[6];
    form->status.nonconform_action_uint |= (EW_FORM_INITIALIZED);
    form->value.nonconform_action = osapiStrDup(stat);
    form->status.nonconform_action |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

    policyId = appInfo.data[0];
    classId = appInfo.data[2];

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
    }

    colorAwareFeaturePresent = usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                                        L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID);

    singleOrTwoRatePolicing =
      ((appInfo.data[3] == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE) ||
       (appInfo.data[3] == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE));

    if (colorAwareFeaturePresent == L7_TRUE)
    {
      colorConformMode = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND;
      colorExceedMode = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND;

      if (appInfo.data[18] == 2)   /* color-aware */
      {
        if (appInfo.data[7] == L7_TRUE)
        {
          colorConformClassId = appInfo.data[16];
        }
        else
        {
          /* set the color conform class id to the first color
           * class that exists (uses same core logic as the
           * Select code)
           */
          tempId = 0;
          while (usmDbDiffServClassGetNext(unit, tempId, &tempId) == L7_SUCCESS)
          {
            if (usmDbDiffServPolicyAttrStmtPoliceColorAwareClassValidate(unit, policyId, policyInstId, tempId) == L7_SUCCESS)
            {
              memset(stat, 0, sizeof(stat));
              len = (L7_uint32)sizeof(stat);
              if (usmDbDiffServClassNameGet(unit, tempId, stat, &len) == L7_SUCCESS)
              {
                colorConformClassId = tempId;
                break;
              }
            }
          }
        }
        form->value.color_conform_class = colorConformClassId;
        form->status.color_conform_class |= (EW_FORM_INITIALIZED);

        if (singleOrTwoRatePolicing == L7_TRUE)
        {
          if (appInfo.data[7] == L7_TRUE)
          {
            colorExceedClassId = appInfo.data[17];
            form->value.color_exceed_class = colorExceedClassId;
            form->status.color_exceed_class |= (EW_FORM_INITIALIZED);
          }
        }
      }
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                        prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                        &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       needs to be created or modified */
    while (rc != L7_ERROR)
    {
      if (rc == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId, nextPolicyAttrId,
                                                      &entryType) == L7_SUCCESS)
          {
            /* check if attribute is already specified */
            if (entryType == appInfo.data[3])
            {
              form->value.attribute_exists = L7_TRUE;
              form->value.attribute_id = nextPolicyAttrId;
              form->status.attribute_id |= (EW_FORM_INITIALIZED);

              /* pull in the current color conform and exceed index from the
               * policy attribute, if any, to override the value derived above
               */
              if (colorAwareFeaturePresent == L7_TRUE)
              {
                if (appInfo.data[18] == 2)   /* color-aware */
                {
                  if (appInfo.data[7] == L7_FALSE)
                  {
                    if (usmDbDiffServPolicyAttrStmtPoliceColorConformIndexGet(unit, nextPolicyId, nextPolicyInstId,
                                                                              nextPolicyAttrId, &val) == L7_SUCCESS)
                    {
                      if (val != 0)
                      {
                        colorConformClassId = val;
                      }
                    }

                    if (singleOrTwoRatePolicing == L7_TRUE)
                    {
                      if (usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexGet(unit, nextPolicyId, nextPolicyInstId,
                                                                               nextPolicyAttrId, &val) == L7_SUCCESS)
                      {
                        if (val != 0)
                        {
                          colorExceedClassId = val;
                          form->value.color_exceed_class = colorExceedClassId;
                          form->status.color_exceed_class |= (EW_FORM_INITIALIZED);
                        }
                      }
                    } /* endif single or two rate policing */
                  }
                }
              } /* endif color aware feature */

              /* get the current value(s) */

              if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCrateGet(unit, nextPolicyId, nextPolicyInstId,
                                                                      nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                form->value.committed_rate = val;
                form->status.committed_rate |= (EW_FORM_INITIALIZED);
              }
              if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCburstGet(unit, nextPolicyId, nextPolicyInstId,
                                                                       nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                form->value.committed_burst_size = val;
                form->status.committed_burst_size |= (EW_FORM_INITIALIZED);
              }
              if (usmDbDiffServPolicyAttrStmtPoliceSinglerateEburstGet(unit, nextPolicyId, nextPolicyInstId,
                                                                       nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                form->value.excess_burst_size = val;
                form->status.excess_burst_size |= (EW_FORM_INITIALIZED);
              }
              if (usmDbDiffServPolicyAttrStmtPoliceTworateCrateGet(unit, nextPolicyId, nextPolicyInstId,
                                                                   nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                form->value.committed_rate = val;
                form->status.committed_rate |= (EW_FORM_INITIALIZED);
              }
              if (usmDbDiffServPolicyAttrStmtPoliceTworateCburstGet(unit, nextPolicyId, nextPolicyInstId,
                                                                    nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                form->value.committed_burst_size = val;
                form->status.committed_burst_size |= (EW_FORM_INITIALIZED);
              }
              if (usmDbDiffServPolicyAttrStmtPoliceTworatePrateGet(unit, nextPolicyId, nextPolicyInstId,
                                                                   nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                form->value.peak_rate = val;
                form->status.peak_rate |= (EW_FORM_INITIALIZED);
              }
              if (usmDbDiffServPolicyAttrStmtPoliceTworatePburstGet(unit, nextPolicyId, nextPolicyInstId,
                                                                    nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                form->value.peak_burst_size = val;
                form->status.peak_burst_size |= (EW_FORM_INITIALIZED);
              }
              if (usmDbDiffServPolicyAttrStmtPoliceSimpleCrateGet(unit, nextPolicyId, nextPolicyInstId,
                                                                  nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                form->value.committed_rate = val;
                form->status.committed_rate |= (EW_FORM_INITIALIZED);
              }
              if (usmDbDiffServPolicyAttrStmtPoliceSimpleCburstGet(unit, nextPolicyId, nextPolicyInstId,
                                                                   nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                form->value.committed_burst_size = val;
                form->status.committed_burst_size |= (EW_FORM_INITIALIZED);
              }

              /* exceed val can be ip precedence val or dscp val or keyword  */
              if (usmDbDiffServPolicyAttrStmtPoliceExceedValGet(unit, nextPolicyId, nextPolicyInstId,
                                                                nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                if (appInfo.data[5] == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC)
                {
                  form->value.exceed_ip_prec_val = val;
                  form->status.exceed_ip_prec_val |= (EW_FORM_INITIALIZED);
                }
                if (appInfo.data[5] == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS)
                {
                  form->value.exceed_cos_val = val;
                  form->status.exceed_cos_val |= (EW_FORM_INITIALIZED);
                }
                if (appInfo.data[5] == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2)
                {
                  form->value.exceed_cos2_val = val;
                  form->status.exceed_cos2_val |= (EW_FORM_INITIALIZED);
                }
                else
                {
                  if (usmWebDiffServPolicyAttrSelectFromDscpVal(val, &selectVal) == L7_SUCCESS)
                  {
                    form->value.exceed_dscp_key = selectVal;
                  }
                  else
                  {
                    form->value.exceed_dscp_key = other;
                    form->value.exceed_dscp_val = val;
                    form->status.exceed_dscp_val |= (EW_FORM_INITIALIZED);
                  }
                }
              }

              /* conform val can be ip precedence val or dscp val or keyword  */
              if (usmDbDiffServPolicyAttrStmtPoliceConformValGet(unit, nextPolicyId, nextPolicyInstId,
                                                                 nextPolicyAttrId, &val) == L7_SUCCESS)
              {

                if (appInfo.data[4] == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC)
                {
                  if(precflag)
                  {
                    form->value.conform_ip_prec_val = L7_NULL;
                    form->status.conform_ip_prec_val |= (EW_FORM_INITIALIZED);
                  }
                  else
                  {
                    form->value.conform_ip_prec_val = val;
                    form->status.conform_ip_prec_val |= (EW_FORM_INITIALIZED);
                  }

                }
                if (appInfo.data[4] == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS)
                {
                  form->value.conform_cos_val = val;
                  form->status.conform_cos_val |= (EW_FORM_INITIALIZED);
                }
                if (appInfo.data[4] == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2)
                {
                  form->value.conform_cos2_val = val;
                  form->status.conform_cos2_val |= (EW_FORM_INITIALIZED);
                }

                else
                {
                  if(precflag)
                  {
                    form->value.conform_dscp_key = L7_NULL;
                    form->status.conform_dscp_val |= (EW_FORM_INITIALIZED);
                  }
                  else
                  {
                    if (usmWebDiffServPolicyAttrSelectFromDscpVal(val, &selectVal) == L7_SUCCESS)
                    {
                      form->value.conform_dscp_key = selectVal;
                    }
                    else
                    {
                      form->value.conform_dscp_key = other;
                      form->value.conform_dscp_val = val;
                      form->status.conform_dscp_val |= (EW_FORM_INITIALIZED);
                    }
                  }
                }
              }

              /* nonconform val can be ip precedence val or dscp val or keyword  */
              if (usmDbDiffServPolicyAttrStmtPoliceNonconformValGet(unit, nextPolicyId, nextPolicyInstId,
                                                                    nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                if (appInfo.data[6] == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC)
                {
                  form->value.nonconform_ip_prec_val = val;
                  form->status.nonconform_ip_prec_val |= (EW_FORM_INITIALIZED);
                }
                if (appInfo.data[6] == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS)
                {
                  form->value.nonconform_cos_val = val;
                  form->status.nonconform_cos_val |= (EW_FORM_INITIALIZED);
                }
                if (appInfo.data[6] == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2)
                {
                  form->value.nonconform_cos2_val = val;
                  form->status.nonconform_cos2_val |= (EW_FORM_INITIALIZED);
                }
                else
                {
                  if (usmWebDiffServPolicyAttrSelectFromDscpVal(val, &selectVal) == L7_SUCCESS)
                  {
                    form->value.nonconform_dscp_key = selectVal;
                  }
                  else
                  {
                    form->value.nonconform_dscp_key = other;
                    form->value.nonconform_dscp_val = val;
                    form->status.nonconform_dscp_val |= (EW_FORM_INITIALIZED);
                  }
                }
              }

            }
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */
          prevPolicyAttrId = nextPolicyAttrId;

        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                            prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                            &nextPolicyAttrId);
      }
    } /* endwhile */

    /* build a display string for the color conform and exceed classes */
    if (colorAwareFeaturePresent == L7_TRUE)
    {
      if (appInfo.data[18] == 2)   /* color-aware */
      {
        if (colorConformClassId != 0)
        {
          if (usmDbDiffServPolicyAttrStmtPoliceColorAwareClassInfoGet(unit, colorConformClassId, &colorConformMode, &colorConformVal) != L7_SUCCESS)
          {
            colorConformVal = 0;
          }
        }

        if (singleOrTwoRatePolicing == L7_TRUE)
        {
          if (colorExceedClassId != 0)
          {
            if (usmDbDiffServPolicyAttrStmtPoliceColorAwareClassInfoGet(unit, colorExceedClassId, &colorExceedMode, &colorExceedVal) != L7_SUCCESS)
            {
              colorExceedVal = 0;
            }
          }
          else
          {
            colorExceedMode = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED;
          }
        } /* endif single or two rate policing */
      } /* endif color-aware */

      /* the color mode is displayed whether Blind or Aware */
      memset(buf, 0, sizeof(buf));
      memset(stat, 0, sizeof(stat));
      usmWebDiffServPolicyAttrColorNameFromColorVal(colorConformMode, stat, sizeof(stat));
      if (colorConformMode != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND)
      {
        (void)osapiSnprintf(buf, sizeof(buf), "%s %u", stat, colorConformVal);
      }
      else
      {
        OSAPI_STRNCPY_SAFE(buf, stat);
      }
      form->value.color_conform_mode = osapiStrDup(buf);
      form->status.color_conform_mode |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

      if (singleOrTwoRatePolicing == L7_TRUE)
      {
        memset(stat, 0, sizeof(stat));
        usmWebDiffServPolicyAttrColorNameFromColorVal(colorExceedMode, stat, sizeof(stat));
        if ((colorExceedMode != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND) &&
            (colorExceedMode != L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED))
        {
          (void)osapiSnprintf(buf, sizeof(buf), "%s %u", stat, colorExceedVal);
        }
        else
        {
          OSAPI_STRNCPY_SAFE(buf, stat);
        }
        form->value.color_exceed_mode = osapiStrDup(buf);
        form->status.color_exceed_mode |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
      }
    } /* endif color aware feature */
  }
  else
  {
    form->value.err_flag = L7_FALSE;
    form->value.err_msg = NULL;
    form->status.err_msg |= (EW_FORM_INITIALIZED);
  }

  if (appInfo.data[7] == L7_TRUE)
  {
    form->value.conform_dscp_key = appInfo.data[8];
    form->value.exceed_dscp_key = appInfo.data[9];
    form->value.nonconform_dscp_key = appInfo.data[10];

    form->value.committed_rate        = appInfo.data[11];
    form->value.committed_burst_size  = appInfo.data[12];
    form->value.excess_burst_size     = appInfo.data[13];
    form->value.peak_rate             = appInfo.data[14];
    form->value.peak_burst_size       = appInfo.data[15];
    form->value.color_conform_class   = appInfo.data[16];
    form->value.color_exceed_class    = appInfo.data[17];

    form->status.committed_rate |= (EW_FORM_INITIALIZED);
    form->status.committed_burst_size |= (EW_FORM_INITIALIZED);
    form->status.excess_burst_size |= (EW_FORM_INITIALIZED);
    form->status.peak_rate |= (EW_FORM_INITIALIZED);
    form->status.peak_burst_size |= (EW_FORM_INITIALIZED);
  }

  form->status.exceed_dscp_key |= (EW_FORM_INITIALIZED);
  form->status.conform_dscp_key |= (EW_FORM_INITIALIZED);
  form->status.nonconform_dscp_key |= (EW_FORM_INITIALIZED);
  form->status.color_conform_class |= (EW_FORM_INITIALIZED);
  form->status.color_exceed_class |= (EW_FORM_INITIALIZED);

  form->value.entry_type = appInfo.data[3];
  form->status.entry_type |= (EW_FORM_INITIALIZED);

  form->value.Submit = (char *) osapiStrDup( pStrInfo_qos_CfgureSelectedAttr);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the policing_cfg form
*
* @returns policing_cfg.html
*
* @end
*
*********************************************************************/
L7_char8 *ewaFormSubmit_policing_cfg( EwsContext context, EwaForm_policing_cfgP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 pageToLoad[USMWEB_FILENAME_SIZE];
  L7_uint32 val;
  L7_uint32 policyAttrId;
  L7_uint32 policyId;
  L7_uint32 classId;
  L7_uint32 policyInstId;
  L7_uint32 entryType;
  L7_uint32 status;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  memset(&pageToLoad, 0, sizeof(pageToLoad));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  usmDbDiffServPolicyNameToIndex(unit, form->value.policy_name, &policyId);
  appInfo.data[0] = policyId;
  usmDbDiffServClassNameToIndex(unit, form->value.class_name, &classId);
  appInfo.data[2] = classId;

  appInfo.data[3] = form->value.entry_type;

  appInfo.data[18] = form->value.color_mode_uint;

  appInfo.data[4] = form->value.conform_action_uint;
  appInfo.data[5] = form->value.exceed_action_uint;
  appInfo.data[6] = form->value.nonconform_action_uint;

  entryType = form->value.entry_type;

  if (form->status.Submit & EW_FORM_RETURNED)
  { /* range checking first */
    if (form->value.committed_burst_size < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MIN ||
        form->value.committed_burst_size > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MAX ||
        form->status.committed_burst_size & EW_FORM_PARSE_ERROR)
    {
      appInfo.err.err_flag = L7_TRUE;
    }

    if (form->value.committed_rate < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CRATE_KBPS_MIN ||
        form->value.committed_rate > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CRATE_KBPS_MAX ||
        form->status.committed_rate & EW_FORM_PARSE_ERROR)
    {
      appInfo.err.err_flag = L7_TRUE;
    }
 
    /* this field exists only for Single Rate */
    if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
    {
      if (form->value.excess_burst_size < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_EBURST_MIN ||
          form->value.excess_burst_size > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_EBURST_MAX ||
          form->status.excess_burst_size & EW_FORM_PARSE_ERROR)
      {
        appInfo.err.err_flag = L7_TRUE;
      }
      /* todo -- guarantee no errors when setting these values */
      if (form->value.excess_burst_size < form->value.committed_burst_size)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, appInfo.err.msg, pStrInfo_qos_ExcessBurstSizeMustBeEqualToOrGreaterThanCommittedBurstSize);

      }
    }

    /* these 2 fields exist only for Two Rate */
    if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
    {
      if (form->value.peak_rate < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PRATE_KBPS_MIN ||
          form->value.peak_rate > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PRATE_KBPS_MAX ||
          form->status.peak_rate & EW_FORM_PARSE_ERROR)
      {
        appInfo.err.err_flag = L7_TRUE;
      }

      if (form->value.peak_burst_size < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PBURST_MIN ||
          form->value.peak_burst_size > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PBURST_MAX ||
          form->status.peak_burst_size & EW_FORM_PARSE_ERROR)
      {
        appInfo.err.err_flag = L7_TRUE;
      }
      /* todo -- guarantee no errors when setting these values */
      if (form->value.peak_burst_size < form->value.committed_burst_size)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, appInfo.err.msg, pStrInfo_qos_PeakBurstSizeMustBeEqualToOrGreaterThanCommittedBurstSize);

      }

    }

    if (appInfo.err.err_flag == L7_TRUE)
    {
      /* Invalid value field.  No action taken.  */
      OSAPI_STRNCAT(appInfo.err.msg, pStrErr_common_ValFieldNoActionTaken);

      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFilePolicingCfg);
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID) == L7_TRUE)
    {
      if (form->value.color_mode_uint == 2)
      {
        if (form->value.color_conform_class == none_color_class_value)
        {
          appInfo.err.err_flag = L7_TRUE;
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, appInfo.err.msg, pStrInfo_qos_AColorConformClassSelectionIsRequiredForColorAwareMode);
        }
        else if (usmDbDiffServPolicyAttrStmtPoliceColorAwareClassesAreEquivalent(unit,
                                                                                 form->value.color_conform_class,
                                                                                 form->value.color_exceed_class) == L7_TRUE)
        {
          appInfo.err.err_flag = L7_TRUE;
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, appInfo.err.msg, pStrInfo_qos_ColorConformAndExceedClassesMustNotDefineAnEquivalentModeAndVal);
        }

        if (appInfo.err.err_flag == L7_TRUE)
        {
          return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFilePolicingCfg);
        }
      }
    }

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      appInfo.err.err_flag = L7_TRUE;
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, appInfo.err.msg, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }

    if (form->value.attribute_exists == L7_FALSE)
    {
      /* find the free index in the policy attribute table*/
      if ( usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                            &policyAttrId ) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        if (0 == policyAttrId)
        {
          OSAPI_STRNCAT(appInfo.err.msg, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
        }
        else
        {
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_FindAFreeIdxInPolicyAttrTbl);
        }
      }

      /* create the row entry in the policy attribute table */
      if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                        policyAttrId, L7_TRUE) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_CreatePolicyAttr);
      }

      /* specify the attribute type */

      if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId,
                                                  policyInstId,
                                                  policyAttrId,
                                                  entryType) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetPolicyAttrType);
        /* delete the row entry just created*/
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
      }
    }
    else
    {
      policyAttrId = form->value.attribute_id;
    }

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                 L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID) == L7_TRUE)
    {
      /* before setting the actual color conform and exceed indexes, clear
       * out the existing values by setting the conform index to 0 (color-blind)
       * first -- this prevents false conflict check errors when the new
       * conform index is compared against the old exceed index
       *
       * NOTE:  USMDB automatically sets the exceed index to 0 when
       *        the conform index is set to 0
       */
      if (usmDbDiffServPolicyAttrStmtPoliceColorConformIndexSet(unit,
                                                                policyId, policyInstId, policyAttrId,
                                                                0) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetPolicingAttrColorConformClass);
      }
      else
      {
        /* set the conform color class index in the policing policy attr,
         * which will cause the corresponding color mode and value to be set
         * (we just set the index to 0, so no need to set it to 0 again)
         */
        if (form->value.color_conform_class != 0)
        {
          if (usmDbDiffServPolicyAttrStmtPoliceColorConformIndexSet(unit,
                                                                    policyId, policyInstId, policyAttrId,
                                                                    form->value.color_conform_class) != L7_SUCCESS)
          {
            appInfo.err.err_flag = L7_TRUE;
            OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetPolicingAttrColorConformClass);
          }
        }

        /* set the exceed color class index in the policing policy attr,
         * which will cause the corresponding color mode and value to be set
         * (only valid for single-rate or two-rate policing)
         *
         * the color_exceed_class will be 0 if the exceed color is not
         * specified
         */
        if ((entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
            || (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE))
        {
          if (usmDbDiffServPolicyAttrStmtPoliceColorExceedIndexSet(unit,
                                                                   policyId, policyInstId, policyAttrId,
                                                                   form->value.color_exceed_class) != L7_SUCCESS)
          {
            appInfo.err.err_flag = L7_TRUE;
            OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetPolicingAttrColorExceedClass);
          }
        }
      }
    }

    if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE ||
        entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
    {
      if ( usmDbDiffServPolicyAttrStmtPoliceExceedActSet(unit,
                                                         policyId, policyInstId, policyAttrId,
                                                         form->value.exceed_action_uint) != L7_SUCCESS)
      {  /* error */
      }

      if (form->value.exceed_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP)
      {
        if (usmWebDiffServPolicyAttrDscpValFromSelect(form->value.exceed_dscp_key, &val) != L7_SUCCESS)
        {
          val = form->value.exceed_dscp_val;
        }

      }
      else if (form->value.exceed_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC)
      {
        val = form->value.exceed_ip_prec_val;
      }
      else if (form->value.exceed_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS)
      {
        val = form->value.exceed_cos_val;
      }
      else if (form->value.exceed_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2)
      {
        val = form->value.exceed_cos2_val;
      }
      else
      {
        val = 0;    /* this won't be used by 'set' function */
      }
      /* set val only for markprec & markdscp */
      if ((form->value.exceed_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC ||
           form->value.exceed_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS ||
           form->value.exceed_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2 ||
           form->value.exceed_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP) &&
          usmDbDiffServPolicyAttrStmtPoliceExceedValSet(unit,
                                                        policyId, policyInstId, policyAttrId,
                                                        val) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetPolicyAttrExceedVal);
      }

    }  /* end exceed action fields */

    /* modify the action */
    if (usmDbDiffServPolicyAttrStmtPoliceConformActSet(unit,
                                                       policyId, policyInstId, policyAttrId,
                                                       form->value.conform_action_uint) != L7_SUCCESS)
    {
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetPolicingConformActionVal);
      appInfo.err.err_flag = L7_TRUE;
    }

    if (form->value.conform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP)
    {

      if (usmWebDiffServPolicyAttrDscpValFromSelect(form->value.conform_dscp_key, &val) != L7_SUCCESS)
      {
        val = form->value.conform_dscp_val;
      }

    }
    else if (form->value.conform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC)
    {
      val = form->value.conform_ip_prec_val;
    }
    else if (form->value.conform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS)
    {
      val = form->value.conform_cos_val;
    }
    else if (form->value.conform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2)
    {
      val = form->value.conform_cos2_val;
    }
    else
    {
      val = 0;    /* this won't be used by 'set' function */
    }
    /* set val only for markprec & markdscp */
    if ((form->value.conform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC ||
         form->value.conform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS ||
         form->value.conform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2 ||
         form->value.conform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP) &&
        usmDbDiffServPolicyAttrStmtPoliceConformValSet(unit,
                                                       policyId, policyInstId, policyAttrId,
                                                       val) != L7_SUCCESS)
    {
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetConformingActionVal);
      appInfo.err.err_flag = L7_TRUE;
    }

    if ( usmDbDiffServPolicyAttrStmtPoliceNonconformActSet(unit,
                                                           policyId, policyInstId, policyAttrId,
                                                           form->value.nonconform_action_uint) != L7_SUCCESS)
    {
    }

    if (form->value.nonconform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP)
    {

      if (usmWebDiffServPolicyAttrDscpValFromSelect(form->value.nonconform_dscp_key, &val) != L7_SUCCESS)
      {
        val = form->value.nonconform_dscp_val;
      }

    }
    else if (form->value.nonconform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC)
    {
      val = form->value.nonconform_ip_prec_val;
    }
    else if (form->value.nonconform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS)
    {
      val = form->value.nonconform_cos_val;
    }
    else if (form->value.nonconform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2)
    {
      val = form->value.nonconform_cos2_val;
    }
    else
    {
      val = 0;    /* this won't be used by 'set' function */
    }
    if ((form->value.nonconform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC ||
         form->value.nonconform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS ||
         form->value.nonconform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2 ||
         form->value.nonconform_action_uint == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP) &&
        usmDbDiffServPolicyAttrStmtPoliceNonconformValSet(unit,
                                                          policyId, policyInstId, policyAttrId,
                                                          val) != L7_SUCCESS)
    {
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetNonconformingActionVal);
      appInfo.err.err_flag = L7_TRUE;
    }

    switch (entryType)
    {
    
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      /* set the data rate */
      if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCrateSet(unit,
                                                              policyId, policyInstId, policyAttrId,
                                                              form->value.committed_rate) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetSingleRatePolicingCommittedRateVal);
      }

      /* set the burst size */
      if (usmDbDiffServPolicyAttrStmtPoliceSinglerateCburstSet(unit,
                                                               policyId, policyInstId, policyAttrId,
                                                               form->value.committed_burst_size) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetSingleRatePolicingCommittedBurstSizeVal);
      }

      /* set the exceeding burst size */
      if (usmDbDiffServPolicyAttrStmtPoliceSinglerateEburstSet(unit,
                                                               policyId, policyInstId, policyAttrId,
                                                               form->value.excess_burst_size) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        memcpy(net->app_pointer, &appInfo, sizeof(usmWeb_AppInfo_t));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetSingleRatePolicingExcessBurstSizeVal);
        ewsContextSendReply(context, pStrInfo_qos_HtmlFilePolicingCfg);
        return NULL;
      }

      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      if (usmDbDiffServPolicyAttrStmtPoliceTworateCrateSet(unit,
                                                           policyId, policyInstId, policyAttrId,
                                                           form->value.committed_rate) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetTwoRatePolicingCommittedRateVal);
      }

      /* set the burst size */
      if (usmDbDiffServPolicyAttrStmtPoliceTworateCburstSet(unit,
                                                            policyId, policyInstId, policyAttrId,
                                                            form->value.committed_burst_size) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetTwoRatePolicingCommittedBurstSizeVal);
      }

      if (usmDbDiffServPolicyAttrStmtPoliceTworatePrateSet(unit,
                                                           policyId, policyInstId, policyAttrId,
                                                           form->value.peak_rate) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetTwoRatePolicingPeakRateVal);
      }

      if (usmDbDiffServPolicyAttrStmtPoliceTworatePburstSet(unit,
                                                            policyId, policyInstId, policyAttrId,
                                                            form->value.peak_burst_size) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        memcpy(net->app_pointer, &appInfo, sizeof(usmWeb_AppInfo_t));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetTwoRatePolicingPeakBurstSizeVal);
        ewsContextSendReply(context, pStrInfo_qos_HtmlFilePolicingCfg);
        return NULL;
      }

      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      if (usmDbDiffServPolicyAttrStmtPoliceSimpleCrateSet(unit,
                                                          policyId, policyInstId, policyAttrId,
                                                          form->value.committed_rate) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetSimplePolicingCommittedRateVal);
      }

      /* set the burst size */
      if (usmDbDiffServPolicyAttrStmtPoliceSimpleCburstSet(unit,
                                                           policyId, policyInstId, policyAttrId,
                                                           form->value.committed_burst_size) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        memcpy(net->app_pointer, &appInfo, sizeof(usmWeb_AppInfo_t));
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetSimplePolicingCommittedBurstSizeVal);
        ewsContextSendReply(context, pStrInfo_qos_HtmlFilePolicingCfg);
        return NULL;
      }
      break;
    default:
      break;

    }

    /* make sure row status becomes active */
    usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                        policyAttrId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* Log error message*/
    }

    if (appInfo.err.err_flag == L7_TRUE)
    {
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFilePolicingCfg);
    }
    return usmWebEwsContextSendReply (context, L7_TRUE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyAttrSumm);
  }
  else if (form->status.Cancel & EW_FORM_RETURNED)
  {
    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFilePolicingAttrs);
  }
  /* following is executed when the user changes a select form dscp keyword (ONCHANGE())
   * or the color conform or exceed class
   */

  appInfo.data[7] = L7_TRUE;
  appInfo.data[8] = form->value.conform_dscp_key;
  appInfo.data[9] = form->value.exceed_dscp_key;
  appInfo.data[10] = form->value.nonconform_dscp_key;
  appInfo.data[11] = form->value.committed_rate;
  appInfo.data[12] = form->value.committed_burst_size;
  appInfo.data[13] = form->value.excess_burst_size;
  appInfo.data[14] = form->value.peak_rate;
  appInfo.data[15] = form->value.peak_burst_size;
  appInfo.data[16] = form->value.color_conform_class;
  appInfo.data[17] = form->value.color_exceed_class;

  return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFilePolicingCfg);
}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the mark_ip_dscp form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_mark_ip_dscp(EwsContext context, EwaForm_mark_ip_dscpP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 val, selectVal, entryType;
  L7_RC_t rc;
  L7_char8 stat[80];
  L7_uint32 len = 32;
  L7_uint32 policyId, classId, policyInstId,
            prevPolicyId, prevPolicyInstId, prevPolicyAttrId,
            nextPolicyId, nextPolicyInstId, nextPolicyAttrId;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  net = ewsContextNetHandle(context);

  form->value.attribute_exists = L7_FALSE;
  form->status.attribute_exists |= (EW_FORM_INITIALIZED);

  memset(stat, 0, sizeof(stat));
  (void)osapiSnprintf(stat, sizeof(stat), " (%d %s %u)", L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN,
                      pStrInfo_common_To_5, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX);
  form->value.dscp_number_limits = osapiStrDup(stat);
  form->status.dscp_number_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->status.err_flag |= (EW_FORM_INITIALIZED);

  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    if (form->value.err_flag != 0)
    {
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
    else
    {
      form->value.err_msg = NULL;
      form->status.err_msg |= (EW_FORM_INITIALIZED);
    }
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyNameGet(unit, appInfo.data[0], stat, &len) == L7_SUCCESS)
    {
      form->value.policy_name = osapiStrDup(stat);
      form->status.policy_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyTypeGet(unit, appInfo.data[0], &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_common_In);
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_qos_Out);
        break;
      default:
        (void)osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_EmptyString);
        break;
      }
      form->value.policy_type = osapiStrDup(stat);
      form->status.policy_type |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    len = 32;
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServClassNameGet(unit, appInfo.data[2], stat, &len) == L7_SUCCESS)
    {
      form->value.class_name = osapiStrDup(stat);
      form->status.class_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    policyId = appInfo.data[0];
    classId = appInfo.data[2];

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                        prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                        &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       needs to be created or modified */
    while (rc != L7_ERROR)
    {
      if (rc == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId, nextPolicyAttrId,
                                                      &entryType) == L7_SUCCESS)
          {
            /* check if attribute is already specified */
            if ( entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL)
            {
              form->value.attribute_exists = L7_TRUE;
              form->value.attribute_id = nextPolicyAttrId;
              form->status.attribute_id |= (EW_FORM_INITIALIZED);
              /* get the current value(s) */
              if (usmDbDiffServPolicyAttrStmtMarkIpDscpValGet(unit, nextPolicyId, nextPolicyInstId,
                                                              nextPolicyAttrId, &val) == L7_SUCCESS)
              {

                if (usmWebDiffServPolicyAttrSelectFromDscpVal(val, &selectVal) == L7_SUCCESS)
                {
                  form->value.dscp_keyword = selectVal;
                }
                else
                {
                  form->value.dscp_keyword = other;
                  form->value.dscp_number = val;
                  form->status.dscp_number |= (EW_FORM_INITIALIZED);
                }

              }

            }
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */
          prevPolicyAttrId = nextPolicyAttrId;

        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                            prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                            &nextPolicyAttrId);
      }
    }
  }
  else
  {
    form->value.err_flag = L7_FALSE;
    form->value.err_msg = NULL;
    form->status.err_msg |= (EW_FORM_INITIALIZED);
  }

  if (appInfo.data[7] == L7_TRUE)
  {
    form->value.dscp_keyword = appInfo.data[8];
  }

  form->status.dscp_keyword |= (EW_FORM_INITIALIZED);

  form->value.Submit = (char *) osapiStrDup( pStrInfo_common_Submit);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the mark_ip_dscp form
*
* @returns mark_ip_dscp.html
*
* @end
*
*********************************************************************/
L7_char8 *ewaFormSubmit_mark_ip_dscp( EwsContext context, EwaForm_mark_ip_dscpP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 pageToLoad[USMWEB_FILENAME_SIZE];
  L7_uint32 policyAttrId, policyId, classId, policyInstId, entryType, status, val;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  memset(&pageToLoad, 0, sizeof(pageToLoad));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  usmDbDiffServPolicyNameToIndex(unit, form->value.policy_name, &policyId);
  appInfo.data[0] = policyId;

  usmDbDiffServClassNameToIndex(unit, form->value.class_name, &classId);
  appInfo.data[2] = classId;

  if (form->status.Submit & EW_FORM_RETURNED)
  {
    /* range checking */
    if (form->value.dscp_number < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN ||
        form->value.dscp_number > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX ||
        form->status.dscp_number & EW_FORM_PARSE_ERROR)
    {
      usmWebAppInfoCatErrMsg (&appInfo, L7_NULLPTR, pStrErr_common_ValFieldNoActionTaken);
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileMarkIpDscp);
    }

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      appInfo.err.err_flag = L7_TRUE;
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, appInfo.err.msg, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }

    if (form->value.attribute_exists == L7_FALSE)
    {
      /* find the free index in the policy attribute table*/
      if ( usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                            &policyAttrId ) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        if (0 == policyAttrId)
        {
          OSAPI_STRNCAT(appInfo.err.msg, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
        }
        else
        {
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_FindAFreeIdxInPolicyAttrTbl);
        }
      }

      /* create the row entry in the policy attribute table */
      if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                        policyAttrId, L7_TRUE) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_CreatePolicyAttr);
      }

      /* specify the attribute type */
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL;

      if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId,
                                                  policyInstId,
                                                  policyAttrId,
                                                  entryType) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetPolicyAttrType);
        /* delete the row entry just created*/
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
      }

    }
    else
    {
      policyAttrId = form->value.attribute_id;
    }

    /* set the dscp value */

    if (usmWebDiffServPolicyAttrDscpValFromSelect(form->value.dscp_keyword, &val) != L7_SUCCESS)
    {
      val = form->value.dscp_number;
    }

    if (usmDbDiffServPolicyAttrStmtMarkIpDscpValSet(unit, policyId,
                                                    policyInstId, policyAttrId,
                                                    val) != L7_SUCCESS)
    {
      appInfo.err.err_flag = L7_TRUE;
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetMarkingIpDscpVal);
    }

    /* make sure row status becomes active */
    usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                        policyAttrId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* Log error message*/
    }

    if (appInfo.err.err_flag == L7_TRUE)
    {
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileMarkIpDscp);
    }

    return usmWebEwsContextSendReply (context, L7_TRUE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyAttrSumm);

  }
  else if (form->status.Cancel & EW_FORM_RETURNED)
  {
    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyDef);
  }

  appInfo.data[7] = L7_TRUE;
  appInfo.data[8] = form->value.dscp_keyword;

  return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileMarkIpDscp);
}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the mark_ip_prec form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_mark_ip_prec(EwsContext context, EwaForm_mark_ip_precP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 val, entryType;
  L7_RC_t rc;
  L7_char8 stat[80];
  L7_uint32 len = 32;
  L7_uint32 policyId, classId, policyInstId,
            prevPolicyId, prevPolicyInstId, prevPolicyAttrId,
            nextPolicyId, nextPolicyInstId, nextPolicyAttrId;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  net = ewsContextNetHandle(context);

  form->value.attribute_exists = L7_FALSE;
  form->status.attribute_exists |= (EW_FORM_INITIALIZED);

  memset(stat, 0, sizeof(stat));
  (void)osapiSnprintf(stat, sizeof(stat), " (%d %s %u)", L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN,
                      pStrInfo_common_To_5, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX);
  form->value.ip_prec_val_limits = osapiStrDup(stat);
  form->status.ip_prec_val_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  form->status.err_flag |= (EW_FORM_INITIALIZED);

  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    if (form->value.err_flag != 0)
    {
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
    else
    {
      form->value.err_msg = NULL;
      form->status.err_msg |= (EW_FORM_INITIALIZED);
    }
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyNameGet(unit, appInfo.data[0], stat, &len) == L7_SUCCESS)
    {
      form->value.policy_name = osapiStrDup(stat);
      form->status.policy_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyTypeGet(unit, appInfo.data[0], &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_common_In);
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_qos_Out);
        break;
      default:
        (void)osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_EmptyString);
        break;
      }
      form->value.policy_type = osapiStrDup(stat);
      form->status.policy_type |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    len = 32;
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServClassNameGet(unit, appInfo.data[2], stat, &len) == L7_SUCCESS)
    {
      form->value.class_name = osapiStrDup(stat);
      form->status.class_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    policyId = appInfo.data[0];
    classId = appInfo.data[2];

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                        prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                        &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       needs to be created or modified */
    while (rc != L7_ERROR)
    {
      if (rc == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId, nextPolicyAttrId,
                                                      &entryType) == L7_SUCCESS)
          {
            /* check if attribute is already specified */
            if ( entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL)
            {
              form->value.attribute_exists = L7_TRUE;
              form->value.attribute_id = nextPolicyAttrId;
              form->status.attribute_id |= (EW_FORM_INITIALIZED);
              /* get the current value(s) */
              if (usmDbDiffServPolicyAttrStmtMarkIpPrecedenceValGet(unit, nextPolicyId, nextPolicyInstId,
                                                                    nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                form->value.ip_prec_val = val;
                form->status.ip_prec_val |= (EW_FORM_INITIALIZED);
              }

            }
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */
          prevPolicyAttrId = nextPolicyAttrId;

        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                            prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                            &nextPolicyAttrId);
      }
    }
  }
  else
  {
    form->value.err_flag = L7_FALSE;
    form->value.err_msg = NULL;
    form->status.err_msg |= (EW_FORM_INITIALIZED);
  }

  form->value.Submit = (char *) osapiStrDup( pStrInfo_common_Submit);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the mark_ip_prec form
*
* @returns mark_ip_prec.html
*
* @end
*
*********************************************************************/
L7_char8 *ewaFormSubmit_mark_ip_prec( EwsContext context, EwaForm_mark_ip_precP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 pageToLoad[USMWEB_FILENAME_SIZE];
  L7_uint32 policyAttrId, policyId, classId, policyInstId, entryType, status;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  memset(&pageToLoad, 0, sizeof(pageToLoad));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  usmDbDiffServPolicyNameToIndex(unit, form->value.policy_name, &policyId);
  appInfo.data[0] = policyId;

  usmDbDiffServClassNameToIndex(unit, form->value.class_name, &classId);
  appInfo.data[2] = classId;

  if (form->status.Submit & EW_FORM_RETURNED)
  {
    /* range checking */
    if (form->value.ip_prec_val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN ||
        form->value.ip_prec_val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX ||
        (form->status.ip_prec_val & EW_FORM_PARSE_ERROR)==EW_FORM_PARSE_ERROR)
    {
      usmWebAppInfoCatErrMsg (&appInfo, L7_NULLPTR, pStrErr_common_ValFieldNoActionTaken);
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileMarkIpPrec);
    }

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      appInfo.err.err_flag = L7_TRUE;
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_GetPolicyInstFromClass);
    }

    if (form->value.attribute_exists == L7_FALSE)
    {
      /* find the free index in the policy attribute table*/
      if ( usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                            &policyAttrId ) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        if (0 == policyAttrId)
        {
          OSAPI_STRNCAT(appInfo.err.msg, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
        }
        else
        {
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_FindAFreeIdxInPolicyAttrTbl);
        }
      }

      /* create the row entry in the policy attribute table */
      if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                        policyAttrId, L7_TRUE) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_CreatePolicyAttr);
      }

      /* specify the attribute type */
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL;

      if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId, policyInstId,
                                                  policyAttrId, entryType) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetPolicyAttrType);
        /* delete the row entry just created*/
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
      }

    }
    else
    {
      policyAttrId = form->value.attribute_id;
    }
    /* set fields */

    if (usmDbDiffServPolicyAttrStmtMarkIpPrecedenceValSet(unit, policyId, policyInstId,
                                                          policyAttrId, form->value.ip_prec_val) != L7_SUCCESS)
    {
      appInfo.err.err_flag = L7_TRUE;
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetMarkingIpPrecedenceVal);
    }

    /* make sure row status becomes active */
    usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                        policyAttrId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* Log error message*/
    }

    if (appInfo.err.err_flag == L7_TRUE)
    {
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileMarkIpPrec);
    }

  }
  else if (form->status.Cancel & EW_FORM_RETURNED)
  {
    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyDef);
  }

  return usmWebEwsContextSendReply (context, L7_TRUE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyAttrSumm);

}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the mark_cos form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_mark_cos(EwsContext context, EwaForm_mark_cosP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 val, policyId, classId, policyInstId, prevPolicyId, prevPolicyInstId, prevPolicyAttrId;
  L7_uint32 nextPolicyId, nextPolicyInstId, nextPolicyAttrId, entryType;
  L7_char8 stat[80];
  L7_uint32 len = 32;
  L7_RC_t rc;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  net = ewsContextNetHandle(context);

  form->status.err_flag |= (EW_FORM_INITIALIZED);

  form->value.attribute_exists = L7_FALSE;
  form->status.attribute_exists |= (EW_FORM_INITIALIZED);

  memset(stat, 0, sizeof(stat));
  (void)osapiSnprintf(stat, sizeof(stat), " (%d %s %u)", L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN,
                      pStrInfo_common_To_5, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX);
  form->value.cos_val_limits = osapiStrDup(stat);
  form->status.cos_val_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    if (form->value.err_flag != 0)
    {
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
    else
    {
      form->value.err_msg = NULL;
      form->status.err_msg |= (EW_FORM_INITIALIZED);
    }
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyNameGet(unit, appInfo.data[0], stat, &len) == L7_SUCCESS)
    {
      form->value.policy_name = osapiStrDup(stat);
      form->status.policy_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyTypeGet(unit, appInfo.data[0], &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_common_In);
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_qos_Out);
        break;
      default:
        (void)osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_EmptyString);
        break;
      }
      form->value.policy_type = osapiStrDup(stat);
      form->status.policy_type |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    len = 32;
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServClassNameGet(unit, appInfo.data[2], stat, &len) == L7_SUCCESS)
    {
      form->value.class_name = osapiStrDup(stat);
      form->status.class_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    policyId = appInfo.data[0];
    classId = appInfo.data[2];

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                        prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                        &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       needs to be created or modified */
    while (rc != L7_ERROR)
    {
      if (rc == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId, nextPolicyAttrId,
                                                      &entryType) == L7_SUCCESS)
          {
            /* check if attribute is already specified */
            if ( entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL)
            {
              form->value.attribute_exists = L7_TRUE;

              form->value.attribute_id = nextPolicyAttrId;
              form->status.attribute_id |= (EW_FORM_INITIALIZED);
              /* get the current value(s) */
              if (usmDbDiffServPolicyAttrStmtMarkCosValGet(unit, nextPolicyId, nextPolicyInstId,
                                                           nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                form->value.cos_val = val;
                form->status.cos_val |= (EW_FORM_INITIALIZED);
              }

            }
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */
          prevPolicyAttrId = nextPolicyAttrId;

        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                            prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                            &nextPolicyAttrId);
      }
    }
  }
  else
  {
    form->value.err_flag = L7_FALSE;
    form->value.err_msg = NULL;
    form->status.err_msg |= (EW_FORM_INITIALIZED);
  }

  form->value.Submit = (char *) osapiStrDup( pStrInfo_common_Submit);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the mark_cos form
*
* @returns mark_cos.html
*
* @end
*
*********************************************************************/
L7_char8 *ewaFormSubmit_mark_cos( EwsContext context, EwaForm_mark_cosP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 pageToLoad[USMWEB_FILENAME_SIZE];
  L7_uint32 policyAttrId, policyId, classId, policyInstId, entryType, status;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  memset(&pageToLoad, 0, sizeof(pageToLoad));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  usmDbDiffServPolicyNameToIndex(unit, form->value.policy_name, &policyId);
  appInfo.data[0] = policyId;

  usmDbDiffServClassNameToIndex(unit, form->value.class_name, &classId);
  appInfo.data[2] = classId;

  if (form->status.Submit & EW_FORM_RETURNED)
  {
    /* range checking */
    if (form->value.cos_val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN ||
        form->value.cos_val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX ||
        form->status.cos_val & EW_FORM_PARSE_ERROR)
    {
      usmWebAppInfoCatErrMsg (&appInfo, L7_NULLPTR, pStrErr_common_ValFieldNoActionTaken);
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileMarkCos);
    }

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      appInfo.err.err_flag = L7_TRUE;
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, appInfo.err.msg, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }

    if (form->value.attribute_exists == L7_FALSE)
    {
      /* find the free index in the policy attribute table*/
      if ( usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                            &policyAttrId ) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        if (0 == policyAttrId)
        {
          OSAPI_STRNCAT(appInfo.err.msg, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
        }
        else
        {
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_FindAFreeIdxInPolicyAttrTbl);
        }
      }

      /* create the row entry in the policy attribute table */
      if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                        policyAttrId, L7_TRUE) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_CreatePolicyAttr);
      }
      /* specify the attribute type */
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL;

      if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId, policyInstId,
                                                  policyAttrId, entryType) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetPolicyAttrType);
        /* delete the row entry just created*/
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
      }
    }
    else
    {
      policyAttrId = form->value.attribute_id;
    }

    /* set fields */

    /* set the cos value */
    if (usmDbDiffServPolicyAttrStmtMarkCosValSet(unit,policyId, policyInstId,
                                                 policyAttrId, form->value.cos_val) != L7_SUCCESS)
    {
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetMarkingCosVal);
      appInfo.err.err_flag = L7_TRUE;
    }

    /* make sure row status becomes active */
    usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                        policyAttrId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* Log error message*/
    }

    if (appInfo.err.err_flag == L7_TRUE)
    {
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileMarkCos);
    }

  }
  else if (form->status.Cancel & EW_FORM_RETURNED)
  {
    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyDef);
  }

  return usmWebEwsContextSendReply (context, L7_TRUE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyAttrSumm);
}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the mark_cos2 form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_mark_cos2(EwsContext context, EwaForm_mark_cos2P form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 val, policyId, classId, policyInstId, prevPolicyId, prevPolicyInstId, prevPolicyAttrId;
  L7_uint32 nextPolicyId, nextPolicyInstId, nextPolicyAttrId, entryType;
  L7_char8 stat[80];
  L7_uint32 len = 32;
  L7_RC_t rc;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  net = ewsContextNetHandle(context);

  form->status.err_flag |= (EW_FORM_INITIALIZED);

  form->value.attribute_exists = L7_FALSE;
  form->status.attribute_exists |= (EW_FORM_INITIALIZED);

  memset(stat, 0, sizeof(stat));
  (void)osapiSnprintf(stat, sizeof(stat), " (%d %s %u)", L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN,
                      pStrInfo_common_To_5, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX);
  form->value.cos_val_limits = osapiStrDup(stat);
  form->status.cos_val_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    if (form->value.err_flag != 0)
    {
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
    else
    {
      form->value.err_msg = NULL;
      form->status.err_msg |= (EW_FORM_INITIALIZED);
    }
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyNameGet(unit, appInfo.data[0], stat, &len) == L7_SUCCESS)
    {
      form->value.policy_name = osapiStrDup(stat);
      form->status.policy_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyTypeGet(unit, appInfo.data[0], &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_common_In);
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_qos_Out);
        break;
      default:
        (void)osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_EmptyString);
        break;
      }
      form->value.policy_type = osapiStrDup(stat);
      form->status.policy_type |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    len = 32;
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServClassNameGet(unit, appInfo.data[2], stat, &len) == L7_SUCCESS)
    {
      form->value.class_name = osapiStrDup(stat);
      form->status.class_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    policyId = appInfo.data[0];
    classId = appInfo.data[2];

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                        prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                        &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       needs to be created or modified */
    while (rc != L7_ERROR)
    {
      if (rc == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId, nextPolicyAttrId,
                                                      &entryType) == L7_SUCCESS)
          {
            /* check if attribute is already specified */
            if ( entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL)
            {
              form->value.attribute_exists = L7_TRUE;

              form->value.attribute_id = nextPolicyAttrId;
              form->status.attribute_id |= (EW_FORM_INITIALIZED);
              /* get the current value(s) */
              if (usmDbDiffServPolicyAttrStmtMarkCosValGet(unit, nextPolicyId, nextPolicyInstId,
                                                           nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                form->value.cos_val = val;
                form->status.cos_val |= (EW_FORM_INITIALIZED);
              }

            }
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */
          prevPolicyAttrId = nextPolicyAttrId;

        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          break;
        }
        /* determine next sequential row entry in the policy attribute table */
        rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                            prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                            &nextPolicyAttrId);
      }
    }
  }
  else
  {
    form->value.err_flag = L7_FALSE;
    form->value.err_msg = NULL;
    form->status.err_msg |= (EW_FORM_INITIALIZED);
  }

  form->value.Submit = (char *) osapiStrDup( pStrInfo_common_Submit);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the mark_cos2 form
*
* @returns mark_cos2.html
*
* @end
*
*********************************************************************/
L7_char8 *ewaFormSubmit_mark_cos2( EwsContext context, EwaForm_mark_cos2P form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 pageToLoad[USMWEB_FILENAME_SIZE];
  L7_uint32 policyAttrId, policyId, classId, policyInstId, entryType, status;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  memset(&pageToLoad, 0, sizeof(pageToLoad));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  usmDbDiffServPolicyNameToIndex(unit, form->value.policy_name, &policyId);
  appInfo.data[0] = policyId;

  usmDbDiffServClassNameToIndex(unit, form->value.class_name, &classId);
  appInfo.data[2] = classId;

  if (form->status.Submit & EW_FORM_RETURNED)
  {
    /* range checking */
    if (form->value.cos_val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN ||
        form->value.cos_val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX ||
        form->status.cos_val & EW_FORM_PARSE_ERROR)
    {
      usmWebAppInfoCatErrMsg (&appInfo, L7_NULLPTR, pStrErr_common_ValFieldNoActionTaken);
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileMarkCos2);
    }
    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      appInfo.err.err_flag = L7_TRUE;
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, appInfo.err.msg, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }

    if (form->value.attribute_exists == L7_FALSE)
    {
      /* find the free index in the policy attribute table*/
      if ( usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                            &policyAttrId ) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        if (0 == policyAttrId)
        {
          OSAPI_STRNCAT(appInfo.err.msg, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
        }
        else
        {
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_FindAFreeIdxInPolicyAttrTbl);
        }
      }

      /* create the row entry in the policy attribute table */
      if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                        policyAttrId, L7_TRUE) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_CreatePolicyAttr);
      }
      /* specify the attribute type */
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL;
      if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId, policyInstId,
                                                  policyAttrId, entryType) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetPolicyAttrType);
        /* delete the row entry just created*/
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
      }
    }
    else
    {
      policyAttrId = form->value.attribute_id;
    }

    /* set fields */

    /* set the cos2 value */
    if (usmDbDiffServPolicyAttrStmtMarkCos2ValSet(unit,policyId, policyInstId,
                                                  policyAttrId, form->value.cos_val) != L7_SUCCESS)
    {
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetMarkingSecondaryCosVal);
      appInfo.err.err_flag = L7_TRUE;
    }

    /* make sure row status becomes active */
    usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                        policyAttrId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* Log error message*/
    }

    if (appInfo.err.err_flag == L7_TRUE)
    {
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileMarkCos2);
    }

  }
  else if (form->status.Cancel & EW_FORM_RETURNED)
  {
    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyDef);
  }

  return usmWebEwsContextSendReply (context, L7_TRUE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyAttrSumm);
}
#endif
/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the attr_summ form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_attr_summ(EwsContext context, EwaForm_attr_summP form)
{
  form->value.Refresh = (char *) osapiStrDup( pStrInfo_common_Refresh);
  form->status.Refresh |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the random_drop form
*
* @returns random_drop.html
*
* @end
*
*********************************************************************/
L7_char8 *ewaFormSubmit_attr_summ( EwsContext context, EwaForm_attr_summP form )
{
  ewsContextSendReply(context, pStrInfo_qos_HtmlFileDiffservPolicyAttrSumm);
  return NULL;
}
#if 0
/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the assign_queue form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_assign_queue(EwsContext context, EwaForm_assign_queueP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 val, policyId, classId, policyInstId, prevPolicyId, prevPolicyInstId, prevPolicyAttrId;
  L7_uint32 nextPolicyId, nextPolicyInstId, nextPolicyAttrId, entryType;
  L7_char8 stat[80];
  L7_uint32 len = 32;
  L7_RC_t rc;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  net = ewsContextNetHandle(context);

  form->status.err_flag |= (EW_FORM_INITIALIZED);

  form->value.attribute_exists = L7_FALSE;
  form->status.attribute_exists |= (EW_FORM_INITIALIZED);

  memset(stat, 0, sizeof(stat));
  (void)osapiSnprintf(stat, sizeof(stat), " (%d %s %u)", L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MIN,
                      pStrInfo_common_To_5, L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MAX);
  form->value.queue_id_limits = osapiStrDup(stat);
  form->status.queue_id_limits |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    if (form->value.err_flag != 0)
    {
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
    else
    {
      form->value.err_msg = NULL;
      form->status.err_msg |= (EW_FORM_INITIALIZED);
    }
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyNameGet(unit, appInfo.data[0], stat, &len) == L7_SUCCESS)
    {
      form->value.policy_name = osapiStrDup(stat);
      form->status.policy_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyTypeGet(unit, appInfo.data[0], &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_common_In);
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_qos_Out);
        break;
      default:
        (void)osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_EmptyString);
        break;
      }
      form->value.policy_type = osapiStrDup(stat);
      form->status.policy_type |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
    len = 32;
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServClassNameGet(unit, appInfo.data[2], stat, &len) == L7_SUCCESS)
    {
      form->value.class_name = osapiStrDup(stat);
      form->status.class_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    policyId = appInfo.data[0];
    classId = appInfo.data[2];

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                        prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                        &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       needs to be created or modified */
    while (rc != L7_ERROR)
    {
      if (rc == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId, nextPolicyAttrId,
                                                      &entryType) == L7_SUCCESS)
          {
            /* check if attribute is already specified */
            if ( entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE)
            {
              form->value.attribute_exists = L7_TRUE;

              form->value.attribute_id = nextPolicyAttrId;
              form->status.attribute_id |= (EW_FORM_INITIALIZED);
              /* get the current value(s) */
              if (usmDbDiffServPolicyAttrStmtAssignQueueIdGet(unit, nextPolicyId, nextPolicyInstId,
                                                              nextPolicyAttrId, &val) == L7_SUCCESS)
              {
                form->value.queue_id = val;
                form->status.queue_id |= (EW_FORM_INITIALIZED);
              }

            }
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */
          prevPolicyAttrId = nextPolicyAttrId;

        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          break;
        }
        /* determine next sequential row entry in the policy attribute table */
        rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                            prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                            &nextPolicyAttrId);
      }
    }
  }
  else
  {
    form->value.err_flag = L7_FALSE;
    form->value.err_msg = NULL;
    form->status.err_msg |= (EW_FORM_INITIALIZED);
  }

  form->value.Submit = (char *) osapiStrDup( pStrInfo_common_Submit);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the assign_queue form
*
* @returns assign_queue.html
*
* @end
*
*********************************************************************/
L7_char8 *ewaFormSubmit_assign_queue( EwsContext context, EwaForm_assign_queueP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 pageToLoad[USMWEB_FILENAME_SIZE];
  L7_uint32 policyAttrId, policyId, classId, policyInstId, entryType, status;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  memset(&pageToLoad, 0, sizeof(pageToLoad));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  usmDbDiffServPolicyNameToIndex(unit, form->value.policy_name, &policyId);
  appInfo.data[0] = policyId;

  usmDbDiffServClassNameToIndex(unit, form->value.class_name, &classId);
  appInfo.data[2] = classId;

  if (form->status.Submit & EW_FORM_RETURNED)
  {
    /* range checking */
    if (form->value.queue_id < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MIN ||
        form->value.queue_id > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MAX ||
        form->status.queue_id & EW_FORM_PARSE_ERROR)
    {
      usmWebAppInfoCatErrMsg (&appInfo, L7_NULLPTR, pStrErr_common_ValFieldNoActionTaken);
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileAssignQueue);
    }

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      appInfo.err.err_flag = L7_TRUE;
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, appInfo.err.msg, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }

    if (form->value.attribute_exists == L7_FALSE)
    {
      /* find the free index in the policy attribute table*/
      if ( usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                            &policyAttrId ) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        if (0 == policyAttrId)
        {
          OSAPI_STRNCAT(appInfo.err.msg, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
        }
        else
        {
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_FindAFreeIdxInPolicyAttrTbl);
        }
      }

      /* create the row entry in the policy attribute table */
      if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                        policyAttrId, L7_TRUE) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_CreatePolicyAttr);
      }
      /* specify the attribute type */
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE;

      if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId, policyInstId,
                                                  policyAttrId, entryType) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetPolicyAttrType);
        /* delete the row entry just created*/
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
      }
    }
    else
    {
      policyAttrId = form->value.attribute_id;
    }

    /* set fields */

    /* set the queue id value */
    if (usmDbDiffServPolicyAttrStmtAssignQueueIdSet(unit,policyId, policyInstId,
                                                    policyAttrId, form->value.queue_id) != L7_SUCCESS)
    {
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetQueueId);
      appInfo.err.err_flag = L7_TRUE;
    }

    /* make sure row status becomes active */
    usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                        policyAttrId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* Log error message*/
    }

    if (appInfo.err.err_flag == L7_TRUE)
    {
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileAssignQueue);
    }

  }
  else if (form->status.Cancel & EW_FORM_RETURNED)
  {
    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyDef);
  }
  return usmWebEwsContextSendReply (context, L7_TRUE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyAttrSumm);
}
/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the drop_action form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_Drop(EwsContext context, EwaForm_DropP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 val, entryType;
  L7_RC_t rc;
  L7_char8 stat[80];
  L7_uint32 len = 32;
  L7_uint32 policyId, classId, policyInstId,
            prevPolicyId, prevPolicyInstId, prevPolicyAttrId,
            nextPolicyId, nextPolicyInstId, nextPolicyAttrId;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  net = ewsContextNetHandle(context);

  form->status.err_flag |= (EW_FORM_INITIALIZED);

  form->value.attribute_exists = L7_FALSE;
  form->status.attribute_exists |= (EW_FORM_INITIALIZED);

  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    if (form->value.err_flag != 0)
    {
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
    else
    {
      form->value.err_msg = NULL;
      form->status.err_msg |= (EW_FORM_INITIALIZED);
    }
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyNameGet(unit, appInfo.data[0], stat, &len) == L7_SUCCESS)
    {
      form->value.policy_name = osapiStrDup(stat);
      form->status.policy_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyTypeGet(unit, appInfo.data[0], &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_common_In);
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_qos_Out);
        break;
      default:
        (void)osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_EmptyString);
        break;
      }
      form->value.policy_type = osapiStrDup(stat);
      form->status.policy_type |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    len = 32;
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServClassNameGet(unit, appInfo.data[2], stat, &len) == L7_SUCCESS)
    {
      form->value.class_name = osapiStrDup(stat);
      form->status.class_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    policyId = appInfo.data[0];
    classId = appInfo.data[2];

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                        prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                        &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       needs to be created or modified */
    while (rc != L7_ERROR)
    {
      if (rc == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId, nextPolicyAttrId,
                                                      &entryType) == L7_SUCCESS)
          {
            /* check if attribute is already specified */
            if ( entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP)
            {
              form->value.attribute_exists = L7_TRUE;

              form->value.attribute_id = nextPolicyAttrId;
              form->status.attribute_id |= (EW_FORM_INITIALIZED);

            }
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */
          prevPolicyAttrId = nextPolicyAttrId;

        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                            prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                            &nextPolicyAttrId);
      }
    }

  }
  else
  {
    form->value.err_flag = L7_FALSE;
    form->value.err_msg = NULL;
    form->status.err_msg |= (EW_FORM_INITIALIZED);
  }

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the drop form
*
* @returns Drop.html
*
* @end
*
*********************************************************************/
L7_char8 *ewaFormSubmit_Drop( EwsContext context, EwaForm_DropP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 pageToLoad[USMWEB_FILENAME_SIZE];
  L7_uint32 policyAttrId, policyId, classId, policyInstId, entryType, status;
  L7_uint32 unit;
  L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_t exclude;
  L7_RC_t rc;

  unit = usmDbThisUnitGet();
  memset(&pageToLoad, 0, sizeof(pageToLoad));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  usmDbDiffServPolicyNameToIndex(unit, form->value.policy_name, &policyId);
  appInfo.data[0] = policyId;

  usmDbDiffServClassNameToIndex(unit, form->value.class_name, &classId);
  appInfo.data[2] = classId;

  if (form->status.Submit & EW_FORM_RETURNED)
  {
    if (appInfo.err.err_flag == L7_TRUE)
    {
      /* Invalid value field.  No action taken.  */
      OSAPI_STRNCAT(appInfo.err.msg, pStrErr_common_ValFieldNoActionTaken);
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileDrop);

    }

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      appInfo.err.err_flag = L7_TRUE;
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, appInfo.err.msg, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }

    if (form->value.attribute_exists == L7_FALSE)
    {
      /* find the free index in the policy attribute table*/
      if ( usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                            &policyAttrId ) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        if (0 == policyAttrId)
        {
          OSAPI_STRNCAT(appInfo.err.msg, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
        }
        else
        {
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_FindAFreeIdxInPolicyAttrTbl);
        }
      }

      /* create the row entry in the policy attribute table */
      if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                        policyAttrId, L7_TRUE) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_CreatePolicyAttr);
      }

      /* specify the attribute type */

      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP;

      if (usmDbFeaturePresentCheck(unit, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_POLICY_ATTR_DROP_FEATURE_ID) == L7_TRUE)
      {
        rc = usmDbDiffServPolicyAttrStmtDropFlagGet(unit, policyId, policyInstId, policyAttrId, &exclude);
      }

      /*if (rc == L7_SUCCESS && exclude == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE)*/
      /*if (rc == L7_SUCCESS )
         {*/
      if ( usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId, policyInstId,
                                                   policyAttrId, entryType) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetPolicyAttrType);
        /* delete the row entry just created*/
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
      }
      /*}
         else
         {
         usmWebAppInfoCatErrMsg (&appInfo, L7_NULLPTR, "\r\nCould not set the policy attribute type drop.");
         usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
         }*/
    }
    else
    {
      policyAttrId = form->value.attribute_id;
    }
    /* set fields */

    /* make sure row status becomes active */
    usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                        policyAttrId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* Log error message*/
    }

    if (appInfo.err.err_flag == L7_TRUE)
    {
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileDrop);
    }
  }
  else if (form->status.Cancel & EW_FORM_RETURNED)
  {
    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyDef);
  }
  return usmWebEwsContextSendReply (context, L7_TRUE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyAttrSumm);
}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the mirror_interface form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_mirror_interface(EwsContext context, EwaForm_mirror_interfaceP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 val, policyId, classId, policyInstId, prevPolicyId, prevPolicyInstId, prevPolicyAttrId;
  L7_uint32 nextPolicyId, nextPolicyInstId, nextPolicyAttrId, entryType;
  L7_char8 stat[80];
  L7_uint32 len = 32;
  L7_RC_t rc;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  net = ewsContextNetHandle(context);

  form->status.err_flag |= (EW_FORM_INITIALIZED);

  form->value.attribute_exists = L7_FALSE;
  form->status.attribute_exists |= (EW_FORM_INITIALIZED);

  form->value.redirect_attr_exists = L7_FALSE;
  form->status.redirect_attr_exists |= (EW_FORM_INITIALIZED);

  form->status.diffserv_ports |= (EW_FORM_INITIALIZED);

  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    if (form->value.err_flag != 0)
    {
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
    else
    {
      form->value.err_msg = NULL;
      form->status.err_msg |= (EW_FORM_INITIALIZED);
    }
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyNameGet(unit, appInfo.data[0], stat, &len) == L7_SUCCESS)
    {
      form->value.policy_name = osapiStrDup(stat);
      form->status.policy_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyTypeGet(unit, appInfo.data[0], &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_common_In);
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_qos_Out);
        break;
      default:
        (void)osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_EmptyString);
        break;
      }
      form->value.policy_type = osapiStrDup(stat);
      form->status.policy_type |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    len = 32;
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServClassNameGet(unit, appInfo.data[2], stat, &len) == L7_SUCCESS)
    {
      form->value.class_name = osapiStrDup(stat);
      form->status.class_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    form->value.diffserv_ports = appInfo.data[1];

    policyId = appInfo.data[0];
    classId = appInfo.data[2];

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                        prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                        &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       needs to be created or modified */
    while (rc != L7_ERROR)
    {
      if (rc == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId, nextPolicyAttrId,
                                                      &entryType) == L7_SUCCESS)
          {
            /* check if attribute is already specified */
            if ( entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR)
            {
              form->value.attribute_exists = L7_TRUE;

              form->value.attribute_id = nextPolicyAttrId;
              form->status.attribute_id |= (EW_FORM_INITIALIZED);
            }
            /* don't allow mirror and redirect in same policy-class instance */
            else if ( entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT)
            {
              form->value.redirect_attr_exists = L7_TRUE;
            }
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */
          prevPolicyAttrId = nextPolicyAttrId;

        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                            prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                            &nextPolicyAttrId);
      }
    }
  }
  else
  {
    form->value.err_flag = L7_FALSE;
    form->value.err_msg = NULL;
    form->status.err_msg |= (EW_FORM_INITIALIZED);
  }

  form->value.Submit = (char *) osapiStrDup( pStrInfo_common_Submit);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the mirror_interface form
*
* @returns mirror_interface.html
*
* @end
*
*********************************************************************/
L7_char8 *ewaFormSubmit_mirror_interface( EwsContext context, EwaForm_mirror_interfaceP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 pageToLoad[USMWEB_FILENAME_SIZE];
  L7_uint32 policyAttrId, policyId, classId, policyInstId, entryType, status;
  L7_uint32 unit;
  L7_uint32 intfNum;

  unit = usmDbThisUnitGet();
  memset(&pageToLoad, 0, sizeof(pageToLoad));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  usmDbDiffServPolicyNameToIndex(unit, form->value.policy_name, &policyId);
  appInfo.data[0] = policyId;

  appInfo.data[1] = form->value.diffserv_ports;

  usmDbDiffServClassNameToIndex(unit, form->value.class_name, &classId);
  appInfo.data[2] = classId;

  if (form->status.Submit & EW_FORM_RETURNED)
  {
    if (appInfo.err.err_flag == L7_TRUE)
    {
      /* Invalid value field.  No action taken.  */
      OSAPI_STRNCAT(appInfo.err.msg, pStrErr_common_ValFieldNoActionTaken);
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileMirrorIntf);

    }

    if (form->value.redirect_attr_exists == L7_TRUE)
    {
      /* Mirror and Redirect not allowed in same instance.  */
      OSAPI_STRNCAT_ADD_BLANKS (0, 1, 0, 0, pStrErr_common_Error, appInfo.err.msg, pStrInfo_qos_MirrorIntfAndRedirectIntfCantBeSpecifiedForSamePolicyClassDefinition);
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyDef);
    }

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      appInfo.err.err_flag = L7_TRUE;
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, appInfo.err.msg, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }

    if (form->value.attribute_exists == L7_FALSE)
    {
      /* find the free index in the policy attribute table*/
      if ( usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                            &policyAttrId ) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        if (0 == policyAttrId)
        {
          OSAPI_STRNCAT(appInfo.err.msg, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
        }
        else
        {
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_FindAFreeIdxInPolicyAttrTbl);
        }
      }

      /* create the row entry in the policy attribute table */
      if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                        policyAttrId, L7_TRUE) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_CreatePolicyAttr);
      }
      /* specify the attribute type */
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR;

      if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId, policyInstId,
                                                  policyAttrId, entryType) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetPolicyAttrType);
        /* delete the row entry just created*/
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
      }
    }
    else
    {
      policyAttrId = form->value.attribute_id;
    }

    /* set fields */

    /* set the mirror_if  value */
    intfNum = form->value.diffserv_ports;
    if (usmDbDiffServPolicyAttrStmtMirrorIntfSet(unit,policyId, policyInstId,
                                                 policyAttrId, intfNum) != L7_SUCCESS)
    {
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetMirrorIntf);
      appInfo.err.err_flag = L7_TRUE;
    }

    /* make sure row status becomes active */
    usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                        policyAttrId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* Log error message*/
    }

    if (appInfo.err.err_flag == L7_TRUE)
    {
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileMirrorIntf);
    }

  }
  else if (form->status.Cancel & EW_FORM_RETURNED)
  {
    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyDef);
  }

  return usmWebEwsContextSendReply (context, L7_TRUE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyAttrSumm);
}

/*********************************************************************
*
* @purpose Initialize the strings on the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the redirect_interface form
*
* @returns none
*
* @end
*
*********************************************************************/
void ewaFormServe_redirect_interface(EwsContext context, EwaForm_redirect_interfaceP form)
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_uint32 val, policyId, classId, policyInstId, prevPolicyId, prevPolicyInstId, prevPolicyAttrId;
  L7_uint32 nextPolicyId, nextPolicyInstId, nextPolicyAttrId, entryType;
  L7_char8 stat[80];
  L7_uint32 len = 32;
  L7_RC_t rc;
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  net = ewsContextNetHandle(context);

  form->status.err_flag |= (EW_FORM_INITIALIZED);

  form->value.attribute_exists = L7_FALSE;
  form->status.attribute_exists |= (EW_FORM_INITIALIZED);

  form->value.mirror_attr_exists = L7_FALSE;
  form->status.mirror_attr_exists |= (EW_FORM_INITIALIZED);

  form->status.diffserv_ports |= (EW_FORM_INITIALIZED);

  if (net->app_pointer != NULL)
  {
    memcpy(&appInfo, net->app_pointer, sizeof(usmWeb_AppInfo_t));
    form->value.err_flag = appInfo.err.err_flag;
    if (form->value.err_flag != 0)
    {
      form->value.err_msg = osapiStrDup(appInfo.err.msg);
      form->status.err_msg |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }
    else
    {
      form->value.err_msg = NULL;
      form->status.err_msg |= (EW_FORM_INITIALIZED);
    }
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyNameGet(unit, appInfo.data[0], stat, &len) == L7_SUCCESS)
    {
      form->value.policy_name = osapiStrDup(stat);
      form->status.policy_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServPolicyTypeGet(unit, appInfo.data[0], &val) == L7_SUCCESS)
    {
      switch (val)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_common_In);
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT:
        (void)osapiSnprintf(stat, sizeof(stat), pStrInfo_qos_Out);
        break;
      default:
        (void)osapiSnprintfAddBlanks (0, 0, 0, 7, L7_NULLPTR, stat, sizeof(stat), pStrInfo_common_EmptyString);
        break;
      }
      form->value.policy_type = osapiStrDup(stat);
      form->status.policy_type |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    len = 32;
    memset(stat, 0, sizeof(stat));
    if (usmDbDiffServClassNameGet(unit, appInfo.data[2], stat, &len) == L7_SUCCESS)
    {
      form->value.class_name = osapiStrDup(stat);
      form->status.class_name |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);
    }

    form->value.diffserv_ports = appInfo.data[1];

    policyId = appInfo.data[0];
    classId = appInfo.data[2];

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
    }

    prevPolicyId = policyId;
    prevPolicyInstId = policyInstId;
    prevPolicyAttrId = 0; /* to start the search */

    rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                        prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                        &nextPolicyAttrId);

    /* traverse the policy Attribute table to find whether the given attribute
       needs to be created or modified */
    while (rc != L7_ERROR)
    {
      if (rc == L7_SUCCESS)
      {
        if ((nextPolicyInstId == prevPolicyInstId) && (nextPolicyId == prevPolicyId))
        {
          /* get the attribute type of that row*/
          if (usmDbDiffServPolicyAttrStmtEntryTypeGet(unit, nextPolicyId,
                                                      nextPolicyInstId, nextPolicyAttrId,
                                                      &entryType) == L7_SUCCESS)
          {
            /* check if attribute is already specified */
            if ( entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT)
            {
              form->value.attribute_exists = L7_TRUE;

              form->value.attribute_id = nextPolicyAttrId;
              form->status.attribute_id |= (EW_FORM_INITIALIZED);
            }
            /* don't allow mirror and redirect in same policy-class instance */
            else if ( entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR)
            {
              form->value.mirror_attr_exists = L7_TRUE;
            }
          }

          prevPolicyId = nextPolicyId; /* should be equal to policyId */
          prevPolicyInstId = nextPolicyInstId; /* should be equal to PolicyInstId */
          prevPolicyAttrId = nextPolicyAttrId;

        }
        else
        {
          /* all the attributes had been scanned for the specified policy
             and class combination, this attribute is not yet specified so
             create a row entry in the table and configure this attribute */
          break;
        }

        /* determine next sequential row entry in the policy attribute table */
        rc = usmDbDiffServPolicyAttrGetNext(unit, prevPolicyId, prevPolicyInstId,
                                            prevPolicyAttrId, &nextPolicyId, &nextPolicyInstId,
                                            &nextPolicyAttrId);
      }
    }
  }
  else
  {
    form->value.err_flag = L7_FALSE;
    form->value.err_msg = NULL;
    form->status.err_msg |= (EW_FORM_INITIALIZED);
  }

  form->value.Submit = (char *) osapiStrDup( pStrInfo_common_Submit);
  form->status.Submit |= (EW_FORM_INITIALIZED | EW_FORM_DYNAMIC);

  return;
}

/*********************************************************************
*
* @purpose Save the current values in the form
*
* @param context EmWeb/Server request context handle
*
* @param form pointer to the redirect_interface form
*
* @returns redirect_interface.html
*
* @end
*
*********************************************************************/
L7_char8 *ewaFormSubmit_redirect_interface( EwsContext context, EwaForm_redirect_interfaceP form )
{
  EwaNetHandle net;
  usmWeb_AppInfo_t appInfo;
  L7_char8 pageToLoad[USMWEB_FILENAME_SIZE];
  L7_uint32 policyAttrId, policyId, classId, policyInstId, entryType, status;
  L7_uint32 unit;
  L7_uint32 intfNum;

  unit = usmDbThisUnitGet();
  memset(&pageToLoad, 0, sizeof(pageToLoad));
  net = ewsContextNetHandle(context);
  net->app_pointer = ewaAlloc(sizeof(usmWeb_AppInfo_t));
  memset(net->app_pointer, 0, sizeof(usmWeb_AppInfo_t));
  memset(&appInfo, 0, sizeof(usmWeb_AppInfo_t));
  appInfo.err.err_flag = L7_FALSE;

  usmDbDiffServPolicyNameToIndex(unit, form->value.policy_name, &policyId);
  appInfo.data[0] = policyId;

  appInfo.data[1] = form->value.diffserv_ports;

  usmDbDiffServClassNameToIndex(unit, form->value.class_name, &classId);
  appInfo.data[2] = classId;

  if (form->status.Submit & EW_FORM_RETURNED)
  {
    if (appInfo.err.err_flag == L7_TRUE)
    {
      /* Invalid value field.  No action taken.  */
      OSAPI_STRNCAT(appInfo.err.msg, pStrErr_common_ValFieldNoActionTaken);
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileRedirectIntf);

    }

    if (form->value.mirror_attr_exists == L7_TRUE)
    {
      OSAPI_STRNCAT_ADD_BLANKS (0, 1, 0, 0, pStrErr_common_Error, appInfo.err.msg, pStrInfo_qos_MirrorIntfAndRedirectIntfCantBeSpecifiedForSamePolicyClassDefinition);
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyDef);
    }

    /* get policyInstId */
    if (usmDbDiffServPolicyInstIndexFromClass(unit, policyId, classId,
                                              &policyInstId) != L7_SUCCESS)
    {
      appInfo.err.err_flag = L7_TRUE;
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, appInfo.err.msg, pStrErr_qos_DiffservClassNotAMbrPolicy);
    }

    if (form->value.attribute_exists == L7_FALSE)
    {
      /* find the free index in the policy attribute table*/
      if ( usmDbDiffServPolicyAttrIndexNext(unit, policyId, policyInstId,
                                            &policyAttrId ) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        if (0 == policyAttrId)
        {
          OSAPI_STRNCAT(appInfo.err.msg, pStrInfo_qos_NoMoreAttrsCanBeSpecified);
        }
        else
        {
          OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_FindAFreeIdxInPolicyAttrTbl);
        }
      }

      /* create the row entry in the policy attribute table */
      if (usmDbDiffServPolicyAttrCreate(unit, policyId, policyInstId,
                                        policyAttrId, L7_TRUE) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_CreatePolicyAttr);
      }
      /* specify the attribute type */
      entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT;

      if (usmDbDiffServPolicyAttrStmtEntryTypeSet(unit, policyId, policyInstId,
                                                  policyAttrId, entryType) != L7_SUCCESS)
      {
        appInfo.err.err_flag = L7_TRUE;
        OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetPolicyAttrType);
        /* delete the row entry just created*/
        usmDbDiffServPolicyAttrDelete(unit, policyId, policyInstId, policyAttrId);
      }
    }
    else
    {
      policyAttrId = form->value.attribute_id;
    }

    /* set fields */

    /* set the redirect_if  value */
    intfNum = form->value.diffserv_ports;
    if (usmDbDiffServPolicyAttrStmtRedirectIntfSet(unit,policyId, policyInstId,
                                                   policyAttrId, intfNum) != L7_SUCCESS)
    {
      OSAPI_STRNCAT_ADD_BLANKS (1, 0, 0, 0, pStrErr_common_CouldNot, appInfo.err.msg, pStrInfo_qos_SetRedirectIntf);
      appInfo.err.err_flag = L7_TRUE;
    }

    /* make sure row status becomes active */
    usmDbDiffServPolicyAttrRowStatusGet(unit, policyId, policyInstId,
                                        policyAttrId, &status);
    if (status != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* Log error message*/
    }

    if (appInfo.err.err_flag == L7_TRUE)
    {
      return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileRedirectIntf);
    }

  }
  else if (form->status.Cancel & EW_FORM_RETURNED)
  {
    return usmWebEwsContextSendReply (context, L7_FALSE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyDef);
  }

  return usmWebEwsContextSendReply (context, L7_TRUE, &appInfo,  pStrInfo_qos_HtmlFileDiffservPolicyAttrSumm);
}

/*********************************************************************
*
* @purpose Generate the list of configured physical ports.
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
void *ewaFormSelect_diffserv_ports(EwsContext context,
                                   EwsFormSelectOptionP optionp,
                                   void * iterator )
{
  static L7_uint32 itrValid;
  static L7_uint32 sel_intIfNum;
  static L7_uint32 intIfNum;
  static L7_uint32 firstIntIfNum;
  static L7_BOOL firstIntfAvail;
  static L7_uint32 counter;
  L7_uint32 unit;
  L7_BOOL intfFound;
  L7_char8 buf[APP_BUFFER_SIZE];       /* slot.port */
  L7_char8 * bufChoice;
  L7_uint32 u,s,p;
  L7_RC_t rc;
  L7_uint32 addAllOption;

  unit = usmDbThisUnitGet();
  itrValid = L7_FALSE;
  intfFound = L7_FALSE;
  addAllOption = L7_TRUE;

  if (iterator == L7_NULL)    /* first iteration */
  {
    rc = usmWebIntIfNumTypeFirstGet(unit, &firstIntIfNum);

    if (rc == L7_SUCCESS)
    {
      intIfNum = firstIntIfNum;
      firstIntfAvail = L7_TRUE;
    }
    else
    {
      firstIntfAvail = L7_FALSE;
    }

    counter = 1;  /* Start at the first slot.port interface... */

    /* ...provided read-only or slot.port is supported... */
    if (firstIntfAvail == L7_TRUE)
    {
      intfFound = L7_TRUE;
    }
    /* ------------------------------------------
     * Set the selected interface
     * ------------------------------------------ */
    if (firstIntfAvail == L7_TRUE)
    {
      sel_intIfNum = usmWebSelectedIntIfNumGet(context, firstIntfAvail, firstIntIfNum);
    }
  }
  else
  {
    if (counter == 1)
    {
      if (firstIntfAvail == L7_TRUE)
      {
        intIfNum = firstIntIfNum;
        intfFound = L7_TRUE;
      }
    }
    else      /* counter > 1 */

    {
      rc = usmWebIntIfNumTypeNextGet(unit, intIfNum, &intIfNum);
      if (rc == L7_SUCCESS)
      {
        intfFound = L7_TRUE;
      }
    }
  }

  if (intfFound == L7_TRUE)
  {
    if (counter == 0)
    {
      (void)osapiSnprintf(buf, sizeof(buf), pStrInfo_common_ApProfileRadioScanFrequencyAll);      /* --all-- */

      bufChoice = ewsContextNetHandle(context)->buffer;
      osapiStrncpySafe(bufChoice, buf, APP_BUFFER_SIZE);

      optionp->choice = bufChoice;
      optionp->valuep = (void *) &all_interface_value;

      if (all_interface_value == sel_intIfNum)
      {
        optionp->selected = TRUE;
      }
      else
      {
        optionp->selected = FALSE;
      }
    }
    else
    {
      rc = usmDbUnitSlotPortGet(intIfNum, &u, &s, &p);
      if (usmDbComponentPresentCheck(unit, L7_FLEX_STACKING_COMPONENT_ID))
      {
        (void)osapiSnprintf(buf, sizeof(buf), "%d/%d/%d", u, s, p);
      }
      else
      {
        (void)osapiSnprintf(buf, sizeof(buf), "%d/%d", s, p);
      }

      bufChoice = ewsContextNetHandle(context)->buffer;
      osapiStrncpySafe(bufChoice, buf, APP_BUFFER_SIZE);

      optionp->choice = bufChoice;
      optionp->valuep = (void *) &intIfNum;

      if (intIfNum == sel_intIfNum)
      {
        optionp->selected = TRUE;
      }
      else
      {
        optionp->selected = FALSE;
      }
    }
    itrValid = L7_TRUE;
  }

  counter++;

  if (itrValid == L7_TRUE)
  {
    return (void *) optionp;
  }
  else
  {
    return L7_NULL;
  }

}
#endif
