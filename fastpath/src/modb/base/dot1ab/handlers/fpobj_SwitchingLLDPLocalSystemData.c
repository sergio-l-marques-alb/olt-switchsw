/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingLLDPLocalSystemData.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to LLDP-object.xml
*
* @create  6 February 2008
*
* @author Radha K 
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingLLDPLocalSystemData_obj.h"
#include "usmdb_lldp_api.h"
#include "usmdb_util_api.h"

static L7_BOOL usmdbIsValidLldpLocalIntf(xLibU32_t intIfNum);

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_ChassisId
*
* @purpose Get 'ChassisId'
*
* @description [ChassisId]: The string value used to identify the chassis
*              component associated with the local system. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_ChassisId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objChassisIdValue;
  L7_uchar8 id[LLDP_MGMT_STRING_SIZE_MAX];
  lldpChassisIdSubtype_t subtype = 0;
  L7_ushort16 length = 0; 
  L7_char8 buf[LLDP_MGMT_STRING_SIZE_MAX];
  FPOBJ_TRACE_ENTER (bufp);

  memset(objChassisIdValue,0x00,sizeof(objChassisIdValue));

  /* get the value from application */
  usmDbLldpLocChassisIdSubtypeGet(&subtype);
  memset(id, 0, LLDP_MGMT_STRING_SIZE_MAX);
  memset(buf, 0, sizeof(buf));
  owa.l7rc = usmDbLldpLocChassisIdGet(id, &length);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  usmDbLldpChassisIdFormat(buf, sizeof(buf), subtype, id, length);
  strcpy(objChassisIdValue, buf);
  FPOBJ_TRACE_VALUE (bufp, objChassisIdValue, strlen (objChassisIdValue));

  /* return the object value: ChassisId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objChassisIdValue,
                           strlen (objChassisIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_ChassisIdSubtype
*
* @purpose Get 'ChassisIdSubtype'
*
* @description [ChassisIdSubtype]: The type of encoding used to identify the
*              chassis associated with the local system. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_ChassisIdSubtype (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objChassisIdSubtypeValue;
  lldpChassisIdSubtype_t subtype = 0;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objChassisIdSubtypeValue,0x00,sizeof(objChassisIdSubtypeValue));
  /* get the value from application */
  owa.l7rc =  usmDbLldpLocChassisIdSubtypeGet(&subtype);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiStrncpySafe(objChassisIdSubtypeValue, (L7_char8 *)usmDbLldpChassisIdSubtypeString(subtype), sizeof(objChassisIdSubtypeValue));
  FPOBJ_TRACE_VALUE (bufp, objChassisIdSubtypeValue,
                     strlen (objChassisIdSubtypeValue));

  /* return the object value: ChassisIdSubtype */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objChassisIdSubtypeValue,
                           strlen (objChassisIdSubtypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_SysCapEnabled
*
* @purpose Get 'SysCapEnabled'
*
* @description [SysCapEnabled]: The bitmap value used to identify which system
*              capabilities are enabled on the local system. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_SysCapEnabled (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSysCapEnabledValue;
  L7_ushort16 sysCap = 0;
  L7_char8 buf[256];
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  memset(buf, 0, sizeof(buf));
  owa.l7rc = usmDbLldpLocSysCapEnabledGet(&sysCap);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  usmDbLldpSysCapFormat(buf, sizeof(buf), sysCap);
  strcpy(objSysCapEnabledValue, buf);
  FPOBJ_TRACE_VALUE (bufp, objSysCapEnabledValue,
                     strlen (objSysCapEnabledValue));

  /* return the object value: SysCapEnabled */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSysCapEnabledValue,
                           strlen (objSysCapEnabledValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_SysDesc
*
* @purpose Get 'SysDesc'
*
* @description [SysDesc]: The string value used to identify the system description
*              of the local system. If the local agent supports IETF
*              RFC 3418, SysDesc object should have the same value of
*              sysDesc object. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_SysDesc (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSysDescValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbLldpLocSysDescGet ( objSysDescValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objSysDescValue, strlen (objSysDescValue));

  /* return the object value: SysDesc */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSysDescValue,
                           strlen (objSysDescValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_SysName
*
* @purpose Get 'SysName'
*
* @description [SysName]: The string value used to identify the system name
*              of the local system. If the local agent supports IETF RFC
*              3418, SysName object should have the same value of sysName
*              object. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_SysName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSysNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbLldpLocSysNameGet ( objSysNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objSysNameValue, strlen (objSysNameValue));

  /* return the object value: SysName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSysNameValue,
                           strlen (objSysNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_SysCapSupported
*
* @purpose Get 'SysCapSupported'
*
* @description [SysCapSupported]: The bitmap value used to identify which
*              system capabilities are supported on the local system. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_SysCapSupported (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objSysCapSupportedValue;
  L7_ushort16 sysCap = 0;
  L7_char8 buf[256];
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  memset(buf, 0, sizeof(buf));
  owa.l7rc =
    usmDbLldpLocSysCapSupportedGet (&sysCap);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  usmDbLldpSysCapFormat(buf, sizeof(buf), sysCap);
  strcpy(objSysCapSupportedValue, buf);
  FPOBJ_TRACE_VALUE (bufp, objSysCapSupportedValue,
                     strlen (objSysCapSupportedValue));

  /* return the object value: SysCapSupported */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSysCapSupportedValue,
                           strlen (objSysCapSupportedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_PortNum
*
* @purpose Get 'PortNum'
*
* @description [PortNum]: The index value used to identify the port component
*              (contained in the local chassis with the LLDP agent) associated
*              with this entry. 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_PortNum (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortNumValue;
  xLibU32_t nextObjPortNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPLocalSystemData_PortNum,
                          (xLibU8_t *) & objPortNumValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
    objPortNumValue = 0;
    nextObjPortNumValue = 0;
    owa.l7rc = usmDbLldpValidIntfFirstGet(&nextObjPortNumValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPortNumValue, owa.len);
    owa.l7rc = usmDbLldpValidIntfNextGet(objPortNumValue,
                                         &nextObjPortNumValue);
  } 
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  while (owa.l7rc == L7_SUCCESS)
  {
    if (usmdbIsValidLldpLocalIntf(nextObjPortNumValue) == L7_TRUE)
    {
      owa.l7rc = L7_SUCCESS;
      break;
    }
    owa.l7rc = usmDbLldpValidIntfNextGet(nextObjPortNumValue, &nextObjPortNumValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjPortNumValue, owa.len);

  /* return the object value: PortNum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjPortNumValue,
                           sizeof (objPortNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_PortIdSubtype
*
* @purpose Get 'PortIdSubtype'
*
* @description [PortIdSubtype]: The type of port identifier encoding used
*              in the associated 'PortId' object. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_PortIdSubtype (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPortIdSubtypeValue;
  lldpPortIdSubtype_t portIdType = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPLocalSystemData_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpLocPortIdSubtypeGet ( keyPortNumValue,
                                           &portIdType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  memset(objPortIdSubtypeValue,0x00,sizeof(objPortIdSubtypeValue));
  osapiStrncpySafe(objPortIdSubtypeValue, (L7_char8 *)usmDbLldpPortIdSubtypeString(portIdType), sizeof(objPortIdSubtypeValue));
  /* return the object value: PortIdSubtype */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objPortIdSubtypeValue,
                           strlen (objPortIdSubtypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_PortId
*
* @purpose Get 'PortId'
*
* @description [PortId]: The string value used to identify the port component
*              associated with a given port in the local system. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_PortId (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPortIdValue={0};
  lldpPortIdSubtype_t portIdType = 0;
  L7_uchar8 id[LLDP_MGMT_STRING_SIZE_MAX]={0};
  L7_ushort16 length = 0;
  L7_char8 buf[LLDP_MGMT_STRING_SIZE_MAX]={0};
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPLocalSystemData_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpLocPortIdGet(keyPortNumValue, id, &length);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbLldpLocPortIdSubtypeGet(keyPortNumValue, &portIdType);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  usmDbLldpPortIdFormat(buf, sizeof(buf), portIdType, id, length);
  strcpy(objPortIdValue, buf);
  /* return the object value: PortId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPortIdValue,
                           strlen (objPortIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_PortDesc
*
* @purpose Get 'PortDesc'
*
* @description [PortDesc]: The string value used to identify the 802 LAN station's
*              port description associated with the local system.
*              If the local agent supports IETF RFC 2863, lldpLocPortDesc
*              object should have the same value of ifDescr object 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_PortDesc (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPortDescValue;
  FPOBJ_TRACE_ENTER (bufp);
  memset(objPortDescValue,0x00,sizeof(objPortDescValue));

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPLocalSystemData_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpLocPortDescGet (keyPortNumValue,
                                      objPortDescValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PortDesc */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPortDescValue,
                           strlen (objPortDescValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_ManAddrSubtype
*
* @purpose Get 'ManAddrSubtype'
*
* @description [ManAddrSubtype]: The type of management address identifier
*              encoding used in the associated 'MantAddr' object 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_ManAddrSubtype (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objManAddrSubtypeValue;
  lldpIANAAddrFamilyNumber_t family = 0;
  L7_uchar8 address[LLDP_MGMT_ADDR_SIZE_MAX];
  L7_uchar8 addrlen = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ManAddrSubtype */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPLocalSystemData_ManAddrSubtype,
                          (xLibU8_t *) & objManAddrSubtypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
    memset(objManAddrSubtypeValue, 0, sizeof(objManAddrSubtypeValue));
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &objManAddrSubtypeValue, owa.len);
  owa.l7rc = usmDbLldpLocManAddrGet(&family, address, &addrlen);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiStrncpySafe(objManAddrSubtypeValue, (L7_char8 *)usmDbLldpManAddrFamilyString(family), sizeof(objManAddrSubtypeValue));
  FPOBJ_TRACE_NEW_KEY (bufp, &objManAddrSubtypeValue, owa.len);

  /* return the object value: ManAddrSubtype */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objManAddrSubtypeValue,
                           strlen (objManAddrSubtypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_ManAddr
*
* @purpose Get 'ManAddr'
*
* @description [ManAddr]: The string value used to identify the management
*              address component associated with the local system. The purpose
*              of this address is to contact the management entity 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_ManAddr (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objManAddrValue;
  lldpIANAAddrFamilyNumber_t family = 0;
  L7_uchar8 address[LLDP_MGMT_ADDR_SIZE_MAX];
  L7_uchar8 addrlen = 0;
  L7_char8 buf[256];
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ManAddr */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPLocalSystemData_ManAddr,
                          (xLibU8_t *) objManAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
    memset(objManAddrValue, 0, sizeof(objManAddrValue));
  } 
  FPOBJ_TRACE_CURRENT_KEY (bufp, objManAddrValue, owa.len);
  memset(buf, 0, sizeof(buf));
  owa.l7rc = usmDbLldpLocManAddrGet(&family, address, &addrlen);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  usmDbLldpManAddrFormat(buf, sizeof(buf), family, address, addrlen);
  strcpy(objManAddrValue, buf);
  FPOBJ_TRACE_NEW_KEY (bufp, objManAddrValue, owa.len);

  /* return the object value: ManAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objManAddrValue,
                           strlen (objManAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_ManAddrLen
*
* @purpose Get 'ManAddrLen'
*
* @description [ManAddrLen]: The total length of the management address subtype
*              and the management address fields in LLDPDUs transmitted
*              by the local LLDP agent. The management address length
*              field is needed so that the receiving systems that do not implement
*              SNMP will not be require 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_ManAddrLen (void *wap,
                                                           void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyManAddrValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objManAddrLenValue;
  lldpIANAAddrFamilyNumber_t family;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ManAddr */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPLocalSystemData_ManAddr,
                           (xLibU8_t *) keyManAddrValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  
  FPOBJ_TRACE_CURRENT_KEY (bufp, keyManAddrValue, kwa.len);

  /* get the value from application */
   owa.l7rc =
       usmDbLldpLocManAddrGet(&family,
                              keyManAddrValue,
                              (xLibU8_t *)&objManAddrLenValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ManAddrLen */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objManAddrLenValue,
                           sizeof (objManAddrLenValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_ManAddrIfSubtype
*
* @purpose Get 'ManAddrIfSubtype'
*
* @description [ManAddrIfSubtype]: The enumeration value that identifies the
*              interface numbering method used for defining the interface
*              number, associated with the local system. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_ManAddrIfSubtype (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objManAddrIfSubtypeValue;
  FPOBJ_TRACE_ENTER (bufp);
  
  /* get the value from application */
  owa.l7rc =
    usmDbLldpLocManAddrIfSubtypeGet ( &objManAddrIfSubtypeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ManAddrIfSubtype */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objManAddrIfSubtypeValue,
                           sizeof (objManAddrIfSubtypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_ManAddrIfId
*
* @purpose Get 'ManAddrIfId'
*
* @description [ManAddrIfId]: The integer value used to identify the interface
*              number regarding the management address component associated
*              with the local system. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_ManAddrIfId (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objManAddrIfIdValue;
  FPOBJ_TRACE_ENTER (bufp);
  
  /* get the value from application */
  owa.l7rc =
    usmDbLldpLocManAddrIfIdGet ( &objManAddrIfIdValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ManAddrIfId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objManAddrIfIdValue,
                           sizeof (objManAddrIfIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_ManAddrOID
*
* @purpose Get 'ManAddrOID'
*
* @description [ManAddrOID]: The OID value used to identify the type of hardware
*              component or protocol entity associated with the management
*              address advertised by the local system agent. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_ManAddrOID (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objManAddrOIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbLldpLocManAddrOIDGet ( objManAddrOIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ManAddrOID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objManAddrOIDValue,
                           strlen (objManAddrOIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_ManAddrReadOnly
*
* @purpose Get 'ManAddrReadOnly'
*
* @description [ManAddrReadOnly]: The string value used to identify the management
*              address component associated with the local system. The purpose
*              of this address is to contact the management entity 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_ManAddrReadOnly (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objManAddrValue={0};
  lldpIANAAddrFamilyNumber_t family = 0;
  L7_uchar8 address[LLDP_MGMT_ADDR_SIZE_MAX]={0};
  L7_uchar8 addrlen = 0;
  L7_char8 buf[256];
  FPOBJ_TRACE_ENTER (bufp);

  memset(buf, 0, sizeof(buf));
  owa.l7rc = usmDbLldpLocManAddrGet(&family, address, &addrlen);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  usmDbLldpManAddrFormat(buf, sizeof(buf), family, address, addrlen);
  strcpy(objManAddrValue, buf);
  FPOBJ_TRACE_NEW_KEY (bufp, objManAddrValue, owa.len);

  /* return the object value: ManAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objManAddrValue,
                           strlen (objManAddrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPLocalSystemData_ManAddrSubtypeReadOnly
*
* @purpose Get 'ManAddrSubtypeReadOnly'
*
* @description [ManAddrSubtype]: The type of management address identifier
*              encoding used in the associated 'MantAddr' object 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPLocalSystemData_ManAddrSubtypeReadOnly (void *wap,
                                                                       void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objManAddrSubtypeValue={0};
  lldpIANAAddrFamilyNumber_t family = 0;
  L7_uchar8 address[LLDP_MGMT_ADDR_SIZE_MAX]={0};
  L7_uchar8 addrlen = 0;
  FPOBJ_TRACE_ENTER (bufp);

  owa.l7rc = usmDbLldpLocManAddrGet(&family, address, &addrlen);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  osapiStrncpySafe(objManAddrSubtypeValue, (L7_char8 *)usmDbLldpManAddrFamilyString(family), sizeof(objManAddrSubtypeValue));
  FPOBJ_TRACE_NEW_KEY (bufp, &objManAddrSubtypeValue, owa.len);

  /* return the object value: ManAddrSubtype */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objManAddrSubtypeValue,
                           strlen (objManAddrSubtypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*********************************************************************
*
* @purpose To find out if and interface is enabled for LLDP transmits
*
* @param   {{input}}intIfNum  internal interface number of the interface
*                             in question
*
* @returns L7_TRUE  Valid Local LLDP interface - Transmits enabled
*          L7_FALSE
*
* @end
*
*********************************************************************/
static L7_BOOL usmdbIsValidLldpLocalIntf(xLibU32_t intIfNum)
{
  xLibU32_t txMode;

  if ((usmDbLldpIntfTxModeGet(intIfNum, &txMode) == L7_SUCCESS)
      && (txMode == L7_ENABLE))
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}
