/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   diffserv_config.c
*
* @purpose    DiffServ Component Configuration Functions
*
* @component  DiffServ
*
* @comments   none
*
* @create     04/23/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/
#include "l7_diffserv_include.h"
#include "usmdb_mib_diffserv_common.h"
#include "usmdb_mib_diffserv_private_api.h"

/* file globals */
static char     *pCfgFileName = L7_DIFFSERV_CFG_FILENAME;

/* Function prototypes */                                       
static L7_RC_t diffServClassMibObjectsSet(L7_uint32 classIndex,
                                          L7_diffServClassHdr_t *pHdr);
static L7_RC_t diffServClassRuleMibObjectsSet(L7_uint32 classIndex, 
                                              L7_uint32 classRuleIndex,
                                              L7_diffServClassRule_t *pRule);
static L7_RC_t diffServPolicyMibObjectsSet(L7_uint32 policyIndex, 
                                           L7_diffServPolicyHdr_t *pHdr);
static L7_RC_t diffServPolicyInstMibObjectsSet(L7_uint32 policyIndex, 
                                               L7_uint32 policyInstIndex,  
                                               L7_diffServPolicyInst_t *pInst);
static L7_RC_t diffServPolicyAttrMibObjectsSet(L7_uint32 policyIndex,
                                               L7_uint32 policyInstIndex,  
                                               L7_uint32 policyAttrIndex,
                                               L7_diffServPolicyAttr_t *pAttr);
static L7_RC_t diffServPoliceConformObjectsSet(L7_uint32 policyIndex,
                                               L7_uint32 policyInstIndex,  
                                               L7_uint32 policyAttrIndex,
                                               L7_diffServPolicyAttr_t *pAttr);
static L7_RC_t diffServPoliceExceedObjectsSet(L7_uint32 policyIndex,
                                              L7_uint32 policyInstIndex,  
                                              L7_uint32 policyAttrIndex,
                                              L7_diffServPolicyAttr_t *pAttr);
static L7_RC_t diffServPoliceNonconformObjectsSet(L7_uint32 policyIndex,
                                                  L7_uint32 policyInstIndex,  
                                                  L7_uint32 policyAttrIndex,
                                                  L7_diffServPolicyAttr_t *pAttr);
static L7_RC_t diffServPoliceColorConformObjectsSet(L7_uint32 policyIndex,
                                                    L7_uint32 policyInstIndex,  
                                                    L7_uint32 policyAttrIndex,
                                                    L7_diffServPolicyAttr_t *pAttr);
static L7_RC_t diffServPoliceColorExceedObjectsSet(L7_uint32 policyIndex,
                                                   L7_uint32 policyInstIndex,  
                                                   L7_uint32 policyAttrIndex,
                                                   L7_diffServPolicyAttr_t *pAttr);
static L7_RC_t diffServGetRowActivateStatus(L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t rowStatus,
                                            L7_BOOL *pActivateRow);
static L7_RC_t diffServConfigError(L7_uint32 errLine, const char *routineName);


/*********************************************************************
* @purpose  Build DiffServ user config data  
*
* @param    ver         @b{(input)} Software version of Config Data
* @param    *pCfgData   @b{(input)} Ptr to location to store Config Data
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes    Collects all user configuration information (mostly from
*           the private MIB) and creates a formatted DiffServ config 
*           file.
*
* @end
*********************************************************************/
L7_RC_t diffServBuildUserConfigData(L7_uint32 ver, L7_diffServCfg_t *pCfgData)
{
  dsmibClassEntryCtrl_t                *pClassRow;
  dsmibClassRuleEntryCtrl_t            *pClassRuleRow;
  dsmibPolicyEntryCtrl_t               *pPolicyRow;
  dsmibPolicyInstEntryCtrl_t           *pPolicyInstRow;
  dsmibPolicyAttrEntryCtrl_t           *pPolicyAttrRow;
  dsmibServiceEntryCtrl_t              *pSvcRow;
  L7_diffServClassHdr_t                *pClassHdr;
  L7_diffServClassRule_t               *pRule;
  L7_diffServPolicyHdr_t               *pPolicyHdr;
  L7_diffServPolicyInst_t              *pInst;
  L7_diffServPolicyAttr_t              *pAttr;
  L7_diffServService_t                 *pSvc;
  L7_uint32                            classIndex;
  L7_uint32                            nextClassIndex;
  L7_uint32                            classRuleIndex;
  L7_uint32                            policyIndex;
  L7_uint32                            nextPolicyIndex;
  L7_uint32                            policyInstIndex;
  L7_uint32                            nextPolicyInstIndex;
  L7_uint32                            policyAttrIndex;
  L7_uint32                            intIfNum;
  L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection;
  L7_uint32                            i,j,k;
  L7_diffServIntfCfg_t                 *pIntfCfg;
  L7_uint32                            cfgIndex;
  nimConfigID_t                        configId[L7_DIFFSERV_INTF_MAX_COUNT];

  if (L7_NULLPTR == pCfgData)
  {
    LOG_MSG("%s %d: %s: Error", __FILE__, __LINE__, __FUNCTION__);
    return L7_FAILURE;
  }

  memset(&configId[0], 0, sizeof(nimConfigID_t) * L7_DIFFSERV_INTF_MAX_COUNT);

  /* always copy configIDs FROM the pDiffServCfgData_g global struct */
  for (cfgIndex = 1; cfgIndex < L7_DIFFSERV_INTF_MAX_COUNT; cfgIndex++)
    NIM_CONFIG_ID_COPY(&configId[cfgIndex], &pDiffServCfgData_g->diffServIntfCfg[cfgIndex].configId);

  memset(pCfgData, 0, L7_DIFFSERV_CFG_DATA_SIZE);  

  /* copy saved configIDs TO the config data struct specified by the caller */
  for (cfgIndex = 1; cfgIndex < L7_DIFFSERV_INTF_MAX_COUNT; cfgIndex++)
    NIM_CONFIG_ID_COPY(&pCfgData->diffServIntfCfg[cfgIndex].configId, &configId[cfgIndex]);

  /* build config file header */
  osapiStrncpySafe((char *)pCfgData->cfgHdr.filename, pCfgFileName,
                   sizeof(pCfgData->cfgHdr.filename));
  pCfgData->cfgHdr.version      = ver;
  pCfgData->cfgHdr.componentID  = L7_FLEX_QOS_DIFFSERV_COMPONENT_ID;
  pCfgData->cfgHdr.type         = L7_CFG_DATA;
  pCfgData->cfgHdr.length       = (L7_uint32)L7_DIFFSERV_CFG_DATA_SIZE;
  pCfgData->cfgHdr.dataChanged  = L7_FALSE;

  /* build DiffServ globals */
  diffServAdminModeGet(&pCfgData->adminMode);
  pCfgData->traceMode = diffServTraceModeGet();
  pCfgData->msgLvl = diffServMsgLvlGet();

  /* build class configuration:
   *
   * In the config file, the class and class rule information are each contained
   * in densely-packed arrays.  A class entry contains a chainIndex indicating
   * its first class rule entry (or 0 if no rules exist for the class).  Each 
   * class rule entry contains a chainIndex continuing the singly-linked list
   * started by its class, with a 0 indicating the end of that chain.
   */
  classIndex = 0;
  i = L7_DIFFSERV_CLASS_HDR_START;
  j = L7_DIFFSERV_CLASS_RULE_START;
  while (diffServClassGetNext(classIndex, &classIndex) == L7_SUCCESS 
         &&  i < L7_DIFFSERV_CLASS_HDR_MAX)
  {
    pClassRow = dsmibClassRowFind(classIndex);
    if (pClassRow == L7_NULLPTR)
    {
      LOG_MSG("%s %d: %s: Error", __FILE__, __LINE__, __FUNCTION__);
      return L7_FAILURE;
    }

    pClassHdr = &pCfgData->classHdr[i];

    pClassHdr->inUse = L7_TRUE;
    pClassHdr->chainIndex = j;
    pClassHdr->mib = pClassRow->mib;
    pClassHdr->rowInvalidFlags = pClassRow->rowInvalidFlags;

    classRuleIndex = 0;
    pRule = L7_NULLPTR;
    while ((diffServClassRuleGetNext(classIndex, classRuleIndex, &nextClassIndex,
                                     &classRuleIndex) == L7_SUCCESS) 
           && (classIndex == nextClassIndex) && (j < L7_DIFFSERV_CLASS_RULE_MAX) )
    {
      pClassRuleRow = dsmibClassRuleRowFind(classIndex, classRuleIndex);
      if (pClassRuleRow == L7_NULLPTR)
      {
        LOG_MSG("%s %d: %s: Error", __FILE__, __LINE__, __FUNCTION__);
        return L7_FAILURE;
      }

      pRule = &pCfgData->classRule[j];

      pRule->hdrIndexRef = i;
      pRule->chainIndex = j+1;
      pRule->mib = pClassRuleRow->mib;
      pRule->rowInvalidFlags = pClassRuleRow->rowInvalidFlags;
      pRule->arid = pClassRuleRow->arid;
      j++;
    }/* endwhile class rule */

    if (pRule != L7_NULLPTR)
      pRule->chainIndex = 0;
    else
      pClassHdr->chainIndex = 0;

    i++;
  }/* endwhile class */

  /* build policy configuration:
   *
   * In the config file, the policy, policy instance and policy attribute 
   * entries are each contained in densely-packed arrays.  A policy entry 
   * contains a chainIndex indicating its first policy instance entry (or
   * 0 if no instances exist for the policy).  Each policy instance entry 
   * contains a chainIndex continuing the singly-linked list started by
   * its policy, with a 0 indicating the end of that chain.
   *
   * Likewise, policy instance and policy attribute entries each contain an 
   * attrChainIndex which is used to designate a singly-linked list of 
   * attributes belonging to each instance, with a 0 value indicating the
   * end (or complete absence) of the attribute chain.
   */
  i = L7_DIFFSERV_POLICY_HDR_START;
  j = L7_DIFFSERV_POLICY_INST_START;
  k = L7_DIFFSERV_POLICY_ATTR_START;

  policyIndex = 0;
  while (diffServPolicyGetNext(policyIndex, &policyIndex) == L7_SUCCESS &&
         i < L7_DIFFSERV_POLICY_HDR_MAX)
  {
    pPolicyRow = dsmibPolicyRowFind(policyIndex);
    if (pPolicyRow == L7_NULLPTR)
    {
      LOG_MSG("%s %d: %s: Error", __FILE__, __LINE__, __FUNCTION__);
      return L7_FAILURE;
    }

    pPolicyHdr = &pCfgData->policyHdr[i];

    pPolicyHdr->chainIndex = j;
    pPolicyHdr->inUse = L7_TRUE;
    pPolicyHdr->mib = pPolicyRow->mib;
    pPolicyHdr->rowInvalidFlags = pPolicyRow->rowInvalidFlags;

    policyInstIndex = 0; 
    pInst = L7_NULLPTR;
    while ((diffServPolicyInstGetNext(policyIndex, policyInstIndex, &nextPolicyIndex, 
                                      &policyInstIndex) == L7_SUCCESS) &&
           (policyIndex == nextPolicyIndex) && (j < L7_DIFFSERV_POLICY_INST_MAX) )
    {
      pPolicyInstRow = dsmibPolicyInstRowFind(policyIndex, policyInstIndex);
      if (pPolicyInstRow == L7_NULLPTR)
      {
        LOG_MSG("%s %d: %s: Error", __FILE__, __LINE__, __FUNCTION__);
        return L7_FAILURE;
      }

      pInst = &pCfgData->policyInst[j];

      pInst->hdrIndexRef = i;
      pInst->attrChainIndex  = k;
      pInst->chainIndex = j+1;

      pInst->mib = pPolicyInstRow->mib;
      pInst->rowInvalidFlags = pPolicyInstRow->rowInvalidFlags;

      policyAttrIndex = 0;
      pAttr = L7_NULLPTR;
      while ((diffServPolicyAttrGetNext(policyIndex, policyInstIndex, 
                                        policyAttrIndex, 
                                        &nextPolicyIndex, &nextPolicyInstIndex, 
                                        &policyAttrIndex) == L7_SUCCESS)
             && (nextPolicyIndex == policyIndex) 
             && (nextPolicyInstIndex == policyInstIndex)
             && k < L7_DIFFSERV_POLICY_ATTR_MAX)
      {
        pPolicyAttrRow = dsmibPolicyAttrRowFind(policyIndex, policyInstIndex, 
                                                policyAttrIndex);
        if (pPolicyAttrRow == L7_NULLPTR)
        {
          LOG_MSG("%s %d: %s: Error", __FILE__, __LINE__, __FUNCTION__);
          return L7_FAILURE;
        }

        pAttr = &pCfgData->policyAttr[k];

        pAttr->instIndexRef = j;
        pAttr->chainIndex = k+1;
        pAttr->mib = pPolicyAttrRow->mib;
        pAttr->rowInvalidFlags = pPolicyAttrRow->rowInvalidFlags;
        k++;
      } /* endwhile policy attribute */

      if (pAttr != L7_NULLPTR)
        pAttr->chainIndex = 0;
      else
        pInst->attrChainIndex = 0;

      j++;
    }/* endwhile policy instance */

    if (pInst != L7_NULLPTR)
      pInst->chainIndex = 0;
    else
      pPolicyHdr->chainIndex = 0;

    i++;
  }/* endwhile policy */

  /* build service configuration:
   *
   * The service configuration information is densly packed, and is 
   * organized by configID and is separated into inbound and outbound
   * services
   */
  intIfNum = 0;
  ifDirection = 0;
  while (( diffServServiceGetNext(intIfNum, ifDirection, &intIfNum, 
                                  &ifDirection) == L7_SUCCESS)
         && (diffServMapIntfIsConfigurable(intIfNum, L7_NULLPTR) == L7_TRUE))
  {
    pSvcRow = dsmibServiceRowFind(intIfNum, ifDirection);
    if (pSvcRow == L7_NULLPTR)
    {
      LOG_MSG("%s %d: %s: Error", __FILE__, __LINE__, __FUNCTION__);
      return L7_FAILURE;
    }

    cfgIndex = diffServMapTbl_g[intIfNum];
    pIntfCfg = &pCfgData->diffServIntfCfg[cfgIndex];

    if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
      pSvc = &pIntfCfg->serviceIn;
    else
      pSvc = &pIntfCfg->serviceOut;
    pSvc->inUse = L7_TRUE;
    pSvc->mib = pSvcRow->mib;
    pSvc->rowInvalidFlags = pSvcRow->rowInvalidFlags; 
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configure DiffServ global configuration parameters
*
* @param    void 
*
* @returns  L7_SUCCESS  Config data applied
* @returns  L7_FAILURE  Unexpected condition encountered
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t diffServGlobalConfig(void)
{
  /* NOTE: the admin mode is handled separately */
  diffServTraceModeSet(pDiffServCfgData_g->traceMode);
  diffServMsgLvlSet(pDiffServCfgData_g->msgLvl);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configure Classes and Class Rules
*
* @param    void 
*
* @returns  L7_SUCCESS  Config data applied
* @returns  L7_FAILURE  Unexpected condition encountered
*
* @notes    Set up private MIB Class Group table rows from config file 
*           contents.
*
* @notes    Requires activation of DiffServ 'special config mode' prior
*           to invoking this function.
*
* @end
*********************************************************************/
L7_RC_t diffServClassConfig(void)
{
  L7_uint32                   i, j;  
  L7_uint32                   classIndex;
  L7_uint32                   classRuleIndex;
  L7_uint32                   chainIndex;
  L7_BOOL                     activateRow;
  L7_diffServClassHdr_t       *pHdr, *pTmpHdr;
  L7_diffServClassRule_t      *pRule;
  dsmibRowStatus_t            rowStatus;
  dsmibClassEntryCtrl_t       *pClass;
  dsmibClassRuleEntryCtrl_t   *pClassRule;

  /* NOTE: Must process the class header info twice -- the first time to 
   *       establish all class rows, then to set up all rules for each 
   *       class.  This is necessary so that the reference class rules
   *       can be processed (they require that the class being referenced
   *       already exists). 
   */

  /* set up each class row */
  for (i = L7_DIFFSERV_CLASS_HDR_START; i < L7_DIFFSERV_CLASS_HDR_MAX; i++)
  {
    pHdr = &pDiffServCfgData_g->classHdr[i];

    /* Array entry not in use means end of the array list */
    if (pHdr->inUse != L7_TRUE)
      break;

    /* check each class name against all subsequent class names
     * (it's more efficient to do this here using the config struct rather
     * than during the normal setTest processing when setting up the MIB objects)
     */
    for (j = i + 1; j < L7_DIFFSERV_CLASS_HDR_MAX; j++)
    {
      pTmpHdr = &pDiffServCfgData_g->classHdr[j];
      if (pTmpHdr->inUse == L7_TRUE)
      {
        if (strcmp(pHdr->mib.name, pTmpHdr->mib.name) == 0)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
    }

    /* Get activate status for this class */
    rowStatus = pHdr->mib.rowStatus;
    if (diffServGetRowActivateStatus(rowStatus, &activateRow) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }

    /* Create a class with the given class Index */
    classIndex = pHdr->mib.index;
    if (diffServClassCreate(classIndex, activateRow) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }

    /* get access to the class row */
    pClass = dsmibClassRowFind(classIndex);
    if (pClass == L7_NULLPTR)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }

    /* Set all MIB objects */
    if (diffServClassMibObjectsSet(classIndex, pHdr) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }

    /* Check for consistency of row status */
    if (diffServClassObjectGet(classIndex, L7_DIFFSERV_CLASS_ROW_STATUS,
                               &rowStatus) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
    if ((pHdr->mib.rowStatus != rowStatus) &&
        (pHdr->mib.rowStatus != pClass->mib.rowStatus))
    {
      LOG_MSG("Inconsistent row status value: expected=%u, actual=%u\n", 
              pHdr->mib.rowStatus, rowStatus);
      return diffServConfigError(__LINE__, __FUNCTION__);
    }

  } /* endfor class row */

  /* set up the class rule row(s) for each class */
  for (i = L7_DIFFSERV_CLASS_HDR_START; i < L7_DIFFSERV_CLASS_HDR_MAX; i++)
  {
    pHdr = &pDiffServCfgData_g->classHdr[i];

    /* Array entry not in use means end of the array list */
    if (pHdr->inUse != L7_TRUE)
      break;

    classIndex = pHdr->mib.index;

    /* Set Class Rule Table objects */
    chainIndex = pHdr->chainIndex; 
    while (chainIndex != 0)
    {
      pRule = &pDiffServCfgData_g->classRule[chainIndex];
      /* Consistency check on Class Rule config data*/
      if (pRule->hdrIndexRef != i)
      {
        LOG_MSG("Corrupted contents for config file %s\n", pCfgFileName);
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Get activate status for this Class Rule*/
      rowStatus = pRule->mib.rowStatus;
      if (diffServGetRowActivateStatus(rowStatus, &activateRow) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Create a Class Rule */
      classRuleIndex = pRule->mib.index; 
      if (diffServClassRuleCreate(classIndex, classRuleIndex, activateRow) 
          != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* get access to the class rule row */
      pClassRule = dsmibClassRuleRowFind(classIndex, classRuleIndex);
      if (pClassRule == L7_NULLPTR)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Set Access Rule ID (ARID) field */
      pClassRule->arid = pRule->arid;

      /* Set all MIB objects */
      if (diffServClassRuleMibObjectsSet(classIndex,classRuleIndex, 
                                         pRule) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Check for consistency of row status */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                     L7_DIFFSERV_CLASS_RULE_ROW_STATUS,
                                     &rowStatus) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
      if ((pRule->mib.rowStatus != rowStatus) &&
          (pRule->mib.rowStatus != pClassRule->mib.rowStatus))
      {
        LOG_MSG("Inconsistent row status value: expected=%u, actual=%u\n", 
                pRule->mib.rowStatus, rowStatus);
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
      chainIndex = pRule->chainIndex; 

    } /* endwhile */

  } /* endfor class rule row(s) */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the values of objects in the Class Table 
*
* @param    classIndex  @b{(input)}  Class index
* @param    pHdr        @b{(input)}  Pointer to class config structure
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t diffServClassMibObjectsSet(L7_uint32 classIndex,
                                          L7_diffServClassHdr_t *pHdr)
{
  L7_uchar8     *pName;  

  /* Storage Type */
  if (diffServClassObjectSet(classIndex, 
                             L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_STORAGE_TYPE, 
                             (void *)&pHdr->mib.storageType) != L7_SUCCESS)
  {
    return diffServConfigError(__LINE__, __FUNCTION__);
  }

  /* Class name */
  if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pHdr, DSMIB_CLASS_FLAGS_NAME))
  {
    pName = pHdr->mib.name;
    if (diffServClassObjectSet(classIndex, 
                               L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_NAME, 
                               (void *)&pName) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
  }

  /* Class Type */
  if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pHdr, DSMIB_CLASS_FLAGS_TYPE))
  {
    if (diffServClassObjectSet(classIndex, 
                               L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE,
                               (void *)&pHdr->mib.type) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
  }

  /* Class L3 Protocol */
  if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pHdr, DSMIB_CLASS_FLAGS_L3PROTO))
  {
    if (diffServClassObjectSet(classIndex, 
                               L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_L3_PROTO,
                               (void *)&pHdr->mib.l3Protocol) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
  }

  /* ACL Type and Number */
  if (pHdr->mib.type == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
  {
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pHdr, DSMIB_CLASS_FLAGS_ACLTYPE))
    {
      if (diffServClassObjectSet(classIndex,
                                 L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ACL_TYPE, 
                                 (void *)&pHdr->mib.aclType) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }

    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pHdr, DSMIB_CLASS_FLAGS_ACLNUM))
    {
      if (diffServClassObjectSet(classIndex,
                                 L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ACL_NUMBER, 
                                 (void *)&pHdr->mib.aclNum) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the values of objects in the Class Rule Table 
*
* @param    classIndex      @b{(input)}  Class index
* @param    classRuleIndex  @b{(input)}  Class rule index
* @param    pRule           @b{(input)}  Pointer to class rule config structure
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t diffServClassRuleMibObjectsSet(L7_uint32 classIndex, 
                                              L7_uint32 classRuleIndex,
                                              L7_diffServClassRule_t *pRule)
{
  L7_uchar8     *pVal8;

  /* Storage Type */
  if (diffServClassRuleObjectSet(classIndex, classRuleIndex, 
                                 L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_STORAGE_TYPE,
                                 (void *)&pRule->mib.storageType) != L7_SUCCESS)
  {
    return diffServConfigError(__LINE__, __FUNCTION__);
  }


  /* Class Rule Type */
  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE))
  {
    /* if the entry type is not set, none of the other flags are meaningful */
    return L7_SUCCESS;
  }

  if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                 L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ENTRY_TYPE, 
                                 (void *)&pRule->mib.entryType) != L7_SUCCESS)
  {
    return diffServConfigError(__LINE__, __FUNCTION__);
  }

  /* Exclude Flag */
  if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG))
  {
    if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                   L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_EXCLUDE_FLAG, 
                                   (void *)&pRule->mib.excludeFlag) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
  }

  switch (pRule->mib.entryType)
  {
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS:
    /* Class Of Service Type */
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_COS))
    {
      if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                     L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_COS, 
                                     (void *)&pRule->mib.match.cos) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2:
    /* Secondary Class Of Service Type */
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_COS))
    {
      if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                     L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_COS2, 
                                     (void *)&pRule->mib.match.cos2) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }
    break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
      /* Dest IP addr */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR))
      {
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex, 
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIP_ADDR, 
                                       (void *)&pRule->mib.match.dstIp.addr) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* Dest IP Mask */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK))
      {
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIP_MASK, 
                                       (void *)&pRule->mib.match.dstIp.mask) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
    /* Dest IPv6 addr */
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6ADDR))
    {
      if (diffServClassRuleObjectSet(classIndex, classRuleIndex, 
                                     L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIPV6_ADDR, 
                                     (void *)&pRule->mib.match.dstIpv6.ip6prefix.in6Addr) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }

    /* Dest IPv6 Prefix Length */
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6PLEN))
    {
      if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                     L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIPV6_ADDR_PLEN, 
                                     (void *)&pRule->mib.match.dstIpv6.ip6prefix.in6PrefixLen) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }
    break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
      /* Dest Port range - Start */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START))
      {
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex, 
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTL4PORT_START, 
                                       (void *)&pRule->mib.match.dstL4Port.start) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* Dest Port range - End */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END))
      {
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTL4PORT_END,
                                       (void *)&pRule->mib.match.dstL4Port.end) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC:
      /* Dest MAC Addr */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR))
      {
        pVal8 = pRule->mib.match.dstMac.addr;
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTMAC_ADDR, 
                                       (void *)pVal8) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* Dest MAC mask */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK))
      {
        pVal8 = pRule->mib.match.dstMac.mask;  
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTMAC_MASK, 
                                       (void *)pVal8) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE:
    /* Ethertype keyword */
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_KEYID))
    {
      if (diffServClassRuleObjectSet(classIndex, classRuleIndex, 
                                     L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ETYPE_KEYID, 
                                     (void *)&pRule->mib.match.etype.keyid) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }

    /* Ethertype value (only if custom keyword is used) */
    if (pRule->mib.match.etype.keyid == L7_QOS_ETYPE_KEYID_CUSTOM)
    {
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_VALUE))
      {
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex, 
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ETYPE_VALUE, 
                                       (void *)&pRule->mib.match.etype.value) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
    }
    break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY:
      /* This value is already being set to `TRUE` when `entry type` is set */
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
      /* IPv6 Flow Label */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6FLOWLBL))
      {
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex, 
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_FLOWLBLV6, 
                                       (void *)&pRule->mib.match.ipv6FlowLabel) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
      /* IP DSCP */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_IPDSCP))
      {
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex, 
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPDSCP, 
                                       (void *)&pRule->mib.match.ipDscp) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
      /* IP Precedence */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_IPPRECEDENCE))
      {
        if (diffServClassRuleObjectSet( classIndex, classRuleIndex, 
                                        L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPPRECEDENCE,
                                        (void *)&pRule->mib.match.ipPrecedence) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
      /* IP TOS Bits */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSBITS))
      {
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex, 
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPTOS_BITS, 
                                       (void *)&pRule->mib.match.ipTos.bits) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* IP TOS Mask */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSMASK))
      {
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex, 
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPTOS_MASK, 
                                       (void *)&pRule->mib.match.ipTos.mask) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
      /* IP Protocol Num */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_PROTOCOLNUM))
      {
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex, 
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_PROTOCOL_NUM,
                                       (void *)&pRule->mib.match.protocolNum) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS:
      /* Reference class */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_REFCLASSINDEX))
      {
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_REFCLASS_INDEX,
                                       (void *)&pRule->mib.match.refClassIndex) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
      /* Src IP Addr */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR))
      {
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIP_ADDR, 
                                       (void *)&pRule->mib.match.srcIp.addr) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* Src IP Mask */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK))
      {
        if (diffServClassRuleObjectSet( classIndex, classRuleIndex, 
                                        L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIP_MASK,
                                        (void *)&pRule->mib.match.srcIp.mask) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
    /* Dest IPv6 addr */
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6ADDR))
    {
      if (diffServClassRuleObjectSet(classIndex, classRuleIndex, 
                                     L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIPV6_ADDR, 
                                     (void *)&pRule->mib.match.srcIpv6.ip6prefix.in6Addr) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }

    /* Dest IPv6 Prefix Length */
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6PLEN))
    {
      if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                     L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIPV6_ADDR_PLEN, 
                                     (void *)&pRule->mib.match.srcIpv6.ip6prefix.in6PrefixLen) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }
    break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
      /* Src Port Range - Start */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START))
      {
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCL4PORT_START, 
                                       (void *)&pRule->mib.match.srcL4Port.start) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* Src Port Range - End */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END))
      {
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCL4PORT_END, 
                                       (void *)&pRule->mib.match.srcL4Port.end) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC:
      /* Src MAC Addr */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR))
      {
        pVal8 = pRule->mib.match.srcMac.addr;
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCMAC_ADDR, 
                                       (void *)pVal8) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* Src MAC mask */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK))
      {
        pVal8 = pRule->mib.match.srcMac.mask; 
        if (diffServClassRuleObjectSet(classIndex, classRuleIndex,
                                       L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCMAC_MASK, 
                                       (void *)pVal8) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID:
      /* Vlan Id Range - Start */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART))
      {
        if (diffServClassRuleObjectSet( classIndex, classRuleIndex, 
                                        L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID_START,
                                        (void *)&pRule->mib.match.vlanId.start) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* Vlan Id Range - End */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND))
      {
        if (diffServClassRuleObjectSet( classIndex, classRuleIndex, 
                                        L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID_END,
                                        (void *)&pRule->mib.match.vlanId.end) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2:
    /* Secondary VLAN Id Range - Start */
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART))
    {
      if (diffServClassRuleObjectSet( classIndex, classRuleIndex, 
                                      L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID2_START,
                                      (void *)&pRule->mib.match.vlanId2.start) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }

    /* Secondary VLAN Id Range - End */
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND))
    {
      if (diffServClassRuleObjectSet( classIndex, classRuleIndex, 
                                      L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID2_END,
                                      (void *)&pRule->mib.match.vlanId2.end) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }
    break;

    default:
      LOG_MSG("%s %d: %s: Unknown Class Rule Type (%u)",
              __FILE__, __LINE__, __FUNCTION__, pRule->mib.entryType);
      return L7_FAILURE;

  } /* endswitch */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configure Policies, Policy Instances and Policy Attributes
*
* @param    void 
*
* @returns  L7_SUCCESS  Config data applied
* @returns  L7_FAILURE  Unexpected condition encountered
*
* @notes    Set up private MIB Policy Group table rows from config file 
*           contents.
*
* @notes    Requires activation of DiffServ 'special config mode' prior
*           to invoking this function.
*
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyConfig(void)
{
  L7_uint32                   i, j;  
  L7_uint32                   policyIndex;
  L7_uint32                   policyInstIndex;
  L7_uint32                   policyAttrIndex;
  L7_uint32                   instChainIndex;
  L7_uint32                   attrChainIndex;
  L7_BOOL                     activateRow;
  L7_diffServPolicyHdr_t      *pHdr, *pTmpHdr;
  L7_diffServPolicyInst_t     *pInst;
  L7_diffServPolicyAttr_t     *pAttr;
  dsmibRowStatus_t            rowStatus;
  dsmibPolicyEntryCtrl_t      *pPolicy;
  dsmibPolicyInstEntryCtrl_t  *pPolicyInst;
  dsmibPolicyAttrEntryCtrl_t  *pPolicyAttr;

  /*--------------------------------------------*/
  /* Policy configuration                       */ 
  /*--------------------------------------------*/
  for (i = L7_DIFFSERV_POLICY_HDR_START; i < L7_DIFFSERV_POLICY_HDR_MAX; i++)
  {
    pHdr = &pDiffServCfgData_g->policyHdr[i];

    /* Array entry not in use means end of the array list*/
    if (pHdr->inUse != L7_TRUE)
      break;

    /* Only process inbound policies, since DiffServ outbound is no 
     * longer supported as of Rel I.  Note that configuration migrated
     * from previous releases will have preserved any outbound policy 
     * information, so it is ignored here.
     */
    if (pHdr->mib.type != L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN)
      continue;

    /* check each policy name against all subsequent policy names
     * (it's more efficient to do this here using the config struct rather
     * than during the normal setTest processing when setting up the MIB objects)
     */
    for (j = i + 1; j < L7_DIFFSERV_POLICY_HDR_MAX; j++)
    {
      pTmpHdr = &pDiffServCfgData_g->policyHdr[j];
      if (pTmpHdr->inUse == L7_TRUE)
      {
        if (strcmp(pHdr->mib.name, pTmpHdr->mib.name) == 0)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
    }

    /* Get activate status for this Policy Entry*/
    rowStatus = pHdr->mib.rowStatus;
    if (diffServGetRowActivateStatus(rowStatus, &activateRow) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }

    /* Create a Policy */
    policyIndex = pHdr->mib.index;
    if (diffServPolicyCreate(policyIndex, activateRow) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }

    /* get access to the policy row */
    pPolicy = dsmibPolicyRowFind(policyIndex);
    if (pPolicy == L7_NULLPTR)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }

    /* Set MIB objects */
    if (diffServPolicyMibObjectsSet(policyIndex, pHdr) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }

    /* Check for consistency of row status */
    if (diffServPolicyObjectGet(policyIndex, 
                                L7_DIFFSERV_POLICY_ROW_STATUS,
                                &rowStatus) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
    if ((pHdr->mib.rowStatus != rowStatus) &&
        (pHdr->mib.rowStatus != pPolicy->mib.rowStatus))
    {
      LOG_MSG("Inconsistent row status value: expected=%u, actual=%u\n", 
              pHdr->mib.rowStatus, rowStatus);
      return diffServConfigError(__LINE__, __FUNCTION__);
    }

    /* Set Policy Instance Table Objects */
    instChainIndex = pHdr->chainIndex;
    while (instChainIndex != 0)
    {
      pInst = &pDiffServCfgData_g->policyInst[instChainIndex];

      /* Consistency check on Policy Instance config data*/
      if (pInst->hdrIndexRef != i)
      {
        LOG_MSG("Corrupted contents for config file %s\n", pCfgFileName);
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Get activate status for this Policy Instance Entry*/
      rowStatus = pInst->mib.rowStatus;
      if (diffServGetRowActivateStatus(rowStatus, &activateRow) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Create a Policy Instance */
      policyInstIndex = pInst->mib.index;
      if (diffServPolicyInstCreate(policyIndex, policyInstIndex, activateRow) 
          != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* get access to the policy inst row */
      pPolicyInst = dsmibPolicyInstRowFind(policyIndex, policyInstIndex);
      if (pPolicyInst == L7_NULLPTR)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Set MIB objects */
      if (diffServPolicyInstMibObjectsSet(policyIndex, policyInstIndex, 
                                          pInst) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Check for consistency of row status */
      if (diffServPolicyInstObjectGet(policyIndex, policyInstIndex, 
                                      L7_DIFFSERV_POLICY_INST_ROW_STATUS,
                                      &rowStatus) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
      if ((pInst->mib.rowStatus != rowStatus) &&
          (pInst->mib.rowStatus != pPolicyInst->mib.rowStatus))
      {
        LOG_MSG("Inconsistent row status value: expected=%u, actual=%u\n", 
                pInst->mib.rowStatus, rowStatus);
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Set Policy Attibute Table Objects */
      attrChainIndex = pInst->attrChainIndex; 
      while (attrChainIndex != 0)
      {
        pAttr = &pDiffServCfgData_g->policyAttr[attrChainIndex];

        /* Consistency check on Policy Attribute config data*/
        if (pAttr->instIndexRef != instChainIndex)
        {
          LOG_MSG("Corrupted contents for config file %s\n", pCfgFileName);
          return diffServConfigError(__LINE__, __FUNCTION__);
        }

        /* Get activate status for this Policy Attribute Entry*/
        rowStatus = pAttr->mib.rowStatus;
        if (diffServGetRowActivateStatus(rowStatus, &activateRow) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }

        /* Create a Policy Attribute Entry */
        policyAttrIndex = pAttr->mib.index;
        if (diffServPolicyAttrCreate(policyIndex, policyInstIndex, 
                                     policyAttrIndex, activateRow) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }

        /* get access to the policy attr row */
        pPolicyAttr = dsmibPolicyAttrRowFind(policyIndex, policyInstIndex,
                                             policyAttrIndex);
        if (pPolicyAttr == L7_NULLPTR)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }

        /* Set MIB objects */
        if (diffServPolicyAttrMibObjectsSet(policyIndex, policyInstIndex, 
                                            policyAttrIndex, 
                                            pAttr) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }

        /* Check for consistency of row status */
        if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, 
                                        policyAttrIndex,
                                        L7_DIFFSERV_POLICY_ATTR_ROW_STATUS,
                                        &rowStatus) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
        if ((pAttr->mib.rowStatus != rowStatus) &&
            (pAttr->mib.rowStatus != pPolicyAttr->mib.rowStatus))
        {
          LOG_MSG("Inconsistent row status value: expected=%u, actual=%u\n", 
                  pAttr->mib.rowStatus, rowStatus);
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
        attrChainIndex = pAttr->chainIndex; 

      } /* endwhile policy attr table objects */

      instChainIndex = pInst->chainIndex;

    } /* endwhile policy inst table objects */

  } /* endfor policy config */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the values for objects in the Policy Table 
*
* @param    policyIndex     @b{(input)} Policy index
* @param    pHdr            @b{(input)} Pointer to policy config structure
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t diffServPolicyMibObjectsSet(L7_uint32 policyIndex,
                                           L7_diffServPolicyHdr_t *pHdr)
{
  L7_uchar8     *pName;

  /* Storage Type */
  if (diffServPolicyObjectSet(policyIndex, 
                              L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_STORAGE_TYPE,
                              (void *)&pHdr->mib.storageType) != L7_SUCCESS)
  {
    return diffServConfigError(__LINE__, __FUNCTION__);
  }

  /* Policy name */
  if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pHdr, DSMIB_POLICY_FLAGS_NAME))
  {
    pName = pHdr->mib.name;
    if (diffServPolicyObjectSet(policyIndex, 
                                L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_NAME, 
                                (void *)&pName) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
  }

  /* Policy Type */
  if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pHdr, DSMIB_POLICY_FLAGS_TYPE))
  {
    if (diffServPolicyObjectSet(policyIndex, 
                                L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_TYPE, 
                                (void *)&pHdr->mib.type) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the values for objects in the Policy Instance Table 
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy Instance index
* @param    pInst           @b{(input)}  Pointer to policy instance config structure
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t diffServPolicyInstMibObjectsSet(L7_uint32 policyIndex, 
                                               L7_uint32 policyInstIndex,  
                                               L7_diffServPolicyInst_t *pInst)
{
  /* Storage Type */
  if (diffServPolicyInstObjectSet(policyIndex, policyInstIndex, 
                                  L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_STORAGE_TYPE,
                                  (void *)&pInst->mib.storageType) != L7_SUCCESS)
  {
    return diffServConfigError(__LINE__, __FUNCTION__);
  }

  /* Class Index*/
  if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pInst, DSMIB_POLICY_INST_FLAGS_CLASSINDEX))
  {
    if (diffServPolicyInstObjectSet(policyIndex, policyInstIndex, 
                                    L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_CLASS_INDEX, 
                                    (void *)&pInst->mib.classIndex) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the values for objects in the Policy AttrTable 
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy Instance index
* @param    policyAttrIndex @b{(input)}  Policy Attr index
* @param    pAttr           @b{(input)}  Pointer to policy attribute config 
*                                        structure
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t diffServPolicyAttrMibObjectsSet(L7_uint32 policyIndex,
                                               L7_uint32 policyInstIndex,  
                                               L7_uint32 policyAttrIndex,
                                               L7_diffServPolicyAttr_t *pAttr)
{
  L7_uint32         val;

  /* Storage Type */
  if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                  policyAttrIndex, 
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_STORAGE_TYPE, 
                                  (void *)&pAttr->mib.storageType) != L7_SUCCESS)
  {
    return diffServConfigError(__LINE__, __FUNCTION__);
  }

  /* Entry Type */
  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_ENTRYTYPE))
  {
    /* if the entry type is not set, none of the other flags are meaningful */
    return L7_SUCCESS;
  }

  if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                  policyAttrIndex, 
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_ENTRY_TYPE, 
                                  (void *)&pAttr->mib.entryType) != L7_SUCCESS)
  {
    return diffServConfigError(__LINE__, __FUNCTION__);
  }

  switch (pAttr->mib.entryType)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:

    /* Assign Queue ID */
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_QUEUEID))
    {
      if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                      policyAttrIndex, 
                                      L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_ASSIGN_QUEUE_QID, 
                                      (void *)&pAttr->mib.stmt.assignQueue.qid) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
    /* This value is already being set to `TRUE` when `entry type` is set */
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
    /* COS value */
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_COSVAL))
    {
      val = (L7_uint32)pAttr->mib.stmt.markCos.val; 
      if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                      policyAttrIndex,         
                                      L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_COSVAL, 
                                      (void *)&val) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
    /* COS value */
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_COS2VAL))
    {
      val = (L7_uint32)pAttr->mib.stmt.markCos2.val; 
      if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                      policyAttrIndex,         
                                      L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_COS2VAL, 
                                      (void *)&val) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
    /* COS value */
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_COS_AS_COS2))
    {
      val = (L7_uint32)pAttr->mib.stmt.markCosAsCos2.val; 
      if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                      policyAttrIndex,         
                                      L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_COS_AS_COS2, 
                                      (void *)&val) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }
    break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
      /* IP DSCP value */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_IPDSCPVAL))
      {
        val = (L7_uint32)pAttr->mib.stmt.markIpDscp.val;

        if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                        policyAttrIndex,                  
                                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_IPDSCPVAL, 
                                        (void *)&val) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
      /* IP Precedence value */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_IPPRECEDENCEVAL))
      {
        val = (L7_uint32)pAttr->mib.stmt.markIpPrecedence.val;

        if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                        policyAttrIndex,                  
                                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_IPPRECEDENCEVAL, 
                                        (void *)&val) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
      /* Mirror Interface */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_MIRRORINTF))
      {
        if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex,
                                        policyAttrIndex,
                                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MIRROR_INTF,
                                        (void *)&pAttr->mib.stmt.mirror.configId) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:

      /* Simple Policing Committed Rate */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE))
      {
        if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                        policyAttrIndex,          
                                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CRATE, 
                                        (void *)&pAttr->mib.stmt.policeSimple.crate) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* Simple Policing Committed Burst Size */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST))
      {
        if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                        policyAttrIndex, 
                                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CBURST, 
                                        (void *)&pAttr->mib.stmt.policeSimple.cburst) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* Police Conform Act and Val */
      if (diffServPoliceConformObjectsSet(policyIndex, policyInstIndex, 
                                          policyAttrIndex, pAttr) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Police Non-Conform Act and Val */
      if (diffServPoliceNonconformObjectsSet(policyIndex, policyInstIndex, 
                                             policyAttrIndex, pAttr) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Police Color Conform Index, Mode, Val */
      if (diffServPoliceColorConformObjectsSet(policyIndex, policyInstIndex, 
                                               policyAttrIndex, pAttr) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:

      /* Single-rate Policing Committed Rate */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE))
      {
        if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                        policyAttrIndex, 
                                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CRATE, 
                                        (void *)&pAttr->mib.stmt.policeSingleRate.crate) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST))
      {
        if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                        policyAttrIndex, 
                                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CBURST, 
                                        (void *)&pAttr->mib.stmt.policeSingleRate.cburst) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* Single-rate Policing Excess Burst */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_EBURST))
      {
        if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                        policyAttrIndex, 
                                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_EBURST, 
                                        (void *)&pAttr->mib.stmt.policeSingleRate.eburst) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* Police Conform Act and Val */
      if (diffServPoliceConformObjectsSet(policyIndex, policyInstIndex, 
                                          policyAttrIndex, pAttr) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Police Exceed Act and Val */
      if (diffServPoliceExceedObjectsSet(policyIndex, policyInstIndex, 
                                         policyAttrIndex, pAttr) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Police Non-Conform Act and Val */
      if (diffServPoliceNonconformObjectsSet(policyIndex, policyInstIndex, 
                                             policyAttrIndex, pAttr) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Police Color Conform Index, Mode, Val */
      if (diffServPoliceColorConformObjectsSet(policyIndex, policyInstIndex, 
                                               policyAttrIndex, pAttr) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Police Color Exceed Index, Mode, Val */
      if (diffServPoliceColorExceedObjectsSet(policyIndex, policyInstIndex, 
                                              policyAttrIndex, pAttr) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:

      /* Two-rate Policing Committed Rate */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE))
      {
        if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                        policyAttrIndex,          
                                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CRATE, 
                                        (void *)&pAttr->mib.stmt.policeTwoRate.crate) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* Two-rate Policing Committed Burst */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST))
      {
        if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                        policyAttrIndex, 
                                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CBURST, 
                                        (void *)&pAttr->mib.stmt.policeTwoRate.cburst) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* Two-rate Policing Peak Rate */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_PRATE))
      {
        if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                        policyAttrIndex,          
                                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_PRATE, 
                                        (void *)&pAttr->mib.stmt.policeTwoRate.prate) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* Two-rate Policing Peak Burst */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_PBURST))
      {
        if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                        policyAttrIndex,          
                                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_PBURST, 
                                        (void *)&pAttr->mib.stmt.policeTwoRate.pburst) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }

      /* Police Conform Act and Val */
      if (diffServPoliceConformObjectsSet(policyIndex, policyInstIndex, 
                                          policyAttrIndex, pAttr) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Police Exceed Act and Val */
      if (diffServPoliceExceedObjectsSet(policyIndex, policyInstIndex, 
                                         policyAttrIndex, pAttr) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Police Non-Conform Act and Val */
      if (diffServPoliceNonconformObjectsSet(policyIndex, policyInstIndex, 
                                             policyAttrIndex, pAttr) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Police Color Conform Index, Mode, Val */
      if (diffServPoliceColorConformObjectsSet(policyIndex, policyInstIndex, 
                                               policyAttrIndex, pAttr) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }

      /* Police Color Exceed Index, Mode Val */
      if (diffServPoliceColorExceedObjectsSet(policyIndex, policyInstIndex, 
                                              policyAttrIndex, pAttr) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
      /* Redirect Interface */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_REDIRECTINTF))
      {
        if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                        policyAttrIndex,          
                                        L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_REDIRECT_INTF, 
                                        (void *)&pAttr->mib.stmt.redirect.configId) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      }
      break;

    default:
      LOG_MSG("%s %d: %s: Unknown Policy Attribute Type (%u)", 
              __FILE__, __LINE__, __FUNCTION__, pAttr->mib.entryType);
      return L7_FAILURE;

  } /* endswitch */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the values for Police Conform objects in the Policy AttrTable 
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy Instance index
* @param    policyAttrIndex @b{(input)}  Policy Attr index
* @param    pAttr           @b{(input)}  Pointer to policy attribute config 
*                                        structure
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t diffServPoliceConformObjectsSet(L7_uint32 policyIndex,
                                               L7_uint32 policyInstIndex,  
                                               L7_uint32 policyAttrIndex,
                                               L7_diffServPolicyAttr_t *pAttr)
{
  dsmibPoliceAct_t  policeAct;
  L7_uint32         val;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t  oidAct, oidVal;

  switch (pAttr->mib.entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      policeAct = pAttr->mib.stmt.policeSimple.conformAct;
      val = (L7_uint32)pAttr->mib.stmt.policeSimple.conformVal;
      oidAct = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CONFORM_ACT;
      oidVal = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CONFORM_VAL;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      policeAct = pAttr->mib.stmt.policeSingleRate.conformAct;
      val = (L7_uint32)pAttr->mib.stmt.policeSingleRate.conformVal;
      oidAct = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CONFORM_ACT;
      oidVal = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CONFORM_VAL;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      policeAct = pAttr->mib.stmt.policeTwoRate.conformAct;
      val = (L7_uint32)pAttr->mib.stmt.policeTwoRate.conformVal;
      oidAct = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CONFORM_ACT;
      oidVal = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CONFORM_VAL;
      break;

    default:
      return L7_FAILURE;
      /*PASSTHRU*/

  }

  /* police conform action */
  if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT))
  {
    if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                    policyAttrIndex, oidAct,
                                    &policeAct) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
  }

  /* police conform value */
  if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_IS_MARKING(policeAct) == L7_TRUE)
  {
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL))
    {
      if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                      policyAttrIndex, oidVal,
                                      &val) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the values for Police Exceed objects in the Policy AttrTable 
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy Instance index
* @param    policyAttrIndex @b{(input)}  Policy Attr index
* @param    pAttr           @b{(input)}  Pointer to policy attribute config 
*                                        structure
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t diffServPoliceExceedObjectsSet(L7_uint32 policyIndex,
                                              L7_uint32 policyInstIndex,  
                                              L7_uint32 policyAttrIndex,
                                              L7_diffServPolicyAttr_t *pAttr)
{
  dsmibPoliceAct_t  policeAct;
  L7_uint32         val;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t  oidAct, oidVal;

  switch (pAttr->mib.entryType)
  {
    /* NOTE: exceed objects not used for police simple rate */
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      policeAct = pAttr->mib.stmt.policeSingleRate.exceedAct;
      val = (L7_uint32)pAttr->mib.stmt.policeSingleRate.exceedVal;
      oidAct = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_EXCEED_ACT;
      oidVal = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_EXCEED_VAL;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      policeAct = pAttr->mib.stmt.policeTwoRate.exceedAct;
      val = (L7_uint32)pAttr->mib.stmt.policeTwoRate.exceedVal;
      oidAct = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_EXCEED_ACT;
      oidVal = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_EXCEED_VAL;
      break;

    default:
      return L7_FAILURE;
      /*PASSTHRU*/

  }

  /* police exceed action */
  if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT))
  {
    if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                    policyAttrIndex, oidAct,
                                    &policeAct) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
  }

  /* police exceed value */
  if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_IS_MARKING(policeAct) == L7_TRUE)
  {
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL))
    {
      if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                      policyAttrIndex, oidVal,
                                      &val) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the values for Police Nonconform objects in the Policy AttrTable 
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy Instance index
* @param    policyAttrIndex @b{(input)}  Policy Attr index
* @param    pAttr           @b{(input)}  Pointer to policy attribute config 
*                                        structure
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t diffServPoliceNonconformObjectsSet(L7_uint32 policyIndex,
                                                  L7_uint32 policyInstIndex,  
                                                  L7_uint32 policyAttrIndex,
                                                  L7_diffServPolicyAttr_t *pAttr)
{
  dsmibPoliceAct_t  policeAct;
  L7_uint32         val;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t  oidAct, oidVal;

  switch (pAttr->mib.entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      policeAct = pAttr->mib.stmt.policeSimple.nonconformAct;
      val = (L7_uint32)pAttr->mib.stmt.policeSimple.nonconformVal;
      oidAct = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_NONCONFORM_ACT;
      oidVal = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_NONCONFORM_VAL;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      policeAct = pAttr->mib.stmt.policeSingleRate.nonconformAct;
      val = (L7_uint32)pAttr->mib.stmt.policeSingleRate.nonconformVal;
      oidAct = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_NONCONFORM_ACT;
      oidVal = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_NONCONFORM_VAL;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      policeAct = pAttr->mib.stmt.policeTwoRate.nonconformAct;
      val = (L7_uint32)pAttr->mib.stmt.policeTwoRate.nonconformVal;
      oidAct = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_NONCONFORM_ACT;
      oidVal = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_NONCONFORM_VAL;
      break;

    default:
      return L7_FAILURE;
      /*PASSTHRU*/

  }

  /* police nonconform action */
  if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT))
  {
    if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                    policyAttrIndex, oidAct,
                                    &policeAct) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
  }

  /* police nonconform value */
  if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_IS_MARKING(policeAct) == L7_TRUE)
  {
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL))
    {
      if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                      policyAttrIndex, oidVal,
                                      &val) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the values for Police Color Conform objects in the
*           Policy Attr Table 
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy Instance index
* @param    policyAttrIndex @b{(input)}  Policy Attr index
* @param    pAttr           @b{(input)}  Pointer to policy attribute config 
*                                        structure
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t diffServPoliceColorConformObjectsSet(L7_uint32 policyIndex,
                                                    L7_uint32 policyInstIndex,  
                                                    L7_uint32 policyAttrIndex,
                                                    L7_diffServPolicyAttr_t *pAttr)
{
  L7_uint32           colorIndex;
  dsmibPoliceColor_t  colorMode;
  L7_uint32           val;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t  oidIdx, oidAct, oidVal;

  /* just return successfully if the color-aware feature is not supported */
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                            L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID)
      != L7_TRUE)
  {
    return L7_SUCCESS;
  }

  switch (pAttr->mib.entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      colorIndex = pAttr->mib.stmt.policeSimple.colorConformIndex;
      colorMode = pAttr->mib.stmt.policeSimple.colorConformMode;
      val = (L7_uint32)pAttr->mib.stmt.policeSimple.colorConformVal;
      oidIdx = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_COLOR_CONFORM_INDEX;
      oidAct = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_COLOR_CONFORM_MODE;
      oidVal = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_COLOR_CONFORM_VAL;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      colorIndex = pAttr->mib.stmt.policeSingleRate.colorConformIndex;
      colorMode = pAttr->mib.stmt.policeSingleRate.colorConformMode;
      val = (L7_uint32)pAttr->mib.stmt.policeSingleRate.colorConformVal;
      oidIdx = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_CONFORM_INDEX;
      oidAct = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_CONFORM_MODE;
      oidVal = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_CONFORM_VAL;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      colorIndex = pAttr->mib.stmt.policeTwoRate.colorConformIndex;
      colorMode = pAttr->mib.stmt.policeTwoRate.colorConformMode;
      val = (L7_uint32)pAttr->mib.stmt.policeTwoRate.colorConformVal;
      oidIdx = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_CONFORM_INDEX;
      oidAct = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_CONFORM_MODE;
      oidVal = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_CONFORM_VAL;
      break;

    default:
      return L7_FAILURE;
      /*PASSTHRU*/

  }

  /* police color conform index */
  if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMINDEX))
  {
    if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                    policyAttrIndex, oidIdx,
                                    &colorIndex) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
  }

  /* police color conform mode */
  if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE))
  {
    if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                    policyAttrIndex, oidAct,
                                    &colorMode) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
  }

  /* police color conform value */
  if ((colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS) ||
      (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2) ||
      (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP) ||
      (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC))
  {
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL))
    {
      if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                      policyAttrIndex, oidVal,
                                      &val) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the values for Police Color Exceed objects in the
*           Policy Attr Table 
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy Instance index
* @param    policyAttrIndex @b{(input)}  Policy Attr index
* @param    pAttr           @b{(input)}  Pointer to policy attribute config 
*                                        structure
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t diffServPoliceColorExceedObjectsSet(L7_uint32 policyIndex,
                                                   L7_uint32 policyInstIndex,  
                                                   L7_uint32 policyAttrIndex,
                                                   L7_diffServPolicyAttr_t *pAttr)
{
  L7_uint32           colorIndex;
  dsmibPoliceColor_t  colorMode;
  L7_uint32           val;
  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t  oidIdx, oidAct, oidVal;

  /* just return successfully if the color-aware feature is not supported */
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                            L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID)
      != L7_TRUE)
  {
    return L7_SUCCESS;
  }

  switch (pAttr->mib.entryType)
  {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      colorIndex = pAttr->mib.stmt.policeSingleRate.colorExceedIndex;
      colorMode = pAttr->mib.stmt.policeSingleRate.colorExceedMode;
      val = (L7_uint32)pAttr->mib.stmt.policeSingleRate.colorExceedVal;
      oidIdx = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_EXCEED_INDEX;
      oidAct = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_EXCEED_MODE;
      oidVal = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_EXCEED_VAL;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      colorIndex = pAttr->mib.stmt.policeTwoRate.colorExceedIndex;
      colorMode = pAttr->mib.stmt.policeTwoRate.colorExceedMode;
      val = (L7_uint32)pAttr->mib.stmt.policeTwoRate.colorExceedVal;
      oidIdx = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_EXCEED_INDEX;
      oidAct = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_EXCEED_MODE;
      oidVal = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_EXCEED_VAL;
      break;

    default:
      return L7_FAILURE;
      /*PASSTHRU*/

  }

  /* police color exceed index */
  if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDINDEX))
  {
    if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                    policyAttrIndex, oidIdx,
                                    &colorIndex) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
  }

  /* police color exceed mode */
  if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDMODE))
  {
    if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                    policyAttrIndex, oidAct,
                                    &colorMode) != L7_SUCCESS)
    {
      return diffServConfigError(__LINE__, __FUNCTION__);
    }
  }

  /* police color exceed value */
  if ((colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS) ||
      (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2) ||
      (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP) ||
      (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC))
  {
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDVAL))
    {
      if (diffServPolicyAttrObjectSet(policyIndex, policyInstIndex, 
                                      policyAttrIndex, oidVal,
                                      &val) != L7_SUCCESS)
      {
        return diffServConfigError(__LINE__, __FUNCTION__);
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configure Services 
*
* @param    void 
*
* @returns  L7_SUCCESS  Config data applied
* @returns  L7_FAILURE  Unexpected condition encountered
*
* @notes    Set up private MIB Policy Group table rows from config file 
*           contents.
*
* @notes    Requires activation of DiffServ 'special config mode' prior
*           to invoking this function.
*
*
* @end
*********************************************************************/
L7_RC_t diffServServiceConfig(void)
{
  L7_uint32            intIfNum;
  L7_uint32            cfgIndex;
  L7_diffServIntfCfg_t *pCfg;
  nimConfigID_t        configIdNull;

  memset(&configIdNull, 0, sizeof(configIdNull));

  for (cfgIndex = 1; cfgIndex < L7_DIFFSERV_INTF_MAX_COUNT; cfgIndex++)
  {
    if (NIM_CONFIG_ID_IS_EQUAL(&pDiffServCfgData_g->diffServIntfCfg[cfgIndex].configId, &configIdNull))
      continue;
    if (nimIntIfFromConfigIDGet(&pDiffServCfgData_g->diffServIntfCfg[cfgIndex].configId, &intIfNum) != L7_SUCCESS)
      continue;
    if (diffServMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      if (diffServServiceIntfConfig(intIfNum) != L7_SUCCESS)
        return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Configure Services 
*
* @param    void 
*
* @returns  L7_SUCCESS  Config data applied
* @returns  L7_FAILURE  Unexpected condition encountered
* @returns  L7_ERROR    If interface is not configurable
*
* @notes    Set up private MIB Policy Group table rows from config file 
*           contents.
*
* @notes    Requires activation of DiffServ 'special config mode' prior
*           to invoking this function.
*
*
* @end
*********************************************************************/
L7_RC_t diffServServiceIntfConfig(L7_uint32 intIfNum)
{
  L7_uint32                 ifDirection;
  L7_BOOL                   activateRow;
  L7_diffServService_t      *pSvc;
  dsmibRowStatus_t          rowStatus;
  L7_diffServIntfCfg_t      *pCfg;
  dsmibServiceEntryCtrl_t   *pService;


  if (diffServMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
  {
    /* process each direction for this service interface */
    for (ifDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN;
        ifDirection <= L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;
        ifDirection++)
    {
      if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
        pSvc = &pCfg->serviceIn;
      else
        pSvc = &pCfg->serviceOut;
      /* only create service row if this entry in use */
      if (pSvc->inUse == L7_TRUE)
      {
        /* Only process inbound service interfaces, since DiffServ outbound is
         * no longer supported as of Rel I.  Note that configuration migrated
         * from previous releases will have preserved any outbound service 
         * interface information, so it is ignored here.
         */
        if (ifDirection != L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
          continue;

        /* Get activate status for this service interface */
        rowStatus = pSvc->mib.rowStatus;
        if (diffServGetRowActivateStatus(rowStatus, &activateRow) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }

        /* Service on inbound intf */
        if (diffServServiceCreate(intIfNum, ifDirection, activateRow) 
            != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }

        /* get access to the service row */
        pService = dsmibServiceRowFind(intIfNum, ifDirection);
        if (pService == L7_NULLPTR)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }

        /* Storage Type */
        if (diffServServiceObjectSet(intIfNum, ifDirection,
                                     L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_STORAGE_TYPE,
                                     (void *)&pSvc->mib.storageType) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }

        if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pSvc, DSMIB_SERVICE_FLAGS_POLICYINDEX))
        {
          if (diffServServiceObjectSet(intIfNum, ifDirection, 
                                       L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_POLICY_INDEX,
                                       (void *)&pSvc->mib.policyIndex) != L7_SUCCESS)
          {
            return diffServConfigError(__LINE__, __FUNCTION__);
          }
        }

        /* Check for consistency of row status */
        if (diffServServiceObjectGet(intIfNum, ifDirection,
                                     L7_DIFFSERV_SERVICE_ROW_STATUS,
                                     &rowStatus) != L7_SUCCESS)
        {
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
        if ((pSvc->mib.rowStatus != rowStatus) &&
            (pSvc->mib.rowStatus != pService->mib.rowStatus))
        {
          LOG_MSG("Inconsistent row status for intIfNum %u %s: expected=%u, actual=%u\n",
                  intIfNum, 
                  (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN) ?
                  "inbound" : "outbound",
                   pSvc->mib.rowStatus, rowStatus);
          return diffServConfigError(__LINE__, __FUNCTION__);
        }
      } /* endif entry in use */
    } /* endfor ifDirection */
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clean up DiffServ as part of restoring to default configuration.
*
* @param    void 
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    This routine performs a reset of the DiffServ component, 
*           leaving it in a disabled administrative mode state.
*
* @notes    It is important to execute this function to completion without
*           failure in order to clean up from a partial or erroneous
*           DiffServ configuration.  This is why the "normal" component
*           APIs are avoided in favor of the major sub-component reset
*           functions.
*
* @end
*********************************************************************/
L7_RC_t diffServResetCleanUp(void)
{
  /*------------------------------------------------*/
  /* Reset major elements of the DiffServ component */
  /*                                                */
  /* NOTE:  Reset order is important here!          */
  /* NOTE:  Forces DiffServ admin mode to disabled. */
  /*------------------------------------------------*/
  if (dsDistillerReset() != L7_SUCCESS)
  {
    return diffServConfigError(__LINE__, __FUNCTION__);
  }

  if (diffServStandardMibReset() != L7_SUCCESS)
  {
    return diffServConfigError(__LINE__, __FUNCTION__);
  }

  if (diffServPrivateMibReset() != L7_SUCCESS)
  {
    return diffServConfigError(__LINE__, __FUNCTION__);
  }

  pDiffServCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the rowActive flag from Row Status 
*
* @param    rowStatus    @b{(input)} Row Status that is stored in config file
* @param    pActivateRow @b{(input)} pointer to output value (L7_TRUE, L7_FALSE)
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes  
*
* @end
*********************************************************************/
static L7_RC_t diffServGetRowActivateStatus(
                                           L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t rowStatus, L7_BOOL *pActivateRow)
{
  switch (rowStatus)
  {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      *pActivateRow = L7_TRUE;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      *pActivateRow = L7_FALSE;
      break;

    default:
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print the error location
*
* @param    errLine     @b{(input)} Line number at which this error occured 
* @param    routineName @b{(input)} Function in which this error occured 
*
* @returns  L7_SUCCESS
* @return   L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
static L7_RC_t diffServConfigError(L7_uint32 errLine, const char *routineName)
{
  LOG_MSG("%s %u: %s: Error applying DiffServ config\n", 
          __FILE__, errLine, routineName);
  return L7_FAILURE;
}


/* Display strings for debug show functions (matches enum defs) */
char *dsmibTruthValueStr[] = 
{
  "-", "T", "F"
};
char *dsmibStgTypeStr[] = 
{ 
  "--", "O ", "V ", "NV", "RO", "P "
};
char *dsmibRowPendingStr[] = 
{ 
  "N", "Y"
};
char *dsmibRowStatusStr[] = 
{ 
  "--", "A ", "NS", "NR", "CG", "CW", "D "
};
char *dsmibClassTypeStr[] = 
{ 
  "---", "all", "any", "acl"
};
char *dsmibClassL3ProtoStr[] = 
{ 
  "----", "ipv4", "ipv6"
};
char *dsmibClassAclTypeStr[] = 
{ 
  "----", "ip  ", "mac "
};
char *dsmibClassRuleTypeStr[] = 
{ 
  "----", "cos ", "cos2", "dip ", "dip6", "d4pt", "dmac", "etyp", "evry",
  "flbl", "dscp", "prec", "tos ", "prot", "refc", "sip ", "sip6", "s4pt", 
  "smac", "vlan", "vln2"
};
char *dsmibEtypeKeyidStr[] = 
{ 
  "none", "custom", "appltk", "arp", "ibmsna", "ipv4", "ipv6", "ipx",
  "mplsmc", "mplsuc", "ntbios", "novell", "pppoe", "rarp"
};
char *dsmibPolicyTypeStr[] = 
{ 
  "---", "in ", "out"
};
char *dsmibPolicyAttrTypeStr[] = 
{ 
  "----", "asnq", "bw  ", "drop", "expd", "cos ", "cos2", "dscp",
  "prec", "mirr", "psmp", "psgl", "ptwo", "rand", "rdir", "savg",
  "spk "
};
char *dsmibBwUnitsStr[] = 
{
  "----", "kbps", "perc"
};
char *dsmibPoliceActStr[] = 
{ 
  "-----", "drop ", "mcos ", "mcos2", "mdscp", "mprec", "send "
};  
char *dsmibPoliceColorStr[] = 
{ 
  "-----", "blind", "cos", "cos2", "dscp", "prec", "unused"
};  
char *dsmibNimIntfTypeStr[] = 
{ 
  "(0)", "(1)", "(2)", "phys", "stack", "cpu", "(maxphys)", "lag",
  "lif", "vlanlif", "(maxintf)"
};
char *dsmibServiceIfDir[] = 
{ 
  "---", "in ", "out"
};
char *dsmibServiceIfOperStatus[] = 
{ 
  "---", "UP", "dn"
};
char *dsmibPrvtTableIdStr[] = 
{ 
  "---", "Class", "Class Rule", "Policy", "Policy Inst", "Policy Attr",
  "Policy Perf In", "Policy Perf Out", "Service"
};
char *dsmibStdTableIdStr[] = 
{ 
  "---", "DataPath", "Clfr", "Clfr Element", "Multi-Field Clfr", "Aux MF Clfr",
  "Meter", "TB Param", "Color Aware", "Action", "Mark COS Act", "Mark COS2 Act",
  "Mark IP DSCP Act", "Mark IP Prec Act", "Count Act", "Assign Queue", 
  "Redirect", "Mirror", "Alg Drop", "Random Drop", "Queue", "Scheduler", 
  "Min Rate", "Max Rate"
};


/*********************************************************************
* @purpose  Display the Configuration of Diffserv component
*
* @param    pCfgData     @b{(input)} pointer to config data struct to show
*
* @returns  void
*
* @notes    This is an internal function.
*
* @end
*********************************************************************/
void dsmibConfigDataShow(L7_diffServCfg_t *pCfgData)
{
  L7_uint32               msgLvlReqd;
  L7_fileHdr_t            *pFileHdr;
  L7_diffServClassHdr_t   *pClassHdr;
  L7_diffServClassRule_t  *pClassRuleRow;
  L7_diffServPolicyHdr_t  *pPolicyHdr;
  L7_diffServPolicyInst_t *pPolicyInst;
  L7_diffServPolicyAttr_t *pPolicyAttr;
  L7_diffServService_t    *pSvc;
  L7_uint32               chainIndex;
  L7_uint32               attrChainIndex;
  L7_BOOL                 classFlag = L7_FALSE;
  L7_BOOL                 classRuleFlag = L7_FALSE;
  L7_BOOL                 policyFlag = L7_FALSE;
  L7_BOOL                 policyInstFlag = L7_FALSE;
  L7_BOOL                 policyAttrFlag = L7_FALSE;
  L7_BOOL                 svcFlag = L7_FALSE;
  L7_uint32               i,k;  
  L7_uint32               cfgIndex, intIfNum;
  L7_diffServIntfCfg_t    *pCfg;
  char                    *pSpacer;
  L7_char8                ipv6AddrString[64];

  /* specify required DiffServ message level */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  /* make sure config data exists */
  if (pCfgData == L7_NULLPTR)
  {
    DIFFSERV_PRT(msgLvlReqd, 
                 "\nDiffServ Configuration Data does not exist\n\n");
    return;
  }

  /*------------------------------------------------*/
  /* Config File Header Contents                    */
  /*------------------------------------------------*/
  pFileHdr = &pCfgData->cfgHdr;

  DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Config File Header:\n\n");

  DIFFSERV_PRT(msgLvlReqd, 
               "File name......................  %s\n", (char *)pFileHdr->filename);

  DIFFSERV_PRT(msgLvlReqd, 
               "Version........................  %u\n", pFileHdr->version);

  DIFFSERV_PRT(msgLvlReqd, 
               "Component ID...................  %u (%s)\n", pFileHdr->componentID,
               (pFileHdr->componentID == L7_FLEX_QOS_DIFFSERV_COMPONENT_ID) ? "DiffServ" : "???");

  DIFFSERV_PRT(msgLvlReqd, 
               "Type...........................  %u\n", pFileHdr->type);

  DIFFSERV_PRT(msgLvlReqd, 
               "Length.........................  %u\n", pFileHdr->length);

  DIFFSERV_PRT(msgLvlReqd, 
               "Data Changed flag..............  %u\n", pFileHdr->dataChanged);


  /*------------------------------------------------*/
  /* Global Configuration                           */
  /*------------------------------------------------*/
  DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Global Configuration Contents:\n\n");

  DIFFSERV_PRT(msgLvlReqd, 
               "Admin mode.....................  %s\n",
               (pCfgData->adminMode == L7_ENABLE) ? "Enable" : "Disable");

  DIFFSERV_PRT(msgLvlReqd, 
               "Trace mode.....................  %s\n",
               (pCfgData->traceMode == L7_ENABLE) ? "Enable" : "Disable");

  DIFFSERV_PRT(msgLvlReqd, 
               "Message level..................  %u\n",
               pCfgData->msgLvl);

  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  /*------------------------------------------------*/
  /* Class Configuration Data Show                  */
  /*------------------------------------------------*/
  for (i = L7_DIFFSERV_CLASS_HDR_START; i < L7_DIFFSERV_CLASS_HDR_MAX; i++)
  {
    pClassHdr = &pCfgData->classHdr[i];

    /* Array entry is not in use means end of the array list*/
    if (pClassHdr->inUse != L7_TRUE)
      break;

    if (classFlag == L7_FALSE)
    {
      DIFFSERV_PRT(msgLvlReqd, 
                   "\nDiffServ Class Entries:\n\n");

      DIFFSERV_PRT(msgLvlReqd, "Index ChIdx Type L3Pro AclT Acl#  Next ST RS  Flags   Class Name                  \n");
      DIFFSERV_PRT(msgLvlReqd, "----- ----- ---- ----- ---- ----- ---- -- -- -------- ----------------------------\n");
      classFlag = L7_TRUE;
    }
    DIFFSERV_PRT(msgLvlReqd, " %3u   %3u  %-4s %-4s  %-4s %5u %3u  %2s %2s %8.8x \'%-s\'\n", 
                 i, pClassHdr->chainIndex,
                 dsmibClassTypeStr[pClassHdr->mib.type], 
                 dsmibClassL3ProtoStr[pClassHdr->mib.l3Protocol], 
                 dsmibClassAclTypeStr[pClassHdr->mib.aclType], 
                 pClassHdr->mib.aclNum,
                 pClassHdr->mib.ruleIndexNextFree,
                 dsmibStgTypeStr[pClassHdr->mib.storageType],
                 dsmibRowStatusStr[pClassHdr->mib.rowStatus],
                 pClassHdr->rowInvalidFlags,
                 (char *)pClassHdr->mib.name);
  }

  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  /*------------------------------------------------*/
  /* Class Rule Configuration Data Show             */
  /*------------------------------------------------*/
  for (i = L7_DIFFSERV_CLASS_HDR_START; i < L7_DIFFSERV_CLASS_HDR_MAX; i++)
  {
    pClassHdr = &pCfgData->classHdr[i];

    /* Array entry is not in use means end of the array list*/
    if (pClassHdr->inUse != L7_TRUE)
      break;

    chainIndex = pClassHdr->chainIndex; 

    if (chainIndex != 0 && classRuleFlag == L7_FALSE)
    {
      DIFFSERV_PRT(msgLvlReqd, 
                   "\nDiffServ Class Rule Entries:\n\n");

      DIFFSERV_PRT(msgLvlReqd, "Index HdrIdx ChIdx Arid Type Excl ST RS  Flags   Type-Specific Fields      \n");
      DIFFSERV_PRT(msgLvlReqd, "----- ------ ----- ---- ---- ---- -- -- -------- --------------------------\n");
      classRuleFlag = L7_TRUE;
    }

    while (chainIndex != 0)
    {
      pClassRuleRow = &pCfgData->classRule[chainIndex];

      DIFFSERV_PRT(msgLvlReqd, " %3u   %3u   %3u   %4u %4s  %1s   %2s %2s %8.8x ", 
                   chainIndex, pClassRuleRow->hdrIndexRef, pClassRuleRow->chainIndex,
                   pClassRuleRow->arid,
                   dsmibClassRuleTypeStr[pClassRuleRow->mib.entryType], 
                   dsmibTruthValueStr[pClassRuleRow->mib.excludeFlag],
                   dsmibStgTypeStr[pClassRuleRow->mib.storageType],
                   dsmibRowStatusStr[pClassRuleRow->mib.rowStatus], 
                   pClassRuleRow->rowInvalidFlags);

      /* depends on entry type */
      switch (pClassRuleRow->mib.entryType)
      {
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS:
        DIFFSERV_PRT(msgLvlReqd, "cos=%u\n", 
                     pClassRuleRow->mib.match.cos);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2:
        DIFFSERV_PRT(msgLvlReqd, "cos2=%u\n", 
                     pClassRuleRow->mib.match.cos2);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
        DIFFSERV_PRT(msgLvlReqd, "ipaddr=0x%8.8x ipmask=0x%8.8x\n", 
                     pClassRuleRow->mib.match.dstIp.addr, pClassRuleRow->mib.match.dstIp.mask);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
        osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&pClassRuleRow->mib.match.dstIpv6.ip6prefix.in6Addr,
                      ipv6AddrString, sizeof(ipv6AddrString));
        DIFFSERV_PRT(msgLvlReqd, "ipv6addr=%s ipv6plen=0x%8.8x\n", 
                     ipv6AddrString,
                     pClassRuleRow->mib.match.dstIpv6.ip6prefix.in6PrefixLen);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
        DIFFSERV_PRT(msgLvlReqd, "startport=%u endport=%u\n", 
                     pClassRuleRow->mib.match.dstL4Port.start, 
                     pClassRuleRow->mib.match.dstL4Port.end);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC:
        DIFFSERV_PRT(msgLvlReqd, "macaddr=0x"); 
        for (k = 0; k < L7_MAC_ADDR_LEN-1; k++)
        {
          DIFFSERV_PRT(msgLvlReqd, "%2.2x:", 
                       pClassRuleRow->mib.match.dstMac.addr[k]);
        }
        DIFFSERV_PRT(msgLvlReqd, "%2.2x\n", 
                     pClassRuleRow->mib.match.dstMac.addr[k]);

        DIFFSERV_PRT(msgLvlReqd, "                                                 macmask=0x"); 
        for (k = 0; k < L7_MAC_ADDR_LEN-1; k++)
        {
          DIFFSERV_PRT(msgLvlReqd, "%2.2x:", 
                       pClassRuleRow->mib.match.dstMac.mask[k]);
        }
        DIFFSERV_PRT(msgLvlReqd, "%2.2x\n", 
                     pClassRuleRow->mib.match.dstMac.mask[k]);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE:
        DIFFSERV_PRT(msgLvlReqd, "etype: keywd=%u(%s) value=0x%4.4x\n", 
                     pClassRuleRow->mib.match.etype.keyid,
                     dsmibEtypeKeyidStr[pClassRuleRow->mib.match.etype.keyid],
                     (L7_ushort16)pClassRuleRow->mib.match.etype.value);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY:
        DIFFSERV_PRT(msgLvlReqd, "match every=%s\n", 
                     dsmibTruthValueStr[pClassRuleRow->mib.match.matchEvery]);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
        DIFFSERV_PRT(msgLvlReqd, "ipv6 flow label=%u\n", 
                     pClassRuleRow->mib.match.ipv6FlowLabel);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
        DIFFSERV_PRT(msgLvlReqd, "ip dscp=%u\n", 
                     pClassRuleRow->mib.match.ipDscp);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
        DIFFSERV_PRT(msgLvlReqd, "ip precedence=%u\n", 
                     pClassRuleRow->mib.match.ipPrecedence);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
        DIFFSERV_PRT(msgLvlReqd, "tosbits=0x%2.2x tosmask=0x%2.2x\n", 
                     pClassRuleRow->mib.match.ipTos.bits, pClassRuleRow->mib.match.ipTos.mask);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
        DIFFSERV_PRT(msgLvlReqd, "protocol num=%u\n", 
                     pClassRuleRow->mib.match.protocolNum);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS:
      {
          L7_uchar8   name[L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX+1];
          L7_RC_t     rc;

          DIFFSERV_PRT(msgLvlReqd, "ref class: index=%u ", 
                       pClassRuleRow->mib.match.refClassIndex);
          rc = diffServClassObjectGet(pClassRuleRow->mib.match.refClassIndex, 
                                      L7_DIFFSERV_CLASS_NAME, name);
          if (rc == L7_SUCCESS)
          {
            DIFFSERV_PRT(msgLvlReqd, "name=\'%s\'", (char *)name);
          }
          DIFFSERV_PRT(msgLvlReqd, "\n");
      }
      break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
        DIFFSERV_PRT(msgLvlReqd, "ipaddr=0x%8.8x ipmask=0x%8.8x\n", 
                     pClassRuleRow->mib.match.srcIp.addr, pClassRuleRow->mib.match.srcIp.mask);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
        osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&pClassRuleRow->mib.match.srcIpv6.ip6prefix.in6Addr,
                      ipv6AddrString, sizeof(ipv6AddrString));
        DIFFSERV_PRT(msgLvlReqd, "ipv6addr=%s ipv6plen=0x%8.8x\n", 
                     ipv6AddrString,
                     pClassRuleRow->mib.match.srcIpv6.ip6prefix.in6PrefixLen);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
        DIFFSERV_PRT(msgLvlReqd, "startport=%u endport=%u\n", 
                     pClassRuleRow->mib.match.srcL4Port.start, 
                     pClassRuleRow->mib.match.srcL4Port.end);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC:
        DIFFSERV_PRT(msgLvlReqd, "macaddr=0x"); 
        for (k = 0; k < L7_MAC_ADDR_LEN-1; k++)
        {
          DIFFSERV_PRT(msgLvlReqd, "%2.2x:", 
                       pClassRuleRow->mib.match.srcMac.addr[k]);
        }
        DIFFSERV_PRT(msgLvlReqd, "%2.2x\n", 
                     pClassRuleRow->mib.match.srcMac.addr[k]);

        DIFFSERV_PRT(msgLvlReqd, "                                                 macmask=0x");
        for (k = 0; k < L7_MAC_ADDR_LEN-1; k++)
        {
          DIFFSERV_PRT(msgLvlReqd, "%2.2x:", 
                       pClassRuleRow->mib.match.srcMac.mask[k]);
        }
        DIFFSERV_PRT(msgLvlReqd, "%2.2x\n", 
                     pClassRuleRow->mib.match.srcMac.mask[k]);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID:
        DIFFSERV_PRT(msgLvlReqd, "vlanid: start=%u end=%u\n", 
                     pClassRuleRow->mib.match.vlanId.start,
                     pClassRuleRow->mib.match.vlanId.end);
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2:
        DIFFSERV_PRT(msgLvlReqd, "vlanid2: start=%u end=%u\n", 
                     pClassRuleRow->mib.match.vlanId2.start,
                     pClassRuleRow->mib.match.vlanId2.end);
        break;

      default:
        DIFFSERV_PRT(msgLvlReqd, "\n"); 
        break;
      } /* endswitch */

      chainIndex = pCfgData->classRule[chainIndex].chainIndex; 
    }
  }
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  /*------------------------------------------------*/
  /* Policy Configuration  Data Show                */
  /*------------------------------------------------*/
  for (i = L7_DIFFSERV_POLICY_HDR_START; i < L7_DIFFSERV_POLICY_HDR_MAX; i++)
  {
    pPolicyHdr = &pCfgData->policyHdr[i];

    /* Array entry is not in use means end of the array list*/
    if (pPolicyHdr->inUse != L7_TRUE)
      break;

    if (policyFlag == L7_FALSE)
    {
      DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Policy Entries:\n\n");

      DIFFSERV_PRT(msgLvlReqd, "Index ChIdx Type  Next ST RS  Flags   Policy Name                       \n")
      DIFFSERV_PRT(msgLvlReqd, "----- ----- ----  ---- -- -- -------- ----------------------------------\n");
      policyFlag = L7_TRUE;
    }

    DIFFSERV_PRT(msgLvlReqd, " %3u  %3u   %3s  %3u   %2s %2s %8.8x \'%-s\'\n", 
                 i, pPolicyHdr->chainIndex,
                 dsmibPolicyTypeStr[pPolicyHdr->mib.type], 
                 pPolicyHdr->mib.instIndexNextFree,
                 dsmibStgTypeStr[pPolicyHdr->mib.storageType],
                 dsmibRowStatusStr[pPolicyHdr->mib.rowStatus],
                 pPolicyHdr->rowInvalidFlags,
                 (char *)pPolicyHdr->mib.name);
  }

  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  /*------------------------------------------------*/
  /* Policy Instance Configuration  Data Show       */
  /*------------------------------------------------*/
  for (i = L7_DIFFSERV_POLICY_HDR_START; i < L7_DIFFSERV_POLICY_HDR_MAX; i++)
  {
    pPolicyHdr = &pCfgData->policyHdr[i];

    /* Array entry is not in use means end of the array list*/
    if (pPolicyHdr->inUse != L7_TRUE)
      break;

    /* heading print - only once */
    chainIndex = pPolicyHdr->chainIndex; 

    if (chainIndex != 0 && policyInstFlag == L7_FALSE)
    {
      DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Policy Instance Entries:\n\n");

      DIFFSERV_PRT(msgLvlReqd, "Index HdrIdx AttrChIdx ChIdx ClassIndex  Next ST RS  Flags \n");
      DIFFSERV_PRT(msgLvlReqd, "----- ------ --------- ----- ----------  ---- -- -- -------\n");
      policyInstFlag = L7_TRUE;
    }

    /* scan all policy instances of this policy*/
    while (chainIndex != 0)
    {
      pPolicyInst = &pCfgData->policyInst[chainIndex];

      DIFFSERV_PRT(msgLvlReqd, " %3u   %3u     %3u    %3u     %2u         %2u   %2s %2s %8.8x\n",
                   chainIndex, pPolicyInst->hdrIndexRef, pPolicyInst->attrChainIndex,
                   pPolicyInst->chainIndex,
                   pPolicyInst->mib.classIndex, 
                   pPolicyInst->mib.attrIndexNextFree, 
                   dsmibStgTypeStr[pPolicyInst->mib.storageType],
                   dsmibRowStatusStr[pPolicyInst->mib.rowStatus],
                   pPolicyInst->rowInvalidFlags);

      chainIndex = pPolicyInst->chainIndex;
    }
  }
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  /*-----------------------------------------------*/
  /* Policy Attribute Configuration Data Show      */
  /*-----------------------------------------------*/
  for (i = L7_DIFFSERV_POLICY_HDR_START; i < L7_DIFFSERV_POLICY_HDR_MAX; i++)
  {
    pPolicyHdr = &pCfgData->policyHdr[i];

    /* Array entry is not in use means end of the array list*/
    if (pPolicyHdr->inUse != L7_TRUE)
      break;

    chainIndex = pPolicyHdr->chainIndex; 

    /* scan all policy instances of this policy*/ 
    while (chainIndex != 0)
    {
      pPolicyInst = &pCfgData->policyInst[chainIndex];

      attrChainIndex = pPolicyInst->attrChainIndex;

      /* heading print - only once */
      if (attrChainIndex != 0 && policyAttrFlag == L7_FALSE)
      {
        DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Policy Attribute Entries:\n\n");
        DIFFSERV_PRT(msgLvlReqd, "Index InstIdx ChIdx Type ST RS  Flags   Type-Specific Fields                 \n");
        DIFFSERV_PRT(msgLvlReqd, "----- ------- ----- ---- -- -- -------- -------------------------------------\n");
        policyAttrFlag = L7_TRUE;
      }
      pSpacer =                  "                                       ";                                   

      /* scan all attributes of this policy Instance*/
      while (attrChainIndex != 0)
      {
        pPolicyAttr = &pCfgData->policyAttr[attrChainIndex];
        DIFFSERV_PRT(msgLvlReqd, " %3u   %3u    %3u   %4s %2s %2s %8.8x ", 
                     attrChainIndex, pPolicyAttr->instIndexRef, 
                     pPolicyAttr->chainIndex,
                     dsmibPolicyAttrTypeStr[pPolicyAttr->mib.entryType],
                     dsmibStgTypeStr[pPolicyAttr->mib.storageType],
                     dsmibRowStatusStr[pPolicyAttr->mib.rowStatus],
                     pPolicyAttr->rowInvalidFlags);

        switch (pPolicyAttr->mib.entryType)
        {
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
          DIFFSERV_PRT(msgLvlReqd, "qid=%u\n", 
                       pPolicyAttr->mib.stmt.assignQueue.qid);
          break;

        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
          DIFFSERV_PRT(msgLvlReqd, "drop=%s\n", 
                       dsmibTruthValueStr[pPolicyAttr->mib.stmt.drop]);
          break;

        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
          DIFFSERV_PRT(msgLvlReqd, "cos=%u\n", pPolicyAttr->mib.stmt.markCos.val);
          break;

        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
          DIFFSERV_PRT(msgLvlReqd, "cos2=%u\n", pPolicyAttr->mib.stmt.markCos2.val);
          break;

        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
          DIFFSERV_PRT(msgLvlReqd, "cosAsCos2=%u\n", pPolicyAttr->mib.stmt.markCosAsCos2.val);
          break;

        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
          DIFFSERV_PRT(msgLvlReqd, "dscp=%u\n", pPolicyAttr->mib.stmt.markIpDscp.val);
          break;

        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
          DIFFSERV_PRT(msgLvlReqd, "prec=%u\n", pPolicyAttr->mib.stmt.markIpPrecedence.val);
          break;

        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
          DIFFSERV_PRT(msgLvlReqd, "type=%s usp=%u/%u/%u\n",
                       dsmibNimIntfTypeStr[pPolicyAttr->mib.stmt.mirror.configId.type],
                       (L7_uint32)pPolicyAttr->mib.stmt.mirror.configId.configSpecifier.usp.unit,
                       (L7_uint32)pPolicyAttr->mib.stmt.mirror.configId.configSpecifier.usp.slot,
                       (L7_uint32)pPolicyAttr->mib.stmt.mirror.configId.configSpecifier.usp.port);
          break;

        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
          DIFFSERV_PRT(msgLvlReqd, "crate=%u cburst=%u\n",
                       pPolicyAttr->mib.stmt.policeSimple.crate,
                       pPolicyAttr->mib.stmt.policeSimple.cburst);
          DIFFSERV_PRT(msgLvlReqd, "%s confAct=%s confVal=%u\n",
                       pSpacer,
                       dsmibPoliceActStr[pPolicyAttr->mib.stmt.policeSimple.conformAct],
                       pPolicyAttr->mib.stmt.policeSimple.conformVal);
          DIFFSERV_PRT(msgLvlReqd, "%s nonconfAct=%s nonconfVal=%u\n",
                       pSpacer,
                       dsmibPoliceActStr[pPolicyAttr->mib.stmt.policeSimple.nonconformAct],
                       pPolicyAttr->mib.stmt.policeSimple.nonconformVal);
          DIFFSERV_PRT(msgLvlReqd, "%s colorConfIdx=%u colorConfMode=%s colorConfVal=%u\n",
                       pSpacer,
                       pPolicyAttr->mib.stmt.policeSimple.colorConformIndex,
                       dsmibPoliceColorStr[pPolicyAttr->mib.stmt.policeSimple.colorConformMode],
                       pPolicyAttr->mib.stmt.policeSimple.colorConformVal);
          break;

        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
          DIFFSERV_PRT(msgLvlReqd, "crate=%u cburst=%u eburst=%u\n", 
                       pPolicyAttr->mib.stmt.policeSingleRate.crate,
                       pPolicyAttr->mib.stmt.policeSingleRate.cburst,
                       pPolicyAttr->mib.stmt.policeSingleRate.eburst);
          DIFFSERV_PRT(msgLvlReqd, "%s confAct=%s confVal=%u\n", 
                       pSpacer,
                       dsmibPoliceActStr[pPolicyAttr->mib.stmt.policeSingleRate.conformAct],
                       pPolicyAttr->mib.stmt.policeSingleRate.conformVal);
          DIFFSERV_PRT(msgLvlReqd, "%s excdAct=%s excdVal=%u\n", 
                       pSpacer,
                       dsmibPoliceActStr[pPolicyAttr->mib.stmt.policeSingleRate.exceedAct],
                       pPolicyAttr->mib.stmt.policeSingleRate.exceedVal);
          DIFFSERV_PRT(msgLvlReqd, "%s nonconfAct=%s nonconfVal=%u\n",
                       pSpacer,
                       dsmibPoliceActStr[pPolicyAttr->mib.stmt.policeSingleRate.nonconformAct],
                       pPolicyAttr->mib.stmt.policeSingleRate.nonconformVal);
          DIFFSERV_PRT(msgLvlReqd, "%s colorConfIdx=%u colorConfMode=%s colorConfVal=%u\n",
                       pSpacer, 
                       pPolicyAttr->mib.stmt.policeSingleRate.colorConformIndex,
                       dsmibPoliceColorStr[pPolicyAttr->mib.stmt.policeSingleRate.colorConformMode],
                       pPolicyAttr->mib.stmt.policeSingleRate.colorConformVal);
          DIFFSERV_PRT(msgLvlReqd, "%s colorExcdIdx=%u colorExcdMode=%s colorExcdVal=%u\n",
                       pSpacer,
                       pPolicyAttr->mib.stmt.policeSingleRate.colorExceedIndex,
                       dsmibPoliceColorStr[pPolicyAttr->mib.stmt.policeSingleRate.colorExceedMode],
                       pPolicyAttr->mib.stmt.policeSingleRate.colorExceedVal);
          break;

        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
          DIFFSERV_PRT(msgLvlReqd, "crate=%u cburst=%u\n",
                       pPolicyAttr->mib.stmt.policeTwoRate.crate,
                       pPolicyAttr->mib.stmt.policeTwoRate.cburst);
          DIFFSERV_PRT(msgLvlReqd, "%s prate=%u pburst=%u\n",
                       pSpacer,
                       pPolicyAttr->mib.stmt.policeTwoRate.prate,
                       pPolicyAttr->mib.stmt.policeTwoRate.pburst);
          DIFFSERV_PRT(msgLvlReqd, "%s confAct=%s confVal=%u\n", 
                       pSpacer,
                       dsmibPoliceActStr[pPolicyAttr->mib.stmt.policeTwoRate.conformAct],
                       pPolicyAttr->mib.stmt.policeTwoRate.conformVal);
          DIFFSERV_PRT(msgLvlReqd, "%s excdAct=%s excdVal=%u\n",
                       pSpacer,
                       dsmibPoliceActStr[pPolicyAttr->mib.stmt.policeTwoRate.exceedAct],
                       pPolicyAttr->mib.stmt.policeTwoRate.exceedVal);
          DIFFSERV_PRT(msgLvlReqd, "%s nonconfAct=%s nonconfVal=%u\n",  
                       pSpacer,
                       dsmibPoliceActStr[pPolicyAttr->mib.stmt.policeTwoRate.nonconformAct],
                       pPolicyAttr->mib.stmt.policeTwoRate.nonconformVal);
          DIFFSERV_PRT(msgLvlReqd, "%s colorConfIdx=%u colorConfMode=%s colorConfVal=%u\n",
                       pSpacer, 
                       pPolicyAttr->mib.stmt.policeTwoRate.colorConformIndex,
                       dsmibPoliceColorStr[pPolicyAttr->mib.stmt.policeTwoRate.colorConformMode],
                       pPolicyAttr->mib.stmt.policeTwoRate.colorConformVal);
          DIFFSERV_PRT(msgLvlReqd, "%s colorExcdIdx=%u colorExcdMode=%s colorExcdVal=%u\n",
                       pSpacer,
                       pPolicyAttr->mib.stmt.policeTwoRate.colorExceedIndex,
                       dsmibPoliceColorStr[pPolicyAttr->mib.stmt.policeTwoRate.colorExceedMode],
                       pPolicyAttr->mib.stmt.policeTwoRate.colorExceedVal);
          break;

        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
          DIFFSERV_PRT(msgLvlReqd, "type=%s usp=%u/%u/%u\n", 
                       dsmibNimIntfTypeStr[pPolicyAttr->mib.stmt.redirect.configId.type],
                       (L7_uint32)pPolicyAttr->mib.stmt.redirect.configId.configSpecifier.usp.unit,
                       (L7_uint32)pPolicyAttr->mib.stmt.redirect.configId.configSpecifier.usp.slot,
                       (L7_uint32)pPolicyAttr->mib.stmt.redirect.configId.configSpecifier.usp.port);
          break;

        default:
          DIFFSERV_PRT(msgLvlReqd, "\n"); 
          break;
        }

        attrChainIndex = pPolicyAttr->chainIndex;
      }

      chainIndex = pPolicyInst->chainIndex;
    }
  }
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  /*------------------------------------------------*/
  /* Service Configuration  Data Show               */
  /*------------------------------------------------*/
  for (cfgIndex = 0; cfgIndex < L7_DIFFSERV_INTF_MAX_COUNT; cfgIndex++)
  {
    if (nimIntIfFromConfigIDGet(&pCfgData->diffServIntfCfg[cfgIndex].configId, &intIfNum) != L7_SUCCESS)
      continue;
    if (diffServMapIntfIsConfigurable(intIfNum, &pCfg) == L7_TRUE)
    {
      pSvc = &pCfg->serviceIn;
      if (pSvc->inUse == L7_TRUE)
      {
        if (svcFlag == L7_FALSE)
        {
          DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Service Entries:\n\n");
          DIFFSERV_PRT(msgLvlReqd, "Index Dir PolicyIndex Oper ST RS  Flags\n");
          DIFFSERV_PRT(msgLvlReqd, "----- --- ----------- ---- -- -- --------\n");

          svcFlag = L7_TRUE;
        }

        DIFFSERV_PRT(msgLvlReqd," %3u  %3s     %3u      %2s  %2s %2s %8.8x\n",
                     pSvc->mib.index, dsmibServiceIfDir[pSvc->mib.direction], 
                     pSvc->mib.policyIndex,
                     dsmibServiceIfOperStatus[pSvc->mib.ifOperStatus],
                     dsmibStgTypeStr[pSvc->mib.storageType],
                     dsmibRowStatusStr[pSvc->mib.rowStatus],
                     pSvc->rowInvalidFlags);
      }
      pSvc = &pCfg->serviceOut;
      if (pSvc->inUse == L7_TRUE)
      {
        if (svcFlag == L7_FALSE)
        {
          DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Service Entries:\n\n");
          DIFFSERV_PRT(msgLvlReqd, "Index Dir PolicyIndex Oper ST RS  Flags\n");
          DIFFSERV_PRT(msgLvlReqd, "----- --- ----------- ---- -- -- --------\n");

          svcFlag = L7_TRUE;
        }

        DIFFSERV_PRT(msgLvlReqd," %3u  %3s     %3u      %2s  %2s %2s %8.8x\n",
                     pSvc->mib.index, dsmibServiceIfDir[pSvc->mib.direction], 
                     pSvc->mib.policyIndex,
                     dsmibServiceIfOperStatus[pSvc->mib.ifOperStatus],
                     dsmibStgTypeStr[pSvc->mib.storageType],
                     dsmibRowStatusStr[pSvc->mib.rowStatus],
                     pSvc->rowInvalidFlags);
      }
    }
  }

  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  return; 
}

/*********************************************************************
* @purpose  Display the Configuration of Diffserv component
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void diffServConfigDataShow(void)
{
  dsmibConfigDataShow(pDiffServCfgData_g);
}

