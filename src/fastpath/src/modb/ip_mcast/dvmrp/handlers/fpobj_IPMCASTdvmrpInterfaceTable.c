
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_IPMCASTdvmrpInterfaceTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to IPMCAST-object.xml
*
* @create  01 May 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_IPMCASTdvmrpInterfaceTable_obj.h"
#include "usmdb_mib_dvmrp_api.h"
#include "usmdb_util_api.h"
#include "usmdb_ip_api.h"

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceIfIndex
*
* @purpose Get 'dvmrpInterfaceIfIndex'
*
* @description [dvmrpInterfaceIfIndex] The ifIndex value of the interface for which DVMRP is enabled.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceIfIndex (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdvmrpInterfaceIfIndexValue;
  xLibU32_t nextObjdvmrpInterfaceIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpInterfaceIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceIfIndex,
                          (xLibU8_t *) & objdvmrpInterfaceIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objdvmrpInterfaceIfIndexValue = 0;
    nextObjdvmrpInterfaceIfIndexValue = 0; 
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF | USM_LOGICAL_VLAN_INTF,
                                            0, &nextObjdvmrpInterfaceIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objdvmrpInterfaceIfIndexValue, owa.len);
    nextObjdvmrpInterfaceIfIndexValue = 0;
    owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF | USM_LOGICAL_VLAN_INTF, 0,
                    objdvmrpInterfaceIfIndexValue, &nextObjdvmrpInterfaceIfIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdvmrpInterfaceIfIndexValue, owa.len);

  /* return the object value: dvmrpInterfaceIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdvmrpInterfaceIfIndexValue,
                           sizeof (objdvmrpInterfaceIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceLocalAddress
*
* @purpose Get 'dvmrpInterfaceLocalAddress'
*
* @description [dvmrpInterfaceLocalAddress] The IP address this system will use as a source address on this interface. On unnumbered interfaces, it must be the same value as dvmrpInterfaceLocalAddress for some interface on the system.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceLocalAddress (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t objdvmrpInterfaceLocalAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpInterfaceIfIndex */
  kwadvmrpInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceIfIndex,
                   (xLibU8_t *) & keydvmrpInterfaceIfIndexValue, &kwadvmrpInterfaceIfIndex.len);
  if (kwadvmrpInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpInterfaceIfIndex);
    return kwadvmrpInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpInterfaceIfIndexValue, kwadvmrpInterfaceIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpIntfLocalAddressGet (L7_UNIT_CURRENT, keydvmrpInterfaceIfIndexValue,
                              &objdvmrpInterfaceLocalAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpInterfaceLocalAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpInterfaceLocalAddressValue,
                           sizeof (objdvmrpInterfaceLocalAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceMetric
*
* @purpose Get 'dvmrpInterfaceMetric'
*
* @description [dvmrpInterfaceMetric] The distance metric for this interface which is used to calculate distance vectors.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceMetric (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU16_t));
  xLibU16_t objdvmrpInterfaceMetricValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpInterfaceIfIndex */
  kwadvmrpInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceIfIndex,
                   (xLibU8_t *) & keydvmrpInterfaceIfIndexValue, &kwadvmrpInterfaceIfIndex.len);
  if (kwadvmrpInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpInterfaceIfIndex);
    return kwadvmrpInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpInterfaceIfIndexValue, kwadvmrpInterfaceIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpIntfMetricGet (L7_UNIT_CURRENT, keydvmrpInterfaceIfIndexValue,
                              &objdvmrpInterfaceMetricValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpInterfaceMetric */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpInterfaceMetricValue,
                           sizeof (objdvmrpInterfaceMetricValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceMetric
*
* @purpose Set 'dvmrpInterfaceMetric'
*
* @description [dvmrpInterfaceMetric] The distance metric for this interface which is used to calculate distance vectors.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceMetric (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdvmrpInterfaceMetricValue;

  fpObjWa_t kwadvmrpInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpInterfaceIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: dvmrpInterfaceMetric */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objdvmrpInterfaceMetricValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdvmrpInterfaceMetricValue, owa.len);

  /* retrieve key: dvmrpInterfaceIfIndex */
  kwadvmrpInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceIfIndex,
                   (xLibU8_t *) & keydvmrpInterfaceIfIndexValue, &kwadvmrpInterfaceIfIndex.len);
  if (kwadvmrpInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpInterfaceIfIndex);
    return kwadvmrpInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpInterfaceIfIndexValue, kwadvmrpInterfaceIfIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbDvmrpIntfMetricSet (L7_UNIT_CURRENT, keydvmrpInterfaceIfIndexValue,
                              objdvmrpInterfaceMetricValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceStatus
*
* @purpose Get 'dvmrpInterfaceStatus'
*
* @description [dvmrpInterfaceStatus] The status of this entry. Creating the entry enables DVMRP on the virtual interface; destroying the entry or setting it to notInService disables DVMRP on the virtual interface
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceStatus (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpInterfaceIfIndexValue;
  xLibU32_t mode;
 
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdvmrpInterfaceStatusValue;
  FPOBJ_TRACE_ENTER (bufp);
 
  /* retrieve key: dvmrpInterfaceIfIndex */
  kwadvmrpInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceIfIndex,
                   (xLibU8_t *) & keydvmrpInterfaceIfIndexValue, &kwadvmrpInterfaceIfIndex.len);
  if (kwadvmrpInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpInterfaceIfIndex);
    return kwadvmrpInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpInterfaceIfIndexValue, kwadvmrpInterfaceIfIndex.len);

  if(usmDbDvmrpIntfAdminModeGet(L7_UNIT_CURRENT, keydvmrpInterfaceIfIndexValue, &mode) == L7_SUCCESS)
  {
     if((mode==L7_ENABLE) && (usmDbDvmrpInterfaceOperationalStateGet(L7_UNIT_CURRENT, keydvmrpInterfaceIfIndexValue) == L7_TRUE))
     {
       objdvmrpInterfaceStatusValue = L7_ROW_STATUS_ACTIVE;
       owa.l7rc = L7_SUCCESS;
     }
     else
     {
      objdvmrpInterfaceStatusValue = L7_ROW_STATUS_NOT_IN_SERVICE;
      owa.l7rc =  L7_SUCCESS;
     }
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

  /* return the object value: dvmrpInterfaceStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpInterfaceStatusValue,
                           sizeof (objdvmrpInterfaceStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceStatus
*
* @purpose Set 'dvmrpInterfaceStatus'
*
* @description [dvmrpInterfaceStatus] The status of this entry. Creating the entry enables DVMRP on the virtual interface; destroying the entry or setting it to notInService disables DVMRP on the virtual interface
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdvmrpInterfaceStatusValue;

  fpObjWa_t kwadvmrpInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpInterfaceIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: dvmrpInterfaceStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objdvmrpInterfaceStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdvmrpInterfaceStatusValue, owa.len);

  /* retrieve key: dvmrpInterfaceIfIndex */
  kwadvmrpInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceIfIndex,
                   (xLibU8_t *) & keydvmrpInterfaceIfIndexValue, &kwadvmrpInterfaceIfIndex.len);
  if (kwadvmrpInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpInterfaceIfIndex);
    return kwadvmrpInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpInterfaceIfIndexValue, kwadvmrpInterfaceIfIndex.len);

  owa.l7rc = L7_SUCCESS;
    switch (objdvmrpInterfaceStatusValue)
  {
  case L7_ROW_STATUS_ACTIVE:
       if(usmDbDvmrpInterfaceOperationalStateGet(L7_UNIT_CURRENT, keydvmrpInterfaceIfIndexValue) == L7_TRUE)
         owa.l7rc = usmDbDvmrpIntfAdminModeSet(L7_UNIT_CURRENT,keydvmrpInterfaceIfIndexValue,L7_ENABLE);
       else
         owa.l7rc = L7_FAILURE;
  case L7_ROW_STATUS_CREATE_AND_GO:
       owa.l7rc = usmDbDvmrpIntfAdminModeSet(L7_UNIT_CURRENT,keydvmrpInterfaceIfIndexValue,L7_ENABLE);
       break;

  case L7_ROW_STATUS_DESTROY:
       owa.l7rc = usmDbDvmrpIntfAdminModeSet(L7_UNIT_CURRENT,keydvmrpInterfaceIfIndexValue,L7_DISABLE);
       break;

  default:
    owa.l7rc = L7_FAILURE;
  }


  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceRcvBadPkts
*
* @purpose Get 'dvmrpInterfaceRcvBadPkts'
*
* @description [dvmrpInterfaceRcvBadPkts] The number of DVMRP messages received on the interface by the DVMRP process which were subsequently discarded as invalid (e.g. invalid packet format, or a route report from an unknown neighbor).
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceRcvBadPkts (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibUL32_t));
  xLibUL32_t objdvmrpInterfaceRcvBadPktsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpInterfaceIfIndex */
  kwadvmrpInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceIfIndex,
                   (xLibU8_t *) & keydvmrpInterfaceIfIndexValue, &kwadvmrpInterfaceIfIndex.len);
  if (kwadvmrpInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpInterfaceIfIndex);
    return kwadvmrpInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpInterfaceIfIndexValue, kwadvmrpInterfaceIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpIntfRcvBadPktsGet (L7_UNIT_CURRENT, keydvmrpInterfaceIfIndexValue,
                              &objdvmrpInterfaceRcvBadPktsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpInterfaceRcvBadPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpInterfaceRcvBadPktsValue,
                           sizeof (objdvmrpInterfaceRcvBadPktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceRcvBadRoutes
*
* @purpose Get 'dvmrpInterfaceRcvBadRoutes'
*
* @description [dvmrpInterfaceRcvBadRoutes] The number of routes, in valid DVMRP packets, which were ignored because the entry was invalid.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceRcvBadRoutes (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibUL32_t));
  xLibUL32_t objdvmrpInterfaceRcvBadRoutesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpInterfaceIfIndex */
  kwadvmrpInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceIfIndex,
                   (xLibU8_t *) & keydvmrpInterfaceIfIndexValue, &kwadvmrpInterfaceIfIndex.len);
  if (kwadvmrpInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpInterfaceIfIndex);
    return kwadvmrpInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpInterfaceIfIndexValue, kwadvmrpInterfaceIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpIntfRcvBadRoutesGet (L7_UNIT_CURRENT, keydvmrpInterfaceIfIndexValue,
                              &objdvmrpInterfaceRcvBadRoutesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpInterfaceRcvBadRoutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpInterfaceRcvBadRoutesValue,
                           sizeof (objdvmrpInterfaceRcvBadRoutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceSentRoutes
*
* @purpose Get 'dvmrpInterfaceSentRoutes'
*
* @description [dvmrpInterfaceSentRoutes] The number of routes, in DVMRP Report packets, which have been sent on this interface. Together with dvmrpNeighborRcvRoutes at a peer, this object is useful for detecting routes being lost.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceSentRoutes (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibUL32_t));
  xLibUL32_t objdvmrpInterfaceSentRoutesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpInterfaceIfIndex */
  kwadvmrpInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceIfIndex,
                   (xLibU8_t *) & keydvmrpInterfaceIfIndexValue, &kwadvmrpInterfaceIfIndex.len);
  if (kwadvmrpInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpInterfaceIfIndex);
    return kwadvmrpInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpInterfaceIfIndexValue, kwadvmrpInterfaceIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpIntfSentRoutesGet (L7_UNIT_CURRENT, keydvmrpInterfaceIfIndexValue,
                              &objdvmrpInterfaceSentRoutesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpInterfaceSentRoutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpInterfaceSentRoutesValue,
                           sizeof (objdvmrpInterfaceSentRoutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceAdminMode
*
* @purpose Get 'dvmrpInterfaceAdminMode'
*
* @description [dvmrpInterfaceAdminMode] DVMRP Interface Admin Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceAdminMode (void *wap, void *bufp)
{

  fpObjWa_t kwadvmrpInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpInterfaceIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdvmrpInterfaceAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpInterfaceIfIndex */
  kwadvmrpInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceIfIndex,
                   (xLibU8_t *) & keydvmrpInterfaceIfIndexValue, &kwadvmrpInterfaceIfIndex.len);
  if (kwadvmrpInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpInterfaceIfIndex);
    return kwadvmrpInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpInterfaceIfIndexValue, kwadvmrpInterfaceIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpIntfAdminModeGet (L7_UNIT_CURRENT, keydvmrpInterfaceIfIndexValue,
                              &objdvmrpInterfaceAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: dvmrpInterfaceAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpInterfaceAdminModeValue,
                           sizeof (objdvmrpInterfaceAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceAdminMode
*
* @purpose Set 'dvmrpInterfaceAdminMode'
*
* @description [dvmrpInterfaceAdminMode] DVMRP Interface Admin Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceAdminMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdvmrpInterfaceAdminModeValue;

  fpObjWa_t kwadvmrpInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpInterfaceIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: dvmrpInterfaceAdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objdvmrpInterfaceAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objdvmrpInterfaceAdminModeValue, owa.len);

  /* retrieve key: dvmrpInterfaceIfIndex */
  kwadvmrpInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceIfIndex,
                   (xLibU8_t *) & keydvmrpInterfaceIfIndexValue, &kwadvmrpInterfaceIfIndex.len);
  if (kwadvmrpInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpInterfaceIfIndex);
    return kwadvmrpInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpInterfaceIfIndexValue, kwadvmrpInterfaceIfIndex.len);

  /* set the value in application */
  owa.l7rc = usmDbDvmrpIntfAdminModeSet (L7_UNIT_CURRENT, keydvmrpInterfaceIfIndexValue,
                              objdvmrpInterfaceAdminModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpNumRoutes
*
* @purpose Get 'dvmrpNumRoutes'
*
* @description [dvmrpNumRoutes] The number of entries in the routing table. This can be used to monitor the routing table size to detect illegal advertisements of unicast routes.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpNumRoutes (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdvmrpNumRoutesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpNumRoutesGet (L7_UNIT_CURRENT, &objdvmrpNumRoutesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objdvmrpNumRoutesValue, sizeof (objdvmrpNumRoutesValue));

  /* return the object value: dvmrpNumRoutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpNumRoutesValue,
                           sizeof (objdvmrpNumRoutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpReachableRoutes
*
* @purpose Get 'dvmrpReachableRoutes'
*
* @description [dvmrpReachableRoutes] The number of entries in the routing table with non infinite metrics. This can be used to detect network partitions by observing the ratio of reachable routes to total routes.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpReachableRoutes (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdvmrpReachableRoutesValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpReachableRoutesGet (L7_UNIT_CURRENT, &objdvmrpReachableRoutesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objdvmrpReachableRoutesValue, sizeof (objdvmrpReachableRoutesValue));

  /* return the object value: dvmrpReachableRoutes */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpReachableRoutesValue,
                           sizeof (objdvmrpReachableRoutesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpVersionString
*
* @purpose Get 'dvmrpVersionString'
*
* @description [dvmrpVersionString] The router's DVMRP version information. Similar to sysDescr in MIB-II, this is a free-form field which can be used to display vendor-specific information.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpVersionString (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objdvmrpVersionStringValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbDvmrpVersionStringGet (L7_UNIT_CURRENT, objdvmrpVersionStringValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objdvmrpVersionStringValue, strlen (objdvmrpVersionStringValue));

  /* return the object value: dvmrpVersionString */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objdvmrpVersionStringValue,
                           strlen (objdvmrpVersionStringValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpIntfGenerationId
*
* @purpose Get 'dvmrpIntfGenerationId'
*
* @description [dvmrpIntfGenerationId] DVMRP Generation ID used by the router
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_IPMCASTdvmrpInterfaceTable_dvmrpIntfGenerationId (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibUL32_t));
  xLibUL32_t objdvmrpIntfGenerationIdValue;
  fpObjWa_t kwadvmrpInterfaceIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydvmrpInterfaceIfIndexValue;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: dvmrpInterfaceIfIndex */
  kwadvmrpInterfaceIfIndex.rc =
    xLibFilterGet (wap, XOBJ_IPMCASTdvmrpInterfaceTable_dvmrpInterfaceIfIndex,
                   (xLibU8_t *) & keydvmrpInterfaceIfIndexValue, &kwadvmrpInterfaceIfIndex.len);
  if (kwadvmrpInterfaceIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwadvmrpInterfaceIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwadvmrpInterfaceIfIndex);
    return kwadvmrpInterfaceIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydvmrpInterfaceIfIndexValue, kwadvmrpInterfaceIfIndex.len);


  /* get the value from application */
  owa.l7rc = usmDbDvmrpIntfGenerationIdGet (L7_UNIT_CURRENT, keydvmrpInterfaceIfIndexValue, &objdvmrpIntfGenerationIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objdvmrpIntfGenerationIdValue, sizeof (objdvmrpIntfGenerationIdValue));

  /* return the object value: dvmrpIntfGenerationId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objdvmrpIntfGenerationIdValue,
                           sizeof (objdvmrpIntfGenerationIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
