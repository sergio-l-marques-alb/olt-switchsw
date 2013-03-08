/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_securitydos.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to dos-object.xml
*
* @create  16 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_securitydos_obj.h"
#include "usmdb_dos_api.h"

/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceSIPDIPMode
*
* @purpose Get 'DenialOfServiceSIPDIPMode'
*
* @description Enables or Disables the SIP-DIP Denial of Service protection feature. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceSIPDIPMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceSIPDIPModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmdbDoSSIPDIPModeGet ( &objDenialOfServiceSIPDIPModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceSIPDIPModeValue,
                     sizeof (objDenialOfServiceSIPDIPModeValue));

  /* return the object value: DenialOfServiceSIPDIPMode */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceSIPDIPModeValue,
                    sizeof (objDenialOfServiceSIPDIPModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceSIPDIPMode
*
* @purpose Set 'DenialOfServiceSIPDIPMode'
*
* @description Enables or Disables the SIP-DIP Denial of Service protection feature. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceSIPDIPMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceSIPDIPModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceSIPDIPMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDenialOfServiceSIPDIPModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceSIPDIPModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmdbDoSSIPDIPSet (L7_UNIT_CURRENT, objDenialOfServiceSIPDIPModeValue);
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
* @function fpObjGet_securitydos_DenialOfServiceICMPSize
*
* @purpose Get 'DenialOfServiceICMPSize'
*
* @description Configures the Maximum allowed ICMP data size. This is active 
*              if the ICMP Denial of Service protection feature is enabled. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceICMPSize (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceICMPSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmdbDoSICMPSizeGet ( &objDenialOfServiceICMPSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceICMPSizeValue,
                     sizeof (objDenialOfServiceICMPSizeValue));

  /* return the object value: DenialOfServiceICMPSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceICMPSizeValue,
                           sizeof (objDenialOfServiceICMPSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceICMPSize
*
* @purpose Set 'DenialOfServiceICMPSize'
*
* @description Configures the Maximum allowed ICMP data size. This is active 
*              if the ICMP Denial of Service protection feature is enabled. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceICMPSize (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceICMPSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceICMPSize */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDenialOfServiceICMPSizeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceICMPSizeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmdbDoSICMPSizeSet (L7_UNIT_CURRENT, objDenialOfServiceICMPSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceL4PortMode
*
* @purpose Get 'DenialOfServiceL4PortMode'
*
* @description Enables or Disables the L4 Port Denial of Service protection feature. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceL4PortMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceL4PortModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmdbDoSL4PortModeGet ( &objDenialOfServiceL4PortModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceL4PortModeValue,
                     sizeof (objDenialOfServiceL4PortModeValue));

  /* return the object value: DenialOfServiceL4PortMode */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceL4PortModeValue,
                    sizeof (objDenialOfServiceL4PortModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceL4PortMode
*
* @purpose Set 'DenialOfServiceL4PortMode'
*
* @description Enables or Disables the L4 Port Denial of Service protection feature. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceL4PortMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceL4PortModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceL4PortMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDenialOfServiceL4PortModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceL4PortModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmdbDoSL4PortSet (L7_UNIT_CURRENT, objDenialOfServiceL4PortModeValue);
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
* @function fpObjGet_securitydos_DenialOfServiceTCPFlagMode
*
* @purpose Get 'DenialOfServiceTCPFlagMode'
*
* @description Enables or Disables the TCP Flag Denial of Service protection 
*              feature. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceTCPFlagMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPFlagModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmdbDoSTCPFlagModeGet (&objDenialOfServiceTCPFlagModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPFlagModeValue,
                     sizeof (objDenialOfServiceTCPFlagModeValue));

  /* return the object value: DenialOfServiceTCPFlagMode */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceTCPFlagModeValue,
                    sizeof (objDenialOfServiceTCPFlagModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceTCPFlagMode
*
* @purpose Set 'DenialOfServiceTCPFlagMode'
*
* @description Enables or Disables the TCP Flag Denial of Service protection 
*              feature. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceTCPFlagMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPFlagModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceTCPFlagMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDenialOfServiceTCPFlagModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPFlagModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmdbDoSTCPFlagSet (L7_UNIT_CURRENT, objDenialOfServiceTCPFlagModeValue);
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
* @function fpObjGet_securitydos_DenialOfServiceTCPHdrSize
*
* @purpose Get 'DenialOfServiceTCPHdrSize'
*
* @description Configures the Minimum allowed TCP Header size. This is active 
*              if the First Fragment Denial of Service protection feature is 
*              enabled. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceTCPHdrSize (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPHdrSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmdbDoSFirstFragSizeGet (&objDenialOfServiceTCPHdrSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPHdrSizeValue,
                     sizeof (objDenialOfServiceTCPHdrSizeValue));

  /* return the object value: DenialOfServiceTCPHdrSize */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceTCPHdrSizeValue,
                    sizeof (objDenialOfServiceTCPHdrSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceTCPHdrSize
*
* @purpose Set 'DenialOfServiceTCPHdrSize'
*
* @description Configures the Minimum allowed TCP Header size. This is active 
*              if the First Fragment Denial of Service protection feature is 
*              enabled. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceTCPHdrSize (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPHdrSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceTCPHdrSize */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDenialOfServiceTCPHdrSizeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPHdrSizeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmdbDoSFirstFragSizeSet (L7_UNIT_CURRENT,
                              objDenialOfServiceTCPHdrSizeValue);
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
* @function fpObjGet_securitydos_DenialOfServiceTCPFragMode
*
* @purpose Get 'DenialOfServiceTCPFragMode'
*
* @description Enables or Disables the TCP Fragment Denial of Service protection 
*              feature. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceTCPFragMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPFragModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmdbDoSTCPFragModeGet ( &objDenialOfServiceTCPFragModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPFragModeValue,
                     sizeof (objDenialOfServiceTCPFragModeValue));

  /* return the object value: DenialOfServiceTCPFragMode */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceTCPFragModeValue,
                    sizeof (objDenialOfServiceTCPFragModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceTCPFragMode
*
* @purpose Set 'DenialOfServiceTCPFragMode'
*
* @description Enables or Disables the TCP Fragment Denial of Service protection 
*              feature. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceTCPFragMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPFragModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceTCPFragMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDenialOfServiceTCPFragModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPFragModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmdbDoSTCPFragSet (L7_UNIT_CURRENT, objDenialOfServiceTCPFragModeValue);
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
* @function fpObjGet_securitydos_DenialOfServiceICMPMode
*
* @purpose Get 'DenialOfServiceICMPMode'
*
* @description Enables or Disables the ICMP Denial of Service protection feature. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceICMPMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceICMPModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmdbDoSICMPModeGet ( &objDenialOfServiceICMPModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceICMPModeValue,
                     sizeof (objDenialOfServiceICMPModeValue));

  /* return the object value: DenialOfServiceICMPMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceICMPModeValue,
                           sizeof (objDenialOfServiceICMPModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceICMPMode
*
* @purpose Set 'DenialOfServiceICMPMode'
*
* @description Enables or Disables the ICMP Denial of Service protection feature. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceICMPMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceICMPModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceICMPMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDenialOfServiceICMPModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceICMPModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmdbDoSICMPModeSet (L7_UNIT_CURRENT, objDenialOfServiceICMPModeValue);
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
* @function fpObjGet_securitydos_DenialOfServiceFirstFragMode
*
* @purpose Get 'DenialOfServiceFirstFragMode'
*
* @description Enables or Disables the First Fragment Denial of Service protection 
*              feature. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceFirstFragMode (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceFirstFragModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmdbDoSFirstFragModeGet ( &objDenialOfServiceFirstFragModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceFirstFragModeValue,
                     sizeof (objDenialOfServiceFirstFragModeValue));

  /* return the object value: DenialOfServiceFirstFragMode */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceFirstFragModeValue,
                    sizeof (objDenialOfServiceFirstFragModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceFirstFragMode
*
* @purpose Set 'DenialOfServiceFirstFragMode'
*
* @description Enables or Disables the First Fragment Denial of Service protection 
*              feature. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceFirstFragMode (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceFirstFragModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceFirstFragMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDenialOfServiceFirstFragModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceFirstFragModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmdbDoSFirstFragModeSet (L7_UNIT_CURRENT,
                              objDenialOfServiceFirstFragModeValue);
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
* @function fpObjGet_securitydos_DenialOfServiceGlobalPingFloodingMode
*
* @purpose Get 'DenialOfServiceGlobalPingFloodingMode'
 *@description  [DenialOfServiceGlobalPingFloodingMode] The mode for preventing
* PING flooding DOS attack   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceGlobalPingFloodingMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceGlobalPingFloodingModeValue;
  L7_uint32 param;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDosGlobalPingFloodingGet (&objDenialOfServiceGlobalPingFloodingModeValue,
                                                                              &param);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceGlobalPingFloodingModeValue,
                     sizeof (objDenialOfServiceGlobalPingFloodingModeValue));

  /* return the object value: DenialOfServiceGlobalPingFloodingMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceGlobalPingFloodingModeValue,
                           sizeof (objDenialOfServiceGlobalPingFloodingModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceGlobalPingFloodingMode
*
* @purpose Set 'DenialOfServiceGlobalPingFloodingMode'
 *@description  [DenialOfServiceGlobalPingFloodingMode] The mode for preventing
* PING flooding DOS attack   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceGlobalPingFloodingMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaParam = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceGlobalPingFloodingModeValue;
  xLibU32_t objDenialOfServiceGlobalPingFloodingParamValue;
	 

  FPOBJ_TRACE_ENTER (bufp);

  /*retrieve param value */
  owaParam.rc = xLibFilterGet (wap, XOBJ_securitydos_DenialOfServiceGlobalPingFloodingParam,
                          (xLibU8_t *) & objDenialOfServiceGlobalPingFloodingParamValue, &owaParam.len);
  if (owaParam.rc != XLIBRC_SUCCESS)
  {
  	  /*set param value to default */
	  objDenialOfServiceGlobalPingFloodingParamValue = FD_DOS_DEFAULT_PING_PARAM;
  }

  /* retrieve object: DenialOfServiceGlobalPingFloodingMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDenialOfServiceGlobalPingFloodingModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceGlobalPingFloodingModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbDosGlobalPingFloodingSet (objDenialOfServiceGlobalPingFloodingModeValue,
                                              objDenialOfServiceGlobalPingFloodingParamValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceGlobalPingFloodingParam
*
* @purpose Get 'DenialOfServiceGlobalPingFloodingParam'
 *@description  [DenialOfServiceGlobalPingFloodingParam] The param for
* preventing PING flooding DOS attack   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceGlobalPingFloodingParam (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceGlobalPingFloodingParamValue;
  L7_uint32 mode;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDosGlobalPingFloodingGet (&mode, &objDenialOfServiceGlobalPingFloodingParamValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceGlobalPingFloodingParamValue,
                     sizeof (objDenialOfServiceGlobalPingFloodingParamValue));

  /* return the object value: DenialOfServiceGlobalPingFloodingParam */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceGlobalPingFloodingParamValue,
                           sizeof (objDenialOfServiceGlobalPingFloodingParamValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceGlobalPingFloodingParam
*
* @purpose Set 'DenialOfServiceGlobalPingFloodingParam'
 *@description  [DenialOfServiceGlobalPingFloodingParam] The param for
* preventing PING flooding DOS attack   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceGlobalPingFloodingParam (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaMode = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceGlobalPingFloodingParamValue;
  xLibU32_t objDenialOfServiceGlobalPingFloodingModeValue;

  FPOBJ_TRACE_ENTER (bufp);

	/*retrieve mode value */
  owaMode.rc = xLibFilterGet (wap, XOBJ_securitydos_DenialOfServiceGlobalPingFloodingMode,
                          (xLibU8_t *) & objDenialOfServiceGlobalPingFloodingModeValue, &owaMode.len);
  if (owaMode.rc != XLIBRC_SUCCESS)
  {
  	  /*set param value to default */
	  objDenialOfServiceGlobalPingFloodingModeValue = FD_DOS_DEFAULT_PINGFLOODING_MODE;
  }

  /* retrieve object: DenialOfServiceGlobalPingFloodingParam */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDenialOfServiceGlobalPingFloodingParamValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceGlobalPingFloodingParamValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  if(objDenialOfServiceGlobalPingFloodingParamValue < 64)/* 64 Kbits/sec */
  {
    return XLIBRC_FAILURE;

  }
  owa.l7rc = usmdbDosGlobalPingFloodingSet (objDenialOfServiceGlobalPingFloodingModeValue,
                                 objDenialOfServiceGlobalPingFloodingParamValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceGlobalSmurfAttack
*
* @purpose Get 'DenialOfServiceGlobalSmurfAttack'
*
* @description The mode for preventing Smurf DOS Attack 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceGlobalSmurfAttack (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceGlobalSmurfAttackValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmdbDosGlobalSmurfAttackGet ( &objDenialOfServiceGlobalSmurfAttackValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceGlobalSmurfAttackValue,
                     sizeof (objDenialOfServiceGlobalSmurfAttackValue));

  /* return the object value: DenialOfServiceGlobalSmurfAttack */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) & objDenialOfServiceGlobalSmurfAttackValue,
                    sizeof (objDenialOfServiceGlobalSmurfAttackValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceGlobalSmurfAttack
*
* @purpose Set 'DenialOfServiceGlobalSmurfAttack'
*
* @description The mode for preventing Smurf DOS Attack 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceGlobalSmurfAttack (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceGlobalSmurfAttackValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceGlobalSmurfAttack */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objDenialOfServiceGlobalSmurfAttackValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceGlobalSmurfAttackValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmdbDosGlobalSmurfAttackSet ( objDenialOfServiceGlobalSmurfAttackValue);
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
* @function fpObjGet_securitydos_DenialOfServiceGlobalSynAckFloodingMode
*
* @purpose Get 'DenialOfServiceGlobalSynAckFloodingMode'
*
* @description Global Sync Acknowledge Flooding mode 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceGlobalSynAckFloodingMode (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceGlobalSynAckFloodingModeValue;
  L7_uint32 param;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmdbDosGlobalSynAckFloodingGet ( &objDenialOfServiceGlobalSynAckFloodingModeValue,
                                                                    &param);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceGlobalSynAckFloodingModeValue,
                     sizeof (objDenialOfServiceGlobalSynAckFloodingModeValue));

  /* return the object value: DenialOfServiceGlobalSynAckFloodingMode */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDenialOfServiceGlobalSynAckFloodingModeValue,
                    sizeof (objDenialOfServiceGlobalSynAckFloodingModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceGlobalSynAckFloodingMode
*
* @purpose Set 'DenialOfServiceGlobalSynAckFloodingMode'
*
* @description Global Sync Acknowledge Flooding mode 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceGlobalSynAckFloodingMode (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaParam = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceGlobalSynAckFloodingModeValue;
  xLibU32_t objDenialOfServiceGlobalSynAckFloodingParamValue;
  FPOBJ_TRACE_ENTER (bufp);

  /*retrieve param value */
  owaParam.rc = xLibFilterGet (wap, XOBJ_securitydos_DenialOfServiceGlobalSynAckFloodingParam,
                          (xLibU8_t *) & objDenialOfServiceGlobalSynAckFloodingParamValue, &owaParam.len);
  if (owaParam.rc != XLIBRC_SUCCESS)
  {
  	  /*set param value to default */
	  objDenialOfServiceGlobalSynAckFloodingParamValue = FD_DOS_DEFAULT_SYNACK_PARAM;
  }

  /* retrieve object: DenialOfServiceGlobalSynAckFloodingMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objDenialOfServiceGlobalSynAckFloodingModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceGlobalSynAckFloodingModeValue,
                     owa.len);

  /* set the value in application */
  owa.l7rc =
    usmdbDosGlobalSynAckFloodingSet (objDenialOfServiceGlobalSynAckFloodingModeValue,
                                                      objDenialOfServiceGlobalSynAckFloodingParamValue);
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
* @function fpObjGet_securitydos_DenialOfServiceGlobalSynAckFloodingParam
*
* @purpose Get 'DenialOfServiceGlobalSynAckFloodingParam'
*
* @description Global Sync Acknowledge Flooding param 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceGlobalSynAckFloodingParam (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceGlobalSynAckFloodingParamValue;
  L7_uint32 mode;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmdbDosGlobalSynAckFloodingGet (&mode,
                                     &objDenialOfServiceGlobalSynAckFloodingParamValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceGlobalSynAckFloodingParamValue,
                     sizeof (objDenialOfServiceGlobalSynAckFloodingParamValue));

  /* return the object value: DenialOfServiceGlobalSynAckFloodingParam */
  owa.rc =
    xLibBufDataSet (bufp,
                    (xLibU8_t *) &
                    objDenialOfServiceGlobalSynAckFloodingParamValue,
                    sizeof (objDenialOfServiceGlobalSynAckFloodingParamValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceGlobalSynAckFloodingParam
*
* @purpose Set 'DenialOfServiceGlobalSynAckFloodingParam'
*
* @description Global Sync Acknowledge Flooding param 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceGlobalSynAckFloodingParam (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  fpObjWa_t owaMode = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceGlobalSynAckFloodingParamValue;
  xLibU32_t objDenialOfServiceGlobalSynAckFloodingModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /*retrieve mode value */
  owaMode.rc = xLibFilterGet (wap, XOBJ_securitydos_DenialOfServiceGlobalSynAckFloodingMode,
                          (xLibU8_t *) & objDenialOfServiceGlobalSynAckFloodingModeValue, &owaMode.len);
  if (owaMode.rc != XLIBRC_SUCCESS)
  {
  	  /*set param value to default */
	  objDenialOfServiceGlobalSynAckFloodingModeValue = FD_DOS_DEFAULT_SYNACK_MODE;
  }

  /* retrieve object: DenialOfServiceGlobalSynAckFloodingParam */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objDenialOfServiceGlobalSynAckFloodingParamValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceGlobalSynAckFloodingParamValue,
                     owa.len);

  /* set the value in application */
  if(objDenialOfServiceGlobalSynAckFloodingParamValue < 64)/* 64 Kbits/sec */
  {
    return XLIBRC_FAILURE;
  }

  owa.l7rc =
    usmdbDosGlobalSynAckFloodingSet (objDenialOfServiceGlobalSynAckFloodingModeValue,
                                     objDenialOfServiceGlobalSynAckFloodingParamValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#if 0
/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceICMPV6Mode
*
* @purpose Get 'DenialOfServiceICMPV6Mode'
*
* @description [DenialOfServiceICMPV6Mode] ICMPV6 Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceICMPV6Mode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceICMPV6ModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDoSICMPv6ModeGet ( &objDenialOfServiceICMPV6ModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceICMPV6ModeValue,
                     sizeof (objDenialOfServiceICMPV6ModeValue));

  /* return the object value: DenialOfServiceICMPV6Mode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceICMPV6ModeValue,
                           sizeof (objDenialOfServiceICMPV6ModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceICMPV6Mode
*
* @purpose Set 'DenialOfServiceICMPV6Mode'
*
* @description [DenialOfServiceICMPV6Mode] ICMPV6 Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceICMPV6Mode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceICMPV6ModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceICMPV6Mode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDenialOfServiceICMPV6ModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceICMPV6ModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbDoSICMPv6ModeSet (L7_UNIT_CURRENT, objDenialOfServiceICMPV6ModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#endif

/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceICMPV6MaxSize
*
* @purpose Get 'DenialOfServiceICMPV6MaxSize'
*
* @description [DenialOfServiceICMPV6MaxSize] ICMPV6 Maximum Size
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceICMPV6MaxSize (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceICMPV6MaxSizeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDoSICMPv6SizeGet ( &objDenialOfServiceICMPV6MaxSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceICMPV6MaxSizeValue,
                     sizeof (objDenialOfServiceICMPV6MaxSizeValue));

  /* return the object value: DenialOfServiceICMPV6MaxSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceICMPV6MaxSizeValue,
                           sizeof (objDenialOfServiceICMPV6MaxSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceICMPV6MaxSize
*
* @purpose Set 'DenialOfServiceICMPV6MaxSize'
*
* @description [DenialOfServiceICMPV6MaxSize] ICMPV6 Maximum Size
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceICMPV6MaxSize (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceICMPV6MaxSizeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceICMPV6MaxSize */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDenialOfServiceICMPV6MaxSizeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceICMPV6MaxSizeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbDoSICMPv6SizeSet (L7_UNIT_CURRENT, objDenialOfServiceICMPV6MaxSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceICMPFragMode
*
* @purpose Get 'DenialOfServiceICMPFragMode'
*
* @description [DenialOfServiceICMPFragMode] Global ICMPFrag Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceICMPFragMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceICMPFragModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDoSICMPFragModeGet ( &objDenialOfServiceICMPFragModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceICMPFragModeValue,
                     sizeof (objDenialOfServiceICMPFragModeValue));

  /* return the object value: DenialOfServiceICMPFragMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceICMPFragModeValue,
                           sizeof (objDenialOfServiceICMPFragModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceICMPFragMode
*
* @purpose Set 'DenialOfServiceICMPFragMode'
*
* @description [DenialOfServiceICMPFragMode] Global ICMPFrag Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceICMPFragMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceICMPFragModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceICMPFragMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDenialOfServiceICMPFragModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceICMPFragModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbDoSICMPFragModeSet (L7_UNIT_CURRENT, objDenialOfServiceICMPFragModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceICMPv4Size
*
* @purpose Get 'DenialOfServiceICMPv4Size'
*
* @description [DenialOfServiceICMPv4Size] Global ICMPv4 Size
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceICMPv4Size (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceICMPv4SizeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDoSICMPv4SizeGet ( &objDenialOfServiceICMPv4SizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceICMPv4SizeValue,
                     sizeof (objDenialOfServiceICMPv4SizeValue));

  /* return the object value: DenialOfServiceICMPv4Size */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceICMPv4SizeValue,
                           sizeof (objDenialOfServiceICMPv4SizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceICMPv4Size
*
* @purpose Set 'DenialOfServiceICMPv4Size'
*
* @description [DenialOfServiceICMPv4Size] ICMPV6 Maximum Size
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceICMPv4Size (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceICMPv4SizeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceICMPv4Size */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDenialOfServiceICMPv4SizeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceICMPv4SizeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmdbDoSICMPSizeSet (L7_UNIT_CURRENT, objDenialOfServiceICMPv4SizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceSMACDMACMode
*
* @purpose Get 'DenialOfServiceSMACDMACMode'
*
* @description [DenialOfServiceSMACDMACMode] Global SMACDMAC Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceSMACDMACMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceSMACDMACModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDoSSMACDMACModeGet ( &objDenialOfServiceSMACDMACModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceSMACDMACModeValue,
                     sizeof (objDenialOfServiceSMACDMACModeValue));

  /* return the object value: DenialOfServiceSMACDMACMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceSMACDMACModeValue,
                           sizeof (objDenialOfServiceSMACDMACModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceSMACDMACMode
*
* @purpose Set 'DenialOfServiceSMACDMACMode'
*
* @description [DenialOfServiceSMACDMACMode] Global SMACDMAC Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceSMACDMACMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceSMACDMACModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceSMACDMACMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDenialOfServiceSMACDMACModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceSMACDMACModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbDoSSMACDMACSet (L7_UNIT_CURRENT, objDenialOfServiceSMACDMACModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceTCPFinUrgPshMode
*
* @purpose Get 'DenialOfServiceTCPFinUrgPshMode'
*
* @description [DenialOfServiceTCPFinUrgPshMode] Global TCP FinUrgPsh Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceTCPFinUrgPshMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPFinUrgPshModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDoSTCPFinUrgPshModeGet ( &objDenialOfServiceTCPFinUrgPshModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPFinUrgPshModeValue,
                     sizeof (objDenialOfServiceTCPFinUrgPshModeValue));

  /* return the object value: DenialOfServiceTCPFinUrgPshMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceTCPFinUrgPshModeValue,
                           sizeof (objDenialOfServiceTCPFinUrgPshModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceTCPFinUrgPshMode
*
* @purpose Set 'DenialOfServiceTCPFinUrgPshMode'
*
* @description [DenialOfServiceTCPFinUrgPshMode] Global TCP FinUrgPsh Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceTCPFinUrgPshMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPFinUrgPshModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceTCPFinUrgPshMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDenialOfServiceTCPFinUrgPshModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPFinUrgPshModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbDoSTCPFinUrgPshSet (L7_UNIT_CURRENT, objDenialOfServiceTCPFinUrgPshModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceTCPFlagSeqMode
*
* @purpose Get 'DenialOfServiceTCPFlagSeqMode'
*
* @description [DenialOfServiceTCPFlagSeqMode] Global TCP Flag Seq Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceTCPFlagSeqMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPFlagSeqModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDoSTCPFlagSeqModeGet ( &objDenialOfServiceTCPFlagSeqModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPFlagSeqModeValue,
                     sizeof (objDenialOfServiceTCPFlagSeqModeValue));

  /* return the object value: DenialOfServiceTCPFlagSeqMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceTCPFlagSeqModeValue,
                           sizeof (objDenialOfServiceTCPFlagSeqModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceTCPFlagSeqMode
*
* @purpose Set 'DenialOfServiceTCPFlagSeqMode'
*
* @description [DenialOfServiceTCPFlagSeqMode] Global TCP Flag Seq Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceTCPFlagSeqMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPFlagSeqModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceTCPFlagSeqMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDenialOfServiceTCPFlagSeqModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPFlagSeqModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbDoSTCPFlagSeqSet (L7_UNIT_CURRENT, objDenialOfServiceTCPFlagSeqModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceTCPOffsetMode
*
* @purpose Get 'DenialOfServiceTCPOffsetMode'
*
* @description [DenialOfServiceTCPOffsetMode] Global TCP Offset Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceTCPOffsetMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPOffsetModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDoSTCPOffsetModeGet ( &objDenialOfServiceTCPOffsetModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPOffsetModeValue,
                     sizeof (objDenialOfServiceTCPOffsetModeValue));

  /* return the object value: DenialOfServiceTCPOffsetMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceTCPOffsetModeValue,
                           sizeof (objDenialOfServiceTCPOffsetModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceTCPOffsetMode
*
* @purpose Set 'DenialOfServiceTCPOffsetMode'
*
* @description [DenialOfServiceTCPOffsetMode] Global TCP Offset Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceTCPOffsetMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPOffsetModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceTCPOffsetMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDenialOfServiceTCPOffsetModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPOffsetModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbDoSTCPOffsetSet (L7_UNIT_CURRENT, objDenialOfServiceTCPOffsetModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#if 0
/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceTCPPartialMode
*
* @purpose Get 'DenialOfServiceTCPPartialMode'
*
* @description [DenialOfServiceTCPPartialMode] Global TCP Partial Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceTCPPartialMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPPartialModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDoSTCPPartialModeGet ( &objDenialOfServiceTCPPartialModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPPartialModeValue,
                     sizeof (objDenialOfServiceTCPPartialModeValue));

  /* return the object value: DenialOfServiceTCPPartialMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceTCPPartialModeValue,
                           sizeof (objDenialOfServiceTCPPartialModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceTCPPartialMode
*
* @purpose Set 'DenialOfServiceTCPPartialMode'
*
* @description [DenialOfServiceTCPPartialMode] Global TCP Partial Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceTCPPartialMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPPartialModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceTCPPartialMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDenialOfServiceTCPPartialModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPPartialModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbDoSTCPPartialModeSet (L7_UNIT_CURRENT, objDenialOfServiceTCPPartialModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#endif
/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceTCPPortMode
*
* @purpose Get 'DenialOfServiceTCPPortMode'
*
* @description [DenialOfServiceTCPPortMode] Global TCP Port Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceTCPPortMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPPortModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDoSTCPPortModeGet ( &objDenialOfServiceTCPPortModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPPortModeValue,
                     sizeof (objDenialOfServiceTCPPortModeValue));

  /* return the object value: DenialOfServiceTCPPortMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceTCPPortModeValue,
                           sizeof (objDenialOfServiceTCPPortModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceTCPPortMode
*
* @purpose Set 'DenialOfServiceTCPPortMode'
*
* @description [DenialOfServiceTCPPortMode] Global TCP Port Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceTCPPortMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPPortModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceTCPPortMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDenialOfServiceTCPPortModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPPortModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbDoSTCPPortSet (L7_UNIT_CURRENT, objDenialOfServiceTCPPortModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceTCPSynFinMode
*
* @purpose Get 'DenialOfServiceTCPSynFinMode'
*
* @description [DenialOfServiceTCPSynFinMode] Global TCP Syn Fin Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceTCPSynFinMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPSynFinModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDoSTCPSynFinModeGet ( &objDenialOfServiceTCPSynFinModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPSynFinModeValue,
                     sizeof (objDenialOfServiceTCPSynFinModeValue));

  /* return the object value: DenialOfServiceTCPSynFinMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceTCPSynFinModeValue,
                           sizeof (objDenialOfServiceTCPSynFinModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceTCPSynFinMode
*
* @purpose Set 'DenialOfServiceTCPSynFinMode'
*
* @description [DenialOfServiceTCPSynFinMode] Global TCP Syn Fin Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceTCPSynFinMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPSynFinModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceTCPSynFinMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDenialOfServiceTCPSynFinModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPSynFinModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbDoSTCPSynFinSet (L7_UNIT_CURRENT, objDenialOfServiceTCPSynFinModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceTCPSyncMode
*
* @purpose Get 'DenialOfServiceTCPSyncMode'
*
* @description [DenialOfServiceTCPSyncMode] Global TCP Sync Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceTCPSyncMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPSyncModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDoSTCPSynModeGet ( &objDenialOfServiceTCPSyncModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPSyncModeValue,
                     sizeof (objDenialOfServiceTCPSyncModeValue));

  /* return the object value: DenialOfServiceTCPSyncMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceTCPSyncModeValue,
                           sizeof (objDenialOfServiceTCPSyncModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceTCPSyncMode
*
* @purpose Set 'DenialOfServiceTCPSyncMode'
*
* @description [DenialOfServiceTCPSyncMode] Global TCP Sync Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceTCPSyncMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceTCPSyncModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceTCPSyncMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDenialOfServiceTCPSyncModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceTCPSyncModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbDoSTCPSynSet (L7_UNIT_CURRENT, objDenialOfServiceTCPSyncModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_securitydos_DenialOfServiceUDPPortMode
*
* @purpose Get 'DenialOfServiceUDPPortMode'
*
* @description [DenialOfServiceUDPPortMode] Global UDP Port Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_securitydos_DenialOfServiceUDPPortMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceUDPPortModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmdbDoSUDPPortModeGet ( &objDenialOfServiceUDPPortModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceUDPPortModeValue,
                     sizeof (objDenialOfServiceUDPPortModeValue));

  /* return the object value: DenialOfServiceUDPPortMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDenialOfServiceUDPPortModeValue,
                           sizeof (objDenialOfServiceUDPPortModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_securitydos_DenialOfServiceUDPPortMode
*
* @purpose Set 'DenialOfServiceUDPPortMode'
*
* @description [DenialOfServiceUDPPortMode] Global UDP Port Mode
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_securitydos_DenialOfServiceUDPPortMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDenialOfServiceUDPPortModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DenialOfServiceUDPPortMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDenialOfServiceUDPPortModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDenialOfServiceUDPPortModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmdbDoSUDPPortSet (L7_UNIT_CURRENT, objDenialOfServiceUDPPortModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


