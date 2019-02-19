/*******************************************************************************

*

* (C) Copyright Broadcom Corporation 2000-2007

*

********************************************************************************

*

* @filename fpobj_SwitchingstpCstPortConfigGroup.c

*

* @purpose

*

* @component object handlers

*

* @comments  Refer to MST-object.xml

*

* @create  12 February 2008

*

* @author Radha K 

* @end

*

********************************************************************************/

#include "fpobj_util.h"

#include "_xe_SwitchingstpCstPortConfigGroup_obj.h"

#include "usmdb_dot1s_api.h"

#include "dot1s_exports.h"
#include "usmdb_util_api.h"

#include "usmdb_nim_api.h"



extern void fpObj_StpDot1sBridgeId(xLibU8_t *buf, xLibU8_t *temp, xLibU32_t val);

extern void fpObj_StpDot1sPortId(xLibU8_t * buf, xLibU32_t val);

extern xLibU32_t fpObj_StpPortForwardingStringGet(xLibU32_t val, xLibS8_t *forwardingState);



L7_RC_t

fpObjUtil_SwitchingstpCstPortConfigGroup_Interface_NextGet(L7_uint32 cstPortInput, L7_uint32 *cstPort)

{

  L7_uint32 cistId = DOT1S_CIST_ID;

  L7_uint32 intIfNum = 0;

  L7_uint32 extIfNum = 0;

  L7_INTF_TYPES_t itype;



  L7_uint32 nextExtIfNum = 0;

  L7_uint32 prevExtIfNum = 0;



  *cstPort =0;





  if(cstPortInput == 0)

  {

    /* Get the first valid port interface*/

    intIfNum = 0;

    extIfNum = 0;



    while (usmDbGetNextVisibleExtIfNumber(prevExtIfNum , &nextExtIfNum) == L7_SUCCESS)

    {

      prevExtIfNum  = nextExtIfNum;

      if (usmDbIntIfNumFromExtIfNum(intIfNum, &nextExtIfNum) == L7_SUCCESS)

      {

        (void)usmDbIntfTypeGet(intIfNum, &itype);

        if ((itype == L7_LAG_INTF) || (itype == L7_PHYSICAL_INTF))

        {

          *cstPort = nextExtIfNum;

          return L7_SUCCESS;

        }

      }

    }

    return L7_FAILURE;

  }  



  while (usmDbDot1sPortNextGet(L7_UNIT_CURRENT, cistId, intIfNum, &intIfNum) == L7_SUCCESS)

  {

    /* convert internal interface number to external interface number */

    if (usmDbExtIfNumFromIntIfNum(intIfNum, &extIfNum) == L7_SUCCESS)

    {

      if (extIfNum > cstPortInput)

      {

        /* check to see if this is a visible external interface number */

        if (usmDbVisibleExtIfNumberCheck(L7_UNIT_CURRENT, extIfNum) == L7_SUCCESS)

        {

          *cstPort = extIfNum;

          return L7_SUCCESS;

        }

      }

    }

  }



  return L7_FAILURE;

}



/*******************************************************************************

* @function fpObjGet_SwitchingstpCstPortConfigGroup_Interface

*

* @purpose Get 'Interface'

*

* @description [Interface]: Interfaces on which STP parameters are to be configured

*              

*

* @note  This is KEY Object

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_Interface (void *wap,

                                                            void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objInterfaceValue;

  xLibU32_t nextObjInterfaceValue;

  L7_uint32 cistId = DOT1S_CIST_ID;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & objInterfaceValue, &owa.len);



  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    objInterfaceValue = 0;

    owa.l7rc = usmDbDot1sPortNextGet(L7_UNIT_CURRENT, cistId, objInterfaceValue, 

                                    &nextObjInterfaceValue);

  }

  else

  {

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);

    owa.l7rc = usmDbDot1sPortNextGet(L7_UNIT_CURRENT, cistId, objInterfaceValue, 

                                    &nextObjInterfaceValue);

  }

  

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_ENDOF_TABLE;

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);



  /* return the object value: Interface */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue,

                           sizeof (objInterfaceValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortOperEdge

*

* @purpose Get 'StpCstPortOperEdge'

*

* @description [StpCstPortOperEdge]: The MSTP operational status of a specific

*              port for the CIST 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortOperEdge (void *wap,

                                                                     void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortOperEdgeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  

  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc = usmDbDot1sCistPortOperEdgeGet (L7_UNIT_CURRENT,tempKeyInterfaceValue,

                                            &objStpCstPortOperEdgeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstPortOperEdge */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortOperEdgeValue,

                           sizeof (objStpCstPortOperEdgeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortOperPointToPoint

*

* @purpose Get 'StpCstPortOperPointToPoint'

*

* @description [StpCstPortOperPointToPoint]: The MSTP operational point to

*              point mac of a specific port for the CIST. 

*

* @return

*******************************************************************************/

xLibRC_t

fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortOperPointToPoint (void *wap,

                                                                    void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  xLibU32_t sysIntfType;

  xLibU32_t sysIntfState;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortOperPointToPointValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  

  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* get the value from application */

  owa.l7rc = L7_FAILURE;

  if ((tempKeyInterfaceValue > 0) && (usmDbIntfTypeGet(tempKeyInterfaceValue, &sysIntfType) == L7_SUCCESS))
  {
    if ((sysIntfType == L7_LAG_INTF) && (usmDbIfOperStatusGet(L7_UNIT_CURRENT, tempKeyInterfaceValue, &sysIntfState) == L7_SUCCESS))
    {
      if (sysIntfState == L7_DOWN)
      {
        owa.l7rc = L7_SUCCESS;
        objStpCstPortOperPointToPointValue = L7_FALSE;
      }
      else
      {
        owa.l7rc =  usmDbDot1sCistPortOperPointToPointGet (L7_UNIT_CURRENT, tempKeyInterfaceValue, &objStpCstPortOperPointToPointValue);
      }
    }
    else
    {
      owa.l7rc =  usmDbDot1sCistPortOperPointToPointGet (L7_UNIT_CURRENT, tempKeyInterfaceValue, &objStpCstPortOperPointToPointValue);
    }
  }
  

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstPortOperPointToPoint */

  owa.rc =

    xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortOperPointToPointValue,

                    sizeof (objStpCstPortOperPointToPointValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortTopologyChangeAck

*

* @purpose Get 'StpCstPortTopologyChangeAck'

*

* @description [StpCstPortTopologyChangeAck]: The MSTP topology change acknowledge

*              for a specific port in the CIST 

*

* @return

*******************************************************************************/

xLibRC_t

fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortTopologyChangeAck (void *wap,

                                                                     void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortTopologyChangeAckValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  

  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc =

    usmDbDot1sCistPortTopologyChangeAckGet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                            &objStpCstPortTopologyChangeAckValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstPortTopologyChangeAck */

  owa.rc =

    xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortTopologyChangeAckValue,

                    sizeof (objStpCstPortTopologyChangeAckValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortEdge

*

* @purpose Get 'StpCstPortEdge'

*

* @description [StpCstPortEdge]: The administrative state of a specific port

*              in CIST. enable(1) - enables the port. disable(2) - disables

*              the port. The default port state is disabled. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortEdge (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortEdgeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc = usmDbDot1sCistPortEdgeGet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                        &objStpCstPortEdgeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstPortEdge */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortEdgeValue,

                           sizeof (objStpCstPortEdgeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortEdge

*

* @purpose Set 'StpCstPortEdge'

*

* @description [StpCstPortEdge]: The administrative state of a specific port

*              in CIST. enable(1) - enables the port. disable(2) - disables

*              the port. The default port state is disabled. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortEdge (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortEdgeValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpCstPortEdge */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpCstPortEdgeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstPortEdgeValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* set the value in application */

  owa.l7rc = usmDbDot1sCistPortEdgeSet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                        objStpCstPortEdgeValue);

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

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortForwardingState

*

* @purpose Get 'StpCstPortForwardingState'

*

* @description [StpCstPortForwardingState]: The MSTP forwarding state of a

*              specific port in CIST. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortForwardingState (void

                                                                            *wap,

                                                                            void

                                                                            *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibStr256_t objStpCstPortForwardingStateValue;

  xLibU32_t val;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc =

    usmDbDot1sMstiPortForwardingStateGet (L7_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE,

                                          tempKeyInterfaceValue,

                                          &val);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  if (fpObj_StpPortForwardingStringGet(val, objStpCstPortForwardingStateValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstPortForwardingState */

  owa.rc =

    xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortForwardingStateValue,

                    sizeof (objStpCstPortForwardingStateValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortId

*

* @purpose Get 'StpCstPortId'

*

* @description [StpCstPortId]: The MSTP port identifier of a specific port

*              in CIST. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortId (void *wap,

                                                               void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objStpCstPortIdValue;

  xLibU32_t portId;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  

  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiPortIDGet (L7_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE,

                                      tempKeyInterfaceValue,

                                      &portId);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  fpObj_StpDot1sPortId(objStpCstPortIdValue, portId);



  /* return the object value: StpCstPortId */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpCstPortIdValue,

                           strlen (objStpCstPortIdValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortPathCost

*

* @purpose Get 'StpCstPortPathCost'

*

* @description [StpCstPortPathCost]: The MSTP port path cost in CIST. The

*              default value will correspond to the recommendation specified

*              in IEEE 802.1s Table 13-2 which varies depending upon link

*              speed. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortPathCost (void *wap,

                                                                     void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortPathCostValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  

  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiPortPathCostGet (L7_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE,

                                            tempKeyInterfaceValue,

                                            &objStpCstPortPathCostValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstPortPathCost */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortPathCostValue,

                           sizeof (objStpCstPortPathCostValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortPathCost

*

* @purpose Set 'StpCstPortPathCost'

*

* @description [StpCstPortPathCost]: The MSTP port path cost in CIST. The

*              default value will correspond to the recommendation specified

*              in IEEE 802.1s Table 13-2 which varies depending upon link

*              speed. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortPathCost (void *wap,

                                                                     void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortPathCostValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpCstPortPathCost */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpCstPortPathCostValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstPortPathCostValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  

  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* set the value in application */

  owa.l7rc = usmDbDot1sMstiPortPathCostSet (L7_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE,

                                            tempKeyInterfaceValue,

                                            objStpCstPortPathCostValue);

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

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortPriority

*

* @purpose Get 'StpCstPortPriority'

*

* @description [StpCstPortPriority]: The MSTP port priority in CIST. The priority

*              is in the increments of 16. The default value is 128.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortPriority (void *wap,

                                                                     void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortPriorityValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiPortPriorityGet (L7_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE,

                                            tempKeyInterfaceValue,

                                            &objStpCstPortPriorityValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstPortPriority */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortPriorityValue,

                           sizeof (objStpCstPortPriorityValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortPriority

*

* @purpose Set 'StpCstPortPriority'

*

* @description [StpCstPortPriority]: The MSTP port priority in CIST. The priority

*              is in the increments of 16. The default value is 128.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortPriority (void *wap,

                                                                     void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortPriorityValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);
  
  /* Declare a temp variable for priority */
  xLibU32_t tempPriorityValue;


  /* retrieve object: StpCstPortPriority */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpCstPortPriorityValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstPortPriorityValue, owa.len);

  /* Copy the priority value in temp */
  tempPriorityValue = objStpCstPortPriorityValue;

  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* set the value in application */

  owa.l7rc = usmDbDot1sMstiPortPrioritySet (L7_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE,

                                            tempKeyInterfaceValue,

                                            &objStpCstPortPriorityValue);

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
  if(tempPriorityValue != objStpCstPortPriorityValue)
  {
    owa.rc = XLIBRC_PRIORITY_SET;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstDesignatedBridgeId

*

* @purpose Get 'StpCstDesignatedBridgeId'

*

* @description [StpCstDesignatedBridgeId]: The MSTP designated bridge ID of

*              a specific port in CIST 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstDesignatedBridgeId (void

                                                                           *wap,

                                                                           void

                                                                           *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objStpCstDesignatedBridgeIdValue;

  xLibStr256_t bridgeId;

  xLibU32_t length;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc =

    usmDbDot1sMstiDesignatedBridgeIDGet (L7_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE,

                                         tempKeyInterfaceValue,

                                         bridgeId,

                                         &length);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  

  fpObj_StpDot1sBridgeId(objStpCstDesignatedBridgeIdValue, bridgeId, length);



  /* return the object value: StpCstDesignatedBridgeId */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpCstDesignatedBridgeIdValue,

                           strlen (objStpCstDesignatedBridgeIdValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstDesignatedCost

*

* @purpose Get 'StpCstDesignatedCost'

*

* @description [StpCstDesignatedCost]: The MSTP designated cost of a specific

*              port in CIST. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstDesignatedCost (void

                                                                       *wap,

                                                                       void

                                                                       *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstDesignatedCostValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc =

    usmDbDot1sMstiDesignatedCostGet (L7_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE,

                                     tempKeyInterfaceValue,

                                     &objStpCstDesignatedCostValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstDesignatedCost */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstDesignatedCostValue,

                           sizeof (objStpCstDesignatedCostValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstDesignatedPortId

*

* @purpose Get 'StpCstDesignatedPortId'

*

* @description [StpCstDesignatedPortId]: The MSTP designated port ID of a

*              specific port in CIST. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstDesignatedPortId (void

                                                                         *wap,

                                                                         void

                                                                         *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objStpCstDesignatedPortIdValue={0};

  xLibU16_t portId;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc =

    usmDbDot1sMstiDesignatedPortIDGet (L7_UNIT_CURRENT, L7_DOT1S_CIST_INSTANCE,

                                       tempKeyInterfaceValue,

                                       &portId);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  fpObj_StpDot1sPortId(objStpCstDesignatedPortIdValue, portId);



  /* return the object value: StpCstDesignatedPortId */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpCstDesignatedPortIdValue,

                           strlen (objStpCstDesignatedPortIdValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortHelloTimer

*

* @purpose Get 'StpCstExtPortHelloTimer'

*

* @description               

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortHelloTimer (void

                                                                        *wap,

                                                                        void

                                                                        *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortHelloTimerValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc =

    usmDbDot1sCistPortAdminHelloTimeGet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                           &objStpCstPortHelloTimerValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortHelloTimerValue,

                           sizeof (objStpCstPortHelloTimerValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstExtPortPathCost

*

* @purpose Get 'StpCstExtPortPathCost'

*

* @description [StpCstExtPortPathCost]: The MSTP external port path cost in

*              CIST. The default value varies depending upon the link speed.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstExtPortPathCost (void

                                                                        *wap,

                                                                        void

                                                                        *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstExtPortPathCostValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc =

    usmDbDot1sCistPortExternalPathCostGet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                           &objStpCstExtPortPathCostValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstExtPortPathCost */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstExtPortPathCostValue,

                           sizeof (objStpCstExtPortPathCostValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingstpCstPortConfigGroup_StpCstExtPortPathCost

*

* @purpose Set 'StpCstExtPortPathCost'

*

* @description [StpCstExtPortPathCost]: The MSTP external port path cost in

*              CIST. The default value varies depending upon the link speed.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingstpCstPortConfigGroup_StpCstExtPortPathCost (void

                                                                        *wap,

                                                                        void

                                                                        *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstExtPortPathCostValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpCstExtPortPathCost */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpCstExtPortPathCostValue,

                           &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstExtPortPathCostValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* set the value in application */

  owa.l7rc =

    usmDbDot1sCistPortExternalPathCostSet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                           objStpCstExtPortPathCostValue);

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

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortBpduGuardEffect

*

* @purpose Get 'StpCstPortBpduGuardEffect'

*

* @description [StpCstPortBpduGuardEffect]: BPDU Guard Mode effect on the

*              port. enable(1) - BPDU Guard Mode is enabled on the port. disable(2)

*              - BPDU Guard Mode is disabled on the port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortBpduGuardEffect (void

                                                                            *wap,

                                                                            void

                                                                            *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortBpduGuardEffectValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc = usmDbDot1sBpduGuardEffectGet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                           &objStpCstPortBpduGuardEffectValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstPortBpduGuardEffect */

  owa.rc =

    xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortBpduGuardEffectValue,

                    sizeof (objStpCstPortBpduGuardEffectValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortBpduFilter

*

* @purpose Get 'StpCstPortBpduFilter'

*

* @description [StpCstPortBpduFilter]:  sets BPDU Filter mode on the port.

*              enable(1) - BPDU Filter Mode is enabled on the port. disable(2)

*              - BPDU Filter Mode is disabled on the port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortBpduFilter (void

                                                                       *wap,

                                                                       void

                                                                       *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortBpduFilterValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc = usmDbDot1sIntfBpduFilterGet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                          &objStpCstPortBpduFilterValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstPortBpduFilter */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortBpduFilterValue,

                           sizeof (objStpCstPortBpduFilterValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortBpduFilter

*

* @purpose Set 'StpCstPortBpduFilter'

*

* @description [StpCstPortBpduFilter]:  sets BPDU Filter mode on the port.

*              enable(1) - BPDU Filter Mode is enabled on the port. disable(2)

*              - BPDU Filter Mode is disabled on the port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortBpduFilter (void

                                                                       *wap,

                                                                       void

                                                                       *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortBpduFilterValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpCstPortBpduFilter */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpCstPortBpduFilterValue,

                           &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstPortBpduFilterValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  

  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* set the value in application */

  owa.l7rc = usmDbDot1sIntfBpduFilterSet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                          objStpCstPortBpduFilterValue);

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

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortBpduFlood

*

* @purpose Get 'StpCstPortBpduFlood'

*

* @description [StpCstPortBpduFlood]: sets BPDU Flood mode on the port.enable(1)

*              - BPDU Flood Mode is enabled on the port.disable(2)

*              - BPDU Flood Mode is disabled on the port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortBpduFlood (void *wap,

                                                                      void

                                                                      *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortBpduFloodValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc = usmDbDot1sIntfBpduFloodGet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                         &objStpCstPortBpduFloodValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstPortBpduFlood */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortBpduFloodValue,

                           sizeof (objStpCstPortBpduFloodValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortBpduFlood

*

* @purpose Set 'StpCstPortBpduFlood'

*

* @description [StpCstPortBpduFlood]: sets BPDU Flood mode on the port.enable(1)

*              - BPDU Flood Mode is enabled on the port.disable(2)

*              - BPDU Flood Mode is disabled on the port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortBpduFlood (void *wap,

                                                                      void

                                                                      *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortBpduFloodValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpCstPortBpduFlood */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpCstPortBpduFloodValue,

                           &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstPortBpduFloodValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* set the value in application */

  owa.l7rc = usmDbDot1sIntfBpduFloodSet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                         objStpCstPortBpduFloodValue);

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

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortAutoEdge

*

* @purpose Get 'StpCstPortAutoEdge'

*

* @description [StpCstPortAutoEdge]: sets the auto-edge mode of the port which

*              enables it to become an edge port if it does not see BPDUs

*              for some duration. enable(1) - enables the auto-edge mode

*              for the port. disable(2) - disables the auto-edge mode

*              for the port. The default auto-edge mo 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortAutoEdge (void *wap,

                                                                     void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortAutoEdgeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc = usmDbDot1sPortAutoEdgeGet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                        &objStpCstPortAutoEdgeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstPortAutoEdge */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortAutoEdgeValue,

                           sizeof (objStpCstPortAutoEdgeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortAutoEdge

*

* @purpose Set 'StpCstPortAutoEdge'

*

* @description [StpCstPortAutoEdge]: sets the auto-edge mode of the port which

*              enables it to become an edge port if it does not see BPDUs

*              for some duration. enable(1) - enables the auto-edge mode

*              for the port. disable(2) - disables the auto-edge mode

*              for the port. The default auto-edge mo 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortAutoEdge (void *wap,

                                                                     void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortAutoEdgeValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpCstPortAutoEdge */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpCstPortAutoEdgeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstPortAutoEdgeValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* set the value in application */

  owa.l7rc = usmDbDot1sPortAutoEdgeSet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                        objStpCstPortAutoEdgeValue);

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

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortRootGuard

*

* @purpose Get 'StpCstPortRootGuard'

*

* @description [StpCstPortRootGuard]: sets a port to discard any superior

*              information received by the port and thus protect against root

*              of the device from changing.The port gets put into discarding

*              state and does not forward any packets. enable(1) -

*              enables the root-guard mode for the port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortRootGuard (void *wap,

                                                                      void

                                                                      *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortRootGuardValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc =

    usmDbDot1sPortRestrictedRoleGet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                     &objStpCstPortRootGuardValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstPortRootGuard */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortRootGuardValue,

                           sizeof (objStpCstPortRootGuardValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortRootGuard

*

* @purpose Set 'StpCstPortRootGuard'

*

* @description [StpCstPortRootGuard]: sets a port to discard any superior

*              information received by the port and thus protect against root

*              of the device from changing.The port gets put into discarding

*              state and does not forward any packets. enable(1) -

*              enables the root-guard mode for the port. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortRootGuard (void *wap,

                                                                      void

                                                                      *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortRootGuardValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpCstPortRootGuard */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpCstPortRootGuardValue,

                           &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstPortRootGuardValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* set the value in application */

  owa.l7rc =

    usmDbDot1sPortRestrictedRoleSet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                     objStpCstPortRootGuardValue);

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

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortTCNGuard

*

* @purpose Get 'StpCstPortTCNGuard'

*

* @description [StpCstPortTCNGuard]: This command restricts the port from

*              propagating any topology change information received through

*              that port. enable(1) - enables the tcn-guard mode for the

*              port.disable(2) - disables the tcn-guard mode for the port.

*              The default tcn-guard mode is disabled 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortTCNGuard (void *wap,

                                                                     void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortTCNGuardValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc = usmDbDot1sPortRestrictedTcnGet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                             &objStpCstPortTCNGuardValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstPortTCNGuard */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortTCNGuardValue,

                           sizeof (objStpCstPortTCNGuardValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortTCNGuard

*

* @purpose Set 'StpCstPortTCNGuard'

*

* @description [StpCstPortTCNGuard]: This command restricts the port from

*              propagating any topology change information received through

*              that port. enable(1) - enables the tcn-guard mode for the

*              port.disable(2) - disables the tcn-guard mode for the port.

*              The default tcn-guard mode is disabled 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortTCNGuard (void *wap,

                                                                     void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortTCNGuardValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpCstPortTCNGuard */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpCstPortTCNGuardValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstPortTCNGuardValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  

  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* set the value in application */

  owa.l7rc = usmDbDot1sPortRestrictedTcnSet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                             objStpCstPortTCNGuardValue);

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

* @function fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortLoopGuard

*

* @purpose Get 'StpCstPortLoopGuard'

*

* @description [StpCstPortLoopGuard]: This command enables/disables loop guard on this port

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortLoopGuard (void *wap,

                                                                     void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortLoopGuardValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc = usmDbDot1sPortLoopGuardGet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                             &objStpCstPortLoopGuardValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstPortLoopGuard */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortLoopGuardValue,

                           sizeof (objStpCstPortLoopGuardValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortLoopGuard

*

* @purpose Set 'StpCstPortLoopGuard'

*

* @description [StpCstPortLoopGuard]: This command enables/disables loop guard on this port

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingstpCstPortConfigGroup_StpCstPortLoopGuard (void *wap,

                                                                     void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortLoopGuardValue;

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpCstPortLoopGuard */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpCstPortLoopGuardValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstPortLoopGuardValue, owa.len);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.rc != XLIBRC_SUCCESS)

  {

    kwa.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  

  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* set the value in application */

  owa.l7rc = usmDbDot1sPortLoopGuardSet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                             objStpCstPortLoopGuardValue);

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

* @function fpObjGet_SwitchingstpCstPortConfigGroup_Dot1sCistExtPortPathCostMode

*

* @purpose Get 'Dot1sCistExtPortPathCostMode'

*

* @description [Dot1sCistExtPortPathCostMode]: TODO 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_Dot1sCistExtPortPathCostMode(void *wap, void *bufp) 

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objDot1sCistExtPortPathCostModeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* get the value from application */

  objDot1sCistExtPortPathCostModeValue = usmDbDot1sCistExtPortPathCostModeGet(L7_UNIT_CURRENT,                                                                                                                                   tempKeyInterfaceValue);



  /* return the object value: Dot1sCistExtPortPathCostModeValue */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1sCistExtPortPathCostModeValue,

                           sizeof (objDot1sCistExtPortPathCostModeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingstpCstPortConfigGroup_Dot1sCistPortPathCost

*

* @purpose Get 'Dot1sCistPortPathCost'

*

* @description [Dot1sCistPortPathCost]: TODO 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_Dot1sCistPortPathCost(void *wap, void *bufp) 

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objDot1sCistPortPathCostValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc = usmDbDot1sCistPortPathCostGet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                                                        &objDot1sCistPortPathCostValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: Dot1sCistPortPathCostValue */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1sCistPortPathCostValue,

                                          sizeof (objDot1sCistPortPathCostValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingstpCstPortConfigGroup_Dot1sCistPortRegionalRootID

*

* @purpose Get 'Dot1sCistPortRegionalRootID'

*

* @description [Dot1sCistPortRegionalRootID]: TODO 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_Dot1sCistPortRegionalRootID(void *wap, void *bufp) 

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  xLibU32_t tempKeyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA(sizeof(xLibStr256_t));

  xLibStr256_t objDot1sCistPortRegionalRootIDValue={0};

  xLibStr256_t rootId={0};

  xLibU32_t len;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  if (usmDbIntIfNumFromExtIfNum(keyInterfaceValue, &tempKeyInterfaceValue) != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  /* get the value from application */

  owa.l7rc = usmDbDot1sCistPortRegionalRootIDGet (L7_UNIT_CURRENT, tempKeyInterfaceValue,

                                                  rootId,&len);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  fpObj_StpDot1sBridgeId(objDot1sCistPortRegionalRootIDValue, rootId, len);



  /* return the object value: Dot1sCistPortRegionalRootIDValue */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)objDot1sCistPortRegionalRootIDValue,

                                          strlen (objDot1sCistPortRegionalRootIDValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingStpCstPortConfigGroup_Dot1sCstPortPathCostMode

*

* @purpose Get 'Dot1sCstPortPathCostMode'

*

* @description [Dot1sCstPortPathCostMode]: TODO 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_Dot1sCstPortPathCostMode(void *wap, void *bufp) 

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objDot1sCstPortPathCostModeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiPortPathCostModeGet (L7_UNIT_CURRENT, DOT1S_CIST_ID,

                                              keyInterfaceValue, &objDot1sCstPortPathCostModeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: Dot1sCstPortPathCostModeValue */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot1sCstPortPathCostModeValue,

                           sizeof (objDot1sCstPortPathCostModeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingStpCstPortConfigGroup_Dot1sCstPortRole

*

* @purpose Get 'Dot1sCstPortRole'

*

* @description [Dot1sCstPortRole]: TODO 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_Dot1sCstPortRole(void *wap, void *bufp)

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objDot1sCstPortRoleValue;

  xLibU32_t val;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);



  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiPortRoleGet (L7_UNIT_CURRENT, DOT1S_CIST_ID,

                                        keyInterfaceValue,

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

        strcpy(objDot1sCstPortRoleValue, "Disabled");

        break;

      case L7_DOT1S_ROLE_ROOT:

        strcpy(objDot1sCstPortRoleValue, "Root");

        break;

      case L7_DOT1S_ROLE_DESIGNATED:

        strcpy(objDot1sCstPortRoleValue, "Designated");

        break;

      case L7_DOT1S_ROLE_ALTERNATE:

        strcpy(objDot1sCstPortRoleValue, "Alternate");

        break;

      case L7_DOT1S_ROLE_BACKUP:

        strcpy(objDot1sCstPortRoleValue, "Backup");

        break;

      case L7_DOT1S_ROLE_MASTER:

        strcpy(objDot1sCstPortRoleValue, "Master");

        break;

      default:

        strcpy(objDot1sCstPortRoleValue, "-");

        break;

  }



  /* return the object value: Dot1sCstPortRoleValue */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDot1sCstPortRoleValue,

                           strlen (objDot1sCstPortRoleValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingStpCstPortConfigGroup_Dot1sCstPortDesignatedRootID

*

* @purpose Get 'Dot1sCstPortDesignatedRootID'

*

* @description [Dot1sCstPortDesignatedRootID]: TODO 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_Dot1sCstPortDesignatedRootID(void *wap, void *bufp) 

{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objDot1sCstPortDesignatedRootIDValue={0};

  xLibStr256_t rootId={0};

  xLibU32_t len;



  FPOBJ_TRACE_ENTER (bufp);

  memset(objDot1sCstPortDesignatedRootIDValue, 0x0, sizeof(objDot1sCstPortDesignatedRootIDValue));





  /* retrieve key: Interface */

  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);

  if (kwa.l7rc != L7_SUCCESS)

  {

    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, kwa);

    return kwa.rc;

  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  

  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiPortDesignatedRootIDGet (L7_UNIT_CURRENT, DOT1S_CIST_ID,

                                      keyInterfaceValue, rootId, &len);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  fpObj_StpDot1sBridgeId(objDot1sCstPortDesignatedRootIDValue, rootId, len);



  /* return the object value: Dot1sCstPortDesignatedRootIDValue */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDot1sCstPortDesignatedRootIDValue,

                           strlen (objDot1sCstPortDesignatedRootIDValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpCstPortConfigGroup_StpcstPortLoopInconsistentState

*

* @purpose Get 'StpCstPortLoopInconsistentState'

 *@description  [StpMstPortLoopInconsistentState] TODO   

* @notes       

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortLoopInconsistentState (void *wap,

                                                                                  void *bufp)

{



  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortLoopInconsistentStateValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);

  if (kwaInterface.rc != XLIBRC_SUCCESS)

  {

    kwaInterface.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaInterface);

    return kwaInterface.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);



  /* get the value from application */ 

  owa.l7rc = usmDbDot1sMstiPortLoopInconsistentStateGet(L7_UNIT_CURRENT, DOT1S_CIST_ID,

                              keyInterfaceValue, &objStpCstPortLoopInconsistentStateValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpMstPortLoopInconsistentState */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortLoopInconsistentStateValue,

                           sizeof (objStpCstPortLoopInconsistentStateValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}



/*******************************************************************************

* @function fpObjGet_SwitchingStpCstPortConfigGroup_StpCstPortTransitionsIntoLoopInconsistentState

*

* @purpose Get 'StpMstPortTransitionsIntoLoopInconsistentState'

 *@description  [StpMstPortTransitionsIntoLoopInconsistentState] TODO   

* @notes       

*

* @return

*******************************************************************************/

xLibRC_t

fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortTransitionsIntoLoopInconsistentState (void *wap,

                                                                                        void *bufp)

{



  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortTransitionsIntoLoopInconsistentStateValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);

  if (kwaInterface.rc != XLIBRC_SUCCESS)

  {

    kwaInterface.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaInterface);

    return kwaInterface.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);



  /* get the value from application */ 

  owa.l7rc = usmDbDot1sPortStatsTransitionsIntoLoopInconsistentStateGet(L7_UNIT_CURRENT, DOT1S_CIST_ID,

                              keyInterfaceValue,

                              &objStpCstPortTransitionsIntoLoopInconsistentStateValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpMstPortTransitionsIntoLoopInconsistentState */

  owa.rc =

    xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortTransitionsIntoLoopInconsistentStateValue,

                    sizeof (objStpCstPortTransitionsIntoLoopInconsistentStateValue));

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

fpObjGet_SwitchingstpCstPortConfigGroup_StpCstPortTransitionsOutOfLoopInconsistentState (void *wap,

                                                                                         void *bufp)

{



  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t keyInterfaceValue;



  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstPortTransitionsOutOfLoopInconsistentStateValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve key: Interface */

  kwaInterface.rc = xLibFilterGet (wap, XOBJ_SwitchingstpCstPortConfigGroup_Interface,

                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);

  if (kwaInterface.rc != XLIBRC_SUCCESS)

  {

    kwaInterface.rc = XLIBRC_FILTER_MISSING;

    FPOBJ_TRACE_EXIT (bufp, kwaInterface);

    return kwaInterface.rc;

  }



  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);



  /* get the value from application */ 

  owa.l7rc = usmDbDot1sPortStatsTransitionsOutOfLoopInconsistentStateGet(L7_UNIT_CURRENT, DOT1S_CIST_ID,

                              keyInterfaceValue,

                              &objStpCstPortTransitionsOutOfLoopInconsistentStateValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstPortTransitionsOutOfLoopInconsistentState */

  owa.rc =

    xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstPortTransitionsOutOfLoopInconsistentStateValue,

                    sizeof (objStpCstPortTransitionsOutOfLoopInconsistentStateValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}





