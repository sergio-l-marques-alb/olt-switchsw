/*******************************************************************************

*

* (C) Copyright Broadcom Corporation 2000-2007

*

********************************************************************************

*

* @filename fpobj_SwitchingStpMstConfig.c

*

* @purpose

*

* @component object handlers

*

* @comments  Refer to MST-object.xml

*

* @create  19 February 2008

*

* @author  Radha K

* @end

*

********************************************************************************/

#include "fpobj_util.h"

#include "_xe_SwitchingStpMstConfig_obj.h"

#include "usmdb_dot1s_api.h"

#include "usmdb_common.h"
#include "usmdb_mib_vlan_api.h"


extern void fpObj_StpDot1sBridgeId(xLibU8_t *buf, xLibU8_t *temp, xLibU32_t val);

extern void fpObj_StpDot1sPortId(xLibU8_t * buf, xLibU32_t val);



extern L7_RC_t usmDbNextVlanGet(L7_uint32 UnitIndex, L7_uint32 vid, L7_uint32 *nextvid);

/******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_MstId

*

* @purpose Get 'MstId'

*

* @description [MstId]: The MSTP instance ID 

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_MstId (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objMstIdValue;

  xLibU32_t nextObjMstIdValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: MstId */

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & objMstIdValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    owa.l7rc = usmDbDot1sInstanceFirstGet (L7_UNIT_CURRENT, &nextObjMstIdValue);   
    
  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objMstIdValue, owa.len);

    owa.l7rc = usmDbDot1sInstanceNextGet (L7_UNIT_CURRENT, objMstIdValue,

                                          &nextObjMstIdValue);

  }

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjMstIdValue, owa.len);



  /* return the object value: MstId */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjMstIdValue,

                           sizeof (objMstIdValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}

/*******************************************************************************
* @function fpObjList_SwitchingStpMstConfig_MstId
*
* @purpose Get 'MstId'
*
* @description [MstId]: MST Instance
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingStpMstConfig_MstId(void *wap,void *bufp)
{
  return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_SwitchingStpMstConfig_MstId,
      L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX, 1);
}


/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_StpMstBridgePriority

*

* @purpose Get 'StpMstBridgePriority'

*

* @description [StpMstBridgePriority]: he MSTP bridge priority in a specific

*              instance. The priority is in the increments of 4096. The

*              recommended default value is 32768. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_StpMstBridgePriority (void *wap,

                                                              void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpMstBridgePriorityValue;

  xLibU16_t priority;

  FPOBJ_TRACE_ENTER (bufp);



  memset(&objStpMstBridgePriorityValue, 0, sizeof(objStpMstBridgePriorityValue));



  /* retrieve key: MstId */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & keyMstIdValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiBridgePriorityGet (L7_UNIT_CURRENT, keyMstIdValue,

                                              &priority);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  objStpMstBridgePriorityValue = priority;



  /* return the object value: StpMstBridgePriority */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpMstBridgePriorityValue,

                           sizeof (objStpMstBridgePriorityValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingStpMstConfig_StpMstBridgePriority

*

* @purpose Set 'StpMstBridgePriority'

*

* @description [StpMstBridgePriority]: he MSTP bridge priority in a specific

*              instance. The priority is in the increments of 4096. The

*              recommended default value is 32768. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpMstConfig_StpMstBridgePriority (void *wap,

                                                              void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpMstBridgePriorityValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* Declare a temp variable to store the prioriry value*/
  xLibU32_t tempStpMstBridgePriority;


  /* retrieve object: StpMstBridgePriority */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpMstBridgePriorityValue,

                           &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpMstBridgePriorityValue, owa.len);

  tempStpMstBridgePriority = objStpMstBridgePriorityValue;

  /* retrieve key: MstId */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & keyMstIdValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa.len);



  /* set the value in application */

  owa.l7rc = usmDbDot1sMstiBridgePrioritySet (L7_UNIT_CURRENT, keyMstIdValue,

                                              &objStpMstBridgePriorityValue);

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
  if(tempStpMstBridgePriority != objStpMstBridgePriorityValue)
  {
    owa.rc = XLIBRC_PRIORITY_SET;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_StpMstBridgeIdentifier

*

* @purpose Get 'StpMstBridgeIdentifier'

*

* @description [StpMstBridgeIdentifier]: The MSTP bridge identifier in a specific

*              instance. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_StpMstBridgeIdentifier (void *wap,

                                                                void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objStpMstBridgeIdentifierValue={0};

  xLibStr256_t bridgeId={0};

  xLibU32_t length;

  FPOBJ_TRACE_ENTER (bufp);



  memset(objStpMstBridgeIdentifierValue, 0x0, sizeof(objStpMstBridgeIdentifierValue)); 

  /* retrieve key: MstId */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & keyMstIdValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiBridgeIdentifierGet (L7_UNIT_CURRENT, keyMstIdValue,

                                                bridgeId,

                                                &length);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  fpObj_StpDot1sBridgeId(objStpMstBridgeIdentifierValue, bridgeId, length);



  /* return the object value: StpMstBridgeIdentifier */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpMstBridgeIdentifierValue,

                           strlen (objStpMstBridgeIdentifierValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_StpMstDesignatedRootId

*

* @purpose Get 'StpMstDesignatedRootId'

*

* @description [StpMstDesignatedRootId]: The MSTP designated root bridge identifier

*              in a specific instance. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_StpMstDesignatedRootId (void *wap,

                                                                void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objStpMstDesignatedRootIdValue={0};

  xLibStr256_t rootId={0};

  xLibU32_t length;

  FPOBJ_TRACE_ENTER (bufp);

  memset(objStpMstDesignatedRootIdValue, 0x0, sizeof(objStpMstDesignatedRootIdValue)); 



  /* retrieve key: MstId */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & keyMstIdValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiDesignatedRootIDGet (L7_UNIT_CURRENT, keyMstIdValue,

                                                rootId,

                                                &length);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  fpObj_StpDot1sBridgeId(objStpMstDesignatedRootIdValue, rootId, length);



  /* return the object value: StpMstDesignatedRootId */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpMstDesignatedRootIdValue,

                           strlen (objStpMstDesignatedRootIdValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_StpMstRootPathCost

*

* @purpose Get 'StpMstRootPathCost'

*

* @description [StpMstRootPathCost]: The MSTP root path cost in a specific

*              instance. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_StpMstRootPathCost (void *wap,

                                                            void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpMstRootPathCostValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: MstId */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & keyMstIdValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiRootPathCostGet (L7_UNIT_CURRENT, keyMstIdValue,

                                            &objStpMstRootPathCostValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpMstRootPathCost */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpMstRootPathCostValue,

                           sizeof (objStpMstRootPathCostValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_StpMstRootPortId

*

* @purpose Get 'StpMstRootPortId'

*

* @description [StpMstRootPortId]: The MSTP root port ID in a specific instance.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_StpMstRootPortId (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objStpMstRootPortIdValue;

  xLibU32_t portId=0;

  FPOBJ_TRACE_ENTER (bufp);



  memset(objStpMstRootPortIdValue, 0x0, sizeof(objStpMstRootPortIdValue)); 



  /* retrieve key: MstId */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & keyMstIdValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiRootPortIDGet (L7_UNIT_CURRENT, keyMstIdValue,

                                          &portId);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  fpObj_StpDot1sPortId(objStpMstRootPortIdValue, portId);



  /* return the object value: StpMstRootPortId */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpMstRootPortIdValue,

                           strlen (objStpMstRootPortIdValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_StpMstTimeSinceTopologyChange

*

* @purpose Get 'StpMstTimeSinceTopologyChange'

*

* @description [StpMstTimeSinceTopologyChange]: The MSTP time since the last

*              topology change in a specific instance. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_StpMstTimeSinceTopologyChange (void

                                                                       *wap,

                                                                       void

                                                                       *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  usmDbTimeSpec_t stpMstTimeSinceTopologyChangeValue;

  FPOBJ_TRACE_ENTER (bufp);

  xLibStr256_t  objStpMstTimeSinceTopologyChangeValue={0};



  /* retrieve key: MstId */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & keyMstIdValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa.len);



  memset(&stpMstTimeSinceTopologyChangeValue, 0, sizeof(usmDbTimeSpec_t));



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sMstiTimeSinceTopologyChangeGet (L7_UNIT_CURRENT, keyMstIdValue,

                                              &stpMstTimeSinceTopologyChangeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  memset(objStpMstTimeSinceTopologyChangeValue,0x0,sizeof(objStpMstTimeSinceTopologyChangeValue));

  osapiSnprintf(objStpMstTimeSinceTopologyChangeValue,sizeof(objStpMstTimeSinceTopologyChangeValue), 

                           "%d day %d hr %d min %d sec",stpMstTimeSinceTopologyChangeValue.days,

                            stpMstTimeSinceTopologyChangeValue.hours,stpMstTimeSinceTopologyChangeValue.minutes,

                            stpMstTimeSinceTopologyChangeValue.seconds);                                                        



  /* return the object value: StpMstTimeSinceTopologyChange */

  owa.rc =

    xLibBufDataSet (bufp, (xLibU8_t *) objStpMstTimeSinceTopologyChangeValue,

                    strlen (objStpMstTimeSinceTopologyChangeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_StpMstTopologyChangeCount

*

* @purpose Get 'StpMstTopologyChangeCount'

*

* @description [StpMstTopologyChangeCount]: The MSTP count of topology changes

*              in a specific instance. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_StpMstTopologyChangeCount (void *wap,

                                                                   void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpMstTopologyChangeCountValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: MstId */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & keyMstIdValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa.len);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sMstiTopologyChangeCountGet (L7_UNIT_CURRENT, keyMstIdValue,

                                          &objStpMstTopologyChangeCountValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpMstTopologyChangeCount */

  owa.rc =

    xLibBufDataSet (bufp, (xLibU8_t *) & objStpMstTopologyChangeCountValue,

                    sizeof (objStpMstTopologyChangeCountValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_StpMstTopologyChangeParm

*

* @purpose Get 'StpMstTopologyChangeParm'

*

* @description [StpMstTopologyChangeParm]: The MSTP topology change parameter

*              in a specific instance 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_StpMstTopologyChangeParm (void *wap,

                                                                  void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpMstTopologyChangeParmValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: MstId */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & keyMstIdValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa.len);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sMstiTopologyChangeParmGet (L7_UNIT_CURRENT, keyMstIdValue,

                                         &objStpMstTopologyChangeParmValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpMstTopologyChangeParm */

  owa.rc =

    xLibBufDataSet (bufp, (xLibU8_t *) & objStpMstTopologyChangeParmValue,

                    sizeof (objStpMstTopologyChangeParmValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_StpMstRowStatus

*

* @purpose Get 'StpMstRowStatus'

*

* @description [StpMstRowStatus]: The MSTP instance status. Supported values:

*              Add - used to create a new instance.Remove - removes an

*              instance. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_StpMstRowStatus (void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpMstRowStatusValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: MstId */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & keyMstIdValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

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

* @function fpObjSet_SwitchingStpMstConfig_StpMstRowStatus

*

* @purpose Set 'StpMstRowStatus'

*

* @description [StpMstRowStatus]: The MSTP instance status. Supported values:

*              Add - used to create a new instance.Remove - removes an

*              instance. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpMstConfig_StpMstRowStatus (void *wap, void *bufp)

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

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

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





/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_Dot1sMstiFID

*

* @purpose Get 'Dot1sMstiFID'

*

* @description [Dot1sMstiFID]: TODO 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_Dot1sMstiFID(void *wap, void *bufp) 
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot1sMstiFIDValue;
  xLibU32_t nextObjDot1sMstiFIDValue;
  xLibU32_t objDot1sMstiVIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,
                          (xLibU8_t *) & keyMstIdValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa.len);

  owa.rc = xLibFilterGet(wap, XOBJ_SwitchingStpMstConfig_Dot1sMstiVID,
                                      (xLibU8_t *) &objDot1sMstiVIDValue, &owa.len);
   if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  owa.rc = xLibFilterGet(wap, XOBJ_SwitchingStpMstConfig_Dot1sMstiFID,
                                      (xLibU8_t *) &objDot1sMstiFIDValue, &owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY(bufp, NULL, 0);
    nextObjDot1sMstiFIDValue = objDot1sMstiVIDValue; 
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY(bufp, &objDot1sMstiFIDValue, owa.len);
    owa.l7rc = L7_FAILURE;
  }

  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY(bufp, &nextObjDot1sMstiFIDValue, owa.len);

  /* return the object value: objDot1sMstiFIDValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjDot1sMstiFIDValue,
                           sizeof (nextObjDot1sMstiFIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_Dot1sMstiVID

*

* @purpose Get 'Dot1sMstiVID'

*

* @description [Dot1sMstiVID]: TODO 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_Dot1sMstiVID(void *wap, void *bufp) 

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objDot1sMstiVIDValue;

  xLibU32_t nextObjDot1sMstiVIDValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: MstId */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & keyMstIdValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa.len);



  owa.rc = xLibFilterGet(wap, XOBJ_SwitchingStpMstConfig_Dot1sMstiVID,

                                      (xLibU8_t *) &objDot1sMstiVIDValue, &owa.len);



  if(owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY(bufp, NULL, 0);

    owa.l7rc = usmDbDot1sMstiVIDFirstGet (L7_UNIT_CURRENT, keyMstIdValue, &nextObjDot1sMstiVIDValue);

  }

  else

  {

    FPOBJ_TRACE_CURRENT_KEY(bufp, &objDot1sMstiVIDValue, owa.len);
    do
    {
    owa.l7rc = usmDbDot1sMstiVIDNextGet (L7_UNIT_CURRENT, keyMstIdValue, 

                                                                    objDot1sMstiVIDValue, &nextObjDot1sMstiVIDValue);

    }while ((objDot1sMstiVIDValue == nextObjDot1sMstiVIDValue) && (owa.l7rc == L7_SUCCESS));
  }



  if(owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT(bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_NEW_KEY(bufp, &nextObjDot1sMstiVIDValue, owa.len);



  /* return the object value: objDot1sMstiVIDValue */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjDot1sMstiVIDValue,

                           sizeof (nextObjDot1sMstiVIDValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}

/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_Dot1sMstiRegionalRootID

*

* @purpose Get 'Dot1sMstiRegionalRootID'

*

* @description [Dot1sMstiRegionalRootID]: TODO 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_Dot1sMstiRegionalRootID(void *wap, void *bufp) 

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objDot1sMstiRegionalRootIDValue;

  xLibStr256_t rootId={0};

  xLibU32_t len;



  FPOBJ_TRACE_ENTER (bufp);

  

  /* retrieve key: MstId */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & keyMstIdValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiRegionalRootIDGet (L7_UNIT_CURRENT, keyMstIdValue,

                                              rootId,&len);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  fpObj_StpDot1sBridgeId(objDot1sMstiRegionalRootIDValue, rootId, len);



  /* return the object value: objDot1sMstiRegionalRootIDValue */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDot1sMstiRegionalRootIDValue,

                           strlen (objDot1sMstiRegionalRootIDValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}


/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_Dot1sMstiInstanceIsPresent

*

* @purpose Get 'Dot1sMstiInstanceIsPresent'

*

* @description [Dot1sMstiInstanceIsPresent] TODO 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_Dot1sMstiInstanceIsPresent(void *wap, void *bufp) 

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue;

  xLibU32_t objDot1sMstiInstanceIsPresent;


  FPOBJ_TRACE_ENTER (bufp);

  

  /* retrieve key: MstId */

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & keyMstIdValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = L7_SUCCESS;

  /* Always return L7_SUCCESS and fill the vlaue objDot1sMstiInstanceIsPresent */ 
  if(usmDbdot1sInstCheckInUse (L7_UNIT_CURRENT, keyMstIdValue) != L7_SUCCESS)
  {
     objDot1sMstiInstanceIsPresent = L7_FALSE;
  }
  else
  {
    objDot1sMstiInstanceIsPresent = L7_TRUE;
  }

   
  /* return the object value: objDot1sMstiInstanceIsPresent */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objDot1sMstiInstanceIsPresent,

                           sizeof (objDot1sMstiInstanceIsPresent));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_Dot1sMstiRootPathCost

*

* @purpose Get 'Dot1sMstiRootPathCost'

*

* @description [Dot1sMstiRootPathCost]: TODO 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_Dot1sMstiRootPathCost(void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objDot1sMstiRootPathCostValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: MstId */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & keyMstIdValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwa.len);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sMstiRootPathCostGet (L7_UNIT_CURRENT, keyMstIdValue,

                                         &objDot1sMstiRootPathCostValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: objDot1sMstiRootPathCostValue */

  owa.rc =

    xLibBufDataSet (bufp, (xLibU8_t *) & objDot1sMstiRootPathCostValue,

                    sizeof (objDot1sMstiRootPathCostValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}







/*******************************************************************************

* @function fpObjGet_RangeVLAN_ID

*

* @purpose Get 'ID'

 *@description  [ID] VLAN ID   

* @notes       

*

* @return

*******************************************************************************/

L7_RC_t fpObj_getVlanFromMask(L7_VLAN_MASK_t* vlanMask,L7_uint32 *vlanIndex)

{

  L7_uint32 i =0;

  if(vlanIndex == L7_NULL)
  {
    return L7_FAILURE;
  }

  for(i = 1; i<= L7_VLAN_MAX_MASK_BIT;i++)

  {

    if(L7_VLAN_ISMASKBITSET((*vlanMask),i))
    {
      *vlanIndex = i;
      return L7_SUCCESS;
    }
  } 

  return L7_FAILURE;

}



L7_RC_t fpObj_getNextVlan(L7_VLAN_MASK_t* vlanMask,L7_uint32 vid,L7_uint32 *nxtVlanIndex)

{

  L7_uint32 i = 0;

  for(i = vid+1;i <= L7_VLAN_MAX_MASK_BIT;i++ )

  {

    if(L7_VLAN_ISMASKBITSET((*vlanMask),i))
    {
        *nxtVlanIndex = i;
        return L7_SUCCESS;
    }
  }

  return L7_FAILURE;

}





xLibRC_t fpObjList_SwitchingStpMstConfig_StpMstVlanIndex (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));

  xLibU32_t objIDValue=0;

  xLibU32_t nextObjIDValue;

  L7_VLAN_MASK_t vlanMask;

  L7_VLAN_MASK_t returnMask;



  memset(&vlanMask,0x0,sizeof(vlanMask));

  memset(&returnMask,0x0,sizeof(returnMask));



  FPOBJ_TRACE_ENTER (bufp);



  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_StpMstVlanIndex, (xLibU8_t *)&vlanMask, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);



    owa.l7rc = usmDbNextVlanGet(L7_UNIT_CURRENT, objIDValue, &nextObjIDValue);

  }

  else

  {

    owa.l7rc = fpObj_getVlanFromMask(&vlanMask,&objIDValue);

    if(owa.l7rc != L7_SUCCESS)

    {

      owa.rc = L7_FAILURE;

      return XLIBRC_FAILURE;

    }



    owa.l7rc = usmDbNextVlanGet(L7_UNIT_CURRENT, objIDValue, &nextObjIDValue);

  }



  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  L7_VLAN_SETMASKBIT(returnMask,nextObjIDValue);

  FPOBJ_TRACE_NEW_KEY (bufp, &returnMask, owa.len);



  /* return the object value: ID */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & returnMask, sizeof (returnMask));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_StpMstVlanIndex

*

* @purpose Get 'VlanIndex'

 *@description  [VlanIndex] The VLAN-ID or other identifier refering to this VLAN   

* @notes       

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_StpMstVlanIndex (void *wap, void *bufp)

{



  xLibU32_t objMstIdValue;

  xLibU32_t objVlanIndexValue=0;

  xLibU32_t nextObjVlanIndexValue=0;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  FPOBJ_TRACE_ENTER (bufp);

  L7_VLAN_MASK_t returnMask;

  memset(&returnMask,0x0,sizeof(returnMask));





  /* retrieve key: MstId */

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                          (xLibU8_t *) & objMstIdValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    owa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &objMstIdValue, owa.len);



  do

  {

    owa.l7rc = usmDbDot1sMstiVIDNextGet(L7_UNIT_CURRENT,
                                    objMstIdValue,objVlanIndexValue, &nextObjVlanIndexValue);
    if(owa.l7rc == L7_SUCCESS)
    { 
      L7_VLAN_SETMASKBIT(returnMask, nextObjVlanIndexValue);  
    }
    else
    {
      break;
    }
    objVlanIndexValue = nextObjVlanIndexValue; 

  }while(owa.l7rc == L7_SUCCESS);    



  FPOBJ_TRACE_NEW_KEY (bufp, &returnMask, sizeof(returnMask));



  /* return the object value: vlanMask */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & returnMask, sizeof (returnMask));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



xLibRC_t fpObjSet_SwitchingStpMstConfig_StpMstVlanIndex(void *wap, void *bufp)

{

  return XLIBRC_SUCCESS;

}



/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_StpMstVlanRowStatus

*

* @purpose Get 'MstVlanRowStatus'

 *@description  [MstVlanRowStatus] The association status of an MSTP instance

* and a VLAN. Supported values: Add - used to create a new

* association between an MSTP instance and a VLAN. Delete- removes the

* association between an MSTP instance and a V   

* @notes       

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_StpMstVlanRowStatus (void *wap, void *bufp)

{



  fpObjWa_t kwaMstId = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue;

  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));

  L7_VLAN_MASK_t keyVlanMask;



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objMstVlanRowStatusValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: MstId */

  kwaMstId.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                               (xLibU8_t *) & keyMstIdValue, &kwaMstId.len);

  if (kwaMstId.rc != XLIBRC_SUCCESS)

  {

    kwaMstId.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaMstId);

    return kwaMstId.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwaMstId.len);



  /* retrieve key: VlanIndex */

  kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_StpMstVlanIndex,

                                   (xLibU8_t *) & keyVlanMask, &kwaVlanIndex.len);

  if (kwaVlanIndex.rc != XLIBRC_SUCCESS)

  {

    kwaVlanIndex.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaVlanIndex);

    return kwaVlanIndex.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanMask, kwaVlanIndex.len);



  /* get the value from application */

  objMstVlanRowStatusValue = L7_ROW_STATUS_ACTIVE;



  owa.l7rc = L7_SUCCESS;

            

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: MstVlanRowStatus */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMstVlanRowStatusValue,

                           sizeof (objMstVlanRowStatusValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}







/*******************************************************************************

* @function fpObjSet_SwitchingStpMstConfig_StpMstVlanRowStatus

*

* @purpose Set 'MstVlanRowStatus'

 *@description  [MstVlanRowStatus] The association status of an MSTP instance

* and a VLAN. Supported values: Add - used to create a new

* association between an MSTP instance and a VLAN. Delete- removes the

* association between an MSTP instance and a V   

* @notes       

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpMstConfig_StpMstVlanRowStatus (void *wap, void *bufp)

{



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objMstVlanRowStatusValue;

  fpObjWa_t kwaMstId = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyMstIdValue,tempMstIdValue,keyVlanIndex=0, prevKeyVlanIndex=0;

  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));

  L7_VLAN_MASK_t keyVlanMask;
  L7_BOOL flag = L7_FALSE;

  FPOBJ_TRACE_ENTER (bufp);

  memset(&keyVlanMask,0x0,sizeof(keyVlanMask));



  /* retrieve object: MstVlanRowStatus */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMstVlanRowStatusValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objMstVlanRowStatusValue, owa.len);



  /* retrieve key: MstId */

  kwaMstId.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,

                               (xLibU8_t *) & keyMstIdValue, &kwaMstId.len);

  if (kwaMstId.rc != XLIBRC_SUCCESS)

  {

    kwaMstId.rc = XLIBRC_MST_ID_FAILURE;

    FPOBJ_TRACE_EXIT (bufp, kwaMstId);

    return kwaMstId.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwaMstId.len);



  /* retrieve key: VlanIndex */

  kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_StpMstVlanIndex, 

                          (xLibU8_t *)&keyVlanMask, &kwaVlanIndex.len);

  if (kwaVlanIndex.rc != XLIBRC_SUCCESS)

  {

    memset(&keyVlanMask,0x0,sizeof(keyVlanMask));

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanMask, kwaVlanIndex.len);



  if (objMstVlanRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)

  {

        /* Remove the Existing VLANs which are not in new VLAN set mask */ 

    owa.l7rc = usmDbDot1sMstiVIDFirstGet(L7_UNIT_CURRENT, keyMstIdValue, 

                                         &keyVlanIndex);

    while(owa.l7rc == L7_SUCCESS)

    {

      if (!(L7_VLAN_ISMASKBITSET(keyVlanMask,keyVlanIndex)))

      {

        owa.l7rc = usmDbDot1sMstiVlanRemove(L7_UNIT_CURRENT, keyMstIdValue,

                                            keyVlanIndex);

      }

      prevKeyVlanIndex = keyVlanIndex;

      owa.l7rc = usmDbDot1sMstiVIDNextGet(L7_UNIT_CURRENT, keyMstIdValue, 

                                          prevKeyVlanIndex, &keyVlanIndex);

    }



    kwa.l7rc = fpObj_getVlanFromMask(&keyVlanMask,&keyVlanIndex);

    while(kwa.l7rc == L7_SUCCESS)

    {

      if (usmDbDot1sVlanToMstiGet(L7_UNIT_CURRENT, keyVlanIndex,

                                  &tempMstIdValue)!=L7_SUCCESS)

      {

          owa.rc = XLIBRC_MST_VLAN_ADD_FAILURE;    /* TODO: Change if required */

          FPOBJ_TRACE_EXIT (bufp, owa);

          return owa.rc;

      }

      if (tempMstIdValue != keyMstIdValue)

      {

        owa.l7rc = usmDbDot1sMstiVlanAdd(L7_UNIT_CURRENT, keyMstIdValue, keyVlanIndex);

        if (owa.l7rc != L7_SUCCESS)

        {

          owa.rc = XLIBRC_MST_VLAN_ADD_FAILURE;    /* TODO: Change if required */

          FPOBJ_TRACE_EXIT (bufp, owa);

          return owa.rc;

        }
        if (tempMstIdValue != DOT1S_CIST_ID)
        {
          flag = L7_TRUE;
        }

      }

     kwa.l7rc = fpObj_getNextVlan(&keyVlanMask,keyVlanIndex,&keyVlanIndex); 

    }
    if (flag == L7_TRUE)
    {
      owa.rc = XLIBRC_MST_VLAN_ADD_INFO;
    }
  }

  else if (objMstVlanRowStatusValue == L7_ROW_STATUS_DESTROY)

  {

    owa.l7rc = usmDbDot1sMstiVIDFirstGet(L7_UNIT_CURRENT, keyMstIdValue,

                                         &keyVlanIndex);

    while(owa.l7rc == L7_SUCCESS)

    {

      owa.l7rc = usmDbDot1sMstiVlanRemove(L7_UNIT_CURRENT, keyMstIdValue,

                                          keyVlanIndex);

       if (owa.l7rc != L7_SUCCESS)

       {

         owa.rc = XLIBRC_MST_VLAN_DEL_FAILURE;

         FPOBJ_TRACE_EXIT (bufp, owa);

         return owa.rc;

       }

       prevKeyVlanIndex = keyVlanIndex;

       owa.l7rc = usmDbDot1sMstiVIDNextGet(L7_UNIT_CURRENT, keyMstIdValue, 

                                           prevKeyVlanIndex, &keyVlanIndex);

    }

     /* delete Mst Instance */

    owa.l7rc = usmDbDot1sMstiDelete (L7_UNIT_CURRENT, keyMstIdValue);

    if (owa.l7rc != L7_SUCCESS)

    {

      owa.rc = XLIBRC_MST_INSTANCE_DEL_FAILURE; 

      FPOBJ_TRACE_EXIT(bufp, owa);

      return owa.rc;

    }

  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}

/******************************************************************
* @function fpObjGet_SwitchingStpMstConfig_VlanId

* @purpose Get 'VlanId'

 *@description  [VlanId] List of all the Vlans configured

* @notes

* @return
******************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstConfig_VlanId (void *wap, void *bufp)
{
  return fpObjAppGetKey_VlanInstance (wap, bufp, XOBJ_SwitchingStpMstConfig_VlanId);
}


/*******************************************************************
* @function SwitchingStpMstConfig_VlanParticipation

* @purpose Get 'VlanParticipation'

 *@description  [VlanParticipation] The association status of an MSTP instance to a particular VLAN

* @notes

* @return
********************************************************************/
xLibRC_t fpObjGet_SwitchingStpMstConfig_VlanParticipation (void *wap, void *bufp)
{
  L7_RC_t rv;
  xLibU32_t mstId;
  xLibU32_t vlanId;
  xLibU16_t len;
  xLibId_t oid;
  mstVlanInc_t val;

  oid = XOBJ_SwitchingStpMstConfig_MstId;
  len = sizeof (mstId);
  if (XLIBRC_SUCCESS != xLibFilterGet (wap, oid, (xLibU8_t *) & mstId, &len))
  {
    return XLIBRC_FILTER_MISSING;
  }

  oid = XOBJ_SwitchingStpMstConfig_VlanId;
  len = sizeof (vlanId);
  if (XLIBRC_SUCCESS != xLibFilterGet (wap, oid, (xLibU8_t *) & vlanId, &len))
  {
    return XLIBRC_FILTER_MISSING;
  }

  rv = usmDbDot1sMstiVIDGet (L7_USMDB_UNIT_ZERO, mstId, vlanId);
  val = (rv == L7_SUCCESS) ? L7_ENABLE  : L7_DISABLE;

  return xLibBufDataSet (bufp, (xLibU8_t *) & val, sizeof (val));
}
/************************************************************************
* @function SwitchingStpMstConfig_VlanParticipation

* @purpose Get 'VlanParticipation'

 *@description  [VlanParticipation] Associate VLAN to a particular MST instance

* @notes

* @return
*************************************************************************/
xLibRC_t fpObjSet_SwitchingStpMstConfig_VlanParticipation (void *wap, void *bufp)
{
  L7_RC_t rv;
  xLibU32_t mstId;
  xLibU32_t vlanId;
  xLibU16_t len;
  xLibId_t oid;
  L7_BOOL  val;
  oid = XOBJ_SwitchingStpMstConfig_MstId;
  len = sizeof (mstId);
  if (XLIBRC_SUCCESS != xLibFilterGet (wap, oid, (xLibU8_t *) & mstId, &len))
  {
    return XLIBRC_FILTER_MISSING;
  }

  oid = XOBJ_SwitchingStpMstConfig_VlanId;
  len = sizeof (vlanId);
  if (XLIBRC_SUCCESS != xLibFilterGet (wap, oid, (xLibU8_t *) & vlanId, &len))
  {
    return XLIBRC_FILTER_MISSING;
  }

  rv = xLibBufDataGet (bufp, (xLibU8_t *) & val, &len);
  if(rv != XLIBRC_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }
  if (val == L7_ENABLE)
  {
    rv = usmDbDot1sMstiVlanAdd (L7_USMDB_UNIT_ZERO, mstId, vlanId);
  }
  else
  {
    rv = usmDbDot1sMstiVlanRemove (L7_USMDB_UNIT_ZERO, mstId, vlanId);
  }

  return (rv == L7_SUCCESS) ? XLIBRC_SUCCESS : XLIBRC_FAILURE;
}
/******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_MstIDPreConfig

*

* @purpose Get 'MstId'

*

* @description [MstId]: The MSTP instance ID 

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_MstIDPreConfig (void *wap, void *bufp)

{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMstIdValue;
  xLibU32_t nextObjMstIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MstId */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstIDPreConfig,
                          (xLibU8_t *) & objMstIdValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbDot1sInstanceFirstGet (L7_UNIT_CURRENT, &objMstIdValue);   
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    /* Skip out CST becuase some flavours of STP config will be show as per-configured MST Instaces
     * so dont try to  display only CST
     */
    owa.l7rc = usmDbDot1sInstanceNextGet (L7_UNIT_CURRENT, objMstIdValue,
                                          &nextObjMstIdValue);
    
    if ((owa.l7rc != L7_SUCCESS) &&(nextObjMstIdValue == L7_NULL))
    {
       /* Force the handler to return 1 insted of END_OF_TABLE */
       nextObjMstIdValue = L7_DOT1S_MSTID_MIN;
       owa.rc = XLIBRC_SUCCESS;
       owa.l7rc = L7_SUCCESS;
    }
  }
  else

  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objMstIdValue, owa.len);
    owa.l7rc = usmDbDot1sInstanceNextGet (L7_UNIT_CURRENT, objMstIdValue,
                                          &nextObjMstIdValue);
  }
  if (owa.l7rc != L7_SUCCESS)

  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjMstIdValue, owa.len);

  /* return the object value: MstId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjMstIdValue,
                           sizeof (objMstIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_SwitchingStpMstConfig_MstIDPreConfig
*
* @purpose Get 'MstId'
*
* @description [MstId]: MST Instance
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_SwitchingStpMstConfig_MstIDPreConfig(void *wap,void *bufp)
{
  return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_SwitchingStpMstConfig_MstIDPreConfig,
      L7_DOT1S_MSTID_MIN, L7_DOT1S_MSTID_MAX, 1);
}
/*******************************************************************************

* @function fpObjGet_SwitchingStpMstConfig_StpMstPreConfigVlanRowStatus

*

* @purpose Set 'MstVlanRowStatus'

 *@description  [MstVlanRowStatus] The association status of an MSTP instance

* and a VLAN. Supported values: Add - used to create a new

* association between an MSTP instance and a VLAN. Delete- removes the

* association between an MSTP instance and a V   

* @notes       

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpMstConfig_StpMstPreConfigVlanRowStatus (void *wap, void *bufp)
{
  return XLIBRC_SUCCESS;
}
/*******************************************************************************

* @function fpObjSet_SwitchingStpMstConfig_StpMstPreConfigVlanRowStatus

*

* @purpose Set 'MstVlanRowStatus'

 *@description  [MstVlanRowStatus] The association status of an MSTP instance

* and a VLAN. Supported values: Add - used to create a new

* association between an MSTP instance and a VLAN. Delete- removes the

* association between an MSTP instance and a V   

* @notes       

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpMstConfig_StpMstPreConfigVlanRowStatus (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMstVlanRowStatusValue;
  fpObjWa_t kwaMstId = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyMstIdValue,tempMstIdValue,keyVlanIndex=0, prevKeyVlanIndex=0;
  fpObjWa_t kwaVlanIndex = FPOBJ_INIT_WA (sizeof (L7_VLAN_MASK_t));

  L7_VLAN_MASK_t keyVlanMask;

  FPOBJ_TRACE_ENTER (bufp);
  memset(&keyVlanMask,0x0,sizeof(keyVlanMask));

  /* retrieve object: MstVlanRowStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMstVlanRowStatusValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMstVlanRowStatusValue, owa.len);

  /* retrieve key: MstId */

  kwaMstId.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_MstId,
                               (xLibU8_t *) & keyMstIdValue, &kwaMstId.len);

  if (kwaMstId.rc != XLIBRC_SUCCESS)
  {
    kwaMstId.rc = XLIBRC_MST_ID_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, kwaMstId);
    return kwaMstId.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyMstIdValue, kwaMstId.len);

    if (objMstVlanRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    /* retrieve key: VlanIndex */

    kwaVlanIndex.rc = xLibFilterGet (wap, XOBJ_SwitchingStpMstConfig_StpMstVlanIndex, 
                            (xLibU8_t *)&keyVlanMask, &kwaVlanIndex.len);

    if (kwaVlanIndex.rc != XLIBRC_SUCCESS)
    {
      kwaVlanIndex.rc = XLIBRC_VLANID_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, kwaVlanIndex);
      return kwaVlanIndex.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyVlanMask, kwaVlanIndex.len);

    /* Remove the Existing VLANs which are not in new VLAN set mask */ 
    owa.l7rc = usmDbDot1sMstiVIDFirstGet(L7_UNIT_CURRENT, keyMstIdValue, 
                                         &keyVlanIndex);
    while(owa.l7rc == L7_SUCCESS)
    {
      if (!(L7_VLAN_ISMASKBITSET(keyVlanMask,keyVlanIndex)))

      {
        owa.l7rc = usmDbDot1sMstiVlanRemove(L7_UNIT_CURRENT, keyMstIdValue,
                                            keyVlanIndex);
      }
      prevKeyVlanIndex = keyVlanIndex;
      owa.l7rc = usmDbDot1sMstiVIDNextGet(L7_UNIT_CURRENT, keyMstIdValue, 
                                          prevKeyVlanIndex, &keyVlanIndex);
    }
    kwa.l7rc = fpObj_getVlanFromMask(&keyVlanMask,&keyVlanIndex);
    while(kwa.l7rc == L7_SUCCESS)
    {
      if (usmDbDot1sVlanToMstiGet(L7_UNIT_CURRENT, keyVlanIndex,
                                  &tempMstIdValue)!=L7_SUCCESS)
      {
          owa.rc = XLIBRC_MST_VLAN_ADD_FAILURE;    /* TODO: Change if required */
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
      }
      if (tempMstIdValue != keyMstIdValue)
      {
        owa.l7rc = usmDbDot1sMstiVlanAdd(L7_UNIT_CURRENT, keyMstIdValue, keyVlanIndex);
        if (owa.l7rc != L7_SUCCESS)
        {
          owa.rc = XLIBRC_MST_VLAN_ADD_FAILURE;    /* TODO: Change if required */
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }
      }
     kwa.l7rc = fpObj_getNextVlan(&keyVlanMask,keyVlanIndex,&keyVlanIndex); 
    }
  }

  else if (objMstVlanRowStatusValue == L7_ROW_STATUS_DESTROY)

  {
    owa.l7rc = usmDbDot1sMstiVIDFirstGet(L7_UNIT_CURRENT, keyMstIdValue,
                                         &keyVlanIndex);
    while(owa.l7rc == L7_SUCCESS)
    {
      owa.l7rc = usmDbDot1sMstiVlanRemove(L7_UNIT_CURRENT, keyMstIdValue,
                                          keyVlanIndex);
       if (owa.l7rc != L7_SUCCESS)
       {
         owa.rc = XLIBRC_MST_VLAN_DEL_FAILURE;
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
       }
       prevKeyVlanIndex = keyVlanIndex;
       owa.l7rc = usmDbDot1sMstiVIDNextGet(L7_UNIT_CURRENT, keyMstIdValue, 
                                           prevKeyVlanIndex, &keyVlanIndex);
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

