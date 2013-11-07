/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseport.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  17 April 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Radha K
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseport_obj.h"
#include "commdefs.h"
#include "usmdb_util_api.h"
#include "dot1s_exports.h"
#include "usmdb_nim_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_dot1s_api.h"
#include "usmdb_policy_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_status.h"

/*******************************************************************************
* @function fpObjGet_baseport_Interface
*
* @purpose Get 'Interface'
*
* @description [Interface] The Interface to be used.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_Interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_baseport_Interface, (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objInterfaceValue = 0;
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                 0, &nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
   owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                               0, objInterfaceValue, &nextObjInterfaceValue);
  }

  if (owa.l7rc != L7_SUCCESS)
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
* @function fpObjGet_baseport_Type
*
* @purpose Get 'Type'
*
* @description [Type] For normal ports this field will be blank. Otherwise the possible values are:Mon - the port is a monitoring port. Look at the Port Monitoring screens for more information.LAG - the port is a member of a Link Aggregation trunk. Look at the LAG screens for more information.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_Type (void *wap, void *bufp)
{
  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeValue;
  L7_SPECIAL_PORT_TYPE_t port_type;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbIfSpecialPortTypeGet (L7_UNIT_CURRENT, keyInterfaceValue, &port_type);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objTypeValue =(xLibU32_t) port_type;
  /* return the object value: Type */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objTypeValue, sizeof (objTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseport_InterfaceType
*
* @purpose Get 'InterfaceType'
*
* @description [Type] The possible values are: Physical port, LAG port, CPU interfcace etc...
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_InterfaceType (void *wap, void *bufp)
{
  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeValue;
  L7_INTF_TYPES_t itype;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbIntfTypeGet(keyInterfaceValue, &itype);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objTypeValue =(xLibU32_t) itype;
  /* return the object value: Type */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objTypeValue, sizeof (objTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjGet_baseport_STPMode
*
* @purpose Get 'STPMode'
*
* @description [STPMode] The Spanning Tree Protocol Administrative Mode for the port or LAG. The possible values are:Enable - select this to enable the Spanning Tree Protocol for this port.Disable - select this to disable the Spanning Tree Protocol for this port.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_STPMode (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSTPModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1sPortStateGet (L7_UNIT_CURRENT, keyInterfaceValue, &objSTPModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: STPMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSTPModeValue, sizeof (objSTPModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_STPMode
*
* @purpose Set 'STPMode'
*
* @description [STPMode] The Spanning Tree Protocol Administrative Mode for the port or LAG. The possible values are:Enable - select this to enable the Spanning Tree Protocol for this port.Disable - select this to disable the Spanning Tree Protocol for this port.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_STPMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSTPModeValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: STPMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objSTPModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objSTPModeValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbDot1sPortStateSet (L7_UNIT_CURRENT, keyInterfaceValue, objSTPModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_AdminMode
*
* @purpose Get 'AdminMode'
*
* @description [AdminMode] The Port control administration state.You must select enable if you want the port to participate in the network.The factory default is enabled.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_AdminMode (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbIfAdminStateGet(L7_UNIT_CURRENT, keyInterfaceValue, &objAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdminModeValue, sizeof (objAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_AdminMode
*
* @purpose Set 'AdminMode'
*
* @description [AdminMode] The Port control administration state.You must select enable if you want the port to participate in the network.The factory default is enabled.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_AdminMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminModeValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbIfAdminStateSet (L7_UNIT_CURRENT, keyInterfaceValue, objAdminModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_BroadcastMode
*
* @purpose Get 'BroadcastMode'
*
* @description [BroadcastMode] Enable or disable this option by selecting the corresponding line on the pulldown entry field.When you specify Enable for Broadcast Storm Recovery and the broadcast traffic on the specified Ethernet port exceeds the configured threshold, the switch blocks (discards) the broadcast traffic. The factory default is disable.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_BroadcastMode (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlBcastStormModeIntfGet(keyInterfaceValue, &objBroadcastModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: BroadcastMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBroadcastModeValue,
                           sizeof (objBroadcastModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_BroadcastMode
*
* @purpose Set 'BroadcastMode'
*
* @description [BroadcastMode] Enable or disable this option by selecting the corresponding line on the pulldown entry field.When you specify Enable for Broadcast Storm Recovery and the broadcast traffic on the specified Ethernet port exceeds the configured threshold, the switch blocks (discards) the broadcast traffic. The factory default is disable.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_BroadcastMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastModeValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: BroadcastMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objBroadcastModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBroadcastModeValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlBcastStormModeIntfSet(keyInterfaceValue, objBroadcastModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_BroadcastLevel
*
* @purpose Get 'BroadcastLevel'
*
* @description [BroadcastLevel] Specify the data rate at which storm control activates. The factory default is 5 percent of port speed. The level units can be set to percent or packets-per-second.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_BroadcastLevel (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastLevelValue;
  xLibU32_t objBroadcastLevelUnitValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlBcastStormThresholdIntfGet(keyInterfaceValue,
                                                      &objBroadcastLevelValue,
                                                      &objBroadcastLevelUnitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: BroadcastLevel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBroadcastLevelValue,
                           sizeof (objBroadcastLevelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_BroadcastLevel
*
* @purpose Set 'BroadcastLevel'
*
* @description [BroadcastLevel] Specify the data rate at which storm control activates.
* The factory default is 5 percent of port speed. The level units can be set to percent or packets-per-second.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_BroadcastLevel (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastLevelValue;

  fpObjWa_t kwaBroadcastLevelUnit = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastLevelUnitValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: BroadcastLevel */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objBroadcastLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBroadcastLevelValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve : BroadcastLevelUnit */
  kwaBroadcastLevelUnit.rc = xLibFilterGet (wap, XOBJ_baseport_BroadcastLevelThresholdUnit,
                                   (xLibU8_t *) &objBroadcastLevelUnitValue , &kwaBroadcastLevelUnit.len);
  if (kwaBroadcastLevelUnit.rc != XLIBRC_SUCCESS)
  {
    kwaBroadcastLevelUnit.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaBroadcastLevelUnit);
    return kwaBroadcastLevelUnit.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objBroadcastLevelUnitValue, kwaBroadcastLevelUnit.len);

  if(objBroadcastLevelUnitValue == L7_RATE_UNIT_PERCENT)
  {
    if((objBroadcastLevelValue < L7_STORMCONTROL_LEVEL_MIN) || (objBroadcastLevelValue > L7_STORMCONTROL_LEVEL_MAX))
    {
      owa.rc = XLIBRC_INVALID_BCAST_STORM_CONTROL_PERCENT;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else if(objBroadcastLevelUnitValue == L7_RATE_UNIT_PPS)
  {
    if((objBroadcastLevelValue < L7_STORMCONTROL_RATE_MIN) || (objBroadcastLevelValue > L7_STORMCONTROL_RATE_MAX))
    {
      owa.rc = XLIBRC_INVALID_BCAST_STORM_CONTROL_VAL;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  owa.l7rc = usmDbSwDevCtrlBcastStormThresholdIntfSet(keyInterfaceValue,
                                                          objBroadcastLevelValue,
                                                          objBroadcastLevelUnitValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_BroadcastLevelUnit
*
* @purpose Get 'BroadcastLevelUnit'
*
* @description [BroadcastLevelUnit] Specify the data rate at which storm control activates.
*               The factory default is 5 percent of port speed.
*               The level units can be set to percent or packets-per-second.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_BroadcastLevelUnit (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastLevelValue;
  xLibU32_t objBroadcastLevelUnitValue;
  xLibS8_t  data[16];
  xLibS8_t  tmp[8];

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlBcastStormThresholdIntfGet(keyInterfaceValue,
                                                      &objBroadcastLevelValue,
                                                      &objBroadcastLevelUnitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  osapiSnprintf(data, sizeof(data), "%d,", objBroadcastLevelValue);
  osapiSnprintf(tmp, sizeof(tmp), "%d", objBroadcastLevelUnitValue);
  osapiStrncat(data,tmp, strlen(tmp));

  /* return the object value: BroadcastLevel */
  owa.rc = xLibBufDataSet (bufp, data, strlen(data));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_BroadcastLevelThresholdUnit
*
* @purpose Get 'BroadcastLevelThresholdUnit'
 *@description  [BroadcastLevelThresholdUnit] Configures the broadcast storm
* recovery threshold unit for this port.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_BroadcastLevelThresholdUnit (void *wap, void *bufp)
{
  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastLevelThresholdUnitValue;
  xLibU32_t objBroadcastLevelThresholdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlBcastStormThresholdIntfGet(keyInterfaceValue,
                                                      &objBroadcastLevelThresholdValue,
                                                      &objBroadcastLevelThresholdUnitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objBroadcastLevelThresholdUnitValue,
                     sizeof (objBroadcastLevelThresholdUnitValue));

  /* return the object value: BroadcastLevelThresholdUnit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBroadcastLevelThresholdUnitValue,
                           sizeof (objBroadcastLevelThresholdUnitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_BroadcastLevelThresholdUnit
*
* @purpose Set 'BroadcastLevelThresholdUnit'
 *@description  [BroadcastLevelThresholdUnit] Configures the broadcast storm
* recovery threshold unit for this port.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_BroadcastLevelThresholdUnit (void *wap, void *bufp)
{
  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastLevelThresholdUnitValue;
  xLibU32_t objBroadcastLevelThresholdValue;
  xLibU32_t tempBroadcastLevelThresholdUnitValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: BroadcastLevelThresholdUnit */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objBroadcastLevelThresholdUnitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBroadcastLevelThresholdUnitValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlBcastStormThresholdIntfGet(keyInterfaceValue,
                                                      &objBroadcastLevelThresholdValue,
                                                      &tempBroadcastLevelThresholdUnitValue);

  if (owa.l7rc == L7_SUCCESS)
  {
    if (tempBroadcastLevelThresholdUnitValue != objBroadcastLevelThresholdUnitValue)
    {
      owa.l7rc = usmDbSwDevCtrlBcastStormThresholdIntfSet(keyInterfaceValue,
                                                          objBroadcastLevelThresholdValue,
                                                          objBroadcastLevelThresholdUnitValue);
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseport_MulticastMode
*
* @purpose Get 'MulticastMode'
*
* @description [MulticastMode] Enable or disable this option by selecting the corresponding line on the pulldown entry field. When you specify Enable for Multicast Storm Recovery and the multicast traffic on the specified Ethernet port exceeds the configured threshold, the switch blocks (discards) the multicast traffic. The factory default is disabled.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_MulticastMode (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlMcastStormModeIntfGet(keyInterfaceValue, &objMulticastModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MulticastMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastModeValue,
                           sizeof (objMulticastModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_MulticastMode
*
* @purpose Set 'MulticastMode'
*
* @description [MulticastMode] Enable or disable this option by selecting the corresponding line on the pulldown entry field. When you specify Enable for Multicast Storm Recovery and the multicast traffic on the specified Ethernet port exceeds the configured threshold, the switch blocks (discards) the multicast traffic. The factory default is disabled.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_MulticastMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastModeValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMulticastModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastModeValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlMcastStormModeIntfSet(keyInterfaceValue, objMulticastModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_MulticastLevel
*
* @purpose Get 'MulticastLevel'
*
* @description [MulticastLevel] Specify the data rate at which storm control activates.
* The factory default is 5 percent of port speed. The level units can be set to percent
* or packets-per-second.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_MulticastLevel (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastLevelValue;
  xLibU32_t objMulticastLevelUnitValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlMcastStormThresholdIntfGet(keyInterfaceValue,
                                                      &objMulticastLevelValue,
                                                      &objMulticastLevelUnitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MulticastLevel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastLevelValue,
                           sizeof (objMulticastLevelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_MulticastLevel
*
* @purpose Set 'MulticastLevel'
*
* @description [MulticastLevel] Specify the data rate at which storm control activates.
* The factory default is 5 percent of port speed. The level units can be set to percent or packets-per-second.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_MulticastLevel (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastLevelValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t kwaMulticastLevelUnit = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastLevelUnitValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastLevel */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMulticastLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastLevelValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve : MulticastLevelUnit */
  kwaMulticastLevelUnit.rc = xLibFilterGet (wap, XOBJ_baseport_MulticastLevelThresholdUnit,
                                   (xLibU8_t *) & objMulticastLevelUnitValue, &kwaMulticastLevelUnit.len);
  if (kwaMulticastLevelUnit.rc != XLIBRC_SUCCESS)
  {
    kwaMulticastLevelUnit.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaMulticastLevelUnit);
    return kwaMulticastLevelUnit.rc;
  }


  FPOBJ_TRACE_CURRENT_KEY (bufp, &objMulticastLevelUnitValue, kwaMulticastLevelUnit.len);
  /* set the value in application */
  if(objMulticastLevelUnitValue == L7_RATE_UNIT_PERCENT)
  {
     if((objMulticastLevelValue < L7_STORMCONTROL_LEVEL_MIN) || (objMulticastLevelValue > L7_STORMCONTROL_LEVEL_MAX))
     {
       owa.rc = XLIBRC_INVALID_MCAST_STORM_CONTROL_PERCENT;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }
  }
  else if(objMulticastLevelUnitValue == L7_RATE_UNIT_PPS)
  {
     if((objMulticastLevelValue < L7_STORMCONTROL_RATE_MIN) || (objMulticastLevelValue > L7_STORMCONTROL_RATE_MAX))
     {
       owa.rc = XLIBRC_INVALID_MCAST_STORM_CONTROL_VAL;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }
  }

  owa.l7rc = usmDbSwDevCtrlMcastStormThresholdIntfSet(keyInterfaceValue,
                                                          objMulticastLevelValue,
                                                          objMulticastLevelUnitValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_MulticastLevelUnit
*
* @purpose Get 'MulticastLevel'
*
* @description [MulticastLevel] Specify the data rate at which storm control activates.
* The factory default is 5 percent of port speed. The level units can be set to percent
* or packets-per-second.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_MulticastLevelUnit (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastLevelValue;
  xLibU32_t objMulticastLevelUnitValue;
  xLibS8_t  data[16];
  xLibS8_t  tmp[8];

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlMcastStormThresholdIntfGet(keyInterfaceValue,
                                                      &objMulticastLevelValue,
                                                      &objMulticastLevelUnitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  osapiSnprintf(data, sizeof(data), "%d,", objMulticastLevelValue);
  osapiSnprintf(tmp, sizeof(tmp), "%d", objMulticastLevelUnitValue);
  osapiStrncat(data,tmp, strlen(tmp));

  /* return the object value: MulticastLevel */
  owa.rc = xLibBufDataSet (bufp, data, strlen(data));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_baseport_MulticastLevelThresholdUnit
*
* @purpose Get 'MulticastLevelThresholdUnit'
 *@description  [MulticastLevelThresholdUnit] Configures the multicast storm
* recovery threshold unit for this port.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_MulticastLevelThresholdUnit (void *wap, void *bufp)
{
  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastLevelThresholdUnitValue;
  xLibU32_t objMulticastLevelThresholdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlMcastStormThresholdIntfGet(keyInterfaceValue,
                                                      &objMulticastLevelThresholdValue,
                                                      &objMulticastLevelThresholdUnitValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objMulticastLevelThresholdUnitValue,
                     sizeof (objMulticastLevelThresholdUnitValue));

  /* return the object value: MulticastLevelThresholdUnit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastLevelThresholdUnitValue,
                           sizeof (objMulticastLevelThresholdUnitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_MulticastLevelThresholdUnit
*
* @purpose Set 'MulticastLevelThresholdUnit'
 *@description  [MulticastLevelThresholdUnit] Configures the multicast storm
* recovery threshold unit for this port.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_MulticastLevelThresholdUnit (void *wap, void *bufp)
{
  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastLevelThresholdUnitValue;
  xLibU32_t tempMulticastLevelThresholdUnitValue;
  xLibU32_t objMulticastLevelThresholdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastLevelThresholdUnit */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMulticastLevelThresholdUnitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastLevelThresholdUnitValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlMcastStormThresholdIntfGet(keyInterfaceValue,
                                                      &objMulticastLevelThresholdValue,
                                                      &tempMulticastLevelThresholdUnitValue);

  if (owa.l7rc == L7_SUCCESS)
  {
    if (tempMulticastLevelThresholdUnitValue != objMulticastLevelThresholdUnitValue)
    {
      owa.l7rc = usmDbSwDevCtrlMcastStormThresholdIntfSet(keyInterfaceValue,
                                                          objMulticastLevelThresholdValue,
                                                          objMulticastLevelThresholdUnitValue);
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseport_UnicastMode
*
* @purpose Get 'UnicastMode'
*
* @description [UnicastMode] Enable or disable this option by selecting the corresponding line on the pulldown entry field. When you specify Enable for Unicast Storm Recovery and the unicast traffic on the specified Ethernet port exceeds the configured threshold, the switch blocks (discards) the unicast traffic. The factory default is disabled.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_UnicastMode (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlUcastStormModeIntfGet(keyInterfaceValue, &objUnicastModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnicastMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnicastModeValue, sizeof (objUnicastModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_UnicastMode
*
* @purpose Set 'UnicastMode'
*
* @description [UnicastMode] Enable or disable this option by selecting the corresponding line on the pulldown entry field. When you specify Enable for Unicast Storm Recovery and the unicast traffic on the specified Ethernet port exceeds the configured threshold, the switch blocks (discards) the unicast traffic. The factory default is disabled.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_UnicastMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastModeValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnicastMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objUnicastModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastModeValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlUcastStormModeIntfSet(keyInterfaceValue, objUnicastModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_UnicastLevel
*
* @purpose Get 'UnicastLevel'
*
* @description [UnicastLevel] Specify the data rate at which storm control
* activates. The factory default is 5 percent of port speed. The level
* units can be set to percent or packets-per-second.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_UnicastLevel (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastLevelValue;
  xLibU32_t objUnicastLevelUnitValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlUcastStormThresholdIntfGet(keyInterfaceValue, &objUnicastLevelValue,&objUnicastLevelUnitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnicastLevel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnicastLevelValue,
                           sizeof (objUnicastLevelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_UnicastLevel
*
* @purpose Set 'UnicastLevel'
*
* @description [UnicastLevel] Specify the data rate at which storm control activates.
* The factory default is 5 percent of port speed. The level units can be set to percent or packets-per-second.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_UnicastLevel (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastLevelValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t kwaUnicastLevelUnit = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastLevelUnitValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnicastLevel */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objUnicastLevelValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastLevelValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve : UnicastLevelUnit */
  kwaUnicastLevelUnit.rc = xLibFilterGet (wap, XOBJ_baseport_UnicastLevelThresholdUnit,
                                   (xLibU8_t *) & objUnicastLevelUnitValue, &kwaUnicastLevelUnit.len);
  if (kwaUnicastLevelUnit.rc != XLIBRC_SUCCESS)
  {
    kwaUnicastLevelUnit.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaUnicastLevelUnit);
    return kwaUnicastLevelUnit.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objUnicastLevelUnitValue, kwaUnicastLevelUnit.len);

  /* set the value in application */
  if(objUnicastLevelUnitValue == L7_RATE_UNIT_PERCENT)
  {
    if((objUnicastLevelValue < L7_STORMCONTROL_LEVEL_MIN) || (objUnicastLevelValue > L7_STORMCONTROL_LEVEL_MAX))
    {
      owa.rc = XLIBRC_INVALID_UCAST_STORM_CONTROL_PERCENT;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else if(objUnicastLevelUnitValue == L7_RATE_UNIT_PPS)
  {
    if((objUnicastLevelValue < L7_STORMCONTROL_RATE_MIN) || (objUnicastLevelValue > L7_STORMCONTROL_RATE_MAX))
    {
      owa.rc = XLIBRC_INVALID_UCAST_STORM_CONTROL_VAL;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  owa.l7rc = usmDbSwDevCtrlUcastStormThresholdIntfSet(keyInterfaceValue,
                                                          objUnicastLevelValue,
                                                          objUnicastLevelUnitValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_UnicastLevelUnit
*
* @purpose Get 'UnicastLevelUnit'
*
* @description [UnicastLevelUnit] Specify the data rate at which storm control
* activates. The factory default is 5 percent of port speed. The level
* units can be set to percent or packets-per-second.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_UnicastLevelUnit (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastLevelValue;
  xLibU32_t objUnicastLevelUnitValue;
  xLibS8_t  data[16];
  xLibS8_t  tmp[8];

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlUcastStormThresholdIntfGet(keyInterfaceValue, &objUnicastLevelValue,&objUnicastLevelUnitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  osapiSnprintf(data, sizeof(data), "%d,", objUnicastLevelValue);
  osapiSnprintf(tmp, sizeof(tmp), "%d", objUnicastLevelUnitValue);
  osapiStrncat(data,tmp, strlen(tmp));

  /* return the object value: UnicastLevel */
  owa.rc = xLibBufDataSet (bufp, data, strlen(data));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_baseport_UnicastLevelThresholdUnit
*
* @purpose Get 'UnicastLevelThresholdUnit'
 *@description  [UnicastLevelThresholdUnit] Configures the unicast storm
* recovery threshold unit for this port.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_UnicastLevelThresholdUnit (void *wap, void *bufp)
{
  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastLevelThresholdUnitValue;
  xLibU32_t objUnicastLevelThresholdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbSwDevCtrlUcastStormThresholdIntfGet(keyInterfaceValue,
                                                      &objUnicastLevelThresholdValue,
                                                      &objUnicastLevelThresholdUnitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objUnicastLevelThresholdUnitValue,
                     sizeof (objUnicastLevelThresholdUnitValue));

  /* return the object value: UnicastLevelThresholdUnit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnicastLevelThresholdUnitValue,
                           sizeof (objUnicastLevelThresholdUnitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_UnicastLevelThresholdUnit
*
* @purpose Set 'UnicastLevelThresholdUnit'
 *@description  [UnicastLevelThresholdUnit] Configures the unicast storm
* recovery threshold unit for this port.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_UnicastLevelThresholdUnit (void *wap, void *bufp)
{
  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastLevelThresholdUnitValue;

  xLibU32_t tempUnicastLevelThresholdUnitValue;
  xLibU32_t objUnicastLevelThresholdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnicastLevelThresholdUnit */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objUnicastLevelThresholdUnitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastLevelThresholdUnitValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbSwDevCtrlUcastStormThresholdIntfGet(keyInterfaceValue,
                                                      &objUnicastLevelThresholdValue,
                                                      &tempUnicastLevelThresholdUnitValue);

  if (owa.l7rc == L7_SUCCESS)
  {
    if (tempUnicastLevelThresholdUnitValue != objUnicastLevelThresholdUnitValue)
    {
      owa.l7rc = usmDbSwDevCtrlUcastStormThresholdIntfSet(keyInterfaceValue,
                                                          objUnicastLevelThresholdValue,
                                                          objUnicastLevelThresholdUnitValue);
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseport_LACPMode
*
* @purpose Get 'LACPMode'
*
* @description [LACPMode] Selects the Link Aggregation Control Protocol administration state. The mode must be enabled in order for the port to participate in Link Aggregation. May be enabled or disabled by selecting the corresponding line on the pulldown entry field. The factory default is enabled.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_LACPMode (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLACPModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDot3adAggPortLacpModeGet(L7_UNIT_CURRENT, keyInterfaceValue, &objLACPModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LACPMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLACPModeValue, sizeof (objLACPModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_LACPMode
*
* @purpose Set 'LACPMode'
*
* @description [LACPMode] Selects the Link Aggregation Control Protocol administration state. The mode must be enabled in order for the port to participate in Link Aggregation. May be enabled or disabled by selecting the corresponding line on the pulldown entry field. The factory default is enabled.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_LACPMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLACPModeValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LACPMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLACPModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLACPModeValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbDot3adAggPortLacpModeSet(L7_UNIT_CURRENT, keyInterfaceValue, objLACPModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_FlowControlMode
*
* @purpose Get 'FlowControlMode'
*
* @description [FlowControlMode] The flow control mode.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_FlowControlMode (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFlowControlModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbIfFlowCtrlModeGet(L7_UNIT_CURRENT, keyInterfaceValue, &objFlowControlModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: FlowControlMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFlowControlModeValue,
                           sizeof (objFlowControlModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_FlowControlMode
*
* @purpose Set 'FlowControlMode'
*
* @description [FlowControlMode] The flow control mode.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_FlowControlMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFlowControlModeValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: FlowControlMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objFlowControlModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objFlowControlModeValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbIfFlowCtrlModeSet(L7_UNIT_CURRENT, keyInterfaceValue, objFlowControlModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_PhysicalMode
*
* @purpose Get 'PhysicalMode'
*
* @description [PhysicalMode] Use the pulldown menu to select the port's speed and duplex mode. If you select auto the duplex mode and speed will be set by the auto-negotiation process. Note that the port's maximum capability (full duplex and 100 Mbps) will be advertised. Otherwise, your selection will determine the port's duplex mode and transmission rate. The factory default is auto. The selection when applied against the All option in Unit/Slot/Port is applied to all applicable interfaces only.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_PhysicalMode (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPhysicalModeValue;
  L7_RC_t rc;
  L7_uint32 autoNeg;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbIfSpeedGet (L7_UNIT_CURRENT, keyInterfaceValue, &objPhysicalModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    objPhysicalModeValue = L7_PORTCTRL_PORTSPEED_HALF_10T;
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  rc = usmDbIfAutoNegAdminStatusGet(L7_UNIT_CURRENT, keyInterfaceValue, &autoNeg);
  if (autoNeg == L7_ENABLE)
  {
    objPhysicalModeValue = L7_PORTCTRL_PORTSPEED_AUTO_NEG;
  }
  /* return the object value: PhysicalMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPhysicalModeValue,
                           sizeof (objPhysicalModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_PhysicalMode
*
* @purpose Set 'PhysicalMode'
*
* @description [PhysicalMode] Use the pulldown menu to select the port's speed and duplex mode. If you select auto the duplex mode and speed will be set by the auto-negotiation process. Note that the port's maximum capability (full duplex and 100 Mbps) will be advertised. Otherwise, your selection will determine the port's duplex mode and transmission rate. The factory default is auto. The selection when applied against the All option in Unit/Slot/Port is applied to all applicable interfaces only.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_PhysicalMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t physMode,physical_mode;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t intIfNum;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PhysicalMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & physMode, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &physMode, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & intIfNum, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &intIfNum, kwaInterface.len);

  /* Check if the physical mode has changed. If not, submit the same mode. */
  if ((usmDbIfAutoNegAdminStatusGet(L7_UNIT_CURRENT, intIfNum, &physical_mode) == L7_SUCCESS) &&
          (physical_mode == physMode))
  {
     owa.l7rc = usmDbIfAutoNegoStatusCapabilitiesSet(intIfNum, physMode ? L7_PORT_NEGO_CAPABILITY_ALL : 0);
  }
  else if ((usmDbIfSpeedGet(L7_UNIT_CURRENT, intIfNum, &physical_mode) == L7_SUCCESS) &&
               (physical_mode == physMode))
  {
    owa.l7rc = usmDbIfSpeedGet(L7_UNIT_CURRENT, intIfNum, &physical_mode);
  }
  else
  {
    xLibU32_t portCapability,lag;

    usmDbIntfPhyCapabilityGet(intIfNum, &portCapability);
    if (usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT, intIfNum, &lag) == L7_SUCCESS)
    {
      owa.rc = XLIBRC_COMMON_PORTCFG_PHYSMODE_LAG_ERROR;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    else
    {
      if (physMode == L7_PORTCTRL_PORTSPEED_AUTO_NEG)
      {
        owa.l7rc = usmDbIfAutoNegoStatusCapabilitiesSet(intIfNum, L7_PORT_NEGO_CAPABILITY_ALL);
      }
      else
      {
        owa.l7rc = usmDbIfAutoNegoStatusCapabilitiesSet(intIfNum, L7_DISABLE);
        if (physMode == L7_PORTCTRL_PORTSPEED_HALF_100TX && (portCapability & L7_PHY_CAP_PORTSPEED_HALF_100))
        {
          owa.l7rc = usmDbIfSpeedSet(L7_UNIT_CURRENT, intIfNum, physMode);
        }
        else if (physMode == L7_PORTCTRL_PORTSPEED_FULL_100TX && (portCapability & L7_PHY_CAP_PORTSPEED_FULL_100))
        {
          owa.l7rc = usmDbIfSpeedSet(L7_UNIT_CURRENT, intIfNum, physMode);
        }
        else if (physMode == L7_PORTCTRL_PORTSPEED_HALF_10T && (portCapability & L7_PHY_CAP_PORTSPEED_HALF_10))
        {
          owa.l7rc = usmDbIfSpeedSet(L7_UNIT_CURRENT, intIfNum, physMode);
        }
        else if (physMode == L7_PORTCTRL_PORTSPEED_FULL_10T && (portCapability & L7_PHY_CAP_PORTSPEED_FULL_10))
        {
          owa.l7rc = usmDbIfSpeedSet(L7_UNIT_CURRENT, intIfNum, physMode);
        }
        /* PTin added: Speed 2.5G */
        else if (physMode == L7_PORTCTRL_PORTSPEED_FULL_2P5FX && (portCapability & L7_PHY_CAP_PORTSPEED_FULL_2500))
        {
          owa.l7rc = usmDbIfSpeedSet(L7_UNIT_CURRENT, intIfNum, physMode);
        }
        /* PTin end */
        else if (physMode == L7_PORTCTRL_PORTSPEED_FULL_10GSX && (portCapability & L7_PHY_CAP_PORTSPEED_FULL_10G))
        {
          owa.l7rc = usmDbIfSpeedSet(L7_UNIT_CURRENT, intIfNum, physMode);
        }
        /* PTin added: Speed 40G */
        else if (physMode == L7_PORTCTRL_PORTSPEED_FULL_40G_KR4 && (portCapability & L7_PHY_CAP_PORTSPEED_FULL_40G))
        {
          owa.l7rc = usmDbIfSpeedSet(L7_UNIT_CURRENT, intIfNum, physMode);
        }
        /* PTin added: Speed 100G */
        else if (physMode == L7_PORTCTRL_PORTSPEED_FULL_100G_BKP && (portCapability & L7_PHY_CAP_PORTSPEED_FULL_100G))
        {
          owa.l7rc = usmDbIfSpeedSet(L7_UNIT_CURRENT, intIfNum, physMode);
        }
        /* PTin end */
        else
        {
          owa.rc = XLIBRC_FAILURE;
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }

  if (owa.l7rc != L7_SUCCESS)
  {
          owa.rc = XLIBRC_FAILURE;
          FPOBJ_TRACE_EXIT (bufp, owa);
          return owa.rc;
        }
      }
    }
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_PhysicalStatus
*
* @purpose Get 'PhysicalStatus'
*
* @description [PhysicalStatus] Indicates the port speed and duplex mode.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_PhysicalStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t objPhysicalStatusValue;

  L7_uint32 portState;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* Some Initial Values */
  objPhysicalStatusValue = L7_PORTCTRL_PORTSPEED_UNKNOWN;
  portState = L7_NULL;
  /* get the value from application */
  owa.l7rc = usmDbIfOperStatusGet(L7_UNIT_CURRENT, keyInterfaceValue, &portState);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(owa.l7rc == L7_SUCCESS )
  {
    if( portState == L7_UP)
    {
      owa.l7rc = usmDbIfHighSpeedGet(L7_UNIT_CURRENT, keyInterfaceValue, &objPhysicalStatusValue);
    }
    else if(portState == L7_DOWN)
    {
      owa.l7rc = L7_SUCCESS;
      objPhysicalStatusValue = L7_PORTCTRL_PORTSPEED_UNKNOWN;
    }
    else
    {
      owa.l7rc = L7_FAILURE;
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  /* return the object value: PhysicalStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objPhysicalStatusValue,
                           sizeof (objPhysicalStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_LinkStatus
*
* @purpose Get 'LinkStatus'
*
* @description [LinkStatus] Indicates whether the Link is up or down.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_LinkStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  L7_uint32 link_state;
  xLibStr256_t objLinkStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbIfOperStatusGet(L7_UNIT_CURRENT, keyInterfaceValue, &link_state);
  if (owa.l7rc != L7_SUCCESS)
  {
    strcpy(objLinkStatusValue,"Unknown");
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if ( link_state == L7_UP )
  {
    strcpy(objLinkStatusValue,"Link Up");
  }
  else
  {
    strcpy(objLinkStatusValue,"Link Down");
  }
  /* return the object value: LinkStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLinkStatusValue, strlen (objLinkStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_LinkTrap
*
* @purpose Get 'LinkTrap'
*
* @description [LinkTrap] This object determines whether or not to send a trap when link status changes. The factory default is enabled.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_LinkTrap (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLinkTrapValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbIfLinkUpDownTrapEnableGet(L7_UNIT_CURRENT, keyInterfaceValue, &objLinkTrapValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LinkTrap */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLinkTrapValue, sizeof (objLinkTrapValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_LinkTrap
*
* @purpose Set 'LinkTrap'
*
* @description [LinkTrap] This object determines whether or not to send a trap when link status changes. The factory default is enabled.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_LinkTrap (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLinkTrapValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LinkTrap */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLinkTrapValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLinkTrapValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbIfLinkUpDownTrapEnableSet(L7_UNIT_CURRENT, keyInterfaceValue, objLinkTrapValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_FrameSize
*
* @purpose Get 'FrameSize'
*
* @description [FrameSize] The maximum ethernet frame size the interface supports or is configured, including ethernet header, CRC, and payload. (1518 to 9216). The default maximum frame size is 1518.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_FrameSize (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFrameSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbIfConfigMaxFrameSizeGet(keyInterfaceValue, &objFrameSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: FrameSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFrameSizeValue, sizeof (objFrameSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_FrameSize
*
* @purpose Set 'FrameSize'
*
* @description [FrameSize] The maximum ethernet frame size the interface supports or is configured, including ethernet header, CRC, and payload. (1518 to 9216). The default maximum frame size is 1518.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_FrameSize (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFrameSizeValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: FrameSize */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objFrameSizeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objFrameSizeValue, owa.len);

  if( usmDbSocIs53115Check(L7_UNIT_CURRENT))
  {

     if(  (objFrameSizeValue !=  L7_MIN_FRAME_SIZE) && (objFrameSizeValue != PLAT_MAX_FRAME_SIZE ))
     {
       owa.rc = XLIBRC_INVALID_VALUE_GENERIC;
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }
  }
  else
  {

     if ( objFrameSizeValue < L7_MIN_FRAME_SIZE || objFrameSizeValue > L7_MAX_FRAME_SIZE)
     {
       owa.rc = XLIBRC_INVALID_VALUE_GENERIC;
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }

  }



  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbIfConfigMaxFrameSizeSet(keyInterfaceValue, objFrameSizeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_ifIndex
*
* @purpose Get 'ifIndex'
*
* @description [ifIndex] The ifIndex of the interface table entry associated with this port.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_ifIndex (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbIfIndexGet(L7_UNIT_CURRENT, keyInterfaceValue, &objifIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifIndexValue, sizeof (objifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_mstid
*
* @purpose Get 'mstid'
*
* @description [mstid] Select the Multiple Spanning Tree instance ID from the list of all currently configured MST ID's to determine the values displayed for the Spanning Tree parameters. Changing the selected MST ID will generate a screen refresh. If Spanning Tree is disabled this will be a static value, CST, instead of a selector.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_mstid (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmstidValue;
  xLibU32_t nextObjmstidValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mstid */
  owa.rc = xLibFilterGet (wap, XOBJ_baseport_mstid, (xLibU8_t *) & objmstidValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmstidValue = 1;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmstidValue, owa.len);
    owa.l7rc = L7_ERROR;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmstidValue, owa.len);

  /* return the object value: mstid */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmstidValue, sizeof (objmstidValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_portRole
*
* @purpose Get 'portRole'
*
* @description [portRole] Each MST Bridge Port that is enabled is assigned a Port Role for each spanning tree. The port role will be one of the following values: Root Port, Designated Port, Alternate Port, Backup Port, Master Port or Disabled Port.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_portRole (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwamstid = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymstidValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objportRoleValue;
  L7_uint32 port_role;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: mstid */
  kwamstid.rc = xLibFilterGet (wap, XOBJ_baseport_mstid,
                               (xLibU8_t *) & keymstidValue, &kwamstid.len);
  if (kwamstid.rc != XLIBRC_SUCCESS)
  {
    kwamstid.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamstid);
    return kwamstid.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymstidValue, kwamstid.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1sMstiPortRoleGet(L7_UNIT_CURRENT, keymstidValue,keyInterfaceValue,&port_role);
  if (owa.l7rc != L7_SUCCESS)
  {
    strcpy(objportRoleValue,"--");
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  switch (port_role)
  {
    case L7_DOT1S_ROLE_DISABLED:
      strcpy(objportRoleValue,"Disabled");
      break;
    case L7_DOT1S_ROLE_ROOT:
      strcpy(objportRoleValue,"Root");
      break;
    case L7_DOT1S_ROLE_DESIGNATED:
      strcpy(objportRoleValue,"Designated");
      break;
    case L7_DOT1S_ROLE_ALTERNATE:
      strcpy(objportRoleValue,"Alternate");
      break;
    case L7_DOT1S_ROLE_BACKUP:
      strcpy(objportRoleValue,"Backup");
      break;
    case L7_DOT1S_ROLE_MASTER:
      strcpy(objportRoleValue,"Master");
      break;
    default:
      strcpy(objportRoleValue,"--");
   }
  /* return the object value: portRole */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objportRoleValue, strlen (objportRoleValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_STPStatus
*
* @purpose Get 'STPStatus'
*
* @description [STPStatus] The port's current state Spanning Tree state. This state controls what action a port takes on receipt of a frame. If the bridge detects a malfunctioning port it will place that port into the broken state. The other five states are defined in IEEE 802.1D: Disabled, Blocking, Listening, Learning, Forwarding, Broken.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_STPStatus (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t kwamstid = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymstidValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSTPStatusValue;
  L7_uint32 mstp_status;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* retrieve key: mstid */
  kwamstid.rc = xLibFilterGet (wap, XOBJ_baseport_mstid,
                               (xLibU8_t *) & keymstidValue, &kwamstid.len);
  if (kwamstid.rc != XLIBRC_SUCCESS)
  {
    kwamstid.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamstid);
    return kwamstid.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymstidValue, kwamstid.len);

  /* get the value from application */
  owa.l7rc = usmDbDot1sMstiPortForwardingStateGet(L7_UNIT_CURRENT, keymstidValue,keyInterfaceValue,&mstp_status);
  if (owa.l7rc != L7_SUCCESS)
  {
    strcpy(objSTPStatusValue,"--");
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  switch (mstp_status)
  {
    case L7_DOT1S_DISCARDING:
      strcpy(objSTPStatusValue,"Discarding");
      break;

    case L7_DOT1S_LEARNING:
      strcpy(objSTPStatusValue,"Learning");
      break;

    case L7_DOT1S_FORWARDING:
      strcpy(objSTPStatusValue,"Forwarding");
      break;

    case L7_DOT1S_DISABLED:
      strcpy(objSTPStatusValue,"Disabled");
      break;

    case L7_DOT1S_MANUAL_FWD:
      strcpy(objSTPStatusValue,"Manual Forward");
      break;

    default:
      strcpy(objSTPStatusValue,"--");
  }
  /* return the object value: STPStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSTPStatusValue, strlen (objSTPStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseport_IntfSpeedSimplexDataRate
*
* @purpose Get 'IntfSpeedSimplexDataRate'
 *@description  [IntfSpeedSimplexDataRate] Gets the physical link data rate of
* the specified interface.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_IntfSpeedSimplexDataRate (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIntfSpeedSimplexDataRateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbIfSpeedSimplexDataRateGet(L7_UNIT_CURRENT, keyInterfaceValue,
                              &objIntfSpeedSimplexDataRateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IntfSpeedSimplexDataRate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntfSpeedSimplexDataRateValue,
                           sizeof (objIntfSpeedSimplexDataRateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_activeState
*
* @purpose Get 'activeState'
 *@description  [activeState] Gets the active state of the specified interface.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_activeState (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objactiveStateValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbIfActiveStateGet(L7_UNIT_CURRENT, keyInterfaceValue, &objactiveStateValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: activeState */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objactiveStateValue, sizeof (objactiveStateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_Capabilities
*
* @purpose Get 'Capabilities'
 *@description  [Capabilities] ToDO: Add Help
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_Capabilities (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCapabilitiesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbIntfPhyCapabilityGet ( keyInterfaceValue, &objCapabilitiesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Capabilities */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCapabilitiesValue,
                           sizeof (objCapabilitiesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseport_Duplex
*
* @purpose Get 'Duplex'
 *@description  [Duplex] Gets the physical link duplex state.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_Duplex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDuplexValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseport_Interface, (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIsIntfSpeedFullDuplex(L7_UNIT_CURRENT, keyInterfaceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDuplexValue, sizeof (objDuplexValue));

  /* return the object value: Duplex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDuplexValue, sizeof (objDuplexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_lagMember
*
* @purpose Get 'LAG info'
 *@description   Gets the lag it is the member
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_lagMember (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibStr256_t objLagMemberShipValue;

  xLibU32_t keyInterfaceValue,val=0;
  xLibU8_t  name[L7_LAG_NAME_SIZE+1];
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseport_Interface, (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  memset(objLagMemberShipValue, 0, sizeof(xLibStr256_t));
  memset(name , 0, sizeof(name));
  if (usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT, keyInterfaceValue, &val) == L7_SUCCESS)
  {
    usmDbDot3adNameGet(L7_UNIT_CURRENT, val, name);
    if ( usmDbDot3adIsActiveMember(L7_UNIT_CURRENT,keyInterfaceValue) == L7_SUCCESS )
    {
      strcat(name," : Active");
    }
    else
    {
      strcat(name," : Inactive");
    }
  }
  else
  {
    strcat(objLagMemberShipValue,"not a lag member");
  }

  owa.len = strlen  (name);
  memcpy(objLagMemberShipValue,name,strlen(name));

  FPOBJ_TRACE_VALUE (bufp, objLagMemberShipValue, strlen(objLagMemberShipValue));

  /* return the object value: Duplex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLagMemberShipValue, strlen (objLagMemberShipValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_baseport_AutoNegStatus
*
* @purpose Get 'AutoNegStatus'
 *@description   Gets the AutoNeg Status for a physical port.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_AutoNegStatus (void *wap, void *bufp)
{
  xLibU32_t portIntf;
  L7_uint32 autoNegStatus;
  xLibU16_t intfLen=sizeof(portIntf);

  if(XLIBRC_SUCCESS == xLibFilterGet (wap,XOBJ_baseport_Interface,(xLibU8_t *)&portIntf,&intfLen))
  {
    if( usmDbIfAutoNegAdminStatusGet (0,portIntf,&autoNegStatus) != L7_SUCCESS)
    {
      return XLIBRC_FAILURE;
    }
    else
    {
      xLibBufDataSet(bufp,(xLibU8_t *) &autoNegStatus , sizeof(autoNegStatus));
    }
  }
  else
  {
    return XLIBRC_FAILURE;
  }
  return XLIBRC_SUCCESS;
}



/*******************************************************************************
* @function fpObjGet_baseport_MemberOfLAG
*
* @purpose Get 'MemberOfLAG'
*
* @description [LACPMode] Selects the Link Aggregation Control Protocol administration state. The mode must be enabled in order for the port to participate in Link Aggregation. May be enabled or disabled by selecting the corresponding line on the pulldown entry field. The factory default is enabled.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_MemberOfLAG (void *wap, void *bufp)
{

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMemberOfLAGValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* get the value from application */
  owa.l7rc = usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT,keyInterfaceValue,&objMemberOfLAGValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LACPMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMemberOfLAGValue, sizeof (objMemberOfLAGValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_baseport_MemberOfLAG
*
* @purpose Set 'MemberOfLAG'
*
* @description [MemberOfLAG] Selects the Link Aggregation Control Protocol administration state. The mode must be enabled in order for the port to participate in Link Aggregation. May be enabled or disabled by selecting the corresponding line on the pulldown entry field. The factory default is enabled.
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_MemberOfLAG (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMemberOfLAGValue;
  xLibU32_t currentLagValue;

  fpObjWa_t kwaInterface = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LACPMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMemberOfLAGValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMemberOfLAGValue, owa.len);

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  /* set the value in application */
  owa.l7rc = usmDbDot3adIntfIsMemberGet(L7_UNIT_CURRENT, keyInterfaceValue, (L7_uint32*) &currentLagValue);
  if ( owa.l7rc == L7_SUCCESS)
  {
    if ( currentLagValue != objMemberOfLAGValue )
    {
       owa.l7rc = usmDbDot3adMemberDeleteSet(L7_UNIT_CURRENT, currentLagValue, keyInterfaceValue);
       if ( objMemberOfLAGValue == 1 )
       {
          owa.l7rc = L7_SUCCESS;
       }
       else
       {
          owa.l7rc = L7_FAILURE;
       }
    }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    if ( objMemberOfLAGValue == 1 )
    {
          owa.l7rc = L7_SUCCESS;
    }
    else
    {
       owa.l7rc = usmDbDot3adMemberAddSet(L7_UNIT_CURRENT, objMemberOfLAGValue, keyInterfaceValue);
    }
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/********************************************************************************
*  @function fpObjGet_baseport_PhysicalInftSpeed
*
* @purpose Get 'PhysicalInftSpeed'
* @description  [PhysicalInftSpeed] ToDO: Add Help
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_PhysicalInftSpeed (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objPhysicalInftSpeedValue,autoNeg,portState;

  xLibU32_t keyInterfaceValue;
  xLibU32_t phys_state;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: Interface */
  owa.len = sizeof(keyInterfaceValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseport_Interface,(xLibU8_t *) &keyInterfaceValue,&owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyInterfaceValue,owa.len);

  /* Some Initial Value */
  objPhysicalInftSpeedValue = L7_SPEED_UNKN;
  /* get the value from application */
  owa.l7rc = usmDbIfOperStatusGet(L7_UNIT_CURRENT, keyInterfaceValue, &portState);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbIfAutoNegAdminStatusGet(L7_UNIT_CURRENT, keyInterfaceValue, &autoNeg);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if ((autoNeg == L7_ENABLE) &&(portState == L7_DOWN))
  {
    objPhysicalInftSpeedValue = L7_SPEED_UNKN;
  }
  else
  {
    owa.l7rc = usmDbIfHighSpeedGet(L7_UNIT_CURRENT, keyInterfaceValue, &phys_state);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    switch (phys_state)
    {
        case L7_PORTCTRL_PORTSPEED_HALF_100TX:
        case L7_PORTCTRL_PORTSPEED_FULL_100TX:
        case L7_PORTCTRL_PORTSPEED_FULL_100FX:
           objPhysicalInftSpeedValue = L7_SPEED_100;
           break;
        case L7_PORTCTRL_PORTSPEED_HALF_10T:
        case L7_PORTCTRL_PORTSPEED_FULL_10T:
           objPhysicalInftSpeedValue = L7_SPEED_10;
           break;
        case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
           objPhysicalInftSpeedValue = L7_SPEED_1000;
           break;
        /* PTin added: Speed 2.5G */
        case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
           objPhysicalInftSpeedValue = L7_SPEED_2500;
           break;
        /* PTin added: Speed 10G */
        case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
           objPhysicalInftSpeedValue = L7_SPEED_10G;
           break;
        /* PTin added: Speed 40G */
        case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
           objPhysicalInftSpeedValue = L7_SPEED_40G;
           break;
        /* PTin added: Speed 100G */
        case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
           objPhysicalInftSpeedValue = L7_SPEED_100G;
           break;
        /* PTin end */
        case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
           objPhysicalInftSpeedValue = L7_SPEED_AUTO;
           break;
        default:
           objPhysicalInftSpeedValue = L7_SPEED_UNKN;                /*Unknown*/
           break;
    }
  }
  FPOBJ_TRACE_VALUE (bufp, &objPhysicalInftSpeedValue, sizeof(objPhysicalInftSpeedValue));

  /* return the object value: PhysicalInftSpeed */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objPhysicalInftSpeedValue,
                      sizeof(objPhysicalInftSpeedValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseport_PhysicalInftSpeed
*
* @purpose Set 'PhysicalInftSpeed'
 *@description  [PhysicalInftSpeed] ToDO: Add Help
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_PhysicalInftSpeed (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objPhysicalInftSpeedValue;
  xLibU32_t objPhysicalIntfDuplexValue;
  xLibU32_t speedAndDuplexValue;
  xLibU32_t phys_state;
  xLibU32_t portState;
  xLibU32_t autoNeg;
  xLibU32_t keyInterfaceValue;
  speedAndDuplexValue = 0;
  phys_state = 0;
  portState = 0;
  autoNeg = 0;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve object: PhysicalInftSpeed */
  owa.len = sizeof(objPhysicalInftSpeedValue);
  owa.rc = xLibBufDataGet(bufp,(xLibU8_t *)&objPhysicalInftSpeedValue,&owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPhysicalInftSpeedValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof(keyInterfaceValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseport_Interface,(xLibU8_t *) &keyInterfaceValue,&owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyInterfaceValue, owa.len);
  /* retrieve duplex also as like key: Duplex */
  owa.len = sizeof(objPhysicalIntfDuplexValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseport_PhysicalIntfDuplex,(xLibU8_t *) &objPhysicalIntfDuplexValue,&owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    /************* Not a problem;Simply get the previous configurd Duplex that it **********/
     /* get the value from application
      * all this pain need to be taken as to extract
      * just the only preconfigurd duplex value
      */
      owa.l7rc = usmDbIfOperStatusGet(L7_UNIT_CURRENT, keyInterfaceValue, &portState);
      if (owa.l7rc != L7_SUCCESS)
      {
         owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
      }

      owa.l7rc = usmDbIfAutoNegAdminStatusGet(L7_UNIT_CURRENT, keyInterfaceValue, &autoNeg);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

      if (autoNeg == L7_ENABLE)
      {
        objPhysicalIntfDuplexValue = L7_DUPLEX_AUTO;
      }
      if(autoNeg != L7_ENABLE)
      {
        owa.l7rc = usmDbIfHighSpeedGet(L7_UNIT_CURRENT, keyInterfaceValue, &phys_state);
        if ( owa.l7rc == L7_SUCCESS )
        {
          switch (phys_state)
          {
           case L7_PORTCTRL_PORTSPEED_HALF_100TX:
           case L7_PORTCTRL_PORTSPEED_HALF_10T:
              objPhysicalIntfDuplexValue = L7_DUPLEX_HALF;
              break;
           case L7_PORTCTRL_PORTSPEED_FULL_100TX:
           case L7_PORTCTRL_PORTSPEED_FULL_100FX:
           case L7_PORTCTRL_PORTSPEED_FULL_10T:
           case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
           case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:             /* PTin added: Speed 2.5G */
           case L7_PORTCTRL_PORTSPEED_FULL_10GSX:             /* PTin added: Speed 10G */
           case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:           /* PTin added: Speed 40G */
           case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:          /* PTin added: Speed 100G */
                objPhysicalIntfDuplexValue = L7_DUPLEX_FULL;
                break;
           case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
                objPhysicalIntfDuplexValue = L7_DUPLEX_AUTO;
           break;
           default:
                objPhysicalIntfDuplexValue = L7_DUPLEX_UNKN;                /*Unknown*/
                break;
          }
       }
      }
  }
  if((objPhysicalInftSpeedValue == L7_SPEED_AUTO) || (objPhysicalIntfDuplexValue == L7_DUPLEX_AUTO))
    owa.l7rc = usmDbIfAutoNegoStatusCapabilitiesSet(keyInterfaceValue, L7_PORT_NEGO_CAPABILITY_ALL);

  else if((objPhysicalInftSpeedValue == L7_SPEED_10) && (objPhysicalIntfDuplexValue == L7_DUPLEX_HALF))
    speedAndDuplexValue = L7_PORTCTRL_PORTSPEED_HALF_10T;

  else if((objPhysicalInftSpeedValue == L7_SPEED_10) && (objPhysicalIntfDuplexValue == L7_DUPLEX_FULL))
    speedAndDuplexValue = L7_PORTCTRL_PORTSPEED_FULL_10T;
  else if((objPhysicalInftSpeedValue == L7_SPEED_100) && (objPhysicalIntfDuplexValue == L7_DUPLEX_HALF))
   speedAndDuplexValue = L7_PORTCTRL_PORTSPEED_HALF_100TX;

  else if((objPhysicalInftSpeedValue == L7_SPEED_100) && (objPhysicalIntfDuplexValue == L7_DUPLEX_FULL))
   speedAndDuplexValue = L7_PORTCTRL_PORTSPEED_FULL_100TX;
  else if((objPhysicalInftSpeedValue == L7_SPEED_1000) && (objPhysicalIntfDuplexValue == L7_DUPLEX_FULL))
   speedAndDuplexValue = L7_PORTCTRL_PORTSPEED_FULL_1000SX;
  /* PTin added: Speed 2.5G */
  else if((objPhysicalInftSpeedValue == L7_SPEED_2500) && (objPhysicalIntfDuplexValue == L7_DUPLEX_FULL))
   speedAndDuplexValue = L7_PORTCTRL_PORTSPEED_FULL_2P5FX;
  /* PTin added: Speed 10G */
  else if((objPhysicalInftSpeedValue == L7_SPEED_10G) && (objPhysicalIntfDuplexValue == L7_DUPLEX_FULL))
   speedAndDuplexValue = L7_PORTCTRL_PORTSPEED_FULL_10GSX;
  /* PTin added: Speed 40G */
  else if((objPhysicalInftSpeedValue == L7_SPEED_40G) && (objPhysicalIntfDuplexValue == L7_DUPLEX_FULL))
   speedAndDuplexValue = L7_PORTCTRL_PORTSPEED_FULL_40G_KR4;
  /* PTin added: Speed 100G */
  else if((objPhysicalInftSpeedValue == L7_SPEED_100G) && (objPhysicalIntfDuplexValue == L7_DUPLEX_FULL))
   speedAndDuplexValue = L7_PORTCTRL_PORTSPEED_FULL_100G_BKP;
  /* PTin end */
  else
   speedAndDuplexValue = L7_PORTCTRL_PORTSPEED_HALF_10T;

  /* set the value in application */
  if((objPhysicalInftSpeedValue != L7_SPEED_AUTO) || (objPhysicalIntfDuplexValue != L7_DUPLEX_AUTO))
  {
    owa.l7rc = usmDbIfAutoNegoStatusCapabilitiesSet(keyInterfaceValue, L7_DISABLE);
    if(owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    owa.l7rc = usmDbIfSpeedSet (L7_UNIT_CURRENT, keyInterfaceValue,speedAndDuplexValue);
  }
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE; /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseport_PhysicalIntfDuplex
*
* @purpose Get 'PhysicalIntfDuplex'
 *@description  [PhysicalIntfDuplex] ToDO: Add Help
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_PhysicalIntfDuplex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objPhysicalIntfDuplexValue,portState,autoNeg;
  xLibU32_t phys_state;
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: Interface */
  owa.len = sizeof(keyInterfaceValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseport_Interface,(xLibU8_t *) &keyInterfaceValue,&owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyInterfaceValue,owa.len);

  /* Some Initial Value */
  objPhysicalIntfDuplexValue = L7_DUPLEX_UNKN;

  /* get the value from application */
  owa.l7rc = usmDbIfOperStatusGet(L7_UNIT_CURRENT, keyInterfaceValue, &portState);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
    owa.l7rc = usmDbIfAutoNegAdminStatusGet(L7_UNIT_CURRENT, keyInterfaceValue, &autoNeg);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if ((autoNeg == L7_ENABLE) &&(portState == L7_DOWN))
    {
      objPhysicalIntfDuplexValue = L7_DUPLEX_UNKN;
    }
    else
    {
      owa.l7rc = usmDbIfHighSpeedGet(L7_UNIT_CURRENT, keyInterfaceValue, &phys_state);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
      switch (phys_state)
      {
        case L7_PORTCTRL_PORTSPEED_HALF_100TX:
        case L7_PORTCTRL_PORTSPEED_HALF_10T:
           objPhysicalIntfDuplexValue = L7_DUPLEX_HALF;
           break;
        case L7_PORTCTRL_PORTSPEED_FULL_100TX:
        case L7_PORTCTRL_PORTSPEED_FULL_100FX:
        case L7_PORTCTRL_PORTSPEED_FULL_10T:
        case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
        case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:              /* PTin added: Speed 2.5G */
        case L7_PORTCTRL_PORTSPEED_FULL_10GSX:              /* PTin added: Speed 10G */
        case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:            /* PTin added: Speed 40G */
        case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:           /* PTin added: Speed 100G */
             objPhysicalIntfDuplexValue = L7_DUPLEX_FULL;
             break;
        case L7_PORTCTRL_PORTSPEED_AUTO_NEG:
             objPhysicalIntfDuplexValue = L7_DUPLEX_AUTO;
           break;
        default:
             objPhysicalIntfDuplexValue = L7_DUPLEX_UNKN;                /*Unknown*/
          break;
      }
    }
  FPOBJ_TRACE_VALUE (bufp, &objPhysicalIntfDuplexValue, sizeof(objPhysicalIntfDuplexValue));

  /* return the object value: PhysicalIntfDuplex */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objPhysicalIntfDuplexValue,
                      sizeof(objPhysicalIntfDuplexValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseport_PhysicalIntfDuplex
*
* @purpose Set 'PhysicalIntfDuplex'
 *@description  [PhysicalIntfDuplex] ToDO: Add Help
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_PhysicalIntfDuplex (void *wap, void *bufp)
{

   fpObjWa_t owa = FPOBJ_INIT_WA2();
   xLibU32_t keyInterfaceValue;

    FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: Interface */
  owa.len = sizeof(keyInterfaceValue);
  owa.l7rc = L7_SUCCESS;
  owa.rc = XLIBRC_SUCCESS;
   /* This routine is modified as accoring to the requirements of
    * sepearate duplex and speed settings config. This is made as a dummy
    * routine assuming that both duplex and speed settings will be present in a
    * single page for config
    */
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseport_PortAdminAdvertisement
*
* @purpose Get 'PortAdminAdvertisement'
 *@description  [PortAdminAdvertisement] ToDO: Add Help
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_PortAdminAdvertisement (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objPortAdminAdvertisementValue;
  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: Interface */
  owa.len = sizeof(keyInterfaceValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseport_Interface,(xLibU8_t *) &keyInterfaceValue,&owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyInterfaceValue,owa.len);

  owa.l7rc = usmDbIfAutoNegoStatusCapabilitiesGet(keyInterfaceValue,&objPortAdminAdvertisementValue);

  if (owa.l7rc != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPortAdminAdvertisementValue, sizeof(objPortAdminAdvertisementValue));

  /* return the object value: PhysicalIntfDuplex */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objPortAdminAdvertisementValue,
                      sizeof(objPortAdminAdvertisementValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseport_PortAdminAdvertisement
*
* @purpose Get 'PortAdminAdvertisement'
 *@description  [PortAdminAdvertisement] ToDO: Add Help
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_PortAdminAdvertisement (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objPortAdminAdvertisementValue;
  xLibU32_t keyInterfaceValue;
  xLibU32_t keyAutoNegStatusValue;

  FPOBJ_TRACE_ENTER(bufp);


 /* retrieve object: PhysicalAdvertisement */
  owa.len = sizeof(objPortAdminAdvertisementValue);
  owa.rc = xLibBufDataGet(bufp,(xLibU8_t *)&objPortAdminAdvertisementValue,&owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPortAdminAdvertisementValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof(keyInterfaceValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseport_Interface,(xLibU8_t *) &keyInterfaceValue,&owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }
  
 FPOBJ_TRACE_CURRENT_KEY(bufp, &keyInterfaceValue,owa.len);
 
  owa.l7rc = usmDbIfAutoNegAdminStatusGet(L7_UNIT_CURRENT,keyInterfaceValue,&keyAutoNegStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  /* If the current neg is disable then set admin Advertisement to max */
  if((keyAutoNegStatusValue != L7_ENABLE) &&(objPortAdminAdvertisementValue == 0))
  {
     objPortAdminAdvertisementValue = L7_PORT_NEGO_CAPABILITY_ALL;

  }

  owa.l7rc = usmDbIfAutoNegoStatusCapabilitiesSet(keyInterfaceValue,objPortAdminAdvertisementValue);
  if (owa.l7rc != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  /* return the object value: PhysicalIntfDuplex */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objPortAdminAdvertisementValue,
                      sizeof(objPortAdminAdvertisementValue));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseport_AutoNegotiation
*
* @purpose Get 'AutoNegotiation'
*
* @description [AutoNegotiation]: TO DO
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_AutoNegotiation (void *wap,void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAutoNegStatusValue;
  xLibU32_t keyInterfaceValue;
  xLibU32_t tempAutoNegStatusValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof(keyInterfaceValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseport_Interface,(xLibU8_t *) &keyInterfaceValue,&owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyInterfaceValue,owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfAutoNegoStatusCapabilitiesGet(keyInterfaceValue, &tempAutoNegStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &tempAutoNegStatusValue,
                     sizeof (tempAutoNegStatusValue));

  /* Set the value in accordance with the enum declared in object */
  if(tempAutoNegStatusValue == L7_DISABLE)
  {
    objAutoNegStatusValue = L7_DISABLE;
  }
  else
  {
     objAutoNegStatusValue = L7_ENABLE;
  }
  /* return the object value: GlobalAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAutoNegStatusValue,
                           sizeof (objAutoNegStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseport_AutoNegotiation
*
* @purpose Set 'AutoNegotiation'
*
* @description [AutoNegotiation]: TO DO
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseport_AutoNegotiation (void *wap,void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAutoNegStatusValue;
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof(keyInterfaceValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseport_Interface,(xLibU8_t *) &keyInterfaceValue,&owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyInterfaceValue,owa.len);

  /* retrieve object: objAutoNegStatusValue */
  owa.rc = xLibBufDataGet (bufp,(xLibU8_t *) & objAutoNegStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAutoNegStatusValue, owa.len);

  if(objAutoNegStatusValue == L7_ENABLE)
  {
    owa.l7rc = usmDbIfAutoNegoStatusCapabilitiesSet(keyInterfaceValue,L7_PORT_NEGO_CAPABILITY_ALL);
  }
  else
  {
    owa.l7rc = usmDbIfAutoNegoStatusCapabilitiesSet(keyInterfaceValue,L7_DISABLE);
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
* @function fpObjGet_baseport_PortConnectorType
*
* @purpose Get 'PortConnectorType', if the port combo then checks for capability and returns is Fiber / Copper
 *@description  [PortConnectorType] ToDO: Add Help
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_PortConnectorType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t objPortConnectorType;
  xLibU32_t keyInterfaceValue;
  xLibU32_t objCapabilitiesValue;
  L7_BOOL sfpLinkUp;

  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: Interface */
  owa.len = sizeof(keyInterfaceValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseport_Interface,(xLibU8_t *) &keyInterfaceValue,&owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  objPortConnectorType = L7_CONNECTOR_NONE;
  FPOBJ_TRACE_CURRENT_KEY(bufp, &keyInterfaceValue,owa.len);
  owa.l7rc = usmDbIntfConnectorTypeGet(keyInterfaceValue,&objPortConnectorType);

  if (owa.l7rc != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbIntfPhyCapabilityGet ( keyInterfaceValue, &objCapabilitiesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  if(objCapabilitiesValue & L7_PHY_CAP_PORTSPEED_SFP)
  {
    if (usmDbStatusSFPLinkUpGet(0, keyInterfaceValue, &sfpLinkUp) == L7_SUCCESS)
    {
      if(sfpLinkUp==L7_TRUE)
      {
        objPortConnectorType = L7_MTRJ;
      }
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objPortConnectorType, sizeof(objPortConnectorType));

  /* return the object value: objPortConnectorType */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&objPortConnectorType,
                      sizeof(objPortConnectorType));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseport_TransceiverFirmwarePartNumber
*
* @purpose Get 'TransceiverFirmwarePartNumber'
*
* @description [TransceiverFirmwarePartNumber]
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_TransceiverFirmwarePartNumber (void *wap, void *bufp)
{
  fpObjWa_t kwaInterface      = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue = L7_NULL;

  fpObjWa_t    owa            = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTransceiverFirmwarePartNumberValue;
  xLibStr256_t objTransceiverFirmwareRevisionValue;

  L7_uint32 unit              = L7_NULL;
  L7_uint32 slot              = L7_NULL;
  L7_uint32 port              = L7_NULL;
  L7_uint32 numOfPorts        = L7_NULL;
#if L7_FEAT_SF10GBT
  L7_uint32 val               = L7_NULL;
#endif /*L7_FEAT_SF10GBT*/

  FPOBJ_TRACE_ENTER (bufp);

  memset(objTransceiverFirmwarePartNumberValue, 0x00, sizeof(xLibStr256_t));
  memset(objTransceiverFirmwareRevisionValue, 0x00, sizeof(xLibStr256_t));

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  if (usmDbUnitSlotPortGet(keyInterfaceValue,&unit,&slot,&port) == L7_SUCCESS)
  {
    usmDbPortsPerBoxGet(unit,&numOfPorts);

#if L7_FEAT_SF10GBT
    if ( (usmDbIfTypeGet( unit, keyInterfaceValue, &val) == L7_SUCCESS)
          && (val == L7_IANA_10G_ETHERNET)
          && ((port == numOfPorts - 1) || (port == numOfPorts)) )
    {
      /* get the value from application */
      owa.l7rc = usmDbIfTransceiverFwRevisionStrGet(unit, keyInterfaceValue, objTransceiverFirmwarePartNumberValue, objTransceiverFirmwareRevisionValue);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
#else /* L7_FEAT_SF10GBT */
    {
      strcpy(objTransceiverFirmwarePartNumberValue, "N/A");
      owa.rc = XLIBRC_SUCCESS;
    }
#endif /* L7_FEAT_SF10GBT */
  }

  /* return the object value: TransceiverFirmwarePartNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTransceiverFirmwarePartNumberValue,
                           strlen (objTransceiverFirmwarePartNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseport_TransceiverFirmwareRevision
*
* @purpose Get 'TransceiverFirmwareRevision'
*
* @description [TransceiverFirmwareRevision]
*
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_TransceiverFirmwareRevision (void *wap, void *bufp)
{
  fpObjWa_t kwaInterface      = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue = L7_NULL;

  fpObjWa_t    owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTransceiverFirmwareRevisionValue;
  xLibStr256_t objTransceiverFirmwarePartNumberValue;

  L7_uint32 unit       = L7_NULL;
  L7_uint32 slot       = L7_NULL;
  L7_uint32 port       = L7_NULL;
  L7_uint32 numOfPorts = L7_NULL;
#if L7_FEAT_SF10GBT
  L7_uint32 val               = L7_NULL;
#endif /*L7_FEAT_SF10GBT*/

  FPOBJ_TRACE_ENTER (bufp);

  memset(objTransceiverFirmwarePartNumberValue, 0x00, sizeof(xLibStr256_t));
  memset(objTransceiverFirmwareRevisionValue, 0x00, sizeof(xLibStr256_t));

  /* retrieve key: Interface */
  kwaInterface.rc = xLibFilterGet (wap, XOBJ_baseport_Interface,
                                   (xLibU8_t *) & keyInterfaceValue, &kwaInterface.len);
  if (kwaInterface.rc != XLIBRC_SUCCESS)
  {
    kwaInterface.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaInterface);
    return kwaInterface.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwaInterface.len);

  if (usmDbUnitSlotPortGet(keyInterfaceValue,&unit,&slot,&port) == L7_SUCCESS)
  {
    usmDbPortsPerBoxGet(unit,&numOfPorts);
#if L7_FEAT_SF10GBT
    if ( (usmDbIfTypeGet( unit, keyInterfaceValue, &val) == L7_SUCCESS)
          && (val == L7_IANA_10G_ETHERNET)
          && ((port == numOfPorts - 1) || (port == numOfPorts)) )
    {
      /* get the value from application */
      owa.l7rc = usmDbIfTransceiverFwRevisionStrGet(unit, keyInterfaceValue, objTransceiverFirmwarePartNumberValue, objTransceiverFirmwareRevisionValue);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }
#else /* L7_FEAT_SF10GBT */
    {
      owa.rc = XLIBRC_SUCCESS;
      strcpy(objTransceiverFirmwareRevisionValue, "N/A");
    }
#endif /* L7_FEAT_SF10GBT */
  }

  /* return the object value: TransceiverFirmwareRevision */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTransceiverFirmwareRevisionValue,
                           strlen (objTransceiverFirmwareRevisionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_baseport_currentFlowControl
*
* @purpose Get 'currentFlowControl', if the port combo then checks for capability and returns is Fiber / Copper
 *@description  [currentFlowControl] ToDO: Add Help  
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseport_currentFlowControl (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2();
  xLibU32_t currentFlowControl;
  xLibU32_t keyInterfaceValue;
  xLibU32_t objTxPauseStatus;
  xLibU32_t objRxPauseStatus;
  
  FPOBJ_TRACE_ENTER(bufp);

  /* retrieve key: Interface */
  owa.len = sizeof(keyInterfaceValue);
  owa.rc = xLibFilterGet(wap, XOBJ_baseport_Interface,(xLibU8_t *) &keyInterfaceValue,&owa.len);
  if(owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING; 
    FPOBJ_TRACE_EXIT(bufp, owa);
    return owa.rc;
  }

  /* get the value from application */
  owa.l7rc = usmDbIfPauseStatusGet (L7_UNIT_CURRENT,keyInterfaceValue, &objTxPauseStatus,&objRxPauseStatus);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  switch(objRxPauseStatus | objTxPauseStatus)
  {
      case L7_TRUE:
           currentFlowControl = L7_ENABLE;
          break;

      case L7_FALSE:
           currentFlowControl = L7_DISABLE;
        break;

      default:
           currentFlowControl = L7_DISABLE;
  }
 
  FPOBJ_TRACE_VALUE (bufp, &currentFlowControl, sizeof(currentFlowControl));

  /* return the object value: currentFlowControl */
  owa.rc = xLibBufDataSet(bufp, (xLibU8_t *)&currentFlowControl,
                      sizeof(currentFlowControl));
  FPOBJ_TRACE_EXIT(bufp, owa);
  return owa.rc;
}

