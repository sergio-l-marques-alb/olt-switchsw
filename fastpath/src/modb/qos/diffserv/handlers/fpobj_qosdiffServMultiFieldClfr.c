/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_qosdiffServMultiFieldClfr.c
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
#include "_xe_qosdiffServMultiFieldClfr_obj.h"
#include "usmdb_mib_diffserv_api.h"

/*******************************************************************************
* @function fpObjGet_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId
*
* @purpose Get 'diffServMultiFieldClfrId'
*
* @description [diffServMultiFieldClfrId]: An index that enumerates the MultiField
*              Classifier filter entries. Managers obtain new values
*              for row creation in this table by reading diffServMultiFieldClfrNextFree.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objdiffServMultiFieldClfrIdValue;
  xLibU32_t nextObjdiffServMultiFieldClfrIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMultiFieldClfrId */
  owa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId,
                   (xLibU8_t *) & objdiffServMultiFieldClfrIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objdiffServMultiFieldClfrIdValue = 0;
    nextObjdiffServMultiFieldClfrIdValue = 0;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objdiffServMultiFieldClfrIdValue, owa.len);
  owa.l7rc =
      usmDbDiffServMFClfrGetNext (L7_UNIT_CURRENT,
                                  objdiffServMultiFieldClfrIdValue,
                                  &nextObjdiffServMultiFieldClfrIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjdiffServMultiFieldClfrIdValue, owa.len);

  /* return the object value: diffServMultiFieldClfrId */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjdiffServMultiFieldClfrIdValue,
                    sizeof (objdiffServMultiFieldClfrIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServMultiFieldClfr_AddrType
*
* @purpose Get 'AddrType'
*
* @description [AddrType]: The type of IP address used by this classifier
*              entry. While other types of addresses are defined in the InetAddressType
*              textual convention, and DNS names, a classifier
*              can only look at packets on the wire. Therefore, this object
*              is limi 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMultiFieldClfr_AddrType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMultiFieldClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAddrTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMultiFieldClfrId */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId,
                   (xLibU8_t *) & keydiffServMultiFieldClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMultiFieldClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMFClfrAddrTypeGet (L7_UNIT_CURRENT,
                                    keydiffServMultiFieldClfrIdValue,
                                    &objAddrTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AddrType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAddrTypeValue,
                           sizeof (objAddrTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServMultiFieldClfr_DstAddr
*
* @purpose Get 'DstAddr'
*
* @description [DstAddr]: The IP address to match against the packet's destination
*              IP address. This may not be a DNS name, but may be
*              an IPv4 or IPv6 prefix. diffServMultiFieldClfrDstPrefixLength
*              indicates the number of bits that are relevant. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMultiFieldClfr_DstAddr (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMultiFieldClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDstAddrValue;
  xLibU32_t tempIPAddr;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMultiFieldClfrId */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId,
                   (xLibU8_t *) & keydiffServMultiFieldClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMultiFieldClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMFClfrDstAddrGet (L7_UNIT_CURRENT,
                                   keydiffServMultiFieldClfrIdValue,
                                   &tempIPAddr);
    if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memset(objDstAddrValue,0x00,sizeof(objDstAddrValue));
  memcpy(objDstAddrValue, (char*)&tempIPAddr, sizeof(tempIPAddr));

  /* return the object value: DstAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDstAddrValue,
                           strlen (objDstAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServMultiFieldClfr_DstPrefixLength
*
* @purpose Get 'DstPrefixLength'
*
* @description [DstPrefixLength]: The length of the CIDR Prefix carried in
*              diffServMultiFieldClfrDstAddr. In IPv4 addresses, a length
*              of 0 indicates a match of any address; a length of 32 indicates
*              a match of a single host address, and a length between
*              0 and 32 ind 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMultiFieldClfr_DstPrefixLength (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMultiFieldClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDstPrefixLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMultiFieldClfrId */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId,
                   (xLibU8_t *) & keydiffServMultiFieldClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMultiFieldClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMFClfrDstPrefixLenGet (L7_UNIT_CURRENT,
                                        keydiffServMultiFieldClfrIdValue,
                                        &objDstPrefixLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DstPrefixLength */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDstPrefixLengthValue,
                           sizeof (objDstPrefixLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServMultiFieldClfr_SrcAddr
*
* @purpose Get 'SrcAddr'
*
* @description [SrcAddr]: The IP address to match against the packet's source
*              IP address. This may not be a DNS name, but may be an IPv4
*              or IPv6 prefix. diffServMultiFieldClfrSrcPrefixLength indicates
*              the number of bits that are relevant. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMultiFieldClfr_SrcAddr (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMultiFieldClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSrcAddrValue;
  xLibU32_t tempIPAddr;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMultiFieldClfrId */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId,
                   (xLibU8_t *) & keydiffServMultiFieldClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMultiFieldClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMFClfrSrcAddrGet (L7_UNIT_CURRENT,
                                   keydiffServMultiFieldClfrIdValue,
                                   &tempIPAddr);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  memset(objSrcAddrValue,0x00,sizeof(objSrcAddrValue));
  memcpy(objSrcAddrValue, (char*)&tempIPAddr, sizeof(tempIPAddr));

  /* return the object value: SrcAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSrcAddrValue,
                           strlen (objSrcAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServMultiFieldClfr_SrcPrefixLength
*
* @purpose Get 'SrcPrefixLength'
*
* @description [SrcPrefixLength]: The length of the CIDR Prefix carried in
*              diffServMultiFieldClfrSrcAddr. In IPv4 addresses, a length
*              of 0 indicates a match of any address; a length of 32 indicates
*              a match of a single host address, and a length between
*              0 and 32 ind 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMultiFieldClfr_SrcPrefixLength (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMultiFieldClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSrcPrefixLengthValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMultiFieldClfrId */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId,
                   (xLibU8_t *) & keydiffServMultiFieldClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMultiFieldClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMFClfrSrcPrefixLenGet (L7_UNIT_CURRENT,
                                        keydiffServMultiFieldClfrIdValue,
                                        &objSrcPrefixLengthValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: SrcPrefixLength */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSrcPrefixLengthValue,
                           sizeof (objSrcPrefixLengthValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServMultiFieldClfr_Dscp
*
* @purpose Get 'Dscp'
*
* @description [Dscp]: The value that the DSCP in the packet must have to
*              match this entry. A value of -1 indicates that a specific DSCP
*              value has not been defined and thus all DSCP values are
*              considered a match. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMultiFieldClfr_Dscp (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMultiFieldClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDscpValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMultiFieldClfrId */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId,
                   (xLibU8_t *) & keydiffServMultiFieldClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMultiFieldClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMFClfrDscpGet (L7_UNIT_CURRENT,
                                keydiffServMultiFieldClfrIdValue,
                                &objDscpValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dscp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDscpValue,
                           sizeof (objDscpValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServMultiFieldClfr_FlowId
*
* @purpose Get 'FlowId'
*
* @description [FlowId]: The flow identifier in an IPv6 header. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMultiFieldClfr_FlowId (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMultiFieldClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objFlowIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMultiFieldClfrId */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId,
                   (xLibU8_t *) & keydiffServMultiFieldClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMultiFieldClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMFClfrFlowIdGet (L7_UNIT_CURRENT,
                                  keydiffServMultiFieldClfrIdValue,
                                  &objFlowIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: FlowId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objFlowIdValue,
                           sizeof (objFlowIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_qosdiffServMultiFieldClfr_Protocol
*
* @purpose Get 'Protocol'
*
* @description [Protocol]: The IP protocol to match against the IPv4 protocol
*              number or the IPv6 Next- Header number in the packet. A
*              value of 255 means match all. Note the protocol number of
*              255 is reserved by IANA, and Next-Header number of 0 is used
*              in IPv6. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMultiFieldClfr_Protocol (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMultiFieldClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProtocolValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMultiFieldClfrId */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId,
                   (xLibU8_t *) & keydiffServMultiFieldClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMultiFieldClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMFClfrProtocolGet (L7_UNIT_CURRENT,
                                    keydiffServMultiFieldClfrIdValue,
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
* @function fpObjGet_qosdiffServMultiFieldClfr_DstL4PortMin
*
* @purpose Get 'DstL4PortMin'
*
* @description [DstL4PortMin]: The minimum value that the layer-4 destination
*              port number in the packet must have in order to match this
*              classifier entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMultiFieldClfr_DstL4PortMin (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMultiFieldClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDstL4PortMinValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMultiFieldClfrId */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId,
                   (xLibU8_t *) & keydiffServMultiFieldClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMultiFieldClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMFClfrDstL4PortMinGet (L7_UNIT_CURRENT,
                                        keydiffServMultiFieldClfrIdValue,
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
* @function fpObjGet_qosdiffServMultiFieldClfr_DstL4PortMax
*
* @purpose Get 'DstL4PortMax'
*
* @description [DstL4PortMax]: The maximum value that the layer-4 destination
*              port number in the packet must have in order to match this
*              classifier entry. This value must be equal to or greater
*              than the value specified for this entry in diffServMultiFieldClfrDstL4Port
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMultiFieldClfr_DstL4PortMax (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMultiFieldClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDstL4PortMaxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMultiFieldClfrId */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId,
                   (xLibU8_t *) & keydiffServMultiFieldClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMultiFieldClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMFClfrDstL4PortMaxGet (L7_UNIT_CURRENT,
                                        keydiffServMultiFieldClfrIdValue,
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
* @function fpObjGet_qosdiffServMultiFieldClfr_SrcL4PortMin
*
* @purpose Get 'SrcL4PortMin'
*
* @description [SrcL4PortMin]: The minimum value that the layer-4 source port
*              number in the packet must have in order to match this classifier
*              entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMultiFieldClfr_SrcL4PortMin (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMultiFieldClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSrcL4PortMinValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMultiFieldClfrId */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId,
                   (xLibU8_t *) & keydiffServMultiFieldClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMultiFieldClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMFClfrSrcL4PortMinGet (L7_UNIT_CURRENT,
                                        keydiffServMultiFieldClfrIdValue,
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
* @function fpObjGet_qosdiffServMultiFieldClfr_SrcL4PortMax
*
* @purpose Get 'SrcL4PortMax'
*
* @description [SrcL4PortMax]: The maximum value that the layer-4 source port
*              number in the packet must have in order to match this classifier
*              entry. This value must be equal to or greater than
*              the value specified for this entry in diffServMultiFieldClfrSrcL4PortMin.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMultiFieldClfr_SrcL4PortMax (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMultiFieldClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objSrcL4PortMaxValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMultiFieldClfrId */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId,
                   (xLibU8_t *) & keydiffServMultiFieldClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMultiFieldClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMFClfrSrcL4PortMaxGet (L7_UNIT_CURRENT,
                                        keydiffServMultiFieldClfrIdValue,
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
* @function fpObjGet_qosdiffServMultiFieldClfr_Storage
*
* @purpose Get 'Storage'
*
* @description [Storage]: The storage type for this conceptual row. Conceptual
*              rows having the value 'permanent' need not allow write-access
*              to any columnar objects in the row. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMultiFieldClfr_Storage (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMultiFieldClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStorageValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMultiFieldClfrId */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId,
                   (xLibU8_t *) & keydiffServMultiFieldClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMultiFieldClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMFClfrStorageGet (L7_UNIT_CURRENT,
                                   keydiffServMultiFieldClfrIdValue,
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
* @function fpObjGet_qosdiffServMultiFieldClfr_Status
*
* @purpose Get 'Status'
*
* @description [Status]: The status of this conceptual row. All writable objects
*              in this row may be modified at any time. Setting this
*              variable to 'destroy' when the MIB contains one or more RowPointers
*              pointing to it results in destruction being delayed
*              until t 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_qosdiffServMultiFieldClfr_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keydiffServMultiFieldClfrIdValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: diffServMultiFieldClfrId */
  kwa.rc =
    xLibFilterGet (wap, XOBJ_qosdiffServMultiFieldClfr_diffServMultiFieldClfrId,
                   (xLibU8_t *) & keydiffServMultiFieldClfrIdValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keydiffServMultiFieldClfrIdValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDiffServMFClfrStatusGet (L7_UNIT_CURRENT,
                                  keydiffServMultiFieldClfrIdValue,
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
