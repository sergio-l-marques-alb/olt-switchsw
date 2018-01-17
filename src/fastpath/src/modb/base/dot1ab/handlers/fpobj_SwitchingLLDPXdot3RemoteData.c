/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingLLDPXdot3RemoteData.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to LLDP-object.xml
*
* @create  7 February 2008
*
* @author  Radha K 
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingLLDPXdot3RemoteData_obj.h"
#include "usmdb_lldp_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXdot3RemoteData_TimeMark
*
* @purpose Get 'TimeMark'
*
* @description [TimeMark]: A TimeFilter for this entry. See the TimeFilter
*              textual convention in IETF RFC 2021 and http://www.ietf.org/IESG/Implementations/RFC2021-Implementation.txt
*              to see how
*              TimeFilter works. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXdot3RemoteData_TimeMark (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTimeMarkValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  xLibU32_t RemIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: LocalPortNum */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa1.len);

  /* retrieve key: TimeMark */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_TimeMark,
                          (xLibU8_t *) & objTimeMarkValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
    owa.l7rc = usmDbLldpRemEntryGet (keyLocalPortNumValue, &RemIndexValue,
                                     &objTimeMarkValue );
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objTimeMarkValue, owa.len);
    while(usmDbGetNextPhysicalIntIfNumber(keyLocalPortNumValue,
                                          &keyLocalPortNumValue) == L7_SUCCESS)
    {
      owa.l7rc = usmDbLldpRemEntryGet (keyLocalPortNumValue,
                                       &RemIndexValue, &objTimeMarkValue);
    }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &objTimeMarkValue, owa.len);

  /* return the object value: TimeMark */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTimeMarkValue,
                           sizeof (objTimeMarkValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXdot3RemoteData_LocalPortNum
*
* @purpose Get 'LocalPortNum'
*
* @description [LocalPortNum]: The index value used to identify the port component
*              (contained in the local chassis with the LLDP agent)
*              associated with this entry. The LocalPortNum identifies
*              the port on which the remote system information is received.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXdot3RemoteData_LocalPortNum (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLocalPortNumValue;
  xLibU32_t nextObjLocalPortNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LocalPortNum */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_LocalPortNum,
                          (xLibU8_t *) & objLocalPortNumValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet( &nextObjLocalPortNumValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLocalPortNumValue, owa.len);
    owa.l7rc = usmDbGetNextPhysicalIntIfNumber( objLocalPortNumValue,
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
* @function fpObjGet_SwitchingLLDPXdot3RemoteData_RemIndex
*
* @purpose Get 'RemIndex'
*
* @description [RemIndex]: This object represents an arbitrary local integer
*              value used by this agent to identify a particular connection
*              instance, unique only for the indicated remote system.
*              An agent is encouraged to assign monotonically increasing
*              index values to new entries, s 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXdot3RemoteData_RemIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRemIndexValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  xLibU32_t TimeMarkValue;
  FPOBJ_TRACE_ENTER (bufp);
  
  /* retrieve key: LocalPortNum */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa1.len);

  /* retrieve key: RemIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_RemIndex,
                          (xLibU8_t *) & objRemIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
   FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
   owa.l7rc = usmDbLldpRemEntryGet (keyLocalPortNumValue, &objRemIndexValue,
                                     &TimeMarkValue );
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRemIndexValue, owa.len);
    while(usmDbGetNextPhysicalIntIfNumber(keyLocalPortNumValue,
                                          &keyLocalPortNumValue) == L7_SUCCESS)
    {
      owa.l7rc = usmDbLldpRemEntryGet (keyLocalPortNumValue,
                                       &objRemIndexValue, &TimeMarkValue);
    }
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &objRemIndexValue, owa.len);

  /* return the object value: RemIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRemIndexValue,
                           sizeof (objRemIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXdot3RemoteData_ManAddrSubtype
*
* @purpose Get 'ManAddrSubtype'
*
* @description [ManAddrSubtype]: The type of management address identifier
*              encoding used in the associated 'lldpRemManagmentAddr' object.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXdot3RemoteData_ManAddrSubtype (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objManAddrSubtypeValue;
  xLibU32_t nextObjManAddrSubtypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ManAddrSubtype */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_ManAddrSubtype,
                          (xLibU8_t *) & objManAddrSubtypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, owa.len);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objManAddrSubtypeValue, owa.len);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjManAddrSubtypeValue, owa.len);

  /* return the object value: ManAddrSubtype */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjManAddrSubtypeValue,
                           sizeof (objManAddrSubtypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXdot3RemoteData_ManAddr
*
* @purpose Get 'ManAddr'
*
* @description [ManAddr]: The string value used to identify the management
*              address component associated with the remote system. The purpose
*              of this address is to contact the management entity.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXdot3RemoteData_ManAddr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objManAddrValue;
  xLibStr256_t nextObjManAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ManAddr */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_ManAddr,
                          (xLibU8_t *) objManAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objManAddrValue, owa.len);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, nextObjManAddrValue, owa.len);

  /* return the object value: ManAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjManAddrValue,
                           strlen (objManAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXdot3RemoteData_PortAutoNegSupported
*
* @purpose Get 'PortAutoNegSupported'
*
* @description [PortAutoNegSupported]: The truth value used to indicate whether
*              the given port (associated with the local system) supports
*              Auto-negotiation. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXdot3RemoteData_PortAutoNegSupported (void *wap,
                                                                     void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortAutoNegSupportedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXdot3RemPortAutoNegSupportedGet (keyLocalPortNumValue,
                                              keyRemIndexValue,
                                              keyTimeMarkValue,
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
* @function fpObjGet_SwitchingLLDPXdot3RemoteData_PortAutoNegEnabled
*
* @purpose Get 'PortAutoNegEnabled'
*
* @description [PortAutoNegEnabled]: The truth value used to indicate whether
*              port Auto-negotiation is enabled on the given port associated
*              with the local system. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXdot3RemoteData_PortAutoNegEnabled (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortAutoNegEnabledValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXdot3RemPortAutoNegEnabledGet (keyLocalPortNumValue,
                                            keyRemIndexValue,
                                            keyTimeMarkValue,
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
* @function fpObjGet_SwitchingLLDPXdot3RemoteData_PortAutoNegAdvertisedCap
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
xLibRC_t fpObjGet_SwitchingLLDPXdot3RemoteData_PortAutoNegAdvertisedCap (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPortAutoNegAdvertisedCapValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXdot3RemPortAutoNegAdvertizedCapGet (keyLocalPortNumValue,
                                                  keyRemIndexValue,
                                                  keyTimeMarkValue,
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
* @function fpObjGet_SwitchingLLDPXdot3RemoteData_PortOperMauType
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
xLibRC_t fpObjGet_SwitchingLLDPXdot3RemoteData_PortOperMauType (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyTimeMarkValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLocalPortNumValue;
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRemIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortOperMauTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TimeMark */
  kwa1.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_TimeMark,
                           (xLibU8_t *) & keyTimeMarkValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTimeMarkValue, kwa1.len);

  /* retrieve key: LocalPortNum */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_LocalPortNum,
                           (xLibU8_t *) & keyLocalPortNumValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLocalPortNumValue, kwa2.len);

  /* retrieve key: RemIndex */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXdot3RemoteData_RemIndex,
                           (xLibU8_t *) & keyRemIndexValue, &kwa3.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRemIndexValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXdot3RemPortAutoNegOperMauTypeGet (keyLocalPortNumValue,
                                                keyRemIndexValue,
                                                keyTimeMarkValue,
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
