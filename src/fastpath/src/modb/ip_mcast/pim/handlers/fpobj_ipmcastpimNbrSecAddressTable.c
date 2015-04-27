
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_ipmcastpimNbrSecAddressTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to ipmcast-object.xml
*
* @create  10 May 2008, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_ipmcastpimNbrSecAddressTable_obj.h"

/*******************************************************************************
* @function fpObjGet_ipmcastpimNbrSecAddressTable_pimNbrSecAddressIfIndex
*
* @purpose Get 'pimNbrSecAddressIfIndex'
*
* @description [pimNbrSecAddressIfIndex] The value of ifIndex for the interface used to reach this PIM neighbor.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNbrSecAddressTable_pimNbrSecAddressIfIndex (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNbrSecAddressIfIndexValue;
  xLibU32_t nextObjpimNbrSecAddressIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNbrSecAddressIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNbrSecAddressTable_pimNbrSecAddressIfIndex,
                          (xLibU8_t *) & objpimNbrSecAddressIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT, &nextObjpimNbrSecAddressIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimNbrSecAddressIfIndexValue, owa.len);
    owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT, objpimNbrSecAddressIfIndexValue,
                                    &nextObjpimNbrSecAddressIfIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimNbrSecAddressIfIndexValue, owa.len);

  /* return the object value: pimNbrSecAddressIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimNbrSecAddressIfIndexValue,
                           sizeof (objpimNbrSecAddressIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNbrSecAddressTable_pimNbrSecAddressType
*
* @purpose Get 'pimNbrSecAddressType'
*
* @description [pimNbrSecAddressType] The address type of this PIM neighbor.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNbrSecAddressTable_pimNbrSecAddressType (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objpimNbrSecAddressTypeValue;
  xLibU32_t nextObjpimNbrSecAddressTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNbrSecAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNbrSecAddressTable_pimNbrSecAddressType,
                          (xLibU8_t *) & objpimNbrSecAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT, &nextObjpimNbrSecAddressTypeValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objpimNbrSecAddressTypeValue, owa.len);
    owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT, objpimNbrSecAddressTypeValue,
                                    &nextObjpimNbrSecAddressTypeValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjpimNbrSecAddressTypeValue, owa.len);

  /* return the object value: pimNbrSecAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjpimNbrSecAddressTypeValue,
                           sizeof (objpimNbrSecAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNbrSecAddressTable_pimNbrSecAddressPrimary
*
* @purpose Get 'pimNbrSecAddressPrimary'
*
* @description [pimNbrSecAddressPrimary] The primary IP address of this PIM neighbor. The InetAddressType is given by the pimNbrSecAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNbrSecAddressTable_pimNbrSecAddressPrimary (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpimNbrSecAddressPrimaryValue;
  xLibStr256_t nextObjpimNbrSecAddressPrimaryValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNbrSecAddressPrimary */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNbrSecAddressTable_pimNbrSecAddressPrimary,
                          (xLibU8_t *) objpimNbrSecAddressPrimaryValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT, nextObjpimNbrSecAddressPrimaryValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objpimNbrSecAddressPrimaryValue, owa.len);
    owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT, objpimNbrSecAddressPrimaryValue,
                                    nextObjpimNbrSecAddressPrimaryValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjpimNbrSecAddressPrimaryValue, owa.len);

  /* return the object value: pimNbrSecAddressPrimary */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjpimNbrSecAddressPrimaryValue,
                           sizeof (objpimNbrSecAddressPrimaryValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}

/*******************************************************************************
* @function fpObjGet_ipmcastpimNbrSecAddressTable_pimNbrSecAddress
*
* @purpose Get 'pimNbrSecAddress'
*
* @description [pimNbrSecAddress] The secondary IP address of this PIM neighbor. The InetAddressType is given by the pimNbrSecAddressType object.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_ipmcastpimNbrSecAddressTable_pimNbrSecAddress (void *wap, void *bufp)
{
#ifdef NOT_IMPLEMENTED
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objpimNbrSecAddressValue;
  xLibStr256_t nextObjpimNbrSecAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: pimNbrSecAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastpimNbrSecAddressTable_pimNbrSecAddress,
                          (xLibU8_t *) objpimNbrSecAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT, nextObjpimNbrSecAddressValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objpimNbrSecAddressValue, owa.len);
    owa.l7rc = usmDbGetNextUnknown (L7_UNIT_CURRENT, objpimNbrSecAddressValue,
                                    nextObjpimNbrSecAddressValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjpimNbrSecAddressValue, owa.len);

  /* return the object value: pimNbrSecAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjpimNbrSecAddressValue,
                           sizeof (objpimNbrSecAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

#endif

  return XLIBRC_NOT_IMPLEMENTED;
}
