/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingLLDPXdot3LocalData.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to LLDP-object.xml
*
* @create  5 February 2008
*
* @author Radha K 
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingLLDPXdot3LocalData_obj.h"
#include "usmdb_lldp_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXdot3LocalData_LocalPortNum
*
* @purpose Get 'LocalPortNum'
*
* @description [LocalPortNum]: The index value used to identify the port component
*              (contained in the local chassis with the LLDP agent)
*              associated with this entry. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXdot3LocalData_LocalPortNum (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLocalPortNumValue;
  xLibU32_t nextObjLocalPortNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LocalPortNum */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3LocalData_LocalPortNum,
                          (xLibU8_t *) & objLocalPortNumValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet (&nextObjLocalPortNumValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLocalPortNumValue, owa.len);
    owa.l7rc = usmDbGetNextVisibleExtIfNumber ( objLocalPortNumValue,
                                                &nextObjLocalPortNumValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLocalPortNumValue, owa.len);

  /* return the object value: LocalPortNum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLocalPortNumValue,
                           sizeof (objLocalPortNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXdot3LocalData_PortAutoNegSupported
*
* @purpose Get 'PortAutoNegSupported'
*
* @description [PortAutoNegSupported]: The truth value used to indicate whether
*              the given port (associated with the local system) supports
*              Auto-negotiation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXdot3LocalData_PortAutoNegSupported (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortAutoNegSupportedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LocalPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3LocalData_LocalPortNum,
                          (xLibU8_t *) & keyLocalPortNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXdot3LocPortAutoNegSupportedGet ( keyLocalPortNumValue,
                                               &objPortAutoNegSupportedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PortAutoNegSupported */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPortAutoNegSupportedValue,
                           sizeof (objPortAutoNegSupportedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXdot3LocalData_PortAutoNegEnabled
*
* @purpose Get 'PortAutoNegEnabled'
*
* @description [PortAutoNegEnabled]: The truth value used to indicate whether
*              port Auto-negotiation is enabled on the given port associated
*              with the local system. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXdot3LocalData_PortAutoNegEnabled (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortAutoNegEnabledValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LocalPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3LocalData_LocalPortNum,
                          (xLibU8_t *) & keyLocalPortNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXdot3LocPortAutoNegEnabledGet ( keyLocalPortNumValue,
                                             &objPortAutoNegEnabledValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PortAutoNegEnabled */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPortAutoNegEnabledValue,
                           sizeof (objPortAutoNegEnabledValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXdot3LocalData_PortAutoNegAdvertisedCap
*
* @purpose Get 'PortAutoNegAdvertisedCap'
*
* @description [PortAutoNegAdvertisedCap]: This object contains the value
*              (bitmap) of the ifMauAutoNegCapAdvertisedBits object (defined
*              in IETF RFC 3636) which is associated with the given port
*              on the local system. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXdot3LocalData_PortAutoNegAdvertisedCap (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPortAutoNegAdvertisedCapValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LocalPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3LocalData_LocalPortNum,
                          (xLibU8_t *) & keyLocalPortNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXdot3LocPortAutoNegAdvertizedCapGet ( keyLocalPortNumValue,
                                                   objPortAutoNegAdvertisedCapValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PortAutoNegAdvertisedCap */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPortAutoNegAdvertisedCapValue,
                           strlen (objPortAutoNegAdvertisedCapValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXdot3LocalData_PortOperMauType
*
* @purpose Get 'PortOperMauType'
*
* @description [PortOperMauType]: An integer value that indicates the operational
*              MAU type of the given port on the local system. This
*              object contains the integer value derived from the list position
*              of the corresponding dot3MauType as listed in IETF
*              RFC 3636 (or subsequent revisions) and 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXdot3LocalData_PortOperMauType (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortOperMauTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LocalPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3LocalData_LocalPortNum,
                          (xLibU8_t *) & keyLocalPortNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXdot3LocPortAutoNegOperMauTypeGet ( keyLocalPortNumValue,
                                                 &objPortOperMauTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PortOperMauType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPortOperMauTypeValue,
                           sizeof (objPortOperMauTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
