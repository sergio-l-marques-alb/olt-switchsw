/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/emweb/web/qos/diffserv/web_diffserv.h
 *
 * @purpose Support of the EmWeb code
 *
 * @component diffserv
 *
 * @comments
 *
 * @create 07/24/2002
 *
 * @author gbrown
 * @end
 *
 **********************************************************************/

#ifndef WEB_DIFFSERV_H__
#define WEB_DIFFSERV_H__

extern L7_RC_t usmWebDiffservNumericRuleLimitsGet(L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t ruleType, L7_uchar8 * ruleLimits, L7_uint32 ruleLimitsLen);
extern L7_RC_t usmWebl4portShowPortNumberFieldsSet(L7_BOOL displayFields);
extern L7_RC_t usmWebIntIfNumTypeFirstGet(L7_uint32 unit, L7_uint32 * pIntIfNum);
extern L7_RC_t usmWebSelectedIntIfNumGet(EwsContext context, L7_BOOL firstIntfAvail, L7_uint32 firstIntIfNum);
extern L7_RC_t usmWebIntIfNumTypeNextGet(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32 * pIntIfNum);

typedef enum
{
  USMWEB_DIFFSERV_PACKETS = 1,
  USMWEB_DIFFSERV_OCTETS
} USMWEB_DIFFSERV_STATS_MODE_t;

/* For Use in DIFFSERV Web Component*/
typedef struct
{
  L7_char8 entryType[50];
  L7_char8 value[50];
  L7_char8 excluded[50];
  L7_char8 errorlog[70];
} usmWeb_diffservClassRuleInfo_t;

typedef struct
{
  L7_uint32 policyIndex;
  L7_uint32 intIfIndex;
  L7_uint32 ifDirection;
} usmWeb_diffservServiceSummInfo_t;

typedef struct
{
  L7_uint32 policyId;
  L7_uint32 policyInstId;
  L7_uint32 policyAttrId;
  L7_uint32 type;

} usmWeb_diffServPolicyAttrEntry_t;

/*********************************************************************
*
* @purpose  Return isCreateLoaded, the flag that determines if we're creating
*           a class or on the normal cfg page
*
* @param    val
*
* @returns  Interface number
*
* @end
*
*********************************************************************/
L7_BOOL usmWebDiffservClassIsCreateLoadedGet();

/*********************************************************************
*
* @purpose  Return the Interface number to route_nbr_summ.html
*
* @param    val
*
* @returns  Interface number
*
* @end
*
*********************************************************************/
L7_char8 *usmWebDiffservClassRuleFormat(L7_char8 * entryType, L7_char8 * value, L7_char8 * excluded, L7_char8 * errorlog);

/*********************************************************************
*
* @purpose Checks whether the srcl4port should show the Port Number
*          Start and Port Number End fields
*
* @param   none
*
* @returns L7_SUCCESS   Yes, show fields
*          L7_FAILURE   No, do not show fields
*
* @end
*
*********************************************************************/
L7_RC_t usmWebl4portShowPortNumberFieldsGet();

/*********************************************************************
*
* @purpose Checks whether the protocol should show the Port Number
*          Start and Port Number End fields
*
* @param   none
*
* @returns L7_SUCCESS   Yes, show fields
*          L7_FAILURE   No, do not show fields
*
* @end
*
*********************************************************************/
L7_RC_t usmDbShowProtocolNumberFieldGet();

/*********************************************************************
*
* @purpose Generate the list Class of Service numbers (0-7)
*
* @param
*
* @returns
*
* @end
*
*********************************************************************/
L7_char8 *usmWebDiffServClassSummaryInfoGet(L7_uint32 unit, L7_uint32 classIndex);

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
L7_char8 *usmWebDiffServPolicyAttrInfoGet(L7_uint32 unit, usmWeb_diffServPolicyAttrEntry_t policyAttr_itr);

#endif
