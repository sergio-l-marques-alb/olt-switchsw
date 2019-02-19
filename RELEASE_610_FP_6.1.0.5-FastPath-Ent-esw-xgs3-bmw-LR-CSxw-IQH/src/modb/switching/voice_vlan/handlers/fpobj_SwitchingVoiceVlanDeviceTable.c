
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingVoiceVlanDeviceTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  24 September 2008, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingVoiceVlanDeviceTable_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_voice_vlan_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanDeviceTable_Interface
*
* @purpose Get 'Interface'
 *@description  [Interface] The value of ifIndex for the interface used to reach
* this PIM neighbor.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanDeviceTable_Interface (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (objInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanDeviceTable_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_U32 (objInterfaceValue);
    owa.l7rc = usmDbGetNextPhysicalIntIfNumber(0,&nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc = usmDbGetNextPhysicalIntIfNumber(objInterfaceValue, &nextObjInterfaceValue);
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue,
                           sizeof (nextObjInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanDeviceTable_VoiceVlanDeviceMacAddress
*
* @purpose Get 'VoiceVlanDeviceMacAddress'
 *@description  [VoiceVlanDeviceMacAddress] The MAC address of the VOIP device
* connected to this interface   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanDeviceTable_VoiceVlanDeviceMacAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();

  xLibU32_t keyInterfaceValue;

  xLibStr6_t objVoiceVlanDeviceMacAddressValue;
  xLibStr6_t nextObjVoiceVlanDeviceMacAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanDeviceTable_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* retrieve key: VoiceVlanDeviceMacAddress */
  owa.len = sizeof (objVoiceVlanDeviceMacAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanDeviceTable_VoiceVlanDeviceMacAddress,
                          (xLibU8_t *) objVoiceVlanDeviceMacAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    FPOBJ_CLR_MAC (objVoiceVlanDeviceMacAddressValue);
    owa.l7rc = usmdbVoiceVlanPortDeviceInfoFirstGet(L7_UNIT_CURRENT,
                                     keyInterfaceValue,
                                     nextObjVoiceVlanDeviceMacAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objVoiceVlanDeviceMacAddressValue, owa.len);
    memcpy(nextObjVoiceVlanDeviceMacAddressValue, objVoiceVlanDeviceMacAddressValue, sizeof(objVoiceVlanDeviceMacAddressValue));

    owa.l7rc = usmdbVoiceVlanPortDeviceInfoNextGet(L7_UNIT_CURRENT, 
                                    keyInterfaceValue,
                                    nextObjVoiceVlanDeviceMacAddressValue);

  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjVoiceVlanDeviceMacAddressValue, owa.len);

  /* return the object value: VoiceVlanDeviceMacAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjVoiceVlanDeviceMacAddressValue,
                           sizeof (nextObjVoiceVlanDeviceMacAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
