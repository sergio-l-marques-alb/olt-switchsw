/*******************************************************************************

*

* (C) Copyright Broadcom Corporation 2000-2007

*

********************************************************************************

*

* @filename fpobj_SwitchingStpGlobalConfig.c

*

* @purpose

*

* @component object handlers

*

* @comments  Refer to MST-object.xml

*

* @create  12 February 2008

*

* @author  

* @end

*

********************************************************************************/

#include "fpobj_util.h"

#include "_xe_SwitchingStpGlobalConfig_obj.h"

#include "usmdb_dot1s_api.h"

#include "dot1s_exports.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"
#include "usmdb_dot1q_api.h"

void fpObj_StpDot1sBridgeId(xLibU8_t *buf, xLibU8_t *temp, xLibU32_t val);

void fpObj_StpDot1sPortId(xLibU8_t * buf, xLibU32_t val);



/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpConfigDigestKey

*

* @purpose Get 'StpConfigDigestKey'

*

* @description [StpConfigDigestKey]: The MST configuration digest key 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpConfigDigestKey (void *wap,

                                                               void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objStpConfigDigestKeyValue;

  L7_uchar8 buf[40], temp[40];

  FPOBJ_TRACE_ENTER (bufp);



  memset (buf, 0, sizeof (buf));

  memset (temp, 0, sizeof (temp));

  /* get the value from application */

  owa.l7rc = usmDbDot1sConfigDigestKeyGet (L7_UNIT_CURRENT, temp);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  sprintf(buf, "%s0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",

            buf, temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7],

            temp[8], temp[9], temp[10], temp[11], temp[12], temp[13], temp[14], temp[15]); 

  strcpy(objStpConfigDigestKeyValue,buf);

  FPOBJ_TRACE_VALUE (bufp, objStpConfigDigestKeyValue,

                     strlen (objStpConfigDigestKeyValue));



  /* return the object value: StpConfigDigestKey */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpConfigDigestKeyValue,

                           strlen (objStpConfigDigestKeyValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstRegionalRootId

*

* @purpose Get 'StpCstRegionalRootId'

*

* @description [StpCstRegionalRootId]: The MSTP regional root identifier for

*              the CIST 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstRegionalRootId (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objStpCstRegionalRootIdValue={0};

  xLibStr256_t rootId={0};

  xLibU32_t length;

  FPOBJ_TRACE_ENTER (bufp);

  

  memset (objStpCstRegionalRootIdValue, 0x0, sizeof (objStpCstRegionalRootIdValue));



  /* get the value from application */

  owa.l7rc = usmDbDot1sCistRegionalRootIDGet (L7_UNIT_CURRENT, rootId, &length);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  fpObj_StpDot1sBridgeId(objStpCstRegionalRootIdValue, rootId, length);



  /* return the object value: StpCstRegionalRootId */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpCstRegionalRootIdValue,

                           strlen (objStpCstRegionalRootIdValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstRegionalRootPathCost

*

* @purpose Get 'StpCstRegionalRootPathCost'

*

* @description [StpCstRegionalRootPathCost]: The MSTP regional root path cost

*              for the CIST 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstRegionalRootPathCost (void

                                                                       *wap,

                                                                       void

                                                                       *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstRegionalRootPathCostValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sCistRegionalRootPathCostGet (L7_UNIT_CURRENT,

                                           &objStpCstRegionalRootPathCostValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstRegionalRootPathCostValue,

                     sizeof (objStpCstRegionalRootPathCostValue));



  /* return the object value: StpCstRegionalRootPathCost */

  owa.rc =

    xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstRegionalRootPathCostValue,

                    sizeof (objStpCstRegionalRootPathCostValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpAdminMode

*

* @purpose Get 'StpAdminMode'

*

* @description [StpAdminMode]: The spanning tree operational status. enable(1)

*              - enables spanning tree operational status on the switch.disable(2)

*              - disables spanning tree operational status on

*              the switch. The default status is disabled. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpAdminMode (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc = usmDbDot1sModeGet (L7_UNIT_CURRENT, &objStpAdminModeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpAdminModeValue,

                     sizeof (objStpAdminModeValue));



  /* return the object value: StpAdminMode */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpAdminModeValue,

                           sizeof (objStpAdminModeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingStpGlobalConfig_StpAdminMode

*

* @purpose Set 'StpAdminMode'

*

* @description [StpAdminMode]: The spanning tree operational status. enable(1)

*              - enables spanning tree operational status on the switch.disable(2)

*              - disables spanning tree operational status on

*              the switch. The default status is disabled. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpGlobalConfig_StpAdminMode (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpAdminModeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpAdminMode */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStpAdminModeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpAdminModeValue, owa.len);



  /* set the value in application */

  owa.l7rc = usmDbDot1sModeSet (L7_UNIT_CURRENT, objStpAdminModeValue);

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

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstRootFwdDelay

*

* @purpose Get 'StpCstRootFwdDelay'

*

* @description [StpCstRootFwdDelay]: The MSTP root port forward delay for

*              the CIST. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstRootFwdDelay (void *wap,

                                                               void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstRootFwdDelayValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sCistRootFwdDelayGet (L7_UNIT_CURRENT,

                                   &objStpCstRootFwdDelayValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstRootFwdDelayValue,

                     sizeof (objStpCstRootFwdDelayValue));



  /* return the object value: StpCstRootFwdDelay */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstRootFwdDelayValue,

                           sizeof (objStpCstRootFwdDelayValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstBridgePriority

*

* @purpose Get 'StpCstBridgePriority'

*

* @description [StpCstBridgePriority]: The MSTP bridge priority for the CIST.

*              The default value is 32768. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstBridgePriority (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstBridgePriorityValue;

  FPOBJ_TRACE_ENTER (bufp);

  xLibU16_t priorityVal=0;



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sMstiBridgePriorityGet (L7_UNIT_CURRENT,

                                     DOT1S_CIST_ID,

                                     &priorityVal);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  objStpCstBridgePriorityValue = priorityVal;



  FPOBJ_TRACE_VALUE (bufp, &objStpCstBridgePriorityValue,

                     sizeof (objStpCstBridgePriorityValue));



  /* return the object value: StpCstBridgePriority */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstBridgePriorityValue,

                           sizeof (objStpCstBridgePriorityValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingStpGlobalConfig_StpCstBridgePriority

*

* @purpose Set 'StpCstBridgePriority'

*

* @description [StpCstBridgePriority]: The MSTP bridge priority for the CIST.

*              The default value is 32768. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpGlobalConfig_StpCstBridgePriority (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstBridgePriorityValue=0;
  
  /* Declara a temp variable for priority */
  xLibU32_t tempPriorityValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpCstBridgePriority */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpCstBridgePriorityValue,

                           &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstBridgePriorityValue, owa.len);

  /* Copy the priority value in temp */
  tempPriorityValue = objStpCstBridgePriorityValue;

  /* set the value in application */

  owa.l7rc = usmDbDot1sMstiBridgePrioritySet (L7_UNIT_CURRENT, 

                                              DOT1S_CIST_ID,

                                              &objStpCstBridgePriorityValue);



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
  if(tempPriorityValue != objStpCstBridgePriorityValue)
  {
    owa.rc = XLIBRC_PRIORITY_SET;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstBridgeHoldCount

*

* @purpose Get 'StpCstBridgeHoldCount'

*

* @description [StpCstBridgeHoldCount]: The MSTP bridge hold count for the

*              CIST. This command sets the value of maximum bpdus that a

*              bridge is allowed to send within a hello time window.The default

*              value is 6. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstBridgeHoldCount (void *wap,

                                                                  void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstBridgeHoldCountValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sBridgeTxHoldCountGet (L7_UNIT_CURRENT,

                                    &objStpCstBridgeHoldCountValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstBridgeHoldCountValue,

                     sizeof (objStpCstBridgeHoldCountValue));



  /* return the object value: StpCstBridgeHoldCount */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstBridgeHoldCountValue,

                           sizeof (objStpCstBridgeHoldCountValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingStpGlobalConfig_StpCstBridgeHoldCount

*

* @purpose Set 'StpCstBridgeHoldCount'

*

* @description [StpCstBridgeHoldCount]: The MSTP bridge hold count for the

*              CIST. This command sets the value of maximum bpdus that a

*              bridge is allowed to send within a hello time window.The default

*              value is 6. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpGlobalConfig_StpCstBridgeHoldCount (void *wap,

                                                                  void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstBridgeHoldCountValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpCstBridgeHoldCount */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpCstBridgeHoldCountValue,

                           &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstBridgeHoldCountValue, owa.len);



  /* set the value in application */

  owa.l7rc =

    usmDbDot1sBridgeTxHoldCountSet (L7_UNIT_CURRENT,

                                    objStpCstBridgeHoldCountValue);

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

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstMaxAge

*

* @purpose Get 'StpCstMaxAge'

*

* @description [StpCstMaxAge]: The MSTP root port max age for the CIST. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstMaxAge (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstMaxAgeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc = usmDbDot1sCistMaxAgeGet (L7_UNIT_CURRENT, &objStpCstMaxAgeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstMaxAgeValue,

                     sizeof (objStpCstMaxAgeValue));



  /* return the object value: StpCstMaxAge */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstMaxAgeValue,

                           sizeof (objStpCstMaxAgeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpConfigFormatSelector

*

* @purpose Get 'StpConfigFormatSelector'

*

* @description [StpConfigFormatSelector]: he MST configuration format selector.

*              A value other than 0 (zero) indicates non-support for

*              the IEEE 802.1s standard. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpConfigFormatSelector (void *wap,

                                                                    void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpConfigFormatSelectorValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sConfigFormatSelectorGet (L7_UNIT_CURRENT,

                                       &objStpConfigFormatSelectorValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpConfigFormatSelectorValue,

                     sizeof (objStpConfigFormatSelectorValue));



  /* return the object value: StpConfigFormatSelector */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpConfigFormatSelectorValue,

                           sizeof (objStpConfigFormatSelectorValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjSet_SwitchingStpGlobalConfig_StpConfigFormatSelector

*

* @purpose Set 'StpConfigFormatSelector'

*

* @description [StpConfigFormatSelector]: he MST configuration format selector.

*              A value other than 0 (zero) indicates non-support for

*              the IEEE 802.1s standard. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpGlobalConfig_StpConfigFormatSelector (void *wap,

                                                                    void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpConfigFormatSelectorValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpConfigFormatSelector */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpConfigFormatSelectorValue,

                           &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpConfigFormatSelectorValue, owa.len);



  /* set the value in application */

  owa.l7rc = usmDbDot1sConfigFormatSelectorSet (L7_UNIT_CURRENT, objStpConfigFormatSelectorValue);

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

* @function fpObjGet_SwitchingStpGlobalConfig_StpConfigName

*

* @purpose Get 'StpConfigName'

*

* @description [StpConfigName]: The MST configuration name of at most 32 characters.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpConfigName (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objStpConfigNameValue;

  FPOBJ_TRACE_ENTER (bufp);



  memset(objStpConfigNameValue, 0x0, sizeof(objStpConfigNameValue));



  /* get the value from application */

  owa.l7rc = usmDbDot1sConfigNameGet (L7_UNIT_CURRENT, objStpConfigNameValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objStpConfigNameValue,

                     strlen (objStpConfigNameValue));



  /* return the object value: StpConfigName */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpConfigNameValue,

                           strlen (objStpConfigNameValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingStpGlobalConfig_StpConfigName

*

* @purpose Set 'StpConfigName'

*

* @description [StpConfigName]: The MST configuration name of at most 32 characters.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpGlobalConfig_StpConfigName (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objStpConfigNameValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpConfigName */

  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objStpConfigNameValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, objStpConfigNameValue, owa.len);



  /* set the value in application */

  owa.l7rc = usmDbDot1sConfigNameSet (L7_UNIT_CURRENT, objStpConfigNameValue);

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

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstBridgeMaxAge

*

* @purpose Get 'StpCstBridgeMaxAge'

*

* @description [StpCstBridgeMaxAge]: The MSTP bridge max age for the CIST.

*              The default value is 20. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstBridgeMaxAge (void *wap,

                                                               void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstBridgeMaxAgeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sCistBridgeMaxAgeGet (L7_UNIT_CURRENT,

                                   &objStpCstBridgeMaxAgeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstBridgeMaxAgeValue,

                     sizeof (objStpCstBridgeMaxAgeValue));



  /* return the object value: StpCstBridgeMaxAge */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstBridgeMaxAgeValue,

                           sizeof (objStpCstBridgeMaxAgeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingStpGlobalConfig_StpCstBridgeMaxAge

*

* @purpose Set 'StpCstBridgeMaxAge'

*

* @description [StpCstBridgeMaxAge]: The MSTP bridge max age for the CIST.

*              The default value is 20. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpGlobalConfig_StpCstBridgeMaxAge (void *wap,

                                                               void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstBridgeMaxAgeValue;
  xLibU32_t objStpCstBridgeFwdDelayValue;
  xLibU32_t objStpCstBridgeHelloTimeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpCstBridgeMaxAge */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpCstBridgeMaxAgeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstBridgeMaxAgeValue, owa.len);

  /* Get the Forward Delay value */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpGlobalConfig_StpCstBridgeFwdDelay,

                          (xLibU8_t *) & objStpCstBridgeFwdDelayValue, &owa.len);
  if (owa.rc == XLIBRC_SUCCESS)

  {
    FPOBJ_TRACE_VALUE (bufp, &objStpCstBridgeFwdDelayValue, owa.len);

    if(objStpCstBridgeMaxAgeValue > ((objStpCstBridgeFwdDelayValue - 1)<<1))
    {
      owa.rc = XLIBRC_BRIDGE_MAX_AGE_CONDITION_FAILED;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
  
      return owa.rc;
    }
    else
    {
                /* Get the Current Hello Time value */
      owa.l7rc = usmDbDot1sCistBridgeHelloTimeGet (L7_UNIT_CURRENT,
                                     &objStpCstBridgeHelloTimeValue);

      if (owa.l7rc == L7_SUCCESS)
      {
                /* Set Forward Delay,Max age,Hello Time values */
        owa.l7rc = usmDbDot1sCistBridgeParametersSet (L7_UNIT_CURRENT,
                                     objStpCstBridgeFwdDelayValue,
                                     objStpCstBridgeMaxAgeValue,
                                     objStpCstBridgeHelloTimeValue);
        if (owa.l7rc == L7_SUCCESS)
        {
          owa.rc = XLIBRC_SUCCESS;    /* TODO: Change if required */
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }
      }
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }



  /* set the value in application */

  owa.l7rc =

    usmDbDot1sCistBridgeMaxAgeSet (L7_UNIT_CURRENT, objStpCstBridgeMaxAgeValue);

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

* @function fpObjGet_SwitchingStpGlobalConfig_StpBpduGuardMode

*

* @purpose Get 'StpBpduGuardMode'

*

* @description [StpBpduGuardMode]: The spanning tree BPDU Guard Mode. enable(1)

*              - enables BPDU Guard Mode on the switch. disable(2) -

*              disables BPDU Guard Mode on the switch. The default status

*              is disabled. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpBpduGuardMode (void *wap,

                                                             void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpBpduGuardModeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sBpduGuardGet (L7_UNIT_CURRENT, &objStpBpduGuardModeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpBpduGuardModeValue,

                     sizeof (objStpBpduGuardModeValue));



  /* return the object value: StpBpduGuardMode */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpBpduGuardModeValue,

                           sizeof (objStpBpduGuardModeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingStpGlobalConfig_StpBpduGuardMode

*

* @purpose Set 'StpBpduGuardMode'

*

* @description [StpBpduGuardMode]: The spanning tree BPDU Guard Mode. enable(1)

*              - enables BPDU Guard Mode on the switch. disable(2) -

*              disables BPDU Guard Mode on the switch. The default status

*              is disabled. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpGlobalConfig_StpBpduGuardMode (void *wap,

                                                             void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpBpduGuardModeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpBpduGuardMode */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpBpduGuardModeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpBpduGuardModeValue, owa.len);



  /* set the value in application */

  owa.l7rc = usmDbDot1sBpduGuardSet (L7_UNIT_CURRENT, objStpBpduGuardModeValue);

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

* @function fpObjGet_SwitchingStpGlobalConfig_StpBpduFilterDefault

*

* @purpose Get 'StpBpduFilterDefault'

*

* @description [StpBpduFilterDefault]: The spanning tree BPDU Filter Mode,

*              it enables BPDU Filter on all edge ports. enable(1) - enables

*              BPDU Filter Mode on the switch. disable(2) - disables BPDU

*              Filter Mode on the switch. The default status is disabled.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpBpduFilterDefault (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpBpduFilterDefaultValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sBpduFilterGet (L7_UNIT_CURRENT, &objStpBpduFilterDefaultValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpBpduFilterDefaultValue,

                     sizeof (objStpBpduFilterDefaultValue));



  /* return the object value: StpBpduFilterDefault */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpBpduFilterDefaultValue,

                           sizeof (objStpBpduFilterDefaultValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjSet_SwitchingStpGlobalConfig_StpBpduFilterDefault

*

* @purpose Get 'StpBpduFilterDefault'

*

* @description [StpBpduFilterDefault]: The spanning tree BPDU Filter Mode,

*              it enables BPDU Filter on all edge ports. enable(1) - enables

*              BPDU Filter Mode on the switch. disable(2) - disables BPDU

*              Filter Mode on the switch. The default status is disabled.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpGlobalConfig_StpBpduFilterDefault (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpBpduFilterDefaultValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpBpduFilterDefault */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpBpduFilterDefaultValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpBpduFilterDefaultValue, owa.len);





  /* get the value from application */

  owa.l7rc =

    usmDbDot1sBpduFilterSet (L7_UNIT_CURRENT, objStpBpduFilterDefaultValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpBpduFilterDefaultValue,

                     sizeof (objStpBpduFilterDefaultValue));



  /* return the object value: StpBpduFilterDefault */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpBpduFilterDefaultValue,

                           sizeof (objStpBpduFilterDefaultValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}







/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstBridgeHoldTime

*

* @purpose Get 'StpCstBridgeHoldTime'

*

* @description [StpCstBridgeHoldTime]: The MSTP bridge hold time for the CIST.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstBridgeHoldTime (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstBridgeHoldTimeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sCistBridgeHoldTimeGet (L7_UNIT_CURRENT,

                                     &objStpCstBridgeHoldTimeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstBridgeHoldTimeValue,

                     sizeof (objStpCstBridgeHoldTimeValue));



  /* return the object value: StpCstBridgeHoldTime */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstBridgeHoldTimeValue,

                           sizeof (objStpCstBridgeHoldTimeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstBridgeHelloTime

*

* @purpose Get 'StpCstBridgeHelloTime'

*

* @description [StpCstBridgeHelloTime]: The MSTP bridge hello time for the

*              CIST. The default value is 2. According to IEEE 802.1Q-REV

*              2005 updating hello time is disallowed 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstBridgeHelloTime (void *wap,

                                                                  void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstBridgeHelloTimeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sCistBridgeHelloTimeGet (L7_UNIT_CURRENT,

                                      &objStpCstBridgeHelloTimeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstBridgeHelloTimeValue,

                     sizeof (objStpCstBridgeHelloTimeValue));



  /* return the object value: StpCstBridgeHelloTime */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstBridgeHelloTimeValue,

                           sizeof (objStpCstBridgeHelloTimeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstBridgeFwdDelay

*

* @purpose Get 'StpCstBridgeFwdDelay'

*

* @description [StpCstBridgeFwdDelay]: The MSTP bridge forward delay for the

*              CIST. The default value is 15 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstBridgeFwdDelay (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstBridgeFwdDelayValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sCistBridgeFwdDelayGet (L7_UNIT_CURRENT,

                                     &objStpCstBridgeFwdDelayValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstBridgeFwdDelayValue,

                     sizeof (objStpCstBridgeFwdDelayValue));



  /* return the object value: StpCstBridgeFwdDelay */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstBridgeFwdDelayValue,

                           sizeof (objStpCstBridgeFwdDelayValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingStpGlobalConfig_StpCstBridgeFwdDelay

*

* @purpose Set 'StpCstBridgeFwdDelay'

*

* @description [StpCstBridgeFwdDelay]: The MSTP bridge forward delay for the

*              CIST. The default value is 15 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpGlobalConfig_StpCstBridgeFwdDelay (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstBridgeFwdDelayValue;
  xLibU32_t objStpCstBridgeMaxAgeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpCstBridgeFwdDelay */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpCstBridgeFwdDelayValue,

                           &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstBridgeFwdDelayValue, owa.len);

        /* Get the Bridge max Age Value */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingStpGlobalConfig_StpCstBridgeMaxAge,

                          (xLibU8_t *) & objStpCstBridgeMaxAgeValue, &owa.len);
  if (owa.rc == XLIBRC_SUCCESS)

  {
    FPOBJ_TRACE_VALUE (bufp, &objStpCstBridgeMaxAgeValue, owa.len);

    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }



  /* set the value in application */

  owa.l7rc =

    usmDbDot1sCistBridgeFwdDelaySet (L7_UNIT_CURRENT,

                                     objStpCstBridgeFwdDelayValue);

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

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstHelloTime

*

* @purpose Get 'StpCstHelloTime'

*

* @description [StpCstHelloTime]: The MSTP root port hello time for the CIST

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstHelloTime (void *wap,

                                                            void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstHelloTimeValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sCistHelloTimeGet (L7_UNIT_CURRENT, &objStpCstHelloTimeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstHelloTimeValue,

                     sizeof (objStpCstHelloTimeValue));



  /* return the object value: StpCstHelloTime */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstHelloTimeValue,

                           sizeof (objStpCstHelloTimeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpConfigRevision

*

* @purpose Get 'StpConfigRevision'

*

* @description [StpConfigRevision]: The MST configuration revision. The default

*              value is 1 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpConfigRevision (void *wap,

                                                              void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpConfigRevisionValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sConfigRevisionGet (L7_UNIT_CURRENT, &objStpConfigRevisionValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpConfigRevisionValue,

                     sizeof (objStpConfigRevisionValue));



  /* return the object value: StpConfigRevision */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpConfigRevisionValue,

                           sizeof (objStpConfigRevisionValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingStpGlobalConfig_StpConfigRevision

*

* @purpose Set 'StpConfigRevision'

*

* @description [StpConfigRevision]: The MST configuration revision. The default

*              value is 1 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpGlobalConfig_StpConfigRevision (void *wap,

                                                              void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpConfigRevisionValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpConfigRevision */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpConfigRevisionValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpConfigRevisionValue, owa.len);



  /* set the value in application */

  owa.l7rc =

    usmDbDot1sConfigRevisionSet (L7_UNIT_CURRENT, objStpConfigRevisionValue);

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

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstBridgeMaxHops

*

* @purpose Get 'StpCstBridgeMaxHops'

*

* @description [StpCstBridgeMaxHops]: The MSTP bridge max hops for the CIST.

*              The default value is 20 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstBridgeMaxHops (void *wap,

                                                                void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstBridgeMaxHopsValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sBridgeMaxHopGet (L7_UNIT_CURRENT, &objStpCstBridgeMaxHopsValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstBridgeMaxHopsValue,

                     sizeof (objStpCstBridgeMaxHopsValue));



  /* return the object value: StpCstBridgeMaxHops */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstBridgeMaxHopsValue,

                           sizeof (objStpCstBridgeMaxHopsValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingStpGlobalConfig_StpCstBridgeMaxHops

*

* @purpose Set 'StpCstBridgeMaxHops'

*

* @description [StpCstBridgeMaxHops]: The MSTP bridge max hops for the CIST.

*              The default value is 20 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpGlobalConfig_StpCstBridgeMaxHops (void *wap,

                                                                void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstBridgeMaxHopsValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpCstBridgeMaxHops */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpCstBridgeMaxHopsValue,

                           &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpCstBridgeMaxHopsValue, owa.len);



  /* set the value in application */

  owa.l7rc = usmDbDot1sBridgeMaxHopSet(L7_UNIT_CURRENT, objStpCstBridgeMaxHopsValue);

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

* @function fpObjGet_SwitchingStpGlobalConfig_StpForceVersion

*

* @purpose Get 'StpForceVersion'

*

* @description [StpForceVersion]: The MST configuration force protocol version.

*              The default version is dot1s. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpForceVersion (void *wap,

                                                            void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpForceVersionValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sForceVersionGet (L7_UNIT_CURRENT, &objStpForceVersionValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpForceVersionValue,

                     sizeof (objStpForceVersionValue));



  /* return the object value: StpForceVersion */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpForceVersionValue,

                           sizeof (objStpForceVersionValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjSet_SwitchingStpGlobalConfig_StpForceVersion

*

* @purpose Set 'StpForceVersion'

*

* @description [StpForceVersion]: The MST configuration force protocol version.

*              The default version is dot1s. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpGlobalConfig_StpForceVersion (void *wap,

                                                            void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpForceVersionValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpForceVersion */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpForceVersionValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpForceVersionValue, owa.len);



  /* set the value in application */

  owa.l7rc =

    usmDbDot1sForceVersionSet (L7_UNIT_CURRENT, objStpForceVersionValue);

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

* @function fpObjGet_SwitchingStpGlobalConfig_StpiBridgeIdentifier

*

* @purpose Get 'StpForceVersion'

*

* @description [StpForceVersion]: The MST configuration force protocol version.

*              The default version is dot1s. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstBridgeIdentifier(void *wap,

                                                                  void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objStpBridgeIdentifierValue={0};

  xLibStr256_t bridgeId={0};

  xLibU32_t length;

  FPOBJ_TRACE_ENTER (bufp);



  memset(objStpBridgeIdentifierValue, 0x0, sizeof(objStpBridgeIdentifierValue)); 



  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiBridgeIdentifierGet(L7_UNIT_CURRENT, DOT1S_CIST_ID,

                                                bridgeId, &length);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



/*  fpObj_StpFormatBridgeIdentifier(objStpBridgeIdentifierValue, length, bridgeId); */

  fpObj_StpDot1sBridgeId(objStpBridgeIdentifierValue, bridgeId, length);



  /* return the object value: StpBridgeIdentifier */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpBridgeIdentifierValue,

                           strlen (objStpBridgeIdentifierValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;



}





/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstTimeSinceTopologyChange

*

* @purpose Get 'StpMstTimeSinceTopologyChange'

*

* @description [StpMstTimeSinceTopologyChange]: The MSTP time since the last

*              topology change in a specific instance. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstTimeSinceTopologyChange (void

                                                                       *wap,

                                                                       void

                                                                       *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  usmDbTimeSpec_t stpCstTimeSinceTopologyChangeValue;

  FPOBJ_TRACE_ENTER (bufp);

  xLibStr256_t  objStpCstTimeSinceTopologyChangeValue;



  memset(&stpCstTimeSinceTopologyChangeValue, 0, sizeof(usmDbTimeSpec_t));



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sMstiTimeSinceTopologyChangeGet(L7_UNIT_CURRENT, DOT1S_CIST_ID,

                                              &stpCstTimeSinceTopologyChangeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  memset(objStpCstTimeSinceTopologyChangeValue,0x0,sizeof(objStpCstTimeSinceTopologyChangeValue));

  osapiSnprintf(objStpCstTimeSinceTopologyChangeValue,sizeof(objStpCstTimeSinceTopologyChangeValue), 

                           "%d day %d hr %d min %d sec",stpCstTimeSinceTopologyChangeValue.days,

                            stpCstTimeSinceTopologyChangeValue.hours,stpCstTimeSinceTopologyChangeValue.minutes,

                            stpCstTimeSinceTopologyChangeValue.seconds);                                                        



  /* return the object value: StpMstTimeSinceTopologyChange */

  owa.rc =

    xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstTimeSinceTopologyChangeValue,

                    sizeof (objStpCstTimeSinceTopologyChangeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstTopologyChangeCount

*

* @purpose Get 'StpCstTopologyChangeCount'

*

* @description [StpCstTopologyChangeCount]: The MSTP count of topology changes

*              in a specific instance. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstTopologyChangeCount (void *wap,

                                                                   void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstTopologyChangeCountValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sMstiTopologyChangeCountGet(L7_UNIT_CURRENT, DOT1S_CIST_ID,

                                          &objStpCstTopologyChangeCountValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstTopologyChangeCount */

  owa.rc =

    xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstTopologyChangeCountValue,

                    sizeof (objStpCstTopologyChangeCountValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstTopologyChangeParm

*

* @purpose Get 'StpCstTopologyChangeParm'

*

* @description [StpCstTopologyChangeParm]: The MSTP topology change parameter

*              in a specific instance 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstTopologyChangeParm (void *wap,

                                                                  void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstTopologyChangeParmValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc =

    usmDbDot1sMstiTopologyChangeParmGet(L7_UNIT_CURRENT, DOT1S_CIST_ID,

                                         &objStpCstTopologyChangeParmValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstTopologyChangeParm */

  owa.rc =

    xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstTopologyChangeParmValue,

                    sizeof (objStpCstTopologyChangeParmValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}







/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstDesignatedRootId

*

* @purpose Get 'StpMstDesignatedRootId'

*

* @description [StpMstDesignatedRootId]: The MSTP designated root bridge identifier

*              in a specific instance. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstDesignatedRootId (void *wap,

                                                                void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objStpCstDesignatedRootIdValue={0};

  xLibStr256_t rootId={0};

  xLibU32_t length;

  FPOBJ_TRACE_ENTER (bufp);

  memset(objStpCstDesignatedRootIdValue, 0x0, sizeof(objStpCstDesignatedRootIdValue)); 



  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiDesignatedRootIDGet (L7_UNIT_CURRENT, DOT1S_CIST_ID,

                                                rootId,

                                                &length);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  fpObj_StpDot1sBridgeId(objStpCstDesignatedRootIdValue,rootId, length);



  /* return the object value: StpCstDesignatedRootId */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpCstDesignatedRootIdValue,

                           strlen (objStpCstDesignatedRootIdValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstRootPathCost

*

* @purpose Get 'StpMstRootPathCost'

*

* @description [StpMstRootPathCost]: The MSTP root path cost in a specific

*              instance. 

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstRootPathCost (void *wap,

                                                            void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpCstRootPathCostValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiRootPathCostGet (L7_UNIT_CURRENT, DOT1S_CIST_ID,

                                            &objStpCstRootPathCostValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  /* return the object value: StpCstRootPathCost */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpCstRootPathCostValue,

                           sizeof (objStpCstRootPathCostValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}





/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpCstRootPortId

*

* @purpose Get 'StpMstRootPortId'

*

* @description [StpMstRootPortId]: The MSTP root port ID in a specific instance.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpCstRootPortId (void *wap, void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));

  xLibStr256_t objStpCstRootPortIdValue={0};

  xLibU32_t portId;

  FPOBJ_TRACE_ENTER (bufp);



  memset(objStpCstRootPortIdValue, 0x0, sizeof(objStpCstRootPortIdValue)); 



  /* get the value from application */

  owa.l7rc = usmDbDot1sMstiRootPortIDGet (L7_UNIT_CURRENT, DOT1S_CIST_ID,

                                          &portId);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }



  fpObj_StpDot1sPortId(objStpCstRootPortIdValue, portId);



  /* return the object value: StpCstRootPortId */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objStpCstRootPortIdValue,

                           strlen (objStpCstRootPortIdValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpBpduFilterDefault

*

* @purpose Get 'StpBpduFilterDefault'

*

* @description [StpBpduFilterDefault]: The spanning tree BPDU Filter Mode,

*              it enables BPDU Filter on all edge ports. enable(1) - enables

*              BPDU Filter Mode on the switch. disable(2) - disables BPDU

*              Filter Mode on the switch. The default status is disabled.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpGlobalCstLoopGuard (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpGlobalCstLoopGuardValue;
  xLibU32_t intIfNum=0;
  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */
  if (usmDbGetNextPhysicalIntIfNumber(intIfNum, &intIfNum) == L7_SUCCESS)
  {
  owa.l7rc =

    usmDbDot1sPortLoopGuardGet (L7_UNIT_CURRENT, intIfNum,&objStpGlobalCstLoopGuardValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }
  }
  FPOBJ_TRACE_VALUE (bufp, &objStpGlobalCstLoopGuardValue,

                     sizeof (objStpGlobalCstLoopGuardValue));



  /* return the object value: StpBpduFilterDefault */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpGlobalCstLoopGuardValue,

                           sizeof (objStpGlobalCstLoopGuardValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjSet_SwitchingStpGlobalConfig_SStpGlobalCstLoopGuard

*

* @purpose Get 'StpGlobalCstLoopGuard'

*

* @description [StpGlobalCstLoopGuard: The spanning tree BPDU Loop Guard Mode,

*              it enables BPDU guard  on all edge ports. enable(1) - enables

*              BPDU guard Mode on the switch. disable(2) - disables BPDU

*              guard Mode on the switch. The default status is disabled.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpGlobalConfig_StpGlobalCstLoopGuard(void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpGlobalCstLoopGuardValue;
  xLibU32_t intIfNum=0;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpBpduFilterDefault */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpGlobalCstLoopGuardValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpGlobalCstLoopGuardValue, owa.len);





  /* get the value from application */

   while (1)
   {
       if (usmDbGetNextPhysicalIntIfNumber(intIfNum, &intIfNum) != L7_SUCCESS)
         break;
       if ((usmDbDot1sPortLoopGuardSet(L7_UNIT_CURRENT, intIfNum, objStpGlobalCstLoopGuardValue)) != L7_SUCCESS)
       {
         owa.l7rc = L7_FAILURE; 
         break;
       }
  }    
  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpGlobalCstLoopGuardValue,

                     sizeof (objStpGlobalCstLoopGuardValue));



  /* return the object value: StpBpduFilterDefault */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpGlobalCstLoopGuardValue,

                           sizeof (objStpGlobalCstLoopGuardValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}

/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpGlobalCstLoopGuard

*

* @purpose Get 'StpGlobalCstLoopGuard'

*

* @description [StpGlobalCstLoopGuard]: The spanning tree BPDU Loop guard Mode,

*              it enables BPDU Loop guard on all edge ports. enable(1) - enables

*              BPDU Loop guard Mode on the switch. disable(2) - disables BPDU

*              Loop guard Mode on the switch. The default status is disabled.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpGlobalCstPortEdge (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpGlobalCstPortEdgeValue;
  xLibU32_t intIfNum=0;
  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */
  if (usmDbGetNextPhysicalIntIfNumber(intIfNum, &intIfNum) == L7_SUCCESS)
  {
  owa.l7rc =

    usmDbDot1sCistPortEdgeGet (L7_UNIT_CURRENT, intIfNum,&objStpGlobalCstPortEdgeValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }
  }
  FPOBJ_TRACE_VALUE (bufp, &objStpGlobalCstPortEdgeValue,

                     sizeof (objStpGlobalCstPortEdgeValue));



  /* return the object value: StpBpduFilterDefault */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpGlobalCstPortEdgeValue,

                           sizeof (objStpGlobalCstPortEdgeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjSet_SwitchingStpGlobalConfig_StpGlobalCstPortEdge

*

* @purpose Get 'StpGlobalCstPortEdge'

*

* @description [StpGlobalCstPortEdge]: The spanning tree BPDU Port Fast Mode,

*              it enables BPDU Port Fast on all edge ports. enable(1) - enables

*              BPDU Port Fast Mode on the switch. disable(2) - disables BPDU

*              Port Fast Mode on the switch. The default status is disabled.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpGlobalConfig_StpGlobalCstPortEdge(void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpGlobalCstPortEdgeValue;
  xLibU32_t intIfNum=0;
  xLibU32_t portMode;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpBpduFilterDefault */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpGlobalCstPortEdgeValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpGlobalCstPortEdgeValue, owa.len);





  /* get the value from application */

   while (1)
   {
       if (usmDbGetNextPhysicalIntIfNumber(intIfNum, &intIfNum) != L7_SUCCESS)
         break;
       if (((usmDbDot1qSwPortModeGet(L7_UNIT_CURRENT, intIfNum, &portMode)) == L7_SUCCESS)
                            && (portMode == DOT1Q_SWPORT_MODE_ACCESS))
       {
         if ((usmDbDot1sCistPortEdgeSet(L7_UNIT_CURRENT, intIfNum, objStpGlobalCstPortEdgeValue)) != L7_SUCCESS)
         {
           owa.l7rc = L7_FAILURE; 
           break;
         }
      } 
  }    
  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpGlobalCstPortEdgeValue,

                     sizeof (objStpGlobalCstPortEdgeValue));



  /* return the object value: StpBpduFilterDefault */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpGlobalCstPortEdgeValue,

                           sizeof (objStpGlobalCstPortEdgeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}


/*******************************************************************************

* @function fpObjGet_SwitchingStpGlobalConfig_StpGlobalCstBpduFlood

*

* @purpose Get 'StpGlobalCstBpduFlood'

*

* @description StpGlobalCstBpduFlood]: The spanning tree BPDU Port Fast Mode,

*              it enables BPDU Port Fast on all edge ports. enable(1) - enables

*              BPDU Port Fast Mode on the switch. disable(2) - disables BPDU

*              Port Fast Mode on the switch. The default status is disabled.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjGet_SwitchingStpGlobalConfig_StpGlobalCstBpduFlood (void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpGlobalCstBpduFloodValue;
  xLibU32_t intIfNum=0;

  FPOBJ_TRACE_ENTER (bufp);



  /* get the value from application */
  if (usmDbGetNextPhysicalIntIfNumber(intIfNum, &intIfNum) == L7_SUCCESS)
  {
  owa.l7rc =

    usmDbDot1sIntfBpduFloodGet (L7_UNIT_CURRENT, intIfNum,&objStpGlobalCstBpduFloodValue);

  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }
  }
  FPOBJ_TRACE_VALUE (bufp, &objStpGlobalCstBpduFloodValue,

                     sizeof (objStpGlobalCstBpduFloodValue));



  /* return the object value: StpBpduFilterDefault */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpGlobalCstBpduFloodValue,

                           sizeof (objStpGlobalCstBpduFloodValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}



/*******************************************************************************

* @function fpObjSet_SwitchingStpGlobalConfig_StpGlobalCstBpduFlood

*

* @purpose Get 'StpGlobalCstBpduFlood'

*

* @description [StpGlobalCstBpduFlood]: The spanning tree BPDU flood Mode,

*              it enables BPDU flood on all edge ports. enable(1) - enables

*              BPDU flood Mode on the switch. disable(2) - disables BPDU

*              flood Mode on the switch. The default status is disabled.

*              

*

* @return

*******************************************************************************/

xLibRC_t fpObjSet_SwitchingStpGlobalConfig_StpGlobalCstBpduFlood(void *wap,

                                                                 void *bufp)

{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  xLibU32_t objStpGlobalCstBpduFloodValue;

  FPOBJ_TRACE_ENTER (bufp);



  /* retrieve object: StpBpduFilterDefault */

  owa.rc = xLibBufDataGet (bufp,

                           (xLibU8_t *) & objStpGlobalCstBpduFloodValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)

  {

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpGlobalCstBpduFloodValue, owa.len);





  /* get the value from application */

   owa.l7rc = usmDbDot1sIntfBpduFloodSet(L7_UNIT_CURRENT,L7_ALL_INTERFACES,objStpGlobalCstBpduFloodValue); 
  if (owa.l7rc != L7_SUCCESS)

  {

    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */

    FPOBJ_TRACE_EXIT (bufp, owa);

    return owa.rc;

  }

  FPOBJ_TRACE_VALUE (bufp, &objStpGlobalCstBpduFloodValue,

                     sizeof (objStpGlobalCstBpduFloodValue));



  /* return the object value: StpBpduFilterDefault */

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStpGlobalCstBpduFloodValue,

                           sizeof (objStpGlobalCstBpduFloodValue));

  FPOBJ_TRACE_EXIT (bufp, owa);

  return owa.rc;

}
void fpObj_StpDot1sBridgeId(xLibU8_t *buf, xLibU8_t *temp, xLibU32_t val)

{

  xLibU32_t ct;



  sprintf(buf, "%02x", temp[0]);

  for (ct = 1; ct < val; ct++)

  {

    sprintf(buf, "%s:%02x", buf, temp[ct]);

  }

}





void fpObj_StpDot1sPortId(xLibU8_t * buf, xLibU32_t val)

{

  xLibU32_t val1, val2;



  /*the port ID is formed from the last 4 bytes of interface num in

  hex format with a ':' in the middle*/

  val1 = (val >> 8) & 0x000000ff;

  val2 = val & 0x000000ff;

  sprintf(buf, "%02x:%02x", val1, val2);

}

void fpObj_StpDot1sFormatPortId(xLibU8_t * buf, xLibU16_t val)
{
  memset(buf, 0, sizeof(buf));
  sprintf(buf,"%3u.%-3u", (L7_char8)((val & 0xf000) >> 8), (val & 0xfff));
  return;
}

