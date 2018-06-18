/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingStpMstPortConfigGroup.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to MST-object.xml
*
* @create  13 February 2008
*
* @author  Radha K
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingStpMstPortConfigGroup_obj.h"
#include "usmdb_dot1s_api.h"
#include "dot1s_exports.h"
#include "usmdb_util_api.h"
#include "usmdb_nim_api.h"

extern void fpObj_StpDot1sBridgeId(xLibU8_t *buf, xLibU8_t *temp, xLibU32_t val);
extern void fpObj_StpDot1sPortId(xLibU8_t * buf, xLibU32_t val);

xLibU32_t fpObj_StpPortForwardingStringGet(xLibU32_t val, xLibS8_t *forwardingState);

/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_MstId
*
* @purpose Get 'MstId'
 *@description  [MstId] The MSTP instance ID   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstPortConfigGroup_MstId (void *wap, void *bufp)
{

  xLibU32_t objMstIdValue;
  xLibU32_t nextObjMstIdValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                          (xLibU8_t *) & objMstIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objMstIdValue, 0, sizeof (objMstIdValue));
    memset (&objInterfaceValue, 0, sizeof (objInterfaceValue));
    nextObjMstIdValue = objMstIdValue ;
    nextObjInterfaceValue = objInterfaceValue;

    do
    {
       owa.l7rc = usmDbDot1sMstiPortNextGet(L7_UNIT_CURRENT, nextObjMstIdValue,
                                             &nextObjMstIdValue, nextObjInterfaceValue,
                                             &nextObjInterfaceValue);
    }
    while ((objMstIdValue == nextObjMstIdValue) && (owa.l7rc == L7_SUCCESS));

    if ((owa.l7rc != L7_SUCCESS))
    {
       owa.rc = XLIBRC_ENDOF_TABLE;
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
    }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objMstIdValue, owa.len);
    memset (&objInterfaceValue, 0, sizeof (objInterfaceValue));

    nextObjMstIdValue = objMstIdValue ;
    nextObjInterfaceValue = objInterfaceValue;

    do
    {
       owa.l7rc = usmDbDot1sMstiPortNextGet(L7_UNIT_CURRENT, nextObjMstIdValue,
                                             &nextObjMstIdValue, nextObjInterfaceValue,
                                             &nextObjInterfaceValue);
    }
    while ((objMstIdValue == nextObjMstIdValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjMstIdValue, owa.len);

  /*return the object value: MstId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjMstIdValue, sizeof (objMstIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_SwitchingStpMstPortConfigGroup_MstId
*
* @purpose Get 'MstId'
*
* @description [MstId]: MST Instance
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingStpMstPortConfigGroup_MstId(void *wap,void *bufp)
{
  return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_SwitchingStpMstPortConfigGroup_MstId,
      L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX, 1);
}

/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] Interfaces on which MSTP parameters are to be
* configured   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstPortConfigGroup_Interface (void *wap, void *bufp)
{

  xLibU32_t objMstIdValue;
  xLibU32_t nextObjMstIdValue;
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                          (xLibU8_t *) & objMstIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objMstIdValue, owa.len);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objInterfaceValue, 0, sizeof (objInterfaceValue));
    owa.l7rc = usmDbDot1sMstiPortNextGet(L7_UNIT_CURRENT,
                                     objMstIdValue,
                                     &nextObjMstIdValue, objInterfaceValue, &nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    
    nextObjMstIdValue = objMstIdValue ;
    nextObjInterfaceValue = objInterfaceValue; 

    owa.l7rc = usmDbDot1sMstiPortNextGet (L7_UNIT_CURRENT,
                                    nextObjMstIdValue,
                                    &nextObjMstIdValue, nextObjInterfaceValue, &nextObjInterfaceValue);

  }

  if ((objMstIdValue != nextObjMstIdValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue, sizeof (objInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_SwitchingStpMstPortConfigGroup_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] Interfaces on which MSTP parameters are to be
* configured   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingStpMstPortConfigGroup_Interface (void *wap, void *bufp)
{

  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset (&objInterfaceValue, 0, sizeof (objInterfaceValue));
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    

  }
  owa.l7rc = usmDbValidIntIfNumNext(objInterfaceValue,&nextObjInterfaceValue);

  if ( owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue, sizeof (objInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortForwardingState
*
* @purpose Get 'StpMstPortForwardingState'
*
* @description [StpMstPortForwardingState]: The MSTP forwarding state of a
*              specific port in a specific instance. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortForwardingState (void
                                                                            *wap,
                                                                            void
                                                                            *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  xLibU32_t tempKeyInterfaceValue,val;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objStpMstPortForwardingStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                           (xLibU8_t *) & keyMstIdValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa1.len);

  /* retrieve key: Interface */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                           (xLibU8_t *) & keyInterfaceValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa2.len);
  
  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* get the value from application */
  owa.l7rc =
    usmDbDot1sMstiPortForwardingStateGet (L7_UNIT_CURRENT, keyMstIdValue,
                                          tempKeyInterfaceValue,
                                          &val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (fpObj_StpPortForwardingStringGet(val, objStpMstPortForwardingStateValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* return the object value: StpMstPortForwardingState */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objStpMstPortForwardingStateValue,
                    strlen (objStpMstPortForwardingStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortId
*
* @purpose Get 'StpMstPortId'
*
* @description [StpMstPortId]: The MSTP port identifier of a specific port
*              in a specific instance 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortId (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  xLibU32_t tempKeyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objStpMstPortIdValue;
  xLibU32_t portId;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objStpMstPortIdValue, 0x0, sizeof(objStpMstPortIdValue));

  /* retrieve key: MstId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                           (xLibU8_t *) & keyMstIdValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa1.len);

  /* retrieve key: Interface */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                           (xLibU8_t *) & keyInterfaceValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa2.len);

  
  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbDot1sMstiPortIDGet (L7_UNIT_CURRENT, keyMstIdValue,
                                      tempKeyInterfaceValue, 
                                      &portId);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  fpObj_StpDot1sPortId(objStpMstPortIdValue, portId);

  /* return the object value: StpMstPortId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpMstPortIdValue,
                           strlen (objStpMstPortIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortPathCost
*
* @purpose Get 'StpMstPortPathCost'
*
* @description [StpMstPortPathCost]: The MSTP port path cost in a specific
*              instance. The default value will correspond to the recommendation
*              specified in IEEE 802.1s Table 13-2 which varies depending
*              upon link speed. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortPathCost (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  xLibU32_t tempKeyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpMstPortPathCostValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                           (xLibU8_t *) & keyMstIdValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa1.len);

  /* retrieve key: Interface */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                           (xLibU8_t *) & keyInterfaceValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa2.len);

  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbDot1sMstiPortPathCostGet (L7_UNIT_CURRENT, keyMstIdValue,
                                            tempKeyInterfaceValue,
                                            &objStpMstPortPathCostValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpMstPortPathCost */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpMstPortPathCostValue,
                           sizeof (objStpMstPortPathCostValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingStpMstPortConfigGroup_StpMstPortPathCost
*
* @purpose Set 'StpMstPortPathCost'
*
* @description [StpMstPortPathCost]: The MSTP port path cost in a specific
*              instance. The default value will correspond to the recommendation
*              specified in IEEE 802.1s Table 13-2 which varies depending
*              upon link speed. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingStpMstPortConfigGroup_StpMstPortPathCost (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpMstPortPathCostValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  xLibU32_t tempKeyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StpMstPortPathCost */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objStpMstPortPathCostValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStpMstPortPathCostValue, owa.len);

  /* retrieve key: MstId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                           (xLibU8_t *) & keyMstIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa1.len);

  /* retrieve key: Interface */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                           (xLibU8_t *) & keyInterfaceValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa2.len);
  
  
  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbDot1sMstiPortPathCostSet (L7_UNIT_CURRENT, keyMstIdValue,
                                            tempKeyInterfaceValue,
                                            objStpMstPortPathCostValue);
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
* @function fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortPriority
*
* @purpose Get 'StpMstPortPriority'
*
* @description [StpMstPortPriority]: The MSTP port priority in a specific
*              instance. The priority is in the increments of 16. The default
*              value is 128. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortPriority (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  xLibU32_t tempKeyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpMstPortPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                           (xLibU8_t *) & keyMstIdValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa1.len);

  /* retrieve key: Interface */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                           (xLibU8_t *) & keyInterfaceValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa2.len);

  
  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbDot1sMstiPortPriorityGet (L7_UNIT_CURRENT, keyMstIdValue,
                                            tempKeyInterfaceValue,
                                            &objStpMstPortPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpMstPortPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpMstPortPriorityValue,
                           sizeof (objStpMstPortPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingStpMstPortConfigGroup_StpMstPortPriority
*
* @purpose Set 'StpMstPortPriority'
*
* @description [StpMstPortPriority]: The MSTP port priority in a specific
*              instance. The priority is in the increments of 16. The default
*              value is 128. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingStpMstPortConfigGroup_StpMstPortPriority (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpMstPortPriorityValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  xLibU32_t tempKeyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* Declara a temp variable for priority */
  xLibU32_t tempPriorityValue;

  /* retrieve object: StpMstPortPriority */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objStpMstPortPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStpMstPortPriorityValue, owa.len);


  /* Copy the priority value in temp */
  tempPriorityValue = objStpMstPortPriorityValue;

  /* retrieve key: MstId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                           (xLibU8_t *) & keyMstIdValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa1.len);

  /* retrieve key: Interface */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                           (xLibU8_t *) & keyInterfaceValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa2.len);

  
  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* set the value in application */
  owa.l7rc = usmDbDot1sMstiPortPrioritySet (L7_UNIT_CURRENT, keyMstIdValue,
                                            tempKeyInterfaceValue,
                                            &objStpMstPortPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  /* Check if the same priority value is set if
   * not return this error code. This is to keep
   * the error handling consistent accross old and New CLI
   */
  if(tempPriorityValue != objStpMstPortPriorityValue)
  {
    owa.rc = XLIBRC_PORT_PRIORITY_SET;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_StpMstDesignatedBridgeId
*
* @purpose Get 'StpMstDesignatedBridgeId'
*
* @description [StpMstDesignatedBridgeId]: The MSTP designated bridge ID of
*              a specific port in a specific instance. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstPortConfigGroup_StpMstDesignatedBridgeId (void
                                                                           *wap,
                                                                           void
                                                                           *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  xLibU32_t tempKeyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objStpMstDesignatedBridgeIdValue={0};
  xLibStr256_t bridgeId={0};
  xLibU32_t length;
  FPOBJ_TRACE_ENTER (bufp);
  
  memset(objStpMstDesignatedBridgeIdValue,0x0,sizeof(objStpMstDesignatedBridgeIdValue));
  /* retrieve key: MstId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                           (xLibU8_t *) & keyMstIdValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa1.len);

  /* retrieve key: Interface */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                           (xLibU8_t *) & keyInterfaceValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa2.len);

  
  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc =
    usmDbDot1sMstiDesignatedBridgeIDGet (L7_UNIT_CURRENT, keyMstIdValue,
                                         tempKeyInterfaceValue,
                                         bridgeId,
                                         &length);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  fpObj_StpDot1sBridgeId(objStpMstDesignatedBridgeIdValue, bridgeId, length);

  /* return the object value: StpMstDesignatedBridgeId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpMstDesignatedBridgeIdValue,
                           strlen (objStpMstDesignatedBridgeIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_StpMstDesignatedCost
*
* @purpose Get 'StpMstDesignatedCost'
*
* @description [StpMstDesignatedCost]: he MSTP designated cost of a specific
*              port in a specific instance. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstPortConfigGroup_StpMstDesignatedCost (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  xLibU32_t tempKeyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpMstDesignatedCostValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                           (xLibU8_t *) & keyMstIdValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa1.len);

  /* retrieve key: Interface */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                           (xLibU8_t *) & keyInterfaceValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa2.len);

  
  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbDot1sMstiDesignatedCostGet (L7_UNIT_CURRENT, keyMstIdValue,
                                              tempKeyInterfaceValue,
                                              &objStpMstDesignatedCostValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpMstDesignatedCost */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpMstDesignatedCostValue,
                           sizeof (objStpMstDesignatedCostValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_StpMstDesignatedPortId
*
* @purpose Get 'StpMstDesignatedPortId'
*
* @description [StpMstDesignatedPortId]: The MSTP designated port ID of a
*              specific port in a specific instance. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstPortConfigGroup_StpMstDesignatedPortId (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  xLibU32_t tempKeyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objStpMstDesignatedPortIdValue;
  xLibU16_t portId;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objStpMstDesignatedPortIdValue, 0x0, sizeof(objStpMstDesignatedPortIdValue));
  /* retrieve key: MstId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                           (xLibU8_t *) & keyMstIdValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa1.len);

  /* retrieve key: Interface */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                           (xLibU8_t *) & keyInterfaceValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa2.len);
  
  
  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbDot1sMstiDesignatedPortIDGet (L7_UNIT_CURRENT, keyMstIdValue,
                                                tempKeyInterfaceValue,
                                                &portId);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  fpObj_StpDot1sPortId(objStpMstDesignatedPortIdValue, portId);


  /* return the object value: StpMstDesignatedPortId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpMstDesignatedPortIdValue,
                           strlen (objStpMstDesignatedPortIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortLoopInconsistentState
*
* @purpose Get 'StpMstPortLoopInconsistentState'
 *@description  [StpMstPortLoopInconsistentState] The loop inconsistent state of
* a specific port in a specific instance.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortLoopInconsistentState (void *wap,
                                                                                  void *bufp)
{
  fpObjWa_t kwaMstId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpMstPortLoopInconsistentStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwaMstId.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                               (xLibU8_t *) & keyMstIdValue, &kwaMstId.len);
  if (kwaMstId.rc != XLIBRC_SUCCESS)
  {
    kwaMstId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMstId);
    return kwaMstId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwaMstId.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1sMstiPortLoopInconsistentStateGet (L7_UNIT_CURRENT, keyMstIdValue,
                                                                                      keyInterfaceValue, &objStpMstPortLoopInconsistentStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpMstPortLoopInconsistentState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpMstPortLoopInconsistentStateValue,
                           sizeof (objStpMstPortLoopInconsistentStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortTransitionsIntoLoopInconsistentState
*
* @purpose Get 'StpMstPortTransitionsIntoLoopInconsistentState'
 *@description  [StpMstPortTransitionsIntoLoopInconsistentState] The MSTP
* designated cost of a specific port in a specific instance.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortTransitionsIntoLoopInconsistentState (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t kwaMstId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpMstPortTransitionsIntoLoopInconsistentStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwaMstId.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                               (xLibU8_t *) & keyMstIdValue, &kwaMstId.len);
  if (kwaMstId.rc != XLIBRC_SUCCESS)
  {
    kwaMstId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMstId);
    return kwaMstId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwaMstId.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1sPortStatsTransitionsIntoLoopInconsistentStateGet (L7_UNIT_CURRENT, keyMstIdValue,
                                                                                                             keyInterfaceValue,
                                                                                                             &objStpMstPortTransitionsIntoLoopInconsistentStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpMstPortTransitionsIntoLoopInconsistentState */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objStpMstPortTransitionsIntoLoopInconsistentStateValue,
                    sizeof (objStpMstPortTransitionsIntoLoopInconsistentStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortTransitionsOutOfLoopInconsistentState
*
* @purpose Get 'StpMstPortTransitionsOutOfLoopInconsistentState'
 *@description  [StpMstPortTransitionsOutOfLoopInconsistentState] The MSTP
* designated cost of a specific port in a specific instance.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortTransitionsOutOfLoopInconsistentState (void *wap,
                                                                                         void *bufp)
{

  fpObjWa_t kwaMstId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpMstPortTransitionsOutOfLoopInconsistentStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwaMstId.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                               (xLibU8_t *) & keyMstIdValue, &kwaMstId.len);
  if (kwaMstId.rc != XLIBRC_SUCCESS)
  {
    kwaMstId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMstId);
    return kwaMstId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwaMstId.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1sPortStatsTransitionsOutOfLoopInconsistentStateGet (L7_UNIT_CURRENT, keyMstIdValue,
                                                                                                                keyInterfaceValue,
                                                                                                                &objStpMstPortTransitionsOutOfLoopInconsistentStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpMstPortTransitionsOutOfLoopInconsistentState */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objStpMstPortTransitionsOutOfLoopInconsistentStateValue,
                    sizeof (objStpMstPortTransitionsOutOfLoopInconsistentStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_Dot1sMstPortDesignatedRootID
*
* @purpose Get 'Dot1sMstPortDesignatedRootID'
*
* @description [Dot1sMstPortDesignatedRootID]: TODO 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstPortConfigGroup_Dot1sMstPortDesignatedRootID(void *wap, void *bufp) 
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  xLibU32_t tempKeyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDot1sMstPortDesignatedRootIDValue={0};
  xLibStr256_t rootId={0};
  xLibU32_t len;

  FPOBJ_TRACE_ENTER (bufp);
  memset(objDot1sMstPortDesignatedRootIDValue, 0x0, sizeof(objDot1sMstPortDesignatedRootIDValue));

  /* retrieve key: MstId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                           (xLibU8_t *) & keyMstIdValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa1.len);

  /* retrieve key: Interface */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                           (xLibU8_t *) & keyInterfaceValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa2.len);

  
  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbDot1sMstiPortDesignatedRootIDGet (L7_UNIT_CURRENT, keyMstIdValue,
                                      tempKeyInterfaceValue, rootId, &len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  fpObj_StpDot1sBridgeId(objDot1sMstPortDesignatedRootIDValue, rootId, len);

  /* return the object value: Dot1sMstPortDesignatedRootIDValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDot1sMstPortDesignatedRootIDValue,
                           strlen (objDot1sMstPortDesignatedRootIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_Dot1sMstPortPathCostMode
*
* @purpose Get 'Dot1sMstPortPathCostMode'
*
* @description [Dot1sMstPortPathCostMode]: TODO 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstPortConfigGroup_Dot1sMstPortPathCostMode(void *wap, void *bufp) 
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  xLibU32_t tempKeyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1sMstPortPathCostModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                           (xLibU8_t *) & keyMstIdValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa1.len);

  /* retrieve key: Interface */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                           (xLibU8_t *) & keyInterfaceValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa2.len);

  
  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbDot1sMstiPortPathCostModeGet (L7_UNIT_CURRENT, keyMstIdValue,
                                              tempKeyInterfaceValue,&objDot1sMstPortPathCostModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot1sMstPortPathCostModeValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1sMstPortPathCostModeValue,
                           sizeof (objDot1sMstPortPathCostModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_Dot1sMstPortRole
*
* @purpose Get 'Dot1sMstPortRole'
*
* @description [Dot1sMstPortRole]: TODO 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstPortConfigGroup_Dot1sMstPortRole(void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  xLibU32_t tempKeyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t val;
  xLibStr256_t objDot1sMstPortRoleValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                           (xLibU8_t *) & keyMstIdValue, &kwa1.len);
  if (kwa1.l7rc != L7_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa1.len);

  /* retrieve key: Interface */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                           (xLibU8_t *) & keyInterfaceValue, &kwa2.len);
  if (kwa2.l7rc != L7_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa2.len);

  
  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbDot1sMstiPortRoleGet (L7_UNIT_CURRENT, keyMstIdValue,
                                        tempKeyInterfaceValue,
                                        &val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  switch (val)
  {
      case L7_DOT1S_ROLE_DISABLED:
        strcpy(objDot1sMstPortRoleValue, "Disabled");
        break;
      case L7_DOT1S_ROLE_ROOT:
        strcpy(objDot1sMstPortRoleValue, "Root");
        break;
      case L7_DOT1S_ROLE_DESIGNATED:
        strcpy(objDot1sMstPortRoleValue, "Designated");
        break;
      case L7_DOT1S_ROLE_ALTERNATE:
        strcpy(objDot1sMstPortRoleValue, "Alternate");
        break;
      case L7_DOT1S_ROLE_BACKUP:
        strcpy(objDot1sMstPortRoleValue, "Backup");
        break;
      case L7_DOT1S_ROLE_MASTER:
        strcpy(objDot1sMstPortRoleValue, "Master");
        break;
      default:
        strcpy(objDot1sMstPortRoleValue, "-");
        break;
  }


  /* return the object value: Dot1sMstPortRoleValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDot1sMstPortRoleValue,
                           strlen (objDot1sMstPortRoleValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#if 0
/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortLoopInconsistentState
*
* @purpose Get 'StpMstPortLoopInconsistentState'
 *@description  [StpMstPortLoopInconsistentState] TODO   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortLoopInconsistentState (void *wap,
                                                                                  void *bufp)
{

  fpObjWa_t kwaMstId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpMstPortLoopInconsistentStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwaMstId.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                               (xLibU8_t *) & keyMstIdValue, &kwaMstId.len);
  if (kwaMstId.rc != XLIBRC_SUCCESS)
  {
    kwaMstId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMstId);
    return kwaMstId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwaMstId.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */ 
  owa.l7rc = usmDbDot1sMstiPortLoopInconsistentStateGet(L7_UNIT_CURRENT, keyMstIdValue,
                              keyInterfaceValue, &objStpMstPortLoopInconsistentStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpMstPortLoopInconsistentState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpMstPortLoopInconsistentStateValue,
                           sizeof (objStpMstPortLoopInconsistentStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortTransitionsIntoLoopInconsistentState
*
* @purpose Get 'StpMstPortTransitionsIntoLoopInconsistentState'
 *@description  [StpMstPortTransitionsIntoLoopInconsistentState] TODO   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortTransitionsIntoLoopInconsistentState (void *wap,
                                                                                        void *bufp)
{

  fpObjWa_t kwaMstId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpMstPortTransitionsIntoLoopInconsistentStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwaMstId.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                               (xLibU8_t *) & keyMstIdValue, &kwaMstId.len);
  if (kwaMstId.rc != XLIBRC_SUCCESS)
  {
    kwaMstId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMstId);
    return kwaMstId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwaMstId.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */ 
  owa.l7rc = usmDbDot1sPortStatsTransitionsIntoLoopInconsistentStateGet(L7_UNIT_CURRENT, keyMstIdValue,
                              keyInterfaceValue,
                              &objStpMstPortTransitionsIntoLoopInconsistentStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpMstPortTransitionsIntoLoopInconsistentState */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objStpMstPortTransitionsIntoLoopInconsistentStateValue,
                    sizeof (objStpMstPortTransitionsIntoLoopInconsistentStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortTransitionsOutOfLoopInconsistentState
*
* @purpose Get 'StpMstPortTransitionsOutOfLoopInconsistentState'
 *@description  [StpMstPortTransitionsOutOfLoopInconsistentState] TODO   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_SwitchingStpMstPortConfigGroup_StpMstPortTransitionsOutOfLoopInconsistentState (void *wap,
                                                                                         void *bufp)
{

  fpObjWa_t kwaMstId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpMstPortTransitionsOutOfLoopInconsistentStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwaMstId.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                               (xLibU8_t *) & keyMstIdValue, &kwaMstId.len);
  if (kwaMstId.rc != XLIBRC_SUCCESS)
  {
    kwaMstId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMstId);
    return kwaMstId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwaMstId.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */ 
  owa.l7rc = usmDbDot1sPortStatsTransitionsOutOfLoopInconsistentStateGet(L7_UNIT_CURRENT, keyMstIdValue,
                              keyInterfaceValue,
                              &objStpMstPortTransitionsOutOfLoopInconsistentStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpMstPortTransitionsOutOfLoopInconsistentState */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objStpMstPortTransitionsOutOfLoopInconsistentStateValue,
                    sizeof (objStpMstPortTransitionsOutOfLoopInconsistentStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*********************************************************************
* @purpose  Format the provided char* into 802.1 bridge identifier format
*
* @param    stat output string
* @param    stat output string buffer size
* @param    idIn input string to be formatted
*
* @returns void
*
* @notes none
*
* @end
*********************************************************************/

void fpObj_StpFormatBridgeIdentifier( xLibS8_t * stat, xLibU32_t statSize, xLibU8_t * idIn )
{
  osapiSnprintf(stat, statSize, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                idIn[0], idIn[1], idIn[2], idIn[3], idIn[4], idIn[5], idIn[6], idIn[7]);
  return;
}

/*********************************************************************
* @purpose  Format the provided ushort16 port identifier into a string
*           displayable in the format xx:xx.
*
* @param    stat output string. Must be at least 5 bytes long.
* @param    stat output string buffer size
* @param    val16 input val
*
* @returns void
*
* @notes
*
* @end
*********************************************************************/

void fpObj_StpFormatPortIdentifier( xLibS8_t * stat, xLibU32_t statSize, xLibU16_t val16 )
{
  xLibS8_t temp[5];

  memset (temp, 0,sizeof(temp));
  memset (stat, 0, statSize);
  osapiSnprintf(temp, sizeof(temp), "%04X",val16);
  osapiSnprintf(stat, statSize, "%c%c:%c%c", temp[0], temp[1], temp[2], temp[3]);

  return;
}
#endif

xLibU32_t fpObj_StpPortForwardingStringGet(xLibU32_t val, xLibS8_t *forwardingState)
{
  switch (val)
  {
    case L7_DOT1S_DISCARDING:
      sprintf( forwardingState, "Discarding" );        /* "Discarding" */
      break;
    case L7_DOT1S_LEARNING:
      sprintf( forwardingState, "Learning" );        /* "Learning" */
      break;
    case L7_DOT1S_FORWARDING:
      sprintf( forwardingState, "Forwarding" );        /* "Forwarding" */
      break;
    case L7_DOT1S_MANUAL_FWD:
      sprintf( forwardingState, "Manual forwarding" );        /* "Manual forwarding" */
      break;
    case L7_DOT1S_NOT_PARTICIPATE:
      sprintf( forwardingState, "Not participating" );        /* "Not participating" */
      break;
    case L7_DOT1S_DISABLED:
      sprintf( forwardingState, "Disabled" );        /* "Disabled" */
      break;
    default:
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*******************************************************************************

* @function fpObjGet_SwitchingStpMstPortConfigGroup_StpMstRowStatus
*
 @purpose Get 'StpMstRowStatus'
*
 @description [StpMstRowStatus]: The MSTP instance status. Supported values:
*              Add - used to create a new instance.Remove - removes an
*              instance.
*
* @return

*******************************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstPortConfigGroup_StpMstRowStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpMstRowStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                          (xLibU8_t *) & keyMstIdValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa.len);
  /* get the value from application */
  owa.l7rc = L7_SUCCESS;
  if(usmDbDot1sInstanceGet(L7_UNIT_CURRENT,keyMstIdValue) == L7_SUCCESS)
  {
     owa.l7rc = L7_SUCCESS;
     objStpMstRowStatusValue = L7_ROW_STATUS_ACTIVE;
  }
  else
  {
     owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StpMstRowStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpMstRowStatusValue,
                           sizeof (objStpMstRowStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************

* @function fpObjSet_SwitchingStpMstPortConfigGroup_StpMstRowStatus
*
* @purpose Set 'StpMstRowStatus'
*
* @description [StpMstRowStatus]: The MSTP instance status. Supported values:
*              Add - used to create a new instance.Remove - removes an
*              instance.
*
* @return
*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpMstPortConfigGroup_StpMstRowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStpMstRowStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: StpMstRowStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objStpMstRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objStpMstRowStatusValue, owa.len);

  /* retrieve key: MstId */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstPortConfigGroup_MstId,
                          (xLibU8_t *) & keyMstIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_MST_ID_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa.len);

  /* call the usmdb only for add and delete */
  if (objStpMstRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* Create a row */
    if (usmDbDot1sInstanceGet(L7_UNIT_CURRENT, keyMstIdValue) != L7_SUCCESS)
    {
     owa.l7rc = usmDbDot1sMstiCreate (L7_UNIT_CURRENT,keyMstIdValue);
      /* As the MST instance creation is a Event
       * delay is added to this to wait for 2 seconds
       * so that the dependent objects will get the appropriate key
       * for configuration. This is useful when both MST instance
       * and its dependent paramenters are configured in a single shot
       */
       if(usmDbdot1sInstCheckInUse(L7_UNIT_CURRENT,keyMstIdValue) != L7_SUCCESS)
      {
        osapiSleep(2);
      }
    }
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_MST_INSTANCE_CREATE_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objStpMstRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    if (usmDbDot1sInstanceGet(L7_UNIT_CURRENT, keyMstIdValue) == L7_SUCCESS)
    {
      owa.l7rc = usmDbDot1sMstiDelete (L7_UNIT_CURRENT,
                                       keyMstIdValue);
    }
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_MST_INSTANCE_DEL_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}

