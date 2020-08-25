/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingLLDPManAddrConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to LLDP-object.xml
*
* @create  5 February 2008
*
* @author  
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingLLDPManAddrConfig_obj.h"
#include "usmdb_lldp_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPManAddrConfig_LLDPLocManAddrSubtype
*
* @purpose Get 'LLDPLocManAddrSubtype'
*
* @description [LLDPLocManAddrSubtype]: The type of management address identifier
*              encoding used in the associated 'lldpLocManagmentAddr'
*              object. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPManAddrConfig_LLDPLocManAddrSubtype (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLLDPLocManAddrSubtypeValue;
  xLibU32_t nextObjLLDPLocManAddrSubtypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LLDPLocManAddrSubtype */
  owa.rc =
    xLibFilterGet (wap, XOBJ_SwitchingLLDPManAddrConfig_LLDPLocManAddrSubtype,
                   (xLibU8_t *) & objLLDPLocManAddrSubtypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
#if 0
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetFirstUnknown (L7_UNIT_CURRENT,
                                     &nextObjLLDPLocManAddrSubtypeValue);
#endif
  }
  else
  {
#if 0
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objLLDPLocManAddrSubtypeValue, owa.len);
    owa.l7rc =
      usmDbGetNextUnknown (L7_UNIT_CURRENT, objLLDPLocManAddrSubtypeValue,
                           &nextObjLLDPLocManAddrSubtypeValue);
#endif
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLLDPLocManAddrSubtypeValue, owa.len);

  /* return the object value: LLDPLocManAddrSubtype */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLLDPLocManAddrSubtypeValue,
                    sizeof (objLLDPLocManAddrSubtypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPManAddrConfig_LLDPLocManAddr
*
* @purpose Get 'LLDPLocManAddr'
*
* @description [LLDPLocManAddr]: The string value used to identify the management
*              address Component associated with the local system.
*              The purpose of this address is to contact the management entity.
*              
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPManAddrConfig_LLDPLocManAddr (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objLLDPLocManAddrValue;
  lldpIANAAddrFamilyNumber_t family;
  L7_uchar8                  length;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LLDPLocManAddr */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPManAddrConfig_LLDPLocManAddr,
                          (xLibU8_t *) objLLDPLocManAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
    owa.l7rc = usmDbLldpLocManAddrGet (&family,
                                       objLLDPLocManAddrValue,
                                       &length);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objLLDPLocManAddrValue, owa.len);
    owa.l7rc = usmDbLldpLocManAddrGet (&family,
                                       objLLDPLocManAddrValue,
                                       &length);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, objLLDPLocManAddrValue, owa.len);

  /* return the object value: LLDPLocManAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLLDPLocManAddrValue,
                           strlen (objLLDPLocManAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPManAddrConfig_LLDPConfigManAddrPortsTxEnable
*
* @purpose Get 'LLDPConfigManAddrPortsTxEnable'
*
* @description [LLDPConfigManAddrPortsTxEnable]: A set of ports that are identified
*              by a PortList, in which each port is represented
*              as a bit. The corresponding local system management address
*              instance will be transmitted on the member ports of the lldpManAddrPortsTxEnable.The
*              default value for lldpCon 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_SwitchingLLDPManAddrConfig_LLDPConfigManAddrPortsTxEnable (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLLDPLocManAddrSubtypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyLLDPLocManAddrValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objLLDPConfigManAddrPortsTxEnableValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LLDPLocManAddrSubtype */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_SwitchingLLDPManAddrConfig_LLDPLocManAddrSubtype,
                   (xLibU8_t *) & keyLLDPLocManAddrSubtypeValue, &kwa1.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLLDPLocManAddrSubtypeValue, kwa1.len);

  /* retrieve key: LLDPLocManAddr */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPManAddrConfig_LLDPLocManAddr,
                           (xLibU8_t *) keyLLDPLocManAddrValue, &kwa2.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyLLDPLocManAddrValue, kwa2.len);

  /* get the value from application */
#if 0
  owa.l7rc =
    usmDbLldpIntfTxMgmtAddrGet (L7_UNIT_CURRENT, keyLLDPLocManAddrSubtypeValue,
                                keyLLDPLocManAddrValue,
                                objLLDPConfigManAddrPortsTxEnableValue);
#endif
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LLDPConfigManAddrPortsTxEnable */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objLLDPConfigManAddrPortsTxEnableValue,
                    strlen (objLLDPConfigManAddrPortsTxEnableValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingLLDPManAddrConfig_LLDPConfigManAddrPortsTxEnable
*
* @purpose Set 'LLDPConfigManAddrPortsTxEnable'
*
* @description [LLDPConfigManAddrPortsTxEnable]: A set of ports that are identified
*              by a PortList, in which each port is represented
*              as a bit. The corresponding local system management address
*              instance will be transmitted on the member ports of the lldpManAddrPortsTxEnable.The
*              default value for lldpCon 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_SwitchingLLDPManAddrConfig_LLDPConfigManAddrPortsTxEnable (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objLLDPConfigManAddrPortsTxEnableValue;
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLLDPLocManAddrSubtypeValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyLLDPLocManAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LLDPConfigManAddrPortsTxEnable */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objLLDPConfigManAddrPortsTxEnableValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objLLDPConfigManAddrPortsTxEnableValue, owa.len);

  /* retrieve key: LLDPLocManAddrSubtype */
  kwa1.rc =
    xLibFilterGet (wap, XOBJ_SwitchingLLDPManAddrConfig_LLDPLocManAddrSubtype,
                   (xLibU8_t *) & keyLLDPLocManAddrSubtypeValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLLDPLocManAddrSubtypeValue, kwa1.len);

  /* retrieve key: LLDPLocManAddr */
  kwa2.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPManAddrConfig_LLDPLocManAddr,
                           (xLibU8_t *) keyLLDPLocManAddrValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyLLDPLocManAddrValue, kwa2.len);

  /* set the value in application */
#if 0
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keyLLDPLocManAddrSubtypeValue,
                              keyLLDPLocManAddrValue,
                              objLLDPConfigManAddrPortsTxEnableValue);
#endif
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

