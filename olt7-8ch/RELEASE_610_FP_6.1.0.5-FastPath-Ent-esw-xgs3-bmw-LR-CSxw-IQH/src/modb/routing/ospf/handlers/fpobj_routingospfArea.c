/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_routingospfArea.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to ospfarea-object.xml
*
* @create  1 March 2008
*
* @author  
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_routingospfArea_obj.h"
#include "usmdb_mib_ospf_api.h"
#include "usmdb_ospf_api.h"

/*******************************************************************************
* @function fpObjGet_routingospfArea_Id
*
* @purpose Get 'Id'
*
* @description [Id]: A 32-bit integer uniquely identifying an area. Area ID
*              0.0.0.0 is used for the OSPF backbone 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_Id (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIdValue;
  xLibU32_t nextObjIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & objIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    /*objIdValue = 0;
    owa.l7rc = usmDbOspfAreaEntryNext(L7_UNIT_CURRENT, objIdValue, &nextObjIdValue); */
    owa.l7rc = usmDbOspfAreaCfgGetFirst(L7_UNIT_CURRENT, &nextObjIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIdValue, owa.len);
    /*owa.l7rc = usmDbOspfAreaEntryNext (L7_UNIT_CURRENT, objIdValue,
                                       &nextObjIdValue);*/
      owa.l7rc = usmDbOspfAreaCfgGetNext(L7_UNIT_CURRENT, objIdValue,
                                       &nextObjIdValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIdValue, owa.len);

  /* return the object value: Id */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIdValue,
                           sizeof (objIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfArea_ImportAsExtern
*
* @purpose Get 'ImportAsExtern'
*
* @description [ImportAsExtern]: The area's support for importing AS external
*              link- state advertisements. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_ImportAsExtern (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objImportAsExternValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfImportAsExternGet (L7_UNIT_CURRENT, keyIdValue,
                                         &objImportAsExternValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ImportAsExtern */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objImportAsExternValue,
                           sizeof (objImportAsExternValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfArea_ImportAsExtern
*
* @purpose Set 'ImportAsExtern'
*
* @description [ImportAsExtern]: The area's support for importing AS external
*              link- state advertisements. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfArea_ImportAsExtern (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objImportAsExternValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ImportAsExtern */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objImportAsExternValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objImportAsExternValue, owa.len);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfImportAsExternSet (L7_UNIT_CURRENT, keyIdValue,
                                         objImportAsExternValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfArea_SpfRuns
*
* @purpose Get 'SpfRuns'
*
* @description [SpfRuns]: The number of times that the intra-area route table
*              has been calculated using this area's link-state database.
*              This is typically done using Dijkstra's algorithm. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_SpfRuns (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSpfRunsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNumSPFRunsGet (L7_UNIT_CURRENT, keyIdValue,
                                     &objSpfRunsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SpfRuns */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSpfRunsValue,
                           sizeof (objSpfRunsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfArea_BdrRtrCount
*
* @purpose Get 'BdrRtrCount'
*
* @description [BdrRtrCount]: The total number of area border routers reach-
*              able within this area. This is initially zero, and is calculated
*              in each SPF Pass. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_BdrRtrCount (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBdrRtrCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfAreaBorderRtrCountGet (L7_UNIT_CURRENT, keyIdValue,
                                             &objBdrRtrCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: BdrRtrCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBdrRtrCountValue,
                           sizeof (objBdrRtrCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfArea_ospfAsBdrRtrCount
*
* @purpose Get 'ospfAsBdrRtrCount'
*
* @description [ospfAsBdrRtrCount]: The total number of Autonomous System
*              border routers reachable within this area. This is initially
*              zero, and is calculated in each SPF Pass. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_ospfAsBdrRtrCount (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objospfAsBdrRtrCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfAsBdrRtrCountGet (L7_UNIT_CURRENT, keyIdValue,
                                        &objospfAsBdrRtrCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ospfAsBdrRtrCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objospfAsBdrRtrCountValue,
                           sizeof (objospfAsBdrRtrCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfArea_LsaCount
*
* @purpose Get 'LsaCount'
*
* @description [LsaCount]: The total number of link-state advertisements in
*              this area's link-state database, excluding AS External LSA's.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_LsaCount (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLsaCountValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfAreaLSACountGet (L7_UNIT_CURRENT, keyIdValue,
                                       &objLsaCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LsaCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLsaCountValue,
                           sizeof (objLsaCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfArea_LsaCksumSum
*
* @purpose Get 'LsaCksumSum'
*
* @description [LsaCksumSum]: The 32-bit unsigned sum of the link-state ad-
*              vertisements' LS checksums contained in this area's link-state
*              database. This sum excludes external (LS type 5) link-state
*              advertisements. The sum can be used to determine if
*              there has been a change in a router's link state data- base,
*              and to compare the link-state database of two routers. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_LsaCksumSum (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLsaCksumSumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfAreaLSACksumSumGet (L7_UNIT_CURRENT, keyIdValue,
                                          &objLsaCksumSumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LsaCksumSum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLsaCksumSumValue,
                           sizeof (objLsaCksumSumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfArea_Summary
*
* @purpose Get 'Summary'
*
* @description [Summary]: The variable ospfAreaSummary controls the im- port
*              of summary LSAs into stub areas. It has no effect on other
*              areas. If it is noAreaSummary, the router will neither originate
*              nor propagate summary LSAs into the stub area. It
*              will rely entirely on its de- fault route. If it is sendAreaSummary,
*              the router will both summarize and propagate summary
*              LSAs. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_Summary (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSummaryValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfAreaSummaryGet (L7_UNIT_CURRENT, keyIdValue,
                                      &objSummaryValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Summary */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSummaryValue,
                           sizeof (objSummaryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfArea_Summary
*
* @purpose Set 'Summary'
*
* @description [Summary]: The variable ospfAreaSummary controls the im- port
*              of summary LSAs into stub areas. It has no effect on other
*              areas. If it is noAreaSummary, the router will neither originate
*              nor propagate summary LSAs into the stub area. It
*              will rely entirely on its de- fault route. If it is sendAreaSummary,
*              the router will both summarize and propagate summary
*              LSAs. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfArea_Summary (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSummaryValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Summary */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSummaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSummaryValue, owa.len);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfAreaSummarySet (L7_UNIT_CURRENT, keyIdValue,
                                      objSummaryValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfArea_Status
*
* @purpose Get 'Status'
*
* @description [Status]: This variable displays the status of the en- try.
*              Setting it to 'invalid' has the effect of rendering it inoperative.
*              The internal effect (row removal) is implementation
*              dependent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfAreaStatusGet (L7_UNIT_CURRENT, keyIdValue,
                                     &objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfArea_Status
*
* @purpose Set 'Status'
*
* @description [Status]: This variable displays the status of the en- try.
*              Setting it to 'invalid' has the effect of rendering it inoperative.
*              The internal effect (row removal) is implementation
*              dependent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfArea_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* call the usmdb only for add and delete */
  if (objStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    owa.l7rc = usmDbOspfAreaStatusSet(L7_UNIT_CURRENT, keyIdValue, objStatusValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else if (objStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbOspfAreaDelete(L7_UNIT_CURRENT, keyIdValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_routingospfArea_NSSAdefInfoOrig
*
* @purpose Get 'NSSAdefInfoOrig'
*
* @description [NSSAdefInfoOrig]: Get/Set the default information origination
*              configuration for the specified NSSA 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_NSSAdefInfoOrig (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNSSAdefInfoOrigValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNSSADefaultInfoOriginateGet(L7_UNIT_CURRENT, keyIdValue,
                                                 &objNSSAdefInfoOrigValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: NSSAdefInfoOrig */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNSSAdefInfoOrigValue,
                           sizeof (objNSSAdefInfoOrigValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfArea_NSSAdefInfoOrig
*
* @purpose Set 'NSSAdefInfoOrig'
*
* @description [NSSAdefInfoOrig]: Get/Set the default information origination
*              configuration for the specified NSSA 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfArea_NSSAdefInfoOrig (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNSSAdefInfoOrigValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NSSAdefInfoOrig */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objNSSAdefInfoOrigValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNSSAdefInfoOrigValue, owa.len);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* set the value in application */
  if(L7_FALSE == objNSSAdefInfoOrigValue)
  {
    /* When default-info-originate is disabled, reset the metric and metric type values to defaults */
    owa.l7rc = usmDbOspfNSSADefaultInfoSet(L7_UNIT_CURRENT, keyIdValue,
                                           L7_FALSE, FD_OSPF_NSSA_DEFAULT_METRIC_TYPE, FD_OSPF_NSSA_DEFAULT_METRIC);
  }
  else
  {
    owa.l7rc = usmDbOspfNSSADefaultInfoOriginateSet(L7_UNIT_CURRENT, keyIdValue,
                                                   objNSSAdefInfoOrigValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfArea_NSSAdefMetric
*
* @purpose Get 'NSSAdefMetric'
*
* @description [NSSAdefMetric]: Get/Set the NSSA default metric value. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_NSSAdefMetric (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNSSAdefMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNSSADefaultMetricGet (L7_UNIT_CURRENT, keyIdValue,
                                            &objNSSAdefMetricValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: NSSAdefMetric */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNSSAdefMetricValue,
                           sizeof (objNSSAdefMetricValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfArea_NSSAdefMetric
*
* @purpose Set 'NSSAdefMetric'
*
* @description [NSSAdefMetric]: Get/Set the NSSA default metric value. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfArea_NSSAdefMetric (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNSSAdefMetricValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NSSAdefMetric */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objNSSAdefMetricValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNSSAdefMetricValue, owa.len);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfNSSADefaultMetricSet (L7_UNIT_CURRENT, keyIdValue,
                                            objNSSAdefMetricValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfArea_NSSAdefMetricType
*
* @purpose Get 'NSSAdefMetricType'
*
* @description [NSSAdefMetricType]: Set/Get the metric type of the default
*              route for the NSSA 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_NSSAdefMetricType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNSSAdefMetricTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNSSADefaultMetricTypeGet (L7_UNIT_CURRENT, keyIdValue,
                                                &objNSSAdefMetricTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: NSSAdefMetricType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNSSAdefMetricTypeValue,
                           sizeof (objNSSAdefMetricTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfArea_NSSAdefMetricType
*
* @purpose Set 'NSSAdefMetricType'
*
* @description [NSSAdefMetricType]: Set/Get the metric type of the default
*              route for the NSSA 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfArea_NSSAdefMetricType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNSSAdefMetricTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NSSAdefMetricType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objNSSAdefMetricTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNSSAdefMetricTypeValue, owa.len);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfNSSADefaultMetricTypeSet (L7_UNIT_CURRENT, keyIdValue,
                                                objNSSAdefMetricTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfArea_NSSAimportSummaries
*
* @purpose Get 'NSSAimportSummaries'
*
* @description [NSSAimportSummaries]: Get/Set the import summary configuration
*              for the specified NSSA 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_NSSAimportSummaries (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNSSAimportSummariesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNSSAImportSummariesGet (L7_UNIT_CURRENT, keyIdValue,
                                              &objNSSAimportSummariesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: NSSAimportSummaries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNSSAimportSummariesValue,
                           sizeof (objNSSAimportSummariesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfArea_NSSAimportSummaries
*
* @purpose Set 'NSSAimportSummaries'
*
* @description [NSSAimportSummaries]: Get/Set the import summary configuration
*              for the specified NSSA 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfArea_NSSAimportSummaries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNSSAimportSummariesValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NSSAimportSummaries */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objNSSAimportSummariesValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNSSAimportSummariesValue, owa.len);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfNSSAImportSummariesSet (L7_UNIT_CURRENT, keyIdValue,
                                              objNSSAimportSummariesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfArea_NSSAredistribute
*
* @purpose Get 'NSSAredistribute'
*
* @description [NSSAredistribute]: Get/Set the route redistribution configuration
*              for the specified NSSA 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_NSSAredistribute (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNSSAredistributeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNSSARedistributeGet (L7_UNIT_CURRENT, keyIdValue,
                                           &objNSSAredistributeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: NSSAredistribute */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNSSAredistributeValue,
                           sizeof (objNSSAredistributeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfArea_NSSAredistribute
*
* @purpose Set 'NSSAredistribute'
*
* @description [NSSAredistribute]: Get/Set the route redistribution configuration
*              for the specified NSSA 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfArea_NSSAredistribute (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNSSAredistributeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NSSAredistribute */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objNSSAredistributeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNSSAredistributeValue, owa.len);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfNSSARedistributeSet (L7_UNIT_CURRENT, keyIdValue,
                                           objNSSAredistributeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfArea_NSSAtranslatorRole
*
* @purpose Get 'NSSAtranslatorRole'
*
* @description [NSSAtranslatorRole]: Get/Set the NSSA Translator Role of the
*              specified NSSA 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_NSSAtranslatorRole (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNSSAtranslatorRoleValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNSSATranslatorRoleGet (L7_UNIT_CURRENT, keyIdValue,
                                             &objNSSAtranslatorRoleValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: NSSAtranslatorRole */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNSSAtranslatorRoleValue,
                           sizeof (objNSSAtranslatorRoleValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfArea_NSSAtranslatorRole
*
* @purpose Set 'NSSAtranslatorRole'
*
* @description [NSSAtranslatorRole]: Get/Set the NSSA Translator Role of the
*              specified NSSA 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfArea_NSSAtranslatorRole (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNSSAtranslatorRoleValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NSSAtranslatorRole */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objNSSAtranslatorRoleValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNSSAtranslatorRoleValue, owa.len);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfNSSATranslatorRoleSet (L7_UNIT_CURRENT, keyIdValue,
                                             objNSSAtranslatorRoleValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfArea_NSSAtranslatorStabilityInterval
*
* @purpose Get 'NSSAtranslatorStabilityInterval'
*
* @description [NSSAtranslatorStabilityInterval]: Get/Set the Translator Stability
*              Interval of the specified NSSA 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_NSSAtranslatorStabilityInterval (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNSSAtranslatorStabilityIntervalValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbOspfNSSATranslatorStabilityIntervalGet (L7_UNIT_CURRENT, keyIdValue,
                                                 &objNSSAtranslatorStabilityIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: NSSAtranslatorStabilityInterval */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objNSSAtranslatorStabilityIntervalValue,
                    sizeof (objNSSAtranslatorStabilityIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_routingospfArea_NSSAtranslatorStabilityInterval
*
* @purpose Set 'NSSAtranslatorStabilityInterval'
*
* @description [NSSAtranslatorStabilityInterval]: Get/Set the Translator Stability
*              Interval of the specified NSSA 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfArea_NSSAtranslatorStabilityInterval (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNSSAtranslatorStabilityIntervalValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NSSAtranslatorStabilityInterval */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objNSSAtranslatorStabilityIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNSSAtranslatorStabilityIntervalValue, owa.len);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbOspfNSSATranslatorStabilityIntervalSet (L7_UNIT_CURRENT, keyIdValue,
                                                 objNSSAtranslatorStabilityIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfArea_NSSATranslatorState
*
* @purpose Get 'NSSATranslatorState'
*
* @description [NSSATranslatorState]: Get the NSSA Translator State of the
*              specified NSSA 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_NSSATranslatorState (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNSSATranslatorStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  kwa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id,
                          (xLibU8_t *) & keyIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfNSSATranslatorStateGet(L7_UNIT_CURRENT, keyIdValue,
                                            &objNSSATranslatorStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: NSSATranslatorState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNSSATranslatorStateValue,
                           sizeof (objNSSATranslatorStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_routingospfArea_ImportSummaryLSA
*
* @purpose Get 'ImportSummaryLSA'
 *@description  [ImportSummaryLSA] Summary LSA mode for a given stub area.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_ImportSummaryLSA (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objImportSummaryLSAValue;

  xLibIpV4_t keyIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  owa.len = sizeof (keyIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id, (xLibU8_t *) & keyIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfMapAreaSummaryGet (L7_UNIT_CURRENT, keyIdValue, &objImportSummaryLSAValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objImportSummaryLSAValue, sizeof (objImportSummaryLSAValue));

  /* return the object value: ImportSummaryLSA */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objImportSummaryLSAValue,
                           sizeof (objImportSummaryLSAValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfArea_ImportSummaryLSA
*
* @purpose Set 'ImportSummaryLSA'
 *@description  [ImportSummaryLSA] Summary LSA mode for a given stub area.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfArea_ImportSummaryLSA (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objImportSummaryLSAValue;

  xLibIpV4_t keyIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ImportSummaryLSA */
  owa.len = sizeof (objImportSummaryLSAValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objImportSummaryLSAValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objImportSummaryLSAValue, owa.len);

  /* retrieve key: Id */
  owa.len = sizeof (keyIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id, (xLibU8_t *) & keyIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfMapAreaSummarySet (L7_UNIT_CURRENT, keyIdValue, objImportSummaryLSAValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_routingospfArea_NSSAStatus
*
* @purpose Get 'NSSAStatus'
 *@description  [NSSAStatus] This variable displays the status of the entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfArea_NSSAStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objNSSAStatusValue;

  xLibIpV4_t keyIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Id */
  owa.len = sizeof (keyIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id, (xLibU8_t *) & keyIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbOspfImportAsExternGet (L7_UNIT_CURRENT, keyIdValue, &objNSSAStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objNSSAStatusValue, sizeof (objNSSAStatusValue));

  /* return the object value: NSSAStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNSSAStatusValue, sizeof (objNSSAStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfArea_NSSAStatus
*
* @purpose Set 'NSSAStatus'
 *@description  [NSSAStatus] This variable displays the status of the entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfArea_NSSAStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objNSSAStatusValue;

  xLibIpV4_t keyIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NSSAStatus */
  owa.len = sizeof (objNSSAStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objNSSAStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNSSAStatusValue, owa.len);

  /* retrieve key: Id */
  owa.len = sizeof (keyIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingospfArea_Id, (xLibU8_t *) & keyIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIdValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (objNSSAStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    owa.l7rc = usmDbOspfImportAsExternSet (L7_UNIT_CURRENT, keyIdValue, L7_OSPF_AREA_IMPORT_NSSA);
  }
  else if (objNSSAStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc = usmDbOspfImportAsExternSet (L7_UNIT_CURRENT, keyIdValue, L7_OSPF_AREA_IMPORT_EXT);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
