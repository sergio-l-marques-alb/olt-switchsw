/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosDiffServAuxMfClfr.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to diffserv-object.xml
*
* @create  2 February 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_qosDiffServAuxMfClfr_obj.h"
#include "usmdb_mib_diffserv_api.h"


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_DiffServAuxMfClfrId
*
* @purpose Get 'DiffServAuxMfClfrId'
*
* @description [DiffServAuxMfClfrId]: An index that enumerates the Auxiliary
*              MultiField Classifier filter entries. Managers obtain new
*              values for row creation in this table by reading agentDiffServAuxMfClfrNextFree.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_DiffServAuxMfClfrId (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDiffServAuxMfClfrIdValue;
  xLibU32_t nextObjDiffServAuxMfClfrIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  owa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & objDiffServAuxMfClfrIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objDiffServAuxMfClfrIdValue = 0 ;
    nextObjDiffServAuxMfClfrIdValue = 0;	
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objDiffServAuxMfClfrIdValue, owa.len);
  owa.l7rc =
      usmDbDiffServAuxMFClfrGetNext (L7_UNIT_CURRENT,
                                     objDiffServAuxMfClfrIdValue,
                                     &nextObjDiffServAuxMfClfrIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjDiffServAuxMfClfrIdValue, owa.len);

  /* return the object value: DiffServAuxMfClfrId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjDiffServAuxMfClfrIdValue,
                           sizeof (objDiffServAuxMfClfrIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_DstAddr
*
* @purpose Get 'DstAddr'
*
* @description [DstAddr]: The IP address to match against the packet's destination
*              IP address. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_DstAddr (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDstAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrDstAddrGet (L7_UNIT_CURRENT,
                                      keyDiffServAuxMfClfrIdValue,
                                      &objDstAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DstAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDstAddrValue,
                           sizeof (objDstAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_DstMask
*
* @purpose Get 'DstMask'
*
* @description [DstMask]: This mask value identifies the portion of agentDiffServAuxMfClfrDstAddr
*              that is compared against a packet.
*              A non-contiguous mask value is permitted. A mask of 0 indicates
*              a match of any address. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_DstMask (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDstMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrDstMaskGet (L7_UNIT_CURRENT,
                                      keyDiffServAuxMfClfrIdValue,
                                      &objDstMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DstMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDstMaskValue,
                           sizeof (objDstMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_SrcAddr
*
* @purpose Get 'SrcAddr'
*
* @description [SrcAddr]: The IP address to match against the packet's source
*              IP address. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_SrcAddr (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSrcAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrSrcAddrGet (L7_UNIT_CURRENT,
                                      keyDiffServAuxMfClfrIdValue,
                                      &objSrcAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SrcAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSrcAddrValue,
                           sizeof (objSrcAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_SrcMask
*
* @purpose Get 'SrcMask'
*
* @description [SrcMask]: This mask value identifies the portion of agentDiffServAuxMfClfrSrcAddr
*              that is compared against a packet.
*              A non-contiguous mask value is permitted. A mask of 0 indicates
*              a match of any address. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_SrcMask (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSrcMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrSrcMaskGet (L7_UNIT_CURRENT,
                                      keyDiffServAuxMfClfrIdValue,
                                      &objSrcMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SrcMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSrcMaskValue,
                           sizeof (objSrcMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_Protocol
*
* @purpose Get 'Protocol'
*
* @description [Protocol]: The IP protocol to match against the IPv4 protocol
*              number. A value of 255 means match all. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_Protocol (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrProtocolGet (L7_UNIT_CURRENT,
                                       keyDiffServAuxMfClfrIdValue,
                                       &objProtocolValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Protocol */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objProtocolValue,
                           sizeof (objProtocolValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_DstL4PortMin
*
* @purpose Get 'DstL4PortMin'
*
* @description [DstL4PortMin]: The minimum value that the layer-4 destination
*              port number in the packet must have in order to match this
*              classifier entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_DstL4PortMin (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDstL4PortMinValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrDstL4PortMinGet (L7_UNIT_CURRENT,
                                           keyDiffServAuxMfClfrIdValue,
                                           &objDstL4PortMinValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DstL4PortMin */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDstL4PortMinValue,
                           sizeof (objDstL4PortMinValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_DstL4PortMax
*
* @purpose Get 'DstL4PortMax'
*
* @description [DstL4PortMax]: The maximum value that the layer-4 destination
*              port number in the packet must have in order to match this
*              classifier entry. This value must be equal to or greater
*              than the value specified for this entry in agentDiffServAuxMfClfrDstL4PortMin
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_DstL4PortMax (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDstL4PortMaxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrDstL4PortMaxGet (L7_UNIT_CURRENT,
                                           keyDiffServAuxMfClfrIdValue,
                                           &objDstL4PortMaxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DstL4PortMax */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDstL4PortMaxValue,
                           sizeof (objDstL4PortMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_SrcL4PortMin
*
* @purpose Get 'SrcL4PortMin'
*
* @description [SrcL4PortMin]: The minimum value that the layer-4 source port
*              number in the packet must have in order to match this classifier
*              entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_SrcL4PortMin (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSrcL4PortMinValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrSrcL4PortMinGet (L7_UNIT_CURRENT,
                                           keyDiffServAuxMfClfrIdValue,
                                           &objSrcL4PortMinValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SrcL4PortMin */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSrcL4PortMinValue,
                           sizeof (objSrcL4PortMinValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_SrcL4PortMax
*
* @purpose Get 'SrcL4PortMax'
*
* @description [SrcL4PortMax]: The maximum value that the layer-4 source port
*              number in the packet must have in order to match this classifier
*              entry. This value must be equal to or greater than
*              the value specified for this entry in diffServMultiFieldClfrSrcL4PortMi
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_SrcL4PortMax (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSrcL4PortMaxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrSrcL4PortMaxGet (L7_UNIT_CURRENT,
                                           keyDiffServAuxMfClfrIdValue,
                                           &objSrcL4PortMaxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SrcL4PortMax */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSrcL4PortMaxValue,
                           sizeof (objSrcL4PortMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_Cos
*
* @purpose Get 'Cos'
*
* @description [Cos]: Three-bit user priority field in the 802.1Q tag header
*              of a tagged Ethernet frame used as a class-match parameter.
*              For frames containing a double VLAN tag, this field is
*              located in the first/outer tag. A value of -1 indicates that
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_Cos (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCosValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrCosGet (L7_UNIT_CURRENT, keyDiffServAuxMfClfrIdValue,
                                  &objCosValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Cos */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCosValue,
                           sizeof (objCosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_Tos
*
* @purpose Get 'Tos'
*
* @description [Tos]: IP TOS bits value, defined as all eight bits of the
*              Service Type octet in the IPv4 header. There are multiple,
*              overlapping, meanings of the TOS octet in use today: Precedence
*              (bits 7-5): IP Precedence, values 0-7 DSCP (bits 7-2):
*              IP 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_Tos (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTosValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrTosGet (L7_UNIT_CURRENT, keyDiffServAuxMfClfrIdValue,
                                  objTosValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Tos */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTosValue,
                           strlen (objTosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_TosMask
*
* @purpose Get 'TosMask'
*
* @description [TosMask]: IP TOS bits mask value. It identifies the portion
*              of agentDiffServAuxMfClfrTos that is compared against a packet.
*              A non-contiguous mask value is permitted. A mask of
*              0 indicates a match of any TOS value. There are multiple, overlapping
*              mea 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_TosMask (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTosMaskValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrTosMaskGet (L7_UNIT_CURRENT,
                                      keyDiffServAuxMfClfrIdValue,
                                      objTosMaskValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TosMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTosMaskValue,
                           strlen (objTosMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_DstMac
*
* @purpose Get 'DstMac'
*
* @description [DstMac]: Destination MAC address. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_DstMac (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDstMacValue;
  xLibU32_t stringLengthValue = sizeof(objDstMacValue);
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrDstMacGet (L7_UNIT_CURRENT,
                                     keyDiffServAuxMfClfrIdValue,
                                     objDstMacValue,
                                     &stringLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DstMac */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDstMacValue,
                           strlen (objDstMacValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_DstMacMask
*
* @purpose Get 'DstMacMask'
*
* @description [DstMacMask]: Destination MAC address mask value. This mask
*              value identifies the portion of agentDiffServAuxMfClfrDstMac
*              that is compared against a packet. A non-contiguous mask
*              value is permitted. A mask of 0 indicates a match of any MAC
*              address. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_DstMacMask (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDstMacMaskValue;
  xLibU32_t stringLengthValue = sizeof(objDstMacMaskValue);

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrDstMacMaskGet (L7_UNIT_CURRENT,
                                         keyDiffServAuxMfClfrIdValue,
                                         objDstMacMaskValue,
                                         &stringLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DstMacMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDstMacMaskValue,
                           strlen (objDstMacMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_SrcMac
*
* @purpose Get 'SrcMac'
*
* @description [SrcMac]: Source MAC address. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_SrcMac (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSrcMacValue;
  xLibU32_t stringLengthValue = sizeof(objSrcMacValue);

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrSrcMacGet (L7_UNIT_CURRENT,
                                     keyDiffServAuxMfClfrIdValue,
                                     objSrcMacValue,
                                    &stringLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SrcMac */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSrcMacValue,
                           strlen (objSrcMacValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_SrcMacMask
*
* @purpose Get 'SrcMacMask'
*
* @description [SrcMacMask]: Source MAC address mask value. This mask value
*              identifies the portion of agentDiffServAuxMfClfrSrcMac that
*              is compared against a packet. A non-contiguous mask value
*              is permitted. A mask of 0 indicates a match of any MAC address.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_SrcMacMask (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSrcMacMaskValue;
  xLibU32_t stringLengthValue = sizeof(objSrcMacMaskValue);

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrSrcMacMaskGet (L7_UNIT_CURRENT,
                                         keyDiffServAuxMfClfrIdValue,
                                         objSrcMacMaskValue,
                                        &stringLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SrcMacMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSrcMacMaskValue,
                           strlen (objSrcMacMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_VlanId
*
* @purpose Get 'VlanId'
*
* @description [VlanId]: VLAN ID value for the classifier. A value of -1 indicates
*              that a specific VLAN ID value has not been defined
*              and thus all VLAN ID values are considered a match. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_VlanId (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);
  
  owa.l7rc = L7_SUCCESS;

  /* get the value from application */
  /*owa.l7rc = usmDbGetUnknown (L7_UNIT_CURRENT, keyDiffServAuxMfClfrIdValue,
                              &objVlanIdValue);*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VlanId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanIdValue,
                           sizeof (objVlanIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_Storage
*
* @purpose Get 'Storage'
*
* @description [Storage]: The storage type for this conceptual row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_Storage (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStorageValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrStorageGet (L7_UNIT_CURRENT,
                                      keyDiffServAuxMfClfrIdValue,
                                      &objStorageValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Storage */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStorageValue,
                           sizeof (objStorageValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_Status
*
* @purpose Get 'Status'
*
* @description [Status]: The status of this conceptual row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrStatusGet (L7_UNIT_CURRENT,
                                     keyDiffServAuxMfClfrIdValue,
                                     &objStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_Cos2
*
* @purpose Get 'Cos2'
*
* @description [Cos2]: Three-bit user priority field in the second/inner 802.1Q
*              tag header of a double VLAN tagged Ethernet frame used
*              as a class-match parameter A value of -1 indicates that a
*              specific Secondary COS value has not been defined and thus
*              all Secondary CO 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_Cos2 (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCos2Value;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrCos2Get (L7_UNIT_CURRENT, keyDiffServAuxMfClfrIdValue,
                                   &objCos2Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Cos2 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCos2Value,
                           sizeof (objCos2Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_EtypeVal1
*
* @purpose Get 'EtypeVal1'
*
* @description [EtypeVal1]: Ethertype value to be compared in order to match
*              this classifier entry. The allowed value for this object
*              is 0x0600 to 0xFFFF, with a value of 0 used to indicate this
*              object is not involved in the classifier entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_EtypeVal1 (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEtypeVal1Value;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrEtypeVal1Get (L7_UNIT_CURRENT,
                                        keyDiffServAuxMfClfrIdValue,
                                        &objEtypeVal1Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: EtypeVal1 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEtypeVal1Value,
                           sizeof (objEtypeVal1Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_EtypeVal2
*
* @purpose Get 'EtypeVal2'
*
* @description [EtypeVal2]: A second Ethertype value to be compared in order
*              to match this classifier entry. This value is considered
*              in addition to the value specified by agentDiffServAuxMfClfrEtypeVal1
*              and serves as a second possible match value (i.e.
*              a packet can match 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_EtypeVal2 (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEtypeVal2Value;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrEtypeVal2Get (L7_UNIT_CURRENT,
                                        keyDiffServAuxMfClfrIdValue,
                                        &objEtypeVal2Value);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: EtypeVal2 */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEtypeVal2Value,
                           sizeof (objEtypeVal2Value));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_VlanIdMin
*
* @purpose Get 'VlanIdMin'
*
* @description [VlanIdMin]: The minimum value that the VLAN ID in the packet
*              must have in order to match this classifier entry. This
*              field is defined as the 12-bit VLAN identifier in the 802.1Q
*              tag header of a tagged Ethernet frame. For a double VLAN
*              tagged frame, t 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_VlanIdMin (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanIdMinValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrVlanIdMinGet (L7_UNIT_CURRENT,
                                        keyDiffServAuxMfClfrIdValue,
                                        &objVlanIdMinValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VlanIdMin */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanIdMinValue,
                           sizeof (objVlanIdMinValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_VlanIdMax
*
* @purpose Get 'VlanIdMax'
*
* @description [VlanIdMax]: The maximum value that the VLAN ID in the packet
*              must have in order to match this classifier entry. This
*              field is defined as the 12-bit VLAN identifier in the 802.1Q
*              tag header of a tagged Ethernet frame. For a double VLAN
*              tagged frame, t 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_VlanIdMax (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanIdMaxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrVlanIdMaxGet (L7_UNIT_CURRENT,
                                        keyDiffServAuxMfClfrIdValue,
                                        &objVlanIdMaxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VlanIdMax */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanIdMaxValue,
                           sizeof (objVlanIdMaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_VlanId2Min
*
* @purpose Get 'VlanId2Min'
*
* @description [VlanId2Min]: The minimum value that the Secondary VLAN ID
*              in the packet must have in order to match this classifier entry.
*              This field is defined as the 12-bit VLAN identifier in
*              the second/inner 802.1Q tag header of a double VLAN tagged
*              Ethernet frame. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_VlanId2Min (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanId2MinValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrVlanId2MinGet (L7_UNIT_CURRENT,
                                         keyDiffServAuxMfClfrIdValue,
                                         &objVlanId2MinValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VlanId2Min */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanId2MinValue,
                           sizeof (objVlanId2MinValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosDiffServAuxMfClfr_VlanId2Max
*
* @purpose Get 'VlanId2Max'
*
* @description [VlanId2Max]: The maximum value that the Secondary VLAN ID
*              in the packet must have in order to match this classifier entry.
*              This field is defined as the 12-bit VLAN identifier in
*              the second/inner 802.1Q tag header of a double VLAN tagged
*              Ethernet frame. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosDiffServAuxMfClfr_VlanId2Max (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyDiffServAuxMfClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVlanId2MaxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: DiffServAuxMfClfrId */
  kwa.rc = xLibFilterGet (wap, XOBJ_qosDiffServAuxMfClfr_DiffServAuxMfClfrId,
                          (xLibU8_t *) & keyDiffServAuxMfClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyDiffServAuxMfClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServAuxMFClfrVlanId2MaxGet (L7_UNIT_CURRENT,
                                         keyDiffServAuxMfClfrIdValue,
                                         &objVlanId2MaxValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VlanId2Max */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVlanId2MaxValue,
                           sizeof (objVlanId2MaxValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
