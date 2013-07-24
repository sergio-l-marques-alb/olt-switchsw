/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_tlv.h
*
* @purpose    DiffServ component TLV definitions header file
*
* @component  diffserv
*
* @comments   This header is for the private use of the DiffServ Distiller
*
* @create     07/22/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/
#ifndef INCLUDE_DIFFSERV_TLV_H
#define INCLUDE_DIFFSERV_TLV_H

#include "l7_common.h"
#include "l7_product.h"
#include "l7_diffserv_api.h"
#include "diffserv_config.h"
#include "diffserv_prvtmib.h"
#include "tlv_api.h"


/* DiffServ TLV Policy Presentation (normally combined TLV)
 * NOTE: this value is used unless overridden by platform.h definition
 */
#ifndef L7_DIFFSERV_TLV_POLICY_COMBINED
  #define L7_DIFFSERV_TLV_POLICY_COMBINED       L7_TRUE
#endif

/* general constants */
#define L7_QOS_DIFFSERV_TLV_BLOCK_SIZE_MAX      65536   /* largest block size */
#define L7_QOS_DIFFSERV_PARSE_MARGIN_STEP       2       /* for parsing display*/
#define L7_QOS_DIFFSERV_PARSE_MARGIN_MAX        20      /* for parsing display*/


/* the following union covers all TLV native value overlays defined above 
 * (i.e., those TLVs defined with a nonzero native length)
 * NOTE: this is necessary to derive the appropriate work buffer size
 */
typedef union
{
  /* QOS DiffServ Functional Category TLVs */
  diffServTlvClsfInst_t                 clsfInst;     
  diffServTlvInstDelList_t              instDelList;  
  diffServTlvPolicyAdd_t                policyAdd;  

  /* Class Rule Specification TLVs */
  diffServTlvClassRuleDef_t             classRuleDef; 
  diffServTlvMatchCos_t                 cos;          
  diffServTlvMatchCos2_t                cos2;          
  diffServTlvMatchDstIp_t               dstIp;        
  diffServTlvMatchDstIpv6_t             dstIpv6;        
  diffServTlvMatchDstL4Port_t           dstL4Port;    
  diffServTlvMatchDstMac_t              dstMac;       
  diffServTlvMatchEtype_t               etype;       
  diffServTlvMatchIpv6FlowLabel_t       ipv6FlowLabel;
  diffServTlvMatchIpTos_t               ipTos;        
  diffServTlvMatchProtocol_t            protocol;     
  diffServTlvMatchSrcIp_t               srcIp;        
  diffServTlvMatchSrcIpv6_t             srcIpv6;        
  diffServTlvMatchSrcL4Port_t           srcL4Port;    
  diffServTlvMatchSrcMac_t              srcMac;       
  diffServTlvMatchVlanId_t              vlanId;         
  diffServTlvMatchVlanId2_t             vlanId2;         

  /* Policy Attribute Specification TLVs */
  diffServTlvAttrAssignQueue_t          assignQueue;        
  diffServTlvAttrBwAlloc_t              bwAlloc;        
  diffServTlvAttrMarkCosVal_t           markCos;
  diffServTlvAttrMarkCos2Val_t          markCos2;
  diffServTlvAttrMarkCosValAsCos2_t markCosAsCos2;
  diffServTlvAttrMarkIpDscpVal_t        markIpDscp;
  diffServTlvAttrMarkIpPrecedenceVal_t  markIpPrecedence;
  diffServTlvAttrMirror_t               mirror;
  diffServTlvAttrPoliceDef_t            policeDef;      
  diffServTlvAttrPoliceConform_t        policeConform;      
  diffServTlvAttrPoliceExceed_t         policeExceed;      
  diffServTlvAttrPoliceNonconform_t     policeNonconform;      
  diffServTlvAttrRandomDrop_t           randomDrop;            /* deprecated */
  diffServTlvAttrRedirect_t             redirect;              
  diffServTlvAttrShaping_t              shaping;               /* deprecated */

} dsDstlTlvNative_t;

#define L7_QOS_DIFFSERV_TLV_VAL_SIZE_MAX    sizeof(dsDstlTlvNative_t)


/* TLV working info structure
 *
 * NOTE CAREFULLY!!!  This is not an actual TLV per se, but contains the type,
 * length, and value information that gets passed to the TLV utility APIs.
 * As such, the type and length fields are represented in host byte order,
 * but the value buffer contents are built in network byte order in preparation
 * for the TLV API call.
 */
typedef struct
{
  L7_uint32         tlvType;                                    /* host order */
  L7_uint32         tlvLen;                                     /* host order */
  L7_uchar8         tlvVal[L7_QOS_DIFFSERV_TLV_VAL_SIZE_MAX];   /* net order  */
} dsDstlTlvWorkInfo_t;


/* rule order list information */
typedef struct 
{
  L7_uint32             classIndex;             /* class index key            */
  L7_uint32             classRuleIndex;         /* class rule index key       */
  dsmibClassRuleType_t  classRuleType;          /* class rule entry type      */
  L7_BOOL               deny;                   /* no Diffserv if rule matches*/
  L7_BOOL               open;                   /* open new TLV nest level    */
  L7_BOOL               close;                  /* close current TLV nest lvl */
  L7_BOOL               forceEvery;             /* set 'match every' TLV rule */
  L7_BOOL               needIpEtype;            /* set IPv4 Etype TLV rule    */
} dsDstlRuleListElem_t;

typedef struct
{
  L7_uint32             ruleCount;              /* number of rules in list    */
  L7_BOOL               needSummaryOpen;        /* flag for class type 'all'  */
  dsDstlRuleListElem_t  list[DSMIB_CLASS_REF_NESTED_RULE_MAX];  /* rule list  */
} dsDstlRuleOrder_t; 


/* Distiller TLV internal function prototypes */
L7_RC_t dsDstlTlvPolicyAddSeparate(L7_uint32 policyIndex, L7_uint32 intIfNum,
                                   dsmibServiceIfDir_t ifDirection);
L7_RC_t dsDstlTlvPolicyAddCombined(L7_uint32 policyIndex, L7_uint32 intIfNum,
                                   dsmibServiceIfDir_t ifDirection);
L7_RC_t dsDstlTlvPolicyCombinedTlvGet(L7_uint32 policyIndex, L7_uint32 intIfNum,
                                      dsmibServiceIfDir_t ifDirection, L7_tlvHandle_t tlvHandle);
L7_RC_t dsDstlTlvPolicyAddInstBuild(L7_tlvHandle_t tlvHandle,
                                    L7_uint32 instanceKey,
                                    L7_uint32 policyIndex, 
                                    L7_uint32 policyInstIndex,
                                    L7_uint32 intIfNum);
L7_RC_t dsDstlTlvPolicyAddTlvIssue(L7_tlvHandle_t tlvHandle, 
                                   L7_uint32 intIfNum,
                                   dsmibServiceIfDir_t ifDirection);
L7_RC_t dsDstlTlvClassDefBuild(L7_tlvHandle_t tlvHandle, 
                               L7_uint32 policyIndex,
                               L7_uint32 policyInstIndex);
L7_RC_t dsDstlTlvRuleListBuild(L7_uint32 classIndex, 
                               dsDstlRuleOrder_t *pRuleOrderInfo,
                               L7_uint32 depth);
L7_RC_t dsDstlTlvRuleListBuildAcl(L7_uint32 classIndex, 
                                  dsDstlRuleOrder_t *pRuleOrderInfo);
void dsDstlTlvRuleListRefine(dsDstlRuleOrder_t *pRuleOrderInfo);
L7_RC_t dsDstlTlvMaskableFieldCheck(L7_uint32 classIndex, 
                                    L7_uint32 classRuleIndex);
L7_RC_t dsDstlTlvIpFieldCheck(L7_uint32 classIndex, 
                              L7_uint32 classRuleIndex);
L7_RC_t dsDstlTlvClassMatchInfoGet(L7_uint32 classIndex, 
                                   L7_uint32 classRuleIndex,
                                   dsDstlTlvWorkInfo_t *pTlvWorkInfo);
L7_RC_t dsDstlTlvPolicyDefBuild(L7_tlvHandle_t tlvHandle, 
                                L7_uint32 policyIndex,
                                L7_uint32 policyInstIndex,
                                L7_uint32 intIfNum);
L7_RC_t dsDstlTlvPolicyAttrBuild(L7_tlvHandle_t tlvHandle,
                                 L7_uint32 policyIndex, 
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 policyAttrIndex,
                                 L7_uint32 intIfNum,
                                 dsDstlTlvWorkInfo_t *pTlvWorkInfo);
L7_RC_t dsDstlTlvPolicyBestEffortBuild(L7_tlvHandle_t tlvHandle, 
                                       dsDstlTlvWorkInfo_t *pTlvWorkInfo);
L7_RC_t dsDstlTlvPoliceSimple(L7_tlvHandle_t tlvHandle,
                              L7_uint32 policyIndex, 
                              L7_uint32 policyInstIndex,
                              L7_uint32 policyAttrIndex,
                              dsDstlTlvWorkInfo_t *pTlvWorkInfo);
L7_RC_t dsDstlTlvPoliceSingleRate(L7_tlvHandle_t tlvHandle,
                                  L7_uint32 policyIndex, 
                                  L7_uint32 policyInstIndex,
                                  L7_uint32 policyAttrIndex,
                                  dsDstlTlvWorkInfo_t *pTlvWorkInfo);
L7_RC_t dsDstlTlvPoliceTwoRate(L7_tlvHandle_t tlvHandle,
                               L7_uint32 policyIndex, 
                               L7_uint32 policyInstIndex,
                               L7_uint32 policyAttrIndex,
                               dsDstlTlvWorkInfo_t *pTlvWorkInfo);
L7_RC_t dsDstlTlvPoliceConformBuild(L7_tlvHandle_t tlvHandle,
                                    L7_uint32 dataRate,
                                    L7_uint32 burstSize,
                                    L7_uint32 markAct,
                                    L7_uint32 markVal,
                                    dsDstlTlvWorkInfo_t *pTlvWorkInfo);
L7_RC_t dsDstlTlvPoliceExceedBuild(L7_tlvHandle_t tlvHandle,
                                   L7_uint32 dataRate,
                                   L7_uint32 burstSize,
                                   L7_uint32 markAct,
                                   L7_uint32 markVal,
                                   dsDstlTlvWorkInfo_t *pTlvWorkInfo);
L7_RC_t dsDstlTlvPoliceNonconformBuild(L7_tlvHandle_t tlvHandle,
                                       L7_uint32 markAct,
                                       L7_uint32 markVal,
                                       dsDstlTlvWorkInfo_t *pTlvWorkInfo);
L7_uint32 dsDstlTlvPoliceActionXlate(dsmibPoliceAct_t action);
void dsDstlTlvInstToKey(L7_uint32 policyIndex, 
                        L7_uint32 policyInstIndex, 
                        L7_uint32 *pKey);
void dsDstlTlvKeyToInst(L7_uint32 key, 
                        L7_uint32 *pPolicyIndex,
                        L7_uint32 *pPolicyInstIndex);
L7_RC_t dsDstlTlvParse(L7_tlv_t *pTlv, L7_uint32 intIfNum);
L7_RC_t dsDstlTlvEntryDisplay(L7_tlv_t *pTlv, L7_uint32 nestLvl, 
                              L7_uint32 *pEntrySize);

#endif /* INCLUDE_DIFFSERV_TLV_H */
